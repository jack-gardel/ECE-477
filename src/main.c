#include "stm32f0xx.h"
#include <stdio.h>
#include <stdbool.h>
#include "feedback-display.h"
#include "board.h"
#include "uart-jetson.h"

// Global variables
int timer[2];
int timerPrec = 10; // Timer precision

enum PlayerTurn {PLAYER_WHITE, PLAYER_BLACK};
enum PlayerTurn playerTurn = PLAYER_WHITE;

enum GameState {SETUP, AWAIT_MOVE, GOT_MOVE, GAME_OVER};
enum GameState gameState = SETUP;

bool hold = false;

int sX, sY, dX, dY;
sX, sY, dX, dY = 9;

bool ready = false;

int numPieceRecv = 0;

void enable_ports()
{
    // Enable GPIOC
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
}

void config_buttons()
{
    // Configure button Interrupts
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
    EXTI->IMR |= EXTI_IMR_MR1 | EXTI_IMR_MR3 | EXTI_IMR_MR10 | EXTI_IMR_MR13;
    EXTI->RTSR |= EXTI_RTSR_TR1 | EXTI_RTSR_TR3 | EXTI_RTSR_TR10 | EXTI_RTSR_TR13;

    SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI1 | SYSCFG_EXTICR1_EXTI3);
    SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI1_PC | SYSCFG_EXTICR1_EXTI3_PC;
    SYSCFG->EXTICR[2] &= ~SYSCFG_EXTICR3_EXTI10;
    SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PC;
    SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

    NVIC->ISER[0] |= 1 << EXTI0_1_IRQn;
    NVIC->ISER[0] |= 1 << EXTI2_3_IRQn;
    NVIC->ISER[0] |= 1 << EXTI4_15_IRQn;
}

// Record Button
void EXTI0_1_IRQHandler()
{
    if (!hold)
    {
        if (gameState == AWAIT_MOVE)
            send_record();
        else if (gameState == GOT_MOVE) {
            sX, sY, dX, dY = 9;
            clear_feedback_display_portion(16, "bottom", "center");
            send_record();
            gameState = AWAIT_MOVE;
        }
    }
    hold = true;
    EXTI->PR |= EXTI_PR_PR1;
}

// Confirm Button
void EXTI2_3_IRQHandler()
{
    if (!hold)
    {
        if (gameState == SETUP) {
            gameState = AWAIT_MOVE;
            send_confirm();
        } else if (gameState == GOT_MOVE) {
            move_piece(sX, sY, dX, dY);
            sX, sY, dX, dY = 9;
            playerTurn = playerTurn == PLAYER_WHITE ? PLAYER_BLACK : PLAYER_WHITE;
            send_confirm();
            gameState = AWAIT_MOVE;
        } else if (gameState == GAME_OVER)
            gameState = SETUP;
    }
    hold = true;
    EXTI->PR |= EXTI_PR_PR3;
}

// Timer Up/Down Button
void EXTI4_15_IRQHandler()
{
    if(!hold) {
        if (gameState == SETUP) {
            // Increase or decrease timers
            if (EXTI->PR & EXTI_PR_PR13) {
                if (timer[PLAYER_WHITE] < 99 * 60 * timerPrec) {
                    timer[PLAYER_WHITE] = timer[PLAYER_WHITE] + 60 * timerPrec;
                    timer[PLAYER_BLACK] = timer[PLAYER_BLACK] + 60 * timerPrec;
                }
            } else if (EXTI->PR & EXTI_PR_PR10) {
                if (timer[PLAYER_WHITE] >= 60 * timerPrec) {
                    timer[PLAYER_WHITE] = timer[PLAYER_WHITE] - 60 * timerPrec;
                    timer[PLAYER_BLACK] = timer[PLAYER_BLACK] - 60 * timerPrec;
                }
            }
        }
    }
    hold = true;
    EXTI->PR |= EXTI_PR_PR10 | EXTI_PR_PR13;
}

// Timer for updating game timers
void init_tim6(void)
{
    timer[PLAYER_WHITE] = (int) 600 * timerPrec;
    timer[PLAYER_BLACK] = (int) 600 * timerPrec;

    // Enable Clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

    // Set frequency
    int freq = timerPrec; // Frequency (Hz)
    int psc = 4800;
    int arr = (48000000/psc) / freq;
    TIM6->PSC = psc-1;
    TIM6->ARR = arr-1;

    // Enable interrupt
    TIM6->DIER |= TIM_DIER_UIE;
    NVIC_SetPriority(TIM6_DAC_IRQn, 1);
    NVIC->ISER[0] |= 1 << TIM6_DAC_IRQn;

    // Enable timer
    TIM6->CR1 |= TIM_CR1_CEN;
}

// Timer for board refresh
void init_tim7(void)
{
    // Enable Clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

    // Set frequency
    int freq = 120; // Frequency (Hz)
    int psc = 4800;
    int arr = (48000000/psc) / freq;
    TIM7->PSC = psc-1;
    TIM7->ARR = arr-1;

    // Enable interrupt
    TIM7->DIER |= TIM_DIER_UIE;
    NVIC_SetPriority(TIM7_IRQn, 1);
    NVIC->ISER[0] |= 1 << TIM7_IRQn;

    // Enable timer
    TIM7->CR1 |= TIM_CR1_CEN;
}


// Timer interrupt (also turns off button hold)
void TIM6_DAC_IRQHandler()
{
    TIM6->SR &= ~TIM_SR_UIF;

    // Update timers
    if (gameState != GAME_OVER) {
        write_time_to_feedback_display(timer[PLAYER_WHITE], "top", "left");
        write_time_to_feedback_display(timer[PLAYER_BLACK], "top", "right");
        if (gameState != SETUP) {
            if (playerTurn == PLAYER_WHITE && timer[PLAYER_WHITE] >= 0)
                timer[PLAYER_WHITE]--;
            else if (playerTurn == PLAYER_BLACK && timer[PLAYER_BLACK] >= 0)
                timer[PLAYER_BLACK]--;
        }
    }

    // Turn off button hold
    hold = false;
}

// Board refresh interrupt
void TIM7_IRQHandler() {
    TIM7->SR &= ~TIM_SR_UIF;
    if (gameState == SETUP)
        write_blank_board();
    else
        write_board();
}

void USART1_IRQHandler() {
    uint8_t signal = USART1->RDR & 0xFF;
    enum CommRecv type = signal >> 6;

    if (type == RDY) {
        //ready();
        ready = true;
    } else if (type == FDBK) {
        write_feedback_from_code(signal & 0x3f, "bottom", "center");
    } else if (gameState == SETUP) {
        // If in setup state, add received piece to board
        add_piece_to_board(numPieceRecv++, signal);
        if (numPieceRecv >= 64) {
            numPieceRecv = 0;
        }
    } else {
        int X = (signal >> 3) & 0x7;
        int Y = signal & 0x7;
        if (type == SRC) {
            sX = X;
            sY = Y;
        } else if (type == DST) {
            dX = X;
            dY = Y;
        }
    }

    // Got move?
    if(sX < 8 && sY < 8 && dX < 8 && dY < 8) {
        write_move_to_feedback_display(sX, sY, dX, dY, "bottom", "center");
        gameState = GOT_MOVE;
    }
}

int main(void)
{
    setup_uart_jetson();
    setup_feedback_display();
    setup_board();
    enable_ports();
    init_tim6();
    init_tim7();
    config_buttons();

    while(1)
    {
    }
}

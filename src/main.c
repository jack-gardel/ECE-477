#include "stm32f0xx.h"
#include <stdio.h>
#include <stdbool.h>
#include "feedback-display.h"
#include "board.h"
#include "uart-jetson.h"

enum Player {PLAYER_WHITE, PLAYER_BLACK};

int timer[2];
timerPrec = 1; // Timer precision

bool init = true;
bool hold = false;
int sX = 9;
int sY = 9;
int dX = 9;
int dY = 9;

bool confirm = false;
bool undo = false;
bool conDeny = false;
int playerTurn = PLAYER_WHITE;

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
    EXTI->IMR |= EXTI_IMR_MR0 | EXTI_IMR_MR3 | EXTI_IMR_MR10 | EXTI_IMR_MR13;
    EXTI->RTSR |= EXTI_RTSR_TR0 | EXTI_RTSR_TR3 | EXTI_RTSR_TR10 | EXTI_RTSR_TR13;
    SYSCFG->EXTICR[0] |= 0x2002;
    SYSCFG->EXTICR[2] |= 0x200;
    SYSCFG->EXTICR[3] |= 0x20;
    NVIC->ISER[0] |= 1 << EXTI0_1_IRQn;
    NVIC->ISER[0] |= 1 << EXTI2_3_IRQn;
    NVIC->ISER[0] |= 1 << EXTI4_15_IRQn;
}


// Record Button
void EXTI0_1_IRQHandler()
{
    EXTI->PR |= EXTI_PR_PR0;
    if (!hold)
    {
        if (conDeny)
        {
            undo = true;
            conDeny = false;
        }
        send_record();
    }
    hold = true;
}

// Confirm Button
void EXTI2_3_IRQHandler()
{
    EXTI->PR |= EXTI_PR_PR3;
    if (!hold)
    {
        if (init)
            init = false;
        else
            if (conDeny)
            {
                confirm = true;
                conDeny = false;
                playerTurn = !playerTurn;
            }
    }
    hold = true;
}

// Timer Up/Down Button
void EXTI4_15_IRQHandler()
{
    if(init && !hold)
    {
        if(((EXTI->PR & (0x1 << 13)) >> 13) == 1)
        {
            if (timer[PLAYER_WHITE] < 99 * 60 * timerPrec)
            {
                timer[PLAYER_WHITE] = timer[PLAYER_WHITE] + 60 * timerPrec;
                timer[PLAYER_BLACK] = timer[PLAYER_BLACK] + 60 * timerPrec;
            }
        }
        else if (((EXTI->PR & (0x1 << 10)) >> 10) == 1)
        {
            if (timer[PLAYER_WHITE] >= 60 * timerPrec)
            {
                timer[PLAYER_WHITE] = timer[PLAYER_WHITE] - 60 * timerPrec;
                timer[PLAYER_BLACK] = timer[PLAYER_BLACK] - 60 * timerPrec;
            }
        }
    }
    hold = true;
    EXTI->PR |= EXTI_PR_PR10 | EXTI_PR_PR13;
}

void init_tim6(void)
{
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

void init_tim7(void)
{
    timer[PLAYER_WHITE] = (int) 600;
    timer[PLAYER_BLACK] = (int) 600;

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

char command[16];
void requestConfirm()
{
    write_move_to_feedback_display(sX, sY, dX, dY, "bottom", "center");
    conDeny = true;
}

void TIM6_DAC_IRQHandler() // LCD TEXT DISPLAY
{
    TIM6->SR &= ~TIM_SR_UIF;
    if (playerTurn == PLAYER_WHITE)
    {
        if (timer[PLAYER_WHITE] >= 0)
        {
            write_time_to_feedback_display(timer[PLAYER_WHITE], "top", "left");
            write_time_to_feedback_display(timer[PLAYER_BLACK], "top", "right");
            if (!init)
                timer[PLAYER_WHITE]--;
        }
    }
    else
    {
        if (timer[PLAYER_BLACK] >= 0)
        {
            write_time_to_feedback_display(timer[PLAYER_WHITE], "top", "left");
            write_time_to_feedback_display(timer[PLAYER_BLACK], "top", "right");
            if (!init)
                timer[PLAYER_BLACK]--;
        }
    }

    hold = false;

    if(sX < 8 && sY < 8 && dX < 8 && dY < 8)
        requestConfirm();

    if (confirm)
    {
        move_piece(sX, sY, dX, dY);
        sX = 9;
        sY = 9;
        dX = 9;
        dY = 9;
        write_to_feedback_display("                ", "bottom", "center");
        send_confirm();
    }
    else if (undo)
    {
        sX = 9;
        sY = 9;
        dX = 9;
        dY = 9;
        write_to_feedback_display("                ", "bottom", "center");
    }

    confirm = false;
    undo = false;
}

void TIM7_IRQHandler() // LED DISPLAY
{
    TIM7->SR &= ~TIM_SR_UIF;
    if(!init)
    {
        write_board();
    }
    else
        write_blank_board();

//    hold = false;
}

void USART1_IRQHandler() {
    int signal = USART1->RDR & 0xFF;
    int code = signal >> 6;
    int X = (signal >> 3) & 0x7;
    int Y = signal & 0x7;

    if (code == 2)
    {
        //ready();
        ready = true;
    }
    else if (code == 3)
    {
        //notReady();
    }
    // Portotype code for setting up the board to a desired position
    // for testing purposes
    else if (init) {
        // If in setup state, add received piece to board
        add_piece_to_board(numPieceRecv++, signal);
        if (numPieceRecv >= 64) {
            numPieceRecv = 0;
            write_to_feedback_display("Board received!", "bottom", "center");
        }
    } else {
        if (code == 0)
        {
            sX = X;
            sY = Y;
        }
        else if (code == 1)
        {
            dX = X;
            dY = Y;
        }
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
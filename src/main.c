#include "stm32f0xx.h"
#include <stdio.h>
#include "feedback-display.h"
#include "board.h"

int timer0 = 0; // Time in seconds
int timer1 = 0;
int init = 1;
int hold = 0;
int sX = 9;
int sY = 9;
int dX = 9;
int dY = 9;

int confirm = 0;
int undo = 0;
int conDeny = 0;
int playerTurn = 0;


void enable_ports()
{
    // Enable GPIOC
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    // Enable USART1
    GPIOA->MODER &= ~(0xF<<18);
    GPIOA->MODER |= 0xA<<18;
    GPIOA->AFR[1] |= 17 << 4;
    RCC->APB2ENR |= 1 << 14;
    USART1->CR1 &= ~USART_CR1_UE;                       //USART Disable
    USART1->CR1 &= ~USART_CR1_M ;                       //Word Size of 8
    USART1->CR2 &= ~USART_CR2_STOP;                     //One stop bit
    USART1->CR1 &= ~USART_CR1_PCE;                      //No parity
    USART1->CR1 &= ~USART_CR1_OVER8;                    //16x oversampling
    USART1->BRR = 0x1A1;                                //baud rate of 115200
    USART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);       //TE enable RE enable
    USART1->CR1 |= USART_CR1_UE;                        //USART enable

    while ((USART1->ISR & USART_ISR_TEACK) == 0) { }
    while ((USART1->ISR & USART_ISR_REACK) == 0) { }
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

void sendRecord()
{
    USART1->TDR = 0x00;
    while((USART1->ISR & 0x40) >> 6 != 1);
}

void sendShutdown()
{
    USART1->TDR = 0x40;
    while((USART1->ISR & 0x40) >> 6 != 1);
}

void sendByte(int byteToSend)
{
    USART1->TDR = byteToSend & 0xFF;
    while((USART1->ISR & 0x40) >> 6 != 1);
}

int pressNo = 0;

// Record Button
void EXTI0_1_IRQHandler()
{
    EXTI->PR |= EXTI_PR_PR0;
    if (hold == 0)
    {
        if (conDeny)
        {
            undo = 1;
            conDeny = 0;
        }
        sendRecord();
    }
    hold = 1;
}

// Confirm Button
void EXTI2_3_IRQHandler()
{
    EXTI->PR |= EXTI_PR_PR3;
    if (hold == 0)
    {
        if (init == 1)
            init = 0;
        else
            if (conDeny)
            {
                confirm = 1;
                conDeny = 0;
                playerTurn = !playerTurn;
            }
    }
    hold = 1;
}

// Timer Up/Down Button
void EXTI4_15_IRQHandler()
{
    if(init == 1 && hold == 0)
    {
        if(((EXTI->PR & (0x1 << 13)) >> 13) == 1)
        {
            if (timer0 < 99 * 60)
            {
                timer0 = timer0 + 60;
                timer1 = timer1 + 60;
            }
        }
        else if (((EXTI->PR & (0x1 << 10)) >> 10) == 1)
        {
            if (timer0 >= 60)
            {
                timer0 = timer0 - 60;
                timer1 = timer1 - 60;
            }
        }
    }
    hold = 1;
    EXTI->PR |= EXTI_PR_PR10 | EXTI_PR_PR13;
}

void init_tim6(void)
{
    // Enable Clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

    // Set rate to 1 Hz
    int psc = 4800;
    int arr = (48000000/psc) / 1;
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
    // Enable Clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

    // Set rate to 1 Hz
    int psc = 4800;
    int arr = (48000000/psc) / 1;
    TIM6->PSC = psc-1;
    TIM6->ARR = arr-1;

    // Enable interrupt
    TIM7->DIER |= TIM_DIER_UIE;
    NVIC_SetPriority(TIM7_IRQn, 1);
    NVIC->ISER[0] |= 1 << TIM7_IRQn;

    // Enable timer
    TIM7->CR1 |= TIM_CR1_CEN;
}

void TIM6_DAC_IRQHandler() // LCD TEXT DISPLAY
{
    TIM6->SR &= ~TIM_SR_UIF;
    if (!playerTurn)
    {
        if (timer0 >= 0)
        {
            write_time_to_feedback_display(timer0, "top", "left");
            write_time_to_feedback_display(timer1, "top", "right");
            if (!init)
                timer0--;
        }
    }
    else
    {
        if (timer1 >= 0)
        {
            write_time_to_feedback_display(timer0, "top", "left");
            write_time_to_feedback_display(timer1, "top", "right");
            if (!init)
                timer1--;
        }
    }
    hold = 0;

    if(sX < 8 && sY < 8 && dX < 8 && dY < 8)
        requestConfirm();

    if (confirm)
    {
        move_piece(sX, 7-sY, dX, 7-dY);
        sX = 9;
        sY = 9;
        dX = 9;
        dY = 9;
        write_to_feedback_display("                ", "bottom", "center");
    }
    else if (undo)
    {
        sX = 9;
        sY = 9;
        dX = 9;
        dY = 9;
        write_to_feedback_display("                ", "bottom", "center");
    }

    confirm = 0;
    undo = 0;
}

char command[16];
void requestConfirm()
{
    sprintf(command, "%d,%d to %d,%d", sX+1, sY+1, dX+1, dY+1);
    write_to_feedback_display(command, "bottom", "center");
    conDeny = 1;
}

void TIM7_IRQHandler() // LED DISPLAY
{
    TIM7->SR &= ~TIM_SR_UIF;
    if(init == 0)
    {
        send_board();
    }
    else
        send_black();
}

void enable_UART_interrupt() {
    USART1->CR1 |= USART_CR1_RXNEIE;
    NVIC_SetPriority(USART1_IRQn, 0);
    NVIC->ISER[0] |= (1 << USART1_IRQn);
}

void USART1_IRQHandler() {
    int signal = USART1->RDR & 0xFF;
    int code = signal >> 6;
    int X = (signal >> 3) & 0x7;
    int Y = signal & 0x7;

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
    else if (code == 2)
    {
        //ready();
    }
    else if (code == 3)
    {
        //notReady();
    }
}

int main(void)
{
    setup_feedback_display();
    setup_board();
    enable_ports();
    init_tim6();
    init_tim7();
    config_buttons();
    enable_UART_interrupt();

    while(1)
    {
    }
}

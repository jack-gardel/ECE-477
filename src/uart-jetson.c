#include "stm32f0xx.h"
#include "uart-jetson.h"

void send_byte(int byteToSend) {
    USART1->TDR = byteToSend & 0xFF;
    while ((USART1->ISR & 0x40) >> 6 != 1)
        ;
}

void send_record() {
    send_byte(REC << 6);
}

void send_confirm() {
    send_byte(CFM << 6);
}

void send_shutdown() {
    send_byte(SHTDWN << 6);
}

void send_timeout() {
    send_byte(TIMOUT << 6);
}

void wait_for_ready() {
    while (!ready) {
    }
    ready = !ready;
}

void setup_uart_jetson_gpio() {
    // Enable GPIOA
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Enable USART1
    GPIOA->MODER &= ~(0xF << 18);
    GPIOA->MODER |= 0xA << 18;
    GPIOA->AFR[1] |= 17 << 4;
}

void setup_usart1() {
    RCC->APB2ENR |= 1 << 14;
    USART1->CR1 &= ~USART_CR1_UE;                       //USART Disable
    USART1->CR1 &= ~USART_CR1_M;                       //Word Size of 8
    USART1->CR2 &= ~USART_CR2_STOP;                     //One stop bit
    USART1->CR1 &= ~USART_CR1_PCE;                      //No parity
    USART1->CR1 &= ~USART_CR1_OVER8;                    //16x oversampling
    USART1->BRR = 0x1A1;                                //baud rate of 115200
    USART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);       //TE enable RE enable
    USART1->CR1 |= USART_CR1_UE;                        //USART enable

    while ((USART1->ISR & USART_ISR_TEACK) == 0) {
    }
    while ((USART1->ISR & USART_ISR_REACK) == 0) {
    }
}

void enable_UART_interrupt() {
    USART1->CR1 |= USART_CR1_RXNEIE;
    NVIC_SetPriority(USART1_IRQn, 0);
    NVIC->ISER[0] |= (1 << USART1_IRQn);
}

void setup_uart_jetson() {
    setup_uart_jetson_gpio();
    setup_usart1();
    enable_UART_interrupt();
}

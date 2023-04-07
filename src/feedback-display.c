#include "stm32f0xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "feedback-display.h"

uint16_t display[34] = { 0x002, // Command to set the cursor at the first position line 1
        0x200 | ' ', 0x200 | ' ', 0x200 | ' ', 0x200 | ' ', 0x200 | ' ', 0x200
                | ' ', 0x200 | ' ', 0x200 | ' ', 0x200 | ' ', 0x200 | ' ', 0x200
                | ' ', 0x200 | ' ', 0x200 | ' ', 0x200 | ' ', 0x200 | ' ', 0x200
                | ' ', 0x0c0, // Command to set the cursor at the first position line 2
        0x200 | ' ', 0x200 | ' ', 0x200 | ' ', 0x200 | ' ', 0x200 | ' ', 0x200
                | ' ', 0x200 | ' ', 0x200 | ' ', 0x200 | ' ', 0x200 | ' ', 0x200
                | ' ', 0x200 | ' ', 0x200 | ' ', 0x200 | ' ', 0x200 | ' ', 0x200
                | ' ', };

char *time_to_string(int time_int) {
    // Convert a given time integer to string format

    int mins = (time_int / timerPrec) / 60;
    int secs = (time_int / timerPrec) % 60;

    char *time_str = malloc(sizeof(char) * 8); // Space for 00:00.0
    if (mins >= 10)
        sprintf(time_str, "%d:%02d", mins, secs);
    else
        sprintf(time_str, "%02d:%02d", mins, secs);

    return time_str;
}

void write_time_to_feedback_display(int time, char *align_vertical, char *align_horizontal) {
    char *time_str = time_to_string(time);
    char *prev_time_str = time_to_string(time + 1);

    clear_feedback_display_portion(strlen(prev_time_str), align_vertical, align_horizontal);
    write_to_feedback_display(time_str, align_vertical, align_horizontal);

    free(time_str);
    free(prev_time_str);
}

void write_move_to_feedback_display(int sX, int sY, int dX, int dY, char *align_vertical, char *align_horizontal) {
    // sX, sY: Source X and Y coordinates
    // dX, dY: Destination X and Y coordinates

    char sFile = (char) (sX + 65);
    char dFile = (char) (dX + 65);

    char command[16];
    sprintf(command, "%c%d to %c%d", sFile, sY + 1, dFile, dY + 1);
    overwrite_to_feedback_display(command, align_vertical, align_horizontal);
}

void write_feedback_from_code(FeedbackCode code, char *align_vertical, char *align_horizontal) {
    if (code == MATE_WHITE_WINS) {
        overwrite_to_feedback_display("Checkmate!", "top", "center");
        overwrite_to_feedback_display("White wins!", "bottom", "center");
    } else if (code == MATE_BLACK_WINS) {
        overwrite_to_feedback_display("Checkmate!", "top", "center");
        overwrite_to_feedback_display("Black wins!", "bottom", "center");
    } else if (code == TIME_OUT_WHITE_WINS) {
        overwrite_to_feedback_display("Time out!", "top", "center");
        overwrite_to_feedback_display("White wins!", "bottom", "center");
    } else if (code == TIME_OUT_BLACK_WINS) {
        overwrite_to_feedback_display("Time out!", "top", "center");
        overwrite_to_feedback_display("Black wins!", "bottom", "center");
    } else if (code == DRAW_STALEMATE) {
        overwrite_to_feedback_display("Draw by", "top", "center");
        overwrite_to_feedback_display("Stalemate!", "bottom", "center");
    } else if (code == DRAW_INSUFFICIENT) {
        overwrite_to_feedback_display("Draw by", "top", "center");
        overwrite_to_feedback_display("Insufficient!", "bottom", "center");
    } else if (code == DRAW_REPETITION) {
        overwrite_to_feedback_display("Draw by", "top", "center");
        overwrite_to_feedback_display("Repetition!", "bottom", "center");
    } else if (code == SET_TIMERS) {
        overwrite_to_feedback_display("Set timers", "bottom", "center");
    } else if (code == WHITES_TURN) {
        overwrite_to_feedback_display("White's turn", "bottom", "center");
    } else if (code == BLACKS_TURN) {
        overwrite_to_feedback_display("Black's turn", "bottom", "center");
    } else if (code == PRESS_CONFIRM) {
        overwrite_to_feedback_display("Press confirm", "bottom", "center");
    } else if (code == ILLEGAL_MOVE) {
        overwrite_to_feedback_display("Illegal move!", "bottom", "center");
    }
}

void overwrite_to_feedback_display(char *text, char *align_vertical, char *align_horizontal) {
    // Overwrite an entire row
    clear_feedback_display_portion(16, align_vertical, align_horizontal);
    write_to_feedback_display(text, align_vertical, align_horizontal);
}

void write_to_feedback_display(char *text, char *align_vertical, char *align_horizontal) {
    // Input Arguments:
    // text - The text to be displayed
    // align_vertical - "top" to write in top row, "bottom" to write in bottom row
    // align_horizontal - "left", "center", or "right" to align horizontally

    int len = strlen(text);
    int start_offset = 1; // Starting index of text in display buffer

    if (strcmp(align_vertical, "top") == 0) {
        // Do nothing
    } else if (strcmp(align_vertical, "bottom") == 0) {
        start_offset = 18;
    } else {
        return;
    }

    if (strcmp(align_horizontal, "left") == 0) {
        // Do nothing
    } else if (strcmp(align_horizontal, "right") == 0) {
        start_offset += 16 - len;
    } else if (strcmp(align_horizontal, "center") == 0) {
        start_offset += 8 - len / 2;
    } else {
        return;
    }

    // Write the string to display buffer
    for (int i = 0; i < len; i++) {
        if ((strcmp(align_vertical, "top") == 0)
                && (start_offset + i < 1 || start_offset + i > 16))
            continue;
        if ((strcmp(align_vertical, "bottom") == 0)
                && (start_offset + i < 18 || start_offset + i > 33))
            continue;
        display[start_offset + i] = 0x200 | text[i];
    }
}

void clear_feedback_display_portion(int length, char *align_vertical,
        char *align_horizontal) {
    // Clear the length of spaces in a given screen area

    char *spaces = malloc(sizeof(char) * (++length));
    for (int i = 0; i < length; i++) {
        spaces[i] = ' ';
    }
    spaces[length - 1] = '\0';

    write_to_feedback_display(spaces, align_vertical, align_horizontal);
    free(spaces);
}

void clear_feedback_display() {
    for (int i = 1; i < 17; i++) {
        display[i] = 0x200 | ' ';
    }
    for (int i = 18; i < 34; i++) {
        display[i] = 0x200 | ' ';
    }
}

void setup_feedback_display_gpio() {
    // Enable GPIOA
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Reset MODER and then enable Alternate Function mode
    GPIOA->MODER &=
            ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER7 | GPIO_MODER_MODER15);
    GPIOA->MODER |= (GPIO_MODER_MODER5_1 | GPIO_MODER_MODER7_1
            | GPIO_MODER_MODER15_1);

    // Set to AF0
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFR5 | GPIO_AFRL_AFR7);
    GPIOA->AFR[1] &= ~(GPIO_AFRH_AFR15);
}

void init_spi1() {
    // Enable SPI Clock
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Disable SPI
    SPI1->CR1 &= ~SPI_CR1_SPE;

    // Set SPI to Master and max baud rate
    SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_BR;

    // Slave select output enabled
    // Interrupt when transmit buffer is empty
    // Set data length to 10 bits
    SPI1->CR2 = SPI_CR2_NSSP | SPI_CR2_SSOE | SPI_CR2_TXDMAEN |
    SPI_CR2_DS_3 | SPI_CR2_DS_0;

    // Enable SPI
    SPI1->CR1 |= SPI_CR1_SPE;
}

void setup_dma() {
    // Enable DMA Clock
    RCC->AHBENR |= RCC_AHBENR_DMAEN;

    // Disable DMA
    DMA1_Channel3->CCR &= ~DMA_CCR_EN;

    // Set memory and peripheral address
    DMA1_Channel3->CPAR = (uint32_t) (&(SPI1->DR));
    DMA1_Channel3->CMAR = (uint32_t) (&display);

    // Set data count
    DMA1_Channel3->CNDTR = 34;

    // Set memory increment, circular, direction, and data sizes
    DMA1_Channel3->CCR = DMA_CCR_MINC | DMA_CCR_CIRC | DMA_CCR_DIR
            | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0;
}

void enable_dma() {
    DMA1_Channel3->CCR |= DMA_CCR_EN;
}

void spi_cmd(unsigned int data) {
    while (!(SPI1->SR & SPI_SR_TXE)) {
    }
    SPI1->DR = data;
}

void nano_wait(unsigned int n) {
    asm( "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

void spi1_init_oled() {
    nano_wait(1000000);
    spi_cmd(0x38);
    spi_cmd(0x08);
    spi_cmd(0x01);
    nano_wait(2000000);
    spi_cmd(0x06);
    spi_cmd(0x02);
    spi_cmd(0x0c);
}

void setup_feedback_display() {
    setup_feedback_display_gpio();
    init_spi1();
    spi1_init_oled();
    setup_dma();
    enable_dma();
}

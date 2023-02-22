#include "stm32f0xx.h"
#include <stdio.h>

const int RED = 0x1;
const int GREEN = 0x2;
const int BLUE = 0x4;
const int WHITE = 0x7;
const int BLACK = 0x0;
const int YELLOW = 0x3;
const int PURPLE = 0x5;
const int TEAL = 0x6;

const int NONE = 0;
const int PAWN = 1;
const int ROOK = 2;
const int KNIGHT = 3;
const int BISHOP = 4;
const int QUEEN = 5;
const int KING = 6;

int timer = 60; // Time in seconds
int init = 1;
int hold = 0;

 int pieces[7][8][8] = {{{0,0,0,0,0,0,0,0},         // None
                         {0,0,0,0,0,0,0,0},
                         {0,0,0,0,0,0,0,0},
                         {0,0,0,0,0,0,0,0},
                         {0,0,0,0,0,0,0,0},
                         {0,0,0,0,0,0,0,0},
                         {0,0,0,0,0,0,0,0},
                         {0,0,0,0,0,0,0,0}},
                     {{0,0,0,0,0,0,0,0},            // Pawn
                      {0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0},
                      {0,0,0,1,1,0,0,0},
                      {0,0,0,1,1,0,0,0},
                      {0,0,1,1,1,1,0,0},
                      {0,0,1,1,1,1,0,0},
                      {0,0,0,0,0,0,0,0}},
                         {{0,0,0,0,0,0,0,0},        // Rook
                          {0,0,1,1,1,1,0,0},
                          {0,0,0,1,1,0,0,0},
                          {0,0,0,1,1,0,0,0},
                          {0,0,0,1,1,0,0,0},
                          {0,0,1,1,1,1,0,0},
                          {0,1,1,1,1,1,1,0},
                          {0,0,0,0,0,0,0,0}},
                     {{0,0,0,0,0,0,0,0},            // Knight
                      {0,0,1,1,0,0,0,0},
                      {0,0,1,1,1,0,0,0},
                      {0,0,1,1,1,1,0,0},
                      {0,0,1,1,0,0,0,0},
                      {0,0,1,1,1,1,0,0},
                      {0,1,1,1,1,1,1,0},
                      {0,0,0,0,0,0,0,0}},
                         {{0,0,0,0,0,0,0,0},        // Bishop
                          {0,0,0,1,1,0,0,0},
                          {0,0,1,1,1,1,0,0},
                          {0,0,0,1,1,0,0,0},
                          {0,0,0,1,1,0,0,0},
                          {0,0,1,1,1,1,0,0},
                          {0,1,1,1,1,1,1,0},
                          {0,0,0,0,0,0,0,0}},
                     {{0,0,0,0,0,0,0,0},            // Queen
                      {0,1,0,1,1,0,1,0},
                      {0,1,1,0,0,1,1,0},
                      {0,0,1,1,1,1,0,0},
                      {0,0,0,1,1,0,0,0},
                      {0,0,1,1,1,1,0,0},
                      {0,1,1,1,1,1,1,0},
                      {0,0,0,0,0,0,0,0}},
                         {{0,0,0,0,0,0,0,0},        // King
                          {0,1,0,1,1,0,1,0},
                          {0,1,1,1,1,1,1,0},
                          {0,0,0,1,1,0,0,0},
                          {0,0,0,1,1,0,0,0},
                          {0,0,1,1,1,1,0,0},
                          {0,1,1,1,1,1,1,0},
                          {0,0,0,0,0,0,0,0}}};

typedef struct {
    int player;
    int piece;
} tile;

char *time_to_string(int time_int);
void write_time_to_feedback_display(int time, char *align_vertical, char *align_horizontal);
void write_to_feedback_display(char *text, char *align_vertical, char *align_horizontal);
void clear_feedback_display_portion(int length, char *align_vertical, char *align_horizontal);
void clear_feedback_display();
void sendBoard();
void sendBlack();

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

uint16_t display[34] = {
        0x002, // Command to set the cursor at the first position line 1
        0x200|' ', 0x200|' ', 0x200|' ', 0x200|' ', 0x200|' ', 0x200|' ', 0x200|' ', 0x200|' ',
        0x200|' ', 0x200|' ', 0x200|' ', 0x200|' ', 0x200|' ', 0x200|' ', 0x200|' ', 0x200|' ',
        0x0c0, // Command to set the cursor at the first position line 2
        0x200|' ', 0x200|' ', 0x200|' ', 0x200|' ', 0x200|' ', 0x200|' ', 0x200|' ', 0x200|' ',
        0x200|' ', 0x200|' ', 0x200|' ', 0x200|' ', 0x200|' ', 0x200|' ', 0x200|' ', 0x200|' ',
};

void enablePorts()
{
    // Enable GPIOA
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Reset MODER and then enable Alternate Function mode
    GPIOA->MODER &= ~(GPIO_MODER_MODER5   | GPIO_MODER_MODER7   | GPIO_MODER_MODER15  );
    GPIOA->MODER |=  (GPIO_MODER_MODER5_1 | GPIO_MODER_MODER7_1 | GPIO_MODER_MODER15_1);

    // Set to AF0
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFR5 | GPIO_AFRL_AFR7);
    GPIOA->AFR[1] &= ~(GPIO_AFRH_AFR15);

    // Enable GPIOB
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	// Configure PB pins
	GPIOB->MODER &= 0x0;
	GPIOB->PUPDR &= 0x0;
	GPIOB->MODER &= 0x0;
	GPIOB->MODER |= 0x55555555;
	GPIOB->BRR = 0xFFFF;

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

// Reset Button
void EXTI0_1_IRQHandler()
{

    EXTI->PR |= EXTI_PR_PR0;
    if (hold == 0)
    {

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
            if (timer < 99 * 60)
                timer = timer + 60;
        }
        else if (((EXTI->PR & (0x1 << 10)) >> 10) == 1)
        {
            if (timer >= 60)
                timer = timer - 60;
        }
    }
    hold = 1;
    EXTI->PR |= EXTI_PR_PR10 | EXTI_PR_PR13;
}

void init_spi1()
{
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

void setup_dma()
{
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
    DMA1_Channel3->CCR = DMA_CCR_MINC | DMA_CCR_CIRC | DMA_CCR_DIR | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0;
}

void enable_dma()
{
    DMA1_Channel3->CCR |= DMA_CCR_EN;
}

void spi_cmd(unsigned int data) {
    while(!(SPI1->SR & SPI_SR_TXE)) {}
    SPI1->DR = data;
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

void init_tim6(void)
{
    // Enable Clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

    // Set rate to 10 Hz
    int psc = 480;
    int arr = (48000000/psc) / 10;
    TIM6->PSC = 4800-1;
    TIM6->ARR = arr-1;

    // Enable interrupt
    TIM6->DIER |= TIM_DIER_UIE;
    NVIC->ISER[0] |= 1 << TIM6_DAC_IRQn;

    // Enable timer
    TIM6->CR1 |= TIM_CR1_CEN;
}

void init_tim7(void)
{
    // Enable Clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

    // Set rate to 10 Hz
    TIM7->PSC = 10-1;
    TIM7->ARR = 480000-1;

    // Enable interrupt
    TIM7->DIER |= TIM_DIER_UIE;
    NVIC->ISER[0] |= 1 << TIM7_IRQn;

    // Enable timer
    TIM7->CR1 |= TIM_CR1_CEN;
}

void TIM6_DAC_IRQHandler() // LCD TEXT DISPLAY
{
    TIM6->SR &= ~TIM_SR_UIF;
    if (timer >= 0) {
        write_time_to_feedback_display(timer, "top", "left");
        write_time_to_feedback_display(timer, "top", "right");
        if (!init)
            timer--;
    }
    hold = 0;
}

void TIM7_IRQHandler() // LED DISPLAY
{
    TIM7->SR &= ~TIM_SR_UIF;
    if(init == 0)
    {
        sendBoard();
    }
    else
        sendBlack();
}

void clear_feedback_display()
{
    for(int i = 1; i < 17; i++){
        display[i] = 0x200|' ';
    }
    for(int i = 18; i < 34; i++){
        display[i] = 0x200|' ';
    }
}

char *time_to_string(int time_int)
{
    // Convert a given time integer to string format

    int mins = time_int / 60;
    int secs = time_int % 60;

    char *time_str = malloc(sizeof(char) * 8); // Space for 00:00.0
    if (mins >= 10)
        sprintf(time_str, "%d:%02d", mins, secs);
    else
        sprintf(time_str, "0%d:%02d", mins, secs);

    return time_str;
}

void write_time_to_feedback_display(int time, char *align_vertical, char *align_horizontal)
{
    char *time_str = time_to_string(time);
    char *prev_time_str = time_to_string(time+1);

    clear_feedback_display_portion(strlen(prev_time_str), align_vertical, align_horizontal);
    write_to_feedback_display(time_str, align_vertical, align_horizontal);

    free(time_str);
    free(prev_time_str);
}

void write_to_feedback_display(char *text, char *align_vertical, char *align_horizontal)
{
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
        start_offset += 8 - len/2;
    } else {
        return;
    }

    // Write the string to display buffer
    for (int i = 0; i < len; i++) {
        if ((strcmp(align_vertical, "top"   ) == 0) && (start_offset + i <  1 || start_offset + i > 16)) continue;
        if ((strcmp(align_vertical, "bottom") == 0) && (start_offset + i < 18 || start_offset + i > 33)) continue;
        display[start_offset + i] =  0x200|text[i];
    }
}

void clear_feedback_display_portion(int length, char *align_vertical, char *align_horizontal)
{
    // Clear the length of spaces in a given screen area

    char *spaces = malloc(sizeof(char) * (++length));
    for (int i = 0; i < length; i++) {
        spaces[i] = ' ';
    }
    spaces[length-1] = '\0';

    write_to_feedback_display(spaces, align_vertical, align_horizontal);
    free(spaces);
}

tile board[8][8];

void initBoard()
{
    for (int a = 0; a < 8; a++)
    {
        for (int b = 0; b < 8; b++)
        {
            board[a][b].player = NONE;
            board[a][b].piece = NONE;
        }
    }
    for (int a = 0; a < 2; a++)
    {
        for (int b = 0; b < 8; b++)
        {
            board[a][b].player = GREEN;
            if (a == 1)
                board[a][b].piece = PAWN;
        }
    }

    for (int a = 6; a < 8; a++)
    {
        for (int b = 0; b < 8; b++)
        {
            board[a][b].player = BLUE;
            if (a == 6)
                board[a][b].piece = PAWN;
        }
    }
    board[0][0].piece = ROOK;
    board[0][1].piece = KNIGHT;
    board[0][2].piece = BISHOP;
    board[0][3].piece = QUEEN;
    board[0][4].piece = KING;
    board[0][5].piece = BISHOP;
    board[0][6].piece = KNIGHT;
    board[0][7].piece = ROOK;

    board[7][0].piece = ROOK;
    board[7][1].piece = KNIGHT;
    board[7][2].piece = BISHOP;
    board[7][3].piece = QUEEN;
    board[7][4].piece = KING;
    board[7][5].piece = BISHOP;
    board[7][6].piece = KNIGHT;
    board[7][7].piece = ROOK;
}

void sendBlack()
{
    GPIOB->BRR = 0xFFFF;
    for (int i = 0; i < 64; i++)
    {
        GPIOB->BSRR = 0x1 << 12;
        GPIOB->BRR = 0x1 << 12;
    }
    GPIOB->BSRR = 0x1 << 11;
    GPIOB->BSRR = 0x1 << 13;
    GPIOB->BRR = 0x1 << 11;
    GPIOB->BRR = 0x1 << 13;
}

void sendBoard()
{
    int out;
    int out1;
    int out2;
    int row;

    for (int i = 0; i < 4; i++) // Rows of tiles / 2
    {
        for (int j = 0; j < 8; j++) // Rows of pixels per tile
        {
            row = 8 * i + j - 1;
            if (row == -1)
                row = 31;
            GPIOB->BRR = 0x1F;
            GPIOB->BSRR = row;

            for (int k = 0; k < 8; k++) // Columns of tiles
            {
                for (int l = 0; l < 8; l++) // Columns of pixels per tile
                {
                    GPIOB->BRR = 0x3F << 5;
                    if (board[i+4][k].player == GREEN)
                    {
                        if (pieces[board[i+4][k].piece][7-j][l] == 1)
                            out1 = board[i+4][k].player << 5;
                        else
                        {
                            if ((i + k) % 2 == 0)
                                out1 = WHITE << 5;
                            else
                                out1 = BLACK << 5;
                        }
                    }
                    else
                    {
                        if (pieces[board[i+4][k].piece][j][l] == 1)
                            out1 = board[i+4][k].player << 5;
                        else
                        {
                            if ((i + k) % 2 == 0)
                                out1 = WHITE << 5;
                            else
                                out1 = BLACK << 5;
                        }
                    }
                    if (board[i][k].player == GREEN)
                    {
                        if (pieces[board[i][k].piece][7-j][l] == 1)
                            out2 = board[i][k].player << 8;
                        else
                        {
                            if ((i + k) % 2 == 0)
                                out2 = WHITE << 8;
                            else
                                out2 = BLACK << 8;
                        }
                    }
                    else
                    {
                        if (pieces[board[i][k].piece][j][l] == 1)
                            out2 = board[i][k].player << 8;
                        else
                        {
                            if ((i + k) % 2 == 0)
                                out2 = WHITE << 8;
                            else
                                out2 = BLACK << 8;
                        }
                    }
                    out = out2 | out1;
                    GPIOB->BSRR = out;
                    GPIOB->BSRR = 0x1 << 12;
                    GPIOB->BRR = 0x1 << 12;
                }
            }
            GPIOB->BSRR = 0x1 << 11;
            GPIOB->BSRR = 0x1 << 13;
            GPIOB->BRR = 0x1 << 11;
            GPIOB->BRR = 0x1 << 13;
        }
    }
}

void movePiece (int sx, int sy, int ex, int ey)
{
    board[ex][ey].piece = board[sx][sy].piece;
    board[sx][sy].piece = NONE;

    board[ex][ey].player = board[sx][sy].player;
    board[sx][sy].player = NONE;
}

int main(void)
{
	enablePorts();
    init_spi1();
    spi1_init_oled();
    setup_dma();
    enable_dma();
    init_tim6();
    init_tim7();
    config_buttons();
    write_to_feedback_display("A5 to B6", "bottom", "center");
    initBoard();

	while(1)
	{
	}
}

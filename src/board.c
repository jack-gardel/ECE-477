#include "stm32f0xx.h"
#include "board.h"

int pieces[7][8][8] = {

        // None
       {{0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0}},

        // Pawn
       {{0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,1,1,0,0,0},
        {0,0,0,1,1,0,0,0},
        {0,0,1,1,1,1,0,0},
        {0,0,1,1,1,1,0,0},
        {0,0,0,0,0,0,0,0}},

        // Rook
       {{0,0,0,0,0,0,0,0},
        {0,0,1,1,1,1,0,0},
        {0,0,0,1,1,0,0,0},
        {0,0,0,1,1,0,0,0},
        {0,0,0,1,1,0,0,0},
        {0,0,1,1,1,1,0,0},
        {0,1,1,1,1,1,1,0},
        {0,0,0,0,0,0,0,0}},

        // Knight
       {{0,0,0,0,0,0,0,0},
        {0,0,1,1,0,0,0,0},
        {0,0,1,1,1,0,0,0},
        {0,0,1,1,1,1,0,0},
        {0,0,1,1,0,0,0,0},
        {0,0,1,1,1,1,0,0},
        {0,1,1,1,1,1,1,0},
        {0,0,0,0,0,0,0,0}},

        // Bishop
       {{0,0,0,0,0,0,0,0},
        {0,0,0,1,1,0,0,0},
        {0,0,1,1,1,1,0,0},
        {0,0,0,1,1,0,0,0},
        {0,0,0,1,1,0,0,0},
        {0,0,1,1,1,1,0,0},
        {0,1,1,1,1,1,1,0},
        {0,0,0,0,0,0,0,0}},

        // Queen
       {{0,0,0,0,0,0,0,0},
        {0,1,0,1,1,0,1,0},
        {0,1,1,0,0,1,1,0},
        {0,0,1,1,1,1,0,0},
        {0,0,0,1,1,0,0,0},
        {0,0,1,1,1,1,0,0},
        {0,1,1,1,1,1,1,0},
        {0,0,0,0,0,0,0,0}},

        // King
       {{0,0,0,0,0,0,0,0},
        {0,1,0,1,1,0,1,0},
        {0,1,1,1,1,1,1,0},
        {0,0,0,1,1,0,0,0},
        {0,0,0,1,1,0,0,0},
        {0,0,1,1,1,1,0,0},
        {0,1,1,1,1,1,1,0},
        {0,0,0,0,0,0,0,0}}
};

tile board[8][8];

void init_board()
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

void send_board()
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

void send_black()
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

void move_piece (int bx, int by, int ex, int ey) {
    board[ey][ex].piece = board[by][bx].piece;
    board[by][bx].piece = NONE;

    board[ey][ex].player = board[by][bx].player;
    board[by][bx].player = NONE;
}

void setup_board_gpio() {
    // Enable GPIOB
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // Configure PB pins
    GPIOB->MODER &= 0x0;
    GPIOB->PUPDR &= 0x0;
    GPIOB->MODER &= 0x0;
    GPIOB->MODER |= 0x55555555;
    GPIOB->BRR = 0xFFFF;
}

void setup_board() {
    setup_board_gpio();
    init_board();
}

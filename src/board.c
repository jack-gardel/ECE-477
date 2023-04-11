#include <stdlib.h>

#include "stm32f0xx.h"
#include "board.h"

int pieces[7][8][8] = {

// None
        { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0,
                0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0,
                0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, {
                0, 0, 0, 0, 0, 0, 0, 0 } },

        // Pawn
        { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0,
                0, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0,
                0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, {
                0, 0, 0, 0, 0, 0, 0, 0 } },

        // Knight
        { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 0, 0, 0, 0 }, { 0, 0, 1, 1,
                1, 0, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 1, 1, 0, 0, 0,
                0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 1, 1, 1, 1, 1, 1, 0 }, {
                0, 0, 0, 0, 0, 0, 0, 0 } },

        // Bishop
        { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 1, 1,
                1, 1, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0,
                0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 1, 1, 1, 1, 1, 1, 0 }, {
                0, 0, 0, 0, 0, 0, 0, 0 } },

        // Rook
        { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 0, 1,
                1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0,
                0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 1, 1, 1, 1, 1, 1, 0 }, {
                0, 0, 0, 0, 0, 0, 0, 0 } },

        // Queen
        { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 1, 0, 1, 1, 0, 1, 0 }, { 0, 1, 1, 0,
                0, 1, 1, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0,
                0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 1, 1, 1, 1, 1, 1, 0 }, {
                0, 0, 0, 0, 0, 0, 0, 0 } },

        // King
        { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 1, 0, 1, 1, 0, 1, 0 }, { 0, 1, 1, 1,
                1, 1, 1, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0,
                0 }, { 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 1, 1, 1, 1, 1, 1, 0 }, {
                0, 0, 0, 0, 0, 0, 0, 0 } } };

tile board[8][8];

void init_board_default() {
    for (int a = 0; a < 8; a++) {
        for (int b = 0; b < 8; b++) {
            board[a][b].player = NONE;
            board[a][b].piece = NONE;
        }
    }
    for (int a = 0; a < 2; a++) {
        for (int b = 0; b < 8; b++) {
            board[a][b].player = BLACK_PLAYER_COLOR;
            if (a == 1)
                board[a][b].piece = PAWN;
        }
    }

    for (int a = 6; a < 8; a++) {
        for (int b = 0; b < 8; b++) {
            board[a][b].player = WHITE_PLAYER_COLOR;
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

void add_piece_to_board(int numPiece, char piece) {
    int col = numPiece % 8;
    int row = numPiece / 8;

    switch (piece) {
    case ('.'):
        board[row][col].piece = NONE;
        break;
    case ('P'):
    case ('p'):
        board[row][col].piece = PAWN;
        break;
    case ('N'):
    case ('n'):
        board[row][col].piece = KNIGHT;
        break;
    case ('B'):
    case ('b'):
        board[row][col].piece = BISHOP;
        break;
    case ('R'):
    case ('r'):
        board[row][col].piece = ROOK;
        break;
    case ('Q'):
    case ('q'):
        board[row][col].piece = QUEEN;
        break;
    case ('K'):
    case ('k'):
        board[row][col].piece = KING;
        break;
    default:
        break;
    }

    // White pieces
    if (piece == 'P' || piece == 'N' || piece == 'B' || piece == 'R'
            || piece == 'Q' || piece == 'K')
        board[row][col].player = WHITE_PLAYER_COLOR;
    // Black pieces
    else if (piece == 'p' || piece == 'n' || piece == 'b' || piece == 'r'
            || piece == 'q' || piece == 'k')
        board[row][col].player = BLACK_PLAYER_COLOR;
    else
        board[row][col].player = NONE;
}

void write_board() {
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
                    if (board[i + 4][k].player == BLACK_PLAYER_COLOR) {
                        if (pieces[board[i + 4][k].piece][7 - j][l] == 1)
                            out1 = board[i + 4][k].player << 5;
                        else {
                            if ((i + k) % 2 == 0)
                                out1 = WHITE_TILE_COLOR << 5;
                            else
                                out1 = BLACK_TILE_COLOR << 5;
                        }
                    } else {
                        if (pieces[board[i + 4][k].piece][j][l] == 1)
                            out1 = board[i + 4][k].player << 5;
                        else {
                            if ((i + k) % 2 == 0)
                                out1 = WHITE_TILE_COLOR << 5;
                            else
                                out1 = BLACK_TILE_COLOR << 5;
                        }
                    }
                    if (board[i][k].player == BLACK_PLAYER_COLOR) {
                        if (pieces[board[i][k].piece][7 - j][l] == 1)
                            out2 = board[i][k].player << 8;
                        else {
                            if ((i + k) % 2 == 0)
                                out2 = WHITE_TILE_COLOR << 8;
                            else
                                out2 = BLACK_TILE_COLOR << 8;
                        }
                    } else {
                        if (pieces[board[i][k].piece][j][l] == 1)
                            out2 = board[i][k].player << 8;
                        else {
                            if ((i + k) % 2 == 0)
                                out2 = WHITE_TILE_COLOR << 8;
                            else
                                out2 = BLACK_TILE_COLOR << 8;
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

void write_blank_board() {
    GPIOB->BRR = 0xFFFF;
    for (int i = 0; i < 64; i++) {
        GPIOB->BSRR = 0x1 << 12;
        GPIOB->BRR = 0x1 << 12;
    }
    GPIOB->BSRR = 0x1 << 11;
    GPIOB->BSRR = 0x1 << 13;
    GPIOB->BRR = 0x1 << 11;
    GPIOB->BRR = 0x1 << 13;
}

void move_piece(int sCol, int sY, int dCol, int dY) {
    int sRow = 7 - sY;
    int dRow = 7 - dY;

    // Promotion:
    // Piece is white pawn, row is 6 or Piece is black pawn, row is 1
    if ((board[sRow][sCol].piece == PAWN && board[sRow][sCol].player == WHITE_PLAYER_COLOR && sRow == 1)
     || (board[sRow][sCol].piece == PAWN && board[sRow][sCol].player == BLACK_PLAYER_COLOR && sRow == 6)) {
        // dY holds the promotion piece
        board[sRow][sCol].piece = dY;
        if (sRow == 1)
            dRow = 0;
        else
            dRow = 7;
    }

    // En Passant:
    // Piece is pawn, move is diagonal, and destination is empty
    else if (board[sRow][sCol].piece == PAWN && sCol != dCol
            && board[dRow][dCol].piece == NONE) {
        board[sRow][dCol].piece = NONE; // Remove piece behind
        board[sRow][dCol].player = NONE;
    }

    // Castling:
    // Piece is king, move is more than 1 column
    else if (board[sRow][sCol].piece == KING && abs(sCol - dCol) > 1) {
        // King side castle if king moves right
        if (dCol > sCol) {
            // Move rook to left of king
            move_piece(sCol + 3, sY, dCol - 1, dY);
        } else {
            // Move rook to right of king
            move_piece(sCol - 4, sY, dCol + 1, dY);
        }
    }

    board[dRow][dCol].piece = board[sRow][sCol].piece;
    board[sRow][sCol].piece = NONE;

    board[dRow][dCol].player = board[sRow][sCol].player;
    board[sRow][sCol].player = NONE;
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

void setup_board() {
    setup_board_gpio();
    init_board_default();
    write_blank_board();
    init_tim7();
}

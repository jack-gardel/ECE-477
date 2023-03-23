#ifndef BOARD
#define BOARD

#define RED 0x1
#define GREEN 0x2
#define BLUE 0x4
#define WHITE 0x7
#define BLACK 0x0
#define YELLOW 0x3
#define PURPLE 0x5
#define TEAL 0x6

#define NONE 0
#define PAWN 1
#define ROOK 2
#define KNIGHT 3
#define BISHOP 4
#define QUEEN 5
#define KING 6

typedef struct {
    int player;
    int piece;
} tile;

void write_board();
void write_blank_board();
void move_piece (int bx, int by, int ex, int ey);
void setup_board();

#endif

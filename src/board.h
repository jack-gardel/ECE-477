#ifndef BOARD
#define BOARD

enum Color {
    BLACK=0x0, WHITE=0x7,
    RED=0x1, GREEN=0x2, BLUE=0x4,
    YELLOW=0x3, PURPLE=0x5, TEAL=0x6
};

enum Piece {NONE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING};

typedef struct {
    int player;
    int piece;
} tile;

#define WHITE_TILE_COLOR BLUE
#define BLACK_TILE_COLOR BLACK
#define WHITE_PLAYER_COLOR WHITE
#define BLACK_PLAYER_COLOR GREEN

void write_board();
void write_blank_board();
void move_piece (int sX, int sY, int dX, int dY);
void setup_board();
void add_piece_to_board(int numPiece, char piece);

#endif

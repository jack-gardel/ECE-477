#ifndef FEEDBACK_DISPLAY
#define FEEDBACK_DISPLAY

uint16_t display[34];

extern int timerPrec;

typedef enum FeedbackCode {
    MATE_WHITE_WINS=0,
    MATE_BLACK_WINS=1,
    TIME_OUT_WHITE_WINS=2,
    TIME_OUT_BLACK_WINS=3,
    DRAW_STALEMATE=4,
    DRAW_INSUFFICIENT=5,
    DRAW_REPETITION=6,
    SET_TIMERS=7,
    WHITES_TURN=8,
    BLACKS_TURN=9,
    PRESS_CONFIRM=14,
    ILLEGAL_MOVE=15
} FeedbackCode;

char *time_to_string(int time_int);
void write_time_to_feedback_display(int time, char *align_vertical, char *align_horizontal);
void write_move_to_feedback_display(int sX, int sY, int dX, int dY, char *align_vertical, char *align_horizontal);
void write_feedback_from_code(FeedbackCode code, char *align_vertical, char *align_horizontal);
void write_to_feedback_display(char *text, char *align_vertical, char *align_horizontal);
void clear_feedback_display_portion(int length, char *align_vertical, char *align_horizontal);
void clear_feedback_display();
void setup_feedback_display();

#endif

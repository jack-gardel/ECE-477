#ifndef FEEDBACK_DISPLAY
#define FEEDBACK_DISPLAY

uint16_t display[34];

char *time_to_string(int time_int);
void write_time_to_feedback_display(int time, char *align_vertical, char *align_horizontal);
void write_to_feedback_display(char *text, char *align_vertical, char *align_horizontal);
void clear_feedback_display_portion(int length, char *align_vertical, char *align_horizontal);
void clear_feedback_display();
void setup_feedback_display();

#endif

#ifndef UART_JETSON
#define UART_JETSON

#include <stdbool.h>

enum Comm {REC, CFM, SHTDWN};

extern bool ready;

void send_byte(int byteToSend);
void send_record();
void send_confirm();
void send_shutdown();
void setup_uart_jetson();

#endif

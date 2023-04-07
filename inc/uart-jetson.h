#ifndef UART_JETSON
#define UART_JETSON

#include <stdbool.h>

enum CommSend {REC=0, CFM=1, SHTDWN=2, TIMOUT=3};
enum CommRecv {SRC=0, DST=1, FDBK=2, RDY=3};

extern bool ready;

void send_byte(int byteToSend);
void send_record();
void send_confirm();
void send_shutdown();
void setup_uart_jetson();

#endif

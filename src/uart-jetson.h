#ifndef UART_JETSON
#define UART_JETSON

#define REC 0
#define CFM 1
#define SHTDWN 2

void send_byte(int byteToSend);
void send_record();
void send_confirm();
void send_shutdown();
void setup_uart_jetson();

#endif

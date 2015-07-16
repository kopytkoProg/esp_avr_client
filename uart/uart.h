#ifndef __uart_h__
#define __uart_h__

void init_uart(void);
void send(char *string);

extern void onChar(char c);

#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "uart/uart.h"
#include "esp/esp.h"

int main(void) {

	init_uart();			// init uart

	sei();					// enable global interrupt

	send("Hello world");
	send("111");
	send("Hello world");

	DDRB |= 1 << 0;
	while (1) {
		PORTB ^= 1 << 0;
		_delay_ms(1000);

		if(esp_isMsgToExe()){
			char b[101];
			esp_getMsg_done(b);
			send(b);

		}
	}

}

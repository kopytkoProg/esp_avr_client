#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#define BAUD 38400
#include <util/setbaud.h>
#include <string.h>

#include "uart.h"


char usart_bufor[UART_SEND_BUFFER_SIZE + 1];
uint8_t usart_bufor_ind = 0;

enum enum_state {
	idle, sending
};
static enum enum_state volatile state = idle;

/**
 * interrupt generated after byte received.
 * received data stored in UDR
 * USART, RX Complete Handler
 */
ISR(USART_RX_vect) {
	//	interrupt generated after byte received.
	//  received data stored in UDR
	//--------------------------------------------------
	onChar(UDR0);
}

/**
 * interrupt generated when output buffer is empty
 * USART, UDR Empty Handle
 */
ISR(USART_UDRE_vect) {

	// interrupt generated when output buffer is empty
	//--------------------------------------------------
	if (usart_bufor[usart_bufor_ind] != 0) {	// check if not end of string
		UDR0 = usart_bufor[usart_bufor_ind++];	// send next char
	} else {									// if end of string
		state = idle;
		UCSR0B &= ~(1 << UDRIE0); 				// disable UDRE interrupt
	}
}

/**
 * data as a string
 */
void send(char *string) {

	/*
	 * 	The UDREn Flag indicates if the transmit buffer (UDRn) is ready to receive new data. If UDREn
	 * 	is one, the buffer is empty, and therefore ready to be written. The UDREn Flag can generate a
	 * 	Data Register Empty interrupt (see description of the UDRIE bit). UDREn is set after a reset to
	 * 	indicate that the Transmitter is ready.
	 */

	while (!(UCSR0A & (1 << UDRE0)) || state != idle)
		;

	state = sending;
	usart_bufor_ind = 0;						// copy data to buffer
	strcpy(usart_bufor, string);
	strcat(usart_bufor, "\r\n");
	UCSR0B |= (1 << UDRIE0); 					// enable empty buffer interrupt

}

void init_uart(void) {

	// set computed value by util/setbaud.h
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
#if USE_2X
	UCSR0A |= (1<<U2X0);
#else
	UCSR0A &= ~(1 << U2X0);
#endif

	//U W A G A !!!
	//W ATmega8, aby zapisać do rejestru UCSRC należy ustawiać bit URSEL
	//zobacz także: http://mikrokontrolery.blogspot.com/2011/04/avr-czyhajace-pulapki.html#avr_pulapka_2
	UCSR0C = /* (1<<URSEL) | */(1 << UCSZ01) | (1 << UCSZ00);  	//bitów danych: 8
																//bity stopu:  1
																//parzystość:  brak

	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
}

//====================================================

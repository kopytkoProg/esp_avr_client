#include <stdio.h>
#include <stdlib.h>
#include "uart/uart.h"

// will be code to reasembly msg from uart
#define BUFFER_SIZE  	100

char buffer[BUFFER_SIZE + 1];
static uint8_t p = 0;
static uint8_t id = 0;
static uint8_t size = 0;

enum enum_esp_state {
	waiting_for_cmd, receiving_data, reciving_unknown_length_data, reciving_header, waiting_for_executoion
};

static enum enum_esp_state volatile esp_state = waiting_for_cmd;

uint8_t find_first(char s[], uint8_t length, uint8_t c) {

	int16_t r = -1;
	int16_t i = 0;

	for (i = 0; r == -1 && i < length; i++) {
		if (s[i] == c) r = i;
	}
	return (uint8_t) (r & 0xff);
}

char esp_isMsgToExe(void) {
	return esp_state == waiting_for_executoion;
}

/**
 * Remember to call esp_done() when execution done
 */
char* esp_getMsg(void) {
	return buffer;
}
/**
 * esp_done() called automatically
 * buffer size should be great or equal then size(buffer)
 */
void esp_getMsg_done(char *b) {
	strcpy(b, buffer);
	esp_done();
}

/**
 * call after msg executed
 */
void esp_done(void) {
	esp_state = waiting_for_cmd;
}

void onChar(char c) {

	char b[50];
	sprintf(b, "%c", c);
	send(b);

	switch (esp_state) {
	case waiting_for_cmd:
		p = 0;
		if (c == '{') {
			buffer[p++] = c;
			esp_state = reciving_unknown_length_data;
			return;
		}

		/****************************************************************
		 * Command to send data to the remote client
		 * receive <id, size> thru uart
		 * id - of connection
		 * size - of data to receive by uart and send to client
		 ****************************************************************/

		if (c == '<') {
			buffer[p++] = c;
			esp_state = reciving_header;
			return;
		}

		break;

	case reciving_header:
		//--------------------------------------------------------
		buffer[p++] = c;

		if (p && buffer[p - 1] == '>') {

			id = atoi(buffer + 1);
			size = atoi((char *) (buffer + 1 + find_first(buffer, p, ',')));

			esp_state = receiving_data;

			buffer[p] = 0;

			p = 0;
		}

		break;
	case reciving_unknown_length_data:
		//--------------------------------------------------------
		if (p >= BUFFER_SIZE) {
			esp_state = waiting_for_cmd;
			p = 0;
			return;
		}

		buffer[p++] = c;

		if (c == '}') {

			buffer[p] = 0;
			esp_state = waiting_for_executoion;
			p = 0;
		}

		break;
	case receiving_data:
		//--------------------------------------------------------
		buffer[p++] = c;

		if (p >= size) {
			// done SEND data !

			buffer[p] = 0;
			esp_state = waiting_for_executoion;
			p = 0;

		}

		break;
	default:
		//--------------------------------------------------------
		break;
	}

}


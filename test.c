#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include "uart/uart.h"
#include "esp/esp.h"
#include "commands.h"
#include "onewire.h"
#include "ds18x20.h"

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5], (a)[6], (a)[7]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x"
char b[50];

static void send_temp(void) {
#define NUM_SENSORS 5
	static uint8_t sensor_id[NUM_SENSORS][OW_ROMCODE_SIZE];
	static uint8_t nfound = 0;
	// uint8_t txbuf[(NUM_SENSORS*2) + 4];
	int16_t temp[NUM_SENSORS];
	uint8_t i;

	if (!nfound) {
		uint8_t dnum = OW_SEARCH_FIRST;

		ow_reset();



		while (dnum != OW_LAST_DEVICE ) {
			if(nfound) memcpy(sensor_id[nfound], sensor_id[nfound-1], OW_ROMCODE_SIZE);
			dnum = ow_rom_search(dnum, sensor_id[nfound]);

			sprintf(b, "Found: "MACSTR", dnum: %u", MAC2STR(sensor_id[nfound]), dnum);
			esp_debug(b);
			_delay_ms(1000);

			nfound++;
		}

		if (!nfound) {
			esp_debug("Error: No sensor founds!");
			return;
		}

		sprintf(b, "Found: %u", nfound);
		esp_debug(b);

	}

	for (i = 0; i < nfound; i++) {
		if (DS18X20_conversion_in_progress()) return;
		DS18X20_read_decicelsius(&sensor_id[i][0], &temp[i]);
	}

	sprintf(b, "t1: %d, t2: %d, t3: %d", temp[0], temp[1], temp[2]);
	esp_debug(b);

	DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL);

}

int main(void) {
	char b[101];

	init_uart();			// init uart

	sei();
	// enable global interrupt

	send("Hello world");
	send("111");
	send("Hello world");

	DDRB |= 1 << 0;
	while (1) {
		PORTB ^= 1 << 0;
		_delay_ms(1000);
		send_temp();

		if (esp_isMsgToExe()) {
			esp_getMsg_done(b);

			if (strcmp(b, CMD_HELLO_AVR) == 0) {
				send(RESPONSE_HELLO_AVR);
			} else {
				send(RESPONSE_UNKNOWN_COMMAND);
			}

		}
	}

}

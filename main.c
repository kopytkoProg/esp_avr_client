#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include "uart/uart.h"
#include "esp/esp.h"
#include "commands.h"
#include "ds1820/onewire.h"
#include "ds1820/ds18x20.h"
#include "main.h"

static uint8_t sensor_id[NUM_SENSORS][OW_ROMCODE_SIZE];
static uint8_t nfound = 0;
static int16_t temp[NUM_SENSORS];

static void update_temp(void) {
	uint8_t i;

	if (!nfound) {
		uint8_t dnum = OW_SEARCH_FIRST;

		ow_reset();

		while (dnum != OW_LAST_DEVICE) {
			if (nfound) memcpy(sensor_id[nfound], sensor_id[nfound - 1], OW_ROMCODE_SIZE);
			dnum = ow_rom_search(dnum, sensor_id[nfound]);
			esp_debugf("Found: "IDSTR", dnum: %u", ID2STR(sensor_id[nfound]), dnum);
			nfound++;
		}

		if (!nfound) {
			esp_debug("Error: No sensor founds!");
			return;
		}

		esp_debugf("Found: %u", nfound);

	}

	for (i = 0; i < nfound; i++) {
		if (DS18X20_conversion_in_progress()) return;
		DS18X20_read_decicelsius(&sensor_id[i][0], &temp[i]);
	}

	esp_debugf("t1: %d, t2: %d, t3: %d", temp[0], temp[1], temp[2]);

	DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL);

}

/**
 * '{<cmd>:<param>, <param>, ...}'
 * '{<cmd>}'
 */
uint8_t cmp_cmd_of_msg(char *msg1, char *msg2) {
	uint8_t i = 0;

	while (*msg1 == '}' && *msg1 == ':' && *msg1 == *msg2) {
		i++;
		msg1++;
		msg2++;
	}
	return *msg1 == *msg2 ? 0 : i;
}

int main(void) {

	init_uart();			// init uart

	sei();
// enable global interrupt

	send("Hello world");

	DDRB |= 1 << 0;
	while (1) {
		PORTB ^= 1 << 0;

		update_temp();

		if (esp_isMsgToExe()) {
			char msg_buffer[ESP_RECEIVE_BUFFER_SIZE + 1];
			esp_getMsg_done(msg_buffer);

			if (strcmp(msg_buffer, CMD_HELLO_AVR) == 0) {

				send(RESPONSE_HELLO_AVR);

			} else if (strcmp(msg_buffer, CMD_GET_SENSORS_ID) == 0) {

				esp_sendf(IDSTR", "IDSTR", "IDSTR, ID2STR(sensor_id[0]), ID2STR(sensor_id[1]), ID2STR(sensor_id[2]));

			} else if (cmp_cmd_of_msg(msg_buffer, CMD_GET_TEMP) == 0) {
				/* 23 = strlen(28:ff:da:60:62:14:03:e4) */

				char b[IDSTR_LEN + 1];
				uint8_t i = 0;
				uint8_t j = 0;
				uint8_t k = 0;

				for (i = 0; i < nfound && k != IDSTR_LEN; i++) {

					sprintf(b, IDSTR, ID2STR(sensor_id[i]));

					for (j = 0; msg_buffer[j] != ':'; j++);
					j += 1;

					for (k = 0; k < IDSTR_LEN && msg_buffer[j + k] == b[k]; k++)
						;

				}

				if (k == IDSTR_LEN) esp_sendf("%i", temp[i-1]);
				else  esp_sendf("Err: id not found");

			} else {

				send(RESPONSE_UNKNOWN_COMMAND);

			}
			//esp_done();
		}
	}

}

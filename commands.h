#ifndef __commands_h__
#define __commands_h__

//==================== COMMANDS ===================
#define CMD_HELLO_AVR						"{hello-avr}"
#define CMD_GET_SENSORS_ID					"{get-sensor-id-avr}"
#define CMD_GET_TEMP						"{get-temp-avr:<id>}"

//==================== RESPONSES ===================
#define RESPONSE_UNKNOWN_COMMAND			"{unknown-command}"
#define RESPONSE_HELLO_AVR					"{hello-avr}"

//==================== HEADERS ===================
#define HEADER_DEBUG						"avr-debug"

#define T1					"{"
#define T2					"zzzzzzzzzzzzzzzzzzzzzzzzzXXXXzzzzzzzzzzzzzzzzzzzzzzzzzXXXXzzzzzzzzzzzasdz"
#define T3					"}"

#endif

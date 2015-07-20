#ifndef __esp_h__
#define __esp_h__

#define ESP_RECEIVE_BUFFER_SIZE  	(100)

void esp_debug(char *s);
char esp_isMsgToExe(void);
char* esp_getMsg(void);
void onChar(char c);
void esp_done(void);
void esp_getMsg_done(char *b);
void esp_debugf(char *s, ...);
void esp_sendf(char *s, ...);
void esp_send(char *s);
#endif

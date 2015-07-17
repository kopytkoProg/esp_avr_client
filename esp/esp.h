#ifndef __esp_h__
#define __esp_h__

void esp_debug(char *s);
char esp_isMsgToExe(void);
char* esp_getMsg(void);
void onChar(char c);
void esp_done(void);
void esp_getMsg_done(char *b);

#endif

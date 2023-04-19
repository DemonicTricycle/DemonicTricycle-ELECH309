#include <stdint.h>

#ifndef PI
#define PI 3.1415926535897932
#endif

#ifndef INT_MAX
#define INT_MAX 65535
#endif



int Uint16ToInt(uint16_t x);
float FloatAbs(float value);
int FloatSign(float value);

void sendUartChars(char *chars);
void breakUartLine(void);
void sendUartMessage(char *chars);
void sendUartInt16(int toSend);
int strlen(const char *s);
void itoa(int num, char *data, int ba);
void initUart(void);
void uint16_t_to_char_array(uint16_t val, char *arr, size_t arr_size);

void StartupMessage();

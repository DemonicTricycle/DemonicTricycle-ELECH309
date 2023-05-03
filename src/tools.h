#ifndef TEST
#include "xc.h"
//#include "libpic30.h" // Contains __delay_ms definition
#endif
#ifdef TEST
#include <stdint.h>
#endif

#ifndef PI
#define PI 3.1415926535897932
#endif

#ifndef UINT16_MAX
#define UINT16_MAX 65535
#endif

extern unsigned char c;
extern char* to_send;

int Uint16ToInt(uint16_t x); //unsigned int ?
float FloatAbs(float value);
int FloatSign(float value);

void sendUartChars(char *chars);
void breakUartLine(void);
void sendUartMessage(char *chars);
void sendUartInt16(int toSend);
int strlen(const char *s);
void itoa(int num, char *data, int ba);
void initUart(void);
//void uint16_t_to_char_array(uint16_t val, char *arr, size_t arr_size); //unsigned int ?
void uint16_t_to_char_array(unsigned int val, char *arr, unsigned int arr_size); //unsigned int ?

void StartupMessage();

void sendChar();
void sendString();
void sendInt16(int16_t to_send);
void sendInt32(int32_t to_send);
void sendIntConverted(int32_t to_send);
void sendLine();

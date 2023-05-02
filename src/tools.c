// <editor-fold defaultstate="collapsed" desc="Include and imports">
#ifndef TEST
#include "xc.h"
#include "libpic30.h" // Contains __delay_ms definition
#endif
#include <stdint.h>
#include <math.h>
#ifndef TEST
//#include "libpic30.h" // Contains __delay_ms definition
#endif
#ifdef TEST
#include <stdio.h>
#endif

// </editor-fold>

unsigned char c;
char* to_send;


int Uint16ToInt(uint16_t x)
{
    if (x > UINT16_MAX)
    {
#ifdef TEST
        printf("Uint16ToInt integer overflow");
#endif
        // do something else?
#ifndef TEST
        sendUartMessage("Uint16ToInt integer overflow");
#endif
    }
    int result = 0;
    result = (int)(x & 0x7FFF); // clear the MSB (most significant bit)
    if ((x & 0x8000) != 0)
    { // if the MSB was set, convert to negative
        result = -((int)(0x7FFF) + 1 - result); //! susceptible to int overflow !
    }
    return result;
}

float FloatAbs(float value)
{
    if (value < 0)
    {
        return -value;
    }
    return value;
}
int FloatSign(float value)
{
    if (value < 0)
    {
        return -1;
    }
    return 1;
}

// --- from motors code ---
// <editor-fold defaultstate="collapsed" desc="Debug (motors)">

int strlen(const char *s) {

    int len = 0;
    while (*s++) {
        len++;
    }

    return len;
}

void itoa(int num, char *data, int ba) {
    int i = 0;
    int sign = 0;

    /* Handle negative integers */
    if (num < 0 && ba == 10) {
        sign = 1;
        num = -num;
    }

    /* Process individual digits */
    while (num != 0) {
        int rem = num % ba;
        data[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / ba;
    }

    /* If number is negative, append '-' */
    if (sign) {
        data[i++] = '-';
    }

    /* Add null terminator */
    data[i] = '\0';

    /* Reverse the string */
    int j, len = strlen(data);
    for (i = 0, j = len - 1; i < j; i++, j--) {
        char temp = data[i];
        data[i] = data[j];
        data[j] = temp;
    }
}

void sendUartChars(char *chars)
{
    #ifndef TEST
    int length = strlen(chars);
    for (int i = 0; i < length; i++)
    {
        while (U1STAbits.UTXBF)
        {
            // wait until the buffer is empty
        }
        U1TXREG = chars[i];
    }
    #endif
    #ifdef TEST
    printf(chars);
    #endif
}
void breakUartLine(void)
{
    char *data2 = "\n";
    sendUartChars(data2);
}
void sendUartMessage(char *chars)
{
    sendUartChars(chars);
    breakUartLine();
}
void sendUartInt16(int toSend)
{
    char data[8];
    itoa(toSend, data, 10);
    sendUartChars(&data);
    breakUartLine();
}

void uint16_t_to_char_array(uint16_t val, char *arr, size_t arr_size)
{
    int i;
    for (i = 0; i < 16 && i < arr_size; i++)
    {
        arr[i] = (val & (1 << (15 - i))) ? '1' : '0';
    }
    if (i < arr_size)
    {
        arr[i] = '\0';
    }
}
void StartupMessage()
{
    #ifndef TEST
    //__delay_ms(10);
    sendUartMessage("Booting : ");
    //__delay_ms(10);
    #endif
}

void initUart(void)
{
#ifndef TEST
    _U1RXR = 6; // U1RX -> RP6 //TODO: put in a define in parameters.h
    _RP7R = 3;  // RP7 -> U1Tx //TODO: put in a define in parameters.h

    // TODO: set with a define ?
    //  Configuration of UART1 with 8N1 frame format
    U1MODEbits.PDSEL = 0; // 8 bits, no parity
    U1MODEbits.STSEL = 0; // 1 stop bit
    // baud rate = FCY / (16*(U1BRG+1)
    // => U1BRG = (3.685MHz / (16*57.6kHz)) - 1  =  2.998
    //  20 MHz / (16 * 115.2 kHz)) - 1 = 9.85
    U1MODEbits.BRGH = 0; // High Baud Rate Select bit
    // U1BRG = 80; // 30864 baudrate
    // U1BRG = 8; // 278 000
    U1BRG = 1; // 1 250 000 //TODO: put in a define in parameters.h ?

    U1MODEbits.UARTEN = 1; // enable UART
    U1STAbits.UTXEN = 1;   // enable transmission
#endif
}
// </editor-fold>

// --- from filter code ---
// <editor-fold defaultstate="collapsed" desc="Debug (filter)">
//unsigned char c;
//char* to_send;

/*
void init_uart()
{
    _U1RXR = 6;    // U1RX -> RP6 //put in a define in parameters.h
    _RP7R = 3;     // RP7 -> U1Tx //put in a define in parameters.h

    //set with a define ?
    // Configuration de l'UART1 avec un format de trame 8N1
    U1MODEbits.PDSEL = 0;       // 8 bits, no parity
    U1MODEbits.STSEL = 0;       // 1 stop bit
    // baud rate = FCY / (16*(U1BRG+1)
    // => U1BRG = (3.685MHz / (16*57.6kHz)) - 1  =  2.998
    //  20 MHz / (16 * 115.2 kHz)) - 1 = 9.85
    U1MODEbits.BRGH = 0; // High Baud Rate Select bit
    // U1BRG = 80; // 30864 de baudrate
    // U1BRG = 8; // 278 000
    U1BRG = 1;             // 1 250 000
    U1MODEbits.UARTEN = 1; // activate UART
    U1STAbits.UTXEN = 1;   // TX on
}
*/

void sendChar ()
{
#ifndef TEST
    while (U1STAbits.UTXBF) {}  // wait until the buffer is empty (writing to a register is much faster than the transmission)
    U1TXREG = c;                // write to the TX register
#endif
#ifdef TEST
    printf("%c", c);
#endif
}

void sendString ()
{
    while(*to_send)
    {
        c = *to_send++;
        sendChar();
    }
}

void sendInt16 (int16_t to_send)
{
    for (int i = 15; i >= 0; i--)
    {
        c = (char) 48 + ((to_send >> i) & 1);
        sendChar();
    }
    c = '\n';
    sendChar();
}

void sendInt32 (int32_t to_send)
{
    for (int i = 31; i >= 0; i--)
    {
        c = (char) 48 + ((to_send >> i) & 1);
        sendChar();
    }
    c = '\n';
    sendChar();
}

void sendIntConverted (int32_t to_send)
{
    char buffer[10];
    int i = 0;
    int negative = 0;
    if (to_send < 0)
    {
        to_send = -to_send;
        negative = 1;
    }

    // Convert integer to string in reverse order
    do {
        buffer[i++] = (char) (to_send % 10) + '0'; // convert digit to ASCII equivalent
        to_send /= 10;
    } while (to_send > 0);

    if (negative == 1)
    {
        c = '-';
        sendChar();
    }

    // Send string over UART in reverse order
    for (int j = i - 1; j >= 0; j--)
    {
        c = buffer[j];
        sendChar();
    }
    c = '\n'; sendChar();
}

void sendLine()
{
    sendString();
    c = '\n';
    sendChar();
}
// </editor-fold>
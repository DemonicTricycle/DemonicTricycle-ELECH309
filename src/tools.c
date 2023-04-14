// <editor-fold defaultstate="collapsed" desc="Include and imports">
#include "xc.h"
#include <stdint.h>
#include <math.h>
#include "libpic30.h" // Contains __delay_ms definition

// </editor-fold>

int Uint16ToInt(uint16_t x)
{
    int result = 0;
    result = (int)(x & 0x7FFF); // clear the MSB (most significant bit)
    if ((x & 0x8000) != 0)
    { // if the MSB was set, convert to negative
        result = -((int)(0x7FFF) + 1 - result);
    }
    return result;
}

float floatAbs(float value)
{
    if (value < 0)
    {
        return -value;
    }
    return value;
}
int floatSign(float value)
{
    if (value < 0)
    {
        return -1;
    }
    return 1;
}

// <editor-fold defaultstate="collapsed" desc="Debug">
void sendUartChars(char *chars)
{
    int length = strlen(chars);
    for (int i = 0; i < length; i++)
    {
        while (U1STAbits.UTXBF)
        {
        }
        U1TXREG = chars[i];
    }
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
void sendUartInt(int toSend)
{
    char data[8];
    itoa(toSend, data, 10);
    sendUartChars(&data);
    breakUartLine();
}
int strlen(const char *s)
{

    int len = 0;
    while (*s++)
    {
        len++;
    }

    return len;
}
void itoa(int num, char *data, int ba)
{
    int i = 0;
    int sign = 0;

    /* Handle negative integers */
    if (num < 0 && ba == 10)
    {
        sign = 1;
        num = -num;
    }

    /* Process individual digits */
    while (num != 0)
    {
        int rem = num % ba;
        data[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / ba;
    }

    /* If number is negative, append '-' */
    if (sign)
    {
        data[i++] = '-';
    }

    /* Add null terminator */
    data[i] = '\0';

    /* Reverse the string */
    int j, len = strlen(data);
    for (i = 0, j = len - 1; i < j; i++, j--)
    {
        char temp = data[i];
        data[i] = data[j];
        data[j] = temp;
    }
}
void initUart(void)
{
    _U1RXR = 6; // U1RX -> RP6
    _RP7R = 3;  // RP7 -> U1Tx

    // Configuration de l'UART1 avec un format de trame 8N1, ? 57600 bits/s
    U1MODEbits.PDSEL = 0; // 8 bits, pas de parit?
    U1MODEbits.STSEL = 0; // 1 stop bit

    U1MODEbits.BRGH = 0;
    U1BRG = 3;

    U1MODEbits.UARTEN = 1; // on active l'UART //TODO: enable UART with switch, and only execute sendChars etc if enabled
    U1STAbits.UTXEN = 1;   // on active l'?mission
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
    __delay_ms(10);
    sendMessage("Booting : ");
    __delay_ms(10);
}

// </editor-fold>
#include <xc.h>
#include "mcp4821.h"


#define SPI1_SCK_OUT    0b01000
#define SPI1_SDO        0b00111


int16_t dataMask;

void mcp4821Init(int16_t gain) {
    dataMask = gain;
    MCP4821_CS_TRIS = 0;
    MCP4821_CS_LAT = 1;
    MCP4821_SCK = SPI1_SCK_OUT;      // Sélection de la patte pour SCK
	MCP4821_SDI = SPI1_SDO;          // Sélection de la patte pour SDO
    SPI1CON1bits.MODE16 = 1;    // mots de 16 bits
    SPI1CON1bits.PPRE = 3;      // primary prescaler = 16:1
    SPI1CON1bits.SPRE = 6;      // secondary prescaler = 1:1
    SPI1CON1bits.CKP = 1;       // on veut le mode (1,1), ...
    SPI1CON1bits.CKE = 0;       // mais (CKE est l'inverse de CPHA)
    SPI1CON1bits.MSTEN = 1;     // mode master
    SPI1STATbits.SPIROV = 0;
    SPI1STATbits.SPIEN = 1;     // active le périphérique
}


void mcp4821Write(int16_t sig)  {
    /* reset the SPI interrupt flag because we will poll it to detect
     * the end of the transmission */
    _SPI1IF = 0;
    
    MCP4821_CS_LAT = 0;     // enable the DAC chip select
    sig = sig & 0x0FFF;     // data on 12 bit => set the 4 MSB to 0 (just to be sure)
    sig = sig | dataMask;   // SHDN = 1 (bit11) => DAC active and set gain to x1 or x2
    SPI1BUF = sig;          // send the data to the DAC 
    while(!_SPI1IF);        // wait for the and of transmission
    MCP4821_CS_LAT = 1;     // disable the DAC chip select
    SPI1BUF;                // dummy read of the "received data" to reset the state machine
}

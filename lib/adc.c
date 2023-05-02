#include "xc.h"
#include "adc.h"

void adcInit(int mode) {
    if ( (mode == ADC_MANUAL_SAMPLING) || (mode == ADC_TIMER3_SAMPLING) ) {
        AD1CON1bits.AD12B = 1;          // Converter on 12 bits
        AD1CON3bits.ADCS = 3;           // ADC's clock divider
        AD1CON1bits.ASAM = 1;           // auto sample enabled
        AD1CSSLbits.CSS0 = 1;           // Conversion of only the AN0 pin
        AD1PCFGLbits.PCFG0 = 0;         // AN0 in analog mode
        AD1CON1bits.SSRC = mode;        // mode choice
        AD1CON1bits.ADON = 1;           // ADC enabling
        while(AD1CON1bits.SAMP == 0);   // We wait for the configuration to be finished
    }
}


int adcRead(void) {
    if (AD1CON1bits.DONE) {
        AD1CON1bits.DONE = 0;
        return ADC1BUF0;
    } else {
        //If we try to read during a conversion: returns -32768, being an impossible value for a conversion result
        return (-32768);
    }
}


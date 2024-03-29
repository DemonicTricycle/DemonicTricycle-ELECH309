#include "xc.h"
#include "lib/adc.h"

unsigned char c;
char* to_send;

void sendChar () {
    while (U1STAbits.UTXBF) {}  // wait until the buffer is empty (writing to a register is much faster than the transmission)
    U1TXREG = c;                // write to the TX register
}

void sendString () {
    while(*to_send) {
        c = *to_send++;
        sendChar();
    }
}

void sendInt16 (int16_t to_send) {
    for (int i = 15; i >= 0; i--) {
        c = (char) 48 + ((to_send >> i) & 1);
        sendChar();
    }
    c = '\n';
    sendChar();
}

void sendInt32 (int32_t to_send) {
    for (int i = 31; i >= 0; i--) {
        c = (char) 48 + ((to_send >> i) & 1);
        sendChar();
    }
    c = '\n';
    sendChar();
}

void sendIntConverted (int32_t to_send) {
    char buffer[10];
    int i = 0;
    int negative = 0;
    if (to_send < 0) {
        to_send = -to_send;
        negative = 1;
    }

    // Convert integer to string in reverse order
    do {
        buffer[i++] = (char) (to_send % 10) + '0'; // convert digit to ASCII equivalent
        to_send /= 10;
    } while (to_send > 0);
    
    if (negative == 1) {
        c = '-';
        sendChar();
    }
    
    // Send string over UART in reverse order
    for (int j = i - 1; j >= 0; j--) {
        c = buffer[j];
        sendChar();
    }
    c = '\n'; sendChar();
}

void sendLine() {
    sendString();
    c = '\n';
    sendChar();
}

void init_uart() {
    _U1RXR = 6;    // U1RX -> RP6
	_RP7R = 3;     // RP7 -> U1Tx

    // Configuration de l'UART1 avec un format de trame 8N1
    U1MODEbits.PDSEL = 0;       // 8 bits, no parity
    U1MODEbits.STSEL = 0;       // 1 stop bit
    /* baud rate = FCY / (16*(U1BRG+1) 
     * => U1BRG = (3.685MHz / (16*57.6kHz)) - 1  =  2.998
        20 MHz / (16 * 115.2 kHz)) - 1 = 9.85 */
    U1MODEbits.BRGH = 0; // High Baud Rate Select bit
    // U1BRG = 80; // 30864 de baudrate
    // U1BRG = 8; // 278 000
    U1BRG = 1; // 1 250 000
    U1MODEbits.UARTEN = 1;      // activate UART
    U1STAbits.UTXEN = 1;        // TX on
}

#define FLOORS 4
const short FLOOR_ORDER = 2;

// filter constants

#define M 18 // Q18.14
#define factor 262144 // 1 << M
#define ff (float) (factor) // sans les parenth�ses �a marche pas � cause de la priorit� de << aled j'ai perdu 10 min sur �a
#define fs 15000

// filter around 1100 hz = filter 1
/*const double gs_d_1[] = {0.003035, 0.002955, 0.02263, 0.02271};
const double bs_d_1[4][3] = {{1, 2, 1}, {1, 2, 1}, {1, -2, 1}, {1, -2, 1}};
const double as_d_1[4][3] = {{1, -1.77495574, 0.98471132}, {1, -1.7807248, 0.98490396}, {1, -1.77882789, 0.99358215}, {1, -1.79257281, 0.99377646}};*/

const int32_t gs_1[] = {(int32_t) (0.002019 * ff), (int32_t) (0.001964*ff), (int32_t) (0.0228*ff), (int32_t) (0.02294*ff)};
const int32_t bs_1[4][3] = {{1*ff, 2*ff, 1*ff}, {1*ff, 2*ff, 1*ff}, {1*ff, -2*ff, 1*ff}, {1*ff, -2*ff, 1*ff}};
const int32_t as_1[4][3] = {{factor, ff * -1.84602348, ff * 0.98747166}, {factor, ff * -1.84997046, ff * 0.98763379}, {factor, ff * -1.85002393, ff * 0.99474384}, {factor, ff * -1.8593739, ff * 0.99490711}};


// filter around 1100 hz = filter 2
/*const double gs_d_2[] = {0.003035, 0.002955, 0.02263, 0.02271};
const double bs_d_2[4][3] = {{1, 2, 1}, {1, 2, 1}, {1, -2, 1}, {1, -2, 1}};
const double as_d_2[4][3] = {{1, -1.77495574, 0.98471132}, {1, -1.7807248, 0.98490396}, {1, -1.77882789, 0.99358215}, {1, -1.79257281, 0.99377646}}; */

const int32_t gs_2[] = {0.003035 * ff, 0.002955*ff, 0.02263*ff, 0.02271*ff};
const int32_t bs_2[4][3] = {{1*ff, 2*ff, 1*ff}, {1*ff, 2*ff, 1*ff}, {1*ff, -2*ff, 1*ff}, {1*ff, -2*ff, 1*ff}};
const int32_t as_2[4][3] = {{factor, ff * -1.77495574, ff * 0.98471132}, {factor, ff * -1.7807248, ff * 0.98490396}, {factor, ff * -1.77882789, ff * 0.99358215}, {factor, ff * -1.79257281, ff * 0.99377646}};

int32_t ys_1[5][12]; // dynamic table for filter 1
int32_t ys_2[5][12]; // dynamic table for filter 1
int32_t last_values_1[17];
int32_t last_values_2[17];

int16_t seuil = 500;

short pointer_last = 10;
short pointer_before = 11;
short pointer_current = 0;
short pointer_last_values = 0;

short sample_count = 0;
short bit_count = 0;
short filtre_0 = 0;
short filtre_1 = 0;
short is_listening = 0;
int32_t max_1 = 0;
int32_t max_2 = 0;
int16_t average_sample = 0;

short noise_counter = 0;

void init_tables() {
    for (int i = 0; i < FLOORS + 1; i++) {
        for (int zz = 0; zz < 12; zz ++) {
            ys_1[i][zz] = 0;
            ys_2[i][zz] = 0;
        }
    }    
}

void print_values() {
    to_send = "M : "; sendString();
    sendIntConverted(M);
    to_send = "Factor : "; sendString();
    sendIntConverted(factor);
    to_send = "Filter 1 - 900 hz : "; sendLine();
    for (int k = 0; k < FLOORS; k++) {
        to_send = "Gain k: "; sendLine();
        sendIntConverted(gs_1[k]);
        to_send = "as k: "; sendLine();
        for (int i = 0; i < 3; i++) sendIntConverted(as_1[k][i]);
        to_send = "bs k: "; sendLine();
        for (int i = 0; i < 3; i++) sendIntConverted(bs_1[k][i]);
    }
    to_send = "Filter 2 - 1100 hz : "; sendLine();
    for (int k = 0; k < FLOORS; k++) {
        to_send = "Gain k: "; sendLine();
        sendIntConverted(gs_2[k]);
        to_send = "as k: "; sendLine();
        for (int i = 0; i < 3; i++) sendIntConverted(as_2[k][i]);
        to_send = "bs k: "; sendLine();
        for (int i = 0; i < 3; i++) sendIntConverted(bs_2[k][i]);
    }
}

void reset_tables() {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 12; j++) {
            ys_1[i][j] = 0;
            ys_2[i][j] = 0;
        }
    }
    for (int i = 0; i < 17; i++) {
        last_values_1[i] = 0;
        last_values_2[i] = 0;
    }
}

void init_leds() {
    _TRISB12 = 0; // green = listening
    _TRISB4 = 0; // red = fluke
    _TRISB14 = 0; // blue = bit 0
    _TRISB5 = 0; // yellow = bit 1
}

int main(void) {
    // Configure PLL prescaler, PLL postscaler, PLL divisor
    PLLFBD = 43; // found using trial and error
    CLKDIVbits.PLLPOST=0; // N2 = 2
    CLKDIVbits.PLLPRE=0; // N1 = 2
    // Initiate Clock Switch to Primary Oscillator with PLL (NOSC = 0b011)
    __builtin_write_OSCCONH(0x03);
    __builtin_write_OSCCONL(OSCCON | 0x01);

	init_uart();
    reset_tables();
    init_leds();
    
    to_send = "Chip strating !";
    sendLine();
    
    print_values();
    
    int16_t voltage;
    adcInit(ADC_TIMER3_SAMPLING);
    
    // PR3 = 40 MHz / 15 kHz - 1 = 2665.5
    PR3 = 2755; // �a donne 15017 Hz mesur� au picoscope
    PR1 = 41800; // �a donne 994 Hz
    // starts timer1
    T1CONbits.TON = 1;
    // Enable timer1 interrupt, so that its ISR will be called on overflow
    _T1IE = 1;  // _T>1IE is a shortcut for IEC0bits.T1IE

	T3CONbits.TON = 1;
    
	while(1) {
        if (adcConversionDone()) {
            voltage = adcRead();
            if (voltage >= 4096 || voltage < 0)
                continue;
            
            ys_1[0][pointer_current] = voltage; // updates current value
            ys_2[0][pointer_current] = voltage;

            for (int k = 0; k < FLOORS; k++) { // filter id
                ys_1[k+1][pointer_current] =((gs_1[k] * ( (bs_1[k][0]*ys_1[k][pointer_current] + bs_1[k][1]*ys_1[k][pointer_before] + bs_1[k][2]*ys_1[k][pointer_last]) >> M)) - as_1[k][1]*ys_1[k+1][pointer_before] - as_1[k][2]*ys_1[k+1][pointer_last] ) >> (M);
                ys_2[k+1][pointer_current] =((gs_2[k] * ( (bs_2[k][0]*ys_2[k][pointer_current] + bs_2[k][1]*ys_2[k][pointer_before] + bs_2[k][2]*ys_2[k][pointer_last]) >> M)) - as_2[k][1]*ys_2[k+1][pointer_before] - as_2[k][2]*ys_2[k+1][pointer_last] ) >> (M);
            } 
            
            last_values_1[pointer_last_values] = ys_1[4][pointer_current];
            last_values_2[pointer_last_values] = ys_2[4][pointer_current];

            // updating pointers
            pointer_last = (pointer_last + 1) % 12; pointer_before = (pointer_before + 1) % 12; pointer_current = (pointer_current + 1) % 12;
            pointer_last_values = (pointer_last_values + 1) % 18;
        }
	}
    while(1) {}
}

void __attribute__((interrupt, no_auto_psv))_T1Interrupt(void) {
    // ISR code does the same things that the main loop did in polling
    _T1IF = 0;
    
    max_1 = 0;
    max_2 = 0;
    for (int ii = 0; ii < 18; ii++) {
        if (max_1 < last_values_1[ii]) max_1 = last_values_1[ii];
        if (max_2 < last_values_2[ii]) max_2 = last_values_2[ii];
    }

    filtre_0 = (max_1 > seuil) ? 1 : 0;
    filtre_1 = (max_2 > seuil) ? 1 : 0;
    /*
    if (max_1 > seuil) sendIntConverted(max_1);
    if (max_2 > seuil) sendIntConverted(max_2);*/
    
    if (filtre_0 == 1 || filtre_1 == 1) {
        if (sample_count == 0) {
            bit_count = 0;
            average_sample = 0;
            is_listening = 1;
            noise_counter = 0;
            to_send = "Listening"; sendLine();
            _LATB12 = 1; _LATB4 = 0;
        }
        
        average_sample += filtre_1;
        average_sample -= filtre_0; 
    }
    
    if (is_listening && bit_count == 0 && sample_count < 75 && filtre_0 == 0 && filtre_1 == 0) {
        noise_counter ++;
        if (noise_counter > 10) {
            // probably fluke
            sample_count = 0;
            bit_count = 0;
            average_sample = 0;
            is_listening = 0; _LATB12 = 0;
            to_send = "Fluke"; sendLine();
            _LATB4 = 1;
            reset_tables();
            noise_counter = 0;

            //resetFSM(); //?
        }
    }
    
    if (sample_count == 100) {
        bit_count ++;
        sample_count = 0;
        to_send = "Bit received"; sendLine();
        // bit has been received
        _LATB14 = (average_sample < 0);
        _LATB5 = (average_sample > 0);
        c = '0' + ((average_sample > 0) ? 1 : 0);
        sendChar();
        c = '\n';
        sendChar();

        // send to FrameFSM() //?

        average_sample = 0;
    }
    
    if (bit_count == 13) {
        sample_count = 0;
        bit_count = 0;
        average_sample = 0;
        c = '\n';
        sendChar();
        is_listening = 0; _LATB12 = 0; _LATB14 = 0; _LATB5 = 0;
        reset_tables();

        //resetFSM(); //?
    }
    if (is_listening)
        sample_count ++;
}

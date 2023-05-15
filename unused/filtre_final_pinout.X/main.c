#include "xc.h"
#include "lib/adc.h"
#include "system.h"
#include "frameFSM.h"

// <editor-fold defaultstate="collapsed" desc="Include and imports from filter">
#define FLOORS 4
// const short FLOOR_ORDER = 2;

// filter constants

#define M 18 // Q18.14
#define factor 262144 // 1 << M
#define ff (float) (factor) // sans les parenthèses ça marche pas à cause de la priorité de << aled j'ai perdu 10 min sur ça
#define fs 15000

#define k_transla 3.69
#define k_rota 1.03 * 3

#define CRUISE_ROTATION_SPEED 4  // rad / second
#define MAX_ACCELERATION_TIME 1.2  // seconds
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

int16_t seuil = 300;

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
int16_t total_sample_1 = 0;
int16_t total_sample_2 = 0;
int16_t average_sample = 0;

short noise_counter = 0;

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="Include and imports from regulator">

#include <math.h>

//#define FCY 3685000     // cycle frequency. Needed for __delay_ms
#include "libpic30.h"   // Contains __delay_ms definition
#define NUM_TICKS_PER_TURN 1423
#define PI 3.1415926535897932
#define INT_MAX 65535
#define WHEEL_RADIUS 0.04 //in m
#define acceptable_error_transla 0.04
#define acceptable_error_rota 0.06981  //(float)4 / 360 * 2 * PI
//PWM parameters and macro
#define PWM_TIME_PERIOD 0.001 //In s, one period is one up AND one down,
//not just one 'hill' up
#define CLOCK_PRESCALER_BINARY 0b00 //In ms
#define CLOCK_PRESCALER 1 //0b00 corresponds to a scale of 1
//Adaptation of the formula in the datasheet
#define GET_PTPER() (int)(((float)FCY *  PWM_TIME_PERIOD/ CLOCK_PRESCALER) - 1) 
#define WHEELBASE 0.135
#define MAX_PWM 0.8

typedef enum {
    forward,
    backward
} direction;

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="PWM">
void PWMInit(void)
{
    //If more details are required, check the following link :
    //https://ww1.microchip.com/downloads/en/DeviceDoc/70187E.pdf
    
    PTCONbits.PTEN = 0; //Start by disabling PWM pulse generation
    PTCONbits.PTOPS = 0; // PWM time base output postscale
    PTCONbits.PTCKPS = CLOCK_PRESCALER_BINARY; // PWM time base input clock Prescale
    PTPER = GET_PTPER(); // PWM Timebase period
    PWMCON1bits.PMOD1 = 1; //Sets to independent output mode
    PWMCON1bits.PMOD2 = 1; //Sets to independent output mode
    PWMCON1bits.PEN1L = 1; //RB15// Enables pin for PWM output
    PWMCON1bits.PEN2L = 1; //RB13
    PDC1 = 0;
    PDC2 = 0;
    PTCONbits.PTEN = 1; //Ends by enabling PWM pulse generation
}

void setPWM1(float param) //1 = full power, 0 = turned off
{
    PDC1 = (int)((float)GET_PTPER() * param * 2);
}
void setPWM2(float param) //1 = full power, 0 = turned off
{
    PDC2 = (int)((float)GET_PTPER() * param * 2);
}



// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Encoders">
void InitialiseEncoders(void)
{
    QEI1CONbits.QEIM = 0b011;//Disables the module
    QEI2CONbits.QEIM = 0b011;//Disables the module
    
    RPINR14bits.QEA1R = 10; //Assigns channel A of QE1 to RP10(=RB10)
    RPINR14bits.QEB1R = 11; //Assigns channel B of QE1 to RP11(=RB11)
    RPINR16bits.QEA2R = 8;//Assigns channel A of QE2 to RP8(=RB8)
    RPINR16bits.QEB2R = 9; //Assigns channel B of QE2 to RP9(=RB9)
    
    QEI1CONbits.QEIM = 0b111;        
    QEI2CONbits.QEIM = 0b111; //Enable the quadrature encoder interface
    //x4 mode with position counter reset by match
}


int Uint16ToInt(uint16_t x) {
    int result = 0;
    result = (int)(x & 0x7FFF); // clear the MSB (most significant bit)
    if ((x & 0x8000) != 0) { // if the MSB was set, convert to negative
        result = -((int)(0x7FFF) + 1 - result);
    }
    return result;
}

void ResetPos(void)
{
    POS1CNT = 0;
    POS2CNT = 0;    
}

float GetPosMotor1(void) //Get the position in radian
{ 
    return ((float)Uint16ToInt(POS1CNT)*(2*PI))/1423.0;
}
float GetPosMotor2(void)
{        
    return ((float)Uint16ToInt(POS2CNT)*(2*PI))/1423.0;
}
float GetDistMotor1(void) //Get the distance in m
{
    return GetPosMotor1() * WHEEL_RADIUS;
}
float GetDistMotor2(void)
{
    return GetPosMotor2() * WHEEL_RADIUS;
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Math function">
float Abs(float value)
{
    if (value < 0)
    {
        return -value;
    }
    return value;
}
int Sign(float value)
{
    if (value < 0)
    {
        return -1;
    }
    return 1;
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Movement">

void CapMotorVoltage(float *value)
{
    if (Abs(*value) > MAX_PWM)
    {
       *value = MAX_PWM *  Sign(*value);
    }    
}


void SetMotor1Dir(direction dir)
{
    if (dir == forward)
    {
        LATBbits.LATB2 = 1;
    }    
    if (dir == backward)
    {
        LATBbits.LATB2 = 0;
    }
}

void SetMotor2Dir(direction dir)
{
    if (dir == forward)
    {
        LATBbits.LATB3 = 1;
    }    
    if (dir == backward)
    {
        LATBbits.LATB3 = 0;
    }
}

void SetMotor1(float value)
{
    setPWM1(Abs(value));
    direction dir = forward;
    if (value < 0)
    {
        dir = backward;
    }
    SetMotor1Dir(dir);
}

void SetMotor2(float value)
{
    setPWM2(Abs(value));
    direction dir = forward;
    if (value < 0)
    {
        dir = backward;
    }
    SetMotor2Dir(dir);
}

float GetTarget(float time, float final_target) //time in s, end in m = final distance
{
    int sign = Sign(final_target);
    float end = Abs(final_target);
    float acceleration = 0.5;
    
    if (end < 0.5)
    {
        float half_time = sqrt(end / 0.5); 
        if (time > half_time * 2)
        {
            return end * sign;
        }
        if (time < half_time)
        {
            return (time * time * acceleration / 2) * sign;
        }
        else
        {
            float distAldreadyMade = half_time * half_time * acceleration / 2;
            float time_since_slowing = time - half_time;
            return (distAldreadyMade  + 0.5 * half_time * time_since_slowing - time_since_slowing * time_since_slowing * 0.25) * sign;
        }
    }
    if (time < 1)
    {
        return (time * time * 0.25) * sign;
    } 

    float time_at_cruise = (end - 0.5)/0.5;
    
    if (time >= time_at_cruise + 1)//Slowing down
    {
        float time_remaining = time_at_cruise + 2 - time;
        float time_since_slowing = time - time_at_cruise - 1;
        
        if (time_remaining < 0)
        {
            return (0.5 *  time_at_cruise + 0.5) * sign;
        }
        
        return (0.5 * time_at_cruise + 0.25 + 0.5 * time_since_slowing - time_since_slowing * time_since_slowing * 0.25) * sign;
    }
    return ((time - 1 ) * 0.5 + 0.25) * sign; 
}

float getTargetAngle(float given_angle, float time_since_start) 
{//Not very optimised but at least somewhat readable


    int sign = Sign(given_angle);
    float target_angle = Abs(given_angle);
    // Calculate the maximum angle that can be reached before reaching the cruise speed
    float MAX_ANGLE = (float)CRUISE_ROTATION_SPEED * 0.5 * (MAX_ACCELERATION_TIME * 2);

    float angle_to_return = 0;
    
    if (target_angle > MAX_ANGLE)
    {
        return 0; //Such an angle is not sendable in 8 bits
    }
    float acceleration = (float)CRUISE_ROTATION_SPEED / MAX_ACCELERATION_TIME; //The acceleration rate
    float half_time = sqrt(target_angle / acceleration); // The time at wich we start decelerating

    if (time_since_start < half_time)
    {
        //Simple acceleration
       angle_to_return =  time_since_start * time_since_start * acceleration / 2;
    }
    else
    {
        float time_since_slowing = time_since_start - half_time;
        //Distance aldready made when accelerating
        angle_to_return = half_time * half_time * acceleration / 2;
        //Distance since deceleration
        angle_to_return += acceleration * (half_time * time_since_slowing - time_since_slowing * time_since_slowing / 2);
    }
    if (time_since_start >= half_time * 2)
    {
        angle_to_return = target_angle;
    }
    
    return angle_to_return* sign;
}

float getAngle()
{
    return (GetDistMotor2() - GetDistMotor1()) / WHEELBASE; 
}

void Move(float translation, float angle)
{    
    T1CONbits.TON = 0;
    /*sendChars("Moving\n");
    sendIntConverted((int32_t)(1000*translation));
    sendChars("\n");
    sendIntConverted((int32_t)(1000*angle));
    return;*/
    ResetPos(); //Sets the encoders to 0
    
    float error_translation = acceptable_error_transla + 1;
    float error_rotation = acceptable_error_rota + 1;
    
    

    float time = 0;
    float target_transla = GetTarget(time, translation);
    float target_rota = getTargetAngle(angle, time); 
    int time_interval = 1; //ms
    PR2 = 41800; //(FCY/1000 -1) * time_interval;//10 ms 41800
    T2CONbits.TON = 1;
    
    while (
            (Abs(error_translation) > acceptable_error_transla)
            || (Abs(error_rotation) > acceptable_error_rota)
            || (Abs(target_transla) < Abs(translation)) || (Abs(target_rota) < Abs(angle))
            )
    {
        if (_T2IF == 1)
        {
            _T2IF = 0;
           
            time += (float)time_interval / 1000;
           
            target_transla = GetTarget(time, translation);

            error_translation = target_transla - (GetDistMotor1() + GetDistMotor2()) / 2;     
            target_rota = getTargetAngle(angle, time);
            error_rotation = getAngle() - getTargetAngle(angle, time);        
            float v1 = error_translation * k_transla + error_rotation * k_rota;
            CapMotorVoltage(&v1);
            SetMotor1(v1);
            float v2 = error_translation * k_transla - error_rotation * k_rota;
            CapMotorVoltage(&v2);
            SetMotor2(v2);
        }
    } 
    SetMotor1(0);
    SetMotor2(0);
    T2CONbits.TON = 0;
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Debug">
void sendChars(char *chars)
{
    int length = strlen(chars);
    for (int i = 0; i < length; i++)
            {
                while (U1STAbits.UTXBF) {}
                U1TXREG = chars[i]; 
            }
}
void breakLine(void)
{
    char *data2 = "\n";
    sendChars(data2);
}
void sendMessage(char *chars)
{
    sendChars(chars);
    breakLine();
}
void sendInt(int toSend)
{    
    char data[8] ;
    itoa(toSend, data, 10);
    sendChars(&data);
    breakLine();
}
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
void initUart(void)
{
    _U1RXR = 6;    // U1RX -> RP6
	_RP7R = 3;     // RP7 -> U1Tx

    // Configuration de l'UART1 avec un format de trame 8N1, ? 57600 bits/s
    U1MODEbits.PDSEL = 0;       // 8 bits, pas de parit?
    U1MODEbits.STSEL = 0;       // 1 stop bit
   
    U1MODEbits.BRGH = 0;
    U1BRG = 3;
    
    U1MODEbits.UARTEN = 1;      // on active l'UART
    U1STAbits.UTXEN = 1;        // on active l'?mission
}


void uint16_t_to_char_array(uint16_t val, char *arr, size_t arr_size) {
    int i;
    for (i = 0; i < 16 && i < arr_size; i++) {
        arr[i] = (val & (1 << (15 - i))) ? '1' : '0';
    }
    if (i < arr_size) {
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

void initialiseMotors(void)
{ 
  direction dir = forward;
  TRISBbits.TRISB2 = 0; 
  TRISBbits.TRISB3 = 0;//Declares dir pins as output
  SetMotor2Dir(dir);
  SetMotor2Dir(dir);
}
void initialise (void)
{
  initialiseMotors();  
  InitialiseEncoders(); 
  PWMInit();
  initUart();    
  StartupMessage(); 
  __delay_ms(1000);    
}

// <editor-fold defaultstate="collapsed" desc="Debug area from filter">
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
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Init from filter">
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
// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="Filter main">
int main(void) {
    
   
    // Configure PLL prescaler, PLL postscaler, PLL divisor
    PLLFBD= 43; // par essai erreur
    CLKDIVbits.PLLPOST=0; // N2 = 2
    CLKDIVbits.PLLPRE=0; // N1 = 2
    initialise();
    // Initiate Clock Switch to Primary Oscillator with PLL (NOSC = 0b011)
    __builtin_write_OSCCONH(0x03);
    __builtin_write_OSCCONL(OSCCON | 0x01);

	init_uart();
    reset_tables();
    init_leds();
    resetFSMtest();
    
    to_send = "Lancement du chip !";
    sendLine();
    
    print_values();
    
    int16_t voltage;
    adcInit(ADC_TIMER3_SAMPLING);
    
    // PR3 = 40 MHz / 15 kHz - 1 = 2665.5
    PR3 = 2755; // ça donne 15017 Hz mesuré au picoscope
    PR1 = 41800; // ça donne 994 Hz
    // starts timer1
    T1CONbits.TON = 1;
    // Enable timer1 interrupt, so that its ISR will be called on overflow
    _T1IE = 1;  // _T>1IE is a shortcut for IEC0bits.T1IE

	T3CONbits.TON = 1;
    
    
    /*
     int vars[13] = {0,0,0,0,0,1,1,0,0,1,0,1,1};
    for (int i = 0; i < 13; i++)
    {
        FrameFSM(vars[i] > 0);
        sendLine();
        
    }
    
    while (1)
    {}
    */
    
    
     /*while (1)
    {
         Move(20,0);
         Move(0,PI);
    }*/
      
    
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
}
// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="Interrupts">
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
        if (noise_counter > 15) {
            // probably fluke
            sample_count = 0;
            bit_count = 0;
            average_sample = 0;
            is_listening = 0; _LATB12 = 0;
            to_send = "Fluke"; sendLine();
            _LATB4 = 1;
            reset_tables();
            resetFSMtest();
            noise_counter = 0;
        }
    }
    
    if (sample_count == 99 || (bit_count == 0 && sample_count == 74)) {
        bit_count ++;
        sample_count = 0;
        //to_send = "Bit received"; sendLine();
        // bit has been received
        c = '0' + (average_sample > 0); sendChar();
        sendIntConverted(average_sample);
        
        _LATB14 = (average_sample > 0);
        _LATB5 = (average_sample < 0);
             
        
        // envoi Ã  la fsm
        FrameFSM(average_sample > 0);
        reset_tables();
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
            resetFSMtest();
            T1CONbits.TON = 1;
        }
    
    if (is_listening)
        sample_count ++;
}
// </editor-fold>

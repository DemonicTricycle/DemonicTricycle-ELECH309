// <editor-fold defaultstate="collapsed" desc="Include and imports">
#include "xc.h"
#include <math.h>

#ifdef TEST
#include <stdint.h>
#endif

#include "libpic30.h" // Contains __delay_ms definition
#include "tools.h"
#include "parameters.h"

// Adaptation of the formula in the datasheet
#define GET_PTPER() (int)(((float)FCY * PWM_TIME_PERIOD / CLOCK_PRESCALER) - 1)

// </editor-fold>

// TODO: use the FSM's .h direction/command enum ?
typedef enum
{
    forward,
    backward
} direction;


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
    POS1CNT = 0; //?
    POS2CNT = 0; //?
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
  SetMotor2Dir(dir); //! ???????
  SetMotor2Dir(dir);
}

void initialise(void)
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
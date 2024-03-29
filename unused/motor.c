#define DEBUG_PC 1

#include "xc.h"
#define FCY 3685000   // cycle frequency. Needed for __delay_ms
#include "libpic30.h" // Contains __delay_ms definition

#include "frameFSM.h"

#define NUM_TICKS_PER_TURN 1423
#define PI 3.1415926535897932
#define INT_MAX 65535
#define WHEEL_RADIUS 0.037 // in m
#define TRACK_WIDTH 0.134 // in m

// PWM parameters and macro
#define PWM_TIME_PERIOD 0.001 // In s, one period is one up AND one down,
// not just one 'hill' up
#define CLOCK_PRESCALER_BINARY 0b00 // In ms
#define CLOCK_PRESCALER 1           // 0b00 corresponds to a scale of 1
// Adaptation of the formula in the datasheet
#define GET_PTPER() (int)(((float)FCY * PWM_TIME_PERIOD / CLOCK_PRESCALER) - 1)
// TODO: check this acutally works with a picoscope

#define something QEI1CONbits.QEIM // TODO: ?
#define somethingalso QEI2CONbits.QEIM // TODO: ?
#define QE1_CHANNEL_A RPINR14bits.QEA1R
#define QE1_CHANNEL_B RPINR14bits.QEB1R
#define QE2_CHANNEL_A RPINR16bits.QEA2R
#define QE2_CHANNEL_B RPINR16bits.QEB2R
#define maybeMOTOR_1_PWM LATBbits.LATB2
#define maybeMOTOR_2_PWM LATBbits.LATB3
#define whatisthis TRISBbits.TRISB2 // set pin as output?
#define whatisthis2 TRISBbits.TRISB3
//TODO: name these defines

//#define delay __delay_ms //? maybe

void PWMInit(void)
{
    // If more details are required, check the following link :
    // https://ww1.microchip.com/downloads/en/DeviceDoc/70187E.pdf

    // TODO: set #defines to make this clearer
    PTCONbits.PTEN = 0; // Start by disabling PWM pulse generation

    PTCONbits.PTOPS = 0;                       // PWM time base output postscale
    PTCONbits.PTCKPS = CLOCK_PRESCALER_BINARY; // PWM time base input clock Prescale
    PTPER = GET_PTPER();                       // PWM Timebase period
    PWMCON1bits.PMOD1 = 1;                     // Sets to independent output mode
    PWMCON1bits.PMOD2 = 1;                     // Sets to independent output mode
    PWMCON1bits.PEN1L = 1;                     // RB15// Enables pin for PWM output
    PWMCON1bits.PEN2L = 1;                     // RB13
    PDC1 = 0;
    PDC2 = 0;
    PTCONbits.PTEN = 1; // Ends by enabling PWM pulse generation
}

void setPWM1(float param) // 1 = full power, 0 = turned off
{
    PDC1 = (int)((float)GET_PTPER() * param * 2);
}
void setPWM2(float param) // 1 = full power, 0 = turned off
{
    PDC2 = (int)((float)GET_PTPER() * param * 2);
}

void InitialiseEncoders(void)
{
    // TODO: set #defines to make this clearer
    QEI1CONbits.QEIM = 0b011; // Disables the module
    QEI2CONbits.QEIM = 0b011; // Disables the module

    RPINR14bits.QEA1R = 10; // Assigns channel A of QE1 to RP10(=RB10)
    RPINR14bits.QEB1R = 11; // Assigns channel B of QE1 to RP11(=RB11)
    RPINR16bits.QEA2R = 8;  // Assigns channel A of QE2 to RP8(=RB8)
    RPINR16bits.QEB2R = 9;  // Assigns channel B of QE2 to RP9(=RB9)

    QEI1CONbits.QEIM = 0b111;
    QEI2CONbits.QEIM = 0b111; // Enable the quadrature encoder interface
    // x4 mode with position counter reset by match
}

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

float GetPosMotor1(void)
{
    return ((float)Uint16ToInt(POS1CNT) * (2 * PI)) / 1423.0;
}
float GetPosMotor2(void)
{
    return ((float)Uint16ToInt(POS2CNT) * (2 * PI)) / 1423.0;
}
float GetDistMotor1(void)
{
    return GetPosMotor1() * WHEEL_RADIUS;
}
float GetDistMotor2(void)
{
    return GetPosMotor2() * WHEEL_RADIUS;
}
float Abs(float value)
{
    if (value < 0)
    {
        return -value;
    }
    return value;
}
void ResetPos(void)
{
    POS1CNT = 0;
    POS2CNT = 0;
}

int Sign(float value) //TODO: needed?
{
    if (value < 0)
    {
        return -1;
    }
    return 1;
}

void CapMotorVoltage(float *value)
{
    float max_value = 0.9;
    if (Abs(*value) > max_value)
    {
        *value = max_value * Sign(*value);
    }
}
void SetMotor1(float value)
{
    setPWM1(Abs(value));
    if (value < 0)
    {
        LATBbits.LATB2 = 1;
    }
    else
    {
        LATBbits.LATB2 = 0;
    }
}

void SetMotor2(float value)
{
    setPWM2(Abs(value));
    if (value < 0)
    {
        LATBbits.LATB3 = 1;
    }
    else
    {
        LATBbits.LATB3 = 0;
    }
}

// TODO: speed curve shape: triangle or trapezoid
float GetTarget(float time, float end) // time in s, end in m = final distance
{
    if (time < 1)
    {
#ifdef DEBUG_PC
        printf("Cas 1");
#endif
        return time * time * 0.25;
    } // Acceleration at the start

    float time_at_cruise = (end - 0.5) / 0.5;

#ifdef DEBUG_PC
    printf("Time at cruise : %f", time_at_cruise);
#endif

    if (time >= time_at_cruise + 1) // Slowing down
    {
        float time_remaining = time_at_cruise + 2 - time;
        float time_since_slowing = time - time_at_cruise - 1;

#ifdef DEBUG_PC
        printf("Time time_remaining : %f \n", time_remaining);
        printf("Time time_since_slowing : %f \n", time_since_slowing);
#endif

        if (time_remaining < 0)
        {
            return 0.5 * time_at_cruise + 0.5;
        }

#ifdef DEBUG_PC
        printf("Cas 2");
#endif

        return 0.5 * time_at_cruise + 0.25 + 0.5 * time_since_slowing - time_since_slowing * time_since_slowing * 0.25;
    }
    return (time - 1) * 0.5 + 0.25;
}

// TODO: Turn right etc (maybe an enum: RIGHT or LEFT, and degrees like: Turn(RIGHT, 90); or negative is left, positive is right like: Turn(-45); means left 45°)
void Move(float final_dist1, float final_dist2)
{
    // a_max = 0.5 m/s²
    // v_max = 0.5 m/s
    //-> 1 s to accelerate
    // 2 s minimum
    //-> 0.5 m minimum for now

    ResetPos();
    float acceptable_error = 0.002;
    float error1 = acceptable_error + 1;
    float error2 = acceptable_error + 1;
    float trans_k = 3.69;
    // TODO: use timers and GetTarget
    float time = 0;
    float target_1 = GetTarget(time, final_dist1);
    float target_2 = GetTarget(time, final_dist2);

    //? Abs(error) ?< acceptable_error
    while ((Abs(error1) > acceptable_error || Abs(error2) > acceptable_error) || (target_1 < final_dist1 || target_2 < final_dist2))
    {
        // if (IFS0bits.T1IF)
        // {
        //     IFS0bits.T1IF = 0;
        // TODO: timer
        time += 0.050;
        target_1 = GetTarget(time, final_dist1);
        target_2 = GetTarget(time, final_dist2);

        error1 = target_1 - GetDistMotor1();

        error2 = target_2 - GetDistMotor2();
        sendMessage("Erreurs : ");
        sendInt((int)(error1 * 1000));
        sendInt((int)(error2 * 1000));
        sendMessage("Finaldist - target ");
        sendInt((int)((final_dist1 - target_1) * 1000));
        sendMessage("Finaldist");
        sendInt((int)((final_dist1)*1000));
        sendMessage("Target");
        sendInt((int)((target_1)*1000));
        float v1 = error1 * trans_k;
        CapMotorVoltage(&v1);
        SetMotor1(v1);
        // sendInt((int)(1000*v1));
        float v2 = error2 * trans_k;
        CapMotorVoltage(&v2);
        // sendInt((int)(1000*v2));
        SetMotor2(v2);
        __delay_ms(50);
        // }
    }
    SetMotor1(0);
    SetMotor2(0);
}

void TurnDist(float degrees) //TODO: Turn()
{
    // track width = 134 mm
    // WHEEL_RADIUS = 0.037 m
    //? not 41 mm ?
    // wheel speed for rotation is set constant at 0.5 m/s ?
    // alpha = R/E * (PHI_r - PHI_l)

    // rot_circumference = TRACK_WIDTH * PI
    // traveled_arc_length = alpha/360 * rot_circumference

    ResetPos();
    int8_t acceptable_error = 2;
    int8_t error1 = acceptable_error + 1;
    int8_t error2 = acceptable_error + 1;
    float rot_k = 3.69; // TODO
    // TODO: use timers and GetTarget
    float time = 0;

    float target_1;
    float target_2;

    float rot_dist = (degrees/360) * (0.134*PI);

    if (degrees > 0)
    { // turn right
        //TODO
        target_1 = GetTarget(time, rot_dist);
        target_2 = -target_1;
    }
    else
    { // turn left
        //TODO
        target_2 = GetTarget(time, rot_dist);
        target_1 = -target_2;
    }

    //? Abs(error) ?< acceptable_error
    while ((Abs(error1) > acceptable_error || Abs(error2) > acceptable_error) || (target_1 < rot_dist || target_2 < rot_dist)) //TODO:
    {
        // if (IFS0bits.T1IF)
        // {
        //     IFS0bits.T1IF = 0;
        // TODO: timer
        time += 0.050;

        if (degrees > 0)
        { // turn right
            //TODO
            target_1 = GetTarget(time, rot_dist);
            target_2 = -target_1;

            error1 = target_1 - GetDistMotor1();
            error2 = target_2 - GetDistMotor2(); //? + ?
        }
        else
        { // turn left
            //TODO
            target_2 = GetTarget(time, rot_dist);
            target_1 = -target_2;

            error1 = target_1 - GetDistMotor1(); //? + ?
            error2 = target_2 - GetDistMotor2();
        }
        sendMessage("Erreurs : ");
        sendInt((int)(error1 * 1000));
        sendInt((int)(error2 * 1000));
        sendMessage("Finaldist - target ");
        sendInt((int)((rot_dist - target_1) * 1000));
        sendMessage("Finaldist");
        sendInt((int)((rot_dist)*1000));
        sendMessage("Target");
        sendInt((int)((target_1)*1000));
        float v1 = error1 * rot_k;
        CapMotorVoltage(&v1);
        SetMotor1(v1);
        // sendInt((int)(1000*v1));
        float v2 = error2 * rot_k;
        CapMotorVoltage(&v2);
        // sendInt((int)(1000*v2));
        SetMotor2(v2);
        __delay_ms(50);
        // }
        }
    SetMotor1(0);
    SetMotor2(0);
}

// <editor-fold defaultstate="collapsed" desc="Debug">
void sendChars(char *chars)
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
    char data[8];
    itoa(toSend, data, 10);
    sendChars(&data);
    breakLine();
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

    U1MODEbits.UARTEN = 1; // on active l'UART
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

void MotorOrder(movement order)
{
    //TODO: do with uint ?
    switch (order.cmd)
    {
    case FORWARD:
        float distance = (float) order.params;
        Move(distance);
        break;
    
    case BACKWARD:
        float distance = (float) order.params;
        Move(-distance);
        break;
    case TURN_RIGHT:
        float angle = (float) order.params;
        Turn(angle);
        break;
    case TURN_LEFT:
        float angle = (float) order.params;
        Turn(-angle); //TODO:
        break;
    }
}

int main(void)
{
    // PR1 = 36849;
    //  stars timer1
    // T1CONbits.TON = 1;

    TRISBbits.TRISB2 = 0; // dir 1
    TRISBbits.TRISB3 = 0;
    LATBbits.LATB2 = 0;
    LATBbits.LATB3 = 0;
    InitialiseEncoders();
    PWMInit();
    initUart();
    StartupMessage();
    __delay_ms(1000);
    // SetMotor1(1);
    // SetMotor2(0);

    while (1)
    {
        Move(1.5, 1.5);
        __delay_ms(2000);
        Move(0.5, 0.5);
        __delay_ms(2000);
        Move(-1, -1);
        __delay_ms(2000);
        // TODO: Turn(RIGHT, 90); or Turn(-45); ?
        Turn(90);
        __delay_ms(2000);
        Turn(-45);
        __delay_ms(2000);
    }

    return 0;
}

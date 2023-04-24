// <editor-fold defaultstate="collapsed" desc="Include and imports">
#include "xc.h"
#include <math.h>
#include <stdint.h>

// cycle frequency. Needed for __delay_ms
#ifndef FCY
#define FCY 3685000
#endif

#include "libpic30.h" // Contains __delay_ms definition
#include "tools.h"
#include "parameters.h"

#ifndef NUM_TICKS_PER_TURN
#define NUM_TICKS_PER_TURN 1423
#endif

#ifndef PI
#define PI 3.1415926535897932
#endif

#define INT_MAX 65535 // rename UINT16_MAX ?
// Wheel radius, in m
#ifndef WHEEL_RADIUS
#define WHEEL_RADIUS 0.04
#endif

// PWM parameters and macro
#ifndef PWM_TIME_PERIOD
#define PWM_TIME_PERIOD 0.001
#endif
// In s, one period is one up AND one down,
// not just one 'hill' up
#ifndef CLOCK_PRESCALER_BINARY
#define CLOCK_PRESCALER_BINARY 0b00
#endif
// In ms
#ifndef CLOCK_PRESCALER
#define CLOCK_PRESCALER 1
#endif
// 0b00 corresponds to a scale of 1

// Adaptation of the formula in the datasheet
#define GET_PTPER() (int)(((float)FCY * PWM_TIME_PERIOD / CLOCK_PRESCALER) - 1)

// Wheelbase, in m
#ifndef WHEELBASE
#define WHEELBASE 0.135
#endif

#ifndef MAX_PWM
#define MAX_PWM 0.8
#endif

#define something QEI1CONbits.QEIM     // TODO: ? encoder mode ?
#define somethingalso QEI2CONbits.QEIM // TODO: ?
#define PWM_GEN_ENABLE PTCONbits.PTEN
#define PWM_TIME_BASE_OUTPUT_POSTSCALE PTCONbits.PTOPS
#define PWM_TIME_BASE_INPUT_CLOCK_PRESCALE PTCONbits.PTCKPS
#define PWM_OUTPUT_1_MODE PWMCON1bits.PMOD1
#define PWM_OUTPUT_2_MODE PWMCON1bits.PMOD2
#define PWM_OUTPUT_1_PIN_ENABLE PWMCON1bits.PEN1L
#define PWM_OUTPUT_2_PIN_ENABLE PWMCON1bits.PEN2L
#define MOTOR_1_FORWARD MOTOR_1_DIR_PIN
#define MOTOR_2_FORWARD MOTOR_2_DIR_PIN
#define TIMER_1_ENABLE T1CONbits.TON
#define TIMER_1_INTERRUPT_FLAG IFS0bits.T1IF //? or just _T1IF
//#define TIMER_1_INTERRUPT_ENABLE IEC0bits.T1IE

//TODO: put in a .h ?
typedef enum
{
    forward,
    backward
} direction;

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="PWM">
void PWMInit(void)
{
    // If more details are required, check the following link :
    // https://ww1.microchip.com/downloads/en/DeviceDoc/70187E.pdf

    //PTCONbits.PTEN = 0;                        // Start by disabling PWM pulse generation
    PWM_GEN_ENABLE = 0;                        // Start by disabling PWM pulse generation

    //PTCONbits.PTOPS = 0;                       // PWM time base output postscale
    PWM_TIME_BASE_OUTPUT_POSTSCALE = 0;        // PWM time base output postscale
    //PTCONbits.PTCKPS = CLOCK_PRESCALER_BINARY; // PWM time base input clock Prescale
    PWM_TIME_BASE_INPUT_CLOCK_PRESCALE = CLOCK_PRESCALER_BINARY; // PWM time base input clock Prescale
    PTPER = GET_PTPER();                       // PWM Timebase period
    //PWMCON1bits.PMOD1 = 1;                     // Sets to independent output mode
    PWM_OUTPUT_1_MODE = 1;                     // Sets to independent output mode
    //PWMCON1bits.PMOD2 = 1;                     // Sets to independent output mode
    PWM_OUTPUT_2_MODE = 1;                     // Sets to independent output mode
    //PWMCON1bits.PEN1L = 1;                     // RB15 // Enables pin for PWM output
    PWM_OUTPUT_1_PIN_ENABLE = 1;               // RB15 // Enables pin for PWM output
    //PWMCON1bits.PEN2L = 1;                     // RB13
    PWM_OUTPUT_2_PIN_ENABLE = 1;               // RB13 // Enables pin for PWM output
    PDC1 = 0;
    PDC2 = 0;

    //PTCONbits.PTEN = 1;                        // Ends by enabling PWM pulse generation
    PWM_GEN_ENABLE = 1;                        // Ends by enabling PWM pulse generation
}

void setPWM1(float param) // 1 = full power, 0 = turned off
{
    PDC1 = (int)((float)GET_PTPER() * param * 2);
}
void setPWM2(float param) // 1 = full power, 0 = turned off
{
    PDC2 = (int)((float)GET_PTPER() * param * 2);
}

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Encoders">
void InitialiseEncoders(void)
{
    // https://ww1.microchip.com/downloads/en/DeviceDoc/70187E.pdf

    //TODO: use defines for values
    QEI1CONbits.QEIM = 0b011; // Disables the module
    QEI2CONbits.QEIM = 0b011; // Disables the module

    RPINR14bits.QEA1R = QE1_CHANNEL_A_PIN; // Assigns channel A of QE1
    RPINR14bits.QEB1R = QE1_CHANNEL_B_PIN; // Assigns channel B of QE1
    RPINR16bits.QEA2R = QE2_CHANNEL_A_PIN;  // Assigns channel A of QE2
    RPINR16bits.QEB2R = QE2_CHANNEL_B_PIN;  // Assigns channel B of QE2

    QEI1CONbits.QEIM = 0b111;
    QEI2CONbits.QEIM = 0b111; // Enable the quadrature encoder interface
    // x4 mode with position counter reset by match
}

/* //defined in tools.c
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
*/

void ResetPos(void)
{
    POS1CNT = 0;
    POS2CNT = 0;
}

float GetPosMotor1(void) // Get the position in radian
{
    return ((float)Uint16ToInt(POS1CNT) * (2 * PI)) / 1423.0;
}
float GetPosMotor2(void)
{
    return ((float)Uint16ToInt(POS2CNT) * (2 * PI)) / 1423.0;
}
float GetDistMotor1(void) // Get the distance in m
{
    return GetPosMotor1() * WHEEL_RADIUS;
}
float GetDistMotor2(void)
{
    return GetPosMotor2() * WHEEL_RADIUS;
}
// </editor-fold>

/* //defined in tools.c
// <editor-fold defaultstate="collapsed" desc="Math function">
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
// </editor-fold>
*/

// <editor-fold defaultstate="collapsed" desc="Movement">

void CapMotorVoltage(float *value)
{
    if (FloatAbs(*value) > MAX_PWM)
    {
        *value = MAX_PWM * FloatSign(*value);
    }
}

void SetMotor1Dir(direction dir)
{
    if (dir == forward)
    {
        MOTOR_1_FORWARD = 1;
    }
    if (dir == backward)
    {
        MOTOR_1_FORWARD = 0;
    }
}

void SetMotor2Dir(direction dir)
{
    if (dir == forward)
    {
        MOTOR_2_FORWARD = 1;
    }
    if (dir == backward)
    {
        MOTOR_2_FORWARD = 0;
    }
}

void SetMotor1(float value)
{
    setPWM1(FloatAbs(value));
    direction dir = forward;
    if (value < 0)
    {
        dir = backward;
    }
    SetMotor1Dir(dir);
}

void SetMotor2(float value)
{
    setPWM2(FloatAbs(value));
    direction dir = forward;
    if (value < 0)
    {
        dir = backward;
    }
    SetMotor2Dir(dir);
}

float GetTarget(float time, float final_target) // time in s, end in m = final distance
{
    int sign = FloatSign(final_target);
    float end = FloatAbs(final_target);
    //float acceleration = 0.5; //! set in a #define ACCELERATION 0.5 in parameters.h
    if (end < 0.5) //TODO: set in a #define, ?
    {
        float half_time = sqrt(end / 0.5);
        if (time > half_time * 2)
        {
            return end * sign;
        }
        if (time < half_time)
        {
            return (time * time * ACCELERATION / 2) * sign;
        }
        else
        {
            //TODO: set as #defines, ?
            float distAldreadyMade = half_time * half_time * ACCELERATION / 2;
            float time_since_slowing = time - half_time;
            return (distAldreadyMade + 0.5 * half_time * time_since_slowing - time_since_slowing * time_since_slowing * 0.25) * sign;
        }
    }
    if (time < 1)
    {
        return (time * time * 0.25) * sign;
    }

    float time_at_cruise = (end - 0.5) / 0.5;

    if (time >= time_at_cruise + 1) // Slowing down
    {
        float time_remaining = time_at_cruise + 2 - time;
        float time_since_slowing = time - time_at_cruise - 1;

        if (time_remaining < 0)
        {
            return (0.5 * time_at_cruise + 0.5) * sign;
        }

        return (0.5 * time_at_cruise + 0.25 + 0.5 * time_since_slowing - time_since_slowing * time_since_slowing * 0.25) * sign;
    }
    return ((time - 1) * 0.5 + 0.25) * sign;
}

float getTargetAngle(float given_angle, float time_since_start)
{                                 // Not very optimised but at least somewhat readable
//#define CRUISE_ROTATION_SPEED 4   // rad / second //set in parameters.h
//#define MAX_ACCELERATION_TIME_ROTATION 1.2 // seconds // set in parameters.h

    int sign = FloatSign(given_angle);
    float target_angle = FloatAbs(given_angle);
    // Calculate the maximum angle that can be reached before reaching the cruise speed
    //float MAX_ANGLE = (float)CRUISE_ROTATION_SPEED * 0.5 * (MAX_ACCELERATION_TIME_ROTATION * 2);

    float angle_to_return = 0;

    if (target_angle > MAX_ANGLE)
    {
        return 0; // Such an angle is not sendable in 8 bits
    }
    //float acceleration_rotation = (float)CRUISE_ROTATION_SPEED / MAX_ACCELERATION_TIME_ROTATION; // The acceleration rate //set in parameters.h
    float half_time = sqrt(target_angle / ACCELERATION_ROTATION);                       // The time at wich we start decelerating

    if (time_since_start < half_time)
    {
        // Simple acceleration
        angle_to_return = time_since_start * time_since_start * ACCELERATION_ROTATION / 2;
    }
    else
    {
        float time_since_slowing = time_since_start - half_time;
        // Distance aldready made when accelerating
        angle_to_return = half_time * half_time * ACCELERATION_ROTATION / 2;
        // Distance since deceleration
        angle_to_return += ACCELERATION_ROTATION * (half_time * time_since_slowing - time_since_slowing * time_since_slowing / 2);
    }
    if (time_since_start >= half_time * 2)
    {
        angle_to_return = target_angle;
    }

    return angle_to_return * sign;
}

float getAngle()
{
    return (GetDistMotor2() - GetDistMotor1()) / WHEELBASE;
}

void Move(float translation, float angle)
{
    ResetPos(); // Sets the encoders to 0
//#define acceptable_error_translation 0.04
//#define acceptable_error_rotation 0.06981 //(float)4 / 360 * 2 * PI
    //float error_translation = acceptable_error_translation + 1.0;
    //float error_rotation = acceptable_error_rotation + 1.0;
    float error_translation = acceptable_error_translation+1.0;
    float error_rotation = acceptable_error_rotation+1.0;

//#define k_translation 3.69
//#define k_rotation 1.03 * 3

    float time = 0;
    float target_translation = GetTarget(time, translation);
    float target_rotation = getTargetAngle(angle, time);
    int time_interval = 3;                  // ms
    PR1 = (FCY / 1000 - 1) * time_interval; // 10 ms
    //T1CONbits.TON = 1;
    TIMER_1_ENABLE = 1;
    // int cnt = 0;
    while (
        (FloatAbs(error_translation) > acceptable_error_translation) || (FloatAbs(error_rotation) > acceptable_error_rotation) || (FloatAbs(target_translation) < FloatAbs(translation)) || (FloatAbs(target_rotation) < FloatAbs(angle)))
    {
        //if (_T1IF == 1)
        (if TIMER_1_INTERRUPT_FLAG == 1)
        {
            //_T1IF = 0;
            TIMER_1_INTERRUPT_FLAG = 0;

            /*  cnt++;
             if (cnt >= 10)
             {
                  sendUartMessage("Position V2 : ");
                  cnt = 0;
                    int a = (int)(GetPosMotor1()*1000);
             sendUartInt(a);
             a = (int)(GetPosMotor2()*1000);
                  sendUartMessage("Position en cm : ");
                  cnt = 0;
                    a = (int)(GetDistMotor1()*100);
             sendUartInt(a);
             a = (int)(GetDistMotor2()*100);
         sendUartInt(a);
                  sendUartMessage("Target pos en cm : ");

             a = (int)(target_transla*100);
         sendUartInt(a);
             }*/
            time += (float)time_interval / 1000;

            target_translation = GetTarget(time, translation);

            error_translation = target_translation - (GetDistMotor1() + GetDistMotor2()) / 2;
            target_rotation = getTargetAngle(angle, time);
            error_rotation = getAngle() - getTargetAngle(angle, time);
            float v1 = error_translation * k_translation + error_rotation * k_rotation;
            CapMotorVoltage(&v1);
            SetMotor1(v1);
            float v2 = error_translation * k_translation - error_rotation * k_rotation;
            CapMotorVoltage(&v2);
            SetMotor2(v2);
        }
    }
    SetMotor1(0);
    SetMotor2(0);
}
// </editor-fold>

void initialiseMotors(void)
{
    direction dir = forward;
    M1_DIR_PIN_AS_INPUT = 0;
    M2_DIR_PIN_AS_INPUT = 0; // Declares dir pins as output
    SetMotor1Dir(dir);
    SetMotor2Dir(dir);
}
void initialise(void)
{
    initialiseMotors();
    InitialiseEncoders();
    PWMInit();
    initUart(); // TODO: only enable UART if debugging ?
    StartupMessage();
    __delay_ms(1000);
}

int main(void)
{
    initialise();

#ifdef DEBUG
    while (1)
    {
        Move(1, 0);
        __delay_ms(1000);
        Move(0, -PI);
        __delay_ms(1000);
    }
#endif

    return 0;
}

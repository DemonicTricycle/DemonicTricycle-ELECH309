// <editor-fold defaultstate="collapsed" desc="Include and imports">
#include "xc.h"
//#include "libpic30.h" // Contains __delay_ms definition
#include <math.h>

#ifdef TEST
#include <stdint.h>
//#include <stdio.h>
#endif

//#include "libpic30.h" // Contains __delay_ms definition
#include "tools.h"
#include "parameters.h"
//#include "libpic30.h" // Contains __delay_ms definition

// Adaptation of the formula in the datasheet
#define GET_PTPER() (int)(((float)FCY * PWM_TIME_PERIOD / CLOCK_PRESCALER) - 1)

#define something QEI1CONbits.QEIM     // TODO: ? encoder mode ?
#define somethingalso QEI2CONbits.QEIM // TODO: ?
#define PWM_GEN_ENABLE PTCONbits.PTEN
#define PWM_TIME_BASE_OUTPUT_POSTSCALE PTCONbits.PTOPS
#define PWM_TIME_BASE_INPUT_CLOCK_PRESCALE PTCONbits.PTCKPS
#define PWM_OUTPUT_1_MODE PWMCON1bits.PMOD1
#define PWM_OUTPUT_2_MODE PWMCON1bits.PMOD2
#define PWM_OUTPUT_1_PIN_ENABLE PWMCON1bits.PEN1L
#define PWM_OUTPUT_2_PIN_ENABLE PWMCON1bits.PEN2L
#define TIMER_1_ENABLE T1CONbits.TON
#define TIMER_1_INTERRUPT_FLAG IFS0bits.T1IF //? or just _T1IF
//#define TIMER_1_INTERRUPT_ENABLE IEC0bits.T1IE

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
    PDC1 = 0; //?
    PDC2 = 0; //?

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
    RPINR16bits.QEA2R = QE2_CHANNEL_A_PIN; // Assigns channel A of QE2
    RPINR16bits.QEB2R = QE2_CHANNEL_B_PIN; // Assigns channel B of QE2

    QEI1CONbits.QEIM = 0b111; // Enable the quadrature encoder interface 1
    QEI2CONbits.QEIM = 0b111; // Enable the quadrature encoder interface 2
    // x4 mode with position counter reset by match
}

void ResetPos(void)
{
    POS1CNT = 0; //?
    POS2CNT = 0; //?
}

float GetPosMotor1(void) // Get the position in radian
{
    return ((float)Uint16ToInt(POS1CNT) * (2 * PI)) / 1423.0; // TODO: use defines for values
}
float GetPosMotor2(void)
{
    return ((float)Uint16ToInt(POS2CNT) * (2 * PI)) / 1423.0; // TODO: use defines for values
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

// <editor-fold defaultstate="collapsed" desc="Movement">
void CapMotorVoltage(float *value)
{
    if (FloatAbs(*value) > MAX_PWM)
    {
        *value = MAX_PWM * FloatSign(*value);
    }
}

void SetMotor1Dir(direction dir)
//void SetMotor1Dir(command dir)
{
    if (dir == forward)
    //if (dir == FORWARD)
    {
        MOTOR_1_DIR_PIN = 1; // set forward
    }
    if (dir == backward)
    //if (dir == BACKWARD)
    {
        MOTOR_1_DIR_PIN = 0; // set not forward
    }
}

void SetMotor2Dir(direction dir)
//void SetMotor2Dir(command dir)
{
    if (dir == forward)
    //if (dir == FORWARD)
    {
        MOTOR_2_DIR_PIN = 1; // set forward
    }
    if (dir == backward)
    //if (dir == BACKWARD)
    {
        MOTOR_2_DIR_PIN = 0; // set not forward
    }
}

void SetMotor1(float value)
{
    setPWM1(FloatAbs(value));
    direction dir = forward;
    //command dir = FORWARD;
    if (value < 0)
    {
        dir = backward;
        //dir = BACKWARD;
    }
    SetMotor1Dir(dir);
}

void SetMotor2(float value)
{
    setPWM2(FloatAbs(value));
    direction dir = forward;
    //command dir = FORWARD;
    if (value < 0)
    {
        dir = backward;
        //dir = BACKWARD;
    }
    SetMotor2Dir(dir);
}

float GetTarget(float time, float final_target) // time in s, end in m = final distance
{
    int sign = FloatSign(final_target);
    float end = FloatAbs(final_target);

    if (end < 0.5) //TODO: set in a #define ?
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
            float distAldreadyMade = half_time * half_time * ACCELERATION / 2;
            float time_since_slowing = time - half_time;
            // TODO: set as #defines ?
            return (distAldreadyMade + 0.5 * half_time * time_since_slowing - time_since_slowing * time_since_slowing * 0.25) * sign;
        }
    }
    if (time < 1)
    {
        // TODO: set as #defines ?
        return (time * time * 0.25) * sign;
    }

    // TODO: set as #defines ?
    float time_at_cruise = (end - 0.5) / 0.5;

    if (time >= time_at_cruise + 1) // Slowing down
    {
        float time_remaining = time_at_cruise + 2 - time;
        float time_since_slowing = time - time_at_cruise - 1;

        if (time_remaining < 0)
        {
            // TODO: set as #defines ?
            return (0.5 * time_at_cruise + 0.5) * sign;
        }

        // TODO: set as #defines ?
        return (0.5 * time_at_cruise + 0.25 + 0.5 * time_since_slowing - time_since_slowing * time_since_slowing * 0.25) * sign;
    }
    // TODO: set as #defines ?
    return ((time - 1) * 0.5 + 0.25) * sign;
}

float getTargetAngle(float given_angle, float time_since_start)
{// Not very optimised but at least somewhat readable

    int sign = FloatSign(given_angle);
    float target_angle = FloatAbs(given_angle);
    // Calculate the maximum angle that can be reached before reaching the cruise speed

    float angle_to_return = 0;

    if (target_angle > MAX_ANGLE)
    {
        return 0; // Such an angle is not sendable in 8 bits
    }

    float half_time = sqrt(target_angle / ACCELERATION_ROTATION); // The time at wich we start decelerating

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
    T1CONbits.TON = 0; // Disable Timer1 //? TODO: check if needed
    ResetPos(); // Sets the encoders to 0
//#define acceptable_error_translation 0.04
//#define acceptable_error_rotation 0.06981 //(float)4 / 360 * 2 * PI
    //float error_translation = acceptable_error_translation + 1.0;
    //float error_rotation = acceptable_error_rotation + 1.0;
    float error_translation = ACCEPTABLE_ERROR_TRANSLATION + 1.0;
    float error_rotation = ACCEPTABLE_ERROR_ROTATION + 1.0;

    float time = 0;
    float target_translation = GetTarget(time, translation);
    float target_rotation = getTargetAngle(angle, time);
    int time_interval = 1; // ms 3? //TODO: put as #define in parameters.h
    //PR2 = (FCY / 1000 - 1) * time_interval; // 10 ms //TODO: check ?
    PR2 = 41800; //(FCY/1000 -1) * time_interval;//10 ms 41800
    T2CONbits.TON = 1;
    //TIMER_1_ENABLE = 1; //TODO: change to timer 2
    // int cnt = 0;
    while (
        (FloatAbs(error_translation) > ACCEPTABLE_ERROR_TRANSLATION) || (FloatAbs(error_rotation) > ACCEPTABLE_ERROR_ROTATION) || (FloatAbs(target_translation) < FloatAbs(translation)) || (FloatAbs(target_rotation) < FloatAbs(angle)))
    {
        //if (_T1IF == 1)
        //(if TIMER_1_INTERRUPT_FLAG == 1)
        if (_T2IF == 1) //TODO: write a define for this
        {
            //_T1IF = 0;
            //TIMER_1_INTERRUPT_FLAG = 0;
            _T2IF = 0; //TODO: write a define for this

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
            float v1 = error_translation * K_TRANSLATION + error_rotation * K_ROTATION;
            CapMotorVoltage(&v1);
            SetMotor1(v1);
            float v2 = error_translation * K_TRANSLATION - error_rotation * K_ROTATION;
            CapMotorVoltage(&v2);
            SetMotor2(v2);
        }
    }
    SetMotor1(0);
    SetMotor2(0);
    T2CONbits.TON = 0; //TODO: write a define for this
}

// </editor-fold>

void initialiseMotors(void)
{
    direction dir = forward; //TODO: use FSM's enum ?
    //command dir = FORWARD;
    M1_DIR_PIN_AS_INPUT = 0; // Declares dir pin 1 as output
    M2_DIR_PIN_AS_INPUT = 0; // Declares dir pin 2 as output
    SetMotor1Dir(dir);
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

/*
int main(void)
{
    initialise();

    while (1)
    {
        Move(1, 0);
        __delay_ms(1000);
        Move(0, -PI);
        __delay_ms(1000);
    }

    return 0;
}
*/

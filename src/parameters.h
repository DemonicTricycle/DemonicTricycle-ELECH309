// parameters defines

//?  correct ?
#define SYS_FREQ 80000000L
#define FCY SYS_FREQ/2

// for filter.c :
// <editor-fold defaultstate="collapsed" desc="motors.c parameters">

#define FLOORS 4
// const short FLOOR_ORDER = 2;

#define THRESHOLD 300

// filter constants
#define M_FILTER 18 // Q18.14
#define FACTOR_FILTER 262144 // 1 << M
//#define FLOAT_FACTOR_FILTER (float) (factor) // doesn't work without parentheses because of <<'s priority, lost 10min just on this
#define FS_FILTER 15000 //? used ?

// </editor-fold>

// for motors.c :
// <editor-fold defaultstate="collapsed" desc="motors.c parameters">

#ifndef WHEELBASE
#define WHEELBASE 0.135
// in m
#endif

#ifndef WHEEL_RADIUS
#define WHEEL_RADIUS 0.04
// in m
#endif

#ifndef NUM_TICKS_PER_TURN
#define NUM_TICKS_PER_TURN 1423
// more or less
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

// Max PWM value for the motors
#define MAX_PWM 0.8

#define QE1_CHANNEL_A_PIN 10 // Assigns channel A of QE1 to RP10(=RB10)
#define QE1_CHANNEL_B_PIN 11 // Assigns channel B of QE1 to RP11(=RB11)
#define QE2_CHANNEL_A_PIN 8  // Assigns channel A of QE2 to RP8(=RB8)
#define QE2_CHANNEL_B_PIN 9  // Assigns channel B of QE2 to RP9(=RB9)
#define CRUISE_ROTATION_SPEED 4.0   // rad / second
#define MAX_ACCELERATION_TIME_ROTATION 1.2 // seconds
#define MAX_ANGLE (CRUISE_ROTATION_SPEED*0.5*(MAX_ACCELERATION_TIME_ROTATION*2))
#define ACCELERATION_ROTATION (CRUISE_ROTATION_SPEED/MAX_ACCELERATION_TIME_ROTATION)
// TODO: name these defines in all caps
#define acceptable_error_translation 0.04
#define acceptable_error_rotation 0.06981 // = (float)4 / 360 * 2 * PI
//#define error_translation acceptable_error_transla+1.0
//#define error_rotation acceptable_error_rota+1.0
#define k_translation 3.69
#define k_rotation (1.03*3)
//#define time_interval 3 // ms
#define M1_DIR_PIN_AS_INPUT TRISBbits.TRISB2 // set to 0 to output
#define M2_DIR_PIN_AS_INPUT TRISBbits.TRISB3 // set to 0 to output

#define MOTOR_1_DIR_PIN LATBbits.LATB2
#define MOTOR_2_DIR_PIN LATBbits.LATB3
//#define maybeMOTOR_1_REVERSE LATBbits.LATB2
//#define maybeMOTOR_2_REVERSE LATBbits.LATB3

#define ACCELERATION 0.5 // seconds //?

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="FSM parameters">
#define FRAME_LENGTH 13 // Frame length in bits
#define FRAME_DATA_LENGTH 10 // Frame data length in bits
//#define FRAME_PARAMS_LENGTH FRAME_DATA_LENGTH-2 // Frame parameters length in bits
// </editor-fold>

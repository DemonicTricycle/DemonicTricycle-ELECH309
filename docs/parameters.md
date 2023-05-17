---
layout: default
title: Parameters
nav_order: 8
---

# Robot Parameters
In order to easily change some parameters of the robot's code, we placed them in a common file: [parameters.h](https://github.com/DemonicTricycle/DemonicTricycle-ELECH309/blob/main/src/parameters.h).  
This allows us to test different values for certain functions quickly between compilations.

## [Code](https://github.com/DemonicTricycle/DemonicTricycle-ELECH309/blob/main/src/parameters.h)

<details open markdown="block">
  <summary>
    parameters.h
  </summary>

```c
// parameters defines

#define SYS_FREQ 80000000L
#define FCY (SYS_FREQ/2)

// for filter.c :
// <editor-fold defaultstate="collapsed" desc="filter.c parameters">

#define FLOORS 4

#define THRESHOLD 230

// filter constants
#define M_FILTER 18 // Q18.14
#define FACTOR_FILTER 262144 // 1 << M
#define FS_FILTER 15000

// </editor-fold>

// for motors.c :
// <editor-fold defaultstate="collapsed" desc="motors.c parameters">

#ifndef WHEELBASE
#define WHEELBASE 0.135 // in m
#endif

#ifndef WHEEL_RADIUS
#define WHEEL_RADIUS 0.04 // in m
#endif

#ifndef NUM_TICKS_PER_TURN
#define NUM_TICKS_PER_TURN 1430 // more or less
#endif

// PWM parameters and macro
#ifndef PWM_TIME_PERIOD
#define PWM_TIME_PERIOD 0.001 // In s, one period is one up AND one down, not just one 'hill' up
#endif
  
#ifndef CLOCK_PRESCALER_BINARY
#define CLOCK_PRESCALER_BINARY 0b00 // In ms
#endif

#ifndef CLOCK_PRESCALER
#define CLOCK_PRESCALER 1 // 0b00 corresponds to a scale of 1
#endif

#define MAX_PWM 0.8 // Max PWM value for the motors

#define QE1_CHANNEL_A_PIN 10 // Assigns channel A of QE1 to RP10(=RB10)
#define QE1_CHANNEL_B_PIN 11 // Assigns channel B of QE1 to RP11(=RB11)
#define QE2_CHANNEL_A_PIN 8  // Assigns channel A of QE2 to RP8(=RB8)
#define QE2_CHANNEL_B_PIN 9  // Assigns channel B of QE2 to RP9(=RB9)
#define CRUISE_ROTATION_SPEED 3.5   // rad / second
#define MAX_ACCELERATION_TIME_ROTATION 1.5 // seconds
#define MAX_ANGLE (CRUISE_ROTATION_SPEED*0.5*(MAX_ACCELERATION_TIME_ROTATION*2))
#define ACCELERATION_ROTATION (CRUISE_ROTATION_SPEED/MAX_ACCELERATION_TIME_ROTATION)
#define ACCEPTABLE_ERROR_TRANSLATION 0.04 // 4cm
#define ACCEPTABLE_ERROR_ROTATION 0.06981 // = (float)4 / 360 * 2 * PI

#define K_TRANSLATION 3.69
#define K_ROTATION (1.03 * 3)
//#define time_interval 3 // ms
  
#define M1_DIR_PIN_AS_INPUT TRISBbits.TRISB2 // set to 0 to output
#define M2_DIR_PIN_AS_INPUT TRISBbits.TRISB3 // set to 0 to output
#define MOTOR_1_DIR_PIN LATBbits.LATB2 // pin used for motor1's direction
#define MOTOR_2_DIR_PIN LATBbits.LATB3 // pin used for motor2's direction

#define ACCELERATION 0.5 // seconds

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="FSM parameters">
#define FRAME_LENGTH 13 // Frame length in number of bits
#define FRAME_DATA_LENGTH 10 // Frame data length in number of bits
//#define FRAME_PARAMS_LENGTH FRAME_DATA_LENGTH-2 // Frame parameters length in number of bits
// </editor-fold>

```

</details>

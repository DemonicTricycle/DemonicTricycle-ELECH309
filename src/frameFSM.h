
//#ifndef FRAMEFSM_H_ /* Include guard */
//#define FRAMEFSM_H_

#ifdef TEST
#include <stdint.h>
#endif

typedef enum
{ /// Commands:
    FORWARD,
    BACKWARD,
    TURN_RIGHT,
    TURN_LEFT
} command;

/*
// State Machine states:
typedef enum
{ /// States:
    IDLE,
    START,
    DATA,
    PARITY,
    STOP
} state;

typedef enum
{ /// Signals:
    NO_SIGNAL,
    NOISE,
    BIT_0,
    BIT_1
} signal;
*/

typedef struct
{ /// Movement:
    command cmd;
    uint8_t params;
} movement;

// Movement instance
extern movement order;
//? used externally ?

// Frame State Machine function
int FrameFSM(int received_bit);
void resetFSM(void);

// Debug/test access to functions and variables
#ifdef TEST
int IdleHandler(signal signal_state); //? needed?
int StartHandler(signal signal_state);
int DataHandler(signal signal_state);
int ParityHandler(signal signal_state);
int StopHandler(signal signal_state);
extern state current_state;
extern uint8_t bit_count;
extern int8_t ones;
extern int8_t zeroes;
extern uint8_t bit_params;
extern uint8_t cmd_bits;
#endif

//#endif // FRAMEFSM_H_

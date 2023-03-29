/* 
 TODO
*/

//#ifndef FRAMEFSM_H_ /* Include guard */
//#define FRAMEFSM_H_

#include <stdint.h>

// TODO: use ?
#define FRAME_LENGTH 13 // Frame length in bits
#define FRAME_DATA_LENGTH 10 // Frame data length in bits
#define FRAME_PARAMS_LENGTH FRAME_DATA_LENGTH-2 // Frame parameters length in bits

typedef enum
{ /// Commands:
    FORWARD,
    BACKWARD,
    TURN_RIGHT,
    TURN_LEFT
} command;

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

typedef struct
{ /// Movement:
    command cmd;
    uint16_t params;
} movement;

// Movement instance
extern movement order;

// Frame State Machine function
int FrameFSM(uint8_t low, uint8_t high);

void resetFSM(void);

//#endif // FRAMEFSM_H_

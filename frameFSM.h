/* 
 TODO
*/

//#ifndef FRAMEFSM_H_ /* Include guard */
//#define FRAMEFSM_H_

#include <stdint.h>

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

typedef struct
{ /// Movement:
    command cmd;
    uint16_t params;
} movement;

// Movement instance
extern movement order;

// Frame State Machine function
int FrameFSM(uint16_t frame);

//#endif // FRAMEFSM_H_

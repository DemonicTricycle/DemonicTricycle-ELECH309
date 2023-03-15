/* 
 TODO
*/

//#ifndef FRAMEFSM_H_ /* Include guard */
//#define FRAMEFSM_H_

#include <stdint.h>

typedef enum
{
    FORWARD,
    BACKWARD,
    TURN_RIGHT,
    TURN_LEFT
} command;

// State Machine states:
typedef enum
{
    IDLE,
    START,
    DATA,
    PARITY,
    STOP
} state;

typedef struct
{
    command cmd;
    uint16_t params;
} movement;

extern movement order;

int FrameFSM(uint16_t frame);

//#endif // FRAMEFSM_H_

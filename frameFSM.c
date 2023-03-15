// Frame FSM (Finite State Machine)

// ------------------------
/*
 * frame
 *
 * Directions commands and data for the robot is sent with frames via FM sound.
 *
 * Command frame format:
 * [preamble]
 *    |
 *    | [data]
 *    |   |
 *    |   |     [postamble]
 *    |   |          |
 *    | __+________ _+_
 * => 0 bb bbbbbbbb b 1
 *    ^ ^     ^     ^ ^
 *    | |     |     | |
 *    | |   params  | |
 *    | |           | |
 *    | command     | |
 *    |             | |
 *    |        parity |
 *    |               |
 * start bit (0)      |
 *                    |
 *               stop bit (1)
 *
 * data in frame:
 * forward:
 *  command: 0b00
 *  param: unsigned distance in cm
 *
 * backward:
 *  command: 0b01
 *  param: unsigned distance in cm
 *
 * turn right:
 *  command: 0b10
 *  param: unsigned angle in degrees
 *
 * turn left:
 *  command: 0b11
 *  param: unsigned angle in degrees
 *
 */
// ------------------------

#include "frameFSM.h"
#include <stdint.h>


/*
 ! defined in header file
// State Machine states:
typedef enum
{
    IDLE,
    START,
    DATA,
    PARITY,
    STOP
} state;

typedef enum
{
    FORWARD,
    BACKWARD,
    TURN_RIGHT,
    TURN_LEFT
} command;
*/

//command cmd;
//uint16_t params;
////int success = 0; // motors should do nothing if success == 0

movement order;

int IdleHandler(uint16_t frame)
//? needed ?
{
    // TODO
    #ifdef DEBUG
        printf("IdleHandler\n");
    #endif
        // if (frame == 0b0000000000000) // more efficient than the following ?
        if (frame >> 12 == 0b0)
    { // need to test
        #ifdef DEBUG
                printf("IdleHandler success\n");
        #endif
        return 0;
    }
    else
    {
        return 1;
    }
}

int StartHandler(uint16_t frame)
{
    // TODO
    #ifdef DEBUG
        printf("StartHandler\n");
    #endif
    // if (!frame & 0b1000000000000) // more efficient than the following ?
    if (frame >> 12 == 0b0)
    { // need to test
        #ifdef DEBUG
            printf("StartHandler success\n");
        #endif
        return 0;
    }
    else
    {
        return 1;
    }
}

int DataHandler(uint16_t frame)
{
    // TODO
    #ifdef DEBUG
        printf("DataHandler\n");
    #endif
    // -- cmd --
    // more efficient way than the following ? ike just comparing the whole 13bit frame to the 4 possible commands
    switch (frame >> 10)
    {
    case 0b00:
        // TODO
        #ifdef DEBUG
            printf("FORWARD\n");
        #endif
        order.cmd = FORWARD;
        break;
    case 0b01:
        // TODO
        #ifdef DEBUG
            printf("BACKWARD\n");
        #endif
        order.cmd = BACKWARD;
        break;
    case 0b10:
        // TODO
        #ifdef DEBUG
            printf("TURN_RIGHT\n");
        #endif
        order.cmd = TURN_RIGHT;
        break;
    case 0b11:
        // TODO
        #ifdef DEBUG
            printf("TURN_LEFT\n");
        #endif
        order.cmd = TURN_LEFT;
        break;
        // default: //? needed ?
        //     // TODO
        //     break;
    }
    // -- params --
    order.params = (uint16_t)frame >> 2 & 0b00011111111; // need to test //? uint16_t
    #ifdef DEBUG
        printf("params: %d\n", order.params);
    #endif
    // or simply (might be 1 instruction faster? or maybe x16's gcc has -O3 ?):
    // params = (unsigned int) frame & 0b0001111111100; // need to test
    #ifdef DEBUG
        printf("DataHandler success\n");
    #endif
    return 0;
}

int ParityHandler(uint16_t frame)
{
    // TODO
    #ifdef DEBUG
        printf("ParityHandler\n");
    #endif
    // EVEN PARITY => if even number of 1's in frame, parity bit should be 0
    uint16_t x = frame >> 2; // get the frame without the parity and stop bits => 11 bits //? uint16_t
    // start bit is always 0 (or else the FSM would be in IDLE state and never get here)

    // Hamming weight algorithm:
    x ^= x >> 1;
    x ^= x >> 2;
    x ^= x >> 4;
    x ^= x >> 6;
    uint16_t parity = x & 1;

    #ifdef DEBUG
        printf("parity: %d\n", parity);
    #endif

    if (parity == 0 && frame >> 1 == 0b000000000000)
    { // EVEN
        // TODO
        #ifdef DEBUG
            printf("ParityHandler success\n");
        #endif
        return 0;
    }
    else if (parity == 1 && frame >> 1 == 0b000000000001)
    { // ODD
        // TODO
        #ifdef DEBUG
            printf("ParityHandler success\n");
        #endif
        return 0;
    }
    else
    {
        // PARITY ERROR
        // TODO
        return 1;
    }
}

int StopHandler(uint16_t frame)
{
    // TODO
    #ifdef DEBUG
        printf("StopHandler\n");
    #endif
    if (frame & 0b0000000000001 == 1)
    {
        // TODO
        #ifdef DEBUG
            printf("StopHandler success\n");
        #endif
        return 0;
    }
    else
    {
        // TODO
        // STOP ERROR
        return 1;
    }
}

//* Frame Finite State Machine
int FrameFSM(uint16_t frame)
{ /// an event (sound freq demodulation) should call the Frame FSM with the 13 bit frame as arg

    frame = frame;             //? really needed ? haha
    state currentState = IDLE; //? test

    #ifdef DEBUG
        printf("FrameFSM received frame: %u\n", frame);
    #endif

    int finished = 0;
    int success = 0; // motors should do nothing if success == 0

    while (finished == 0)
    {
        switch (currentState)
        {
        case IDLE: //? needed ?
            // TODO
            #ifdef DEBUG
                printf("IDLE state");
            #endif
            int idle;
            idle = IdleHandler(frame);
            if (idle == 0)
            {
                currentState = START;
            }
            else
            {
                currentState = IDLE;
                finished = 1;
            }
            break;
        case START:
            // TODO
            #ifdef DEBUG
                printf("START state");
            #endif
            int start;
            start = StartHandler(frame);
            if (start == 0)
            {
                currentState = DATA;
            }
            else
            {
                currentState = IDLE;
                finished = 1;
            }
            break;
        case DATA:
            // TODO
            #ifdef DEBUG
                printf("DATA state");
            #endif
            int data;
            data = DataHandler(frame);
            if (data == 0)
            {
                currentState = PARITY;
            }
            else
            {
                //* souldn't really happen
                currentState = IDLE;
                finished = 1;
            }
            break;
        case PARITY:
            // TODO
            #ifdef DEBUG
                printf("PARITY state");
            #endif
            int parity;
            parity = ParityHandler(frame);
            if (parity == 0)
            {
                currentState = STOP;
            }
            else
            {
                // should throw error
                currentState = IDLE;
                finished = 1;
            }
            break;
        case STOP:
            // TODO
            #ifdef DEBUG
                printf("STOP state");
            #endif
            int stop;
            stop = StopHandler(frame);
            if (stop == 0)
            {
                currentState = IDLE;
                finished = 1;
                success = 1;
                #ifdef DEBUG
                    printf("FSM finished and succeeded\n");
                #endif
            }
            else
            {
                // should throw error
                currentState = IDLE;
                finished = 1;
                #ifdef DEBUG
                    printf("FSM finished and failed\n");
                #endif
            }
            break;
        //default: // needed?
        //    // TODO
        //    currentState = IDLE;
        //    finished = 1;
        //    break;
        }
    }
    if (success == 1)
    {
        #ifdef DEBUG
            printf("FSM success\n");
        #endif
        return 0; // the motors can get the command and data to run
    }
    else
    {
        return 1; // the motors should do nothing
    }
}


/*
typedef enum {
    FORWARD,
    BACKWARD,
    TURN_RIGHT,
    TURN_LEFT
} command;

typedef struct {
    command cmd;
    unsigned int param;
} frame_data;

typedef enum {
    START_BIT,
    DATA, // needed?
    PARITY,
    STOP_BIT
} frame_state;

typedef struct {
    frame_state start_bit;
    frame_data data;
    frame_state parity;
    frame_state stop_bit;
} frame;

int FrameFSM() {

    frame_state frame_state = START_BIT;
    frame frame;
    frame.data.cmd = FORWARD;
    frame.data.param = 10;
    int parity = 0; // even parity, or odd parity?
    int started = 0;
    int stopped = 0;

    //TODO state machine ?

    switch (frame_state) {
        case START_BIT:
            // TODO
            started = 1;
            stopped = 0;
            break;
        case DATA:
            // TODO
            switch (frame.data.cmd) {
                case FORWARD:
                    // TODO
                    parity++; //TODO
                    forward(frame.data.param);
                    break;
                case BACKWARD:
                    // TODO
                    backward(frame.data.param);
                    break;
                case TURN_RIGHT:
                    // TODO
                    turn_right(frame.data.param);
                    break;
                case TURN_LEFT:
                    // TODO
                    turn_left(frame.data.param);
                    break;
            }
            break;
        case PARITY:
            // TODO
            if (frame_state == 0 && parity%2 == 0) {// TODO
                // TODO
            } else if (frame_state == 1 && parity%2 == 1) {// TODO
                // TODO
            } else {
                // error
            }
            break;
        case STOP_BIT:
            // TODO
            stopped = 1;
            started = 0;
            break;
        default:
            // TODO
            // error?
            break;
    }

    return 0;
}
*/

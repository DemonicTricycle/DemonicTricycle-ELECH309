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

// command cmd;
// uint16_t params;
////int success = 0; // motors should do nothing if success == 0

movement order; // TODO:
state current_state = IDLE;
uint8_t bit_count = 0;
int8_t ones = 0;
int8_t zeroes = 0;
uint8_t params = 0;
uint8_t bit_params = 0;
uint8_t cmd_bits = 0;
command cmd;

void resetFSM()
{
    current_state = IDLE;
    bit_count = 0;
    ones = 0;
    zeroes = 0;
    params = 0;
    bit_params = 0;
}

int IdleHandler(signal signal_state) //? needed?
{
// TODO
#ifdef DEBUG
    printf("IdleHandler\n");
#endif

    if (signal_state == BIT_0)
    {
#ifdef DEBUG
        printf("StartHandler success\n");
#endif
        current_state = START;
        zeroes++;
        return 0;
    }
    else
    {
#ifdef DEBUG
        printf("StartHandler fail\n");
#endif
        current_state = IDLE;
        resetFSM();
        return 1;
    }

    ////    // if (frame == 0b0000000000000) // more efficient than the following ?
    ////    if (frame >> 12 == 0b0)
    ////{ // need to test
    ////    #ifdef DEBUG
    ////            printf("IdleHandler success\n");
    ////    #endif
    ////    return 0;
    ////}
    ////else
    ////{
    ////    return 1;
    ////}
}

int StartHandler(signal signal_state)
//? useful?
{
    // TODO
#ifdef DEBUG
    printf("StartHandler\n");
#endif

    // if (signal_state == BIT_0)
    //{
    // #ifdef DEBUG
    //     printf("StartHandler success\n");
    // #endif
    //     current_state = START;
    //     zeroes++;
    //     return 0;
    // }
    // else
    //{
    // #ifdef DEBUG
    //     printf("StartHandler fail\n");
    // #endif
    //     current_state = IDLE;
    //     resetFSM();
    //     return 1;
    // }

    ////
    ////// if (!frame & 0b1000000000000) // more efficient than the following ?
    ////if (frame >> 12 == 0b0)
    ////{ // need to test
    ////    #ifdef DEBUG
    ////        printf("StartHandler success\n");
    ////    #endif
    ////    return 0;
    ////}
    ////else
    ////{
    ////    return 1;
    ////}
}

int DataHandler(signal signal_state)
{
// TODO
#ifdef DEBUG
    printf("DataHandler\n");
#endif

    if (signal_state == BIT_0)
    {
        if (bit_count <= 2) // cmd bits
        {
            cmd_bits |= 0 << (2 - bit_count);
            zeroes++;
            bit_count++;
        }
        else // params bits
        {
            // params = params | (0 << (7-bit_params));
            params |= 0 << (10 - bit_count);
            zeroes++;
            bit_count++;
            bit_params++;
        }
    }
    else // signal_state == BIT_1
    {
        if (bit_count <= 2) // cmd bits
        {
            cmd_bits |= 0 << (2 - bit_count);
            ones++;
            bit_count++;
        }
        else // params bits
        {
            // params = params | (1 << (7-bit_params)); // or just |=
            params |= 1 << (10 - bit_count);
            ones++;
            bit_count++;
            bit_params++;
        }
    }

    if (bit_count == 2)
    {
        switch (cmd_bits)
        {
        case 0b00:
            cmd = FORWARD;
            break;
        case 0b01:
            cmd = BACKWARD;
            break;
        case 0b10:
            cmd = TURN_RIGHT;
            break;
        case 0b11:
            cmd = TURN_LEFT;
            break;
        }
    }

    if (bit_count == 10)
    {
        current_state = PARITY;
#ifdef DEBUG
        printf("DataHandler success\n");
#endif
        return 0;
    }
    else
    {
#ifdef DEBUG
        printf("Data bit added\n");
#endif
        return 1;
    }

    ////// -- cmd --
    ////// more efficient way than the following ? ike just comparing the whole 13bit frame to the 4 possible commands
    ////switch (frame >> 10)
    ////{
    ////case 0b00:
    ////    // TODO
    ////    #ifdef DEBUG
    ////        printf("FORWARD\n");
    ////    #endif
    ////    order.cmd = FORWARD;
    ////    break;
    ////case 0b01:
    ////    // TODO
    ////    #ifdef DEBUG
    ////        printf("BACKWARD\n");
    ////    #endif
    ////    order.cmd = BACKWARD;
    ////    break;
    ////case 0b10:
    ////    // TODO
    ////    #ifdef DEBUG
    ////        printf("TURN_RIGHT\n");
    ////    #endif
    ////    order.cmd = TURN_RIGHT;
    ////    break;
    ////case 0b11:
    ////    // TODO
    ////    #ifdef DEBUG
    ////        printf("TURN_LEFT\n");
    ////    #endif
    ////    order.cmd = TURN_LEFT;
    ////    break;
    ////    // default: //? needed ?
    ////    //     // TODO
    ////    //     break;
    ////}
    ////// -- params --
    ////order.params = (uint16_t)frame >> 2 & 0b00011111111; // need to test //? uint16_t
    ////#ifdef DEBUG
    ////    printf("params: %d\n", order.params);
    ////#endif
    ////// or simply (might be 1 instruction faster? or maybe x16's gcc has -O3 ?):
    ////// params = (unsigned int) frame & 0b0001111111100; // need to test
    ////#ifdef DEBUG
    ////    printf("DataHandler success\n");
    ////#endif
    ////return 0;
}

int ParityHandler(signal signal_state)
{
// TODO
#ifdef DEBUG
    printf("ParityHandler\n");
#endif

    if (zeroes - ones == 0)
    {
        // even parity
        if (signal_state == BIT_0)
        {
#ifdef DEBUG
            printf("ParityHandler success\n");
#endif
            current_state = STOP;
            return 0;
        }
        else
        {
#ifdef DEBUG
            printf("ParityHandler fail\n");
#endif
            resetFSM();
            return 1;
        }
    }
    else
    {
        // odd parity
        if (signal_state == BIT_1)
        {
#ifdef DEBUG
            printf("ParityHandler success\n");
#endif
            current_state = STOP;
            return 0;
        }
        else
        {
#ifdef DEBUG
            printf("ParityHandler fail\n");
#endif
            resetFSM();
            return 1;
        }
    }

    /*
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
        */
}

int StopHandler(signal signal_state)
{
#ifdef DEBUG
    printf("StopHandler\n");
#endif

    if (signal_state == BIT_1)
    {
#ifdef DEBUG
        printf("StopHandler success\n");
#endif
        //TODO: send order to robot
        order.cmd = cmd;
        order.params = params;
        sendToMotors(order); // TODO

        resetFSM();
        return 0;
    }
    else
    {
#ifdef DEBUG
        printf("StopHandler fail\n");
#endif
        resetFSM();
        return 1;
    }

    // if (frame & 0b0000000000001 == 1)
    //{
    //    #ifdef DEBUG
    //        printf("StopHandler success\n");
    //    #endif
    //    return 0;
    //}
    // else
    //{
    //    // TODO
    //    // STOP ERROR
    //    return 1;
    //}
}

//* Frame Finite State Machine
int FrameFSM(uint8_t low, uint8_t high) //?
{
// TODO:
#ifdef DEBUG
    printf("FrameFSM bit received:\n");
    if (low)
    {
        printf("low\n");
    }
    else
    {
        printf("high\n");
    }
#endif

    // uint8_t bit;

    signal signal_state;

    if (low)
    {
        if (high)
        {
            // noise
            signal_state = NOISE;
        }
        else
        {
            // BIT 0
            // bit = 0;
            signal_state = BIT_0;
        }
    }
    else
    {
        if (high)
        {
            // BIT 1
            // bit = 1;
            signal_state = BIT_1;
        }
        else
        {
            // no signal
            signal_state = NO_SIGNAL;
        }
    }

    // int finished = 0;
    int success = 0; // motors should do nothing if success == 0

    switch (current_state)
    {
    case IDLE:
// TODO
#ifdef DEBUG
        printf("IDLE state");
#endif

        int idle; //? needed?
        idle = IdleHandler(signal_state);
        break;

        // if (idle == 0)
        //{
        //     current_state = START;
        //     bit_count++;
        //     zeroes++;
        // }
        // else
        //{
        //     current_state = IDLE;
        //     bit_count = 0;
        //     zeroes = 0;
        //     ones = 0;
        //     //finished = 1;
        // }
        //break;
    case START:
// TODO
#ifdef DEBUG
        printf("START state");
#endif
        int start;
        start = StartHandler(signal_state);
        break;

        // if (start == 0)
        //{
        //     current_state = DATA;
        // }
        // else
        //{
        //     current_state = IDLE;
        //     finished = 1;
        // }
        //break;
    case DATA:
// TODO
#ifdef DEBUG
        printf("DATA state");
#endif
        int data;
        data = DataHandler(signal_state);
        break;

        //if (data == 0)
        //{
        //    current_state = PARITY;
        //}
        //else
        //{
        //    //* souldn't really happen
        //    current_state = IDLE;
        //    finished = 1;
        //}
        //break;
    case PARITY:
// TODO
#ifdef DEBUG
        printf("PARITY state");
#endif
        int parity;
        parity = ParityHandler(signal_state);
        break;

        //if (parity == 0)
        //{
        //    current_state = STOP;
        //}
        //else
        //{
        //    // should throw error
        //    current_state = IDLE;
        //    finished = 1;
        //}
        //break;
    case STOP:
// TODO
#ifdef DEBUG
        printf("STOP state");
#endif
        int stop;
        stop = StopHandler(signal_state);
        if (stop == 0)
        {
            success = 1;
        }
        else
        {
            // should throw error
            success = 0;
        }
        break;

        //if (stop == 0)
        //{
        //    current_state = IDLE;
        //    finished = 1;
        //    success = 1;
        //#ifdef DEBUG
        //    printf("FSM finished and succeeded\n");
        //#endif
        //}
        //else
        //{
        //    // should throw error
        //    current_state = IDLE;
        //    finished = 1;
        //#ifdef DEBUG
        //    printf("FSM finished and failed\n");
        //#endif
        //}
        //break;
        // default: // needed?
        //     // TODO
        //     currentState = IDLE;
        //     finished = 1;
        //     break;
    }

    if (success == 1)
    {
#ifdef DEBUG
        printf("FSM success\n");
#endif
        //! here or in StopHandler?
        order.cmd = cmd;
        order.params = params;
        MotorsOrder(order); // TODO:
        return 0; // the motors can get the command and data to run
    }
    else
    {
        return 1; // the motors should do nothing, frame is either invalid or being processed
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

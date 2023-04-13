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

#ifdef DEBUG
#include <stdio.h>
#endif

movement order; // TODO:
state current_state = IDLE;
uint8_t bit_count_fsm = 0;
int8_t ones = 0;
int8_t zeroes = 0;
uint8_t params = 0;
uint8_t bit_params = 0;
uint8_t cmd_bits = 0;
command cmd;

void resetFSM()
{
    current_state = IDLE;
    bit_count_fsm = 0;
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

    return 0;
}

int StartHandler(signal signal_state)
//? useful?
{
    // TODO
#ifdef DEBUG
    printf("StartHandler\n");
#endif

     if (signal_state == BIT_0)
    {
     #ifdef DEBUG
         printf("StartHandler success\n");
     #endif
         current_state = DATA;
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
}

int DataHandler(signal signal_state)
{
// TODO
#ifdef DEBUG
    printf("DataHandler\n");
#endif

    if (signal_state == BIT_0)
    {
        if (bit_count_fsm <= 2) // cmd bits
        {
            cmd_bits |= 0 << (2 - bit_count_fsm);
            zeroes++;
            bit_count_fsm++;
        }
        else // params bits
        {
            // params = params | (0 << (7-bit_params));
            params |= 0 << (10 - bit_count_fsm);
            zeroes++;
            bit_count_fsm++;
            bit_params++;
        }
    }
    else // signal_state == BIT_1
    {
        if (bit_count_fsm <= 2) // cmd bits
        {
            cmd_bits |= 0 << (2 - bit_count_fsm);
            ones++;
            bit_count_fsm++;
        }
        else // params bits
        {
            // params = params | (1 << (7-bit_params)); // or just |=
            params |= 1 << (10 - bit_count_fsm);
            ones++;
            bit_count_fsm++;
            bit_params++;
        }
    }

    if (bit_count_fsm == 2)
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

    if (bit_count_fsm == 10)
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
        //sendToMotors(order); //TODO:

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
}

//* Frame Finite State Machine
int FrameFSM(uint8_t low, uint8_t high) //?
{
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

    if ( current_state == IDLE & (signal_state == BIT_0 || signal_state == BIT_1))
    {
        current_state = START;
        // int idle; //? needed?
        // idle = IdleHandler(signal_state);
    }

    switch (current_state)
    {
    case IDLE:
// TODO
#ifdef DEBUG
        printf("IDLE state");
#endif

        //* int idle; //? needed?
        //* idle = IdleHandler(signal_state);
        break;
    case START:
// TODO
#ifdef DEBUG
        printf("START state");
#endif
        StartHandler(signal_state);
        break;
    case DATA:
// TODO
#ifdef DEBUG
        printf("DATA state");
#endif
        DataHandler(signal_state);
        break;
    case PARITY:
// TODO
#ifdef DEBUG
        printf("PARITY state");
#endif
        ParityHandler(signal_state);
        break;
    case STOP: ;
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
    }

    if (success == 1)
    {
#ifdef DEBUG
        printf("FSM success\n");
#endif
        //! here or in StopHandler?
        order.cmd = cmd;
        order.params = params;
        //MotorsOrder(order); // TODO:
        return 0; // the motors can get the command and data to run
    }
    else
    {
        return 1; // the motors should do nothing, frame is either invalid or being processed
    }
}

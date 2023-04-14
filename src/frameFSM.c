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
//#include "main.h"
#include "tools.h"
#include "parameters.h"
#include <stdint.h>

#ifdef TEST
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

int IdleHandler(signal signal_state)
{
// TODO
#ifdef TEST
    printf("IdleHandler\n");
#endif

    sendUartChars("Idling\n");
    return 0;
}

int StartHandler(signal signal_state)
{
    // TODO
#ifdef TEST
    printf("StartHandler\n");
#endif

     if (signal_state == BIT_0)
    {
     #ifdef TEST
         printf("StartHandler success\n");
     #endif
        sendUartChars("Started\n");
        current_state = DATA;
        zeroes++;
        bit_count_fsm++;
        return 0;
     }
     else
    {
     #ifdef TEST
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
#ifdef TEST
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

    if (bit_count_fsm == 3)
    {
        switch (cmd_bits)
        {
        case 0b00:
            cmd = FORWARD;
            sendUartChars("=> FORWARD\n");
            break;
        case 0b01:
            cmd = BACKWARD;
            sendUartChars("=> BACKWARD\n");
            break;
        case 0b10:
            cmd = TURN_RIGHT;
            sendUartChars("=> TURN RIGHT\n");
            break;
        case 0b11:
            cmd = TURN_LEFT;
            sendUartChars("=> TURN LEFT\n");
            break;
        }
    }

    if (bit_count_fsm > 10)
    {
        current_state = PARITY;
#ifdef TEST
        printf("DataHandler success\n");
#endif
        sendUartChars("data success, data:\n");
        sendUartInt16(params);
        return 0;
    }
    else
    {
#ifdef TEST
        printf("Data bit added\n");
#endif
        return 1;
    }
}

int ParityHandler(signal signal_state)
{
// TODO
#ifdef TEST
    printf("ParityHandler\n");
#endif

    if (!(ones & 0b01)) // bitwise operation that checks if the number of ones is not odd
    {
        sendUartChars("even parity\n");
        // even parity
        if (signal_state == BIT_0)
        {
#ifdef TEST
            printf("ParityHandler success\n");
#endif
            sendUartChars("parity success\n");
            bit_count_fsm++;
            current_state = STOP;
            return 0;
        }
        else
        {
#ifdef TEST
            printf("ParityHandler fail\n");
#endif
            sendUartChars("parity fail\n");
            resetFSM();
            return 1;
        }
    }
    else
    {
        sendUartChars("odd parity\n");
        // odd parity
        if (signal_state == BIT_1)
        {
#ifdef TEST
            printf("ParityHandler success\n");
#endif
            sendUartChars("parity success\n");
            bit_count_fsm++;
            current_state = STOP;
            return 0;
        }
        else
        {
#ifdef TEST
            printf("ParityHandler fail\n");
#endif
            sendUartChars("parity fail\n");
            resetFSM();
            return 1;
        }
    }
}

int StopHandler(signal signal_state)
{
#ifdef TEST
    printf("StopHandler\n");
#endif

    if (signal_state == BIT_1)
    {
#ifdef TEST
        printf("StopHandler success\n");
#endif
        //TODO: send order to robot
        //order.cmd = cmd;
        //order.params = params;
        //sendToMotors(order); //TODO:

        bit_count_fsm++;

        //resetFSM();
        return 0;
    }
    else
    {
#ifdef TEST
        printf("StopHandler fail\n");
#endif
        resetFSM();
        return 1;
    }
}

//* Frame Finite State Machine
int FrameFSM(uint8_t low, uint8_t high)
{
    sendUartChars("Received: ");
    if (low)
    {
        sendUartChars("LOW\n");
    }
    if (high)
    {
        sendUartChars("HIGH\n");
    }
#ifdef TEST
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

    if ( current_state == IDLE && (signal_state == BIT_0 || signal_state == BIT_1))
    {
        current_state = START;
    }

    switch (current_state)
    {
    case IDLE:
#ifdef TEST
        printf("IDLE state\n");
#endif

        IdleHandler(signal_state);
        break;
    case START:
#ifdef TEST
        printf("START state\n");
#endif
        StartHandler(signal_state);
        break;
    case DATA:
#ifdef TEST
        printf("DATA state\n");
#endif
        DataHandler(signal_state);
        break;
    case PARITY:
#ifdef TEST
        printf("PARITY state\n");
#endif
        ParityHandler(signal_state);
        break;
    case STOP: ; //requires this semicolon for unknown reason
#ifdef TEST
        printf("STOP state\n");
#endif
        int stop;
        stop = StopHandler(signal_state);
        if (stop == 0) // no error
        {
            sendUartChars("FSM stop\n");
            success = 1;
        }
        else
        {
            // should throw error

            success = 0;
        }
        break;
    }

    if (success)
    {
#ifdef TEST
        printf("FSM success\n");
#endif
        sendUartChars("FSM success\n");
        //! here or in StopHandler?
        //order.cmd = cmd;
        //order.params = params;
        //MotorsOrder(order); // TODO:
        switch (cmd)
        {
            case FORWARD:
                sendUartChars("Forward:\n");
                sendUartInt16((int16_t) params);
                #ifndef TEST
                Move((float)params, 0.0);
                #endif
                break;
            case BACKWARD:
                sendUartChars("Backward:\n");
                sendUartInt16((int16_t) params);
                #ifndef TEST
                Move(-((float)params), 0.0);
                #endif
                break;
            case TURN_RIGHT:
                sendUartChars("Right:\n");
                sendUartInt16((int16_t) params);
                #ifndef TEST
                Move(0.0, (-(float)params) * 3.14 / 180.0); //TODO: use PI define
                #endif
                break;
            case TURN_LEFT: ;
                sendUartChars("Left:\n");
                sendUartInt16((int16_t) params);
                #ifndef TEST
                Move(0.0, ((float)params) * 3.14 / 180.0);
                #endif
                break;
        }

        resetFSM();

        return 0; // the motors can get the command and data to run
    }
    else
    {
        return 1; // the motors should do nothing, frame is either invalid or being processed
    }
}

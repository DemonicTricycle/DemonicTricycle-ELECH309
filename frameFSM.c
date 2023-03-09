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

// TODO: make state machine diagram

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

command cmd;
unsigned int params;
////int success = 0; // motors should do nothing if success == 0

int FrameFSM(frame)
{ /// an event (sound freq demodulation) should call the Frame FSM with the 13 bit frame as arg

    frame = frame;             //? test
    state currentState = IDLE; //? test

    int finished = 0;
    int success = 0; // motors should do nothing if success == 0

    while (finished == 0)
    {
        switch (currentState)
        {
        case IDLE: //? needed ?
            // TODO
            int idle = IdleHandler(frame);
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
            int start = StartHandler(frame);
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
            int data = DatatHandler(frame);
            if (data == 0)
            {
                currentState = PARITY;
            }
            else
            {
                // souldn't really happen
                currentState = IDLE;
                finished = 1;
            }
            break;
        case PARITY:
            // TODO
            int parity = ParityHandler(frame);
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
            int stop = StopHandler(frame);
            if (stop == 0)
            {
                currentState = IDLE;
                finished = 1;
                success = 1;
            }
            else
            {
                // should throw error
                currentState = IDLE;
                finished = 1;
            }
            break;
        default: // needed?
            // TODO
            currentState = IDLE;
            finished = 1;
            break;
        }
    }
    if (success == 1)
    {
        return 0; // the motors can get the command and data to run
    }
    else
    {
        return 1; // the motors should do nothing
    }
}

int IdleHandler(frame)
//? needed ?
{
    // TODO
    if (frame >> 12 == 0b0)
    { // need to test
        return 0;
    }
    else
    {
        return 1;
    }
}

int StartHandler(frame)
{
    // TODO
    if (frame >> 12 == 0b0)
    { // need to test
        return 0;
    }
    else
    {
        return 1;
    }
}

int DataHandler(frame)
{
    // TODO
    // -- cmd --
    switch (frame >> 10)
    {
    case 0b00:
        // TODO
        cmd = FORWARD;
        break;
    case 0b01:
        // TODO
        cmd = BACKWARD;
        break;
    case 0b10:
        // TODO
        cmd = TURN_RIGHT;
        break;
    case 0b11:
        // TODO
        cmd = TURN_LEFT;
        break;
    default: //? needed ?
        // TODO
        break;
    }
    // -- params --
    params = (unsigned int)frame >> 2 & 0b00011111111; // need to test
    // or simply:
    // params = (unsigned int) frame & 0b0001111111100; // need to test
    return 0;
}

int ParityHandler(frame)
{
    // TODO
    // EVEN PARITY => if even number of 1's in frame, parity bit should be 0
    unsigned int x = frame >> 2; // get the frame without the parity and stop bits => 11 bits
    // start bit is always 0 (or else the FSM would be in IDLE state and never get here)

    // Hamming weight algorithm:
    x ^= x >> 1;
    x ^= x >> 2;
    x ^= x >> 4;
    x ^= x >> 6;
    unsigned int parity = x & 1;

    if (parity == 0 && frame >> 1 == 0b000000000000)
    { // EVEN
        // TODO
        return 0;
    }
    else if (parity == 1 && frame >> 1 == 0b000000000001)
    { // ODD
        // TODO
        return 0;
    }
    else
    {
        // PARITY ERROR
        // TODO
        return 1;
    }
}

int StopHandler(frame)
{
    // TODO
    if (frame & 0b0000000000001 == 1)
    {
        // TODO
        return 0;
    }
    else
    {
        // TODO
        // STOP ERROR
        return 1;
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

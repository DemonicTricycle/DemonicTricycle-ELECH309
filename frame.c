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

// TODO change function name
int TODO_CHANGE_FCTN_NAME() {

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
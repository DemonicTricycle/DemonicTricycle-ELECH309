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

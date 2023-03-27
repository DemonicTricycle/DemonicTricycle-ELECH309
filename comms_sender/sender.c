#include <stdio.h>
#include "../frameFSM.h"
#include <stdint.h>

// TODO

// input cm or degrees must be 0 <= x <= 255 (8bits of params, uint8_t)

void main() {
    while(1) {
        // TODO
        uint16_t input_command;
        command requested_command;
        uint16_t input_params;
        uint16_t requested_params;

        uint16_t command_correct = 0;
        while (command_correct == 0) {
            printf("Enter command :\n1. FORWARD\n2. BACKWARD\n3. TURN_RIGHT\n4. TURN_LEFT\n? :\n");
            scanf("%d", &input_command);
            switch (input_command)
            {
            case 0:
                requested_command = FORWARD;
                command_correct = 1;
                break;
            case 1:
                requested_command = BACKWARD;
                command_correct = 1;
                break;
            case 2:
                requested_command = TURN_RIGHT;
                command_correct = 1;
                break;
            case 3:
                requested_command = TURN_LEFT;
                command_correct = 1;
                break;
            default:
                printf("Invalid command, try again\n");
                break;
            }
        }

        input_params = 256;
        while(input_params < 0 || input_params > 255)
        {
            printf("Enter params (");
            switch (requested_command)
            {
            case FORWARD:
                printf("cm)");
                break;
            case BACKWARD:
                printf("cm)");
                break;
            case TURN_RIGHT:
                printf("degrees)");
                break;
            case TURN_LEFT:
                printf("degrees)");
                break;
            }
            printf(" between 0 and 255 :\n");
            scanf("%u", &input_params);
        }
        requested_params = input_params;
        printf("Request correctly entered\n");

        int create_result = createSoundFrame(requested_command, input_params);
        printf("Sound frame created, code %d\n", create_result);
        int send_result = sendSoundFrame(sound);
        printf("Sound frame sent, code %d\n", send_result);

    } // sender program loop
}

void printBinary(uint16_t number)
{
    if (number >> 1)
    {
        print_binary(number >> 1);
    }
    putc((number & 1) ? '1' : '0', stdout);
}

int createSoundFrame(command requested_command, uint16_t requested_params) {
    // TODO

    uint16_t frame;
    // create frame
    switch (requested_command)
    {
    case FORWARD:
        frame = 0b0000000000000;
        break;
    case BACKWARD:
        frame = 0b0010000000000;
        break;
    case TURN_RIGHT:
        frame = 0b0100000000000;
        break;
    case TURN_LEFT:
        frame = 0b0110000000000;
        break;
    }
    frame = frame | (requested_params << 2); // bitwise OR to add params to frame (double bitshift left to add 2 zeroes after the params)

    // check parity using the hamming weight algorithm
    uint16_t n = frame;
    n ^= n >> 6;
    n ^= n >> 3;
    n ^= n >> 1;
    int parity = n & 1;
    if (parity == 0) {
        // even parity
        frame = frame | 0b0000000000000;
    } else {
        // odd parity
        frame = frame | 0b0000000000010;
    }

    // add stop bit
    frame = frame | 0b0000000000001;
    // frame created
    printf("Frame created : ");
    printBinary(frame);
    pringf("\n");

    // TODO
    

    //? :
    uint16_t frame = 0;
    frame = frame | (sound << 8);
    frame = frame | (0b10 << 6);
    frame = frame | (0b00 << 4);
    frame = frame | (0b00 << 2);
    frame = frame | (0b00 << 0);
    return frame;
}

int sendSoundFrame(sound){
    // TODO
    // play sound through speakers
}
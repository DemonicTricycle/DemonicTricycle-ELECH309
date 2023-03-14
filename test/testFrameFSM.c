#include <stdio.h>
#include "../frameFSM.h"

int errors = 0;
int test_errors = 0;

void printBinary(unsigned int number)
{
    if (number >> 1)
    {
        print_binary(number >> 1);
    }
    putc((number & 1) ? '1' : '0', stdout);
}

void printTest(unsigned int frame) {
    printf("Frame: ");
    printBinary(frame);
    printf("\n");
    int res = FrameFSM(frame);
    printf("Result: %d\n", res);
    if (res == 0)
    {
        printf("Frame is valid\n");
        printf("Command: ");
        switch (cmd)
        {
        case FORWARD:
            printf("FORWARD\n");
            break;
        case BACKWARD:
            printf("BACKWARD\n");
            break;
        case TURN_RIGHT:
            printf("TURN_RIGHT\n");
            break;
        case TURN_LEFT:
            printf("TURN_LEFT\n");
            break;
        }
        printf("Params: %u\n", params);
    }
    else
    {
        printf("Frame is invalid\n");
        test_errors++;
        printf("Command: ");
        switch (cmd)
        {
        case FORWARD:
            printf("FORWARD\n");
            break;
        case BACKWARD:
            printf("BACKWARD\n");
            break;
        case TURN_RIGHT:
            printf("TURN_RIGHT\n");
            break;
        case TURN_LEFT:
            printf("TURN_LEFT\n");
            break;
        }
        printf("Params: %u\n", params);
    }
}

int main()
{
    errors = 0;

    printf("Test 1: start, BACKWARD, xx cm, even parity bit, parity error, stop\n"); // error
    unsigned int frame = 0b0011111111001;
    printTest(frame);

    printf("Test 2: start, FORWARD, xx cm, even parity bit, parity correct, stop\n"); // success
    unsigned int frame = 0b0001111111001;
    printTest(frame);

    printf("Test 3: start, TURN_RIGHT, xx degrees, even parity bit, parity error, stop error\n"); // error
    unsigned int frame = 0b0101111111000;
    printTest(frame);

    return errors==test_errors;

}


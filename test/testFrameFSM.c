#define DEBUG 1

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "../frameFSM.h"
//#include "frameFSM.h"

//! use assert() etc

int errors = 0;
int test_errors = 0;
//uint16_t params;
//command cmd;

void printBinary(uint16_t number)
{
    if (number >> 1)
    {
        printBinary(number >> 1);
    }
    putc((number & 1) ? '1' : '0', stdout);
}

void printTest(uint16_t frame) 
{
    printf("Frame: ");
    printBinary(frame);
    printf("\n");
    int res = FrameFSM(frame);
    printf("Result: %d\n", res);
    if (res == 0)
    {
        printf("Frame is valid\n");
        printf("Command: ");
        switch (order.cmd)
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
        printf("Params: %u\n", order.params);
    }
    else
    {
        printf("Frame is invalid\n");
        test_errors++;
        printf("Command: ");
        switch (order.cmd)
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
        printf("Params: %u\n", order.params);
    }
}

int main(void)
{
    errors = 0;
    uint16_t frame;

    printf("Test 1: start, BACKWARD, xx cm, even parity bit, parity error, stop\n"); // error
    frame = 0b0011101111001;
    errors++;
    printTest(frame);

    printf("Test 2: start, FORWARD, xx cm, even parity bit, parity correct, stop\n"); // success
    frame = 0b0001101111001;
    printTest(frame);

    printf("Test 3: start, TURN_RIGHT, xx degrees, even parity bit, parity error, stop error\n"); // error
    frame = 0b0101101111000;
    errors++;
    printTest(frame);

    assert(errors == test_errors);

    return 0;
}


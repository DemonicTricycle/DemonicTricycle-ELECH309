/*

Implementation test of FrameFSM function

*/

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "../../src/frameFSM.h"
//#include "frameFSM.h"

//#define DEBUG 1

//! use assert() etc

int errors = 0;
int test_errors = 0;
//uint16_t params;
//command cmd;
//movement order;

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
    printf(" > Frame: ");
    printBinary(frame);
    printf("\n");
    int res;

    ////for (int i = 0; i < 16; i++)
    ////{
    ////    printf("%d", (frame >> i) & 1);
    ////}

    for (int p = 13; p >= 1; p--)
    {
        uint16_t bit = (frame >> p) & 0b01;
        uint8_t low;
        uint8_t high;
        if (bit)
        {
            high = 1;
            low = 0;
        }
        else
        {
            high = 0;
            low = 1;
        }
        res = FrameFSM(low, high);
    }
    //int res = FrameFSM(frame);
    printf("=> Result: %d\n", res);
    if (res == 0)
    {
        printf("____Frame is valid____\n");
        printf("-Command: ");
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
        printf("-Params: %u\n", order.params);
    }
    else
    {
        printf("_____Frame is invalid_____\n");
        test_errors++;
        printf("-Command: ");
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
        printf("-Params: %u\n", order.params);
    }
}

int main(void)
{
    errors = 0;
    uint16_t frame;

    resetFSM();
    printf("[Test 1]: start, BACKWARD, xx cm, even parity bit, parity error, stop\n"); // error
    frame = 0b0011101111001;
    errors++;
    printTest(frame);

    printf("--------------------------------------------------------------------\n");

    resetFSM();
    printf("[Test 2]: start, FORWARD, xx cm, even parity bit, parity correct, stop\n"); // success
    frame = 0b0001101111001;
    printTest(frame);

    printf("--------------------------------------------------------------------\n");

    resetFSM();
    printf("[Test 3]: start, TURN_RIGHT, xx degrees, even parity bit, parity error, stop error\n"); // error
    frame = 0b0101101111000;
    errors++;
    printTest(frame);

    printf("--------------------------------------------------------------------\n");

    assert(errors == test_errors);

    return 0;
}


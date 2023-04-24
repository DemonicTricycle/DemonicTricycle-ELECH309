/*

Implementation test of FrameFSM function

*/

#define DEBUG 1

#include "../test.h"
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "../../src/frameFSM.h"
//#include "frameFSM.h"

//#define DEBUG 1

int errors = 0;
int test_errors = 0;
//uint16_t params;
//command cmd;
//movement order;

int Test(uint16_t frame) 
{
    printf(" > Frame: ");
    printBinary(frame);
    printf("\n");
    int res;
    int test;

    ////for (int i = 0; i < 16; i++)
    ////{
    ////    printf("%d", (frame >> i) & 1);
    ////}

    for (int p = 12; p >= 0; p--)
    {
        uint16_t bit = (frame >> p) & 0b1;
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
        test = 1;
        /*
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
        */
    }
    else
    {
        printf("_____Frame is invalid_____\n");
        test = 0;
        test_errors++;
        /*
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
        */
    }
    return !res;
}

int main(void)
{
    errors = 0;
    uint16_t frame;
    int test;

    resetFSM();
    printf("[Test 1]: start, BACKWARD, xx cm, even parity bit, parity error, stop\n"); // error
    frame = 0b0011101111001;
    errors++;
    test = Test(frame);
    assert(!test);

    printf("--------------------------------------------------------------------\n");

    resetFSM();
    printf("[Test 2]: start, FORWARD, xx cm, even parity bit, parity correct, stop\n"); // success
    frame = 0b0001101111001;
    test = Test(frame);
    assert(test);

    printf("--------------------------------------------------------------------\n");

    resetFSM();
    printf("[Test 3]: start, TURN_RIGHT, xx degrees, even parity bit, parity error, stop error\n"); // error
    frame = 0b0101101111000;
    errors++;
    test = Test(frame);
    assert(!test);

    assert(errors == test_errors);
    printf("\n----------------------------\n");
    printf("|      - Test ended -      |\n");
    printf("----------------------------\n\n");

    return 0;
}


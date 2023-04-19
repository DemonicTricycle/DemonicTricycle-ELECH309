/*

Unit test of FrameFSM's DataHandler function

*/

#define DEBUG 1

#include "../test.h"
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "../../src/frameFSM.h"
// #include "frameFSM.h"

int errors = 0;
int test_errors = 0;
uint16_t frame;

int sendBits(uint16_t frame)
{
    int res;

    for (int p = 13; p >= 1; p--)
    {
        uint16_t bit = (frame >> p) & 0b01;
        if (bit)
        {
            res = DataHandler(BIT_1);
        }
        else
        {
            res = DataHandler(BIT_0);
        }
    }
    printf("=> Result: %d", res);
    return res; // res should be 0 when finished
}

int main(void)
{
    //TODO:
    //errors = 0;
    int res;

    printf("------------------------------\n");
    printf("[Test 1]: FORWARD 13cm\n");
    frame = 0b0000000110111;
    printf(" > Frame: ");
    printBinary(frame);
    res = sendBits(frame);
    assert(!res); // should be 0 if finished
    printf("Command: %s\n", order.cmd);
    printf("Params: %d\n", order.params);

    printf("------------------------------\n");
    printf("[Test 2]: TURN_LEFT 40 degrees\n");
    frame = 0b0110010100001;
    printf(" > Frame: ");
    printBinary(frame);
    res = sendBits(frame);
    assert(!res);
    printf("Command: %s\n", order.cmd);
    printf("Params: %d\n", order.params);

    printf("------------------------------\n");
    printf("[Test 3]: BACKWARD 128cm\n");
    frame = 0b0011000000001;
    printf(" > Frame: ");
    printBinary(frame);
    res = sendBits(frame);
    assert(!res);
    printf("Command: %s\n", order.cmd);
    printf("Params: %d\n", order.params);

    printf("------------------------------\n");
    printf(" - Test ended - \n");

    return 0;
}


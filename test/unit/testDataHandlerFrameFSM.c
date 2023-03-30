/*

Unit test of FrameFSM's DataHandler function

*/

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "../../src/frameFSM.h"
// #include "frameFSM.h"

void printBinary(uint16_t number)
{
    if (number >> 1)
    {
        printBinary(number >> 1);
    }
    putc((number & 1) ? '1' : '0', stdout);
}

int main(void)
{
    ;
}


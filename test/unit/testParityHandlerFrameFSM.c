/*

Unit test of FrameFSM's ParityHandler function

*/

#define DEBUG 1

#include "../test.h"
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "../../src/frameFSM.h"
// #include "frameFSM.h"

//int errors = 0;
//int test_errors = 0;

int main(void)
{
    signal parity;
    //errors = 0;
    int test;

    printf("------------------------------\n");
    printf("[Test 1]: even parity, parity bit 0\n");         // success
    parity = BIT_0;
    ones = 0;
    zeroes = 13 - ones;
    test = ParityHandler(parity);
    assert(!test); // because returns 0 on success

    printf("------------------------------\n");
    printf("[Test 2]: even parity, parity bit 1 => fail\n");         // fail
    parity = BIT_1;
    ones = 4;
    zeroes = 13 - ones;
    test = ParityHandler(parity);
    assert(test);

    printf("------------------------------\n");
    printf("[Test 3]: odd parity, parity bit 1\n");         // success
    parity = BIT_1;
    ones = 3;
    zeroes = 13 - ones;
    test = ParityHandler(parity);
    assert(!test);

    printf("------------------------------\n");
    printf(" - Test ended - \n");

    return 0;
}


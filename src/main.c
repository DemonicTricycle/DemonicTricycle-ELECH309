/*
 * Main file of Demonic Tricycle
 * Author: Martin Fonteyn, Lucas Placentino, Sylvain Powis de Tenbossche
 *
 */

#include "xc.h" // for xc16 compiler
#ifdef TEST
#include <stdint.h>
//#include <stdio.h>
#endif

#include "parameters.h"
#include "tools.h"
#include "filter.h"

int main(void) {
    
    // Starts digital filter:
    start();

    while (1) {} // won't even get here
    return 0;
}
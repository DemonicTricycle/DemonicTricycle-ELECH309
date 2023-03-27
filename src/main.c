/*
 * Main file of Demonic Tricycle
 * Author: Martin Fonteyn, Lucas Placentino, Sylvain Powis de Tenbossche
 *
 */

#define DEBUG 1

#include "xc.h" // for xc16 compiler
#ifdef DEBUG
#include <stdio.h>
#endif


int main(void) {
    /// Starting point of the microcontroller program
    #ifdef DEBUG
    printf("starting");
    #endif


    while(1) {
        // TODO
        // check for received sound
        // get data from sound
        // send command to motors
        // wait for next sound
    }

    return 0;
}
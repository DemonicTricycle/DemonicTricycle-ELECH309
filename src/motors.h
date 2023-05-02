#ifndef TEST
#include "libpic30.h" // Contains __delay_ms definition
#include "xc.h"
#endif
#ifdef TEST
#include <stdint.h>
#endif

void Move(float translation, float angle);
void initialise(void);

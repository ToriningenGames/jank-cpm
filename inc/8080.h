#ifndef I8080_H
#define I8080_H

#include <stdint.h>


void init();
void decode();
extern uint8_t cpmimage[65536];

#endif

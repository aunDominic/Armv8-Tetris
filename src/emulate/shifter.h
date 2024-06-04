//
// Created by Dominic Ng on 25/05/2024.
//

#ifndef EMULATOR_SHIFTER_H
#define EMULATOR_SHIFTER_H
#include <stdlib.h>
#include <stdint.h>
#endif //EMULATOR_SHIFTER_H

// Treat these instructions as the ALU shifter
int64_t lsl(int bits, int64_t value, int width);
int64_t asr(int bits, int64_t value, int width);

int64_t lsr(int bits, int64_t value, int width);

int64_t ror(int bits, int64_t value, int width);


// eg
// setWidth(value, 32);
// if (want to lsl){
// lsl
// else {
// lsr
// etc
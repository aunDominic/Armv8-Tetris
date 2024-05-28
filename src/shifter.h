//
// Created by Dominic Ng on 25/05/2024.
//

#ifndef EMULATOR_SHIFTER_H
#define EMULATOR_SHIFTER_H
#include <stdlib.h>
#include <stdint.h>
#endif //EMULATOR_SHIFTER_H

// Treat these instructions as the ALU shifter
uint64_t lsl(int bits, uint64_t value); // returns the value shifted by bits amount
uint64_t lsr(int bits, uint64_t value); // value can be either a 32 bit or 64 bit number
uint64_t asr(int bits, uint64_t value);
uint64_t ror(int bits, uint64_t value);
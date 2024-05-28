//
// Created by Dominic Ng on 25/05/2024.
//

#ifndef EMULATOR_SHIFTER_H
#define EMULATOR_SHIFTER_H
#include <stdlib.h>
#endif //EMULATOR_SHIFTER_H

// Treat these instructions as the ALU shifter
u_int64_t lsl(int bits, u_int64_t value); // returns the value shifted by bits amount
u_int64_t lsr(int bits, u_int64_t value); // value can be either a 32 bit or 64 bit number
u_int64_t asr(int bits, u_int64_t value);
u_int64_t ror(int bits, u_int64_t value);
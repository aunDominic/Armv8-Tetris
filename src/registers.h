//
// Created by Dominic Ng on 25/05/2024.
//

#ifndef EMULATOR_REGISTERS_C
#define EMULATOR_REGISTERS_C
#include <stdbool.h>
struct regFile{
    // Each register represented as 64 bools, ie bits
    bool registers[31][64];
    bool zeroRegister[64];
    bool programCounter[64];
    bool PSTATE[64];
};

extern struct regFile register_file;

#endif //EMULATOR_REGISTERS_C

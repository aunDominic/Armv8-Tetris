//
// Created by Dominic Ng on 25/05/2024.
//

#ifndef EMULATOR_REGISTERS_C
#define EMULATOR_REGISTERS_C
#include <stdbool.h>
#include <stdint.h>

typedef int8_t BYTE;
typedef int32_t INST;

extern const int64_t HALT;
extern int64_t registers[31];
extern int64_t programCounter;
extern int64_t zeroRegister;
struct PSTATE{
    bool Z;
    bool N;
    bool C;
    bool V;
};
extern BYTE memory[1 << 21];
extern struct PSTATE pstate;
#endif //EMULATOR_REGISTERS_C

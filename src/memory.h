//
// Created by Dominic Ng on 25/05/2024.
//

#ifndef EMULATOR_REGISTERS_C
#define EMULATOR_REGISTERS_C
#include <stdbool.h>

typedef u_int8_t BYTE;
typedef u_int32_t INST;

extern const u_int64_t HALT;
extern u_int64_t registers[31];
extern u_int64_t programCounter;
extern u_int64_t zeroRegister;
struct PSTATE{
    bool Z;
    bool N;
    bool C;
    bool V;
};
extern BYTE memory[1 << 21];
extern struct PSTATE pstate;
#endif //EMULATOR_REGISTERS_C

//
// Created by Dominic Ng on 25/05/2024.
//

#ifndef EMULATOR_REGISTERS_C
#define EMULATOR_REGISTERS_C
#include <stdbool.h>
#include <stdint.h>

typedef uint8_t BYTE;
typedef uint32_t INST;

extern const uint64_t HALT;
extern uint64_t registers[31];
extern uint64_t programCounter;
extern uint64_t zeroRegister;
struct PSTATE{
    bool Z;
    bool N;
    bool C;
    bool V;
};
extern BYTE memory[1 << 21];
extern struct PSTATE pstate;
#endif //EMULATOR_REGISTERS_C

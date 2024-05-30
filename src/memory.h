//
// Created by Dominic Ng on 25/05/2024.
//

#ifndef EMULATOR_REGISTERS_C
#define EMULATOR_REGISTERS_C
#include <stdbool.h>
#include <stdint.h>

typedef int8_t BYTE;
typedef int32_t INST;
#define regCount 31
#define memSize 1 << 21
extern const int64_t HALT;
extern int64_t registers[regCount];
extern int64_t programCounter;
extern int64_t zeroRegister;
struct PSTATE{
    bool Z;
    bool N;
    bool C;
    bool V;
};
extern BYTE memory[memSize];
extern struct PSTATE pstate;
void printMemory(int start, int end);
void printRegisters();
#endif //EMULATOR_REGISTERS_C

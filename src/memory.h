//
// Created by Dominic Ng on 25/05/2024.
//

#ifndef EMULATOR_REGISTERS_C
#define EMULATOR_REGISTERS_C
#include <stdbool.h>
#include <stdint.h>

<<<<<<< HEAD
typedef uint8_t BYTE;
typedef int32_t INST;
#define regCount (31)
#define memSize (1 << 21)
extern const int32_t HALT;
extern int64_t registers[regCount];
=======
typedef int8_t BYTE;
typedef int32_t INST;

extern const int64_t HALT;
extern int64_t registers[31];
>>>>>>> 31c2f6806900d41caae6e00ae79e488f76d93070
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
INST getInstAtAddr(int addr);
INST fetch();
void printMemory(int start, int end);
void printRegisters();
#endif //EMULATOR_REGISTERS_C

//
// Created by Dominic Ng on 25/05/2024.
//

#include "memory.h"
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
// Zero register is set at the beginning
struct PSTATE pstate = {
        true, false, false, false
};
// 64-bit number to represent which bits are set
// Use bitmasks to query the registers
int64_t registers[31] = {0};
int64_t programCounter = 0;
int64_t zeroRegister = 0;
BYTE memory[1 << 21] = {0};
const int32_t HALT =  0x8a000000;
void printMemory(int start, int end){
    for (int i = start; i < end; i++){
        printf("Address %d: %x\n", i, memory[i]);
    }
}
void printRegisters(){
    for (int i = 0; i < regCount; i++) {
        printf("Register %d: %d\n", i, registers[i]);
    }


}
INST getInstAtAddr(int addr){
    assert(addr <= (memSize - 4) &&  "Error: Memory address out of bounds."); // ideally we have a macro for 1 << 21 or 21
    BYTE byte1 = memory[addr];
    BYTE byte2 = memory[addr+1];
    BYTE byte3 = memory[addr+2];
    BYTE byte4 = memory[addr+3];
g
    // default implementation is just to interpret these as is
    // ie default is little-endian
    INST instruction = 0;
    // now perform bitwise manipulation
    instruction |= byte4 << 24;
    instruction |= byte3 << 16;
    instruction |= byte2 << 8;
    instruction |= byte1;
    return instruction;
}
INST fetch() {
    return getInstAtAddr(programCounter);
}

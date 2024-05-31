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
const int64_t HALT =  0x8a000000;
void printMemory(int start, int end){
    for (int i = start; i < end; i++){
        printf("Address %d: %d\n", i, memory[i]);
    }
}
void printRegisters(){
    for (int i = 0; i < regCount; i++) {
        printf("Register %d: %d\n", i, registers[i]);
    }
}

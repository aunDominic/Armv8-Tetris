//
// Created by Dominic Ng on 25/05/2024.
//

#include "memory.h"
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

// Zero register is set at the beginning
struct PSTATE pstate = {
        true, false, false, false
};
// 64-bit number to represent which bits are set
// Use bitmasks to query the registers
uint64_t registers[31] = {0};
uint64_t programCounter = 0;
uint64_t zeroRegister = 0;
BYTE memory[1 << 21] = {0};
const uint64_t HALT =  0x8a000000;
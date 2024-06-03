//
// Created by Dominic Ng on 25/05/2024.
//

#include "datatransfer.h"
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "bitmanipulation.h"
#include <string.h>

// ADDR should be 21 bits
typedef uint32_t ADDR;
bool SF;
bool L;
bool U;
bool I;

const uint32_t MAX_OFFSET = 1 << 12;

static void setFlags(INST instruction){
    U = 1 << 24 & instruction;
    SF = 1 << 30 & instruction;
    L = 1 << 22 & instruction;
    I = 1 << 11 & instruction;
}
static INST SDTconstructer(int SF, int U, int L, int offset, int xn, int rt){
    assert (SF == 0 || SF == 1);
    assert (U == 0 || U == 1);
    assert(L == 0 || L == 1);
    assert(offset <= MAX_OFFSET);
    assert(rt <= regCount);
    assert(xn <= regCount);
    return (1 << 31) + (SF << 30) + (1 << 29) + (1 << 28) + (1 << 27) + (U << 24) + (L << 22) + (offset << 10) + (xn << 5) + rt;

}

static int64_t getAddress(INST instruction){
    uint32_t XN = extractBits(instruction, 5, 9);
    int64_t simm9 = sign_extend(extractBits(instruction, 12, 20), 9);

    // these may not be mutually exclusive
    if (U){
        printf("SF: %d\n", SF);
        printf("U %d\n", U);
        int64_t uoffset = extractBits(instruction, 10, 21);
        printf("offset: %lld\n", uoffset);
        if (SF) uoffset *= 8;
        else uoffset *= 4;
        printf("offset: %lld\n", uoffset);
        printf("register %d: %lld\n", XN, registers[XN]);

        return registers[XN] + uoffset;
    }
    if (I){
        return registers[XN] + simm9;
    } else {
        int64_t ret = registers[XN];
        registers[XN] += simm9;
        return ret;
    }
}
static void loadStoreHandler(ADDR addr, INST instruction){
    int rt = (int) extractBits(instruction, 0, 4);
    printf("rt: %d\n", rt);
    int n_addr;
    if (U) n_addr = 4;
    else n_addr = 8;
    if (L){
        printf("L: %d\n", L);
        int64_t bytes = 0;
        for (int i = 0; i < n_addr; i++){
            printf("Memory[%d]: %d\n", addr, memory[addr + i]);
            bytes += memory[addr + i] << (i * 8);
        }
        registers[rt] = bytes;
    } else { // store
        printf("L: %d\n", L);

        for (int i = 0; i < n_addr; i++){
            memory[addr] = (BYTE) extractBits(registers[rt], i * 8, (i+1) * 8 - 1);
        }
    }
}
bool singleDataTransferHandler(INST instruction){
    // Preconditions:
    // Format: 1 sf 1110 0U0L offset xn rt
    // rt -> target register
    // xn -> base register (stores address)
    setFlags(instruction);
    ADDR addr = getAddress(instruction);
    printf("Address at %d\n", addr);
    loadStoreHandler(addr, instruction);
    return true; // errors
}
bool loadLiteralHandler(INST instruction){
    int64_t simm19 = sign_extend(extractBits(instruction, 5, 23), 19);
    ADDR addr = programCounter + simm19 * 4;
    loadStoreHandler(addr, instruction);

    return true; // errors
}

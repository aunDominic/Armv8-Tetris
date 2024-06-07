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
#define isRegisterOffset(instruction) ((extractBits(instruction, 10, 15) == 26) && (extractBits(instruction, 21, 21) == 1))
#define isIndexed(instruction) ((extractBits(instruction, 10, 10) == 1) && (extractBits(instruction, 21, 21) == 0))
static int64_t getAddress(INST instruction){
    int64_t XN = extractBits(instruction, 5, 9);
    int32_t XM = extractBits(instruction, 16, 20);
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
    if (isIndexed(instruction)){
        if (I){
            return registers[XN] += simm9;
        } else {
            int64_t ret = registers[XN];
            registers[XN] += simm9;
            return ret;
        }
    }
    if (isRegisterOffset(instruction)){
        return registers[XN] + registers[XM];
    }
}
static void loadStoreHandler(ADDR addr, INST instruction){
    int rt = (int) extractBits(instruction, 0, 4);
    printf("rt: %d\n", rt);
    int n_addr;
    if (SF) n_addr = 8;
    else n_addr = 4;
    if (L){
        printf("Loading from memory...\n", L);
        int64_t bytes = 0;
        for (int i = 0; i < n_addr; i++){
            printf("Memory[%d]: %x\n", addr + i, memory[addr + i]);
            int64_t byte = (int64_t) memory[addr + i] << (i * 8);
            printf("Loading byte into correct positions: %llx. ", byte);
            bytes |= byte;
        }
        registers[rt] = bytes;
    } else { // store
        printf("Storing into memory.\n");

        for (int i = 0; i < n_addr; i++){
            memory[addr + i] = (BYTE) extractBits(registers[rt], i * 8, (i+1) * 8 - 1);
        }
    }
}
bool single_data_transfer_handler(INST instruction){
    // Preconditions:
    // Format: 1 sf 1110 0U0L offset xn rt
    // rt -> target register
    // xn -> base register (stores address)
    printf("SDT called.\n");
    setFlags(instruction);
    ADDR addr = getAddress(instruction);
    assert((addr <= memSize - 4) && "Error: Memory out of bounds");
    printf("Address at %d\n", addr);
    loadStoreHandler(addr, instruction);
    return true; // errors
}
bool load_literal_handler(INST instruction){
    printf("Load Literal called.\n");
    setFlags(instruction);
    int rt = (int) extractBits(instruction, 0, 4);
    int64_t simm19 = sign_extend(extractBits(instruction, 5, 23), 19);
    ADDR addr = programCounter + simm19 * 4;
    assert((addr <= memSize - 4) && "Error: Memory out of bounds");
    L = true;
    loadStoreHandler(addr, instruction);
    return true; // errors
}
//int main(){
//    printf ("%lld\n", binaryToDecimal("111"));
//    registers[3] = 1; // third register holds 1
//    registers[4] = 5;
//    // 64 bit, immediate offset 123, from register 3, taken the data from register 3
//    memory[988] = 1;
//    INST uload64 = SDTconstructer(1,1,1, 123, 3, 4);
//    // Desired effect: memory[R3 + 123 * 8] = 5
//
//    single_data_transfer_handler(uload64);
//    printMemory(970, 990);
//    printRegisters();
//
//}

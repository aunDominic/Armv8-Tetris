//
// Created by Dominic Ng on 25/05/2024.
//

#include "datatransfer.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitmanipulation.h"

const uint32_t MAX_OFFSET = 1 << 12;
typedef uint32_t ADDR;

bool U;   // Unsigned Immediate Flag
bool I;   // Pre/Post Indexed Flag
bool L;   // Load-Store Flag
bool SF;  // Size of load, (32/64 bits)

/*
 *
 * @params: INST instruction
 * Sets the load store instruction flags
 * State Changes: U, I, L, SF
 */
static void setFlags(INST instruction) {
    U = 1 << 24 & instruction;
    I = 1 << 11 & instruction;
    L = 1 << 22 & instruction;
    SF = 1 << 30 & instruction;
}

#define isRegisterOffset(instruction) \
    ((extractBits((instruction), 10, 15) == 26) && (extractBits((instruction), 21, 21) == 1))
#define isIndexed(instruction) ((extractBits((instruction), 10, 10) == 1) && (extractBits((instruction), 21, 21) == 0))

/*
 *  PRE: A valid instruction is supplied, ie: an addressing mode is found in the instruction bits
 *  Returns the address to load-store depending on the addressing mode
 *  Addressing modes supported are:
 *  Unsigned immediate, Pre/Post indexed, Register offset, Load Literal
 */
static int64_t getAddress(INST instruction) {
    int64_t XN = extractBits(instruction, 5, 9);
    int32_t XM = extractBits(instruction, 16, 20);
    int64_t simm9 = sign_extend(extractBits(instruction, 12, 20), 9);

    // Assumes that one of the clauses will hold.
    assert(U || isIndexed(instruction) || isRegisterOffset(instruction));
    if (U) {
        int64_t uoffset = extractBits(instruction, 10, 21);
        if (SF)
            uoffset *= 8;
        else
            uoffset *= 4;

        return registers[XN] + uoffset;
    }
    if (isIndexed(instruction)) {
        if (I) {
            return registers[XN] += simm9;
        } else {
            int64_t ret = registers[XN];
            registers[XN] += simm9;
            return ret;
        }
    }
    if (isRegisterOffset(instruction)) {
        return registers[XN] + registers[XM];
    }

    perror("Invalid instruction passed in getAddress or something else\n");
    exit(EXIT_FAILURE);
}

/*
 * @param addr - An address within memory bounds
 * @param instruction - An valid instruction that can be parsed
 * POST: Performs the load/store operation using the given address, depending on the target register (32/64 bits)
 */
static void loadStoreHandler(ADDR addr, INST instruction) {
    int rt = (int)extractBits(instruction, 0, 4);
    int n_addr;
    // Sets the number of bytes to load/store depending on the target register.
    if (SF)
        n_addr = 8;
    else
        n_addr = 4;
    if (L) {
        // Loading from memory...
        int64_t bytes = 0;
        for (int i = 0; i < n_addr; i++) {
            int64_t byte = (int64_t)memory[addr + i] << (i * 8);
            bytes |= byte;
        }
        registers[rt] = bytes;
    } else {
        // Storing into memory...
        for (int i = 0; i < n_addr; i++) {
            memory[addr + i] = (BYTE)extractBits(registers[rt], i * 8, (i + 1) * 8 - 1);
        }
    }
}

/*
 * PRE: instruction bits has the format: 1 SF 1 1 1 0 0 U 0 L OFFSET XN RT
 * SF -> Size of load, specifies target register as 32 or 64 bits
 * U -> Unsigned offset flag, specfies the addressing mode as unsigned offset.
 * OFFSET -> Defines the offset in the addressing mode. Value of offset is altered based on addressing mode.
 * XN -> Base register containing the relevant address to load/store from.
 * RT -> Target register that holds the data to load/store from
 * POST: Performs the Single Data Transfer instruction as specified by AArch64
 */
bool singleDataTransferHandler(INST instruction) {
    setFlags(instruction);
    ADDR addr = getAddress(instruction);
    assert((addr <= memSize - 4) && "Error: Memory out of bounds");
    loadStoreHandler(addr, instruction);
    return true;  // errors
}

/*
 * PRE: instruction bits has the format: 0 SF 0 1 1 0 0 0 SIMM19 RT
 * SF -> Size of load, specifies target register as 32 or 64 bits
 * SIMM19 -> 19 bit offset from the PC.
 * RT -> Target register that holds the data to load/store from
 * POST: Performs the Load Literal instruction as specified by AArch64
 */
bool loadLiteralHandler(INST instruction) {
    setFlags(instruction);
    int64_t simm19 = sign_extend(extractBits(instruction, 5, 23), 19);
    ADDR addr = programCounter + simm19 * 4;
    assert((addr <= memSize - 4) && "Error: Memory out of bounds");  // this is a trick in C
    L = true;
    loadStoreHandler(addr, instruction);
    return true;  // errors
}

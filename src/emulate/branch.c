//
// Created by Dominic Ng on 25/05/2024.
//

#include "branch.h"
#include "bitmanipulation.h"
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <assert.h>

/*
 * Updates the PSTATE according to the condition type
 * @param A conditional branch instruction containing the condition type
 */
typedef enum{
    EQ = 0b0000,
    NE = 0b0001,
    GE = 0b1010,
    LT = 0b1011,
    GT = 0b1100,
    LE = 0b1101,
    AL = 0b1110
} CondType;
static bool pstateHandler(int64_t cond){
    int cond_type = extractBits(cond, 0,3);
    if      (cond_type == EQ) return pstate.Z == 1;
    else if (cond_type == NE) return pstate.Z == 0;
    else if (cond_type == GE) return pstate.N == pstate.V;
    else if (cond_type == LT) return pstate.N != pstate.V;
    else if (cond_type == GT) return pstate.Z == 0 && pstate.N == pstate.V;
    else if (cond_type == LE) return !(pstate.Z == 0 && pstate.N == pstate.V);
    else if (cond_type == AL) return true;
    else return false; // not a valid condition type
}

/*
 * Performs branch instructions as specified by AArch64
 */
#define is_unconditional(instruction) (extractBits(instruction, 26, 31) == 0b000101)
#define is_register(instruction) (extractBits(inst, 10, 31) == 0b1101011000011111000000 && extractBits(inst, 0, 4) == 0)
#define is_conditional(instruction) (extractBits(inst, 24, 31) == 0b01010100 && extractBits(inst, 4,4) == 0)
bool branchHandler(INST inst){
    const int64_t zero = 0b11111;

    assert (is_unconditional(inst) || is_conditional(inst) || is_register(inst));
    if (is_unconditional(inst)){
        int64_t simm26 = sign_extend(extractBits(inst, 0, 25), 26);
        programCounter += simm26 * 4;
    }
    else if (is_register(inst)){
        int64_t xn = extractBits(inst, 5, 9);
        if (xn == zero) return true;
        programCounter = registers[xn];
    }
    else if (is_conditional(inst)){
        int64_t simm19 = sign_extend(extractBits(inst, 5, 23), 19);
        int64_t cond = extractBits(inst, 0, 3);
        if (pstateHandler(cond)) programCounter += simm19 * 4;
        else programCounter += 4;
    }
    else {
        // Not a valid branch instruction;
        return false;
    }
    return true;
}
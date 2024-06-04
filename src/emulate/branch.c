//
// Created by Dominic Ng on 25/05/2024.
//

#include "branch.h"
#include "bitmanipulation.h"
#include <string.h>
#include <memory.h>
#include <stdio.h>
static bool pstateHandler(int64_t cond){
    char byte[5];
    byte[4] = '\0';
    for (int i = 3; i >= 0; i--){
        byte[3 - i] = (1 << i & cond) ? '1' : '0';
    }
    printf("Condition: %s\n", byte);
    printf("ZF: %d\n", pstate.Z);
    if (strcmp(byte, "0000") == 0) return pstate.Z == 1;
    else if (strcmp(byte, "0001") == 0) return pstate.Z == 0;
    else if (strcmp(byte, "1010") == 0) return pstate.N == pstate.V;
    else if (strcmp(byte, "1011") == 0) return pstate.N != pstate.V;
    else if (strcmp(byte, "1100") == 0) return pstate.Z == 0 && pstate.N == pstate.V;
    else if (strcmp(byte, "1101") == 0) return !(pstate.Z == 0 && pstate.N == pstate.V);
    else if (strcmp(byte, "1110") == 0) return true;
    else return false; // not a valid encoding
}
bool branchHandler(INST inst){
    printf("Branch instruction called...\n");
    const int64_t isRegister = binaryToDecimal("1101011000011111000000");
    const int64_t isUnconditional = binaryToDecimal("000101");
    const int64_t isConditional = binaryToDecimal("01010100");
    const int64_t zero = binaryToDecimal("11111");
    if (extractBits(inst, 26, 31) == isUnconditional){
        printf("Uncondtional branch!\n");
        int64_t simm26 = sign_extend(extractBits(inst, 0, 25), 26);
        programCounter += simm26 * 4;
    } else if (extractBits(inst, 10, 31) == isRegister
        && extractBits(inst, 0, 4) == 0){
        printf("Register branch!\n");
        int64_t xn = extractBits(inst, 5, 9);
        if (xn == zero) return true;
        programCounter = registers[xn];
    } else if (extractBits(inst, 24, 31) == isConditional && extractBits(inst, 4,4) == 0){
        printf("Conditional branch!\n");
        int64_t simm19 = sign_extend(extractBits(inst, 5, 23), 19);
        printf("Offset: %llx\n", simm19 * 4);
        int64_t cond = extractBits(inst, 0, 3);
        if (pstateHandler(cond)) programCounter += simm19 * 4;
        else programCounter += 4;
    } else {
        // Not a valid branch instruction;
        return false;
    }
    return true;
}
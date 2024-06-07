//
// Created by Ahmad Jamsari on 31/05/24.
//

#include "branch_assembly.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "symbol_table.h"
#include "parsing_common.h"

// these are done according to section 1.8
static uint8_t cond_encoding(const Opcode bOpcode) {
    switch (bOpcode) {
        case B_EQ:
            return 0;
        case B_NE:
            return 1;
        case B_GE:
            return 10;
        case B_LT:
            return 11;
        case B_GT:
            return 12;
        case B_LE:
            return 13;
        case B_AL:
            return 14;
        default:
            perror("condition opc in immediate handling in condEncoding triggered");
            exit(EXIT_FAILURE);
    }
}

INST b_inst(const char *remainingLine, const uint32_t address) {
    INST instr = 0;

    // literally just a literal remaining which is a label or an integer
    while (isspace(*remainingLine)) {
        remainingLine++;
    }

    int32_t imm;
    if (*remainingLine == '#') {
        imm = strtol(remainingLine + 1, NULL, 0);
    } else {
        // must be a label
        imm = getValue(symbol_table, remainingLine);
    }

    int32_t offset = (imm - (int32_t) address) / 4;

    instr = modify_instruction(instr, 26, 31, 5);
    instr = modify_instruction(instr, 0, 25, offset);

    return instr;
}

// I can definitely refactor this but later on
INST b_cond(const char *remainingLine, const uint32_t address, const Opcode bOpCode) {
    INST instr = 0;

    // literally just a literal remaining which is a label or an integer
    while (isspace(*remainingLine)) {
        remainingLine++;
    }

    int32_t imm;
    if (*remainingLine == '#') {
        imm = strtol(remainingLine + 1, NULL, 0);
    } else {
        // must be a label
        imm = getValue(symbol_table, remainingLine);
    }

    int32_t offset = (imm - (int32_t) address) / 4;

    instr = modify_instruction(instr, 24, 31, 0x54); // 0101 0100
    instr = modify_instruction(instr, 5, 23, offset);

    uint8_t encoding = cond_encoding(bOpCode);

    instr = modify_instruction(instr, 0, 3, encoding);

    return instr;

}
INST br_inst(char *remainingLine, uint32_t address) {
    INST instr = 0;
    Register reg1 = handle_register(&remainingLine);

    instr = modify_instruction(instr, 25, 31, 0x6b); // magic ish value
    instr = modify_instruction(instr, 16, 20, 31); // magic ish value
    instr = modify_instruction(instr, 5, 9, reg_to_binary(reg1));
    return instr;
}
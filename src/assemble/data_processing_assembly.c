//
// Created by Ahmad Jamsari on 31/05/24.
//

#include "data_processing_assembly.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parsing_common.h"

static uint8_t get_opc_arithmetic(Opcode opcode) {
    switch (opcode) {
        case ADD: return 0;
        case ADDS: return 1;
        case SUB: return 2;
        case SUBS: return 3;
        default: perror("condition opc in immediate handling in get_opc_arithmetic triggered\n"); exit(EXIT_FAILURE);
    }
}

static uint8_t get_opc_bitwise(Opcode opcode) {
    switch (opcode) {
        case AND:
        case BIC: return 0;
        case ORR:
        case ORN: return 1;
        case EOR:
        case EON: return 2;
        case ANDS:
        case BICS: return 3;
        default: perror("condition opc in immediate handling in get_opc_bitwise triggered\n"); exit(EXIT_FAILURE);
    }
}

static uint8_t get_opc_mov(Opcode opcode) {
    switch (opcode) {
        case MOVN: return 0;
        case MOVZ: return 2;
        case MOVK: return 3;
        default: perror("condition opc in immediate handling in get_opc_mov triggered\n"); exit(EXIT_FAILURE);
    }
}

INST two_op_inst(char *remainingLine, Opcode opcode) {
    // general steps are read the next 2 registers
    // do smth with the registers
    // then determine if its immediate

    INST instr = 0;

    Register reg1 = handle_register(&remainingLine);
    Register reg2 = handle_register(&remainingLine);

    // this common idiom could be a function
    // but would have to return a pointer or have char **
    // which is slightly awkward to use
    while (isspace((unsigned char)*remainingLine)) {
        remainingLine++;
    }

    // do stuff common to both of them
    modify_instruction(&instr, 31, 31, reg1.is64Mode);  // set 31 bit
    modify_instruction(&instr, 0, 4, reg_to_binary(reg1));
    modify_instruction(&instr, 5, 9, reg_to_binary(reg2));

    // now determine if its immediate or another register
    // even though the spec doesn't specify, im gonna assume immediate's can also be specified in hex
    assert((*remainingLine == '#') || (*remainingLine == 'w') || (*remainingLine == 'x'));

    if (*remainingLine == '#') {
        // this handles DP (immediate)
        char *rest;
        uint32_t imm = strtoul(remainingLine + 1, &rest, 0);
        // guaranteed that we're in ADD, ADDS, SUB, SUBS land: since these are the only
        // ones with immediate operations at this stage
        uint8_t opc = get_opc_arithmetic(opcode);

        modify_instruction(&instr, 29, 30, opc);
        modify_instruction(&instr, 26, 28, 4);  // 0b100 decimal equivalent
        modify_instruction(&instr, 23, 25, 2);  // 0b010 equivalent
        modify_instruction(&instr, 10, 21, imm);

        if (*rest == NULL_CHAR) {
            return instr;
        }

        // there's optional things to handle
        // ie handle "lsl #<imm>#
        uint32_t num;
        sscanf(rest, ", lsl #%u", &num);

        if (num == 12) {
            modify_instruction(&instr, 22, 22, 1);
            return instr;
        }

        // honestly this case is only reached if we have lsl #0 which is useless
        // perhaps some compiler might generate this though
        return instr;
    }

    // we know its a register type instruction due to early return from other case
    Register reg3 = handle_register(&remainingLine);
    modify_instruction(&instr, 16, 20, reg_to_binary(reg3));
    // assumes comma is already handled by handle_register
    // determine shift type

    Shifter shifter = determineShift(remainingLine);
    // from line_reader.c we know that its arithmetic or bit-logic hence M (bit 28) stays 0
    modify_instruction(&instr, 25, 27, 5);  // 101, common for both

    // determine if its bitwise or arithmetic now
    uint8_t opc;
    OpcodeType opcodeType = get_opcode_type(opcode);

    // Can't refactor this properly due to MULTIPLY.
    // There are some common elements between ARITHMETIC and BITWISE though
    // ie bits 10-15 with shiftAmount and shiftType
    // but i just wanted to be explicit rather than things be out of cases
    if (opcodeType.type == ARITHMETIC) {
        opc = get_opc_arithmetic(opcode);
        modify_instruction(&instr, 29, 30, opc);
        modify_instruction(&instr, 24, 24, 1);
        modify_instruction(&instr, 22, 23, shifter.shiftType);
        // leave bit 21 the same as nothing changes from 0
        modify_instruction(&instr, 10, 15, shifter.shiftAmount);
    } else if (opcodeType.type == BITWISE) {
        opc = get_opc_bitwise(opcode);
        modify_instruction(&instr, 29, 30, opc);
        // leave bit 24 the same ie at 0
        modify_instruction(&instr, 22, 23, shifter.shiftType);
        modify_instruction(&instr, 21, 21, opcodeType.isNegated);
        modify_instruction(&instr, 10, 15, shifter.shiftAmount);
    } else if (opcodeType.type == MULTIPLY) {
        // opc = 0; // is 0 for multiplication
        // modify_instruction(&instr, 29, 30, opc); // opc, is unnecessary
        modify_instruction(&instr, 28, 28, 1);   // M
        modify_instruction(&instr, 21, 24, 8);   // opr for multiplication
        modify_instruction(&instr, 10, 14, 31);  // set to zero register
        modify_instruction(&instr, 15, 15, opcodeType.isNegated);
    }

    return instr;
}

INST multiply_inst(char *remainingLine, Opcode opcode) {
    INST instr = 0;
    Register rd = handle_register(&remainingLine);
    Register rn = handle_register(&remainingLine);
    Register rm = handle_register(&remainingLine);
    Register ra = handle_register(&remainingLine);

    modify_instruction(&instr, 31, 31, rd.is64Mode);  // set 31 bit
    modify_instruction(&instr, 21, 30, 0xd8);         // 00 1101 1000
    modify_instruction(&instr, 16, 20, reg_to_binary(rm));
    int x = 0;
    switch (opcode) {
        case MADD: x = 0; break;
        case MSUB: x = 1; break;
        default: perror("Error in multiply_inst, invalid opcode\n"); exit(EXIT_FAILURE);
    }

    modify_instruction(&instr, 15, 15, x);
    modify_instruction(&instr, 10, 14, reg_to_binary(ra));
    modify_instruction(&instr, 5, 9, reg_to_binary(rn));
    modify_instruction(&instr, 0, 4, reg_to_binary(rd));

    return instr;
}

INST single_op_inst_alias(char *remainingLine, Opcode opcode) {
    // handle cases due to aliases, this is done by transforming the string
    transform_middle(remainingLine);
    switch (opcode) {
        case MOV: return two_op_inst(remainingLine, ORR);
        case NEG: return two_op_inst(remainingLine, SUB);
        case NEGS: return two_op_inst(remainingLine, SUBS);
        case MVN: return two_op_inst(remainingLine, ORN);
        default: perror("Error in single_op_inst_alias call. Invalid arguments\n"); exit(EXIT_FAILURE);
    }
}

// honestly this has so much in common with the immediate part of two_op_inst
// However I like separating the function to separate out responsibility
INST wide_move_inst(char *remainingLine, Opcode opcode) {
    INST instr = 0;
    Register rd = handle_register(&remainingLine);

    while (isspace((unsigned char)*remainingLine)) {
        remainingLine++;
    }

    char *rest;

    // remainingLine starts with '#' so skip one over
    uint32_t imm = strtoul(remainingLine + 1, &rest, 0);

    // PRINT("Imm value is %d\n", imm);

    modify_instruction(&instr, 31, 31, rd.is64Mode);  // set 31 bit
    uint8_t opc = get_opc_mov(opcode);

    modify_instruction(&instr, 29, 30, opc);
    modify_instruction(&instr, 26, 28, 4);  // common immediate stuff
    modify_instruction(&instr, 23, 25, 5);  // opi

    // PRINT("wide_mov_inst immediate value %d\n", imm);
    modify_instruction(&instr, 5, 20, imm);
    modify_instruction(&instr, 0, 4, reg_to_binary(rd));

    // now handle optional operand
    if (*rest == NULL_CHAR) {
        return instr;
    }

    uint32_t num;
    sscanf(rest, ", lsl #%u", &num);  // num should be a multiple of 16
    modify_instruction(&instr, 21, 22, num / 16);
    return instr;
}

INST two_op_nodest_inst(char *remainingLine, Opcode opcode) {
    // handle cases due to aliases, does this by modifying the remainingLine string.
    transform_start(remainingLine);
    switch (opcode) {
        case CMP: return two_op_inst(remainingLine, SUBS);
        case CMN: return two_op_inst(remainingLine, ADDS);
        case TST: return two_op_inst(remainingLine, ANDS);
        default: perror("Error in single_op_inst_alias call. Invalid arguments"); exit(EXIT_FAILURE);
    }
}
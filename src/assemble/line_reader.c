//
// Created by Ahmad Jamsari on 30/05/24.
//

#include "line_reader.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "branch_assembly.h"
#include "common_types.h"
#include "data_processing_assembly.h"
#include "directives.h"
#include "loadstore.h"
#include "opcode_table.h"
#include "../debug.h"

#define DEFAULT_RETURN_VAL 128

bool is_label(const char *line) {
    int i = 0;

    // Skip leading whitespace
    while (isspace((unsigned char)line[i])) {
        i++;
    }

    // Check if the first character is a letter, underscore, or period
    if (!isalpha((unsigned char)line[i]) && line[i] != '_' && line[i] != '.') {
        return false;
    }
    i++;  // technically could overflow but in rare circumstances

    // Check the rest of the characters
    while (isalnum((unsigned char)line[i]) || line[i] == '$' || line[i] == '_' || line[i] == '.') {
        i++;
    }

    // Check if the next character is a colon
    return (line[i] == ':');
}

// there's a case for lineHandler to not take an address and instead the address should be global
// reduces clarity but might be simpler
// this is basically the function to edit when adding more instruction support
// address is instructoin's current address
// currently only needed for branch + load/str instructions
INST lineHandler(char *line, uint32_t address) {
    // first step: get opcode first
    char str_opcode[MAX_LENGTH];
    char *remainingLine;  // used the store the remaining unparsed string
    const char *token = strtok_r(line, " ", &remainingLine);
    strcpy(str_opcode, token);

    Opcode opcode = getValue(opcodeTable, str_opcode);
    // uses an opcode table using the symbol_table ADT

    PRINT("Opcode is %d. Remaining string: %s\n", opcode, remainingLine);

    // now match appropiate opcode to a function, can do switch or function pointers
    // I don't want to populate a huge array again so I'm sticking to massive switch
    INST returnVal = DEFAULT_RETURN_VAL;  // default value for debugging
    switch (opcode) {
        case MADD:
        case MSUB: returnVal = multiply_inst(remainingLine, opcode); break;

        case ADD:
        case ADDS:
        case SUB:
        case SUBS:
        case AND:
        case ANDS:
        case BIC:
        case BICS:
        case EOR:
        case ORR:
        case EON:
        case ORN:
        case MUL:
        case MNEG: returnVal = two_op_inst(remainingLine, opcode); break;

        case MOV:
        case NEG:
        case NEGS:
        case MVN:
            // these are aliases for two_op_inst
            returnVal = single_op_inst_alias(remainingLine, opcode);
            break;

        case MOVZ:
        case MOVN:
        case MOVK: returnVal = wide_move_inst(remainingLine, opcode); break;

        case CMP:
        case CMN:
        case TST:
            // these are also aliases for two_op_inst
            returnVal = two_op_nodest_inst(remainingLine, opcode);
            break;

        case B: returnVal = b_inst(remainingLine, address); break;

        case B_EQ:
        case B_NE:
        case B_GE:
        case B_LT:
        case B_GT:
        case B_LE:
        case B_AL: returnVal = b_cond(remainingLine, address, opcode); break;

        case BR: returnVal = br_inst(remainingLine); break;

        case STR:
        case LDR: returnVal = strload_inst(remainingLine, address, opcode); break;

        case SPECIAL_INT: returnVal = int_directive(remainingLine); break;

        default: perror("THIS SHOULDN'T HAPPEN FOR NORMAL USES"); exit(EXIT_FAILURE);
    }

    assert(returnVal != DEFAULT_RETURN_VAL);
    return returnVal;
}

//
// Created by Ahmad Jamsari on 30/05/24.
//

#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "line_reader.h"

#include <stdlib.h>

#include "opcode_table.h"
#include "branch_assembly.h"
#include "common_types.h"
#include "data_processing_assembly.h"
#include "directives.h"
#include "loadstore.h"

bool is_label(const char *line) {
    int i = 0;

    // Skip leading whitespace
    while (isspace(line[i])) {
        i++;
    }

    // Check if the first character is a letter, underscore, or period
    if (!isalpha(line[i]) && line[i] != '_' && line[i] != '.') {
        return false;
    }
    i++;

    // Check the rest of the characters
    while (isalnum(line[i]) || line[i] == '$' || line[i] == '_' || line[i] == '.') {
        i++;
    }

    // Check if the next character is a colon
    return (line[i] == ':');
}

// there's a case for lineHandler to not take an address and instead the address should be global
// reduces clarity but might be simpler
INST lineHandler(char *line, uint32_t address) {

    // first step: get opcode first
    char str_opcode[MAX_LENGTH];
    char *remainingLine; // used the store the remaining unparsed string
    char *token = strtok_r(line, " ", &remainingLine);
    strcpy(str_opcode, token);

    Opcode opcode = getValue(opcodeTable, str_opcode);

    printf("Opcode is %d. Remaining string: %s\n", opcode, remainingLine);

    // now match appropiate opcode to a function, can do switch or function pointers
    // I don't want to populate a huge array again so I'm sticking to massive switch
    INST returnVal = 128; // default value for debugging
    switch (opcode) {
        case MADD: case MSUB:
            returnVal = multiply_inst(remainingLine, address, opcode);
            break;

        case ADD: case ADDS: case SUB: case SUBS:
        case AND: case ANDS: case BIC: case BICS:
        case EOR: case ORR: case EON: case ORN:
        case MUL: case MNEG:
            returnVal = two_op_inst(remainingLine, address, opcode);
            break;

        case MOV: case NEG: case NEGS: case MVN:
            // these are aliases
            // so just transform them into the two_op_inst
            returnVal = single_op_inst_alias(remainingLine, address, opcode);
            break;

        case MOVZ: case MOVN: case MOVK:
            returnVal = wide_move_inst(remainingLine, address, opcode);
            break;

        case CMP: case CMN: case TST:
            // basically all we have to do is transform them using the zero register to a two_op
            returnVal = two_op_nodest_inst(remainingLine, address, opcode);
            break;

        case B:
            returnVal = b_inst(remainingLine, address);
            break;

        case B_EQ: case B_NE: case B_GE: case B_LT: case B_GT: case B_LE: case B_AL:
            returnVal = b_cond(remainingLine, address, opcode);
            break;

        case BR:
            returnVal = br_inst(remainingLine, address);
            break;

        case STR: case LDR:
            returnVal = strload_inst(remainingLine, address, opcode);
            break;

        case SPECIAL_INT:
            returnVal = int_directive(remainingLine, address);
            break;

        default:
            perror("THIS SHOULDN'T HAPPEN FOR NORMAL USES");
            exit(EXIT_FAILURE);
    }

    return returnVal;

}

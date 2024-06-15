//
// Created by Ahmad Jamsari on 31/05/24.
//

#include "loadstore.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "parsing_common.h"
#include "symbol_table.h"
#include <string.h>

typedef enum {
    POST_INDEX,
    PRE_INDEX,
    UNSIGNED_OFFSET,
    REGISTER_OFFSET,
    LITERAL,
    INVALID
} AddressingMode;

// slightly inefficient, could use unions
// but storage is abundant so take advantage of it
// structs are easier to work with
typedef struct {
    AddressingMode mode;
    Register Rt;
    Register Xn_SP;
    int32_t simm;
    int32_t imm;
    Register Xm;
    int32_t literal;
} ParsedAddress;

// this exists because handle_register skips over the character after the register
// ie given "x30,a", handle _register will leave the line at 'a' while
// this will leave the ',' (this is only needed in this one file)
static Register handle_reg_inplace(char **strReg) {
    Register reg;
    if (**strReg == 'x') {
        reg.is64Mode = true;
    } else if (**strReg == 'w') {
        reg.is64Mode = false;
    } else {
        fprintf(stderr, "%s\n", *strReg);
        perror("issue for handle_reg_inplace receiving invalid input");
        exit(EXIT_FAILURE);
    }

    // now check for zero register
    if (strcmp(*strReg + 1, "zr") == 0) {
        reg.isZeroReg = true;
        reg.regNumber = 255; // set a value for debugging purposes
        return reg;
    }

    reg.isZeroReg = false;
    reg.regNumber = strtoul(*strReg + 1, strReg, 0);
    return reg;
}

// honestly could've made this just return a ParsedAddress instead of a pointer to one
// I think either one is fine
void parse_addressing_mode(char *remainingLine, ParsedAddress *parsed) {
    // Initialize ParsedAddress
    parsed->simm = 0;
    parsed->imm = 0;
    parsed->literal = 0;

    // Parse Rt
    parsed->Rt = handle_register(&remainingLine);

    // printf("remaining line 1 is %s\n", remainingLine);

    while (isspace((unsigned char) *remainingLine)) {
        remainingLine++;
    }

    // Check for immediate literal addressing mode
    if (*remainingLine == '#') {
        parsed->mode = LITERAL;
        parsed->literal = atoi(remainingLine + 1);
        return;
    }

    // labels are handled here
    int32_t imm = getValue(symbol_table, remainingLine);
    if (imm != NOT_FOUND) {
        // we know its a label
        parsed->literal = imm;
        parsed->mode = LITERAL;
        return;
    }

    // not a label anymore

    // Skip the opening bracket '['
    if (*remainingLine == '[') {
        remainingLine++;
    }

    // Parse Xn|SP
    parsed->Xn_SP = handle_reg_inplace(&remainingLine);
    // printf("remaining line is %s\n", remainingLine);

    // Check for closing bracket ']' for POST_INDEX
    if (*remainingLine == ']' && *(remainingLine + 1) == ',') {
        remainingLine += 2;

        // handle rest of post_index

        while (isspace((unsigned char) *remainingLine) || *remainingLine == '#') {
            remainingLine++;
        }

        parsed->mode = POST_INDEX;
        parsed->simm = strtol(remainingLine, NULL, 0);
        return;
    }

    // check for optional unsigned offset part
    if (*remainingLine == ']') {
        parsed->imm = 0;
        parsed->mode = UNSIGNED_OFFSET;
        return;
    }

    // Check for unsigned offset now vs register offset vs pre-index
    if (*remainingLine == ',') {
        remainingLine++;
        while (isspace((unsigned char) *remainingLine)) {
            remainingLine++;
        }

        if (*remainingLine == '#') {
            char *rest;
            int32_t value = strtol(remainingLine + 1, &rest, 0);
            // value can be either for pre-index or unsigned offset
            // now determine if its pre-index or unsigned offset

            // rest should point to ']' now
            if (*(rest + 1) == '!') {
                parsed->mode = PRE_INDEX;
                parsed->simm = value;
                return;
            }

            // now we know its unsigned offset
            parsed->mode = UNSIGNED_OFFSET;
            parsed->imm = value;
            return;
        } else {
            parsed->mode = REGISTER_OFFSET;
            parsed->Xm = handle_register(&remainingLine);
            return;
        }
    }
    parsed->mode = INVALID;
    return;
}

// handles both str and load in one go as they have so much in common
INST strload_inst(char *remainingLine, uint32_t address, Opcode opcode) {
    INST instr = 0;

    ParsedAddress parsed;
    parse_addressing_mode(remainingLine, &parsed);

    modify_instruction(&instr, 30, 30, parsed.Rt.is64Mode);
    modify_instruction(&instr, 0, 4, reg_to_binary(parsed.Rt));
    if (parsed.mode == LITERAL) {
        // do literal handling
        modify_instruction(&instr, 31, 31, 0);
        modify_instruction(&instr, 27, 29, 3);
        // handle simm19 stuff

        printf("parsed.literal is %d\n", parsed.literal);
        int32_t offset = (parsed.literal - (int32_t) address) / 4;
        modify_instruction(&instr, 5, 23, offset);
    } else {
        // one of the other modes that we need to handle

        // do common things
        modify_instruction(&instr, 31, 31, 1);
        modify_instruction(&instr, 25, 29, 0x1C);
        modify_instruction(&instr, 24, 24, parsed.mode == UNSIGNED_OFFSET ? 1 : 0);
        modify_instruction(&instr, 22, 22, opcode == LDR ? 1 : 0);

        modify_instruction(&instr, 5, 9, reg_to_binary(parsed.Xn_SP));

        // now handle the different cases based on addressing modes
        switch (parsed.mode) {
            case REGISTER_OFFSET:
                modify_instruction(&instr, 21, 21, 1);
                modify_instruction(&instr, 16, 20, reg_to_binary(parsed.Xm));
                modify_instruction(&instr, 10, 15, 0x1a);
                break;
            case PRE_INDEX: case POST_INDEX:
                modify_instruction(&instr, 12, 20, parsed.simm);
                modify_instruction(&instr, 11, 11, parsed.mode == PRE_INDEX ? 1 : 0);
                modify_instruction(&instr, 10, 10, 1);
                break;
            case UNSIGNED_OFFSET:
                int32_t imm12 = parsed.Rt.is64Mode ? parsed.imm / 8 : parsed.imm / 4;
                // printf("imm12 for unsigned offset is %d\n", imm12);
                modify_instruction(&instr, 10, 21, imm12);
                break;
            default:
                fprintf(stderr, "mode is %d\n", parsed.mode);
                perror("Error in strload_inst, invalid mode\n");
                exit(EXIT_FAILURE);
        }
    }

    return instr;
}
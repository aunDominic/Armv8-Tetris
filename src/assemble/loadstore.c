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

typedef struct {
    AddressingMode mode;
    Register Rt;
    Register Xn_SP;
    int32_t simm;
    int32_t imm;
    Register Xm;
    int32_t literal;
} ParsedAddress;

static Register handle_reg_inplace(char **strReg) {
    Register reg;
    if (**strReg == 'x') {
        reg.is64Mode = true;
    } else if (**strReg == 'w') {
        reg.is64Mode = false;
    } else {
        fprintf(stderr, "%s\n", *strReg);
        perror("issue for reg_from_regStr receiving invalid input");
        exit(EXIT_FAILURE);
    }

    // now check for zero register
    if (strcmp(*(strReg) + 1, "zr") == 0) {
        reg.isZeroReg = true;
        reg.regNumber = 255; // set a value for debugging purposes
        return reg;
    }

    reg.isZeroReg = false;
    reg.regNumber = strtoul(*strReg + 1, strReg, 0);
    return reg;
}

AddressingMode parse_addressing_mode(const char *line, ParsedAddress *parsed) {
    char *remainingLine = strdup(line); // Duplicate the line to avoid modifying the original
    char *originalLine = remainingLine; // Keep a pointer to free later

    // Initialize ParsedAddress
    parsed->simm = 0;
    parsed->imm = 0;
    parsed->literal = 0;

    // Parse Rt
    parsed->Rt = handle_register(&remainingLine);

    printf("remaining line 1 is %s\n", remainingLine);

    while (isspace((unsigned char) *remainingLine)) {
        remainingLine++;
    }

    // Check for immediate literal addressing mode
    if (*remainingLine == '#') {
        parsed->mode = LITERAL;
        parsed->literal = atoi(remainingLine + 1);
        free(originalLine);
        return LITERAL;
    }

    // labels are handled here
    int32_t imm = getValue(symbol_table, remainingLine);
    if (imm != NOT_FOUND) {
        parsed->literal = imm;
        parsed->mode = LITERAL;
        return LITERAL;
    }


    // Skip any whitespace and the opening bracket '['
    while (isspace((unsigned char) *remainingLine) || *remainingLine == '[') {
        remainingLine++;
    }

    // Parse Xn|SP
    parsed->Xn_SP = handle_reg_inplace(&remainingLine);
    printf("remaining line is %s\n", remainingLine);
    // Check for closing bracket ']' for POST_INDEX
    if (*remainingLine == ']' && *(remainingLine + 1) == ',') {
        remainingLine += 2;

        // handle rest of post_index

        while (isspace((unsigned char) *remainingLine) || *remainingLine == '#') {
            remainingLine++;
        }

        parsed->mode = POST_INDEX;
        parsed->simm = strtol(remainingLine, NULL, 0);
        free(originalLine);
        return POST_INDEX;
    }

    // check for optional unsigned offset part
    if (*remainingLine == ']') {
        parsed->imm = 0;
        parsed->mode = UNSIGNED_OFFSET;
        free(originalLine);
        return UNSIGNED_OFFSET;
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
                free(originalLine);
                return PRE_INDEX;
            }

            // now we know its unsigned offset
            free(originalLine);
            parsed->mode = UNSIGNED_OFFSET;
            parsed->imm = value;
            return UNSIGNED_OFFSET;
        } else {
            parsed->mode = REGISTER_OFFSET;
            parsed->Xm = handle_register(&remainingLine);
            free(originalLine);
            return REGISTER_OFFSET;
        }
    }
    parsed->mode = INVALID;
    free(originalLine);
    return INVALID;
}

// AddressingMode parse_addressing_mode(const char *line, ParsedAddress *parsed) {
//     char *token;
//     char line_copy[100];
//     strcpy(line_copy, line);
//
//     // Initialize parsed structure
//     parsed->mode = INVALID;
//     parsed->simm = 0;
//     parsed->imm = 0;
//     parsed->literal = 0;
//
//     // Tokenize the input string
//     token = strtok(line_copy, " ,[]");
//     if (token == NULL) return INVALID;
//
//     // Parse Rt
//     parsed->Rt = reg_from_regStr(token);
//
//     // Parse the rest of the address
//     token = strtok(NULL, " ,[]");
//     if (token == NULL) return INVALID;
//
//     // Check if it's an immediate or a label
//     if (token[0] == '#') {
//         parsed->literal = strtol(token + 1, NULL, 0);
//         parsed->mode = LITERAL;
//         return LITERAL;
//     }
//
//     // check if its a label
//     int32_t imm = getValue(symbol_table, token);
//     if (imm != NOT_FOUND) {
//         parsed->literal = imm;
//         parsed->mode = LITERAL;
//         return LITERAL;
//     }
//
//     // Parse Xn|SP
//     parsed->Xn_SP = reg_from_regStr(token);
//
//     // Check for post-index
//     token = strtok(NULL, " ,[]");
//     if (token != NULL && token[0] == '#') {
//         parsed->simm = strtol(token + 1, NULL, 0);
//         token = strtok(NULL, " ,[]");
//         if (token == NULL) {
//             parsed->mode = POST_INDEX;
//             return POST_INDEX;
//         }
//     }
//
//     // Check for pre-index
//     if (token != NULL && strcmp(token, "!") == 0) {
//         parsed->mode = PRE_INDEX;
//         return PRE_INDEX;
//     }
//
//     // Check for unsigned offset
//     if (token == NULL) {
//         parsed->imm = 0;
//         parsed->mode = UNSIGNED_OFFSET;
//         return UNSIGNED_OFFSET;
//     }
//
//     if (token[0] == '#') {
//         parsed->imm = strtol(token + 1, NULL, 0);
//         parsed->mode = UNSIGNED_OFFSET;
//         return UNSIGNED_OFFSET;
//     }
//
//     // Check for register offset
//
//     parsed->Xm = reg_from_regStr(token);
//     parsed->mode = REGISTER_OFFSET;
//     return REGISTER_OFFSET;
// }

INST strload_inst(char *remainingLine, uint32_t address, Opcode opcode) {
    INST instr = 0;

    ParsedAddress parsed;
    parse_addressing_mode(remainingLine, &parsed);

    instr = modify_instruction(instr, 30, 30, parsed.Rt.is64Mode);
    instr = modify_instruction(instr, 0, 4, reg_to_binary(parsed.Rt));
    if (parsed.mode == LITERAL) {
        // do literal handling
        instr = modify_instruction(instr, 31, 31, 0);
        instr = modify_instruction(instr, 27, 29, 3);
        // handle simm19 stuff

        printf("parsed.literal is %d\n", parsed.literal);
        int32_t offset = (parsed.literal - (int32_t) address) / 4; // might need a divide by 4
        instr = modify_instruction(instr, 5, 23, offset);
    } else {
        instr = modify_instruction(instr, 31, 31, 1);
        instr = modify_instruction(instr, 25, 29, 0x1C);
        instr = modify_instruction(instr, 24, 24, parsed.mode == UNSIGNED_OFFSET ? 1 : 0);
        instr = modify_instruction(instr, 22, 22, opcode == LDR ? 1 : 0);

        instr = modify_instruction(instr, 5, 9, reg_to_binary(parsed.Xn_SP));

        // now handle the different cases based on addressing modes
        switch (parsed.mode) {
            case REGISTER_OFFSET:
                instr = modify_instruction(instr, 21, 21, 1);
                instr = modify_instruction(instr, 16, 20, reg_to_binary(parsed.Xm));
                instr = modify_instruction(instr, 10, 15, 0x1a);
                break;
            case PRE_INDEX: case POST_INDEX:
                instr = modify_instruction(instr, 12, 20, parsed.simm);
                instr = modify_instruction(instr, 11, 11, parsed.mode == PRE_INDEX ? 1 : 0);
                instr = modify_instruction(instr, 10, 10, 1);
                break;
            case UNSIGNED_OFFSET:
                int32_t imm12 = parsed.Rt.is64Mode ? parsed.imm / 8 : parsed.imm / 4;
                printf("imm12 for unsigned offset is %d\n", imm12);
                instr = modify_instruction(instr, 10, 21, imm12);
                break;
            default:
                fprintf(stderr, "mode is %d\n", parsed.mode);
                perror("Error in strload_inst, invalid mode\n");
                exit(EXIT_FAILURE);
        }
    }
    return instr;
}
//
// Created by Ahmad Jamsari on 31/05/24.
//

#include <ctype.h>
#include <string.h>
#include "parsing_common.h"

#include <stdio.h>
#include <stdlib.h>

// used for parsing string representation of registers in assembly
// macros need to be done like this because preprocessor does outside in
// when expanding
#define MAX_REG_LENGTH 10
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

static Register reg_from_regStr(const char *strReg);

/* PRE: string is after op_code ie it starts with a known register definition
 * example string this will receive is "x0, x0, #1, lsl #12"
 * remainingLine will skip the character after the comma
 * Assumes it may have leading whitespace.
 * Modifies remainingLine to point to the thing after the comma.

 * could probably just make handle_register return a binary value but its useful
 * to have an intermediate representation (as its used for other functions)
 * sometimes I might need the extra information
 */
Register handle_register(char **remainingLine) {

    // skips any leading whitespace
    while (isspace((unsigned char) **remainingLine)) {
        (*remainingLine)++;
    }
    // printf("Register handling: %s\n", *remainingLine);

    char *rest;
    const char *strReg = strtok_r(*remainingLine, ",", &rest);
    // str_reg should hopefully be "x0"

    // now determine type of register from string, adjust remainingLine so it points to the thing after comma
    *remainingLine = rest;
    return reg_from_regStr(strReg);
}

// this should only really be called by handle_register as the
// register string part must be a proper C string ie terminated by NULL_CHAR
// this is done using strtok_r in handle_register
static Register reg_from_regStr(const char *strReg) {
    Register reg;
    if (*strReg == 'x') {
        reg.is64Mode = true;
    } else if (*strReg == 'w') {
        reg.is64Mode = false;
    } else {
        fprintf(stderr, "%s\n", strReg);
        perror("issue for reg_from_regStr receiving invalid input");
        exit(EXIT_FAILURE);
    }

    // now check for zero register
    if (strcmp(strReg + 1, "zr") == 0) {
        reg.isZeroReg = true;
        reg.regNumber = 255; // set a value for debugging purposes
        return reg;
    }

    reg.isZeroReg = false;
    reg.regNumber = atoi(strReg + 1);
    return reg;
}

// function used for debugging purposes
void print_binary(INST number) {
    // Loop through each bit from the most significant to the least significant
    for (int i = 31; i >= 0; i--) {
        // Check if the i-th bit is set
        uint32_t bit = (number >> i) & 1;
        // Print the bit
        printf("%u", bit);

        // Optionally, add a space every 4 bits for readability
        if (i % 4 == 0 && i != 0) {
            printf(" ");
        }
    }
    printf("\n");
}

// could change this function so it takes in a INST * instead and return void
// however i like being more explicit and allows instr to be used as a register by compiler potentially
void modify_instruction(INST *instruction, const int x, const int y, const int value) {
    // Calculate the number of bits to be modified

    assert(x <= y);
    int32_t num_bits = y - x + 1;

    // printf("value is %u, max_val is %u\n", value, 1 << num_bits);
    assert(value < (1 << num_bits));

    // Create a mask for the bits to be modified
    uint32_t mask = ((1 << num_bits) - 1) << x;

    // Clear the bits in the original instruction
    *instruction &= ~mask;

    // Set the new value in the cleared bits
    *instruction |= (value << x) & mask;
}

uint32_t reg_to_binary(Register reg) {
    if (reg.isZeroReg) {
        return 31; // decimal representation of 11111
    }

    return reg.regNumber;
}

Shifter determineShift(char *remainingLine) {
    Shifter shifter;
    shifter.shiftAmount = 0;
    shifter.shiftType = LSL;

    if (*remainingLine == NULL_CHAR) {
        return shifter;
    }

    char *saveptr;

    // Skip leading whitespace
    while (isspace((unsigned char) *remainingLine)) {
        remainingLine++;
    }

    // Tokenize the shift type
    char *shift = strtok_r(remainingLine, " #", &saveptr);
    if (shift == NULL) {
        fprintf(stderr, "Error in Shifter: Invalid input format\n");
        exit(EXIT_FAILURE);
    }

    // Tokenize the shift amount
    const char *amountStr = strtok_r(NULL, " #", &saveptr);
    if (amountStr == NULL) {
        fprintf(stderr, "Error in Shifter: Invalid input format\n");
        exit(EXIT_FAILURE);
    }

    // Convert the shift amount to an unsigned long
    unsigned long imm = strtoul(amountStr, NULL, 0);
    if (imm == 0 && amountStr == saveptr) {
        fprintf(stderr, "Error in Shifter: Invalid shift amount\n");
        exit(EXIT_FAILURE);
    }

    shifter.shiftAmount = imm;

    // Determine the shift type
    // this could be done using our table ADT, however
    // I really don't want to keep track of any more tables
    if (strcasecmp(shift, "lsl") == 0) {
        shifter.shiftType = LSL;
    } else if (strcasecmp(shift, "lsr") == 0) {
        shifter.shiftType = LSR;
    } else if (strcasecmp(shift, "asr") == 0) {
        shifter.shiftType = ASR;
    } else if (strcasecmp(shift, "ror") == 0) {
        shifter.shiftType = ROR;
    } else {
        fprintf(stderr, "Error: Unknown shift type '%s'\n", shift);
        exit(EXIT_FAILURE);
    }

    return shifter;
}

// primarily used for data_processing_assembly.c
// it's like a form of intermediate representation
// easy to mess this up but also easy to fix
OpcodeType get_opcode_type(Opcode opcode) {
    OpcodeType opcodeType;

    switch (opcode) {
        case ADD: case ADDS: case SUB: case SUBS:
            opcodeType.type = ARITHMETIC;
            opcodeType.isNegated = false;
            break;
        case AND: case ORR: case EOR: case ANDS:
            opcodeType.type = BITWISE;
            opcodeType.isNegated = false;
            break;
        case BIC: case ORN: case EON: case BICS:
            opcodeType.type = BITWISE;
            opcodeType.isNegated = true;
            break;
        case MUL:
            opcodeType.type = MULTIPLY;
            opcodeType.isNegated = false;
            break;
        case MNEG:
            opcodeType.type = MULTIPLY;
            opcodeType.isNegated = true;
            break;
        default:
            fprintf(stderr, "get_opcode_type for other opcodes not available yet");
            // dummy implemenation
            opcodeType.type = MULTIPLY;
            opcodeType.isNegated = false;
            break;
    }

    return opcodeType;
}

// basically if i have " R1, R2", I need to transform it to
// "R1, RZR, R2"
// ie inserting RZR in the middle
// make sure instruction is of sufficient length
void transform_middle(char *instruction) {
    // Create a new string to hold the transformed instruction
    char transformed_instruction[MAX_LENGTH];

    // Extract the destination and source registers
    char Xd[MAX_REG_LENGTH + 1], Xm[MAX_REG_LENGTH + 1];
    // sscanf(instruction, " %s %s", Xd, Xm);
    // done to allow more flexibile handling of field width's with macro definitions
    // instead of hardcoding "%10s" which would require multiple changes in the future
    sscanf(instruction, " %" STR(MAX_REG_LENGTH) "s %" STR(MAX_REG_LENGTH) "s", Xd, Xm);

    // Format the new instruction as "ORR Xd, XZR, Xm"

    if (Xd[0] == 'x') {
        snprintf(transformed_instruction, sizeof(transformed_instruction), "%s xzr, %s", Xd, Xm);
    } else if (Xd[0] == 'w') {
        snprintf(transformed_instruction, sizeof(transformed_instruction), "%s wzr, %s", Xd, Xm);
    } else {
        perror("Invalid register prefix in transform_middle(); \n");
        exit(EXIT_FAILURE);
    }

    // Copy the transformed instruction back to the original string
    strcpy(instruction, transformed_instruction);
}

// basically transform X1, X2 to XZR, X1, X2 ie transform the start
// modifies instruction, make sure instruction is of sufficient length
void transform_start(char *instruction) {
    // Create a new string to hold the transformed instruction
    char transformed_instruction[MAX_LENGTH];

    // Extract the destination and source registers
    char Xd[MAX_REG_LENGTH + 1];
    // sscanf(instruction, " %s", Xd);
    // done to allow more flexibile handling of field width's with macro definitions
    // instead of hardcoding "%10s" which would require multiple changes in the future
    sscanf(instruction, " %" STR(MAX_REG_LENGTH) "s", Xd);

    if (Xd[0] == 'x') {
        strcpy(transformed_instruction, "xzr, ");
    } else if (Xd[0] == 'w') {
        strcpy(transformed_instruction, "wzr, ");
    } else {
        perror("Invalid register prefix in transform_middle(); \n");
        exit(EXIT_FAILURE);
    }

    strcat(transformed_instruction, instruction);
    strcpy(instruction, transformed_instruction);
}


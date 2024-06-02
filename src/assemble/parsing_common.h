//
// Created by terp on 31/05/24.
//

#ifndef PARSING_COMMON_H
#define PARSING_COMMON_H

#include "common_types.h"
#include <assert.h>

// returns the pointer to the remainingString after register input is handled
// alternative to this is returning a Register but using char **remainingLine
Register handle_register(char **remainingLine);

Register reg_from_regStr(const char *strReg);

void print_binary(INST number);

INST modify_instruction(INST instruction, int x, int y, int32_t value);

uint32_t reg_to_binary(Register reg);

// assumes it starts with whitespace or start of shift
// also assumes there's a # for all shifts
// ie doesn't start with comma
// example would be " asr #15"
// might also be a null char
Shifter determineShift(char *remainingLine);

// could add basic functions for determining the type of instruction
// from opcode ie isBitwise();

// takes in an opcode and categorises it and sees if its negated operated
OpcodeType get_opcode_type(Opcode opcode);

// basically if i have Opcode X1, X2, I need to transform it to
// Opcode X1, XZR, X2
// ie inserting XZR to the start
void transform_middle(char *instruction);

// basically transform X1, X2 to XZR, X1, X2 ie transform the start
void transform_start(char *instruction);

#endif //PARSING_COMMON_H

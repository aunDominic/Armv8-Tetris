//
// Created by Ahmad Jamsari on 31/05/24.
//

#ifndef PARSING_COMMON_H
#define PARSING_COMMON_H

#include "common_types.h"
#include <assert.h>

// returns the pointer to the remainingString after register input is handled
// alternative to this is returning a Register but using char **remainingLine
Register handle_register(char **remainingLine);

void print_binary(INST number);

INST modify_instruction(INST instruction, int x, int y, int32_t value);

uint32_t reg_to_binary(Register reg);

// assumes string starts with whitespace or start of shift 'string'
// also assumes there's a # for all shifts
// ie doesn't start with comma
// example would be " asr #15"
// might also be a null char
Shifter determineShift(char *remainingLine);

// takes in an opcode and categorises it and sees if its negated operated
OpcodeType get_opcode_type(Opcode opcode);

// basically if i have " R1, R2", I need to transform it to
// "R1, RZR, R2"
// ie inserting RZR in the middle
void transform_middle(char *instruction);

// basically transform R1, R2 to RZR, R1, R2 ie prepends zero register
void transform_start(char *instruction);

#endif //PARSING_COMMON_H

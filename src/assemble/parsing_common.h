//
// Created by Ahmad Jamsari on 31/05/24.
//

#ifndef PARSING_COMMON_H
#define PARSING_COMMON_H

#include "common_types.h"
#include <assert.h>

/* PRE: string is after op_code ie it starts with a known register definition
 * example string this will receive is "x0, x0, #1, lsl #12"
 * remainingLine will skip the character after the comma
 * Assumes it may have leading whitespace.
 * Modifies remainingLine to point to the thing after the comma.

 * could probably just make handle_register return a binary value but its useful
 * to have an intermediate representation (as its used for other functions)
 * sometimes I might need the extra information
 */
Register handle_register(char **remainingLine);

void print_binary(INST number);

void modify_instruction(INST *instruction, int x, int y, int32_t value);

uint32_t reg_to_binary(Register reg);

// PRE: string starts with whitespace or start of shift 'string'
// also assumes there's a # for all shifts
// example input: " asr #15"
// might also be a null char
Shifter determineShift(char *remainingLine);

// takes in an opcode and categorises it and sees if its negated operated
OpcodeType get_opcode_type(Opcode opcode);

// PRE: Input is like " R1, R2"
// MODIFIES inputString to be "R1, RZR, R2"
// ie inserting RZR in the middle
void transform_middle(char *instruction);

// basically transform R1, R2 to RZR, R1, R2 ie prepends zero register
void transform_start(char *instruction);

#endif //PARSING_COMMON_H

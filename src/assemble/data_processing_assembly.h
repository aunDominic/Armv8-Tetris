//
// Created by Ahmad Jamsari on 31/05/24.
//

#ifndef DATA_PROCESSING_ASSEMBLY_H
#define DATA_PROCESSING_ASSEMBLY_H
#include "common_types.h"

// In the future, you can remove some of the address parameters
// As they aren't used in the implementation of these.
// Feel free to change this file to have addresses as parameter as needed

/*
 * PRE - starts with the string after the opcode part
 * Example: if the originalLine is "add x1, x1, x1 " then remainingLine is " x1, x1, x1"
 * Assumes: remainingLine can have optional whitespace at the start
 * RETURNS the INST to then write later on
 */
INST two_op_inst(char *remainingLine, Opcode opcode);
INST multiply_inst(char *remainingLine, Opcode opcode);
INST single_op_inst_alias(char *remainingLine, Opcode opcode);
INST wide_move_inst(char *remainingLine, Opcode opcode);
INST two_op_nodest_inst(char *remainingLine, Opcode opcode); // two operands no destination

#endif //DATA_PROCESSING_ASSEMBLY_H

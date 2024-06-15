//
// Created by Ahmad Jamsari on 31/05/24.
//

#ifndef BRANCH_ASSEMBLY_H
#define BRANCH_ASSEMBLY_H

#include "common_types.h"

// In the future, you can remove some of the address parameters
// As they aren't used in the implementation of these.

/*
 * PRE - starts with the string after the opcode part
 * Example: if the originalLine is "b rest" then remainingLine is " rest"
 * Assumes: remainingLine can have optional whitespace at the start
 * There may or may not be whitespace at start.
 * RETURNS the INST to then write later on
 */

INST b_inst(const char *remainingLine, uint32_t address);
INST b_cond(const char *remainingLine, uint32_t address, Opcode bOpCode);

// br_inst branches to a value in a register so doesn't
// need to do offsets from current address
// hence doesn't need address as parameter
INST br_inst(char *remainingLine);

#endif //BRANCH_ASSEMBLY_H

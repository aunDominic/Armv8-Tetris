//
// Created by Ahmad Jamsari on 31/05/24.
//

#ifndef LOADSTORE_H
#define LOADSTORE_H

#include "common_types.h"

// str and load are so similar, we don't really need to distinguish between them
/*
 * PRE - starts with the string after the opcode part
 * Example: if the originalLine is "str w0, [x2]" then remainingLine is " w0, [x2]"
 * Assumes: remainingLine can have optional whitespace at the start
 * RETURNS the INST to then write later on
 */
INST strload_inst(char *remainingLine, uint32_t address, Opcode opcode);

#endif //LOADSTORE_H

//
// Created by Ahmad Jamsari on 31/05/24.
//

#ifndef DIRECTIVES_H
#define DIRECTIVES_H

#include "common_types.h"

/*
 * PRE - starts with the string after the .int directive part
 * Example: if the originalLine is ".int 1000" then remainingLine is " 1000"
 * Assumes: remainingLine can have optional whitespace at the start
 * RETURNS the INST to then write later on
 */
INST int_directive(const char *remainingLine, uint32_t address);

#endif //DIRECTIVES_H

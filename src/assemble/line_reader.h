//
// Created by Ahmad Jamsari on 30/05/24.
//

#ifndef LINE_READER_H
#define LINE_READER_H
#include <stdbool.h>
#include <stdint.h>
#include "common_types.h"

bool is_label(const char *line);

// Takes in a line from a file as a string with an address
// lineHandler will then get the opcode and route the remainingLine
// to the rest of the functions

INST lineHandler(char *line, uint32_t address);

#endif //LINE_READER_H

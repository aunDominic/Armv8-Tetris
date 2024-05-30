//
// Created by Ahmad Jamsari on 30/05/24.
//

#include <ctype.h>
#include <stdbool.h>
#include "line_reader.h"

#include <stdio.h>
#include <string.h>

#include "opcode_table.h"
#define MAX_LENGTH 100

bool is_label(const char *line) {
    int i = 0;

    // Skip leading whitespace
    while (isspace(line[i])) {
        i++;
    }

    // Check if the first character is a letter, underscore, or period
    if (!isalpha(line[i]) && line[i] != '_' && line[i] != '.') {
        return false;
    }
    i++;

    // Check the rest of the characters
    while (isalnum(line[i]) || line[i] == '$' || line[i] == '_' || line[i] == '.') {
        i++;
    }

    // Check if the next character is a colon
    return (line[i] == ':');
}

INST lineHandler(char *line, uint32_t address) {
    // dummy implementation
    static uint32_t functionCount = 0;

    // first step: get opcode first
    char str_opcode[MAX_LENGTH];
    char *token = strtok(line, " ");
    strcpy(str_opcode, token);

    int opcodeVal = getValue(opcodeTable, str_opcode);

    printf("Opcode is %s, Value is %d\n", str_opcode, opcodeVal);

    return functionCount++;
}


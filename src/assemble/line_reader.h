//
// Created by Ahmad Jamsari on 30/05/24.
//

#ifndef LINE_READER_H
#define LINE_READER_H
#include <stdbool.h>
#include <stdint.h>
#include "common_types.h"

bool is_label(const char *line);
INST lineHandler(char *line, uint32_t address);

#endif //LINE_READER_H

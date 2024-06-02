//
// Created by Ahmad Jamsari on 31/05/24.
//

#include "directives.h"

#include <stdlib.h>

INST int_directive(char *remainingLine, uint32_t address) {

    INST returnVal = strtol(remainingLine, NULL, 0);

    return returnVal;
}

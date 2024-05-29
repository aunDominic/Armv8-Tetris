//
// Created by Dominic Ng on 25/05/2024.
//

#ifndef EMULATOR_DATAPROCESSING_H
#define EMULATOR_DATAPROCESSING_H

#include <stdbool.h>
#include "memory.h"
#include "assert.h"
// Immediate
//
bool immediateHandler(INST instruction);
bool registerHandler (INST instruction);
#endif //EMULATOR_DATAPROCESSING_H

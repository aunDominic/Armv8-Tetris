//
// Created by Dominic Ng on 25/05/2024.
//

#ifndef EMULATOR_DATAPROCESSING_H
#define EMULATOR_DATAPROCESSING_H

#include <stdbool.h>

#include "memory.h"
// Immediate
//
bool immediate_handler(INST instruction);
bool register_handler (INST instruction);
#endif //EMULATOR_DATAPROCESSING_H

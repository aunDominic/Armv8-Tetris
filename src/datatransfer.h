//
// Created by Dominic Ng on 25/05/2024.
//


#ifndef EMULATOR_DATATRANSFER_H
#define EMULATOR_DATATRANSFER_H
#include <stdbool.h>
#include "memory.h"

bool singleDataTransferHandler(INST instruction);
bool loadLiteralHandler(INST instruction);
#endif //EMULATOR_DATATRANSFER_H

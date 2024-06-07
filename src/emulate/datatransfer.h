//
// Created by Dominic Ng on 25/05/2024.
//


#ifndef EMULATOR_DATATRANSFER_H
#define EMULATOR_DATATRANSFER_H
#include <stdbool.h>
#include "memory.h"

bool single_data_transfer_handler(INST instruction);
bool load_literal_handler(INST instruction);

#endif //EMULATOR_DATATRANSFER_H

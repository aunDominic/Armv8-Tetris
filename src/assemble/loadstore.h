//
// Created by Ahmad Jamsari on 31/05/24.
//

#ifndef LOADSTORE_H
#define LOADSTORE_H

#include "common_types.h"

// str and load are so similar, we don't really need to distinguish between them
INST strload_inst(char *remainingLine, uint32_t address, Opcode opcode);

#endif //LOADSTORE_H

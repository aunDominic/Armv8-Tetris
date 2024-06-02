//
// Created by Ahmad Jamsari on 31/05/24.
//

#ifndef LOADSTORE_H
#define LOADSTORE_H

#include "common_types.h"

// this may need fine-tuning on what exactly the public interface should be
// for now I'll do generic, and its up to the implementation to define additional functions to help them
INST strload_inst(char *remainingLine, uint32_t address, Opcode opcode);
#endif //LOADSTORE_H

//
// Created by Ahmad Jamsari on 31/05/24.
//

#ifndef BRANCH_ASSEMBLY_H
#define BRANCH_ASSEMBLY_H

#include "common_types.h"

INST b_inst(const char *remainingLine, uint32_t address);
INST b_cond(const char *remainingLine, uint32_t address, Opcode bOpCode);
INST br_inst(char *remainingLine, uint32_t address);

#endif //BRANCH_ASSEMBLY_H

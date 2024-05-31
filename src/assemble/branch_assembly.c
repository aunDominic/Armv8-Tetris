//
// Created by Ahmad Jamsari on 31/05/24.
//

#include "branch_assembly.h"
#include "symbol_table.h"

INST b_inst(char *remainingLine, uint32_t address) {
    return 48;
}
INST b_cond(char *remainingLine, uint32_t address, Opcode bOpCode) {
    return 49;
}
INST br_inst(char *remainingLine, uint32_t address) {
    return 50;
}
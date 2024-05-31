//
// Created by Ahmad Jamsari on 31/05/24.
//

#ifndef DATA_PROCESSING_ASSEMBLY_H
#define DATA_PROCESSING_ASSEMBLY_H
#include "common_types.h"

INST two_op_inst(char *remainingLine, uint32_t address, Opcode opcode);
INST multiply_inst(char *remainingLine, uint32_t address, Opcode opcode);
INST single_op_inst_dest(char *remainingLine, uint32_t address, Opcode opcode);
INST two_op_nodest_inst(char *remainingLine, uint32_t address, Opcode opcode); // two operands no destination

#endif //DATA_PROCESSING_ASSEMBLY_H

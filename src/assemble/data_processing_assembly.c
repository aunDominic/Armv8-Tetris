//
// Created by Ahmad Jamsari on 31/05/24.
//

#include "data_processing_assembly.h"

INST two_op_inst(char *remainingLine, uint32_t address, Opcode opcode) {
    return 64;
}
INST multiply_inst(char *remainingLine, uint32_t address, Opcode opcode) {
    return 65;
}
INST single_op_inst_dest(char *remainingLine, uint32_t address, Opcode opcode) {
    return 66;
}
INST two_op_nodest_inst(char *remainingLine, uint32_t address, Opcode opcode) {
    return 67;
}
//
// Created by Ahmad Jamsari on 31/05/24.
//

#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <stdint.h>

#define INST_SIZE 4
#define NULL_CHAR '\0'
typedef uint32_t INST;
typedef enum {
    ADD, ADDS, SUB, SUBS,
    CMP, CMN,
    NEG, NEGS,
    AND, ANDS, BIC, BICS,
    EOR, ORR, EON, ORN,
    TST,
    MOVK, MOVN, MOVZ,
    MOV,
    MVN,
    MADD, MSUB,
    MUL, MNEG,
    B, B_EQ, B_NE, B_GE, B_LT, B_GT, B_LE, B_AL,
    BR,
    STR, LDR,
    SPECIAL_INT
} Opcode;


#endif //COMMON_TYPES_H

//
// Created by terp on 30/05/24.
//

#ifndef OPCODE_TABLE_H
#define OPCODE_TABLE_H

#include "symbol_table.h"

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

extern SymbolTable *opcodeTable;

SymbolTable* createOpCodeTable();

#endif //OPCODE_TABLE_H

//
// Created by Ahmad Jamsari on 31/05/24.
//

#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#define INST_SIZE 4
#define NULL_CHAR '\0'
#define MAX_LENGTH 200
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

typedef struct {
    enum {
        ARITHMETIC, BITWISE, MULTIPLY
    } type;
    bool isNegated;
} OpcodeType;

typedef struct {
    uint8_t regNumber;
    bool isZeroReg;
    bool is64Mode;
} Register;

// DO NOT CHANGE THE ORDER OF THIS
typedef enum {
    LSL, LSR, ASR, ROR
} ShiftType;

typedef struct {
    ShiftType shiftType;
    uint32_t shiftAmount;
} Shifter;

#endif //COMMON_TYPES_H

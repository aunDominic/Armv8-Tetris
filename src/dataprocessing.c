//
// Created by Dominic Ng on 25/05/2024.
//

#include "dataprocessing.h"

// If you need more helper functions, use static functions

#include <assert.h>

static INST extractBits(INST instruction, int start, int end) {
    assert(start <= end);
    INST mask = ((1U << (end - start + 1)) -1) << start;
    return (instruction & mask) >> start;
}


static bool patternMatch(INST bits, INST pattern) {
    return bits == pattern;
}

static void setNZFlags(INST result, bool is64Bit)
{
    int bitWidth = is64Bit ? 64 : 32;
    pstate.N = extractBits(result, bitWidth - 1, bitWidth - 1);
    pstate.Z = (result == 0);
}

bool setRegisterValue(int64_t *reg, int64_t value, int bitWidth) {
    if (bitWidth == 32){
        *reg = (*reg & 0xFFFFFFFF00000000) | (value & 0xFFFFFFFF);
    } else if (bitWidth == 64) {
        *reg = value;
    } else{
        return false;
    }
    return true;
}

int64_t getRegisterValue(const int64_t *reg, int bitWidth) {
    if (bitWidth == 32) {
        return *reg & 0xFFFFFFFF;
    } else if (bitWidth == 64) {
        return *reg;
    } else {
        return 0;
    }
}

static bool arithmeticInstProcessor(INST instruction) {

    INST maskedSF    = extractBits(instruction, 31, 31);
    INST maskedOPC   = extractBits(instruction, 29, 30);
    INST maskedSH    = extractBits(instruction, 22, 22);
    INST maskedIMM12 = extractBits(instruction, 10,21);
    INST maskedRN    = extractBits(instruction, 5, 9);
    INST maskedRD     = extractBits(instruction, 0, 4);

    assert(maskedRN != 31 || maskedOPC == 1 || maskedOPC == 3); //RN check

    if (patternMatch(maskedSH, 0b1)) { //Is SH set
        maskedIMM12 = maskedIMM12 << 12;
    }


    if (patternMatch(maskedOPC, 0b00)) { //Add

    } else if (patternMatch(maskedOPC, 0b01)) { //Add and Set Flags

    } else if (patternMatch(maskedOPC, 0b10)) { //Subtract

    } else if (patternMatch(maskedOPC, 0b11)) { //Subtract and Update Flags

    } else {
        return false;
    }

}
static bool isWideMovePatternProcessor(INST instruction);

bool immediateHandler(INST instruction){

    int opiStart = 23;
    int opiEnd = 25;

    INST maskedOPI = extractBits(instruction, opiStart, opiEnd);

    INST arithmeticPattern = 0b010;
    INST wideMovePattern = 0b101;

    bool isArithmeticInst  = patternMatch(maskedOPI, arithmeticPattern);
    bool isWideMovePattern = patternMatch(maskedOPI, wideMovePattern);

    if (isArithmeticInst) {
        return arithmeticInstProcessor(instruction);
    } else if (isWideMovePattern) {
        return isWideMovePatternProcessor(instruction);
    } else {
        return false; //if error occurs
    }

}


bool registerHandler (INST instruction){
    return false; // if errors occur
}

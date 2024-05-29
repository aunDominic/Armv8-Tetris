//
// Created by Dominic Ng on 25/05/2024.
//

#include "dataprocessing.h"

// If you need more helper functions, use static functions

#include <assert.h>

static void setPSTATE32(uint32_t result);

static void setPSTATE64(uint64_t result);

static INST extractBits(INST instruction, int start, int end) {
    assert(start <= end);
    INST mask = ((1U << (end - start + 1)) -1) << start;
    return (instruction & mask) >> start;
}

static bool patternMatch(INST bits, INST pattern) {
    return bits == pattern;
}

static bool arithmeticInstProcessor(INST instruction) {

    INST maskedSF    = extractBits(instruction, 31, 31);
    INST maskedOPC   = extractBits(instruction, 29, 30);
    INST maskedSH    = extractBits(instruction, 22, 22);
    INST maskedIMM12 = extractBits(instruction, 10,21);
    INST maskedRN    = extractBits(instruction, 5, 9);
    INST maskedRD     = extractBits(instruction, 0, 4);


    INST addPattern = 0b00;
    INST addsPattern = 0b01;
    INST subPattern = 0b10;
    INST subsPattern = 0b11;
    INST shPattern = 0b1;

    bool isADD  = patternMatch(maskedOPC, addPattern);
    bool isADDS = patternMatch(maskedOPC, addsPattern);
    bool isSUB = patternMatch(maskedOPC, subPattern);
    bool isSUBS = patternMatch(maskedOPC, subsPattern);
    bool shSET = patternMatch(maskedSH, shPattern);

    if (shSET) {
        maskedIMM12 = maskedIMM12 << 12;
    }

    if (isADD) {

    } else if (isADDS) {

    } else if (isSUB) {

    } else if (isSUBS) {

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
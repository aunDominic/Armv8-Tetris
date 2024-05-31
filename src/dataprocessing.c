//
// Created by Dominic Ng on 25/05/2024.
//

//Okay so potential bug found is when we extraact bits we are returning a signed integer,
// so when we are doing the pattern matching we have to ensure that we have decoded the pattern properly
// i.e if the pattern has a 1 at the start then it would be negative?

#include "dataprocessing.h"

// If you need more helper functions, use static functions

#include <assert.h>
#define ZERO_REGISTER 31

//Extraction Functions - test passed

static uint32_t extractBits(INST instruction, int start, int end) {
    assert(start <= end);
    uint32_t mask;
    if (end - start == 31) {
        mask = 0xFFFFFFFF; // Mask for all 32 bits
    } else {
        mask = ((1U << (end - start + 1)) - 1) << start;
    }
    return (instruction & mask) >> start;
}

static bool patternMatch(INST bits, INST pattern) {
    return bits == pattern;
}

//Writing and Reading to Registers - test passed

static void writeReg(int reg, int64_t signedVal, int mode) {
    if (reg == ZERO_REGISTER) {
        return;
    } else {
        registers[reg] = (mode) ? signedVal : (int32_t)signedVal;
    }
}

static int64_t readReg(int reg, int mode) {
    if (reg == ZERO_REGISTER) {
        return 0;
    } else {
        return (mode) ? registers[reg] : (int32_t)(registers[reg]);
    }
}


//Shifter Functions - test passed

static int64_t lsl (int64_t value, int shift_amount, int mode) {
    int64_t result = value << shift_amount;
    result = (mode == 32) ? uint32_t(result) : result;
    return result;
}

static int64_t lsr (int64_t value, int shift_amount, int mode) {
    int64_t result = value >> shift_amount;
    result = (mode == 32) ? uint32_t(result) : result;
    return result;
}

static int64_t asr (uint64_t value, int shift_amount, int mode) {

    switch (mode) {
        case 32:
            int signBit32 = (value >> 31) &1;
            value >>= shift_amount;
            int32_t signExtension32 = (signBit32) ? -1LL << (32 - shift_amount) : 0;
            return (uint32_t)(value | signExtension32);
            break;
        case 64:
            int signBit64 = (value >> 63) & 1;
            value >>= shift_amount;
            int64_t signExtension64 = (signBit64) ? -1LL << (64 - shift_amount) : 0;
            return value | signExtension64;
            break;
        default: ;
    }

}

static int64_t ror (uint64_t value, int shift_amount, int mode) {
    switch (mode) {
        case 32:
            return uint32_t((value >> shift_amount) | (value << (32-shift_amount)));
            break;
        case 64:
            return (value >> shift_amount) | (value << (64-shift_amount));
            break;
        default: ;
    }
}


//Immediate Handler and Processor Logic Below

static bool arithmeticImmInstProcessor(INST instruction) { //Unsure about how to set flags

    INST maskedSF    = extractBits(instruction, 31, 31);
    INST maskedOPC   = extractBits(instruction, 29, 30);
    INST maskedSH    = extractBits(instruction, 22, 22);
    int16_t maskedIMM12 = extractBits(instruction, 10,21);
    INST maskedRN    = extractBits(instruction, 5, 9);
    INST maskedRD     = extractBits(instruction, 0, 4);

    assert(maskedRN != 31 || maskedOPC == 1 || maskedOPC == 3); //RN check

    if (patternMatch(maskedSH, 0b1)) { //Is SH set
        maskedIMM12 = 0;
    }

    int64_t operand = maskedIMM12;

    int64_t rn = (maskedSF == 11111) ? 0 : registers[maskedRN];   //If RN is 1111 then we encode it as the Zero Register?

    if (patternMatch(maskedOPC, 0b00))
    { //Add
        if (maskedSF)
        {
            const int64_t result = rn + operand;
            registers[maskedRD] = result;
        } else
        {
            const int64_t result = uint32_t(uint32_t(rn) + operand);
            registers[maskedRD] = result;
        }

    } else if (patternMatch(maskedOPC, 0b01))
    { //Add and Set Flags
        if (maskedSF) //64bits registers
        {
            const int64_t result = rn + operand;
            registers[maskedRD] = result;

            //Setting pstate flags
            pstate.N = result & 1ULL << 63;

            pstate.Z = result == 0;

            if (result < rn || result < operand)
            {
                pstate.C = 1;
            } else
            {
                pstate.C = 0;
            }

            pstate.V = (operand > 0 && rn > (INT64_MAX - operand)) || (operand < 0 && rn < INT64_MIN - operand)
            || (operand > 0 && rn < INT64_MIN + operand) || (operand < 0 && rn > INT64_MAX + operand);

        } else //32bits registers
        {
            const int64_t result = uint32_t(uint32_t(rn) + operand);
            registers[maskedRD] = result;

            //Setting pstate flags
            pstate.N = extractBits(result, 31, 31);
            pstate.Z = result == 0;

            if (result < uint32_t(rn) || result < operand)
            {
                pstate.C = 1;
            } else
            {
                pstate.C = 0;
            }

            pstate.V = (operand > 0 && uint32_t(rn) > (INT64_MAX - operand)) || (operand < 0 && uint32_t(rn) < INT64_MIN - operand)
            || (operand > 0 && uint32_t(rn) < INT64_MIN + operand) || (operand < 0 && uint32_t(rn) > INT64_MAX + operand);

        }



    } else if (patternMatch(maskedOPC, 0b10)) { //Subtract

        if (maskedSF) {
            registers[maskedRD] = rn - operand;
        } else {
            registers[maskedRD] = uint32_t(uint32_t(rn) - operand);
        }

    } else if (patternMatch(maskedOPC, 0b11)) { //Subtract and Update Flags

        if (maskedSF) {
            int64_t result = rn - operand;
            registers[maskedRD] = result;
            pstate.N = (result >> 63) & 1;
            pstate.Z = result == 0;
            return true;
        } else {

            int64_t result = uint32_t(uint32_t(rn) - operand);
            registers[maskedRD] = result;
            pstate.N = (result >> 31) & 1;
            pstate.Z = result == 0;

        }


    } else {

        return false;
    }
    return true;

}

static bool wideMoveImmInstProcessor(INST instruction) {

    INST maskedSF    = extractBits(instruction, 31, 31);
    INST maskedOPC   = extractBits(instruction, 29, 30);
    INST maskedHW    = extractBits(instruction, 21, 22);
    INST maskedIMM16 = extractBits(instruction, 5, 20);
    INST maskedRD    = extractBits(instruction, 0, 4);

    if (~maskedSF) //If working with 32 bit registers our shift can only be 0 or 1
    {
        assert(patternMatch(maskedHW, 0b00) || patternMatch(maskedHW, 0b01));
    }

    int64_t operand = maskedIMM16 << (maskedHW * 16);

    if (patternMatch(maskedOPC, 0b00)) //movn
    {
        int64_t result = ~operand;
        if (maskedSF)
        {
            registers[maskedRD] = result;
        } else
        {
            registers[maskedRD] = (int32_t)result;
        }
        return true;
    } else if (patternMatch(maskedOPC, 0b10)) //movz
    {
        int64_t result = operand;
        if (maskedSF)
        {
            registers[maskedRD] = result;
        } else
        {
            registers[maskedRD] = (int32_t)result;
        }
        return true;
    } else if (patternMatch(maskedOPC, 0b11)) //movk - UNSURE about the shifting logic correct if bugs
    {
        int64_t shiftValue = (maskedHW * 16);
        int64_t valueInRegister = registers[maskedRD];
        int64_t upToL = extractBits(valueInRegister, 0, shiftValue - 1);
        int64_t lToU = maskedIMM16 << shiftValue;
        int64_t uToEnd = extractBits(valueInRegister, shiftValue + 16, 64);
        int64_t result = upToL + lToU + uToEnd;
        if (maskedSF)
        {
            registers[maskedRD] = result;
        } else
        {
            registers[maskedRD] = (uint32_t)result;
        }
        return true;

    } else
    {
        return false;
    }


}


//Immediate Handler (detects arithmetic/widemove inst) - passed all tests

bool immediateHandler(INST instruction){

    INST maskedOPI = extractBits(instruction, 23, 25);

    if (patternMatch(maskedOPI, 0b010)) { //Arithmetic Instruction
        return arithmeticImmInstProcessor(instruction);
    } else if (patternMatch(maskedOPI, 0b101)) { //WideMove Instruction
        return wideMoveImmInstProcessor(instruction);
    } else {
        return false; //if error occurs
    }

}





//Register Handler and Processor Logic Below

// Enumeration for shift types
typedef enum {
    LSL = 0b00,
    LSR = 0b01,
    ASR = 0b10,
    ROR = 0b11
} ShiftType;

// Enumeration for operation codes
typedef enum {
    AND = 0b000,
    BIC = 0b001,
    ORR = 0b010,
    ORN = 0b011,
    EOR = 0b100,
    EON = 0b101,
    ANDS = 0b110,
    BICS = 0b111
} BitLogicCode;

typedef enum {
    MADD = 0b0,
    MSUB = 0b1
} MultiplyOpType;

bool multiplyRegInstProcessor(INST instruction) {
    // Extract fields from instruction
    INST sf = extractBits(instruction, 31, 31);
    INST rm = extractBits(instruction, 16, 20);
    INST ra = extractBits(instruction, 10, 14);
    INST op = extractBits(instruction, 15, 15); // X field
    INST rn = extractBits(instruction, 5, 9);
    INST rd = extractBits(instruction, 0, 4);

    // Read the values from the registers, handling zero register cases
    int64_t reg_ra = (patternMatch(ra, 0b11111)) ? zeroRegister : registers[ra];
    reg_ra = (sf) ? reg_ra : (uint32_t)reg_ra; // Adjust ra according to bit width

    int64_t reg_rm = (patternMatch(rm, 0b11111)) ? zeroRegister : registers[rm];
    reg_rm = (sf) ? reg_rm : (uint32_t)reg_rm; // Adjust rm according to bit width

    int64_t reg_rn = (patternMatch(rn, 0b11111)) ? zeroRegister : registers[rn];
    reg_rn = (sf) ? reg_rn : (uint32_t)reg_rn; // Adjust rn according to bit width

    int64_t result;

    // Perform the operation based on the op code
    switch ((MultiplyOpType)op) {
        case MADD:
            result = reg_ra + (reg_rn * reg_rm);
        break;
        case MSUB:
            result = reg_ra - (reg_rn * reg_rm);
        break;
        default:
            return false;
    }

    // Store the result in the destination register
    registers[rd] = (sf) ? result : (uint32_t)result;
    return true;
}

bool arithmeticRegInstProcessor(INST instruction) {

    int32_t sf  = extractBits(instruction, 31, 31);
    int32_t opc = extractBits(instruction, 29, 30);
    int32_t shift = extractBits(instruction, 22, 23);
    int32_t rm  = extractBits(instruction, 16, 20);
    int32_t op  = extractBits(instruction, 10, 15);
    int32_t rn  = extractBits(instruction, 5, 9);
    int32_t rd  = extractBits(instruction, 0, 4);

    assert(!(patternMatch(rd, 0b11111)));

    // Read the values from the registers
    int64_t reg_rn = (sf) ? registers[rn] : (uint32_t)registers[rn];
    int64_t reg_rm = (sf) ? registers[rm] : (uint32_t)registers[rm];
    int operand = extractBits(op, 0, (sf) ? 5 : 4);
    int64_t operand2;

    int mode = (sf) ? 64 : 32;

    // Perform the shift operation
    switch ((ShiftType)shift) {
    case LSL:
        operand2 = lsl(reg_rm, operand, mode);
        break;
    case LSR:
        operand2 = lsr(reg_rm, operand, mode);
        break;
    case ASR:
        operand2 = asr(reg_rm, operand, mode);
        break;
    case ROR:
        operand2 = ror(reg_rm, operand, mode);
        break;
    default:
        return false;
    }

    // Handle special zero register cases
    if (patternMatch(rn, 0b11111)) reg_rn = zeroRegister;
    if (patternMatch(rm, 0b11111)) operand2 = zeroRegister;

    if (patternMatch(opc, 0b00)) { //Add
        if (sf) {
            const int64_t result = reg_rn + operand2;
            registers[rd] = result;
        } else {
            const int64_t result = uint32_t(uint32_t(reg_rn) + operand2);
            registers[rd] = result;
        }

    } else if (patternMatch(opc, 0b01)) { //Add and Set Flags
        if (sf) { //64bits registers
            const int64_t result = reg_rn + operand2;
            registers[rd] = result;

            //Setting pstate flags
            pstate.N = result & 1ULL << 63;

            pstate.Z = result == 0;

            if (result < reg_rn || result < operand2)
            {
                pstate.C = 1;
            } else
            {
                pstate.C = 0;
            }

            pstate.V = (operand2 > 0 && reg_rn > (INT64_MAX - operand2)) || (operand2 < 0 && reg_rn < INT64_MIN - operand2)
            || (operand2 > 0 && reg_rn < INT64_MIN + operand2) || (operand2 < 0 && reg_rn > INT64_MAX + operand2);

        } else { //32bits registers

            const int64_t result = uint32_t(uint32_t(reg_rn) + operand2);
            registers[rd] = result;

            //Setting pstate flags
            pstate.N = extractBits(result, 31, 31);
            pstate.Z = result == 0;

            if (result < uint32_t(reg_rn) || result < operand2) {
                pstate.C = 1;
            } else {
                pstate.C = 0;
            }

            pstate.V = (operand2 > 0 && uint32_t(reg_rn) > (INT64_MAX - operand2)) || (operand2 < 0 && uint32_t(reg_rn) < INT64_MIN - operand2)
            || (operand2 > 0 && uint32_t(reg_rn) < INT64_MIN + operand2) || (operand2 < 0 && uint32_t(reg_rn) > INT64_MAX + operand2);

        }

    } else if (patternMatch(opc, 0b10)) { //Subtract

        if (sf) {
            registers[rd] = reg_rn - operand2;
        } else {
            registers[rd] = uint32_t(uint32_t(reg_rn) - operand2);
        }

    } else if (patternMatch(opc, 0b11)) { //Subtract and Update Flags - have not implemented last 2 flags
        if (sf) {
            int64_t result = reg_rn - operand2;
            registers[rd] = result;
            pstate.N = (result >> 63) & 1;
            pstate.Z = result == 0;
            return true;
        } else {
            int64_t result = uint32_t(uint32_t(reg_rn) - operand2);
            registers[rd] = result;
            pstate.N = (result >> 31) & 1;
            pstate.Z = result == 0;
            return true;
        }
    } else {
        return false;
    }

    return true;

}


bool bitLogicRegInstProcessor(INST instruction) {
    // Extract fields from instruction
    INST sf = extractBits(instruction, 31, 31);
    INST opc = extractBits(instruction, 29, 30);
    INST shift = extractBits(instruction, 22, 23);
    INST n = extractBits(instruction, 21, 21);
    INST rm = extractBits(instruction, 16, 20);
    INST op = extractBits(instruction, 10, 15);
    INST rn = extractBits(instruction, 5, 9);
    INST rd = extractBits(instruction, 0, 4);

    // Check if destination register is invalid
    assert(!patternMatch(rd, 0b11111));

    // Read the values from the registers
    int64_t reg_rn = (sf) ? registers[rn] : (uint32_t)registers[rn];
    int64_t reg_rm = (sf) ? registers[rm] : (uint32_t)registers[rm];
    int operand = extractBits(op, 0, (sf) ? 5 : 4);
    int64_t operand2;

    int mode = (sf) ? 64 : 32;

    // Perform the shift operation
    switch ((ShiftType)shift) {
        case LSL:
            operand2 = lsl(reg_rm, operand, mode);
            break;
        case LSR:
            operand2 = lsr(reg_rm, operand, mode);
            break;
        case ASR:
            operand2 = asr(reg_rm, operand, mode);
            break;
        case ROR:
            operand2 = ror(reg_rm, operand, mode);
            break;
        default:
            return false;
    }

    // Handle special zero register cases
    if (patternMatch(rn, 0b11111)) reg_rn = zeroRegister;
    if (patternMatch(rm, 0b11111)) operand2 = zeroRegister;

    INST code = (opc << 1) + n;

    // Perform the operation based on the opcode
    int64_t result;
    switch ((BitLogicCode)code) {
        case AND:
            result = reg_rn & operand2;
            break;
        case BIC:
            result = reg_rn & ~operand2;
            break;
        case ORR:
            result = reg_rn | operand2;
            break;
        case ORN:
            result = reg_rn | ~operand2;
            break;
        case EOR:
            result = reg_rn ^ operand2;
            break;
        case EON:
            result = reg_rn ^ ~operand2;
            break;
        case ANDS:
            result = reg_rn & operand2;
            registers[rd] = (sf) ? result : (uint32_t)result;
            pstate.N = (sf) ? extractBits(result, 63, 63) : extractBits(result, 31, 31);
            pstate.Z = (result == 0);
            pstate.C = 0;
            pstate.V = 0;
            return true;
        case BICS:
            result = reg_rn & ~operand2;
            registers[rd] = (sf) ? result : (uint32_t)result;
            pstate.N = (sf) ? extractBits(result, 63, 63) : extractBits(result, 31, 31);
            pstate.Z = (result == 0);
            pstate.C = 0;
            pstate.V = 0;
            return true;
        default:
            return false;
    }

    // Store the result in the destination register
    registers[rd] = (sf) ? result : (uint32_t)result;
    return true;
}



bool registerHandler(INST instruction) {
    // Extract fields from instruction
    INST m = extractBits(instruction, 28, 28);
    INST opr = extractBits(instruction, 21, 24);
    INST bit24 = extractBits(instruction, 24, 24);
    INST bit21 = extractBits(instruction, 21, 21);

    // Check for multiply instruction
    if (m && patternMatch(opr, 0b1000)) {
        return multiplyRegInstProcessor(instruction);
    }
    // Check for arithmetic instruction
    else if (!m && bit24 && !bit21) {
        return arithmeticRegInstProcessor(instruction);
    }
    // Check for bit logic instruction
    else if (!m && !bit24) {
        return bitLogicRegInstProcessor(instruction);
    }
    // If none of the conditions are met, return false indicating an error
    else {
        return false;
    }
}

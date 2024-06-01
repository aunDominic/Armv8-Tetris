//
// Created by Dominic Ng on 25/05/2024.
//

#include "dataprocessing.h"
#include <stdio.h>
// If you need more helper functions, use static functions
#include <stdio.h>
#include <assert.h>
#define ZERO_REGISTER 31      // Defining Zero Register
typedef enum {
    LSL = 0b00,
    LSR = 0b01,
    ASR = 0b10,
    ROR = 0b11
} ShiftType; // Enumeration for shift types

<<<<<<< HEAD
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
static bool isWideMovePatternProcessor(INST instruction) {
    return false;
};

bool immediateHandler(INST instruction){
    printf("Immediate called.\n");
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
    printf("Register called.\n");

    return false; // if errors occur
}
=======
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
        printf("writing to zero register - null"); //r
        return;
    } else {
        printf("writing value %lld to register %d in %d mode\n", signedVal, reg, mode); //r
        registers[reg] = (mode) ? signedVal : (int32_t)signedVal;
        return;
    }
}
static int64_t readReg(int reg, int mode) {
    if (reg == ZERO_REGISTER) {
        printf("reading from a zero register\n");
        return 0;
    } else {
        printf("reading from register %d in %d mode\n", reg, mode);
        return (mode) ? registers[reg] : (int32_t)(registers[reg]);
    }
}

//Shifter Functions - test passed
static int64_t lsl(int64_t value, int shift_amount, int mode) {
    int64_t result = value << shift_amount;
    result = (mode) ? result : (int64_t)(uint32_t)result;
    return result;
}
static int64_t lsr(int64_t value, int shift_amount, int mode) {
    int64_t result = (uint64_t)value >> shift_amount;
    result = (mode) ? result : (int64_t)(uint32_t)result;
    return result;
}
static int64_t asr(int64_t value, int shift_amount, int mode) {
    switch (mode) {
        case 0: {
            int signBit32 = (value >> 31) & 1;
            value >>= shift_amount;
            int32_t signExtension32 = (signBit32) ? -1LL << (32 - shift_amount) : 0;
            return (uint32_t)(value | signExtension32);
        }
        case 1: {
            int signBit64 = (value >> 63) & 1;
            value >>= shift_amount;
            int64_t signExtension64 = (signBit64) ? -1LL << (64 - shift_amount) : 0;
            return value | signExtension64;
        }
        default:
            return value;
    }
}
static int64_t ror(int64_t value, int shift_amount, int mode) {
    shift_amount %= (mode ? 64 : 32);
    switch (mode) {
        case 0:
            return (uint32_t)((value >> shift_amount) | (value << (32 - shift_amount)));
        case 1:
            return (value >> shift_amount) | (value << (64 - shift_amount));
        default:
            return value;
    }
}

//Updating pstate
static void updateFlags(int64_t a, int64_t b, int64_t result, int opc, int mode) {

    typedef enum{
        ADDS = 0b01,
        SUBS = 0b11,
        ANDS = 0b110,
        BICS = 0b111
    } Code;

    pstate.N = (mode) ? (extractBits(result, 63, 63)) : (extractBits(result, 31, 31));
    pstate.Z = result == 0;

    switch ((Code)opc) {

        case ADDS: {
            break;
        }

        case SUBS: {
            break;
        }

        case ANDS: {
            pstate.C = 0;
            pstate.V = 0;
            break;
        }

        case BICS: {
            pstate.C = 0;
            pstate.V = 0;
            break;
        }

    }
}

//Processor Logic

//Immediate Processor Logic Below
static bool arithmeticImmInstProcessor(INST instruction) { //Unsure about how to set flags

    typedef enum {
        ADD = 0b00,
        ADDS = 0b01,
        SUB = 0b10,
        SUBS = 0b11
    } Code;

    //Deconstructing Instruction
    int sf        = extractBits(instruction, 31, 31);
    int opc       = extractBits(instruction, 29, 30);
    int shiftFlag = extractBits(instruction, 22, 22);
    int imm12     = (shiftFlag) ? 0 : extractBits(instruction, 10,21);
    int rnI        = extractBits(instruction, 5, 9);
    int rdI        = extractBits(instruction, 0, 4);

    //Values for Arithmetic Instruction
    int64_t rn  = readReg(rnI, sf);
    int64_t op2 = imm12;

    //Action
    switch ((Code) opc) {
        case ADD: {
            printf("this is an add immediate instruction\n"); //r
            int64_t result = rn + op2;
            writeReg(rdI, result, sf);
            break;
        }
        case ADDS: {
            printf("this is an add immediate instruction and updating cond flags\n"); //r
            int64_t result = rn + op2;
            writeReg(rdI, result, sf);
            updateFlags(rn, op2, result, opc,  sf);
            break;
        }
        case SUB: {
            printf("this is an sub imm instruction\n"); //r
            int64_t result = rn - op2;
            writeReg(rdI, result, sf);
            break;
        }
        case SUBS: {
            printf("this is a sub imm ins and update c flags\n"); //r
            int64_t result = rn - op2;
            writeReg(rdI, result, sf);
            updateFlags(rn, op2, result, opc,  sf);
            break;
        }
    }

    return true;

}
static bool wideMoveImmInstProcessor(INST instruction) {
    printf("this is a wide move immediate instruction processor!\n"); //r

    typedef enum {
        MOVN = 0b00,
        MOVZ = 0b10,
        MOVK = 0b11
    } Code;

    int sf    = extractBits(instruction, 31, 31);
    int opc   = extractBits(instruction, 29, 30);
    int hw    = extractBits(instruction, 21, 22); assert(patternMatch(hw, 0b00) || patternMatch(hw, 0b01) || sf);
    uint16_t imm16 = extractBits(instruction, 5, 20);
    int rdI    = extractBits(instruction, 0, 4);

    int shiftValue = hw * 16;

    uint64_t op = imm16 << shiftValue;

    switch ((Code)opc) {

        case MOVN: {
            printf("movn - value: %llu - register.no: %d - mode: %d\n", ~op, rdI, sf); //r
            writeReg(rdI, ~op, sf);
            break;
        }
        case MOVZ: {
            printf("movz - value: %llu - register.no: %d - mode: %d\n", op, rdI, sf); //r
            writeReg(rdI, op, sf);
            break;
        }
        case MOVK: {
            uint64_t mask = (uint64_t)0xFFFF << shiftValue;
            int64_t rd = readReg(rdI, 1);
            rd &= ~mask;
            int64_t result = rd || op;
            printf("movk - value: %lld - register.no: %d - mode: %d\n", result, rdI, sf); //r
            writeReg(rdI, result, sf);
            break;
        }
    }
    return true;
}


//Register Processor Logic Below
static bool multiplyRegInstProcessor(INST instruction) {
    printf("this is a processor for multiply insturctions with reg val\n");

    typedef enum {
        MADD = 0b0,
        MSUB = 0b1
    } MultiplyOpType;

    // Extract fields from instruction
    int sf = extractBits(instruction, 31, 31);
    int rmI = extractBits(instruction, 16, 20);
    int raI = extractBits(instruction, 10, 14);
    int x = extractBits(instruction, 15, 15); // X field
    int rnI = extractBits(instruction, 5, 9);
    int rdI = extractBits(instruction, 0, 4);

    assert(~patternMatch(rdI, 31));

    // Read the values from the registers, handling zero register cases
    int64_t ra = readReg(raI, sf);
    int64_t rm = readReg(rmI, sf);
    int64_t rn = readReg(rnI, sf);

    int64_t result;

    // Perform the operation based on the op code
    switch ((MultiplyOpType)x) {
        case MADD:
            result = ra + (rn * rm);
            printf("MADD - value: %lld \n", result); //r
        break;
        case MSUB:
            result = ra - (rn * rm);
            printf("MSUB bit logic - value: %lld \n", result); //r
        break;
        default:
            return false;
    }

    // Store the result in the destination register
    writeReg(rdI, result, sf);
    return true;

}
static bool arithmeticRegInstProcessor(INST instruction) {
    printf("this is a processor for arithmeticRegInstProcessor\n");

    typedef enum {
        ADD = 0b00,
        ADDS = 0b01,
        SUB = 0b10,
        SUBS = 0b11
    } Code;

    // Extract fields from instruction
    int sf = extractBits(instruction, 31, 31);
    int opc = extractBits(instruction, 29, 30);
    int shiftType = extractBits(instruction, 22, 23);
    int rmI = extractBits(instruction, 16, 20);
    int shiftAmount = (sf) ? extractBits(instruction, 10, 15) : extractBits(instruction, 10, 14);
    int rnI = extractBits(instruction, 5, 9);
    int rdI = extractBits(instruction, 0, 4);

    // Check if destination register is invalid
    assert(!patternMatch(rdI, 0b11111));

    // Read the values from the registers
    int64_t rm = readReg(rmI, sf);
    int64_t rn = readReg(rnI, sf);
    int64_t operand2;

    // Perform the shift operation
    switch ((ShiftType)shiftType) {
        case LSL:
            operand2 = lsl(rm, shiftAmount, sf);
        break;
        case LSR:
            operand2 = lsr(rm, shiftAmount, sf);
        break;
        case ASR:
            operand2 = asr(rm, shiftAmount, sf);
        break;
        case ROR:
            operand2 = ror(rm, shiftAmount, sf);
        break;
        default:
            return false;
    }

    //Action
    switch ((Code) opc) {
        case ADD: {
            printf("this is an add register instruction\n"); //r
            int64_t result = rn + operand2;
            writeReg(rdI, result, sf);
            break;
        }
        case ADDS: {
            printf("this is an add register instruction and updating cond flags\n"); //r
            int64_t result = rn + operand2;
            writeReg(rdI, result, sf);
            updateFlags(rn, operand2, result, opc,  sf);
            break;
        }
        case SUB: {
            printf("this is an sub register instruction\n"); //r
            int64_t result = rn - operand2;
            writeReg(rdI, result, sf);
            break;
        }
        case SUBS: {
            printf("this is a sub register ins and update c flags\n"); //r
            int64_t result = rn - operand2;
            writeReg(rdI, result, sf);
            updateFlags(rn, operand2, result, opc,  sf);
            break;
        }
    }

    return true;

}
static bool bitLogicRegInstProcessor(INST instruction) {
    printf("this is a processor for bitLogicRegInstProcessor\n");

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
    } Code;

    // Extract fields from instruction
    int sf = extractBits(instruction, 31, 31);
    int opc = extractBits(instruction, 29, 30);
    int shiftType = extractBits(instruction, 22, 23);
    int n = extractBits(instruction, 21, 21);
    int rmI = extractBits(instruction, 16, 20);
    int shiftAmount = (sf) ? extractBits(instruction, 10, 15) : extractBits(instruction, 10, 14);
    int rnI = extractBits(instruction, 5, 9);
    int rdI = extractBits(instruction, 0, 4);

    // Check if destination register is invalid
    assert(!patternMatch(rdI, 0b11111));

    // Read the values from the registers
    int64_t rm = readReg(rmI, sf);
    int64_t rn = readReg(rnI, sf);
    int64_t operand2;

    // Perform the shift operation
    switch ((ShiftType)shiftType) {
        case LSL:
            operand2 = lsl(rm, shiftAmount, sf);
            break;
        case LSR:
            operand2 = lsr(rm, shiftAmount, sf);
            break;
        case ASR:
            operand2 = asr(rm, shiftAmount, sf);
            break;
        case ROR:
            operand2 = ror(rm, shiftAmount, sf);
            break;
        default:
            return false;
    }

    INST code = (opc << 1) + n;

    // Perform the operation based on the opcode
    int64_t result;
    switch ((Code) code) {
        case AND:
            result = rn & operand2;
            printf("and bit logic - value: %lld \n", result); //r
            break;
        case BIC:
            result = rn & ~operand2;
            printf("bic bit logic - value: %lld \n", result); //r
            break;
        case ORR:
            result = rn | operand2;
            printf("orr bit logic - value: %lld \n", result); //r
            break;
        case ORN:
            result = rn | ~operand2;
            printf("orn bit logic - value: %lld \n", result); //r
            break;
        case EOR:
            result = rn ^ operand2;
            printf("eor bit logic - value: %lld \n", result); //r
            break;
        case EON:
            result = rn ^ ~operand2;
            printf("eor bit logic - value: %lld \n", result); //r
            break;
        case ANDS:
            result = rn & operand2;
            printf("ands bit logic - value: %lld \n", result); //r
            updateFlags(rn, operand2, result, ANDS, sf);
            break;
        case BICS:
            result = rn & ~operand2;
            printf("bics bit logic - value: %lld \n", result); //r
            updateFlags(rn, operand2, result, BICS, sf);
            break;
        default:
            return false;
    }

    // Store the result in the destination register
    writeReg(rdI, result, sf);
    return true;
}

//Handlers

//Immediate Handler (detects arithmetic/widemove inst) - passed all tests
bool immediateHandler(INST instruction) {

    INST opi = extractBits(instruction, 23, 25);

    if (patternMatch(opi, 0b010)) { //Arithmetic Instruction
        printf("this is an arithmetic instruction with an immediate value\n"); //r
        return arithmeticImmInstProcessor(instruction);
    } else if (patternMatch(opi, 0b101)) { //WideMove Instruction
        printf("this is an arithmetic instruction with an immediate value\n"); //r
        return wideMoveImmInstProcessor(instruction);
    } else {
        return false; //if error occurs
    }

}

//Register Handler
bool registerHandler(INST instruction) {
    printf("register handler\n"); //r
    // Extract fields from instruction
    INST m = extractBits(instruction, 28, 28);
    INST opr = extractBits(instruction, 21, 24);
    INST bit24 = extractBits(instruction, 24, 24);
    INST bit21 = extractBits(instruction, 21, 21);

    if (m && patternMatch(opr, 0b1000)) {
        return multiplyRegInstProcessor(instruction);
    }

    if (!m && bit24 && !bit21) {
        return arithmeticRegInstProcessor(instruction);
    }

    if (!m && !bit24) {
        return bitLogicRegInstProcessor(instruction);
    }

    // If none of the conditions are met, return false indicating an error
    return false;
}
>>>>>>> 31c2f6806900d41caae6e00ae79e488f76d93070

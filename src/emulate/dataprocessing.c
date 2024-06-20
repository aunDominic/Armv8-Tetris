//
// Created by Dominic Ng on 25/05/2024.
//

#include "dataprocessing.h"

#include <stdio.h>
// If you need more helper functions, use static functions
#include <assert.h>

#include "bitmanipulation.h"
#include "../debug.h"
#define ZERO_REGISTER 31  // Defining Zero Register

typedef enum { LSL = 0b00, LSR = 0b01, ASR = 0b10, ROR = 0b11 } ShiftType;  // Enumeration for shift types

// could change this function so it takes in a INST * instead and return void
// however i like being more explicit and allows instr to be used as a register by compiler potentially
// From assemble.c
static int64_t modify_bits(int64_t bits, const int64_t x, const int64_t y, const int64_t value) {
    // Calculate the number of bits to be modified

    assert(x <= y);
    int64_t num_bits = y - x + 1;

    PRINT("value is %u, max_val is %u\n", value, 1 << num_bits);
    assert(value < (1LL << num_bits));

    // Create a mask for the bits to be modified
    uint64_t mask = ((1LL << num_bits) - 1) << x;

    // Clear the bits in the original instruction
    bits &= ~mask;

    // Set the new value in the cleared bits
    bits |= (value << x) & mask;

    return bits;
}

// Extraction Functions - test passed
// static uint32_t extractBits(INST instruction, int start, int end) {
//     assert(start <= end);
//     uint32_t mask;
//     if (end - start == 31) {
//         mask = 0xFFFFFFFF; // Mask for all 32 bits
//     } else {
//         mask = ((1U << (end - start + 1)) - 1) << start;
//     }
//     return (instruction & mask) >> start;
// }
static bool patternMatch(INST bits, INST pattern) {
    return bits == pattern;
}

// Writing and Reading to Registers - test passed
static void writeReg(int reg, int64_t signedVal, int mode) {
    if (reg == ZERO_REGISTER) {
        PRINT("writing to zero register - null");  // r
    } else {
        PRINT("writing value %ld to register %d in %d mode\n", signedVal, reg, mode);  // r
        registers[reg] = (mode) ? signedVal : ((1LL << 32) - 1) & signedVal;
    }
}

static int64_t readReg(int reg, int mode) {
    if (reg == ZERO_REGISTER) {
        PRINT("reading from a zero register\n");
        return 0;
    } else {
        PRINT("reading from register %d in %d mode\n", reg, mode);
        return (mode) ? registers[reg] : (int32_t)(registers[reg]);
    }
}

// Shifter Functions - test passed
//  Mode 1 -> 64 bit
//  0 -> 32 bit
static int64_t lsl(int64_t value, int shift_amount, int mode) {
    if (mode) {
        return value << shift_amount;
    } else {
        value &= ((1LL << 32) - 1);
        int64_t result = value << shift_amount;
        result = ((1LL << 32) - 1) & result;
        return result;
    }
}

static int64_t lsr(uint64_t value, int shift_amount, int mode) {
    if (mode) {
        return (int64_t)(value >> shift_amount);
    } else {
        value &= ((1LL << 32) - 1);
        int64_t result = value >> shift_amount;
        result = ((1LL << 32) - 1) & result;
        return result;
    }
}

static int64_t asr(int64_t value, int shift_amount, int mode) {
    switch (mode) {
        case 0: {
            shift_amount &= 0x1f;
            uint32_t uval = (uint32_t)value;
            uint32_t result = (uval >> shift_amount) | -((uval & 0x80000000) >> shift_amount);
            return (int32_t)result;
        }
        case 1: {
            int signBit64 = (value < 0) ? 1 : 0;
            value >>= shift_amount;
            int64_t signExtension64 = (signBit64) ? ~((1LL << (64 - shift_amount)) - 1) : 0;
            return value | signExtension64;
        }
        default: return value;
    }
}

static int64_t ror(uint64_t value, int shift_amount, int mode) {
    shift_amount %= (mode ? 64 : 32);
    PRINT("ror\n");

    uint64_t fir = value >> shift_amount;
    uint64_t sec = (value << (64 - shift_amount));
    uint64_t res = fir | sec;
    switch (mode) {
        case 0:
            value &= ((1LL << 32) - 1);
            return ((value >> shift_amount) | (value << (32 - shift_amount))) & ((1LL << 32) - 1);
        case 1: PRINT("ROR %lx: %lx %lx %lx", value, fir, sec, res); return res;
        default: return value;
    }
}

// Updating pstate
static void updateFlags(int64_t a, int64_t b, int64_t result, int opc, int mode) {
    typedef enum { ADDS = 0b01, SUBS = 0b11, ANDS = 0b110, BICS = 0b111 } Code;

    pstate.N = (mode) ? (extractBits(result, 63, 63)) : (extractBits(result, 31, 31));
    pstate.Z = (mode) ? result == 0 : (((1LL << 32) - 1) & result) == 0;
    PRINT("Result: %lx\n", result);
    PRINT("N Flag: %d\n", pstate.N);

    switch ((Code)opc) {
        case ADDS: {
            pstate.C = (uint64_t)result < (uint64_t)a;
            pstate.V = (((a < 0) == (b < 0)) && ((result < 0) != (a < 0)));
            break;
        }

        case SUBS: {
            pstate.C = !((uint64_t)a < (uint64_t)b);
            pstate.V = ((a < 0) != (b < 0)) && ((result < 0) == (b < 0));
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

// Processor Logic

// Immediate Processor Logic Below
static bool arithmeticImmInstProcessor(INST instruction) {  // Unsure about how to set flags
    PRINT("Arithmetic immediate...\n");

    typedef enum { ADD = 0b00, ADDS = 0b01, SUB = 0b10, SUBS = 0b11 } Code;

    // Deconstructing Instruction
    int sf = extractBits(instruction, 31, 31);
    int opc = extractBits(instruction, 29, 30);
    int shiftFlag = extractBits(instruction, 22, 22);
    int imm12 = extractBits(instruction, 10, 21);
    int rnI = extractBits(instruction, 5, 9);
    int rdI = extractBits(instruction, 0, 4);
    if (shiftFlag) imm12 <<= 12;
    // Values for Arithmetic Instruction
    int64_t rn = readReg(rnI, sf);
    int64_t op2 = imm12;

    // Action
    switch ((Code)opc) {
        case ADD: {
            PRINT("add called\n");  // r
            int64_t result = rn + op2;
            writeReg(rdI, result, sf);
            break;
        }
        case ADDS: {
            PRINT("adds called\n");  // r
            int64_t result = rn + op2;
            writeReg(rdI, result, sf);
            updateFlags(rn, op2, result, opc, sf);
            break;
        }
        case SUB: {
            PRINT("sub called\n");  // r
            int64_t result = rn - op2;
            writeReg(rdI, result, sf);
            break;
        }
        case SUBS: {
            PRINT("subs called\n");  // r
            int64_t result = rn - op2;
            writeReg(rdI, result, sf);
            updateFlags(rn, op2, result, opc, sf);
            break;
        }
    }

    return true;
}

static bool wideMoveImmInstProcessor(INST instruction) {
    PRINT("Wide Move IMM called...\n");  // r

    typedef enum { MOVN = 0b00, MOVZ = 0b10, MOVK = 0b11 } Code;

    int sf = extractBits(instruction, 31, 31);
    int opc = extractBits(instruction, 29, 30);
    int hw = extractBits(instruction, 21, 22);
    assert(patternMatch(hw, 0b00) || patternMatch(hw, 0b01) || sf);
    int64_t imm16 = extractBits(instruction, 5, 20);
    int rdI = extractBits(instruction, 0, 4);

    int shiftValue = hw * 16;

    uint64_t op = imm16 << shiftValue;

    switch ((Code)opc) {
        case MOVN: {
            PRINT("movn - value: %lu - register.no: %d - mode: %d\n", ~op, rdI, sf);  // r
            writeReg(rdI, ~op, sf);
            break;
        }
        case MOVZ: {
            PRINT("movz - value: %lu - register.no: %x - mode: %d\n", op, rdI, sf);  // r
            writeReg(rdI, op, sf);
            break;
        }
        case MOVK: {
            PRINT("Shift value: %d\n", shiftValue);
            PRINT("Register: %lx\n", registers[rdI]);
            int64_t result = modify_bits(registers[rdI], shiftValue, shiftValue + 15, imm16);
            PRINT("Result: %lx\n", result);

            writeReg(rdI, result, sf);
            break;
        }
    }
    return true;
}

// Register Processor Logic Below
static bool multiplyRegInstProcessor(INST instruction) {
    PRINT("MultiplyReg Called!\n");

    typedef enum { MADD = 0b0, MSUB = 0b1 } MultiplyOpType;

    // Extract fields from instruction
    int sf = extractBits(instruction, 31, 31);
    int rmI = extractBits(instruction, 16, 20);
    int raI = extractBits(instruction, 10, 14);
    int x = extractBits(instruction, 15, 15);  // X field
    int rnI = extractBits(instruction, 5, 9);
    int rdI = extractBits(instruction, 0, 4);

    //    assert(!patternMatch(rdI, 31));

    // Read the values from the registers, handling zero register cases
    int64_t ra = readReg(raI, sf);
    int64_t rm = readReg(rmI, sf);
    int64_t rn = readReg(rnI, sf);

    int64_t result;

    // Perform the operation based on the op code
    switch ((MultiplyOpType)x) {
        case MADD:
            result = ra + (rn * rm);
            PRINT("MADD - value: %ld \n", result);  // r
            break;
        case MSUB:
            result = ra - (rn * rm);
            PRINT("MSUB bit logic - value: %ld \n", result);  // r
            break;
        default: return false;
    }

    // Store the result in the destination register
    writeReg(rdI, result, sf);
    return true;
}

static bool arithmeticRegInstProcessor(INST instruction) {
    PRINT("Arithmetic instruction...\n");

    typedef enum { ADD = 0b00, ADDS = 0b01, SUB = 0b10, SUBS = 0b11 } Code;

    // Extract fields from instruction
    int sf = extractBits(instruction, 31, 31);
    int opc = extractBits(instruction, 29, 30);
    int shiftType = extractBits(instruction, 22, 23);
    int rmI = extractBits(instruction, 16, 20);
    int shiftAmount = (sf) ? extractBits(instruction, 10, 15) : extractBits(instruction, 10, 14);
    int rnI = extractBits(instruction, 5, 9);
    int rdI = extractBits(instruction, 0, 4);

    // Check if destination register is invalid
    //    assert(!patternMatch(rdI, 0b11111));

    // Read the values from the registers
    int64_t rm = readReg(rmI, sf);
    int64_t rn = readReg(rnI, sf);
    int64_t operand2;

    // Perform the shift operation
    switch ((ShiftType)shiftType) {
        case LSL: operand2 = lsl(rm, shiftAmount, sf); break;
        case LSR: operand2 = lsr(rm, shiftAmount, sf); break;
        case ASR: operand2 = asr(rm, shiftAmount, sf); break;
        case ROR: operand2 = ror(rm, shiftAmount, sf); break;
        default: return false;
    }

    // Action
    switch ((Code)opc) {
        case ADD: {
            PRINT("add called!\n");  // r
            int64_t result = rn + operand2;
            writeReg(rdI, result, sf);
            break;
        }
        case ADDS: {
            PRINT("adds called!\n");  // r
            int64_t result = rn + operand2;
            writeReg(rdI, result, sf);
            updateFlags(rn, operand2, result, opc, sf);
            break;
        }
        case SUB: {
            PRINT("sub called!\n");  // r
            int64_t result = rn - operand2;
            writeReg(rdI, result, sf);
            break;
        }
        case SUBS: {
            PRINT("subs called!\n");  // r
            int64_t result = rn - operand2;
            writeReg(rdI, result, sf);
            updateFlags(rn, operand2, result, opc, sf);
            break;
        }
    }

    return true;
}

static bool bitLogicRegInstProcessor(INST instruction) {
    PRINT("Bit Logic asdasdcalled!\n");

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

    // Read the values from the registers
    int64_t rm = readReg(rmI, sf);
    int64_t rn = readReg(rnI, sf);
    int64_t operand2;

    // Perform the shift operation
    PRINT("shift type: %d\n", shiftType);
    switch ((ShiftType)shiftType) {
        case LSL: operand2 = lsl(rm, shiftAmount, sf); break;
        case LSR: operand2 = lsr(rm, shiftAmount, sf); break;
        case ASR: operand2 = asr(rm, shiftAmount, sf); break;
        case ROR:
            PRINT("ror\n");
            operand2 = ror(rm, shiftAmount, sf);
            break;
        default: PRINT("false\n"); return false;
    }

    PRINT("Operand2: %lx\n", operand2);
    PRINT("rmI: %x\n", rmI);
    INST code = (opc << 1) + n;

    // Perform the operation based on the opcode
    int64_t result;
    switch ((Code)code) {
        case AND:
            result = rn & operand2;
            PRINT("and bit logic - value: %ld \n", result);  // r
            break;
        case BIC:
            result = rn & ~operand2;
            PRINT("bic bit logic - value: %ld \n", result);  // r
            break;
        case ORR:
            result = rn | operand2;
            PRINT("orr bit logic - value: %ld \n", result);  // r
            break;
        case ORN:
            result = rn | ~operand2;
            PRINT("orn bit logic - value: %ld \n", result);  // r
            break;
        case EOR:
            result = rn ^ operand2;
            PRINT("eor bit logic - value: %ld \n", result);  // r
            break;
        case EON:
            result = rn ^ ~operand2;
            PRINT("eor bit logic - value: %ld \n", result);  // r
            break;
        case ANDS:
            result = rn & operand2;
            PRINT("ands bit logic - value: %ld \n", result);  // r
            updateFlags(rn, operand2, result, ANDS, sf);
            break;
        case BICS:
            PRINT("rn: %lx\n", rn);
            result = rn & ~operand2;
            PRINT("bics bit logic - value: %ld \n", result);  // r
            updateFlags(rn, operand2, result, BICS, sf);
            break;
        default: return false;
    }

    // Store the result in the destination register
    writeReg(rdI, result, sf);
    return true;
}

// Handlers

// Immediate Handler (detects arithmetic/widemove inst) - passed all tests
bool immediateHandler(INST instruction) {
    PRINT("Immediate called...\n");
    INST opi = extractBits(instruction, 23, 25);

    if (patternMatch(opi, 0b010)) {      // Arithmetic Instruction
        PRINT("Arithmetic Called!\n");  // r
        return arithmeticImmInstProcessor(instruction);
    } else if (patternMatch(opi, 0b101)) {  // WideMove Instruction
        PRINT("Wide move called!\n");      // r
        return wideMoveImmInstProcessor(instruction);
    } else {
        return false;  // if error occurs
    }
}

// Register Handler
bool registerHandler(INST instruction) {
    PRINT("Register called...\n");  // r
    // Extract fields from instruction
    INST m = extractBits(instruction, 28, 28);
    INST opr = extractBits(instruction, 21, 24);
    INST bit24 = extractBits(instruction, 24, 24);
    INST bit21 = extractBits(instruction, 21, 21);

    if (m && patternMatch(opr, 0b1000)) {
        PRINT("Multiply called!\n");
        return multiplyRegInstProcessor(instruction);
    }

    if (!m && bit24 && !bit21) {
        PRINT("ArithmeticReg called!\n");
        return arithmeticRegInstProcessor(instruction);
    }

    if (!m && !bit24) {
        PRINT("Logic called!\n");
        return bitLogicRegInstProcessor(instruction);
    }

    // If none of the conditions are met, return false indicating an error
    return false;
}

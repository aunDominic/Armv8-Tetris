//
// Created by Dominic Ng on 25/05/2024.
//

//Okay so potential bug found is when we extraact bits we are returning a signed integer,
// so when we are doing the pattern matching we have to ensure that we have decoded the pattern properly
// i.e if the pattern has a 1 at the start then it would be negative?

#include "dataprocessing.h"

// If you need more helper functions, use static functions
#include <stdio.h>
#include <assert.h>
#define ZERO_REGISTER 31

// Enumeration for shift types
typedef enum {
    LSL = 0b00,
    LSR = 0b01,
    ASR = 0b10,
    ROR = 0b11
} ShiftType;

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

//Immediate Handler and Processor Logic Below

//Updating pstate

static void updateFlags(int64_t a, int64_t b, int64_t result, uint32_t opc, int mode) {

    pstate.N = (mode) ? (extractBits(result, 63, 63)) : (extractBits(result, 31, 31));
    pstate.Z = result == 0;

}

//Processing Arithmetic Instructions with an Unsgined 12 Bit Immediate Value

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
    int hw    = extractBits(instruction, 21, 22);
    assert(patternMatch(hw, 0b00) || patternMatch(hw, 0b01) || sf);
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

//Register Handler and Processor Logic Below

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

    int mode = sf;

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



//Handler for instructions with register values

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

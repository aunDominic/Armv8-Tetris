/**
 * @file util.c
 * @brief ARMv8 Instruction Set Module Implementation
 * @author Andrei Cravtov
 *
 * TODO: document this implementation
 */

#include "instruction_set.h"

#include "util.h"

// ---- TYPE DEFINITIONS ----
enum InstructionGroup {
 DATA_PROCESSING_IMMEDIATE,
 DATA_PROCESSING_REGISTER,
 SINGLE_DATA_TRANSFER,
 LOAD_LITERAL,
 BRANCH
};

enum RegisterWidth {
 REG_64_BIT, REG_32_BIT
};

enum DataProcessingOperaionType {
 ARITHMETIC, WIDE_MOVE
};

enum ArithmeticOpcode {
 ADD, ADDS, SUB, SUBS
};

enum AddressableRegister {
 // General purpose
 REG_R0, REG_R1, REG_R2, REG_R3, REG_R4, REG_R5, REG_R6, REG_R7, REG_R8, REG_R9,
 REG_R10, REG_R11, REG_R12, REG_R13, REG_R14, REG_R15, REG_R16, REG_R17, REG_R18, REG_R19,
 REG_R20, REG_R21, REG_R22, REG_R23, REG_R24, REG_R25, REG_R26, REG_R27, REG_R28, REG_R29,
 REG_R30,

 // Special purpose
 REG_ZP
};

// ---- VARIABLE DEFINITIONS ----

// ---- HELPER METHODS ----

// ---- INTERFACE IMPLEMENTATION ----
static void data_processing_immediate_wide_move(u32_t binary_instruction) {
 // 31 | 30-29 | 28-23  | 22-21 | 20-5  | 4-0
 // sf | opc   | 100101 | hw    | imm16 | rd

 // sf | register width
 // 0  | 32-bit
 // 1  | 64-bit
 enum RegisterWidth register_width =
   is_bit_set(binary_instruction, 31)
    ? REG_64_BIT
    : REG_32_BIT;

 // opc | instruction
 // -----------------
 // 00  | movn (Rd := ~Op)
 // 01  | movz (Rd := Op)
 // 10  | movk (Rd[shift+15:shift] := imm16)

 // hw: logical shift left by hw*16 bits, i.e. imm16 << (hw*16)
 // for, 32-bit mode hw is either 00 or 01

 // imm16: 16-bit immediate value to move

 // rd: the encoding of which register is destination
 // 11111 is zero reg
 // anything else is numberic general purpose
}

static void data_processing_immediate_arithmetic(u32_t binary_instruction) {
 // 31 | 30-29 | 28-23  | 22 | 21-10 | 9-5 | 4-0
 // sf | opc   | 100010 | sh | imm12 | rn  | rd

 // sf | register width
 // 0  | 32-bit
 // 1  | 64-bit
 enum RegisterWidth register_width =
   is_bit_set(binary_instruction, 31)
    ? REG_64_BIT
    : REG_32_BIT;

 // opc | instruction
 // -----------------
 // 00  | add  (Rd := Rn + Op2)
 // 01  | adds (Rd := Rn + Op2, update PSTATE)
 // 10  | sub  (Rd := Rn - Op2)
 // 11  | subs (Rd := Rn - Op2, update PSTATE)

 // sh: left shift imm12 by 12 bits or not
 // imm12: 12-bit unsigned immediate value

 // rd, rd: the encoding of which registers
 // 11111 is zero reg
 // anything else is numberic general purpose
 // maybe make stack pointer register addressable???
}

static void data_processing_immediate(u32_t binary_instruction) {
 // 31 | 30-29 | 28-26 | 25-23 | 22-5    | 4-0
 // sf | opc   | 100   | opi   | operand | rd

 // opi | data processing operaion type
 // -----------------------------------
 // 010 | arithmetic operation
 // 101 | wide move
 if (is_bit_set(binary_instruction, 25)) {
  data_processing_immediate_wide_move(binary_instruction);
 } else {
  data_processing_immediate_arithmetic(binary_instruction);
 }
}

static void data_processing_register_arithmetic(u32_t binary_instruction) {
 // 31 | 30-29 | 28-24 | 23-22 | 21 | 20-16 | 15-10    | 9-5 | 4-0
 // sf | opc   | 01011 | shift | 0  | rm    | operand  | rn  | rd

 // sf | register width
 // 0  | 32-bit
 // 1  | 64-bit
 enum RegisterWidth register_width =
   is_bit_set(binary_instruction, 31)
    ? REG_64_BIT
    : REG_32_BIT;

 // opc | instruction
 // -----------------
 // 00  | add  (Rd := Rn + Op2)
 // 01  | adds (Rd := Rn + Op2, update PSTATE)
 // 10  | sub  (Rd := Rn - Op2)
 // 11  | subs (Rd := Rn - Op2, update PSTATE)

 // shift | shift type to perform on Rm
 // ------------------
 // 00    | lsl
 // 01    | lsr
 // 10    | asr

 // operand: 6-bit unsigned immediate value determining the shift amount
 // for 32-bit variant, this is in the range 0-31. For 64-bit variant, the range is 0-63

 // rm, rn, rd: the encoding of which registers
 // 11111 is zero reg
 // anything else is numberic general purpose
 // maybe make stack pointer register addressable???
}

static void data_processing_register_bit_logic(u32_t binary_instruction) {
 // 31 | 30-29 | 28-24 | 23-22 | 21 | 20-16 | 15-10   | 9-5 | 4-0
 // sf | opc   | 01010 | shift | N  | rm    | operand | rn  | rd

 // sf | register width
 // 0  | 32-bit
 // 1  | 64-bit
 enum RegisterWidth register_width =
   is_bit_set(binary_instruction, 31)
    ? REG_64_BIT
    : REG_32_BIT;

 // opc N | instruction
 // -----------------
 // 00 0  | and  (Rd := Rn & Op2)
 // 00 1  | bic  (Rd := Rn & ~Op2)
 // 01 0  | orr  (Rd := Rn | Op2)
 // 01 1  | orn  (Rd := Rn | ~Op2)
 // 10 0  | eor  (Rd := Rn ^ Op2)
 // 10 1  | eon  (Rd := Rn ^ ~Op2)
 // 11 0  | ands (Rd := Rn & Op2, update PSTATE)
 // 11 1  | bics (Rd := Rn & ~Op2, update PSTATE)

 // shift | shift type to perform on Rm
 // ------------------
 // 00    | lsl
 // 01    | lsr
 // 10    | asr
 // 11    | ror

 // N: whether the shifted register is to be bitwise negated

 // operand: 6-bit unsigned immediate value determining the shift amount
 // for 32-bit variant, this is in the range 0-31. For 64-bit variant, the range is 0-63

 // rm, rn, rd: the encoding of which registers
 // 11111 is zero reg
 // anything else is numberic general purpose
}

static void data_processing_register_multiply(u32_t binary_instruction) {
 // 31 | 30-21      | 20-16 | 15 | 14-10 | 9-5 | 4-0
 // sf | 0011011000 | rm    | x  | ra    | rn  | rd

 // sf | register width
 // 0  | 32-bit
 // 1  | 64-bit
 enum RegisterWidth register_width =
   is_bit_set(binary_instruction, 31)
    ? REG_64_BIT
    : REG_32_BIT;

 // opc=00 and opr=1000, implicitly

 // x | instruction
 // 0 | madd (Rd := Ra + (Rn*Rm))
 // 1 | msub (Rd := Ra - (Rn*Rm))

 // ra, rm, rn, rd: the encoding of which registers
 // 11111 is zero reg
 // anything else is numberic general purpose
 // maybe make stack pointer register addressable???
}


static void data_processing_register(u32_t binary_instruction) {
 // 31 | 30-29 | 28 | 27-25 | 24-21 | 20-16 | 15-10   | 9-5 | 4-0
 // sf | opc   | M  | 101   | opr   | rm    | operand | rn  | rd

 // M opr  | data processing operaion type
 // 0 1xx0 | arithmetic
 // 0 0xxx | bit-logic
 // 1 1000 | multiply
 if (is_bit_set(binary_instruction, 28)) {
  data_processing_register_multiply(binary_instruction);
 } else {
  if (is_bit_set(binary_instruction, 24)) {
   data_processing_register_arithmetic(binary_instruction);
  } else {
   data_processing_register_bit_logic(binary_instruction);
  }
 }
}

static void loads_and_stores(u32_t binary_instruction) {
 // load literal to register,
 // or load memory to register
 // or store register to memory

 // Single Data Transfer: 31 | 30 | 29-22    | 21-10  | 9-5 | 4-0
 //                       1  | sf | 1110010L | offset | xn  | rt

 // Load Literal: 31 | 30 | 29-24  | 23-5   | 4-0
 //               0  | sf | 011000 | simm19 | rt
 //Literal (Load only): <literal>. A literal can be either an integer, #N, or a label.
 // The address represented by the integer or label must be within 1MB of the location of this instruction
 // (as it is encoded as an offset), and must be 4-byte aligned. The assembler should compute the offset
}

static void branches(u32_t binary_instruction) {
 // LITERAL ENCODING:
 // The encoding of the branch to literal instructions doesn’t store the value of the literal,
 // it instead stores the offset from the address of the instruction to the literal.


 // (Unconditional) simm26: The offset to apply to the PC is encoded as simm26 ∗ 4 (sign extended to 64-bit)

 // (Register) xn: The encoding of Xn, the register containing the address to jump to. When xn is 11111,
 // this encodes the zero register, xzr; you don’t need to handle this case

 // (Conditional) simm19: The offset to apply to the PC is encoded as simm19 ∗ 4 (sign extended to 64-bit)

 // (Conditional) cond: a 4-bit encoding of the condition that must be satisfied for the branch to be executed
 // cond | meaning | PSTATE Flags
 // -----------------------------
 // 0000 | EQ      | Z = 1
 // 0001 | NE      | Z = 0
 // 1010 | GE      | N = V
 // 1011 | LT      | N != V
 // 1100 | GT      | (Z = 0) and (N = V)
 // 1101 | LE      | not [(Z = 0) and (N = V)]
 // 1110 | AL      | any

 // When encoding the offset (divided by 4) into 19 or 26 bits, the sign bit must be preserved to ensure the
 // correct value is encoded. When decoding an offset, care must be taken to properly sign extend it to 64-bit.

 // instruction | meaning
 // ---------------------
 // b           | PC := PC + offset
 // br          | PC := Xn
 // b.cond      | If cond, PC := PC + offset

 // ------------

 // Unconditional: 31-30 | 29-26 | 25-0
 //                00    | 0101  | simm26

 // Register:      31-10                  | 9-5 | 4-0
 //                1101011000011111000000 | xn  | 00000

 // Conditional:   31-24    | 23-5   | 4 | 3-0
 //                01010100 | simm19 | 0 | cond
}

static void test(const u32_t binary_instruction) {
 // TODO: HALT instruction handling
 // 10001010000000000000000000000000
 // = 8a000000
 // = 'and x0 x0 x0'

 // extract the bits: op0 = (28, 27, 26, 25)
 const bool bit_27 = is_bit_set(binary_instruction, 27);
 const bool bit_26 = is_bit_set(binary_instruction, 26);
 const bool bit_25 = is_bit_set(binary_instruction, 25);

 // op0  | Group
 // ----------------------------------
 // 100x | Data Processing (Immediate)
 // x101 | Data Processing (Register)
 // x1x0 | Loads and Stores
 // 101x | Branches

 if (bit_27) {
  // op0  | Group
  // ----------------------------------
  // x101 | Data Processing (Register)
  // x1x0 | Loads and Stores

  if (bit_25) {
   // op0  | Group
   // ----------------------------------
   // x101 | Data Processing (Register)

   data_processing_register(binary_instruction);
  } else {
   // op0  | Group
   // ----------------------------------
   // x1x0 | Loads and Stores

   loads_and_stores(binary_instruction);
  }
 } else {
  // op0  | Group
  // ----------------------------------
  // 100x | Data Processing (Immediate)
  // 101x | Branches

  if (bit_26) {
   // op0  | Group
   // ----------------------------------
   // 101x | Branches

   branches(binary_instruction);
  } else {
   // op0  | Group
   // ----------------------------------
   // 100x | Data Processing (Immediate)

   data_processing_immediate(binary_instruction);
  }
 }
}

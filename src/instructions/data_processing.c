/**
 * @file data_processing.c
 * @brief ARMv8 Data Processing Instructions Module Implementation
 * @author Andrei Cravtov
 *
 * TODO: document this implementation
 */

#include "data_processing.h"

#include <assert.h>
#include <stdio.h>

#include "../util.h"

// ---- TYPE DEFINITIONS ----

// ---- VARIABLE DEFINITIONS ----

// ---- HELPER METHODS ----

/**
 * Parses a (little endian) 32-bit binary instruction into an immediate ArithmeticInstruction instance
 * @param binary_instruction the 32-bit binary instruction
 * @return pointer to immediate ArithmeticInstruction instance, or NULL if parsing failed
 */
static struct ArithmeticInstruction *immediate_arithmetic_instruction_from_binary(const u32_t binary_instruction) {
 // 31 | 30-29 | 28-23  | 22 | 21-10 | 9-5 | 4-0
 // sf | opc   | 100010 | sh | imm12 | rn  | rd

 // sf | register width
 // 0  | 32-bit
 // 1  | 64-bit
 const bool register_width = is_bit_set(binary_instruction, 31);

 // opc (30, 29) | instruction
 // ---------------------------------------------------
 //      0,  0   | add  (Rd := Rn + Op2)
 //      0,  1   | adds (Rd := Rn + Op2, update PSTATE)
 //      1,  0   | sub  (Rd := Rn - Op2)
 //      1,  1   | subs (Rd := Rn - Op2, update PSTATE)
 const bool instruction_type = is_bit_set(binary_instruction, 30);
 const bool set_flags = is_bit_set(binary_instruction, 29);

 // left shift imm12 by 12 bits or not
 const bool sh = is_bit_set(binary_instruction, 22);

 // extract imm12 (mask and right shift), which is a 12-bit unsigned immediate value
 const u16_t imm12 = (0b00000000001111111111110000000000 & binary_instruction) >> 10;

 // extract rn, rd (mask and right shift): the encoding of input and destination registers
 const u8_t rn = (0b0000001111100000 & binary_instruction) >> 5;
 const u8_t rd = 0b00011111 & binary_instruction;

 // construct arithmetic instruction, and return pointer to it
 struct ArithmeticInstruction arithmetic_instruction = {
  // check the comment docs of this field,
  // the enum values should align with the provided boolean
  .register_width = register_width,

  .instruction = {
   .set_flags = set_flags,

   // check the comment docs of this field,
   // the enum values should align with the provided boolean
   .instruction_type = instruction_type
  },

  .operand = {
   .operand_type = IMMEDIATE_OPERAND,
   .operand_value = {
    .immediate_operand_value = {
     .rd = rn,
     .rn = rd,
     .imm12 = imm12,
     .apply_lsl_imm12 = sh
    }
   }
  },
 };
 return &arithmetic_instruction;
}

/**
 * Parses a (little endian) 32-bit binary instruction into a register ArithmeticInstruction instance
 * @param binary_instruction the 32-bit binary instruction
 * @return pointer to register ArithmeticInstruction instance, or NULL if parsing failed
 */
static struct ArithmeticInstruction *register_arithmetic_instruction_from_binary(const u32_t binary_instruction) {
 // 31 | 30-29 | 28-24 | 23-22 | 21 | 20-16 | 15-10    | 9-5 | 4-0
 // sf | opc   | 01011 | shift | 0  | rm    | operand  | rn  | rd

 // sf | register width
 // 0  | 32-bit
 // 1  | 64-bit
 const bool register_width = is_bit_set(binary_instruction, 31);

 // opc (30, 29) | instruction
 // ---------------------------------------------------
 //      0,  0   | add  (Rd := Rn + Op2)
 //      0,  1   | adds (Rd := Rn + Op2, update PSTATE)
 //      1,  0   | sub  (Rd := Rn - Op2)
 //      1,  1   | subs (Rd := Rn - Op2, update PSTATE)
 const bool instruction_type = is_bit_set(binary_instruction, 30);
 const bool set_flags = is_bit_set(binary_instruction, 29);

 // shift (23, 22) | shift type to perform on Rm
 // --------------------------------------------
 //        0,  0   | logical shift left
 //        0,  1   | logical shift right
 //        1,  0   | arithmetic shift right
 // extract value of shift (by bit manipulation)
 const u8_t shift = (is_bit_set(23, binary_instruction) << 1) | is_bit_set(22, binary_instruction);
 if (shift == 0b11)
  // if unsupported shitft type encountered, abort the parsing
  return NULL;

 // extract operand (mask and right shift)
 // which is 6-bit unsigned immediate value determining the shift amount
 const u8_t operand = (0b00000000000000001111110000000000 & binary_instruction) >> 10;
 // if register_width = false (i.e. 32-bit), then restrict range to 0-31
 if (!register_width & operand >= 32)
  // if incorrect range in operand, abort the parsing
  return NULL;

 // extract rm, rn, rd (mask and right shift): the encoding of input and destination registers
 const u8_t rm = (0b00000000000111110000000000000000 & binary_instruction) >> 16;
 const u8_t rn = (0b0000001111100000 & binary_instruction) >> 5;
 const u8_t rd = 0b00011111 & binary_instruction;

 // construct arithmetic instruction, and return pointer to it
 struct ArithmeticInstruction arithmetic_instruction = {
  // check the comment docs of this field,
  // the enum values should align with the provided boolean
  .register_width = register_width,

  .instruction = {
   .set_flags = set_flags,

   // check the comment docs of this field,
   // the enum values should align with the provided boolean
   .instruction_type = instruction_type
  },

  .operand = {
   .operand_type = REGISTER_OPERAND,
   .operand_value = {
    .register_operand_value = {
     .rd = rd,
     .rn = rn,
     .rm = rm,

     // check the comment docs of this field,
     // the enum values should align with the provided boolean
     .shift_type = shift,

     .operand = operand
    }
   }
  },
 };
 return &arithmetic_instruction;
}

/**
 * Parses a (little endian) 32-bit binary instruction into an immediate DataProcessingInstruction instance
 * @param binary_instruction the 32-bit binary instruction
 * @return pointer to immediate DataProcessingInstruction instance, or NULL if parsing failed
 */
static struct DataProcessingInstruction *immediate_instruction_from_binary(u32_t binary_instruction) {
 // extract the bits: opi = (25, 24, 23)
 const bool bit_25 = is_bit_set(binary_instruction, 25);
 const bool bit_24 = is_bit_set(binary_instruction, 24);
 const bool bit_23 = is_bit_set(binary_instruction, 23);
 // 31 | 30-29 | 28-26 | 25-23 | 22-5    | 4-0
 // sf | opc   | 100   | opi   | operand | rd

 // opi | Group
 // ----------------
 // 010 | Arithmetic
 if (!bit_25 && bit_24 && !bit_23) {
  // attempt to construct arithmetic instruction
  const struct ArithmeticInstruction *arithmetic_instruction =
    immediate_arithmetic_instruction_from_binary(binary_instruction);

  // if arithmetic instruction cannot be constructed,
  // then the overall instruction cannot be constructed
  if (arithmetic_instruction == NULL) return NULL;

  // construct data processing instruction, and return pointer
  struct DataProcessingInstruction data_processing_instruction = {
   .instruction_type = DATA_PROCESSING_ARITHMETIC,
   .instruction_data = {.arithmetic_instruction = *arithmetic_instruction}
  };
  return &data_processing_instruction;
 }

 // opi | Group
 // ----------------
 // 101 | Wide-Move
 if (bit_25 && !bit_24 && bit_23) {
  // attempt to construct wide-move instruction
  const struct WideMoveInstruction *wide_move_instruction =
    wide_move_instruction_from_binary(binary_instruction);

  // if wide-move instruction cannot be constructed,
  // then the overall instruction cannot be constructed
  if (wide_move_instruction == NULL) return NULL;

  // construct data processing instruction, and return pointer
  struct DataProcessingInstruction data_processing_instruction = {
   .instruction_type = DATA_PROCESSING_WIDE_MOVE,
   .instruction_data = {.wide_move_instruction = *wide_move_instruction}
  };
  return &data_processing_instruction;
 }

 // anything else is not an immediate instruction
 return NULL;
}

/**
 * Parses a (little endian) 32-bit binary instruction into a register DataProcessingInstruction instance
 * @param binary_instruction the 32-bit binary instruction
 * @return pointer to register DataProcessingInstruction instance, or NULL if parsing failed
 */
static struct DataProcessingInstruction *register_instruction_from_binary(u32_t binary_instruction) {
 // extract the bits: M = 28, opr = (24, 23, 22, 21)
 const bool M = is_bit_set(binary_instruction, 28);
 const bool bit_24 = is_bit_set(binary_instruction, 24);
 const bool bit_23 = is_bit_set(binary_instruction, 23);
 const bool bit_22 = is_bit_set(binary_instruction, 22);
 const bool bit_21 = is_bit_set(binary_instruction, 21);
 // 31 | 30-29 | 28 | 27-25 | 24-21 | 20-16 | 15-10   | 9-5 | 4-0
 // sf | opc   | M  | 101   | opr   | rm    | operand | rn  | rd

 // M opr  | Group
 // -----------------
 // 1 1000 | multiply
 if (M && bit_24 && !bit_23 && !bit_22 && !bit_21) {
  // attempt to construct multiply instruction
  const struct MultiplyInstruction *multiply_instruction =
    multiply_instruction_from_binary(binary_instruction);

  // if multiply instruction cannot be constructed,
  // then the overall instruction cannot be constructed
  if (multiply_instruction == NULL) return NULL;

  // construct data processing instruction, and return pointer
  struct DataProcessingInstruction data_processing_instruction = {
   .instruction_type = DATA_PROCESSING_MULTIPLY,
   .instruction_data = {.multiply_instruction = *multiply_instruction}
  };
  return &data_processing_instruction;
 }

 // M opr  | Group
 // -----------------
 // 0 1xx0 | arithmetic
 if (!M && bit_24 && !bit_21) {
  // attempt to construct arithmetic instruction
  const struct ArithmeticInstruction *arithmetic_instruction =
    register_arithmetic_instruction_from_binary(binary_instruction);

  // if arithmetic instruction cannot be constructed,
  // then the overall instruction cannot be constructed
  if (arithmetic_instruction == NULL) return NULL;

  // construct data processing instruction, and return pointer
  struct DataProcessingInstruction data_processing_instruction = {
   .instruction_type = DATA_PROCESSING_ARITHMETIC,
   .instruction_data = {.arithmetic_instruction = *arithmetic_instruction}
  };
  return &data_processing_instruction;
 }

 // M opr  | data processing operaion type
 // --------------------------------------
 // 0 0xxx | bit-logic
 if (!M && !bit_24) {
  // attempt to construct bit-logic instruction
  const struct LogicalInstruction *logical_instruction =
    logical_instruction_from_binary(binary_instruction);

  // if bit-logic instruction cannot be constructed,
  // then the overall instruction cannot be constructed
  if (logical_instruction == NULL) return NULL;

  // construct data processing instruction, and return pointer
  struct DataProcessingInstruction data_processing_instruction = {
   .instruction_type = DATA_PROCESSING_LOGICAL,
   .instruction_data = {.logical_instruction = *logical_instruction}
  };
  return &data_processing_instruction;
 }

 // anything else is not a register instruction
 return NULL;
}

// ---- INTERFACE IMPLEMENTATION ----

bool data_processing_immediate(const struct DataProcessingInstruction *instruction) {
 // no null pointers
 assert(instruction != NULL);

 // wide move is immediate
 if (instruction->instruction_type == DATA_PROCESSING_WIDE_MOVE) return true;

 // arithmetic could be immediate
 if (instruction->instruction_type == DATA_PROCESSING_ARITHMETIC
     && instruction->instruction_data.arithmetic_instruction.operand.operand_type == IMMEDIATE_OPERAND)
  return true;

 // everything else is not immediate
 return false;
}

bool data_processing_register(const struct DataProcessingInstruction *instruction) {
 // no null pointers
 assert(instruction != NULL);

 // logical and multiply are of type register
 if (instruction->instruction_type == DATA_PROCESSING_LOGICAL
     || instruction->instruction_type == DATA_PROCESSING_MULTIPLY)
  return true;

 // arithmetic could be of type register
 if (instruction->instruction_type == DATA_PROCESSING_ARITHMETIC
     && instruction->instruction_data.arithmetic_instruction.operand.operand_type == REGISTER_OPERAND)
  return true;

 // everything else is not of type register
 return false;
}

struct ArithmeticInstruction *arithmetic_instruction_from_binary(const u32_t binary_instruction) {
 // extract the bits: op0 = (28, 27, 26, 25)
 const bool bit_28 = is_bit_set(binary_instruction, 28);
 const bool bit_27 = is_bit_set(binary_instruction, 27);
 const bool bit_26 = is_bit_set(binary_instruction, 26);
 const bool bit_25 = is_bit_set(binary_instruction, 25);

 // op0  | Group
 // ----------------------------------
 // 100x | Data Processing (Immediate)
 if (bit_28 && !bit_27 && !bit_26) {
  // extract the bits: opi = (25, 24, 23)
  const bool bit_24 = is_bit_set(binary_instruction, 24);
  const bool bit_23 = is_bit_set(binary_instruction, 23);
  // 31 | 30-29 | 28-26 | 25-23 | 22-5    | 4-0
  // sf | opc   | 100   | opi   | operand | rd

  // opi | Group
  // ----------------
  // 010 | Arithmetic
  if (!bit_25 && bit_24 && !bit_23) {
   return immediate_arithmetic_instruction_from_binary(binary_instruction);
  }

  // anything else is not an arithmetic instruction
  return NULL;
 }

 // op0  | Group
 // ----------------------------------
 // x101 | Data Processing (Register)
 if (bit_27 && !bit_26 && bit_25) {
  // extract the bits: M, opr = (24, 23, 22, 21)
  const bool M = bit_28;
  const bool bit_24 = is_bit_set(binary_instruction, 24);
  const bool bit_21 = is_bit_set(binary_instruction, 21);
  // 31 | 30-29 | 28 | 27-25 | 24-21 | 20-16 | 15-10   | 9-5 | 4-0
  // sf | opc   | M  | 101   | opr   | rm    | operand | rn  | rd

  // M opr  | Group
  // -----------------
  // 0 1xx0 | arithmetic
  if (!M && bit_24 && !bit_21) {
   return register_arithmetic_instruction_from_binary(binary_instruction);
  }

  // anything else is not an arithmetic instruction
  return NULL;
 }

 // anything else is not a data processing instruction
 return NULL;
}

struct WideMoveInstruction *wide_move_instruction_from_binary(const u32_t binary_instruction) {
 // extract the bits: op0 = (28, 27, 26, 25)
 const bool bit_28 = is_bit_set(binary_instruction, 28);
 const bool bit_27 = is_bit_set(binary_instruction, 27);
 const bool bit_26 = is_bit_set(binary_instruction, 26);

 // op0  | Group
 // ----------------------------------
 // 100x | Data Processing (Immediate)
 if (!(bit_28 && !bit_27 && !bit_26))
  // if encountered non-immediate instruction, abort decoding
  return NULL;

 // extract the bits: opi = (25, 24, 23)
 const bool bit_25 = is_bit_set(binary_instruction, 25);
 const bool bit_24 = is_bit_set(binary_instruction, 24);
 const bool bit_23 = is_bit_set(binary_instruction, 23);
 // 31 | 30-29 | 28-26 | 25-23 | 22-5    | 4-0
 // sf | opc   | 100   | opi   | operand | rd

 // opi | Group
 // ----------------
 // 101 | Wide-Move
 if (!(bit_25 && !bit_24 && bit_23))
  // if encountered non wide-move instruction, abort decoding
  return NULL;

 // 31 | 30-29 | 28-23  | 22-21 | 20-5  | 4-0
 // sf | opc   | 100101 | hw    | imm16 | rd

 // sf | register width
 // 0  | 32-bit
 // 1  | 64-bit
 const bool register_width = is_bit_set(binary_instruction, 31);

 // opc (30, 29) | instruction
 // ---------------------------------------------------
 //      0,  0   | movn (Rd := ~Op)
 //      0,  1   | movz (Rd := Op)
 //      1,  0   | movk (Rd[shift+15:shift] := imm16)
 const u8_t instruction = (is_bit_set(binary_instruction, 30) << 1) | is_bit_set(binary_instruction, 29);
 if (instruction == 0b11)
  // if unsupported operation encountered, abort the parsing
  return NULL;

 // hw (22, 21): logical shift left by hw*16 bits, i.e. imm16 << (hw*16)
 const u8_t hw = (is_bit_set(binary_instruction, 22) << 1) | is_bit_set(binary_instruction, 21);
 // if register_width = false (i.e. 32-bit), then restrict hw to 0x00 or 0x01
 if (!register_width && !(hw == 0b00 || hw == 0b01))
  // if unsupported hw encountered, abort the parsing
  return NULL;

 // extract imm16 (mask and right shift) 16-bit immediate value to move
 const u16_t imma16 = (0b00000000000111111111111111100000 & binary_instruction) >> 5;

 // extract rd (bit-mask): the encoding of destination register
 const u8_t rd = 0b00011111 & binary_instruction;

 // construct wide-move instruction, and return pointer to it
 struct WideMoveInstruction wide_move_instruction = {
  // check the comment docs of this field,
  // the enum values should align with the provided boolean
  .register_width = register_width,

  // check the comment docs of this field,
  // the enum values should align with the provided boolean
  .instruction = instruction,

  .rd = rd,
  .operand = {
   .imm16 = imma16,
   .lsl_multiplier = hw
  },
 };
 return &wide_move_instruction;
}

struct LogicalInstruction *logical_instruction_from_binary(const u32_t binary_instruction) {
 // extract the bits: op0 = (28, 27, 26, 25)
 const bool bit_28 = is_bit_set(binary_instruction, 28);
 const bool bit_27 = is_bit_set(binary_instruction, 27);
 const bool bit_26 = is_bit_set(binary_instruction, 26);
 const bool bit_25 = is_bit_set(binary_instruction, 25);

 // op0  | Group
 // ----------------------------------
 // x101 | Data Processing (Register)
 if (!(bit_27 && !bit_26 && bit_25))
  // if encountered non-register instruction, abort decoding
  return NULL;

 // extract the bits: M = 28, opr = (24, 23, 22, 21)
 const bool M = bit_28;
 const bool bit_24 = is_bit_set(binary_instruction, 24);
 // 31 | 30-29 | 28 | 27-25 | 24-21 | 20-16 | 15-10   | 9-5 | 4-0
 // sf | opc   | M  | 101   | opr   | rm    | operand | rn  | rd

 // M opr  | data processing operaion type
 // --------------------------------------
 // 0 0xxx | bit-logic
 if (!(!M && !bit_24))
  // if encountered non bit-logic instruction, abort decoding
  return NULL;

 // 31 | 30-29 | 28-24 | 23-22 | 21 | 20-16 | 15-10   | 9-5 | 4-0
 // sf | opc   | 01010 | shift | N  | rm    | operand | rn  | rd

 // sf | register width
 // 0  | 32-bit
 // 1  | 64-bit
 const bool register_width = is_bit_set(binary_instruction, 31);

 // opc (30, 29) N | instruction
 // ---------------------------------------------------
 //      0,  0   0 | and  (Rd := Rn & Op2)
 //      0,  0   1 | bic  (Rd := Rn & ~Op2)
 //      0,  1   0 | orr  (Rd := Rn | Op2)
 //      0,  1   1 | orn  (Rd := Rn | ~Op2)
 //      1,  0   0 | eor  (Rd := Rn ^ Op2)
 //      1,  0   1 | eon  (Rd := Rn ^ ~Op2)
 //      1,  1   0 | ands (Rd := Rn & Op2, update PSTATE)
 //      1,  1   1 | bics (Rd := Rn & ~Op2, update PSTATE)
 const u8_t instruction_type = (is_bit_set(binary_instruction, 30) << 1) | is_bit_set(binary_instruction, 29);
 const bool N = is_bit_set(binary_instruction, 21);

 // shift (23, 22) | shift type to perform on Rm
 // --------------------------------------------
 //        0,  0   | logical shift left
 //        0,  1   | logical shift right
 //        1,  0   | arithmetic shift right
 //        1,  1   | rotate right
 // extract value of shift (by bit manipulation)
 const u8_t shift = (is_bit_set(23, binary_instruction) << 1) | is_bit_set(22, binary_instruction);

 // extract operand (mask and right shift)
 // which is 6-bit unsigned immediate value determining the shift amount
 const u8_t operand = (0b00000000000000001111110000000000 & binary_instruction) >> 10;
 // if register_width = false (i.e. 32-bit), then restrict range to 0-31
 if (!register_width & operand >= 32)
  // if incorrect range in operand, abort the parsing
  return NULL;

 // extract rm, rn, rd (mask and right shift): the encoding of input and destination registers
 const u8_t rm = (0b00000000000111110000000000000000 & binary_instruction) >> 16;
 const u8_t rn = (0b0000001111100000 & binary_instruction) >> 5;
 const u8_t rd = 0b00011111 & binary_instruction;

 // construct bit-logic instruction, and return pointer to it
 struct LogicalInstruction logical_instruction = {
  // check the comment docs of this field,
  // the enum values should align with the provided boolean
  .register_width = register_width,

  .instruction = {
   // check the comment docs of this field,
   // the enum values should align with the provided boolean
   .instruction_type = instruction_type,

   .negate_second_input = N
  },

  .rd = rd,
  .rn = rn,
  .rm = rm,

  // check the comment docs of this field,
  // the enum values should align with the provided boolean
  .shift_type = shift,

  .operand = operand
 };
 return &logical_instruction;
}

struct MultiplyInstruction *multiply_instruction_from_binary(const u32_t binary_instruction) {
 // extract the bits: op0 = (28, 27, 26, 25)
 const bool bit_28 = is_bit_set(binary_instruction, 28);
 const bool bit_27 = is_bit_set(binary_instruction, 27);
 const bool bit_26 = is_bit_set(binary_instruction, 26);
 const bool bit_25 = is_bit_set(binary_instruction, 25);

 // op0  | Group
 // ----------------------------------
 // x101 | Data Processing (Register)
 if (!(bit_27 && !bit_26 && bit_25))
  // if encountered non-register instruction, abort decoding
  return NULL;

 // extract the bits: M = 28, opr = (24, 23, 22, 21)
 const bool M = bit_28;
 const bool bit_24 = is_bit_set(binary_instruction, 24);
 const bool bit_23 = is_bit_set(binary_instruction, 23);
 const bool bit_22 = is_bit_set(binary_instruction, 22);
 const bool bit_21 = is_bit_set(binary_instruction, 21);
 // 31 | 30-29 | 28 | 27-25 | 24-21 | 20-16 | 15-10   | 9-5 | 4-0
 // sf | opc   | M  | 101   | opr   | rm    | operand | rn  | rd

 // M opr  | Group
 // -----------------
 // 1 1000 | multiply
 if (!(M && bit_24 && !bit_23 && !bit_22 && !bit_21))
  // if encountered non multiply instruction, abort decoding
  return NULL;

 // 31 | 30-21      | 20-16 | 15 | 14-10 | 9-5 | 4-0
 // sf | 0011011000 | rm    | x  | ra    | rn  | rd

 // sf | register width
 // 0  | 32-bit
 // 1  | 64-bit
 const bool register_width = is_bit_set(binary_instruction, 31);

 // x | instruction
 // 0 | madd (Rd := Ra + (Rn*Rm))
 // 1 | msub (Rd := Ra - (Rn*Rm))
 const bool x = is_bit_set(binary_instruction, 15);

 // extract rm, rn, rd (mask and right shift): the encoding of input and destination registers
 const u8_t rm = (0b00000000000111110000000000000000 & binary_instruction) >> 16;
 const u8_t ra = (0b00000000000000000111110000000000 & binary_instruction) >> 10;
 const u8_t rn = (0b0000001111100000 & binary_instruction) >> 5;
 const u8_t rd = 0b00011111 & binary_instruction;

 // construct multiply instruction, and return pointer to it
 struct MultiplyInstruction multiply_instruction = {
  // check the comment docs of this field,
  // the enum values should align with the provided boolean
  .register_width = register_width,

  // check the comment docs of this field,
  // the enum values should align with the provided boolean
  .instruction = x,

  .rd = rd,
  .ra = ra,
  .rn = rn,
  .rm = rm,
 };
 return &multiply_instruction;
}

struct DataProcessingInstruction *data_processing_instruction_from_binary(const u32_t binary_instruction) {
 // extract the bits: op0 = (28, 27, 26, 25)
 const bool bit_28 = is_bit_set(binary_instruction, 28);
 const bool bit_27 = is_bit_set(binary_instruction, 27);
 const bool bit_26 = is_bit_set(binary_instruction, 26);
 const bool bit_25 = is_bit_set(binary_instruction, 25);

 // op0  | Group
 // ----------------------------------
 // 100x | Data Processing (Immediate)
 if (bit_28 && !bit_27 && !bit_26) {
  return immediate_instruction_from_binary(binary_instruction);
 }

 // op0  | Group
 // ----------------------------------
 // x101 | Data Processing (Register)
 if (bit_27 && !bit_26 && bit_25) {
  return register_instruction_from_binary(binary_instruction);
 }

 // anything else is not a data processing instruction
 return NULL;
}



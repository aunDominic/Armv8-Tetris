/**
 * @file data_processing.h
 * @brief ARMv8 Data Processing Instructions Module Interface
 * @author Andrei Cravtov
 *
 * TODO: document this implementation
 */

#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H

#include "./../types.h"

// ---- MACRO DEFINITIONS ----

// ---- TYPE DEFINITIONS ----
/// An arithmetic instruction, a type of (immediate or register) data processing instruction
struct ArithmeticInstruction {
 /// The width of the registers, i.e. Xn versus Wn
 /// - true: 64-bit register
 /// - false: 32-bit register
 enum {
  REG_64_BIT = true,
  REG_32_BIT = false,
 } register_width: 1;

 /// The arithmetic instruction being performed, specified by [instruction_type|set_flags]:
 /// - [0|0]: add, i.e. Rd := Rn + Op2
 /// - [0|1]: adds, i.e. add then update PSTATE flags
 /// - [1|0]: sub, i.e. Rd := Rn - Op2
 /// - [1|1]: subs, i.e. sub then update PSTATE flags
 struct {
  /// Whether to set the PSTATE flags after execution, or not
  /// - true: set PSTATE flags after execution
  /// - false: don't set them
  bool set_flags: 1;

  /// The type of arithmetic instruction being performed
  /// - true: sub(s)
  /// - false: add(s)
  enum {
   ARITHMETIC_SUB = true,
   ARITHMETIC_ADD = false,
  } instruction_type: 1;
 } instruction: 2;

 /// Tagged union for representing the operand data
 struct {
  /// Data processing operand type:
  /// - true: Register Operand Type
  /// - false: Immediate Operand Type
  enum {
   REGISTER_OPERAND = true,
   IMMEDIATE_OPERAND = false,
  } operand_type: 1;

  /// Data processing operand value: Immediate vs. Register
  union {
   /// Data processing immediate operand value
   struct {
    /// The destination register, a 5-bit unsigned immediate value
    /// - 0b00000..0b11110 encodes R0..R30
    /// - if set_flags = true, 0b11111 encodes the zero-register
    /// - if set_flags = false, 0b11111 encodes the stack-pointer register
    u8_t rd: 5;

    /// The first input register, a 5-bit unsigned immediate value
    /// - 0b00000..0b11110 encodes R0..R30
    /// - 0b11111 encodes the stack-pointer register
    u8_t rn: 5;

    /// The second input, a 12-bit unsigned immediate value
    u16_t imm12: 12;

    /// Whether to logically shift left imm12 by 12 bits, or not
    /// - true: imm12 will be logically shifted left by 12 bits
    /// - false: imm12 won't be logically shifted left by 12 bits
    bool apply_lsl_imm12: 1;
   } immediate_operand_value: 23;

   /// Data processing register operand value
   struct {
    /// The destination register, a 5-bit unsigned immediate value
    /// - 0b00000..0b11110 encodes R0..R30
    /// - 0b11111 encodes the zero-register
    u8_t rd: 5;

    /// The first input register, a 5-bit unsigned immediate value
    /// - 0b00000..0b11110 encodes R0..R30
    /// - 0b11111 encodes the zero-register
    u8_t rn: 5;

    /// The second input register, a 5-bit unsigned immediate value
    /// - 0b00000..0b11110 encodes R0..R30
    /// - 0b11111 encodes the zero-register
    ///
    /// It is first shifted using the appropriate shift_type,
    /// by operand number of bits, and then used as the second argument.
    u8_t rm: 5;

    /// The type of shift to perform on the second input register, i.e. rm
    /// - 0b00: logical shift left
    /// - 0b01: logical shift right
    /// - 0b10: arithmeric shift right
    enum {
     LOGICAL_SHIFT_LEFT = 0b00,
     LOGICAL_SHIFT_RIGHT = 0b01,
     ARITHMETIC_SHIFT_LEFT = 0b10
    } shift_type: 2;

    /// A 6-bit unsigned immediate value, representing how many bits by,
    /// to shift the second input register, i.e. rm:
    /// - if register_width = true (i.e. 64-bit), then the range is 0-63
    /// - if register_width = false (i.e. 32-bit), then the range is 0-31
    u8_t operand: 6;
   } register_operand_value: 23;
  } operand_value: 23;
 } operand: 24;
};

/// A wide-move instruction, a type of (immediate) data processing instruction
struct WideMoveInstruction {
 /// The width of the registers, i.e. Xn versus Wn
 /// - true: 64-bit register
 /// - false: 32-bit register
 enum {
  REG_64_BIT = true,
  REG_32_BIT = false,
 } register_width: 1;

 /// The wide-move instruction being performed
 /// - 0b00: Move wide with NOT, i.e. Rd := ∼Op
 /// - 0b10: Move wide with ZERO, i.e. Rd := Op
 /// - 0b11: Move wide with KEEP, i.e. Rd[shift + 15 : shift] := imm16
 enum {
  MOVE_WIDE_NOT = 0b00,
  MOVE_WIDE_ZERO = 0b10,
  MOVE_WIDE_KEEP = 0b11
 } instruction: 2;

 /// The destination register, a 5-bit unsigned immediate value
 /// - 0b00000..0b11110 encodes R0..R30
 /// - 0b11111 encodes the zero-register
 u8_t rd: 5;

 /// The unsigned immediate value, of the operand, is computed by
 /// logical left shift of (lsl_multiplier * 16) bits on imm16,
 /// and the resulting value is moved into the destination register
 struct {
  /// A 16-bit unsigned immediate value to move, which will get
  /// logically left shifted by (lsl_multiplier * 16) bits
  u16_t imm16: 16;

  /// A logical left shift of (lsl_multiplier * 16) bits, is applied to imm16
  /// before being parsed as the value to be interpreted as a value
  /// - if register_width = true (i.e. 64-bit): can be 0,1,2,3
  /// - if register_width = true (i.e. 32-bit): can only be 0,1
  u8_t lsl_multiplier: 2;
 } operand: 18;
};

/// A bit-wise logical instruction, a type of (register) data processing instruction
struct LogicalInstruction {
 /// The width of the registers, i.e. Xn versus Wn
 /// - true: 64-bit register
 /// - false: 32-bit register
 enum {
  REG_64_BIT = true,
  REG_32_BIT = false,
 } register_width: 1;

 /// The bitwise operation being performed, specified by [instruction_type|negate_second_input]:
 /// - [00|0]: and, i.e. Rd := Rn & Op2
 /// - [00|1]: bic, i.e. Rd := Rn & ∼Op2
 /// - [01|0]: orr, i.e. Rd := Rn | Op2
 /// - [01|1]: orn, i.e. Rd := Rn | ∼Op2
 /// - [10|0]: eor, i.e. Rd := Rn ^ Op2
 /// - [10|1]: eon, i.e. Rd := Rn ^ ~Op2
 /// - [11|0]: ands, i.e. and then update PSTATE flags
 /// - [11|1]: bics, i.e. bic then update PSTATE flags
 struct {
  /// The type of bitwise operation being performed between the two inputs, i.e. rn and rm
  enum {
   BITWISE_AND = 0b00,
   BITWISE_OR = 0b01,
   BITWISE_XOR = 0b10,
   BITWISE_AND_SET_FLAGS = 0b11
  } instruction_type: 2;

  /// Whether to negate the second input register, i.e. rm,
  /// before performing the logical operation
  /// - true: it will be negated
  /// - false: it won't be negated
  bool negate_second_input: 1;
 } instruction: 3;

 /// The destination register, a 5-bit unsigned immediate value
 /// - 0b00000..0b11110 encodes R0..R30
 /// - 0b11111 encodes the zero-register
 u8_t rd: 5;

 /// The first input register, a 5-bit unsigned immediate value
 /// - 0b00000..0b11110 encodes R0..R30
 /// - 0b11111 encodes the zero-register
 u8_t rn: 5;

 /// The second input register, a 5-bit unsigned immediate value
 /// - 0b00000..0b11110 encodes R0..R30
 /// - 0b11111 encodes the zero-register
 ///
 /// It is first shifted using the appropriate shift_type, by operand bits,
 /// then optionally negated (depending on negate_second_input),
 /// and then used as the second argument.
 u8_t rm: 5;

 /// The type of shift to perform on the second input register, i.e. rm
 /// - 0b00: logical shift left
 /// - 0b01: logical shift right
 /// - 0b10: arithmeric shift right
 /// - 0b11: rotate right
 enum {
  LOGICAL_SHIFT_LEFT = 0b00,
  LOGICAL_SHIFT_RIGHT = 0b01,
  ARITHMETIC_SHIFT_LEFT = 0b10,
  ROTATE_RIGHT = 0b11
 } shift_type: 2;

 /// A 6-bit unsigned immediate value, representing how many bits by,
 /// to shift the second input register, i.e. rm:
 /// - if register_width = true (i.e. 64-bit), then the range is 0-63
 /// - if register_width = false (i.e. 32-bit), then the range is 0-31
 u8_t operand: 6;
};

/// A multiply instruction, a type of (register) data processing instruction
struct MultiplyInstruction {
 /// The width of the registers, i.e. Xn versus Wn
 /// - true: 64-bit register
 /// - false: 32-bit register
 enum {
  REG_64_BIT = true,
  REG_32_BIT = false,
 } register_width: 1;

 /// The multiplication operation being performed:
 /// - true: the product is negated, i.e. Rd := Ra - (Rn*Rm)
 /// - false: the product isn't negated, i.e. Rd := Ra + (Rn*Rm)
 enum {
  MULTIPLY_SUB = true,
  MULTIPLY_ADD = false,
 } instruction: 1;

 /// The destination register, a 5-bit unsigned immediate value
 /// - 0b00000..0b11110 encodes R0..R30
 /// - 0b11111 encodes the zero-register
 u8_t rd: 5;

 /// The first input register, a 5-bit unsigned immediate value
 /// - 0b00000..0b11110 encodes R0..R30
 /// - 0b11111 encodes the zero-register
 u8_t ra: 5;

 /// The second input register, a 5-bit unsigned immediate value
 /// - 0b00000..0b11110 encodes R0..R30
 /// - 0b11111 encodes the zero-register
 u8_t rn: 5;

 /// The third input register, a 5-bit unsigned immediate value
 /// - 0b00000..0b11110 encodes R0..R30
 /// - 0b11111 encodes the zero-register
 u8_t rm: 5;
};

/// A tagged union representing the types of data processing instructions:
/// ArithmeticInstruction, WideMoveInstruction, LogicalInstruction, MultiplyInstruction
struct DataProcessingInstruction {
 /// Types of data processing instructions
 /// - DATA_PROCESSING_ARITHMETIC = ArithmeticInstruction
 /// - DATA_PROCESSING_WIDE_MOVE = WideMoveInstruction
 /// - DATA_PROCESSING_LOGICAL = LogicalInstruction
 /// - DATA_PROCESSING_MULTIPLY = MultiplyInstruction
 enum {
  DATA_PROCESSING_ARITHMETIC,
  DATA_PROCESSING_WIDE_MOVE,
  DATA_PROCESSING_LOGICAL,
  DATA_PROCESSING_MULTIPLY
 } instruction_type;

 /// Data of the actual instruction: arithmetic, wide-move, logical, or multiply
 union {
  struct ArithmeticInstruction arithmetic_instruction;
  struct WideMoveInstruction wide_move_instruction;
  struct LogicalInstruction logical_instruction;
  struct MultiplyInstruction multiply_instruction;
 } instruction_data;
};

// ---- INTERFACE DEFINITIONS ----
/**
 * Check if a DataProcessingInstruction is immediate, or not
 *
 * @param instruction
 * @return true if DataProcessingInstruction is immediate, false otherwise
 * @note a NULL pointer will cause a crash
 */
bool data_processing_immediate(const struct DataProcessingInstruction *instruction);

/**
 * Check if a DataProcessingInstruction is of register type, or not
 *
 * @param instruction
 * @return true if DataProcessingInstruction is of register type, false otherwise
 * @note a NULL pointer will cause a crash
 */
bool data_processing_register(const struct DataProcessingInstruction *instruction);

// binary to instruction struct parsers
/**
 * Parses a (little endian) 32-bit binary instruction into a ArithmeticInstruction instance
 * @param binary_instruction the 32-bit binary instruction
 * @return pointer to ArithmeticInstruction instance, or NULL if parsing failed
 */
struct ArithmeticInstruction *arithmetic_instruction_from_binary(u32_t binary_instruction);

/**
 * Parses a (little endian) 32-bit binary instruction into a WideMoveInstruction instance
 * @param binary_instruction the 32-bit binary instruction
 * @return pointer to WideMoveInstruction instance, or NULL if parsing failed
 */
struct WideMoveInstruction *wide_move_instruction_from_binary(u32_t binary_instruction);

/**
 * Parses a (little endian) 32-bit binary instruction into a LogicalInstruction instance
 * @param binary_instruction the 32-bit binary instruction
 * @return pointer to LogicalInstruction instance, or NULL if parsing failed
 */
struct LogicalInstruction *logical_instruction_from_binary(u32_t binary_instruction);

/**
 * Parses a (little endian) 32-bit binary instruction into a MultiplyInstruction instance
 * @param binary_instruction the 32-bit binary instruction
 * @return pointer to MultiplyInstruction instance, or NULL if parsing failed
 */
struct MultiplyInstruction *multiply_instruction_from_binary(u32_t binary_instruction);

/**
 * Parses a (little endian) 32-bit binary instruction into a DataProcessingInstruction instance
 * @param binary_instruction the 32-bit binary instruction
 * @return pointer to DataProcessingInstruction instance, or NULL if parsing failed
 */
struct DataProcessingInstruction *data_processing_instruction_from_binary(u32_t binary_instruction);

#endif //DATA_PROCESSING_H

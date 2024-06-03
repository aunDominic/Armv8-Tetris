/**
 * @file registers.h
 * @brief ARMv8 Registers Module Interface
 * @author Andrei Cravtov
 *
 * TODO: document this implementation
 */

#ifndef REGISTERS_H
#define REGISTERS_H

#include "./../types.h"

// ---- MACRO DEFINITIONS ----
/// The number of general purpose registers in the ARMv8 emulator
/// which is 31 registers, being R0..R30
#define NUM_GENERAL_PURPOSE_REGISTERS 31

#define DECLARE_REG_FUNCS(reg_num) \
/** \
 * Reads the 64-bit value from register R##reg_num \
 * @return the 64-bit value \
 */ \
reg_64_t reg_X##reg_num##_read(void); \
\
/** \
 * Writes a 64-bit value to register R##reg_num \
 * @param value the 64-bit value \
 */ \
void reg_X##reg_num##_write(reg_64_t value); \
\
/** \
 * Reads the 32-bit value from register R##reg_num \
 * @return the 32-bit value \
 */ \
reg_32_t reg_W##reg_num##_read(void); \
\
/** \
 * Writes a 32-bit value to register R##reg_num \
 * @param value the 32-bit value \
 */ \
void reg_W##reg_num##_write(reg_32_t value);

// ---- TYPE DEFINITIONS ----
/// Register index
typedef u8_t reg_index_t;

/// Value of register in 64-bit mode, i.e. Xn
typedef u64_t reg_64_t;
/// Value of register in 32-bit mode, i.e. Wn
typedef u32_t reg_32_t;

/// Pointer to a 64-bit value of register
typedef reg_64_t *reg_64_ptr_t;
/// Constant pointer to a 64-bit value of register, useful for read-only access
typedef const reg_64_t *const_reg_64_ptr_t;

/// Pointer to a 32-bit value of register
typedef reg_32_t *reg_32_ptr_t;
/// Constant pointer to a 32-bit value of register, useful for read-only access
typedef const reg_32_t *const_reg_32_ptr_t;

// ---- INTERFACE DEFINITIONS ----

// indexed general purpose I/O
/**
 * Reads 64-bit value of a general purpose register, specified by reg_index
 *
 * @param reg_index the index of the register
 * @return constant pointer to the value, or NULL if out of bounds
 */
const_reg_64_ptr_t reg_read_64(reg_index_t reg_index);

/**
 * Writes 64-bit value to a general purpose register,specified by reg_index
 *
 * @param reg_index the index of the register
 * @param value the 64-bit value
 * @return true if successful write, false if out of bounds
 */
bool reg_write_64(reg_index_t reg_index, reg_64_t value);

/**
 * Reads 32-bit value of a general purpose register, specified by reg_index
 *
 * @param reg_index the index of the register
 * @return constant pointer to the value, or NULL if out of bounds
 */
const_reg_32_ptr_t reg_read_32(reg_index_t reg_index);

/**
 * Writes 32-bit value to a general purpose register,specified by reg_index
 *
 * @param reg_index the index of the register
 * @param value the 32-bit value
 * @return true if successful write, false if out of bounds
 */
bool reg_write_32(reg_index_t reg_index, reg_32_t value);

// non-indexed general purpose I/O
DECLARE_REG_FUNCS(0)

DECLARE_REG_FUNCS(1)

DECLARE_REG_FUNCS(2)

DECLARE_REG_FUNCS(3)

DECLARE_REG_FUNCS(4)

DECLARE_REG_FUNCS(5)

DECLARE_REG_FUNCS(6)

DECLARE_REG_FUNCS(7)

DECLARE_REG_FUNCS(8)

DECLARE_REG_FUNCS(9)

DECLARE_REG_FUNCS(10)

DECLARE_REG_FUNCS(11)

DECLARE_REG_FUNCS(12)

DECLARE_REG_FUNCS(13)

DECLARE_REG_FUNCS(14)

DECLARE_REG_FUNCS(15)

DECLARE_REG_FUNCS(16)

DECLARE_REG_FUNCS(17)

DECLARE_REG_FUNCS(18)

DECLARE_REG_FUNCS(19)

DECLARE_REG_FUNCS(20)

DECLARE_REG_FUNCS(21)

DECLARE_REG_FUNCS(22)

DECLARE_REG_FUNCS(23)

DECLARE_REG_FUNCS(24)

DECLARE_REG_FUNCS(25)

DECLARE_REG_FUNCS(26)

DECLARE_REG_FUNCS(27)

DECLARE_REG_FUNCS(28)

DECLARE_REG_FUNCS(29)

DECLARE_REG_FUNCS(30)

// zero-register I/O
/**
 * Reads 64-bit value from the zero-register
 * @return the 64-bit value, which is always zero
 */
reg_64_t reg_XZR_read(void);

/**
 * Attempts to write a 64-bit value to the zero-register, but the write is always ignored
 * @param value the 64-bit value, which is always ignored
 */
void reg_XZR_write(reg_64_t value);

/**
 * Reads 32-bit value from the zero-register
 * @return the 32-bit value, which is always zero
 */
reg_32_t reg_WZR_read(void);

/**
 * Attempts to write a 32-bit value to the zero-register, but the write is always ignored
 * @param value the 32-bit value, which is always ignored
 */
void reg_WZR_write(reg_32_t value);

// program-counter register I/O
/// Reset the program-counter register, by zeroing it
void reg_PC_reset(void);

/**
 * Reads 64-bit memory address from the program-counter register
 * @return the 64-bit memory address
 */
reg_64_t reg_PC_read(void);

/**
 * Adds a signed offset to the current
 * memory address in the program-counter regiser
 * @param offset the signed offset
 */
void reg_PC_add_offset(i32_t offset);

// PSTATE register I/O
/// Reset the PSTATE register,
/// by setting the Z flag, and clearing the others
void reg_PSTATE_reset(void);

/**
 * The Negative condition flag of PSTATE register
 * @return true if set, false otherwise
 */
bool reg_PSTATE_n(void);

/**
 * The Zero condition flag of PSTATE register
 * @return true if set, false otherwise
 */
bool reg_PSTATE_z(void);

/**
 * The Carry condition flag of PSTATE register
 * @return true if set, false otherwise
 */
bool reg_PSTATE_c(void);

/**
 * The oVerflow condition flag of PSTATE register
 * @return true if set, false otherwise
 */
bool reg_PSTATE_v(void);

/**
 * Set the PSTATE register condition flags
 *
 * @param n Negative condition flag
 * @param z Zero condition flag
 * @param c Carry condition flag
 * @param v oVerflow condition flag
 */
void reg_PSTATE_set(bool n, bool z, bool c, bool v);

#endif //REGISTERS_H

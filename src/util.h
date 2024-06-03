/**
 * @file util.h
 * @brief Utilities Module Interface
 * @author Andrei Cravtov
 *
 * TODO: document this implementation
 */

#ifndef UTIL_H
#define UTIL_H
#include "types.h"

// ---- MACRO DEFINITIONS ----

// ---- TYPE DEFINITIONS ----

// ---- INTERFACE DEFINITIONS ----
/**
 * Concatenate the bits of two 8-bit integers, into a single 16-bit integer
 * @param lower the lower bits
 * @param higher the higher bits
 * @return the concatenated 16-bit integer
 */
u16_t concat_u8(u8_t lower, u8_t higher);

/**
 * Concatenate the bits of two 16-bit integers, into a single 32-bit integer
 * @param lower the lower bits
 * @param higher the higher bits
 * @return the concatenated 32-bit integer
 */
u32_t concat_u16(u16_t lower, u16_t higher);

/**
 * Concatenate the bits of two 32-bit integers, into a single 64-bit integer
 * @param lower the lower bits
 * @param higher the higher bits
 * @return the concatenated 64-bit integer
 */
u64_t concat_u32(u32_t lower, u32_t higher);

/**
 * Check if the bit at the specified position is set
 *
 * @param n the value being checked
 * @param bit_pos the bit position
 * @return true if set, false if not
 */
bool is_bit_set(u32_t n, u8_t bit_pos);

// TODO: fix the shifting API
i64_t left_shift_logical(int bits, i64_t value, int width);

i64_t right_shift_logical(int bits, i64_t value, int width);

i64_t right_shift_arithmetic(int bits, i64_t value, int width);

i64_t right_rotate(int bits, i64_t value, int width);

#endif //UTIL_H

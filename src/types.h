/**
 * @file types.h
 * @brief Useful type definitions
 * @author Andrei Cravtov
 */

#ifndef TYPES_H
#define TYPES_H

#include <bits/types.h>

// signed integers
/// Signed 8-bit integer
typedef __int_least8_t i8_t;
/// Signed 16-bit integer
typedef __int_least16_t i16_t;
/// Signed 32-bit integer
typedef __int_least32_t i32_t;
/// Signed 64-bit integer
typedef __int_least64_t i64_t;

// unsigned integers
/// Unsigned 8-bit integer
typedef __uint_least8_t u8_t;
/// Unsigned 16-bit integer
typedef __uint_least16_t u16_t;
/// Unsigned 32-bit integer
typedef __uint_least32_t u32_t;
/// Unsigned 64-bit integer
typedef __uint_least64_t u64_t;

// bytes
/// A single byte - value in the interval 0..255
typedef u8_t byte_t;
/// Pointer to a byte
typedef byte_t *byte_ptr_t;
/// Constant pointer to a byte, useful for read-only access
typedef const byte_t *const_byte_ptr_t;

#endif //TYPES_H

/**
 * @file util.c
 * @brief Utilities Module Implementation
 * @author Andrei Cravtov
 *
 * TODO: document this implementation
 */

#include "util.h"

// ---- TYPE DEFINITIONS ----

// ---- VARIABLE DEFINITIONS ----

// ---- HELPER METHODS ----

// ---- INTERFACE IMPLEMENTATION ----
u16_t concat_u8(const u8_t lower, const u8_t higher) {
 return (u16_t) higher << 8 | lower;
}

u32_t concat_u16(const u16_t lower, const u16_t higher) {
 return (u32_t) higher << 16 | lower;
}

u64_t concat_u32(const u32_t lower, const u32_t higher) {
 // Note: Need to cast higher to u64_t before shifting to avoid undefined behavior
 return ((u64_t) higher << 32) | lower;
}

bool is_bit_set(const u32_t n, const u8_t bit_pos) {
 return n & (1 << bit_pos);
}


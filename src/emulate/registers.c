/**
 * @file registers.c
 * @brief ARMv8 Registers Module Implementation
 * @author Andrei Cravtov
 *
 * TODO: document this implementation
 */

#include "./../types.h"
#include "registers.h"

#include <stddef.h>

// ---- TYPE DEFINITIONS ----

// ---- VARIABLE DEFINITIONS ----
static u64_t general_purpose_registers[31] = {0};
static u64_t program_counter = 0;

// PSTATE register state,
// initialize to Z set and others clear
static bool pstate_n = false;
static bool pstate_z = true;
static bool pstate_c = false;
static bool pstate_v = false;

// ---- HELPER METHODS ----
/**
 * Checks if register index is out of bounds
 *
 * @param index the register index
 * @return true if out of bounds, false otherwise
 */
static bool out_of_bounds(const reg_index_t index) {
 return index >= NUM_GENERAL_PURPOSE_REGISTERS;
}

// ---- INTERFACE IMPLEMENTATION ----
const_reg_64_ptr_t reg_read_64(const reg_index_t reg_index) {
 if (out_of_bounds(reg_index)) return NULL;
 return &general_purpose_registers[reg_index];
}

bool reg_write_64(const reg_index_t reg_index, const reg_64_t value) {
 if (out_of_bounds(reg_index)) return false;
 general_purpose_registers[reg_index] = value;
 return true;
}

const_reg_32_ptr_t reg_read_32(reg_index_t reg_index) {
 // TODO: ensure the write semantics are correct
 if (out_of_bounds(reg_index)) return NULL;
 return &general_purpose_registers[reg_index];
}

bool reg_write_32(const reg_index_t reg_index, const reg_32_t value) {
 if (out_of_bounds(reg_index)) return false;
 general_purpose_registers[reg_index] = value;
 return true;
}

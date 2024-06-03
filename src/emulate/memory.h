/**
 * @file memory.h
 * @brief ARMv8 Memory Module Interface
 * @author Andrei Cravtov
 *
 * This module provides an interface to the ARMv8 emulator memory.
 * It encapsulates access to the memory, ensuring that all interactions are
 * performed through a controlled and safe API. This includes functions for
 * reading from it, writing to it, and other utility.
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h> // for size_t
#include <stdio.h>
#include "./../types.h"

// ---- MACRO DEFINITIONS ----

/// Capacity of ARMv8 emulator memory, which is 2MB (2*2^20 bytes)
#define EMULATOR_MEMORY_CAPACITY (2 * (2 << 20))

/// Instructions are aligned on a 4-byte boundary,
/// in the ARMv8 emulator memory
#define INSTRUCTION_MEM_ALIGNMENT 4

// ---- TYPE DEFINITIONS ----

/// Address of a location in ARMv8 emulator memory
typedef u32_t memory_address_t;

/**
 * ARMv8 emulator memory, which has a capacity of
 * 2MB (2*2^20 bytes), and is initialised to zero
 */
typedef byte_t emulator_memory_t[EMULATOR_MEMORY_CAPACITY];

// ---- INTERFACE DEFINITIONS ----

/// Resets the emulator memory - by zeroing it all
void emulator_memory_reset(void);

/**
 * Reads byte at specified memory address
 *
 * @param address the address of the memory location
 * @return constant pointer to the value, or NULL if out of bounds
 */
const_byte_ptr_t emulator_memory_read_value(memory_address_t address);

/**
 * Writes byte to the specified memory address
 *
 * @param address the address of the memory location
 * @param value the byte
 * @return true if successful write, false if out of bounds
 */
bool emulator_memory_write_value(memory_address_t address, byte_t value);

/// The result indicating the outcome of calling emulator_memory_write_file
enum WriteFileResult {
 WRITE_FILE_RESULT_NULL_PTR_ERROR,
 WRITE_FILE_RESULT_OUT_OF_BOUNDS_ERROR,
 WRITE_FILE_RESULT_IO_ERROR,
 WRITE_FILE_RESULT_SUCCESS
};

/**
 * Writes file binary to emulator memory,
 * starting at the specified memory address
 *
 * @param file the file from which to read the binary
 * @param starting_address the starting address
 * @return WFR_NULL_PTR_ERROR if the file pointer is NULL,
 * WFR_OUT_OF_BOUNDS_ERROR if attempting to write out of bounds,
 * WFR_IO_ERROR if failed to read from file correctly, and finally
 * WFR_SUCCESS if the write operation completed successfully
 */
enum WriteFileResult emulator_memory_write_file(
 FILE *file, memory_address_t starting_address);

/// Efficient check that the instruction address is memory aligned
bool instruction_is_mem_aligned(memory_address_t instruction_address);

/// The result indicating the outcome of calling emulator_memory_read_binary_instruction
enum ReadBinaryInstructionResult {
 READ_BINARY_INSTRUCTION_RESULT_NULL_PTR_ERROR,
 READ_BINARY_INSTRUCTION_RESULT_ALIGNMENT_ERROR,
 READ_BINARY_INSTRUCTION_RESULT_OUT_OF_BOUNDS_ERROR,
 READ_BINARY_INSTRUCTION_RESULT_SUCCESS,
};

/**
 * Reads the 32-bit instruction at a given address
 *
 * @param instruction_address the address of the instruction
 * @param out pointer to which the 32-bit instruction is written
 * @return READ_BINARY_INSTRUCTION_RESULT_NULL_PTR_ERROR if the out pointer is NULL,
 * READ_BINARY_INSTRUCTION_RESULT_ALIGNMENT_ERROR if the instruction address is not aligned on a 4-byte boundary,
 * READ_BINARY_INSTRUCTION_RESULT_OUT_OF_BOUNDS_ERROR if attempting to read an out of bounds address,
 * READ_BINARY_INSTRUCTION_RESULT_SUCCESS if the read operation completed successfully
 */
enum ReadBinaryInstructionResult emulator_memory_read_binary_instruction(
 memory_address_t instruction_address, u32_t *out);

#endif //MEMORY_H

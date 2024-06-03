/**
 * @file memory.c
 * @brief ARMv8 Memory Module Implementation
 * @author Andrei Cravtov
 *
 * Implements the ARMv8 emulator memory module interface defined
 * in memory.h. This file contains the static memory array and
 * functions to interact with it. The memory array is initialized
 * to zero and can be accessed only through the provided API.
 *
 * @see memory.h for the interface and usage instructions.
 */

#include "memory.h"
#include <string.h>

#include "../util.h"

// ---- TYPE DEFINITIONS ----

// ---- VARIABLE DEFINITIONS ----
static emulator_memory_t emulator_memory = {0};

// ---- HELPER METHODS ----
/**
 * Checks if memory addess is out of bounds
 *
 * @param address the memory address
 * @return true if out of bounds, false otherwise
 */
static bool out_of_bounds(const memory_address_t address) {
    return address >= EMULATOR_MEMORY_CAPACITY;
}

// ---- INTERFACE IMPLEMENTATION ----
void emulator_memory_reset() {
    memset(emulator_memory, 0, EMULATOR_MEMORY_CAPACITY);
}

const_byte_ptr_t emulator_memory_read_value(const memory_address_t address) {
    if (out_of_bounds(address)) return NULL;
    return &emulator_memory[address];
}

bool emulator_memory_write_value(const memory_address_t address, const byte_t value) {
    if (out_of_bounds(address)) return false;
    emulator_memory[address] = value;
    return true;
}

bool instruction_is_mem_aligned(const memory_address_t instruction_address) {
    // the INSTRUCTION_MEM_ALIGNMENT is 4, and
    // any multiple of 4 has last two bits 00
    // so if XY & 11 = 00, then XY = 00 and multiple of 4
    return (instruction_address & 0b11) == 0;
}

enum WriteFileResult emulator_memory_write_file(FILE *file, const memory_address_t starting_address) {
    // ensure we aren't reading from null pointer
    if (file == NULL) return WRITE_FILE_RESULT_NULL_PTR_ERROR;

    // compute file size
    fseek(file, 0L, SEEK_END); // go to end of file
    const long file_size = ftell(file); // current position (i.e. the end of the file)
    rewind(file); // put file pointer back to the start

    // ensure that `file_size + offset` is not out of bounds,
    // and then write file content to memory
    if (out_of_bounds(starting_address + file_size - 1)) return WRITE_FILE_RESULT_OUT_OF_BOUNDS_ERROR;
    const size_t bytes_read = fread(emulator_memory, 1, file_size, file); // reads file into memory

    // check that correct amount is read,
    // otherwise indicate error
    if (bytes_read != file_size) return WRITE_FILE_RESULT_IO_ERROR;
    return WRITE_FILE_RESULT_SUCCESS;
}

enum ReadBinaryInstructionResult emulator_memory_read_binary_instruction(const memory_address_t instruction_address,
                                                                         u32_t *out) {
    // ensure we aren't reading from null pointer
    if (out == NULL) return READ_BINARY_INSTRUCTION_RESULT_NULL_PTR_ERROR;

    // ensure correct address alignment
    if (instruction_is_mem_aligned(instruction_address))
        return READ_BINARY_INSTRUCTION_RESULT_ALIGNMENT_ERROR;

    // ensure address is not out of bounds
    if (out_of_bounds(instruction_address + 3))
        return READ_BINARY_INSTRUCTION_RESULT_OUT_OF_BOUNDS_ERROR;

    // read instruction bytes (little endian) into `out`
    *out = concat_u16(
        concat_u8(
            emulator_memory[instruction_address],
            emulator_memory[instruction_address + 1]),
        concat_u8(
            emulator_memory[instruction_address + 2],
            emulator_memory[instruction_address + 3]));

    return READ_BINARY_INSTRUCTION_RESULT_SUCCESS;
}

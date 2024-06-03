#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "emulate/memory.h"
#include "emulate/registers.h"

void output_handler(FILE *output_file) {
    const char *text = "LOREM IPSUM dolor sit amet, consectetur adipiscing elit.\n";
    fputs(text, output_file);
}

int run_emulation() {
    do {
        // 1: FETCH
        // read the binary instruction that PC points to
        u32_t binary_instruction;
        switch (emulator_memory_read_binary_instruction(reg_PC_read(), &binary_instruction)) {
            case READ_BINARY_INSTRUCTION_RESULT_NULL_PTR_ERROR:
                perror("An erroneous null pointer error occured, that pointer should never be null");
                abort();
            case READ_BINARY_INSTRUCTION_RESULT_ALIGNMENT_ERROR:
                perror("An erroneous alignment error occured, PC should never be un-aligned");
                abort();
            // TODO: gracefully handle this branch in the future
            case READ_BINARY_INSTRUCTION_RESULT_OUT_OF_BOUNDS_ERROR:
                perror("PC instruction out of bounds. TODO: gracefully handle this in the future");
                abort();
            case READ_BINARY_INSTRUCTION_RESULT_SUCCESS:
                // do nothing for the success case
                break;
        }

        // 2. DECODE
        //
    } while (true);
}

int main(const int argc, char *argv[]) {
    printf("%d", argc);

    // check for the right number of arguments, has to be 2 or 3
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "Usage: `%s <file_in>` or `%s <file_in> <file_out>`\n", argv[0], argv[0]);
        return EXIT_FAILURE;
    }

    // read binary from input file, into emulator memory
    FILE *input_file = fopen(argv[1], "rb"); // "rb" means read binary
    const enum WriteFileResult write_file_result = emulator_memory_write_file(input_file, 0);
    switch (write_file_result) {
        case WRITE_FILE_RESULT_NULL_PTR_ERROR:
            perror("Error opening input file");
            break;
        case WRITE_FILE_RESULT_OUT_OF_BOUNDS_ERROR:
        case WRITE_FILE_RESULT_IO_ERROR:
            perror(write_file_result == WRITE_FILE_RESULT_OUT_OF_BOUNDS_ERROR
                       ? "Input file too large"
                       : "Error reading from file");
        // fall-through to SUCCESS for closing the file
        case WRITE_FILE_RESULT_SUCCESS: fclose(input_file);
    }
    if (write_file_result != WRITE_FILE_RESULT_SUCCESS) return EXIT_FAILURE;

    // open the output file, if specified - default to stdout if not
    FILE *output_file = stdout;
    if (argc == 3 && (output_file = fopen(argv[2], "w")) == NULL) {
        perror("Error opening output file");
        return EXIT_FAILURE;
    }

    // execute emulator
    run_emulation();

    // program finished, write output
    output_handler(output_file);

    // release resources, and exit
    if (output_file != stdout) fclose(output_file);
    return EXIT_SUCCESS;
}


#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "symbol_table.h"
#include "line_reader.h"
#include "opcode_table.h"
#include "common_types.h"

static uint32_t lineNumber(const uint32_t address) {
    return address / INST_SIZE;
}

int main(const int argc, const char **argv) {

    // VERY IMPORTANT TO INITIALIZE THESE
    symbol_table = createSymbolTable(); // handles labels
    opcodeTable = createOpCodeTable();

    FILE *inputFile, *outputFile;
    if (argc != 3) {
        perror("Invalid command line arguments. Should be ./assemble add01.s add01.bin\n");
        return EXIT_FAILURE;
    }

    if ((inputFile = fopen(argv[1], "r")) == NULL) {
        perror("Error opening input file\n");
        return EXIT_FAILURE;
    }

    if ((outputFile = fopen(argv[2], "wb")) == NULL) {
        perror("Error opening output file\n");
        fclose(inputFile);
        return EXIT_FAILURE;
    }


    // FIRST PASS STRUCTURE
    // reads file line by line
    // address is just line number times 4
    // for each line, check if its a label definition
    // then adds to symbol table

    char buffer[MAX_LENGTH]; // defined in common_types.h
    uint32_t address = 0;
    while (fgets(buffer, sizeof(buffer), inputFile) != NULL) {
        // fgets stores '\n' so turns this into a NULL_CHAR as we don't want this
        buffer[strcspn(buffer, "\n")] = NULL_CHAR;

        if (strlen(buffer) > 0) {
            // non-empty line
            if (is_label(buffer)) {

                // first remove the colon at the end of buffer for labels
                const char *pos = strchr(buffer, ':');
                buffer[pos - buffer] = NULL_CHAR;
                addSymbol(symbol_table, buffer, address);
            } else {
                address += INST_SIZE;
            }
        }
    }


    // SECOND PASS
    // total amount of instructions that we need to output
    uint32_t instructionAmount = lineNumber(address);

    // instruction buffer to write everything at once.
    // This is a VLA array - perhaps this could be changed to a dynamic array
    // empty initializer due to VLA array restriction
    INST instructions[instructionAmount] = {};

    // reset these variables for 2nd pass
    rewind(inputFile); // move file pointer back to start
    address = 0;
    while (fgets(buffer, sizeof(buffer), inputFile) != NULL) {
        // fgets stores '\n' so turns this into a NULL_CHAR as we don't want this
        buffer[strcspn(buffer, "\n")] = NULL_CHAR;
        if (strlen(buffer) > 0) {
            // we don't lines with labels
            if (is_label(buffer)) {
                continue;
            }

            // lineHandler then handles the different cases
            const INST instructionToOutput = lineHandler(buffer, address);
            instructions[lineNumber(address)] = instructionToOutput;
            address += INST_SIZE;
        }
    }


    size_t noElementsWritten = fwrite(instructions, sizeof(INST), instructionAmount, outputFile);
    assert(noElementsWritten == instructionAmount);

    // Free everything
    freeSymbolTable(symbol_table);
    freeSymbolTable(opcodeTable);
    fclose(inputFile);
    fclose(outputFile);
    printf("Assemble running \n");
    return EXIT_SUCCESS;
}

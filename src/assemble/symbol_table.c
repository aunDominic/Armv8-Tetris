//
// Created by Ahmad Jamsari on 30/05/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

#define INITIAL_SIZE 16

// make sure to initialize this at start of program.
SymbolTable *symbol_table = NULL;

SymbolTable* createSymbolTable() {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    table->symbols = (Symbol*) malloc(INITIAL_SIZE * sizeof(Symbol));
    table->size = 0;
    table->capacity = INITIAL_SIZE;
    return table;
}

void addSymbol(SymbolTable *table, const char *symbol, const int addr) {
    // resizing logic, just double the size
    if (table->size == table->capacity) {
        table->capacity *= 2;
        table->symbols = (Symbol*) realloc(table->symbols, table->capacity * sizeof(Symbol));
    }
    table->symbols[table->size].symbol = strdup(symbol);
    table->symbols[table->size].addr = addr;
    table->size++;
}

int32_t getValue(const SymbolTable *table, const char *symbol) {
    for (int i = 0; i < table->size; i++) {
        if (strcmp(table->symbols[i].symbol, symbol) == 0) {
            return table->symbols[i].addr;
        }
    }
    // perror("Symbol_Table symbol not found. Shouldn't happen.\n");
    // printf("%sgetValue attempted match with \n", symbol);
    return NOT_FOUND;
}

void freeSymbolTable(SymbolTable *table) {
    for (int i = 0; i < table->size; i++) {
        free(table->symbols[i].symbol);
    }
    free(table->symbols);
    free(table);
}

int getTableSize(const SymbolTable *table) {
    return table->size;
}
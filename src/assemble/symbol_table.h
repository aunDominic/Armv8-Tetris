//
// Created by Ahmad Jamsari on 30/05/24.
//

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef struct {
    char *symbol;
    int addr;
} Symbol;

typedef struct {
    Symbol *symbols;
    int size;
    int capacity;
} SymbolTable;

// Function to create a new symbol table
SymbolTable* createSymbolTable();

// Function to add a symbol to the symbol table
void addSymbol(SymbolTable *table, const char *symbol, int addr);

// Function to find the address of a symbol in the symbol table
int getValue(const SymbolTable *table, const char *symbol);

// Function to free the memory allocated for the symbol table
void freeSymbolTable(SymbolTable *table);

int getTableSize(const SymbolTable *table);

extern SymbolTable *symbol_table;

#endif // SYMBOL_TABLE_H


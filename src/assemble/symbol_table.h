//
// Created by Ahmad Jamsari on 30/05/24.
//

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define NOT_FOUND -1

typedef struct {
    char *symbol;
    int addr;
} Symbol;

struct SymbolTable;
// its up to the .c file to define this
// allows more flexibility of implementation

typedef struct SymbolTable SymbolTable;

// Function to create a new symbol table
SymbolTable* createSymbolTable();

// Function to add a symbol to the symbol table
void addSymbol(SymbolTable *table, const char *symbol, int addr);

// Function to get the value of a symbol in the symbol table
int getValue(const SymbolTable *table, const char *symbol);

// Function to free the memory allocated for the symbol table
void freeSymbolTable(SymbolTable *table);

int getTableSize(const SymbolTable *table);

extern SymbolTable *symbol_table;

#endif // SYMBOL_TABLE_H


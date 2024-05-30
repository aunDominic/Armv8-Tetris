//
// Created by Ahmad Jamsari on 30/05/24.
//

#include "opcode_table.h"

#include <stddef.h>

SymbolTable *opcodeTable = NULL;

// make sure to set this to opcodeTable
SymbolTable* createOpCodeTable() {
    SymbolTable *table = createSymbolTable();
    // Add opcodes to the symbol table, yes this is ugly af
    // there's probably a bette way to do this
    addSymbol(table, "add", ADD);
    addSymbol(table, "adds", ADDS);
    addSymbol(table, "sub", SUB);
    addSymbol(table, "subs", SUBS);
    addSymbol(table, "cmp", CMP);
    addSymbol(table, "cmn", CMN);
    addSymbol(table, "neg", NEG);
    addSymbol(table, "negs", NEGS);
    addSymbol(table, "and", AND);
    addSymbol(table, "ands", ANDS);
    addSymbol(table, "bic", BIC);
    addSymbol(table, "bics", BICS);
    addSymbol(table, "eor", EOR);
    addSymbol(table, "orr", ORR);
    addSymbol(table, "eon", EON);
    addSymbol(table, "orn", ORN);
    addSymbol(table, "tst", TST);
    addSymbol(table, "movk", MOVK);
    addSymbol(table, "movn", MOVN);
    addSymbol(table, "movz", MOVZ);
    addSymbol(table, "mov", MOV);
    addSymbol(table, "mvn", MVN);
    addSymbol(table, "madd", MADD);
    addSymbol(table, "msub", MSUB);
    addSymbol(table, "mul", MUL);
    addSymbol(table, "mneg", MNEG);
    addSymbol(table, "b", B);
    addSymbol(table, "b.eq", B_EQ);
    addSymbol(table, "b.ne", B_NE);
    addSymbol(table, "b.ge", B_GE);
    addSymbol(table, "b.lt", B_LT);
    addSymbol(table, "b.gt", B_GT);
    addSymbol(table, "b.le", B_LE);
    addSymbol(table, "b.al", B_AL);
    addSymbol(table, "br", BR);
    addSymbol(table, "str", STR);
    addSymbol(table, "ldr", LDR);
    addSymbol(table, ".int", SPECIAL_INT);

    return table;
}
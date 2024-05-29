//
// Created by Dominic Ng on 25/05/2024.
//

#include "datatransfer.h"
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "bitmanipulation.h"

// ADDR should be 21 bits
typedef uint32_t ADDR;
bool SF;
bool L;
bool U;
bool I;


static void setFlags(INST instruction){
    U = 1 << 24 & instruction;
    SF = 1 << 30 & instruction;
    L = 1 << 20 & instruction;
    I = 1 << 11 & instruction;
}

static int64_t getAddress(INST instruction){
    int64_t XN = extractBits(instruction, 5, 9);
    int64_t simm9 = sign_extend(extractBits(instruction, 12, 20), 9);

    // these may not be mutually exclusive
    if (U){
        int64_t uoffset = extractBits(instruction, 10, 21);
        if (SF) uoffset *= 8;
        else uoffset *= 4;
        return XN + uoffset;
    }
    if (I){
        return XN + simm9;
    } else {
        int64_t ret = XN;
        XN += simm9;
        return ret;
    }

}
bool singleDataTransferHandler(INST instruction){
    // Preconditions:
    // Format: 1 sf 1110 0U0L offset xn rt
    // rt -> target register
    // xn -> base register (stores address)
    setFlags(instruction);
    ADDR addr = getAddress(instruction);

    return false; // errors
}
bool loadLiteralHandler(INST instruction){
    int64_t simm19 = sign_extend(extractBits(instruction, 5, 23), 19);
    int64_t address = programCounter + simm19 * 4;
    return false; // errors
}


int main(){
    int64_t n = 256;
    int64_t a = 1;
    printBits(n);
    int64_t sextn = sign_extend(n, 9);
    printBits(sextn);
    int64_t n2 = extractBits(a, 0, 0);
    printBits(n2);
    printf("%lld", sextn);
}
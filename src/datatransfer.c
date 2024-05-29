//
// Created by Dominic Ng on 25/05/2024.
//

#include "datatransfer.h"
#include <stdint.h>
#include <stdlib.h>
// ADDR should be 21 bits
typedef uint32_t ADDR;
struct SDTFormat{
    bool sf;
    bool u;
    bool l;
};
bool singleDataTransferHandler(INST instruction){
    // Preconditions:
    // Format: 1 sf 1110 0U0L offset xn rt
    // rt -> target register
    // xn -> base register (stores address)

    return false; // errors
}
bool loadLiteralHandler(INST instruction){
    return false; // errors
}


// static int extractBits(INST instruction, int start, int end){
//     int
// }
// // Addressing modes
// bool
//
// Created by Dominic Ng on 29/05/2024.
//

#include "bitmanipulation.h"
#include "stdint.h"
#include "memory.h"
#include <stdio.h>
#define BIT(instruction, bit) (1 << (bit) & (instruction))
// Assumes little endian
// Usage, printBits(sizeof(n), &n)
void printBits(int64_t n)
{
    for (int i = 63; i >= 0; i--){
        if ((1LL << i) & n) {
            printf("%d", 1);
        } else {
            printf("%d", 0);
        }
    }
    printf("\n");
}
// Function to sign-extend an n-bit signed integer stored as int64_t
// Eg sign_extend(256, 9) would produce -256
int64_t sign_extend(int64_t num, int n) {
    int64_t mask = 1LL << (n - 1);  // Create a mask with the sign bit set
    num = num & ((1LL << n) - 1);  // Mask the lower n bits
    if (num & mask) {               // Check if the sign bit is set
        num = num | (~((1ULL << n) - 1));  // Sign-extend by setting upper bits to 1
    }
    return (int64_t)num;
}
int64_t extractBits(INST instruction, int start, int end){
    return ((instruction >> start) & ((1 << (end - start + 1)) - 1));
}

//
// Created by Dominic Ng on 25/05/2024.
//

#include "shifter.h"
#include <stdint.h>
#include "bitmanipulation.h"


int64_t lsl(int bits, int64_t value, int width){
    return value << bits;
}
int64_t lsr(int bits, int64_t value, int width){
    return value >> bits;
}
int64_t asr(int bits, int64_t value, int width){
    return 0;
}
int64_t ror(int bits, int64_t value, int width){
    return 0;
}


// Loads binary data, then emulates the execution and fetch cycle
#include <stdio.h>
#include "registers.h"
#include <stdlib.h>

int main(int argc, char **argv){
    for (int i = 0; i < 64; i++) {
        register_file.PSTATE[i] = true;
        printf("%d",register_file.PSTATE[0]);
    }
    return EXIT_SUCCESS;


}

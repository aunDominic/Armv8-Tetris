//
// Created by terp on 20/06/24.
//

#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

// Define DEBUG to 1 to enable debug prints, or 0 to disable them
#define DEBUG 0

#if DEBUG
    #define DEBUG_PRINT(fmt, ...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__)
    #define PRINT(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) ((void)0)
    #define PRINT(fmt, ...) ((void)0)
#endif

#endif //DEBUG_H

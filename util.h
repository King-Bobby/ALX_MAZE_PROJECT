#ifndef UTIL_H
#define UTIL_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include "render.h" // Include rendering-related structures and constants

#define ASSERT(cond, ...) \
    if (!cond) { \
        fprintf(stderr, __VA_ARGS__); \
        exit(1); \
    }

// Define constants and utility functions used in the main program

#endif
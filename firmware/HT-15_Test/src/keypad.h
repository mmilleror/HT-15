// keypad.h
// Public API for keypad scanning extracted from HT-15_Test.cpp
// Provides initialization and periodic scan functions for the 6x4 button matrix and encoder/pot reads.

#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdint.h>
#include <stdbool.h>

// Initialize keypad GPIOs and ADC inputs used by keypad code.
void keypad_init(void);

// Run one iteration of keypad processing. This will scan the matrix, handle debouncing,
// Call this periodically from the main loop.
void keypad_poll(void);

// Optional helpers to read analog values used by keypad (battery and volume pot).
// These mirror the functions previously in HT-15_Test.cpp and remain available
// as C functions for reuse.

#endif // KEYPAD_H

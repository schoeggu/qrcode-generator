#ifndef UTIL_H
#define UTIL_H

/*
 * Utilities
 */

typedef unsigned char byte;

typedef unsigned char bool;
#define true 1
#define false 0


/*
 * reverse bit order of a byte
 */
byte reverse_bit_b(byte b);

/*
 * reverse bit order of an int
 */
unsigned int reverse_bit_i(unsigned int i);

#endif //UTIL_H

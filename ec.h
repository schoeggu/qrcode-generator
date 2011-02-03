#ifndef EC_H
#define EC_H

/*
 * Error Correction
 * Uses a Solomon Reed algorithm
 * QR Code uses pp=285 for the galois field
 */

#include "util.h"

/*
 *	generate <ecBlocks> Error Correction Blocks from <data> and save it in <dest>
 */
int generateErrorCorrectionCode(const byte* const data, int dataSize, byte* dest, int ecBlocks);

/*
 * A galois field is needed 
 */
void calculate_galois_field(const int pp);

/*
 * alpha Value to primitive element
 * alpha is the index in the galois field
 */
byte altope(const int alpha);

/*
 * primitive element to alpha Value
 * alpha is the index in the galois field
 */
int petoal(const byte integer);

#endif //EC_H


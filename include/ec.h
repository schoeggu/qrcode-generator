#ifndef EC_H_
#define EC_H_

/*
 * Error Correction
 * Uses a Solomon Reed algorithm
 * QR Code uses pp=285 for the galois field
 */

#include "util.h"

/* QR Code uses pp=285 for the galois field calculation */
static const int PP = 285;

/*
 *	generate <ecBlocks> Error Correction Blocks from <data> and save it in <dest>
 */
bool generateErrorCorrectionCode(const byte* const data, int dataSize, byte* dest, int ecBlocks);

/*
 * BCH (Bose-Chaudhuri-Hocquenghem-Code) used to encode Format and Version information
 *
 */
int bch(int totalBits, int DataBits, int data, int gpb);

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

#endif //EC_H_


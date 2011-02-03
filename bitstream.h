#ifndef BITSTREAM_H
#define BITSTREAM_H

/*
 * struct for adding bitwise data
 */

#include "util.h"

#include <stdlib.h>

typedef struct {
	int size; // size int bits
	int buffersize; // number bytes in buffer
	byte* buffer; // buffer holding the data
} bitstream;

/*
 * allocate a new bitstream
 */
bitstream* bs_init();

/*
 * free the memory
 */
void bs_destroy(bitstream* bs);

/*
 * add <numBits> bits of byte <data> to <bs> 
 * starting with least significnat bit
 */
void bs_add_b(bitstream* bs, byte data, int numBits);

/*
 * add <numBits> bits of byte array <data> to <bs> 
 * starting with first bytes least siginificant bit 
 */
void bs_add_bs(bitstream* bs, const byte* data, int dataSize, int numBits);

/*
 * add <numBits> bits of int <data> to <bs> 
 * starting with least siginificant bit 
 */
void bs_add_i(bitstream* bs, unsigned int data, int numBits);

/*
 * get the byte at <index>
 */
byte bs_get(bitstream* bs, int index);

/*
 * get <numBytes> bytes from the <bs> starting at <index>
 * save to <dest>
 */
void bs_geta(bitstream* bs, byte* dest, int index, int numBytes);

/*
 * print bitstreams contents
 * for debug
 */
void bs_print_full(bitstream* bs);


#endif //BITSTREAM_H

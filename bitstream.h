#ifndef BITSTREAM_H
#define BITSTREAM_H

#include "util.h"

#include <stdlib.h>

typedef struct {
	size_t size;		
	size_t buffersize;
	byte* buffer;
} bitstream;

bitstream* bs_init();

void bs_destroy(bitstream* bs);

void bs_add_b(bitstream* bs, byte data, size_t numBits);
void bs_add_bs(bitstream* bs, byte* data, size_t bitsSize, size_t numBits);
void bs_add_i(bitstream* bs, unsigned int data, size_t numBits);

//byte* bs_get(bitstream* bs, size_t numBits);
byte bs_get(bitstream* bs, size_t index);

//void bs_print(bitstream* bs);
void bs_print_full(bitstream* bs);

#endif //BITSTREAM_H

#include "bitstream.h"

#include <stdio.h>

bitstream* bs_init()
{
	bitstream* bs = malloc(sizeof(bitstream));
	bs->size = 0;
	bs->buffersize = 4;
	bs->buffer = calloc(sizeof(byte), bs->buffersize);
	return bs;
}		

void bs_destroy(bitstream* bs) 
{
	free(bs->buffer);
	free(bs);
}

void bs_add_b(bitstream* bs, byte data, size_t numBits) 
{
	int i;

	if (bs->buffersize * 8 < bs->size + numBits) {
		bs->buffersize++;
		bs->buffer = realloc(bs->buffer, bs->buffersize);
		bs->buffer[bs->buffersize-1] &= 0;
		
	}

	int numBytes = bs->size / 8;
	int trailingbits = bs->size % 8;
	byte mask = 0;
	
	for (i = 0; i < numBits; i++) mask = (mask << 1) | 1;

	if (trailingbits) {bs->buffer[numBytes] |= ((mask & data) << trailingbits); numBytes++; }
	if (numBits - trailingbits > 0) bs->buffer[numBytes] |= (mask & data) >> (trailingbits ? 8 - trailingbits : 0);

	bs->size += numBits;

}

void bs_add_i(bitstream* bs, unsigned int data, size_t numBits) {
	while (numBits > 8) {
		bs_add_b(bs, (byte)data, 8);
		data >>= 8;
		numBits -= 8;
	}
	bs_add_b(bs, (byte)data, numBits);
}

byte bs_get(bitstream* bs, size_t index)
{
	if (index * 8 >= bs->size) return 0;
	return bs->buffer[index];
}

void bs_print_full(bitstream* bs) {
	printf("size: %u \t buffersize: %u\t\t", (unsigned int)bs->size, (unsigned int)bs->buffersize);
	int i, j;
	for (i = 0; i < bs->buffersize; i++) {
		printf (" ");
		for (j = 0; j < 8; j++) {
			if (8*i+j == bs->size) printf("(");
			printf("%d", (bs->buffer[i] >> j) & 1);
		}
	}
	if (bs->size%8) printf(")");
	printf("\n");
}

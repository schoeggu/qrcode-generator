#include "bitstream.h"
#include "util.h"

#include <stdio.h>
#include <string.h>

/*
 * private function,
 * adds a byte to the bitstream
 */
void bs_add_internal(bitstream* bs, byte data, int numBits);

bitstream* bs_init()
{
	bitstream* bs = malloc(sizeof(bitstream));
	bs->size = 0;
	bs->buffersize = 4; /*start with 4 bytes*/
	bs->buffer = calloc(bs->buffersize, sizeof(byte));
	return bs;
}

void bs_destroy(bitstream* bs) 
{
	free(bs->buffer);
	free(bs);
}

void bs_add_b(bitstream* bs, byte data, int numBits)
{
	
	data = reverse_bit_b(data); /*reverse bit order, otherwise data order is wrong*/
	data >>= (sizeof(data)*8 - numBits);
	bs_add_internal(bs, data, numBits);
}

void bs_add_bs(bitstream* bs, const byte* data, int dataSize, int numBits)
{
	if (!data) return;

	/* for every byte call bs_add_b() */
	while(numBits > 8 && dataSize) {
		bs_add_b(bs, *data, 8);
		data++;
		numBits -=8;
		dataSize--;
	}
	/* if we have trailing bits, add them too */
	if (dataSize) bs_add_b(bs, *data, numBits);

}

void bs_add_i(bitstream* bs, unsigned int data, int numBits) {
	data = reverse_bit_i(data);/*reverse bit order, otherwise data order is wrong*/
	data >>= (sizeof(data)*8 - numBits);
	/* add one byte after the other */
	while (numBits > 8) {
		bs_add_internal(bs, (byte)data, 8);
		data >>= 8;
		numBits -= 8;
	}
	bs_add_internal(bs, (byte)data, numBits);
}

void bs_add_internal(bitstream* bs, byte data, int numBits) 
{
	int i;

	/* if nescessary allocate new memory */
	if (bs->buffersize * 8 < bs->size + numBits) {
		bs->buffersize++;


		/* with realloc an error occurs at some datalengths
		 * *** glibc detected *** ./qrgen: free(): invalid next size (normal): 0x095a6400 ***
		 * don't know why, we just use a workaround until we find whats wrong
		 *
		 * bs->buffer = realloc(bs->buffer, sizeof(byte)*bs->buffersize);
		 *
		 */
		byte* tmp = calloc(bs->buffersize, sizeof(byte));
		memcpy(tmp, bs->buffer, bs->buffersize-1);
		free(bs->buffer);
		bs->buffer = tmp;
		tmp = NULL;
		



		bs->buffer[bs->buffersize-1] = 0;
		
	}

	int numBytes = bs->size / 8;
	int trailingbits = bs->size % 8;
	byte mask = 0;
	
	for (i = 0; i < numBits; i++) mask = (mask << 1) | 1;

	/* first, fill up the last byte of buffer */
	if (trailingbits) {
	   	bs->buffer[numBytes] |= ((mask & data) << trailingbits); 
		if (numBits+trailingbits >= 8) bs->buffer[numBytes] = reverse_bit_b(bs->buffer[numBytes]);
		numBytes++;
   	}
	/* then, put the rest to the next byte */
	if (numBits - (trailingbits ? 8 - trailingbits : 0) > 0) {
		bs->buffer[numBytes] |= (byte)((mask & data) >> (trailingbits ? 8 - trailingbits : 0));
		if (numBits - trailingbits == 8) bs->buffer[numBytes] = reverse_bit_b(bs->buffer[numBytes]);
	}

	bs->size += numBits;

}

byte bs_get(bitstream* bs, int index)
{
	if (index * 8 >= bs->size) return 0;
	return bs->buffer[index];
}

void  bs_geta(bitstream* bs, byte* dest, int index, int numBytes)
{
	if (!dest || !bs || index*8 >= bs->size) return;	

	if (bs->size < (numBytes+index)*8) {numBytes = bs->size/8-index;}
	memcpy(dest, bs->buffer+index, numBytes);
}


void bs_print_full(bitstream* bs) {
	printf("size: %u \t buffersize: %u\t\t", (unsigned int)bs->size, (unsigned int)bs->buffersize);
	int i, j;
	for (i = 0; i < bs->buffersize; i++) {
		printf (" ");
		for (j = 7; j >= 0; j--) {
			if (8*i+7-j == bs->size) printf("(");
			printf("%d", (bs->buffer[i] >> j) & 1);
		}
	}
	if (bs->size%8) printf(")");
	printf("\n");
}

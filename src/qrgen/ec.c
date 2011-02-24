#include "ec.h"
#include "gp.h"

#include "util.h"

#include <string.h> 
#include <stdlib.h> 
#include <stdio.h> 

static bool initialized = false;
static byte galois[256];

bool generateErrorCorrectionCode(const byte* const data, int dataSize, byte* dest, int ecBlocks)
{
	if (!data || !dest) {
		error("Cannot calculate Error correction, inputData or Dest is NULL");	
		return 0;
	}

	/* first get de Generator Polinomial */
	const byte* genpoly = get_gp(ecBlocks);
	if (!genpoly) {
		error("Cannot calculate Error correction, coudn't retreive Generator Polynomial");	
		return false;
	}

	int totalSize = dataSize + ecBlocks;
	int i;
	byte* errorCorrection = (byte*)calloc(totalSize, sizeof(char));

	/* put the data in the first elements of the errrCorrection Array */
	for (i = 0; i < dataSize; i++) {
		errorCorrection[i] = data[i];
	}

	/* for every element of data, add the alphas from the data-array with the generator polinomial, 
	 * and XOR its primitive Value with the current dataElement */
	for (i = 0; i < dataSize; i++) {
		byte add = petoal(errorCorrection[i]); /* alpha value of first dataBlock that is not 0*/
		int j;
		for (j = 0; j < ecBlocks+1; j++) {
			unsigned int alpha = add + genpoly[j];
			if (alpha >255U) alpha -= 255U; // alpha mustn't overflow
			errorCorrection[j+i] = altope(alpha) ^ errorCorrection[j+i];
		}
	}

	memcpy(dest, errorCorrection + dataSize, ecBlocks); //copy the errorCorrection Codes to the output Parameter

	free(errorCorrection);

	return true;
}

int bch(int totalBits, int dataBits, int data, int gpb) {
	int ecBits = totalBits - dataBits;
	int top = 1<<(totalBits-1);

	data <<= ecBits;
	gpb <<=(dataBits-1);

	while (top > 1<<(ecBits-1)) {
		if ((top & data)) {
			data ^= gpb;
		}
		top >>= 1;
		gpb >>= 1;
	}
	return data;
}



void calculate_galois_field(const int pp) 
{
	int i;
	unsigned int x;

	x = 1;
	galois[0] = 1;
	for (i = 1; i < 256; i++) {
		x *= 2;	
		if (x>255U) x ^= pp; // if it over flows, XOR with pp, so it's never over 255
		galois[i] = (byte) x;
	}
	initialized = true;
}

byte altope(const int alpha) {
	if (!initialized) { calculate_galois_field(PP); }
	if (alpha < 0 || alpha > 255) return 0; // not in range
	return galois[alpha];
}

int petoal(const byte integer) {
	if (!initialized) { calculate_galois_field(PP); }
	int i;
	for (i = 0; i < 256; i++) {
		if (galois[i] == integer) return i;
	}
	return -1;
}


#include "ec.h"
#include "gp.h"

#include "util.h"

#include <string.h> 
#include <stdlib.h> 

static int initialized = 0;
static byte galois[256];
static const int PP = 285;


int generateErrorCorrectionCode(const byte* const data, size_t dataSize, byte* dest, size_t ecBlocks)
{
	if (!data || !dest) return 0;
	const byte* gp = get_gp(ecBlocks);
	if (!gp) return 0;

	size_t totalSize = dataSize + ecBlocks;
	size_t i;
	byte* errorCorrection = (byte*)calloc(totalSize, sizeof(char));

	for (i = 0; i < dataSize; i++) {
		errorCorrection[i] = data[i];
	}

	for (i = 0; i < dataSize; i++) {
		byte add = petoal(errorCorrection[i]);
		size_t j;
		for (j = 0; j < ecBlocks+1; j++) {
			unsigned int alpha = add + gp[j];
			if (alpha >255U) alpha -= 255U;
			errorCorrection[j+i] = altope(alpha) ^ errorCorrection[j+i];
		}
	}

	memcpy(dest, errorCorrection + dataSize, ecBlocks);

	free(errorCorrection);

	return 1;
}


void calculate_galois_field(const int pp) 
{
	int i;
	unsigned int x;

	x = 1;
	galois[0] = 1;
	for (i = 1; i < 256; i++) {
		x *= 2;	
		if (x>255U) x ^= pp;
		galois[i] = (byte) x;
	}
	initialized = 1;
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


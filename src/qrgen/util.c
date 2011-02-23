#include "util.h"

#include <limits.h>

byte reverse_bit_b(byte b)
{
	byte r = b; // r will be reversed bits of v; first get LSB of v
	int s = sizeof(b) * CHAR_BIT - 1; // extra shift needed at end

	for (b >>= 1; b; b >>= 1)
	{   
		r <<= 1;
		r |= b & 1;
		s--;
	}
	r <<= s; // shift when v's highest bits are zero
	return r;
}

unsigned int reverse_bit_i(unsigned int i)
{
	unsigned int r = i; // r will be reversed bits of v; first get LSB of v
	int s = sizeof(i) * CHAR_BIT - 1; // extra shift needed at end

	for (i >>= 1; i; i >>= 1)
	{   
		r <<= 1;
		r |= i & 1;
		s--;
	}
	r <<= s; // shift when v's highest bits are zero
	return r;
}
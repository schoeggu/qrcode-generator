#ifndef DATAENCODER_H
#define DATAENCODER_H

#include "util.h"
#include "bitstream.h"


typedef enum {
	EC_NONE = 0,
	EC_L,
	EC_M,
	EC_Q,
	EC_H
} ECLevel;

typedef enum {
	ModeNumeric = 1, // 0001
	ModeAlpha =   2, // 0010
	ModeByte =    4  // 0100
} EncodeModeIndicator;

static const byte Terminator = 0; //     0000
static const byte Padding1 = 236; // 11101100
static const byte Padding2 = 17;  // 00010001


/*
 * This needs to be extended
 * now only Version 1-9 are coded
 * Version    Numeric   Alpha   Byte
 * 1 -  9     10         9       8
 * 10 - 26    12        11      16
 * 27 - 40	  13        13      16
 */
typedef enum {
	CountByte      = 8,
	CountAlpha,  //  9
	CountNumeric // 10
} CharacterCountBitsCount;


void encodeData(bitstream* bs, const byte* data, EncodeModeIndicator mode, int dataSize);
int getBitCount(int numChars, EncodeModeIndicator mode);

#endif //DATAENCODER_H

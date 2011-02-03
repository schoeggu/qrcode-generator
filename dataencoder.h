#ifndef DATAENCODER_H
#define DATAENCODER_H

#include "util.h"
#include "bitstream.h"


enum EncodeModeIndicator {
	ModeNumeric = 1, // 0001
	ModeAlpha =   2, // 0010
	ModeByte =    4  // 0100
};

static const byte Terminator = 0; // 0000


/*
 * This needs to be extended
 * now only Version 1-9 are coded
 * Version    Numeric   Alpha   Byte
 * 1 -  9     10         9       8
 * 10 - 26    12        11      16
 * 27 - 40	  13        13      16
 */
enum CharacterCountBitsCount {
	CountByte      = 8,
	CountAlpha,  //  9
	CountNumeric // 10
};


void encodeData(bitstream* bs, const byte* data, enum EncodeModeIndicator mode, int dataSize);

#endif //DATAENCODER_H

#ifndef DATAENCODER_H
#define DATAENCODER_H

#include "util.h"
#include "bitstream.h"
#include "symbolinfo.h"


static const byte Terminator = 0; //     0000
static const byte Padding[] = {236, 17}; // 11101100 and 00010001


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

struct SymbolInfo;

bool encodeData(bitstream* bs, const SymbolInfo* si);
int getBitCount(int numChars, EncodeModeIndicator mode);

#endif //DATAENCODER_H

#ifndef DATAENCODER_H
#define DATAENCODER_H

#include "util.h"
#include "bitstream.h"
#include "symbolinfo.h"


static const byte Terminator = 0; //     0000
static const byte Padding[] = {236, 17}; // 11101100 and 00010001


struct SymbolInfo;

bool encodeData(bitstream* bs, const SymbolInfo* si);
int getBitCount(int numChars, EncodeModeIndicator mode, int version);
int getByteCount(int numChars, EncodeModeIndicator mode, int version);
int getCharacterCountBitCount(EncodeModeIndicator mode, int version);

#endif //DATAENCODER_H

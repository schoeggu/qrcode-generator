#ifndef DATAENCODER_H
#define DATAENCODER_H

#include "util.h"

enum EncodeMode {
	Numeric = 1, // 0001
	Alpha =   2, // 0010
	Byte =    4  // 0100
};

const static byte Terminator = 0; // 0000

//enum CharacterCountBits



#endif //DATAENCODER_H

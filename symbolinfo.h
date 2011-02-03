#ifndef SYMBOLINFO_H
#define SYMBOLINFO_H

#include "dataencoder.h"

typedef struct {
	int version;	
	int autoVersion;

	int ecLevel;

	const byte* data;
	int dataSize;
	int encodedDataSize;

	enum EncodeMode m;
	
	
	int totalCW;
	int dataCW;
	int ecCW;

	int blocks;

	
} symbolinfo;

typedef struct {
	int totalCodeWords;	
	int ecCodeWords[4];
	int dataCodeWords[4];
	int blocks[4];
} codeWords;

codeWords* cw;
static int cw_initialized;

int si_init_codewords();


int si_check_integrity(symbolinfo* si);

#endif //SYMBOLINFO_H

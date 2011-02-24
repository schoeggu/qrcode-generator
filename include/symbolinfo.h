#ifndef SYMBOLINFO_H
#define SYMBOLINFO_H

#include "util.h"

static const int VERSION_AUTO = 0;
static const int MASK_AUTO = -1;

typedef enum {
	EC_NONE = 4,
	EC_L = 1, // 01
	EC_M = 0, // 00
	EC_Q = 3, // 11
	EC_H = 2  // 10
} ECLevel;

typedef enum {
	ModeNumeric = 1, // 0001
	ModeAlpha =   2, // 0010
	ModeByte =    4  // 0100
} EncodeModeIndicator;

typedef struct {
	int totalCodeWords;             /* Total Number of CodeWords in Block */
	int ecCodeWords;                /* Number of Error Correction Code Words in Block */
	int dataCodeWords;              /* Number of data Code Words in Block */
} Block;

typedef struct {
	int numberOfBlocks;             /* Number of Data Blocks in Symbol */
	Block* block;                   /* Array of Blocks */
} BlockInfo;

typedef struct {
	int version;					/* Symbol version 1-40, 0 for auto */
	bool autoVersion;				/* Choose version automatically */

	ECLevel ecLevel;				/* Error Correction Level: L, M, Q or H */

	byte* inputData;			    /* Pointer to the data */
	byte* encodedData;   			/* Pointer to the final data set */
	int dataCount;					/* Count of how many data elements */
	int encodedDataCount;           /* Number of bytes after encoding */
	EncodeModeIndicator encodeMode; /* Encode Mode: Byte, Alph or Numeric */
	
	int totalCodeWords;				/* Total number of codeWords in Symbol */
	int ecCodeWords;				/* Number of Error Correction Code Words in Symbol */
	int dataCodeWords;				/* Number of data Code Words in Symbol */

	BlockInfo blockInfo;	     	/* Datablocks of Symbol */
	
	int mask;						/* Mask indicator. 3 bit */
	bool automask;                  /* Choose mask automatically */

	int formatInfo;  				/* Encoded  Format Info. 15bit */
	int versionInfo;				/* Encoded Version Info. 18bit */

} SymbolInfo;


SymbolInfo* si_create(const byte* data, int dataLen);
SymbolInfo* si_create_conf(const byte* data, int dataCount, int version, int mode, int level, int mask);

void si_destroy(SymbolInfo* si);

bool si_set_version(SymbolInfo* si, int version);
bool si_set_auto_version(SymbolInfo* si);

bool si_set_data(SymbolInfo* si, const byte* data, int dataCount);
bool si_set_mode(SymbolInfo* si, int mode);
bool si_set_eclevel(SymbolInfo* si, int level);

bool si_set_mask(SymbolInfo* si, int mask);
void si_set_auto_mask(SymbolInfo* si);

bool si_encode(SymbolInfo* si);


int get_min_version(int dataCount, int encMode, int ecLevel);
bool fits_to_version(int version, int dataCount, int encMode, int ecLevel);
int get_total_codewords(int version);
int get_ec_codewords(int version, int ecLevel);
int get_data_codewords(int version, int ecLevel);
BlockInfo get_block_info(int version, int ecLevel);

#endif //SYMBOLINFO_H

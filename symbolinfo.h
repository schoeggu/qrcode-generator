#ifndef SYMBOLINFO_H
#define SYMBOLINFO_H

//#include "dataencoder.h"
#include "util.h"

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


typedef struct {
	int version;					/* Symbol version 1-40, 0 for auto */
	bool autoVersion;				/* Choose version automatically */

	ECLevel ecLevel;				/* Error Correction Level: L, M, Q or H */

	const byte* inputData;			/* Pointer to the data */
	int dataCount;					/* Count of how many data elements */
	int encodedDataCount;           /* Number of bytes after encoding */
	EncodeModeIndicator encodeMode; /* Encode Mode: Byte, Alph or Numeric */

	int totalCodeWords;				/* Total number of codeWords in Symbol */
	int ecCodeWords;				/* Number of Error Correction Code Words in Symbol */
	int dataCodeWords;				/* Number of data Code Words in Symbol */

	int blocks;						/* Number of dataBlocks in Symbol */
	
} SymbolInfo;

typedef struct {
	int totalCodeWords;	
	int ecCodeWords[4];
	int dataCodeWords[4];
	int blocks[4];
} codeWords;


bool si_init(SymbolInfo* si, const byte* data, int dataCound, EncodeModeIndicator mode, ECLevel level);
void si_reset(SymbolInfo* si);

bool si_init_codewords();
void si_destroy_codewords();

bool si_set_version(SymbolInfo* si, int version);
bool si_set_data(SymbolInfo* si, const byte* data, int dataCount, EncodeModeIndicator mode);
bool si_set_eclevel(SymbolInfo* si, ECLevel level);

bool si_check_integrity(SymbolInfo* si);

int get_min_version(int dataCount, int ecLevel);

int get_total_codewords(int version);
int get_ec_codewords(int version, int ecLevel);
int get_data_codewords(int version, int ecLevel);


#endif //SYMBOLINFO_H

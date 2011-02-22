#include "symbolinfo.h"
#include "dataencoder.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int cw_initialized = 0;
codeWords* cw;

static const int si_indicator[200][7] = {

		{   1,   26},
		{   7,    1,   26,   19,    0,    0,    0},
		{  10,    1,   26,   16,    0,    0,    0},
		{  13,    1,   26,   13,    0,    0,    0},
		{  17,    1,   26,    9,    0,    0,    0},

		{   2,   44},
		{  10,    1,   44,   34,    0,    0,    0},
		{  16,    1,   44,   28,    0,    0,    0},
		{  22,    1,   44,   22,    0,    0,    0},
		{  28,    1,   44,   16,    0,    0,    0},

		{   3,   70},
		{  15,    1,   70,   55,    0,    0,    0},
		{  26,    1,   70,   44,    0,    0,    0},
		{  36,    2,   35,   17,    0,    0,    0},
		{  44,    2,   35,   13,    0,    0,    0},

		{   4,  100},
		{  20,    1,  100,   80,    0,    0,    0},
		{  36,    2,   50,   32,    0,    0,    0},
		{  52,    2,   50,   24,    0,    0,    0},
		{  64,    4,   25,    9,    0,    0,    0},

		{   5,  134},
		{  26,    1,  134,  108,    0,    0,    0},
		{  48,    2,   67,   43,    0,    0,    0},
		{  72,    2,   33,   15,    2,   34,   16},
		{  88,    2,   33,   11,    2,   34,   12},

		{   6,  172},
		{  36,    2,   86,   68,    0,    0,    0},
		{  64,    4,   43,   27,    0,    0,    0},
		{  96,    4,   43,   19,    0,    0,    0},
		{ 112,    4,   43,   15,    0,    0,    0},

		{   7,  196},
		{  40,    2,   98,   78,    0,    0,    0},
		{  72,    4,   49,   31,    0,    0,    0},
		{ 108,    2,   32,   14,    4,   33,   15},
		{ 130,    4,   39,   13,    1,   40,   14},

		{   8,  242},
		{  48,    2,  121,   97,    0,    0,    0},
		{  88,    2,   60,   38,    2,   61,   39},
		{ 132,    4,   40,   18,    2,   41,   19},
		{ 156,    4,   40,   14,    2,   41,   15},

		{   9,  292},
		{  60,    2,  146,  116,    0,    0,    0},
		{ 110,    3,   58,   36,    2,   59,   37},
		{ 160,    4,   36,   16,    4,   37,   17},
		{ 192,    4,   36,   12,    4,   37,   13},

		{  10,  346},
		{  72,    2,   86,   68,    2,   87,   69},
		{ 130,    4,   69,   43,    1,   70,   44},
		{ 192,    6,   43,   19,    2,   44,   20},
		{ 224,    6,   43,   15,    2,   44,   16},

		{  11,  404},
		{  80,    4,  101,   81,    0,    0,    0},
		{ 150,    1,   80,   50,    4,   81,   51},
		{ 224,    4,   50,   22,    4,   51,   23},
		{ 264,    3,   36,   12,    8,   37,   13},

		{  12,  466},
		{  96,    2,  116,   92,    2,  117,   93},
		{ 176,    6,   58,   36,    2,   59,   37},
		{ 260,    4,   46,   20,    6,   47,   21},
		{ 308,    7,   42,   14,    4,   43,   15},

		{  13,  532},
		{ 104,    4,  133,  107,    0,    0,    0},
		{ 198,    8,   59,   37,    1,   60,   38},
		{ 288,    8,   44,   20,    4,   45,   21},
		{ 352,   12,   33,   11,    4,   34,   12},

		{  14,  581},
		{ 120,    3,  145,  115,    1,  146,  116},
		{ 216,    4,   64,   40,    5,   65,   41},
		{ 320,   11,   36,   16,    5,   37,   17},
		{ 384,   11,   36,   12,    5,   37,   13},

		{  15,  655},
		{ 132,    5,  109,   87,    1,  110,   88},
		{ 240,    5,   65,   41,    5,   66,   42},
		{ 360,    5,   54,   24,    7,   55,   25},
		{ 432,   11,   36,   12,    7,   37,   13},

		{  16,  733},
		{ 144,    5,  122,   98,    1,  123,   99},
		{ 280,    7,   73,   45,    3,   74,   46},
		{ 408,   15,   43,   19,    2,   44,   20},
		{ 480,    3,   45,   15,   13,   46,   16},

		{  17,  815},
		{ 168,    1,  135,  107,    5,  136,  108},
		{ 308,   10,   74,   46,    1,   75,   47},
		{ 448,    1,   50,   22,   15,   51,   23},
		{ 532,    2,   42,   14,   17,   43,   15},

		{  18,  901},
		{ 180,    5,  150,  120,    1,  151,  121},
		{ 338,    9,   69,   43,    4,   70,   44},
		{ 504,   17,   50,   22,    1,   51,   23},
		{ 588,    2,   42,   14,   19,   43,   15},

		{  19,  991},
		{ 196,    3,  141,  113,    4,  142,  114},
		{ 364,    3,   70,   44,   11,   71,   45},
		{ 546,   17,   47,   21,    4,   48,   22},
		{ 650,    9,   39,   13,   16,   40,   14},

		{  20, 1085},
		{ 224,    3,  135,  107,    5,  136,  108},
		{ 416,    3,   67,   41,   13,   68,   42},
		{ 600,   15,   54,   24,    5,   55,   25},
		{ 700,   15,   43,   15,   10,   44,   16},

		{  21, 1156},
		{ 224,    4,  144,  116,    4,  145,  117},
		{ 442,   17,   68,   42,    0,    0,    0},
		{ 644,   17,   50,   22,    6,   51,   23},
		{ 750,   19,   46,   16,    6,   47,   17},

		{  22, 1258},
		{ 252,    2,  139,  111,    7,  140,  112},
		{ 476,   17,   74,   46,    0,    0,    0},
		{ 690,    7,   54,   24,   16,   55,   25},
		{ 816,   34,   37,   13,    0,    0,    0},

		{  23, 1364},
		{ 270,    4,  151,  121,    5,  152,  122},
		{ 504,    4,   75,   47,   14,   76,   48},
		{ 750,   11,   54,   24,   14,   55,   25},
		{ 900,   16,   45,   15,   14,   46,   16},

		{  24, 1474},
		{ 300,    6,  147,  117,    4,  148,  118},
		{ 560,    6,   73,   45,   14,   74,   46},
		{ 810,   11,   54,   24,   16,   55,   25},
		{ 960,   30,   46,   16,    2,   47,   17},

		{  25, 1588},
		{ 312,    8,  132,  106,    4,  133,  107},
		{ 588,    8,   75,   47,   13,   76,   48},
		{ 870,    7,   54,   24,   22,   55,   25},
		{1050,   22,   45,   15,   13,   46,   16},

		{  26, 1706},
		{ 336,   10,  142,  114,    2,  143,  115},
		{ 644,   19,   74,   46,    4,   75,   47},
		{ 952,   28,   50,   22,    6,   51,   23},
		{1110,   33,   46,   16,    4,   47,   17},

		{  27, 1828},
		{ 360,    8,  152,  122,    4,  153,  123},
		{ 700,   22,   73,   45,    3,   74,   46},
		{1020,    8,   53,   23,   26,   54,   24},
		{1200,   12,   45,   15,   28,   46,   16},

		{  28, 1921},
		{ 390,    3,  147,  117,   10,  148,  118},
		{ 728,    3,   73,   45,   23,   74,   46},
		{1050,    4,   54,   24,   31,   55,   25},
		{1260,   11,   45,   15,   31,   46,   16},
		
		{  29, 2051},
		{ 420,    7,  146,  116,    7,  147,  117},
		{ 784,   21,   73,   45,    7,   74,   46},
		{1140,    1,   53,   23,   37,   54,   24},
		{1350,   19,   45,   15,   26,   46,   16},

		{  30, 2185},
		{ 450,    5,  145,  115,   10,  146,  116},
		{ 812,   19,   75,   47,   10,   76,   48},
		{1200,   15,   54,   24,   25,   55,   25},
		{1440,   23,   45,   15,   25,   46,   16},

		{  31, 2323},
		{ 480,   13,  145,  115,    3,  146,  116},
		{ 868,    2,   74,   46,   29,   75,   47},
		{1290,   42,   54,   24,    1,   55,   25},
		{1530,   23,   45,   15,   28,   46,   16},

		{  32, 2465},
		{ 510,   17,  145,  115,    0,    0,    0},
		{ 924,   10,   74,   46,   23,   75,   47},
		{1350,   10,   54,   24,   35,   55,   25},
		{1620,   19,   45,   15,   35,   46,   16},

		{  33, 2611},
		{ 540,   17,  145,  115,    1,  146,  116},
		{ 980,   14,   74,   46,   21,   75,   47},
		{1440,   29,   54,   24,   19,   55,   25},
		{1710,   11,   45,   15,   46,   46,   16},

		{  34, 2761},
		{ 570,   13,  145,  115,    6,  146,  116},
		{1036,   14,   74,   46,   23,   75,   47},
		{1530,   44,   54,   24,    7,   55,   25},
		{1800,   59,   46,   16,    1,   47,   17},

		{  35, 2876},
		{ 570,   12,  151,  121,    7,  152,  122},
		{1064,   12,   75,   47,   26,   76,   48},
		{1590,   39,   54,   24,   14,   55,   25},
		{1890,   22,   45,   15,   41,   46,   16},

		{  36, 3034},
		{ 600,    6,  151,  121,   14,  152,  122},
		{1120,    6,   75,   47,   34,   76,   48},
		{1680,   46,   54,   24,   10,   55,   25},
		{1980,    2,   45,   15,   64,   46,   16},

		{  37, 3196},
		{ 630,   17,  152,  122,    4,  153,  123},
		{1204,   29,   74,   46,   14,   75,   47},
		{1770,   49,   54,   24,   10,   55,   25},
		{2100,   24,   45,   15,   46,   46,   16},

		{  38, 3362},
		{ 660,    4,  152,  122,   18,  153,  123},
		{1260,   13,   74,   46,   32,   75,   47},
		{1860,   48,   54,   24,   14,   55,   25},
		{2220,   42,   45,   15,   32,   46,   16},

		{  39, 3532},
		{ 720,   20,  147,  117,    4,  148,  118},
		{1316,   40,   75,   47,    7,   76,   48},
		{1950,   43,   54,   24,   22,   55,   25},
		{2310,   10,   45,   15,   67,   46,   16},

		{  40, 3706},
		{ 750,   19,  148,  118,    6,  149,  119},
		{1372,   18,   75,   47,   31,   76,   48},
		{2040,   34,   54,   24,   34,   55,   25},
		{2430,   20,   45,   15,   61,   46,   16}

};

bool si_init(SymbolInfo* si, const byte* data, int dataCount, EncodeModeIndicator mode, ECLevel level)
{
	si->encodedData = NULL;
	si->blockInfo.block = NULL;
	si_reset(si);
	si_set_data(si, data, dataCount, mode);
	si_set_eclevel(si, level);

	return si_check_integrity(si);
}

bool si_set_version(SymbolInfo* si, int version)
{
	if (!si || version < 0 || version > 40) return false; 
	si->version = version;
	si->autoVersion = !version; // si_set_version(0) == autoversion

	return si_check_integrity(si);
}

bool si_set_data(SymbolInfo* si, const byte* data, int dataCount, EncodeModeIndicator mode)
{
	int tmp;

	if (!data) {
		fprintf(stderr, "Error: no Input data\n");			
		return false;
	}

	si->inputData = data;
	si->dataCount = dataCount;
	si->encodeMode = mode;
	
	tmp = getBitCount(dataCount, mode);
	si->encodedDataCount = tmp / 8 + ((tmp % 8) ? 1 : 0);

	return si_check_integrity(si);
}

bool si_set_eclevel(SymbolInfo* si, ECLevel level)
{
	if (!si || level < 0 || level > 3) {
		fprintf(stderr, "Error: invalid EC_Level\n");
		return false;	
	}
	si->ecLevel = level;

	return si_check_integrity(si);
}

bool si_check_integrity(SymbolInfo* si)
{
	if (!si) {
		fprintf(stderr, "Error: SymbolInfo is NULL\n");
		return false;	
	}

	int version = si->version;
	bool autoVersion = si->autoVersion;

	ECLevel ecLevel = si->ecLevel;

	int encodedDataCount = si->encodedDataCount;

	// if we need, choose the minimum version
	if(autoVersion && ecLevel  < 4 && encodedDataCount) {
		si->version = get_min_version(encodedDataCount, ecLevel);
		version = si->version;
		if (!si->version) {
			fprintf(stderr, "Error: Cannot choose version, lower, EC_Level or DataSize\n");
			return false;
		}
	// otherwise check if the data fits in chosen version
	} else if (version && ecLevel < 4 && encodedDataCount) {
		if (encodedDataCount > get_data_codewords(version, ecLevel)) {
			fprintf(stderr, "Error: Specified Version is too small\n");
			return false;
		}
	}

	if (version) {
		si->totalCodeWords = get_total_codewords(version);
		if (ecLevel < 4) {
			si->dataCodeWords = get_data_codewords(version, ecLevel);
			si->ecCodeWords = get_ec_codewords(version, ecLevel);
		}
	}


	if (version && ecLevel < 4) {
		si->blockInfo = get_block_info(version, ecLevel);
	}

	return true;
}

int get_min_version(int dataCount, int ecLevel) 
{
	int i;
	if (dataCount < 1 || ecLevel < 0 || ecLevel > 3) return 0;
	if (!cw_initialized) si_init_codewords();
	for (i = 0; i < 40; i++) {
		if (get_data_codewords(i+1, ecLevel) >= dataCount) return i+1;
	}
	return 0;
}

int get_data_codewords(int version, int ecLevel)
{
	if (version > 0 && version < 41 && ecLevel >= 0 && ecLevel < 4) {
		if (!cw_initialized) si_init_codewords();
		return cw[version - 1].dataCodeWords[ecLevel];
	}
	return 0;
}

int get_ec_codewords(int version, int ecLevel)
{
	if (version > 0 && version < 41 && ecLevel >= 0 && ecLevel < 4) {
		if (!cw_initialized) si_init_codewords();
		return cw[version - 1].ecCodeWords[ecLevel];
	}
	return 0;
}

int get_total_codewords(int version)
{
	if (version > 0 && version < 41) { 
		if (!cw_initialized) si_init_codewords();
		return cw[version - 1].totalCodeWords;
	}
	return 0;
}

BlockInfo get_block_info(int version, int ecLevel)
{
	if (version > 0 && version < 41) {
		return cw[version-1].blocks[ecLevel];
	}	
	BlockInfo b;
	b.numberOfBlocks = 0;
	b.block = NULL;
	return b;
	
}



void si_reset(SymbolInfo* si)
{
	si->version                  = 0;
	si->autoVersion              = true;
	si->ecLevel                  = EC_NONE;
	si->encodeMode               = 0;
	si->inputData                = NULL;
	si->dataCount                = 0;
	si->encodedDataCount         = 0;
	si->totalCodeWords           = 0;
	si->ecCodeWords              = 0;
	si->dataCodeWords            = 0;
	si->mask                     = 0;
	si->blockInfo.numberOfBlocks = 0;
	si->blockInfo.block          = NULL;

	if (si->encodedData) {
		free(si->encodedData);
		si->encodedData = NULL;
	}
}




bool si_init_codewords()
{
	cw = calloc(40, sizeof(codeWords));

	int version;
	for (version = 0; version < 40; version++) {
		int versionIndex = version * 5;

		int totalCW = si_indicator[versionIndex][1];
		cw[version].totalCodeWords = totalCW;

		int i;
		ECLevel l[] = { EC_L, EC_M, EC_Q, EC_H };
		versionIndex++;
		for (i = 0; i < 4; i++) {
			int j = 0;
			ECLevel ecLevel = l[i];



			int ecCW = si_indicator[versionIndex+i][j++];

			cw[version].ecCodeWords[ecLevel] = ecCW;
			cw[version].dataCodeWords[ecLevel] = totalCW - ecCW;

			int numFirstBlock = 0;
			int numSecondBlock = 0;
			int totalFirstBlock = 0;
			int totalSecondBlock = 0;
			int dataFirstBlock = 0;
			int dataSecondBlock = 0;

			numFirstBlock = si_indicator[versionIndex+i][j++];

			totalFirstBlock = si_indicator[versionIndex+i][j++];

			dataFirstBlock = si_indicator[versionIndex+i][j++];

			numSecondBlock = si_indicator[versionIndex+i][j++];

			totalSecondBlock = si_indicator[versionIndex+i][j++];
	
			dataSecondBlock = si_indicator[versionIndex+i][j++];

			cw[version].blocks[ecLevel].numberOfBlocks = numFirstBlock + numSecondBlock;
			cw[version].blocks[ecLevel].block = calloc(numFirstBlock + numSecondBlock, sizeof(Block));

			int k;
			for (k = 0; k < numFirstBlock; k++) {
				cw[version].blocks[ecLevel].block[k].totalCodeWords = totalFirstBlock;
				cw[version].blocks[ecLevel].block[k].dataCodeWords = dataFirstBlock;
				cw[version].blocks[ecLevel].block[k].ecCodeWords = totalFirstBlock - dataFirstBlock;
			}
			for (; k < numFirstBlock + numSecondBlock; k++) {
				cw[version].blocks[ecLevel].block[k].totalCodeWords = totalSecondBlock;
				cw[version].blocks[ecLevel].block[k].dataCodeWords = dataSecondBlock;
				cw[version].blocks[ecLevel].block[k].ecCodeWords = totalSecondBlock - dataSecondBlock;
			}
		}
	}

	cw_initialized = true;	

	return true;
}

void si_destroy_codewords()
{
	if (cw_initialized) {
		int i;
		int j;
		for (j = 0; j < 40; j++) {
			for(i = 0; i < 4; i++) {
				free(cw[j].blocks[i].block);
			}
		}
		free(cw);
		cw_initialized = false;
	}
}

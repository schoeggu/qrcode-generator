#include "symbolinfo.h"
#include "dataencoder.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int cw_initialized = 0;
codeWords* cw;

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
	FILE* file = fopen("./si.rc", "r");
	if (!file) {
		exit(1);
	}
	
	cw = calloc(40, sizeof(codeWords));

	char buffer[256];
	char delim[] = " ";
	char* number;
	while (fgets(buffer, 256, file) != NULL) {
		if (buffer[0] == '#') {continue;} // skip comments
		number = strtok(buffer, delim);
		int version = atoi(number);
		if (version < 0 || version > 40) return false;

		number = strtok(NULL, delim);
		int totalCW = atoi(number);
		cw[version-1].totalCodeWords = totalCW;

		int i;
		ECLevel l[] = { EC_L, EC_M, EC_Q, EC_H };
		for (i = 0; i < 4; i++) {
			if(!fgets(buffer, 256, file)) return false;
			ECLevel ecLevel = l[i];

			number = strtok(buffer, delim);

			int ecCW = atoi(number);

			cw[version-1].ecCodeWords[ecLevel] = ecCW;
			cw[version-1].dataCodeWords[ecLevel] = totalCW - ecCW;

			int numFirstBlock = 0;
			int numSecondBlock = 0;
			int totalFirstBlock = 0;
			int totalSecondBlock = 0;
			int dataFirstBlock = 0;
			int dataSecondBlock = 0;

			number = strtok(NULL, delim);
			numFirstBlock = atoi(number);

			number = strtok(NULL, delim);
			totalFirstBlock = atoi(number);

			number = strtok(NULL, delim);
			dataFirstBlock = atoi(number);

			number = strtok(NULL, delim);
			if (number) {
				numSecondBlock = atoi(number);

				number = strtok(NULL, delim);
				totalSecondBlock = atoi(number);
	
				number = strtok(NULL, delim);
				dataSecondBlock = atoi(number);
			}

			cw[version-1].blocks[ecLevel].numberOfBlocks = numFirstBlock + numSecondBlock;
			cw[version-1].blocks[ecLevel].block = calloc(numFirstBlock + numSecondBlock, sizeof(Block));
			int j;
			for (j = 0; j < numFirstBlock; j++) {
				cw[version-1].blocks[ecLevel].block[j].totalCodeWords = totalFirstBlock;
				cw[version-1].blocks[ecLevel].block[j].dataCodeWords = dataFirstBlock;
				cw[version-1].blocks[ecLevel].block[j].ecCodeWords = totalFirstBlock - dataFirstBlock;
			}
			for (; j < numFirstBlock + numSecondBlock; j++) {
				cw[version-1].blocks[ecLevel].block[j].totalCodeWords = totalSecondBlock;
				cw[version-1].blocks[ecLevel].block[j].dataCodeWords = dataSecondBlock;
				cw[version-1].blocks[ecLevel].block[j].ecCodeWords = totalSecondBlock - dataSecondBlock;
			}
		}
	}

	fclose(file);
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

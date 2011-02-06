#include "symbolinfo.h"
#include "dataencoder.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int cw_initialized = 0;
codeWords* cw;

bool si_init(SymbolInfo* si, const byte* data, int dataCount, EncodeModeIndicator mode, ECLevel level)
{
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

	if (!data) return false;

	si->inputData = data;
	si->dataCount = dataCount;
	si->encodeMode = mode;
	
	tmp = getBitCount(dataCount, mode);
	si->encodedDataCount = tmp / 8 + ((tmp % 8) ? 1 : 0);

	return si_check_integrity(si);
}

bool si_set_eclevel(SymbolInfo* si, ECLevel level)
{
	if (!si || level < 1 || level > 4) return false;	
	si->ecLevel = level;

	return si_check_integrity(si);
}

bool si_check_integrity(SymbolInfo* si)
{
	if (!si) return false;

	int version = si->version;
	bool autoVersion = si->autoVersion;

	ECLevel ecLevel = si->ecLevel;

	int encodedDataCount = si->encodedDataCount;

	int blocks;
	

	// if we need, choose the minimum version
	if(autoVersion && ecLevel && encodedDataCount) {
		si->version = get_min_version(encodedDataCount, ecLevel);
		if (!version) return false;
	// otherwise check if the data fits in chosen version
	} else if (version && ecLevel && encodedDataCount) {
		if (encodedDataCount > get_data_codewords(version, ecLevel)) return false;
	}

	if (version) {
		si->totalCodeWords = get_total_codewords(version);
		if (ecLevel) {
			si->dataCodeWords = get_data_codewords(version, ecLevel);
			si->ecCodeWords = get_ec_codewords(version, ecLevel);
		}
	}


	//TODO blocks if version and eclevel



	return true;
}

int get_min_version(int dataCount, int ecLevel) 
{
	int i;
	if (dataCount < 1 || ecLevel < 1 || ecLevel > 4) return 0;
	if (!cw_initialized) si_init_codewords();
	for (i = 0; i < 40; i++) {
		printf("v[%d], cw[%d], encdat[%d]\n", i+1, get_data_codewords(i+1, ecLevel), dataCount);
		if (get_data_codewords(i+1, ecLevel) >= dataCount) return i+1;
	}
	return 0;
}

int get_data_codewords(int version, int ecLevel)
{
	if (version > 0 && version < 41 && ecLevel > 0 && ecLevel < 5) {
		if (!cw_initialized) si_init_codewords();
		return cw[version - 1].dataCodeWords[ecLevel - 1];
	}
	return 0;
}

int get_ec_codewords(int version, int ecLevel)
{
	if (version > 0 && version < 41 && ecLevel > 0 && ecLevel < 5) {
		if (!cw_initialized) si_init_codewords();
		return cw[version - 1].ecCodeWords[ecLevel - 1];
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



void si_reset(SymbolInfo* si)
{
	si->version          = 0;
	si->autoVersion      = true;
	si->ecLevel          = EC_NONE;
	si->encodeMode       = 0;
	si->inputData        = NULL;
	si->dataCount        = 0;
	si->encodedDataCount = 0;
	si->totalCodeWords   = 0;
	si->ecCodeWords      = 0;
	si->dataCodeWords    = 0;
	si->blocks           = 0;
}




bool si_init_codewords()
{
	FILE* file = fopen("./si.rc", "r");
	if (!file) {
		exit(1);
	}

	
	cw = calloc(40, sizeof(codeWords));

	int index = 0;
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
		for(index = 0; index < 4; index++) {
			number = strtok(NULL, delim);
			int ecCW = atoi(number);
			cw[version-1].ecCodeWords[index] = ecCW;
			cw[version-1].dataCodeWords[index] = totalCW - ecCW;

			number = strtok(NULL, delim);
			int blocks = atoi(number);
			cw[version-1].blocks[index] = blocks;
		
		}

	}

	fclose(file);
	cw_initialized = true;	

	return true;
}

void si_destroy_codewords()
{
	if (cw_initialized) {
		free(cw);
		cw_initialized = false;
	}
}

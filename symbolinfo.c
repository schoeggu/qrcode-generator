#include "symbolinfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int cw_initialized = 0;

int si_check_integrity(symbolinfo* si)
{

	if (!si) return 0;

	int version = si->version;
	int autoVersion = si->autoVersion;

	int ecLevel = si->ecLevel;

	int encodedDataSize = si->encodedDataSize;

	int totalCW;
	int dataCW;
	int ecCW;

	int blocks;


	//select version if autoversion, ecLevel, encodedDataSize
	//check if not autoversion, version eclevel encodeddatasize must be ok
	//select totalCW if version
	//select dataCW if version ecLevel
	//select ecCW if version ecLevel
	//select blocks if version

	if(autoVersion && ecLevel && encodedDataSize) {
		version = si_get_min_Version(encodedDataSize, ecLevel);
		if (!version) return 0;
	} else if (version && ecLevel && encodedDataSize) {
		if (encodedDataSize > si_get_max_dataSize(version, ecLevel)) return 0;
	}

	return 1;

}

int si_init_codewords()
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
		if (buffer[0] == '#') {continue; } // skip comments
		number = strtok(buffer, delim);
		int version = atoi(number);
		if (version < 0 || version > 40) return 0;


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
	cw_initialized = 1;	

	return 1;
}

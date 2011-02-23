#include "symbolinfo.h"
#include "dataencoder.h"
#include "bitstream.h"
#include "ec.h"
#include "si.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


bool blockwiseErrorCorrection(SymbolInfo* si, bitstream* bs);

SymbolInfo* si_create(const byte* data, int dataCount)
{
	if (!data || dataCount < 0) {
		fprintf(stderr, "Error: no data specified\n");
		return NULL;
	}
	
	SymbolInfo* si = calloc(sizeof(SymbolInfo), 1);
	si->version = VERSION_AUTO;
	si->autoVersion = true;

	si->ecLevel = EC_M;

	si->inputData = data;
	si->dataCount = dataCount;
		
	si->encodeMode = ModeByte;
	
	si->mask = MASK_AUTO;
	si->automask = true;

	return si;
}

SymbolInfo* si_create_conf(const byte* data, int dataCount, int version, int mode, int level, int mask)
{
	SymbolInfo* si = si_create(data, dataCount);
	if (!(si_set_mode(si, mode) && si_set_eclevel(si, level) && si_set_version(si, version) && si_set_mask(si, mask))) {
		// something did go wrong. free si and return NULL
		// no need for an error message. the cause of the problem has been printed in the failed si_set_...() function
		si_destroy(si);
		return NULL;
	}
	
	return si;
}

void si_destroy(SymbolInfo* si)
{
	if (si->encodedData) {
		free(si->encodedData);
		si->encodedData = NULL;
	}
	if(si->blockInfo.block) {
		free(si->blockInfo.block);
	}
	free(si);
}

bool si_set_version(SymbolInfo* si, int version)
{
	if (!si || version < 0 || version > 40) {
		fprintf(stderr, "Error: specified version not in range: %d", version);
		return false; 
	}
	if (!fits_to_version(version, si->dataCount, si->encodeMode, si->ecLevel)) {
		fprintf(stderr, "Error: cannot set version. version has not enough data capacity. Minimal required version: %d\n", get_min_version(si->dataCount, si->encodeMode, si->ecLevel));
		return false;
	}

	si->version = version;
	si->autoVersion = (version == VERSION_AUTO);
	
	return true;
}

bool si_set_autoversion(SymbolInfo* si)
{
	return si_set_version(si, VERSION_AUTO);
}

bool si_set_data(SymbolInfo* si, const byte* data, int dataCount)
{
	if (!data || dataCount < 0) {
		fprintf(stderr, "Error: no Input data\n");			
		return false;
	}
	
	if (!fits_to_version(si->version, dataCount, si->encodeMode, si->ecLevel)) {
		fprintf(stderr, "Error: data does not fit to new version. Minimal required version: %d\n", get_min_version(dataCount, si->encodeMode, si->ecLevel));
		return false;
	}

	si->inputData = data;
	si->dataCount = dataCount;

	return true;
}

bool si_set_mode(SymbolInfo* si, int mode)
{
	if (mode != ModeNumeric && mode != ModeAlpha && mode != ModeByte) {
		fprintf(stderr, "Error: Invalid encode mode: %d", mode);
		return false;
	}
	
	if (!fits_to_version(si->version, si->dataCount, mode, si->ecLevel)) {
		fprintf(stderr, "Error: data does not fit to version with new encode mode. Minimal required version: %d\n", get_min_version(si->dataCount, mode, si->ecLevel));
		return false;
	}
	
	si->encodeMode = mode;	
	return true;
}

bool si_set_eclevel(SymbolInfo* si, int level)
{
	if (!si || level < 0 || level > 3) {
		fprintf(stderr, "Error: Invalid error correction level.\n");
		return false;
	}
	
	if (!fits_to_version(si->version, si->dataCount, si->encodeMode, level)) {
		fprintf(stderr, "Error: data does not fit to version with new error correction level. Minimal required version: %d\n", get_min_version(si->dataCount, si->encodeMode, level));
		return false;
	}
	
	si->ecLevel = level;

	return true;
}

bool si_set_mask(SymbolInfo* si, int mask)
{
	if (!si || mask < -1 || mask > 7) {
		fprintf(stderr, "Error: invalid mask\n");
		return false;
	}
	
	si->mask = mask;
	si->automask = (mask == MASK_AUTO);
	return true;
}

void si_set_automask(SymbolInfo* si)
{
	si_set_mask(si, MASK_AUTO);
}

bool si_encode(SymbolInfo* si)
{
	/* 1. get autoversion, dataCodewords and blockinfo */
	if (si->autoVersion) si->version = get_min_version(si->dataCount, si->encodeMode, si->ecLevel);
	si->totalCodeWords = get_total_codewords(si->version);
	si->ecCodeWords = get_ec_codewords(si->version, si->ecLevel);
	si->dataCodeWords = get_data_codewords(si->version, si->ecLevel);
	si->blockInfo = get_block_info(si->version, si->ecLevel);
	si->encodedDataCount = getByteCount(si->dataCount, si->encodeMode);
	
	/* 2. encode data */
	bitstream* bs = bs_init();
	if (!bs) {
		fprintf(stderr, "Error: Counldn't initialize bitstream\n");
		return false;
	}	
	bool ret = encodeData(bs, si);
	if (!ret) {
		bs_destroy(bs);
		return false;
	}

	/* 3. split up in datablocks and calculate error correction */
	ret = blockwiseErrorCorrection(si, bs);
	bs_destroy(bs);
	if (!ret) return false;

	/* 4. choose best mask if nescessary */
	//TODO automaticly choose best mask
	if (si->automask) si->mask = 3;

	/* 5. Calculate Format Information */
	// 5 bit formatinfo: 2 bits ecLevel and 3 bits mask indicator
	// add 10 bits of error correction code
	// XOR with 101010000010010, to ensure that no combination of ecLevel and Mask will result in an all-zero data.
	int format = (si->ecLevel << 3) | (si->mask & 7);
	format = format << 10 | bch(15, 5, format, 0x537); // 0x537 = 10100110111 = x^10 + x^8 + x^5 + x^4 + x^2 + x^ + 1
	si->formatInfo = format ^ 0x5412; // 101010000010010

	/* 6. Calculate Version Information */
	// only needed from Version 7 and upwards
	// 6 bit versioninfo, add 12 bits of error correction code
	if (si->version >= 7) {
		si->versionInfo = si->version << 12 | bch(18, 6, si->version, 0x1F25); //  0x1F25 = 1111100100101 = x^12 + x^11 + x^10 + x^9 + x^8 + x^5 + x^2 + 1
	}
	
	return true;
}

int get_min_version(int dataCount, int encMode, int ecLevel) 
{
	dataCount = getByteCount(dataCount, encMode);
	int i;
	if (dataCount < 0 || ecLevel < 0 || ecLevel > 3) return 0;
	for (i = 0; i < 40; i++) {
		if (get_data_codewords(i+1, ecLevel) >= dataCount) return i+1;
	}
	return 0;
}

bool fits_to_version(int version, int dataCount, int encMode, int ecLevel) 
{
	int minVersion = get_min_version(dataCount, encMode, ecLevel);
	if (version) { /* not automatic */
		return version >= minVersion;
	} else { /* automatic */
		return minVersion; /* any version that is not 0 is ok */
	}
}

int get_data_codewords(int version, int ecLevel)
{
	if (version > 0 && version < 41 && ecLevel >= 0 && ecLevel < 4) {
		return get_total_codewords(version) - get_ec_codewords(version, ecLevel);
	}
	return -1;
}

int get_ec_codewords(int version, int ecLevel)
{
	if (version > 0 && version < 41 && ecLevel >= 0 && ecLevel < 4) {
		return si_indicator[(version - 1) * 5 + 1 + ecLevel][0];
	}
	return 0;
}

int get_total_codewords(int version)
{
	if (version > 0 && version < 41) { 
		return si_indicator[(version - 1) * 5][1];
	}
	return 0;
}

BlockInfo get_block_info(int version, int ecLevel)
{
	BlockInfo bi;
	int versionIndex = (version -1) * 5 + ecLevel + 1;
	int j = 1;

	int numFirstBlock = 0;
	int numSecondBlock = 0;
	int totalFirstBlock = 0;
	int totalSecondBlock = 0;
	int dataFirstBlock = 0;
	int dataSecondBlock = 0;


	numFirstBlock = si_indicator[versionIndex][j++];
	totalFirstBlock = si_indicator[versionIndex][j++];
	dataFirstBlock = si_indicator[versionIndex][j++];
	numSecondBlock = si_indicator[versionIndex][j++];
	totalSecondBlock = si_indicator[versionIndex][j++];
	dataSecondBlock = si_indicator[versionIndex][j++];

	bi.numberOfBlocks = numFirstBlock + numSecondBlock;
	bi.block = calloc(numFirstBlock + numSecondBlock, sizeof(Block));

	int k;
	for (k = 0; k < numFirstBlock; k++) {
		bi.block[k].totalCodeWords = totalFirstBlock;
		bi.block[k].dataCodeWords = dataFirstBlock;
		bi.block[k].ecCodeWords = totalFirstBlock - dataFirstBlock;
	}
	for (; k < numFirstBlock + numSecondBlock; k++) {
		bi.block[k].totalCodeWords = totalSecondBlock;
		bi.block[k].dataCodeWords = dataSecondBlock;
		bi.block[k].ecCodeWords = totalSecondBlock - dataSecondBlock;
	}

	return bi;
	
}

bool blockwiseErrorCorrection(SymbolInfo* si, bitstream* bs)
{

	const BlockInfo bi = si->blockInfo;

	byte* finalData = malloc(si->totalCodeWords);
	byte** encData = malloc(bi.numberOfBlocks * sizeof(byte*));
	byte** errorCorrection = malloc(bi.numberOfBlocks * sizeof(byte*));
	
	bool ret = true;
	int position = 0;
	int i;
	for (i = 0; i < bi.numberOfBlocks; i++) {
		int dataCW = bi.block[i].dataCodeWords;
		int ecCW = bi.block[i].ecCodeWords;
		
		encData[i] = malloc(dataCW);
		errorCorrection[i] = malloc(ecCW);

		bs_geta(bs, encData[i], position, dataCW);
		position += dataCW;

		ret = generateErrorCorrectionCode(encData[i], dataCW, errorCorrection[i], ecCW);
		if (!ret) goto Error;
		printf("\nBlock %d\n", i);
		printarray(encData[i], dataCW);
		printarray(errorCorrection[i], ecCW);
	}

	//add data to final string
	int j;
	int index = 0;
	for (i = 0; i < bi.block[bi.numberOfBlocks-1].dataCodeWords; i++) {
		for (j = 0; j < bi.numberOfBlocks; j++) {
			if (bi.block[j].dataCodeWords <= i) continue;
			finalData[index++] = encData[j][i];
		}
	}

	for (i = 0; i < bi.block[bi.numberOfBlocks - 1].ecCodeWords; i++) {
		for (j = 0; j < bi.numberOfBlocks; j++) {
			if (bi.block[j].ecCodeWords <= i) continue;
			finalData[index++] = errorCorrection[j][i];
		}
	}

	if (si->encodedData) free(si->encodedData); 
	si->encodedData = malloc(si->totalCodeWords * sizeof(byte));
	memcpy(si->encodedData, finalData, si->totalCodeWords);


Error: 
	for (i = 0; i < si->blockInfo.numberOfBlocks; i++) {
		free(encData[i]);
		free(errorCorrection[i]);
	}
	free(encData);
	free(errorCorrection);
	free(finalData);
	encData = NULL;
	errorCorrection = NULL;
	finalData = NULL;

	return ret;
}

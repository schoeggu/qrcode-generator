#include "qrgen.h"

#include "ec.h"
#include "ap.h"
#include "symbolinfo.h"
#include "bitstream.h"
#include "dataencoder.h"
#include "painter.h"

#include <stdio.h>
#include <string.h>

#define printarray(A, B) { int q; for (q=0; q < (B); q++) { printf("%d\t", (A)[q]); } printf("\n"); }

/*
 * split up in datablocks and calcualte error corrction
 */
bool blockwiseErrorCorrection(SymbolInfo* si, bitstream* bs);


void qrgen_init()
{
	si_init_codewords();
	initialize_ap();
}

void qrgen_destroy()
{
	si_destroy_codewords();
	destroy_ap();
}

bool qrgen_generate(const byte* data, int dataSize, int version, EncodeModeIndicator mode, ECLevel ecLevel, int mask, cairo_t* ctx, int pixSize)
{
	/* 1. init symbolinfo and check if data is ok */
	SymbolInfo si;
	bool ret = si_init(&si, data, dataSize, mode, ecLevel);
	if (!ret) return false;
	
	ret = si_set_version(&si, version); /* set desired version, if version is 0 the Program chooses itself */
	if (!ret) return false;

	/* 2. encode data */
	bitstream* bs = bs_init();
	if (!bs) {
		fprintf(stderr, "Error: Counldn't initialize bitstream\n");
		return false;
	}	
	ret = encodeData(bs, &si);
	if (!ret) return false;

	/* 3. split up in datablocks and calculate error correction */
	ret = blockwiseErrorCorrection(&si, bs);
	if (!ret) return false;

	/* 4. Mask data */
	//TODO automaticly choose best mask
	//int mask = 3;
	si.mask = mask;

	/* 5. Calculate Format Information */
	// 5 bit formatinfo: 2 bits ecLevel and 3 bits mask indicator
	// add 10 bits of error correction code
	// XOR with 101010000010010, to ensure that no combination of ecLevel and Mask will result in an all-zero data.
	int format = (si.ecLevel << 3) | (mask & 7);
	format = format << 10 | bch(15, 5, format, 0x537); // 0x537 = 10100110111 = x^10 + x^8 + x^5 + x^4 + x^2 + x^ + 1
	si.formatInfo = format ^ 0x5412; // 101010000010010

	/* 6. Calculate Version Information */
	// only needed from Version 7 and upwards
	// 6 bit versioninfo, add 12 bits of error correction code
	if (si.version >= 7) {
		si.versionInfo = si.version << 12 | bch(18, 6, si.version, 0x1F25); //  0x1F25 = 1111100100101 = x^12 + x^11 + x^10 + x^9 + x^8 + x^5 + x^2 + 1
	}

	/* 7. draw QrCode */
	paint_qrcode(ctx, &si, pixSize);

	/*** TMP ***/
	printf("\nfinal\n");
	printarray(si.encodedData, si.totalCodeWords);
	printf("version      %d\n", si.version);
	printf("autoversion  %d\n", si.autoVersion);
	printf("eclevel      %d\n", si.ecLevel);
	printf("dataCount    %d\n", si.dataCount);
	printf("encdCount    %d\n", si.encodedDataCount);
	printf("encmode      %d\n", si.encodeMode);
	printf("totalcw      %d\n", si.totalCodeWords);
	printf("datacw       %d\n", si.dataCodeWords);
	printf("eccw         %d\n", si.ecCodeWords);
	printf("formatinfo   %d\n", si.formatInfo);
	int blocknum = si.blockInfo.numberOfBlocks;
	printf("numblocks    %d\n", blocknum);
	int q;
	for (q = 0; q < blocknum; q++) {
		printf("  block[%d].total   %d\n", q, si.blockInfo.block[q].totalCodeWords);
		printf("  block[%d].ecCW    %d\n", q, si.blockInfo.block[q].ecCodeWords);
		printf("  block[%d].dataCW  %d\n\n", q, si.blockInfo.block[q].dataCodeWords);
	}
	/***  TMP  ***/

	/* 8. free memory */
	bs_destroy(bs);
	bs = NULL;

	si_reset(&si);

	return true;
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

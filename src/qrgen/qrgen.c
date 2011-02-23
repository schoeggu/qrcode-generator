#include "qrgen.h"

#include "symbolinfo.h"
#include "painter.h"

#include <stdio.h>
#include <string.h>

bool qrgen_generate(const byte* data, int dataSize, int version, EncodeModeIndicator mode, ECLevel ecLevel, int mask, cairo_t* ctx, int pixSize)
{
	SymbolInfo* si = si_create_conf(data, dataSize, version, mode, ecLevel, mask);
	if (!si) return false;
	if (si_encode(si)) {
		/* 7. draw QrCode */
		paint_qrcode(ctx, si, pixSize);
	

		/*** TMP ***/
		printf("\nfinal\n");
		printarray(si->encodedData, si->totalCodeWords);
		printf("version      %d\n", si->version);
		printf("autoversion  %d\n", si->autoVersion);
		printf("eclevel      %d\n", si->ecLevel);
		printf("dataCount    %d\n", si->dataCount);
		printf("encdCount    %d\n", si->encodedDataCount);
		printf("encmode      %d\n", si->encodeMode);
		printf("totalcw      %d\n", si->totalCodeWords);
		printf("datacw       %d\n", si->dataCodeWords);
		printf("eccw         %d\n", si->ecCodeWords);
		printf("formatinfo   %d\n", si->formatInfo);
		int blocknum = si->blockInfo.numberOfBlocks;
		printf("numblocks    %d\n", blocknum);
		int q;
		for (q = 0; q < blocknum; q++) {
			printf("  block[%d].total   %d\n", q, si->blockInfo.block[q].totalCodeWords);
			printf("  block[%d].ecCW    %d\n", q, si->blockInfo.block[q].ecCodeWords);
			printf("  block[%d].dataCW  %d\n\n", q, si->blockInfo.block[q].dataCodeWords);
		}
		/***  TMP  ***/
	}
	
	/* 8. free memory */

	si_destroy(si);

	return true;
}

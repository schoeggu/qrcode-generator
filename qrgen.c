#include "qrgen.h"

#include "ec.h"
#include "gp.h"
#include "symbolinfo.h"
#include "bitstream.h"
#include "dataencoder.h"

#include <stdio.h>

#define printarray(A, B) { int q; for (q=0; q < (B); q++) { printf("%d\t", (A)[q]); } printf("\n"); }

void qrgen_init()
{
	calculate_galois_field(PP);
	initialize_gp();
	si_init_codewords();
}

void qrgen_destroy()
{
	destroy_gp();
	si_destroy_codewords();
}

bool qrgen_generate(const byte* data, int dataSize, EncodeModeIndicator mode, ECLevel ecLevel, cairo_surface_t* surface, int pixSize)
{
	// Version 0: Programm chooses smallest possible Version
	return qrgen_generate_force_version(data, dataSize, 0, mode, ecLevel, surface, pixSize);
}

bool qrgen_generate_force_version(const byte* data, int dataSize, int version, EncodeModeIndicator mode, ECLevel ecLevel, cairo_surface_t* surface, int pixSize)
{
	/* 1. init symbolinfo and check if data is ok */
	SymbolInfo si;
	bool ret = si_init(&si, data, dataSize, mode, ecLevel);
	if (!ret) return false;

	ret = si_set_version(&si, version);
	if (!ret) return false;

	/* 2. encode data */
	//TODO blocks
	bitstream* bs = bs_init();
	if (!bs) {
		fprintf(stderr, "Error: Counldn't initialize bitstream\n");
		return false;
	}	
	ret = encodeData(bs, &si);
	if (!ret) return false;

	/* 3. calculate error correction */
	//TODO blocks
	byte* encData = malloc(si.dataCodeWords);
	byte* errorCorrection = malloc(si.ecCodeWords);

	bs_geta(bs, encData, 0, si.dataCodeWords);

	ret = generateErrorCorrectionCode(encData, si.dataCodeWords, errorCorrection, si.ecCodeWords);
	if (!ret) return false;

	bs_add_bs(bs, errorCorrection, si.ecCodeWords, si.ecCodeWords * 8);

	/* 4. Mask data */
	int mask = 0;

	/* 5. Calculate Format Information */

	// 5 bit formatinfo: 2 bits ecLevel and 3 bits mask indicator
	// add 10 bits of error correction code
	// XOR with 0x5412, to ensure that no combination of ecLevel and Mask will result in an all-zero data.
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


	/***
	 * *  TMP
	 * */
	bs_print_full(bs);
	byte* c = malloc(si.totalCodeWords);
	bs_geta(bs, c, 0, si.totalCodeWords);
	printarray(c, si.totalCodeWords);
	free(c);
	c = NULL;
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
	printf("versioninfo  %d\n", si.versionInfo);
	/***
	 * *  TMP
	 * */

	/* 8. free memory */
	free(encData);
	free(errorCorrection);
	bs_destroy(bs);
	encData = NULL;
	errorCorrection = NULL;
	bs = NULL;

	return true;
}

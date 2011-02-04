#include "ec.h"
#include "gp.h"
#include "bitstream.h"
#include "dataencoder.h"
#include "symbolinfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define printarray(A, B) { int q; for (q=0; q < (B); q++) { printf("%d\t", (A)[q]); } printf("\n"); }


int main(int argc, char** argv)
{


//	int i;
//	for (i = 0; i < 40; i++) {
//		printf("%d: %d, %d, %d\n", i+1, cw[i].totalCodeWords, cw[i].ecCodeWords[0], cw[i].dataCodeWords[0]);
//	}
//	int i = 0;
//	int lastversion = -1;
//	for(; i < 4000; i++) {
//		int version = get_min_version(i, EC_L);
//		if (version && version != lastversion) {
//			lastversion = version;
//			int total = get_total_codewords(version);
//			int ec = get_ec_codewords(version, EC_L);
//			int data = get_data_codewords(version, EC_L);
//			printf("version %d: total[%d], data[%d], ec[%d]\n", version, total, data, ec);
//		}
//	}
//	printf("\n");
//
//	return 0;



	if (argc < 4) { printf("galois [a|n] [l|m|q|h] ...\n"); return 0; }

	EncodeModeIndicator mode = ((argv[1][0] == 'a') ? ModeAlpha : ((argv[1][0] == 'n') ? ModeNumeric : 1));
	char ec = argv[2][0];
	int ecLevel;
	switch(ec) {
	case 'l':
	case 'L':
		ecLevel = EC_L;
		break;
	case 'm':
	case 'M':
		ecLevel = EC_M;
		break;
	case 'q':
	case 'Q':
		ecLevel = EC_Q;
		break;
	case 'h':
	case 'H':
		ecLevel = EC_H;
		break;
	default:
		return 0;
	}	
	if (mode == 0) return 0;

	SymbolInfo si;

	if (!si_init(&si, (byte*)argv[3], strlen(argv[3]), mode, ecLevel)) {
		printf("init failed"); return 0;
	}

	printf("version      %d\n", si.version);
	printf("autoversion  %d\n", si.autoVersion);
	printf("eclevel      %d\n", si.ecLevel);
	printf("dataCount    %d\n", si.dataCount);
	printf("encdCount    %d\n", si.encodedDataCount);
	printf("encmode      %d\n", si.encodeMode);
	printf("totalcw      %d\n", si.totalCodeWords);
	printf("datacw       %d\n", si.dataCodeWords);
	printf("eccw         %d\n", si.ecCodeWords);

	bitstream* bs = bs_init();

	encodeData(bs, &si);

	

	bs_print_full(bs);


	byte c[256];
	bs_geta(bs, c, 0, si.dataCodeWords);
	printarray(c, si.dataCodeWords);

	initialize_gp();

	printf("dataSize: %d\n", si.dataCodeWords);


	byte* errorCorrection = malloc(si.ecCodeWords);

	if (generateErrorCorrectionCode(c, si.dataCodeWords, errorCorrection, si.ecCodeWords)) {
		printarray(errorCorrection, si.ecCodeWords);
	} else {
		printf ("Error\n");
	}

	bs_add_bs(bs, errorCorrection, si.ecCodeWords, si.ecCodeWords*8);
	bs_print_full(bs);

	bs_geta(bs, c, 0, si.totalCodeWords);
	printarray(c, si.totalCodeWords);

	destroy_gp();
	free(errorCorrection);

	bs_destroy(bs);
	bs = NULL;

	si_destroy_codewords();

	return 0;
}

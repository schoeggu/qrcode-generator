#include "ec.h"
#include "gp.h"
#include "bitstream.h"
#include "dataencoder.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define printarray(A, B) { int q; for (q=0; q < (B); q++) { printf("%d\t", (A)[q]); } printf("\n"); }

int main(int argc, char** argv)
{

	int mode = ((argv[1][0] == 'a') ? ModeAlpha : ((argv[1][0] == 'n') ? ModeNumeric : 1));
	int ecBlocks = atoi(argv[2]); 
	if (mode == 0) return 0;

	bitstream* bs = bs_init();


	encodeData(bs, (unsigned char*)argv[3], mode, strlen(argv[3]));
	
//	byte b[] = {0x3, 0x3, 0x3 };
//	byte b[] = {"ABCDE123"};
//	byte b[] = {"01234567"};

//	encodeData(bs, b, ModeNumeric, 8);
//	encodeData(bs, b, ModeByte, 3);
//	encodeData(bs, b, ModeAlpha, 8);

//	bs_add_b(bs, 236, 8);
	bs_print_full(bs);

	int dataSize = bs->size/8;

	byte c[256];
	bs_geta(bs, c, 0, dataSize);
	printarray(c, dataSize);

	if (!ecBlocks) return 0;

	initialize_gp();



	printf("dataSize: %d\n", dataSize);


	byte* errorCorrection = malloc(ecBlocks);

	if (generateErrorCorrectionCode(c, dataSize, errorCorrection, ecBlocks)) {
		printarray(errorCorrection, ecBlocks);
	} else {
		printf ("Error\n");
	}

	bs_add_bs(bs, errorCorrection, ecBlocks, ecBlocks*8);
	bs_print_full(bs);

	bs_geta(bs, c, 0, dataSize+ecBlocks);
	printarray(c, dataSize+ecBlocks);

	destroy_gp();
	free(errorCorrection);

	bs_destroy(bs);
	bs = NULL;

	return 0;
}


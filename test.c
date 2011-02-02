#include "ec.h"
#include "gp.h"
#include "bitstream.h"

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char** argv)
{

	bitstream* bs = bs_init();

	bs_print_full(bs);
	bs_add_b(bs, 2, 4);
	bs_print_full(bs);
	bs_add_b(bs, 8, 9);
	bs_print_full(bs);
	bs_add_i(bs, 461, 11);
	bs_print_full(bs);
	bs_add_i(bs, 553, 11);
	bs_print_full(bs);
	bs_add_i(bs, 631, 11);
	bs_print_full(bs);
	bs_add_i(bs, 93, 11);
	bs_print_full(bs);
	bs_add_i(bs, 0, 4);
	bs_print_full(bs);
	bs_add_i(bs, 0, 3);
	bs_print_full(bs);
	bs_add_i(bs, 55, 8);
	bs_print_full(bs);

	int p;
	for (p = 0; p*8 < bs->size; p++) {
		printf("%d ", bs_get(bs, p));
	}



	bs_destroy(bs);
	bs = NULL;


	return 0;

////////////////
	if (argc < 2) return 0;

	initialize_gp();

	size_t dataSize = argc - 2;
	size_t ecBlocks = atoi(argv[1]);

	byte* code = malloc(dataSize);

	int i;
	for (i = 0; i < argc - 2; i++) {
		code[i] = atoi(argv[i+2]);
		printf("%d\t", code[i]);
	}

	printf("\n");

	byte* errorCorrection = malloc(ecBlocks);

	if (generateErrorCorrectionCode(code, dataSize, errorCorrection, ecBlocks)) {
		int k;
		for (k = 0; k < ecBlocks; k++) { printf("%d\t", errorCorrection[k]); } printf("\n");
	} else {
		printf ("Error\n");
	}


	destroy_gp();
	free(errorCorrection);
	free(code);

	return 0;
}


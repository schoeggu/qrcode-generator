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
	bs_add_i(bs, 8, 9);
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
	bs_add_i(bs, 236, 8);
	bs_print_full(bs);

	byte b[9];
	bs_geta(bs, b, 0, 9);
	int p;
	for (p = 0; p < 9; p++) {
		printf("%d ", b[p]);
	}


	initialize_gp();

	size_t dataSize = 9;
	size_t ecBlocks = 17;

	byte* errorCorrection = malloc(ecBlocks);

	if (generateErrorCorrectionCode(b, dataSize, errorCorrection, ecBlocks)) {
		int k;
		for (k = 0; k < ecBlocks; k++) { printf("%d\t", errorCorrection[k]); } printf("\n");
	} else {
		printf ("Error\n");
	}

	bs_add_bs(bs, errorCorrection, 17, 17*8);
	bs_print_full(bs);

	byte c[26];
	bs_geta(bs, c, 0, 26);
	for (p = 0; p < 26; p++) {
		printf("%d ", c[p]);
	}

	destroy_gp();
	free(errorCorrection);

	bs_destroy(bs);
	bs = NULL;

	return 0;
}


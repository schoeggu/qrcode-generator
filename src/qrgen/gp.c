#include "gp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

byte** gp = 0;
static bool gp_initialized = false;

void initialize_gp()
{
	FILE* file = fopen("./rc/gp.rc", "r");
	if (!file) {
		fprintf(stderr, "Error: File not found './rc/gp.rc");
		exit(1);
	}

	gp = calloc(68, sizeof(byte*));

	char buffer[256];
	char delim[] = " ";
	char* number;
	while (fgets(buffer, 256, file) != NULL) {
		/* first number of line is the number of ErrorCorrectionBlocks for the Polynomial */
		number = strtok(buffer, delim);
		int rNum = atoi(number);
		int num = 0;
 
		/* after that, the alpha^n values are listed */
		gp[rNum-1] = malloc(rNum+1);
		while ((number = strtok(NULL, delim)) != NULL ) {
			gp[rNum-1][num] = (byte)atoi(number);
			num++;

		}

		if (rNum != num - 1) fprintf(stderr, "Error: in gp.rc: %d has %d items\n", rNum, num);

	}

	fclose(file);
	gp_initialized = true;	
}	

void destroy_gp()
{
	if (gp_initialized && gp) {
		int i;
		for (i = 0; i < 68; i++) {
			free(gp[i]);
			gp[i] = NULL;
		}
//		free(gp);
		gp = NULL;
	}
	gp_initialized = false;	
}

const byte* get_gp(unsigned int numBlocks)
{
	if (!gp_initialized) {
		initialize_gp();
	}
	numBlocks--; //array index starts with 0, numBlocks with 1
	if (numBlocks < 68) return gp[numBlocks];
	return NULL;
}

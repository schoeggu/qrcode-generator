#include "gp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

byte** gp;
static int gp_initialized = 0;

void initialize_gp()
{
	FILE* file = fopen("./gp.rc", "r");
	if (!file) {
		exit(1);
	}

	gp = calloc(68, sizeof(byte*));

	char buffer[256];
	char delim[] = " ";
	char* number;
	while (fgets(buffer, 256, file) != NULL) {
		number = strtok(buffer, delim);
		int rNum = atoi(number);
		int num = 0;
 
		gp[rNum-1] = malloc(rNum+1);
		while ((number = strtok(NULL, delim)) != NULL ) {
			gp[rNum-1][num] = (byte)atoi(number);
			num++;

		}

		if (rNum != num - 1) printf("Error in gp.rc: %d has %d items\n", rNum, num);

	}

	fclose(file);
	gp_initialized = 1;	
}	

void destroy_gp()
{
	if (gp) {
		int i;
		for (i = 0; i < 68; i++) {
			free(gp[i]);
			gp[i] = NULL;
		}
		free(gp);
		gp = NULL;
	}
	gp_initialized = 0;	
}

const byte* get_gp(unsigned int numBlocks)
{
	if (!gp_initialized) {
		initialize_gp();
	}
	numBlocks--;
	if (numBlocks < 68) return gp[numBlocks];
	return NULL;
}

#include "ap.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

AlignementPatternPosition* ap;
static bool ap_initialized = false;

void initialize_ap()
{
	FILE* file = fopen("./ap.rc", "r");
	if (!file) {
		exit(1);
	}

	int i = 0;
	int j = 0;

	ap = calloc(40, sizeof(AlignementPatternPosition));

	char buffer[256];
	const char delim[] = " ";
	char* number;
	int numPosition;
	int version = 0;
	int numberAp;
	while (fgets(buffer, 256, file) != NULL) {
		int apPosition[7];
		numPosition = 0;
		version = 0;
		j = i = 0;

		printf("line: %s\n", buffer);
		/* first number is version */
		number = strtok(buffer, delim);
		printf("version[%s] ", number);
		fflush(stdout);
		version = atoi(number);

		/* number of AlignementPatterns */
		number = strtok(NULL, delim);
		printf("numAp[%s] ", number);
		fflush(stdout);
		numberAp = atoi(number);

		while ((number = strtok(NULL, delim)) != NULL) {
			printf("number[%s] ", number);
			fflush(stdout);
			apPosition[i++] = atoi(number);
			numPosition++;

		}
		printf("\n");



		ap[version-1].version = version; 
		version--; /* decrement version because array index starts with 0 */
		ap[version].numberOfPatterns = numberAp;

		printf("number indexes[%d]\n", numPosition);
		ap[version].position = calloc(numberAp, sizeof(point));

		int x;
		int y;
		i = 0;
		printf("numAp[%d]\n", numberAp);
		for (x = 0; x < numPosition; x++) {
			for (y = 0; y < numPosition; y++) {
				if ((x == 0 && (y == numPosition - 1 || y == 0)) || (x == numPosition -1 && y == 0))  continue;
				
				printf("pos[%d, %d] j[%d]", apPosition[x], apPosition[y], j);
				fflush(stdout);
				ap[version].position[j].x = apPosition[x];
				ap[version].position[j].y = apPosition[y];
				j++;
			}
		}
		printf("\n");

	}

	fclose(file);
	ap_initialized = true;

}

void destroy_ap()
{
	int i;
	if (ap) {
		for (i = 0; i < 40; i++) {
			free(ap[i].position);
		}
		free(ap);
		ap = NULL;
	}
	ap_initialized = false;
}

const AlignementPatternPosition* getAp(int version)
{
	if (!ap_initialized) {
		initialize_ap();
	}

	if (version >= 1 && version <= 40) return &ap[version-1];
	return NULL;

}



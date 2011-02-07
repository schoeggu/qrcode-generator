#include "ec.h"
#include "gp.h"
#include "bitstream.h"
#include "dataencoder.h"
#include "symbolinfo.h"
#include "qrgen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char** argv)
{


	if (argc < 4) { printf("galois [a|n] [l|m|q|h] ...\n"); return 0; }

	EncodeModeIndicator mode;

	switch(argv[1][0]) {
	case 'a':
		mode = ModeAlpha;
	break;
	case 'n':
		mode = ModeNumeric;
	break;
	case 'b':
		mode = ModeByte;
	break;
	default: return 0;
	}
   
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

	qrgen_init();

	bool ret;
	if (argc == 5) {
		int version = atoi(argv[3]);
		ret+= qrgen_generate_force_version((byte*)argv[4], strlen(argv[4]), version, mode, ecLevel, NULL, 300);
	} else {
		ret = qrgen_generate((byte*)argv[3], strlen(argv[3]), mode, ecLevel, NULL, 300);
	}



	if (ret) {
		printf("Successful\n");
	} else {
		printf("Not Successful\n");
	}

	qrgen_destroy();

	return 0;
}

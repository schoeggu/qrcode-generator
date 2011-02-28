#include "symbolinfo.h"
#include "paintcontext.h"
#include "painter.h"
//#include "window.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>


void printHelp(char* progname);


int main(int argc, char** argv)
{
	EncodeModeIndicator mode = ModeByte;
	int ecLevel = EC_H;
	int version = 0;
	bool displayWindow = false;
	bool writeToFile = false;
	char* filename = NULL;
	int size = 200;
	bool modulsize = false;
	int displaySize = 200;
	int len;
	int quietzone = 0;
	bool nomask = false;
	bool nodata = false;
	bool optimalsize = false;
	int mask = MASK_AUTO;
	
	struct option options[] = { 
		{"mode", 1, NULL, 'm'},
		{"eclevel", 1, NULL, 'e'},
		{"symbol-version", 1, NULL, 'v'},
		{"window", 0, NULL, 'w'},
		{"file", 1, NULL, 'f'},
		{"size", 1, NULL, 's'},
		{"modul-size", 1, NULL, 'n'},
		{"mask", 1, NULL, 'z'},
		{"no-mask", 0, NULL, 'a'},
		{"no-data", 0, NULL, 'd'},
		{"quiet-zone", 2, NULL, 'q'},
		{"optimal-size", 2, NULL, 'o'},
		{"help", 0, NULL, 'h'},
		{"version", 0, NULL, 'x'},
		{0, 0, 0, 0}
	};
	
	int opt;
	int idx;
    while ((opt = getopt_long(argc, argv, "m:e:v:wq::f:s:on:hx", options, &idx)) != -1) {
        switch (opt) {
		
        case 'm':
            switch(optarg[0]) {
			case 'a':
				mode = ModeAlpha;
			break;
			case 'n':
				mode = ModeNumeric;
			break;
			case 'b':
				mode = ModeByte;
			break;
			default: 
				fprintf(stderr, "Error: Invalid encode mode '%c'.\n", optarg[0]);
				printHelp(argv[0]);
				return 0;
			}
		break;
		
        case 'e':
            switch(optarg[0]) {
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
				fprintf(stderr, "Error: Invalid error corretion level '%c'.\n", optarg[0]);
				printHelp(argv[0]);
				return 0;
			}	
        break;
		
        case 'v':
			version = atoi(optarg);
			if (version < 0 || version > 40) {
				fprintf(stderr, "Error: Invalid version '%s'.\n", optarg);
				printHelp(argv[0]);
				return 0;
			}
        break;
		
		case 'a':
			nomask = true;
		break;
		
		case 'd':
			nodata = true;
		break;
		
		case 'o':
			optimalsize = true;
		break;
		
        case 'w':
            displayWindow = true;
        break;
		
		case 'f':
			writeToFile = true;
			len = strlen(optarg);
			if (!len) {
				fprintf(stderr, "Error: No file specified\n");
				printHelp(argv[0]);
				return 0;
			}
			
			filename = calloc(len, sizeof(char));
			strcpy(filename, optarg);
			
		break;
		
		case 'n':
			modulsize = true;
		case 's':
			size = atoi(optarg);
			displaySize = size;
			if (size < 0) {
				fprintf(stderr, "Error: invalid size '%s'", optarg);
				return 0;
			}
		break;
		
		case 'q':
			quietzone = 4;
			if (optarg) {
				quietzone = atoi(optarg);
			}
		break;
		
		case 'z':
			mask = atoi(optarg);
			if (mask < 0 || mask > 7) {
				fprintf(stderr, "Error: invalid mask '%s'", optarg);
				return 0;
			}
		break;
		
        case 'h':
            printHelp(argv[0]);
            return 0;

		case 'x':
			printf("%s 0.1\n", argv[0]);
			return 0;
			
        default:
            fprintf(stderr, "Error: Unknown option: %c", (char)opt);
            printHelp(argv[0]);
            return 0;
        }
    }
	
	if (!writeToFile) {
		displayWindow = true;
	}
	
	if (displayWindow) {
		size = 3000; // large size, so it keeps sharp when resizing the window
	}
	
	char* data;
	int dataLen = 0;
	int i;
	
	for (i = optind; i < argc; i++) {
		dataLen += strlen(argv[i]) + 1; // +1 for the spaces
	}
	
	if (dataLen == 0) {
		fprintf(stderr, "Error: No data specified\n");
		printHelp(argv[0]);
		return 0;
	}
	
	data = calloc(dataLen, sizeof(char));
	for (i = optind; i < argc; i++) {
		strcat(data, argv[i]);
		strcat(data, " ");
	}
	dataLen--; //last space doesn't count
	
	data[strlen(data)-1] = '\0'; // remove last ' '
	
	SymbolInfo* si = si_create_conf((byte*)data, dataLen, version, mode, ecLevel, mask);
	if (!si) return 0;
	if (!si_encode(si)) {
		si_destroy(si);
		fprintf(stderr, "Error: did not encode\n");
		return 0;
	}
	
	PaintContext* pc = pc_create_for_file(filename);
	pc_set_size(pc, size, modulsize);
//	if (optimalsize) pc_calculate_optimal_size(pc, true);
	if (quietzone) pc_set_quiet_zone_size(pc, quietzone);
	
//	pc_set_foreground_color(pc, WHITE);
//	pc_set_background_color(pc, BLACK);
	
	pc_set_dont_mask(pc, nomask);
	pc_set_draw_no_data(pc, nodata);

	pc_set_highlighted_zones(pc, QR_ALL);
	
	paint_qrcode(si, pc);
	
	si_destroy(si);

	free(data);
	free(filename);
	data = NULL;
	filename = NULL;

	return 0;
}

void printHelp(char* progname) 
{
	printf("%s 0.1\nusage: %s [-m mode] [-e errorCorrectionLevel] [-v version] [-w] [-f file] [-s size | -n size] [-q size] [-o] [--mask mask] [--no-mask] [--no-data] [-h] [--] data ...\n"
			
			"\n"
			"-m, --mode             Encoding Mode. Use 'a' for alphanumeric, 'n' for numeric or 'b' for binary encodation.\n"
			"                       Default: binary\n\n"
			"-e, --eclevel          Error Correction Level. Use 'l' for low, 'm' for medium, 'q' for more or 'h' for high.\n"
			"                       Default: high\n\n"
			"-v, --symbol-version   Version. The specified version must be between and including 1 and 40.\n"
			"                       If the version is 0, the programm chooses the smallest possible version itself.\n"
			"                       Default: 0\n\n"
			"-w, --window           Displays the qrcode in a window.\n\n"
			"-f, --file             Save the qrcode to the specified file.\n"
			"                       If -f is not set, the programm displays the qrcode in a window.\n"
			"                       NOTE: Only png and svg are supported.\n\n"
			"-s, --size             Qrcode Size in Pixels.\n"
			"                       Default 200.\n\n"
			"-m, --modul-size       The size is interpreted as the size of one bit module.\n\n"
			"-o, --optimal-size     Calculate the size, that each module has an integral number of bits.\n\n"
			"-q, --quiet-zone       Draw a quietzone around the Qrcodes with a size of <size> modules.\n"
			"                       Default size 4.\n\n"
			"--mask                 Use this mask.\n"
			"                       Default auto.\n\n"
			"--no-data              Don't encode any data. data and error correction codewords are set to 0.\n\n"
			"--no-mask              Don't mask the data.\n\n"
			"-h, --help             Print this help.\n\n"
			"--version              Print version number.\n\n"
			"--                     Ignore all options after this.\n\n"
			"data                   The data to encode.\n", progname, progname);
}

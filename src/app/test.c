#include "ec.h"
#include "gp.h"
#include "bitstream.h"
#include "dataencoder.h"
#include "symbolinfo.h"
#include "qrgen.h"
#include "window.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cairo.h>
#include <cairo-svg.h> 


void printHelp(char* progname);


int main(int argc, char** argv)
{
	EncodeModeIndicator mode = ModeByte;
	int ecLevel = EC_H;
	int version = 0;
	bool displayWindow = false;
	bool writeToFile = false;
	char* filename = NULL;
	char filetype[4];
	int size = 200;
	int displaySize = 200;
	int len;
	
	int opt;
    while ((opt = getopt(argc, argv, "m:e:v:wf:s:h")) != -1) {
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
			
			strcpy(filetype, (filename+len-3));
			printf("%s\n", filetype);
			
			if (strcmp(filetype, "svg") && strcmp(filetype, "png")) {
				fprintf(stderr, "Error: Filetype unsupported '%s'", filetype);
				return 0;
			}
			
		break;
		
		case 's':
			size = atoi(optarg);
			displaySize = size;
			if (size < 0) {
				fprintf(stderr, "Error: invalid size '%s'", optarg);
				return 0;
			}
		break;
		
        case 'h':
            printHelp(argv[0]);
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
	
	cairo_surface_t* surface;
	cairo_rectangle_t rect = {0, 0, size, size};

	
	surface = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, &rect);
	
	qrgen_init();
	
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
	
	
	printf("data: [%s], len[%d]\n", data, dataLen);
	
	bool ret;
	if (version) {
		ret = qrgen_generate_force_version((byte*)data, dataLen, version, mode, ecLevel, surface, size);
	} else {
		ret = qrgen_generate((byte*)data, dataLen, mode, ecLevel, surface, size);
	}
	
	qrgen_destroy();
	
	if (ret) {
		if (writeToFile) {
			cairo_surface_t* sur;
			if (!strcmp(filetype, "png")) {
				sur = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, displaySize, displaySize);
			} else if (!strcmp(filetype, "svg")) {
				sur = cairo_svg_surface_create(filename, displaySize, displaySize);
			} else { return 0; }
			
			cairo_t* cr = cairo_create(sur);
			cairo_scale(cr, ((float)displaySize)/size, ((float)displaySize)/size);
			cairo_set_source_surface(cr, surface, 0, 0);
			cairo_paint(cr);
			
			if (!strcmp(filetype, "png")) {
				cairo_surface_write_to_png(sur, filename);
			}
			cairo_destroy(cr);
			cairo_surface_destroy(sur);
		}
		
		if (displayWindow) {
			displayInWindow(surface, size, displaySize, displaySize, argc, argv);
		}
	}
		
	cairo_surface_destroy(surface);

	free(data);
	free(filename);
	data = NULL;
	filename = NULL;

	return 0;
}

void printHelp(char* progname) 
{
	printf("usage: %s [-m mode] [-e errorCorrectionLevel] [-v version] [-w] [-f file] [-s size] [-h] data ...\n"
			
			"\n"
			"[-m mode]         Encoding Mode. Use 'a' for alphanumeric, 'n' for numeric or 'b' for binary encodation.\n"
			"                  Default: binary\n\n"
			"[-e ecLevel]      Error Correction Level. Use 'l' for low, 'm' for medium, 'q' for more or 'h' for high.\n"
			"                  Default: high\n\n"
			"[-v version]      Version. The specified version must be between and including 1 and 40.\n"
			"                  If the version is 0, the programm chooses the smallest possible version itself.\n"
			"                  Default: 0\n\n"
			"[-w]              Displays the qrcode in a window.\n\n"
			"[-f file]         Save the qrcode to the specified file.\n"
			"                  If -f is not set, the programm displays the qrcode in a window.\n"
			"                  NOTE: Only png and svg are supported.\n\n"
			"[-s size]         Qrcode Size in Pixels.\n"
			"                  Default 200.\n\n"
			"[-h]              Print this help.\n\n"
			"data              The data to encode.\n", progname);
}

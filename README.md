qrgen 0.1
=========

qrgen is a library that generates qrcodes.
It comes with a sample program to demonstrate how to use the library.

Requirements
------------
- gnu tool chain
- libCairo >= 1.10.0 (probably works also with older releases, but i didn't test it)

***Optional***

- gtk+-2.0 >= 2.20.1
- python   >= 2.7
- pygtk    >= 2.20
- pycairo  >= 1.8.10

Usage (Sample Program)
----------------------

	usage: qrgen.exe [-m mode] [-e errorCorrectionLevel] [-v version] [-w] [-f file] [-s size | -n size] [-q size] [-o] [--mask mask] [--no-mask] [--no-data] [-h] [--] data ...

	-m, --mode             Encoding Mode. Use 'a' for alphanumeric, 'n' for numeric or 'b' for binary encodation.
						   Default: binary

	-e, --eclevel          Error Correction Level. Use 'l' for low, 'm' for medium, 'q' for more or 'h' for high.
						   Default: high

	-v, --symbol-version   Version. The specified version must be between and including 1 and 40.
						   If the version is 0, the programm chooses the smallest possible version itself.
						   Default: 0

	-w, --window           Displays the qrcode in a window.

	-f, --file             Save the qrcode to the specified file.
						   If -f is not set, the programm displays the qrcode in a window.
						   NOTE: Only png and svg are supported.

	-s, --size             Qrcode Size in Pixels.
						   Default 200.

	-m, --modul-size       The size is interpreted as the size of one bit module.

	-o, --optimal-size     Calculate the size, that each module has an integral number of bits.

	-q, --quiet-zone       Draw a quietzone around the Qrcodes with a size of <size> modules.
						   Default size 4.

	--mask                 Use this mask.
						   Default auto.

	--no-data              Don't encode any data. data and error correction codewords are set to 0.

	--no-mask              Don't mask the data.

	-h, --help             Print this help.

	--version              Print version number.

	--                     Ignore all options after this.

	data                   The data to encode.

	
***For Example***

	$ qrgen -m b -e h -f ./filename.svg -s 111 https://github.com/schoeggu/qrcode-generator

Will create this

![generated QR Code](http://img11.imageshack.us/img11/1316/qrgen1.png)
	
Usage (Library)
---------------

***Requirded Includes***

	#include "qrgen.h"

***SymbolInfo***
`SymbolInfo* si_create(byte* data, int dataLen)` 
Return a pointer to a newly allocated SymbolInfo object. It is initialized with default values.

`SymbolInfo* si_create_conf(const byte* data, int dataCount, int version, int mode, int level, int mask)` 
Return a pointer to a newly allocated SymbolInfo object.

`void si_destroy(SymbolInfo* si)` 
Free the allocated memory for the SymbolInfo.


`bool si_set_version(SymbolInfo* si, int version)` 
Set the desired symbol version. version must be between 1 - 40, or may be `VERSION_AUTO`.

`bool si_set_auto_version(SymbolInfo* si)` 
Make the library choose the smallest possible version itself.

`bool si_set_data(SymbolInfo* si, const byte* data, int dataCount)` 
Set the data to encode.

`bool si_set_mode(SymbolInfo* si, int mode)` 
Set the encodation mode. Can be either `ModeByte` or `ModeAlpha` or `ModeNumeric`.

`bool si_set_eclevel(SymbolInfo* si, int level)` 
Set the error correction level. Can be either `EC_L` or `EC_M` or `EC_Q` or `EC_H`.

`bool si_set_mask(SymbolInfo* si, int mask)` 
Set the mask to use. Must be between 0 and 7, or may be `MASK_AUTO`

`void si_set_auto_mask(SymbolInfo* si)` 
Make the library choose the optimal mask.


`bool si_encode(SymbolInfo* si)` 
Encode the data. Must be done before painting.

***PaintContext***
`PaintContext* pc_create(cairo_t* cr)` 
Return a pointer to a newly allocated PaintContext. The symbol will be painted to the specified cairo surface.

`PaintContext* pc_create_for_file(char* filename)` 
Return a pointer to a newly allocated PaintContext. The symbol will be written to the specified file.

`void pc_destroy(PaintContext* pc)` 
Free teh allocated memory for the PaintContext.



`bool pc_set_cairo_context(PaintContext* pc, cairo_t* cr)` 
Set another cairo context.

`bool pc_set_filename(PaintContext* pc, char* filename)` 
Set another filename.

`bool pc_set_position(PaintContext* pc, double x, double y)` 
Set where on the cairo context the qrcode will be painted.

`bool pc_set_size(PaintContext* pc, int size, bool isBitSize)` 
Set the size of the symbol. if `isbitSize` is true, `size` specifiec the size of one bit module. Otherwise it's the size of the whole symbol.

`bool pc_set_is_bit_size(PaintContext* pc, bool isBitSize)` 
Set whether the size is the bit module size or the symbol size.

`bool pc_draw_quiet_zone(PaintContext* pc, bool drawQuietZone)` 
Set wheter a quiet zone around the symbol shall be painted.

`bool pc_set_quiet_zone_size(PaintContext* pc, int size)` 
Set the size of the quiet zone (in bit modules)

`bool pc_calculate_optimal_size(PaintContext* pc, bool calculateOptimalSize)` 
Make the library calculate the size, so that each bit module has an integral number of pixels.

`bool pc_set_foreground_color(PaintContext* pc, color c)` 
Change the foreground color.

`bool pc_set_background_color(PaintContext* pc, color c)` 
Change the background color.

***Paint***
`bool paint_qrcode(const SymbolInfo* si, PaintContext* pc)` 
Paint the symbol.


***Example***
Print a QR Code to a file

	#include "qrgen.h"
	#include <string.h> /* for strlen */

	int main(int argc, char** argv)
	{
		char message[] = "Hello Old Friend!";
		char filename[] = "welcome.png";
		
		/* Setup the SymbolInfo */
		SymbolInfo* si = si_create((byte*)message, strlen(message));
		if (!si) return 1;
		
		si_set_mode(si, ModeAlpha);
		si_set_eclevel(si, EC_Q);
		bool ok = si_encode(si);
		if (!ok) {
			si_destroy(si);
			return 1;
		}
		
		/* Setup the PaintContext */
		PaintContext* pc = pc_create_for_file(filename);
		if (!pc) {
			si_destroy(si);
			return 1;
		}
		
		pc_set_size(pc, 105, false);
			
		/* write to the file */
		paint_qrcode(si, pc);
		
		/* free allocated memory */
		si_destroy(si);
		pc_destroy(pc);
		
		return 0;
	}

	
Will generate this:

![generated QR Code](http://img815.imageshack.us/img815/5826/welcomei.png)

Python Bindings
---------------
Python bindings are far from completion.
But basic functionality is implemented.
See `pyqrgen.py` for a GUI example of how to use qrgen python bindings.

To use qrgen in python you need `qrgen.so` and `pyqrgen.so` (called `pyqrgen.pyd` in Windows)


Building
--------

Just call make

	make

License
-------

Copyright (C) 2011 Joel Sutter (joel.sutter.dev@gmail.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

See LICENCE.txt and GPL.txt

NOTE: The term 'QR code' is a registered trademark of Denso Wave Incorporated.

qrgen 0.1
=========

qrgen is a library that generates qrcodes.
It comes with a sample program to demonstrate how to use the library.

Requirements
------------
- gnu tool chain
- libCairo >= 1.10.0 (probably works also with older releases, but i didn't test it)

Optional:

- gtk+-2.0 >= 2.20.1
- python >= 2.7

Usage (Sample Program)
----------------------

	qrgen [-m mode] [-e errorCorrectionLevel] [-v version] [-w] [-f file] [-s size] [-h] [--] data ...

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
	
	-h, --help             Print this help.
	
	--version              Print version number.
	
	--                     Ignore all options after this.
	
	data                   The data to encode.

	
For Example:

	$ qrgen -m b -e h -f ./filename.svg -s 111 https://github.com/schoeggu/qrcode-generator

Will create this:

![generated QR Code](http://img11.imageshack.us/img11/1316/qrgen1.png)
	
Usage (Library)
---------------

Requirded Includes:

	#include "qrgen.h"
	#include <cairo.h>


Functions:
Initialize The Qrgen Library

	void qrgen_init()
Free the allocated Memory again

	void qrgen_destroy()
	
Generate a qrcode and draw it to `context`

	bool qrgen_generate(const byte* data, int dataSize, int version, EncodeModeIndicator mode, ECLevel ecLevel, cairo__t* context, int pixSize)
	
Parameters:

- `data`      The data you want to encode.
- `dataSize`  Number of bytes in data.
- `version`   The Symbol Version. 1 - 40. If 0, qrgen chooses the smallest possible version.
- `mode`      Encode Mode.
- `ecLevel`   Error Correction Level.
- `context`   The cairo context to which is painted.
- `pixSize`   Size of the qrcode in pixels.

Returns:

- `true`      if Succeeded
- `false`     if Failed

Example:

	#include "qrgen.h"
	#include <cairo.h>

	int main(int argc, char** argv)
	{
		cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 105, 105);
		cairo_t* cr = cairo_create(surface);
		
		/* initialize qrgen */
		qrgen_init();
		
		/* generate qrcode and draw it to the surface    */
		/* Version: auto, Encode mode: Alphanumeric      */
		/* Error correction level: Medium, Size: 105x105 */
		qrgen_generate((byte*)"Hello Old Friend", 17, 0, ModeAlpha, EC_M, cr, 105);

		/* write the qrcode to a file */
		cairo_surface_write_to_png(surface, "hi.png");

		/*tear down qrgen */
		qrgen_destroy();
		
		cairo_destroy(cr);
		cairo_surface_destroy(surface);
		
		return 0;
	}

	
Will generate this:

![generated QR Code](http://img26.imageshack.us/img26/5615/qrgen2.png)
	
Building
--------

Just call make

	make
If you want gtk support (for -w option) call

	make USE_GTK=1
_NOTE:_ There is a directory called *rc*. The programm only works if you call it from the directory containing that *rc* directory. This is pretty annoying an i am going to fix that soon.

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

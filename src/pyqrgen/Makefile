.PHONY: all clean


all: pyqrgen.pyd

clean:
	rm -rf pyqrgen.so pyqrgen.pyd

pyqrgen.so: pyqrgen.c
	cc  -Wall -shared -fPIC pyqrgen.c $$(pkg-config --cflags --libs python cairo pycairo) -o pyqrgen.so

pyqrgen.pyd: pyqrgen.c
	gcc -shared pyqrgen.c -I../../include -o pyqrgen.pyd -mms-bitfields -IC:/Python27/include/pycairo -IC:/Programme/gtk/include/cairo -IC:/Programme/gtk/include/glib-2.0 -IC:/Programme/gtk/lib/glib-2.0/include -IC:/Programme/gtk/include -IC:/Programme/gtk/include/freetype2 -IC:/Programme/gtk/include/libpng14 -LC:/Programme/gtk/lib -lcairo -IC:/Python27/include C:\Python27\libs\python27.lib C:\Python27\libs\libpython27.a ../qrgen/libqrgen.a -LC:/Programme/gtk/lib -lgtk-win32-2.0 -lgdk-win32-2.0 -latk-1.0 -lgio-2.0 -lpangowin32-1.0 -lgdi32 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lpango-1.0 -lcairo -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lglib-2.0 -lintl
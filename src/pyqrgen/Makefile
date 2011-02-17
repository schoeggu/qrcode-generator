.PHONY: all clean

all: pyqrgen.so

clean:
	rm -rf pyqrgen.so

pyqrgen.so: pyqrgen.c
	cc  -Wall -shared -fPIC pyqrgen.c $$(pkg-config --cflags --libs python cairo pycairo) -o pyqrgen.so

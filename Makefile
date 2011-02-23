SUBDIRS = src
.PHONY : all src

all : subdirs

install:

	if [ ! -h libqrgen.so.1 ]; then ln -s src/qrgen/libqrgen.so.1 . ; fi
	if [ ! -h qrgen ]; then ln -s src/app/qrgen . ; fi
	if [ ! -h pyqrgen.so.1 ]; then ln -s src/pyqrgen/pyqrgen.so.1 . ; fi
	if [ ! -h test.py ]; then ln -s src/pyqrgen/test.py . ; fi
	if [ ! -h pyqrgen.py ]; then ln -s src/pyqrgen/pyqrgen.py . ; fi

GENFILES = qrgen libqrgen.so libqrgen.so.1 pyqrgen.so pyqrgen.so.1 test.py pyqrgen.py

include Makefile.master

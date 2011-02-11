SUBDIRS = src
.PHONY : all src

all : subdirs

install:
	cp src/qrgen/libqrgen.so .
	cp src/qrgen/libqrgen.so.1 .
	cp src/app/qrgen .

GENFILES = qrgen libqrgen.so libqrgen.so.1

include Makefile.master

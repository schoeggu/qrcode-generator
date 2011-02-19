SRC =	pyqrgen.c \

LIB = pyqrgen.so

CFLAGS += -I../../include
LDFLAGS += -L../qrgen -lqrgen

PKGCONFIGCFLAGS = $(shell pkg-config --cflags cairo python pycairo)
PKGCONFIGLIBS = $(shell pkg-config --libs cairo python pycairo)

DEPENDFILE = .depend

GENFILES = $(LIB).1


all : lib

include ../../Makefile.master

-include $(DEPENDFILE)


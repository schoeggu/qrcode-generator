SRC =	pyqrgen.c \

LIB = pyqrgen.so

CFLAGS += -I../../include
LDFLAGS += -L../qrgen -lqrgen

PKGCONFIGCFLAGS = $(shell pkg-config --cflags cairo pycairo)$(shell python2-config --cflags)
PKGCONFIGLIBS = $(shell pkg-config --libs cairo pycairo)$(shell python2-config --cflags)

DEPENDFILE = .depend

GENFILES = $(LIB).1


all : lib

include ../../Makefile.master

-include $(DEPENDFILE)


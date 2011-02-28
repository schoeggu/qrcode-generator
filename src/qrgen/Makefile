SRC =	symbolinfo.c \
		bitstream.c \
		dataencoder.c \
		ec.c \
		gp.c \
		ap.c \
		util.c \
		painter.c \
		paintcontext.c \
		color.c \

LIB = libqrgen.so

CFLAGS += -I../../include
LDFLAGS += -lm

PKGCONFIGCFLAGS = $(shell pkg-config --cflags cairo )
PKGCONFIGLIBS = $(shell pkg-config --libs cairo )

DEPENDFILE = .depend

GENFILES = $(LIB).1

all : lib

include ../../Makefile.master

-include $(DEPENDFILE)


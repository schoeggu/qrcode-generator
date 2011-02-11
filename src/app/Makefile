SRC	=	test.c \
		window.c \

BIN = qrgen

CFLAGS += -I../../include

PKGCONFIGCFLAGS = $(shell pkg-config --cflags cairo gtk+-2.0)
PKGCONFIGLIBS = $(shell pkg-config --libs cairo gtk+-2.0)

LDFLAGS += -L../qrgen
LDFLAGS += -lqrgen

DEPENDFILE = .depend

all : bin

include ../../Makefile.master

-include $(DEPENDFILE)

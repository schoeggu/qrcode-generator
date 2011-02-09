BIN = galois
BIN_C	= test.c \
		  symbolinfo.c \
		  dataencoder.c \
		  bitstream.c \
		  util.c \
		  ec.c \
		  gp.c \
		  symbolinfo.c \
		  qrgen.c \
		  ap.c \
		  painter.c \
		  window.c \

LIB_NAME = 
LIB_C   = 

DEPENDFILE = .depend

PKGCONFIGCFLAGS = $(shell pkg-config --cflags cairo gtk+-2.0)
PKGCONFIGLIBS = $(shell pkg-config --libs cairo gtk+-2.0)

LIB_SRC = $(LIB_C) $(LIB_CPP)
LIB_OBJ = $(LIB_CPP:%.cpp=%.o)
LIB_OBJ += $(LIB_C:%.c=%.o)
LIB = $(LIB_NAME:%=%.so.1)
BIN_SRC = $(BIN_C) $(BIN_CPP)
BIN_OBJ = $(BIN_CPP:%.cpp=%.o) 
BIN_OBJ += $(BIN_C:%.c=%.o) 
OBJ = $(LIB_OBJ) $(BIN_OBJ)
SRC = $(BIN_SRC) $(LIB_SRC)

CC = cc

CFLAGS += -Wall 
CFLAGS += -ggdb
#CFLAGS += -fPIC
CFLAGS += $(PKGCONFIGCFLAGS)

LDFLAGS += $(PKGCONFIGLIBS)

ifdef O
CFLAGS += -O$O
endif

CPPFLAGS += $(CFLAGS)

CFLAGS +=

LDFLAGS += $(PKGCONFIGLIBS)

.PHONY : clean all depend

all : bin lib
bin : $(DEPENDFILE) $(BIN)
lib : $(DEPENDFILE) $(LIB)
depend : $(DEPENDFILE) $(SRC)

$(BIN) : $(LIB) $(BIN_OBJ) 
	$(CC) $(LDFLAGS) -o $@ $^

$(LIB) : $(LIB_OBJ)
	$(CC) -shared -Wl,-soname,$(LIB_NAME).so.1 -o $@ $^
	ln -fs $@ $(LIB_NAME).so

%.o : %.c
	$(CC) $(CFLAGS) -c $< 

$(DEPENDFILE) : $(SRC)
	$(CC) -MM $^ > $@

clean :	
	rm -rf $(BIN) $(LIB) $(OBJ) $(GENFILES) $(LIB_NAME) $(DEPENDFILE)

-include .depend


CC = gcc

SRCNAME = lketama.c
OBJNAME = lketama.o
LIBNAME = ketama.so

OMIT_FRAME_POINTER = -fomit-frame-pointer
CFLAGS = -I/usr/local/include/ -fPIC -O3 -Wall
LIBS = lketama
LFLAGS = -shared $(OMIT_FRAME_POINTER)
INSTALL_PATH = usr/local/lib/lua/5.1

all: $(LIBNAME)

$(OBJNAME): $(SRCNAME)
	$(CC) -o $(OBJNAME) -c $(SRCNAME) $(CFLAGS)

$(LIBNAME): $(OBJNAME)
	$(CC) -o $(LIBNAME) -shared $(OBJNAME) $(LFLAGS) -$(LIBS)

install: $(LIBNAME)
	install -D -s $(LIBNAME) $(DESTDIR)/$(INSTALL_PATH)/$(LIBNAME)

clean:
	rm -f $(LIBNAME) $(OBJNAME)

.PHONY: clean



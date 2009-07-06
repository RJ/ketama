PREFIX=$(DESTDIR)/usr
CFLAGS+=-fPIC -W -Wall -Werror

build:
	gcc $(CFLAGS) -O3 -c md5.c
	gcc $(CFLAGS) -O3 -c ketama.c
	gcc -shared -Wl,-soname,libketama.so.1 -lm -o libketama.so.1 ketama.o md5.o

debug:
	gcc $(CFLAGS) -g -DDEBUG -o md5-debug.o -c md5.c
	gcc $(CFLAGS) -g -DDEBUG -o ketama-debug.o -c ketama.c
	gcc -g -shared -Wl,-soname,libketama.so.1 -lm -o libketama.so.1 ketama-debug.o md5-debug.o

test: build
	LD_LIBRARY_PATH=. gcc $(CFLAGS) -I. -O3 -lm -o ketama_test ketama_test.c ./libketama.so.1
	@./test.sh
	@./downed_server_test.sh

install:
	install -d $(PREFIX)/lib $(PREFIX)/include
	install libketama.so.1 $(PREFIX)/lib/
	install ketama.h $(PREFIX)/include/
	cd ${PREFIX}/lib && ln -sf libketama.so.1 libketama.so

clean:
	rm -f *.o
	rm -f libketama.so.1
	rm -f ketama_test
	rm -f build-stamp configure-stamp

uninstall:
	rm $(PREFIX)/lib/libketama.so.1
	rm $(PREFIX)/include/ketama.h

deinstall: uninstall

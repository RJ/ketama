PREFIX=$(DESTDIR)/usr
CFLAGS+=-fPIC -W -Wall -Werror

build: driver erlang

driver:
	gcc $(CFLAGS) -I. -O3 -lm -lketama -o ketama_erlang_driver ketama_erlang_driver.c

erlang:
	erlc ketama.erl



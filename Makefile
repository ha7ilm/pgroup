CC = gcc
CFLAGS = --std=gnu99

all: pgroup
pgroup: pgroup.c
	$(CC) $(CFLAGS) pgroup.c -o pgroup
clean: 
	rm -rf pgroup 
install: all
	install -m 0755 pgroup /usr/bin

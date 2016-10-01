
CC=gcc
CFLAGS=-g -Wall -D_FILE_OFFSET_BITS=64
LDFLAGS=-L~/fuse-2.9.5/lib/ -lfuse

OBJ=memfs.o node.o dir.o

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

memfs: $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o memfs

.PHONY: clean
clean:
	rm -f *.o memfs


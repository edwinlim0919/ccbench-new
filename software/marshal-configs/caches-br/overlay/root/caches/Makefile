CC=riscv64-unknown-linux-gnu-gcc
CFLAGS=-Wall -std=gnu99 -fno-common -fno-builtin-printf -ffast-math -funroll-loops -I../common
SOURCES=caches.c cctimer.c cclfsr.c

LD_FLAGS=-static -ffast-math
LD_LIBS=-lm -lc
OBJECTS=caches.o cctimer.o cclfsr.o

compile: $(SOURCES)
	$(CC) $(CFLAGS) -c $(SOURCES)
	$(CC) $(LD_FLAGS) $(OBJECTS) $(LD_LIBS) -o caches

clean:
	rm -f *.o *.riscv *.txt caches

CC=riscv64-unknown-linux-gnu-gcc
CFLAGS=-Wall -std=gnu99 -fno-common -fno-builtin-printf -ffast-math -funroll-loops -I.
SOURCES=strided.c cctimer.c

LD_FLAGS=-static -ffast-math
LD_LIBS=-lm -lc
OBJECTS=strided.o cctimer.o

compile: $(SOURCES)
	$(CC) $(CFLAGS) -c $(SOURCES)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LD_LIBS) -o strided

clean:
	rm -f *.o *.riscv *.txt strided	

CC=riscv64-unknown-linux-gnu-gcc
CFLAGS=-Wall -std=gnu99 -fno-common -fno-builtin-printf -ffast-math -funroll-loops -I../common
SOURCES=caches.c ../common/cctimer.c ../common/cclfsr.c

LD_FLAGS=-static -ffast-math
LD_LIBS=-lm -lc
OBJECTS=caches.o cctimer.o cclfsr.o

RUN_TYPE=0
ARRAY_SIZE=128
NUM_ITERATIONS=10
APP_FLAGS?=$(ARRAY_SIZE) $(NUM_ITERATIONS) $(RUN_TYPE)

compile: $(SOURCES)
	$(CC) $(CFLAGS) -c $(SOURCES)
	$(CC) $(LD_FLAGS) $(OBJECTS) $(LD_LIBS) -o caches

clean:
	-rm -f *.o *.riscv *.txt caches

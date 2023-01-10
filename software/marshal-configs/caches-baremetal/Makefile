CC=riscv64-unknown-elf-gcc
CFLAGS=-std=gnu99 -O2 -ffast-math -funroll-loops -I. -mcmodel=medany -g
LD_FLAGS=-ffast-math -static
LD_LIBS=-lm -lc
SOURCES=caches.c cctimer.c cclfsr.c
OBJECTS=caches.o cctimer.o cclfsr.o

compile: $(SOURCES)
	$(CC) $(CFLAGS) -c $(SOURCES)
	$(CC) $(LD_FLAGS) $(OBJECTS) $(LD_LIBS) -o caches-baremetal.riscv

clean:
	 rm -f *.o *.riscv *.txt

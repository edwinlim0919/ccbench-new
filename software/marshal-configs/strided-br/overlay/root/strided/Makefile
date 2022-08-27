CC=riscv64-unknown-linux-gnu-gcc
CFLAGS=-Wall -std=gnu99 -fno-common -fno-builtin-printf -ffast-math -funroll-loops -I.
C_SOURCES=strided.c cctimer.c
CPP_SOURCES=steadytimer.cpp steadytimer_interface.cpp

LD_FLAGS=-static -ffast-math
LD_LIBS=-lm -lc -lstdc++
OBJECTS=strided.o cctimer.o steadytimer.o steadytimer_interface.o

compile: $(C_SOURCES) $(CPP_SOURCES)
	$(CC) $(CFLAGS) -c $(C_SOURCES)
	riscv64-unknown-linux-gnu-g++ -std=c++0x -c $(CPP_SOURCES)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LD_LIBS) -o strided

clean:
	rm -f *.o *.riscv *.txt strided	

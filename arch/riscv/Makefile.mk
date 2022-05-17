##########################################
# Architecture-specific Makefile fragment
##########################################

#CC=riscv-gcc
CC=riscv64-unknown-elf-gcc

# for now, compile common/barrier.* using our own barrier implementations.
# if your compiler cant handle march, you're using an older version of gcc
# CFLAGS=-Wa,-march=RVIMAFDXhwacha -std=gnu99 -O2 -ffast-math -funroll-loops -I../common
 
#CFLAGS=-std=c99 -mtune=native -march=native -mssse3 -O3 -funroll-loops -I../common -DWITH_BARRIER -Wall
CFLAGS=-Wa,-Wall -O2 -fno-common -fno-builtin-printf -ffast-math -funroll-loops -I../common -spec=htif_nano.specs
#CFLAGS=-mcmodel=medany -Wall -O2 -fno-common -fno-builtin-printf
	
LD_FLAGS= -ffast-math -static -specs=htif_nano.specs
# LDFLAGS=-ffast-math -static -nostdlib -nostartfiles -lgcc
LD_LIBS=-lm -lc

# riscv64-unknown-elf-gcc -fno-common -fno-builtin-printf -specs=htif_nano.specs -c hello.c
# riscv64-unknown-elf-gcc -static -specs=htif_nano.specs hello.o -o hello.riscv
# spike hello.riscv


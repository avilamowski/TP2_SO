include Makefile.inc

KERNEL=kernel.bin
SOURCES=$(wildcard *.c drivers/*.c interruptions/*.c lib/*.c memory/registers.c memory/memoryInfo.c memory/buddyMemoryManager.c pipes/*.c processes/*.c scheduler/*.c semaphores/*.c syscalls/*.c )
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_SHARED=$(wildcard ../Shared/*.c)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_SHARED=$(SOURCES_SHARED:.c=.o)
LOADERSRC=loader.asm


LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM) $(OBJECTS_SHARED)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS) $(OBJECTS_SHARED)
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o kernel.elf $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS) $(OBJECTS_SHARED)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o *.o *.bin

.PHONY: all clean

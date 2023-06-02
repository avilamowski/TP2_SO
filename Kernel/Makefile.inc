GCC=gcc
LD=ld
AR=ar
ASM=nasm

GCCFLAGS=-g -I ../Shared/include -m64 -fno-exceptions -fno-asynchronous-unwind-tables -mno-mmx -mno-sse -mno-sse2 -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc -mno-red-zone -Wall -ffreestanding -nostdlib -fno-common -std=c99
ARFLAGS=rvs
ASMFLAGS=-g -felf64
LDFLAGS=--warn-common -z max-page-size=0x1000 -g

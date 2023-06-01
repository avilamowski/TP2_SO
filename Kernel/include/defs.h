/***************************************************
  Defs.h
****************************************************/

#ifndef _DEFS_H_
#define _DEFS_H_

/* Flags para derechos de acceso de los segmentos */
#define ACS_PRESENT 0x80 /* segmento presente en memoria */
#define ACS_CSEG 0x18	 /* segmento de codigo */
#define ACS_DSEG 0x10	 /* segmento de datos */
#define ACS_READ 0x02	 /* segmento de lectura */
#define ACS_WRITE 0x02	 /* segmento de escritura */
#define ACS_IDT ACS_DSEG
#define ACS_INT_386 0x0E /* Interrupt GATE 32 bits */
#define ACS_INT (ACS_PRESENT | ACS_INT_386)

#define ACS_CODE (ACS_PRESENT | ACS_CSEG | ACS_READ)
#define ACS_DATA (ACS_PRESENT | ACS_DSEG | ACS_WRITE)
#define ACS_STACK (ACS_PRESENT | ACS_DSEG | ACS_WRITE)

/* Direccion del MemoryManager */
#define SYSTEM_VARIABLES 0x5A00
#define MEMORY_MANAGER_ADDRESS 0x50000	  // MemoryManagerCDT
#define SCHEDULER_ADDRESS 0x51000		  // SchedulerCDT
#define SEMAPHORE_MANAGER_ADDRESS 0x60000 // SemaphoreCDT
#define PIPE_MANAGER_ADDRESS 0x70000	  // PipeManagerCDT

/* Constantes */
#define NULL ((void *) 0)
#define EOF ((char) -1)

/* File Descriptors*/
#define DEV_NULL -1
#define STDIN 0
#define STDOUT 1
#define STDERR 2
// #define KBDIN 3

#endif
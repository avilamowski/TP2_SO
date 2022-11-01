 /*
 *   interrupts.h
 *
 *  Created on: Apr 18, 2010
 *      Author: anizzomc
 */

#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include <idtLoader.h>

void _ex00Handler(void);
void _ex06Handler(void);
void _ex0DHandler(void);
void _ex0EHandler(void);
void _irq00Handler(void);
void _irq01Handler(void);
void _irq02Handler(void);
void _irq03Handler(void);
void _irq04Handler(void);
void _irq05Handler(void);
void _syscallHandler(void);
void _exception0Handler(void);

void _cli(void);

void _sti(void);

void _hlt(void);

void picMasterMask(uint8_t mask);

void picSlaveMask(uint8_t mask);

//Termina la ejecución de la cpu.
void haltcpu(void);

#endif /* INTERRUPS_H_ */

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdint.h>
// Cantidad de registros que se guardan
#define QTY_REGS 17 

/**
 * @brief  Imprime los valores de los registros
 * @note  Se asume que todos los registros llegan por stack 
 * @param  rip: Valor del RIP
 * @param  rsp: Valor del RSP
 * @param  newRsp: Direccion del RSP a partir de la que se encuentran los registros
 */
void dump(uint64_t rip, uint64_t rsp, uint64_t * newRsp);

/**
 * @brief Devuelve el snapshot de los registros 
 * @return Puntero a los registros
 */
uint64_t * getLastRegSnapshot();

#endif
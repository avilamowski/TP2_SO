#ifndef _TIME_H_
#define _TIME_H_
#include <stdint.h>

/**
 * @brief Handler del timer tick
 */
void timerHandler();
/**
 * @return Devuelve los ticks 
 */
uint64_t ticksElapsed();

/**
 * @return  Devuelve los segundos
 */
int secondsElapsed();

#endif

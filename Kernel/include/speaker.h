#ifndef _SPEAKER_H
#define _SPEAKER_H
#include <stdint.h>
/**
 * @brief Reproduce un sonido
 * @param  frequency: Frecuencia del sonido
 * @param  ticks: Cantidad de ticks que se reproducira
 */
void playSound(uint64_t frequency, uint64_t ticks);
#endif
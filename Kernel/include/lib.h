#ifndef _LIB_H
#define _LIB_H

#include <stdint.h>

/**
 * @brief Copia a 'destination' 'length' caracteres de source
 * @param  destination: Direccion de origen
 * @param  source: Fuente
 * @param  length: Cantidad de bytes a copiar
 */
void *memcpy(void *destination, const void *source, uint64_t length);

/**
 * @brief Devuelve el scancode de la ultima tecla presionada
 * @return Scancode
 */
char getKeyPressed();

/**
 * @brief Permite obtener el fabricante del cpu
 * @param  result: Puntero donde se quiere guardar el fabricante del cpu
 * @return Direccion donde se escribio el fabricante
 */
char *cpuVendor(char *result);

/**
 * @brief Devuelve la hora actual en formato de 24hs
 * @param  h: Horas
 * @param  m: Minutos
 * @param  s: Segundos
 */
void getTime(uint8_t *h, uint8_t *m, uint8_t *s);

/**
 * @brief Reproduce un sonido en el speaker
 * @param  freq: Frecuencia del sonido
 */
void startSound(uint64_t freq);

/**
 * @brief Apaga el speaker
 */
void stopSound();

/**
 * @brief Realiza un snapshot de los registros en un momento arbitrario
 */
void saveRegisters();

/**
 * @brief  Detiene la ejecucion del programa hasta que se produzca una
 * interrupcion
 */
void _hlt();

void forceTimerTick();

int _xchg(int *lock, int value);

int _xadd(int inc, int *value);

#endif

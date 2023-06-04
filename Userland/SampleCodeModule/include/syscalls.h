#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <color.h>
#include <stdint.h>

int64_t write(int16_t fd, char *sourceBuffer, uint64_t len);

int64_t read(int16_t fd, char *destinationBuffer, uint64_t len);

/**
 * @brief Devuelve la hora expresada en segundos
 * @return Hora expresada en segundos
 */
uint32_t getSeconds();

/**
 * @brief Pone todos los pixeles de la pantalla en negro y limpia el buffer de
 * video
 */
void clear(void);

/**
 * @brief
 * @param regarr: Vector donde se llena la informacion de los registros
 * @return Puntero a la informacion de los registros
 */
uint64_t *getInfoReg(uint64_t *regarr);

/**
 * @brief Cambia el tamaño de la fuente
 * @param size: (1|2|3)
 */
void setFontSize(uint8_t size);

/**
 * @brief Devuelve las dimensiones de la pantalla
 * @return 32 bits menos significativos el ancho, 32 el alto
 */
uint32_t getScreenResolution();

/**
 * @brief  Dibuja un rectangulo
 * @param  x: Origen en x
 * @param  y: Origen en y
 * @param  width: Ancho
 * @param  height: Alto
 * @param  color: Color de relleno
 */
void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
			  Color color);

/**
 * @brief Devuelve la cantidad de ticks actuales
 * @return Cantidad de ticks
 */
uint64_t getTicks();

/**
 * @brief Llena un vector con 32 bytes de informacion a partir de una direccion
 * de memoria en hexa
 * @param pos: Direccion de memoria a partir de la cual se llena el vector
 * @param vec: Vector en el cual se llena la informacion
 */
void getMemory(uint64_t pos, uint8_t *vec);

/**
 * @brief Reproduce un sonido
 * @param frequency: Frecuencia del sonido
 * @param ticks: Cantidad de ticks que se desea que dure el sonido
 */
void playSound(uint64_t frequency, uint64_t ticks);

/**
 * @brief Ejecuta una excepcion de Invalid Opcode Exception
 */
void kaboom();

/**
 * @brief Establece un color de fuente
 * @param r: Color rojo
 * @param g: Color verde
 * @param b: Color azul
 */
void setFontColor(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Retorna el color de fuente que se esta usando actualmente
 * @return Color
 */
Color getFontColor();

void *malloc(uint64_t size);

void free(void *ptr);

int16_t createProcessWithFds(void *code, char **args, char *name, uint8_t priority, int16_t *fileDescriptors);

void exit();

uint16_t getpid();

void *ps(); // TODO cambiar a ProcessSnapshot

int32_t killProcess(uint16_t pid);

int8_t changeProcessPriority(uint16_t pid, uint8_t priority);

int8_t changeProcessStatus(uint16_t pid, uint8_t status);

void yield();

int32_t waitpid();

int8_t semInit(uint16_t id, uint32_t initialValue);

int8_t semOpen(uint16_t id);

int8_t semClose(uint16_t id);

int8_t semPost(uint16_t id);

int8_t semWait(uint16_t id);

int8_t pipeOpen(uint16_t id, uint8_t mode);

int8_t pipeClose(uint16_t id, uint8_t mode);

int16_t pipeGet();

void sleep(int seconds);

#endif
#include <keyboard.h>
#include <lib.h>
#include <scheduler.h>
#include <semaphoreManager.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <video.h>

#define BUFFER_CAPACITY 64 /* Longitud maxima del vector _buffer */
#define LCTRL 29
#define C_HEX 0x2E
#define D_HEX 0x20
#define R_HEX 0x13
#define RELEASED 0x80						  /* Mascara para detectar si se solto una tecla */
static uint8_t _bufferStart = 0;			  /* Indice del comienzo de la cola */
static uint8_t _bufferSize = 0;				  /* Longitud de la cola */
static int8_t _buffer[BUFFER_CAPACITY] = {0}; /* Vector ciclico que guarda las teclas
											   * que se van leyendo del teclado */
static uint8_t _ctrl = 0;					  /* Flag para detectar si se presiono ctrl */
static const char charHexMap[256] =			  /* Mapa de scancode a ASCII */
	{0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',
	 '=', '\b', ' ', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
	 '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
	 ';', '\'', 0, 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',',
	 '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0};

static void writeKey(uint8_t key);

/**
 * @brief  Obtiene el indice del elemento en la cola dado un corrimiento
 * @param  offset: corrimiento
 * @return Indice del elemento en la cola
 */
static int getBufferIndex(int offset) {
	return (_bufferStart + offset) % (BUFFER_CAPACITY);
}

void initializeKeyboardDriver() {
	semInit(IO_SEM_ID, 0);
}

void keyboardHandler() {
	uint8_t key = getKeyPressed();
	if (!(key & RELEASED)) {
		if (key == LCTRL)
			_ctrl = 1;
		else if (_ctrl) {
			if (key == C_HEX) {
				_bufferStart = _bufferSize = 0;
				killForegroundProcess();
			}
			else if (key == R_HEX)
				saveRegisters();
			else if (key == D_HEX && _bufferSize < BUFFER_CAPACITY - 1)
				writeKey(EOF);
		}
		else if (_bufferSize < BUFFER_CAPACITY - 1)
			writeKey(key);
	}
	else {
		if (key == (LCTRL | RELEASED))
			_ctrl = 0;
	}
}

static void writeKey(uint8_t key) {
	_buffer[getBufferIndex(_bufferSize)] = key;
	_bufferSize++;
	semPost(IO_SEM_ID);
}

int8_t getScancode() {
	if (_bufferSize > 0) {
		int8_t c = _buffer[getBufferIndex(0)];
		_bufferStart = getBufferIndex(1);
		_bufferSize--;
		return c;
	}
	return 0;
}

int8_t getAscii() {
	semWait(IO_SEM_ID);
	int scanCode = getScancode();
	if (scanCode == EOF)
		return EOF;
	return charHexMap[scanCode];
}

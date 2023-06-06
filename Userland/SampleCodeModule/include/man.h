#ifndef _MAN_H
#define _MAN_H
#include <shell.h>
/* Matriz con una breve explicacion de uso de cada uno de los comandos */
char *usages[QTY_COMMANDS] = {
	"Uso: help - muestra todos los comandos que existen en la terminal. No "
	"recibe parametros",

	"Uso: man [COMANDO] - explica el funcionamiento de un comando enviado como "
	"parametro",

	"Uso: inforeg - muestra informacion de los registros en un momento "
	"arbitrario de ejecucion del sistema. No recibe parametros",

	"Uso: time - despliega la hora actual. No recibe parametros",

	"Uso: div [OP1] [OP2] - hace la division entera de dos numeros naturales "
	"que recibe por parametro\n"
	"Ejemplo: div 10 5",

	"Uso: kaboom - arroja una excepcion de invalid opcode. No recibe "
	"parametros",

	"Uso: tron - el juego Tron consiste en dos jugadores que se mueven a la "
	"misma velocidad, y deben evitar "
	"chocarse con las paredes, con la cola de ellos mismos y la del "
	"adversario.\n El jugador numero 1 se movera con "
	"las flechas, mientras que el jugador numero 2 con las teclas W A S D. \n"
	"Con la tecla z, el jugador podra activar el modo turbo, que "
	"aumenta su velocidad. El juegador 2 lo hara con la tecla / \n"
	"El primer jugador que se choque, pierde",

	"Uso: tron-zen - el Tron-Zen es como el Tron, pero hay un unico jugador, "
	"que debe evitar chocarse con su propia cola y con las "
	"paredes",

	"Uso: font-size [1|2|3] - cambia la medida de la fuente. Para eso se deben "
	"enviar por parametro el numero 1, 2 o 3\n"
	"Ejemplo: font-size 2",

	"Uso: printmem [DIR] - imprime los primeros 32 bytes de memoria a partir "
	"de una direccion de memoria enviada como parametro\n"
	"Ejemplo: printmem 10ff8c",

	"Uso: clear - limpia la pantalla. No recibe parametros",

	"Uso: loop [INTERVALO] - muestra su PID cada una cierta cantidad de tiempo\n"
	"Ejemplo: loop 1",

	"Uso: kill [PID] - mata a un proceso dado su PID",

	"Uso: ps - muestra la informacion de los procesos vivos y zombificados.\n"
	"No recibe parametros",

	"Uso: mem - muestra informacion del MemoryManager",

	"Uso: nice [PID] [PRIORIDAD] - cambia la prioridad de un proceso dado su ID y la nueva prioridad",

	"Uso: block [PID] - cambia el estado de un proceso entre bloqueado y listo dado su ID",

	"Uso: echo [PARAM] - imprime por pantalla su primer parametro\n"
	"Ejemplo: echo hola",

	"Uso: wc - cuenta la cantidad de lineas del input",

	"Uso: cat - imprime el STDIN tal como lo recibe",

	"Uso: filter - filtra las vocales del input",

	"Uso: phylo - ejecuta la simulacion del problema de los filosofos comensales",

	"Uso: test_mm [BYTES] - ejecuta el test de memory manager utilizando una cierta cantidad de bytes",

	"Uso: test_prio - ejecuta el test de prioridades",

	"Uso: test_sync [N] [USE_SEM] - ejecuta el test de sincronizacion",

	"Uso: test_processes [CANT_PROCESOS] - ejecuta el test de procesos",

	"Uso: test-named-pipes - ejecuta el test de pipes con nombre, creando un proceso escritor que envia\n"
	"un mensaje secreto a un lector a traves de un pipe"};
#endif

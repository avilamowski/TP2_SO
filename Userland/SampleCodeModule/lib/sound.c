// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <sound.h>
#include <syscalls.h>

/* Libreria en desuso, tron no funciona en esta version */

void playSoundVictory1() {
	playSound(390, 3);
	// playSound(520, 3);
	// playSound(660, 3);
	// playSound(780, 6);
	// playSound(660, 3);
	// playSound(780, 6);
}

void playSoundVictory2() {
	playSound(880, 3);
	// playSound(780, 3);
	// playSound(660, 3);
	// playSound(585, 6);
	// playSound(780, 3);
	// playSound(520, 6);
}

void playSoundDraw() {
	for (int i = 0; i < 18; i++)
		playSound(500 - 20 * i, 1);
}

void playSoundLimitReached() {
	playSound(66, 1);
}
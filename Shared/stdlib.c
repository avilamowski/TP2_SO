#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

unsigned int log(uint64_t n, int base) {
	unsigned int count = 0;
	while (n /= base)
		count++;
	return count;
}

int itoa(uint64_t n, char *buffer, int base) {
	if (n == 0) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return 1;
	}

	unsigned int len = 0;
	int i = 0;
	// Mover a otra funcion si se quiere implementar un itoa que soporte negativos
	/*if (n < 0 && base == 10)
	{
		n = -n;
		buffer[i] = '-';
		len++;
		i++;
	}*/

	len += log(n, base) + 1;
	while (n != 0) {
		int r = n % base;
		buffer[len - i++ - 1] = (r > 9) ? (r - 10) + 'A' : r + '0';
		n /= base;
	}
	buffer[i] = '\0';
	return len;
}

int strtoi(char *s, char **end) {
	int num = 0;
	while (isNumber(*s))
		num = num * 10 + *(s++) - '0';
	*end = s;
	return num;
}

int atoi(char *s) {
	int num = 0;
	while (isNumber(*s))
		num = num * 10 + *(s++) - '0';
	return num;
}

int strtoh(char *s, char **end) {
	int num = 0;
	int aux;
	while (isNumber(*s) || isHex(*s)) {
		aux = toLower(*s);
		num =
			num * 16 + isNumber(aux) * (aux - '0') + isHex(aux) * (aux - 'a' + 10);
		s++;
	}
	*end = s;
	return num;
}

int stringArrayLen(char **array) {
	int len = 0;
	while (*(array++) != NULL)
		len++;
	return len;
}

void *memset(void *destiation, int32_t c, uint64_t length) {
	uint8_t chr = (uint8_t) c;
	char *dst = (char *) destiation;

	while (length--)
		dst[length] = chr;

	return destiation;
}
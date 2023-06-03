#include <stdint.h>
#include <string.h>

int strcmp(const char *s1, const char *s2) {
	while (*s1 != 0 && *s1 == *s2) {
		s1++;
		s2++;
	}
	return *s1 - *s2;
}

int strlen(const char *str) {
	int len = 0;
	while (*(str++) != '\0')
		len++;
	return len;
}

int strcpy(char *dest, const char *origin) {
	return strcpychar(dest, origin, '\0');
}

int strcpychar(char *dest, const char *origin, char limit) {
	int idx = 0;
	while (origin[idx] != limit && origin[idx] != '\0') {
		dest[idx] = origin[idx];
		idx++;
	}
	dest[idx] = 0;
	return idx;
}

int strcpycharlimited(char *dest, const char *origin, char limitChar, int limitIndex) {
	int idx = 0;
	while (origin[idx] != limitChar && idx < limitIndex && origin[idx] != '\0') {
		dest[idx] = origin[idx];
		idx++;
	}
	dest[idx] = 0;
	return idx;
}
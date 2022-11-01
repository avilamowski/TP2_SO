#ifndef _STRING_H
#define _STRING_H
/**
 * @brief Determina si dos cadenas de caracteres son iguales o diferentes
 * @param s1: Cadena de caracteres 
 * @param s2: Cadena de caracteres 
 * @return Numero positivo si s1 > s2, 0 si son iguales y numero negativo si s1 < s2
 */
int strcmp(const char * s1, const char * s2);

/**
 * @brief Copia los caracteres de la cadena origin en la cadena dest hasta que aparezca un caracter limit
 * @param dest: Cadena de destino
 * @param origin: Cadena de origen
 * @param limit: Caracter de corte 
 * @return Longitud de la cadena de destino 
 */
int strcpychar(char * dest, const char * origin, char limit);

/**
 * @brief Copia toda la cadena origin en dest (analogo a strcpychar pero limit es un '\0')
 * @param dest: Cadena de destino
 * @param origin: Cadena de origen
 * @return Londitud de la cadena de destino
 */
int strcpy(char * dest, const char * origin);
#endif

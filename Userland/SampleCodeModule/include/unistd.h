#ifndef UNISTD_H
#define UNISTD_H

#include <stdint.h>

uint64_t write(int fd, const void * buf, uint64_t count);
uint64_t read(int fd);

#endif
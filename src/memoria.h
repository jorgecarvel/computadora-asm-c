#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdint.h>

#define MEM_SIZE 256

typedef struct {
    uint8_t data[MEM_SIZE];
} Memoria;

void memoria_init(Memoria *m);

#endif


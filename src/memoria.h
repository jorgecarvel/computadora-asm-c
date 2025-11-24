#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdint.h>

#define MEM_SIZE 256

typedef struct {
    uint8_t data[MEM_SIZE];
} Memoria;

void memoria_init(Memoria *m);
<<<<<<< HEAD
=======
uint8_t memoria_leer(Memoria *m, uint8_t direccion);
void memoria_escribir(Memoria *m, uint8_t direccion, uint8_t valor);
>>>>>>> 3f14a8867c6b4f7bdc29a45de37cf8e4e9dfae36

#endif


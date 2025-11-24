#ifndef CPU_H
#define CPU_H

#include "memoria.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
<<<<<<< HEAD
    uint8_t A;          // Registro acumulador
    uint16_t PC;        // Contador de programa
    uint16_t SP;        // Puntero de pila
    uint8_t Z;          // Bandera de cero
    Memoria *mem;       // Referencia a memoria
    int halted;         // Estado
} CPU;


=======
    uint8_t A; // Acumulador
    uint8_t PC; // Program Counter
    bool running; // bandera de ejecución
    Memoria *mem;
} CPU;

>>>>>>> 3f14a8867c6b4f7bdc29a45de37cf8e4e9dfae36
void cpu_init(CPU *cpu, Memoria *mem);
void cpu_ejecutar(CPU *cpu);

#endif


#ifndef CPU_H
#define CPU_H

#include "memoria.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t A; // Acumulador
    uint8_t PC; // Program Counter
    bool running; // bandera de ejecución
    Memoria *mem;
} CPU;

void cpu_init(CPU *cpu, Memoria *mem);
void cpu_ejecutar(CPU *cpu);

#endif


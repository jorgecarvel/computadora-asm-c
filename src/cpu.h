#ifndef CPU_H
#define CPU_H

#include "memoria.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t A;          // Registro acumulador
    uint16_t PC;        // Contador de programa
    uint16_t SP;        // Puntero de pila
    uint8_t Z;          // Bandera de cero
    Memoria *mem;       // Referencia a memoria
    int halted;         // Estado
} CPU;


void cpu_init(CPU *cpu, Memoria *mem);
void cpu_ejecutar(CPU *cpu);

#endif


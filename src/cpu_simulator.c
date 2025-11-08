/*
 * Archivo: cpu_simulator.c
 * Función: Programa principal del simulador de CPU.
 *           Crea e inicializa la memoria y CPU, carga instrucciones
 *           desde archivo o manualmente, y ejecuta el ciclo del CPU.
 * 
 * El proceso de suma se demuestra con un programa ejemplo que suma dos números
 * usando las instrucciones LOADI, STORE y ADD.
 */

#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "alu.h"
#include "memoria.h"

// Carga datos desde un archivo de texto (.mem) a la memoria simulada.
void cargar_memoria_desde_archivo(Memoria *m, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    int i = 0;
    int val;
    while (i < MEM_SIZE && fscanf(f, "%d", &val) == 1) {
        m->data[i++] = (uint8_t)val;  // Carga byte a byte
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    Memoria mem;
    memoria_init(&mem);  // Inicializa memoria en 0

    if (argc > 1) {
        cargar_memoria_desde_archivo(&mem, argv[1]);
    } else {
        mem.data[100] = 30;  // Dato 1
        mem.data[101] = 20;  // Dato 2
        // Programa de ejemplo
        mem.data[0] = 0x05; mem.data[1] = 30;   // LOADI 30
        mem.data[2] = 0x02; mem.data[3] = 200;  // STORE 200
        mem.data[4] = 0x05; mem.data[5] = 20;   // LOADI 20
        mem.data[6] = 0x03; mem.data[7] = 200;  // ADD 200 -> A=20+30=50
        mem.data[8] = 0x08;                     // HALT
    }

    CPU cpu;
    cpu_init(&cpu, &mem);  // Inicializa CPU
    cpu_ejecutar(&cpu);    // Ejecuta el programa cargado

    printf("Registro A final: %d\n", cpu.A);  // Resultado final
    return 0;
}

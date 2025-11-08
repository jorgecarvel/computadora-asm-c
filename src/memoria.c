/*
 * Archivo: memoria.c
 * Función: Implementa el módulo de memoria principal del simulador.
 *           Se encarga de almacenar y recuperar datos e instrucciones.
 */

#include "memoria.h"  // Define la estructura y tamaño de la memoria.

// Inicializa toda la memoria a 0
void memoria_init(Memoria *m) {
    for (int i = 0; i < MEM_SIZE; i++)
        m->data[i] = 0;  // Limpia cada posición
}

// Lee un byte desde una dirección específica
uint8_t memoria_leer(Memoria *m, uint8_t direccion) {
    return m->data[direccion];  // Retorna el valor almacenado
}

// Escribe un byte en una dirección específica
void memoria_escribir(Memoria *m, uint8_t direccion, uint8_t valor) {
    m->data[direccion] = valor;  // Guarda el valor en la dirección dada
}

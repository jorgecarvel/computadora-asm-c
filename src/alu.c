<<<<<<< HEAD
#include "alu.h"

uint8_t alu_add(uint8_t a, uint8_t b) {
    return (uint8_t)(a + b);
}

uint8_t alu_sub(uint8_t a, uint8_t b) {
    return (uint8_t)(a - b);
}

uint8_t alu_mul(uint8_t a, uint8_t b) {
    return (uint8_t)(a * b);
}

=======
/*
 * Archivo: alu.c
 * Función: Implementa la Unidad Aritmético-Lógica (ALU) del CPU simulado.
 *           Se encarga de realizar operaciones matemáticas básicas.
 *           Aquí se define la función que efectúa la suma de dos operandos.
 * 
 * Inicia el proceso de la suma: 
 *      La función `alu_suma()` es llamada desde `cpu.c` (instrucción ADD o ADDI)
 *      para realizar la suma de dos números.
 */

#include "alu.h"  // Incluye la definición de la interfaz (prototipo) de la ALU.

// Función: alu_suma
// Recibe dos operandos de 8 bits (uint8_t a, b) y devuelve su suma.
// Esta operación representa la parte aritmética de la ALU.
uint8_t alu_suma(uint8_t a, uint8_t b) {
    return a + b;  // Retorna el resultado de la suma binaria simple.
}
>>>>>>> 3f14a8867c6b4f7bdc29a45de37cf8e4e9dfae36

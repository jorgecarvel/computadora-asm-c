/*
 * Archivo: cpu.c
 * Función: Define la unidad central de procesamiento (CPU) del simulador.
 *           Controla el ciclo FETCH-DECODE-EXECUTE, obteniendo instrucciones
 *           de memoria y ejecutándolas mediante la ALU o acceso a memoria.
 * 
 * El proceso de suma se inicia cuando se ejecuta la instrucción:
 *      - `ADD addr` (opcode 0x03) o `ADDI imm` (opcode 0x09),
 *        las cuales llaman a `alu_suma()` definida en `alu.c`.
 */

#include "cpu.h"
#include "alu.h"
#include <stdio.h>

void cpu_init(CPU *cpu, Memoria *mem) {
    cpu->A = 0;           // Registro acumulador inicializado a 0
    cpu->PC = 0;          // Contador de programa en posición 0
    cpu->running = true;  // La CPU comienza en modo activo
    cpu->mem = mem;       // Asocia la memoria al CPU
}

void cpu_ejecutar(CPU *cpu) {
    while (cpu->running) {                       // Ciclo principal de ejecución
        uint8_t opcode = memoria_leer(cpu->mem, cpu->PC);  // FETCH instrucción
        cpu->PC++;  // Avanza el contador de programa

        switch (opcode) {
            case 0x01: { // LOAD addr
                uint8_t addr = memoria_leer(cpu->mem, cpu->PC++);
                cpu->A = memoria_leer(cpu->mem, addr);
                break;
            }
            case 0x02: { // STORE addr
                uint8_t addr = memoria_leer(cpu->mem, cpu->PC++);
                memoria_escribir(cpu->mem, addr, cpu->A);
                break;
            }
            case 0x03: { // ADD addr
                uint8_t addr = memoria_leer(cpu->mem, cpu->PC++);
                uint8_t val = memoria_leer(cpu->mem, addr);
                cpu->A = alu_suma(cpu->A, val);  // Llama a ALU
                break;
            }
            case 0x04: { // SUB addr
                uint8_t addr = memoria_leer(cpu->mem, cpu->PC++);
                uint8_t val = memoria_leer(cpu->mem, addr);
                cpu->A = cpu->A - val;
                break;
            }
            case 0x05: { // LOADI imm
                uint8_t imm = memoria_leer(cpu->mem, cpu->PC++);
                cpu->A = imm;
                break;
            }
            case 0x09: { // ADDI imm
                uint8_t imm = memoria_leer(cpu->mem, cpu->PC++);
                cpu->A = alu_suma(cpu->A, imm);
                break;
            }
            case 0x06: { // JMP addr
                uint8_t addr = memoria_leer(cpu->mem, cpu->PC++);
                cpu->PC = addr;
                break;
            }
            case 0x07: { // JZ addr
                uint8_t addr = memoria_leer(cpu->mem, cpu->PC++);
                if (cpu->A == 0) cpu->PC = addr;
                break;
            }
            case 0x0A: { // OUT addr
                uint8_t addr = memoria_leer(cpu->mem, cpu->PC++);
                printf("OUT: %d\n", memoria_leer(cpu->mem, addr));
                break;
            }
            case 0x08: { // HALT
                cpu->running = false;
                break;
            }
            default:
                printf("Error: opcode desconocido 0x%02X en PC=0x%02X\n", opcode, cpu->PC-1);
                cpu->running = false;
                break;
        }
    }
}

/*
 * Archivo: cpu.c
<<<<<<< HEAD
 * Implementación del simulador de CPU para el sistema ensamblador.
 *
 * Instrucciones soportadas:
 *  1  NOP
 *  2  STORE dir
 *  3  ADD dir
 *  4  SUB dir
 *  5  LOADI val
 *  6  LOADM dir
 *  7  JMP dir
 *  8  HALT
 *  9  PUSH
 * 10  POP
 * 11  CALL dir
 * 12  RET
 * 13  JMPZ dir
 * 14  MUL dir
 */

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "cpu.h"
#include "memoria.h"
#include "alu.h"

/* --- Métricas/telemetría --- */
static unsigned long instr_count = 0;
static unsigned long mem_accesses = 0;
static unsigned long jumps_taken = 0;
static unsigned long jumps_not_taken = 0;
static unsigned long cycles = 0;
static int sp_min_tracked = 0;

/* Estructura CPU inicializa SP y demás */
void cpu_init(CPU *cpu, Memoria *mem) {
    cpu->A = 0;
    cpu->PC = 0;
    cpu->SP = MEM_SIZE - 1;   // Pila al final de la memoria
    cpu->Z = 0;
    cpu->halted = 1;
    cpu->halted = 0;
    cpu->mem = mem;

    /* Inicializar métricas por ejecución */
    instr_count = 0;
    mem_accesses = 0;
    jumps_taken = 0;
    jumps_not_taken = 0;
    cycles = 0;
    sp_min_tracked = cpu->SP;
}

// ==================== FUNCIONES AUXILIARES ====================

static uint8_t fetch(CPU *cpu) {
    if (cpu->PC >= MEM_SIZE) {
        printf("[ERROR] Lectura fuera de memoria en PC=%d\n", cpu->PC);
        cpu->halted = 1;
        return 0;
    }
    return cpu->mem->data[cpu->PC++];
}

static void push(CPU *cpu, uint8_t val) {
    if (cpu->SP == 0) {
        printf("[ERROR] Desbordamiento de pila (SP=%d)\n", cpu->SP);
        cpu->halted = 1;
        return;
    }
    cpu->mem->data[cpu->SP--] = val;
    mem_accesses++; /* escritura en memoria */
    if (cpu->SP < sp_min_tracked) sp_min_tracked = cpu->SP;
}

static uint8_t pop(CPU *cpu) {
    if (cpu->SP >= MEM_SIZE - 1) {
        printf("[ERROR] Pila vacía (SP=%d)\n", cpu->SP);
        cpu->halted = 1;
        return 0;
    }
    uint8_t v = cpu->mem->data[++cpu->SP];
    mem_accesses++; /* lectura en memoria */
    return v;
}

// ==================== EJECUCIÓN PRINCIPAL ====================

void cpu_ejecutar(CPU *cpu) {
    clock_t t0 = clock();

    while (!cpu->halted && cpu->PC < MEM_SIZE) {
        instr_count++;
        cycles++; /* contar un ciclo por instrucción (modelo simple) */

        uint8_t opcode = fetch(cpu);

        switch (opcode) {
            case 1: // NOP
                break;

            case 2: { // STORE dir
                uint8_t addr = fetch(cpu);
                if (addr < MEM_SIZE) {
                    cpu->mem->data[addr] = cpu->A;
                    mem_accesses++;
                } else
                    printf("[WARN] Dirección fuera de rango en STORE %d\n", addr);
                break;
            }

            case 3: { // ADD dir
                uint8_t addr = fetch(cpu);
                if (addr < MEM_SIZE) {
                    mem_accesses++;
                    cpu->A = alu_add(cpu->A, cpu->mem->data[addr]);
                } else {
                    printf("[WARN] Dirección fuera de rango en ADD %d\n", addr);
                    cpu->A = alu_add(cpu->A, 0);
                }
                cpu->Z = (cpu->A == 0);
                break;
            }

            case 4: { // SUB dir
                uint8_t addr = fetch(cpu);
                if (addr < MEM_SIZE) {
                    mem_accesses++;
                    cpu->A = alu_sub(cpu->A, cpu->mem->data[addr]);
                } else {
                    printf("[WARN] Dirección fuera de rango en SUB %d\n", addr);
                    cpu->A = alu_sub(cpu->A, 0);
                }
                cpu->Z = (cpu->A == 0);
                break;
            }

            case 5: { // LOADI val
                uint8_t val = fetch(cpu);
                cpu->A = val;
                cpu->Z = (cpu->A == 0);
                break;
            }

            case 6: { // LOADM dir
                uint8_t addr = fetch(cpu);
                if (addr < MEM_SIZE) {
                    cpu->A = cpu->mem->data[addr];
                    mem_accesses++;
                } else
                    printf("[WARN] Dirección fuera de rango en LOADM %d\n", addr);
                cpu->Z = (cpu->A == 0);
                break;
            }

            case 7: { // JMP dir
                uint8_t addr = fetch(cpu);
                if (addr < MEM_SIZE) {
                    cpu->PC = addr;
                    jumps_taken++;
                } else {
                    printf("[WARN] Salto fuera de rango a %d\n", addr);
                    jumps_not_taken++;
                }
                break;
            }

            case 8: // HALT
                cpu->halted = 1;
                printf("[CPU] HALT ejecutado\n");
                break;

            case 9: // PUSH
                push(cpu, cpu->A);
                break;

            case 10: // POP
                cpu->A = pop(cpu);
                cpu->Z = (cpu->A == 0);
                break;

            case 11: { // CALL dir
                uint8_t addr = fetch(cpu);
                if (addr < MEM_SIZE) {
                    /* Guardamos dirección de retorno como byte */
                    push(cpu, (uint8_t)cpu->PC);
                    cpu->PC = addr;
                    jumps_taken++;
                } else {
                    printf("[WARN] Dirección de CALL fuera de rango %d\n", addr);
                    jumps_not_taken++;
                }
                break;
            }

            case 12: { // RET
                uint8_t retAddr = pop(cpu);
                if (retAddr < MEM_SIZE) {
                    cpu->PC = retAddr;
                    jumps_taken++;
                } else {
                    printf("[WARN] Dirección de RET inválida %d\n", retAddr);
                    jumps_not_taken++;
                }
                break;
            }

            case 13: { // JMPZ dir (salta si Z == 1)
                uint8_t addr = fetch(cpu);
                if (cpu->Z && addr < MEM_SIZE) {
                    cpu->PC = addr;
                    jumps_taken++;
                } else {
                    jumps_not_taken++;
                }
                break;
            }

            case 14: { // MUL dir
                uint8_t addr = fetch(cpu);
                if (addr < MEM_SIZE) {
                    mem_accesses++;
                    cpu->A = alu_mul(cpu->A, cpu->mem->data[addr]);
                } else {
                    printf("[WARN] Dirección fuera de rango en MUL %d\n", addr);
                    cpu->A = alu_mul(cpu->A, 0);
                }
                cpu->Z = (cpu->A == 0);
                break;
            }

            default:
                printf("[ERROR] Opcode desconocido: %d en PC=%d\n", opcode, cpu->PC - 1);
                cpu->halted = 1;
                break;
        }
    }

    clock_t t1 = clock();
    double elapsed = (double)(t1 - t0) / CLOCKS_PER_SEC;

    /* Estado final */
    printf("\n=== CPU Detenida ===\n");
    printf("A = %d, PC = %d, SP = %d, Z = %d\n", cpu->A, cpu->PC, cpu->SP, cpu->Z);

    /* Imprimir métricas */
    printf("\n--- MÉTRICAS DE EJECUCIÓN (CPU) ---\n");
    printf("Instrucciones ejecutadas: %lu\n", instr_count);
    printf("Ciclos (modelo simple): %lu\n", cycles);
    printf("Accesos a memoria: %lu\n", mem_accesses);
    printf("Saltos tomados: %lu\n", jumps_taken);
    printf("Saltos no tomados: %lu\n", jumps_not_taken);
    if (sp_min_tracked <= MEM_SIZE - 1)
        printf("Profundidad máxima de pila (bytes usados): %d\n", (MEM_SIZE - 1) - sp_min_tracked);
    else
        printf("Profundidad máxima de pila: 0\n");
    printf("Tiempo de ejecución (CPU): %.6f s\n", elapsed);
=======
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
>>>>>>> 3f14a8867c6b4f7bdc29a45de37cf8e4e9dfae36
}

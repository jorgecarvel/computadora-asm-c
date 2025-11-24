/*
 * cpu_simulator.c - main que carga .mem (binario por línea o decimal) y ejecuta CPU
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "memoria.h"
#include "cpu.h"

/*
 * Función: cargar_memoria_desde_archivo
 * -------------------------------------
 * Lee un archivo .mem texto donde cada línea contiene:
 *   - un valor binario (ej. 00101011)
 *   - o un número decimal
 *   - o un hexadecimal (0x20)
 * y lo guarda byte a byte dentro de la memoria simulada.
 */
int cargar_memoria_desde_archivo(Memoria *m, const char *path) {

    FILE *f = fopen(path, "r");      // Intenta abrir el archivo en modo lectura
    if (!f) {                        // Si no se pudo abrir...
        fprintf(stderr, "No se pudo abrir %s\n", path);
        return -1;                     // retorna error
    }

    int i = 0;                       // índice de memoria a llenar
    char line[256];                  // buffer para leer cada línea del archivo

    // Bucle principal para leer líneas mientras haya espacio en la memoria
    while (i < MEM_SIZE && fgets(line, sizeof(line), f)) {

        // Apunta a p donde inicia la línea (para poder mover el cursor)
        char *p = line;

        // Quitar espacios en blanco al principio (indentación, tabs, etc.)
        while (*p && isspace((unsigned char)*p)) p++;

        // Saltar líneas vacías o comentarios que inician con ';'
        if (*p == ';' || *p == '\0' || *p == '\n' || *p == '\r')
            continue;

        // Quitar espacios en blanco del final de la línea
        char *end = p + strlen(p) - 1;
        while (end >= p && isspace((unsigned char)*end)) {
            *end = '\0';
            --end;
        }

        // Si después de recortar queda vacía, saltar
        if (strlen(p) == 0)
            continue;

        // Detectar si la línea contiene solamente ceros y unos (binario)
        int only01 = 1;
        for (size_t k = 0; k < strlen(p); ++k) {
            char ch = p[k];

            // Ignorar espacios dentro del binario
            if (ch == ' ' || ch == '\t') continue;

            // Si encuentra algo que no sea 0 o 1 → no es binario
            if (ch != '0' && ch != '1') {
                only01 = 0;
                break;
            }
        }

        int val = 0; // valor numérico final de la instrucción/letra del archivo

        if (only01) {
            // Si es una línea de puro 0/1, la interpretamos como binario
            char tmp[64];
            int pos = 0;

            // Copiar solamente los dígitos 0 o 1 a tmp
            for (size_t k = 0; k < strlen(p) && pos < (int)sizeof(tmp)-1; ++k) {
                if (p[k] == '0' || p[k] == '1')
                    tmp[pos++] = p[k];
            }
            tmp[pos] = '\0';

            // Convertir la cadena binaria a número entero (val)
            val = 0;
            for (int k = 0; tmp[k]; ++k) {
                val = (val << 1) + (tmp[k] - '0');   // shift left y agregar bit
            }

        } else {
            /*
             * Si no es binario, puede ser:
             *   - un decimal (ej. 42)
             *   - un hexadecimal (ej. 0x1F)
             */
            if (strlen(p) > 2 && p[0]=='0' && (p[1]=='x' || p[1]=='X')) {
                // Interpretar como hexadecimal
                val = (int)strtol(p, NULL, 16);
            } else {
                // Interpretar como decimal
                val = atoi(p);
            }
        }

        // Guardar el valor dentro de la memoria como un byte (0–255)
        m->data[i++] = (uint8_t)(val & 0xFF);
    }

    fclose(f);

    return i; /* bytes cargados */
}

/*
 * Cargar un programa de ejemplo si el usuario no carga un archivo .mem
 * Este programa:
 *   - guarda 30 en memoria[30]
 *   - LOADI 30 → A = 30
 *   - STORE 200 → guardar A en memoria[200]
 *   - LOADI 20 → A = 20
 *   - ADD 200 → A = A + memoria[200]
 *   - HALT → detener ejecución
 */
void cargar_programa_ejemplo(Memoria *m) {

    // Variables de ejemplo
    m->data[100] = 30;   // variable N
    m->data[101] = 20;   // variable contador

    // Programa de instrucciones:
    m->data[0] = 5;  m->data[1] = 30;   // LOADI 30
    m->data[2] = 2;  m->data[3] = 200;  // STORE en dirección 200
    m->data[4] = 5;  m->data[5] = 20;   // LOADI 20
    m->data[6] = 3;  m->data[7] = 200;  // ADD memoria[200]
    m->data[8] = 8;                      // HALT
}

/*
 * main()
 * ------
 * Inicializa la memoria, carga un programa (archivo o ejemplo), crea la CPU
 * y la ejecuta. Finalmente muestra estado y variables.
 */
int main(int argc, char *argv[]) {

    Memoria mem;              // Crea la estructura de memoria
    memoria_init(&mem);       // Limpia memoria (probablemente a 0)

    int bytes_loaded = 0;
    clock_t t0 = clock();

    if (argc > 1) {
        // Si el usuario pasó un archivo .mem como argumento, se carga
        bytes_loaded = cargar_memoria_desde_archivo(&mem, argv[1]);
        if (bytes_loaded < 0) {
            fprintf(stderr, "Error cargando %s\n", argv[1]);
            return 1;
        }
    } else {
        // Si no se pasó archivo → cargar programa de ejemplo
        printf("[AVISO] No se proporcionó archivo .mem. Cargando ejemplo.\n");
        cargar_programa_ejemplo(&mem);
        bytes_loaded = 9; /* conocido en el ejemplo */
    }

    clock_t t1 = clock();
    double load_time = (double)(t1 - t0) / CLOCKS_PER_SEC;
    printf("[INFO] Memoria cargada: %d bytes\n", bytes_loaded);
    printf("[METRIC] Tiempo carga .mem: %.6f s\n", load_time);

    // Inicializar la CPU con esa memoria
    CPU cpu;
    cpu_init(&cpu, &mem);

    // Ejecutar instrucciones hasta HALT
    cpu_ejecutar(&cpu);

    // Mostrar estado final (cpu_ejecutar ya imprime estado y métricas CPU)

    // Mostrar valores importantes en memoria
    printf("\n--- Variables principales en memoria ---\n");
    printf("N (MEM[100]) = %d\n", mem.data[100]);
    printf("contador (MEM[101]) = %d\n", mem.data[101]);
    printf("resultado (MEM[200]) = %d\n", mem.data[200]);

    return 0;
}
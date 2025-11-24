#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    printf("\n===============================\n");
    printf("  SISTEMA VON NEUMANN INTEGRADO\n");
    printf("===============================\n\n");

    clock_t t_start = clock();

    printf("[1] Traduciendo C → ASM...\n");
    clock_t t0 = clock();
    int ret = system("./build/c_to_asm ejemplos/factorial.c");
    clock_t t1 = clock();
    double t_c_to_asm = (double)(t1 - t0) / CLOCKS_PER_SEC;
    if (ret != 0) {
        fprintf(stderr, "[ERROR] c_to_asm devolvió %d\n", ret);
        return 1;
    }

    /* mover archivo generado (c_to_asm crea factorial.asm en cwd) */
    system("mv -f factorial.asm build/factorial.asm");

    printf("[2] Ensamblando ASM → MEM...\n");
    clock_t t2 = clock();
    ret = system("./build/assembler build/factorial.asm build/factorial.mem");
    clock_t t3 = clock();
    double t_asm_to_mem = (double)(t3 - t2) / CLOCKS_PER_SEC;
    if (ret != 0) {
        fprintf(stderr, "[ERROR] assembler devolvió %d\n", ret);
        return 1;
    }

    printf("[3] Ejecutando simulador de CPU...\n");
    clock_t t4 = clock();
    ret = system("./build/cpu_simulator build/factorial.mem");
    clock_t t5 = clock();
    double t_cpu = (double)(t5 - t4) / CLOCKS_PER_SEC;
    if (ret != 0) {
        fprintf(stderr, "[ERROR] cpu_simulator devolvió %d\n", ret);
        return 1;
    }

    clock_t t_end = clock();
    double t_total = (double)(t_end - t_start) / CLOCKS_PER_SEC;

    printf("\n=== MÉTRICAS DEL PIPELINE ===\n");
    printf("Tiempo C -> ASM : %.6f s\n", t_c_to_asm);
    printf("Tiempo ASM -> MEM: %.6f s\n", t_asm_to_mem);
    printf("Tiempo CPU: %.6f s\n", t_cpu);
    printf("Tiempo total pipeline (cliente): %.6f s\n", t_total);

    printf("\n=== PROGRAMA FINALIZADO ===\n");
    return 0;
}
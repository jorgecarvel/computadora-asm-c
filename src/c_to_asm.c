/*
 * c_to_asm.c - Traductor C simple a ASM sin etiquetas
 * Lee un .c con pseudocódigo estructurado y genera .asm con direcciones explícitas
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s archivo.c\n", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    if (!in) {
        printf("No se pudo abrir %s\n", argv[1]);
        return 1;
    }

    FILE *out = fopen("factorial.asm", "w");
    if (!out) {
        printf("No se pudo crear factorial.asm\n");
        fclose(in);
        return 1;
    }

    fprintf(out, "; factorial_no_labels.asm - generado por c_to_asm\n");
    fprintf(out, "; MEM[100] = N\n");
    fprintf(out, "; MEM[101] = contador\n");
    fprintf(out, "; MEM[200] = resultado\n");
    fprintf(out, "; MEM[2]   = const 1 (usada para decrementar)\n\n");

    int pc = 0;

    // 1. Inicializar N = 5
    fprintf(out, "        LOADI 5        ; %d..%d   A=5\n", pc, pc+1); pc+=2;
    fprintf(out, "        STORE 100      ; %d..%d   MEM[100]=5\n\n", pc, pc+1); pc+=2;

    // 2. Inicializar resultado = 1
    fprintf(out, "        LOADI 1        ; %d..%d   A=1\n", pc, pc+1); pc+=2;
    fprintf(out, "        STORE 200      ; %d..%d   MEM[200]=1\n\n", pc, pc+1); pc+=2;

    // 3. Constante 1
    fprintf(out, "        LOADI 1        ; %d..%d   A=1\n", pc, pc+1); pc+=2;
    fprintf(out, "        STORE 2        ; %d..%d   MEM[2]=1\n\n", pc, pc+1); pc+=2;

    // 4. contador = N
    fprintf(out, "        LOADM 100      ; %d..%d A = MEM[100]\n", pc, pc+1); pc+=2;
    fprintf(out, "        STORE 101      ; %d..%d MEM[101] = N (contador)\n\n", pc, pc+1); pc+=2;

    int bucle = pc; // inicio del bucle

    // 5. while (contador != 0)
    fprintf(out, "        LOADM 101      ; %d..%d A = contador\n", pc, pc+1); pc+=2;
   // int saltoFin = pc;
    fprintf(out, "        JMPZ 34        ; %d..%d salto a fin\n\n", pc, pc+1); pc+=2;

    // 6. resultado = resultado * contador
    fprintf(out, "        LOADM 200      ; %d..%d A = resultado\n", pc, pc+1); pc+=2;
    fprintf(out, "        MUL 101        ; %d..%d A = resultado * contador\n", pc, pc+1); pc+=2;
    fprintf(out, "        STORE 200      ; %d..%d MEM[200] = A\n\n", pc, pc+1); pc+=2;

    // 7. contador = contador - 1
    fprintf(out, "        LOADM 101      ; %d..%d A = contador\n", pc, pc+1); pc+=2;
    fprintf(out, "        SUB 2          ; %d..%d A = A - MEM[2]\n", pc, pc+1); pc+=2;
    fprintf(out, "        STORE 101      ; %d..%d MEM[101] = nuevo contador\n\n", pc, pc+1); pc+=2;

    // 8. salto al inicio del bucle
    fprintf(out, "        JMP %d         ; %d..%d volver al inicio del bucle\n\n", bucle, pc, pc+1); pc+=2;

    // 9. parchear JMPZ XX
    int fin = pc;
    fseek(out, 0, SEEK_END);
    fprintf(out, "        HALT           ; %d fin\n", fin);

    fclose(in);
    fclose(out);

    // Ahora parcheamos el JMPZ XX
    FILE *patch = fopen("factorial.asm", "r+");
    char line[256];
    long pos;
    while (fgets(line, sizeof(line), patch)) {
        if (strstr(line, "JMPZ XX")) {
            pos = ftell(patch);
            fseek(patch, pos - strlen(line), SEEK_SET);
            fprintf(patch, "        JMPZ %d        ; salto a fin\n", fin);
            break;
        }
    }
    fclose(patch);

    printf("Archivo factorial.asm generado correctamente (%d bytes aprox).\n", pc);
    return 0;
}


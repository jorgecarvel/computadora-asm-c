/*
 * assembler.c
 * Ensamblador de dos pasadas: acepta etiquetas y genera .mem con bytes en BINARIO (8 bits por línea)
 *
 * Uso:
 *   ./assembler entrada.asm salida.mem
 *
 * Concepto general:
 * - Primera pasada:
 *       Recorre el archivo ASM, detecta etiquetas y calcula la dirección (address)
 *       que ocupa cada instrucción. No genera código todavía.
 *
 * - Segunda pasada:
 *       Recorre nuevamente todas las líneas "pendientes", ahora sí genera los opcodes
 *       y resuelve etiquetas a direcciones reales.
 *
 * Notas:
 * - Soporta mnemónicos: NOP, STORE, ADD, SUB, LOADI, LOADM/LOAD, JMP, HALT,
 *   PUSH, POP, CALL, RET, JMPZ, MUL
 * - Etiquetas terminan con ':' (p. ej. loop:)
 * - Los operandos pueden ser números decimales, 0xHEX, 0bBINARIO o etiquetas.
 * - Salida: cada byte escrito como 8 caracteres '0'/'1' por línea.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 512      // Longitud máxima por línea
#define MAX_LABELS 512    // Número máximo de etiquetas
#define MAX_PENDING 2048  // Número máximo de líneas a procesar en la segunda pasada

/* ------------------------- Estructura para etiquetas ------------------------- */
typedef struct {
    char name[128];   // nombre de la etiqueta
    int address;      // dirección donde aparece
} Label;

static Label labels[MAX_LABELS];
static int label_count = 0;

/* ----------- Estructura para almacenar líneas pendientes para 2da pasada ------ */
typedef struct {
    char line[MAX_LINE]; // texto original de la línea
    int address;         // dirección calculada en primera pasada
    int lineno;          // número de línea para mensajes de error
} PendingLine;

static PendingLine pending[MAX_PENDING];
static int pending_count = 0;

/* ------------------------------ trim(): limpia espacios ----------------------- */
void trim(char *s) {
    char *p = s;
    while (*p && isspace((unsigned char)*p)) p++;      // quitar espacios iniciales
    if (p != s) memmove(s, p, strlen(p)+1);

    int len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len-1])) // quitar espacios finales
        s[--len] = '\0';
}

/* ------------------- parse_number(): detecta decimal, hex o binario ------------ */
int parse_number(const char *tok) {
    if (!tok) return -1;

    // Hexadecimal: 0x??
    if (strlen(tok) > 2 && tok[0]=='0' && (tok[1]=='x' || tok[1]=='X'))
        return (int)strtol(tok, NULL, 16);

    // Binario: 0b??
    if (strlen(tok) > 2 && tok[0]=='0' && (tok[1]=='b' || tok[1]=='B'))
        return (int)strtol(tok+2, NULL, 2);

    // Decimal
    if ((tok[0] == '-') || isdigit((unsigned char)tok[0]))
        return atoi(tok);

    return -1; // Si no es número, probablemente es etiqueta
}

/* ----------------------------- Buscar etiqueta -------------------------------- */
int find_label(const char *name) {
    for (int i = 0; i < label_count; ++i)
        if (strcmp(labels[i].name, name) == 0)
            return labels[i].address;
    return -1;
}

/* ---------------------- Tamaño de instrucción según mnemónico ------------------ */
int instr_size(const char *mnem_upper) {
    // Instrucciones sin operando
    if (strcmp(mnem_upper, "NOP")==0 || strcmp(mnem_upper,"HALT")==0 ||
        strcmp(mnem_upper,"PUSH")==0 || strcmp(mnem_upper,"POP")==0 ||
        strcmp(mnem_upper,"RET")==0)
        return 1;

    // Todas las demás ocupan 2 bytes: opcode + operando
    return 2;
}

/* -------------------------- PRIMERA PASADA -----------------------------------
 * Lee el ASM línea por línea
 * - Elimina comentarios
 * - Detecta etiquetas
 * - Calcula dirección de cada instrucción
 * - Guarda líneas que serán procesadas en segunda pasada
 */
void primera_pasada(const char *infile) {
    FILE *f = fopen(infile, "r");
    if (!f) { perror("fopen entrada"); exit(1); }

    char buf[MAX_LINE];
    int address = 0;
    int lineno = 0;

    while (fgets(buf, sizeof(buf), f)) {
        lineno++;

        // Quitar comentario comenzando en ';'
        char *c = strchr(buf, ';');
        if (c) *c = '\0';

        trim(buf);
        if (strlen(buf) == 0) continue; // línea vacía

        // ------------------- Si es etiqueta -------------------
        size_t L = strlen(buf);
        if (L > 0 && buf[L-1] == ':') {
            buf[L-1] = '\0';
            trim(buf);

            if (label_count >= MAX_LABELS) {
                fprintf(stderr,"Too many labels\n");
                exit(1);
            }

            strncpy(labels[label_count].name, buf, sizeof(labels[label_count].name)-1);
            labels[label_count].address = address;
            label_count++;
            continue; // no consume dirección
        }

        // ---------------- Guardar línea para 2da pasada ----------------
        if (pending_count >= MAX_PENDING) {
            fprintf(stderr,"Too many lines\n");
            exit(1);
        }
        strncpy(pending[pending_count].line, buf, MAX_LINE-1);
        pending[pending_count].address = address;
        pending[pending_count].lineno = lineno;
        pending_count++;

        // Detectar mnemónico y sumar tamaño
        char tmp[MAX_LINE];
        strncpy(tmp, buf, sizeof(tmp)-1);

        char *tok = strtok(tmp, " \t,");
        if (!tok) continue;

        char mnem[64];
        size_t i;
        for (i=0;i<sizeof(mnem)-1 && tok[i]; ++i)
            mnem[i] = toupper((unsigned char)tok[i]);
        mnem[i] = '\0';

        address += instr_size(mnem);
    }

    fclose(f);
}

/* ------------------- Escribir un byte como "00101101" ------------------------- */
void bin_write_byte(FILE *fout, int val) {
    unsigned int u = (unsigned int)val & 0xFFu;
    char s[9];
    s[8]='\0';

    for (int i=7;i>=0;--i) {
        s[i] = (u & 1u) ? '1' : '0';
        u >>= 1;
    }

    fprintf(fout, "%s\n", s);
}

/* -------------------------- SEGUNDA PASADA -----------------------------------
 * Ahora sí generamos los opcodes y operandos finales.
 * Aquí se resuelven etiquetas usando find_label()
 */
void segunda_pasada(const char *outfile) {
    FILE *fout = fopen(outfile, "w");
    if (!fout) { perror("fopen salida"); exit(1); }

    for (int p = 0; p < pending_count; ++p) {

        char line[MAX_LINE];
        strncpy(line, pending[p].line, MAX_LINE-1);

        char *tok = strtok(line, " \t,");
        if (!tok) continue;

        char mnem[64];
        size_t j;
        for (j=0; j<sizeof(mnem)-1 && tok[j]; ++j)
            mnem[j] = toupper((unsigned char)tok[j]);
        mnem[j] = '\0';

        /* --------- Cada instrucción genera uno o dos bytes --------- */

        if (strcmp(mnem, "NOP")==0) {
            bin_write_byte(fout, 1);
        }

        else if (strcmp(mnem, "STORE")==0) {
            char *op = strtok(NULL, " \t,");
            int addr = parse_number(op);
            if (addr < 0) addr = find_label(op);
            bin_write_byte(fout, 2);
            bin_write_byte(fout, addr);
        }

        else if (strcmp(mnem, "ADD")==0) {
            char *op = strtok(NULL, " \t,");
            int addr = parse_number(op);
            if (addr < 0) addr = find_label(op);
            bin_write_byte(fout, 3);
            bin_write_byte(fout, addr);
        }

        else if (strcmp(mnem, "SUB")==0) {
            char *op = strtok(NULL, " \t,");
            int addr = parse_number(op);
            if (addr < 0) addr = find_label(op);
            bin_write_byte(fout, 4);
            bin_write_byte(fout, addr);
        }

        else if (strcmp(mnem, "LOADI")==0 || strcmp(mnem, "LOADA")==0) {
            char *op = strtok(NULL, " \t,");
            int imm = parse_number(op);
            bin_write_byte(fout, 5);
            bin_write_byte(fout, imm);
        }

        else if (strcmp(mnem, "LOADM")==0 || strcmp(mnem, "LOAD")==0) {
            char *op = strtok(NULL, " \t,");
            int addr = parse_number(op);
            if (addr < 0) addr = find_label(op);
            bin_write_byte(fout, 6);
            bin_write_byte(fout, addr);
        }

        else if (strcmp(mnem, "JMP")==0) {
            char *op = strtok(NULL, " \t,");
            int addr = parse_number(op);
            if (addr < 0) addr = find_label(op);
            bin_write_byte(fout, 7);
            bin_write_byte(fout, addr);
        }

        else if (strcmp(mnem, "HALT")==0) {
            bin_write_byte(fout, 8);
        }

        else if (strcmp(mnem, "PUSH")==0) {
            bin_write_byte(fout, 9);
        }

        else if (strcmp(mnem, "POP")==0) {
            bin_write_byte(fout, 10);
        }

        else if (strcmp(mnem, "CALL")==0) {
            char *op = strtok(NULL, " \t,");
            int addr = parse_number(op);
            if (addr < 0) addr = find_label(op);
            bin_write_byte(fout, 11);
            bin_write_byte(fout, addr);
        }

        else if (strcmp(mnem, "RET")==0) {
            bin_write_byte(fout, 12);
        }

        else if (strcmp(mnem, "JMPZ")==0) {
            char *op = strtok(NULL, " \t,");
            int addr = parse_number(op);
            if (addr < 0) addr = find_label(op);
            bin_write_byte(fout, 13);
            bin_write_byte(fout, addr);
        }

        else if (strcmp(mnem, "MUL")==0) {
            char *op = strtok(NULL, " \t,");
            int addr = parse_number(op);
            if (addr < 0) addr = find_label(op);
            bin_write_byte(fout, 14);
            bin_write_byte(fout, addr);
        }

        else {
            fprintf(stderr, "Instrucción desconocida en linea %d: %s\n",
                    pending[p].lineno, tok);
            fclose(fout);
            exit(1);
        }
    }

    fclose(fout);
}

/* ------------------------------- main() -------------------------------------- */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s entrada.asm salida.mem\n", argv[0]);
        return 1;
    }

    // Reset de contadores
    label_count = 0;
    pending_count = 0;

    primera_pasada(argv[1]);   // Detecta etiquetas
    segunda_pasada(argv[2]);   // Genera .mem final

    printf("Ensamblado completado -> %s (formato: binario 8 bits por linea)\n",
        argv[2]);

    return 0;
}

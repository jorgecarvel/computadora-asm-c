// factorial.c - código C fuente simple para traducir con c_to_asm
int N = 5;
int contador;
int resultado = 1;
int uno = 1;

contador = N;

while (contador != 0) {
    resultado = resultado * contador;
    contador = contador - uno;
}

// Fin del programa


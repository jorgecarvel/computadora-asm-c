#include "memoria.h"
#include <string.h>

void memoria_init(Memoria *m) {
    memset(m->data, 0, MEM_SIZE);
}


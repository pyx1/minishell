#include "TElemento.h"
#include <string.h>

int igualPid (TElemento * e1, int pid) { //Devuelve 1 si son iguales
    if (e1->pid==pid) return 1;
    return 0;
    
}
int igual (TElemento e1, TElemento e2) { //Devuelve 1 si son iguales
    if (e1.pid==e2.pid) return 1;
    return 0;
}
void crear (int pid, char *n, int state, TElemento *nuevo) {
    nuevo->pid=pid;
    strcpy (nuevo->nombre, n);
    nuevo->state=state;
}
void asignar (TElemento *copia, TElemento original) {
    strcpy (copia->nombre, original.nombre);
    copia->pid = original.pid;
    copia->state = original.state;
}
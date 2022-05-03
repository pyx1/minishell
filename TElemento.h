#include <stdio.h>
#include <stdlib.h>
#define MAX 1024
struct TProceso {
    int pid;
    char nombre[MAX];
    int state; //0 running 1 stopped
};
typedef struct TProceso TElemento;

int igualPid(TElemento * e1, int pid);
int igual (TElemento e1, TElemento e2); //Herramienta para comparar Elementos ==
void crear (int pid, char *n, int state, TElemento *nuevo); //Herramienta para crear un TElemento mediante sus partes
void asignar (TElemento *copia, TElemento original); //Herramienta para sacar una fotocopia de un Elemento
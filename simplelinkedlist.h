#include <stdio.h>
#include <stdlib.h>
#include "TElemento.h"



struct Nodo{
    TElemento info;
    struct Nodo* sig;
};

typedef struct Nodo Tnodo;
typedef Tnodo* TLinkedList;

void crearVacia (TLinkedList* a);
void insertar (TElemento e, TLinkedList* a);
void insertarFinal (TElemento e, TLinkedList* a);
int eliminar (TElemento e, TLinkedList* a);
void primero (TLinkedList* a, TElemento* e);
void resto (TLinkedList* a);
void ultimo (TLinkedList* a, TElemento* e);
TElemento obtenerBusqueda(TLinkedList * a, int pid);
void obtenerBusquedaIndice(TLinkedList * a, int indice, TElemento * e);
void concatenar(TLinkedList* a1, TLinkedList* a2);
int contiene (TElemento e, TLinkedList* a);
int esVacia (TLinkedList* a);
int longitud (TLinkedList* a);

void copiar(TLinkedList* original, TLinkedList* copia);
void destruir(TLinkedList* a);
#include "simplelinkedlist.h"

void crearVacia (TLinkedList  *a) {
    *a = NULL;
}

void insertar (TElemento e, TLinkedList* a) {
    TLinkedList nuevo;
    nuevo = (TLinkedList ) malloc (1* sizeof (Tnodo));
    asignar(&(nuevo->info), e);
    nuevo->sig = *a;
    *a = nuevo;
}

void insertarFinal (TElemento e, TLinkedList* a) {
    TLinkedList nuevo;
    nuevo = (TLinkedList ) malloc (1 * sizeof (Tnodo));
    nuevo->info = e;
    nuevo->sig = NULL;

    if (*a==NULL) {
        *a = nuevo;
    }else {
        TLinkedList paux;
        paux = *a;
        while (paux->sig!=NULL) {
            paux = paux->sig;
        }
        paux->sig = nuevo;
    }


}

void primero (TLinkedList* a, TElemento* e) {
    if (*a!=NULL)
        asignar(e,(*a)->info);
}

void ultimo (TLinkedList* a, TElemento* e) {
    TLinkedList paux;
    if (*a!=NULL) {
        paux = *a;
        while (paux->sig) {
            paux = paux -> sig;
        }
        asignar(e,paux->info);
    }
}
TElemento obtenerBusqueda(TLinkedList * a, int pid){
    int encontrado = 0;
    TLinkedList paux = *a;
    while(encontrado == 0 && paux!=NULL){
        if(paux->info.pid == pid){
            encontrado=1;
        }
        else paux = paux->sig;
    }
    return paux->info;
}

void obtenerBusquedaIndice(TLinkedList * a, int indice, TElemento * e){
    int encontrado = 0;
    TLinkedList paux = *a;
    while(encontrado != indice && paux!=NULL){
        paux = paux->sig;
        encontrado++;
        
    }
    asignar(e, paux->info);
}

void restoV2(TLinkedList* a) {
    TLinkedList paux;
    if (*a != NULL) {
        paux = *a;
        paux = paux->sig;
        free(a);
        *a = paux;
    }
}


void resto (TLinkedList* a) {

    if (*a!=NULL) {
        TLinkedList p;
        p = *a;
        *a = (*a)->sig;
        free(p);
    }
}

int longitud (TLinkedList* a) {
    int cont=0;
    TLinkedList paux;
    paux = *a;
    while (paux!=NULL) {
        cont++;
        paux = paux->sig;
    }
    return cont;
}

int esVaciaV2 (TLinkedList* a) {
    if (*a==NULL) {
        return 1; //Verdad
    }else {
        return 0; //Mentira
    }
}
int esVacia (TLinkedList* a) {
    return (*a==NULL);
}
int contiene (TElemento e, TLinkedList* a) {
    int encontrado = 0;
    TLinkedList paux;
    paux = *a;
    while ((paux!=NULL) && (encontrado==0)) {
        encontrado = igual (paux->info, e); //TAD Elemento
        if (encontrado==0){
            paux = paux -> sig;
        }
    }
    return encontrado;
}

int eliminar (TElemento e, TLinkedList* a) {
    int encontrado = 0;
    TLinkedList pe, pant;
    pant=NULL;
    pe = *a;
    while ((pe!=NULL) && (encontrado==0)) {
        encontrado = igual (pe->info,e);
        if (encontrado==0){
            pant=pe;
            pe = pe -> sig;
        }
    }
    if (encontrado==1) {
        if (pant==NULL) { //Significa que no tienes que coser, porque borras el primero
            *a = pe->sig;
        }else {
            pant->sig = pe->sig; //Cosemos la lista
        }
        free (pe);
    }
    return encontrado;
}

void concatenar(TLinkedList* a1, TLinkedList* a2) {
    TLinkedList paux = *a2;
    while (paux!=NULL) {
        insertarFinal(paux->info, a1);
        paux= paux->sig;
    }
}

void copiar(TLinkedList* original, TLinkedList* copia) {
    *copia = NULL;
    TLinkedList paux;
    paux = *original;
    while (paux!=NULL){
        insertarFinal(paux->info, copia);
        paux = paux->sig;
    }
}
void destruir(TLinkedList* a) {
    while (!esVacia(a)) {
        resto (a);
    }
}

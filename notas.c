#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "tramo.h"
#include "decodificacion.h"
#include "procesamientos.h"
#include "notas.h"


#define MAX_NOTAS 127


struct encendidas{
    uint32_t *tiempo;           //Inicio de cada nota
    float *velocidad;            //Velocidades de cada nota
};


encendidas_t *encendidas_crear(){
    encendidas_t *s = malloc(sizeof(encendidas_t));
    if (s == NULL)
        return NULL;
    s->tiempo =  malloc(sizeof(encendidas_t) * MAX_NOTAS);
    if (s->tiempo == NULL){
        free(s);
        return NULL;
    }
    s->velocidad =  malloc(sizeof(encendidas_t) * MAX_NOTAS);
    if (s->velocidad == NULL){
        free(s->tiempo);
        free(s);
        return NULL;
    }


    return s;
}


bool almacenar_notas(encendidas_t *v, nota_t nota, signed char octava, uint32_t tiempo, float velocidad){
    
    int n = (octava + 1) * 12 + nota;

    v->tiempo[n] = tiempo;
    v->velocidad[n] = velocidad;


    return true;
}

bool nota_finalizada(encendidas_t *v, nota_t nota, signed char octava, uint32_t *inicio, float *frecuencia, float *velocidad){

    float potencia = ((float)octava + nota/12.0) - (4.0 + 9/12.0);
    int n = (octava + 1) * 12 + nota;

    *frecuencia = 440.0 * exp2(potencia);
    *inicio = v->tiempo[n];
    *velocidad = v->velocidad[n];
    
    return true;
}


void free_notas_encendidas(encendidas_t *v){
    
    if(v->tiempo != NULL)
        free(v->tiempo);

    if(v->velocidad != NULL)
        free(v->velocidad);
        
    if (v != NULL)
        free(v);
 
}
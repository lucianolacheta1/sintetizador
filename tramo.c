#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "sintetizador.h"
#include "tramo.h"

struct tramo {
    float *v;
    size_t n;
    double t0;
    int f_m;
};

struct tint16 {
    int16_t *v;
    size_t n;
};

struct funcion {
    char *nombre;
    float params[3];
};

struct diccionario {
    char *funcion;
    modulacion_t modulacion;
};

static const dicc_t dicc[] = {
    {"CONSTANT",modulacion_constante},
    {"LINEAR",modulacion_lineal},
    {"INVLINEAR",modulacion_invlinear},
    {"SIN",modulacion_seno},
    {"EXP",modulacion_exp},
    {"INVEXP",modulacion_invexp},
    {"QUARTCOS",modulacion_quartcos},
    {"QUARTSIN",modulacion_quartsin},
    {"HALFCOS",modulacion_halfcos},
    {"HALFSIN",modulacion_halfsin},
    {"LOG",modulacion_log},
    {"INVLOG",modulacion_invlog},
    {"TRI",modulacion_tri},
    {"PULSES",modulacion_pulses}
};


void inicializar_muestras(float v[], size_t n){
    for (size_t i = 0; i < n; i++)
        v[i]= 0;
}

void imprimir_muestras(const float v[], size_t n, double t0, int f_m){
    double ti;
    for (size_t i = 0; i < n; i++){
        ti = t0 + i/(double)f_m;
        printf("%f , %f\n",ti , v[i]);
        if(v[i] == 0 && v[i-1] == 0 && v[i-2] == 0)
            break;
    }
}

void imprimir_mayor(const float v[], size_t n){
    double taux = v[0];
    for (size_t i = 0; i < n; i++){
        if(v[i] > taux)
            taux = v[i];
        //printf("%f\n", v[i]);
    }
    printf("%f\n", taux);
}

void imprimir_mayor_int16(const int16_t v[], size_t n){
    uint16_t taux = v[0];
    for (size_t i = 0; i < n; i++){
        if(v[i] > taux)
            taux = v[i];
        //printf("%d\n", v[i]);
    }
    printf("%d\n", taux);
}


void muestrear_senoidal(float v[], size_t n, double t0, int f_m, float f, float a){
    double ti;
    for (size_t i = 0; i < n; i++){
        
        ti = t0 + i/(double)f_m;
        v[i] += a*sin(2*PI*f*ti);
    
    }

}

void muestrear_armonicos(float v[], size_t n, double t0, int f_m, float f, float a, float fa[][2], size_t n_fa){

    inicializar_muestras(v,n);
    size_t i;
    for (i = 0; i < n_fa; i++){
    
        muestrear_senoidal(v,n,t0,f_m,(f*fa[i][0]),(a*fa[i][1]));   
    }  
}


tramo_t *_tramo_crear(double t0, double tf, int f_m){
    
    tramo_t *tramo = malloc(sizeof(tramo_t));
    if(tramo == NULL){       
        return NULL;
    }
    size_t n = (tf - t0)* f_m + 1;
    tramo->v = malloc(sizeof(float) * n);
    if(tramo->v == NULL){
        tramo_destruir(tramo);        
        return NULL;
    }
    tramo->n = n;
    tramo->t0 = t0;
    tramo->f_m = f_m;    

    return tramo;
}

tint16_t *tramo_a_int16(tramo_t *t){
    
    tint16_t *ti = malloc(sizeof(tint16_t));
    if(ti == NULL)    
        return NULL;

    ti->v = malloc(sizeof(int16_t) * t->n);
    if(ti->v == NULL){
        free(ti);    
        return NULL;
    }

    ti->n = t->n;

    double tmax = 0;
    for (size_t i = 0; i < ti->n; i++){
        if(t->v[i] > tmax)
            tmax = t->v[i];
    }

    for (size_t i = 0; i < ti->n; i++){
        ti->v[i] *= 32767.0/tmax;
    }
    return ti;    
}

void tramo_destruir(tramo_t *t){
    if(t){
        if(t->v != NULL)
            free(t->v);
        
        free(t);  
    }
}

void destruir_tramo_int16(tint16_t *t){
    if(t){
        if(t->v != NULL)
            free(t->v);
        
        free(t);  
    }
}

double tiempo_final(double t0, size_t n, int f_m){
    return (t0 + (n - 1)/(double)f_m);
}

tramo_t *tramo_clonar(const tramo_t *t){
    double tf = tiempo_final(t->t0, t->n, t->f_m);
    tramo_t *t2 = _tramo_crear(t->t0,tf, t->f_m);
    
    if(t2 == NULL)
        return NULL;

    for (size_t i = 0; i < t->n; i++){
        t2->v[i] = t->v[i];
    };
    t2->n = t->n;
    t2->t0 = t->t0;
    t2->f_m = t->f_m;

    return t2;    
}



tramo_t *tramo_crear_muestreo(double t0, double tf, int f_m, float f, float a, float fa[][2], size_t n_fa, funcion_t *decaimiento){

    tramo_t *tramo = _tramo_crear(t0,tf + decaimiento->params[0],f_m);
    
    if(tramo == NULL)
        return NULL;

    muestrear_armonicos(tramo->v,tramo->n,tramo->t0, tramo->f_m,f,a,fa,n_fa);
    
    return tramo;
    
}

bool tramo_redimensionar(tramo_t *t, double tf){

    size_t n = (tf - t->t0)* t->f_m + 1;
    float *aux;
    if ((aux = realloc(t->v,n  * sizeof(float))) == NULL){
            return false;
    }
    t->v = aux;

    if (t->n < n){
        size_t i;
        for (i = t->n; i < n; i++){
            t->v[i] = 0;
        }
    }
    t->n = n;
    
    return true;
    
}

bool tramo_extender(tramo_t *destino, const tramo_t *extension){
    
    double tf = tiempo_final(destino->t0, destino->n, destino->f_m);
    double tf2 = tiempo_final(extension->t0, extension->n, extension->f_m);
    if(tramo_redimensionar(destino, max(tf,tf2)) == false)
        return false;

    size_t n = (extension->t0 - destino->t0)* destino->f_m;
    size_t aux = 0;
    for (size_t i = n; i < destino->n; i++){
        destino->v[i] += extension->v[aux];
        aux++;
        if (aux == extension->n)
            break;
    }
    return true;
}

void escribir_encabezado_wav(FILE *f, tint16_t *t, uint32_t f_muestreo){

    char riff[] = "RIFF";
    fwrite(riff,sizeof(char),4,f);
    uint32_t tamagno = 36 + 2 * t->n;
    fwrite(&tamagno,sizeof(uint32_t),1,f);
    char wav[] = "WAVE";
    fwrite(wav,sizeof(char),4,f);
    char fmt[] = "fmt ";
    fwrite(fmt,sizeof(char),4,f);
    tamagno = 16;
    fwrite(&tamagno,sizeof(uint32_t),1,f);
    uint16_t aux2 = 1;
    fwrite(&aux2,sizeof(uint16_t),1,f);
    fwrite(&aux2,sizeof(uint16_t),1,f);
    fwrite(&f_muestreo,sizeof(uint32_t),1,f);
    tamagno = 2 * f_muestreo;
    fwrite(&tamagno,sizeof(uint32_t),1,f);
    aux2 = 2;
    fwrite(&aux2,sizeof(uint16_t),1,f);
    aux2 = 16;
    fwrite(&aux2,sizeof(uint16_t),1,f);
    char data[] = "data";
    fwrite(data,sizeof(char),4,f);
    tamagno = 2 * t->n;
    fwrite(&tamagno,sizeof(uint32_t),1,f);

}

void escribir_tramo_wav(FILE *f, tint16_t *t){

    for (size_t i = 0; i < t->n; i++){
        fwrite(&t->v[i],sizeof(int16_t),1,f);
    }
}

funcion_t *extraer_modulacion(FILE *f){
    
    char s[MAX_STR];
    fgets(s,MAX_STR,f);
    funcion_t *funcion = malloc(sizeof(funcion_t));
    if(funcion == NULL)
        return NULL;

    size_t pos_esp = 0;
    
    while(s[pos_esp] != ' ' && s[pos_esp] != '\n' && s[pos_esp] != '\0')
        pos_esp++;
    
    funcion->nombre = malloc(sizeof(char)* pos_esp + 1);
    if(funcion->nombre == NULL){
        free(funcion);
        return NULL;
    }

    strncpy(funcion->nombre,s,pos_esp);
    funcion->nombre[pos_esp] = '\0';

    size_t i;
    for (i = 0; i < sizeof(nombre_funciones_)/sizeof(nombre_funciones_[0]); i++){
        if (!strcmp(funcion->nombre,nombre_funciones_[i]))
            break;        
    }
    if (i == sizeof(nombre_funciones_)/sizeof(nombre_funciones_[0])){
        free(funcion->nombre);
        free(funcion);
        return NULL;
    }

    char *pend = &s[pos_esp];
    char *pend2;
   
    funcion->params[0] = strtof(pend,&pend2);

    funcion->params[1] = strtof(pend2,&pend);

    funcion->params[2] = strtof(pend,NULL);

    return funcion;

}


void tramo_modular(tramo_t *tramo, double tf, funcion_t *ataque, funcion_t *sostenido, funcion_t *decaimiento){
    
    modulacion_t accion;

    for(size_t i = 0; i < sizeof(dicc) / sizeof(dicc[0]); i++) {
        if(!strcmp(dicc[i].funcion,ataque->nombre)){
            accion = dicc[i].modulacion;
            break;
        }
    }

    size_t aux = (ataque->params[0])* tramo->f_m + 1;
    
    size_t j;
    double ti;
    double ataq;
    for (j = 0; j < aux; j++)
    {
        ti = tramo->t0 + j/(double)tramo->f_m;
        if(ti>tramo->t0 && ti<tramo->t0 + ataque->params[0])
            ataq = accion(ti - tramo->t0,ataque->params);
        
        tramo->v[j] *= ataq;
    }

    for(size_t i = 0; i < sizeof(dicc) / sizeof(dicc[0]); i++) {
        if(!strcmp(dicc[i].funcion,sostenido->nombre)){
            accion = dicc[i].modulacion;
            break;
        }
    }

    aux = (tf - tramo->t0)* tramo->f_m + 1;
    double sost;
    for (size_t i = j; i < aux; i++)
    {
        ti = tramo->t0 + i/(double)tramo->f_m;
        if(ti>tramo->t0+ataque->params[0] && ti<tf)
            sost = accion(ti-(tramo->t0 + sostenido->params[0]),sostenido->params);
        
        
        tramo->v[i] *= sost;
    }

    modulacion_t accion2;
    for(size_t i = 0; i < sizeof(dicc) / sizeof(dicc[0]); i++) {
        if(!strcmp(dicc[i].funcion,decaimiento->nombre)){
            accion2 = dicc[i].modulacion;
            break;
        }
    }
    
    double decaim;
    for (size_t i = aux; i < tramo->n; i++)
    {
        ti = tramo->t0 + i/(double)tramo->f_m;
        if(ti>tf && ti<tf + decaimiento->params[0])
            decaim = accion(tf,sostenido->params) * accion2(ti-tf,decaimiento->params);
        tramo->v[i] *= decaim;
    }
}

void free_funcion_t(funcion_t *f){

    if(f->nombre != NULL)
        free(f->nombre);

    if (f != NULL){
        free(f);
    }
    return;
}


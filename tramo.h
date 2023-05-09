#ifndef TRAMO_H
#define TRAMO_H


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "sintetizador.h"

#define PI 3.141592653589793115997963468544185161590576171875

typedef struct tramo tramo_t;

typedef struct tint16 tint16_t;

typedef struct funcion funcion_t;

typedef float (*modulacion_t)(double,float[]);

typedef struct diccionario dicc_t;

void inicializar_muestras(float v[], size_t n);
void imprimir_mayor(const float *v, size_t n);
void imprimir_mayor_int16(const int16_t v[], size_t n);
void muestrear_senoidal(float v[], size_t n, double t0, int f_m, float f, float a);
void imprimir_muestras(const float v[], size_t n, double t0, int f_m);
void muestrear_armonicos(float v[], size_t n, double t0, int f_m, float f, float a, float fa[][2], size_t n_fa);
void tramo_destruir(tramo_t *t);

tramo_t *_tramo_crear(double t0, double tf, int f_m);

tint16_t *tint16_t_crear();

tint16_t *tramo_a_int16(tramo_t *t);

void tramo_destruir(tramo_t *t);

void destruir_tramo_int16(tint16_t *t);

double tiempo_final(double t0, size_t n, int f_m);

tramo_t *tramo_clonar(const tramo_t *t);

tramo_t *tramo_crear_muestreo(double t0, double tf, int f_m, float f, float a, float fa[][2], size_t n_fa, funcion_t *decaimiento);  
 
bool tramo_redimensionar(tramo_t *t, double tf);

double max (double a, double b);

bool tramo_extender(tramo_t *destino, const tramo_t *extension);

void escribir_encabezado_wav(FILE *f, tint16_t *t, uint32_t f_muestreo);

void escribir_tramo_wav(FILE *f, tint16_t *t);

funcion_t *extraer_modulacion(FILE *f);

void tramo_modular(tramo_t *tramo, double tf, funcion_t *ataque, funcion_t *sostenido, funcion_t *decaimiento);

void free_funcion_t(funcion_t *f);

#endif
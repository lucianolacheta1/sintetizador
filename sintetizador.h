#ifndef SINTETIZADOR_H
#define SINTETIZADOR_H

#include <math.h>

#include "decodificacion.h"
#include "tramo.h"

#define METAEVENTO_FIN_DE_PISTA 0x2F
#define EVENTO_MAX_LONG 10

#define MAX_STR 256

static const char *nombre_funciones_[] = {
    "CONSTANT","LINEAR","INVLINEAR","SIN","EXP","INVEXP","QUARTCOS","QUARTSIN","HALFCOS","HALFSIN","LOG","INVLOG","TRI","PULSES"
};

typedef enum{
    CONSTANT, LINEAR,INVLINEAR,SIN,EXP,INVEXP,QUARTCOS,QUARTSIN,HALFCOS,HALFSIN,LOG,INVLOG,TRI,PULSES
}funciones_t;


static const int parametros[] = {
    [CONSTANT] = 0,
    [LINEAR] = 1,
    [SIN] = 1,
    [EXP] = 2,
    [INVEXP] = 1,
    [QUARTCOS] = 1,
    [QUARTSIN] = 1,
    [HALFCOS] = 1,
    [HALFSIN] = 1,
    [LOG] = 1,
    [INVLOG] = 1,
    [TRI] = 3,
    [PULSES] = 3
};


double max (double a, double b);

double min (double a, double b);

float modulacion_constante(double t,float params[3]);

float modulacion_lineal(double t,float params[3]);

float modulacion_invlinear(double t,float params[3]);

float modulacion_seno(double t,float params[3]);

float modulacion_exp(double t,float params[3]);

float modulacion_invexp(double t,float params[3]);

float modulacion_quartcos(double t,float params[3]);

float modulacion_quartsin(double t,float params[3]);

float modulacion_halfcos(double t,float params[3]);

float modulacion_halfsin(double t,float params[3]);

float modulacion_log(double t,float params[3]);

float modulacion_invlog(double t,float params[3]);

float modulacion_tri(double t,float params[3]);

float modulacion_pulses(double t,float params[3]);

void extraer_armonicos(FILE *f, float par[], float fa[][2], char s[], size_t n);

void extraer_par(float par[], const char s[]);

funciones_t cadena_a_funcion(const char *s);

#endif
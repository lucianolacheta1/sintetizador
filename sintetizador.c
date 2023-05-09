#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "sintetizador.h"
#include "tramo.h"
#include "notas.h"


double max (double a, double b){
    if(a > b)
        return a;
    return b;
}

double min (double a, double b){
    if(a > b)
        return b;
    return a;
}

float modulacion_constante(double t,float params[3]){
    return 1;
}

float modulacion_lineal(double t,float params[3]){
    return t/params[0];
}

float modulacion_invlinear(double t,float params[3]){
    return max(1 - (t/params[0]),0);
}

float modulacion_seno(double t,float params[3]){
    return 1 + (params[0]*sin(params[1] * t));
}

float modulacion_exp(double t,float params[3]){
    return exp((5*(t-params[0])/params[0]));
}

float modulacion_invexp(double t,float params[3]){
    return exp((-5*t)/params[0]);
}

float modulacion_quartcos(double t,float params[3]){
    return cos((PI*t)/(2*params[0]));
}

float modulacion_quartsin(double t,float params[3]){
    return sin((PI*t)/(2*params[0]));
}

float modulacion_halfcos(double t,float params[3]){
    return (1 + cos((PI*t)/(params[0])))/2;
}

float modulacion_halfsin(double t,float params[3]){
    return (1 + sin((PI*t)/(params[0])))/2;
}

float modulacion_log(double t,float params[3]){
    return log10(((9*t)/params[0]) +1);
}

float modulacion_invlog(double t,float params[3]){
    if(t >= params[0])
        return 0;
    
    return log10((((-9)*t)/params[0]) +10);
}

float modulacion_tri(double t,float params[3]){
    
    if(t<params[1]){
        return (t*params[2])/params[1];
    }else if(t==params[1]){
        return params[2];
    } else {
        return (t - params[1])/(params[1] - params[0]) * (params[2] -1) + params[2];
    }
}

float modulacion_pulses(double t,float params[3]){
    
    double t2 = (t/params[0] - floor(t /params[0])) * params[0];

    float a = (1 - params[2]) / params[1] * (t2 - params[0] + params[1]);
    
    if(a < 0)
        a = -a;

    return min(a + params[2], 1);
}


void extraer_par(float par[], const char s[]) {
    size_t pos_esp = 0;
    
    while(s[pos_esp] != ' ')
        pos_esp++;
    
    char aux[MAX_STR];
    
    pos_esp++;
    size_t i;
    for(i = 0; s[i + pos_esp]; i++)
        aux[i] = s[pos_esp + i];
    aux[i] = '\0';

    par[0] = atof(s);
    par[1] = atof(aux);

}

void extraer_armonicos(FILE *f, float par[], float fa[][2], char s[], size_t n){

    for (size_t i = 0; i < n; i++){
            fgets(s,MAX_STR,f);
            extraer_par(par,s);
            fa[i][0] = par[0];
            fa[i][1] = par[1];
    }

}


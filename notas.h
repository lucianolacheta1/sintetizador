#ifndef NOTAS_H
#define NOTAS_H

#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "tramo.h"
#include "decodificacion.h"
#include "procesamientos.h"

#define METAEVENTO_FIN_DE_PISTA 0x2F
#define EVENTO_MAX_LONG 10

enum {EVNOTA_NOTA, EVNOTA_VELOCIDAD};
enum {METAEVENTO_TIPO, METAEVENTO_LONGITUD};

#define MAX_NOTAS 127

typedef struct encendidas encendidas_t;

encendidas_t *encendidas_crear();

bool almacenar_notas(encendidas_t *v, nota_t nota, signed char octava, uint32_t tiempo, float velocidad);

bool nota_finalizada(encendidas_t *v, nota_t nota, signed char octava, uint32_t *inicio, float *frecuencia, float *velocidad);

void free_notas_encendidas(encendidas_t *v);

#endif
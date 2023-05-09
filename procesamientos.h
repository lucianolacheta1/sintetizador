#ifndef PROCESAMIENTOS_H
#define PROCESAMIENTOS_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "decodificacion.h"

#define ENCABEZADO_VALIDO 0x4D546864
#define PISTA_VALIDA 0x4D54726B
#define TIME_CONTINUE 0x80

uint8_t leer_uint8_t(FILE *f);

uint16_t leer_uint16_t(FILE *f);

uint32_t leer_uint32_t(FILE *f);

bool leer_encabezado(FILE *f, formato_t *formato, uint16_t *numero_pistas, uint16_t *pulso_negras);

bool leer_pista(FILE *f, uint32_t *tamagno);

bool leer_tiempo(FILE *f, uint32_t *tiempo);


bool leer_evento(FILE *f, evento_t *evento, char *canal, int *longitud, uint8_t mensaje[]);

void descartar_metaevento(FILE *f, uint8_t tamagno);

#endif
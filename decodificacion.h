#ifndef DECODIFICACION_H
#define DECODIFICACION_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>



typedef enum{
    PISTA_UNICA,
    MULTIPISTAS_SINCORNICAS,
    MULTIPISTAS_ASINCORNICAS
}formato_t;

typedef enum{
    NOTA_APAGADA,
    NOTA_ENCENDIDA,
    NOTA_DURANTE,
    CAMBIO_DE_CONTROL,
    CAMBIO_DE_PROGRAMA,
    VARIAR_CANAL,
    CAMBIO_DE_PITCH,
    METAEVENTO
}evento_t;

typedef enum{
    DO,
    DO_SOSTENIDO,
    RE,
    RE_SOSTENIDO,
    MI,
    FA,
    FA_SOSTENIDO,
    SOL,
    SOL_SOSTENIDO,
    LA,
    LA_SOSTENIDO,
    SI
}nota_t;


struct codificacion_t{
    char *cadena;
    int var;
};

bool decodificar_formato(uint16_t valor, formato_t *formato);

bool decodificar_evento(uint8_t valor, evento_t *evento, char *canal, int *longitud);

bool decodificar_nota(uint8_t valor, nota_t *nota, signed char *octava);

const char *codificar_formato(formato_t formato);

const char *codificar_evento(evento_t evento);

const char *codificar_nota(nota_t nota);


#endif











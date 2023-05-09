#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "decodificacion.h"

#define MASK_EVENTO 0X70
#define MASK_CANAL 0X0F
#define SHIFT_EVENTO 4
#define SHIFT_CANAL 0
#define MASK_EVENTO_VALIDO 0X80
#define MASK_NOTA_VALIDA 0X7F

static const int longitudes[] = {
    [NOTA_APAGADA] = 2,
    [NOTA_ENCENDIDA] = 2,
    [NOTA_DURANTE] = 2,
    [CAMBIO_DE_CONTROL] = 2,
    [CAMBIO_DE_PROGRAMA] = 1,
    [VARIAR_CANAL] = 1,
    [CAMBIO_DE_PITCH] = 2,
    [METAEVENTO] = 2
};

static const char *formato_a_cadena[] = {
    [PISTA_UNICA] = "Pista unica",
    [MULTIPISTAS_SINCORNICAS] = "Multipistas sincronicas",
    [MULTIPISTAS_ASINCORNICAS] = "Multipistas asincronicas"
};

static const char *evento_a_cadena[] = {
    [NOTA_APAGADA] = "Nota apagada",
    [NOTA_ENCENDIDA] = "Nota encendida",
    [NOTA_DURANTE] = "Nota durante",
    [CAMBIO_DE_CONTROL] = "Cambio de control",
    [CAMBIO_DE_PROGRAMA] = "Cambio de programa",
    [VARIAR_CANAL] = "Variar canal",
    [CAMBIO_DE_PITCH] = "Cambio de pitch",
    [METAEVENTO] = "Metaevento"
};

static const char *nota_a_cadena[] = {
    [DO] = "C",
    [DO_SOSTENIDO] = "C#",
    [RE] = "D",
    [RE_SOSTENIDO] = "D#",
    [MI] = "E",
    [FA] = "F",
    [FA_SOSTENIDO] = "F#",
    [SOL] = "G",
    [SOL_SOSTENIDO] = "G#",
    [LA] = "A",
    [LA_SOSTENIDO] = "A#",
    [SI] = "B"
};

bool decodificar_formato(uint16_t valor, formato_t *formato){ 
    if (valor > 2)
        return false;

    *formato = valor;
    return true;
}

bool decodificar_evento(uint8_t valor, evento_t *evento, char *canal, int *longitud){

    if (!(valor & MASK_EVENTO_VALIDO))
        return false;

    *evento = (valor & MASK_EVENTO) >> SHIFT_EVENTO;
    *canal = (valor & MASK_CANAL);
    *longitud = longitudes[(valor & MASK_EVENTO) >> SHIFT_EVENTO];
    return true;

}

bool decodificar_nota(uint8_t valor, nota_t *nota, signed char *octava){

    if(valor & ~MASK_NOTA_VALIDA)
        return false;

    *nota = valor % 12;
    *octava = (valor / 12) -1;

    return true;
}

const char *codificar_formato(formato_t formato){
    return formato_a_cadena[formato];
        
}

const char *codificar_evento(evento_t evento){
    return evento_a_cadena[evento];
        
}

const char *codificar_nota(nota_t nota){
    return nota_a_cadena[nota];
        
}
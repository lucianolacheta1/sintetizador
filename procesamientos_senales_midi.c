#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "decodificacion.h"
#include "procesamientos.h"
#include "tramo.h"


uint8_t leer_uint8_t(FILE *f){

    uint8_t s;
    fread(&s,sizeof(uint8_t),1,f);
    return s;
}

uint16_t leer_uint16_t(FILE *f){

    uint8_t v[2];
    fread(v,sizeof(uint8_t),2,f);
    uint16_t s = v[0] << 8 | v[1];
    
    return s;
}

uint32_t leer_uint32_t(FILE *f){

    uint8_t v[4];
    fread(v,sizeof(uint8_t),4,f);
    uint32_t s = v[0] << 24 | v[1] << 16 | v[2] << 8 | v[3];
    
    return s;
}

bool leer_encabezado(FILE *f, formato_t *formato, uint16_t *numero_pistas, uint16_t *pulso_negras){

    if(leer_uint32_t(f) != ENCABEZADO_VALIDO)
        return false;

    if(leer_uint32_t(f) != 6)
        return false;

    if(decodificar_formato(leer_uint16_t(f),formato) == false)
        return false;
    
    if(!(*numero_pistas = leer_uint16_t(f)))
        return false;

    if(!(*pulso_negras = leer_uint16_t(f)))
        return false;

    return true;

}

bool leer_pista(FILE *f, uint32_t *tamagno){

    if(leer_uint32_t(f) != PISTA_VALIDA)
        return false;

    if(!(*tamagno = leer_uint32_t(f)))
        return false;

    return true;

}

bool leer_tiempo(FILE *f, uint32_t *tiempo){

    uint8_t v[4];
    *tiempo = 0;
    size_t i = 0;
    
    while(1){
        
        if(i == 4)
            return false;
        
       v[i] = leer_uint8_t(f);
        
        if(!(v[i] & TIME_CONTINUE)){
            *tiempo |= v[i];
            break;         
        }
        
       
        
        v[i] &= ~TIME_CONTINUE;
        *tiempo |= v[i];
        *tiempo = *tiempo << 7;
        i++;
 
    }

    return true;

}

bool leer_evento(FILE *f, evento_t *evento, char *canal, int *longitud, uint8_t mensaje[]){

    uint8_t aux = leer_uint8_t(f);

    if(decodificar_evento(aux, evento, canal, longitud)){
        for (size_t i = 0; i < *longitud; i++){
            mensaje[i] = leer_uint8_t(f);
        }
        
    }else{
        mensaje[0] = aux;
        for (size_t i = 1; i < *longitud; i++){
            mensaje[i] = leer_uint8_t(f);
        }
        
    }
    
    return true;
}

void descartar_metaevento(FILE *f, uint8_t tamagno){

    uint8_t aux;
    for (size_t i = 0; i < tamagno; i++)
    {
        aux = leer_uint8_t(f);
    }
}

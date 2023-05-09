#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "tramo.h"
#include "decodificacion.h"
#include "procesamientos.h"
#include "notas.h"
#include "sintetizador.h"


int main( int argc, char *argv[]){

    if(argc < 6)
        return 1;
    
    int channel = 0;
    uint32_t f_muestreo = 44100;
    unsigned int pps = 120;
    tramo_t *tramo = NULL;
    tramo_t *tramo_aux = NULL;
    FILE *sintetizador;
    FILE *midi;
    FILE *wave;


    for (size_t i = 1; i < argc; i++){
        
        if(strcmp(argv[i],"-s") == 0){
            sintetizador = fopen(argv[i+1], "rt");
            if(sintetizador == NULL) {
                fprintf(stderr, "No se pudo abrir \"%s\"\n", argv[i+1]);
                fclose(midi);
                fclose(wave);
                return 1;
            }
        }
        if(strcmp(argv[i],"-i") == 0){    
            midi = fopen(argv[i+1], "rb");
            if(midi == NULL) {
                fprintf(stderr, "No se pudo abrir \"%s\"\n", argv[i+1]);
                fclose(sintetizador);
                fclose(wave);
                return 1;
            }
            
        }

        if(strcmp(argv[i],"-o") == 0){
            wave = fopen(argv[i+1], "wb");
            if(wave == NULL) {
                fprintf(stderr, "No se pudo abrir \"%s\"\n", argv[i+1]);
                fclose(midi);
                fclose(sintetizador);
                return 1;
            }
        }
        
        if(argc > 7){
            if(strcmp(argv[i],"-c") == 0){
                channel = atoi(argv[i+1]);
            }

            if(strcmp(argv[i],"-f") == 0){
                f_muestreo = atoi(argv[i+1]);
            }

            if(strcmp(argv[i],"-r") == 0){
                pps = atoi(argv[i+1]);
            }
        }
    }
///////////////////////////////////////////////////////////////////


    //Leo el archivo sintetizador.txt
    size_t cant_armonicos;
    char s[MAX_STR];
    float par[2];
    fgets(s,MAX_STR,sintetizador);
    cant_armonicos = strtol(s,NULL,10);
    
    float fa[cant_armonicos][2];

    extraer_armonicos(sintetizador,par,fa,s,cant_armonicos);

    encendidas_t *notas = encendidas_crear();
    if(notas == NULL){
        fprintf(stderr, "No se creo 'notas' correctamente\n");
        fclose(sintetizador);
        fclose(midi);
        fclose(wave);
        return 1;
    }

    //almaceno datos de ataque
    funcion_t *ataque = extraer_modulacion(sintetizador);
    if(ataque == NULL){
        fprintf(stderr, "No se obtuvo 'ataque' correctamente\n");
        fclose(sintetizador);
        fclose(midi);
        fclose(wave);
        free_notas_encendidas(notas);
        return 1;
    }
    //almaceno datos de sostenido
    funcion_t *sostenido = extraer_modulacion(sintetizador);
    if(sostenido == NULL){
        fprintf(stderr, "No se obtuvo 'sostenido' correctamente\n");
        free_funcion_t(ataque);
        free_funcion_t(sostenido);
        fclose(sintetizador);
        fclose(midi);
        fclose(wave);
        free_notas_encendidas(notas);
        return 1;
    }
    //almaceno datos de decaimiento
    funcion_t *decaimiento = extraer_modulacion(sintetizador);
    if(decaimiento == NULL){
        fprintf(stderr, "No se obtuvo 'decaimiento' correctamente\n");
        free_funcion_t(ataque);
        free_funcion_t(sostenido);
        fclose(sintetizador);
        fclose(midi);
        fclose(wave);
        free_notas_encendidas(notas);
        return 1;
    }

///////////////////////////////////////////////////////////////////

    // LECTURA DEL ENCABEZADO:
    formato_t formato;
    uint16_t numero_pistas;
    uint16_t pulsos_negra;

    if(! leer_encabezado(midi, &formato, &numero_pistas, &pulsos_negra)) {
        fprintf(stderr, "Fallo lectura encabezado\n");
        fclose(sintetizador);
        fclose(midi);
        fclose(wave);
        free_notas_encendidas(notas);
        return 1;
    }

    // ITERAMOS LAS PISTAS:
    for(uint16_t pista = 0; pista < numero_pistas; pista++) {
        // LECTURA ENCABEZADO DE LA PISTA:
        uint32_t tamagno_pista;
        if(! leer_pista(midi, &tamagno_pista)) {
            fprintf(stderr, "Fallo lectura pista\n");
            fclose(sintetizador);
            fclose(midi);
            fclose(wave);
            free_notas_encendidas(notas);
            tramo_destruir(tramo_aux);
            tramo_destruir(tramo);
            return 1;
        }

        char canal;
        evento_t evento;
        int longitud;
        uint32_t tiempo = 0;

        // ITERAMOS LOS EVENTOS:
        while(1) {
            uint32_t delta_tiempo;
            leer_tiempo(midi, &delta_tiempo);
            tiempo += delta_tiempo;

            // LECTURA DEL EVENTO:
            uint8_t buffer[EVENTO_MAX_LONG];
            if(! leer_evento(midi, &evento, &canal, &longitud, buffer)) {
                fprintf(stderr, "Error leyendo evento\n");
                fclose(sintetizador);
                fclose(midi);
                fclose(wave);
                free_notas_encendidas(notas);
                tramo_destruir(tramo_aux);
                tramo_destruir(tramo);
                return 1;
            }
            

            // PROCESAMOS EL EVENTO:
            if(evento == METAEVENTO && canal == 0xF) {
                // METAEVENTO:
                if(buffer[METAEVENTO_TIPO] == METAEVENTO_FIN_DE_PISTA) {
                    putchar('\n');
                    break;
                }

                descartar_metaevento(midi, buffer[METAEVENTO_LONGITUD]);
            }
            else if ((evento == NOTA_ENCENDIDA || evento == NOTA_APAGADA)) {
                // NOTA:
                nota_t nota;
                signed char octava;
                if(! decodificar_nota(buffer[EVNOTA_NOTA], &nota, &octava)) {
                    fprintf(stderr, "Error leyendo nota\n");
                    fclose(sintetizador);
                    fclose(midi);
                    fclose(wave);
                    free_notas_encendidas(notas);
                    tramo_destruir(tramo);
                    return 1;
                }
                
                if(canal == channel){
                    uint32_t inicio = 0;
                    float frecuencia = 0;

                    if (evento == NOTA_ENCENDIDA && buffer[EVNOTA_VELOCIDAD] != 0){
                        if(!almacenar_notas(notas, nota, octava, tiempo,buffer[EVNOTA_VELOCIDAD])){
                            fprintf(stderr, "Error extendiendo tramo\n");
                            free_notas_encendidas(notas);
                            fclose(sintetizador);
                            fclose(midi);
                            fclose(wave);
                            tramo_destruir(tramo);
                            return 1;
                        }
                    } else if ((evento == NOTA_ENCENDIDA && buffer[EVNOTA_VELOCIDAD] == 0)|| evento == NOTA_APAGADA){
                        float velocidad;
                        if(!nota_finalizada(notas, nota, octava, &inicio, &frecuencia, &velocidad)){
                            fprintf(stderr, "Error extendiendo tramo\n");
                            fclose(sintetizador);
                            fclose(midi);
                            fclose(wave);
                            free_notas_encendidas(notas);
                            tramo_destruir(tramo);
                            return 1;
                        }
                        double t0 = (double)inicio / pps;
                        double tf = (double)tiempo / pps;
                        
                        if (tramo == NULL){
                            tramo = tramo_crear_muestreo(t0, tf,f_muestreo,frecuencia,velocidad,fa,cant_armonicos,decaimiento);
                            tramo_modular(tramo,tf,ataque,sostenido,decaimiento);
                        }else{
                            tramo_aux = tramo_crear_muestreo(t0, tf,f_muestreo,frecuencia,velocidad,fa,cant_armonicos,decaimiento); 
                            tramo_modular(tramo_aux,tf,ataque,sostenido,decaimiento);

                            if(!tramo_extender(tramo,tramo_aux)){
                            fprintf(stderr, "Error extendiendo tramo\n");
                            free_notas_encendidas(notas);
                            fclose(sintetizador);
                            fclose(midi);
                            fclose(wave);
                            tramo_destruir(tramo_aux);
                            tramo_destruir(tramo);
                            return 1;
                            }
                            
                            tramo_destruir(tramo_aux);
                        }
                        
                    }
                }
            }
        }
    }
    //CONVIERTO EL TRAMO DE FLOATS A INT16_T

    tint16_t *v = tramo_a_int16(tramo);
    
    if(v == NULL){
        fprintf(stderr, "Error creando V\n");
        tramo_destruir(tramo);
        fclose(sintetizador);
        fclose(midi);
        fclose(wave);
        return 1;
    }

    tramo_destruir(tramo);

    // ESCRITURA DEL ARCHIVO WAVE:
    escribir_encabezado_wav(wave,v,f_muestreo);

    escribir_tramo_wav(wave, v);
    
    destruir_tramo_int16(v);
    
    //LIBERO LA MEMORIA PEDIDA

    free_funcion_t(ataque);
    free_funcion_t(sostenido);
    free_funcion_t(decaimiento);
    free_notas_encendidas(notas);
    fclose(sintetizador);
    fclose(midi);
    fclose(wave);

    return 0;
}
CFLAGS=-Wall -std=c99 -pedantic -g

all:main

decodificacion_senales_midi.o: decodificacion_senales_midi.c decodificacion.h
	gcc $(CFLAGS) decodificacion_senales_midi.c -c -lm

procesamientos_senales_midi.o: procesamientos_senales_midi.c decodificacion.h tramo.h
	gcc $(CFLAGS) procesamientos_senales_midi.c -c -lm

tramo.o: tramo.c sintetizador.h tramo.h
	gcc $(CFLAGS) tramo.c -c -lm

notas.o: notas.c procesamientos.h decodificacion.h tramo.h notas.h
	gcc $(CFLAGS) notas.c -c -lm

sintetizador.o: sintetizador.c notas.h tramo.h sintetizador.h
	gcc $(CFLAGS) sintetizador.c -c -lm

main.o: main.c decodificacion.h procesamientos.h sintetizador.h notas.h tramo.h
	gcc $(CFLAGS) main.c -c -lm

main: main.o decodificacion_senales_midi.o procesamientos_senales_midi.o tramo.o notas.o sintetizador.o
	gcc $(CFLAGS) main.o decodificacion_senales_midi.o procesamientos_senales_midi.o tramo.o notas.o sintetizador.o -o main -lm











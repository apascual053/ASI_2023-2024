programa: programa.o fich1.o fich2.o fich3.o teclado.o
	gcc -o programa programa.o fich1.o fich2.o fich3.o teclado.o

programa.o: programa.c fich1.c fich2.c fich3.c teclado.h
	gcc -c programa.c
fich1.o: fich1.c fich1.h fich2.h fich3.h teclado.h
	gcc -c fich1.c
fich2.o: fich2.c fich2.h fich3.h teclado.h
	gcc -c fich2.c
fich3.o: fich3.c fich3.h teclado.h
	gcc -c fich3.c
teclado.o: teclado.c teclado.h
	gcc -c teclado.c

limpiar:
	rm programa programa.o fich1.o fich2.o fich3.o teclado.o

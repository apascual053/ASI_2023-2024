#include <stdio.h>
#include <stdlib.h>
#include "fich1.h"
#include "fich2.h"
#include "fich3.h"
#include "teclado.h"

void main(void)
{
	printf("PRINCIPAL: PRINCIPIO\n");
	printf("EJECUTANDO PRINCIPAL...\n");
	pulsaEnter();
	printf("PRIMERA DESDE PRINCIPAL\n");
	primera();
	printf("SEGUNDA DESDE PRINCIPAL\n");
	segunda();
	printf("TERCERA DESDE PRINCIPAL\n");
	tercera();
	printf("PRINCIPAL: FIN\n");
}

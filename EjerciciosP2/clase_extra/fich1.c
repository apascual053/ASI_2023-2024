#include <stdio.h>
#include "fich1.h"
#include "fich2.h"
#include "fich3.h"
#include "teclado.h"

void primera(void);

#endif /* _FICH1_H */

void primera(void)
{
	printf("\nPRIMERA: PRINCIPIO\n");
	pulsaEnter();
	printf("SEGUNDA DESDE PRIMERA\n");
	segunda();
	printf("TERCERA DESDE PRIMERA\n");
	tercera();
	printf("PRIMERA:FIN");
}

#include <stdio.h>
#include "fich3.h"
#include "teclado.h"

void segunda(void);

#endif /* _FICH2_H */

void segunda(void)
{
	printf("\nSEGUNDA: PRINCIPIO\n");
	pulsaEnter();
	printf("TERCERA DESDE SEGUNDA\n");
	tercera();
	printf("SEGUNDA:FIN");
}

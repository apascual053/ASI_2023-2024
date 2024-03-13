#include <stdio.h>
#include <stdio_ext.h>

void pulsaEnter();

#endif /* TECLADO_H */

void pulsaEnter(void)
{
    printf("Pulsa ENTER para continuar");
    fflush(stdout);
    __fpurge(stdin);
    getchar();
}

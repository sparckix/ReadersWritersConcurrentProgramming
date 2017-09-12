#include <signal.h>
#include <time.h>
#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                                                } while (0)
#define NUM_FILOSOFOS 5
void inicializar_filosofos();
void cogerPalillos(int i);
void soltarPalillos(int i);
void eliminar_filosofos();
void comer(int filo, int *steps, timer_t timerid);
void muestraResultado();

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "filosofos.h"

#define IZQ (i+1) % NUM_FILOSOFOS
#define DER (i+(NUM_FILOSOFOS -1)) % NUM_FILOSOFOS

pthread_mutex_t lock;
pthread_cond_t puedeComer[NUM_FILOSOFOS];
int tenedores[NUM_FILOSOFOS] = {[0 ... NUM_FILOSOFOS - 1] = 2 };

int veces_comidas[NUM_FILOSOFOS];
long long contador_filosofos[NUM_FILOSOFOS];
int t_comiendo[NUM_FILOSOFOS];
volatile int f_comiendo = 0;
void comiendo(int filo, int *steps, timer_t timerid)
{
	struct itimerspec its;
	int valor_actual = *steps;
	int nrand = (rand() % (1500 - 500) + 500);
	long int i;
	if (nrand < 1000) {
		its.it_value.tv_sec = 0;
		i = nrand * 1000000;
	} else {
		its.it_value.tv_sec = 1;
		i = (nrand - 1000) * 1000000;
	}
	its.it_value.tv_nsec = i;	//conversion a nanosegundos
	if (timer_settime(timerid, 0, &its, NULL) == -1)
		errExit("timer_settime");
	while (!*steps) {
		contador_filosofos[filo]++;
	}
	*steps = 0;
	t_comiendo[filo] += nrand;
}

void comer(int filo, int *steps, timer_t timerid)
{
	cogerPalillos(filo);
	comiendo(filo, steps, timerid);
	soltarPalillos(filo);

}

void muestraResultado()
{
	int i;
	for (i = 0; i < NUM_FILOSOFOS; i++) {
		printf
		    ("Filosofo %d: \tTiempo comiendo: %d \tVeces comidas: %d \t Valor contador: %lld \n",
		     i, t_comiendo[i], veces_comidas[i], contador_filosofos[i]);
	}
}

void inicializar_filosofos()
{
	int i;
	for (i = 0; i < NUM_FILOSOFOS; i++)
		pthread_cond_init(&puedeComer[i], NULL);
	pthread_mutex_init(&lock, NULL);
}

void cogerPalillos(int i)
{
	pthread_mutex_lock(&lock);
	while (tenedores[i] != 2)
		pthread_cond_wait(&puedeComer[i], &lock);
	tenedores[IZQ]--;
	tenedores[DER]--;
//      printf("[FILO %d]Tenedores (izq %d, der %d): %d %d\n", i, IZQ, DER, tenedores[IZQ], tenedores[DER]);
	f_comiendo++;
	printf("El filosofo %d está comiendo\n", i);
	if (f_comiendo > 1)
		printf("¡¡ESTÁN COMIENDO %d FILÓSOFOS!!\n", f_comiendo);
	pthread_mutex_unlock(&lock);
}

void soltarPalillos(int i)
{
	pthread_mutex_lock(&lock);
	veces_comidas[i]++;
	tenedores[IZQ]++;
	tenedores[DER]++;
	f_comiendo--;
	printf("El filósofo %d ha dejado de comer\n", i);
	if (tenedores[IZQ] == 2)
		pthread_cond_signal(&puedeComer[IZQ]);
	if (tenedores[DER] == 2)
		pthread_cond_signal(&puedeComer[DER]);
	pthread_mutex_unlock(&lock);
}

void eliminar_filosofos()
{
	pthread_mutex_destroy(&lock);
	int i;
	for (i = 0; i < NUM_FILOSOFOS; i++)
		pthread_cond_destroy(&puedeComer[i]);
}

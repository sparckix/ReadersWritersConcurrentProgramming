/*
 La versión con semaforos posix threads se incluye comentada (descomentar segun se quiera)
*/


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "lectores_escritores.h"
#include <time.h>
#include <stdint.h>

#define LEER 1
#define ESCRIBIR 0
#define NUM_THREADS 4
#define MAX_ITERACIONES 100000000L

pthread_mutex_t lock;
volatile long count = 0;
volatile unsigned long long registro_lectura[NUM_THREADS] = { 0 };
volatile unsigned long long registro_escritura[NUM_THREADS] = { 0 };

long escrituras_totales = 0;

unsigned long long timespecDiff(struct timespec *timeA_p,
				struct timespec *timeB_p)
{
	return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
	    ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

void *counter(void *threadid)
{
	int nrand, op_lectura;
	unsigned int iseed = (unsigned int)time(NULL);
	long i, max = MAX_ITERACIONES / NUM_THREADS;
	long tid;
	long veces_escritas = 0;
	long veces_leidas = 0;
	long max_escrituras = max * 0.1;
	long max_lecturas = max - max_escrituras;
	struct timespec start, end;

	tid = (long)threadid;
	srand(iseed);
	for (i = 0; i < max; i++) {
		nrand = rand() % 2;
		if ((nrand == LEER && veces_leidas < max_lecturas)) {	//leer
                        clock_gettime(CLOCK_MONOTONIC, &start);
			entrada_lectores();
//                        pthread_mutex_lock(&lock);
                        clock_gettime(CLOCK_MONOTONIC, &end);
                        registro_lectura[tid] += timespecDiff(&end,&start);

			op_lectura = count;
			veces_leidas++;
			salida_lectores();
//                        pthread_mutex_unlock(&lock);
		} else if (nrand == ESCRIBIR && veces_escritas < max_escrituras) {	//escribir
                        clock_gettime(CLOCK_MONOTONIC, &start);
			entrada_escritores();
//                        pthread_mutex_lock(&lock);
                        clock_gettime(CLOCK_MONOTONIC, &end);
                        registro_escritura[tid] += timespecDiff(&end,&start);

			count++;
			escrituras_totales++;
			salida_escritores();
//                        pthread_mutex_unlock(&lock);
			veces_escritas++;
		}
	}

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	pthread_t threads[NUM_THREADS];
	int rc;
	long t;
	unsigned long long tespera_esc = 0, tespera_lec = 0;
	int i;

	inicializar_le();
//        pthread_mutex_init(&lock,NULL);
	for (t = 0; t < NUM_THREADS; t++) {
		rc = pthread_create(&threads[t], NULL, counter, (void *)t);
		if (rc) {
			printf
			    ("ERROR; return code from pthread_create() is %d\n",
			     rc);
			exit(-1);
		}
	}

	for (t = 0; t < NUM_THREADS; t++) {
		pthread_join(threads[t], NULL);
	}
	eliminar_le();
//        pthread_mutex_destroy(&lock);
	float error = (MAX_ITERACIONES - count) / (float)MAX_ITERACIONES * 100;

	printf("Final result: %ld Expected: %ld Error: %3.2f%%\n", count,
	       MAX_ITERACIONES, error);
	for (i=0; i<NUM_THREADS; i++) {
		tespera_esc += registro_escritura[i];
		tespera_lec += registro_lectura[i];
	}

	tespera_esc /= escrituras_totales;
	tespera_lec /= (MAX_ITERACIONES-escrituras_totales);
	printf("Tiempo espera escritura: %lld\n",tespera_esc);
	printf("Tiempo espera lectura:   %lld\n",tespera_lec);
	pthread_exit(NULL);

}

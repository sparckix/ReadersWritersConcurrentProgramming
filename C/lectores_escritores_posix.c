#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#include "lectores_escritores.h"

sem_t mutex;
sem_t mutex2;
static int lectores = 0;

void inicializar_le()
{
	sem_init(&mutex2, 0, 1);
	sem_init(&mutex, 0, 1);
}

void entrada_lectores()
{
	sem_wait(&mutex2);
	lectores++;
	if (lectores == 1) {
		sem_wait(&mutex);
	}
	sem_post(&mutex2);
}

void salida_lectores()
{
	sem_wait(&mutex2);
	lectores--;
	if (lectores == 0)
		sem_post(&mutex);
	sem_post(&mutex2);
}

void entrada_escritores()
{
	sem_wait(&mutex);
}

void salida_escritores()
{
	sem_post(&mutex);
}

void eliminar_le()
{
	sem_destroy(&mutex2);
	sem_destroy(&mutex);
}

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#include "lectores_escritores.h"

pthread_mutex_t lock;
pthread_cond_t turno;
int leyendo, escribiendo, escritores;	//escritores indica si tenemos un escritor esperando para entrar

void inicializar_le()
{
	pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&turno, NULL);
	leyendo = 0;
	escribiendo = 0;
	escritores = 0;
}

void entrada_lectores()
{
	pthread_mutex_lock(&lock);
	if (escritores) {
		pthread_cond_wait(&turno, &lock);
	}
	while (escribiendo) {
		pthread_cond_wait(&turno, &lock);
	}
	leyendo++;
	pthread_mutex_unlock(&lock);
}

void salida_lectores()
{
	pthread_mutex_lock(&lock);
	leyendo--;
	pthread_cond_broadcast(&turno);	//si hay mas de un lector esperando, signal ineficiente
	pthread_mutex_unlock(&lock);
}

void entrada_escritores()
{
	pthread_mutex_lock(&lock);
	escritores++;
	while (leyendo || escribiendo)
		pthread_cond_wait(&turno, &lock);
	escribiendo++;
	escritores--;
	pthread_mutex_unlock(&lock);
}

void salida_escritores()
{
	pthread_mutex_lock(&lock);
	escribiendo--;
	pthread_cond_broadcast(&turno);
	pthread_mutex_unlock(&lock);
}

void eliminar_le()
{
	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&turno);
}

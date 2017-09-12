#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include "filosofos.h"
#include <errno.h>

#define TIEMPO 30

int continuar = 1;
volatile long count = 0;
static void handler(int sig, siginfo_t * si, void *uc)
{
	int *cont = (int *)si->si_value.sival_ptr;
	*cont = 0;
}

static void handler_condicion(int sig, siginfo_t * si, void *uc)
{
	int *cond = (int *)si->si_value.sival_ptr;
	*cond = 1;
}

void pensar(int filo, int num_milisegundos)
{
//      printf("El filosofo %d está pensando\n",filo);
	usleep(num_milisegundos);
}

void crear_timer(timer_t * timerid, int signal, void (*handler), int *variable)
{
	struct sigevent sev;
	sigset_t mask;
	struct sigaction sa;

	/* Establecemos el handler para la señal del timer */
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handler;
	sigemptyset(&sa.sa_mask);
	if (sigaction(signal, &sa, NULL) == -1)
		errExit("sigaction");

	/* Bloqueamos la señal del timer temporalmente (no necesario) */
	sigemptyset(&mask);
	sigaddset(&mask, signal);
	if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
		errExit("sigprocmask");

	/* Finalmente creamos el timer */
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = signal;
	sev.sigev_value.sival_ptr = variable;
	if (timer_create(CLOCK_REALTIME, &sev, timerid) == -1)
		errExit("timer_create");

	if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1)
		errExit("sigprocmask");
}

void *counter(void *threadid)
{
	unsigned int iseed = (unsigned int)time(NULL);
	srand(iseed);
	int nrand;
	timer_t timerid = 0;
	long tid;
	tid = (long)threadid;
	int steps = 0;
	int signal = SIGRTMIN + tid + 1;	// La señal para cada hijo se obtiene de la base SIGRTMIN + el tid del thread

	crear_timer(&timerid, signal, handler_condicion, &steps);
	while (continuar) {
		nrand = (rand() % (1500 - 500) + 500);
		pensar((int)tid, nrand);
		comer((int)tid, &steps, timerid);
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	pthread_t threads[NUM_FILOSOFOS];
	int rc;
	long t;

	inicializar_filosofos();
	timer_t timerid = 0;
	int signal = SIGRTMIN;

	crear_timer(&timerid, signal, handler, &continuar);
	struct itimerspec its;

	/* Lo lanzamos */
	its.it_value.tv_sec = TIEMPO;
	its.it_value.tv_nsec = 0;
	its.it_interval.tv_sec = 0;
	its.it_interval.tv_nsec = 0;

	if (timer_settime(timerid, 0, &its, NULL) == -1) {
		printf("error %d\n", errno);
		errExit("timer_settime");
	}

	for (t = 0; t < NUM_FILOSOFOS; t++) {
		rc = pthread_create(&threads[t], NULL, counter, (void *)t);
		if (rc) {
			printf
			    ("ERROR; return code from pthread_create() is %d\n",
			     rc);
			exit(-1);
		}
	}
	while (continuar)
		pause();
	for (t = 0; t < NUM_FILOSOFOS; t++) {
		pthread_join(threads[t], NULL);
	}

	muestraResultado();
	eliminar_filosofos();
	return 0;
}

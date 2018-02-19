#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include "monitor.h"

bool x = false;

//you can edit these values for #define (just don't set them to 0)
#define LONG_TIME 100000000
#define SHORT_TIME 5000000
#define EXECUTION_LENGTH 5
#define FOREVER true

pthread_mutex_t myMutex=PTHREAD_MUTEX_INITIALIZER;;
typedef char threadID_t[10];
void *philo(void *); // the function that implement a philosopher thread
void *wait(void *); //This implements our waiter

int main(void) {
	init();
	int i;

	threadID_t id[N_THREADS]; // an array of N_THREADS strings, containing the IDs of philosophers
	pthread_t ph_thread[N_THREADS]; // N_THREADS philosopher threads
	pthread_t waiter_thread; //thread for waiter

	// create threads
	for(i=0;i<N_THREADS;i++) {
		sprintf(id[i],"%d",i);
		pthread_create(&ph_thread[i], NULL, philo, id[i]);
	}
	pthread_create(&waiter_thread, NULL, wait, NULL);

	// join threads
	for(i=0;i<N_THREADS;i++) {
		pthread_join(ph_thread[i],NULL);
	}
	printf("I am done\n");
	pthread_join(waiter_thread, NULL);

	destroy();
	pthread_mutex_destroy(&myMutex);
	return EXIT_SUCCESS;
}

/*
 * thread's entry point: notice that it's independent of the solution being adopted
 * the only argument, arg, is a string that shows philo's ID (0, 1, ...)
 * Knowing the philosopher's ID is necessary in order to use the correct chopsticks
 */

void *philo(void *arg) {
	int philo=atoi((char*)arg);
	int n=EXECUTION_LENGTH, i;
	while(n--) {
		if (FOREVER) { n++; }
    do_something(philo,"THINKING", SHORT_TIME);
	pthread_mutex_lock(&myMutex);
		tabs(philo);
		printf("%d: HUNGRY!!\n", philo);
		tabs(philo);
		printf("%d: This is my %d iteration\n",philo,n);
	pthread_mutex_unlock(&myMutex);
	i = assignSeat(philo);
	pickup(i);
	do_something(philo,"EATING", LONG_TIME);
	putdown(i);
	if (n!=0) {
		printf("If %d is here at 0 iteration, something is wrong\n",philo);
		GoToSleep(philo);
	}
	else {
		printf("If %d is here at 0 iteration, everything is right\n",philo);
		WakeupBeforeLeaving();
	}
}
	return NULL;
}

void *wait(void *arg) {
	while (!x) {
		x = waiter();
	}
	printf("I guess the waiter is not needed anymore\n");
	return NULL;
}

//monitor.c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include "monitor.h"

#define LEFT (i+N_SEATS-1)%N_SEATS /* seat to the left */
#define RIGHT (i+1)%N_SEATS        /* seat to the right */

enum {THINKING, HUNGRY, EATING} seats[N_SEATS];
enum {ASSIGNED, NOT_ASSIGNED} state[N_THREADS];

pthread_cond_t self[N_THREADS], notAtTable[N_THREADS], leavingTable[N_THREADS], mySeat[N_SEATS];
pthread_mutex_t mutex, om, mutey, waiterMut;
int theSeatAssigned[N_THREADS], thePhilosopherAssigned[N_SEATS];
int arrival, leaving, nextOne, wakeupagain, seatsOccupied;
bool nowStop, IHaveEaten[N_THREADS], thisIsYourLastTurn;

int assignSeat(int i) {
	pthread_mutex_lock(&om);
	int j;
	while (state[i]==NOT_ASSIGNED) {
		for (j=0;j<N_SEATS;j++) {
			if (seats[j]==THINKING) {
				if (!IHaveEaten[i]) {
					leaving++;
					seats[j]=HUNGRY;
					state[i]=ASSIGNED;
					theSeatAssigned[i] = j;
					thePhilosopherAssigned[j] = i;
					j=N_SEATS;
				}
			}
		}
		if (state[i]==NOT_ASSIGNED) {
			IHaveEaten[i]=false;
			arrival++;
			if (!nowStop) {
				printf("%d: I didn't get a seat, so going to sleep\n",i);
				pthread_cond_wait(&self[i], &om);
			}
		}
	}
	pthread_mutex_unlock(&om);
	return theSeatAssigned[i];
}

bool waiter() {
	if (!nowStop) {
		if (arrival >= N_THREADS - seatsOccupied) {
			printf("%d philosophers who didn't get a seat are now asleep\n",arrival);
			pthread_mutex_lock(&om);
			int i;
			for(i=0;i<N_THREADS;i++) {
				pthread_cond_signal(&notAtTable[i]);
			}
			arrival = 0;
			pthread_mutex_unlock(&om);
		}
	}

	if (nextOne >= seatsOccupied && !nowStop) {
		printf("5 philosophers who have vacated their seat are now asleep or have left entirely\n");
		pthread_mutex_lock(&waiterMut);
		seatsOccupied = N_THREADS - seatsOccupied;
		int k;
		pthread_mutex_lock(&om);
		for(k=0;k<N_THREADS;k++) {
			pthread_cond_signal(&self[k]);
		}
		nextOne = 0;
		if (thisIsYourLastTurn) { nowStop = true; }
		pthread_mutex_unlock(&om);
		pthread_mutex_unlock(&waiterMut);
	}

	if (leaving >= seatsOccupied && !nowStop) {
		printf("5 philosophers who have vacated their seat are now asleep\n");
		pthread_mutex_lock(&waiterMut);
		int j;
		for(j=0;j<N_THREADS;j++) {
			pthread_cond_signal(&leavingTable[j]);
		}
		leaving = 0;
		pthread_mutex_unlock(&waiterMut);
	}
	if (nowStop) {
		return true;
	}
	else {
		return false;
	}
}

void pickup(int i) {
	pthread_mutex_lock(&mutex);
	if (!nowStop) {
		printf("%d: I am in my seat, going to sleep\n",thePhilosopherAssigned[i]);
		pthread_cond_wait(&notAtTable[i], &mutex);
	}
	test(i);
	while(seats[i]==HUNGRY) {
		pthread_cond_wait(&mySeat[i], &mutex);
	}
	pthread_mutex_unlock(&mutex);
}

void putdown(int i) {
	pthread_mutex_lock(&mutex);
	int philo = thePhilosopherAssigned[i];
	state[philo]=NOT_ASSIGNED;
	seats[i]=THINKING;
	theSeatAssigned[philo] = -1;
	thePhilosopherAssigned[i] = -1;
	test(LEFT);
	test(RIGHT);
	pthread_mutex_unlock(&mutex);
}

void GoToSleep(int i) {
	pthread_mutex_lock(&waiterMut);
	IHaveEaten[i] = true;
	nextOne++;
	if (!nowStop) {
		printf("%d: I have vacated my seat and now going to sleep\n",i);
		pthread_cond_wait(&leavingTable[i], &waiterMut);
	}
	pthread_mutex_unlock(&waiterMut);
}

void WakeupBeforeLeaving() {
	pthread_mutex_lock(&waiterMut);
	nextOne++;
    if (nextOne >= N_SEATS) { thisIsYourLastTurn = true; }
	pthread_mutex_unlock(&waiterMut);
	printf("I am leaving now and nextOne is %d\n",nextOne);
}

void test(int i) {
	int philo = thePhilosopherAssigned[i];
	if(seats[i]==HUNGRY && seats[LEFT]!=EATING && seats[RIGHT]!=EATING) {
		if (philo!=-1) {
			seats[i]=EATING;
			pthread_cond_signal(&mySeat[i]);
		}
	}
}

void init() {
	int i;
	if (N_SEATS >= N_THREADS) { printf("I am telling waiter to leave\n"); nowStop = true; }
	else { printf("my waiter is active\n"); nowStop = false; }
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&om, NULL);
	pthread_mutex_init(&mutey, NULL);
	pthread_mutex_init(&waiterMut, NULL);
	arrival = 0, leaving = 0, nextOne = 0, wakeupagain = 0, seatsOccupied = N_SEATS, thisIsYourLastTurn = false;
	for (i=0;i<N_THREADS;i++) {
		pthread_cond_init(&self[i], NULL);
		pthread_cond_init(&notAtTable[i], NULL);
		pthread_cond_init(&leavingTable[i], NULL);
		theSeatAssigned[i] = -1;
		state[i] = NOT_ASSIGNED;
		IHaveEaten[i] = false;
	}
	for (i=0;i<N_SEATS;i++) {
		thePhilosopherAssigned[i] = -1;
		seats[i] = THINKING;
		pthread_cond_init(&mySeat[i], NULL);
	}
}

void destroy() {
	int i;
	pthread_mutex_destroy(&mutex);
	pthread_mutex_destroy(&om);
	pthread_mutex_destroy(&mutey);
	pthread_mutex_destroy(&waiterMut);
	for (i=0;i<N_THREADS;i++) {
		pthread_cond_destroy(&self[i]);
		pthread_cond_destroy(&notAtTable[i]);
		pthread_cond_destroy(&leavingTable[i]);
	}
	for (i=0;i<N_SEATS;i++) {
		pthread_cond_destroy(&mySeat[i]);
	}
}

void do_something (int i, char *what, long cycles) {
	double x, sum=0.0, step;
	long f, N_STEPS=rand()%cycles;
	pthread_mutex_lock(&mutey);
		tabs(i);
		printf("%d: %s ...\n", i, what);
	pthread_mutex_unlock(&mutey);
	step = 1/(double)N_STEPS;
	for(f=0; f<N_STEPS; f++) {
		x = (f+0.5)*step;
		sum+=4.0/(1.0+x*x);
	}
	pthread_mutex_lock(&mutey);
		tabs(i);
		printf("%d: ... DONE %s\n", i, what);
		x=step*sum;
	pthread_mutex_unlock(&mutey);
}

void tabs(int n) {while(n-->0)putchar('\t');}

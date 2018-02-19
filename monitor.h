//monitor.h
#ifndef MONITOR_H_
#define MONITOR_H_

//You can change these defines (just don't set them to 0)
#define N_THREADS 10
#define N_SEATS 11

void pickup(int i);

void putdown(int i);

void test(int i);

bool waiter();

void GoToSleep(int i);

void WakeupBeforeLeaving();

int assignSeat(int i);

void init();

void destroy();

void do_something(int i, char *what, long cycles);

void tabs(int n);

#endif /* MONITOR_H_ */

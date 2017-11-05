/* main .h */

#ifndef INCLUDES_MAIN_TASK_H_
#define INCLUDES_MAIN_TASK_H_

#include "generic.h"


/* Static Main task message queue descriptors */

struct sigevent mq_mainTask_timer;
struct sigaction timer_sa;
struct sigaction mq_mainTask_sa;
int8_t mainSigNo;

/* condition variable */
pthread_cond_t cond_main;

/* Mutexes */
pthread_mutex_t main_mtx;

struct sigevent mq_mainTask_notify;

#endif

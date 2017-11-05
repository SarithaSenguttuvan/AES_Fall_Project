/* main .h */

#ifndef INCLUDES_MAIN_TASK_H_
#define INCLUDES_MAIN_TASK_H_

#include "generic.h"

/* Signal event structures */
struct sigevent mq_mainTask_timer;
struct sigevent mq_mainTask_notify;

int8_t mainSigNo;
#endif

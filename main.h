/**
 * @file 
 *		main.h 
 * @brief 
 *		This file contains function declarations for variables used in main.c
 *		 
 * @author 
 *		Saritha Senguttuvan, Savitha Senguttuvan
 * @date 
 *		Nov 4,2017
 *
 */

#ifndef INCLUDES_MAIN_TASK_H_
#define INCLUDES_MAIN_TASK_H_

#include "generic.h"

/* Signal event structures */
struct sigevent mq_mainTask_timer;
struct sigevent mq_mainTask_notify;
/* Signal number */
int8_t mainSigNo;
#endif

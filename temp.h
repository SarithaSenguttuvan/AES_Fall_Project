/* temp.h */

#ifndef INCLUDES_TEMP_QUEUE_H_
#define INCLUDES_TEMP_QUEUE_H_
#include "generic.h"

pthread_cond_t cond_temp;

pthread_mutex_t temp_mtx;

struct sigaction mq_tempTask_sa;

struct sigevent mq_tempTask_notify;
struct sigaction mq_tempTask_sa;

/**
 * @brief 
 * 		This function is the pthread function for the temperature task
 * @return 
 *      void
 */
void *tempTaskFunc(void *arg);



uint8_t send_temp_msg(uint8_t destTaskId, msgStruct_t *temp_msg);

#endif

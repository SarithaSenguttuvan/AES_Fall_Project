/* light.h */

#ifndef INCLUDES_LIGHT_QUEUE_H_
#define INCLUDES_LIGHT_QUEUE_H_
#include "generic.h"

pthread_cond_t cond_light;

struct sigaction mq_lightTask_sa;

struct sigevent mq_lightTask_notify;

pthread_mutex_t light_mtx;

/**
 * @brief 
 * 		This function is the pthread function for the Light task
 * @return 
 *      void
 */
void *lightTaskFunc(void *arg);



uint8_t send_light_msg(uint8_t destTaskId, msgStruct_t * light_msg);


#endif

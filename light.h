/* light.h */

#ifndef INCLUDES_LIGHT_QUEUE_H_
#define INCLUDES_LIGHT_QUEUE_H_
#include "generic.h"

struct sigevent mq_lightTask_notify;

/**
 * @brief 
 * 		This function is the pthread function for the Light task
 * @return 
 *      void
 */
void *lightTaskFunc(void *arg);



uint8_t send_light_msg(uint8_t destTaskId, msgStruct_t * light_msg);


#endif

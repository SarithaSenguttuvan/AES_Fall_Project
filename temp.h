/* temp.h */

#ifndef INCLUDES_TEMP_QUEUE_H_
#define INCLUDES_TEMP_QUEUE_H_
#include "generic.h"

struct sigevent mq_tempTask_notify;
/**
 * @brief 
 * 		This function is the pthread function for the temperature task
 * @return 
 *      void
 */
void *tempTaskFunc(void *arg);



uint8_t send_temp_msg(uint8_t destTaskId, msgStruct_t *temp_msg);

#endif

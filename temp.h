/**
 * @file 
 *		temp.h 
 * @brief 
 *		This file contains function declarations for temperature functions
 *		 
 * @author 
 *		Saritha Senguttuvan, Savitha Senguttuvan
 * @date 
 *		Nov 4,2017
 *
 */

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
/**
 * @brief 
 * 		This function is the send the temperature data 
 * @return 
 *      success - 0, failure -1
 */
uint8_t send_temp_msg(uint8_t destTaskId, msgStruct_t *temp_msg, mqd_t qdes_temp);

#endif

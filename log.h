/**
 * @file 
 *		log.h 
 * @brief 
 *		This file contains function declarations for log functions
 *		 
 * @author 
 *		Saritha Senguttuvan, Savitha Senguttuvan
 * @date 
 *		Nov 4,2017
 *
 */

#ifndef INCLUDES_LOGGER_TASK_H_
#define INCLUDES_LOGGER_TASK_H_

#include <pthread.h>
#include <stdint.h>

#define LOG_ITEM(data)		log_item(data)

/* Signal event structure for notifying the message queues */
struct sigevent mq_logTask_notify;

/* Enum for the logger level */
typedef enum
{	
	LOG_FATAL,
	LOG_ERROR,
	LOG_DEBUG,
	LOG_WARN,
	LOG_INIT,
	LOG_INFO
}LOGGER_level;

/* Structure for log packets */
typedef struct 						/* Log structure */
{
	LOGGER_level logLevel; 			/* Log type */
	uint32_t log_timestamp; 		/* Time */
	size_t logPayloadLen;			/* Log length  */
	void * logPayload; 				/* Log associated data */
}log_t;

/**
 * @brief 
 * 		This function is the pthread function for the log task
 * @return 
 *      void
 */
void *logTaskFunc(void *arg);

/**
 * @brief 
 * 		This function is used to receive logs to the log queue
 * @return 
 *      void
 */
uint8_t receive_log();

/**
 * @brief 
 * 		This is a function handler for the log task
 * @return 
 *      void
 */
void logTask_handler(int signum);


#endif /* INCLUDES_LOGGER_QUEUE_H_ */

/*
 * log.h
 *
 *  Created on: Oct, 2017
 *      Author: Saritha Senguttuvan, Savitha Senguttuvan
 */

#ifndef INCLUDES_LOGGER_TASK_H_
#define INCLUDES_LOGGER_TASK_H_

#include <pthread.h>
#include <stdint.h>

#define LOG_ITEM(data)		log_item(data)

pthread_cond_t cond_log;

pthread_mutex_t log_mtx;

struct sigaction mq_logTask_sa;

struct sigevent mq_logTask_notify;

/* Enum for the logger level */
typedef enum
{	
	LOG_FATAL,
	LOG_ERROR,
	LOG_DEBUG,
	LOG_WARN,
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

#if 0

/**
 * @brief
 *
 * This function displays the logs in the desired format to the file, to log specific events
 *
 * @param Log structure which has to be printed
 * @return none
 */
void log_item(log_t * data);

/**
 * @brief 
 * 		This function is used to print the string pointing to by a ptr
 * @param 
 *		str - points to the location of the string to be printed
 * @return 
 *      void
 */
void log_raw_string(uint8_t * str);

/**
 * @brief 
 * 		This function is used to print the integer data 
 * @param 
 *		int_data - holds the integer data to be printed
 * @return 
 *      void
 */
void log_raw_int(int32_t int_data);

#endif

#endif /* INCLUDES_LOGGER_QUEUE_H_ */

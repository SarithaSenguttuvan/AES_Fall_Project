/*
 * log.h
 *
 *  Created on: Oct, 2017
 *      Author: Saritha Senguttuvan, Savitha Senguttuvan
 */

#ifndef INCLUDES_LOGGER_QUEUE_H_
#define INCLUDES_LOGGER_QUEUE_H_
#include <stdlib.h>
#include <stdint.h>

#define LOG_ITEM(data)		log_item(data)

#define MAIN_LOG_ID 1
#define TEMPERATURE_LOG_ID 2
#define LIGHT_LOG_ID 3
#define LOGGER_LOG_ID 4
typedef enum
{
	LOGGER_HEARTBEAT,
	LOGGER_ INITIALZED,
	LOGGER_ERROR,
	LOGGER_DATA_REQ
} LOGGER_level;


/* Structure for log packets */
typedef struct 							/* Log structure */
{
	LOGGER_level log_id; 				/* Log type */
	uint8_t src_log_id; 				/* Source Task ID */
	uint8_t log_len;					/* Log length  */
	uint32_t log_timestamp; 			/* Time */
	uint8_t * log_payload; 				/* Log associated data */
} log_t;

#if 0
/* Structure for data packets */
typedef struct 							/* Msg structure */
{
	uint8_t src_msg_id; 				/* Source Task ID */
	uint8_t log_len;						/* Log length  */
	uint8_t * msg_payload; 				/* Msg associated data */
} msg_t;

#endif

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

#if 0
/**
 * @brief 
 * 		This function is used to clear the log data
 * @return 
 *      void
 */
 
 #endif
void log_flush();

/**
 * @brief 
 * 		This function is used to send logs to the log queue
 * @param 

 *		log_data - holds the log packet of type log_t
 * @return 
 *      void
 */
LOGGER_level send_log(log_t log_data);

/**
 * @brief 
 * 		This function is used to check logs and parse the message
 * @param 
 *		log_data - holds the log packet of type log_t
 * @return 
 *      void
 */
LOGGER_level read_log(log_t log_data);

/**
 * @brief 
 * 		This function is used to send message requests
 * @param 
 *		task_id - ID of the task to which the request is to be sent
 * @return 
 *      void
 */
LOGGER_level msg_req(uint8_t task_id);


#endif /* INCLUDES_LOGGER_QUEUE_H_ */

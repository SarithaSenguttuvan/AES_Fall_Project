/* generic.h */

#ifndef INCLUDES_GENERIC_TASK_H_
#define INCLUDES_GENERIC_TASK_H_

#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <mqueue.h>
#include <sys/sysinfo.h>
#include <semaphore.h>
#include <signal.h>
#include "log.h"

#include <sys/types.h>
#include <sys/syscall.h>

#define READ_QUEUE_PERMISSIONS 0660

/* Queue names */
#define MAIN_TASK_MQ_NAME "/main_task_queue"
#define LOG_TASK_MQ_NAME "/log_task_queue"
#define TEMP_TASK_MQ_NAME "/temp_task_queue"
#define LIGHT_TASK_MQ_NAME "/light_task_queue"

#define MAIN_TASK_ID 1
#define LOG_TASK_ID 2
#define TEMP_TASK_ID 3
#define LIGHT_TASK_ID 4

/* Signal Macros */ 
#define SIGMAIN 50
#define SIGLOG 51	
#define SIGTEMP 52
#define SIGLIGHT 53
#define SIGTIMER 54

/* Thread names */
pthread_t logTask;
pthread_t tempTask;
pthread_t lightTask;

/* Thread atributes */
pthread_attr_t logTask_attr;
pthread_attr_t tempTask_attr;
pthread_attr_t lightTask_attr;

/* Queue attributes */
struct mq_attr main_queue_attr;
struct mq_attr log_queue_attr;
struct mq_attr temp_queue_attr;
struct mq_attr light_queue_attr;

/* Message queue Sigaction */

#define SIGNALMASK 0




/* Enum for type of message i.e., Message ID */
typedef enum
{
	MSGID_HB_RESP,
	MSGID_TEMP_REQ,
	MSGID_LIGHT_REQ,
	MSGID_HB_REQ,
	MSGID_TEMP_DATA,
	MSGID_LIGHT_DATA,
	MSGID_LOGMSG
}msgid_t;

/* Structure for a message */
typedef struct
{
	msgid_t msgId;
	uint8_t msgSrcTask;				/* Source Task ID */
	size_t msgPayloadLen;			/* Msg length  */
	void * msgPayload; 				/* Msg associated data */
}msgStruct_t;


/* Mutex name */
pthread_mutex_t rsrcA;

/**
 * @brief 
 * 		This function is used to send logs to the log queue
 * @param 
 *		srcTaskId - Holds the sourcetask ID that sends the logs
 *		logPacket - holds the log packet of type log_t
 * @return 
 *      uint8_t
 */
uint8_t send_log(uint8_t srcTaskId, log_t* logPacket);		//Frame the message of type msgStruct_t in this function


/**
 * @brief 
 * 		This function is used to send heartbeat messages to the main queue
 * @param 
 *		srcTask - Holds the sourcetask ID that sends the logs
 *		logPacket - holds the log packet of type log_t
 * @return 
 *      uint8_t
 */
uint8_t send_heartBeat(int8_t srcTaskId, msgStruct_t *HB_main);

uint8_t send_hb_req();

uint8_t send_light_req(uint8_t srcTaskId);

uint8_t send_temp_req(uint8_t srcTaskId);

/* Handlers */
/**
 * @brief 
 * 		This is a function handler for the main task
 * @return 
 *      void
 */
void mainTask_handler(int signum);



/**
 * @brief 
 * 		This is a function handler for the Light task
 * @return 
 *      void
 */
void lightTask_handler(int signum);

/**
 * @brief 
 * 		This is a function handler for the Temperature task
 * @return 
 *      void
 */
void tempTask_handler(int signum);


void blockSignals(int taskID);

int32_t unblockOnSignal(int taskID);
#endif



















































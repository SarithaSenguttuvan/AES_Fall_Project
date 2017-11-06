/* light.c */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "light.h"
#include "generic.h"
#include "I2C_light.h"


extern bool sigHandle;

mqd_t qdes_loc_lightTask;
char *current_ambience;
	
/* Function call to send the light data */
uint8_t send_light_msg(uint8_t destTaskId, msgStruct_t * light_msg, mqd_t qdes_light)
{
	printf("send_light_msg::In function send light msg\n");

	/* initialize the packet values */
	light_msg->msgId = MSGID_LIGHT_DATA;
	light_msg->msgSrcTask = LIGHT_TASK_ID;
	light_msg->msgPayload = current_ambience;
	light_msg->msgPayloadLen = sizeof(*(light_msg->msgPayload));

	if(mq_send(qdes_light, (char *)light_msg, sizeof(msgStruct_t), 0) == -1)  /* send the light message to task queue specified by qdes_light */
    {
    	printf ("send_light_msg::ERROR No: %d queue unable to send light data to the task %d\n", errno, destTaskId);
    }
    else
    	printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$send_light_msg::Light message Sent from send_lightmsg to task %d\n", destTaskId);

    return 0;
}

/* Start routine for the Light Task */
void *lightTaskFunc(void *arg)
{
	printf("lightTaskFunc::In the Light task function\n");
	int8_t rc = 0;	
	int8_t n = 0;	
	int32_t recvSig = 0;   
    mqd_t qdes_loc_lightMainQueue;
    mqd_t qdes_lightTask_To_Log;
    bool test_ambience;
    bool light_ambience;
    char logPayloadBuff[200];
	
	/* configuring the mq_notify signals */	
	mq_lightTask_notify.sigev_notify = SIGEV_SIGNAL;						
	mq_lightTask_notify.sigev_signo = SIGLIGHT;

	if((qdes_loc_lightMainQueue = mq_open(MAIN_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1)  /* opem the main task queue */
	{
    	printf ("lightTaskFunc::ERROR No: %d Unable to open main queue to send hb response\n", errno);
    }
	
	if((qdes_loc_lightTask = mq_open(LIGHT_TASK_MQ_NAME, O_NONBLOCK | O_RDONLY)) == (mqd_t)-1)  /* opem the main light queue */
	{
    	printf ("lightTaskFunc::ERROR No: %d Unable to open light task\n", errno);
    }	
 
	if((qdes_lightTask_To_Log = mq_open(LOG_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1) /* opem the log task queue */
	{
    	printf ("lightTaskFunc::ERROR No: %d Unable to open log task\n", errno);
    }
        
    if(mq_notify (qdes_loc_lightTask, &mq_lightTask_notify) == -1)						/* Registering the mq_notify signals */	
    {
    	printf("lightTaskFunc::The error number in mq_notify in the Light Task is %d\n", errno);
        if(errno == EBUSY)
                printf("lightTaskFunc::Another process has registered for notifications in Light task queue.\n");
    } 

/* initialize the I2C */
	if(light_setup() == I2C_LSUCCESS)
	{
		printf("Configured I2C\n");
	}
	else
	{
		printf("Error in configuring I2C\n");
	}

	light_or_dark(&test_ambience);    		/* Fetch the value */
    light_ambience = test_ambience;
    if(light_ambience)
    	current_ambience = "Day";
    else 	
    	current_ambience = "Night";
    	
    /* initializing the log packets */
	log_t *lightLog = (log_t *)malloc(sizeof(log_t));
	lightLog->logLevel = LOG_INFO;
	lightLog->log_timestamp = time(NULL);
	sprintf(logPayloadBuff, "Log Level is %d\n current ambience is %s\n Time is %d", lightLog->logLevel, current_ambience, lightLog->log_timestamp); 
	lightLog->logPayload = logPayloadBuff;
	lightLog->logPayloadLen = sizeof(lightLog->logPayload);

	msgStruct_t *read_light_msg_queue = (msgStruct_t *)malloc(sizeof(msgStruct_t));
	msgStruct_t *light_msg = (msgStruct_t *)malloc(sizeof(msgStruct_t));
	msgStruct_t *HB_main = (msgStruct_t *)malloc(sizeof(msgStruct_t));	
	msgStruct_t *lightTaskLogMsg = (msgStruct_t *)malloc(sizeof(msgStruct_t));
	blockSignals(0);		/* blocking all the signals */

	while(!sigHandle)
	{
		/* check if day or night */
	   	if(test_ambience)
	   		current_ambience = "Day";
	    else
	    	current_ambience = "dark";
	    if(light_ambience != test_ambience) /* if changed from previous value send logs */
	    {
	    	//send_log
	    	light_ambience = test_ambience;
	    }
	   	printf("lightTaskFunc::Waiting for Signals\n");
	  	recvSig = unblockOnSignal(LIGHT_TASK_ID); 		/* unblock on SIGLIGHT */  
        printf("lightTaskFunc::Received Signal after Sigwait:%d\n", recvSig);
            
        /* Re-registering signals */
        if(mq_notify (qdes_loc_lightTask, &mq_lightTask_notify) == -1)							
		{
			printf("lightTask_handler::The error number in mq_notify in the Light Task is %d\n", errno);
		    if(errno == EBUSY)
		            printf("lightTask_handler::Another process has registered for notifications in Light task\n");
		}
		 
		do
		{
			if((mq_receive(qdes_loc_lightTask, (char *)read_light_msg_queue, sizeof(msgStruct_t), NULL)) == -1)  /* receive the msg from queue */
			{
				n = errno;
				printf("lightTaskFunc::ERROR number to receive the data in light task message queue is %d\n", errno);
			} 
			else
			{
				if(read_light_msg_queue->msgId == MSGID_LIGHT_REQ)			/* If light request */										
				{
					printf("light()::^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Received light request\n");
					send_light_msg(read_light_msg_queue->msgSrcTask, light_msg, qdes_loc_lightMainQueue);
				}
				else if(read_light_msg_queue->msgId == MSGID_HB_REQ)		 /* If heartbeat request */										
				{
					send_heartBeat(LIGHT_TASK_ID,HB_main, qdes_loc_lightMainQueue);	
					light_or_dark(&test_ambience);	
					send_log(LIGHT_TASK_ID, lightLog, lightTaskLogMsg, qdes_lightTask_To_Log);			
				}
			}
		printf("lightTaskFunc::n = %d\n",n);
		}while(n != EAGAIN);
		n = 0;
	}	
	
	printf("RECEIVED TERMINATING IN LIGHT TASK\n");

/* free all the dynamic memory allocated */	
	free(light_msg);
	free(HB_main);
	free(read_light_msg_queue);
	free(lightLog);
	free(lightTaskLogMsg);

/* close all the message queues */
    if (mq_close (qdes_loc_lightTask) == -1) 
    {
        printf("ERROR No %d: Closing the Light task queue\n",errno);
    }
    if (mq_close (qdes_loc_lightMainQueue) == -1) 
    {
        printf("ERROR No %d: Closing the Main task queue from light task\n",errno);
    }
    if (mq_close (qdes_lightTask_To_Log) == -1) 
    {
        printf("ERROR No %d: Closing the Log task queue from light task\n",errno);
    }
    
    /* Unlink all the messasges */
    if (mq_unlink (LIGHT_TASK_MQ_NAME) == -1) 
    {
        printf("ERROR No: %d Unable to unlink the light queue \n", errno);
    } 
    if (mq_unlink (MAIN_TASK_MQ_NAME) == -1) 
    {
        printf("ERROR No: %d Unable to unlink the main queue \n", errno);
    } 
    if (mq_unlink (LOG_TASK_MQ_NAME) == -1) 
    {
        printf("ERROR No: %d Unable to unlink the log queue \n", errno);
    } 
	
	pthread_exit(NULL);
}

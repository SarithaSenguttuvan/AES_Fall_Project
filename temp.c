/* temp.c */
#include <stdio.h>
#include <stdlib.h>
#include "temp.h"
#include "generic.h"
#include <stdbool.h>

extern bool sigHandle;

mqd_t qdes_loc_tempTask;

/* Function call to send the temperature data */
uint8_t send_temp_msg(uint8_t destTaskId, msgStruct_t *temp_msg, mqd_t qdes_temp)
{
	printf("send_temp_msg::In function send temp data\n");
	int8_t temp_data = 24;
	
	temp_msg->msgId = MSGID_TEMP_DATA;
	temp_msg->msgSrcTask = TEMP_TASK_ID;
	temp_msg->msgPayload = &temp_data;
	temp_msg->msgPayloadLen = sizeof(* temp_msg->msgPayload);
#if 0
	if(destTaskId == 1)
	{
		if((qdes_temp = mq_open(MAIN_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1) 
		{
			printf ("send_temp_msg::***********************ERROR No: %d Main queue unable to open\n", errno);
		}
	}
	else if(destTaskId == 2)
	{
		if((qdes_temp = mq_open(LOG_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1) 
		{
			printf ("send_temp_msg::***********************ERROR No: %d log queue unable to open\n", errno);
		}
	}
	else if(destTaskId == 4)
	{
		if((qdes_temp = mq_open(LIGHT_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1) 
		{
			printf ("send_temp_msg::***********************ERROR No: %d Light queue unable to open\n", errno);
		}
	}
#endif
	if(mq_send(qdes_temp, (char *)temp_msg, sizeof(msgStruct_t), 0) == -1) 
    {
    	printf ("send_temp_msg::ERROR No: %d queue unable to send temperature data to the task %d\n", errno, destTaskId);
    }
    else
    	printf("send_temp_msg::Temperature message Sent from send_tempmsg to task %d\n", destTaskId);

#if 0    
    if (mq_close (qdes_temp) == -1) 
    {
        printf("send_temp_msg::ERROR No %d: Closing the Temperature task queue in send_tempmsg\n",errno);
    }
#endif
    return 0;
}


/* Start routine for the temp Task */
void *tempTaskFunc(void *arg)
{
	int8_t rc = 0;
	printf("tempTaskFunc::In the Temperature task function\n");
	int8_t n = 0;
	int32_t recvSig = 0;
	mqd_t qdes_loc_tempMainQueue;
	mqd_t qddes_tempTask_To_Log;

	mq_tempTask_notify.sigev_notify = SIGEV_SIGNAL;						
	mq_tempTask_notify.sigev_signo = SIGTEMP;

	if((qdes_loc_tempMainQueue = mq_open(MAIN_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1) 
	{
    	printf ("tempTaskFunc::ERROR No: %d Unable to open main queue to send hb response\n", errno);
    }

	if((qdes_loc_tempTask = mq_open(TEMP_TASK_MQ_NAME, O_NONBLOCK | O_RDONLY)) == (mqd_t)-1) 
	{
    	printf ("tempTaskFunc::ERROR No: %d Unable to open Temperature task\n", errno);
    }

	if((qddes_tempTask_To_Log = mq_open(LOG_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1) 
	{
    	printf ("tempTaskFunc::ERROR No: %d Unable to open log task\n", errno);
    }
        	
    if(mq_notify (qdes_loc_tempTask, &mq_tempTask_notify) == -1)						
    {
    	printf("tempTaskFunc::The error number in mq_notify in the temperature Task is %d\n", errno);
        if(errno == EBUSY)
                printf("tempTaskFunc::Another process has registered for notifications in Temperature task queue.\n");
    }    
    
	log_t *tempLog = (log_t *)malloc(sizeof(log_t));
	tempLog->logLevel = LOG_INFO;
	tempLog->log_timestamp = 0;
	tempLog->logPayload = "Temperature";
	tempLog->logPayloadLen = sizeof(tempLog->logPayload);

	//printf("Sending log from Temperature task\n");
	//send_log(TEMP_TASK_ID, tempLog);
	
	msgStruct_t *read_temp_msg_queue = (msgStruct_t *)malloc(sizeof(msgStruct_t));
	msgStruct_t *HB_main = (msgStruct_t *)malloc(sizeof(msgStruct_t));	
	msgStruct_t *temp_msg = (msgStruct_t *)malloc(sizeof(msgStruct_t));	
	msgStruct_t *tempTaskLogMsg = (msgStruct_t *)malloc(sizeof(msgStruct_t));
	    	
	blockSignals(0);

	//printf("temp():: Sending log ###################################################################### \n");
    //send_log(TEMP_TASK_ID, tempLog, tempTaskLogMsg, qddes_tempTask_To_Log);
    	
	while(!sigHandle)
	{	
	   	printf("tempTaskFunc::Signal Waiting\n");
	   	recvSig = unblockOnSignal(TEMP_TASK_ID);   
        printf("tempTaskFunc::Received Signal after Sigwait:%d\n", recvSig);	
        
        if(mq_notify (qdes_loc_tempTask, &mq_tempTask_notify) == -1)						
		{
			printf("tempTask_handler::The error number in mq_notify in the Temperature Task is %d\n", errno);
		    if(errno == EBUSY)
		            printf("tempTask_handler::Another process has registered for notifications in Temperature task\n");
		}
			   
		do
		{
			if((mq_receive(qdes_loc_tempTask, (char *)read_temp_msg_queue, sizeof(msgStruct_t), NULL)) == -1) 
			{
				n = errno;
				printf("tempTaskFunc::ERROR number to receive the data in temp task message queue is %d\n", errno);
			} 
			else
			{
				if(read_temp_msg_queue->msgId == MSGID_TEMP_REQ)												
				{
					send_temp_msg(read_temp_msg_queue->msgSrcTask, temp_msg, qdes_loc_tempTask);
				}
				else if(read_temp_msg_queue->msgId == MSGID_HB_REQ)												
				{
					send_heartBeat(TEMP_TASK_ID,HB_main, qdes_loc_tempMainQueue);				
				}
			}
		printf("tempTaskFunc::n = %d\n",n);
		}while(n != EAGAIN);
		n = 0;
	}	
	
	printf("???????????????????RECEIVED TERMINATING IN TEMPERATURE TASK\n");
	
	
	free(HB_main);
	free(read_temp_msg_queue);
	free(temp_msg);
	free(tempLog);
	free(tempTaskLogMsg);
	
    if (mq_close (qdes_loc_tempTask) == -1) 
    {
        printf("ERROR No %d: Closing the Temperature task queue\n",errno);
    }
    if (mq_close (qdes_loc_tempMainQueue) == -1) 
    {
        printf("ERROR No %d: Closing the Main task queue from temperature task\n",errno);
    }
    if (mq_close (qddes_tempTask_To_Log) == -1) 
    {
        printf("ERROR No %d: Closing the Log task queue from temperature task\n",errno);
    }
    if (mq_unlink (TEMP_TASK_MQ_NAME) == -1) 
    {
        printf("ERROR No: %d Unable to unlink the temperature queue \n", errno);
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

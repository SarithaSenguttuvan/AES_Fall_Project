/* light.c */

#include <stdio.h>
#include <stdlib.h>
#include "light.h"
#include "generic.h"

mqd_t qdes_loc_lightTask;
	
/* Function call to send the light data */
uint8_t send_light_msg(uint8_t destTaskId, msgStruct_t * light_msg)
{
	printf("send_light_msg::In function send light msg\n");
	mqd_t qdes_light;
	char *lightData = "Day";
	light_msg->msgId = MSGID_LIGHT_DATA;
	light_msg->msgSrcTask = LIGHT_TASK_ID;
	light_msg->msgPayload = lightData;
	light_msg->msgPayloadLen = sizeof(* light_msg->msgPayload);

	if(destTaskId == 1)
	{
		if((qdes_light = mq_open(MAIN_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1) 
		{
			printf ("send_light_msg::ERROR No: %d Main queue unable to open\n", errno);
		}
	}
	else if(destTaskId == 2)
	{
		if((qdes_light = mq_open(LOG_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1) 
		{
			printf ("send_light_msg::ERROR No: %d log queue unable to open\n", errno);
		}
	}
	else if(destTaskId == 3)
	{
		if((qdes_light = mq_open(TEMP_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1) 
		{
			printf ("send_light_msg::ERROR No: %d Temperature queue unable to open\n", errno);
		}
	}
	if(mq_send(qdes_light, (char *)light_msg, sizeof(msgStruct_t), 0) == -1) 
    {
    	printf ("send_light_msg::ERROR No: %d queue unable to send light data to the task %d\n", errno, destTaskId);
    }
    else
    	printf("send_light_msg::Light message Sent from send_lightmsg to task %d\n", destTaskId);
    
    if (mq_close (qdes_light) == -1) 
    {
        printf("send_light_msg::ERROR No %d: Closing the Light task queue in send_lightmsg\n",errno);
    }
    return 0;
}

/* Start routine for the Light Task */
void *lightTaskFunc(void *arg)
{
	printf("lightTaskFunc::In the Light task function\n");
	int8_t rc = 0;	
	int8_t n = 0;	
	int32_t recvSig = 0;   
    	
	mq_lightTask_notify.sigev_notify = SIGEV_SIGNAL;						//////////////////////////////////////////////////////
	mq_lightTask_notify.sigev_signo = SIGLIGHT;
	
	if((qdes_loc_lightTask = mq_open(LIGHT_TASK_MQ_NAME, O_NONBLOCK | O_RDONLY)) == (mqd_t)-1) 
	{
    	printf ("lightTaskFunc::ERROR No: %d Unable to open light task\n", errno);
    }	
    
    if(mq_notify (qdes_loc_lightTask, &mq_lightTask_notify) == -1)							/////////////////////////////////////////////
    {
    	printf("lightTaskFunc::The error number in mq_notify in the Light Task is %d\n", errno);
        if(errno == EBUSY)
                printf("lightTaskFunc::Another process has registered for notifications in Light task queue.\n");
    } 
    	
	log_t *lightLog = (log_t *)malloc(sizeof(log_t));
	lightLog->logLevel = LOG_INFO;
	lightLog->log_timestamp = 0;
	lightLog->logPayload = "Light";
	lightLog->logPayloadLen = sizeof(lightLog->logPayload);

	//printf("Sending log from light task\n");
	//send_log(LIGHT_TASK_ID, lightLog);

	msgStruct_t *read_light_msg_queue = (msgStruct_t *)malloc(sizeof(msgStruct_t));
	msgStruct_t *light_msg = (msgStruct_t *)malloc(sizeof(msgStruct_t));
	msgStruct_t *HB_main = (msgStruct_t *)malloc(sizeof(msgStruct_t));	

	blockSignals(0);
	
	while(1)
	{
		
	   		
	   	printf("lightTaskFunc::Waiting for Signals\n");
	  	recvSig = unblockOnSignal(LIGHT_TASK_ID);   
        printf("lightTaskFunc::Received Signal after Sigwait:%d\n", recvSig);
            
        
        if(mq_notify (qdes_loc_lightTask, &mq_lightTask_notify) == -1)							/////////////////////////////////////////////
		{
			printf("lightTask_handler::The error number in mq_notify in the Light Task is %d\n", errno);
		    if(errno == EBUSY)
		            printf("lightTask_handler::Another process has registered for notifications in Light task\n");
		}
		 
		do
		{
			//printf("Entering here\n");
			if((mq_receive(qdes_loc_lightTask, (char *)read_light_msg_queue, sizeof(msgStruct_t), NULL)) == -1) 
			{
				n = errno;
				printf("lightTaskFunc::ERROR number to receive the data in light task message queue is %d\n", errno);
			} 
			else
			{
				if(read_light_msg_queue->msgId == MSGID_LIGHT_REQ)												////////////////////////////////
				{
					//printf("=) Light Req MSG: The Source task ID for the light message queue is %d\n",(read_light_msg_queue->msgSrcTask));
					//printf("=) Light Req MSG: The src_message_id for the light message queue is %d\n",(read_light_msg_queue->msgId));
					send_light_msg(read_light_msg_queue->msgSrcTask, light_msg);
				}
				else if(read_light_msg_queue->msgId == MSGID_HB_REQ)												////////////////////////////////
				{
					send_heartBeat(LIGHT_TASK_ID,HB_main);				
				}
			}
		printf("lightTaskFunc::n = %d\n",n);
		}while(n != EAGAIN);
		n = 0;
	}	
	free(light_msg);
	free(HB_main);
	free(read_light_msg_queue);

    if (mq_close (qdes_loc_lightTask) == -1) 
    {
        printf("ERROR No %d: Closing the Light task queue\n",errno);
    }
    if (mq_unlink (LIGHT_TASK_MQ_NAME) == -1) 
    {
        printf("ERROR No: %d Unable to unlink the light queue \n", errno);
    } 

	free(lightLog);
	pthread_exit(NULL);
}

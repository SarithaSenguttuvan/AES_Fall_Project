/* log.c */

#include <stdio.h>
#include <stdlib.h>
#include "generic.h"
#include "log.h"
#include <stdbool.h>

extern bool sigHandle;
extern char *file_name;
mqd_t qdes_loc_logTask;


/* Start routine for the Log Task */
void *logTaskFunc(void *arg)
{
	printf("logTaskFunc::In the log task function\n");
	/* Message queue Sigaction */
	size_t n = 0;
	int8_t rc = 0;
	int32_t recvSig = 0;
	FILE *fp;
	mqd_t qdes_loc_logMainQueue;
	log_t *read_log_queue = (log_t *)malloc(sizeof(log_t));
	msgStruct_t *read_msg_queue = (msgStruct_t *)malloc(sizeof(msgStruct_t));

	mq_logTask_notify.sigev_notify = SIGEV_SIGNAL;						
	mq_logTask_notify.sigev_signo = SIGLOG;

	if((qdes_loc_logMainQueue = mq_open(MAIN_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1) 
	{
    	printf ("logTaskFunc::ERROR No: %d Unable to open main queue to send hb response\n", errno);
    }

	if((qdes_loc_logTask = mq_open(LOG_TASK_MQ_NAME, O_NONBLOCK | O_RDONLY)) == (mqd_t)-1) 
	{
    	printf ("logTaskFunc::ERROR No: %d Unable to open Log task\n", errno);
    }
    
    if(mq_notify (qdes_loc_logTask, &mq_logTask_notify) == -1)							
    {
    	printf("logTaskFunc::The error number in mq_notify in the Log Task is %d\n", errno);
        if(errno == EBUSY)
                printf("logTaskFunc::Another process has registered for notifications.\n");
    }
	msgStruct_t *HB_main = (msgStruct_t *)malloc(sizeof(msgStruct_t));	

	blockSignals(0);
	#if 1
	fp = fopen(file_name, "w");
    if (fp == NULL) 
    {
      printf("logTaskFunc():: Unable to open the file\n"); 
    }
    #endif
   // fprintf(fp, "%s\n", "Saritha");
	while(!sigHandle)
	{
	   		
		printf("logTaskFunc::Signal Waiting\n");   		
		recvSig = unblockOnSignal(LOG_TASK_ID);   
        printf("logTaskFunc::Received Signal after Sigwait:%d\n", recvSig);
		   
		if(mq_notify (qdes_loc_logTask, &mq_logTask_notify) == -1)							
		{
			printf("logTask_handler::The error number in mq_notify in the Log Task is %d\n", errno);
		    if(errno == EBUSY)
		            printf("logTask_handler::Another process has registered for notifications.\n");
		}   
		   
        do
		{
			printf("logTaskFunc::Entered Receive Message\n");
			if((mq_receive(qdes_loc_logTask, (char*)read_msg_queue, sizeof(msgStruct_t ), NULL)) == -1) 
			{
				n = errno;
				printf("logTaskFunc::ERROR number to receive the data in log task message queue is %d\n", errno);
			} 
			else
			{
				//printf("logTaskFunc:^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
				read_log_queue = (log_t *)(read_msg_queue->msgPayload);

				if(read_msg_queue->msgId == MSGID_LOGMSG)												
				{
					#if 0
					printf("***LOG MSG: The Log Source task ID is %d\n",(read_msg_queue->msgSrcTask));
					printf("***LOG MSG: The Log src_message_id for the main message queue is %d\n",(read_msg_queue->msgId));
					printf("***LOG MSG: The Log payload is %s\n",(char *)(read_log_queue->logPayload));
					printf("***LOG MSG: The Log payload len for the main message queue is %ld\n",(read_msg_queue->msgPayloadLen));
					#endif
					//write to the file
					printf("logTaskFunc:: Writing to the file\n");
					#if 0
						fp = fopen("AES_Project_Log.txt", "a");
						if (fp == NULL) 
						{
						  printf("logTaskFunc():: Unable to open the file\n"); 
						}
					#endif
					fprintf(fp, "\nMsg Id: %d\n",read_msg_queue->msgId);
					fprintf(fp, "Src Task Id: %d\n",read_msg_queue->msgSrcTask);
					fprintf(fp, "Payload: %s\n",(char *)(read_log_queue->logPayload));
					fprintf(fp, "Payload length: %ld\n",read_msg_queue->msgPayloadLen);
					printf("logTaskFunc:: Completed writing to the file\n");
					//fclose(fp);
										
				}
				else if(read_msg_queue->msgId == MSGID_HB_REQ)												
				{				
					send_heartBeat(LOG_TASK_ID,HB_main, qdes_loc_logMainQueue);				
				}
			}
		printf("logTaskFunc::n=%ld\n",n);
		}while(n != EAGAIN);
		n = 0;
	}
	
	printf("???????????????????RECEIVED TERMINATING IN LOG TASK\n");
	
	fclose(fp);
	free(HB_main);
	free(read_msg_queue);
	free(read_log_queue);
    
    if (mq_close (qdes_loc_logTask) == -1) 
    {
        printf("ERROR No %d: Closing the Log task queue\n",errno);
    }

    if (mq_close (qdes_loc_logMainQueue) == -1) 
    {
        printf("ERROR No %d: Closing the Main task queue from Log task\n",errno);
    }
    
    if (mq_unlink (LOG_TASK_MQ_NAME) == -1) 
    {
        printf("ERROR No: %d Unable to unlink the log queue \n", errno);
    }
    if (mq_unlink (MAIN_TASK_MQ_NAME) == -1) 
    {
        printf("ERROR No: %d Unable to unlink the main queue \n", errno);
    }
      
	pthread_exit(NULL);
}


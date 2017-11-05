/* log.c */

#include <stdio.h>
#include <stdlib.h>
#include "generic.h"
#include "log.h"

mqd_t qdes_loc_logTask;

	
/* Defining my owm handler to handle the system signals */
void logTask_handler(int signum)
{
	int8_t rc = 0;
    printf("logTask_handler::Log Task Signal Handler%d : %ld\n",signum, syscall(SYS_gettid));
	if(signum == SIGLOG)
	{
    	printf("logTask_handler::Received SIGLOG signal in the Log task\n");
		if(mq_notify (qdes_loc_logTask, &mq_logTask_notify) == -1)							/////////////////////////////////////////////
		{
			printf("logTask_handler::The error number in mq_notify in the Log Task is %d\n", errno);
		    if(errno == EBUSY)
		            printf("logTask_handler::Another process has registered for notifications.\n");
		}
		rc = pthread_cond_signal(&cond_log);                    			/* Log task handler sends the condition signal */   
		if (rc) 
		{
			printf("logTask_handler::ERROR in Log task: pthread_cond_signal() rc is %d\n", rc); 
		}   
		printf("logTask_handler::Cond Signaled\n");
    } 
    return;
}

/* Start routine for the Log Task */
void *logTaskFunc(void *arg)
{
	printf("logTaskFunc::In the log task function\n");
	/* Message queue Sigaction */
	size_t n = 0;
	int8_t rc = 0;
	int32_t recvSig = 0;
	
	log_t *read_log_queue = (log_t *)malloc(sizeof(log_t));
	msgStruct_t *read_msg_queue = (msgStruct_t *)malloc(sizeof(msgStruct_t));
	
	mq_logTask_sa.sa_handler = logTask_handler; /* Assigning the signal handler function */
    //sigaction(SIGLOG, &mq_logTask_sa, NULL);  /* Registering the signals */ 

	mq_logTask_notify.sigev_notify = SIGEV_SIGNAL;						
	mq_logTask_notify.sigev_signo = SIGLOG;

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
    rc = pthread_cond_init(&cond_log, NULL);           /* initializing the pthread condition variable */
    if(rc)
    	printf("logTaskFunc::condition init error\n");	
	
	blockSignals(0);
	
	while(1)
	{
	   		
		printf("logTaskFunc::Signal Waiting\n");   		
		recvSig = unblockOnSignal(LOG_TASK_ID);   
        printf("logTaskFunc::Received Signal after Sigwait:%d\n", recvSig);
		   
		if(mq_notify (qdes_loc_logTask, &mq_logTask_notify) == -1)							/////////////////////////////////////////////
		{
			printf("logTask_handler::The error number in mq_notify in the Log Task is %d\n", errno);
		    if(errno == EBUSY)
		            printf("logTask_handler::Another process has registered for notifications.\n");
		}   
		   
        do
		{
			printf("logTaskFunc:Entered Receive Message\n");
			if((mq_receive(qdes_loc_logTask, (char*)read_msg_queue, sizeof(msgStruct_t ), NULL)) == -1) 
			{
				n = errno;
				printf("logTaskFunc::ERROR number to receive the data in log task message queue is %d\n", errno);
			} 
			else
			{
				read_log_queue = (log_t *)(read_msg_queue->msgPayload);

				if(read_msg_queue->msgId == MSGID_LOGMSG)												////////////////////////////////
				{
					printf("***LOG MSG: The Log Source task ID is %d\n",(read_msg_queue->msgSrcTask));
					printf("***LOG MSG: The Log src_message_id for the main message queue is %d\n",(read_msg_queue->msgId));
					printf("***LOG MSG: The Log payload is %s\n",(char *)(read_log_queue->logPayload));
					printf("***LOG MSG: The Log payload len for the main message queue is %ld\n",(read_msg_queue->msgPayloadLen));					
				}
				else if(read_msg_queue->msgId == MSGID_HB_REQ)												////////////////////////////////
				{				
					send_heartBeat(LOG_TASK_ID,HB_main);				
				}
			}
		printf("logTaskFunc::n=%ld\n",n);
		}while(n != EAGAIN);
		n = 0;
	}
	free(HB_main);
	free(read_msg_queue);
	free(read_log_queue);
    
    if (mq_close (qdes_loc_logTask) == -1) 
    {
        printf("ERROR No %d: Closing the Log task queue\n",errno);
    }
    if (mq_unlink (LOG_TASK_MQ_NAME) == -1) 
    {
        printf("ERROR No: %d Unable to unlink the log queue \n", errno);
    }

      
	pthread_exit(NULL);
}


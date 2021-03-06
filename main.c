/* main.c */

#define _GNU_SOURCE
#include "main.h"
#include "log.h"
#include "temp.h"
#include "light.h"
#include "generic.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

bool sigHandle = false;
char *file_name;

/* Defining my owm handler to handle the system signals */
void my_handler(int signum)
{
    if((signum == SIGINT) || (signum == SIGTSTP) )               /* If SIGTERM signal */
    {
		FILE *LEDHandle = NULL;
		char *LEDBrightness = "/sys/class/leds/beaglebone:green:usr1/brightness";

		if((LEDHandle = fopen(LEDBrightness, "r+")) != NULL){
		 fwrite("1", sizeof(char), 1, LEDHandle);
		 fclose(LEDHandle);
		}
        printf("Received SIGTERM!\n");
        pthread_kill(pthread_self(), SIGTIMER);
        pthread_kill(logTask, SIGLOG);
        pthread_kill(tempTask, SIGTEMP);
        pthread_kill(lightTask, SIGLIGHT);
		sigHandle = true;
    }
}
	
/*main task that spawns others tassks*/
int main(int argc, char *argv[])
{
	int rc = 0;
	size_t n = 0;
	int32_t recvSig = 0;

	mqd_t qdes_mainTask;
	mqd_t qdes_logTask;
	mqd_t qdes_tempTask;
	mqd_t qdes_lightTask;
	
	bool log_flag = true;
	bool temp_flag = true;
	bool light_flag = true;

	timer_t timerid;
    struct itimerspec its;
	char main_queue_data[100];
	blockSignals(0);
	char logPayloadBuff[200];
    
    /* Message and log packet */     
	log_t *mainLog = (log_t *)malloc(sizeof(log_t));
	log_t *mainInitLog = (log_t *)malloc(sizeof(log_t));	
	msgStruct_t *read_queue = (msgStruct_t *)malloc(sizeof(msgStruct_t));	
	msgStruct_t *HB_req = (msgStruct_t *)malloc(sizeof(msgStruct_t));    
	msgStruct_t *mainTaskLogMsg = (msgStruct_t *)malloc(sizeof(msgStruct_t)); 
	msgStruct_t *mainTaskInitMsg = (msgStruct_t *)malloc(sizeof(msgStruct_t)); 	
	
	struct sigaction my_sig;		/* Registering signal for Terminating signals */
	
	file_name = argv[1];
	
    my_sig.sa_handler = my_handler; /* Assigning the signal handler function */
    sigaction(SIGINT, &my_sig, NULL);  /* Registering the signals */ 
    sigaction(SIGTSTP, &my_sig, NULL);  /* Registering the signals */ 
    	
    /* sigevents registeration for SIGTIMER and SIGMAIN */	
	mq_mainTask_notify.sigev_notify = SIGEV_SIGNAL;						
	mq_mainTask_notify.sigev_signo = SIGMAIN;	    
   	mq_mainTask_timer.sigev_notify = SIGEV_SIGNAL;
    mq_mainTask_timer.sigev_signo = SIGTIMER;
 
    /* Setting timer value */
    its.it_value.tv_sec = 2;
    its.it_value.tv_nsec = 100 % 1000000000;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;
    
    /* Timer create */ 
    if (timer_create(CLOCK_REALTIME, &mq_mainTask_timer, &timerid) == -1)
        printf("main::ERROR: timer_create\n");
    printf("main::Timer Created\n");

	/* Main task message queue config and create, notification config and register */ 
    main_queue_attr.mq_maxmsg = 30;
	main_queue_attr.mq_msgsize = sizeof(msgStruct_t);        
	if((qdes_mainTask = mq_open(MAIN_TASK_MQ_NAME, O_RDONLY | O_CREAT | O_NONBLOCK, S_IRWXU, &main_queue_attr)) == -1) 
    {
    	printf("main::ERROR number in opening the main task queue is %d\n", errno);
    } 
	printf("main::Main task message queue opened\n");	

	/* Registering notification for message queues */ 
    if(mq_notify (qdes_mainTask, &mq_mainTask_notify) == -1)							
    {
    	printf("main::The error number in mq_notify in the Main Task is %d\n", errno);
        if(errno == EBUSY)
                printf("main::Another process has pthread_kill(pthread_self(), SIGTIMER);registered for notifications for Main queue.\n");
    }  
    
    /* Log task message queue config and create */ 
	log_queue_attr.mq_maxmsg = 30;
	log_queue_attr.mq_msgsize = sizeof(msgStruct_t);
	if((qdes_logTask = mq_open(LOG_TASK_MQ_NAME, O_RDWR | O_CREAT | O_NONBLOCK, S_IRWXU, &log_queue_attr)) == -1) 
    {
    	printf("main::ERROR number to open the log task name is %d\n", errno);
    }	
    printf("main::Log task message queue opened\n");	
     
    /* Temperature task message queue config and create */ 
	temp_queue_attr.mq_maxmsg = 30;
	temp_queue_attr.mq_msgsize = sizeof(msgStruct_t);
	if((qdes_tempTask = mq_open(TEMP_TASK_MQ_NAME, O_RDWR | O_CREAT | O_NONBLOCK, S_IRWXU, &temp_queue_attr)) == -1) 
    {
    	printf("main::ERROR number to open the temp task name is %d\n", errno);
    }	
    printf("main::temp task message queue opened\n");	
    
    /* Light task message queue config and create */ 
	light_queue_attr.mq_maxmsg = 10;
	light_queue_attr.mq_msgsize = sizeof(msgStruct_t);
	if((qdes_lightTask = mq_open(LIGHT_TASK_MQ_NAME, O_RDWR | O_CREAT | O_NONBLOCK, S_IRWXU, &light_queue_attr)) == -1) 
    {
    	printf("main::ERROR number to open the light task name is %d\n", errno);
    }	
    printf("main::Light task message queue opened\n");	
 
    /* Initialising the thread attributes */
    pthread_attr_init(&logTask_attr);
    pthread_attr_init(&tempTask_attr);	
    pthread_attr_init(&lightTask_attr);
    
    blockSignals(0);			/* Blocking all the signals */
            
    rc = pthread_create(&logTask, &logTask_attr, logTaskFunc, NULL);  		/* Create log thread  */
    if(rc) 
    {
        printf("main::ERROR: pthread_create() Log task, rc is %d\n", rc); 
        exit(-1);
    }	

    rc = pthread_create(&tempTask, &tempTask_attr, tempTaskFunc, NULL);  		/* Create Temperatue thread  */
    if(rc) 
    {
        printf("main::ERROR: pthread_create() Temp task, rc is %d\n", rc); 
        exit(-1);
    }

    rc = pthread_create(&lightTask, &lightTask_attr, lightTaskFunc, NULL);  		/* Create Light thread  */
    if(rc) 
    {
        printf("main::ERROR: pthread_create() Light task, rc is %d\n", rc); 
        exit(-1);
    }
    sleep(2);    

	/* Initializing the msg packet values */
	mainInitLog->logLevel = LOG_INIT;
	mainInitLog->log_timestamp = time(NULL);
	mainInitLog->logPayload = "Main task Initialized";
	mainInitLog->logPayloadLen = sizeof(mainLog->logPayload);
	send_log(MAIN_TASK_ID, mainInitLog, mainTaskInitMsg, qdes_logTask);

	/* Initializing the msg packet values */
	mainLog->logLevel = LOG_INFO;
	mainLog->log_timestamp = time(NULL);
	sprintf(logPayloadBuff, "Log Level is %d  Time is %d  Received Heartbeats", mainLog->logLevel, mainLog->log_timestamp); 
	mainLog->logPayload = logPayloadBuff;
	mainLog->logPayloadLen = sizeof(mainLog->logPayload);

	/* Start the Timer */
    if (timer_settime(timerid, 0, &its, NULL) == -1)
         printf("main::ERRROR: timer_settime\n");
 
 	send_light_req(MAIN_TASK_ID, mainTaskLogMsg, qdes_lightTask);        	
	while(!sigHandle)				/* Run continuously */
	{	
		recvSig = unblockOnSignal(MAIN_TASK_ID);   				/* unblock on timer and mq_notify signal */
        printf("main:::Received Signal after Sigwait:%d\n", recvSig);
        
        if(recvSig == SIGTIMER)			/* If timer elapses */
        {
        	
        	//send_hb_req(HB_req, qdes_logTask, qdes_tempTask, qdes_lightTask);        	       	
        	printf("main::Inside SigTimer IF block\n");
       		if(log_flag == true && temp_flag == true && light_flag == true)			/* If heartbeats received from all tasks */
            {
                log_flag = false;
                temp_flag = false;
                light_flag = false;
                //make_packet(LOG_INFO, mainLog);
                send_log(MAIN_TASK_ID, mainLog, mainTaskLogMsg, qdes_logTask);
                send_hb_req(HB_req, qdes_logTask, qdes_tempTask, qdes_lightTask);
                //send_temp_req(MAIN_TASK_ID, mainTaskLogMsg, qdes_tempTask);
                printf("main::Inside SigTimer IF block\n");    
            }
        	else					/* If heartbeats not received */
            {
                if(log_flag == false)
                {
                    printf("HB not received by the log task\n");
                }
                else if(temp_flag == false)
                {
                    printf("HB not received by the temp task\n");
                }
                else if(light_flag == false)
                {
                    printf("HB not received by the light task\n");
                }
                raise(SIGINT);
            }
        }
        else if(recvSig == SIGMAIN)					/* If data received on main task message queue */
        { 	
        	printf("main::Inside SigMain IF block\n");
        	if(mq_notify (qdes_mainTask, &mq_mainTask_notify) == -1)					/* Re-registering for message queus */		
			{
				printf("main::Error Registering Notify SIGMAIN: %d\n", errno);
				if(errno == EBUSY)
					printf("main::Another process has registered for notifications using SIGMAIN.\n");
			} 	
			do
			{
				printf("main::Entering to read message\n");
				
				if((mq_receive(qdes_mainTask, (char *)read_queue, sizeof(msgStruct_t), NULL)) == -1) 		/* receive from queue */
				{
					n = errno;
					printf("main::ERROR number to receive the data in main task message queue is %d\n", errno);
				} 
				else
				{
					if(read_queue->msgId == MSGID_HB_RESP)							 /* if heartbeat response */				
					{
						printf("main::HB MSG: The Source task ID is %d\n",(read_queue->msgSrcTask));
						if(read_queue->msgSrcTask == LOG_TASK_ID)
							log_flag = true;
						else if(read_queue->msgSrcTask == TEMP_TASK_ID)
							temp_flag = true;
						else if(read_queue->msgSrcTask == LIGHT_TASK_ID)
							light_flag = true;							
					}
					else if(read_queue->msgId == MSGID_LIGHT_DATA)
					{
						printf("main::LIGHT_DATA %s\n",(char*)(read_queue->msgPayload));
						
					}
					else if(read_queue->msgId == MSGID_TEMP_DATA)
					{
						printf("main::TEMPERATURE_DATA %s\n", (char *)(read_queue->msgPayload));
					}								
				}
				
			printf("main::n=%ld\n",n);
			}while(n != EAGAIN);
			n = 0;
		}
	}
	printf("RECEIVED TERMINATING IN MAIN\n");

/* Free all the dynamic memory */
	free(HB_req);
	free(mainLog);
	free(mainTaskLogMsg);
	free(read_queue);
	free(mainInitLog);
	    
    if(pthread_join(logTask, NULL) == 0)      /* Wait until log thread has completed execution*/
       printf("Log task done\n");
    else
       printf("Log task Error\n");   
    
    if(pthread_join(tempTask, NULL) == 0)      /* Wait until temperature thread has completed execution*/
       printf("Temp task done\n");
    else
       printf("Temp task Error\n"); 
       
    if(pthread_join(lightTask, NULL) == 0)      /* Wait until light thread has completed execution*/
       printf("Light task done\n");
    else
       printf("Light task Error\n"); 
 
 
/* Close the message queues */
    if (mq_close (qdes_mainTask) == -1) 
    {
    	printf("ERROR No: %d Unable to close the main queue \n", errno);
    }	   
    if (mq_close (qdes_logTask) == -1) 
    {
        printf("main::ERROR No %d: Closing the Log task queue in main\n",errno);
    }
 
    if (mq_close (qdes_tempTask) == -1) 
    {
        printf("main::ERROR No %d: Closing the Temperature task queue in main\n",errno);
    }
 
    if (mq_close (qdes_lightTask) == -1) 
    {
        printf("main::ERROR No %d: Closing the Light task queue in main\n",errno);
    } 
/* Unlink the message queues */    
    if (mq_unlink (MAIN_TASK_MQ_NAME) == -1) 
    {
        printf("ERROR No: %d Unable to unlink the main queue \n", errno);
    }  
    if (mq_unlink (LOG_TASK_MQ_NAME) == -1) 
    {
        printf("ERROR No: %d Unable to unlink the log queue \n", errno);
    }
    if (mq_unlink (TEMP_TASK_MQ_NAME) == -1) 
    {
        printf("ERROR No: %d Unable to unlink the temperature queue \n", errno);
    }         
    if (mq_unlink (LIGHT_TASK_MQ_NAME) == -1) 
    {
        printf("ERROR No: %d Unable to unlink the light queue \n", errno);
    }   
    
    printf("EXITED NORMALLY\n"); 
   	return 0;	
}


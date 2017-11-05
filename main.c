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


mqd_t qdes_mainTask;

/* Defining my owm handler to handle the system signals */
void mainTask_handler(int signum)
{
	int8_t rc = 0;
    mainSigNo = signum;
    printf("mainTask_handler::Signal number is %d : %ld\n", mainSigNo, syscall(SYS_gettid));
	if(signum == SIGMAIN)
	{
    	printf("mainTask_handler::Received SIGMAIN\n");
		if(mq_notify (qdes_mainTask, &mq_mainTask_notify) == -1)							
		{
			printf("mainTask_handler::Error Registering Notify SIGMAIN: %d\n", errno);
		    if(errno == EBUSY)
		    	printf("mainTask_handler::Another process has registered for notifications using SIGMAIN.\n");
		}      	
    }
	else if(signum == SIGTIMER)
	{
    	printf("mainTask_handler::Received SIGTIMER\n");    	
    }
     
    rc = pthread_cond_signal(&cond_main);                    			/* Main task handler sends the condition signal */   
    if (rc) 
    {
    	printf("mainTask_handler::ERROR: pthread_cond_signal() rc is %d\n", rc);             
    }   
    
    printf("mainTask_handler::Condition Variable Signaled\n"); 
}

/*main task that spawns others tassks*/
int main()
{
	int rc = 0;
	size_t n = 0;
	int32_t recvSig = 0;
	mqd_t qdes_logTask;
	mqd_t qdes_tempTask;
	mqd_t qdes_lightTask;
	timer_t timerid;
    struct itimerspec its;
	char main_queue_data[100];
	blockSignals(0);
	msgStruct_t *read_queue = (msgStruct_t *)malloc(sizeof(msgStruct_t));
	

    rc = pthread_cond_init(&cond_main, NULL);           /* initializing the pthread condition variable */
    if(rc)
    	printf("main::condition init error\n");	
	rc = pthread_mutex_init(&main_mtx, NULL);        /* Set default protocol for mutex */
    if(rc)
        printf("main::Mutex init in Main task error\n");

	/* Timer config */      
    timer_sa.sa_handler = mainTask_handler;	
	//sigaction(SIGTIMER, &timer_sa, NULL);  /* Registering the signals */

	mq_mainTask_notify.sigev_notify = SIGEV_SIGNAL;						
	mq_mainTask_notify.sigev_signo = SIGMAIN;
	    
   	mq_mainTask_timer.sigev_notify = SIGEV_SIGNAL;
    mq_mainTask_timer.sigev_signo = SIGTIMER;
    
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
	if((qdes_mainTask = mq_open(MAIN_TASK_MQ_NAME, O_RDONLY | O_CREAT | O_NONBLOCK, READ_QUEUE_PERMISSIONS, &main_queue_attr)) == -1) 
    {
    	printf("main::ERROR number in opening the main task queue is %d\n", errno);
    } 
	printf("main::Main task message queue opened\n");	
	
    mq_mainTask_sa.sa_handler = mainTask_handler; /* Assigning the signal handler function */
    //sigaction(SIGMAIN, &mq_mainTask_sa, NULL);  /* Registering the signals */ 

    if(mq_notify (qdes_mainTask, &mq_mainTask_notify) == -1)							
    {
    	printf("main::The error number in mq_notify in the Main Task is %d\n", errno);
        if(errno == EBUSY)
                printf("main::Another process has registered for notifications for Main queue.\n");
    }  
    
    /* Log task message queue config and create */ 
	log_queue_attr.mq_maxmsg = 30;
	log_queue_attr.mq_msgsize = sizeof(msgStruct_t);
	if((qdes_logTask = mq_open(LOG_TASK_MQ_NAME, O_RDONLY | O_CREAT | O_NONBLOCK, READ_QUEUE_PERMISSIONS, &log_queue_attr)) == -1) 
    {
    	printf("main::ERROR number to open the log task name is %d\n", errno);
    }	
    printf("main::Log task message queue opened\n");	
    
    if (mq_close (qdes_logTask) == -1) 
    {
        printf("main::ERROR No %d: Closing the Log task queue in main\n",errno);
    }
   
#if 1        
    /* Temperature task message queue config and create */ 
	temp_queue_attr.mq_maxmsg = 30;
	temp_queue_attr.mq_msgsize = sizeof(msgStruct_t);
	if((qdes_tempTask = mq_open(TEMP_TASK_MQ_NAME, O_RDONLY | O_CREAT | O_NONBLOCK, READ_QUEUE_PERMISSIONS, &temp_queue_attr)) == -1) 
    {
    	printf("main::ERROR number to open the temp task name is %d\n", errno);
    }	
    printf("main::temp task message queue opened\n");	
    
    if (mq_close (qdes_tempTask) == -1) 
    {
        printf("main::ERROR No %d: Closing the Temperature task queue in main\n",errno);
    }
    
    /* Light task message queue config and create */ 
	light_queue_attr.mq_maxmsg = 10;
	light_queue_attr.mq_msgsize = sizeof(msgStruct_t);
	if((qdes_lightTask = mq_open(LIGHT_TASK_MQ_NAME, O_RDONLY | O_CREAT | O_NONBLOCK, READ_QUEUE_PERMISSIONS, &light_queue_attr)) == -1) 
    {
    	printf("main::ERROR number to open the light task name is %d\n", errno);
    }	
    printf("main::Light task message queue opened\n");	
    
    if (mq_close (qdes_lightTask) == -1) 
    {
        printf("main::ERROR No %d: Closing the Light task queue in main\n",errno);
    }
       
#endif 
    /* Initialising the thread attributes */
    pthread_attr_init(&logTask_attr);
    pthread_attr_init(&tempTask_attr);	
    pthread_attr_init(&lightTask_attr);
    
    blockSignals(0);
            
    rc = pthread_create(&logTask, &logTask_attr, logTaskFunc, NULL);  		/* Create log thread  */
    if(rc) 
    {
        printf("main::ERROR: pthread_create() Log task, rc is %d\n", rc); 
        exit(-1);
    }	
    sleep(2);
#if 1 
    rc = pthread_create(&tempTask, &tempTask_attr, tempTaskFunc, NULL);  		/* Create Temperatue thread  */
    if(rc) 
    {
        printf("main::ERROR: pthread_create() Temp task, rc is %d\n", rc); 
        exit(-1);
    }
#endif
#if 1 
    rc = pthread_create(&lightTask, &lightTask_attr, lightTaskFunc, NULL);  		/* Create Light thread  */
    if(rc) 
    {
        printf("main::ERROR: pthread_create() Light task, rc is %d\n", rc); 
        exit(-1);
    }
    sleep(2);    
#endif
    
    /* Message log packet */     
	log_t *mainLog = (log_t *)malloc(sizeof(log_t));
	mainLog->logLevel = LOG_INFO;
	mainLog->log_timestamp = 0;
	mainLog->logPayload = "Main";
	mainLog->logPayloadLen = sizeof(mainLog->logPayload);

	//printf("Sending log from Main task\n");
	//send_log(MAIN_TASK_ID, mainLog);

	//printf("Sending light req from main task\n");
    //send_light_req(MAIN_TASK_ID);
	msgStruct_t *HB_req = (msgStruct_t *)malloc(sizeof(msgStruct_t));    

	/* Set the Timer */
    if (timer_settime(timerid, 0, &its, NULL) == -1)
         printf("main::ERRROR: timer_settime\n");
         	
	while(1)
	{	
				
		printf("main::Waiting for pthread condition signal from the main task handler\n");
        
        //rc = pthread_cond_wait(&cond_main, &main_mtx);                      /* Pthread waits for a condition, and releases the mutex until condition ha met */
        /*if(rc)
            printf("main::Main task pthread_cond_wait error\n");*/
		recvSig = unblockOnSignal(MAIN_TASK_ID);   
        printf("main:::Received Signal after Sigwait:%d\n", recvSig);
        
        if(recvSig == SIGTIMER)
        {
        	
        	send_hb_req(HB_req);
        	       	
        	printf("main::Inside SigTimer IF block\n");
        }
        else if(recvSig == SIGMAIN)
        { 	
        	printf("main::Inside SigMain IF block\n");
        	if(mq_notify (qdes_mainTask, &mq_mainTask_notify) == -1)							
			{
				printf("main::Error Registering Notify SIGMAIN: %d\n", errno);
				if(errno == EBUSY)
					printf("main::Another process has registered for notifications using SIGMAIN.\n");
			} 	
			do
			{
				printf("main::Entering to read message\n");
				
				if((mq_receive(qdes_mainTask, (char *)read_queue, sizeof(msgStruct_t), NULL)) == -1) 
				{
					n = errno;
					printf("main::ERROR number to receive the data in main task message queue is %d\n", errno);
				} 
				else
				{
					//read_queue = (msgStruct_t *)main_queue_data;
					if(read_queue->msgId == MSGID_HB_RESP)												////////////////////////////////
					{
						printf("main::********************************************HB MSG: The Source task ID is %d\n",(read_queue->msgSrcTask));
					}
					else if(read_queue->msgId == MSGID_LIGHT_DATA)
					{
						printf("main::LIGHT_DATA %s\n",(char*)(read_queue->msgPayload));
					}
					else if(read_queue->msgId == MSGID_TEMP_DATA)
					{
						printf("main::TEMPERATURE_DATA %s\n",(char*)(read_queue->msgPayload));
					}								
				}
				
			printf("main::n=%ld\n",n);
			}while(n != EAGAIN);
			n = 0;
		}
	}

	free(HB_req);
	    
    if(pthread_join(logTask, NULL) == 0)      /* Wait until log thread has completed execution*/
       printf("Log task done\n");
    else
       printf("Log task Error\n");   
    
    if(pthread_join(tempTask, NULL) == 0)      /* Wait until log thread has completed execution*/
       printf("Temp task done\n");
    else
       printf("Temp task Error\n"); 
       
    if(pthread_join(lightTask, NULL) == 0)      /* Wait until log thread has completed execution*/
       printf("Light task done\n");
    else
       printf("Light task Error\n"); 
 
 
	if (mq_unlink (MAIN_TASK_MQ_NAME) == -1) 
    {
        printf("ERROR No: %d Unable to unlink the main queue \n", errno);
    }  
    
    if (mq_close (qdes_mainTask) == -1) 
    {
    	printf("ERROR No: %d Unable to close the main queue \n", errno);
    }	
    
    rc = pthread_cond_destroy(&cond_main);       /* Destory the condition variable */
    if (rc) 
    {
        printf("ERROR; pthread_cond_destroy() rc is %d\n", rc); 
    }
	return 0;	
}


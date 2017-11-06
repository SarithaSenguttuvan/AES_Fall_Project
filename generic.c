/* generic.c */
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include "generic.h"
#include "main.h"

/* Function call to send the heartBeat signal */
uint8_t send_hb_req(msgStruct_t * HB_req, mqd_t qdes_req_hb_log, mqd_t qdes_req_hb_temp, mqd_t qdes_req_hb_light)
{
	printf("send_hb_req::In function send request for heartBeat\n");
	
	HB_req->msgId = MSGID_HB_REQ;
	HB_req->msgSrcTask = MAIN_TASK_ID;
	HB_req->msgPayload = NULL;
	HB_req->msgPayloadLen = 0;

    
	if(mq_send(qdes_req_hb_log, (char *)HB_req, sizeof(msgStruct_t), 0) == -1) 			/* Send to Log task */
    {
    	printf ("send_hb_req::ERROR No: %d queue unable to send heart beat request to Log task\n", errno);
    }
    else
    	printf("send_hb_req::HB message request sent to Log task \n");  

	if(mq_send(qdes_req_hb_light, (char *)HB_req, sizeof(msgStruct_t), 0) == -1) 			/* Send to Light task */
    {
    	printf ("send_hb_req::ERROR No: %d queue unable to send heart beat request to Light task\n", errno);
    }
    else
    	printf("send_hb_req::HB message request sent to Light task \n");    
    
	if(mq_send(qdes_req_hb_temp, (char *)HB_req, sizeof(msgStruct_t), 0) == -1) 			/* Send to Temperature task */
    {
    	printf ("send_hb_req::ERROR No: %d queue unable to send heart beat request to Temperature task\n", errno);
    }
    else
    	printf("send_hb_req::HB message request sent to Temperature task \n");           	
    return 0;
}
/* Function call to send the heartBeat signal */
uint8_t send_heartBeat(int8_t srcTaskId, msgStruct_t *HB_main, mqd_t mq_des_hb_resp)
{
	int8_t rc = 0;
	printf("send_heartBeat::In function send HB\n");
	//mqd_t qdes_hb;
	
	HB_main->msgId = MSGID_HB_RESP;
	HB_main->msgSrcTask = srcTaskId;
	HB_main->msgPayload = NULL;
	HB_main->msgPayloadLen = 0;
           
	if(mq_send(mq_des_hb_resp, (char *)HB_main, sizeof(msgStruct_t), 0) == -1) 		/* Send heartbeat to main task */
    {
    	printf ("send_heartBeat::ERROR No: %d queue unable to send heart beat to the Main task from task number %d\n", errno, srcTaskId);
    }
    else
    	printf("send_heartBeat::HB Sent %d\n", srcTaskId);  
    return 0;
}

/* Send log to the log task */
uint8_t send_log(uint8_t srcTaskId, log_t* logPacket, msgStruct_t *msg, mqd_t msg_qdes)   
{
	struct mq_attr *check_attr;
	
	msg->msgId = MSGID_LOGMSG;
	msg->msgSrcTask = srcTaskId;
	msg->msgPayload = logPacket;
	msg->msgPayloadLen = sizeof(msg->msgPayload);

    if(mq_send(msg_qdes, (char *)msg, sizeof(msgStruct_t), 0) == -1) 
    {
    	printf ("send_log::ERROR No: %d queue unable to send Log message to the Log task from task number %d\n", errno, srcTaskId);
    }
    else
    	printf("send_log::Log message Sent from send_log by task %d\n", srcTaskId);

	return 0;
}

/* Send light request to the light task */
uint8_t send_light_req(uint8_t srcTaskId, msgStruct_t *light_req, mqd_t qdes_light_req)
{
	printf("send_light_req::Sending Light request from send_light_req\n");
	light_req->msgId = MSGID_LIGHT_REQ;
	light_req->msgSrcTask = srcTaskId;
	light_req->msgPayload = NULL;
	light_req->msgPayloadLen = 0;	

	if(mq_send(qdes_light_req, (char *)light_req, sizeof(msgStruct_t), 0) == -1) 		/* Send to light task */
    {
    	printf ("send_light_req::ERROR No: %d queue unable to send light request to the Light task\n", errno);
    }
    else
    	printf("send_light_req::Light request sent from task %d\n", srcTaskId);

    return 0;
} 

/* Send temperature request to the light task */
uint8_t send_temp_req(uint8_t srcTaskId, msgStruct_t *temp_req, mqd_t qdes_temp_req)
{
	printf("send_temp_req::Sendign temperature request\n");	
	temp_req->msgId = MSGID_TEMP_REQ;
	temp_req->msgSrcTask = srcTaskId;
	temp_req->msgPayload = NULL;
	temp_req->msgPayloadLen = 0;

	if(mq_send(qdes_temp_req, (char *)temp_req, sizeof(msgStruct_t), 0) == -1) 		/* send to temp task */
    {
    	printf ("send_temp_req::ERROR No: %d queue unable to send temperature request to the tempertuare task\n", errno);
    }
    else
    	printf("send_temp_req::Temperature request sent from task %d\n", srcTaskId);
    
    return 0;		
}

/* Function to block all the signals */
void blockSignals(int taskID)
{
	int8_t rc = 0;
	sigset_t globalMask_set;
	sigemptyset(&globalMask_set);
	sigaddset(&globalMask_set, SIGMAIN);
	sigaddset(&globalMask_set, SIGTIMER);
	sigaddset(&globalMask_set, SIGLIGHT);
	sigaddset(&globalMask_set, SIGTEMP);
	sigaddset(&globalMask_set, SIGLOG);			
	rc = pthread_sigmask(SIG_BLOCK, &globalMask_set, NULL);
	if (rc != 0)
   		printf("main::Sigmask Block error\n");
	printf("blockSignals::Blocked Signals\n");
}

/* Unblock the signals based on the task that requests */
int32_t unblockOnSignal(int taskID)
{
	int8_t rc = 0;
	int32_t sigNo = 0;
	sigset_t globalMask_set;
	sigemptyset(&globalMask_set);
	if(taskID == MAIN_TASK_ID)				/* if main task */	
	{
		sigaddset(&globalMask_set, SIGMAIN);
		sigaddset(&globalMask_set, SIGTIMER);
		printf("unblockSignals::Main\n");
	}
	else if(taskID == LOG_TASK_ID)		/* If log task */
	{
		sigaddset(&globalMask_set, SIGLOG);
		printf("unblockSignals::LOG\n");
	}
	else if(taskID == TEMP_TASK_ID)		/* if temperature rask */
	{
		sigaddset(&globalMask_set, SIGTEMP);
		printf("unblockSignals::Temp\n");
	}
	else if(taskID == LIGHT_TASK_ID)		/* if light task */
	{
		sigaddset(&globalMask_set, SIGLIGHT);
		printf("unblockSignals::Light\n");
	}
	
   	if(sigwait(&globalMask_set, &sigNo))			/* wait on the signals based on the signal mask */
   	{
   		printf("sigwait_error in TaskId %d : %d", taskID, errno);
   		return -1;
   	}
   	else
   	{
   		return sigNo;			/* Return the signal number */
   	}
}


#include "amsfunc.h"


void *ServiceProcTask(void *pThreadId)
{
	int	iThread         = (int)pThreadId;
	cpu_set_t           mask;  
	cpu_set_t           get; 
	int                 cpuNum = 1;
	int                 cpuId = 0;
	int                 i = 0;

	Display("AMS ServiceProcTask[%d] started\r\n", iThread);

	//��CPU SET 
	cpuNum = sysconf(_SC_NPROCESSORS_CONF);//ͳ��cpu����   
	dbgprint("ams has %d processor(s)\n", cpuNum);  
	if(cpuNum <= 0)
	{
		Display("AMS-ServiceProcTask[%d] cpu num[%d] error!\n", iThread, cpuNum);
		return NULL;
	}

	// sched_setaffinity
	if(cpuNum <= MAX_MSG_PROCESSORS)
	{
		cpuId = (iThread%cpuNum);
	}
	else
	{
		cpuId = iThread%(cpuNum - MAX_MSG_PROCESSORS);
		cpuId += MAX_MSG_PROCESSORS;
	}

	CPU_ZERO(&mask);
	CPU_SET(cpuId, &mask);
	if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &mask) < 0) 
	{
		Display("AMS-ServiceProcTask[%d] set thread affinity cpu[%d] failed\n", iThread, cpuId); 
	}
	else
	{
		dbgprint("AMS-ServiceProcTask[%d] set thread affinity cpu[%d] success\n", iThread, cpuId); 
	}

	/*check setaffinity*/
	CPU_ZERO(&get);  
	if (pthread_getaffinity_np(pthread_self(), sizeof(get), &get) < 0) 
	{  
		Display("AMS-ServiceProcTask[%d] get thread affinity failed\n", iThread);
	}  
	for (i = 0; i < cpuNum; i++) 
	{  
		if (CPU_ISSET(i, &get)) 
		{  
			dbgprint("AMS-ServiceProcTask[%d] is running in cpu[%d]\n", iThread, i);
		}  
	}	
	
	while(1)
	{
        //do something ...
        
		AmsSendServiceProcMsg();
		sleep(2);
	}

	return NULL;
}


int ProcessAmsMessage(int iThreadId,MESSAGE_t *pMsg)
{
	int	iret=0,type;

	Pthread_mutex_lock(&SystemData.amsProcMutex);

	type=pMsg->eMessageAreaId;
	switch(type)
	{
	case A:
		iret = AmsProcAMsg(iThreadId,pMsg);
		break;	
	case B:
		iret = AmsProcBMsg(iThreadId,pMsg);	
		break;
	case C:
		iret = AmsProcCMsg(iThreadId,pMsg);		
		break;	

	default:
		Display("AMS: Recv Unknown AreaId[%d] Msg", type);
		iret = FAILURE;
		break;
	}
	//iretͳ��ϵͳ�ܴ���

	Pthread_mutex_unlock(&SystemData.amsProcMutex);	
	return	0;
}


//------------------------------------
//����:	A����Ϣ������	
//���:	iThreadId:�̺߳� pMsg:A����Ϣ
//����:	0:�ɹ�,����:ʧ��	
//˵��:	Ams A����Ϣ����������
//�޸�:
//------------------------------------
int AmsProcAMsg(int iThreadId,MESSAGE_t *pMsg)
{
	int					iret = 0;
	
	//��μ��
	if(NULL == pMsg)
	{	
		return AMS_ERROR;
	}

	if((pMsg->iMessageLength < 0) || (pMsg->iMessageLength >= MSG_BODY_LEN))
	{
		//ͳ��
		AmsMsgStat.amsErrMsg++;
		
		return AMS_ERROR;
	}

    //���ս��̺ż��
	if((pMsg->s_ReceiverPid.cModuleId != SystemData.cMid)||(pMsg->s_ReceiverPid.cFunctionId != FID_AMS))
	{
		//ͳ��
		AmsMsgStat.amsErrMsg++;
		
		return AMS_ERROR;
	}

	switch(pMsg->s_SenderPid.cFunctionId)
	{					
		case FID_CRM:
			AmsMsgStatProc(AMS_CRM_MSG, pMsg->iMessageType);
			iret = AmsProcCrmMsg(iThreadId,pMsg); 
			break;
		
		/*case FID_VTM:	
			AmsMsgStatProc(AMS_VTM_MSG, pMsg->iMessageType);
			iret = AmsProcVtmMsg(iThreadId,pMsg); 
			break;*/

		case FID_CMS:
			AmsMsgStatProc(AMS_CMS_MSG, pMsg->iMessageType);
			iret = AmsProcCmsMsg(iThreadId,pMsg); 
			break;
		
		/*case FID_RCAS:	
			AmsMsgStatProc(AMS_RCAS_MSG, pMsg->iMessageType);
			iret = AmsProcRcasMsg(iThreadId,pMsg); 
			break;*/
		
		default:
			//��Ϣͳ��
			AmsMsgStatProc(AMS_OTHER_MSG_TYPE, pMsg->iMessageType);
			dbgprint("AMS A Area Msg Error: MsgCode: iMessageType=0x%x.\n\t", pMsg->iMessageType);
			return AMS_ERROR;
	}

	return iret;
}


//------------------------------------
//����:	Crm��Ϣ������	
//���:	iThreadId:�̺߳� pMsg:Crm��Ϣ
//����:	0:�ɹ�,����:ʧ��	
//˵��:	Vta��Ϣ����������
//�޸�:
//------------------------------------
int AmsProcCrmMsg(int iThreadId, MESSAGE_t *pMsg)
{
	int    iret = 0;
	
	switch(pMsg->iMessageType)
	{					
		case A_TELLER_LOGIN_REQ:
			iret = CrmLoginReqProc(iThreadId,pMsg); 
			break;
		case A_TELLER_LOGOUT_REQ:
			iret = CrmLoginOutProc(iThreadId,pMsg);
			break;
		case A_TELLER_MODIFY_PASSWORD_REQ:
			iret = CrmModifyPswdReq(iThreadId,pMsg);
			break;
		case A_TELLER_FORCE_LOGIN_REQ:
			iret = CrmForceLoginReq(iThreadId,pMsg);
			break;
		case A_TELLER_QUERY_INFO_REQ:
			iret = CrmQueryInfoReq(iThreadId,pMsg);
			break;
		case A_TELLER_EVENT_NOTICE:
			iret = CrmEventNoticeMsg(iThreadId,pMsg);
			break;
		case A_TELLER_STATE_OPERATE_REQ:
			iret = CrmStateOperateReqProc(iThreadId,pMsg); 
			break;
		case A_TELLER_STATE_OPERATE_CNF:
			iret = CrmStateOperateCnfProc(iThreadId,pMsg);
			break;
		default:
			//��Ϣͳ��
			AmsMsgStatProc();
			dbgprint("AMS Proc Crm Msg Error: MsgCode: iMessageType=0x%x.\n\t", pMsg->iMessageType);
			return AMS_ERROR;
	}
	return iret;
}
//------------------------------------
//����:	Cms��Ϣ������	
//���:	iThreadId:�̺߳� pMsg:Cms��Ϣ
//����:	0:�ɹ�,����:ʧ��	
//˵��:	Cms��Ϣ����������
//�޸�:
//------------------------------------
int AmsProcCmsMsg(int iThreadId, MESSAGE_t *pMsg)
{
	int    iret = 0;
	
	switch(pMsg->iMessageType)
	{					
		case A_TELLER_REG_REQ:	//��ϯע������
			iret = CrmRegReqProc(iThreadId,pMsg); 
			break;
			
        case A_TELLER_GET_REQ:	//��ϯ��������
			iret = CrmGetReqProc(iThreadId,pMsg); 
			break;	

		case A_TELLER_CALLOUT_REQ://��ϯ�������
			iret = CrmCalloutReqProc(iThreadId,pMsg);
			break;

		case A_TELLER_AUTH_INFO_QUERY_REQ://��ϯ��Ȩ��Ϣ��ѯ����
			iret = CrmAuthinfoReqProc(iThreadId,pMsg);
			break;
			
        /*case A_TERM_NETINFO_QUERY_REQ:	
			iret = AmsQueryTermNetInfoReqProc(iThreadId,pMsg); 
			break;	*/
			
        case A_AMS_CALL_EVENT_NOTICE:	
			iret = CallEventNoticeProc(iThreadId,pMsg); 
			break;	

        /*case A_AMS_CALL_EVENT_IND_CNF:	
			iret = CallEventIndCnfProc(iThreadId,pMsg); 
			break;	*/
		
		default:
			//��Ϣͳ��
			AmsMsgStatProc();
			dbgprint("AMS Proc Cms Msg Error: MsgCode: iMessageType=0x%x.\n\t", pMsg->iMessageType);
			return AMS_ERROR;
	}

	return iret;
}

//------------------------------------
//����:	B����Ϣ������	
//���:	iThreadId:�̺߳� pMsg:B����Ϣ
//����:	0:�ɹ�,����:ʧ��	
//˵��:	Ams B����Ϣ����������
//�޸�:
//------------------------------------
int AmsProcBMsg(int iThreadId,MESSAGE_t *pMsg)
{
	int		            iret = 0;
	TIMEMESSAGE_t	    *pTmMsg;

	pTmMsg = (TIMEMESSAGE_t *)pMsg;
	
	//��μ��
	if((pTmMsg->iMessageLength < 0) || (pTmMsg->iMessageLength >= MSG_BODY_LEN))
	{
		//ͳ��
		AmsMsgStat.amsErrBMsg++;
		
		return AMS_ERROR;
	}

    //���ս��̺ż��
	if((pTmMsg->s_ReceiverPid.cModuleId != SystemData.cMid)||(pTmMsg->s_ReceiverPid.cFunctionId != FID_AMS))
	{
		//ͳ��
		AmsMsgStat.amsErrBMsg++;	
		
		return AMS_ERROR;
	}
	
	//MSG STAT
	AmsMsgStatProc(AMS_B_MSG, pMsg->iMessageType);

    switch(pTmMsg->iMessageType)
	{
		case B_AMS_TELLER_STATE_OP_IND_TIMEOUT:
			AmsTimerStatProc(T_AMS_CRM_STATE_OPERATE_IND_TIMER, AMS_TIMER_TIMEOUT);
			iret = CrmStateOperateIndTimeoutProc(iThreadId,pTmMsg); 
			break;

		case B_AMS_CUSTOMER_IN_QUEUE_TIMEOUT:			
			AmsTimerStatProc(T_AMS_CUSTOMER_IN_QUEUE_TIMER, AMS_TIMER_TIMEOUT);
            iret = AmsCustomerInQueueTimeoutProc(iThreadId,pTmMsg); 
			break;
			
		default:
			//��Ϣͳ�ƣ�Ĭ��δ֪��ʱ��ͳ��
			AmsTimerStatProc(T_AMS_TIMER_MAX, AMS_TIMER_TIMEOUT);
			dbgprint("AMS B Area Msg Error: MsgCode: iMessageType=0x%x.\n\t", pTmMsg->iMessageType);
			return AMS_ERROR;
	}

	return iret;
}


int AmsProcCMsg(int iThreadId,MESSAGE_t *pMsg)
{
	int 				iret = 0;
		
	//��μ��
	if(NULL == pMsg)
	{	
		return AMS_ERROR;
	}

	if((pMsg->iMessageLength < 0) || (pMsg->iMessageLength >= MSG_BODY_LEN))
	{
		//ͳ��
		AmsMsgStat.amsErrMsg++;
		
		return AMS_ERROR;
	}

	//���ս��̺ż��
	if(   (pMsg->s_ReceiverPid.cModuleId != SystemData.cMid)
	   || (pMsg->s_ReceiverPid.cFunctionId != FID_AMS)
	   || (pMsg->s_SenderPid.cModuleId != SystemData.cMid)
	   || (pMsg->s_SenderPid.cFunctionId != FID_AMS))
	{
		//ͳ��
		AmsMsgStat.amsErrMsg++;
		
		return AMS_ERROR;
	}

	AmsMsgStatProc(AMS_C_MSG, pMsg->iMessageType);

	switch(pMsg->iMessageType)
	{					
		case C_AMS_SERVICE_PROC_REQ:
			AmsMsgStatProc(AMS_C_MSG, pMsg->iMessageType);
			iret = AmsProcServiceProcMsg(iThreadId,pMsg); 
			break;

		default:
			//��Ϣͳ��
			AmsMsgStatProc(AMS_OTHER_MSG_TYPE, pMsg->iMessageType);
			dbgprint("AMS C Area Msg Error: MsgCode: iMessageType=0x%x.\n\t", pMsg->iMessageType);
			return AMS_ERROR;
	}

	return iret;

}


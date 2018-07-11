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

	//绑定CPU SET 
	cpuNum = sysconf(_SC_NPROCESSORS_CONF);//统计cpu个数   
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
		//iret = AmsProcCMsg(iThreadId,pMsg);		
		break;	

	default:
		Display("AMS: Recv Unknown AreaId[%d] Msg", type);
		iret = FAILURE;
		break;
	}
	//iret统计系统总错误

	Pthread_mutex_unlock(&SystemData.amsProcMutex);	
	return	0;
}


//------------------------------------
//功能:	A区消息处理函数	
//入口:	iThreadId:线程号 pMsg:A区消息
//返回:	0:成功,其他:失败	
//说明:	Ams A区消息处理函数调用
//修改:
//------------------------------------
int AmsProcAMsg(int iThreadId,MESSAGE_t *pMsg)
{
	int					iret = 0;
	
	//入参检查
	if(NULL == pMsg)
	{	
		return AMS_ERROR;
	}

	if((pMsg->iMessageLength < 0) || (pMsg->iMessageLength >= MSG_BODY_LEN))
	{
		//统计
		AmsMsgStat.amsErrMsg++;
		
		return AMS_ERROR;
	}

    //接收进程号检查
	if((pMsg->s_ReceiverPid.cModuleId != SystemData.cMid)||(pMsg->s_ReceiverPid.cFunctionId != FID_AMS))
	{
		//统计
		AmsMsgStat.amsErrMsg++;
		
		return AMS_ERROR;
	}

	switch(pMsg->s_SenderPid.cFunctionId)
	{					
		case FID_VTA:
			AmsMsgStatProc(AMS_VTA_MSG, pMsg->iMessageType);
			iret = AmsProcVtaMsg(iThreadId,pMsg); 
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
			//消息统计
			AmsMsgStatProc(AMS_OTHER_MSG_TYPE, pMsg->iMessageType);
			dbgprint("AMS A Area Msg Error: MsgCode: iMessageType=0x%x.\n\t", pMsg->iMessageType);
			return AMS_ERROR;
	}

	return iret;
}


//------------------------------------
//功能:	Vta消息处理函数	
//入口:	iThreadId:线程号 pMsg:Vta消息
//返回:	0:成功,其他:失败	
//说明:	Vta消息处理函数调用
//修改:
//------------------------------------
int AmsProcVtaMsg(int iThreadId, MESSAGE_t *pMsg)
{
	int    iret = 0;
	
	switch(pMsg->iMessageType)
	{					
		case A_VTA_LOGIN_REQ:
			iret = VtaLoginReqProc(iThreadId,pMsg); 
			break;
		case A_VTA_STATE_OPERATE_REQ:
			iret = VtaStateOperateReqProc(iThreadId,pMsg); 
			break;
		case A_VTA_STATE_OPERATE_CNF:
			iret = VtaStateOperateCnfProc(iThreadId,pMsg);
			break;
		default:
			//消息统计
			AmsMsgStatProc();
			dbgprint("AMS Proc Vta Msg Error: MsgCode: iMessageType=0x%x.\n\t", pMsg->iMessageType);
			return AMS_ERROR;
	}
	return iret;
}
//------------------------------------
//功能:	Cms消息处理函数	
//入口:	iThreadId:线程号 pMsg:Cms消息
//返回:	0:成功,其他:失败	
//说明:	Cms消息处理函数调用
//修改:
//------------------------------------
int AmsProcCmsMsg(int iThreadId, MESSAGE_t *pMsg)
{
	int    iret = 0;
	
	switch(pMsg->iMessageType)
	{					
		case A_VTA_REG_REQ:	//坐席注册请求
			iret = VtaRegReqProc(iThreadId,pMsg); 
			break;
			
        case A_VTA_GET_REQ:	//坐席分配请求
			iret = VtaGetReqProc(iThreadId,pMsg); 
			break;	

		case A_VTA_CALLOUT_REQ://坐席外呼请求
			iret = VtaCalloutReqProc(iThreadId,pMsg);
			break;

		case A_AUTHINFO_QUERY_REQ://坐席鉴权信息查询请求
			iret = VtaAuthinfoReqProc(iThreadId,pMsg);
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
			//消息统计
			AmsMsgStatProc();
			dbgprint("AMS Proc Cms Msg Error: MsgCode: iMessageType=0x%x.\n\t", pMsg->iMessageType);
			return AMS_ERROR;
	}

	return iret;
}

//------------------------------------
//功能:	B区消息处理函数	
//入口:	iThreadId:线程号 pMsg:B区消息
//返回:	0:成功,其他:失败	
//说明:	Ams B区消息处理函数调用
//修改:
//------------------------------------
int AmsProcBMsg(int iThreadId,MESSAGE_t *pMsg)
{
	int		            iret = 0;
	TIMEMESSAGE_t	    *pTmMsg;

	pTmMsg = (TIMEMESSAGE_t *)pMsg;
	
	//入参检查
	if((pTmMsg->iMessageLength < 0) || (pTmMsg->iMessageLength >= MSG_BODY_LEN))
	{
		//统计
		AmsMsgStat.amsErrBMsg++;
		
		return AMS_ERROR;
	}

    //接收进程号检查
	if((pTmMsg->s_ReceiverPid.cModuleId != SystemData.cMid)||(pTmMsg->s_ReceiverPid.cFunctionId != FID_AMS))
	{
		//统计
		AmsMsgStat.amsErrBMsg++;	
		
		return AMS_ERROR;
	}
	
	//MSG STAT
	AmsMsgStatProc(AMS_B_MSG, pMsg->iMessageType);

    switch(pTmMsg->iMessageType)
	{
		case B_AMS_VTA_STATE_OP_IND_TIMEOUT:
			AmsTimerStatProc(T_AMS_VTA_STATE_OPERATE_IND_TIMER, AMS_TIMER_TIMEOUT);
			iret = VtaStateOperateIndTimeoutProc(iThreadId,pTmMsg); 
			break;
						
		default:
			//消息统计，默认未知定时器统计
			AmsTimerStatProc(T_AMS_TIMER_MAX, AMS_TIMER_TIMEOUT);
			dbgprint("AMS B Area Msg Error: MsgCode: iMessageType=0x%x.\n\t", pTmMsg->iMessageType);
			return AMS_ERROR;
	}

	return iret;
}


#include "amsfunc.h"

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
		//AmsMsgStat.amsErrMsg++;
		
		return AMS_ERROR;
	}

    //接收进程号检查
	if((pMsg->s_ReceiverPid.cModuleId != SystemData.cMid)||(pMsg->s_ReceiverPid.cFunctionId != FID_AMS))
	{
		//统计
		//AmsMsgStat.amsErrMsg++;
		
		return AMS_ERROR;
	}

	switch(pMsg->s_SenderPid.cFunctionId)
	{					
		case FID_VTA:
			//AmsMsgStatProc(AMS_VTA_MSG, pMsg->iMessageType);
			iret = AmsProcVtaMsg(iThreadId,pMsg); 
			break;
		
		/*case FID_VTM:	
			AmsMsgStatProc(AMS_VTM_MSG, pMsg->iMessageType);
			iret = AmsProcVtmMsg(iThreadId,pMsg); 
			break;*/

		case FID_CMS:
			//AmsMsgStatProc(AMS_CMS_MSG, pMsg->iMessageType);
			iret = AmsProcCmsMsg(iThreadId,pMsg); 
			break;
		
		/*case FID_RCAS:	
			AmsMsgStatProc(AMS_RCAS_MSG, pMsg->iMessageType);
			iret = AmsProcRcasMsg(iThreadId,pMsg); 
			break;*/
		
		default:
			//消息统计
			//AmsMsgStatProc(AMS_OTHER_MSG_TYPE, pMsg->iMessageType);
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
		case A_VTA_STATE_OPERATE_REQ
			iret = VtaStateOperateReqProc(iThreadId,pMsg); 
			break;
		default:
			//消息统计
			//AmsMsgStatProc()
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

		case A_AUTHINFO_QUERY_REQ:
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
			//AmsMsgStatProc()
			dbgprint("AMS Proc Cms Msg Error: MsgCode: iMessageType=0x%x.\n\t", pMsg->iMessageType);
			return AMS_ERROR;
	}

	return iret;
}


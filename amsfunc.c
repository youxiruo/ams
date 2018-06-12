/******************************************************************
*
* FILE NAME						                           VERSION
*	
*	amsfunc.c	                                            v1.0.0
*
* DESCRIPTION
*
*	AMS应用管理服务器的基本功能处理函数
*
* COPYRIGHT
*	
*	Switch & Network Division, Eastern Communications Company Limited	
*
* HISTORY                                                        
*																  
*     NAME       DATE              REMARKS		            TARGET    
*    edward  	 2015-12-18        Created version 1.0.0    for VTC
******************************************************************/

#include "amsfunc.h"


int AmsAllocPid(PID_t *spid)
{
	spid->cModuleId = SystemData.cMid; 
	spid->cFunctionId = FID_AMS;
	spid->iProcessId = AllocPid(*spid);
	
	if (( spid->iProcessId <= 0 )||( spid->iProcessId >= LOGIC_PROCESS_SIZE))
	{
		return AMS_ERROR;
	}
	else
	{
		return AMS_OK;
	}
}

int AmsReleassPid(PID_t sPid,int ret)
{
    int ipid = sPid.iProcessId;
    
    sPid.cModuleId = SystemData.cMid;
	sPid.cFunctionId = FID_AMS;	
    
    if(FreePid(ret,sPid) == ipid)
	{
       //
	}
	
	return AMS_OK;
}

int AmsCreateTimer(int iPid,int *timerId, int timerType,int tmcnt)
{
	PID_t sPid;
	LP_AMS_DATA_t *lpAmsData;
	int iret;
	
	if(NULL == timerId)
	{
		return AMS_ERROR;
	}
	
	lpAmsData = (LP_AMS_DATA_t *)ProcessData[iPid];	
	sPid.cModuleId = SystemData.cMid;
	sPid.cFunctionId = FID_AMS;
	sPid.iProcessId = iPid;
		
	if(*timerId >= 0)
	{
        dbgprint("AmsCreateTimer killTimer[%d][0x%x] Err",*timerId, timerId);
		KillLTimer(*timerId,sPid);
	}
	
	iret = CreateLTimer(tmcnt,timerType,sPid,NULL);
	if(iret < 0)
	{
		return AMS_ERROR;
	}
	
	*timerId = iret;
    
	if(lpAmsData->timerTrace)
	{
		unsigned char description [100];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,100,"create timer %d,timelen=%d;",*timerId,tmcnt);
		
		AmsTraceInfoToFile(iPid,tmcnt,description,descrlen,lpAmsData->sTraceName);
	}
    
	return AMS_OK;
}

int AmsCreateTimerPara(int iPid,int *timerId,int timerType,int tmcnt,unsigned char *para)
{
	PID_t sPid;
//	LP_AMS_DATA_t *lpAmsData;
	int iret;
	
	if(NULL == timerId)
	{
		return AMS_ERROR;
	}
	
//	lpAmsData = (LP_AMS_DATA_t *)ProcessData[iPid];	
	sPid.cModuleId = SystemData.cMid;
	sPid.cFunctionId = FID_AMS;
	sPid.iProcessId = iPid;
		
	if(*timerId >= 0)
	{
        dbgprint("AmsCreateTimerPara killTimer[%d][0x%x] Err",*timerId, timerId);
		KillLTimer(*timerId,sPid);
	}
	
	iret = CreateLTimer(tmcnt,timerType,sPid,para);
	if(iret < 0)
	{
		return AMS_ERROR;
	}
	
	*timerId = iret;
    
//	if(lpAmsData->timerTrace)
	if(AmsTimerTrace)
	{
		unsigned char description [100];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,100,"create timer Para %d,timelen=%d,para[%d][%d][%d][%d];",
			*timerId,tmcnt,para[0],para[1],para[2],para[3]);
		
//		AmsTraceInfoToFile(iPid,tmcnt,description,descrlen,lpAmsData->sTraceName);
		AmsTraceInfoToFile(iPid,tmcnt,description,descrlen,"ams");
	}

	return AMS_OK;
}

int AmsKillTimer(int iPid, int *timerId)
{
	PID_t sPid;
//	LP_AMS_DATA_t *lpAmsData;
	int iret;

	if(NULL == timerId)
	{
		return AMS_ERROR;
	}
	
//	lpAmsData = (LP_AMS_DATA_t *)ProcessData[iPid];	
//	if(lpAmsData->timerTrace)
	if(AmsTimerTrace)
	{
		unsigned char description [100];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,100,"kill timer %d;",*timerId);
		
//		AmsTraceInfoToFile(iPid,0,description,descrlen,lpAmsData->sTraceName);
		AmsTraceInfoToFile(iPid,0,description,descrlen,"ams");
	}

    sPid.cModuleId = SystemData.cMid;	
    sPid.cFunctionId = FID_AMS;		
   	sPid.iProcessId = iPid;

    iret = KillLTimer(*timerId,sPid);
	if(iret < 0)
	{
		return AMS_ERROR;
	}
	
	*timerId = -1;
	
	return AMS_OK;
}


int AmsQueueCreateTimerPara(int iPid,int *timerId,int timerType,int tmcnt,unsigned char *para)
{
	PID_t               sPid;
	LP_QUEUE_DATA_t     *lpQueueData = NULL;
	int                 iret;
	
	if(NULL == timerId)
	{
		return AMS_ERROR;
	}
	
	lpQueueData = (LP_QUEUE_DATA_t *)ProcessData[iPid];	
	sPid.cModuleId = SystemData.cMid;
	sPid.cFunctionId = FID_AMS;
	sPid.iProcessId = iPid;

	if(*timerId >= 0)
	{
        dbgprint("AmsQueueCreateTimerPara killTimer[%d][0x%x] Err",*timerId, timerId);
		KillLTimer(*timerId,sPid);
	}
	
	iret = CreateLTimer(tmcnt,timerType,sPid,para);
	if(iret < 0)
	{
		return AMS_ERROR;
	}
	
	*timerId = iret;
    
	if(lpQueueData->timerTrace)
	{
		unsigned char description [100];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,100,"Queue create timer Para %d,timelen=%d,para[%d][%d][%d][%d];",
			*timerId,tmcnt,para[0],para[1],para[2],para[3]);
		
		AmsTraceInfoToFile(iPid,tmcnt,description,descrlen,lpQueueData->sTraceName);
	}

	return AMS_OK;
}


int AmsQueueKillTimer(int iPid, int *timerId)
{
	PID_t               sPid;
	LP_QUEUE_DATA_t     *lpQueueData = NULL;
	int                 iret;

	if(NULL == timerId)
	{
		return AMS_ERROR;
	}
	
	lpQueueData = (LP_QUEUE_DATA_t *)ProcessData[iPid];	
	if(lpQueueData->timerTrace)
	{
		unsigned char description [100];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,100,"Queue kill timer %d;",*timerId);
		
		AmsTraceInfoToFile(iPid,0,description,descrlen,lpQueueData->sTraceName);
	}

    sPid.cModuleId = SystemData.cMid;	
    sPid.cFunctionId = FID_AMS;		
   	sPid.iProcessId = iPid;

    iret = KillLTimer(*timerId,sPid);
	if(iret < 0)
	{
		return AMS_ERROR;
	}
	
	*timerId = -1;
	
	return AMS_OK;
}


char *strAmsTimerName[]=
{
	"T_AMS_TIMER_NULL",

	"T_AMS_VTA_STATE_OPERATE_IND_TIMER",
	"T_AMS_CALL_EVENT_IND_TIMER",
	"T_AMS_CUSTOMER_IN_QUEUE_TIMER",
	"T_AMS_VOLUME_CTRL_TIMER",
	"T_AMS_RCAS_REMOTE_COOP_TIMER",	
	"T_AMS_VTM_REMOTE_COOP_TIMER",		
	"T_AMS_SNAP_TIMER",	
	"T_AMS_REST_TIMER",
	"T_AMS_VTA_RECV_MSG_TIMER",
	"T_AMS_VTM_RECV_MSG_TIMER",
	"T_AMS_VTA_RECV_FILE_TIMER",
	"T_AMS_VTM_RECV_FILE_TIMER",	
	"T_AMS_MULTI_SESS_TIMER",
	"T_AMS_MONITOR_TIMER",
	"T_AMS_VTA_PARA_CFG_TIMER",
	"T_AMS_VTM_PARA_CFG_TIMER",	
	
	"T_AMS_PARA_CFG_TIMER",		
	
	"T_AMS_TIMER_MAX",
};


char *AmsGetTimerName(int code)
{
	if(code < T_AMS_TIMER_NULL || code > T_AMS_TIMER_MAX)
	{
		return strAmsTimerName[T_AMS_TIMER_MAX];
	}

	return strAmsTimerName[code];
}

//max string len is 16
char *AmsGetStateTypeString(int type,int state)
{
    switch(type)
    {
	case AMS_VTA_STATE:
		switch(state)
		{
		case AMS_VTA_STATE_IDLE:
			return "Idle";
		case AMS_VTA_STATE_BUSY:
			return "Busy";
		case AMS_VTA_STATE_REST:
			return "Rest";
		case AMS_VTA_STATE_PREPARE:
			return "Prepare";
	    case AMS_VTA_STATE_OFFLINE:
	        return "Offline";	
		default:
			break;
		}
		break;
    case AMS_CALL_STATE:
        switch(state)
		{
		case AMS_CALL_STATE_NULL:
			return "Call Idle";
        case AMS_CALL_STATE_WAIT_ANSWER:
            return "Call Wait Answer";			
        case AMS_CALL_STATE_TRANSFER:
			return "Call Transfer";  
        case AMS_CALL_STATE_ANSWER:
            return "Call Answer";
		case AMS_CALL_STATE_HOLD:
			return "Call Hold";			
        case AMS_CALL_STATE_INNER_CALL:
            return "Call In Inner Call";
        case AMS_CALL_STATE_MONITOR_CALL:
            return "Call In Monitor Call";			
        case AMS_CALL_STATE_CONF:
            return "Call In Conf";

		default:
			break;
		}
    case AMS_VTM_STATE:
        switch(state)
		{
		case AMS_VTM_STATE_IDLE:
			return "Idle";
		case AMS_VTM_STATE_BUSY:
			return "Busy";
        case AMS_VTM_STATE_OFFLINE:
			return "Offline";    

		default:
			break;
		}
		break;
		
    case AMS_CUSTOMER_STATE:
        switch(state)
		{
		case AMS_CUSTOMER_SERVICE_NULL:
			return "Null";
		case AMS_CUSTOMER_IN_QUEUE:
			return "In Queue";
        case AMS_CUSTOMER_IN_SERVICE:
			return "In Service";    

		default:
			break;
		}
		break;
		
	default:
		break;
    }
    
    return  "Unkown State";
}

void AmsSetVtaState(int iThreadId,LP_AMS_DATA_t *lpAmsData,VTA_NODE *pVtaNode,int state,int stateReason)
{
	struct tm           *tLocalTime;      // 东8区
	
	if(NULL == lpAmsData || NULL == pVtaNode)
	{
		return;
	}
	
	pVtaNode->state = state;

	//record state start time
	time(&pVtaNode->stateStartTime);
	tLocalTime = localtime(&pVtaNode->stateStartTime);
	pVtaNode->stateStartLocalTime.year   = tLocalTime->tm_year + 1900; //+1900
	pVtaNode->stateStartLocalTime.month  = tLocalTime->tm_mon + 1;     //+1
	pVtaNode->stateStartLocalTime.day    = tLocalTime->tm_mday;
	pVtaNode->stateStartLocalTime.hour   = tLocalTime->tm_hour;
	pVtaNode->stateStartLocalTime.minute = tLocalTime->tm_min;
	pVtaNode->stateStartLocalTime.second = tLocalTime->tm_sec;

	if(pVtaNode->tellerCfgPos < AMS_MAX_VTA_NUM)
	{
		AmsTellerStat(pVtaNode->tellerCfgPos).vtaStateInfo.lastState = pVtaNode->state;
		AmsTellerStat(pVtaNode->tellerCfgPos).vtaStateInfo.lastStateStartTime = pVtaNode->stateStartTime;
	}

	//柜员实时状态信息入库-柜员状态信息
	if(AMS_VTA_STATE_BUSY == state && VTA_STATE_OPERATE_BUSY_IN_CALL == stateReason)
	{
		//UpdateDbTellerState and InsertDbTellerSerialState by Cms
	}
	else
	{
		AmsUpdateDbTellerState(iThreadId, lpAmsData, AMS_TELLER_STATE_ITEM_BASE, state, &pVtaNode->stateStartTime);

		AmsInsertDbTellerSerialState(iThreadId, lpAmsData, state, &pVtaNode->stateStartLocalTime, 0);
	}
	
	if (lpAmsData->stateTrace)
	{
		unsigned char description [256];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen = snprintf(description,256,"Teller[%s][%u] AmsPid[0x%x]\r\n", 
			pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
			lpAmsData->amsPid);		
		descrlen += snprintf(description+descrlen,256-descrlen,"Current[%d-%d-%d %d:%d:%d]State=%s;",
			pVtaNode->stateStartLocalTime.year,
			pVtaNode->stateStartLocalTime.month,
			pVtaNode->stateStartLocalTime.day,
			pVtaNode->stateStartLocalTime.hour,
			pVtaNode->stateStartLocalTime.minute,
			pVtaNode->stateStartLocalTime.second,
			AmsGetStateTypeString(AMS_VTA_STATE, pVtaNode->state));
		
		AmsTraceInfoToFile(lpAmsData->amsPid,0,description,descrlen,lpAmsData->sTraceName);
	}
}

void AmsManagerSetVtaState(int iThreadId,LP_AMS_DATA_t *lpAmsManagerData,LP_AMS_DATA_t *lpAmsData,VTA_NODE *pVtaNode,int state)
{
//  time_t              stateStartTime;      // long int
    struct tm           *tLocalTime;         // 东8区

	if(NULL == lpAmsManagerData || NULL == lpAmsData || NULL == pVtaNode)
	{
		return;
	}
	
	pVtaNode->state = state;

	//record state start time
//	time(&stateStartTime);
//	tLocalTime = localtime(&stateStartTime);
	time(&pVtaNode->stateStartTime);
	tLocalTime = localtime(&pVtaNode->stateStartTime);
	pVtaNode->stateStartLocalTime.year   = tLocalTime->tm_year + 1900; //+1900
	pVtaNode->stateStartLocalTime.month  = tLocalTime->tm_mon + 1;     //+1
	pVtaNode->stateStartLocalTime.day    = tLocalTime->tm_mday;
	pVtaNode->stateStartLocalTime.hour   = tLocalTime->tm_hour;
	pVtaNode->stateStartLocalTime.minute = tLocalTime->tm_min;
	pVtaNode->stateStartLocalTime.second = tLocalTime->tm_sec;

	if(pVtaNode->tellerCfgPos < AMS_MAX_VTA_NUM)
	{
		AmsTellerStat(pVtaNode->tellerCfgPos).vtaStateInfo.lastState = pVtaNode->state;
		AmsTellerStat(pVtaNode->tellerCfgPos).vtaStateInfo.lastStateStartTime = pVtaNode->stateStartTime;
	}

	//柜员实时状态信息入库-柜员状态信息
	AmsUpdateDbTellerState(iThreadId, lpAmsData, AMS_TELLER_STATE_ITEM_BASE, state, &pVtaNode->stateStartTime); 
	
	AmsInsertDbTellerSerialState(iThreadId, lpAmsData, state, &pVtaNode->stateStartLocalTime, 0); 
	
	if (lpAmsData->stateTrace)
	{
		unsigned char description [256];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,256,"AmsManger[%s][%u] SetTeller[%s][%u]-[0x%x]\r\n;",
			lpAmsManagerData->tellerNo, lpAmsManagerData->tellerId,
			pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
			lpAmsData->amsPid);
		descrlen +=snprintf(description+descrlen,256-descrlen,"Current[%d-%d-%d %d:%d:%d]State=%s;",
			pVtaNode->stateStartLocalTime.year,
			pVtaNode->stateStartLocalTime.month,
			pVtaNode->stateStartLocalTime.day,
			pVtaNode->stateStartLocalTime.hour,
			pVtaNode->stateStartLocalTime.minute,
			pVtaNode->stateStartLocalTime.second,
			AmsGetStateTypeString(AMS_VTA_STATE, pVtaNode->state));	
		
		AmsTraceInfoToFile(lpAmsData->amsPid,0,description,descrlen,lpAmsData->sTraceName);
	}
}

void AmsSetVtaCallState(LP_AMS_DATA_t *lpAmsData,VTA_NODE *pVtaNode,int state)
{
	struct tm           *tLocalTime;      // 东8区

	if(NULL == lpAmsData || NULL == pVtaNode)
	{
		return;
	}
	
	pVtaNode->callState = state;

	//record call state start time
	time(&pVtaNode->callStateStartTime);
	tLocalTime = localtime(&pVtaNode->callStateStartTime);
	pVtaNode->callStateStartLocalTime.year   = tLocalTime->tm_year + 1900; //+1900
	pVtaNode->callStateStartLocalTime.month  = tLocalTime->tm_mon + 1;     //+1
	pVtaNode->callStateStartLocalTime.day    = tLocalTime->tm_mday;
	pVtaNode->callStateStartLocalTime.hour   = tLocalTime->tm_hour;
	pVtaNode->callStateStartLocalTime.minute = tLocalTime->tm_min;
	pVtaNode->callStateStartLocalTime.second = tLocalTime->tm_sec;

	if (lpAmsData->stateTrace)
	{
		unsigned char description [256];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,256,"Teller[%s][%u] Vtm[%s][%u] AmsPid[0x%x]\r\n", 
			pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
			lpAmsData->vtmNo, lpAmsData->vtmId, lpAmsData->amsPid);
		descrlen += snprintf(description+descrlen,256-descrlen,"Current[%d-%d-%d %d:%d:%d]CallState=%s;",			
			pVtaNode->callStateStartLocalTime.year,
			pVtaNode->callStateStartLocalTime.month,
			pVtaNode->callStateStartLocalTime.day,
			pVtaNode->callStateStartLocalTime.hour,
			pVtaNode->callStateStartLocalTime.minute,
			pVtaNode->callStateStartLocalTime.second,
			AmsGetStateTypeString(AMS_CALL_STATE, pVtaNode->callState));
			
		AmsTraceInfoToFile(lpAmsData->amsPid,0,description,descrlen,lpAmsData->sTraceName);
	}
}

void AmsSetVtmState(int iThreadId,VTM_NODE *pVtmNode,int state)
{
	struct tm           *tLocalTime;      // 东8区

	if(NULL == pVtmNode)
	{
		return;
	}
	
	pVtmNode->state = state;

	//record state start time
	time(&pVtmNode->stateStartTime);
	tLocalTime = localtime(&pVtmNode->stateStartTime);
	pVtmNode->stateStartLocalTime.year   = tLocalTime->tm_year + 1900; //+1900
	pVtmNode->stateStartLocalTime.month  = tLocalTime->tm_mon + 1;     //+1
	pVtmNode->stateStartLocalTime.day    = tLocalTime->tm_mday;
	pVtmNode->stateStartLocalTime.hour   = tLocalTime->tm_hour;
	pVtmNode->stateStartLocalTime.minute = tLocalTime->tm_min;
	pVtmNode->stateStartLocalTime.second = tLocalTime->tm_sec;

	AmsUpdateDbVtmState(iThreadId,pVtmNode,state); 
	
	if(AmsStateTrace)
	{
		unsigned char description [256];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,256,"Vtm[%s][%u] AmsPid[0x%x] CustPid[0x%x]\r\n",
			pVtmNode->vtmInfo.vtmNo, pVtmNode->vtmInfo.vtmId,
			pVtmNode->amsPid, pVtmNode->customerPid);
		descrlen +=snprintf(description+descrlen,256-descrlen,"Current[%d-%d-%d %d:%d:%d]State=%s;",
			pVtmNode->stateStartLocalTime.year,
			pVtmNode->stateStartLocalTime.month,
			pVtmNode->stateStartLocalTime.day,
			pVtmNode->stateStartLocalTime.hour,
			pVtmNode->stateStartLocalTime.minute,
			pVtmNode->stateStartLocalTime.second,
			AmsGetStateTypeString(AMS_VTM_STATE, pVtmNode->state));
			
		AmsTraceInfoToFile(pVtmNode->amsPid,0,description,descrlen,"ams"); //pVtmNode->amsPid not used, so not add pVtmNode->customerPid para
	}
}

void AmsSetVtmCallState(VTM_NODE *pVtmNode,int state)
{
	struct tm           *tLocalTime;      // 东8区

	if(NULL == pVtmNode)
	{
		return;
	}
	
	pVtmNode->callState = state;

	//record state start time
	time(&pVtmNode->callStateStartTime);
	tLocalTime = localtime(&pVtmNode->callStateStartTime);
	pVtmNode->callStateStartLocalTime.year   = tLocalTime->tm_year + 1900; //+1900
	pVtmNode->callStateStartLocalTime.month  = tLocalTime->tm_mon + 1;     //+1
	pVtmNode->callStateStartLocalTime.day    = tLocalTime->tm_mday;
	pVtmNode->callStateStartLocalTime.hour   = tLocalTime->tm_hour;
	pVtmNode->callStateStartLocalTime.minute = tLocalTime->tm_min;
	pVtmNode->callStateStartLocalTime.second = tLocalTime->tm_sec;

	if(AmsStateTrace)
	{
		unsigned char description [256];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,256,"Vtm[%s][%u] AmsPid[0x%x]\r\n",
			pVtmNode->vtmInfo.vtmNo, pVtmNode->vtmInfo.vtmId, pVtmNode->amsPid);
		descrlen +=snprintf(description+descrlen,256-descrlen,"Current[%d-%d-%d %d:%d:%d]CallState=%s;",
			pVtmNode->callStateStartLocalTime.year,
			pVtmNode->callStateStartLocalTime.month,
			pVtmNode->callStateStartLocalTime.day,
			pVtmNode->callStateStartLocalTime.hour,
			pVtmNode->callStateStartLocalTime.minute,
			pVtmNode->callStateStartLocalTime.second,
			AmsGetStateTypeString(AMS_CALL_STATE, pVtmNode->callState));
			
		AmsTraceInfoToFile(pVtmNode->amsPid,0,description,descrlen,"ams");
	}
}

void AmsSetVtmServiceState(VTM_NODE *pVtmNode,int state)
{
	struct tm           *tLocalTime;      // 东8区

	if(NULL == pVtmNode)
	{
		return;
	}
	
	pVtmNode->serviceState = state;

	//record state start time
	time(&pVtmNode->serviceStateStartTime);
	tLocalTime = localtime(&pVtmNode->serviceStateStartTime);
	pVtmNode->serviceStateStartLocalTime.year   = tLocalTime->tm_year + 1900; //+1900
	pVtmNode->serviceStateStartLocalTime.month  = tLocalTime->tm_mon + 1;     //+1
	pVtmNode->serviceStateStartLocalTime.day    = tLocalTime->tm_mday;
	pVtmNode->serviceStateStartLocalTime.hour   = tLocalTime->tm_hour;
	pVtmNode->serviceStateStartLocalTime.minute = tLocalTime->tm_min;
	pVtmNode->serviceStateStartLocalTime.second = tLocalTime->tm_sec;

	if(AmsStateTrace)
	{
		unsigned char description [256];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,256,"Vtm[%s][%u] AmsPid[0x%x] Customer[%u]\r\n",
			pVtmNode->vtmInfo.vtmNo, pVtmNode->vtmInfo.vtmId, 
			pVtmNode->amsPid, pVtmNode->customerPid);
		descrlen +=snprintf(description+descrlen,256-descrlen,"Current[%d-%d-%d %d:%d:%d]ServiceState=%s;",
			pVtmNode->serviceStateStartLocalTime.year,
			pVtmNode->serviceStateStartLocalTime.month,
			pVtmNode->serviceStateStartLocalTime.day,
			pVtmNode->serviceStateStartLocalTime.hour,
			pVtmNode->serviceStateStartLocalTime.minute,
			pVtmNode->serviceStateStartLocalTime.second,
			AmsGetStateTypeString(AMS_CUSTOMER_STATE, pVtmNode->serviceState));
			
		AmsTraceInfoToFile(pVtmNode->amsPid,0,description,descrlen,"ams");
	}
}

VTA_NODE *AmsSearchVtaNode(unsigned int srvGrpId, unsigned int tellerId)
{
	VTA_NODE *pVtaNode = NULL;	
	int i = 0;
	int find = 0;
	
	pVtaNode = (VTA_NODE *)lstFirst(&AmsSrvData(srvGrpId).vtaList);
	while(NULL != pVtaNode && i < AMS_MAX_VTA_NODES)
	{
        if(pVtaNode->vtaInfo.tellerId == tellerId)
        {
			find = 1;
			break;			
        }
		
        pVtaNode = (VTA_NODE *)lstNext((NODE *)pVtaNode);
		i++;
    }

	if(0 == find)
	{
		pVtaNode = NULL;	
	}
	
    return pVtaNode;
	
}

//no offline
int AmsUpdateVtaState(int iThreadId, LP_AMS_DATA_t *lpAmsData, VTA_NODE *pVtaNode, int stateOperate, int stateReason)
{
	int                 iret = AMS_OK;
	int                 newState = AMS_VTA_STATE_RSVD;
	time_t              currentTime;

	if(NULL == lpAmsData || NULL == pVtaNode)
	{
		iret = AMS_ERROR;
		return iret;
	}

    switch(pVtaNode->state)
	{
	case AMS_VTA_STATE_IDLE:
		if(stateOperate == VTA_STATE_OPERATE_BUSY)
		{
			newState = AMS_VTA_STATE_BUSY;
		}
		else if(stateOperate == VTA_STATE_OPERATE_REST)
		{
			newState = AMS_VTA_STATE_REST;
		}	
		else if(stateOperate == VTA_STATE_OPERATE_PREPARE)
		{
			newState = AMS_VTA_STATE_PREPARE;
		}
//		else if(stateOperate == VTA_STATE_OPERATE_LEAVE)
//		{
//			newState = AMS_VTA_STATE_LEAVE;
//		}
		else if(stateOperate == VTA_STATE_OPERATE_IDLE)
		{
//			return iret;
			newState = AMS_VTA_STATE_IDLE;
		}
		else
		{
			dbgprint("AMS Recv Err Operate[%d] in Idle State", stateOperate);
			iret = AMS_ERROR;
		}	
		break;	
	case AMS_VTA_STATE_BUSY:
		if(stateOperate == VTA_STATE_OPERATE_IDLE)
		{
			newState = AMS_VTA_STATE_IDLE;
		}
		else if(stateOperate == VTA_STATE_OPERATE_REST)
		{
			newState = AMS_VTA_STATE_REST;
		}		
		else if(stateOperate == VTA_STATE_OPERATE_PREPARE)
		{
			newState = AMS_VTA_STATE_PREPARE;
		}
//		else if(stateOperate == VTA_STATE_OPERATE_LEAVE)
//		{
//			newState = AMS_VTA_STATE_LEAVE;
//		}
		else if(stateOperate == VTA_STATE_OPERATE_BUSY)
		{
//			return iret;
			newState = AMS_VTA_STATE_BUSY;
		}
		else
		{
			dbgprint("AMS Recv Err Operate[%d] in Busy State", stateOperate);
			iret = AMS_ERROR;
		}	
		break;
	case AMS_VTA_STATE_REST:
		if(stateOperate == VTA_STATE_OPERATE_IDLE)
		{
			newState = AMS_VTA_STATE_IDLE;
		}
		else if(stateOperate == VTA_STATE_OPERATE_PREPARE)
		{
			newState = AMS_VTA_STATE_PREPARE;
		}
		else if(stateOperate == VTA_STATE_OPERATE_CANCEL_REST)
		{
			newState = AMS_VTA_STATE_IDLE;
		}	
//		else if(stateOperate == VTA_STATE_OPERATE_LEAVE)
//		{
//			newState = AMS_VTA_STATE_LEAVE;
//		}		
		else if(stateOperate == VTA_STATE_OPERATE_REST)
		{
//			return iret;
			newState = AMS_VTA_STATE_REST;
		}
		else
		{
			dbgprint("AMS Recv Err Operate[%d] in Rest State", stateOperate);
			iret = AMS_ERROR;
		}		
		break;		
	case AMS_VTA_STATE_PREPARE:
		if(stateOperate == VTA_STATE_OPERATE_PREPARE_COMPLETELY)
		{
			newState = AMS_VTA_STATE_IDLE;
		}
		else if(stateOperate == VTA_STATE_OPERATE_REST)
		{
			newState = AMS_VTA_STATE_REST;
		}	
//		else if(stateOperate == VTA_STATE_OPERATE_LEAVE)
//		{
//			newState = AMS_VTA_STATE_LEAVE;
//		}
		else if(stateOperate == AMS_VTA_STATE_PREPARE)
		{
//			return iret;
			newState = AMS_VTA_STATE_PREPARE;
		}
		else
		{
			dbgprint("AMS Recv Err Operate[%d] in Prepara State", stateOperate);
			iret = AMS_ERROR;
		}
		break;	
	case AMS_VTA_STATE_OFFLINE:
		if(stateOperate == VTA_STATE_OPERATE_IDLE)
		{
			newState = AMS_VTA_STATE_IDLE;
		}
		else if(stateOperate == AMS_VTA_STATE_OFFLINE)
		{
//			return iret;
			newState = AMS_VTA_STATE_OFFLINE;
		}		
		else
		{
			dbgprint("AMS Recv Err Operate[%d] in Offline State", stateOperate);
			iret = AMS_ERROR;
		}
		break;			
	default:
		dbgprint("AMS Err State[%d]",pVtaNode->state);
		iret = AMS_ERROR;
		break;
	}
	
	if(AMS_OK != iret)
	{
		return iret;
	}
	
    //calc vta workInfo
	time(&currentTime);	   
	AmsUpdateSingleVtaWorkInfo(pVtaNode, currentTime);
	
	//set Vta State and State Start Time
	AmsSetVtaState(iThreadId, lpAmsData, pVtaNode, newState, stateReason);

	return iret;
}

VTM_NODE *AmsSearchVtmNode(unsigned int srvGrpId, unsigned int vtmId)
{
	VTM_NODE *pVtmNode = NULL;	
	int i = 0;
	int find = 0;
	
	pVtmNode = (VTM_NODE *)lstFirst(&AmsSrvData(srvGrpId).vtmList);
	while(NULL != pVtmNode && i < AMS_MAX_VTM_NODES)
    {
        if(pVtmNode->vtmInfo.vtmId == vtmId)
        {
			find = 1;
			break;			
        }
		
        pVtmNode = (VTM_NODE *)lstNext((NODE *)pVtmNode);
		i++;
    }

	if(0 == find)
	{
		pVtmNode = NULL;	
	}
	
    return pVtmNode;
	
}


int AmsManagerUpdateVtaState(int iThreadId, LP_AMS_DATA_t *lpAmsManagerData, LP_AMS_DATA_t *lpAmsData, VTA_NODE *pVtaNode, unsigned int stateSet)
{
	int                 iret = AMS_OK;
	unsigned int        newState;
	int                 pid = 0;
	time_t              currentTime;

	if(NULL == lpAmsManagerData || NULL == lpAmsData || NULL == pVtaNode)
	{
		iret = AMS_ERROR;
		return iret;
	}
	
    switch(pVtaNode->state)
	{
	case AMS_VTA_STATE_IDLE:
 		if(stateSet > AMS_VTA_STATE_OFFLINE)
		{
			dbgprint("AMS Manager[%s][%u]SetTeller[%s][%u]State[%d]toNewState[%d] Err", 
				lpAmsManagerData->tellerNo, lpAmsManagerData->tellerId, 
				pVtaNode->vtaInfo.tellerNo,	pVtaNode->vtaInfo.tellerId, 
				pVtaNode->state, stateSet);
			iret = AMS_ERROR;		
		}	
		break;	
	case AMS_VTA_STATE_BUSY:
 		if(stateSet > AMS_VTA_STATE_OFFLINE)
		{
			dbgprint("AMS Manager[%s][%u]SetTeller[%s][%u]State[%d]toNewState[%d] Err", 
				lpAmsManagerData->tellerNo, lpAmsManagerData->tellerId, 
				pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
				pVtaNode->state, stateSet);
			iret = AMS_ERROR;					
		}	
		break;
	case AMS_VTA_STATE_REST:
		if(stateSet == AMS_VTA_STATE_BUSY || stateSet > AMS_VTA_STATE_OFFLINE)
		{
			dbgprint("AMS Manager[%s][%u]SetTeller[%s][%u]State[%d]toNewState[%d] Err", 
				lpAmsManagerData->tellerNo, lpAmsManagerData->tellerId, 
				pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
				pVtaNode->state, stateSet);
			iret = AMS_ERROR;	
		}				
		break;	
	case AMS_VTA_STATE_PREPARE:
		if(stateSet == AMS_VTA_STATE_BUSY || stateSet > AMS_VTA_STATE_OFFLINE)
		{
			dbgprint("AMS Manager[%s][%u]SetTeller[%s][%u]State[%d]toNewState[%d] Err", 
				lpAmsManagerData->tellerNo, lpAmsManagerData->tellerId, 
				pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
				pVtaNode->state, stateSet);
			iret = AMS_ERROR;	
		}				
		break;	
	case AMS_VTA_STATE_OFFLINE:
		if(stateSet != AMS_VTA_STATE_IDLE && stateSet != AMS_VTA_STATE_OFFLINE)
		{
			dbgprint("AMS Manager[%s][%u]SetTeller[%s][%u]State[%d]toNewState[%d] Err", 
				lpAmsManagerData->tellerNo, lpAmsManagerData->tellerId, 
				pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
				pVtaNode->state, stateSet);
			iret = AMS_ERROR;					
		}			
		break;			
	default:
		dbgprint("AMS Manager[%s][%u]SetTeller[%s][%u]State[%d]Err", 
			lpAmsManagerData->tellerNo, lpAmsManagerData->tellerId, 
			pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
			pVtaNode->state);
		iret = AMS_ERROR;
		break;
	}

	if(AMS_OK != iret)
	{
		return iret;
	}
	
    //calc vta workInfo
	time(&currentTime);	   
	AmsUpdateSingleVtaWorkInfo(pVtaNode, currentTime);
	
    //manager set Vta State and State Start Time
    AmsManagerSetVtaState(iThreadId, lpAmsManagerData, lpAmsData, pVtaNode, stateSet);

	return iret;
}

int AmsGetStateOpIndFromStateSet(unsigned int stateSet,unsigned short *stateOpInd)
{
	int                 iret = AMS_OK;
	
	if(stateSet > AMS_VTA_STATE_OFFLINE)
	{
		iret = AMS_ERROR;
		return iret;
	}
	
    switch(stateSet)
	{
	case AMS_VTA_STATE_IDLE:
 		*stateOpInd = VTA_STATE_OPERATE_IND_IDLE;	
		break;	
	case AMS_VTA_STATE_BUSY:
 		*stateOpInd = VTA_STATE_OPERATE_IND_BUSY;
		break;
	case AMS_VTA_STATE_REST:
		*stateOpInd = VTA_STATE_OPERATE_IND_REST;			
		break;	
	case AMS_VTA_STATE_PREPARE:
		*stateOpInd = VTA_STATE_OPERATE_IND_PREPARE;			
		break;	
	case AMS_VTA_STATE_OFFLINE:
		*stateOpInd = VTA_STATE_OPERATE_IND_OFFLINE;						
		break;			
	default:
		dbgprint("AMS GetStateOpIndFromStateSet[%d]Err", stateSet);
		iret = AMS_ERROR;
		break;
	}
	
	return iret;	
}

int AmsGetNewStateFromStateOpInd(unsigned short stateOpInd,unsigned int *newState)
{
	int                 iret = AMS_OK;
	
	if(stateOpInd < VTA_STATE_OPERATE_IND_IDLE || stateOpInd >= VTA_STATE_OPERATE_IND_MAX)
	{
		iret = AMS_ERROR;
		return iret;
	}
	
    switch(stateOpInd)
	{
	case VTA_STATE_OPERATE_IND_IDLE:
 		*newState = AMS_VTA_STATE_IDLE;	
		break;	
	case VTA_STATE_OPERATE_IND_BUSY:
 		*newState = AMS_VTA_STATE_BUSY;
		break;
	case VTA_STATE_OPERATE_IND_REST:
		*newState = AMS_VTA_STATE_REST;			
		break;	
	case VTA_STATE_OPERATE_IND_CANCEL_REST:
		*newState = AMS_VTA_STATE_IDLE;			
		break;			
	case VTA_STATE_OPERATE_IND_PREPARE:
		*newState = AMS_VTA_STATE_PREPARE;			
		break;	
	case VTA_STATE_OPERATE_IND_PREPARE_COMPLETELY:
		*newState = AMS_VTA_STATE_IDLE;			
		break;			
	case VTA_STATE_OPERATE_IND_OFFLINE:
		*newState = AMS_VTA_STATE_OFFLINE;						
		break;			
	default:
		dbgprint("AMS GetNewStateFromStateOpInd[%d]Err", stateOpInd);
		iret = AMS_ERROR;
		break;
	}
	
	return iret;	
}

int AmsUpdateRemoteCoopState(int iThreadId, LP_AMS_DATA_t *lpAmsData, unsigned char remoteCoopTarget, unsigned short remoteOp, int iret)
{
	if(NULL == lpAmsData)
	{
		return AMS_ERROR;
	}

	if(remoteCoopTarget&AMS_REMOTE_COOP_TARGET_RCAS)
	{
		if(AMS_RCAS_PRCOESS_SUCCESS != iret)
		{	
			//update Rcas remoteOp State
			if(VTA_START_SCREEN_SHARE == remoteOp)
			{
				//Update Rcas Cncr Num
				AmsUpdateRcasCncrNum(lpAmsData, AMS_DECREASE_RCAS_CNCR_NUM);
					
				lpAmsData->rcasRemoteCoopState = AMS_DESKTOP_SHARE_NULL;
			}
			else if(VTA_START_SCREEN_CTRL == remoteOp)
			{
				lpAmsData->rcasRemoteCoopState = AMS_DESKTOP_SHARING;
			}
			else if(VTA_STOP_SCREEN_CTRL == remoteOp)
			{
				lpAmsData->rcasRemoteCoopState = AMS_DESKTOP_CONTROLLING;
			}
			else // if(VTA_STOP_SCREEN_SHARE == remoteOp) abort
			{
				//Update Rcas Cncr Num
				AmsUpdateRcasCncrNum(lpAmsData, AMS_DECREASE_RCAS_CNCR_NUM);
				
				lpAmsData->rcasRemoteCoopState = AMS_DESKTOP_SHARE_NULL;
			}
		}
		else
		{
			//update Rcas remoteOp State
			if(VTA_START_SCREEN_SHARE == remoteOp)
			{
				//Update Rcas Cncr Num
				AmsUpdateRcasCncrNum(lpAmsData, AMS_INCREASE_RCAS_CNCR_NUM);

				lpAmsData->rcasRemoteCoopState = AMS_DESKTOP_SHARING;
			}
			else if(VTA_START_SCREEN_CTRL == remoteOp)
			{
				lpAmsData->rcasRemoteCoopState = AMS_DESKTOP_CONTROLLING;
			}
			else if(VTA_STOP_SCREEN_CTRL == remoteOp)
			{
				lpAmsData->rcasRemoteCoopState = AMS_DESKTOP_SHARING;
			}
			else // if(VTA_STOP_SCREEN_SHARE == remoteOp) abort
			{
				//Update Rcas Cncr Num
				AmsUpdateRcasCncrNum(lpAmsData, AMS_DECREASE_RCAS_CNCR_NUM);
				
				lpAmsData->rcasRemoteCoopState = AMS_DESKTOP_SHARE_NULL;
			}	
		}
	}

	if(remoteCoopTarget&AMS_REMOTE_COOP_TARGET_VTM)		
	{
		if(AMS_VTM_MNG_SUCCESS != iret)
		{	
			//update vtm remoteOp State
			if(VTA_START_SCREEN_SHARE == remoteOp)
			{
				lpAmsData->vtmRemoteCoopState = AMS_DESKTOP_SHARE_NULL;
			}
			else if(VTA_START_SCREEN_CTRL == remoteOp)
			{
				lpAmsData->vtmRemoteCoopState = AMS_DESKTOP_SHARING;
			}
			else if(VTA_STOP_SCREEN_CTRL == remoteOp)
			{
				lpAmsData->vtmRemoteCoopState = AMS_DESKTOP_CONTROLLING;
			}
			else // if(VTA_STOP_SCREEN_SHARE == remoteOp) abort
			{
				lpAmsData->vtmRemoteCoopState = AMS_DESKTOP_SHARE_NULL;
			}
		}
		else
		{
			//update vtm remoteOp State
			if(VTA_START_SCREEN_SHARE == remoteOp)
			{
				lpAmsData->vtmRemoteCoopState = AMS_DESKTOP_SHARING;
			}
			else if(VTA_START_SCREEN_CTRL == remoteOp)
			{
				lpAmsData->vtmRemoteCoopState = AMS_DESKTOP_CONTROLLING;
			}
			else if(VTA_STOP_SCREEN_CTRL == remoteOp)
			{
				lpAmsData->vtmRemoteCoopState = AMS_DESKTOP_SHARING;
			}
			else // if(VTA_STOP_SCREEN_SHARE == remoteOp) abort
			{
				lpAmsData->vtmRemoteCoopState = AMS_DESKTOP_SHARE_NULL;
			}	
		}
	}	

	//柜员实时状态信息入库-远程协作状态
	AmsUpdateDbTellerState(iThreadId, lpAmsData, AMS_TELLER_REMOTE_COOP_STATE, 0, NULL); 
	
	return AMS_OK;
	
}

int AmsKillVtaAllTimer(LP_AMS_DATA_t *lpAmsData, int pid)
{
	if(NULL == lpAmsData)
	{
		return AMS_ERROR;
	}

	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		return AMS_ERROR;
	}

	if(lpAmsData->iTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->iTimerId);
		AmsTimerStatProc(T_AMS_VTA_STATE_OPERATE_IND_TIMER, AMS_KILL_TIMER);
	} 
	
	if(lpAmsData->callTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->callTimerId);
		AmsTimerStatProc(T_AMS_CALL_EVENT_IND_TIMER, AMS_KILL_TIMER);
	} 
		
	if(lpAmsData->volumeCtrlTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->volumeCtrlTimerId);
		AmsTimerStatProc(T_AMS_VOLUME_CTRL_TIMER, AMS_KILL_TIMER);
	} 

	if(lpAmsData->rcasRemoteCoopTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->rcasRemoteCoopTimerId);
		AmsTimerStatProc(T_AMS_RCAS_REMOTE_COOP_TIMER, AMS_KILL_TIMER);
	} 

	if(lpAmsData->vtmRemoteCoopTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->vtmRemoteCoopTimerId);
		AmsTimerStatProc(T_AMS_VTM_REMOTE_COOP_TIMER, AMS_KILL_TIMER);
	} 
	
	if(lpAmsData->snapTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->snapTimerId);
		AmsTimerStatProc(T_AMS_SNAP_TIMER, AMS_KILL_TIMER);
	}

	if(lpAmsData->restTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->restTimerId);
		AmsTimerStatProc(T_AMS_REST_TIMER, AMS_KILL_TIMER);
	} 

	if(lpAmsData->sendMsgToVtaTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->sendMsgToVtaTimerId);
		AmsTimerStatProc(T_AMS_VTA_RECV_MSG_TIMER, AMS_KILL_TIMER);
	} 

	if(lpAmsData->sendMsgToVtmTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->sendMsgToVtmTimerId);
		AmsTimerStatProc(T_AMS_VTM_RECV_MSG_TIMER, AMS_KILL_TIMER);
	}

	if(lpAmsData->sendFileToVtaTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->sendFileToVtaTimerId);
		AmsTimerStatProc(T_AMS_VTA_RECV_FILE_TIMER, AMS_KILL_TIMER);
	} 

	if(lpAmsData->sendFileToVtmTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->sendFileToVtmTimerId);
		AmsTimerStatProc(T_AMS_VTM_RECV_FILE_TIMER, AMS_KILL_TIMER);
	} 

	if(lpAmsData->multiSessTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->multiSessTimerId);
		AmsTimerStatProc(T_AMS_MULTI_SESS_TIMER, AMS_KILL_TIMER);
	}

	if(lpAmsData->monitorTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->monitorTimerId);
		AmsTimerStatProc(T_AMS_MONITOR_TIMER, AMS_KILL_TIMER);
	}

	if(lpAmsData->vtaParaCfgTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->vtaParaCfgTimerId);
		AmsTimerStatProc(T_AMS_VTA_PARA_CFG_TIMER, AMS_KILL_TIMER);
	}
	
	return AMS_OK;	
}


int AmsKillVtaAllCallTimer(LP_AMS_DATA_t *lpAmsData, int pid)
{
	if(NULL == lpAmsData)
	{
		return AMS_ERROR;
	}

	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		return AMS_ERROR;
	}
	
	if(lpAmsData->callTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->callTimerId);
		AmsTimerStatProc(T_AMS_CALL_EVENT_IND_TIMER, AMS_KILL_TIMER);
	} 

	if(lpAmsData->rcasRemoteCoopTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->rcasRemoteCoopTimerId);
		AmsTimerStatProc(T_AMS_RCAS_REMOTE_COOP_TIMER, AMS_KILL_TIMER);
	} 

	if(lpAmsData->vtmRemoteCoopTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->vtmRemoteCoopTimerId);
		AmsTimerStatProc(T_AMS_VTM_REMOTE_COOP_TIMER, AMS_KILL_TIMER);
	} 
	
	if(lpAmsData->snapTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->snapTimerId);
		AmsTimerStatProc(T_AMS_SNAP_TIMER, AMS_KILL_TIMER);
	}

	if(lpAmsData->sendMsgToVtaTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->sendMsgToVtaTimerId);
		AmsTimerStatProc(T_AMS_VTA_RECV_MSG_TIMER, AMS_KILL_TIMER);
	} 

	if(lpAmsData->sendMsgToVtmTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->sendMsgToVtmTimerId);
		AmsTimerStatProc(T_AMS_VTM_RECV_MSG_TIMER, AMS_KILL_TIMER);
	}

	if(lpAmsData->sendFileToVtaTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->sendFileToVtaTimerId);
		AmsTimerStatProc(T_AMS_VTA_RECV_FILE_TIMER, AMS_KILL_TIMER);
	} 

	if(lpAmsData->sendFileToVtmTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->sendFileToVtmTimerId);
		AmsTimerStatProc(T_AMS_VTM_RECV_FILE_TIMER, AMS_KILL_TIMER);
	} 

	if(lpAmsData->multiSessTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->multiSessTimerId);
		AmsTimerStatProc(T_AMS_MULTI_SESS_TIMER, AMS_KILL_TIMER);
	}

	if(lpAmsData->monitorTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->monitorTimerId);
		AmsTimerStatProc(T_AMS_MONITOR_TIMER, AMS_KILL_TIMER);
	}

	return AMS_OK;	
}

int AmsKillVtmAllTimer(VTM_NODE *pVtmNode, int pid)
{
	if(NULL == pVtmNode)
	{
		return AMS_ERROR;
	}

	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		return AMS_ERROR;
	}

	if(pVtmNode->iTimerId >= 0)
	{
	    AmsKillTimer(pid, &pVtmNode->iTimerId);//NO USE YET
//		AmsTimerStatProc(T_AMS_VTA_STATE_OPERATE_IND_TIMER, AMS_KILL_TIMER);
	} 
	
	if(pVtmNode->sendMsgToVtaTimerId >= 0)
	{
	    AmsKillTimer(pid, &pVtmNode->sendMsgToVtaTimerId);
		AmsTimerStatProc(T_AMS_VTA_RECV_MSG_TIMER, AMS_KILL_TIMER);
	} 
		
	if(pVtmNode->sendMsgToVtmTimerId >= 0)
	{
	    AmsKillTimer(pid, &pVtmNode->sendMsgToVtmTimerId);
		AmsTimerStatProc(T_AMS_VTM_RECV_MSG_TIMER, AMS_KILL_TIMER);
	} 

	if(pVtmNode->sendFileToVtaTimerId >= 0)
	{
	    AmsKillTimer(pid, &pVtmNode->sendFileToVtaTimerId);
		AmsTimerStatProc(T_AMS_VTA_RECV_FILE_TIMER, AMS_KILL_TIMER);
	} 

	if(pVtmNode->sendFileToVtmTimerId >= 0)
	{
	    AmsKillTimer(pid, &pVtmNode->sendFileToVtmTimerId);
		AmsTimerStatProc(T_AMS_VTM_RECV_FILE_TIMER, AMS_KILL_TIMER);
	} 

	if(pVtmNode->vtmParaCfgTimerId >= 0)
	{
	    AmsKillTimer(pid, &pVtmNode->vtmParaCfgTimerId);
		AmsTimerStatProc(T_AMS_VTM_PARA_CFG_TIMER, AMS_KILL_TIMER);
	} 
	
	return AMS_OK;	
}


int AmsKillVtmAllCallTimer(VTM_NODE *pVtmNode, int pid)
{
	if(NULL == pVtmNode)
	{
		return AMS_ERROR;
	}

	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		return AMS_ERROR;
	}

	if(pVtmNode->iTimerId >= 0)
	{
	    AmsKillTimer(pid, &pVtmNode->iTimerId);//NO USE YET
//		AmsTimerStatProc(T_AMS_VTA_STATE_OPERATE_IND_TIMER, AMS_KILL_TIMER);
	} 
	
	if(pVtmNode->sendMsgToVtaTimerId >= 0)
	{
	    AmsKillTimer(pid, &pVtmNode->sendMsgToVtaTimerId);
		AmsTimerStatProc(T_AMS_VTA_RECV_MSG_TIMER, AMS_KILL_TIMER);
	} 
		
	if(pVtmNode->sendMsgToVtmTimerId >= 0)
	{
	    AmsKillTimer(pid, &pVtmNode->sendMsgToVtmTimerId);
		AmsTimerStatProc(T_AMS_VTM_RECV_MSG_TIMER, AMS_KILL_TIMER);
	} 

	if(pVtmNode->sendFileToVtaTimerId >= 0)
	{
	    AmsKillTimer(pid, &pVtmNode->sendFileToVtaTimerId);
		AmsTimerStatProc(T_AMS_VTA_RECV_FILE_TIMER, AMS_KILL_TIMER);
	} 

	if(pVtmNode->sendFileToVtmTimerId >= 0)
	{
	    AmsKillTimer(pid, &pVtmNode->sendFileToVtmTimerId);
		AmsTimerStatProc(T_AMS_VTM_RECV_FILE_TIMER, AMS_KILL_TIMER);
	} 

	return AMS_OK;	
}


static int          freeAmsVtaIdListInitialled = 0;
pthread_mutex_t     freeAmsVtaIdListMtx;
LIST 				freeAmsVtaIdList;
VTA_ID_NODE        	*freeAmsVtaIdListBufPtr = NULL;

static int          freeAmsVtmIdListInitialled = 0;
pthread_mutex_t     freeAmsVtmIdListMtx;
LIST 				freeAmsVtmIdList;
VTM_ID_NODE         *freeAmsVtmIdListBufPtr = NULL;

static int          freeAmsOrgIdListInitialled = 0;
pthread_mutex_t     freeAmsOrgIdListMtx;
LIST 				freeAmsOrgIdList;
ORG_ID_NODE         *freeAmsOrgIdListBufPtr = NULL;

int VtaIdListInit()
{
	VTA_ID_NODE         *pNode;
	int                 size;	
	int                 i;
	
	lstInit(&freeAmsVtaIdList);
	
	size = sizeof(VTA_ID_NODE) * AMS_MAX_VTA_NUM;
	freeAmsVtaIdListBufPtr = (VTA_ID_NODE *)malloc(size);
	pNode = (VTA_ID_NODE *)freeAmsVtaIdListBufPtr;
	if(NULL == pNode)
	{
		dbgprint("VtaIdListInit AllocMem[%d]Err\r\n",size);
		return AMS_ALLOC_MEM_FAILED;
	}
	
	for(i = 0; i < AMS_MAX_VTA_NUM; i++,pNode++)
	{
        lstAdd(&freeAmsVtaIdList,(NODE *)pNode);
	}
	
    Pthread_mutex_init(&freeAmsVtaIdListMtx,NULL);
	
	freeAmsVtaIdListInitialled = 1;
	
	return AMS_SUCCESS;
}

int VtmIdListInit()
{
	VTM_ID_NODE         *pNode;
	int                 size;	
	int                 i;
	
	if(0 != freeAmsVtmIdListInitialled)
	{
		return AMS_SUCCESS;
	}

	lstInit(&freeAmsVtmIdList);
	
	size = sizeof(VTM_ID_NODE) * AMS_MAX_VTM_NUM;
	freeAmsVtmIdListBufPtr = (VTM_ID_NODE *)malloc(size);
	pNode = (VTM_ID_NODE *)freeAmsVtmIdListBufPtr;
	if(NULL == pNode)
	{
		dbgprint("VtmIdListInit AllocMem[%d]Err\r\n",size);		
		return AMS_ALLOC_MEM_FAILED;
	}
	
	for(i = 0; i < AMS_MAX_VTM_NUM; i++, pNode++)
	{
        lstAdd(&freeAmsVtmIdList,(NODE *)pNode);
	}
	
    Pthread_mutex_init(&freeAmsVtmIdListMtx,NULL);
	
	freeAmsVtmIdListInitialled = 1;
	
	return AMS_SUCCESS;
}

int OrgIdListInit()
{
	ORG_ID_NODE         *pNode;
	int                 size;	
	int                 i;
	
	if(0 != freeAmsOrgIdListInitialled)
	{
		return AMS_SUCCESS;
	}

	lstInit(&freeAmsOrgIdList);
	
	size = sizeof(ORG_ID_NODE) * AMS_MAX_ORG_NUM;
	freeAmsOrgIdListBufPtr = (ORG_ID_NODE *)malloc(size);
	pNode = (ORG_ID_NODE *)freeAmsOrgIdListBufPtr;
	if(NULL == pNode)
	{
		dbgprint("OrgIdListInit AllocMem[%d]Err\r\n",size);		
		return AMS_ALLOC_MEM_FAILED;
	}
	
	for(i = 0; i < AMS_MAX_ORG_NUM; i++, pNode++)
	{
        lstAdd(&freeAmsOrgIdList,(NODE *)pNode);
	}
	
    Pthread_mutex_init(&freeAmsOrgIdListMtx,NULL);
	
	freeAmsOrgIdListInitialled = 1;
	
	return AMS_SUCCESS;
}

//---------------------------------------------------------------
//功能： get an idle vta Id node
//入口： 
//返回： pNode
//说明： 
//----------------------------------------------------------------
VTA_ID_NODE  *AmsGetVtaIdNode()
{
	VTA_ID_NODE         *pNode = NULL;
	
    if(!freeAmsVtaIdListInitialled)
    {
		dbgprint("VtaIdNodeGet InitFlag[%d]Err\r\n",freeAmsVtaIdListInitialled);
        return NULL;
    }
	
    Pthread_mutex_lock(&freeAmsVtaIdListMtx);
    pNode = (VTA_ID_NODE *)lstGet(&freeAmsVtaIdList);
    Pthread_mutex_unlock(&freeAmsVtaIdListMtx);
	
	return pNode;
}

//---------------------------------------------------------------
//功能： free one vta id node
//入口： VTA_ID_NODE * pNode
//返回： AMS_OK
//	     AMS_ERROR	
//说明： 
//----------------------------------------------------------------
int AmsFreeVtaIdNode(VTA_ID_NODE *pNode)
{
    if(!freeAmsVtaIdListInitialled)
    {
        return;
    }
	
	if(NULL == pNode)
	{
		return AMS_ERROR;
	}
	
	pNode->tellerId = 0;
	pNode->tellerPos = 0xffffffff;

    Pthread_mutex_lock(&freeAmsVtaIdListMtx);
    lstAdd(&freeAmsVtaIdList,(NODE *)pNode);
    Pthread_mutex_unlock(&freeAmsVtaIdListMtx);
	
	return AMS_OK;
}


/*******************************************************************************
* AmsCalcVtaIdHashIdx - get vta id hash table index
*
* RETURNS: vta id hash table index
*/
static int AmsCalcVtaIdHashIdx(DWORD tellerId)
{
	return tellerId % AMS_VTA_ID_HASH_SIZE;
}

/*******************************************************************************
* AmsSearchVtaIdHash - search vta id node via tellerId
*
* RETURNS: a pointer to vta id node
*/
VTA_ID_NODE *AmsSearchVtaIdHash(DWORD tellerId)
{
	int                 hashIdx;
	VTA_ID_NODE         *pVtaIdNode = NULL;

	hashIdx = AmsCalcVtaIdHashIdx(tellerId);
	pVtaIdNode = AmsVtaIdHashTbl[hashIdx];

	while(pVtaIdNode)
	{
	    if(pVtaIdNode->tellerId == tellerId)
	    {
	        break;
	    }
		
	    pVtaIdNode = pVtaIdNode->hashNext;
	}

	return pVtaIdNode;

}

/*******************************************************************************
* AmsInsertVtaIdHash - insert a node to vta Id hash table 
*
* RETURNS: N/A
*/
void AmsInsertVtaIdHash(VTA_ID_NODE *pVtaIdNode)
{
	int                 hashIdx;

	if(NULL == pVtaIdNode)
	{
		return;
	}
	
	hashIdx = AmsCalcVtaIdHashIdx(pVtaIdNode->tellerId);
	pVtaIdNode->hashNext = AmsVtaIdHashTbl[hashIdx];
	AmsVtaIdHashTbl[hashIdx] = pVtaIdNode;
}

/*******************************************************************************
* AmsDelVtaIdHash - delete a node from vta id hash table
*
* RETURNS: N/A
*/
void AmsDeleteVtaIdHash(VTA_ID_NODE *pVtaIdNode)
{
	int                 hashIdx;
	VTA_ID_NODE         *pTmpNode = NULL;
	VTA_ID_NODE         *pPreNode = NULL;

	if(NULL == pVtaIdNode)
	{
		return;
	}

	hashIdx = AmsCalcVtaIdHashIdx(pVtaIdNode->tellerId);
	pTmpNode = AmsVtaIdHashTbl[hashIdx];
	pPreNode = NULL;

	while(pTmpNode)
	{
	    if(pTmpNode == pVtaIdNode)
	    {
	        break;
	    }
		
	    pPreNode = pTmpNode;
	    pTmpNode = pTmpNode->hashNext;
	}

	/* not found */
	if(NULL == pTmpNode)
	{
	    return;
	}

	/* at the head of list */
	if(NULL == pPreNode)
	{
	    AmsVtaIdHashTbl[hashIdx] = pTmpNode->hashNext;
	    return;
	}

	/* at middle or tail of the list */
	pPreNode->hashNext = pTmpNode->hashNext;
	
}

	   
//---------------------------------------------------------------
//功能： get an idle vtm Id node
//入口： 
//返回： pNode
//说明： 
//----------------------------------------------------------------
VTM_ID_NODE  *AmsGetVtmIdNode()
{
	VTM_ID_NODE         *pNode = NULL;

    if(!freeAmsVtmIdListInitialled)
    {
		dbgprint("VtmIdNodeGet InitFlag[%d]Err\r\n",freeAmsVtmIdListInitialled);		
        return NULL;
    }
	
    Pthread_mutex_lock(&freeAmsVtmIdListMtx);
    pNode = (VTM_ID_NODE *)lstGet(&freeAmsVtmIdList);
    Pthread_mutex_unlock(&freeAmsVtmIdListMtx);
	
	return pNode;
}

//---------------------------------------------------------------
//功能： free one vtm id node
//入口： VTM_ID_NODE * pNode
//返回： AMS_OK
//	     AMS_ERROR	
//说明： 
//----------------------------------------------------------------
int AmsFreeVtmIdNode(VTM_ID_NODE *pNode)
{
    if(!freeAmsVtmIdListInitialled)
    {
        return;
    }
	
	if(NULL == pNode)
	{
		return AMS_ERROR;
	}

	pNode->vtmId = 0;
	pNode->vtmPos = 0xffffffff;

    Pthread_mutex_lock(&freeAmsVtmIdListMtx);
    lstAdd(&freeAmsVtmIdList,(NODE *)pNode);
    Pthread_mutex_unlock(&freeAmsVtmIdListMtx);
	
	return AMS_OK;
}


/*******************************************************************************
* AmsCalcVtmIdHashIdx - get vtm id hash table index
*
* RETURNS: vtm id hash table index
*/
static int AmsCalcVtmIdHashIdx(DWORD vtmId)
{
	return vtmId % AMS_VTM_ID_HASH_SIZE;
}

/*******************************************************************************
* AmsSearchVtmIdHash - search vtm id node via vtmId
*
* RETURNS: a pointer to vta id node
*/
VTM_ID_NODE *AmsSearchVtmIdHash(DWORD vtmId)
{
	int                 hashIdx;
	VTM_ID_NODE         *pVtmIdNode = NULL;

	hashIdx = AmsCalcVtmIdHashIdx(vtmId);
	pVtmIdNode = AmsVtmIdHashTbl[hashIdx];

	while(pVtmIdNode)
	{
	    if(pVtmIdNode->vtmId == vtmId)
	    {
	        break;
	    }
		
	    pVtmIdNode = pVtmIdNode->hashNext;
	}

	return pVtmIdNode;
	
}

/*******************************************************************************
* AmsInsertVtmIdHash - insert a node to vtm Id hash table 
*
* RETURNS: N/A
*/
void AmsInsertVtmIdHash(VTM_ID_NODE *pVtmIdNode)
{
	int                 hashIdx;

	if(NULL == pVtmIdNode)
	{
		return;
	}

	hashIdx = AmsCalcVtmIdHashIdx(pVtmIdNode->vtmId);
	pVtmIdNode->hashNext = AmsVtmIdHashTbl[hashIdx];
	AmsVtmIdHashTbl[hashIdx] = pVtmIdNode;
}

/*******************************************************************************
* AmsDeleteVtmIdHash - delete a node from vtm id hash table
*
* RETURNS: N/A
*/
void AmsDeleteVtmIdHash(VTM_ID_NODE *pVtmIdNode)
{
	int                 hashIdx;
	VTM_ID_NODE         *pTmpNode = NULL;
	VTM_ID_NODE         *pPreNode = NULL;

	if(NULL == pVtmIdNode)
	{
		return;
	}

	hashIdx = AmsCalcVtmIdHashIdx(pVtmIdNode->vtmId);
	pTmpNode = AmsVtmIdHashTbl[hashIdx];
	pPreNode = NULL;

	while(pTmpNode)
	{
	    if(pTmpNode == pVtmIdNode)
	    {
	        break;
	    }
		
	    pPreNode = pTmpNode;
	    pTmpNode = pTmpNode->hashNext;
	}

	/* not found */
	if(NULL == pTmpNode)
	{
	    return;
	}

	/* at the head of list */
	if(NULL == pPreNode)
	{
	    AmsVtmIdHashTbl[hashIdx] = pTmpNode->hashNext;
	    return;
	}

	/* at middle or tail of the list */
	pPreNode->hashNext = pTmpNode->hashNext;
	
}

int AmsGetTellerNoById(unsigned int tellerId, unsigned char *pTellerNo)
{
	VTA_ID_NODE         *pVtaIdNode;

	if(0 == tellerId || NULL == pTellerNo)
	{
		return AMS_ERROR;
	}

	pVtaIdNode = AmsSearchVtaIdHash(tellerId);
	
	if(NULL == pVtaIdNode)
	{
		return AMS_ERROR;
	}

	if(pVtaIdNode->tellerPos >= AMS_MAX_VTA_NUM)
	{
		return AMS_ERROR;
	}
	
	if(    AmsCfgTeller(pVtaIdNode->tellerPos).tellerId != tellerId
		|| AmsCfgTeller(pVtaIdNode->tellerPos).tellerNoLen > AMS_MAX_TELLER_NO_LEN
		|| AmsCfgTeller(pVtaIdNode->tellerPos).flag != AMS_TELLER_INSTALL)
	{
		return AMS_ERROR;	
	}

	//get tellerNo
	memcpy(pTellerNo, AmsCfgTeller(pVtaIdNode->tellerPos).tellerNo, AmsCfgTeller(pVtaIdNode->tellerPos).tellerNoLen);
	pTellerNo[AmsCfgTeller(pVtaIdNode->tellerPos).tellerNoLen] = '\0';
		
	return AMS_OK;	
	
}

int AmsGetVtmNoById(unsigned int vtmId, unsigned char *pVtmNo)
{
	VTM_ID_NODE         *pVtmIdNode;

	if(0 == vtmId || NULL == pVtmNo)
	{
		return AMS_ERROR;
	}

	pVtmIdNode = AmsSearchVtmIdHash(vtmId);
	
	if(NULL == pVtmIdNode)
	{
		return AMS_ERROR;
	}

	if(pVtmIdNode->vtmPos >= AMS_MAX_VTM_NUM)
	{
		return AMS_ERROR;
	}
	
	if(    AmsCfgVtm(pVtmIdNode->vtmPos).vtmId != vtmId
		|| AmsCfgVtm(pVtmIdNode->vtmPos).vtmNoLen > AMS_MAX_VTM_NO_LEN
		|| AmsCfgVtm(pVtmIdNode->vtmPos).flag != AMS_VTM_INSTALL)
	{
		return AMS_ERROR;	
	}

	//get vtmNo
	memcpy(pVtmNo, AmsCfgVtm(pVtmIdNode->vtmPos).vtmNo, AmsCfgVtm(pVtmIdNode->vtmPos).vtmNoLen);
	pVtmNo[AmsCfgVtm(pVtmIdNode->vtmPos).vtmNoLen] = '\0';

	return AMS_OK;	
	
}

//---------------------------------------------------------------
//功能： get an idle org Id node
//入口： 
//返回： pNode
//说明： 
//----------------------------------------------------------------
ORG_ID_NODE  *AmsGetOrgIdNode()
{
	ORG_ID_NODE         *pNode = NULL;

    if(!freeAmsOrgIdListInitialled)
    {
		dbgprint("OrgIdNodeGet InitFlag[%d]Err\r\n",freeAmsOrgIdListInitialled);		
        return NULL;
    }
	
    Pthread_mutex_lock(&freeAmsOrgIdListMtx);
    pNode = (ORG_ID_NODE *)lstGet(&freeAmsOrgIdList);
    Pthread_mutex_unlock(&freeAmsOrgIdListMtx);
	
	return pNode;
}

//---------------------------------------------------------------
//功能： free one org id node
//入口： ORG_ID_NODE * pNode
//返回： AMS_OK
//	     AMS_ERROR	
//说明： 
//----------------------------------------------------------------
int AmsFreeOrgIdNode(ORG_ID_NODE *pNode)
{
    if(!freeAmsOrgIdListInitialled)
    {
        return;
    }
	
	if(NULL == pNode)
	{
		return AMS_ERROR;
	}

	pNode->orgId = 0;
	pNode->orgPos = 0xffffffff;

    Pthread_mutex_lock(&freeAmsOrgIdListMtx);
    lstAdd(&freeAmsOrgIdList,(NODE *)pNode);
    Pthread_mutex_unlock(&freeAmsOrgIdListMtx);
	
	return AMS_OK;
}


/*******************************************************************************
* AmsCalcOrgIdHashIdx - get org id hash table index
*
* RETURNS: org id hash table index
*/
static int AmsCalcOrgIdHashIdx(DWORD orgId)
{
	return orgId % AMS_ORG_ID_HASH_SIZE;
}

/*******************************************************************************
* AmsSearchOrgIdHash - search org id node via orgId
*
* RETURNS: a pointer to org id node
*/
ORG_ID_NODE *AmsSearchOrgIdHash(DWORD orgId)
{
	int                 hashIdx;
	ORG_ID_NODE         *pOrgIdNode = NULL;

	hashIdx = AmsCalcOrgIdHashIdx(orgId);
	pOrgIdNode = AmsOrgIdHashTbl[hashIdx];

	while(pOrgIdNode)
	{
	    if(pOrgIdNode->orgId == orgId)
	    {
	        break;
	    }
		
	    pOrgIdNode = pOrgIdNode->hashNext;
	}

	return pOrgIdNode;
	
}

/*******************************************************************************
* AmsInsertOrgIdHash - insert a node to org Id hash table 
*
* RETURNS: N/A
*/
void AmsInsertOrgIdHash(ORG_ID_NODE *pOrgIdNode)
{
	int                 hashIdx;

	if(NULL == pOrgIdNode)
	{
		return;
	}

	hashIdx = AmsCalcOrgIdHashIdx(pOrgIdNode->orgId);
	pOrgIdNode->hashNext = AmsOrgIdHashTbl[hashIdx];
	AmsOrgIdHashTbl[hashIdx] = pOrgIdNode;
}

/*******************************************************************************
* AmsDeleteOrgIdHash - delete a node from org id hash table
*
* RETURNS: N/A
*/
void AmsDeleteOrgIdHash(ORG_ID_NODE *pOrgIdNode)
{
	int                 hashIdx;
	ORG_ID_NODE         *pTmpNode = NULL;
	ORG_ID_NODE         *pPreNode = NULL;

	if(NULL == pOrgIdNode)
	{
		return;
	}

	hashIdx = AmsCalcOrgIdHashIdx(pOrgIdNode->orgId);
	pTmpNode = AmsOrgIdHashTbl[hashIdx];
	pPreNode = NULL;

	while(pTmpNode)
	{
	    if(pTmpNode == pOrgIdNode)
	    {
	        break;
	    }
		
	    pPreNode = pTmpNode;
	    pTmpNode = pTmpNode->hashNext;
	}

	/* not found */
	if(NULL == pTmpNode)
	{
	    return;
	}

	/* at the head of list */
	if(NULL == pPreNode)
	{
	    AmsOrgIdHashTbl[hashIdx] = pTmpNode->hashNext;
	    return;
	}

	/* at middle or tail of the list */
	pPreNode->hashNext = pTmpNode->hashNext;
	
}

//zhuyn added 20160721
#ifdef SIMU_TERMPARA_CNF    
//type==1 vta  type ==2 vtm 
//TellerId	DWORD	柜员标识
//ConfigInd	WORD	参见3.1.66注A
//Result	DWORD	参见5原因值定义
//TermConfigPara	O	注A
int AmsSimuTermParaCnf(BYTE type,MESSAGE_t *s_msg, DWORD result)
{
    MESSAGE_t *p_Msg;
    int pos;
    BYTE *srcp,*dstp;
    unsigned short configInd;
    OA_AMS_TERMPARA_CONFIG_t termPara;

    if (NULL == s_msg || NULL==(p_Msg = base_malloc(sizeof(MESSAGE_t))))
    {
        return -1;
    }

    p_Msg->eMessageAreaId = s_msg->eMessageAreaId;
    p_Msg->s_ReceiverPid = s_msg->s_SenderPid;
    p_Msg->s_SenderPid = s_msg->s_ReceiverPid;
    switch(s_msg->iMessageType)
    {
    case A_VTA_PARA_CFG_IND:
        p_Msg->iMessageType = A_VTA_PARA_CFG_IND_CNF;
        break;

    case A_VTM_PARA_CFG_IND:
        p_Msg->iMessageType = A_VTM_PARA_CFG_IND_CNF;
        break;

    default:
        base_free(p_Msg);
        return -1;
    }
    srcp = s_msg->cMessageBody;
    dstp = p_Msg->cMessageBody;
    memcpy(dstp, srcp, 4);    //TellerId 
    srcp += 4;
    dstp += 4;
        
    BEGETSHORT(configInd, srcp);
    if (CONFIGIND_UPDATE != configInd && CONFIGIND_GET != configInd)
    {
        base_free(p_Msg);
        return -1;
    }
    memcpy(dstp, srcp, 2);    //ConfigInd
    srcp += 2;
    dstp += 2;
        
    BEPUTLONG(result, dstp);    //Result
    dstp += 4;

    //TermConfigPara
    if (CONFIGIND_GET == configInd && 0 == result)
    {
        memset(termPara.flag,1,AMSTERMPARACFG_ITEM_MAX);
        termPara.Duration = 5000;
        termPara.LimitBWTx = 1024*1024;
        termPara.LimitBWRx = 1024*1024;
        termPara.AudioSampleRate = 0;
        termPara.AudioFrameRate = 100;
        termPara.AudioTargetBitRate = 64*1024;
        termPara.VedioDefination = 1;
        termPara.VedioFrameRate = 101;
        termPara.VedioTargetBitRate = 512*1024;
        termPara.AudioRecDefination = 2;
        termPara.AudioRecFrameRate = 102;
        termPara.AudioRecTargetBitRate = 32*1024;
        termPara.ScreenRecDefination = 0;
        termPara.ScreenRecFrameRate = 103;
        termPara.ScreenRecTargetBitRate = 256*1024;
        termPara.RemDesktopDefination = 1;
        termPara.RemDesktopFrameRate = 104;
        termPara.RemDesktopTargetBitRate = 128*1024;
        termPara.AppShareDefination = 2;
        termPara.AppShareFrameRate = 105;
        termPara.AppShareTargetBitRate = 64*1024;
        dstp += AmsPackTermConfigPara(&termPara, dstp, p_Msg->cMessageBody + MSG_BODY_LEN - dstp);
    }
    p_Msg->iMessageLength = dstp - p_Msg->cMessageBody;

    SendMsgBuff(p_Msg,0);
    if(AmsMsgTrace)
	{	
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"send A_VTA/M_PARA_CFG_IND msg[%u] \n",p_Msg->iMessageType);	
		AmsTraceToFile(p_Msg->s_ReceiverPid,p_Msg->s_SenderPid,"A_VTA/M_PARA_CFG_IND",description,
			descrlen,p_Msg->cMessageBody,p_Msg->iMessageLength,"ams");	
	}
    base_free(p_Msg);
    return 0;
}
#endif


//zry added for scc 2018
void AmsSetSeatCallState(LP_AMS_DATA_t *lpAmsData,SEAT_NODE *pSeatNode,int state)
{
	struct tm           *tLocalTime;      // 东8区

	if(NULL == lpAmsData || NULL == pSeatNode)
	{
		return;
	}
	
	pSeatNode->callState = state;

	//record call state start time
	time(&pSeatNode->callStateStartTime);
	tLocalTime = localtime(&pSeatNode->callStateStartTime);
	pSeatNode->callStateStartLocalTime.year   = tLocalTime->tm_year + 1900; //+1900
	pSeatNode->callStateStartLocalTime.month  = tLocalTime->tm_mon + 1;     //+1
	pSeatNode->callStateStartLocalTime.day    = tLocalTime->tm_mday;
	pSeatNode->callStateStartLocalTime.hour   = tLocalTime->tm_hour;
	pSeatNode->callStateStartLocalTime.minute = tLocalTime->tm_min;
	pSeatNode->callStateStartLocalTime.second = tLocalTime->tm_sec;
}

void AmsSetSeatState(int iThreadId,LP_AMS_DATA_t *lpAmsData,SEAT_NODE *pSeatNode,int state,int stateReason)
{
	struct tm			*tLocalTime;	  // 东8区
		
	if(NULL == lpAmsData || NULL == pSeatNode)
	{
		return;
	}
	
	pSeatNode->state = state;

	//record state start time
	time(&pSeatNode->stateStartTime);
	tLocalTime = localtime(&pSeatNode->stateStartTime);
	pSeatNode->stateStartLocalTime.year	 = tLocalTime->tm_year + 1900; //+1900
	pSeatNode->stateStartLocalTime.month  = tLocalTime->tm_mon + 1;	   //+1
	pSeatNode->stateStartLocalTime.day	 = tLocalTime->tm_mday;
	pSeatNode->stateStartLocalTime.hour	 = tLocalTime->tm_hour;
	pSeatNode->stateStartLocalTime.minute = tLocalTime->tm_min;
	pSeatNode->stateStartLocalTime.second = tLocalTime->tm_sec;

	if(pSeatNode->tellerCfgPos < AMS_MAX_SEAT_NUM)
	{
		AmsTellerStat(pSeatNode->tellerCfgPos).vtaStateInfo.lastState = pSeatNode->state;
		AmsTellerStat(pSeatNode->tellerCfgPos).vtaStateInfo.lastStateStartTime = pSeatNode->stateStartTime;
	}

	//柜员实时状态信息入库-柜员状态信息
/*	if(AMS_VTA_STATE_BUSY == state && VTA_STATE_OPERATE_BUSY_IN_CALL == stateReason)
	{
		//UpdateDbTellerState and InsertDbTellerSerialState by Cms
	}
	else
	{
		AmsUpdateDbTellerState(iThreadId, lpAmsData, AMS_TELLER_STATE_ITEM_BASE, state, &pSeatNode->stateStartTime);

		AmsInsertDbTellerSerialState(iThreadId, lpAmsData, state, &pSeatNode->stateStartLocalTime, 0);
	}
*/
}


void AmsSetUserState(int iThreadId,USER_NODE *pUserNode,int state)
{
	struct tm			*tLocalTime;	  // 东8区
	
	if(NULL == pUserNode)
	{
		return;
	}
	
	pUserNode->state = state;

	//record state start time
	time(&pUserNode->stateStartTime);
	tLocalTime = localtime(&pUserNode->stateStartTime);
	pUserNode->stateStartLocalTime.year	 = tLocalTime->tm_year + 1900; //+1900
	pUserNode->stateStartLocalTime.month  = tLocalTime->tm_mon + 1;	   //+1
	pUserNode->stateStartLocalTime.day	 = tLocalTime->tm_mday;
	pUserNode->stateStartLocalTime.hour	 = tLocalTime->tm_hour;
	pUserNode->stateStartLocalTime.minute = tLocalTime->tm_min;
	pUserNode->stateStartLocalTime.second = tLocalTime->tm_sec;

	AmsUpdateDbUserState(iThreadId,pUserNode,state); 

}
void AmsSetUserCallState(USER_NODE *pUserNode,int state)
{
	struct tm           *tLocalTime;      // 东8区

	if(NULL == pUserNode)
	{
		return;
	}
	
	pUserNode->callState = state;

	//record state start time
	time(&pUserNode->callStateStartTime);
	tLocalTime = localtime(&pUserNode->callStateStartTime);
	pUserNode->callStateStartLocalTime.year   = tLocalTime->tm_year + 1900; //+1900
	pUserNode->callStateStartLocalTime.month  = tLocalTime->tm_mon + 1;     //+1
	pUserNode->callStateStartLocalTime.day    = tLocalTime->tm_mday;
	pUserNode->callStateStartLocalTime.hour   = tLocalTime->tm_hour;
	pUserNode->callStateStartLocalTime.minute = tLocalTime->tm_min;
	pUserNode->callStateStartLocalTime.second = tLocalTime->tm_sec;

}
void AmsSetUserServiceState(USER_NODE *pUserNode,int state)
{
	struct tm           *tLocalTime;      // 东8区

	if(NULL == pUserNode)
	{
		return;
	}
	
	pUserNode->serviceState = state;

	//record state start time
	time(&pUserNode->serviceStateStartTime);
	tLocalTime = localtime(&pUserNode->serviceStateStartTime);
	pUserNode->serviceStateStartLocalTime.year   = tLocalTime->tm_year + 1900; //+1900
	pUserNode->serviceStateStartLocalTime.month  = tLocalTime->tm_mon + 1;     //+1
	pUserNode->serviceStateStartLocalTime.day    = tLocalTime->tm_mday;
	pUserNode->serviceStateStartLocalTime.hour   = tLocalTime->tm_hour;
	pUserNode->serviceStateStartLocalTime.minute = tLocalTime->tm_min;
	pUserNode->serviceStateStartLocalTime.second = tLocalTime->tm_sec;

}



//seat part

SEAT_NODE *AmsSearchSeatNode(unsigned char srvgrpid,unsigned char tellerId[],unsigned char tellerIdlen)
{
	SEAT_NODE *pSeatNode = NULL;	
	int i = 0;
	int find = 0;

	//通过 srvGrpId 找到 对应业务组下标
	
	pSeatNode = (SEAT_NODE *)lstFirst(&AmsSrvData(srvgrpid).seatList);
	while(NULL != pSeatNode && i < AMS_MAX_SEAT_NODES)
	{
		if(strcmp(pSeatNode->seatInfo.tellerId,tellerId)
		{
			find = 1;
			break;			
		}
		
		pSeatNode = (VTA_NODE *)lstNext((NODE *)pSeatNode);
		i++;
	}

	if(0 == find)
	{
		pSeatNode = NULL;	
	}
	
	return pSeatNode;

}


static int          freeAmsSeatIdListInitialled = 0;
pthread_mutex_t     freeAmsSeatIdListMtx;
LIST 				freeAmsSeatIdList;
VTA_ID_NODE        	*freeAmsSeatIdListBufPtr = NULL;


int SeatIdListInit()
{
	SEAT_ID_NODE         *pNode;
	int                 size;	
	int                 i;
	
	lstInit(&freeAmsSeatIdList);
	
	size = sizeof(SEAT_ID_NODE) * AMS_MAX_SEAT_NUM;
	freeAmsSeatIdListBufPtr = (SEAT_ID_NODE *)malloc(size);
	pNode = (SEAT_ID_NODE *)freeAmsSeatIdListBufPtr;
	if(NULL == pNode)
	{
		dbgprint("SeatIdListInit AllocMem[%d]Err\r\n",size);
		return AMS_ALLOC_MEM_FAILED;
	}
	
	for(i = 0; i < AMS_MAX_SEAT_NUM; i++,pNode++)
	{
        lstAdd(&freeAmsSeatIdList,(NODE *)pNode);
	}
	
    Pthread_mutex_init(&freeAmsSeatIdListMtx,NULL);
	
	freeAmsSeatIdListInitialled = 1;
	
	return AMS_SUCCESS;
}

//---------------------------------------------------------------
//功能： get an idle seat Id node
//入口： 
//返回： pNode
//说明： 
//----------------------------------------------------------------
SEAT_ID_NODE  *AmsGetSeatIdNode()
{
	SEAT_ID_NODE         *pNode = NULL;
	
    if(!freeAmsSeatIdListInitialled)
    {
		dbgprint("VtaIdNodeGet InitFlag[%d]Err\r\n",freeAmsSeatIdListInitialled);
        return NULL;
    }
	
    Pthread_mutex_lock(&freeAmsSeatIdListMtx);
    pNode = (SEAT_ID_NODE *)lstGet(&freeAmsSeatIdList);
    Pthread_mutex_unlock(&freeAmsSeatIdListMtx);
	
	return pNode;
}


//---------------------------------------------------------------
//功能： free one seat id node
//入口： SEAT_ID_NODE * pNode
//返回： AMS_OK
//	     AMS_ERROR	
//说明： 
//----------------------------------------------------------------
int AmsFreeSeatIdNode(SEAT_ID_NODE *pNode)
{
    if(!freeAmsSeatIdListInitialled)
    {
        return;
    }
	
	if(NULL == pNode)
	{
		return AMS_ERROR;
	}
	
	memset(pNode->tellerId,0,AMS_MAX_TELLER_ID_LEN+1);
	pNode->tellerPos = 0xffffffff;

    Pthread_mutex_lock(&freeAmsSeatIdListMtx);
    lstAdd(&freeAmsSeatIdListMtx,(NODE *)pNode);
    Pthread_mutex_unlock(&freeAmsSeatIdListMtx);
	
	return AMS_OK;
}



/*******************************************************************************
* AmsCalcSeatIdHashIdx - get seat id hash table index
*
* RETURNS: seat id hash table index
*/
static int AmsCalcSeatIdHashIdx(unsigned char tellerId[],unsigned char tellerIdlen)
{
	return tellerId % AMS_VTA_ID_HASH_SIZE;//算法待换
}

/*******************************************************************************
* AmsSearchSeatIdHash - search seat id node via tellerId
*
* RETURNS: a pointer to vta id node
*/
SEAT_ID_NODE *AmsSearchSeatIdHash(unsigned char tellerId[])
{
	int                 hashIdx;
	SEAT_ID_NODE         *pSeatIdNode = NULL;

	hashIdx = AmsCalcSeatIdHashIdx(tellerId);
	pSeatIdNode = AmsSeatIdHashTbl[hashIdx];

	while(pSeatIdNode)
	{
	    if(strcmp(pSeatIdNode->tellerId,tellerId))
	    {
	        break;
	    }
		
	    pSeatIdNode = pSeatIdNode->hashNext;
	}

	return pSeatIdNode;

}


/*******************************************************************************
* AmsInsertSeatIdHash - insert a node to seat Id hash table 
*
* RETURNS: N/A
*/
void AmsInsertSeatIdHash(SEAT_ID_NODE *pSeatIdNode)
{
	int                 hashIdx;

	if(NULL == pSeatIdNode)
	{
		return;
	}
	
	hashIdx = AmsCalcSeatIdHashIdx(pSeatIdNode->tellerId);
	pSeatIdNode->hashNext = AmsSeatIdHashTbl[hashIdx];
	AmsSeatIdHashTbl[hashIdx] = pSeatIdNode;
}

/*******************************************************************************
* AmsDelSeatIdHash - delete a node from seat id hash table
*
* RETURNS: N/A
*/
void AmsDeleteSeatIdHash(SEAT_ID_NODE *pSeatIdNode)
{
	int                 hashIdx;
	SEAT_ID_NODE         *pTmpNode = NULL;
	SEAT_ID_NODE         *pPreNode = NULL;

	if(NULL == pSeatIdNode)
	{
		return;
	}

	hashIdx = AmsCalcSeatIdHashIdx(pSeatIdNode->tellerId);
	pTmpNode = AmsVtaIdHashTbl[hashIdx];
	pPreNode = NULL;

	while(pTmpNode)
	{
	    if(pTmpNode == pSeatIdNode)
	    {
	        break;
	    }
		
	    pPreNode = pTmpNode;
	    pTmpNode = pTmpNode->hashNext;
	}

	/* not found */
	if(NULL == pTmpNode)
	{
	    return;
	}

	/* at the head of list */
	if(NULL == pPreNode)
	{
	    AmsSeatIdHashTbl[hashIdx] = pTmpNode->hashNext;
	    return;
	}

	/* at middle or tail of the list */
	pPreNode->hashNext = pTmpNode->hashNext;
	
}


//需要研究源代码为什么是*入参
int AmsGetSeatIdNoById(unsigned char tellerId[])
{
	SEAT_ID_NODE         *pSeatIdNode;

	if(0 == tellerId)
	{
		return AMS_ERROR;
	}

	pSeatIdNode = AmsSearchSeatIdHash(tellerId);
	
	if(NULL == pSeatIdNode)
	{
		return AMS_ERROR;
	}

	if(pSeatIdNode->tellerPos >= AMS_MAX_VTA_NUM)
	{
		return AMS_ERROR;
	}
	
	if(    (0 != strcmp(AmsCfgTeller(pSeatIdNode->tellerPos).tellerId, tellerId))
		|| AmsCfgTeller(pSeatIdNode->tellerPos).tellerIdLen > AMS_MAX_TELLER_NO_LEN
		|| AmsCfgTeller(pSeatIdNode->tellerPos).flag != AMS_TELLER_INSTALL)
	{
		return AMS_ERROR;	
	}

	//get tellerNo
	//memcpy(pTellerNo, AmsCfgTeller(pVtaIdNode->tellerPos).tellerNo, AmsCfgTeller(pVtaIdNode->tellerPos).tellerNoLen);
	//pTellerNo[AmsCfgTeller(pVtaIdNode->tellerPos).tellerNoLen] = '\0';
		
	return AMS_OK;	
	
}


//user part
//user part
USER_NODE *AmsSearchUserNode(unsigned int srvGrpId, unsigned char userId)
{
	USER_NODE *pUserNode = NULL;	
	int i = 0;
	int find = 0;
	
	pUserNode = (USER_NODE *)lstFirst(&AmsSrvData(srvGrpId).userList);
	while(NULL != pUserNode && i < AMS_MAX_USER_NODES)
    {
        if(0 == strcmp(pUserNode->userInfo.userId, userId))
        {
			find = 1;
			break;			
        }
		
        pUserNode = (USER_NODE *)lstNext((NODE *)pUserNode);
		i++;
    }

	if(0 == find)
	{
		pUserNode = NULL;	
	}
	
    return pUserNode;
	
}


//added end

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

int AmsKillCrmAllTimer(LP_AMS_DATA_t *lpAmsData, int pid)
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
		AmsTimerStatProc(T_AMS_CRM_STATE_OPERATE_IND_TIMER, AMS_KILL_TIMER);
	} 

	if(lpAmsData->restTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->restTimerId);
		AmsTimerStatProc(T_AMS_REST_TIMER, AMS_KILL_TIMER);
	} 

	return AMS_OK;	
}


int AmsKillTermAllTimer(TERM_NODE *pTermNode, int pid)
{
	if(NULL == pTermNode)
	{
		return AMS_ERROR;
	}

	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		return AMS_ERROR;
	}

	if(pTermNode->iTimerId >= 0)
	{
	    AmsKillTimer(pid, &pTermNode->iTimerId);//NO USE YET
		AmsTimerStatProc(T_AMS_CRM_STATE_OPERATE_IND_TIMER, AMS_KILL_TIMER);
	}
	
	return AMS_OK;	
}

int AmsKillTermAllCallTimer(TERM_NODE *pTermNode, int pid)
{
	if(NULL == pTermNode)
	{
		return AMS_ERROR;
	}

	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		return AMS_ERROR;
	}

	if(pTermNode->iTimerId >= 0)
	{
	    AmsKillTimer(pid, &pTermNode->iTimerId);//NO USE YET
		AmsTimerStatProc(T_AMS_CRM_STATE_OPERATE_IND_TIMER, AMS_KILL_TIMER);
	} 
	
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

	"T_AMS_CUSTOMER_IN_QUEUE_TIMER",
	"T_AMS_CRM_STATE_OPERATE_IND_TIMER",
	"T_AMS_REST_TIMER",

	
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
	case AMS_CRM_STATE:
		switch(state)
		{
		case AMS_CRM_STATE_IDLE:
			return "Idle";
		case AMS_CRM_STATE_BUSY:
			return "Busy";
		case AMS_CRM_STATE_REST:
			return "Rest";
		case AMS_CRM_STATE_PREPARE:
			return "Prepare";
	    case AMS_CRM_STATE_OFFLINE:
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
    case AMS_TERM_STATE:
        switch(state)
		{
		case AMS_TERM_STATE_IDLE:
			return "Idle";
		case AMS_TERM_STATE_BUSY:
			return "Busy";
        case AMS_TERM_STATE_OFFLINE:
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



void AmsSetCrmState(int iThreadId,LP_AMS_DATA_t *lpAmsData,CRM_NODE *pCrmNode,int state,int stateReason)
{
	struct tm           *tLocalTime;      // 东8区
	
	if(NULL == lpAmsData || NULL == pCrmNode)
	{
		return;
	}
	
	pCrmNode->state = state;

	//record state start time
	time(&pCrmNode->stateStartTime);
	tLocalTime = localtime(&pCrmNode->stateStartTime);
	pCrmNode->stateStartLocalTime.year   = tLocalTime->tm_year + 1900; //+1900
	pCrmNode->stateStartLocalTime.month  = tLocalTime->tm_mon + 1;     //+1
	pCrmNode->stateStartLocalTime.day    = tLocalTime->tm_mday;
	pCrmNode->stateStartLocalTime.hour   = tLocalTime->tm_hour;
	pCrmNode->stateStartLocalTime.minute = tLocalTime->tm_min;
	pCrmNode->stateStartLocalTime.second = tLocalTime->tm_sec;

	if(pCrmNode->tellerCfgPos < AMS_MAX_CRM_NUM)
	{
		AmsTellerStat(pCrmNode->tellerCfgPos).crmStateInfo.lastState = pCrmNode->state;
		AmsTellerStat(pCrmNode->tellerCfgPos).crmStateInfo.lastStateStartTime = pCrmNode->stateStartTime;
	}

	//柜员实时状态信息入库-柜员状态信息
	/*if(AMS_CRM_STATE_BUSY == state && CRM_STATE_OPERATE_BUSY_IN_CALL == stateReason)
	{
		//UpdateDbTellerState and InsertDbTellerSerialState by Cms
	}
	else
	{
		AmsUpdateDbTellerState(iThreadId, lpAmsData, AMS_TELLER_STATE_ITEM_BASE, state, &pCrmNode->stateStartTime);

		AmsInsertDbTellerSerialState(iThreadId, lpAmsData, state, &pCrmNode->stateStartLocalTime, 0);
	}*/

}

void AmsSetCrmCallState(LP_AMS_DATA_t *lpAmsData,CRM_NODE *pCrmNode,int state)
{
	struct tm           *tLocalTime;      // 东8区

	if(NULL == lpAmsData || NULL == pCrmNode)
	{
		return;
	}
	
	pCrmNode->callState = state;

	//record call state start time
	time(&pCrmNode->callStateStartTime);
	tLocalTime = localtime(&pCrmNode->callStateStartTime);
	pCrmNode->callStateStartLocalTime.year   = tLocalTime->tm_year + 1900; //+1900
	pCrmNode->callStateStartLocalTime.month  = tLocalTime->tm_mon + 1;     //+1
	pCrmNode->callStateStartLocalTime.day    = tLocalTime->tm_mday;
	pCrmNode->callStateStartLocalTime.hour   = tLocalTime->tm_hour;
	pCrmNode->callStateStartLocalTime.minute = tLocalTime->tm_min;
	pCrmNode->callStateStartLocalTime.second = tLocalTime->tm_sec;

}



void AmsSetTermState(int iThreadId,TERM_NODE *pTermNode,int state)
{
	struct tm           *tLocalTime;      // 东8区

	if(NULL == pTermNode)
	{
		return;
	}
	
	pTermNode->state = state;

	//record state start time
	time(&pTermNode->stateStartTime);
	tLocalTime = localtime(&pTermNode->stateStartTime);
	pTermNode->stateStartLocalTime.year   = tLocalTime->tm_year + 1900; //+1900
	pTermNode->stateStartLocalTime.month  = tLocalTime->tm_mon + 1;     //+1
	pTermNode->stateStartLocalTime.day    = tLocalTime->tm_mday;
	pTermNode->stateStartLocalTime.hour   = tLocalTime->tm_hour;
	pTermNode->stateStartLocalTime.minute = tLocalTime->tm_min;
	pTermNode->stateStartLocalTime.second = tLocalTime->tm_sec;

	//AmsUpdateDbVtmState(iThreadId,pTermNode,state); 
	
	if(AmsStateTrace)
	{
		unsigned char description [256];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,256,"Term[%s] AmsPid[0x%x] CustPid[0x%x]\r\n",
			pTermNode->termInfo.termId,
			pTermNode->amsPid, pTermNode->customerPid);
		descrlen +=snprintf(description+descrlen,256-descrlen,"Current[%d-%d-%d %d:%d:%d]State=%s;",
			pTermNode->stateStartLocalTime.year,
			pTermNode->stateStartLocalTime.month,
			pTermNode->stateStartLocalTime.day,
			pTermNode->stateStartLocalTime.hour,
			pTermNode->stateStartLocalTime.minute,
			pTermNode->stateStartLocalTime.second,
			AmsGetStateTypeString(AMS_TERM_STATE, pTermNode->state));
			
		AmsTraceInfoToFile(pTermNode->amsPid,0,description,descrlen,"ams"); //pVtmNode->amsPid not used, so not add pVtmNode->customerPid para
	}
}

void AmsSetTermCallState(TERM_NODE *pTermNode,int state)
{
	struct tm           *tLocalTime;      // 东8区

	if(NULL == pTermNode)
	{
		return;
	}
	
	pTermNode->callState = state;

	//record state start time
	time(&pTermNode->callStateStartTime);
	tLocalTime = localtime(&pTermNode->callStateStartTime);
	pTermNode->callStateStartLocalTime.year   = tLocalTime->tm_year + 1900; //+1900
	pTermNode->callStateStartLocalTime.month  = tLocalTime->tm_mon + 1;     //+1
	pTermNode->callStateStartLocalTime.day    = tLocalTime->tm_mday;
	pTermNode->callStateStartLocalTime.hour   = tLocalTime->tm_hour;
	pTermNode->callStateStartLocalTime.minute = tLocalTime->tm_min;
	pTermNode->callStateStartLocalTime.second = tLocalTime->tm_sec;

	if(AmsStateTrace)
	{
		unsigned char description [256];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,256,"Term[%s] AmsPid[0x%x]\r\n",
			pTermNode->termInfo.termId, pTermNode->amsPid);
		descrlen +=snprintf(description+descrlen,256-descrlen,"Current[%d-%d-%d %d:%d:%d]CallState=%s;",
			pTermNode->callStateStartLocalTime.year,
			pTermNode->callStateStartLocalTime.month,
			pTermNode->callStateStartLocalTime.day,
			pTermNode->callStateStartLocalTime.hour,
			pTermNode->callStateStartLocalTime.minute,
			pTermNode->callStateStartLocalTime.second,
			AmsGetStateTypeString(AMS_CALL_STATE, pTermNode->callState));
			
		AmsTraceInfoToFile(pTermNode->amsPid,0,description,descrlen,"ams");
	}
}

void AmsSetTermServiceState(TERM_NODE *pTermNode,int state)
{
	struct tm           *tLocalTime;      // 东8区

	if(NULL == pTermNode)
	{
		return;
	}
	
	pTermNode->serviceState = state;

	//record state start time
	time(&pTermNode->serviceStateStartTime);
	tLocalTime = localtime(&pTermNode->serviceStateStartTime);
	pTermNode->serviceStateStartLocalTime.year   = tLocalTime->tm_year + 1900; //+1900
	pTermNode->serviceStateStartLocalTime.month  = tLocalTime->tm_mon + 1;     //+1
	pTermNode->serviceStateStartLocalTime.day    = tLocalTime->tm_mday;
	pTermNode->serviceStateStartLocalTime.hour   = tLocalTime->tm_hour;
	pTermNode->serviceStateStartLocalTime.minute = tLocalTime->tm_min;
	pTermNode->serviceStateStartLocalTime.second = tLocalTime->tm_sec;

	if(AmsStateTrace)
	{
		unsigned char description [256];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,256,"Term[%s] AmsPid[0x%x] Customer[%u]\r\n",
			pTermNode->termInfo.termId,pTermNode->amsPid, pTermNode->customerPid);
		descrlen +=snprintf(description+descrlen,256-descrlen,"Current[%d-%d-%d %d:%d:%d]ServiceState=%s;",
			pTermNode->serviceStateStartLocalTime.year,
			pTermNode->serviceStateStartLocalTime.month,
			pTermNode->serviceStateStartLocalTime.day,
			pTermNode->serviceStateStartLocalTime.hour,
			pTermNode->serviceStateStartLocalTime.minute,
			pTermNode->serviceStateStartLocalTime.second,
			AmsGetStateTypeString(AMS_CUSTOMER_STATE, pTermNode->serviceState));
			
		AmsTraceInfoToFile(pTermNode->amsPid,0,description,descrlen,"ams");
	}
}


CRM_NODE *AmsSearchCrmNode(unsigned int srvGrpId, unsigned char tellerId[],unsigned int tellerIdLen)
{
	CRM_NODE *pCrmNode = NULL;	
	int i = 0;
	int find = 0;
	
	pCrmNode = (CRM_NODE *)lstFirst(&AmsSrvData(srvGrpId).crmList);
	while(NULL != pCrmNode && i < AMS_MAX_CRM_NODES)
	{
        if(0 == strcmp(pCrmNode->crmInfo.tellerId,tellerId))
        {
			find = 1;
			break;			
        }
		
        pCrmNode = (CRM_NODE *)lstNext((NODE *)pCrmNode);
		i++;
    }

	if(0 == find)
	{
		pCrmNode = NULL;	
	}
	
    return pCrmNode;
	
}

//no offline
int AmsUpdateCrmState(int iThreadId, LP_AMS_DATA_t *lpAmsData, CRM_NODE *pCrmNode, int stateOperate, int stateReason)
{
	int                 iret = AMS_OK;
	int                 newState = AMS_CRM_STATE_RSVD;
	time_t              currentTime;

	if(NULL == lpAmsData || NULL == pCrmNode)
	{
		iret = AMS_ERROR;
		return iret;
	}

    switch(pCrmNode->state)
	{
	case AMS_CRM_STATE_IDLE:
		if(stateOperate == CRM_STATE_OPERATE_BUSY)
		{
			newState = AMS_CRM_STATE_BUSY;
		}
		else if(stateOperate == CRM_STATE_OPERATE_REST)
		{
			newState = AMS_CRM_STATE_REST;
		}	
		else if(stateOperate == CRM_STATE_OPERATE_PREPARE)
		{
			newState = AMS_CRM_STATE_PREPARE;
		}
//		else if(stateOperate == CRM_STATE_OPERATE_LEAVE)
//		{
//			newState = AMS_CRM_STATE_LEAVE;
//		}
		else if(stateOperate == CRM_STATE_OPERATE_IDLE)
		{
//			return iret;
			newState = AMS_CRM_STATE_IDLE;
		}
		else
		{
			dbgprint("AMS Recv Err Operate[%d] in Idle State", stateOperate);
			iret = AMS_ERROR;
		}	
		break;	
	case AMS_CRM_STATE_BUSY:
		if(stateOperate == CRM_STATE_OPERATE_IDLE)
		{
			newState = AMS_CRM_STATE_IDLE;
		}
		else if(stateOperate == CRM_STATE_OPERATE_REST)
		{
			newState = AMS_CRM_STATE_REST;
		}		
		else if(stateOperate == CRM_STATE_OPERATE_PREPARE)
		{
			newState = AMS_CRM_STATE_PREPARE;
		}
//		else if(stateOperate == CRM_STATE_OPERATE_LEAVE)
//		{
//			newState = AMS_CRM_STATE_LEAVE;
//		}
		else if(stateOperate == CRM_STATE_OPERATE_BUSY)
		{
//			return iret;
			newState = AMS_CRM_STATE_BUSY;
		}
		else
		{
			dbgprint("AMS Recv Err Operate[%d] in Busy State", stateOperate);
			iret = AMS_ERROR;
		}	
		break;
	case AMS_CRM_STATE_REST:
		if(stateOperate == CRM_STATE_OPERATE_IDLE)
		{
			newState = AMS_CRM_STATE_IDLE;
		}
		else if(stateOperate == CRM_STATE_OPERATE_PREPARE)
		{
			newState = AMS_CRM_STATE_PREPARE;
		}
//		else if(stateOperate == CRM_STATE_OPERATE_LEAVE)
//		{
//			newState = AMS_CRM_STATE_LEAVE;
//		}		
		else if(stateOperate == CRM_STATE_OPERATE_REST)
		{
//			return iret;
			newState = AMS_CRM_STATE_REST;
		}
		else
		{
			dbgprint("AMS Recv Err Operate[%d] in Rest State", stateOperate);
			iret = AMS_ERROR;
		}		
		break;		
	case AMS_CRM_STATE_PREPARE:
		if(stateOperate == CRM_STATE_OPERATE_REST)
		{
			newState = AMS_CRM_STATE_REST;
		}	
		else if(stateOperate == AMS_CRM_STATE_PREPARE)
		{
//			return iret;
			newState = AMS_CRM_STATE_PREPARE;
		}
		else
		{
			dbgprint("AMS Recv Err Operate[%d] in Prepara State", stateOperate);
			iret = AMS_ERROR;
		}
		break;	
	case AMS_CRM_STATE_OFFLINE:
		if(stateOperate == CRM_STATE_OPERATE_IDLE)
		{
			newState = AMS_CRM_STATE_IDLE;
		}
		else if(stateOperate == AMS_CRM_STATE_OFFLINE)
		{
//			return iret;
			newState = AMS_CRM_STATE_OFFLINE;
		}		
		else
		{
			dbgprint("AMS Recv Err Operate[%d] in Offline State", stateOperate);
			iret = AMS_ERROR;
		}
		break;			
	default:
		dbgprint("AMS Err State[%d]",pCrmNode->state);
		iret = AMS_ERROR;
		break;
	}
	
	if(AMS_OK != iret)
	{
		return iret;
	}
	
    //calc crm workInfo
	time(&currentTime);	   
	AmsUpdateSingleCrmWorkInfo(pCrmNode, currentTime);
	
	//set Crm State and State Start Time
	AmsSetCrmState(iThreadId, lpAmsData, pCrmNode, newState, stateReason);

	return iret;
}


TERM_NODE *AmsSearchTermNode(unsigned int srvGrpId, unsigned char termId[],unsigned char termIdLen)
{
	TERM_NODE *pTermNode = NULL;	
	int i = 0;
	int find = 0;
	
	pTermNode = (TERM_NODE *)lstFirst(&AmsSrvData(srvGrpId).termList);
	while(NULL != pTermNode && i < AMS_MAX_TERM_NODES)
    {
        if(0 == memcmp(pTermNode->termInfo.termId,termId,termIdLen))
        {
			find = 1;
			break;			
        }
		
        pTermNode = (TERM_NODE *)lstNext((NODE *)pTermNode);
		i++;
    }

	if(0 == find)
	{
		pTermNode = NULL;	
	}
	
    return pTermNode;
	
}




int AmsKillCrmAllCallTimer(LP_AMS_DATA_t *lpAmsData, int pid)
{
	if(NULL == lpAmsData)
	{
		return AMS_ERROR;
	}

	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		return AMS_ERROR;
	}
	


	return AMS_OK;	
}

/*******************************************************************************
* AmsCalcTellerInfoHashIdx - get teller info hash table index
*
* RETURNS: teller info hash table index
*/
static int AmsCalcTellerInfoHashIdx(unsigned char tellerId[],unsigned char len)
{
	return __BKDRHash(tellerId,len)%AMS_CRM_ID_HASH_SIZE;
}

/*******************************************************************************
* AmsSearchTellerInfoHash - search teller info node via tellerId
*
* RETURNS: a pointer to teller info node
*/
TELLER_INFO_NODE *AmsSearchTellerInfoHash(unsigned char tellerId[],unsigned char len)
{
	int                 hashIdx;
	TELLER_INFO_NODE    *pTellerInfoNode = NULL;

	hashIdx = AmsCalcTellerInfoHashIdx(tellerId,len);
	pTellerInfoNode= AmsCfgTellerHashTbl[hashIdx];

	while(pTellerInfoNode)
	{
	    if(0 == strcmp(pTellerInfoNode->tellerId,tellerId))
	    {
	        break;
	    }
		
	    pTellerInfoNode = pTellerInfoNode->hashNext;
	}

	return pTellerInfoNode;
	
}


/*******************************************************************************
* AmsInsertTellerInfoHash - insert a node to teller info hash table 
*
* RETURNS: N/A
*/
void AmsInsertTellerInfoHash(TELLER_INFO_NODE *pTellerInfoNode)
{
	int                 hashIdx;

	if(NULL == pTellerInfoNode)
	{
		return;
	}

	hashIdx = AmsCalcTellerInfoHashIdx(pTellerInfoNode->tellerId,pTellerInfoNode->tellerIdLen);
	pTellerInfoNode->hashNext = AmsCfgTellerHashTbl[hashIdx];
	AmsCfgTellerHashTbl[hashIdx] = pTellerInfoNode;
}



/*******************************************************************************
* AmsSearchRegTellerInfoHash - search teller info node via tellerId
*
* RETURNS: a pointer to teller info node
*/
TELLER_REGISTER_INFO_NODE *AmsSearchRegTellerInfoHash(unsigned char tellerId[],unsigned char len)
{
	int                 hashIdx;
	TELLER_REGISTER_INFO_NODE    *pTellerInfoNode = NULL;

	hashIdx = AmsCalcTellerInfoHashIdx(tellerId,len);
	pTellerInfoNode= AmsRegTellerHashTbl[hashIdx];

	while(pTellerInfoNode)
	{
	    if(0 == strcmp(pTellerInfoNode->tellerRegInfo.tellerId,tellerId))
	    {
	        break;
	    }
		
	    pTellerInfoNode = pTellerInfoNode->hashNext;
	}

	return pTellerInfoNode;
	
}



/*******************************************************************************
* AmsInsertRegTellerInfoHash - insert a node to teller info hash table 
*
* RETURNS: N/A
*/
void AmsInsertRegTellerInfoHash(TELLER_REGISTER_INFO_NODE *pTellerInfoNode)
{
	int                 hashIdx;

	if(NULL == pTellerInfoNode)
	{
		return;
	}

	hashIdx = AmsCalcTellerInfoHashIdx(pTellerInfoNode->tellerRegInfo.tellerId,pTellerInfoNode->tellerRegInfo.tellerIdLen);
	pTellerInfoNode->hashNext = AmsRegTellerHashTbl[hashIdx];
	AmsRegTellerHashTbl[hashIdx] = pTellerInfoNode;
}


/*******************************************************************************
* AmsCalcVtmInfoHashIdx - get vtm info hash table index
*
* RETURNS: vtm info hash table index
*/
static int AmsCalcVtmInfoHashIdx(unsigned char vtmId[],unsigned char len)
{
	return __BKDRHash(vtmId,len)%AMS_VTM_ID_HASH_SIZE;
}

/*******************************************************************************
* AmsSearchTellerInfoHash - search teller info node via tellerId
*
* RETURNS: a pointer to teller info node
*/
VTM_INFO_NODE *AmsSearchVtmInfoHash(unsigned char vtmId[],unsigned char len)
{
	int                 hashIdx;
	VTM_INFO_NODE    *pVtmInfoNode = NULL;

	hashIdx = AmsCalcVtmInfoHashIdx(vtmId,len);
	pVtmInfoNode= AmsCfgVtmHashTbl[hashIdx];

	while(pVtmInfoNode)
	{
	    if(0 == strcmp(pVtmInfoNode->vtmId,vtmId))
	    {
	        break;
	    }
		
	    pVtmInfoNode = pVtmInfoNode->hashNext;
	}

	return pVtmInfoNode;
	
}


/*******************************************************************************
* AmsInsertVtmInfoHash - insert a node to vtm info hash table 
*
* RETURNS: N/A
*/
void AmsInsertVtmInfoHash(VTM_INFO_NODE *pVtmInfoNode)
{
	int                 hashIdx;

	if(NULL == pVtmInfoNode)
	{
		return;
	}

	hashIdx = AmsCalcVtmInfoHashIdx(pVtmInfoNode->vtmId,pVtmInfoNode->vtmIdLen);
	pVtmInfoNode->hashNext = AmsCfgVtmHashTbl[hashIdx];
	AmsCfgVtmHashTbl[hashIdx] = pVtmInfoNode;
}



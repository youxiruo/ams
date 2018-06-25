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
	/*if(AMS_VTA_STATE_BUSY == state && VTA_STATE_OPERATE_BUSY_IN_CALL == stateReason)
	{
		//UpdateDbTellerState and InsertDbTellerSerialState by Cms
	}
	else
	{
		AmsUpdateDbTellerState(iThreadId, lpAmsData, AMS_TELLER_STATE_ITEM_BASE, state, &pVtaNode->stateStartTime);

		AmsInsertDbTellerSerialState(iThreadId, lpAmsData, state, &pVtaNode->stateStartLocalTime, 0);
	}*/

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

}


VTA_NODE *AmsSearchVtaNode(unsigned int srvGrpId, unsigned char tellerId[],unsigned int tellerIdLen)
{
	VTA_NODE *pVtaNode = NULL;	
	int i = 0;
	int find = 0;
	
	pVtaNode = (VTA_NODE *)lstFirst(&AmsSrvData(srvGrpId).vtaList);
	while(NULL != pVtaNode && i < AMS_MAX_VTA_NODES)
	{
        if(0 == strcmp(pVtaNode->vtaInfo.tellerId,tellerId))
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
		if(stateOperate == VTA_STATE_OPERATE_REST)
		{
			newState = AMS_VTA_STATE_REST;
		}	
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



static int			freeAmsTellerInfoListInitialled = 0;
LIST				freeAmsTellerInfoList;
TELLER_INFO_NODE	*freeAmsTellerInfoListBufPtr = NULL;

static int			freeAmsVtmInfoListInitialled = 0;
LIST				freeAmsVtmInfoList;
VTM_INFO_NODE		*freeAmsVtmInfoListBufPtr = NULL;

static int			freeAmsRegTellerInfoListInitialled = 0;
LIST				freeAmsRegTellerInfoList;
TELLER_INFO_NODE	*freeAmsRegTellerInfoListBufPtr = NULL;


int TellerInfoListInit()
{
	TELLER_INFO_NODE			*pNode;
	int							size;
	int							i;

	if(0 != freeAmsTellerInfoListInitialled)
	{
		return AMS_SUCCESS;
	}

	lstInit(&freeAmsTellerInfoList);

	size = sizeof(TELLER_INFO_NODE) * AMS_MAX_VTA_NODES;
	freeAmsTellerInfoListBufPtr = (TELLER_INFO_NODE *)malloc(size);
	pNode = (TELLER_INFO_NODE *)freeAmsTellerInfoListBufPtr;
	if(NULL == pNode)
	{
		dbgprint("TellerInfoListInit AllocMem[%d]Err\r\n", size);
		return AMS_ALLOC_MEM_FAILED;
	}

	for(i = 0; i < AMS_MAX_VTA_NODES;i++,pNode++)
	{
		lstAdd(&freeAmsTellerInfoList,(NODE *)pNode);
	}

	freeAmsTellerInfoListInitialled = 1;

	return AMS_SUCCESS;
}


int RegTellerInfoListInit()
{
	TELLER_REGISTER_INFO_NODE			*pNode;
	int							size;
	int							i;

	if(0 != freeAmsRegTellerInfoListInitialled)
	{
		return AMS_SUCCESS;
	}

	lstInit(&freeAmsRegTellerInfoList);

	size = sizeof(TELLER_REGISTER_INFO_NODE) * AMS_MAX_VTA_NODES;
	freeAmsRegTellerInfoListBufPtr = (TELLER_REGISTER_INFO_NODE *)malloc(size);
	pNode = (TELLER_REGISTER_INFO_NODE *)freeAmsRegTellerInfoListBufPtr;
	if(NULL == pNode)
	{
		dbgprint("TellerInfoListInit AllocMem[%d]Err\r\n", size);
		return AMS_ALLOC_MEM_FAILED;
	}

	for(i = 0; i < AMS_MAX_VTA_NODES;i++,pNode++)
	{
		lstAdd(&freeAmsRegTellerInfoList,(NODE *)pNode);
	}

	freeAmsRegTellerInfoListInitialled = 1;

	return AMS_SUCCESS;
}


int VtmInfoListInit()
{
	VTM_INFO_NODE			*pNode;
	int							size;
	int							i;

	if(0 != freeAmsVtmInfoListInitialled)
	{
		return AMS_SUCCESS;
	}

	lstInit(&freeAmsVtmInfoList);

	size = sizeof(VTM_INFO_NODE) * AMS_MAX_VTM_NODES;
	freeAmsVtmInfoListBufPtr = (VTM_INFO_NODE *)malloc(size);
	pNode = (VTM_INFO_NODE *)freeAmsVtmInfoListBufPtr;
	if(NULL == pNode)
	{
		dbgprint("TellerInfoListInit AllocMem[%d]Err\r\n", size);
		return AMS_ALLOC_MEM_FAILED;
	}

	for(i = 0; i < AMS_MAX_VTA_NODES;i++,pNode++)
	{
		lstAdd(&freeAmsVtmInfoList,(NODE *)pNode);
	}

	freeAmsVtmInfoListInitialled = 1;

	return AMS_SUCCESS;
}

//---------------------------------------------------------------
//功能： get an idle teller info node
//入口： 
//返回： pNode
//说明： 
//----------------------------------------------------------------
TELLER_INFO_NODE  *AmsGetTellerInfoNode()
{
	TELLER_INFO_NODE         *pNode = NULL;
	
    if(!freeAmsTellerInfoListInitialled)
    {
		dbgprint("AmsGetTellerInfoNode InitFlag[%d]Err\r\n",freeAmsTellerInfoListInitialled);
        return NULL;
    }
	
    pNode = (TELLER_INFO_NODE *)lstGet(&freeAmsTellerInfoList);
	
	return pNode;
}

//---------------------------------------------------------------
//功能： get an idle Regteller info node
//入口： 
//返回： pNode
//说明： 
//----------------------------------------------------------------
TELLER_REGISTER_INFO_NODE  *AmsGetRegTellerInfoNode()
{
	TELLER_REGISTER_INFO_NODE         *pNode = NULL;
	
    if(!freeAmsRegTellerInfoListInitialled)
    {
		dbgprint("AmsGetTellerInfoNode InitFlag[%d]Err\r\n",freeAmsRegTellerInfoListInitialled);
        return NULL;
    }
	
    pNode = (TELLER_REGISTER_INFO_NODE *)lstGet(&freeAmsRegTellerInfoList);
	
	return pNode;
}


//---------------------------------------------------------------
//功能： get an idle vtm info node
//入口： 
//返回： pNode
//说明： 
//----------------------------------------------------------------
VTM_INFO_NODE  *AmsGetVtmInfoNode()
{
	VTM_INFO_NODE         *pNode = NULL;
	
    if(!freeAmsVtmInfoListInitialled)
    {
		dbgprint("AmsGetTellerInfoNode InitFlag[%d]Err\r\n",freeAmsVtmInfoListInitialled);
        return NULL;
    }
	
    pNode = (VTM_INFO_NODE *)lstGet(&freeAmsVtmInfoList);
	
	return pNode;
}

/*******************************************************************************
* AmsCalcTellerInfoHashIdx - get teller info hash table index
*
* RETURNS: teller info hash table index
*/
static int AmsCalcTellerInfoHashIdx(unsigned char tellerId[],unsigned char len)
{
	return __BKDRHash(tellerId,len)%AMS_VTA_ID_HASH_SIZE;
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



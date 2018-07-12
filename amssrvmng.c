#include "amsfunc.h"


static int 			amsDataInitialised = 0;
static int			amsCfgDataInitialised = 0;


static int          freeAmsVtaListInitialled = 0;
pthread_mutex_t     freeAmsVtaListMtx;
LIST 				freeAmsVtaList;
VTA_NODE        	*freeAmsVtaListBufPtr = NULL;

static int          freeAmsTermListInitialled = 0;
pthread_mutex_t     freeAmsTermListMtx;
LIST 				freeAmsTermList;
TERM_NODE            *freeAmsTermListBufPtr = NULL;

int AmsDataInit()
{
	int i = 0,ret = 0;
	wchar_t param[1024];

	if(0 != amsDataInitialised)
	{
		return AMS_SUCCESS;
	}

	// get Lic data
	//AmsGetLicData();
	
    // init free vta list 
	if(AMS_SUCCESS != VtaListInit())
	{
		Display("AmsDataInit:Error-vta nodes error!\r\n");
    	return AMS_ERROR;
	}	    

    // init free vtm list 
	if(AMS_SUCCESS != TermListInit())
	{
		Display("AmsDataInit:Error-term nodes error!\r\n");
    	return AMS_ERROR;
	}

    // init free vta id list 
	/*if(AMS_SUCCESS != VtaIdListInit())
	{
		Display("AmsDataInit:Error-vta id nodes error!\r\n");
    	return AMS_ERROR;
	}*/	    

    // init free vtm id list 
	/*if(AMS_SUCCESS != VtmIdListInit())
	{
		Display("AmsDataInit:Error-vtm id nodes error!\r\n");
    	return AMS_ERROR;
	}*/

    // init free org id list 
	/*if(AMS_SUCCESS != OrgIdListInit())
	{
		Display("AmsDataInit:Error-org id nodes error!\r\n");
    	return AMS_ERROR;
	}
		
#ifdef AMS_TEST_NEED_DB
    //zhuyn added 20161101 数据库数据加载需放在ListInit之后
    if (0 != AmsLoadDbData())
    {
		Display("AmsDataInit:AmsLoadDbData Fail!\r\n");
    	return AMS_ERROR;
	}
#else
	//load org hash
	if(AMS_SUCCESS != OrgIdHashInit())
	{
		Display("AmsDataInit:OrgIdLoad error!\r\n");
    	return AMS_ERROR;
	}	
#endif*/

    for (i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
    {	
	    //init sem
	    Sem_init(&AmsSrvData(i).vtaCtrl,0,1);

		Sem_init(&AmsSrvData(i).termCtrl,0,1);

		Sem_init(&AmsSrvData(i).freevtaCtrl,0,1);

//		AmsSrvData(i).serviceState = AMS_SERVICE_INACTIVE;

//		AmsSrvData(i).preSrvTellerId = 0;
		
    }

	/* clear regData */
	memset(&SystemData.AmsPriData.amsRegData, 0, sizeof(AMS_DATA_REGISTER));

	/* clear stat */	
	/*memset(&SystemData.AmsPriData.amsStat, 0, sizeof(AMS_STAT));
	for(i = 0; i < Min(SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NUM); i++)
	{
		if(0 != AmsCfgTeller(i).tellerId)
		{
			AmsTellerStat(i).tellerId = AmsCfgTeller(i).tellerId; //设置tellerId
			Display("AmsTeller[%s][%u]Stat Init[%d]\r\n", 
				AmsCfgTeller(i).tellerNo, AmsCfgTeller(i).tellerId, i);
		}
	}
	for(i = 0; i < Min(SystemData.AmsPriData.amsCfgData.maxVtmNum, AMS_MAX_VTM_NUM); i++)
	{
		if(0 != AmsCfgVtm(i).vtmId)
		{
			AmsVtmStat(i).vtmId = AmsCfgVtm(i).vtmId;   //设置vtmId
			Display("AmsVtm[%s][%u]Stat Init[%d]\r\n", 
				AmsCfgVtm(i).vtmNo, AmsCfgVtm(i).vtmId, i);
		}
	}*/

	//init AmsRcasMngData

	amsDataInitialised = 1;
	
	Display("AmsDataInit:sys config finished.\r\n");
	
	return AMS_SUCCESS;
}



int VtaListInit()
{
	VTA_NODE            *pNode;
	int                 size;	
	int                 i;
	
	if(0 != freeAmsVtaListInitialled)
	{
		return AMS_SUCCESS;
	}

	lstInit(&freeAmsVtaList);
	
	size = sizeof(VTA_NODE) * AMS_MAX_VTA_NODES;
	freeAmsVtaListBufPtr = (VTA_NODE *)malloc(size);
	pNode = (VTA_NODE *)freeAmsVtaListBufPtr;
	if(NULL == pNode)
	{
		dbgprint("VtaListInit AllocMem[%d]Err\r\n", size);
		return AMS_ALLOC_MEM_FAILED;
	}
	
	for(i = 0;i < AMS_MAX_VTA_NODES;i++,pNode++)
	{
        lstAdd(&freeAmsVtaList,(NODE *)pNode);
	}
	
    Pthread_mutex_init(&freeAmsVtaListMtx,NULL);
	
	freeAmsVtaListInitialled = 1;
	
	return AMS_SUCCESS;
}

VTA_NODE * VtaNodeGet(void)
{
	VTA_NODE            *pNode;
	
    if(!freeAmsVtaListInitialled)
    {
		dbgprint("VtaNodeGet InitFlag[%d] Err\r\n",freeAmsVtaListInitialled);
        return NULL;
    }
	
    Pthread_mutex_lock(&freeAmsVtaListMtx);
    pNode = (VTA_NODE *)lstGet(&freeAmsVtaList);
    Pthread_mutex_unlock(&freeAmsVtaListMtx);
	
    return pNode;
}

void VtaNodeFree(VTA_NODE *pNode)
{
    if(!freeAmsVtaListInitialled)
    {
        return;
    }
	
    Pthread_mutex_lock(&freeAmsVtaListMtx);
    lstAdd(&freeAmsVtaList,(NODE *)pNode);
    Pthread_mutex_unlock(&freeAmsVtaListMtx);
}


int TermListInit()
{
	TERM_NODE            *pNode;
	int                 size;	
	int                 i;
	
	if(0 != freeAmsTermListInitialled)
	{
		return AMS_SUCCESS;
	}

	lstInit(&freeAmsTermList);
	
	size = sizeof(TERM_NODE) * AMS_MAX_TERM_NODES;
	freeAmsTermListBufPtr = (TERM_NODE *)malloc(size);
	pNode = (TERM_NODE *)freeAmsTermListBufPtr;
	if(NULL == pNode)
	{
		dbgprint("TermListInit AllocMem[%d]Err\r\n",size);		
		return AMS_ALLOC_MEM_FAILED;
	}
	
	for(i = 0;i < AMS_MAX_TERM_NODES;i++,pNode++)
	{
        lstAdd(&freeAmsTermList,(NODE *)pNode);
	}
	
    Pthread_mutex_init(&freeAmsTermListMtx,NULL);
	
	freeAmsTermListInitialled = 1;
	
	return AMS_SUCCESS;
}

TERM_NODE * TermNodeGet(void)
{
	TERM_NODE            *pNode;
	
    if(!freeAmsTermListInitialled)
    {
		dbgprint("VtmNodeGet InitFlag[%d]Err\r\n",freeAmsTermListInitialled);		
        return NULL;
    }
	
    Pthread_mutex_lock(&freeAmsTermListMtx);
    pNode = (TERM_NODE *)lstGet(&freeAmsTermList);
    Pthread_mutex_unlock(&freeAmsTermListMtx);
	
    return pNode;
}

void TermNodeFree(TERM_NODE *pNode)
{
    if(!freeAmsTermListInitialled)
    {
        return;
    }
	
    Pthread_mutex_lock(&freeAmsTermListMtx);
    lstAdd(&freeAmsTermList,(NODE *)pNode);
    Pthread_mutex_unlock(&freeAmsTermListMtx);
}




int AmsCfgDataInit()
{
	int i = 0,ret = 0;
	wchar_t	param[1024];

	if(0 != amsCfgDataInitialised)
	{
		return AMS_SUCCESS;
	}

	//init teller info list
	if(AMS_SUCCESS != TellerInfoListInit())
	{
		Display("TellerInfoListInit:Error-tellerinfo nodes error!\r\n");
		return AMS_ERROR;
	}

	//init vtm info list
	if(AMS_SUCCESS != VtmInfoListInit())
	{
		Display("VtmInfoListInit:Error-vtminfo nodes error!\r\n");
		return AMS_ERROR;
	}

	//init reg teller info list	
	if(AMS_SUCCESS != RegTellerInfoListInit())
	{
		Display("RegTellerInfoListInit:Error-regtellerinfo nodes error!\r\n");
		return AMS_ERROR;
	}
	
	amsCfgDataInitialised = 1;
	
	Display("AmsCfgDataInit Ok!\r\n");
	
	return AMS_SUCCESS;
}

int AmsUpdateSingleVtaWorkInfo(VTA_NODE *pVtaNode, time_t currentTime)
{
	if(NULL == pVtaNode)
	{
		return AMS_ERROR;
	}
	
	if(currentTime > pVtaNode->stateStartTime && currentTime > pVtaNode->workInfoUpdateTime)
	{
		if(AMS_VTA_STATE_BUSY == pVtaNode->state || AMS_VTA_STATE_PREPARE == pVtaNode->state)
		{
			if(pVtaNode->workInfoUpdateTime > pVtaNode->stateStartTime)
			{
				pVtaNode->vtaWorkInfo.workSeconds += (currentTime - pVtaNode->workInfoUpdateTime);
			}
			else
			{
				pVtaNode->vtaWorkInfo.workSeconds += (currentTime - pVtaNode->stateStartTime);
			}
			
			pVtaNode->workInfoUpdateTime = currentTime;

			return AMS_OK;
		}
	
		if(AMS_VTA_STATE_IDLE == pVtaNode->state || AMS_VTA_STATE_REST == pVtaNode->state)
		{
			if(pVtaNode->workInfoUpdateTime > pVtaNode->stateStartTime)
			{
				pVtaNode->vtaWorkInfo.idleSeconds += (currentTime - pVtaNode->workInfoUpdateTime);
			}
			else
			{
				pVtaNode->vtaWorkInfo.idleSeconds += (currentTime - pVtaNode->stateStartTime);
			}
			
			pVtaNode->workInfoUpdateTime = currentTime;

			return AMS_OK;
		}
	}

	return AMS_ERROR;
}

int AmsCustCalcSrvGrpIdQueueInfo(unsigned char termId[], unsigned int srvGrpId, QUEUE_INFO *pQueueInfo)
{
	TERM_NODE            *pTempTermNode = NULL;
	int                 custNum = 0;
	unsigned int        tellerNum = 0;		
	int                 i = 0;

	if(srvGrpId > AMS_MAX_SERVICE_GROUP_NUM)
	{
		dbgprint("Ams Cust[%u]CalcSrvGrpIdQueueInfo SrvGrpId[%u]Err", termId, srvGrpId);		
		return AMS_ERROR;
	}
	
	custNum = lstCount(&AmsSrvData(srvGrpId).termList);
	tellerNum = lstCount(&AmsSrvData(srvGrpId).freevtaList);
	pTempTermNode = (TERM_NODE *)lstFirst(&AmsSrvData(srvGrpId).termList);
	
	while(NULL != pTempTermNode && i < custNum)//不必 <=
	{
		//count cust ahead
		if(0 == memcmp(pTempTermNode->termInfo.termId,termId,pTempTermNode->termInfo.termIdLen))
		{
			break;
		}
		if(AMS_CUSTOMER_IN_QUEUE == pTempTermNode->serviceState)
		{
			pQueueInfo->queuingLen++;   
		}

		pTempTermNode = (TERM_NODE *)lstNext((NODE *)pTempTermNode);
		
		i++;	
	}	

	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		if(    AMS_QUEUE_CFG == AmsCfgQueueSys(i).flag 
			&& AmsCfgQueueSys(i).srvGrpIdPos == srvGrpId
			&& AmsCfgQueueSys(i).avgSrvTime <= AMS_MAX_AVG_SERVICE_TIME)
		{
			if(tellerNum > 0)
			{
				pQueueInfo->queuingTime = AmsCfgQueueSys(i).avgSrvTime * (pQueueInfo->queuingLen/tellerNum + 0.5);//0.5 假定当前业务平均完成一半
			}
			else
			{
				pQueueInfo->queuingTime = AmsCfgQueueSys(i).avgSrvTime * pQueueInfo->queuingLen; //等待柜员来服务，一般不会发生		
				
			}
			
			break;
		}
	}
	if(i >= AMS_MAX_SERVICE_GROUP_NUM)
	{
		if(tellerNum > 0)
		{
			pQueueInfo->queuingTime = AMS_AVG_SERVICE_TIME * (pQueueInfo->queuingLen/tellerNum + 0.5);	//0.5 假定当前业务平均完成一半			
		}
		else
		{
			pQueueInfo->queuingTime = AMS_AVG_SERVICE_TIME * pQueueInfo->queuingLen; //等待柜员来服务，一般不会发生			
			
		}			
	}	
	
//	dbgprint("Ams CustCalcSrvGrpIdQueueInfo vtmId[%u][%u]pQueueInfo[%d][%d]\r\n", 
//		vtmId, srvGrpId, pQueueInfo->queuingLen, pQueueInfo->queuingTime);	
	
	return AMS_OK;
}


int AmsSeleteGrpId(unsigned int srvGrpId,unsigned char termId[],unsigned char serviceType[],unsigned int *srvGrpSelect)
{
	QUEUE_INFO          queueInfo;
	VTA_NODE            *pVtaNode = NULL;	
	unsigned int        idleVtaNum = 0;
	unsigned int        i = 0;
	unsigned int        j = 0;
	unsigned int        k = 0;	
	unsigned int        find=0;


	//检查业务类型
	for(i=0;i<AmsCfgSrvGroup(srvGrpId).srvlogpos;i++)
	{
		if(0 == memcmp(serviceType,AmsCfgSrvGroup(srvGrpId).srvInfo[i].serviceName,AmsCfgSrvGroup(srvGrpId).srvInfo[i].serviceNameLen))
		{
			find=1;
			break;
		}
	}
	if(find == 0)
	{
		return AMS_ERROR;	
	}

	//检查当前业务组空闲柜员数量
	idleVtaNum = lstCount(&AmsSrvData(srvGrpId).freevtaList);

	//当前业务组有空闲柜员
	if(0 != idleVtaNum)
	{
		//初步选中业务组
		*srvGrpSelect = srvGrpId;

		if(AmsDebugTrace)	
		{
			dbgprint("AmsSelectSrvGrp Term[%s] would be SrvGrp[%u]", 
				termId, srvGrpId);
		}	
		return AMS_OK;
	}
	else
	{
		//检查当前业务组的客户排队长度
		memset(&queueInfo,0,sizeof(QUEUE_INFO));
		if(AMS_OK != AmsCustCalcSrvGrpIdQueueInfo(termId, srvGrpId, &queueInfo))
		{
			if(AmsDebugTrace)	
			{
				dbgprint("AmsSelectSrvGrp Term[%s] CalcSrvGrp[%u]QueueInfoErr", 
					termId, srvGrpId);
			}
			
			return AMS_ERROR;			
		}	
		for(k = 0; k < AMS_MAX_SERVICE_GROUP_NUM; k++)
		{
			if(    AMS_QUEUE_CFG == AmsCfgQueueSys(k).flag 
			&& 0 == memcmp(AmsCfgQueueSys(k).srvGrpId,AmsCfgSrvGroup(srvGrpId).srvGrpId,AmsCfgSrvGroup(srvGrpId).srvGrpIdLen))//重复~
			{
				//已经达到当前业务组允许的最大排队长度
				if(queueInfo.queuingLen >= AmsCfgQueueSys(k).maxQueLen)
				{
					return AMS_OK;		//待定�???是否需要继续查找别的智能业务组					
				}
				else
				{
					if(0 == srvGrpSelect)
					{
						//初步选中业务组
						*srvGrpSelect = srvGrpId;

						if(AmsDebugTrace)	
						{
							dbgprint("AmsSelectSrvGrp Term[%s] would be SrvGrp[%u].", 
								termId, srvGrpId);
						}	
					}
					return AMS_OK;
				}
			}
		}
		return AMS_ERROR;
	}

}

/*未指定业务组
1 先在该业务类型的智能业务组中进行查找
2 1无法找到，再继续在人工业务组中进行查找
3 2也无法找到，返回查找失败
*/
int AmsSelectSrvGrpIdByServiceType(unsigned char termId[],						                          
											 unsigned char serviceType[],
											 unsigned int *pSrvGrpId)
{
	VTA_NODE            *pVtaNode = NULL;	
	unsigned int        vtaNum = 0;
	unsigned int        idleVtaNum = 0;
	QUEUE_INFO          queueInfo;
	unsigned int        srvGrpId = 0;
	unsigned int        idleRate = 0;
	unsigned int        idleRateMax = 0;
	unsigned int        srvGrpSelect = 0;
	unsigned int        i = 0;
	unsigned int        j = 0;
	unsigned int        k = 0;	
	int					iret=0;

	//选择智能业务组
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		//找到配置此业务类型的业务组
		if(AmsCfgSrvGroup(i).flag != AMS_SERVICE_GROUP_INSTALL)
		{
			continue;
		}
		else
		{
			if(AmsCfgSrvGroup(i).isAutoFlag != AMS_SRVGRP_TYPE_AUTO)
			{
				continue;
			}

			iret = AmsSeleteGrpId(i,termId,serviceType,&srvGrpSelect);
			
			if(iret == AMS_ERROR)
			{
				continue;
			}
			else 
			{
				break;
			}
		}
	}

	if(0 == srvGrpSelect)
	{
		//选择智能业务组
		for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
		{
			//找到配置此业务类型的业务组
			if(AmsCfgSrvGroup(i).flag != AMS_SERVICE_GROUP_INSTALL)
			{
				continue;
			}
			else
			{
				if(AmsCfgSrvGroup(i).isAutoFlag != AMS_SRVGRP_TYPE_HUMAN)
				{
					continue;
				}
	
				iret = AmsSeleteGrpId(i,termId,serviceType,&srvGrpSelect);
				
				if(iret == AMS_ERROR)
				{
					continue;
				}
				else 
				{
					break;
				}
			}
		}
	}

	if(0 == srvGrpSelect)
	{
		if(AmsDebugTrace)	
		{
			dbgprint("AmsTerm[%s] serviceType[0x%x] SelectSrvGrp Fail", 
				termId, serviceType);
		}
		
		return AMS_CMS_GET_VTA_NO_VALID_SERVICE_GROUP_ID_OR_TYPE;
	}

	//选中业务组
	*pSrvGrpId = srvGrpSelect;

	if(AmsDebugTrace)	
	{
		dbgprint("AmsSelectSrvGrp Term[%s] is SrvGrp[%u]", 
			termId, *pSrvGrpId);
	}
	
	return AMS_OK;

}


int AmsStartCustomerQueueProcess(MESSAGE_t *pMsg,
								TERM_NODE *pTermNode,
								unsigned int srvGrpId,
								unsigned char servicetypelen,
								unsigned char serviceType[],                                     
								unsigned char callIdLen,
								unsigned char srvGrpSelfAdapt)
{
	int					iret = AMS_CMS_PRCOESS_SUCCESS;
	LP_QUEUE_DATA_t     *lpQueueData = NULL;      //排队进程数据区指针 
	PID_t				Pid;
	unsigned char       timerPara[AMS_MAX_TIMER_PARA_LEN];
	MESSAGE_t           msg;

	if(NULL == pMsg || NULL == pTermNode)
	{
		dbgprint("AmsStartCustomerQueueProcess Para[%d][%d]", pMsg, pTermNode);
		iret = AMS_CMS_GET_VTA_PARA_ERR;
		return iret;
	}
	
    //分配排队服务进程号
	memset(&Pid,0,sizeof(PID_t));
	iret = AmsAllocPid(&Pid);
	if(-1 == iret)
	{
		dbgprint("AmsStartCustomerQueueProcess Term[%s] AllocPid: SysBusy",
			pTermNode->termInfo.termId);
		
		iret = AMS_CMS_GET_VTA_LP_RESOURCE_LIMITED;
		return iret;
	}

	//record Customer In Queue Pid
	pTermNode->customerPid = Pid.iProcessId;
	
	lpQueueData=(LP_QUEUE_DATA_t *)ProcessData[pTermNode->customerPid];

	//排队进程数据区初始化
    memset(lpQueueData, 0, sizeof(LP_AMS_DATA_t));
	
	//record vtmId
	lpQueueData->termIdLen = pTermNode->termInfo.termIdLen;
	memcpy(lpQueueData->termId,pTermNode->termInfo.termId,pTermNode->termInfo.termIdLen);
	
	//record srvGrpId
	lpQueueData->srvGrpId = srvGrpId;
	
	//record serviceType
	lpQueueData->serviceTypeLen = servicetypelen;
	memcpy(lpQueueData->serviceType,serviceType,servicetypelen);

	//record callId
    lpQueueData->callIdLen = callIdLen;
	memcpy(lpQueueData->callId, &pMsg->cMessageBody[1], callIdLen);

	//record srvGrpSelfAdapt
	lpQueueData->srvGrpSelfAdapt = srvGrpSelfAdapt;
	
	//record callType
//	lpQueueData->callType = callType;
	
	//record callTarget  ????
/*	lpQueueData->callTarget.callTargetType      = callTarget.callTargetType;
	lpQueueData->callTarget.targetTellerGroupId = callTarget.targetTellerGroupId;
	lpQueueData->callTarget.targetTellerId      = callTarget.targetTellerId;*/
	
	//init timer
	lpQueueData->iTimerId             = -1;
		
	//record myPid
	lpQueueData->myPid.cModuleId   = SystemData.cMid;
	lpQueueData->myPid.cFunctionId = FID_AMS;
	lpQueueData->myPid.iProcessId  = Pid.iProcessId;
	
	//record cmsPid
	lpQueueData->cmsPid.cModuleId	 = pMsg->s_SenderPid.cModuleId;
	lpQueueData->cmsPid.cFunctionId  = pMsg->s_SenderPid.cFunctionId;
	lpQueueData->cmsPid.iProcessId   = pMsg->s_SenderPid.iProcessId;

	//record vtmPid, may be used later
    memcpy(&lpQueueData->vtmPid,&pTermNode->rPid,sizeof(PID_t));

	lpQueueData->queueId = ((((unsigned int)lpQueueData->myPid.cModuleId) << 24)&0xff000000) | 
		                 ((((unsigned int)lpQueueData->myPid.cFunctionId) << 16)&0x00ff0000) |
		                 (((unsigned int)lpQueueData->myPid.iProcessId)&0x0000ffff);
	
	//消息跟踪开关配置
	lpQueueData->debugTrace  = (unsigned char)AmsDebugTrace;
	lpQueueData->commonTrace = (unsigned char)AmsCommonTrace;
	lpQueueData->msgTrace    = (unsigned char)AmsMsgTrace;
	lpQueueData->stateTrace  = (unsigned char)AmsStateTrace;
	lpQueueData->timerTrace  = (unsigned char)AmsTimerTrace;
	lpQueueData->errorTrace  = (unsigned char)AmsErrorTrace;
	lpQueueData->alarmTrace  = (unsigned char)AmsAlarmTrace;	
	
	memcpy(lpQueueData->sTraceName,"ams",3);
	lpQueueData->lTraceNameLen = 3;
	
    //创建排队定时器
	//Customer Wait In Queue,default: 600s
    if(    AmsCfgData.amsCustomerInQueueTimeLength > 0 
	    && AmsCfgData.amsCustomerInQueueTimeLength <= T_AMS_CUSTOMER_IN_QUEUE_TIMER_LENGTH_MAX)
    {
		memset(timerPara, 0, PARA_LEN);
		BEPUTLONG(lpQueueData->queueId,timerPara);
        if(AMS_OK != AmsQueueCreateTimerPara(lpQueueData->myPid.iProcessId,
											 &lpQueueData->iTimerId, 
											 B_AMS_CUSTOMER_IN_QUEUE_TIMEOUT, 
											 AmsCfgData.amsCustomerInQueueTimeLength,
											 timerPara))
        {
			dbgprint("StartCustomerQueueProcess[%d] Term[%s] CreateTimer Err",
				lpQueueData->myPid.iProcessId, lpQueueData->termId);
			iret = AMS_CMS_GET_VTA_CREATE_TIMER_ERR;
			AmsReleassPid(Pid, FAILURE);
			return iret;
        }

		AmsTimerStatProc(T_AMS_CUSTOMER_IN_QUEUE_TIMER, AMS_CREATE_TIMER);

        if(lpQueueData->commonTrace)
        {
            dbgprint("Ams[%d] Create T_AMS_CUSTOMER_IN_QUEUE_TIMER Timer:timerId=%d.",
				lpQueueData->myPid.iProcessId, lpQueueData->iTimerId);
        }	
    }

	return iret;
}



VTA_NODE *AmsSelectVta(	unsigned char termId[],
						   unsigned int srvGrpId, 
						   unsigned char serviceType[],
						   unsigned int vtaNum,
						   int *pResult)
{
	VTA_NODE            *pVtaNode = NULL;	
	VTA_NODE            *pVtaIdleLongestNode = NULL;	
	unsigned int        idleLongestTime = 0;
	time_t              currentTime;	
	unsigned int        i = 0;

	
	//检查业务组编号
	if(srvGrpId > AMS_MAX_SERVICE_GROUP_NUM)
	{
		dbgprint("AmsSelectVta Term[%s] SrvGrpId[%d]Err", termId, srvGrpId);
		*pResult = AMS_CMS_GET_VTA_SIR_SERVICE_GROUP_ID_ERR;
		return NULL;		
	}

	//获取坐席链表头结点
	pVtaNode = (VTA_NODE *)lstFirst(&AmsSrvData(srvGrpId).vtaList);

	//没有坐席登陆
	if(NULL == pVtaNode)
	{
		dbgprint("AmsSelectVta Term[%s] SrvGrpId[%u] VtaListEmpty.", 
			termId, srvGrpId);			
		*pResult = AMS_CMS_GET_VTA_SIR_VTA_EMPTY;
		return NULL;	
	}

	//获取空闲坐席链表头节点
	pVtaNode == (VTA_NODE *)lstFirst(&AmsSrvData(srvGrpId).freevtaList);
	if(NULL == pVtaNode)
	{
		*pResult = AMS_CMS_GET_VTA_SERVICE_IN_QUEUE;
		return NULL;	
	}
	else
	{
		return pVtaNode;
	}
	
}


VTA_NODE *AmsServiceIntelligentSelectVta(
									   unsigned char termId[],
									   unsigned int srvGrpId, 
									   unsigned char serviceType[],
									   int *pResult)
{
	
	unsigned int		vtaNum;
	VTA_NODE			*pVtaNode = NULL;	
	QUEUE_INFO			queueInfo;
	unsigned int		i = 0;
	
	
	//检查业务组编号
	if(srvGrpId > AMS_MAX_SERVICE_GROUP_NUM)
	{
		dbgprint("AmsSISelectVta Term[%s] SrvGrpId[%d]Err", 
			termId, srvGrpId);
		*pResult = AMS_CMS_GET_VTA_SIR_SERVICE_GROUP_ID_ERR;
		return NULL;		
	}

	//检查空闲坐席队列
	vtaNum = lstCount(&AmsSrvData(srvGrpId).freevtaList);
	if(0 == vtaNum)
	{
		dbgprint("AmsSISelectVta Term[%s] SrvGrpId[%u] VtaListEmpty", 
				termId, srvGrpId);
		*pResult = AMS_CMS_GET_VTA_SIR_VTA_EMPTY;
		return NULL;	
	}

	//检查客户排队长度
	if(lstCount(&AmsSrvData(srvGrpId).termList) > 1)

	{
		memset(&queueInfo,0,sizeof(QUEUE_INFO));
		if(AMS_OK != AmsCustCalcSrvGrpIdQueueInfo(termId, srvGrpId, &queueInfo))
		{
			dbgprint("AmsSISelectVta Term[%s] CalcSrvGrp[%u]QueueInfoErr", 
				termId, srvGrpId);
			*pResult = AMS_CMS_GET_VTA_SIR_CALC_QUEUE_INFO_ERR;
			return NULL;				
		}

		for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
		{
			if(    AMS_QUEUE_CFG == AmsCfgQueueSys(i).flag 
				&& 0 == memcmp(AmsCfgQueueSys(i).srvGrpId,AmsCfgSrvGroup(srvGrpId).srvGrpId,AmsCfgSrvGroup(srvGrpId).srvGrpIdLen))//重复~
			{
				//已经达到允许的最大排队长度
				if(queueInfo.queuingLen >= AmsCfgQueueSys(i).maxQueLen)
				{
					dbgprint("AmsSISelectVta Term[%s] SrvGrpId[%u] tooManyCustomers[%u][%u][%d]!", 
						termId, srvGrpId, 
						queueInfo.queuingLen, AmsCfgQueueSys(i).maxQueLen, i);						
					*pResult = AMS_CMS_GET_VTA_SIR_TOO_MANY_CUSTOMER_IN_QUEUE;
					return NULL;							
				}
			}
		}
	}	

	//常规选择
	pVtaNode = AmsSelectVta(termId,srvGrpId,serviceType,vtaNum,pResult);
	//路由失败，
	if(*pResult != AMS_CMS_PRCOESS_SUCCESS)
	{
			
		//路由失败原因为暂无合适坐席，进入排队
		if(AMS_CMS_GET_VTA_SERVICE_IN_QUEUE == *pResult)
		{
			return NULL;
		}

		//路由失败原因为出现错误
		if(AmsDebugTrace)
		{
			dbgprint("AmsSISelectVta Term[%s] Result[0x%x]Err", termId, *pResult);
		}
		
		return NULL;
	}
	
	//路由成功
	if(NULL != pVtaNode)
	{
		if(AmsDebugTrace)
		{
			dbgprint("AmsSISelectVta Term[%s] GetVta[%s].", 
				termId,  
				pVtaNode->vtaInfo.tellerId);
		}
					
		return pVtaNode;
	}

	//其他类型路由失败，默认进入排队
	//not get, please wait ...
	*pResult = AMS_CMS_GET_VTA_SERVICE_IN_QUEUE;
	
    return NULL;
	
}


int AmsSendServiceProcMsg()
{
	MESSAGE_t           s_Msg;

	memset(&s_Msg,0,sizeof(MESSAGE_t));
	
	s_Msg.eMessageAreaId = C;
	
	s_Msg.s_ReceiverPid.cModuleId   = SystemData.cMid;  
	s_Msg.s_ReceiverPid.cFunctionId = FID_AMS;
	s_Msg.s_ReceiverPid.iProcessId  = 0; 
	
	s_Msg.s_SenderPid.cModuleId     = SystemData.cMid;
	s_Msg.s_SenderPid.cFunctionId   = FID_AMS;
	s_Msg.s_SenderPid.iProcessId    = 0;
	s_Msg.iMessageType = C_AMS_SERVICE_PROC_REQ;
	s_Msg.iMessageLength = 0;
	
	SendMsgBuff(&s_Msg,0);

	if(AmsMsgTrace)
	{	
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"send C_AMS_SERVICE_PROC_REQ msg \n");	
		AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"C_AMS_SERVICE_PROC_REQ",description,
			           descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,"ams");				
	}


	//not need result stat

	return SUCCESS;
}


TERM_NODE * AmsGetIdleLongestTerm(unsigned int srvGrpId, int termNum, time_t currentTime)
{
	TERM_NODE            *pTermNode = NULL;	
	TERM_NODE            *pTermIdleLongestNode = NULL;
	unsigned int        idleLongestTime = 0;	
	unsigned int        j = 0;
	
	if(termNum > AMS_MAX_TERM_NODES)
	{
		return NULL;
	}

	if(srvGrpId >= AMS_MAX_SERVICE_GROUP_NUM)
	{
		return NULL;
	}
	
	//找到呼叫等待时间最长的客户(数量不多，顺序比较)
	pTermNode = (TERM_NODE *)lstFirst(&AmsSrvData(srvGrpId).termList);
	while(NULL != pTermNode && j < termNum)
	{
		if(    AMS_TERM_STATE_BUSY == pTermNode->state 
			&& AMS_CUSTOMER_IN_QUEUE == pTermNode->serviceState 
			&& currentTime > pTermNode->enterQueueTime)
		{
			if(idleLongestTime < (currentTime - pTermNode->enterQueueTime))
			{
				idleLongestTime = (currentTime - pTermNode->enterQueueTime);
				pTermIdleLongestNode = pTermNode;
			}
		}
		
		pTermNode = (TERM_NODE *)lstNext((NODE *)pTermNode);	
		j ++;
	}

	return pTermIdleLongestNode;	
	
}



void AmsGetVtaInServiceProcTask(int iThreadId, time_t currentTime)
{
	int					iret = AMS_CMS_PRCOESS_SUCCESS;		
	LP_QUEUE_DATA_t     *lpQueueData = NULL;      //排队进程数据区指针	
	LP_AMS_DATA_t		*lpAmsData = NULL;        //进程数据区指针			
	TERM_NODE            *pTermNode = NULL;	
	VTA_NODE            *pVtaNode = NULL;
	DWORD               srvGrpId = 0;
	MESSAGE_t           msg;	
	int                 termNum = 0;
	int                 procNum = 0;
	int                 pid = 0;	
	int                 i = 0;	
	int                 j = 0;	
	
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		termNum = lstCount(&AmsSrvData(i).termList);
		termNum = Min(termNum, AMS_MAX_TERM_NODES);

		if(0 == termNum)
		{
			continue;
		}
		
		for(j = 0; j < termNum; j++)	
		{
			pTermNode = AmsGetIdleLongestTerm(i, termNum, currentTime);

			if(NULL == pTermNode)
			{
				break;
			}
			
			iret = AMS_CMS_PRCOESS_SUCCESS;
			
			if(   AMS_CUSTOMER_IN_QUEUE != pTermNode->serviceState 
			   || 0 == pTermNode->customerPid 
			   || pTermNode->customerPid >= LOGIC_PROCESS_SIZE)
			{
				//状态及参数校验错误
				continue;
			}

			lpQueueData=(LP_QUEUE_DATA_t *)ProcessData[pTermNode->customerPid];

//			dbgprint("AmsServiceIntelligentSelectVta Vtm[%s][%u]SrvGrpId[%u]\r\n",
//				lpQueueData->vtmNo, lpQueueData->vtmId, lpQueueData->srvGrpId);	

			//仅携带业务类型，没有指明业务组
			if (1 == lpQueueData->srvGrpSelfAdapt)
			{
				//根据业务类型选择业务组
				iret = AmsSelectSrvGrpIdByServiceType(
				                                      lpQueueData->termId,
				                                      lpQueueData->serviceType,
				                                      &srvGrpId);
				
				if(AMS_OK == iret && srvGrpId != lpQueueData->srvGrpId)
				{
					//转移业务组
					lpQueueData->srvGrpId = srvGrpId;
					
					//呼叫添加到新业务组的链表尾
				    /* Del Vtm Node from Origin List */
				    Sem_wait(&AmsSrvData(pTermNode->termInfo.srvGrpIdpos).termCtrl);
					lstDelete(&AmsSrvData(pTermNode->termInfo.srvGrpIdpos).termList, (NODE *)pTermNode);
					Sem_post(&AmsSrvData(pTermNode->termInfo.srvGrpIdpos).termCtrl);

					/* Add Vtm Node to new List */
				    Sem_wait(&AmsSrvData(srvGrpId).termCtrl);
					lstAdd(&AmsSrvData(srvGrpId).termList, (NODE *)pTermNode);
					Sem_post(&AmsSrvData(srvGrpId).termCtrl);

					//更新当前柜员机的业务组编号
					if(pTermNode->termInfo.srvGrpIdpos!= srvGrpId)
					{
						//update srvGrpId
						pTermNode->termInfo.srvGrpIdpos = srvGrpId;
					}		
				}	
			}

			//业务智能路由
			pVtaNode = AmsServiceIntelligentSelectVta(
													  lpQueueData->termId,
			                                          lpQueueData->srvGrpId,
			                                          lpQueueData->serviceType,
			                                          &iret);
			if(NULL == pVtaNode)
			{
				if(AMS_CMS_GET_VTA_SERVICE_IN_QUEUE != iret)
				{
					dbgprint("ServiceProcTask Vtm[%s] GetVta SISelectVta Failed",
						lpQueueData->termId);
					
					if(AMS_ERROR == iret)
					{
						iret = AMS_CMS_GET_VTA_SERVICE_INTELLIGENT_ROUTING_ERR;
					}

					memset(&msg, 0, sizeof(MESSAGE_t));
					
					//pack cmsPid
					msg.s_SenderPid.cModuleId   = pTermNode->cmsPid.cModuleId;
					msg.s_SenderPid.cFunctionId = pTermNode->cmsPid.cFunctionId;
					msg.s_SenderPid.iProcessId  = pTermNode->cmsPid.iProcessId;

					//pack amsPid
					msg.s_ReceiverPid.iProcessId = 0;
					
					//pack callId
					msg.cMessageBody[0] = lpQueueData->callIdLen;
					if(lpQueueData->callIdLen <= AMS_MAX_CALLID_LEN)
					{
						memcpy(&msg.cMessageBody[1], lpQueueData->callId, lpQueueData->callIdLen);	
					}
					AmsSendCmsVtaGetRsp(NULL,&msg,iret,NULL,pTermNode);	
					
					//update Customer Service State
					AmsSetTermServiceState(pTermNode, AMS_CUSTOMER_SERVICE_NULL);
			
					/* 杀掉定时器 */
					if(lpQueueData->iTimerId >= 0)
					{
						AmsQueueKillTimer(pTermNode->customerPid, &lpQueueData->iTimerId);//或 lpQueueData->myPid.iProcessId
						AmsTimerStatProc(T_AMS_CUSTOMER_IN_QUEUE_TIMER, AMS_KILL_TIMER);
					} 
		
					//release lpQueueData Pid
	        		AmsReleassPid(lpQueueData->myPid, END);
					
					procNum ++;

					if(procNum >= AMS_MAX_PROC_NUM_IN_QUEUE)
					{
						return;
					}						

					continue;
				}
				else
				{					
					break;
				}
			}
			else
			{
				memset(&msg, 0, sizeof(MESSAGE_t));

				//pack cmsPid
				msg.s_SenderPid.cModuleId   = pTermNode->cmsPid.cModuleId;
				msg.s_SenderPid.cFunctionId = pTermNode->cmsPid.cFunctionId;
				msg.s_SenderPid.iProcessId  = pTermNode->cmsPid.iProcessId;

				//pack amsPid
				msg.s_ReceiverPid.iProcessId = 0;

				//pack callId
				msg.cMessageBody[0] = lpQueueData->callIdLen;
				if(lpQueueData->callIdLen <= AMS_MAX_CALLID_LEN)
				{
					memcpy(&msg.cMessageBody[1], lpQueueData->callId, lpQueueData->callIdLen);	
				}

				//检查进程号
				pid = pVtaNode->amsPid & 0xffff;
				if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
				{
					dbgprint("ServiceProcTask Vtm[%s] GetVta Pid:%d Err", 
						lpQueueData->termId, pid);
					
					iret = AMS_CMS_GET_VTA_AMS_PID_ERR;
					AmsSendCmsVtaGetRsp(NULL,&msg,iret,pVtaNode,NULL);

					//update Customer Service State
					AmsSetTermServiceState(pTermNode, AMS_CUSTOMER_SERVICE_NULL);
			
					/* 杀掉定时器 */
					if(lpQueueData->iTimerId >= 0)
					{
						AmsQueueKillTimer(pTermNode->customerPid, &lpQueueData->iTimerId);//或 lpQueueData->myPid.iProcessId
						AmsTimerStatProc(T_AMS_CUSTOMER_IN_QUEUE_TIMER, AMS_KILL_TIMER);
					} 
		
					//release lpQueueData Pid
	        		AmsReleassPid(lpQueueData->myPid, END);

					procNum ++;
					
					if(procNum >= AMS_MAX_PROC_NUM_IN_QUEUE)
					{
						return;
					}
					
					continue;
				}
				
				lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
				
				//更新进程数据
				//record vtmId
				lpAmsData->termIdLen = lpQueueData->termIdLen;
				memcpy(lpAmsData->termId,lpQueueData->termId,lpQueueData->termIdLen);
				
				//record vtmPos
				//lpAmsData->vtmPos = pVtmNode->vtmCfgPos;
				
				//Set vta call State, only one pthread!!!
				AmsSetVtaCallState(lpAmsData, pVtaNode, AMS_CALL_STATE_WAIT_ANSWER);

#ifdef AMS_TEST_LT
			    //calc vta workInfo
				AmsUpdateSingleVtaWorkInfo(pVtaNode, currentTime);
		
				//set Vta State and State Start Time
				AmsSetVtaState(iThreadId, lpAmsData, pVtaNode, AMS_VTA_STATE_BUSY, 0);
#endif

				//record vtmPid, may be used later
				memcpy(&lpAmsData->termPid,&pTermNode->rPid,sizeof(PID_t));
				
				//record callId
				lpAmsData->callIdLen = lpQueueData->callIdLen;
				if(lpQueueData->callIdLen <= AMS_MAX_CALLID_LEN)
				{
					memcpy(lpAmsData->callId, lpQueueData->callId, lpQueueData->callIdLen);
				}

				//record amsPid
				pTermNode->amsPid = pVtaNode->amsPid;

				//update Customer Service State
				AmsSetTermServiceState(pTermNode, AMS_CUSTOMER_IN_SERVICE);

				//record cmsPid
				lpAmsData->cmsPid.cModuleId	   = pTermNode->cmsPid.cModuleId;
				lpAmsData->cmsPid.cFunctionId  = pTermNode->cmsPid.cFunctionId;
				lpAmsData->cmsPid.iProcessId   = pTermNode->cmsPid.iProcessId;

				Sem_wait(&AmsSrvData(srvGrpId).freevtaCtrl);
				lstDelete(&AmsSrvData(srvGrpId).freevtaList, (NODE *)pVtaNode);
				Sem_post(&AmsSrvData(srvGrpId).freevtaCtrl);
				
			}

			//update srvGrpId
//			pVtmNode->vtmInfo.srvGrpId = lpQueueData->srvGrpId;

			//send Vta Get Rsp to CMS
		    AmsSendCmsVtaGetRsp(lpAmsData,&msg,iret,pVtaNode,NULL);

			/* 杀掉定时器 */
			if(lpQueueData->iTimerId >= 0)
			{
				AmsQueueKillTimer(pTermNode->customerPid, &lpQueueData->iTimerId);//或 lpQueueData->myPid.iProcessId
				AmsTimerStatProc(T_AMS_CUSTOMER_IN_QUEUE_TIMER, AMS_KILL_TIMER);
			} 

			//release lpQueueData Pid
			AmsReleassPid(lpQueueData->myPid, END);

			procNum ++;

			if(procNum > AMS_MAX_PROC_NUM_IN_QUEUE)
			{
				return;
			}
			
		}	
	}
}



int AmsProcServiceProcMsg(int iThreadId, MESSAGE_t *pMsg)
{

	time_t              currentTime;

	time(&currentTime);	

	//清理不活动的VTA连接，及释放业务处理进程
	//AmsClearInactiveVta(iThreadId, currentTime);

	//清理不活动的Term连接，及释放排队服务进程
	//AmsClearInactiveTerm(iThreadId, currentTime);


    //判断是否有等待业务的客户
    //若有，从队列头开始顺序为客户选择柜员；
    //若选择成功，则释放排队服务进程	
    AmsGetVtaInServiceProcTask(iThreadId, currentTime);


	//清理不活动的RCAS服务器
	//AmsClearInactiveRcas(iThreadId, currentTime);

	//清理不活动的CMS呼叫
	//AmsClearInactiveCmsCall(iThreadId, currentTime);

	
    //calc teller WorkInfo Every 5 Minutes
	//AmsUpdateVtaWorkInfoProc(currentTime);

	return AMS_OK;
	
}

int AmsSendCmsGetVtaTimeoutRsp(LP_QUEUE_DATA_t *lpQueueData,MESSAGE_t *pMsg,int iret)
{
	MESSAGE_t           s_Msg;
	unsigned char       *p;

	memset(&s_Msg,0,sizeof(MESSAGE_t));

	if(NULL == pMsg || NULL == lpQueueData)
	{
		return AMS_ERROR;
	}

	s_Msg.eMessageAreaId = A;
	memcpy(&s_Msg.s_ReceiverPid,&pMsg->s_SenderPid,sizeof(PID_t));
	s_Msg.s_SenderPid.cModuleId = SystemData.cMid;
	s_Msg.s_SenderPid.cFunctionId = FID_AMS;
	s_Msg.s_SenderPid.iProcessId = pMsg->s_ReceiverPid.iProcessId;
	s_Msg.iMessageType = A_VTA_GET_RSP;
	s_Msg.iMessageLength = 0;

    p = &s_Msg.cMessageBody[0];

	//Pack callId
	*p++ = lpQueueData->callIdLen;
	memcpy(p, lpQueueData->callId, lpQueueData->callIdLen);

	p += lpQueueData->callIdLen;

	s_Msg.iMessageLength += (1 + lpQueueData->callIdLen);	
	
	//Pack iret
	BEPUTLONG(iret, p);

	//Pack amsPid
	p += 4;

	//Pack tellerId
	p += 4;

	s_Msg.iMessageLength += 12;
	
	SendMsgBuff(&s_Msg,0);

	if(AmsMsgTrace)
	{	
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"send A_VTA_GET_RSP msg \n");	
		AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_VTA_GET_RSP",description,
						descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,lpQueueData->sTraceName);				
	}

    AmsMsgStatProc(AMS_CMS_MSG, s_Msg.iMessageType);
	AmsResultStatProc(AMS_CMS_GET_VTA_RESULT, iret);
	
	return SUCCESS;
}


int AmsCustomerInQueueTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg)
{
	int					iret = AMS_CMS_GET_VTA_TIMEOUT;
	LP_QUEUE_DATA_t     *lpQueueData = NULL;             //排队进程数据区指针
	LP_AMS_DATA_t		*lpAmsData = NULL;               //进程数据区指针	
	TERM_NODE            *pTermNode = NULL;			
	int                 pid = 0;
	MESSAGE_t           msg;
	unsigned int        queueId = 0;	
	unsigned int        i = 0;
	unsigned char       *p;
	
	if(AmsMsgTrace)
	{
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"recv B_AMS_CUSTOMER_IN_QUEUE_TIMEOUT msg[%d] \n",pTmMsg->iTimerId);	
		AmsTraceToFile(pTmMsg->s_ReceiverPid,pTmMsg->s_SenderPid,"B_AMS_CUSTOMER_IN_QUEUE_TIMEOUT",description,
						descrlen,pTmMsg->cTimerParameter,PARA_LEN,"ams");
	}
	
	//进程号有效性检查
	pid = pTmMsg->s_ReceiverPid.iProcessId;
	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		dbgprint("AmsCustomerInQueueTimeoutProc Pid:%d Err", pid);
		iret = AMS_CMS_GET_VTA_TIMEOUT_PARA_ERR;
		AmsResultStatProc(AMS_CMS_GET_VTA_RESULT, iret);
		return AMS_ERROR;
	}

	//消息长度检查
	if(pTmMsg->iMessageLength > (PARA_LEN + sizeof(char) + sizeof(int)))
	{
		dbgprint("AmsCustomerInQueueTimeoutProc[%d] Len:%d Err", pid, pTmMsg->iMessageLength);
		iret = AMS_CMS_GET_VTA_TIMEOUT_LEN_ERR;
		AmsResultStatProc(AMS_CMS_GET_VTA_RESULT, iret);
		return AMS_ERROR;
	}

	lpQueueData=(LP_QUEUE_DATA_t *)ProcessData[pid];

	/* 杀掉定时器 */
	if(lpQueueData->iTimerId >= 0)
	{
		AmsQueueKillTimer(pid, &lpQueueData->iTimerId);
		AmsTimerStatProc(T_AMS_CUSTOMER_IN_QUEUE_TIMER, AMS_KILL_TIMER);
		pTmMsg->iTimerId = -1;
	} 

	//进程号匹配性检查
	if(lpQueueData->myPid.iProcessId != pid)
	{
		dbgprint("AmsCustomerInQueueTimeoutProc Term[%s] PID[%d][%d] Not Equal", 
			lpQueueData->termId,
			lpQueueData->myPid.iProcessId, pid);
		iret = AMS_CMS_GET_VTA_TIMEOUT_PARA_ERR;
		AmsResultStatProc(AMS_CMS_GET_VTA_RESULT, iret);
		return AMS_ERROR;
	}
	
	//fill cmsPid
	memset(&msg, 0, sizeof(MESSAGE_t));
	memcpy(&msg.s_SenderPid, &lpQueueData->cmsPid, sizeof(PID_t));
	
	//
	p = pTmMsg->cTimerParameter;
	BEGETLONG(queueId, p);
	if(lpQueueData->queueId != queueId|| 0 == queueId)
	{
		dbgprint("AmsCustomerInQueueTimeoutProc[%d] Term[%s]Id[%u]Err.", 
			pid, lpQueueData->termId, queueId);
		iret = 	AMS_CMS_GET_VTA_TIMEOUT_VTM_ID_ERR;
		AmsResultStatProc(AMS_CMS_GET_VTA_RESULT, iret);
		return AMS_ERROR;		
	}
			
    /* find Vtm Node in process */
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		pTermNode = AmsSearchTermNode(i, lpQueueData->termId,lpQueueData->termIdLen);
		if(NULL != pTermNode)
		{
			break;
		}
	}
	
	if(NULL == pTermNode)
	{
		dbgprint("AmsCustomerInQueueTimeoutProc[%d] Term[%s]Id[%u]Err", 
			pid, lpQueueData->termId, queueId);
		iret = 	AMS_CMS_GET_VTA_TIMEOUT_VTM_ID_ERR;
		AmsResultStatProc(AMS_CMS_GET_VTA_RESULT, iret);
		return AMS_ERROR;		
	}

    //check state
 	if(AMS_TERM_STATE_BUSY != pTermNode->state)
	{
		dbgprint("AmsCustomerInQueueTimeoutProc[%d] Term[%s][%u]State[%d]Err", 
			pid, lpQueueData->termId, pTermNode->state);
		iret = 	AMS_CMS_GET_VTA_TIMEOUT_STATE_ERR;
		AmsResultStatProc(AMS_CMS_GET_VTA_RESULT, iret);	
	}
	if(AMS_TERM_STATE_IDLE != pTermNode->state)
	{
		//set Vtm State and State Start Time
		AmsSetTermState(iThreadId, pTermNode, AMS_TERM_STATE_IDLE);		
	}

	//check vtm serviceState
 	if(AMS_CUSTOMER_IN_QUEUE != pTermNode->serviceState)
	{
		dbgprint("AmsCustomerInQueueTimeoutProc[%d] Term[%s]ServiceState[%d]Err", 
			pid, lpQueueData->termId, pTermNode->serviceState);
		iret = 	AMS_CMS_GET_VTA_TIMEOUT_SERVICE_STATE_ERR;
		AmsResultStatProc(AMS_CMS_GET_VTA_RESULT, iret);	
	}
	if(AMS_CUSTOMER_SERVICE_NULL != pTermNode->serviceState)
	{
		//update Customer Service State
		AmsSetTermServiceState(pTermNode, AMS_CUSTOMER_SERVICE_NULL);
	}
	
	//update callState if need
	if(AMS_CALL_STATE_NULL != pTermNode->callState)
	{
		//set Vtm Call State and State Start Time
		AmsSetTermCallState(pTermNode, AMS_CALL_STATE_NULL);
	}
	
	//reset amsPid
	pTermNode->amsPid = 0;

	//reset customerPid
	pTermNode->customerPid = 0;
		
	//send Cms Vta Get Rsp
	AmsSendCmsGetVtaTimeoutRsp(lpQueueData,&msg,iret);

	//release lpQueueData Pid
	AmsReleassPid(lpQueueData->myPid, END);

	return iret;

}



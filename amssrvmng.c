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
    //zhuyn added 20161101 Êý¾Ý¿âÊý¾Ý¼ÓÔØÐè·ÅÔÚListInitÖ®ºó
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
			AmsTellerStat(i).tellerId = AmsCfgTeller(i).tellerId; //ÉèÖÃtellerId
			Display("AmsTeller[%s][%u]Stat Init[%d]\r\n", 
				AmsCfgTeller(i).tellerNo, AmsCfgTeller(i).tellerId, i);
		}
	}
	for(i = 0; i < Min(SystemData.AmsPriData.amsCfgData.maxVtmNum, AMS_MAX_VTM_NUM); i++)
	{
		if(0 != AmsCfgVtm(i).vtmId)
		{
			AmsVtmStat(i).vtmId = AmsCfgVtm(i).vtmId;   //ÉèÖÃvtmId
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
	
	while(NULL != pTempTermNode && i < custNum)//²»±Ø <=
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
				pQueueInfo->queuingTime = AmsCfgQueueSys(i).avgSrvTime * (pQueueInfo->queuingLen/tellerNum + 0.5);//0.5 ¼Ù¶¨µ±Ç°ÒµÎñÆ½¾ùÍê³ÉÒ»°ë
			}
			else
			{
				pQueueInfo->queuingTime = AmsCfgQueueSys(i).avgSrvTime * pQueueInfo->queuingLen; //µÈ´ý¹ñÔ±À´·þÎñ£¬Ò»°ã²»»á·¢Éú		
				
			}
			
			break;
		}
	}
	if(i >= AMS_MAX_SERVICE_GROUP_NUM)
	{
		if(tellerNum > 0)
		{
			pQueueInfo->queuingTime = AMS_AVG_SERVICE_TIME * (pQueueInfo->queuingLen/tellerNum + 0.5);	//0.5 ¼Ù¶¨µ±Ç°ÒµÎñÆ½¾ùÍê³ÉÒ»°ë			
		}
		else
		{
			pQueueInfo->queuingTime = AMS_AVG_SERVICE_TIME * pQueueInfo->queuingLen; //µÈ´ý¹ñÔ±À´·þÎñ£¬Ò»°ã²»»á·¢Éú			
			
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


	//¼ì²éÒµÎñÀàÐÍ
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

	//¼ì²éµ±Ç°ÒµÎñ×é¿ÕÏÐ¹ñÔ±ÊýÁ¿
	idleVtaNum = lstCount(&AmsSrvData(srvGrpId).freevtaList);

	//µ±Ç°ÒµÎñ×éÓÐ¿ÕÏÐ¹ñÔ±
	if(0 != idleVtaNum)
	{
		//³õ²½Ñ¡ÖÐÒµÎñ×é
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
		//¼ì²éµ±Ç°ÒµÎñ×éµÄ¿Í»§ÅÅ¶Ó³¤¶È
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
			&& 0 == memcmp(AmsCfgQueueSys(k).srvGrpId,AmsCfgSrvGroup(srvGrpId).srvGrpId,AmsCfgSrvGroup(srvGrpId).srvGrpIdLen))//ÖØ¸´~
			{
				//ÒÑ¾­´ïµ½µ±Ç°ÒµÎñ×éÔÊÐíµÄ×î´óÅÅ¶Ó³¤¶È
				if(queueInfo.queuingLen >= AmsCfgQueueSys(k).maxQueLen)
				{
					return AMS_OK;		//´ý¶¨å???ÊÇ·ñÐèÒª¼ÌÐø²éÕÒ±ðµÄÖÇÄÜÒµÎñ×é					
				}
				else
				{
					if(0 == srvGrpSelect)
					{
						//³õ²½Ñ¡ÖÐÒµÎñ×é
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

/*Î´Ö¸¶¨ÒµÎñ×é
1 ÏÈÔÚ¸ÃÒµÎñÀàÐÍµÄÖÇÄÜÒµÎñ×éÖÐ½øÐÐ²éÕÒ
2 1ÎÞ·¨ÕÒµ½£¬ÔÙ¼ÌÐøÔÚÈË¹¤ÒµÎñ×éÖÐ½øÐÐ²éÕÒ
3 2Ò²ÎÞ·¨ÕÒµ½£¬·µ»Ø²éÕÒÊ§°Ü
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

	//Ñ¡ÔñÖÇÄÜÒµÎñ×é
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		//ÕÒµ½ÅäÖÃ´ËÒµÎñÀàÐÍµÄÒµÎñ×é
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
		//Ñ¡ÔñÖÇÄÜÒµÎñ×é
		for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
		{
			//ÕÒµ½ÅäÖÃ´ËÒµÎñÀàÐÍµÄÒµÎñ×é
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

	//Ñ¡ÖÐÒµÎñ×é
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
	LP_QUEUE_DATA_t     *lpQueueData = NULL;      //ÅÅ¶Ó½ø³ÌÊý¾ÝÇøÖ¸Õë 
	PID_t				Pid;
	unsigned char       timerPara[AMS_MAX_TIMER_PARA_LEN];
	MESSAGE_t           msg;

	if(NULL == pMsg || NULL == pTermNode)
	{
		dbgprint("AmsStartCustomerQueueProcess Para[%d][%d]", pMsg, pTermNode);
		iret = AMS_CMS_GET_VTA_PARA_ERR;
		return iret;
	}
	
    //·ÖÅäÅÅ¶Ó·þÎñ½ø³ÌºÅ
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

	//ÅÅ¶Ó½ø³ÌÊý¾ÝÇø³õÊ¼»¯
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
	
	//ÏûÏ¢¸ú×Ù¿ª¹ØÅäÖÃ
	lpQueueData->debugTrace  = (unsigned char)AmsDebugTrace;
	lpQueueData->commonTrace = (unsigned char)AmsCommonTrace;
	lpQueueData->msgTrace    = (unsigned char)AmsMsgTrace;
	lpQueueData->stateTrace  = (unsigned char)AmsStateTrace;
	lpQueueData->timerTrace  = (unsigned char)AmsTimerTrace;
	lpQueueData->errorTrace  = (unsigned char)AmsErrorTrace;
	lpQueueData->alarmTrace  = (unsigned char)AmsAlarmTrace;	
	
	memcpy(lpQueueData->sTraceName,"ams",3);
	lpQueueData->lTraceNameLen = 3;
	
    //´´½¨ÅÅ¶Ó¶¨Ê±Æ÷
	//Customer Wait In Queue,default: 600s
    if(    AmsCfgData.amsCustomerInQueueTimeLength > 0 
	    && AmsCfgData.amsCustomerInQueueTimeLength <= T_AMS_CUSTOMER_IN_QUEUE_TIMER_LENGTH_MAX)
    {
		memset(timerPara, 0, PARA_LEN);
		BEPUTLONG(lpQueueData->vtmId,timerPara);
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
						   unsigned char serviceType,
						   unsigned int vtaNum,
						   int *pResult)
{
	VTA_NODE            *pVtaNode = NULL;	
	VTA_NODE            *pVtaIdleLongestNode = NULL;	
	unsigned int        idleLongestTime = 0;
	time_t              currentTime;	
	unsigned int        i = 0;

	
	//¼ì²éÒµÎñ×é±àºÅ
	if(srvGrpId > AMS_MAX_SERVICE_GROUP_NUM)
	{
		dbgprint("AmsSelectVta Term[%s] SrvGrpId[%d]Err", termId, srvGrpId);
		*pResult = AMS_CMS_GET_VTA_SIR_SERVICE_GROUP_ID_ERR;
		return NULL;		
	}

	//»ñÈ¡×øÏ¯Á´±íÍ·½áµã
	pVtaNode = (VTA_NODE *)lstFirst(&AmsSrvData(srvGrpId).vtaList);

	//Ã»ÓÐ×øÏ¯µÇÂ½
	if(NULL == pVtaNode)
	{
		dbgprint("AmsSelectVta Term[%s] SrvGrpId[%u] VtaListEmpty.", 
			termId, srvGrpId);			
		*pResult = AMS_CMS_GET_VTA_SIR_VTA_EMPTY;
		return NULL;	
	}

	//»ñÈ¡¿ÕÏÐ×øÏ¯Á´±íÍ·½Úµã
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


VTA_NODE *AmsServiceIntelligentSelectVta(unsigned char *pTermNo,
											   unsigned char termId[],
											   unsigned int srvGrpId, 
											   unsigned char serviceType[],
											   int *pResult)
{
	
	unsigned int		vtaNum;
	VTA_NODE			*pVtaNode = NULL;	
	QUEUE_INFO			queueInfo;
	unsigned int		i = 0;

	//¼ì²é¹ñÔ±»úÉè±¸ºÅ
	if(NULL == pTermNo)
	{
		dbgprint("AmsSISelectVta TermId[%u] SrvGrpId[%d] VtmNoErr", 
			termId, srvGrpId);
		*pResult = AMS_CMS_GET_VTA_SIR_VTMNO_ERR;
		return NULL;		
	}
	
	//¼ì²éÒµÎñ×é±àºÅ
	if(srvGrpId > AMS_MAX_SERVICE_GROUP_NUM)
	{
		dbgprint("AmsSISelectVta Term[%s] SrvGrpId[%d]Err", 
			termId, srvGrpId);
		*pResult = AMS_CMS_GET_VTA_SIR_SERVICE_GROUP_ID_ERR;
		return NULL;		
	}

	//¼ì²é¿ÕÏÐ×øÏ¯¶ÓÁÐ
	vtaNum = lstCount(&AmsSrvData(srvGrpId).freevtaList);
	if(0 == vtaNum)
	{
		dbgprint("AmsSISelectVta Term[%s] SrvGrpId[%u] VtaListEmpty", 
				termId, srvGrpId);
		*pResult = AMS_CMS_GET_VTA_SIR_VTA_EMPTY;
		return NULL;	
	}

	//¼ì²é¿Í»§ÅÅ¶Ó³¤¶È
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
				&& 0 == memcmp(AmsCfgQueueSys(i).srvGrpId,AmsCfgSrvGroup(srvGrpId).srvGrpId,AmsCfgSrvGroup(srvGrpId).srvGrpIdLen))//ÖØ¸´~
			{
				//ÒÑ¾­´ïµ½ÔÊÐíµÄ×î´óÅÅ¶Ó³¤¶È
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

	//³£¹æÑ¡Ôñ
	pVtaNode = AmsSelectVta(termId,srvGrpId,serviceType,vtaNum,pResult);
	//Â·ÓÉÊ§°Ü£¬
	if(*pResult != AMS_CMS_PRCOESS_SUCCESS)
	{
			
		//Â·ÓÉÊ§°ÜÔ­ÒòÎªÔÝÎÞºÏÊÊ×øÏ¯£¬½øÈëÅÅ¶Ó
		if(AMS_CMS_GET_VTA_SERVICE_IN_QUEUE == *pResult)
		{
			return NULL;
		}

		//Â·ÓÉÊ§°ÜÔ­ÒòÎª³öÏÖ´íÎó
		if(AmsDebugTrace)
		{
			dbgprint("AmsSISelectVta Term[%s] Result[0x%x]Err", termId, *pResult);
		}
		
		return NULL;
	}
	
	//Â·ÓÉ³É¹¦
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

	//ÆäËûÀàÐÍÂ·ÓÉÊ§°Ü£¬Ä¬ÈÏ½øÈëÅÅ¶Ó
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


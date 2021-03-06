#include "amsfunc.h"


static int 			amsDataInitialised = 0;
static int			amsCfgDataInitialised = 0;


static int          freeAmsCrmListInitialled = 0;
pthread_mutex_t     freeAmsCrmListMtx;
LIST 				freeAmsCrmList;
CRM_NODE        	*freeAmsCrmListBufPtr = NULL;

static int          freeAmsTermListInitialled = 0;
pthread_mutex_t     freeAmsTermListMtx;
LIST 				freeAmsTermList;
TERM_NODE            *freeAmsTermListBufPtr = NULL;



static int			freeAmsTellerInfoListInitialled = 0;
LIST				freeAmsTellerInfoList;
TELLER_INFO_NODE	*freeAmsTellerInfoListBufPtr = NULL;

static int			freeAmsVtmInfoListInitialled = 0;
LIST				freeAmsVtmInfoList;
VTM_INFO_NODE		*freeAmsVtmInfoListBufPtr = NULL;

static int			freeAmsRegTellerInfoListInitialled = 0;
pthread_mutex_t     freeAmsRegTellerInfoListMtx;
LIST				freeAmsRegTellerInfoList;
TELLER_REGISTER_INFO_NODE	*freeAmsRegTellerInfoListBufPtr = NULL;

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
/*	if(AMS_SUCCESS != VtmInfoListInit())
	{
		Display("VtmInfoListInit:Error-vtminfo nodes error!\r\n");
		return AMS_ERROR;
	}
*/
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
	
    // init free crm list 
	if(AMS_SUCCESS != CrmListInit())
	{
		Display("AmsDataInit:Error-crm nodes error!\r\n");
    	return AMS_ERROR;
	}	    

    // init free vtm list 
	if(AMS_SUCCESS != TermListInit())
	{
		Display("AmsDataInit:Error-term nodes error!\r\n");
    	return AMS_ERROR;
	}

    // init free crm id list 
	/*if(AMS_SUCCESS != CrmIdListInit())
	{
		Display("AmsDataInit:Error-crm id nodes error!\r\n");
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
	    Sem_init(&AmsSrvData(i).crmCtrl,0,1);

		Sem_init(&AmsSrvData(i).termCtrl,0,1);

		Sem_init(&AmsSrvData(i).freecrmCtrl,0,1);

//		AmsSrvData(i).serviceState = AMS_SERVICE_INACTIVE;

//		AmsSrvData(i).preSrvTellerId = 0;
		
    }

	/* clear regData */
	memset(&SystemData.AmsPriData.amsRegData, 0, sizeof(AMS_DATA_REGISTER));

	/* clear stat */	
	/*memset(&SystemData.AmsPriData.amsStat, 0, sizeof(AMS_STAT));
	for(i = 0; i < Min(SystemData.AmsPriData.amsCfgData.maxCrmNum, AMS_MAX_CRM_NUM); i++)
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

	size = sizeof(TELLER_INFO_NODE) * AMS_MAX_CRM_NODES;
	freeAmsTellerInfoListBufPtr = (TELLER_INFO_NODE *)malloc(size);
	pNode = (TELLER_INFO_NODE *)freeAmsTellerInfoListBufPtr;
	if(NULL == pNode)
	{
		dbgprint("TellerInfoListInit AllocMem[%d]Err\r\n", size);
		return AMS_ALLOC_MEM_FAILED;
	}

	for(i = 0; i < AMS_MAX_CRM_NODES;i++,pNode++)
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

	size = sizeof(TELLER_REGISTER_INFO_NODE) * AMS_MAX_CRM_NODES;
	freeAmsRegTellerInfoListBufPtr = (TELLER_REGISTER_INFO_NODE *)malloc(size);
	pNode = (TELLER_REGISTER_INFO_NODE *)freeAmsRegTellerInfoListBufPtr;
	if(NULL == pNode)
	{
		dbgprint("TellerInfoListInit AllocMem[%d]Err\r\n", size);
		return AMS_ALLOC_MEM_FAILED;
	}

	for(i = 0; i < AMS_MAX_CRM_NODES;i++,pNode++)
	{
		lstAdd(&freeAmsRegTellerInfoList,(NODE *)pNode);
	}

	Pthread_mutex_init(&freeAmsRegTellerInfoListMtx,NULL);

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

	size = sizeof(VTM_INFO_NODE) * AMS_MAX_TERM_NODES;
	freeAmsVtmInfoListBufPtr = (VTM_INFO_NODE *)malloc(size);
	pNode = (VTM_INFO_NODE *)freeAmsVtmInfoListBufPtr;
	if(NULL == pNode)
	{
		dbgprint("TellerInfoListInit AllocMem[%d]Err\r\n", size);
		return AMS_ALLOC_MEM_FAILED;
	}

	for(i = 0; i < AMS_MAX_CRM_NODES;i++,pNode++)
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
	Pthread_mutex_lock(&freeAmsCrmListMtx);
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


int CrmListInit()
{
	CRM_NODE            *pNode;
	int                 size;	
	int                 i;
	
	if(0 != freeAmsCrmListInitialled)
	{
		return AMS_SUCCESS;
	}

	lstInit(&freeAmsCrmList);
	
	size = sizeof(CRM_NODE) * AMS_MAX_CRM_NODES;
	freeAmsCrmListBufPtr = (CRM_NODE *)malloc(size);
	pNode = (CRM_NODE *)freeAmsCrmListBufPtr;
	if(NULL == pNode)
	{
		dbgprint("CrmListInit AllocMem[%d]Err\r\n", size);
		return AMS_ALLOC_MEM_FAILED;
	}
	
	for(i = 0;i < AMS_MAX_CRM_NODES;i++,pNode++)
	{
        lstAdd(&freeAmsCrmList,(NODE *)pNode);
	}
	
    Pthread_mutex_init(&freeAmsCrmListMtx,NULL);
	
	freeAmsCrmListInitialled = 1;
	
	return AMS_SUCCESS;
}

CRM_NODE * CrmNodeGet(void)
{
	CRM_NODE            *pNode;
	
    if(!freeAmsCrmListInitialled)
    {
		dbgprint("CrmNodeGet InitFlag[%d] Err\r\n",freeAmsCrmListInitialled);
        return NULL;
    }
	
    Pthread_mutex_lock(&freeAmsCrmListMtx);
    pNode = (CRM_NODE *)lstGet(&freeAmsCrmList);
    Pthread_mutex_unlock(&freeAmsCrmListMtx);
	
    return pNode;
}

void CrmNodeFree(CRM_NODE *pNode)
{
    if(!freeAmsCrmListInitialled)
    {
        return;
    }
	
    Pthread_mutex_lock(&freeAmsCrmListMtx);
    lstAdd(&freeAmsCrmList,(NODE *)pNode);
    Pthread_mutex_unlock(&freeAmsCrmListMtx);
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






int AmsUpdateSingleCrmWorkInfo(CRM_NODE *pCrmNode, time_t currentTime)
{
	if(NULL == pCrmNode)
	{
		return AMS_ERROR;
	}
	
	if(currentTime > pCrmNode->stateStartTime && currentTime > pCrmNode->workInfoUpdateTime)
	{
		if(AMS_CRM_STATE_BUSY == pCrmNode->state || AMS_CRM_STATE_PREPARE == pCrmNode->state)
		{
			if(pCrmNode->workInfoUpdateTime > pCrmNode->stateStartTime)
			{
				pCrmNode->crmWorkInfo.workSeconds += (currentTime - pCrmNode->workInfoUpdateTime);
			}
			else
			{
				pCrmNode->crmWorkInfo.workSeconds += (currentTime - pCrmNode->stateStartTime);
			}
			
			pCrmNode->workInfoUpdateTime = currentTime;

			return AMS_OK;
		}
	
		if(AMS_CRM_STATE_IDLE == pCrmNode->state || AMS_CRM_STATE_REST == pCrmNode->state)
		{
			if(pCrmNode->workInfoUpdateTime > pCrmNode->stateStartTime)
			{
				pCrmNode->crmWorkInfo.idleSeconds += (currentTime - pCrmNode->workInfoUpdateTime);
			}
			else
			{
				pCrmNode->crmWorkInfo.idleSeconds += (currentTime - pCrmNode->stateStartTime);
			}
			
			pCrmNode->workInfoUpdateTime = currentTime;

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
	tellerNum = lstCount(&AmsSrvData(srvGrpId).freecrmList);
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
	CRM_NODE            *pCrmNode = NULL;	
	unsigned int        idleCrmNum = 0;
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
	idleCrmNum = lstCount(&AmsSrvData(srvGrpId).freecrmList);

	//当前业务组有空闲柜员
	if(0 != idleCrmNum)
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
					return AMS_ERROR;		//返回失败			
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
	CRM_NODE            *pCrmNode = NULL;	
	unsigned int        crmNum = 0;
	unsigned int        idleCrmNum = 0;
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

	if(iret == AMS_ERROR)
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

	if(iret == AMS_ERROR)
	{
		if(AmsDebugTrace)	
		{
			dbgprint("AmsTerm[%s] serviceType[0x%x] SelectSrvGrp Fail", 
				termId, serviceType);
		}
		
		return AMS_CMS_GET_CRM_NO_VALID_SERVICE_GROUP_ID_OR_TYPE;
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
		iret = AMS_CMS_GET_CRM_PARA_ERR;
		return iret;
	}
	
    //分配排队服务进程号
	memset(&Pid,0,sizeof(PID_t));
	iret = AmsAllocPid(&Pid);
	if(-1 == iret)
	{
		dbgprint("AmsStartCustomerQueueProcess Term[%s] AllocPid: SysBusy",
			pTermNode->termInfo.termId);
		
		iret = AMS_CMS_GET_CRM_LP_RESOURCE_LIMITED;
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
   // memcpy(&lpQueueData->vtmPid,&pTermNode->rPid,sizeof(PID_t));

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
	lpQueueData->cmsgTrace   = (unsigned char)AmsCMsgTrace;
	
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
			iret = AMS_CMS_GET_CRM_CREATE_TIMER_ERR;
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



CRM_NODE *AmsSelectCrm(	unsigned char termId[],
						   unsigned int srvGrpId, 
						   unsigned char serviceType[],
						   unsigned int crmNum,
						   int *pResult)
{
	CRM_NODE            *pCrmNode = NULL;	
	CRM_NODE            *pCrmIdleLongestNode = NULL;	
	unsigned int        idleLongestTime = 0;
	time_t              currentTime;	
	unsigned int        i = 0;

	
	//检查业务组编号
	if(srvGrpId > AMS_MAX_SERVICE_GROUP_NUM)
	{
		dbgprint("AmsSelectCrm Term[%s] SrvGrpId[%d]Err", termId, srvGrpId);
		*pResult = AMS_CMS_GET_CRM_SIR_SERVICE_GROUP_ID_ERR;
		return NULL;		
	}

	//获取坐席链表头结点
	pCrmNode = (CRM_NODE *)lstFirst(&AmsSrvData(srvGrpId).crmList);

	//没有坐席登陆
	if(NULL == pCrmNode)
	{
		dbgprint("AmsSelectCrm Term[%s] SrvGrpId[%u] CrmListEmpty.", 
			termId, srvGrpId);			
		*pResult = AMS_CMS_GET_CRM_SIR_CRM_EMPTY;
		return NULL;	
	}

	//获取空闲坐席链表头节点
	pCrmNode = (CRM_NODE *)lstGet(&AmsSrvData(srvGrpId).freecrmList);
	if(NULL == pCrmNode)
	{
		*pResult = AMS_CMS_GET_CRM_SERVICE_IN_QUEUE;
		return NULL;	
	}
	else
	{
		return pCrmNode;
	}
	
}


CRM_NODE *AmsServiceIntelligentSelectCrm(
									   unsigned char termId[],
									   unsigned int srvGrpId, 
									   unsigned char serviceType[],
									   int *pResult)
{
	
	unsigned int		crmNum;
	CRM_NODE			*pCrmNode = NULL;	
	QUEUE_INFO			queueInfo;
	unsigned int		i = 0;
	
	
	//检查业务组编号
	if(srvGrpId > AMS_MAX_SERVICE_GROUP_NUM)
	{
		dbgprint("AmsSISelectCrm Term[%s] SrvGrpId[%d]Err", 
			termId, srvGrpId);
		*pResult = AMS_CMS_GET_CRM_SIR_SERVICE_GROUP_ID_ERR;
		return NULL;		
	}

	//检查空闲坐席队列
	/*crmNum = lstCount(&AmsSrvData(srvGrpId).freecrmList);
	if(0 == crmNum)
	{
		dbgprint("AmsSISelectCrm Term[%s] SrvGrpId[%u] CrmListEmpty", 
				termId, srvGrpId);
		*pResult = AMS_CMS_GET_CRM_SIR_CRM_EMPTY;
		return NULL;	
	}*/

	//检查客户排队长度
	if(lstCount(&AmsSrvData(srvGrpId).termList) >= 1)

	{
		memset(&queueInfo,0,sizeof(QUEUE_INFO));
		if(AMS_OK != AmsCustCalcSrvGrpIdQueueInfo(termId, srvGrpId, &queueInfo))
		{
			dbgprint("AmsSISelectCrm Term[%s] CalcSrvGrp[%u]QueueInfoErr", 
				termId, srvGrpId);
			*pResult = AMS_CMS_GET_CRM_SIR_CALC_QUEUE_INFO_ERR;
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
					dbgprint("AmsSISelectCrm Term[%s] SrvGrpId[%u] tooManyCustomers[%u][%u][%d]!", 
						termId, srvGrpId, 
						queueInfo.queuingLen, AmsCfgQueueSys(i).maxQueLen, i);						
					*pResult = AMS_CMS_GET_CRM_SIR_TOO_MANY_CUSTOMER_IN_QUEUE;
					return NULL;							
				}
			}
		}
	}	

	//常规选择
	pCrmNode = AmsSelectCrm(termId,srvGrpId,serviceType,crmNum,pResult);
	//路由失败，
	if(*pResult != AMS_CMS_PRCOESS_SUCCESS)
	{
			
		//路由失败原因为暂无合适坐席，进入排队
		if(AMS_CMS_GET_CRM_SERVICE_IN_QUEUE == *pResult)
		{
			return NULL;
		}

		//路由失败原因为出现错误
		if(AmsDebugTrace)
		{
			dbgprint("AmsSISelectCrm Term[%s] Result[0x%x]Err", termId, *pResult);
		}
		
		return NULL;
	}
	
	//路由成功
	if(NULL != pCrmNode)
	{
		if(AmsDebugTrace)
		{
			dbgprint("AmsSISelectCrm Term[%s] GetCrm[%s].", 
				termId,  
				pCrmNode->crmInfo.tellerId);
		}
					
		return pCrmNode;
	}

	//其他类型路由失败，默认进入排队
	//not get, please wait ...
	*pResult = AMS_CMS_GET_CRM_SERVICE_IN_QUEUE;
	
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

	if(AmsCMsgTrace)
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



void AmsGetCrmInServiceProcTask(int iThreadId, time_t currentTime)
{
	int					iret = AMS_CMS_PRCOESS_SUCCESS;		
	LP_QUEUE_DATA_t     *lpQueueData = NULL;      //排队进程数据区指针	
	LP_AMS_DATA_t		*lpAmsData = NULL;        //进程数据区指针			
	TERM_NODE            *pTermNode = NULL;	
	CRM_NODE            *pCrmNode = NULL;
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

//			dbgprint("AmsServiceIntelligentSelectCrm Vtm[%s][%u]SrvGrpId[%u]\r\n",
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
			pCrmNode = AmsServiceIntelligentSelectCrm(
													  lpQueueData->termId,
			                                          lpQueueData->srvGrpId,
			                                          lpQueueData->serviceType,
			                                          &iret);
			if(NULL == pCrmNode)
			{
				if(AMS_CMS_GET_CRM_SERVICE_IN_QUEUE != iret)
				{
					dbgprint("ServiceProcTask Vtm[%s] GetCrm SISelectCrm Failed",
						lpQueueData->termId);
					
					if(AMS_ERROR == iret)
					{
						iret = AMS_CMS_GET_CRM_SERVICE_INTELLIGENT_ROUTING_ERR;
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
					AmsSendCmsCrmGetRsp(NULL,&msg,iret,NULL,pTermNode);	
					
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
					//send Crm Get Rsp to CMS
				    AmsSendCmsCrmGetRsp(NULL,&msg,iret,NULL,NULL);

					/* 杀掉定时器 */
					if(lpQueueData->iTimerId >= 0)
					{
						AmsQueueKillTimer(pTermNode->customerPid, &lpQueueData->iTimerId);//或 lpQueueData->myPid.iProcessId
						AmsTimerStatProc(T_AMS_CUSTOMER_IN_QUEUE_TIMER, AMS_KILL_TIMER);
					} 
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
				pid = pCrmNode->amsPid & 0xffff;
				if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
				{
					dbgprint("ServiceProcTask Vtm[%s] GetCrm Pid:%d Err", 
						lpQueueData->termId, pid);
					
					iret = AMS_CMS_GET_CRM_AMS_PID_ERR;
					AmsSendCmsCrmGetRsp(NULL,&msg,iret,pCrmNode,NULL);

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
				
				//Set crm call State, only one pthread!!!
				AmsSetCrmCallState(lpAmsData, pCrmNode, AMS_CALL_STATE_WAIT_ANSWER);

#ifdef AMS_TEST_LT
			    //calc crm workInfo
				AmsUpdateSingleCrmWorkInfo(pCrmNode, currentTime);
		
				//set Crm State and State Start Time
				AmsSetCrmState(iThreadId, lpAmsData, pCrmNode, AMS_CRM_STATE_BUSY, 0);
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
				pTermNode->amsPid = pCrmNode->amsPid;

				//update Customer Service State
				AmsSetTermServiceState(pTermNode, AMS_CUSTOMER_IN_SERVICE);

				//record cmsPid
				lpAmsData->cmsPid.cModuleId	   = pTermNode->cmsPid.cModuleId;
				lpAmsData->cmsPid.cFunctionId  = pTermNode->cmsPid.cFunctionId;
				lpAmsData->cmsPid.iProcessId   = pTermNode->cmsPid.iProcessId;

				Sem_wait(&AmsSrvData(srvGrpId).freecrmCtrl);
				lstDelete(&AmsSrvData(srvGrpId).freecrmList, (NODE *)pCrmNode);
				Sem_post(&AmsSrvData(srvGrpId).freecrmCtrl);

				//send Crm Get Rsp to CMS
			    AmsSendCmsCrmGetRsp(lpAmsData,&msg,iret,pCrmNode,NULL);

				/* 杀掉定时器 */
				if(lpQueueData->iTimerId >= 0)
				{
					AmsQueueKillTimer(pTermNode->customerPid, &lpQueueData->iTimerId);//或 lpQueueData->myPid.iProcessId
					AmsTimerStatProc(T_AMS_CUSTOMER_IN_QUEUE_TIMER, AMS_KILL_TIMER);
				} 

				/* Del Vtm Node from List */
			    Sem_wait(&AmsSrvData(i).termCtrl);
				lstDelete(&AmsSrvData(i).termList, (NODE *)pTermNode);
				Sem_post(&AmsSrvData(i).termCtrl);

			    TermNodeFree(pTermNode);	
			}

			//update srvGrpId
//			pVtmNode->vtmInfo.srvGrpId = lpQueueData->srvGrpId;

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

void AmsClearInactiveCrm(int iThreadId, time_t currentTime)
{
	LP_AMS_DATA_t		*lpAmsData = NULL;        //进程数据区指针	
	CRM_NODE            *pCrmNode = NULL;
	CRM_NODE            *pCrmTempNode = NULL;	
	int                 clearNum = 0;
	int                 pid = 0;
	int                 i = 0;
	int                 j = 0;	

    /* find inactive Crm Node in process */
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		j = 0;
		pCrmNode = (CRM_NODE *)lstFirst(&AmsSrvData(i).crmList);
		while(NULL != pCrmNode && j < AMS_MAX_CRM_NODES)
		{
#ifdef AMS_TEST_LT
			pCrmNode->handshakeTime = currentTime;
#endif
			if(currentTime - pCrmNode->handshakeTime >= AMS_CRM_CONNECT_CLEAR_INTERVAL)
			{
				dbgprint("Teller[%s] %lu : %lu -:%lu Clear!", pCrmNode->crmInfo.tellerId, 
					currentTime, pCrmNode->handshakeTime, currentTime - pCrmNode->handshakeTime);
				pid = pCrmNode->amsPid & 0xffff;
				if(pid > 0 && pid < LOGIC_PROCESS_SIZE)
				{
					lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
					//notify cms to release call
					if(    pCrmNode->callState > AMS_CALL_STATE_NULL 
						&& pCrmNode->callState < AMS_CALL_STATE_RSVD)
					{
						AmsSendCmsCallEventInd(lpAmsData,AMS_CALL_EVENT_IND_TELLER_LOGOUT);
					}

				    //calc crm workInfo
					AmsUpdateSingleCrmWorkInfo(pCrmNode, currentTime);
	
					//set Crm State and State Start Time
					AmsSetCrmState(iThreadId, lpAmsData, pCrmNode, AMS_CRM_STATE_OFFLINE, 0);

					if(AMS_CALL_STATE_NULL != pCrmNode->callState)
					{
						//set Crm Call State and State Start Time 
						AmsSetCrmCallState(lpAmsData, pCrmNode, AMS_CALL_STATE_NULL);
					}

					//crm失联后，cms term 挂机通知失效，提前设置term状态
					AmsResetTermState(iThreadId, lpAmsData->srvGrpIdPos, lpAmsData->termId, lpAmsData->termIdLen);
					
					//AmsResultStatProc(AMS_CRM_HANDSHAKE_RESULT, AMS_CRM_HANDSHAKE_CLEAR_INACTIVTE_TELLER);

					pCrmTempNode = pCrmNode;
					pCrmNode = (CRM_NODE *)lstNext((NODE *)pCrmNode);
					j++;
					
					/* 杀掉定时器 */
					AmsKillCrmAllTimer(lpAmsData, pid);
							
				    //update time, used later
				    memset(&pCrmTempNode->stateStartLocalTime, 0, sizeof(TIME_INFO));
				    memset(&pCrmTempNode->callStateStartLocalTime, 0, sizeof(TIME_INFO)); 
				    memset(&pCrmTempNode->stateStartTime, 0, sizeof(time_t)); 
				    memset(&pCrmTempNode->callStateStartTime, 0, sizeof(time_t));
				    memset(&pCrmTempNode->startTime, 0, sizeof(time_t)); 	

					//Reset SIU preSrvTellerId
/*					if(pCrmTempNode->crmInfo.tellerId == AmsSrvData(lpAmsData->srvGrpId).preSrvTellerId)
					{
						AmsSrvData(lpAmsData->srvGrpId).preSrvTellerId = 0;
					}
					*/
					/* Del Crm Node from List */
				    Sem_wait(&AmsSrvData(lpAmsData->srvGrpIdPos).crmCtrl);
					lstDelete(&AmsSrvData(lpAmsData->srvGrpIdPos).crmList, (NODE *)pCrmTempNode);
					Sem_post(&AmsSrvData(lpAmsData->srvGrpIdPos).crmCtrl);

					/* Del Crm Node from List */
				    Sem_wait(&AmsSrvData(lpAmsData->srvGrpIdPos).freecrmCtrl);
					lstDelete(&AmsSrvData(lpAmsData->srvGrpIdPos).freecrmList, (NODE *)pCrmTempNode);
					Sem_post(&AmsSrvData(lpAmsData->srvGrpIdPos).freecrmCtrl);

					//release lpAmsData Pid
					AmsReleassPid(lpAmsData->myPid, END);

					CrmNodeFree(pCrmTempNode);

					clearNum++;
					
					if(clearNum >= AMS_MAX_CLERA_NUM_IN_QUEUE)
					{
						return;
					}
					
					continue;
				}
			}
			pCrmNode = (CRM_NODE *)lstNext((NODE *)pCrmNode);
			j ++;		
		}
	}
}


void AmsClearInactiveTerm(int iThreadId, time_t currentTime)
{
	LP_AMS_DATA_t		*lpAmsData = NULL;        //进程数据区指针		
	LP_QUEUE_DATA_t     *lpQueueData = NULL;      //排队进程数据区指针	
	TERM_NODE            *pTermNode = NULL;		
	TERM_NODE            *pTermTempNode = NULL;	
	int                 clearNum = 0;	
	int                 pid = 0;
	int                 i = 0;
	int                 j = 0;

    /* find inactive Vtm Node in process */
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		j = 0;
		pTermNode = (TERM_NODE *)lstFirst(&AmsSrvData(i).termList);
		while(NULL != pTermNode && j < AMS_MAX_TERM_NODES)
		{
#ifdef AMS_TEST_LT
			pTermNode->handshakeTime = currentTime;
#endif
			
			if(currentTime - pTermNode->handshakeTime >= AMS_TERM_CONNECT_CLEAR_INTERVAL)
			{
				dbgprint("Term[%s] %lu : %lu -:%lu Clear!", pTermNode->termInfo.termId, 
					currentTime, pTermNode->handshakeTime, currentTime - pTermNode->handshakeTime);
								
				//杀掉定时器
				AmsKillTermAllTimer(pTermNode, pid);
				
				if(AMS_CUSTOMER_IN_QUEUE != pTermNode->serviceState)
				{
					pid = pTermNode->amsPid & 0xffff;
					if(pid > 0 && pid < LOGIC_PROCESS_SIZE)
					{
						//int reason = AMS_VTM_HANDSHAKE_TIMEOUT_RCAS_REMOTE_COOP_ABORT;
						
						lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
					}
				}
				else //Customer In Queue
				{					
					pid = pTermNode->customerPid & 0xffff; //可以不 & 0xffff
					if(pid > 0 && pid < LOGIC_PROCESS_SIZE)
					{
						lpQueueData=(LP_QUEUE_DATA_t *)ProcessData[pid];

						//AmsSendCmsCallQueueEventInd(lpQueueData,CMS_CALL_EVENT_IND_VTM_OFFLINE_DEQUEUE,NULL);

						/* 杀掉定时器 */
						if(lpQueueData->iTimerId >= 0)
						{
						    AmsQueueKillTimer(pid, &lpQueueData->iTimerId);
							AmsTimerStatProc(T_AMS_CUSTOMER_IN_QUEUE_TIMER, AMS_KILL_TIMER);
						} 
					
						//release lpQueueData Pid
						AmsReleassPid(lpQueueData->myPid, END);
					}					
				}

			    //set Vtm State and State Start Time
				AmsSetTermState(iThreadId, pTermNode, AMS_TERM_STATE_OFFLINE);

				if(AMS_CALL_STATE_NULL != pTermNode->callState)
				{
					//set Vtm Call State and State Start Time
					AmsSetTermCallState(pTermNode, AMS_CALL_STATE_NULL);
				}
				
				//reset amsPid
				pTermNode->amsPid = 0;

				//reset customerPid
				pTermNode->customerPid = 0;
		
				//AmsResultStatProc(AMS_VTM_HANDSHAKE_RESULT, AMS_VTM_HANDSHAKE_CLEAR_INACTIVTE_VTM);

				pTermTempNode = pTermNode;
				pTermNode = (TERM_NODE *)lstNext((NODE *)pTermNode);
				j++;
				
				//update time, not need
					
			    /* Del Vtm Node from List */
			    Sem_wait(&AmsSrvData(i).termCtrl);
				lstDelete(&AmsSrvData(i).termList, (NODE *)pTermTempNode);
				Sem_post(&AmsSrvData(i).termCtrl);

			    TermNodeFree(pTermTempNode);
				
				clearNum++;
				
				if(clearNum >= AMS_MAX_CLERA_NUM_IN_QUEUE)
				{
					return;
				}
				
				continue;				
			}

			pTermNode = (TERM_NODE *)lstNext((NODE *)pTermNode);
			j ++;
		}
	}
}

void AmsClearInactiveCmsCall(int iThreadId, time_t currentTime)
{
	LP_AMS_DATA_t		*lpAmsData = NULL;        //进程数据区指针	
	CRM_NODE            *pCrmNode = NULL;
	CRM_NODE            *pCrmTempNode = NULL;	
	int                 clearNum = 0;
	int                 pid = 0;
	int                 i = 0;
	int                 j = 0;	

    /* find inactive cms call Crm Node in process */
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		j = 0;
		pCrmNode = (CRM_NODE *)lstFirst(&AmsSrvData(i).crmList);
		while(NULL != pCrmNode && j < AMS_MAX_CRM_NODES)
		{	
			//CMS未发送挂机，保护处理
			if(    AMS_CALL_STATE_NULL != pCrmNode->callState
				&& AMS_CRM_STATE_IDLE == pCrmNode->state
				&& currentTime - pCrmNode->stateStartTime >= AMS_CMS_CALL_CLEAR_INTERVAL
				&& currentTime - pCrmNode->callStateStartTime >= AMS_CMS_CALL_CLEAR_INTERVAL)
			{
				pid = pCrmNode->amsPid & 0xffff;
				if(pid > 0 && pid < LOGIC_PROCESS_SIZE)
				{
					lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
					
					dbgprint("Teller[%s] Vtm[%s]CallState[%d] Time[%lu]: [%lu-%lu] [%lu-%lu] Clear!", 
						pCrmNode->crmInfo.tellerId, 
						lpAmsData->termId,
						pCrmNode->callState,
						currentTime, 
						pCrmNode->stateStartTime, currentTime - pCrmNode->stateStartTime,
						pCrmNode->callStateStartTime, currentTime - pCrmNode->callStateStartTime);

					//notify cms to release call
					AmsSendCmsCallEventInd(lpAmsData,AMS_CALL_EVENT_IND_TELLER_ERR,NULL);

					//仅杀掉呼叫相关定时器，包括消息、文件收发
					AmsKillCrmAllCallTimer(lpAmsData, pid);
					
				    //update time
				    memset(&pCrmNode->callStateStartLocalTime, 0, sizeof(TIME_INFO)); 
				    memset(&pCrmNode->callStateStartTime, 0, sizeof(time_t));	
					
					//reset Crm Call State and State Start Time 
					AmsSetCrmCallState(lpAmsData, pCrmNode, AMS_CALL_STATE_NULL);

					AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, AMS_CMS_HANDSHAKE_CLEAR_INACTIVTE_CALL);

					//AmsInsertDbServiceSDR(iThreadId, AMS_SDR_ITEM_BASE, lpAmsData, NULL, 0, 0, NULL);

					//reset sessStat
					//memset(&lpAmsData->sessStat, 0, sizeof(AMS_SESSION_STAT));
					
					pCrmTempNode = pCrmNode;
					pCrmNode = (CRM_NODE *)lstNext((NODE *)pCrmNode);
					j++;

					clearNum++;
					
					if(clearNum >= AMS_MAX_CLERA_NUM_IN_QUEUE)
					{
						return;
					}
					
					continue;
				}

				//amsPid 无效时，保护处理
				if(AMS_CALL_STATE_NULL != pCrmNode->callState)
				{
					pCrmNode->callState = AMS_CALL_STATE_NULL;
				}
			}

			pCrmNode = (CRM_NODE *)lstNext((NODE *)pCrmNode);
			j ++;
			
		}
	}
}

void AmsUpdateCrmWorkInfoProc(time_t currentTime)
{
	CRM_NODE            *pCrmNode = NULL;		
	int                 updateNum = 0;	
	int                 i = 0;
	int                 j = 0;	

    /* update Crm Work Info if Need */
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		j = 0;
		
		pCrmNode = (CRM_NODE *)lstFirst(&AmsSrvData(i).crmList);
		while(NULL != pCrmNode && j < AMS_MAX_CRM_NODES)
		{			
			if(currentTime - pCrmNode->workInfoUpdateTime >= AMS_CRM_WORK_INFO_UPDATE_INTERVAL)
			{		
				if(AMS_OK == AmsUpdateSingleCrmWorkInfo(pCrmNode, currentTime))
				{
					if(pCrmNode->tellerCfgPos < AMS_MAX_CRM_NUM)
					{
						if(AmsTellerStat(pCrmNode->tellerCfgPos).tellerId == pCrmNode->crmInfo.tellerId)
						{
							memcpy(&AmsTellerStat(pCrmNode->tellerCfgPos).crmWorkInfo, &pCrmNode->crmWorkInfo, sizeof(TELLER_WORK_INFO));
						}
					}
					
					updateNum++;

					if(updateNum >= AMS_MAX_UPDATE_WORK_INFO_NUM)
					{
						return;
					}
				}
			}

			pCrmNode = (CRM_NODE *)lstNext((NODE *)pCrmNode);
			j ++;
			
		}
		
	}
	
}


int AmsProcServiceProcMsg(int iThreadId, MESSAGE_t *pMsg)
{

	time_t              currentTime;

	time(&currentTime);	

	//清理不活动的CRM连接，及释放业务处理进程
	AmsClearInactiveCrm(iThreadId, currentTime);

	//清理不活动的Term连接，及释放排队服务进程
	AmsClearInactiveTerm(iThreadId, currentTime);


    //判断是否有等待业务的客户
    //若有，从队列头开始顺序为客户选择柜员；
    //若选择成功，则释放排队服务进程	
    AmsGetCrmInServiceProcTask(iThreadId, currentTime);


	//清理不活动的RCAS服务器
	//AmsClearInactiveRcas(iThreadId, currentTime);

	//清理不活动的CMS呼叫
	AmsClearInactiveCmsCall(iThreadId, currentTime);

	
    //calc teller WorkInfo Every 5 Minutes
	AmsUpdateCrmWorkInfoProc(currentTime);

	return AMS_OK;
	
}

int AmsSendCmsGetCrmTimeoutRsp(LP_QUEUE_DATA_t *lpQueueData,MESSAGE_t *pMsg,int iret)
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
	s_Msg.iMessageType = A_TELLER_GET_RSP;
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
		descrlen=snprintf(description,1024,"send A_TELLER_GET_RSP msg \n");	
		AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_TELLER_GET_RSP",description,
						descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,lpQueueData->sTraceName);				
	}

    AmsMsgStatProc(AMS_CMS_MSG, s_Msg.iMessageType);
	AmsResultStatProc(AMS_CMS_GET_CRM_RESULT, iret);
	
	return SUCCESS;
}


int AmsCustomerInQueueTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg)
{
	int					iret = AMS_CMS_GET_CRM_TIMEOUT;
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
		iret = AMS_CMS_GET_CRM_TIMEOUT_PARA_ERR;
		AmsResultStatProc(AMS_CMS_GET_CRM_RESULT, iret);
		return AMS_ERROR;
	}

	//消息长度检查
	if(pTmMsg->iMessageLength > (PARA_LEN + sizeof(char) + sizeof(int)))
	{
		dbgprint("AmsCustomerInQueueTimeoutProc[%d] Len:%d Err", pid, pTmMsg->iMessageLength);
		iret = AMS_CMS_GET_CRM_TIMEOUT_LEN_ERR;
		AmsResultStatProc(AMS_CMS_GET_CRM_RESULT, iret);
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
		iret = AMS_CMS_GET_CRM_TIMEOUT_PARA_ERR;
		AmsResultStatProc(AMS_CMS_GET_CRM_RESULT, iret);
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
		iret = 	AMS_CMS_GET_CRM_TIMEOUT_TERM_ID_ERR;
		AmsResultStatProc(AMS_CMS_GET_CRM_RESULT, iret);
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
		iret = 	AMS_CMS_GET_CRM_TIMEOUT_TERM_ID_ERR;
		AmsResultStatProc(AMS_CMS_GET_CRM_RESULT, iret);
		return AMS_ERROR;		
	}

    //check state
 	if(AMS_TERM_STATE_BUSY != pTermNode->state)
	{
		dbgprint("AmsCustomerInQueueTimeoutProc[%d] Term[%s][%u]State[%d]Err", 
			pid, lpQueueData->termId, pTermNode->state);
		iret = 	AMS_CMS_GET_CRM_TIMEOUT_STATE_ERR;
		AmsResultStatProc(AMS_CMS_GET_CRM_RESULT, iret);	
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
		iret = 	AMS_CMS_GET_CRM_TIMEOUT_SERVICE_STATE_ERR;
		AmsResultStatProc(AMS_CMS_GET_CRM_RESULT, iret);	
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
		
	//send Cms Crm Get Rsp
	AmsSendCmsGetCrmTimeoutRsp(lpQueueData,&msg,iret);

	//release lpQueueData Pid
	AmsReleassPid(lpQueueData->myPid, END);

	return iret;

}

int AmsClearHeartbeatErrCrm(LP_AMS_DATA_t *lpAmsData,unsigned char tellid[],unsigned char tellidlen)
{
	CRM_NODE            *pCrmNode = NULL;
	CRM_NODE            *pCrmTempNode = NULL;	
	int					pid = 0;	

#ifdef AMS_TEST_LT
	time_t				currentTime;
#endif

	if(lpAmsData == NULL)
	{
		dbgprint("AmsClearHeatbeatErrCrm Err");
		return AMS_ERROR;
	}
	pid = lpAmsData->amsPid & 0xFFFF;

	/*find crm Node*/
	pCrmNode = AmsSearchCrmNode(lpAmsData->srvGrpIdPos, lpAmsData->tellerId,lpAmsData->tellerIdLen);
	if(NULL == pCrmNode)
	{
		dbgprint("AmsClearHeartbeatErrCrm Tellid Err");
		return AMS_ERROR;		
	}

	//calc crm workInfo
	AmsUpdateSingleCrmWorkInfo(pCrmNode, currentTime);

	//set Crm State and State Start Time
	AmsSetCrmState(NULL, lpAmsData, pCrmNode, AMS_CRM_STATE_OFFLINE, 0);

	if(AMS_CALL_STATE_NULL != pCrmNode->callState)
	{
		//set Crm Call State and State Start Time 
		AmsSetCrmCallState(lpAmsData, pCrmNode, AMS_CALL_STATE_NULL);
	}

	//crm失联后，cms vtm 挂机通知失效，提前设置vtm状态
	AmsResetTermState(NULL, lpAmsData->srvGrpIdPos, lpAmsData->termId, lpAmsData->termIdLen);
	
	/* 杀掉定时器 */
	AmsKillCrmAllTimer(lpAmsData, pid);
			
    //update time, used later
    memset(&pCrmNode->stateStartLocalTime, 0, sizeof(TIME_INFO));
    memset(&pCrmNode->callStateStartLocalTime, 0, sizeof(TIME_INFO)); 
    memset(&pCrmNode->stateStartTime, 0, sizeof(time_t)); 
    memset(&pCrmNode->callStateStartTime, 0, sizeof(time_t));
    memset(&pCrmNode->startTime, 0, sizeof(time_t)); 	

	/* Del Crm Node from List */
    Sem_wait(&AmsSrvData(lpAmsData->srvGrpIdPos).crmCtrl);
	lstDelete(&AmsSrvData(lpAmsData->srvGrpIdPos).crmList, (NODE *)pCrmNode);
	Sem_post(&AmsSrvData(lpAmsData->srvGrpIdPos).crmCtrl);

	/* Del Crm Node from freeList */
    Sem_wait(&AmsSrvData(lpAmsData->srvGrpIdPos).freecrmCtrl);
	lstDelete(&AmsSrvData(lpAmsData->srvGrpIdPos).freecrmList, (NODE *)pCrmNode);
	Sem_post(&AmsSrvData(lpAmsData->srvGrpIdPos).freecrmCtrl);

	//release lpAmsData Pid
	AmsReleassPid(lpAmsData->myPid, END);

	CrmNodeFree(pCrmTempNode);

	return AMS_OK;
}

void AmsResetTermState(int iThreadId, unsigned int srvGrpIdPos, unsigned char termId[],unsigned char termIdLen)
{
	LP_QUEUE_DATA_t      *lpQueueData = NULL;    //排队进程数据区指针 	
	TERM_NODE             *pTermNode = NULL;	
	int                  termPid = 0;

	if(srvGrpIdPos >= AMS_MAX_SERVICE_GROUP_NUM)
	{
		return;
	}
	
	pTermNode = AmsSearchTermNode(srvGrpIdPos, termId, termIdLen);	
	if(NULL == pTermNode)
	{
		return;
	}
	
	if(AMS_TERM_STATE_IDLE != pTermNode->state)
	{
	    //set Term State and State Start Time
		AmsSetTermState(iThreadId, pTermNode, AMS_TERM_STATE_IDLE);
	}

	if(AMS_CALL_STATE_NULL != pTermNode->callState)
	{
		//set Vtm Call State and State Start Time
		AmsSetTermCallState(pTermNode, AMS_CALL_STATE_NULL);
	}

	if(AMS_CUSTOMER_SERVICE_NULL != pTermNode->serviceState)
	{
		if(AMS_CUSTOMER_IN_QUEUE == pTermNode->serviceState)
		{
			termPid = pTermNode->customerPid;
			if(termPid > 0 && termPid < LOGIC_PROCESS_SIZE)
			{
				lpQueueData=(LP_QUEUE_DATA_t *)ProcessData[termPid];
				
				/* 杀掉定时器 */
				if(lpQueueData->iTimerId >= 0)
				{
					AmsQueueKillTimer(termPid, &lpQueueData->iTimerId);//或 lpQueueData->myPid.iProcessId
					AmsTimerStatProc(T_AMS_CUSTOMER_IN_QUEUE_TIMER, AMS_KILL_TIMER);
				} 

				//release lpQueueData Pid
				AmsReleassPid(lpQueueData->myPid, END);
			}					
		}
		else
		{
			termPid = pTermNode->amsPid & 0xffff;
			if(termPid > 0 && termPid < LOGIC_PROCESS_SIZE)
			{
				//杀掉定时器
				AmsKillTermAllCallTimer(pTermNode, termPid);
			}
		}
		
		//set Term Service State and State Start Time
		AmsSetTermServiceState(pTermNode, AMS_CUSTOMER_SERVICE_NULL);	
	}

	//reset amsPid
	pTermNode->amsPid = 0;

	//reset customerPid
	pTermNode->customerPid = 0;
	
	return;
}


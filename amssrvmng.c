/******************************************************************
*
* FILE NAME						                           VERSION
*	
*	amssrvmng.c	                                           v1.0.0
*
* DESCRIPTION
*
*	AMS应用管理服务器的业务管理
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

static int 			amsDataInitialised = 0;

static int          freeAmsVtaListInitialled = 0;
pthread_mutex_t     freeAmsVtaListMtx;
LIST 				freeAmsVtaList;
VTA_NODE        	*freeAmsVtaListBufPtr = NULL;

static int          freeAmsVtmListInitialled = 0;
pthread_mutex_t     freeAmsVtmListMtx;
LIST 				freeAmsVtmList;
VTM_NODE            *freeAmsVtmListBufPtr = NULL;

int AmsDataInit()
{
	int i = 0,ret = 0;
	wchar_t param[1024];

	if(0 != amsDataInitialised)
	{
		return AMS_SUCCESS;
	}

	// get Lic data
	AmsGetLicData();
	
    // init free vta list 
	if(AMS_SUCCESS != VtaListInit())
	{
		Display("AmsDataInit:Error-vta nodes error!\r\n");
    	return AMS_ERROR;
	}	    

    // init free vtm list 
	if(AMS_SUCCESS != VtmListInit())
	{
		Display("AmsDataInit:Error-vtm nodes error!\r\n");
    	return AMS_ERROR;
	}

    // init free vta id list 
	if(AMS_SUCCESS != VtaIdListInit())
	{
		Display("AmsDataInit:Error-vta id nodes error!\r\n");
    	return AMS_ERROR;
	}	    

    // init free vtm id list 
	if(AMS_SUCCESS != VtmIdListInit())
	{
		Display("AmsDataInit:Error-vtm id nodes error!\r\n");
    	return AMS_ERROR;
	}

    // init free org id list 
	if(AMS_SUCCESS != OrgIdListInit())
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
#endif

    for (i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
    {	
	    //init sem
	    Sem_init(&AmsSrvData(i).vtaCtrl,0,1);

		Sem_init(&AmsSrvData(i).vtmCtrl,0,1);

//		AmsSrvData(i).serviceState = AMS_SERVICE_INACTIVE;

//		AmsSrvData(i).preSrvTellerId = 0;
		
    }

	/* clear regData */
	memset(&SystemData.AmsPriData.amsRegData, 0, sizeof(AMS_DATA_REGISTER));

	/* clear stat */	
	memset(&SystemData.AmsPriData.amsStat, 0, sizeof(AMS_STAT));
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
	}

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

int VtmListInit()
{
	VTM_NODE            *pNode;
	int                 size;	
	int                 i;
	
	if(0 != freeAmsVtmListInitialled)
	{
		return AMS_SUCCESS;
	}

	lstInit(&freeAmsVtmList);
	
	size = sizeof(VTM_NODE) * AMS_MAX_VTM_NODES;
	freeAmsVtmListBufPtr = (VTM_NODE *)malloc(size);
	pNode = (VTM_NODE *)freeAmsVtmListBufPtr;
	if(NULL == pNode)
	{
		dbgprint("VtmListInit AllocMem[%d]Err\r\n",size);		
		return AMS_ALLOC_MEM_FAILED;
	}
	
	for(i = 0;i < AMS_MAX_VTM_NODES;i++,pNode++)
	{
        lstAdd(&freeAmsVtmList,(NODE *)pNode);
	}
	
    Pthread_mutex_init(&freeAmsVtmListMtx,NULL);
	
	freeAmsVtmListInitialled = 1;
	
	return AMS_SUCCESS;
}

VTM_NODE * VtmNodeGet(void)
{
	VTM_NODE            *pNode;
	
    if(!freeAmsVtmListInitialled)
    {
		dbgprint("VtmNodeGet InitFlag[%d]Err\r\n",freeAmsVtmListInitialled);		
        return NULL;
    }
	
    Pthread_mutex_lock(&freeAmsVtmListMtx);
    pNode = (VTM_NODE *)lstGet(&freeAmsVtmList);
    Pthread_mutex_unlock(&freeAmsVtmListMtx);
	
    return pNode;
}

void VtmNodeFree(VTM_NODE *pNode)
{
    if(!freeAmsVtmListInitialled)
    {
        return;
    }
	
    Pthread_mutex_lock(&freeAmsVtmListMtx);
    lstAdd(&freeAmsVtmList,(NODE *)pNode);
    Pthread_mutex_unlock(&freeAmsVtmListMtx);
}

void AmsClearInactiveVta(int iThreadId, time_t currentTime)
{
	LP_AMS_DATA_t		*lpAmsData = NULL;        //进程数据区指针	
	VTA_NODE            *pVtaNode = NULL;
	VTA_NODE            *pVtaTempNode = NULL;	
	int                 clearNum = 0;
	int                 pid = 0;
	int                 i = 0;
	int                 j = 0;	

    /* find inactive Vta Node in process */
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		j = 0;
		pVtaNode = (VTA_NODE *)lstFirst(&AmsSrvData(i).vtaList);
		while(NULL != pVtaNode && j < AMS_MAX_VTA_NODES)
		{
#ifdef AMS_TEST_LT
			pVtaNode->handshakeTime = currentTime;
#endif
			if(    AMS_TELLER_CALL_TRANSFER_TIMES_ALLOWED <= pVtaNode->callTransferNum 
				&& currentTime - pVtaNode->callTransferFreezeTime >= AMS_VTA_CALL_TRANSFER_FREEZE_TIME)
			{
				//reset callTransferNum
				pVtaNode->callTransferNum = 0;
			}
			
			if(currentTime - pVtaNode->handshakeTime >= AMS_VTA_CONNECT_CLEAR_INTERVAL)
			{
				dbgprint("Teller[%s] %lu : %lu -:%lu Clear!", pVtaNode->vtaInfo.tellerNo, 
					currentTime, pVtaNode->handshakeTime, currentTime - pVtaNode->handshakeTime);
				pid = pVtaNode->amsPid & 0xffff;
				if(pid > 0 && pid < LOGIC_PROCESS_SIZE)
				{
					lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];

					//notify rcas to stop remote coop
					if(lpAmsData->rcasRemoteCoopState != AMS_DESKTOP_SHARE_NULL)
					{
						int reason = AMS_VTA_HANDSHAKE_TIMEOUT_RCAS_REMOTE_COOP_ABORT;
						
						AmsUpdateRemoteCoopState(iThreadId,lpAmsData,AMS_REMOTE_COOP_TARGET_RCAS,VTA_STOP_SCREEN_SHARE,reason);//abort			
						AmsSendRcasEventInd(lpAmsData,AMS_SCREEN_SHARE_ABORT_IND,reason);

						AmsResultStatProc(AMS_VTA_REMOTE_COOPERATIVE_RESULT, reason);
					}

					//notify vtm to stop remote coop
					if(    lpAmsData->vtmRemoteCoopState != AMS_DESKTOP_SHARE_NULL 
						&& lpAmsData->srvGrpId <= AMS_SERVICE_GROUP_ID_MAX
						&& lpAmsData->vtmId != 0 )
					{
						VTM_NODE *pVtmNode = NULL;
						int      reason = AMS_VTA_HANDSHAKE_TIMEOUT_VTM_REMOTE_COOP_ABORT;
						
						pVtmNode = AmsSearchVtmNode(lpAmsData->srvGrpId, lpAmsData->vtmId);		
						if(NULL != pVtmNode)
						{
							if(pVtmNode->state < AMS_VTM_STATE_OFFLINE)
							{
								AmsUpdateRemoteCoopState(iThreadId,lpAmsData,AMS_REMOTE_COOP_TARGET_VTM,VTA_STOP_SCREEN_SHARE,reason);//abort
								AmsSendVtmEventInd(pVtmNode,AMS_OPERATE_ORIGIN_RSVD,0,AMS_VTM_SCREEN_SHARE_ABORT,reason);

								AmsResultStatProc(AMS_VTA_REMOTE_COOPERATIVE_RESULT, reason);
							}		
						}
					}
										
					//notify cms to release call
					if(    pVtaNode->callState > AMS_CALL_STATE_NULL 
						&& pVtaNode->callState < AMS_CALL_STATE_RSVD)
					{
						AmsSendCmsCallEventInd(lpAmsData,CMS_CALL_EVENT_IND_VTA_OFFLINE,NULL);
					}

				    //calc vta workInfo
					AmsUpdateSingleVtaWorkInfo(pVtaNode, currentTime);
	
					//set Vta State and State Start Time
					AmsSetVtaState(iThreadId, lpAmsData, pVtaNode, AMS_VTA_STATE_OFFLINE, 0);

					if(AMS_CALL_STATE_NULL != pVtaNode->callState)
					{
						//set Vta Call State and State Start Time 
						AmsSetVtaCallState(lpAmsData, pVtaNode, AMS_CALL_STATE_NULL);
					}

					//vta失联后，cms vtm 挂机通知失效，提前设置vtm状态
					AmsResetVtmState(iThreadId, lpAmsData->srvGrpId, lpAmsData->vtmId);
					
					AmsResultStatProc(AMS_VTA_HANDSHAKE_RESULT, AMS_VTA_HANDSHAKE_CLEAR_INACTIVTE_TELLER);

					pVtaTempNode = pVtaNode;
					pVtaNode = (VTA_NODE *)lstNext((NODE *)pVtaNode);
					j++;
					
					/* 杀掉定时器 */
					AmsKillVtaAllTimer(lpAmsData, pid);
							
				    //update time, used later
				    memset(&pVtaTempNode->stateStartLocalTime, 0, sizeof(TIME_INFO));
				    memset(&pVtaTempNode->callStateStartLocalTime, 0, sizeof(TIME_INFO)); 
				    memset(&pVtaTempNode->stateStartTime, 0, sizeof(time_t)); 
				    memset(&pVtaTempNode->callStateStartTime, 0, sizeof(time_t));
				    memset(&pVtaTempNode->startTime, 0, sizeof(time_t)); 	

					//Reset SIU preSrvTellerId
/*					if(pVtaTempNode->vtaInfo.tellerId == AmsSrvData(lpAmsData->srvGrpId).preSrvTellerId)
					{
						AmsSrvData(lpAmsData->srvGrpId).preSrvTellerId = 0;
					}
					*/
					/* Del Vta Node from List */
				    Sem_wait(&AmsSrvData(lpAmsData->srvGrpId).vtaCtrl);
					lstDelete(&AmsSrvData(lpAmsData->srvGrpId).vtaList, (NODE *)pVtaTempNode);
					Sem_post(&AmsSrvData(lpAmsData->srvGrpId).vtaCtrl);

					//release lpAmsData Pid
					AmsReleassPid(lpAmsData->myPid, END);

					VtaNodeFree(pVtaTempNode);

					clearNum++;
					
					if(clearNum >= AMS_MAX_CLERA_NUM_IN_QUEUE)
					{
						return;
					}
					
					continue;
				}
			}

			pVtaNode = (VTA_NODE *)lstNext((NODE *)pVtaNode);
			j ++;
			
		}
	}
}


void AmsClearInactiveVtm(int iThreadId, time_t currentTime)
{
	LP_AMS_DATA_t		*lpAmsData = NULL;        //进程数据区指针		
	LP_QUEUE_DATA_t     *lpQueueData = NULL;      //排队进程数据区指针	
	VTM_NODE            *pVtmNode = NULL;		
	VTM_NODE            *pVtmTempNode = NULL;	
	int                 clearNum = 0;	
	int                 pid = 0;
	int                 i = 0;
	int                 j = 0;

    /* find inactive Vtm Node in process */
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		j = 0;
		pVtmNode = (VTM_NODE *)lstFirst(&AmsSrvData(i).vtmList);
		while(NULL != pVtmNode && j < AMS_MAX_VTM_NODES)
		{
#ifdef AMS_TEST_LT
			pVtmNode->handshakeTime = currentTime;
#endif
			
			if(currentTime - pVtmNode->handshakeTime >= AMS_VTM_CONNECT_CLEAR_INTERVAL)
			{
				dbgprint("Vtm[%s] %lu : %lu -:%lu Clear!", pVtmNode->vtmInfo.vtmNo, 
					currentTime, pVtmNode->handshakeTime, currentTime - pVtmNode->handshakeTime);
								
				//杀掉定时器
				AmsKillVtmAllTimer(pVtmNode, pid);
				
				if(AMS_CUSTOMER_IN_QUEUE != pVtmNode->serviceState)
				{
					pid = pVtmNode->amsPid & 0xffff;
					if(pid > 0 && pid < LOGIC_PROCESS_SIZE)
					{
						int reason = AMS_VTM_HANDSHAKE_TIMEOUT_RCAS_REMOTE_COOP_ABORT;
						
						lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];

						//notify rcas to stop remote coop
						if(lpAmsData->rcasRemoteCoopState != AMS_DESKTOP_SHARE_NULL)
						{
							AmsUpdateRemoteCoopState(iThreadId,lpAmsData,AMS_REMOTE_COOP_TARGET_RCAS,VTA_STOP_SCREEN_SHARE,reason);//abort			
							AmsSendRcasEventInd(lpAmsData,AMS_SCREEN_SHARE_ABORT_IND,reason);

							AmsResultStatProc(AMS_VTA_REMOTE_COOPERATIVE_RESULT, reason);
						}

						//notify Vta to stop remote coop, and reset vtm remote coop data
						if(lpAmsData->vtmRemoteCoopState != AMS_DESKTOP_SHARE_NULL)
						{
							AmsUpdateRemoteCoopState(iThreadId,lpAmsData,AMS_REMOTE_COOP_TARGET_VTM,VTA_STOP_SCREEN_SHARE,reason);//abort
							
							AmsSendTellerEventInd(lpAmsData,AMS_OPERATE_ORIGIN_RSVD,0,AMS_TELLER_SCREEN_SHARE_ABORT,reason);

							AmsResultStatProc(AMS_VTA_REMOTE_COOPERATIVE_RESULT, reason);
						}
						
						//notify cms to release call
						if(    pVtmNode->callState > AMS_CALL_STATE_NULL 
							&& pVtmNode->callState < AMS_CALL_STATE_RSVD)
						{
							AmsSendCmsCallEventInd(lpAmsData,CMS_CALL_EVENT_IND_VTM_OFFLINE,NULL);
						}
					}
				}
				else //Customer In Queue
				{					
					pid = pVtmNode->customerPid & 0xffff; //可以不 & 0xffff
					if(pid > 0 && pid < LOGIC_PROCESS_SIZE)
					{
						lpQueueData=(LP_QUEUE_DATA_t *)ProcessData[pid];

						AmsSendCmsCallQueueEventInd(lpQueueData,CMS_CALL_EVENT_IND_VTM_OFFLINE_DEQUEUE,NULL);

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
				AmsSetVtmState(iThreadId, pVtmNode, AMS_VTM_STATE_OFFLINE);

				if(AMS_CALL_STATE_NULL != pVtmNode->callState)
				{
					//set Vtm Call State and State Start Time
					AmsSetVtmCallState(pVtmNode, AMS_CALL_STATE_NULL);
				}
				
				//reset amsPid
				pVtmNode->amsPid = 0;

				//reset customerPid
				pVtmNode->customerPid = 0;
		
				AmsResultStatProc(AMS_VTM_HANDSHAKE_RESULT, AMS_VTM_HANDSHAKE_CLEAR_INACTIVTE_VTM);

				pVtmTempNode = pVtmNode;
				pVtmNode = (VTM_NODE *)lstNext((NODE *)pVtmNode);
				j++;
				
				//update time, not need
					
			    /* Del Vtm Node from List */
			    Sem_wait(&AmsSrvData(i).vtmCtrl);
				lstDelete(&AmsSrvData(i).vtmList, (NODE *)pVtmTempNode);
				Sem_post(&AmsSrvData(i).vtmCtrl);

			    VtmNodeFree(pVtmTempNode);
				
				clearNum++;
				
				if(clearNum >= AMS_MAX_CLERA_NUM_IN_QUEUE)
				{
					return;
				}
				
				continue;				
			}

			pVtmNode = (VTM_NODE *)lstNext((NODE *)pVtmNode);
			j ++;
		}
	}
}

int AmsClearPreLoginVta(int iThreadId, unsigned int srvGrpId, VTA_NODE *pVtaNode, TERM_NET_INFO termNetInfo)
{
	LP_AMS_DATA_t		 *lpPreAmsData = NULL;      //进程数据区指针
	VTM_NODE             *pVtmNode = NULL;
	MESSAGE_t            preMsg;	
	int                  preVtaPid = 0;	
	int                  reason = AMS_ERROR;
	time_t               currentTime;

	if(NULL == pVtaNode)
	{
		return AMS_ERROR;
	}
	
	preVtaPid = pVtaNode->amsPid&0xffff;
	
	if(preVtaPid > 0 && preVtaPid < LOGIC_PROCESS_SIZE)
	{
		lpPreAmsData=(LP_AMS_DATA_t *)ProcessData[preVtaPid];

		if(    lpPreAmsData->srvGrpId != srvGrpId
			|| srvGrpId > AMS_SERVICE_GROUP_ID_MAX)
		{
			return AMS_ERROR;
		}
		
		//notify rcas to stop remote coop
		if(lpPreAmsData->rcasRemoteCoopState != AMS_DESKTOP_SHARE_NULL)
		{
			reason = AMS_VTA_FORCE_LOGIN_RCAS_REMOTE_COOP_ABORT;
			
			AmsUpdateRemoteCoopState(iThreadId,lpPreAmsData,AMS_REMOTE_COOP_TARGET_RCAS,VTA_STOP_SCREEN_SHARE,reason);//abort			
			AmsSendRcasEventInd(lpPreAmsData,AMS_SCREEN_SHARE_ABORT_IND,reason);

			AmsResultStatProc(AMS_VTA_REMOTE_COOPERATIVE_RESULT, reason);
		}

		//notify vtm to stop remote coop
		if(    lpPreAmsData->vtmRemoteCoopState != AMS_DESKTOP_SHARE_NULL 
			&& lpPreAmsData->srvGrpId <= AMS_SERVICE_GROUP_ID_MAX
			&& lpPreAmsData->vtmId != 0 )
		{
			
			reason = AMS_VTA_FORCE_LOGIN_VTM_REMOTE_COOP_ABORT;
			
			pVtmNode = AmsSearchVtmNode(lpPreAmsData->srvGrpId, lpPreAmsData->vtmId);		
			if(NULL != pVtmNode)
			{
				if(pVtmNode->state < AMS_VTM_STATE_OFFLINE)
				{
					AmsUpdateRemoteCoopState(iThreadId,lpPreAmsData,AMS_REMOTE_COOP_TARGET_VTM,VTA_STOP_SCREEN_SHARE,reason);//abort
					AmsSendVtmEventInd(pVtmNode,AMS_OPERATE_ORIGIN_RSVD,0,AMS_VTM_SCREEN_SHARE_ABORT,reason);

					AmsResultStatProc(AMS_VTA_REMOTE_COOPERATIVE_RESULT, reason);
				}		
			}
		}
		
		//notify cms to release call
		if(    pVtaNode->callState > AMS_CALL_STATE_NULL 
			&& pVtaNode->callState < AMS_CALL_STATE_RSVD)
		{
			AmsSendCmsCallEventInd(lpPreAmsData,CMS_CALL_EVENT_IND_VTA_FORCE_LOGIN_OFFLINE,NULL);
		}
		
		//柜员状态检查
		if(pVtaNode->state < AMS_VTA_STATE_OFFLINE)
		{	
			time(&currentTime);	
			
			if(pVtaNode->tellerRegPos < AMS_MAX_VTA_NUM)
			{
				if(    AMS_TELLER_REGISTER == AmsRegTeller(pVtaNode->tellerRegPos).flag
					&& AmsRegTeller(pVtaNode->tellerRegPos).tellerId == pVtaNode->vtaInfo.tellerId
					&& AmsRegTeller(pVtaNode->tellerRegPos).vtaIp != termNetInfo.ip)
				{
					memset(&preMsg,0,sizeof(MESSAGE_t));
					memcpy(&preMsg.s_ReceiverPid, &lpPreAmsData->myPid,sizeof(PID_t));	   //amsPid
					memcpy(&preMsg.s_SenderPid, &lpPreAmsData->rPid,sizeof(PID_t));        //vtaPid	

					//send State Operate Ind to VTA
				    AmsSendVtaStateOperateInd(lpPreAmsData,&preMsg,VTA_STATE_OPERATE_IND_OFFLINE);						
				}
			}
	
		    //calc vta workInfo	   
			AmsUpdateSingleVtaWorkInfo(pVtaNode, currentTime);

			//set Vta State and State Start Time
			AmsSetVtaState(iThreadId, lpPreAmsData, pVtaNode, AMS_VTA_STATE_OFFLINE, 0);	
			
		}	

		if(AMS_CALL_STATE_NULL != pVtaNode->callState)
		{
			//set Vta Call State and State Start Time 
			AmsSetVtaCallState(lpPreAmsData, pVtaNode, AMS_CALL_STATE_NULL);
		}

		//vta登出后，cms vtm 挂机通知失效，提前设置vtm状态
		AmsResetVtmState(iThreadId, lpPreAmsData->srvGrpId, lpPreAmsData->vtmId);

		/* 杀掉定时器 */
		AmsKillVtaAllTimer(lpPreAmsData, preVtaPid);	
		
		//update time
	    memset(&pVtaNode->stateStartLocalTime, 0, sizeof(TIME_INFO));
	    memset(&pVtaNode->callStateStartLocalTime, 0, sizeof(TIME_INFO)); 
	    memset(&pVtaNode->stateStartTime, 0, sizeof(time_t)); 
	    memset(&pVtaNode->callStateStartTime, 0, sizeof(time_t));
	    memset(&pVtaNode->startTime, 0, sizeof(time_t));

		//Reset SIU preSrvTellerId
/*		if(pVtaNode->vtaInfo.tellerId == AmsSrvData(lpPreAmsData->srvGrpId).preSrvTellerId)
		{
			AmsSrvData(lpPreAmsData->srvGrpId).preSrvTellerId = 0;
		}
			*/		
	    /* Del Vta Node from List */
	    Sem_wait(&AmsSrvData(lpPreAmsData->srvGrpId).vtaCtrl);
		lstDelete(&AmsSrvData(lpPreAmsData->srvGrpId).vtaList, (NODE *)pVtaNode);
		Sem_post(&AmsSrvData(lpPreAmsData->srvGrpId).vtaCtrl);

		//release lpAmsData Pid
		AmsReleassPid(lpPreAmsData->myPid, END);

	} 
	
	VtaNodeFree(pVtaNode);	

	return AMS_OK;
}

//termNetInfo not used yet
int AmsClearPreLoginVtm(int iThreadId, unsigned int srvGrpId, VTM_NODE *pVtmNode, TERM_NET_INFO termNetInfo)
{
	LP_AMS_DATA_t		 *lpPreAmsData = NULL;      //进程数据区指针
	LP_QUEUE_DATA_t      *lpPreQueueData = NULL;    //排队进程数据区指针 	
	int                  preVtmPid = 0;
	int                  reason = AMS_ERROR;

	if(NULL == pVtmNode)
	{
		return AMS_ERROR;	
	}

	if(srvGrpId > AMS_SERVICE_GROUP_ID_MAX)
	{
		return AMS_ERROR;			
	}
	
	/* 杀掉定时器 */
	AmsKillVtmAllTimer(pVtmNode, pVtmNode->amsPid&0xffff);
	
	if(AMS_CUSTOMER_IN_QUEUE != pVtmNode->serviceState)
	{
		preVtmPid = pVtmNode->amsPid & 0xffff;
		if(preVtmPid > 0 && preVtmPid < LOGIC_PROCESS_SIZE)
		{
			lpPreAmsData=(LP_AMS_DATA_t *)ProcessData[preVtmPid];

			//notify rcas to stop remote coop
			if(lpPreAmsData->rcasRemoteCoopState != AMS_DESKTOP_SHARE_NULL)
			{
				reason = AMS_VTM_FORCE_LOGIN_RCAS_REMOTE_COOP_ABORT;
				
				AmsUpdateRemoteCoopState(iThreadId,lpPreAmsData,AMS_REMOTE_COOP_TARGET_RCAS,VTA_STOP_SCREEN_SHARE,reason);//abort			
				AmsSendRcasEventInd(lpPreAmsData,AMS_SCREEN_SHARE_ABORT_IND,reason);

				AmsResultStatProc(AMS_VTA_REMOTE_COOPERATIVE_RESULT, reason);
			}

			//notify Vta to stop remote coop, and reset vtm remote coop data
			if(lpPreAmsData->vtmRemoteCoopState != AMS_DESKTOP_SHARE_NULL)
			{
				reason = AMS_VTM_FORCE_LOGIN_VTA_REMOTE_COOP_ABORT;

				AmsUpdateRemoteCoopState(iThreadId,lpPreAmsData,AMS_REMOTE_COOP_TARGET_VTM,VTA_STOP_SCREEN_SHARE,reason);//abort
				
				AmsSendTellerEventInd(lpPreAmsData,AMS_OPERATE_ORIGIN_RSVD,0,AMS_TELLER_SCREEN_SHARE_ABORT,reason);

				AmsResultStatProc(AMS_VTA_REMOTE_COOPERATIVE_RESULT, reason);
			}
			
			//notify cms to release call
			if(    pVtmNode->callState > AMS_CALL_STATE_NULL 
				&& pVtmNode->callState < AMS_CALL_STATE_RSVD)
			{
				AmsSendCmsCallEventInd(lpPreAmsData,CMS_CALL_EVENT_IND_VTM_FORCE_LOGIN_OFFLINE,NULL);
			}
		}
	}
	else //Customer In Queue
	{					
		preVtmPid = pVtmNode->customerPid & 0xffff; //可以不 & 0xffff
		if(preVtmPid > 0 && preVtmPid < LOGIC_PROCESS_SIZE)
		{
			lpPreQueueData=(LP_QUEUE_DATA_t *)ProcessData[preVtmPid];

			AmsSendCmsCallQueueEventInd(lpPreQueueData,CMS_CALL_EVENT_IND_VTM_FORCE_LOGIN_DEQUEUE,NULL);

			/* 杀掉定时器 */
			if(lpPreQueueData->iTimerId >= 0)
			{
			    AmsQueueKillTimer(preVtmPid, &lpPreQueueData->iTimerId);
				AmsTimerStatProc(T_AMS_CUSTOMER_IN_QUEUE_TIMER, AMS_KILL_TIMER);
			} 
		
			//release lpQueueData Pid
			AmsReleassPid(lpPreQueueData->myPid, END);
		}					
	}
	
	//柜员机状态检查
	if(pVtmNode->state < AMS_VTM_STATE_OFFLINE)
	{
	    //set Vtm State and State Start Time
		AmsSetVtmState(iThreadId, pVtmNode, AMS_VTM_STATE_OFFLINE);
	}
	
	if(AMS_CALL_STATE_NULL != pVtmNode->callState)
	{
		//set Vtm Call State and State Start Time
		AmsSetVtmCallState(pVtmNode, AMS_CALL_STATE_NULL);
	}

	//reset amsPid
	pVtmNode->amsPid = 0;

	//reset customerPid
	pVtmNode->customerPid = 0;
	
    /* Del Vtm Node from List */
    Sem_wait(&AmsSrvData(srvGrpId).vtmCtrl);
	lstDelete(&AmsSrvData(srvGrpId).vtmList, (NODE *)pVtmNode);
	Sem_post(&AmsSrvData(srvGrpId).vtmCtrl);

    VtmNodeFree(pVtmNode);	

	return AMS_OK;
}

void AmsClearInactiveRcas(int iThreadId, time_t currentTime)
{
	LP_AMS_DATA_t		*lpAmsData = NULL;        //进程数据区指针		
	VTA_NODE            *pVtaNode = NULL;	
	VTM_NODE            *pVtmNode = NULL;
	int                 clearNum = 0;
	int                 reason = AMS_ERROR;
	int                 pid = 0;
	int                 i = 0;
	int                 j = 0;
	int                 k = 0;
		
    /* find inactive Rcas Module in process */
	for(k = 0; k < AMS_MAX_RCAS_NUM; k++)
	{

#ifdef AMS_TEST_LT
		AmsRcasMngData(k).handshakeTime = currentTime;
#endif

		if(    currentTime - AmsRcasMngData(k).handshakeTime < AMS_RCAS_CONNECT_CLEAR_INTERVAL
			|| AMS_RCAS_ACTIVE != AmsRcasMngData(k).state
			|| 1 != AmsRcasMngData(k).rcasInfo.flag)
		{
			continue;
		}
		
		dbgprint("Rcas[%d] %lu : %lu -:%lu Clear!", 
			AmsRcasMngData(k).rcasPid.cModuleId, 
			currentTime, AmsRcasMngData(k).handshakeTime, 
			currentTime - AmsRcasMngData(k).handshakeTime);

		//update rcas state
		AmsRcasMngData(k).state = AMS_RCAS_INACTIVE;
		
		AmsResultStatProc(AMS_RCAS_HANDSHAKE_RESULT, AMS_RCAS_HANDSHAKE_CLEAR_INACTIVTE_RCAS);

#if 1   //根据实测情况，决定是否需要保留
	    /* find active remote coop Vta Node in process */
		for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
		{
			j = 0;
			pVtaNode = (VTA_NODE *)lstFirst(&AmsSrvData(i).vtaList);
			while(NULL != pVtaNode && j < AMS_MAX_VTA_NODES)
			{
				pid = pVtaNode->amsPid & 0xffff;
				if(pid > 0 && pid < LOGIC_PROCESS_SIZE)
				{
					lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];

					if(   lpAmsData->rcasPos != k 
					   || lpAmsData->rcasPid.cModuleId != AmsRcasMngData(k).rcasPid.cModuleId)
					{
						pVtaNode = (VTA_NODE *)lstNext((NODE *)pVtaNode);
						j++;

						continue;
					}

					//notify rcas to stop remote coop
					if(lpAmsData->rcasRemoteCoopState != AMS_DESKTOP_SHARE_NULL)
					{
						reason = AMS_RCAS_HANDSHAKE_TIMEOUT_VTA_REMOTE_COOP_ABORT;

						/* 杀掉定时器 */
						if(lpAmsData->rcasRemoteCoopTimerId >= 0)
						{
							AmsKillTimer(pid, &lpAmsData->rcasRemoteCoopTimerId);
							AmsTimerStatProc(T_AMS_RCAS_REMOTE_COOP_TIMER, AMS_KILL_TIMER);
						} 
						
						AmsUpdateRemoteCoopState(iThreadId,lpAmsData,AMS_REMOTE_COOP_TARGET_RCAS,VTA_STOP_SCREEN_SHARE,reason);//abort			
						AmsSendRcasEventInd(lpAmsData,AMS_SCREEN_SHARE_ABORT_IND,reason);

						AmsResultStatProc(AMS_VTA_REMOTE_COOPERATIVE_RESULT, reason);
					}

					//notify vtm to stop remote coop
					if(    lpAmsData->vtmRemoteCoopState != AMS_DESKTOP_SHARE_NULL 
						&& lpAmsData->srvGrpId <= AMS_SERVICE_GROUP_ID_MAX
						&& lpAmsData->vtmId != 0 )
					{
						reason = AMS_RCAS_HANDSHAKE_TIMEOUT_VTM_REMOTE_COOP_ABORT;
						
						pVtmNode = AmsSearchVtmNode(lpAmsData->srvGrpId, lpAmsData->vtmId);		
						if(NULL != pVtmNode)
						{
							if(pVtmNode->state < AMS_VTM_STATE_OFFLINE)
							{
								/* 杀掉定时器 */
								if(lpAmsData->vtmRemoteCoopTimerId >= 0)
								{
									AmsKillTimer(pid, &lpAmsData->vtmRemoteCoopTimerId);
									AmsTimerStatProc(T_AMS_VTM_REMOTE_COOP_TIMER, AMS_KILL_TIMER);
								} 
								
								AmsUpdateRemoteCoopState(iThreadId,lpAmsData,AMS_REMOTE_COOP_TARGET_VTM,VTA_STOP_SCREEN_SHARE,reason);//abort
								AmsSendVtmEventInd(pVtmNode,AMS_OPERATE_ORIGIN_RSVD,0,AMS_VTM_SCREEN_SHARE_ABORT,reason);

								AmsResultStatProc(AMS_VTA_REMOTE_COOPERATIVE_RESULT, reason);
							}		
						}
					}
		
					pVtaNode = (VTA_NODE *)lstNext((NODE *)pVtaNode);
					j++;

					clearNum++;
					
					if(clearNum >= AMS_MAX_CLERA_NUM_IN_QUEUE)
					{
						return;
					}
					
					continue;
				}

				pVtaNode = (VTA_NODE *)lstNext((NODE *)pVtaNode);
				j ++;
			}
		}	
#endif		
	}
}

void AmsClearInactiveCmsCall(int iThreadId, time_t currentTime)
{
	LP_AMS_DATA_t		*lpAmsData = NULL;        //进程数据区指针	
	VTA_NODE            *pVtaNode = NULL;
	VTA_NODE            *pVtaTempNode = NULL;	
	int                 clearNum = 0;
	int                 pid = 0;
	int                 i = 0;
	int                 j = 0;	

    /* find inactive cms call Vta Node in process */
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		j = 0;
		pVtaNode = (VTA_NODE *)lstFirst(&AmsSrvData(i).vtaList);
		while(NULL != pVtaNode && j < AMS_MAX_VTA_NODES)
		{	
			//CMS未发送挂机，保护处理
			if(    AMS_CALL_STATE_NULL != pVtaNode->callState
				&& AMS_VTA_STATE_IDLE == pVtaNode->state
				&& currentTime - pVtaNode->stateStartTime >= AMS_CMS_CALL_CLEAR_INTERVAL
				&& currentTime - pVtaNode->callStateStartTime >= AMS_CMS_CALL_CLEAR_INTERVAL)
			{
				pid = pVtaNode->amsPid & 0xffff;
				if(pid > 0 && pid < LOGIC_PROCESS_SIZE)
				{
					lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
					
					dbgprint("Teller[%s] Vtm[%s]CallState[%d] Time[%lu]: [%lu-%lu] [%lu-%lu] Clear!", 
						pVtaNode->vtaInfo.tellerNo, 
						lpAmsData->vtmNo,
						pVtaNode->callState,
						currentTime, 
						pVtaNode->stateStartTime, currentTime - pVtaNode->stateStartTime,
						pVtaNode->callStateStartTime, currentTime - pVtaNode->callStateStartTime);

					//notify cms to release call
					AmsSendCmsCallEventInd(lpAmsData,CMS_CALL_EVENT_IND_VTA_CALL_STATE_ABORT,NULL);

					//仅杀掉呼叫相关定时器，包括消息、文件收发
					AmsKillVtaAllCallTimer(lpAmsData, pid);
					
				    //update time
				    memset(&pVtaNode->callStateStartLocalTime, 0, sizeof(TIME_INFO)); 
				    memset(&pVtaNode->callStateStartTime, 0, sizeof(time_t));	
					
					//reset Vta Call State and State Start Time 
					AmsSetVtaCallState(lpAmsData, pVtaNode, AMS_CALL_STATE_NULL);

					AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, AMS_CMS_HANDSHAKE_CLEAR_INACTIVTE_CALL);

					AmsInsertDbServiceSDR(iThreadId, AMS_SDR_ITEM_BASE, lpAmsData, NULL, 0, 0, NULL);

					//reset sessStat
					memset(&lpAmsData->sessStat, 0, sizeof(AMS_SESSION_STAT));
					
					pVtaTempNode = pVtaNode;
					pVtaNode = (VTA_NODE *)lstNext((NODE *)pVtaNode);
					j++;

					clearNum++;
					
					if(clearNum >= AMS_MAX_CLERA_NUM_IN_QUEUE)
					{
						return;
					}
					
					continue;
				}

				//amsPid 无效时，保护处理
				if(AMS_CALL_STATE_NULL != pVtaNode->callState)
				{
					pVtaNode->callState = AMS_CALL_STATE_NULL;
				}
			}

			pVtaNode = (VTA_NODE *)lstNext((NODE *)pVtaNode);
			j ++;
			
		}
	}
}

void AmsResetVtmState(int iThreadId, unsigned int srvGrpId, unsigned int vtmId)
{
	LP_QUEUE_DATA_t      *lpQueueData = NULL;    //排队进程数据区指针 	
	VTM_NODE             *pVtmNode = NULL;	
	int                  vtmPid = 0;

	if(srvGrpId >= AMS_MAX_SERVICE_GROUP_NUM || 0 == vtmId)
	{
		return;
	}
	
	pVtmNode = AmsSearchVtmNode(srvGrpId, vtmId);	
	if(NULL == pVtmNode)
	{
		return;
	}
	
	if(AMS_VTM_STATE_IDLE != pVtmNode->state)
	{
	    //set Vtm State and State Start Time
		AmsSetVtmState(iThreadId, pVtmNode, AMS_VTM_STATE_IDLE);
	}

	if(AMS_CALL_STATE_NULL != pVtmNode->callState)
	{
		//set Vtm Call State and State Start Time
		AmsSetVtmCallState(pVtmNode, AMS_CALL_STATE_NULL);
	}

	if(AMS_CUSTOMER_SERVICE_NULL != pVtmNode->serviceState)
	{
		if(AMS_CUSTOMER_IN_QUEUE == pVtmNode->serviceState)
		{
			vtmPid = pVtmNode->customerPid;
			if(vtmPid > 0 && vtmPid < LOGIC_PROCESS_SIZE)
			{
				lpQueueData=(LP_QUEUE_DATA_t *)ProcessData[vtmPid];
				
				/* 杀掉定时器 */
				if(lpQueueData->iTimerId >= 0)
				{
					AmsQueueKillTimer(vtmPid, &lpQueueData->iTimerId);//或 lpQueueData->myPid.iProcessId
					AmsTimerStatProc(T_AMS_CUSTOMER_IN_QUEUE_TIMER, AMS_KILL_TIMER);
				} 

				//release lpQueueData Pid
				AmsReleassPid(lpQueueData->myPid, END);
			}					
		}
		else
		{
			vtmPid = pVtmNode->amsPid & 0xffff;
			if(vtmPid > 0 && vtmPid < LOGIC_PROCESS_SIZE)
			{
				//杀掉定时器
				AmsKillVtmAllCallTimer(pVtmNode, vtmPid);
			}
		}
		
		//set Vtm Service State and State Start Time
		AmsSetVtmServiceState(pVtmNode, AMS_CUSTOMER_SERVICE_NULL);	
	}

	//reset amsPid
	pVtmNode->amsPid = 0;

	//reset customerPid
	pVtmNode->customerPid = 0;
	
	return;
}

void AmsResetTellerRegInfo(unsigned int tellerId)
{
	int					i;

	if(0 == tellerId)
	{
		return;	
	}
	
	for(i = 0; i < AMS_MAX_VTA_NUM; i++)
	{
		if(AmsRegTeller(i).flag != AMS_TELLER_REGISTER)
		{
			continue;
		}
		
		if(tellerId == AmsRegTeller(i).tellerId)
		{	
			memset(&AmsRegTeller(i), 0, sizeof(TELLER_REGISTER_INFO));
			//AMS_TELLER_UNREGISTER

			break;			
		}	
	}	
	
}

void AmsResetVtmRegInfo(unsigned int vtmId)
{
	int					i;

	if(0 == vtmId)
	{
		return;	
	}
	
	for(i = 0; i < AMS_MAX_VTM_NUM; i++)
	{
		if(AmsRegVtm(i).flag != AMS_VTM_REGISTER)
		{
			continue;
		}
		
		if(vtmId == AmsRegVtm(i).vtmId)
		{	
			memset(&AmsRegVtm(i), 0, sizeof(VTM_REGISTER_INFO));
			//AMS_VTM_UNREGISTER

			break;			
		}		
	}

}

int OrgIdHashInit()
{
	ORG_ID_NODE          *pNode = NULL;
	int                  i = 0;

	for(i = 0; i < AMS_MAX_ORG_NUM; i++)
	{
		if(AMS_ORG_INSTALL == AmsCfgOrg(i).flag)
		{
		    if (NULL == (pNode = AmsGetOrgIdNode()))
		    {
	          	Display("OrgIdHashInit Get Org Id Node Failed[%d][%d]!\r\n", 
					i, AmsCfgOrg(i).orgId);
				return AMS_ERROR;
		    }
	        pNode->orgId = AmsCfgOrg(i).orgId;
	        pNode->orgPos = i;
	        AmsInsertOrgIdHash(pNode);
		}
	}
	
	return AMS_OK;
}
	
void AmsGetVtaInServiceProcTask(int iThreadId, time_t currentTime)
{
	int					iret = AMS_CMS_PRCOESS_SUCCESS;		
	LP_QUEUE_DATA_t     *lpQueueData = NULL;      //排队进程数据区指针	
	LP_AMS_DATA_t		*lpAmsData = NULL;        //进程数据区指针			
	VTM_NODE            *pVtmNode = NULL;	
	VTA_NODE            *pVtaNode = NULL;
	DWORD               srvGrpId = 0;
	MESSAGE_t           msg;	
	int                 vtmNum = 0;
	int                 procNum = 0;
	int                 pid = 0;	
	int                 i = 0;	
	int                 j = 0;	
	
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		vtmNum = lstCount(&AmsSrvData(i).vtmList);
		vtmNum = Min(vtmNum, AMS_MAX_VTM_NODES);

		if(0 == vtmNum)
		{
			continue;
		}
		
		for(j = 0; j < vtmNum; j++)	
		{
			pVtmNode = AmsGetIdleLongestVtm(i, vtmNum, currentTime);

			if(NULL == pVtmNode)
			{
				break;
			}
			
			iret = AMS_CMS_PRCOESS_SUCCESS;
			
			if(   AMS_CUSTOMER_IN_QUEUE != pVtmNode->serviceState 
			   || 0 == pVtmNode->customerPid 
			   || pVtmNode->customerPid >= LOGIC_PROCESS_SIZE)
			{
				//状态及参数校验错误
				continue;
			}

			lpQueueData=(LP_QUEUE_DATA_t *)ProcessData[pVtmNode->customerPid];

//			dbgprint("AmsServiceIntelligentSelectVta Vtm[%s][%u]SrvGrpId[%u]\r\n",
//				lpQueueData->vtmNo, lpQueueData->vtmId, lpQueueData->srvGrpId);	

			//仅携带业务类型，没有指明业务组
			if (1 == lpQueueData->srvGrpSelfAdapt)
			{
				//根据业务类型选择业务组
				iret = AmsSelectSrvGrpIdByServiceType(lpQueueData->vtmNo,
				                                      lpQueueData->vtmId,
				                                      lpQueueData->serviceType,
				                                      &srvGrpId);
				
				if(AMS_OK == iret && srvGrpId != lpQueueData->srvGrpId)
				{
					//转移业务组
					lpQueueData->srvGrpId = srvGrpId;
					
					//呼叫添加到新业务组的链表尾
				    /* Del Vtm Node from Origin List */
				    Sem_wait(&AmsSrvData(pVtmNode->vtmInfo.srvGrpId).vtmCtrl);
					lstDelete(&AmsSrvData(pVtmNode->vtmInfo.srvGrpId).vtmList, (NODE *)pVtmNode);
					Sem_post(&AmsSrvData(pVtmNode->vtmInfo.srvGrpId).vtmCtrl);

					/* Add Vtm Node to new List */
				    Sem_wait(&AmsSrvData(srvGrpId).vtmCtrl);
					lstAdd(&AmsSrvData(srvGrpId).vtmList, (NODE *)pVtmNode);
					Sem_post(&AmsSrvData(srvGrpId).vtmCtrl);

					//更新当前柜员机的业务组编号
					if(pVtmNode->vtmInfo.srvGrpId != srvGrpId)
					{
						//update srvGrpId
						pVtmNode->vtmInfo.srvGrpId = srvGrpId;
					}		
				}	
			}

			//业务智能路由
			pVtaNode = AmsServiceIntelligentSelectVta(lpQueueData->vtmNo,
													  lpQueueData->vtmId,
			                                          lpQueueData->srvGrpId,
			                                          lpQueueData->serviceType,
			                                          lpQueueData->callType,
			                                          lpQueueData->callTarget,
			                                          pVtmNode->vtmInfo.orgId,
			                                          pVtmNode->orgCfgPos,
			                                          &iret);
			if(NULL == pVtaNode)
			{
				if(AMS_CMS_GET_VTA_SERVICE_IN_QUEUE != iret)
				{
					dbgprint("ServiceProcTask Vtm[%s][%u] GetVta SISelectVta Failed",
						lpQueueData->vtmNo, lpQueueData->vtmId);
					
					if(AMS_ERROR == iret)
					{
						iret = AMS_CMS_GET_VTA_SERVICE_INTELLIGENT_ROUTING_ERR;
					}

					memset(&msg, 0, sizeof(MESSAGE_t));
					
					//pack cmsPid
					msg.s_SenderPid.cModuleId   = pVtmNode->cmsPid.cModuleId;
					msg.s_SenderPid.cFunctionId = pVtmNode->cmsPid.cFunctionId;
					msg.s_SenderPid.iProcessId  = pVtmNode->cmsPid.iProcessId;

					//pack amsPid
					msg.s_ReceiverPid.iProcessId = 0;
					
					//pack callId
					msg.cMessageBody[0] = lpQueueData->callIdLen;
					if(lpQueueData->callIdLen <= AMS_MAX_CALLID_LEN)
					{
						memcpy(&msg.cMessageBody[1], lpQueueData->callId, lpQueueData->callIdLen);	
					}
					AmsSendCmsVtaGetRsp(NULL,&msg,iret,NULL,pVtmNode);	
					
					//update Customer Service State
					AmsSetVtmServiceState(pVtmNode, AMS_CUSTOMER_SERVICE_NULL);
			
					/* 杀掉定时器 */
					if(lpQueueData->iTimerId >= 0)
					{
						AmsQueueKillTimer(pVtmNode->customerPid, &lpQueueData->iTimerId);//或 lpQueueData->myPid.iProcessId
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
				msg.s_SenderPid.cModuleId   = pVtmNode->cmsPid.cModuleId;
				msg.s_SenderPid.cFunctionId = pVtmNode->cmsPid.cFunctionId;
				msg.s_SenderPid.iProcessId  = pVtmNode->cmsPid.iProcessId;

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
					dbgprint("ServiceProcTask Vtm[%s][%u] GetVta Pid:%d Err", 
						lpQueueData->vtmNo, lpQueueData->vtmId, pid);
					
					iret = AMS_CMS_GET_VTA_AMS_PID_ERR;
					AmsSendCmsVtaGetRsp(NULL,&msg,iret,pVtaNode,NULL);

					//update Customer Service State
					AmsSetVtmServiceState(pVtmNode, AMS_CUSTOMER_SERVICE_NULL);
			
					/* 杀掉定时器 */
					if(lpQueueData->iTimerId >= 0)
					{
						AmsQueueKillTimer(pVtmNode->customerPid, &lpQueueData->iTimerId);//或 lpQueueData->myPid.iProcessId
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
				lpAmsData->vtmId = lpQueueData->vtmId;

				//record vtmNo
				if(pVtmNode->vtmInfo.vtmNoLen <= AMS_MAX_VTM_NO_LEN)
				{	
					memset(lpAmsData->vtmNo, 0, (AMS_MAX_VTM_NO_LEN + 1));
					memcpy(lpAmsData->vtmNo, pVtmNode->vtmInfo.vtmNo, pVtmNode->vtmInfo.vtmNoLen);
					lpAmsData->vtmNoLen = pVtmNode->vtmInfo.vtmNoLen;
				}
				
				//record vtmPos
				lpAmsData->vtmPos = pVtmNode->vtmCfgPos;
				
				//Set vta call State, only one pthread!!!
				AmsSetVtaCallState(lpAmsData, pVtaNode, AMS_CALL_STATE_WAIT_ANSWER);

#ifdef AMS_TEST_LT
			    //calc vta workInfo
				AmsUpdateSingleVtaWorkInfo(pVtaNode, currentTime);
		
				//set Vta State and State Start Time
				AmsSetVtaState(iThreadId, lpAmsData, pVtaNode, AMS_VTA_STATE_BUSY, 0);
#endif

				//record vtmPid, may be used later
				memcpy(&lpAmsData->vtmPid,&pVtmNode->rPid,sizeof(PID_t));
				
				//record callId
				lpAmsData->callIdLen = lpQueueData->callIdLen;
				if(lpQueueData->callIdLen <= AMS_MAX_CALLID_LEN)
				{
					memcpy(lpAmsData->callId, lpQueueData->callId, lpQueueData->callIdLen);
				}

				//record amsPid
				pVtmNode->amsPid = pVtaNode->amsPid;

				//update Customer Service State
				AmsSetVtmServiceState(pVtmNode, AMS_CUSTOMER_IN_SERVICE);

				//record cmsPid
				lpAmsData->cmsPid.cModuleId	   = pVtmNode->cmsPid.cModuleId;
				lpAmsData->cmsPid.cFunctionId  = pVtmNode->cmsPid.cFunctionId;
				lpAmsData->cmsPid.iProcessId   = pVtmNode->cmsPid.iProcessId;
				
			}

			//update srvGrpId
//			pVtmNode->vtmInfo.srvGrpId = lpQueueData->srvGrpId;

			//send Vta Get Rsp to CMS
		    AmsSendCmsVtaGetRsp(lpAmsData,&msg,iret,pVtaNode,NULL);

			/* 杀掉定时器 */
			if(lpQueueData->iTimerId >= 0)
			{
				AmsQueueKillTimer(pVtmNode->customerPid, &lpQueueData->iTimerId);//或 lpQueueData->myPid.iProcessId
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

/*
1. 普通客户，仅携带业务类型。
技能组里面的柜员空闲人员比例由高到低路由，空闲率高技能组的优先路由。
（vip组可以配置，不接受普通服务）

1.1 有空闲  选择空闲比 空闲人数/柜员数 最高的；如果有多个业务组，选择第一个
1.2 没有空闲  选择在业务组编号最低的业务组中排队；在不超过排队数阈值的业务组中，选择第一个
1.3 排队中，可以在不同业务组中迁移

2. vip客户，传两参数，指定vip客户的技能组，只有vip客户为他服务。
*/

int AmsSelectSrvGrpIdByServiceType(unsigned char *pVtmNo, 
											 unsigned int vtmId,						                          
											 unsigned int serviceType,
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
	
	//检查柜员机设备号
	if(NULL == pVtmNo)
	{
		if(AmsDebugTrace)	
		{
			dbgprint("AmsSelectSrvGrp VtmId[%u] serviceType[0x%x] VtmNoErr", 
				vtmId, serviceType);
		}
		
		return AMS_CMS_GET_VTA_SIR_VTMNO_ERR;		
	}
	
	//检查业务类型
	if(0 == serviceType)
	{
		if(AmsDebugTrace)	
		{
			dbgprint("AmsSelectSrvGrp Vtm[%s][%u] serviceType[0x%x]Err", 
				pVtmNo, vtmId, serviceType);
		}
		
		return AMS_CMS_GET_VTA_SERVICE_TYPE_ERR;	
	}

	//选择业务组
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		//找到配置此业务类型的业务组
		if(AmsCfgSrvGroup(i).flag != AMS_SERVICE_GROUP_INSTALL)
		{
			continue;
		}

		//检查业务类型
	    if(AMS_OK != AmsCheckTellerServiceAbility(serviceType, AmsCfgSrvGroup(i).serviceType))//逐位比较
	    {		
			continue;
	    }

		//检查业务组编号
		if(    0 == AmsCfgSrvGroup(i).srvGrpId
			|| AmsCfgSrvGroup(i).srvGrpId > AMS_SERVICE_GROUP_ID_MAX)
		{
			if(AmsDebugTrace)	
			{
				dbgprint("AmsSelectSrvGrp Vtm[%s][%u] SrvGrpId[%d][%u]Err", 
					pVtmNo, vtmId, i, AmsCfgSrvGroup(i).srvGrpId);
			}
			
			continue;	
		}

		//检查业务组属性
		if(1 == AmsCfgSrvGroup(i).isHighLevel)
		{			
			continue;	
		}

		//临时存储业务组编号
		srvGrpId = AmsCfgSrvGroup(i).srvGrpId;
		
		//检查当前业务组柜员数量
		vtaNum = lstCount(&AmsSrvData(srvGrpId).vtaList);

		if(0 == vtaNum)
		{
			continue;		
		}

		//检查当前业务组空闲柜员数量
		idleVtaNum = 0;
		j = 0;
		pVtaNode = (VTA_NODE *)lstFirst(&AmsSrvData(srvGrpId).vtaList);
		while(NULL != pVtaNode && j < AMS_MAX_VTA_NODES)
		{	
	        if(    AMS_VTA_STATE_IDLE == pVtaNode->state
				&& AMS_CALL_STATE_NULL == pVtaNode->callState)
	        {
				idleVtaNum++;			
	        }
			
	        pVtaNode = (VTA_NODE *)lstNext((NODE *)pVtaNode);
			j++;
	    }

		//当前业务组有空闲柜员
		if(0 != idleVtaNum)
		{
			//比较柜员空闲率
			idleRate = (idleVtaNum*AMS_VTA_IDLE_RATE_ENLARGE_NUM)/vtaNum;
			if(idleRate > idleRateMax)
			{
				idleRateMax = idleRate;

				//初步选中业务组
				srvGrpSelect = srvGrpId;

				if(AmsDebugTrace)	
				{
					dbgprint("AmsSelectSrvGrp Vtm[%s][%u] would be SrvGrp[%u]", 
						pVtmNo, vtmId, srvGrpId);
				}				
			}
		}
		else //若没有空闲柜员
		{
			//检查当前业务组的客户排队长度
			memset(&queueInfo,0,sizeof(QUEUE_INFO));
			if(AMS_OK != AmsCustCalcSrvGrpIdQueueInfo(vtmId, srvGrpId, &queueInfo))
			{
				if(AmsDebugTrace)	
				{
					dbgprint("AmsSelectSrvGrp Vtm[%s][%u] CalcSrvGrp[%u]QueueInfoErr", 
						pVtmNo, vtmId, srvGrpId);
				}
				
				continue;			
			}

			for(k = 0; k < AMS_MAX_SERVICE_GROUP_NUM; k++)
			{
				if(    AMS_QUEUE_CFG == AmsCfgQueueSys(k).flag 
					&& AmsCfgQueueSys(k).srvGrpId == srvGrpId)//重复~
				{
					//已经达到当前业务组允许的最大排队长度
					if(queueInfo.queuingLen >= AmsCfgQueueSys(k).maxQueLen)
					{
						break;							
					}
					else
					{
						if(0 == srvGrpSelect)
						{
							//初步选中业务组
							srvGrpSelect = srvGrpId;

							if(AmsDebugTrace)	
							{
								dbgprint("AmsSelectSrvGrp Vtm[%s][%u] would be SrvGrp[%u].", 
									pVtmNo, vtmId, srvGrpId);
							}	
						}
					}
				}
			}
			
		}

	}

	if(0 == srvGrpSelect)
	{
		if(AmsDebugTrace)	
		{
			dbgprint("AmsVtm[%s][%u] serviceType[0x%x] SelectSrvGrp Fail", 
				pVtmNo, vtmId, serviceType);
		}
		
		return AMS_CMS_GET_VTA_NO_VALID_SERVICE_GROUP_ID_OR_TYPE;
	}

	//选中业务组
	*pSrvGrpId = srvGrpSelect;

	if(AmsDebugTrace)	
	{
		dbgprint("AmsSelectSrvGrp Vtm[%s][%u] is SrvGrp[%u]", 
			pVtmNo, vtmId, *pSrvGrpId);
	}
	
	return AMS_OK;
}

VTA_NODE *AmsSelectVta(unsigned char *pVtmNo,
						   unsigned int vtmId,
						   unsigned int srvGrpId, 
						   unsigned int serviceType,
						   unsigned int vtaNum,
						   unsigned int orgId,
						   unsigned int selectMode,
						   int *pResult)
{
	VTA_NODE            *pVtaNode = NULL;	
	VTA_NODE            *pVtaIdleLongestNode = NULL;	
	unsigned int        idleLongestTime = 0;
	time_t              currentTime;	
	unsigned int        i = 0;
	
	//检查柜员机设备号
	if(NULL == pVtmNo)
	{
		dbgprint("AmsSelectVta VtmId[%u] SrvGrpId[%d] VtmNoErr", vtmId, srvGrpId);
		*pResult = AMS_CMS_GET_VTA_SIR_VTMNO_ERR;
		return NULL;		
	}

	//检查业务组编号
	if(srvGrpId > AMS_SERVICE_GROUP_ID_MAX)
	{
		dbgprint("AmsSelectVta Vtm[%s][%u] SrvGrpId[%d]Err", pVtmNo, vtmId, srvGrpId);
		*pResult = AMS_CMS_GET_VTA_SIR_SERVICE_GROUP_ID_ERR;
		return NULL;		
	}

	//获取柜员链表头结点
	pVtaNode = (VTA_NODE *)lstFirst(&AmsSrvData(srvGrpId).vtaList);

	//没有柜员登陆
	if(NULL == pVtaNode)
	{
		dbgprint("AmsSelectVta Vtm[%s][%u] SrvGrpId[%u] VtaListEmpty.", 
			pVtmNo, vtmId, srvGrpId);			
		*pResult = AMS_CMS_GET_VTA_SIR_VTA_EMPTY;
		return NULL;	
	}
	
	//记录当前时刻
	time(&currentTime);
			
	//找到满足服务条件且空闲时间最长的柜员(数量不多，顺序比较)
	for(i = 0; i < vtaNum; i++)
	{
		if(NULL == pVtaNode)
		{
			break;
		}

		//检查柜员是否空闲
		if(    AMS_VTA_STATE_IDLE != pVtaNode->state 
			|| AMS_CALL_STATE_NULL != pVtaNode->callState)
		{
			if(AmsDebugTrace)
			{
				dbgprint("AmsSelectVta Teller[%s][%u]State[%d]CallState[%d].", 
					pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
					pVtaNode->state, pVtaNode->callState);	
			}
			pVtaNode = (VTA_NODE *)lstNext((NODE *)pVtaNode);

			continue;
		}

		//优先本机银行
		if(AMS_SELECT_VTA_PREFER_ORG == selectMode)
		{
			//检查柜员机构标识
			if(    pVtaNode->orgCfgPos >= AMS_MAX_ORG_NUM
				|| pVtaNode->vtaInfo.orgId != orgId)
			{
				if(AmsDebugTrace)
				{
					dbgprint("AmsSelectVta Teller[%s][%u]OrgPos[%d]Id[%d][%d]Err", 
						pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
						pVtaNode->orgCfgPos, 
						pVtaNode->vtaInfo.orgId, orgId);
				}
				pVtaNode = (VTA_NODE *)lstNext((NODE *)pVtaNode);

				continue;
			}
		}
	
		//检查柜员是否注册
		if(pVtaNode->tellerRegPos >= AMS_MAX_VTA_NUM)
		{
			if(AmsDebugTrace)
			{
				dbgprint("AmsSelectVta Teller[%s][%u]RegPos[%d]Err", 
					pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
					pVtaNode->tellerRegPos);
			}
			pVtaNode = (VTA_NODE *)lstNext((NODE *)pVtaNode);

			continue;
		}
	    if(AMS_TELLER_REGISTER != AmsRegTeller(pVtaNode->tellerRegPos).flag)
	    {		
			if(AmsDebugTrace)
			{
				dbgprint("AmsSelectVta Teller[%s][%u]NotReg", 
					pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId);
			}
			pVtaNode = (VTA_NODE *)lstNext((NODE *)pVtaNode);

			continue;
	    }
		
		//检查柜员业务技能
	    if(AMS_OK != AmsCheckTellerServiceAbility(serviceType, pVtaNode->vtaInfo.tellerSrvAuth))//逐位比较
	    {		
			if(AmsDebugTrace)
			{
				dbgprint("AmsSelectVta Teller[%s][%u]NoService[%u][0x%x]Ability", 
					pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
					serviceType, pVtaNode->vtaInfo.tellerSrvAuth);
			}		
			pVtaNode = (VTA_NODE *)lstNext((NODE *)pVtaNode);

			continue;
	    }

		//检查柜员连续呼叫转移次数
		if(pVtaNode->callTransferNum >= AMS_TELLER_CALL_TRANSFER_TIMES_ALLOWED)
	    {		
			if(AmsDebugTrace)
			{
				dbgprint("AmsSelectVta Teller[%s][%u]CallTransTooManyTimes[%u]", 
					pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
					pVtaNode->callTransferNum);	
			}
			pVtaNode = (VTA_NODE *)lstNext((NODE *)pVtaNode);

			continue;
	    }
		
		//检查柜员空闲时长
	    if((currentTime - pVtaNode->stateStartTime) < AMS_TELLER_IDLE_TIME_ALLOWED)
	    {		
			if(AmsDebugTrace)
			{
				dbgprint("AmsSelectVta Teller[%s][%u]IdleTime[%u][%u]NotEnough", 
					pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
					currentTime, pVtaNode->stateStartTime);
			}
			pVtaNode = (VTA_NODE *)lstNext((NODE *)pVtaNode);

			continue;
	    }

		//检查服务效果指数
	    if(    pVtaNode->dailyAvgScore < AMS_TELLER_SERVICE_QUALITY_THRESHOLD 
			&& pVtaNode->dailyAvgScoreExpect < AMS_TELLER_SERVICE_QUALITY_THRESHOLD)
	    {
			if(AmsDebugTrace)
			{
				dbgprint("AmsSelectVta Teller[%s][%u]NeedImprove Score[%u][%u]", 
					pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
					pVtaNode->dailyAvgScore, pVtaNode->dailyAvgScoreExpect);
			}

			if(pVtaNode->updateScoreExpectTime < currentTime)
			{
				if(currentTime - pVtaNode->updateScoreExpectTime > 2)// 2S
				{
					//此次不安排工作，让员工调整状态，同时上调得分均值期望
					//有客户持续等待的情况下，最长冻结时间是(7000-1000)/100 * 2 = 120S
					//一直无客户排队的情况下，最长冻结时间是办理(7000-1000)/100 = 60 人的业务
					//重新登录可恢复初始值
					pVtaNode->dailyAvgScoreExpect += AMS_TELLER_SERVICE_QUALITY_EXPECT_ADD;

					pVtaNode->updateScoreExpectTime = currentTime;
				}
			}

			pVtaNode = (VTA_NODE *)lstNext((NODE *)pVtaNode);
			
			continue;
	    }

		//检查柜员疲劳指数(柜员登陆后，工作时间超过85%，即51分钟)
	    if(pVtaNode->vtaWorkInfo.workSeconds > ((currentTime - pVtaNode->startTime) * AMS_TELLER_FATIGUE_VALUE_THRESHOLD / 100))
	    {
			if(AmsDebugTrace)
			{
				dbgprint("AmsSelectVta Teller[%s][%u]Need Rest[%u][%u][%u]", 
					pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
					pVtaNode->vtaWorkInfo.workSeconds, currentTime, pVtaNode->startTime);
			}

			pVtaNode = (VTA_NODE *)lstNext((NODE *)pVtaNode);

			continue;
	    }

		//比较空闲时长
		if(idleLongestTime < (currentTime - pVtaNode->stateStartTime))
		{
			idleLongestTime = (currentTime - pVtaNode->stateStartTime);
			pVtaIdleLongestNode = pVtaNode;
		}

		pVtaNode = (VTA_NODE *)lstNext((NODE *)pVtaNode);			
	}

	//没有符合条件的柜员
	if(NULL == pVtaIdleLongestNode)
	{
		*pResult = AMS_CMS_GET_VTA_SERVICE_IN_QUEUE;
		return NULL;	
	}

	//柜员选择成功:HI~醒醒，该干活了...
	return pVtaIdleLongestNode;	
	
}

int AmsGetParentOrg(unsigned int orgId, unsigned int orgCfgPos, unsigned int *pParentOrgCfgPos)
{
	ORG_ID_NODE         *pOrgIdNode;
	
	if(orgCfgPos >= AMS_MAX_ORG_NUM)
	{
		if(AmsDebugTrace)
		{
			dbgprint("AmsGetParentOrg OrgPos[%d][%d]Err", orgId, orgCfgPos);
		}

		return AMS_ERROR;
	}
	
	//检查是否有上级机构
	if(0 == AmsCfgOrg(orgCfgPos).parentOrgIdFlag)
	{
		if(AmsDebugTrace)
		{
			dbgprint("AmsGetParentOrg OrgId[%d] NoParentOrg[%d][%d]", 
				orgId, AmsCfgOrg(orgCfgPos).parentOrgId, AmsCfgOrg(orgCfgPos).parentOrgIdFlag);
		}

		//当前已是总行
		*pParentOrgCfgPos = orgCfgPos;
		
		return AMS_OK;
		
	}

	//判断上级机构标识
	if(orgId == AmsCfgOrg(orgCfgPos).parentOrgId)
	{
		if(AmsDebugTrace)
		{
			dbgprint("AmsGetParentOrg OrgId[%d]-[%d][%d]Err", 
				orgId, AmsCfgOrg(orgCfgPos).parentOrgId, AmsCfgOrg(orgCfgPos).parentOrgIdFlag);
		}

		//机构ID出错
		return AMS_ERROR;
		
	}
	
	//获取上级机构位置
	orgId = AmsCfgOrg(orgCfgPos).parentOrgId;
	
	pOrgIdNode = AmsSearchOrgIdHash(orgId);
	
	if(NULL == pOrgIdNode)
	{
		if(AmsDebugTrace)
		{
			dbgprint("AmsGetParentOrg OrgId[%d]Err.", orgId);
		}				
		
		return AMS_ERROR;
	}
	else
	{	
		if(AmsDebugTrace)
		{
			dbgprint("AmsGet ParentOrgPos[%d]Id[%d].", pOrgIdNode->orgPos, orgId);
		}
		
		//找到上级机构位置
		*pParentOrgCfgPos = pOrgIdNode->orgPos;
	}

	return AMS_OK;
	
}

VTA_NODE * AmsSelectVtaByOrg(unsigned char *pVtmNo,
								   unsigned int vtmId,
								   unsigned int srvGrpId, 
								   unsigned int serviceType,
								   unsigned int vtaNum,
								   unsigned int orgId,
								   unsigned int orgCfgPos,
								   int *pResult)
{
	VTA_NODE            *pVtaNode = NULL;	
	unsigned int        parentOrgCfgPos = 0xFFFFFFFF;
	unsigned int        i = 0;	
	
	//检查柜员机设备号
	if(NULL == pVtmNo)
	{
		dbgprint("AmsSelectVtaByOrg VtmId[%u] SrvGrpId[%d] VtmNoErr", vtmId, srvGrpId);
		*pResult = AMS_CMS_GET_VTA_SIR_VTMNO_ERR;
		return NULL;		
	}

	//检查业务组编号
	if(srvGrpId > AMS_SERVICE_GROUP_ID_MAX)
	{
		dbgprint("AmsSelectVtaByOrg Vtm[%s][%u] SrvGrpId[%d]Err", pVtmNo, vtmId, srvGrpId);
		*pResult = AMS_CMS_GET_VTA_SIR_SERVICE_GROUP_ID_ERR;
		return NULL;		
	}
	
	//获取柜员链表头结点
	pVtaNode = (VTA_NODE *)lstFirst(&AmsSrvData(srvGrpId).vtaList);
	
	//没有柜员登陆
	if(NULL == pVtaNode)
	{
		dbgprint("AmsSelectVtaByOrg Vtm[%s][%u] SrvGrpId[%u] VtaListEmpty.", 
			pVtmNo, vtmId, srvGrpId);			
		*pResult = AMS_CMS_GET_VTA_SIR_VTA_EMPTY;
		return NULL;	
	}
	
	for(i = 0; i< AMS_MAX_ORG_LEVEL; i++)	
	{
		//检查机构位置
		if(orgCfgPos >= AMS_MAX_ORG_NUM)
		{
			if(AmsDebugTrace)
			{
				dbgprint("AmsSelectVtaByOrg Vtm[%s][%u]OrgPos[%d]Err", pVtmNo, vtmId, orgCfgPos);
			}

			*pResult = AMS_CMS_GET_VTA_ORG_POS_ERR;
			return NULL;
		}

		//检查机构状态
		if(AmsCfgOrg(orgCfgPos).orgState != AMS_ORG_ACTIVE)
		{
			if(AmsDebugTrace)
			{
				dbgprint("AmsSelectVtaByOrg Vtm[%s][%u]Org[%u]State[%d]Err", 
					pVtmNo, vtmId, AmsCfgOrg(orgCfgPos).orgId, AmsCfgOrg(orgCfgPos).orgState);
			}

			if(AMS_OK != AmsGetParentOrg(orgId, orgCfgPos, &parentOrgCfgPos))
			{
				if(AmsDebugTrace)
				{
					dbgprint("AmsSelectVtaByOrg Vtm[%s][%u]OrgId[%u]GetParentOrgErr", 
						pVtmNo, vtmId, orgId);
				}

				*pResult = AMS_CMS_GET_VTA_PARENT_ORG_ID_ERR;
				return NULL;				
			}

			//已经是总行
			if(orgCfgPos == parentOrgCfgPos)
			{
				if(AmsDebugTrace)
				{
					dbgprint("AmsSelectVtaByOrg Vtm[%s][%u]OrgId[%u]In Queue.", pVtmNo, vtmId, orgId);
				}				
				//排队去了。。。
				
				break;
			}

			//转为在上级机构中选择
			orgId = AmsCfgOrg(orgCfgPos).parentOrgId;
			orgCfgPos = parentOrgCfgPos;
			
			continue;		
		}			
				
		pVtaNode = AmsSelectVta(pVtmNo,vtmId,srvGrpId,serviceType,vtaNum,orgId,AMS_SELECT_VTA_PREFER_ORG,pResult);
		
		//路由失败
		if(NULL == pVtaNode)
		{
			if(AMS_OK != AmsGetParentOrg(orgId, orgCfgPos, &parentOrgCfgPos))
			{
				if(AmsDebugTrace)
				{
					dbgprint("AmsSelectVtaByOrg Vtm[%s][%u]OrgId[%u]GetParentOrgErr", pVtmNo, vtmId, orgId);
				}

				*pResult = AMS_CMS_GET_VTA_PARENT_ORG_ID_ERR;
				return NULL;					
			}
			
			//已经是总行
			if(orgCfgPos == parentOrgCfgPos)
			{
				if(AmsDebugTrace)
				{
					dbgprint("AmsSelectVtaByOrg Vtm[%s][%u]OrgId[%u]In Queue..", pVtmNo, vtmId, orgId);
				}				
				//排队去了。。。
				
				break;
			}
			
			//在上级机构中选择
			orgId = AmsCfgOrg(orgCfgPos).parentOrgId;
			orgCfgPos = parentOrgCfgPos;
			
		}
		else
		{
			if(AmsDebugTrace)
			{
				dbgprint("AmsSelectVtaByOrg Vtm[%s][%u]OrgId[%u]GetVta[%s][%u].", 
					pVtmNo, vtmId, orgId, 
					pVtaNode->vtaInfo.tellerNo, 
					pVtaNode->vtaInfo.tellerId);
			}		

			*pResult = AMS_CMS_PRCOESS_SUCCESS;
			
			//路由成功
			break;
		}

	}

	//路由成功或进入排队
	return pVtaNode;	
	
}

VTA_NODE *AmsSelectVtaByTeller(unsigned char *pVtmNo,
									unsigned int vtmId,
									unsigned int srvGrpId, 
									unsigned int serviceType,
									CALL_TARGET callTarget,
									int *pResult)
{
	VTA_NODE            *pVtaNode = NULL;	

	//检查柜员机设备号
	if(NULL == pVtmNo)
	{
		dbgprint("AmsSelectVtaByTeller VtmId[%u] SrvGrpId[%d] VtmNoErr", vtmId, srvGrpId);
		*pResult = AMS_CMS_GET_VTA_SIR_VTMNO_ERR;
		return NULL;		
	}

	//检查业务组编号
	if(srvGrpId > AMS_SERVICE_GROUP_ID_MAX)
	{
		dbgprint("AmsSelectVtaByTeller Vtm[%s][%u] SrvGrpId[%d]Err", pVtmNo, vtmId, srvGrpId);
		*pResult = AMS_CMS_GET_VTA_SIR_SERVICE_GROUP_ID_ERR;
		return NULL;		
	}
	
    /* find Vta Node */
    pVtaNode = AmsSearchVtaNode(srvGrpId, callTarget.targetTellerId);
	if(NULL == pVtaNode)
	{
		dbgprint("AmsSelectVtaByTeller Vtm[%s][%u] SrvGrpId[%u] TargetTeller[%u]NotLogin", 
			pVtmNo, vtmId, srvGrpId, callTarget.targetTellerId); //TargetVtaNo print after the fun.
		*pResult = AMS_CMS_GET_VTA_SIR_TARGET_TELLER_NOT_LOGIN;
		return NULL;		
	}
	
	//检查柜员是否注册
	if(pVtaNode->tellerRegPos >= AMS_MAX_VTA_NUM)
	{
		dbgprint("AmsSelectVtaByTeller Vtm[%s][%u] SrvGrpId[%u] TargetTeller[%s][%u]NotReg", 
			pVtmNo, vtmId, srvGrpId, 
			pVtaNode->vtaInfo.tellerNo, callTarget.targetTellerId);		
		*pResult = AMS_CMS_GET_VTA_SIR_TARGET_TELLER_NOT_REGISTERED;
		return NULL;				
	}
    if(AMS_TELLER_REGISTER != AmsRegTeller(pVtaNode->tellerRegPos).flag)
    {		
		dbgprint("AmsSelectVtaByTeller Vtm[%s][%u] SrvGrpId[%u] TargetTeller[%s][%u]NotReg!", 
			pVtmNo, vtmId, srvGrpId, 
			pVtaNode->vtaInfo.tellerNo, callTarget.targetTellerId);		
		*pResult = AMS_CMS_GET_VTA_SIR_TARGET_TELLER_NOT_REGISTERED;
		return NULL;		
    }			

	//检查柜员状态
	if(pVtaNode->state != AMS_VTA_STATE_IDLE)
	{
		dbgprint("AmsSelectVtaByTeller Vtm[%s][%u] SrvGrpId[%u] TargetTeller[%s][%u]State[%d]Err", 
			pVtmNo, vtmId, srvGrpId, 
			pVtaNode->vtaInfo.tellerNo, callTarget.targetTellerId, 
			pVtaNode->state);		
		*pResult = AMS_CMS_GET_VTA_SIR_TARGET_TELLER_STATE_ERR;
		return NULL;			
	}

	//检查柜员连续呼叫转移次数
	if(pVtaNode->callTransferNum >= AMS_TELLER_CALL_TRANSFER_TIMES_ALLOWED)
    {
		dbgprint("AmsSelectVtaByTeller Vtm[%s][%u] TargetTeller[%s][%u]CallTransTooManyTimes[%u]", 
			pVtmNo, vtmId, 
			pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
			pVtaNode->callTransferNum);			
		*pResult = AMS_CMS_GET_VTA_SIR_CALL_TRANSFER_TOO_MANY_TIMES;
		return NULL;
    }

	//指定柜员时不检查: 空闲时长/服务效果指数/疲劳指数

	//get vta !!!
	return pVtaNode;
	
}

VTA_NODE *AmsServiceIntelligentSelectVta(unsigned char *pVtmNo,
											   unsigned int vtmId,
											   unsigned int srvGrpId, 
											   unsigned int serviceType,
											   unsigned int callType,  
											   CALL_TARGET callTarget,
											   unsigned int orgId,
											   unsigned int orgCfgPos,
											   int *pResult)
{
	unsigned int        vtaNum;
	VTA_NODE            *pVtaNode = NULL;	
	QUEUE_INFO          queueInfo;
	unsigned int        i = 0;

	//检查柜员机设备号
	if(NULL == pVtmNo)
	{
		dbgprint("AmsSISelectVta VtmId[%u] SrvGrpId[%d] VtmNoErr", 
			vtmId, srvGrpId);
		*pResult = AMS_CMS_GET_VTA_SIR_VTMNO_ERR;
		return NULL;		
	}
	
	//检查业务组编号
	if(srvGrpId > AMS_SERVICE_GROUP_ID_MAX)
	{
		dbgprint("AmsSISelectVta Vtm[%s][%u] SrvGrpId[%d]Err", 
			pVtmNo, vtmId, srvGrpId);
		*pResult = AMS_CMS_GET_VTA_SIR_SERVICE_GROUP_ID_ERR;
		return NULL;		
	}

	//检测呼叫类型
	if(callType >= AMS_CALL_TYPE_MAX)
	{
		dbgprint("AmsSISelectVta Vtm[%s][%u] CallType[%d]Err", 
			pVtmNo, vtmId, callType);
		*pResult = AMS_CMS_GET_VTA_SIR_CALL_TYPE_ERR;
		return NULL;		
	}

	//普通呼叫智能路由及指定柜员组呼叫
	if(    AMS_CALL_VTM_TO_VTA == callType 
		|| (AMS_CALL_TRANSFER == callType && AMS_CALL_GROUP == callTarget.callTargetType))
	{		
		//检查柜员队列
		vtaNum = lstCount(&AmsSrvData(srvGrpId).vtaList);
		if(0 == vtaNum)
		{
			dbgprint("AmsSISelectVta Vtm[%s][%u]SrvGrpId[%u] VtaListEmpty", 
				pVtmNo, vtmId, srvGrpId);
			*pResult = AMS_CMS_GET_VTA_SIR_VTA_EMPTY;
			return NULL;		
		}

		//检查客户排队长度
		if(lstCount(&AmsSrvData(srvGrpId).vtmList) > 1)
		{
			memset(&queueInfo,0,sizeof(QUEUE_INFO));
			if(AMS_OK != AmsCustCalcSrvGrpIdQueueInfo(vtmId, srvGrpId, &queueInfo))
			{
				dbgprint("AmsSISelectVta Vtm[%s][%u] CalcSrvGrp[%u]QueueInfoErr", 
					pVtmNo, vtmId, srvGrpId);
				*pResult = AMS_CMS_GET_VTA_SIR_CALC_QUEUE_INFO_ERR;
				return NULL;				
			}

			for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
			{
				if(    AMS_QUEUE_CFG == AmsCfgQueueSys(i).flag 
					&& AmsCfgQueueSys(i).srvGrpId == srvGrpId)//重复~
				{
					//已经达到允许的最大排队长度
					if(queueInfo.queuingLen >= AmsCfgQueueSys(i).maxQueLen)
					{
						dbgprint("AmsSISelectVta Vtm[%s][%u] SrvGrpId[%u] tooManyCustomers[%u][%u][%d]!", 
							pVtmNo, vtmId, srvGrpId, 
							queueInfo.queuingLen, AmsCfgQueueSys(i).maxQueLen, i);						
						*pResult = AMS_CMS_GET_VTA_SIR_TOO_MANY_CUSTOMER_IN_QUEUE;
						return NULL;							
					}
				}
			}
		}

		//是否优选本地分行柜员
		if(1 == AmsBranchInfo.flag && 1 == AmsBranchInfo.branchFlag)
		{
			//就近分配本地分行柜员
			pVtaNode = AmsSelectVtaByOrg(pVtmNo,vtmId,srvGrpId,serviceType,vtaNum,orgId,orgCfgPos,pResult);
		}
		else
		{
			//常规选择
			pVtaNode = AmsSelectVta(pVtmNo,vtmId,srvGrpId,serviceType,vtaNum,orgId,AMS_COMMON_SELECT_VTA,pResult);
		}

		//路由失败，
		if(*pResult != AMS_CMS_PRCOESS_SUCCESS)
		{
				
			//路由失败原因为暂无合适柜员，进入排队
			if(AMS_CMS_GET_VTA_SERVICE_IN_QUEUE == *pResult)
			{
				return NULL;
			}

			//路由失败原因为出现错误
			if(AmsDebugTrace)
			{
				dbgprint("AmsSISelectVta Vtm[%s][%u]Result[0x%x]Err", pVtmNo, vtmId, *pResult);
			}
			
			return NULL;
		}
		
		//路由成功
		if(NULL != pVtaNode)
		{
			if(AmsDebugTrace)
			{
				dbgprint("AmsSISelectVta Vtm[%s][%u]GetVta[%s][%u].", 
					pVtmNo, vtmId,  
					pVtaNode->vtaInfo.tellerNo, 
					pVtaNode->vtaInfo.tellerId);
			}
						
			return pVtaNode;
		}

	}

	//指定柜员呼叫
	if(    (   AMS_CALL_INNER == callType 
		    || AMS_CALL_TRANSFER == callType 
		    || AMS_CALL_MONITOR == callType) 
		&& (AMS_CALL_TELLER == callTarget.callTargetType))
	{				
		pVtaNode = AmsSelectVtaByTeller(pVtmNo,vtmId,srvGrpId,serviceType,callTarget,pResult);

		//路由无论成功失败，均直接返回
		return pVtaNode;
	}

	//其他类型路由失败，默认进入排队
	//not get, please wait ...
	*pResult = AMS_CMS_GET_VTA_SERVICE_IN_QUEUE;
	
    return NULL;
	
}
/*
int AmsCheckTellerServiceInfo(VTA_NODE *pVtaNode, 
	                                unsigned int srvGrpId, 
	                                unsigned int serviceType, 
	                                time_t currentTime)
{
	int iret = AMS_ERROR;

	//检查柜员结点
	if(NULL == pVtaNode)
	{
		return iret;		
	}
	
	//检查业务组编号，可不检查
	if(srvGrpId > AMS_SERVICE_GROUP_ID_MAX)
	{
		if(AmsErrorTrace)
		{
			dbgprint("Ams CheckTellerServiceInfo Teller[%s][%u] SrvGrpId[%u]Err", 
				pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, srvGrpId);
		}
		return iret;		
	}

	//检查柜员是否注册
	if(pVtaNode->tellerRegPos >= AMS_MAX_VTA_NUM)
	{
		if(AmsDebugTrace)
		{
			dbgprint("Ams CheckTellerServiceInfo Teller[%s][%u]RegPos[%d]Err", 
				pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
				pVtaNode->tellerRegPos);
		}
		return iret;
	}
    if(AMS_TELLER_REGISTER != AmsRegTeller(pVtaNode->tellerRegPos).flag)
    {		
		if(AmsDebugTrace)
		{
			dbgprint("Ams CheckTellerServiceInfo Teller[%s][%u]NotReg", 
				pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId);
		}
		return iret;
    }
	
	//检查柜员业务技能
    if(AMS_OK != AmsCheckTellerServiceAbility(serviceType, pVtaNode->vtaInfo.tellerSrvAuth))//逐位比较
    {		
		if(AmsDebugTrace)
		{
			dbgprint("Ams CheckTellerServiceInfo Teller[%s][%u]NoService[%u][0x%x]Ability", 
				pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
				serviceType, pVtaNode->vtaInfo.tellerSrvAuth);
		}		
		return iret;
    }
	
	//检查柜员是否空闲
    if(AMS_VTA_STATE_IDLE != pVtaNode->state || AMS_CALL_STATE_NULL != pVtaNode->callState)
    {		
		if(AmsDebugTrace)
		{
			dbgprint("Ams CheckTellerServiceInfo Teller[%s][%u]State[%u]CallState[%u]NotIdle", 
				pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
				pVtaNode->state, pVtaNode->callState);
		}
		return iret;
    }

	//检查柜员连续呼叫转移次数
	if(pVtaNode->callTransferNum >= AMS_TELLER_CALL_TRANSFER_TIMES_ALLOWED)
    {		
		if(AmsDebugTrace)
		{
			dbgprint("Ams CheckTellerServiceInfo Teller[%s][%u]CallTransTooManyTimes[%u]", 
				pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
				pVtaNode->callTransferNum);	
		}
		return iret;
    }
	
	//检查柜员空闲时长
    if((currentTime - pVtaNode->stateStartTime) < AMS_TELLER_IDLE_TIME_ALLOWED)
    {		
		if(AmsDebugTrace)
		{
			dbgprint("Ams CheckTellerServiceInfo Teller[%s][%u]IdleTime[%u][%u]NotEnough", 
				pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
				currentTime, pVtaNode->stateStartTime);
		}
		return iret;
    }
	
	//检查服务效果指数
    if(    pVtaNode->dailyAvgScore < AMS_TELLER_SERVICE_QUALITY_THRESHOLD 
		&& pVtaNode->dailyAvgScoreExpect < AMS_TELLER_SERVICE_QUALITY_THRESHOLD)
    {
		if(AmsDebugTrace)
		{
			dbgprint("Ams CheckTellerServiceInfo Teller[%s][%u]NeedImprove Score[%u][%u]", 
				pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
				pVtaNode->dailyAvgScore, pVtaNode->dailyAvgScoreExpect);
		}

		if(pVtaNode->updateScoreExpectTime < currentTime)
		{
			if(currentTime - pVtaNode->updateScoreExpectTime > 2)
			{
				//此次不安排工作，让员工调整状态，同时上调得分均值期望
				//有客户持续等待的情况下，最长冻结时间是(7000-1000)/100 * 2 = 120S
				//一直无客户排队的情况下，最长冻结时间是办理(7000-1000)/100 = 60 人的业务
				//重新登录可恢复初始值
				pVtaNode->dailyAvgScoreExpect += AMS_TELLER_SERVICE_QUALITY_EXPECT_ADD;

				pVtaNode->updateScoreExpectTime = currentTime;
			}
		}
		
		return iret;
    }
	
	//检查柜员疲劳指数(柜员登陆后，工作时间超过85%，即51分钟)
    if(pVtaNode->vtaWorkInfo.workSeconds > ((currentTime - pVtaNode->startTime) * AMS_TELLER_FATIGUE_VALUE_THRESHOLD / 100))
    {
		if(AmsDebugTrace)
		{
			dbgprint("Ams CheckTellerServiceInfo Teller[%s][%u]Need Rest[%u][%u][%u]", 
				pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId, 
				pVtaNode->vtaWorkInfo.workSeconds, currentTime, pVtaNode->startTime);
		}

		return iret;
    }

	//柜员选择成功:HI~醒醒，该干活了...
	iret = AMS_OK;
	
	return iret;
}
*/
int AmsCheckTellerServiceAbility(unsigned int serviceType, unsigned int tellerSrvAuth)
{
	unsigned int i = 0;

	for(i = 0; i < AMS_MAX_SERVICE_NUM; i ++)
	{
		if((serviceType & 0x01) > (tellerSrvAuth & 0x01))
		{
			return AMS_ERROR;
		}

		serviceType = (serviceType>>1);
		tellerSrvAuth = (tellerSrvAuth>>1);
	}

	return AMS_OK;
}

int AmsStartCustomerQueueProcess(MESSAGE_t *pMsg,
											VTM_NODE *pVtmNode,
											unsigned int srvGrpId,
											unsigned int serviceType,
											unsigned int callType,  
											CALL_TARGET callTarget,	                                       
											unsigned char callIdLen,
											unsigned char srvGrpSelfAdapt)
{
	int					iret = AMS_CMS_PRCOESS_SUCCESS;
	LP_QUEUE_DATA_t     *lpQueueData = NULL;      //排队进程数据区指针 
	PID_t				Pid;
	unsigned char       timerPara[AMS_MAX_TIMER_PARA_LEN];
	MESSAGE_t           msg;

	if(NULL == pMsg || NULL == pVtmNode)
	{
		dbgprint("AmsStartCustomerQueueProcess Para[%d][%d]", pMsg, pVtmNode);
		iret = AMS_CMS_GET_VTA_PARA_ERR;
		return iret;
	}
	
    //分配排队服务进程号
	memset(&Pid,0,sizeof(PID_t));
	iret = AmsAllocPid(&Pid);
	if(-1 == iret)
	{
		dbgprint("AmsStartCustomerQueueProcess Vtm[%s][%u] AllocPid: SysBusy",
			pVtmNode->vtmInfo.vtmNo, pVtmNode->vtmInfo.vtmId);
		
		iret = AMS_CMS_GET_VTA_LP_RESOURCE_LIMITED;
		return iret;
	}

	//record Customer In Queue Pid
	pVtmNode->customerPid = Pid.iProcessId;
	
	lpQueueData=(LP_QUEUE_DATA_t *)ProcessData[pVtmNode->customerPid];

	//排队进程数据区初始化
    memset(lpQueueData, 0, sizeof(LP_AMS_DATA_t));
	
	//record vtmId
	lpQueueData->vtmId = pVtmNode->vtmInfo.vtmId;
	
	//record srvGrpId
	lpQueueData->srvGrpId = srvGrpId;
	
	//record serviceType
	lpQueueData->serviceType = serviceType;

	//record vtmNo
	if(pVtmNode->vtmInfo.vtmNoLen <= AMS_MAX_VTM_NO_LEN)
	{
		//lpQueueData->vtmNo has been cleared
		memcpy(lpQueueData->vtmNo, pVtmNode->vtmInfo.vtmNo, pVtmNode->vtmInfo.vtmNoLen);
		lpQueueData->vtmNoLen = pVtmNode->vtmInfo.vtmNoLen;
	}

	//record callId
    lpQueueData->callIdLen = callIdLen;
	memcpy(lpQueueData->callId, &pMsg->cMessageBody[1], callIdLen);

	//record srvGrpSelfAdapt
	lpQueueData->srvGrpSelfAdapt = srvGrpSelfAdapt;
	
	//record callType
	lpQueueData->callType = callType;
	
	//record callTarget
	lpQueueData->callTarget.callTargetType      = callTarget.callTargetType;
	lpQueueData->callTarget.targetTellerGroupId = callTarget.targetTellerGroupId;
	lpQueueData->callTarget.targetTellerId      = callTarget.targetTellerId;
	
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
    memcpy(&lpQueueData->vtmPid,&pVtmNode->rPid,sizeof(PID_t));
	
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
		BEPUTLONG(lpQueueData->vtmId,timerPara);
        if(AMS_OK != AmsQueueCreateTimerPara(lpQueueData->myPid.iProcessId,
											 &lpQueueData->iTimerId, 
											 B_AMS_CUSTOMER_IN_QUEUE_TIMEOUT, 
											 AmsCfgData.amsCustomerInQueueTimeLength,
											 timerPara))
        {
			dbgprint("StartCustomerQueueProcess[%d] Vtm[%s][%u] CreateTimer Err",
				lpQueueData->myPid.iProcessId, lpQueueData->vtmNo, lpQueueData->vtmId);
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

VTM_NODE * AmsGetIdleLongestVtm(unsigned int srvGrpId, int vtmNum, time_t currentTime)
{
	VTM_NODE            *pVtmNode = NULL;	
	VTM_NODE            *pVtmIdleLongestNode = NULL;
	unsigned int        idleLongestTime = 0;	
	unsigned int        j = 0;
	
	if(vtmNum > AMS_MAX_VTM_NODES)
	{
		return NULL;
	}

	if(srvGrpId >= AMS_MAX_SERVICE_GROUP_NUM)
	{
		return NULL;
	}
	
	//找到呼叫等待时间最长的客户(数量不多，顺序比较)
	pVtmNode = (VTM_NODE *)lstFirst(&AmsSrvData(srvGrpId).vtmList);
	while(NULL != pVtmNode && j < vtmNum)
	{
		if(    AMS_VTM_STATE_BUSY == pVtmNode->state 
			&& AMS_CUSTOMER_IN_QUEUE == pVtmNode->serviceState 
			&& currentTime > pVtmNode->enterQueueTime)
		{
			if(idleLongestTime < (currentTime - pVtmNode->enterQueueTime))
			{
				idleLongestTime = (currentTime - pVtmNode->enterQueueTime);
				pVtmIdleLongestNode = pVtmNode;
			}
		}
		
		pVtmNode = (VTM_NODE *)lstNext((NODE *)pVtmNode);	
		j ++;
	}

	return pVtmIdleLongestNode;	
	
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

void AmsUpdateVtaWorkInfoProc(time_t currentTime)
{
	VTA_NODE            *pVtaNode = NULL;		
	int                 updateNum = 0;	
	int                 i = 0;
	int                 j = 0;	

    /* update Vta Work Info if Need */
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		j = 0;
		
		pVtaNode = (VTA_NODE *)lstFirst(&AmsSrvData(i).vtaList);
		while(NULL != pVtaNode && j < AMS_MAX_VTA_NODES)
		{			
			if(currentTime - pVtaNode->workInfoUpdateTime >= AMS_VTA_WORK_INFO_UPDATE_INTERVAL)
			{		
				if(AMS_OK == AmsUpdateSingleVtaWorkInfo(pVtaNode, currentTime))
				{
					if(pVtaNode->tellerCfgPos < AMS_MAX_VTA_NUM)
					{
						if(AmsTellerStat(pVtaNode->tellerCfgPos).tellerId == pVtaNode->vtaInfo.tellerId)
						{
							memcpy(&AmsTellerStat(pVtaNode->tellerCfgPos).vtaWorkInfo, &pVtaNode->vtaWorkInfo, sizeof(TELLER_WORK_INFO));
						}
					}
					
					updateNum++;

					if(updateNum >= AMS_MAX_UPDATE_WORK_INFO_NUM)
					{
						return;
					}
				}
			}

			pVtaNode = (VTA_NODE *)lstNext((NODE *)pVtaNode);
			j ++;
			
		}
		
	}
	
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
/*
	if(AmsMsgTrace)
	{	
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"send C_AMS_SERVICE_PROC_REQ msg \n");	
		AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"C_AMS_SERVICE_PROC_REQ",description,
			           descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,"ams");				
	}
*/

	//not need result stat

	return SUCCESS;
}


int AmsProcServiceProcMsg(int iThreadId, MESSAGE_t *pMsg)
{
	time_t              currentTime;

	time(&currentTime);	

	//清理不活动的VTA连接，及释放业务处理进程
	AmsClearInactiveVta(iThreadId, currentTime);

	//清理不活动的VTM连接，及释放排队服务进程
	AmsClearInactiveVtm(iThreadId, currentTime);


    //判断是否有等待业务的客户
    //若有，从队列头开始顺序为客户选择柜员；
    //若选择成功，则释放排队服务进程	
    AmsGetVtaInServiceProcTask(iThreadId, currentTime);


	//清理不活动的RCAS服务器
	AmsClearInactiveRcas(iThreadId, currentTime);

	//清理不活动的CMS呼叫
	AmsClearInactiveCmsCall(iThreadId, currentTime);

	
    //calc teller WorkInfo Every 5 Minutes
	AmsUpdateVtaWorkInfoProc(currentTime);

	return AMS_OK;
	
}
	
int AmsCalcServiceQueueInfo(unsigned int tellerId, unsigned int serviceId, QUEUE_INFO *pQueueInfo)
{
	VTM_NODE            *pTempVtmNode = NULL;
	unsigned int        serviceType = 0;	
	int                 i = 0;
	int                 j = 0;
	
	if(0 == serviceId || serviceId > AMS_MAX_SERVICE_ID_VALUE)
	{
		dbgprint("AmsCalcServiceQueueInfo[%u] serviceId[%u]Err", tellerId, serviceId);		
		return AMS_ERROR;
	}
	
	if(0 == tellerId)
	{
		dbgprint("AmsCalcServiceQueueInfo TellerId[%u]Err", tellerId);			
		return AMS_ERROR;
	}

	pQueueInfo->srvGrpId = 0;	//not used
	
	serviceType = (1 << (serviceId - 1));
	
    for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
    {
		pTempVtmNode = (VTM_NODE *)lstFirst(&AmsSrvData(i).vtmList);
		while(NULL != pTempVtmNode && j < AMS_MAX_VTM_NODES)
	    {
			if(    AMS_CUSTOMER_IN_QUEUE == pTempVtmNode->serviceState
				&& 0 != (pTempVtmNode->serviceType & serviceType))
			{
				pQueueInfo->queuingLen++;   
			}

	        pTempVtmNode = (VTM_NODE *)lstNext((NODE *)pTempVtmNode);
			j++;
	    }
    }

	pQueueInfo->queuingTime = 0;	//not used	

	
//	dbgprint("AmsCalcServiceQueueInfo tellerId[%u][%u]pQueueInfo[%d][%d]\r\n", 
//		tellerId, serviceId, pQueueInfo->queuingLen, pQueueInfo->queuingTime);	
	
	return AMS_OK;
}

int AmsTellerCalcSrvGrpIdQueueInfo(unsigned int tellerId, unsigned int srvGrpId, QUEUE_INFO *pQueueInfo)
{
	VTM_NODE            *pTempVtmNode = NULL;
	int                 custNum = 0;
	unsigned int        tellerNum = 0;		
	int                 i = 0;

	if(srvGrpId > AMS_SERVICE_GROUP_ID_MAX)
	{
		dbgprint("Ams Teller[%u]CalcSrvGrpIdQueueInfo SrvGrpId[%u]Err", 
			tellerId, srvGrpId);		
		return AMS_ERROR;
	}
	
	if(0 == tellerId)
	{
		dbgprint("Ams TellerCalcSrvGrpId[%u]QueueInfo TellerId[%u]Err", 
			srvGrpId, tellerId);			
		return AMS_ERROR;
	}
	
	//Calc Queue Info
	pQueueInfo->srvGrpId = srvGrpId;
	
	custNum = lstCount(&AmsSrvData(srvGrpId).vtmList);
	tellerNum = lstCount(&AmsSrvData(srvGrpId).vtaList);
	pTempVtmNode = (VTM_NODE *)lstFirst(&AmsSrvData(srvGrpId).vtmList);
	
	while(NULL != pTempVtmNode && i < custNum)//不必 <=
	{
		if(AMS_CUSTOMER_IN_QUEUE == pTempVtmNode->serviceState)
		{
			pQueueInfo->queuingLen++;   
		}

		pTempVtmNode = (VTM_NODE *)lstNext((NODE *)pTempVtmNode);
		
		i++;	
	}	

	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		if(    AMS_QUEUE_CFG == AmsCfgQueueSys(i).flag
			&& AmsCfgQueueSys(i).srvGrpId == srvGrpId
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
	
//	dbgprint("Ams TellerCalcSrvGrpIdQueueInfo tellerId[%u][%u]pQueueInfo[%d][%d]\r\n", 
//		tellerId, srvGrpId, pQueueInfo->queuingLen, pQueueInfo->queuingTime);	

	return AMS_OK;
}

int AmsCustCalcSrvGrpIdQueueInfo(unsigned int vtmId, unsigned int srvGrpId, QUEUE_INFO *pQueueInfo)
{
	VTM_NODE            *pTempVtmNode = NULL;
	int                 custNum = 0;
	unsigned int        tellerNum = 0;		
	int                 i = 0;

	if(srvGrpId > AMS_SERVICE_GROUP_ID_MAX)
	{
		dbgprint("Ams Cust[%u]CalcSrvGrpIdQueueInfo SrvGrpId[%u]Err", vtmId, srvGrpId);		
		return AMS_ERROR;
	}
	
	if(0 == vtmId)
	{
		dbgprint("Ams CustCalcSrvGrpId[%u]QueueInfo VtmId[%u]Err", srvGrpId, vtmId);			
		return AMS_ERROR;
	}
	
	//Calc Queue Info
//	pQueueInfo->srvGrpId = srvGrpId;
	
	custNum = lstCount(&AmsSrvData(srvGrpId).vtmList);
	tellerNum = lstCount(&AmsSrvData(srvGrpId).vtaList);
	pTempVtmNode = (VTM_NODE *)lstFirst(&AmsSrvData(srvGrpId).vtmList);
	
	while(NULL != pTempVtmNode && i < custNum)//不必 <=
	{
		//count cust ahead
		if(pTempVtmNode->vtmInfo.vtmId == vtmId)
		{
			break;
		}
		if(AMS_CUSTOMER_IN_QUEUE == pTempVtmNode->serviceState)
		{
			pQueueInfo->queuingLen++;   
		}

		pTempVtmNode = (VTM_NODE *)lstNext((NODE *)pTempVtmNode);
		
		i++;	
	}	

	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		if(    AMS_QUEUE_CFG == AmsCfgQueueSys(i).flag 
			&& AmsCfgQueueSys(i).srvGrpId == srvGrpId
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

void AmsGenerateVncAuthPwd(LP_AMS_DATA_t *lpAmsData, VTM_NODE *pVtmNode) //20161021
{
	unsigned int        rawPwd = 0;
	unsigned char       rawPwdItem[4] = { 0 };
	unsigned int        i = 0;
	unsigned char       len = 0;
	time_t              currentTime;
	
	if(    (NULL == lpAmsData && NULL == pVtmNode) 
		|| (NULL != lpAmsData && NULL != pVtmNode))
	{
		return;
	}

	time(&currentTime);

	if(NULL != lpAmsData)
	{
		lpAmsData->vncAuth.flag = 1;

		rawPwd = lpAmsData->tellerId | currentTime;
		
		memcpy(rawPwdItem, &rawPwd, 4);

		for(i = 0; i < (AMS_MAX_VNC_AUTH_PWD_HEAD_LEN/2); i++)
		{
			len += snprintf(lpAmsData->vncAuth.password + len, AMS_MAX_PWD_LEN - len,
				"%02x", rawPwdItem[i]);
		}
				
		lpAmsData->vncAuth.passwordLen = len;
	}

	if(NULL != pVtmNode)
	{
		pVtmNode->vncAuth.flag = 1;

		rawPwd =  pVtmNode->vtmInfo.vtmId | currentTime;

		memcpy(rawPwdItem, &rawPwd, 4);		

		for(i = 0; i < (AMS_MAX_VNC_AUTH_PWD_HEAD_LEN/2); i++)
		{
			len += snprintf(pVtmNode->vncAuth.password + len, AMS_MAX_PWD_LEN - len,
				"%02x", rawPwdItem[i]);
		}
				
		pVtmNode->vncAuth.passwordLen = len;
	}
	
	return;
	
}

int AmsCustomerInQueueTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg)
{
	int					iret = AMS_CMS_GET_VTA_TIMEOUT;
	LP_QUEUE_DATA_t     *lpQueueData = NULL;             //排队进程数据区指针
	LP_AMS_DATA_t		*lpAmsData = NULL;               //进程数据区指针	
	VTM_NODE            *pVtmNode = NULL;			
	int                 pid = 0;
	MESSAGE_t           msg;
	unsigned int        vtmId = 0;	
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
		dbgprint("AmsCustomerInQueueTimeoutProc Vtm[%s][%u] PID[%d][%d] Not Equal", 
			lpQueueData->vtmNo, lpQueueData->vtmId,
			lpQueueData->myPid.iProcessId, pid);
		iret = AMS_CMS_GET_VTA_TIMEOUT_PARA_ERR;
		AmsResultStatProc(AMS_CMS_GET_VTA_RESULT, iret);
		return AMS_ERROR;
	}
	
	//fill cmsPid
	memset(&msg, 0, sizeof(MESSAGE_t));
	memcpy(&msg.s_SenderPid, &lpQueueData->cmsPid, sizeof(PID_t));
	
	//柜员机设备号检查
	p = pTmMsg->cTimerParameter;
	BEGETLONG(vtmId, p);
	if(lpQueueData->vtmId != vtmId || 0 == vtmId)
	{
		dbgprint("AmsCustomerInQueueTimeoutProc[%d] Vtm[%s]Id[%u][%u]Err.", 
			pid, lpQueueData->vtmNo, lpQueueData->vtmId, vtmId);
		iret = 	AMS_CMS_GET_VTA_TIMEOUT_VTM_ID_ERR;
		AmsResultStatProc(AMS_CMS_GET_VTA_RESULT, iret);
		return AMS_ERROR;		
	}
			
    /* find Vtm Node in process */
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		pVtmNode = AmsSearchVtmNode(i, vtmId);
		if(NULL != pVtmNode)
		{
			break;
		}
	}
	
	if(NULL == pVtmNode)
	{
		dbgprint("AmsCustomerInQueueTimeoutProc[%d] Vtm[%s]Id[%u]Err", 
			pid, lpQueueData->vtmNo, vtmId);
		iret = 	AMS_CMS_GET_VTA_TIMEOUT_VTM_ID_ERR;
		AmsResultStatProc(AMS_CMS_GET_VTA_RESULT, iret);
		return AMS_ERROR;		
	}

    //check state
 	if(AMS_VTM_STATE_BUSY != pVtmNode->state)
	{
		dbgprint("AmsCustomerInQueueTimeoutProc[%d] Vtm[%s][%u]State[%d]Err", 
			pid, lpQueueData->vtmNo, vtmId, pVtmNode->state);
		iret = 	AMS_CMS_GET_VTA_TIMEOUT_STATE_ERR;
		AmsResultStatProc(AMS_CMS_GET_VTA_RESULT, iret);	
	}
	if(AMS_VTM_STATE_IDLE != pVtmNode->state)
	{
		//set Vtm State and State Start Time
		AmsSetVtmState(iThreadId, pVtmNode, AMS_VTM_STATE_IDLE);		
	}

	//check vtm serviceState
 	if(AMS_CUSTOMER_IN_QUEUE != pVtmNode->serviceState)
	{
		dbgprint("AmsCustomerInQueueTimeoutProc[%d] Vtm[%s][%u]ServiceState[%d]Err", 
			pid, lpQueueData->vtmNo, vtmId, pVtmNode->serviceState);
		iret = 	AMS_CMS_GET_VTA_TIMEOUT_SERVICE_STATE_ERR;
		AmsResultStatProc(AMS_CMS_GET_VTA_RESULT, iret);	
	}
	if(AMS_CUSTOMER_SERVICE_NULL != pVtmNode->serviceState)
	{
		//update Customer Service State
		AmsSetVtmServiceState(pVtmNode, AMS_CUSTOMER_SERVICE_NULL);
	}
	
	//update callState if need
	if(AMS_CALL_STATE_NULL != pVtmNode->callState)
	{
		//set Vtm Call State and State Start Time
		AmsSetVtmCallState(pVtmNode, AMS_CALL_STATE_NULL);
	}
	
	//reset amsPid
	pVtmNode->amsPid = 0;

	//reset customerPid
	pVtmNode->customerPid = 0;
		
	//send Cms Vta Get Rsp
	AmsSendCmsGetVtaTimeoutRsp(lpQueueData,&msg,iret);

	//release lpQueueData Pid
	AmsReleassPid(lpQueueData->myPid, END);

	return iret;

}



//zry added for scc 2018
int AmsUpdateSingleSeatWorkInfo(SEAT_NODE *pSeatNode, time_t currentTime)
{
	if(NULL == pSeatNode)
	{
		return AMS_ERROR;
	}
	
	if(currentTime > pSeatNode->stateStartTime && currentTime > pSeatNode->workInfoUpdateTime)
	{
		if(AMS_SEAT_STATE_BUSY == pSeatNode->state || AMS_SEAT_STATE_PREPARE == pSeatNode->state)
		{
			if(pSeatNode->workInfoUpdateTime > pSeatNode->stateStartTime)
			{
				pSeatNode->vtaWorkInfo.workSeconds += (currentTime - pSeatNode->workInfoUpdateTime);
			}
			else
			{
				pSeatNode->vtaWorkInfo.workSeconds += (currentTime - pSeatNode->stateStartTime);
			}
			
			pSeatNode->workInfoUpdateTime = currentTime;

			return AMS_OK;
		}
	
		if(AMS_SEAT_STATE_IDLE == pSeatNode->state || AMS_SEAT_STATE_REST == pSeatNode->state)
		{
			if(pSeatNode->workInfoUpdateTime > pSeatNode->stateStartTime)
			{
				pSeatNode->vtaWorkInfo.idleSeconds += (currentTime - pSeatNode->workInfoUpdateTime);
			}
			else
			{
				pSeatNode->vtaWorkInfo.idleSeconds += (currentTime - pSeatNode->stateStartTime);
			}
			
			pSeatNode->workInfoUpdateTime = currentTime;

			return AMS_OK;
		}
	}

	return AMS_ERROR;
}

//added end

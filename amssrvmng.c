#include "amsfunc.h"


static int 			amsDataInitialised = 0;
static int			amsCfgDataInitialised = 0;


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
	//AmsGetLicData();
	
    // init free vta list 
	if(AMS_SUCCESS != VtaListInit())
	{
		Display("AmsDataInit:Error-vta nodes error!\r\n");
    	return AMS_ERROR;
	}	    

    // init free vtm list 
	/*if(AMS_SUCCESS != VtmListInit())
	{
		Display("AmsDataInit:Error-vtm nodes error!\r\n");
    	return AMS_ERROR;
	}*/

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

		Sem_init(&AmsSrvData(i).vtmCtrl,0,1);

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


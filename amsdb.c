/******************************************************************
*
* FILE NAME						                           VERSION
*	
*	amsdb.c	                                               v1.0.0
*
* DESCRIPTION
*
*	AMS应用管理服务器与数据库交互处理函数
*
* COPYRIGHT
*	
*	Switch & Network Division, Eastern Communications Company Limited	
*
* HISTORY                                                        
*																  
*     NAME       DATE              REMARKS		            TARGET    
*    edward  	 2016-08-04        Created version 1.0.0    for VTC
******************************************************************/

#include "amsfunc.h"


//历史质检-文件记录
int AmsInsertDbFileUploaded(int iThreadId,unsigned char *pCallId,FILE_INFO *pFileInfo)
{
	int					iret = AMS_OK;
	char	            Sqlstr[MAXSQLLEN];
	unsigned short      seq = 0;
	
	if(NULL == pCallId || NULL == pFileInfo)
	{
		dbgprint("AmsInsertDbFileUploaded Para[0x%x][0x%x]Err", pCallId, pFileInfo);
		return iret;
	}

	AmsGetFileSeqFromFileInfo(pFileInfo, &seq);

	sprintf(Sqlstr,"insert into vtc_file(call_id,file_name,file_path,file_type,file_size,file_seq) VALUES('%s','%s','%s',%d,%d,%d)",
		pCallId,
		pFileInfo->fileName,
		pFileInfo->filePath,
		pFileInfo->fileType,
		pFileInfo->fileSize,
		seq);

	SendSqlBuffer(iThreadId,Sqlstr);
	
	return iret;	
}

//柜员实时状态
int AmsUpdateDbTellerState(int iThreadId,LP_AMS_DATA_t *lpAmsData,unsigned short amsTellerStateItem,int tellerState,time_t *pStateStartTime)
{
	int					 iret = AMS_OK;
	unsigned int	     vtcTellerState;	
	TELLER_STATE_DB_INFO tellerStateDbInfo;
	unsigned short       len = 0;
	char                 t0[128];	
		
	if(NULL == lpAmsData)
	{
		dbgprint("AmsUpdateDbTellerState Para[0x%x]Err", lpAmsData);
		iret = AMS_ERROR;
		return iret;
	}

	//pack data
	memset(&tellerStateDbInfo, 0, sizeof(TELLER_STATE_DB_INFO));

	//tellerId
	tellerStateDbInfo.tellerIdFlag = 1;
	memcpy(&tellerStateDbInfo.tellerId, &lpAmsData->tellerId, sizeof(DWORD));

	switch(amsTellerStateItem)
	{
	case AMS_TELLER_STATE_ITEM_BASE:

		if(NULL == pStateStartTime)
		{
			iret = AMS_ERROR;
			return iret;
		}
	
		if(AMS_OK != AmsTransform2VtcTellerState(tellerState, &vtcTellerState))
		{
			iret = AMS_ERROR;
			return iret;
		}
			
		memset(t0, 0, sizeof(t0));

		AmsTransformTime2Str(pStateStartTime, t0);


		//tellerState
		tellerStateDbInfo.tellerStateFlag = 1;
		tellerStateDbInfo.tellerState = vtcTellerState;

		//audrec_state
		tellerStateDbInfo.audioRecStateFlag = 1;
		tellerStateDbInfo.audioRecState = ((lpAmsData->audioRecState != 0) ? 1 : 0);

		//scrrec_state
		tellerStateDbInfo.screenRecStateFlag = 1;
		tellerStateDbInfo.screenRecState = ((lpAmsData->screenRecState != 0) ? 1 : 0);

		//remcoop_state
		tellerStateDbInfo.remCoopStateFlag = 1;
		tellerStateDbInfo.remCoopState = ((lpAmsData->rcasRemoteCoopState != 0 || lpAmsData->vtmRemoteCoopState != 0) ? 1 : 0);

		//snap_state
		tellerStateDbInfo.snapStateFlag = 1;
		tellerStateDbInfo.snapState = ((lpAmsData->snapState != 0) ? 1 : 0);

		//start_time
		tellerStateDbInfo.startTimeFlag = 1;
		memcpy(tellerStateDbInfo.startTime, t0, AMS_DB_TIME_LEN_MAX);

		break;
		
	case AMS_TELLER_AUDIO_REC_STATE:
		
		//tellerState
		tellerStateDbInfo.tellerStateFlag = 0;

		//audrec_state
		tellerStateDbInfo.audioRecStateFlag = 1;
		tellerStateDbInfo.audioRecState = ((lpAmsData->audioRecState != 0) ? 1 : 0);

		//scrrec_state
		tellerStateDbInfo.screenRecStateFlag = 0;

		//remcoop_state
		tellerStateDbInfo.remCoopStateFlag = 0;

		//snap_state
		tellerStateDbInfo.snapStateFlag = 0;

		//start_time
		tellerStateDbInfo.startTimeFlag = 0;
		
		break;

	case AMS_TELLER_SCREEN_REC_STATE:

		//tellerState
		tellerStateDbInfo.tellerStateFlag = 0;
 
		//audrec_state
		tellerStateDbInfo.audioRecStateFlag = 0;
 
		//scrrec_state
		tellerStateDbInfo.screenRecStateFlag = 1;
		tellerStateDbInfo.screenRecState = ((lpAmsData->screenRecState != 0) ? 1 : 0);

		//remcoop_state
		tellerStateDbInfo.remCoopStateFlag = 0;
 
		//snap_state
		tellerStateDbInfo.snapStateFlag = 0;
 
		//start_time
		tellerStateDbInfo.startTimeFlag = 0;
 		
		break;

	case AMS_TELLER_REMOTE_COOP_STATE:

		//tellerState
		tellerStateDbInfo.tellerStateFlag = 0;
 
		//audrec_state
		tellerStateDbInfo.audioRecStateFlag = 0;
 
		//scrrec_state
		tellerStateDbInfo.screenRecStateFlag = 0;
 
		//remcoop_state
		tellerStateDbInfo.remCoopStateFlag = 1;
		tellerStateDbInfo.remCoopState = ((lpAmsData->rcasRemoteCoopState != 0 || lpAmsData->vtmRemoteCoopState != 0) ? 1 : 0);

		//snap_state
		tellerStateDbInfo.snapStateFlag = 0;
 
		//start_time
		tellerStateDbInfo.startTimeFlag = 0;
 		
		break;

	case AMS_TELLER_SNAP_STATE:

		//tellerState
		tellerStateDbInfo.tellerStateFlag = 0;
 
		//audrec_state
		tellerStateDbInfo.audioRecStateFlag = 0;
 
		//scrrec_state
		tellerStateDbInfo.screenRecStateFlag = 0;
 
		//remcoop_state
		tellerStateDbInfo.remCoopStateFlag = 0;
 
		//snap_state
		tellerStateDbInfo.snapStateFlag = 1;
		tellerStateDbInfo.snapState = ((lpAmsData->snapState != 0) ? 1 : 0);

		//start_time
		tellerStateDbInfo.startTimeFlag = 0;
		
		break;
		
	default:
		iret = AMS_ERROR;
		break;
	} 

	AmsSendtellerStateToOms(iThreadId, &tellerStateDbInfo);

	return iret;
}

//柜员状态记录
int AmsInsertDbTellerSerialState(int iThreadId,LP_AMS_DATA_t *lpAmsData,unsigned int tellerState,TIME_INFO *pTimeInfo, int lastStateKeepTime)
{
	int					iret = AMS_OK;
	char	            Sqlstr[MAXSQLLEN];
	char                t0[128]; //state start time & last state end time
	unsigned int        vtcTellerState = 0;
	int                 tellerLastState = 0;
		
	if(NULL == lpAmsData || NULL == pTimeInfo)
	{
		dbgprint("AmsInsertDbTellerSerialState Para[0x%x][0x%x] Err", lpAmsData, pTimeInfo);
		iret = AMS_ERROR;
		return iret;
	}
	
	memset(t0, 0, sizeof(t0));

	AmsGetTime2Str(pTimeInfo, t0);

	if(AMS_OK != AmsTransform2VtcTellerState(tellerState, &vtcTellerState))
	{
		iret = AMS_ERROR;
		return iret;		
	}

/*		
	sprintf(Sqlstr,"insert into vtc_teller_state_record(teller_id,vtm_id,call_id,last_state,state,start_time,end_time,use_time) VALUES(%d,%d,'%s',%d,%d,to_date('%s','YYYY-MM-DD HH24:MI:SS'),to_date('%s','YYYY-MM-DD HH24:MI:SS'),%d)",
		lpAmsData->tellerId,
		lpAmsData->vtmId,
		lpAmsData->callId,
		0,                 //not add 
		vtcTellerState,
		t0,
		t1,                //not add 
		lastStateKeepTime);//not add 
*/	

	sprintf(Sqlstr,"insert into vtc_teller_state_record(teller_id,vtm_id,call_id,state,start_time) VALUES(%d,%d,'%s',%d,to_date('%s','YYYY-MM-DD HH24:MI:SS'))",
		lpAmsData->tellerId,
		lpAmsData->vtmId,
		lpAmsData->callId,
		vtcTellerState,
		t0);
	
	SendSqlBuffer(iThreadId,Sqlstr);

	return iret;
}

//柜员机实时状态
int AmsUpdateDbVtmState(int iThreadId,VTM_NODE *pVtmNode,int vtmState)
{
	int					iret = AMS_OK;
	LP_AMS_DATA_t		*lpAmsData = NULL;               //进程数据区指针	
	VTM_STATE_DB_INFO   vtmStateDbInfo;
	unsigned short      len = 0;	
	char                t0[128];	
	time_t              currentTime;
	int                 pid = 0;
	char                getCallId = 0;

	if(NULL == pVtmNode)
	{
		dbgprint("AmsUpdateDbVtmState Para[0x%x] Err", pVtmNode);
		iret = AMS_ERROR;
		return iret;
	}
	
	time(&currentTime);		
	memset(t0, 0, sizeof(t0));

	AmsTransformTime2Str(&currentTime, t0);
	
	if(AMS_CUSTOMER_IN_QUEUE == pVtmNode->serviceState || AMS_CUSTOMER_IN_SERVICE == pVtmNode->serviceState)
	{
		pid = pVtmNode->amsPid & 0xffff;
		if(pid  > 0 && pid  < LOGIC_PROCESS_SIZE)
		{
			lpAmsData = (LP_AMS_DATA_t *)ProcessData[pid];

			if(AMS_MAX_CALLID_LEN == lpAmsData->callIdLen)
			{
				getCallId = 1;
			}
		}
	}

	//pack data
	memset(&vtmStateDbInfo, 0, sizeof(VTM_STATE_DB_INFO));	

	vtmStateDbInfo.vtmIdFlag = 1;
	memcpy(&vtmStateDbInfo.vtmId, &pVtmNode->vtmInfo.vtmId, sizeof(DWORD));


	vtmStateDbInfo.vtmStateFlag = 1;
	vtmStateDbInfo.vtmState = vtmState;

	//call_id
	if(1 == getCallId)
	{
		vtmStateDbInfo.callIdFlag = 1;
		memcpy(vtmStateDbInfo.callId, lpAmsData->callId, lpAmsData->callIdLen);
	}
	else
	{
		vtmStateDbInfo.callIdFlag = 0;
	}

	vtmStateDbInfo.startTimeFlag = 1;
	memcpy(vtmStateDbInfo.startTime, t0, AMS_DB_TIME_LEN_MAX);

	AmsSendvtmStateToOms(iThreadId, &vtmStateDbInfo);

	return iret;
}

//柜员终端流量
int AmsInsertDbTellerNetFlow(int iThreadId,unsigned char *pCallId,unsigned int tellerId,BANDWIDTH_PARA *pTellerBw)
{
	int					iret = AMS_OK;
	char	            Sqlstr[MAXSQLLEN];
	char                t0[128]; //start time
	time_t              currentTime;

	if(NULL == pCallId || NULL == pTellerBw)
	{
		dbgprint("AmsInsertDbTellerNetFlow Para[0x%x][0x%x] Err", pCallId, pTellerBw);
		iret = AMS_ERROR;
		return iret;
	}

	time(&currentTime);		
	memset(t0, 0, sizeof(t0));

	AmsTransformTime2Str(&currentTime, t0);
	
	sprintf(Sqlstr,"insert into vtc_teller_netflow(teller_id,call_id,tx_total_bit_used,rx_total_bit_used,tx_dur_bit_used,rx_dur_bit_used,duration,start_time) VALUES(%d,'%s',%d,%d,%d,%d,%d,to_date('%s','YYYY-MM-DD HH24:MI:SS'))",
		tellerId,
		pCallId,
		pTellerBw->txTotalBitUsed,
		pTellerBw->rxTotalBitUsed,
		pTellerBw->txDurationBitUsed,
		pTellerBw->rxDurationBitUsed,
		pTellerBw->durationValue,
		t0);

	SendSqlBuffer(iThreadId,Sqlstr);

	return iret;
}

//柜员终端实时流量
int AmsUpdateDbTellerRealNetFlow(int iThreadId,unsigned char *pCallId,unsigned int tellerId,BANDWIDTH_PARA *pTellerBw)
{
	int					iret = AMS_OK;
	char	            Sqlstr[MAXSQLLEN];
	char                t0[128]; //start time
	TELLER_RNF_DB_INFO  tellerRealNetFlowDbInfo;
	unsigned int        callIdLen;
	time_t              currentTime;

	if(NULL == pCallId || NULL == pTellerBw)
	{
		dbgprint("AmsUpdateDbTellerRealNetFlow Para[0x%x][0x%x] Err", pCallId, pTellerBw);
		return AMS_ERROR;
	}

	callIdLen = strlen(pCallId);
	if(callIdLen > AMS_MAX_CALLID_LEN)
	{
		dbgprint("AmsUpdateDbTellerRealNetFlow callIdLen[%d] Err", callIdLen);
		return AMS_ERROR;		
	}
	
	time(&currentTime);		
	memset(t0, 0, sizeof(t0));

	AmsTransformTime2Str(&currentTime, t0);

	//pack data
	memset(&tellerRealNetFlowDbInfo, 0, sizeof(TELLER_RNF_DB_INFO));	

	//tellerId
	tellerRealNetFlowDbInfo.tellerIdFlag = 1;
	tellerRealNetFlowDbInfo.tellerId = tellerId;

	//call_id
	tellerRealNetFlowDbInfo.callIdFlag = 1;
	memcpy(tellerRealNetFlowDbInfo.callId, pCallId, callIdLen);

	//txBitUsed
	tellerRealNetFlowDbInfo.txBitUsedFlag = 1;
	tellerRealNetFlowDbInfo.txBitUsed = pTellerBw->txDurationBitUsed;

	//rxBitUsed
	tellerRealNetFlowDbInfo.rxBitUsedFlag = 1;
	tellerRealNetFlowDbInfo.rxBitUsed = pTellerBw->rxDurationBitUsed;

	//duration
	tellerRealNetFlowDbInfo.durationFlag = 1;
	tellerRealNetFlowDbInfo.duration = pTellerBw->durationValue;
	
	//startTime
	tellerRealNetFlowDbInfo.startTimeFlag = 1;
	memcpy(tellerRealNetFlowDbInfo.startTime, t0, AMS_DB_TIME_LEN_MAX);

	AmsSendTellerRealNetFlowToOms(iThreadId, &tellerRealNetFlowDbInfo);

	return iret;
}

//柜员机终端流量
int AmsInsertDbVtmNetFlow(int iThreadId,unsigned char *pCallId,unsigned int vtmId,BANDWIDTH_PARA *pVtmBw)
{
	int					iret = AMS_OK;
	char	            Sqlstr[MAXSQLLEN];
	char                t0[128]; //start time
	time_t              currentTime;

	if(NULL == pCallId || NULL == pVtmBw)
	{
		dbgprint("AmsInsertDbVtmNetFlow Para[0x%x][0x%x] Err", pCallId, pVtmBw);
		iret = AMS_ERROR;
		return iret;
	}

	time(&currentTime);		
	
	memset(t0, 0, sizeof(t0));
	
	AmsTransformTime2Str(&currentTime, t0);
	
	sprintf(Sqlstr,"insert into vtc_vtm_netflow(vtm_id,call_id,tx_total_bit_used,rx_total_bit_used,tx_dur_bit_used,rx_dur_bit_used,duration,start_time) VALUES(%d,'%s',%d,%d,%d,%d,%d,to_date('%s','YYYY-MM-DD HH24:MI:SS'))",
		vtmId,
		pCallId,
		pVtmBw->txTotalBitUsed,
		pVtmBw->rxTotalBitUsed,		
		pVtmBw->txDurationBitUsed,
		pVtmBw->rxDurationBitUsed,
		pVtmBw->durationValue,
		t0);

	SendSqlBuffer(iThreadId,Sqlstr);

	return iret;
}

//柜员机终端实时流量
int AmsUpateDbVtmRealNetFlow(int iThreadId,unsigned char *pCallId,unsigned int vtmId,BANDWIDTH_PARA *pVtmBw)
{
	int					iret = AMS_OK;
	char	            Sqlstr[MAXSQLLEN];
	char                t0[128]; //start time
	VTM_RNF_DB_INFO     vtmRealNetFlowDbInfo;
	unsigned int        callIdLen;
	time_t              currentTime;

	if(NULL == pCallId || NULL == pVtmBw)
	{
		dbgprint("AmsUpateDbVtmRealNetFlow Para[0x%x][0x%x] Err", pCallId, pVtmBw);
		return AMS_ERROR;
	}

	callIdLen = strlen(pCallId);
	if(callIdLen > AMS_MAX_CALLID_LEN)
	{
		dbgprint("AmsUpateDbVtmRealNetFlow callIdLen[%d] Err", callIdLen);
		return AMS_ERROR;		
	}
	
	time(&currentTime);		
	memset(t0, 0, sizeof(t0));

	AmsTransformTime2Str(&currentTime, t0);

	//pack data
	memset(&vtmRealNetFlowDbInfo, 0, sizeof(VTM_RNF_DB_INFO));	

	//vtmId
	vtmRealNetFlowDbInfo.vtmIdFlag = 1;
	vtmRealNetFlowDbInfo.vtmId = vtmId;

	//call_id
	vtmRealNetFlowDbInfo.callIdFlag = 1;
	memcpy(vtmRealNetFlowDbInfo.callId, pCallId, callIdLen);

	//txBitUsed
	vtmRealNetFlowDbInfo.txBitUsedFlag = 1;
	vtmRealNetFlowDbInfo.txBitUsed = pVtmBw->txDurationBitUsed;

	//rxBitUsed
	vtmRealNetFlowDbInfo.rxBitUsedFlag = 1;
	vtmRealNetFlowDbInfo.rxBitUsed = pVtmBw->rxDurationBitUsed;

	//duration
	vtmRealNetFlowDbInfo.durationFlag = 1;
	vtmRealNetFlowDbInfo.duration = pVtmBw->durationValue;
	
	//startTime
	vtmRealNetFlowDbInfo.startTimeFlag = 1;
	memcpy(vtmRealNetFlowDbInfo.startTime, t0, AMS_DB_TIME_LEN_MAX);

	AmsSendVtmRealNetFlowToOms(iThreadId, &vtmRealNetFlowDbInfo);

	return iret;
}

//业务详单
int AmsInsertDbServiceSDR(int iThreadId,unsigned short amsSdrItem,LP_AMS_DATA_t *lpAmsData,unsigned char *pCallId,unsigned char callIdLen,unsigned int termId,BANDWIDTH_PARA *pTermBw)
{
	int					iret = AMS_OK;
	AMS_SDR             amsSdr;

	switch(amsSdrItem)
	{
	case AMS_SDR_ITEM_BASE:
		AmsSendSDRBaseItem(iThreadId, lpAmsData, pCallId, callIdLen, termId, pTermBw);
		break;
		
	case AMS_SDR_TELLER_NETFLOW:
		AmsSendSDRTellerNetFlow(iThreadId, pCallId, callIdLen, termId, pTermBw);
		break;

	case AMS_SDR_VTM_NETFLOW:
		AmsSendSDRVtmNetFlow(iThreadId, pCallId, callIdLen, termId, pTermBw);
		break;

	default:
		iret = AMS_ERROR;
		break;
	}

	return iret;
	
}

int AmsGetTime2Str(TIME_INFO *pTimeInfo,char *pStrBuf)
{
	int                 len = 0;

	if((NULL == pTimeInfo) || (NULL == pStrBuf))
	{
		dbgprint("AmsGetTime2Str Para[0x%x][0x%x] Err", pTimeInfo, pStrBuf);
		return -1;
	}
		
	len = sprintf(pStrBuf,"%04d%02d%02d %02d%02d%02d",
		pTimeInfo->year,pTimeInfo->month,pTimeInfo->day,pTimeInfo->hour,pTimeInfo->minute,pTimeInfo->second);

	return len;
	
}

int AmsTransformTime2Str(time_t	*ttime,char *pStrBuf)
{
	int                 len = 0;
	struct tm           *ptm;

	if((NULL == ttime) || (NULL == pStrBuf))
	{
		dbgprint("AmsTransformTime2Str Para[0x%x][0x%x] Err", ttime, pStrBuf);
		return -1;
	}

	ptm = localtime(ttime);
		
	len = sprintf(pStrBuf,"%04d%02d%02d %02d%02d%02d",
		ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec);

	return len;
	
}

int AmsTransform2VtcTellerState(unsigned int amsTellerState,unsigned int *pVtcTellerState)
{
	int					iret = AMS_OK;

	if(amsTellerState >= AMS_VTA_STATE_RSVD)
	{
		dbgprint("Ams Transform2VtcTellerState Para[%d] Err", amsTellerState);
		iret = AMS_ERROR;
		return iret;
	}
	
	switch(amsTellerState)
	{
	case AMS_VTA_STATE_IDLE:
 		*pVtcTellerState = AMS_VTC_TELLER_STATE_IDLE;	
		break;	
	case AMS_VTA_STATE_BUSY:
 		*pVtcTellerState = AMS_VTC_TELLER_STATE_BUSY;
		break;
	case AMS_VTA_STATE_REST:
		*pVtcTellerState = AMS_VTC_TELLER_STATE_REST;			
		break;	
	case AMS_VTA_STATE_PREPARE:
		*pVtcTellerState = AMS_VTC_TELLER_STATE_PREPARE;			
		break;			
	case AMS_VTA_STATE_OFFLINE:
		*pVtcTellerState = AMS_VTC_TELLER_STATE_OFFLINE;			
		break;	
						
	default:
		dbgprint("Ams Transform2VtcTellerState[%d]Err", amsTellerState);
		iret = AMS_ERROR;
		break;
	}
	
	return iret;
	
}

int AmsTransformVtc2AmsTellerState(unsigned int vtcTellerState,unsigned int *pAmsTellerState)
{
	int					iret = AMS_OK;

	if(vtcTellerState >= AMS_VTC_TELLER_STATE_RSVD)
	{
		dbgprint("Ams TransformVtc2AmsTellerState Para[%d] Err", vtcTellerState);
		iret = AMS_ERROR;
		return iret;
	}
	
	switch(vtcTellerState)
	{
	case AMS_VTC_TELLER_STATE_IDLE:
 		*pAmsTellerState = AMS_VTA_STATE_IDLE;	
		break;	
	case AMS_VTC_TELLER_STATE_BUSY:
 		*pAmsTellerState = AMS_VTA_STATE_BUSY;
		break;
	case AMS_VTC_TELLER_STATE_REST:
		*pAmsTellerState = AMS_VTA_STATE_REST;			
		break;	
	case AMS_VTC_TELLER_STATE_PREPARE:
		*pAmsTellerState = AMS_VTA_STATE_PREPARE;			
		break;			
	case AMS_VTC_TELLER_STATE_OFFLINE:
		*pAmsTellerState = AMS_VTA_STATE_OFFLINE;			
		break;	
				
	default:
		dbgprint("Ams TransformVtc2AmsTellerState[%d]Err", vtcTellerState);
		iret = AMS_ERROR;
		break;
	}

	return iret;
	
}

int AmsGetFileSeqFromFileInfo(FILE_INFO *pFileInfo, WORD *pSeq)
{
	int					iret = AMS_OK;
	char                seq[AMS_FILE_SEQ_LEN + 1] = {0};
	unsigned short      len = AMS_MAX_CALLID_LEN + 1 + AMS_FILE_TIME_LEN + 1 + AMS_FILE_SEQ_LEN + 1 + 3;

	if(NULL == pFileInfo || NULL == pSeq)
	{
		dbgprint("AmsGetFileSeqFromFileInfo Para[0x%x][0x%x] Err", pFileInfo, pSeq);
		iret = AMS_ERROR;
		return iret;
	}

	if(pFileInfo->fileNameLen < len)
	{
		iret = AMS_ERROR;		
		return iret;		
	}

	len = AMS_MAX_CALLID_LEN + 1 + AMS_FILE_TIME_LEN + 1;
	memcpy(seq, &pFileInfo->fileName[len], AMS_FILE_SEQ_LEN);
	
	*pSeq = atoi(seq);

	return iret;
	
}

int AmsSendSDRBaseItem(int iThreadId,LP_AMS_DATA_t *lpAmsData,unsigned char *pCallId,unsigned char callIdLen,unsigned int termId,BANDWIDTH_PARA *pTermBw)
{
	int					iret = AMS_OK;	
	AMS_SDR             amsSdr;

	if(NULL == lpAmsData)
	{
		dbgprint("AmsSendSDRBaseItem Para[0x%x] Err", lpAmsData);
		iret = AMS_ERROR;
		return iret;
	}
	
	if(   lpAmsData->tellerPos >= AMS_MAX_VTA_NUM 
	   || lpAmsData->vtmPos >= AMS_MAX_VTM_NUM
	   || lpAmsData->srvGrpId > AMS_SERVICE_GROUP_ID_MAX)
	{
		dbgprint("AmsSendSDRBaseItem Para[%d][%u][%u] Err", 
			lpAmsData->tellerPos, lpAmsData->vtmPos, lpAmsData->srvGrpId);
		iret = AMS_ERROR;
		return iret;
	}

	//pack data
	memset(&amsSdr, 0, sizeof(AMS_SDR));	
	
	//call_id
	amsSdr.callIdFlag = 1;
	if(lpAmsData->callIdLen > AMS_MAX_CALLID_LEN)
	{
		amsSdr.callIdLen = AMS_MAX_CALLID_LEN;		
	}
	else
	{
		amsSdr.callIdLen = lpAmsData->callIdLen;					
	}		
	memcpy(amsSdr.callId, lpAmsData->callId, amsSdr.callIdLen);

	//teller_id
	amsSdr.tellerIdFlag = 1;
	memcpy(&amsSdr.tellerId, &lpAmsData->tellerId, sizeof(DWORD));


	//teller_no
	amsSdr.tellerNoFlag = 1;
	if(lpAmsData->tellerNoLen > AMS_MAX_TELLER_NO_LEN)
	{
		amsSdr.tellerNoLen = AMS_MAX_TELLER_NO_LEN;		
	}
	else
	{
		amsSdr.tellerNoLen = lpAmsData->tellerNoLen;
							
	}
	memcpy(amsSdr.tellerNo, lpAmsData->tellerNo, amsSdr.tellerNoLen);	

	//teller_name
	amsSdr.tellerNameFlag = 1;
	if(AmsCfgTeller(lpAmsData->tellerPos).tellerNameLen > AMS_MAX_NAME_LEN)
	{
		amsSdr.tellerNameLen = AMS_MAX_NAME_LEN;		
	}
	else
	{
		amsSdr.tellerNameLen = AmsCfgTeller(lpAmsData->tellerPos).tellerNameLen;				
	}
	memcpy(amsSdr.tellerName, AmsCfgTeller(lpAmsData->tellerPos).tellerName, amsSdr.tellerNameLen);	

	//teller IP
	amsSdr.tellerIpFlag = 1;
	memcpy(amsSdr.tellerIp, AmsCfgTeller(lpAmsData->tellerPos).transIp, AMS_MAX_TRANS_IP_LEN);			

	//vtm_id
	amsSdr.vtmIdFlag = 1;
	memcpy(&amsSdr.vtmId, &lpAmsData->vtmId, sizeof(DWORD));

	//vtm_no
	amsSdr.vtmNoFlag = 1;
	if(AmsCfgVtm(lpAmsData->vtmPos).vtmNoLen > AMS_MAX_VTM_NO_LEN)
	{
		amsSdr.vtmNoLen = AMS_MAX_VTM_NO_LEN;			
	}
	else
	{
		amsSdr.vtmNoLen = AmsCfgVtm(lpAmsData->vtmPos).vtmNoLen;	
	}
	memcpy(amsSdr.vtmNo, AmsCfgVtm(lpAmsData->vtmPos).vtmNo, amsSdr.vtmNoLen);		
	
	//vtm IP
	amsSdr.vtmIpFlag = 1;
	memcpy(amsSdr.vtmIp, AmsCfgVtm(lpAmsData->vtmPos).transIp, AMS_MAX_TRANS_IP_LEN);			

	//skillGroupId
	amsSdr.skillGroupIdFlag = 1;
	memcpy(&amsSdr.skillGroupId, &lpAmsData->srvGrpId, sizeof(DWORD));

	//skillGroupName
	amsSdr.skillGroupNameFlag = 1;
	if(AmsCfgSrvGroup(lpAmsData->srvGrpId).srvGroupNameLen > AMS_MAX_SERVICE_GROUP_NAME_LEN)
	{
		amsSdr.skillGroupNameLen = AMS_MAX_SERVICE_GROUP_NAME_LEN;			
	}
	else
	{
		amsSdr.skillGroupNameLen = AmsCfgSrvGroup(lpAmsData->srvGrpId).srvGroupNameLen;	
	}		
	memcpy(amsSdr.skillGroupName, AmsCfgSrvGroup(lpAmsData->srvGrpId).srvGroupName, amsSdr.skillGroupNameLen);

	//audioRecNum
	amsSdr.audioRecNumFlag = 1;
	amsSdr.audioRecNum = lpAmsData->sessStat.audioRecNum;	

	//audioRecFailNum
	amsSdr.audioRecFailNumFlag = 1;
	amsSdr.audioRecFailNum = lpAmsData->sessStat.audioRecFailNum;	

	//screenRecNum
	amsSdr.screenRecNumFlag = 1;
	amsSdr.screenRecNum = lpAmsData->sessStat.scrRecNum;	

	//screenRecFailNum
	amsSdr.screenRecFailNumFlag = 1;
	amsSdr.screenRecFailNum = lpAmsData->sessStat.scrRecFailNum;
	
	//remCoopNum
	amsSdr.remCoopNumFlag = 1;
	amsSdr.remCoopNum = lpAmsData->sessStat.remCoopNum;	

	//remCoopFailNum
	amsSdr.remCoopFailNumFlag = 1;
	amsSdr.remCoopFailNum = lpAmsData->sessStat.remCoopFailNum;

	//snapNum
	amsSdr.snapNumFlag = 1;
	amsSdr.snapNum = lpAmsData->sessStat.snapNum;	

	//snapFailNum
	amsSdr.snapFailNumFlag = 1;
	amsSdr.snapFailNum = lpAmsData->sessStat.snapFailNum;

	//score
	if(1 == lpAmsData->sessStat.scoreFlag)
	{
		amsSdr.scoreFlag = 1;
		amsSdr.score = (unsigned char)lpAmsData->sessStat.score;
	}
	else
	{
		amsSdr.scoreFlag = 0;		
	}
	

	//txSessionFlow
	amsSdr.txSessionFlowFlag = 0;

	//rxSessionFlow
	amsSdr.rxSessionFlowFlag = 0;

	//txFileFlow
	amsSdr.txFileFlowFlag = 0;

	//rxFileFlow
	amsSdr.rxFileFlowFlag = 0;		

	//txDesktopFlow
	amsSdr.txDesktopFlowFlag = 0;

	//rxDesktopFlow
	amsSdr.rxDesktopFlowFlag = 0;


	//vtmTxSessionFlow
	amsSdr.vtmTxSessionFlowFlag = 0;

	//vtmRxSessionFlow
	amsSdr.vtmRxSessionFlowFlag = 0;

	//vtmTxFileFlow
	amsSdr.vtmTxFileFlowFlag = 0;

	//vtmRxFileFlow
	amsSdr.vtmRxFileFlowFlag = 0;		

	//vtmTxDesktopFlow
	amsSdr.vtmTxDesktopFlowFlag = 0;

	//vtmRxDesktopFlow
	amsSdr.vtmRxDesktopFlowFlag = 0;


	AmsSendSdrToOms(iThreadId, &amsSdr);	

	return iret;	
	
}

int AmsSendSDRTellerNetFlow(int iThreadId,unsigned char *pCallId,unsigned char callIdLen,unsigned int termId,BANDWIDTH_PARA *pTermBw)
{
	int					iret = AMS_OK;	
	AMS_SDR             amsSdr;
	
	if(NULL == pCallId || NULL == pTermBw)
	{
		dbgprint("AmsSendSDRTellerNetFlow Para[0x%x][0x%x] Err", pCallId, pTermBw);		
		iret = AMS_ERROR;
		return iret;
	}	

	//pack data
	memset(&amsSdr, 0, sizeof(AMS_SDR));	
	
	//call_id
	amsSdr.callIdFlag = 1;
	if(callIdLen > AMS_MAX_CALLID_LEN)
	{
		amsSdr.callIdLen = AMS_MAX_CALLID_LEN;		
	}
	else
	{
		amsSdr.callIdLen = callIdLen;					
	}		
	memcpy(amsSdr.callId, pCallId, amsSdr.callIdLen);

	//teller_id
	amsSdr.tellerIdFlag = 1;
	memcpy(&amsSdr.tellerId, &termId, sizeof(DWORD));

	//teller_no
	amsSdr.tellerNoFlag = 0;

	//teller_name
	amsSdr.tellerNameFlag = 0;

	//teller IP
	amsSdr.tellerIpFlag = 0;

	//vtm_id
	amsSdr.vtmIdFlag = 0;

	//vtm_no
	amsSdr.vtmNoFlag = 0;

	//vtm IP
	amsSdr.vtmIpFlag = 0;

	//skillGroupId
	amsSdr.skillGroupIdFlag = 0;

	//skillGroupName
	amsSdr.skillGroupNameFlag = 0;

	//audioRecNum
	amsSdr.audioRecNumFlag = 0;

	//audioRecFailNum
	amsSdr.audioRecFailNumFlag = 0;

	//screenRecNum
	amsSdr.screenRecNumFlag = 0;

	//screenRecFailNum
	amsSdr.screenRecFailNumFlag = 0;

	//remCoopNum
	amsSdr.remCoopNumFlag = 0;

	//remCoopFailNum
	amsSdr.remCoopFailNumFlag = 0;

	//snapNum
	amsSdr.snapNumFlag = 0;

	//snapFailNum
	amsSdr.snapFailNumFlag = 0;

	//score
	amsSdr.scoreFlag = 0;


	//txSessionFlow
	amsSdr.txSessionFlowFlag = 1;
	memcpy(&amsSdr.txSessionFlow, &pTermBw->txLatestSessionBitUsed, sizeof(DWORD));		

	//rxSessionFlow
	amsSdr.rxSessionFlowFlag = 1;
	memcpy(&amsSdr.rxSessionFlow, &pTermBw->rxLatestSessionBitUsed, sizeof(DWORD));		

	//txFileFlow
	amsSdr.txFileFlowFlag = 1;
	memcpy(&amsSdr.txFileFlow, &pTermBw->rxLatestSessionBitUsed, sizeof(DWORD));		

	//rxFileFlow
	amsSdr.rxFileFlowFlag = 1;	
	memcpy(&amsSdr.rxFileFlow, &pTermBw->rxLatestFileBitUsed, sizeof(DWORD));		

	//txDesktopFlow
	amsSdr.txDesktopFlowFlag = 1;
	memcpy(&amsSdr.txDesktopFlow, &pTermBw->txLatestRemoteDesktopBitUsed, sizeof(DWORD));		

	//rxDesktopFlow
	amsSdr.rxDesktopFlowFlag = 1;
	memcpy(&amsSdr.rxDesktopFlow, &pTermBw->rxLatestRemoteDesktopBitUsed, sizeof(DWORD));		


	//vtmTxSessionFlow
	amsSdr.vtmTxSessionFlowFlag = 0;

	//vtmRxSessionFlow
	amsSdr.vtmRxSessionFlowFlag = 0;

	//vtmTxFileFlow
	amsSdr.vtmTxFileFlowFlag = 0;

	//vtmRxFileFlow
	amsSdr.vtmRxFileFlowFlag = 0;		

	//vtmTxDesktopFlow
	amsSdr.vtmTxDesktopFlowFlag = 0;

	//vtmRxDesktopFlow
	amsSdr.vtmRxDesktopFlowFlag = 0;


	AmsSendSdrToOms(iThreadId, &amsSdr);	

	return iret;
	
}	

int AmsSendSDRVtmNetFlow(int iThreadId,unsigned char *pCallId,unsigned char callIdLen,unsigned int termId,BANDWIDTH_PARA *pTermBw)
{
	int					iret = AMS_OK;	
	AMS_SDR             amsSdr;
	
	if(NULL == pCallId || NULL == pTermBw)
	{
		dbgprint("AmsSendSDRVtmNetFlow Para[0x%x][0x%x] Err", pCallId, pTermBw);	
		iret = AMS_ERROR;
		return iret;
	}		

	//pack data
	memset(&amsSdr, 0, sizeof(AMS_SDR));	
		
	//call_id
	amsSdr.callIdFlag = 1;
	if(callIdLen > AMS_MAX_CALLID_LEN)
	{
		amsSdr.callIdLen = AMS_MAX_CALLID_LEN;		
	}
	else
	{
		amsSdr.callIdLen = callIdLen;					
	}		
	memcpy(amsSdr.callId, pCallId, amsSdr.callIdLen);

	//teller_id
	amsSdr.tellerIdFlag = 1;
	memcpy(&amsSdr.tellerId, &termId, sizeof(DWORD));

	//teller_no
	amsSdr.tellerNoFlag = 0;

	//teller_name
	amsSdr.tellerNameFlag = 0;

	//teller IP
	amsSdr.tellerIpFlag = 0;

	//vtm_id
	amsSdr.vtmIdFlag = 0;

	//vtm_no
	amsSdr.vtmNoFlag = 0;

	//vtm IP
	amsSdr.vtmIpFlag = 0;

	//skillGroupId
	amsSdr.skillGroupIdFlag = 0;

	//skillGroupName
	amsSdr.skillGroupNameFlag = 0;

	//audioRecNum
	amsSdr.audioRecNumFlag = 0;

	//audioRecFailNum
	amsSdr.audioRecFailNumFlag = 0;

	//screenRecNum
	amsSdr.screenRecNumFlag = 0;

	//screenRecFailNum
	amsSdr.screenRecFailNumFlag = 0;

	//remCoopNum
	amsSdr.remCoopNumFlag = 0;

	//remCoopFailNum
	amsSdr.remCoopFailNumFlag = 0;

	//snapNum
	amsSdr.snapNumFlag = 0;

	//snapFailNum
	amsSdr.snapFailNumFlag = 0;

	//score
	amsSdr.scoreFlag = 0;


	//txSessionFlow
	amsSdr.txSessionFlowFlag = 0;

	//rxSessionFlow
	amsSdr.rxSessionFlowFlag = 0;

	//txFileFlow
	amsSdr.txFileFlowFlag = 0;

	//rxFileFlow
	amsSdr.rxFileFlowFlag = 0;		

	//txDesktopFlow
	amsSdr.txDesktopFlowFlag = 0;

	//rxDesktopFlow
	amsSdr.rxDesktopFlowFlag = 0;


	//vtmTxSessionFlow
	amsSdr.vtmTxSessionFlowFlag = 1;
	memcpy(&amsSdr.vtmTxSessionFlow, &pTermBw->txLatestSessionBitUsed, sizeof(DWORD));		

	//vtmRxSessionFlow
	amsSdr.vtmRxSessionFlowFlag = 1;
	memcpy(&amsSdr.vtmRxSessionFlow, &pTermBw->rxLatestSessionBitUsed, sizeof(DWORD));		

	//vtmTxFileFlow
	amsSdr.vtmTxFileFlowFlag = 1;
	memcpy(&amsSdr.vtmTxFileFlow, &pTermBw->txLatestFileBitUsed, sizeof(DWORD));		

	//vtmRxFileFlow
	amsSdr.vtmRxFileFlowFlag = 1;	
	memcpy(&amsSdr.vtmRxFileFlow, &pTermBw->rxLatestFileBitUsed, sizeof(DWORD));		

	//vtmTxDesktopFlow
	amsSdr.vtmTxDesktopFlowFlag = 1;
	memcpy(&amsSdr.vtmTxDesktopFlow, &pTermBw->txLatestRemoteDesktopBitUsed, sizeof(DWORD));		

	//vtmRxDesktopFlow
	amsSdr.vtmRxDesktopFlowFlag = 1;
	memcpy(&amsSdr.vtmRxDesktopFlow, &pTermBw->rxLatestRemoteDesktopBitUsed, sizeof(DWORD));		


	AmsSendSdrToOms(iThreadId, &amsSdr);	

	return iret;
	
}


//zry added for scc 2018
//用户实时状态
int AmsUpdateDbUserState(int iThreadId,USER_NODE *pUserNode,int userState)
{
	int					iret = AMS_OK;
	LP_AMS_DATA_t		*lpAmsData = NULL;               //进程数据区指针	
	USER_STATE_DB_INFO   userStateDbInfo;
	unsigned short      len = 0;	
	char                t0[128];	
	time_t              currentTime;
	int                 pid = 0;
	char                getCallId = 0;
/*
	if(NULL == pUserNode)
	{
		dbgprint("AmsUpdateDbVtmState Para[0x%x] Err", pUserNode);
		iret = AMS_ERROR;
		return iret;
	}
	
	time(&currentTime);		
	memset(t0, 0, sizeof(t0));

	AmsTransformTime2Str(&currentTime, t0);
	
	if(AMS_CUSTOMER_IN_QUEUE == pUserNode->serviceState || AMS_CUSTOMER_IN_SERVICE == pUserNode->serviceState)
	{
		pid = pUserNode->amsPid & 0xffff;
		if(pid  > 0 && pid  < LOGIC_PROCESS_SIZE)
		{
			lpAmsData = (LP_AMS_DATA_t *)ProcessData[pid];

			if(AMS_MAX_CALLID_LEN == lpAmsData->callIdLen)
			{
				getCallId = 1;
			}
		}
	}

	//pack data
	memset(&vtmStateDbInfo, 0, sizeof(VTM_STATE_DB_INFO));	

	vtmStateDbInfo.vtmIdFlag = 1;
	memcpy(&vtmStateDbInfo.vtmId, &pUserNode->vtmInfo.vtmId, sizeof(DWORD));


	vtmStateDbInfo.vtmStateFlag = 1;
	vtmStateDbInfo.vtmState = vtmState;

	//call_id
	if(1 == getCallId)
	{
		vtmStateDbInfo.callIdFlag = 1;
		memcpy(vtmStateDbInfo.callId, lpAmsData->callId, lpAmsData->callIdLen);
	}
	else
	{
		vtmStateDbInfo.callIdFlag = 0;
	}

	vtmStateDbInfo.startTimeFlag = 1;
	memcpy(vtmStateDbInfo.startTime, t0, AMS_DB_TIME_LEN_MAX);

	AmsSendvtmStateToOms(iThreadId, &vtmStateDbInfo);
*/
	return iret;
	
}

//added end


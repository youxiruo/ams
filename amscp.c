
#include "amsfunc.h"


int VtaRegReqProc(int iThreadId, MESSAGE_t *pMsg)
{
	int					 iret = AMS_CMS_PRCOESS_SUCCESS;
	unsigned char         tellerIdLen = 0;	
	unsigned char        tellerId[AMS_MAX_TELLER_ID_LEN + 1] = { 0 };	
	TERM_NET_INFO        tellerNetInfo;
	VTA_NODE             *pVtaNode = NULL;	
	unsigned char		 newTransIpLen = 0;
	unsigned char        newTransIp[AMS_MAX_TRANS_IP_LEN + 1]; //柜员IP地址字符串
	unsigned int         vtaIp = 0;
	unsigned char        transIpChange = 0;
	unsigned int         tellerCfgPos = 0,tellerRegPos=0;
	int                  pid = 0;	
	int                  pos = 0;
	int                  i = 0;
	int                  j = 0;
	unsigned char        *p;	
	
	TELLER_INFO_NODE	*pTellerInfoNode = NULL;
	TELLER_REGISTER_INFO_NODE  *pRegTellerInfoNode = NULL;


	//get remote pid
	pid = pMsg->s_SenderPid.iProcessId;

	//检查接受进程号
	if(pMsg->s_ReceiverPid.iProcessId != 0)
	{
		dbgprint("VtaRegReqProc[%d] Pid:%d Err", pid, pMsg->s_ReceiverPid.iProcessId);
		iret = AMS_CMS_VTA_REG_PARA_ERR;
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);

		return AMS_ERROR;
	}

	//消息长度检查
	

	//获取坐席号
	p = pMsg->cMessageBody;
	tellerIdLen=*p++;
	if(tellerIdLen> AMS_MAX_TELLER_ID_LEN)
	{
		dbgprint("VtaRegReqProc[%d] TellerId[%u] Err.", pid, tellerId);
		iret = AMS_CMS_VTA_REG_TELLER_ID_ERR;
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}
	memcpy(tellerId,p,tellerIdLen);
	p+=tellerIdLen;
	
	//unpack Vpart TermNetInfo
	memset(&tellerNetInfo,0,sizeof(TERM_NET_INFO));
	iret = AmsUnpackVtaRegReqOpartPara(p, pMsg->iMessageLength - tellerIdLen-1, &tellerNetInfo);
    if(AMS_OK != iret)
	{
		dbgprint("VtaRegReqProc[%d][%d][%d] Teller[%s] UnpackVtaRegReqPara Err", 
			pMsg->s_SenderPid.cModuleId,
			pMsg->s_SenderPid.cFunctionId,
			pMsg->s_SenderPid.iProcessId,
			tellerId);
		
		if(AMS_ERROR == iret)
		{
			iret = AMS_CMS_VTA_REG_PARA_ERR;
		}
		
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;		
	}

	//unpack Opart TellerPersonalData

	//坐席号检查
	/*check RegTeller in cfg or not*/
	pTellerInfoNode=AmsSearchTellerInfoHash(tellerId,tellerIdLen);
	if( NULL != pTellerInfoNode)
	{
		tellerCfgPos = pTellerInfoNode->tellerInfopos;
		if(AmsCfgTeller(tellerCfgPos).flag != AMS_TELLER_INSTALL)
		{
			tellerCfgPos = -1;
		}
	}

	if(tellerCfgPos == -1)
	{
		dbgprint("VtaLoginReqProc[%d] TellerId[%s][%d]not Find", pid, tellerId, tellerIdLen);
		iret = AMS_VTA_LOGIN_TELLER_NO_ERR;
		AmsSendVtaLoginRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}

	/* check teller has been registered or not */
	//已经注册，则更新注册信息
	pRegTellerInfoNode=AmsSearchRegTellerInfoHash(tellerId,tellerIdLen);
	if(NULL != pRegTellerInfoNode)
	{
		if(pRegTellerInfoNode->tellerRegInfo.flag == AMS_TELLER_REGISTER)
		{
			pos = tellerRegPos;
		}
	}
	else //未注册，则记录注册信息
	{
		pRegTellerInfoNode=AmsGetRegTellerInfoNode(tellerId,tellerIdLen)
		if(NULL == pRegTellerInfoNode)
		{
			dbgprint("VtaRegReqProc[%d][%d][%d] Teller[%s][%u] ResourceLimited", 
				pMsg->s_SenderPid.cModuleId,
			    pMsg->s_SenderPid.cFunctionId,
			    pMsg->s_SenderPid.iProcessId,
				tellerNo, tellerId);		
			
			iret = AMS_CMS_VTA_REG_RESOURCE_LIMITED;
			AmsSendCmsVtaRegRsp(NULL,pMsg,iret);		
			return AMS_ERROR;	
		}
		pRegTellerInfoNode->tellerRegInfo.flag = AMS_TELLER_REGISTER;
		pRegTellerInfoNode->tellerRegInfo.tellerIdLen = tellerIdLen;
		memcpy(pRegTellerInfoNode->tellerRegInfo.tellerId,tellerId,tellerIdLen);
	}

	//记录或更新注册信息
	if(tellerNetInfo.ip != pRegTellerInfoNode->tellerRegInfo.vtaIp)
	{
		pRegTellerInfoNode->tellerRegInfo.vtaIp = tellerNetInfo.ip;
		transIpChange = 1;
	}

	pRegTellerInfoNode->tellerRegInfo.vtaPort = tellerNetInfo.port;	

	pRegTellerInfoNode->tellerRegInfo.cmsPid.iProcessId  = pMsg->s_SenderPid.cModuleId;
	pRegTellerInfoNode->tellerRegInfo.cmsPid.cFunctionId = pMsg->s_SenderPid.cFunctionId;
	pRegTellerInfoNode->tellerRegInfo.cmsPid.cModuleId   = pMsg->s_SenderPid.iProcessId;
	
	pRegTellerInfoNode->tellerRegInfo.myPid.iProcessId  = pMsg->s_ReceiverPid.cModuleId;
	pRegTellerInfoNode->tellerRegInfo.myPid.cFunctionId = pMsg->s_ReceiverPid.cFunctionId;
	pRegTellerInfoNode->tellerRegInfo.myPid.cModuleId   = pMsg->s_ReceiverPid.iProcessId;   

	//send vta reg rsp -to cms
	AmsSendCmsVtaRegRsp(pRegTellerInfoNode,pMsg,iret);
	
	return iret;
}


int VtaGetReqProc(int iThreadId, MESSAGE_t *pMsg)
{
	int					iret = AMS_CMS_PRCOESS_SUCCESS;
	LP_AMS_DATA_t		*lpAmsData = NULL;          //进程数据区指针
	LP_AMS_DATA_t		*lpOriginAmsData = NULL;    //进程数据区指针	
	LP_QUEUE_DATA_t     *lpQueueData = NULL;        //排队进程数据区指针 
	VTA_NODE            *pVtaNode = NULL;	
	VTA_NODE            *pOriginVtaNode = NULL;	
	VTA_NODE            *pTargetVtaNode = NULL;
	VTM_NODE            *pVtmNode = NULL;	
	CALL_TARGET         callTarget;	
	unsigned char       srvGrpSelfAdapt = 0;
	int                 tps = 0;
	int                 pid = 0;	
	int                 originPid = 0;	
	unsigned int        amsPid = 0;
	unsigned int        originTellerId = 0;
	unsigned char       originVtaNo[AMS_MAX_TELLER_NO_LEN + 1] = { 0 };	
	unsigned char       callIdLen = 0;   
	unsigned int        vtmId = 0;	
	unsigned char       vtmNo[AMS_MAX_VTM_NO_LEN + 1] = { 0 };	
	unsigned int        terminalType = 0;	
	unsigned char       targetVtaNo[AMS_MAX_TELLER_NO_LEN + 1] = { 0 };			
	unsigned int        srvGrpId = 0;	
	unsigned int        serviceType = 0;	
	unsigned int        serviceTypeRsvd = 0;
	unsigned short      callType = 0;
	unsigned int        i = 0;
	unsigned char       *p;	

	

	return iret;
}

int VtaCalloutReqProc(int iThreadId, MESSAGE_t *pMsg)
{
	int					iret = AMS_CMS_PRCOESS_SUCCESS;
	LP_AMS_DATA_t		*lpAmsData = NULL;          //进程数据区指针
	LP_AMS_DATA_t		*lpOriginAmsData = NULL;    //进程数据区指针	
	//LP_QUEUE_DATA_t     *lpQueueData = NULL;        //排队进程数据区指针 
	VTA_NODE            *pVtaNode = NULL;	
	VTA_NODE            *pOriginVtaNode = NULL;	
	VTA_NODE            *pTargetVtaNode = NULL;
	VTM_NODE            *pVtmNode = NULL;	
	//CALL_TARGET         callTarget;	
	unsigned char       srvGrpSelfAdapt = 0;
	int                 tps = 0;
	int                 pid = 0;	
	int                 originPid = 0;	
	unsigned int        amsPid = 0;
	unsigned int        originTellerId = 0;
	unsigned char       originVtaNo[AMS_MAX_TELLER_NO_LEN + 1] = { 0 };	
	unsigned char       callIdLen = 0;   
	unsigned int        vtmId = 0;	
	unsigned char       vtmNo[AMS_MAX_VTM_NO_LEN + 1] = { 0 };	
	unsigned int        terminalType = 0;	
	unsigned char       targetVtaNo[AMS_MAX_TELLER_NO_LEN + 1] = { 0 };			
	unsigned int        srvGrpId = 0;	
	unsigned int        serviceType = 0;	
	unsigned int        serviceTypeRsvd = 0;
	unsigned short      callType = 0;
	unsigned int        i = 0;
	unsigned char       *p;	
	unsigned char 		telleridlen=0,srvgrpidlen=0,srvtypelen=0;
	unsigned char		tellerId[AMS_MAX_TELLER_ID_LEN+1];
	unsigned char		srvgrpid[AMS_MAX_SERVICE_GROUP_NAME_LEN+1];
	unsigned char		srvtype[AMS_MAX_SERVICE_NAME_LEN+1];
	TELLER_INFO_NODE	*tellinfonode=NULL;
	TELLER_REGISTER_INFO_NODE *regtellinfonode=NULL;
	VTA_NODE			*vtanode=NULL;
	DWORD				CallOutType=0;

	//get remote pid
	pid = pMsg->s_SenderPid.iProcessId;

	//检查接受进程号
	if(pMsg->s_ReceiverPid.iProcessId != 0)
	{
		dbgprint("VtaGetReqProc[%d] Pid:%d Err", pid, pMsg->s_ReceiverPid.iProcessId);
		iret = AMS_CMS_GET_VTA_PARA_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,NULL,NULL);
		return AMS_ERROR;
	}
	
	p = pMsg->cMessageBody;
	//流水号检查
	callIdLen = *p++;
	if(callIdLen > AMS_MAX_CALLID_LEN)
	{
		dbgprint("VtaGetReqProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_CALLOUT_VTA_CALL_ID_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,NULL,NULL);	
		return AMS_ERROR;		
	}
	p += callIdLen;

	//tellerid 检查
	telleridlen=*p++;
	if(telleridlen>AMS_MAX_TELLER_ID_LEN)
	{
		dbgprint("VtaGetReqProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_CALLOUT_VTA_TELLER_ID_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,NULL,NULL);	
		return AMS_ERROR;

	}
	memcpy(tellerId,p,telleridlen);
	p+=telleridlen;

	//srvGrpId
	srvgrpidlen=*p++;
	if(srvgrpidlen>AMS_MAX_SERVICE_GROUP_NAME_LEN)
	{
		dbgprint("VtaGetReqProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_CALLOUT_VTA_TELLER_ID_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,NULL,NULL);	
		return AMS_ERROR;
	}
	memcpy(srvgrpid,p,srvgrpidlen);
	p+=srvgrpidlen;

	//servicetype
	srvtypelen=*p++;
	if(srvtypelen > AMS_MAX_SERVICE_NAME_LEN)
	{
		dbgprint("VtaGetReqProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_CALLOUT_VTA_TELLER_ID_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,NULL,NULL);	
		return AMS_ERROR;		
	}
	memcpy(srvtype,p,srvtypelen);
	p+=srvtypelen;

	BEPUTLONG(CallOutType,p);

	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		if(AmsCfgSrvGroup(i).flag == AMS_SERVICE_GROUP_UNINSTALL)
		{
			continue;	
		}
		
		if(strcmp(AmsCfgSrvGroup(i).srvGrpId,srvgrpid) == 0)
		{
			srvGrpId=i;
			break;
		}
	}

	//tellid in cfg or not
	tellinfonode=AmsSearchTellerInfoHash(tellerId,telleridlen);
	if(NULL == tellinfonode || (AmsCfgTeller(tellinfonode->tellerInfopos).flag == AMS_TELLER_UNINSTALL))
	{
		dbgprint("VtaGetReqProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_CALLOUT_VTA_TELLER_ID_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,NULL,NULL);	
		return AMS_ERROR;
	}

	//tellerid in regnode or not
	regtellinfonode=AmsSearchRegTellerInfoHash(tellerId,telleridlen)
	if(NULL == regtellinfonode && AmsCfgSrvGroup(i).isAutoFlag == AMS_SRVGRP_HUMAN)
	{
			dbgprint("VtaGetReqProc[%d] CallIdLen[%d]Err", pid, callIdLen);
			iret = AMS_CMS_CALLOUT_VTA_TELLER_ID_ERR;
			AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,NULL,NULL);	
			return AMS_ERROR;		
	} 

	vtanode = AmsSearchVtaNode(srvGrpId,tellerId,telleridlen)
	if(NULL == vtanode)
	{
		dbgprint("VtaGetReqProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_CALLOUT_VTA_TELLER_ID_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,NULL,NULL);	
		return AMS_ERROR;
	}
	
	//get local pid
	pid = pVtaNode->amsPid & 0xffff;
	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		dbgprint("VtaGetReqProc Teller[%s][%u] Vtm[%s][%u] CallType[%d] TargetTeller[%s][%u]Pid:%d Err", 
			originVtaNo, originTellerId, 
			lpOriginAmsData->vtmNo, lpOriginAmsData->vtmId, 
			callType, targetVtaNo, callTarget.targetTellerId, pid);
		iret = AMS_CMS_GET_VTA_AMS_PID_ERR;
		AmsSendCmsVtaGetRsp(NULL,pMsg,iret,pVtaNode,NULL);
		return AMS_ERROR;
	}
	
	lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
	//tellerid state in work or not
	if(AMS_VTA_STATE_BUSY == pVtaNode->state)
	{
		dbgprint("VtaGetReqProc Teller[%s][%u] Vtm[%s][%u] CallType[%d] TargetTeller[%s][%u]Pid:%d Err", 
					originVtaNo, originTellerId, 
					lpOriginAmsData->vtmNo, lpOriginAmsData->vtmId, 
					callType, targetVtaNo, callTarget.targetTellerId, pid);
		iret = AMS_CMS_GET_VTA_AMS_PID_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,pVtaNode,NULL);
		return AMS_ERROR;
	}

	if(AMS_CALL_STATE_NULL != pVtaNode->callState)
	{
		dbgprint("VtaGetReqProc Teller[%s][%u] Vtm[%s][%u] CallType[%d] TargetTeller[%s][%u]Pid:%d Err", 
					originVtaNo, originTellerId, 
					lpOriginAmsData->vtmNo, lpOriginAmsData->vtmId, 
					callType, targetVtaNo, callTarget.targetTellerId, pid);
		iret = AMS_CMS_GET_VTA_AMS_PID_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,pVtaNode,NULL);
		return AMS_ERROR;
	}

	//set tellerid state
	AmsSetVtaCallState(lpAmsData, pVtaNode, AMS_CALL_STATE_WAIT_ANSWER);

#ifdef AMS_TEST_LT
	//calc vta workInfo
	time(&currentTime);	   
	//AmsUpdateSingleVtaWorkInfo(pVtaNode, currentTime);
		
	//set Vta State and State Start Time
	AmsSetVtaState(iThreadId, lpAmsData, pVtaNode, AMS_VTA_STATE_BUSY, 0);
#endif

	//record callId
	lpAmsData->callIdLen = callIdLen;
	memcpy(lpAmsData->callId, &pMsg->cMessageBody[1], callIdLen);

	//record Calloutype
	lpAmsData->CallOutType = CallOutType;
	
	//update cmsPid
	lpAmsData->cmsPid.cModuleId	   = pMsg->s_SenderPid.cModuleId;
	lpAmsData->cmsPid.cFunctionId  = pMsg->s_SenderPid.cFunctionId;
	lpAmsData->cmsPid.iProcessId   = pMsg->s_SenderPid.iProcessId;

	//lpAmsData 可为NULL
	AmsSendCmsVtaCalloutRsp(lpAmsData,pMsg,iret,pVtaNode,NULL);	

	//send vta busy
	
	return iret;
}

int VtaAuthinfoReqProc(int iThreadId, MESSAGE_t *pMsg)
{
	unsigned char tellerIdLen=0;
	unsigned char tellerId[AMS_MAX_TELLER_ID_LEN+1];
	int                 pid = 0;	
	unsigned char       *p;	
	WORD				QueryType=0;

	TELLER_INFO_NODE	*tellinfonode=NULL;
	TELLER_REGISTER_INFO_NODE *regtellinfonode=NULL;
	VTA_NODE			*vtanode=NULL;
	
	//get remote pid
	pid = pMsg->s_SenderPid.iProcessId;
	
	//检查接收进程号
	if(pMsg->s_ReceiverPid.iProcessId != 0)
	{
		dbgprint("VtaGetReqProc[%d] Pid:%d Err", pid, pMsg->s_ReceiverPid.iProcessId);
		iret = AMS_CMS_GET_VTA_PARA_ERR;
		AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);

		return AMS_ERROR;
	}
	p = pMsg->cMessageBody;

	BEPUTSHORT(p,QueryType);
	p+=2;

	//tellerid 检查
	tellerIdLen=*p++;
	if(tellerIdLen>AMS_MAX_TELLER_ID_LEN)
	{
		dbgprint("VtaGetReqProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_CALLOUT_VTA_TELLER_ID_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,NULL,NULL);	
		return AMS_ERROR;

	}
	memcpy(tellerId,p,tellerIdLen);
	p+=tellerIdLen;

	//是否注册
	tellinfonode=AmsSearchTellerInfoHash(tellerId,telleridlen);
	if(NULL == tellinfonode || (AmsCfgTeller(tellinfonode->tellerInfopos).flag == AMS_TELLER_UNINSTALL))
	{
		dbgprint("VtaGetReqProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_CALLOUT_VTA_TELLER_ID_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,NULL,NULL);	
		return AMS_ERROR;
	}

	//tellerid in regnode or not
	regtellinfonode=AmsSearchRegTellerInfoHash(tellerId,telleridlen)
	if(NULL == regtellinfonode && AmsCfgSrvGroup(i).isAutoFlag == AMS_SRVGRP_HUMAN)
	{
		dbgprint("VtaGetReqProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_CALLOUT_VTA_TELLER_ID_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,NULL,NULL);	
		return AMS_ERROR;		
	} 

	AmsSendCmsAuthinfoRsp(AmsCfgTeller(tellinfonode->tellerInfopos),pMsg,iret);
	
}

int AmsSendCmsVtaRegRsp(TELLER_REGISTER_INFO_NODE *tellerRegisterInfo,MESSAGE_t *pMsg,int iret)
{
	MESSAGE_t           s_Msg;
	unsigned char       *p;
	unsigned char       telleridlen=0;

	memset(&s_Msg,0,sizeof(MESSAGE_t));

	if(NULL == pMsg)
	{
		return AMS_ERROR;
	}
	
	s_Msg.eMessageAreaId = A;
	memcpy(&s_Msg.s_ReceiverPid,&pMsg->s_SenderPid,sizeof(PID_t));
	s_Msg.s_SenderPid.cModuleId = SystemData.cMid;
	s_Msg.s_SenderPid.cFunctionId = FID_AMS;
	s_Msg.s_SenderPid.iProcessId = pMsg->s_ReceiverPid.iProcessId;
	s_Msg.iMessageType = A_VTA_REG_RSP;
	s_Msg.iMessageLength = 0;
		
    p = &s_Msg.cMessageBody[0];

	if(tellerRegisterInfo != NULL)
	{
		*p = tellerRegisterInfo->tellerRegInfo.tellerIdLen;
		p++;
		memcpy(p,tellerRegisterInfo->tellerRegInfo.tellerId,tellerRegisterInfo->tellerRegInfo.tellerIdLen);
		p+=tellerRegisterInfo->tellerRegInfo.tellerIdLen;
		telleridlen = tellerRegisterInfo->tellerRegInfo.tellerIdLen;
	}
	else
	{
		*p=0;
		p++;
	}
	
	BEPUTLONG(iret, p);

	s_Msg.iMessageLength = 4+1+telleridlen;
	
	SendMsgBuff(&s_Msg,0);

	//AmsMsgStatProc(AMS_CMS_MSG, s_Msg.iMessageType);
	//AmsResultStatProc(AMS_CMS_VTA_REG_RESULT, iret);	
	return SUCCESS;
}

int AmsSendCmsVtaCalloutRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret,DWORD callouttype)
{
	MESSAGE_t           s_Msg;
	unsigned char       *p;

	MESSAGE_t           s_Msg;
	unsigned char       *p;
	
	memset(&s_Msg,0,sizeof(MESSAGE_t));

	if(NULL == pMsg)
	{
		return AMS_ERROR;
	}
	
	s_Msg.eMessageAreaId = A;
	memcpy(&s_Msg.s_ReceiverPid,&pMsg->s_SenderPid,sizeof(PID_t));
	s_Msg.s_SenderPid.cModuleId = SystemData.cMid;
	s_Msg.s_SenderPid.cFunctionId = FID_AMS;
	
	if(NULL != lpAmsData)
	{
		s_Msg.s_SenderPid.iProcessId = lpAmsData->myPid.iProcessId;
		
	}
	else
	{
		s_Msg.s_SenderPid.iProcessId = pMsg->s_ReceiverPid.iProcessId;
	}

	s_Msg.iMessageType = A_VTA_CALLOUT_RSP;
	s_Msg.iMessageLength = 0;
			
    p = &s_Msg.cMessageBody[0];
	if(lpAmsData != NULL)
	{
		*p++ = lpAmsData->callIdLen;
		if(lpAmsData->callIdLen <= AMS_MAX_CALLID_LEN)
		{
			memcpy(p, lpAmsData->callId, lpAmsData->callIdLen);
		}
		p += lpAmsData->callIdLen;

		s_Msg.iMessageLength += (1 + lpAmsData->callIdLen);
	}
	else
	{
		//pack callId
		*p++ = pMsg->cMessageBody[0];
		if(pMsg->cMessageBody[0] <= AMS_MAX_CALLID_LEN)
		{
			memcpy(p, &pMsg->cMessageBody[1], pMsg->cMessageBody[0]);	
		}
		p += pMsg->cMessageBody[0];
		
		s_Msg.iMessageLength += (1 + pMsg->cMessageBody[0]);				
	}

	if(AMS_VTA_QUEUE_MNG_SUCCESS == iret)
	{
		if(NULL != lpAmsData)
		{
			*p++=lpAmsData->tellerIdLen;
			if(lpAmsData->tellerIdLen<= AMS_MAX_TELLER_ID_LEN)
			{
				memcpy(p,lpAmsData->tellerId,lpAmsData->tellerIdLen);
			}
			p+= lpAmsData->tellerIdLen;
			s_Msg.iMessageLength +=  lpAmsData->tellerIdLen;			
		}
	}
		
	s_Msg.iMessageLength += 1;

	BEPUTLONG(callouttype, p);	
	p += 4;

	BEPUTLONG(iret, p);
	p += 4;

	s_Msg.iMessageLength += 8;

	SendMsgBuff(&s_Msg,0);

	return SUCCESS;

}

int AmsSendCmsAuthinfoRsp(TELLER_INFO *tellcfginfo, MESSAGE_t *pMsg,int iret)
{
	MESSAGE_t			s_Msg;
	unsigned char		*p;
	unsigned char		telleridlen=0;

	memset(&s_Msg,0,sizeof(MESSAGE_t));

	if(NULL == pMsg)
	{
		return AMS_ERROR;
	}
	
	s_Msg.eMessageAreaId = A;
	memcpy(&s_Msg.s_ReceiverPid,&pMsg->s_SenderPid,sizeof(PID_t));
	s_Msg.s_SenderPid.cModuleId = SystemData.cMid;
	s_Msg.s_SenderPid.cFunctionId = FID_AMS;
	s_Msg.s_SenderPid.iProcessId = pMsg->s_ReceiverPid.iProcessId;
	s_Msg.iMessageType = A_AUTHINFO_QUERY_RSP;
	s_Msg.iMessageLength = 0;
		
	p = &s_Msg.cMessageBody[0];

	BEPUTSHORT(2,p);
	p+=2;

	BEPUTLONG(iret,p);
	p+=4;

	if(tellcfginfo != NULL)
	{
		*p = tellcfginfo->tellerIdLen;
		p++;
		memcpy(p,tellcfginfo->tellerId,tellcfginfo->tellerIdLen);
		p+=tellcfginfo->tellerIdLen;
		s_Msg.iMessageLength += tellcfginfo->tellerIdLen;

		*p = tellcfginfo->tellerPwdLen;
		p++;	
		memcpy(p,tellcfginfo->tellerPwd,tellcfginfo->tellerPwdLen);
		p+=tellcfginfo->tellerPwdLen;
		s_Msg.iMessageLength += tellcfginfo->tellerPwdLen;
	}
	
	s_Msg.iMessageLength +=8;

	SendMsgBuff(&s_Msg,0);

	return AMS_SUCCESS;
}





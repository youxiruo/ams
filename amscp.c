
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
	TELLER_PERSONAL_INFO tellerpesonalinfo;
	unsigned char        stringlen=0;

	if(AmsMsgTrace)
	{
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"recv A_VTA_REG_REQ msg \n");	
		AmsTraceToFile(pMsg->s_ReceiverPid,pMsg->s_SenderPid,"A_VTA_REG_REQ",description,
						descrlen,pMsg->cMessageBody,pMsg->iMessageLength,"ams");		
	}


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
	iret = AmsUnpackVtaRegReqVpartPara(p, pMsg->iMessageLength - tellerIdLen-1, &tellerNetInfo);//待开发

	if(AMS_ERROR == iret)
	{
		dbgprint("VtaRegReqProc[%d][%d][%d] Teller[%s] UnpackVtaRegReqPara Err", 
		pMsg->s_SenderPid.cModuleId,
		pMsg->s_SenderPid.cFunctionId,
		pMsg->s_SenderPid.iProcessId,
		tellerId);
		iret = AMS_CMS_VTA_REG_PARA_ERR;
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;	
	}

	p+=iret;

	if(*p= AMS_TELLER_PERSIONALDATA_ID)
	{
		//unpack Opart TellerPersonalData
		memset(&tellerpesonalinfo,0,sizeof(TELLER_PERSONAL_INFO));
		iret = AmsUnpackTellerpersionalinfo(p,pMsg->iMessageLength-tellerIdLen-1-iret,&tellerpesonalinfo);
		if(AMS_ERROR == iret)
		{
			dbgprint("VtaRegReqProc[%d][%d][%d] Teller[%s] UnpackVtaRegReqPara Err", 
			pMsg->s_SenderPid.cModuleId,
			pMsg->s_SenderPid.cFunctionId,
			pMsg->s_SenderPid.iProcessId,
			tellerId);
			iret = AMS_CMS_VTA_REG_PARA_ERR;
			AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
			return AMS_ERROR;	
		}
	}

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
		//AmsSendVtaLoginRsp(NULL,pMsg,iret);
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
		pRegTellerInfoNode=AmsGetRegTellerInfoNode(tellerId,tellerIdLen);
		if(NULL == pRegTellerInfoNode)
		{
			dbgprint("VtaRegReqProc[%d][%d][%d] Teller[%s] ResourceLimited", 
				pMsg->s_SenderPid.cModuleId,
			    pMsg->s_SenderPid.cFunctionId,
			    pMsg->s_SenderPid.iProcessId,tellerId);		
			
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

	pRegTellerInfoNode->tellerRegInfo.tellerpersionalinfo.tellertype = tellerpesonalinfo.tellertype;

	strcpy(pRegTellerInfoNode->tellerRegInfo.tellerpersionalinfo.tellerNickName,tellerpesonalinfo.tellerNickName);
	stringlen = strlen(tellerpesonalinfo.tellerNickName);
	pRegTellerInfoNode->tellerRegInfo.tellerpersionalinfo.tellerNickName[stringlen]='\0';
	
	strcpy(pRegTellerInfoNode->tellerRegInfo.tellerpersionalinfo.tellerUserName,tellerpesonalinfo.tellerUserName);
	stringlen = strlen(tellerpesonalinfo.tellerUserName);
	pRegTellerInfoNode->tellerRegInfo.tellerpersionalinfo.tellerUserName[stringlen]='\0';
	
	//send vta reg rsp -to cms
	AmsSendCmsVtaRegRsp(pRegTellerInfoNode,pMsg,iret);
	
	return iret;
}


int VtaGetReqProc(int iThreadId, MESSAGE_t *pMsg)
{
	int					iret = AMS_CMS_PRCOESS_SUCCESS;
	LP_AMS_DATA_t		*lpAmsData = NULL;          //进程数据区指针	
	LP_QUEUE_DATA_t     *lpQueueData = NULL;        //排队进程数据区指针 
	VTA_NODE            *pVtaNode = NULL;	
	VTA_NODE            *pOriginVtaNode = NULL;	
	VTA_NODE            *pTargetVtaNode = NULL;
	TERM_NODE           *pTermNode = NULL;
	int                 tps = 0;
	int                 pid = 0;	
	unsigned int        amsPid = 0;
	unsigned char       callIdLen = 0;   
	unsigned int        terminalType = 0;	
	int                 srvGrpId = -1;	
	unsigned int        serviceTypeRsvd = 0;
	unsigned short      callType = 0;
	unsigned int        i = 0;
	unsigned char       *p;	
	unsigned char       termidlen=0;
	unsigned char       termId[AMS_MAX_TERM_ID_LEN+1]={0};
	unsigned char       srvgrpidlen=0;
	unsigned char       srvgrpid[AMS_MAX_SERVICE_GROUP_NAME_LEN+1]={0};
	unsigned char		srvtypelen=0;
	unsigned char       srvtype[AMS_MAX_SERVICE_NAME_LEN+1]={0};
	unsigned int        srvGrpSelfAdapt=0;

	
#ifdef AMS_TEST_LT
		time_t				currentTime;
#endif
	

	if(AmsMsgTrace)
	{
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"recv A_VTA_GET_REQ msg \n");	
		AmsTraceToFile(pMsg->s_ReceiverPid,pMsg->s_SenderPid,"A_VTA_GET_REQ",description,
						descrlen,pMsg->cMessageBody,pMsg->iMessageLength,"ams");		
	}	

	//get remote pid
	pid = pMsg->s_SenderPid.iProcessId;

	//检查接受进程号
	if(pMsg->s_ReceiverPid.iProcessId != 0)
	{
		dbgprint("VtaGetReqProc[%d] Pid:%d Err", pid, pMsg->s_ReceiverPid.iProcessId);
		iret = AMS_CMS_GET_VTA_PARA_ERR;
		AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
		return AMS_ERROR;
	}

	p = pMsg->cMessageBody;
	//流水号检查
	callIdLen = *p++;
	if(callIdLen > AMS_MAX_CALLID_LEN)
	{
		dbgprint("VtaGetReqProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_GET_VTA_CALL_ID_ERR;
		AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);	
		return AMS_ERROR;		
	}
	p += callIdLen;

	//check termttype
	BEGETLONG(terminalType,p);
	p+=4;

	if(terminalType != AMS_TERM_TYPE_MBPHONE)
	{
		dbgprint("VtaGetReqProc[%d] termtype [%d] Err",pid,terminalType);
		iret = AMS_CMS_GET_VTA_TERMINAL_TYPE_ERR;
		AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
		return AMS_ERROR;
	}

	termidlen = *p++;
	if( termidlen > AMS_MAX_TERM_ID_LEN)
	{
		dbgprint("VtaGetReqProc[%d] termidlen [%d] Err",pid,termidlen);
		iret = AMS_CMS_GET_VTA_TERM_ID_ERR;
		AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
		return AMS_ERROR;		
	}
	memcpy(termId,p,termidlen);
	p+=termidlen;

	
	//srvGrpId
	srvgrpidlen=*p++;
	if(srvgrpidlen>AMS_MAX_SERVICE_GROUP_NAME_LEN)
	{
		dbgprint("VtaGetReqProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_GET_VTA_SRVGRP_ID_ERR;
		AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);	
		return AMS_ERROR;
	}
	memcpy(srvgrpid,p,srvgrpidlen);
	p+=srvgrpidlen;

	//servicetype
	srvtypelen=*p++;
	if(srvtypelen > AMS_MAX_SERVICE_NAME_LEN)
	{
		dbgprint("VtaGetReqProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_GET_VTA_SRVTYPE_ERR;
		AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);	
		return AMS_ERROR;		
	}
	memcpy(srvtype,p,srvtypelen);
	p+=srvtypelen;

	/*check srvgrpid is installed or not*/
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

	if(srvGrpId == -1)
	{
		dbgprint("VtaGetReqProc[%d] SrvGrpId[%s] Err", pid, srvgrpid);
		iret = AMS_CMS_GET_VTA_SRVGRP_ID_ERR;
		AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);	
		return AMS_ERROR;		
	}

	 /* find Term Node in process */
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		pTermNode = AmsSearchTermNode(i,termId,termidlen);		
		if(NULL != pTermNode)
		{
			break;
		}
	}

	if(NULL == pTermNode)
	{
		pTermNode = TermNodeGet();
		if(NULL == pTermNode)
		{
			dbgprint("VtaGetReqProc[%d][%s] TermNodeGet Failed",
				pid, termId);
			iret = AMS_CMS_GET_VTA_TERM_RESOURCE_LIMITED;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);		
			return AMS_ERROR;
		}
	}

	/*TERM 状态异常保护*/
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
			if(pTermNode->customerPid > 0 && pTermNode->customerPid < LOGIC_PROCESS_SIZE)
			{
				if(pTermNode->customerPid > 0 && pTermNode->customerPid < LOGIC_PROCESS_SIZE)
				{
					lpQueueData=(LP_QUEUE_DATA_t *)ProcessData[pTermNode->customerPid];
					
					/* 杀掉定时器 */
					if(lpQueueData->iTimerId >= 0)
					{
						AmsQueueKillTimer(pTermNode->customerPid, &lpQueueData->iTimerId);//或 lpQueueData->myPid.iProcessId
						AmsTimerStatProc(T_AMS_CUSTOMER_IN_QUEUE_TIMER, AMS_KILL_TIMER);
					} 

					//release lpQueueData Pid
					AmsReleassPid(lpQueueData->myPid, END);
				}
			}					
		}		
		
		//set Vtm Service State and State Start Time
		AmsSetTermServiceState(pTermNode, AMS_CUSTOMER_SERVICE_NULL);	
	}

	//reset amsPid
	pTermNode->amsPid = 0;

	//reset customerPid
	pTermNode->customerPid = 0;

	//检查业务组编号与业务类型组合
	if(srvtypelen == 0 && srvgrpidlen == 0)
	{
		dbgprint("VtaGetReqProc[%d] Termid[%s] Srvgrpid Srvtype Err",pid,termId);
		iret = AMS_CMS_GET_VTA_VALID_SRVTYPE_OR_SRVGRPID;
		AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);	
	}

	//仅携带业务类型，没有指明业务组编号 待讨论
	if(srvtypelen != 0 && srvgrpidlen == 0)
	{
		//根据业务类型选择业务组
		iret = AmsSelectSrvGrpIdByServiceType(termId,srvtype,&srvGrpId);
		
		if(AMS_OK != iret)
		{
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);		
			return AMS_ERROR;			
		}	

		srvGrpSelfAdapt = 1;
	}

	//新呼叫添加到链表尾
    /* Del Vtm Node from Origin List */
    Sem_wait(&AmsSrvData(pTermNode->termInfo.srvGrpIdpos).termCtrl);
	lstDelete(&AmsSrvData(pTermNode->termInfo.srvGrpIdpos).termList, (NODE *)pTermNode);
	Sem_post(&AmsSrvData(pTermNode->termInfo.srvGrpIdpos).termCtrl);

	/* Add Vtm Node to new List */
    Sem_wait(&AmsSrvData(srvGrpId).termCtrl);
	lstAdd(&AmsSrvData(srvGrpId).termList, (NODE *)pTermNode);
	Sem_post(&AmsSrvData(srvGrpId).termCtrl);

	//更新当前终端的业务组编号
	if(0 != memcmp(pTermNode->termInfo.srvGrpId,srvgrpid,srvgrpidlen))
	{
		//update srvGrpId
		strcpy(pTermNode->termInfo.srvGrpId,srvgrpid);
		pTermNode->termInfo.srvGrpId[srvgrpidlen]='\0';
	}

	//业务智能路由
	pVtaNode = AmsServiceIntelligentSelectVta(termId,srvGrpId,srvtype,&iret);
	if(NULL == pVtaNode)
	{
		if(AMS_CMS_GET_VTA_SERVICE_IN_QUEUE != iret)
		{
			dbgprint("VtaGetReqProc[%d] Term[%s]SISelectVta Failed", 
				pid, termId);
			
			if(AMS_ERROR == iret)
			{
				iret = AMS_CMS_GET_VTA_SERVICE_INTELLIGENT_ROUTING_ERR;
			}
					
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);		
			return AMS_ERROR;			
		}

		iret = AmsStartCustomerQueueProcess(pMsg,pTermNode,srvGrpId,srvtype,callIdLen,srvGrpSelfAdapt);
		
		if(AMS_OK != iret)
		{
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);		
			return AMS_ERROR;			
		}
		
		//Customer In Queue...
		iret = AMS_CMS_GET_VTA_SERVICE_IN_QUEUE;
		
		//update Customer Service State
		AmsSetTermServiceState(pTermNode, AMS_CUSTOMER_IN_QUEUE);

		//init enterQueueTime
		time(&pTermNode->enterQueueTime);

		//record serviceType	
		pTermNode->serviceTypeLen = srvtypelen;
		strcpy(pTermNode->serviceType,srvtype);
		pTermNode->serviceType[srvtypelen]='\0';
		
		if(AmsStateTrace)
		{
			unsigned char description [256];
			int descrlen;
			memset(description,0,sizeof(description));
			descrlen=snprintf(description,256,"Ams Customer of Term[%s]-[%d][%s][%s] is in Queue...",
				pTermNode->termInfo.termId, srvGrpId, srvgrpid,srvtype);
			
			AmsTraceInfoToFile(0,0,description,descrlen,"ams");	
		}	
	}
	else
	{
		//检查进程号
		//get local pid		
		pid = pVtaNode->amsPid & 0xffff;
		if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
		{
			dbgprint("VtaGetReqProc Term[%s] Teller[%s] Pid[0x%x][%d]Err", 
				termId, pVtaNode->vtaInfo.tellerId,
				pVtaNode->amsPid, pid);
			iret = AMS_CMS_GET_VTA_AMS_PID_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,pVtaNode,NULL);
			return AMS_ERROR;
		}
		
		lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];

		//更新进程数据
		//record termId
		lpAmsData->termIdLen = termidlen;
		memcpy(lpAmsData->termId,termId,termidlen);
		
		//Set vta call State, only one pthread!!!
		AmsSetVtaCallState(lpAmsData, pVtaNode, AMS_CALL_STATE_WAIT_ANSWER);

#ifdef AMS_TEST_LT
	    //calc vta workInfo
		time(&currentTime);	   
		AmsUpdateSingleVtaWorkInfo(pVtaNode, currentTime);
	
		//set Vta State and State Start Time
		AmsSetVtaState(iThreadId, lpAmsData, pVtaNode, AMS_VTA_STATE_BUSY, 0);
#endif

		//record vtmPid
        memcpy(&lpAmsData->termPid,&pTermNode->rPid,sizeof(PID_t));
		
		//record callId
	    lpAmsData->callIdLen = callIdLen;
		memcpy(lpAmsData->callId, &pMsg->cMessageBody[1], callIdLen);

		//record amsPid
		pTermNode->amsPid = pVtaNode->amsPid;

		//record serviceType	
		memcpy(pTermNode->serviceType,srvtype,srvtypelen);
		
	    //update Customer Service State
		AmsSetTermServiceState(pTermNode, AMS_CUSTOMER_IN_SERVICE);

		//update cmsPid
		lpAmsData->cmsPid.cModuleId	   = pMsg->s_SenderPid.cModuleId;
		lpAmsData->cmsPid.cFunctionId  = pMsg->s_SenderPid.cFunctionId;
		lpAmsData->cmsPid.iProcessId   = pMsg->s_SenderPid.iProcessId;

		Sem_wait(&AmsSrvData(srvGrpId).freevtaCtrl);
		lstDelete(&AmsSrvData(srvGrpId).freevtaList, (NODE *)pVtaNode);
		Sem_post(&AmsSrvData(srvGrpId).freevtaCtrl);
		
	}
	
	//update cmsPid
	pTermNode->cmsPid.cModuleId	  = pMsg->s_SenderPid.cModuleId;
	pTermNode->cmsPid.cFunctionId  = pMsg->s_SenderPid.cFunctionId;
	pTermNode->cmsPid.iProcessId   = pMsg->s_SenderPid.iProcessId;
	
    //set Term State and State Start Time
	AmsSetTermState(iThreadId, pTermNode, AMS_TERM_STATE_BUSY);

	//send Vta Get Rsp to CMS
	if(AMS_CUSTOMER_IN_SERVICE == pTermNode->serviceState)
	{
		//lpAmsData 可为NULL
    	AmsSendCmsVtaGetRsp(lpAmsData,pMsg,iret,pVtaNode,NULL);
	}
	
	if(AMS_CUSTOMER_IN_QUEUE == pTermNode->serviceState)
	{
		//lpAmsData 可为NULL
    	AmsSendCmsVtaGetRsp(lpAmsData,pMsg,iret,NULL,pTermNode);
	}
	
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
	//VTM_NODE            *pVtmNode = NULL;	
	//CALL_TARGET         callTarget;	
	unsigned char       srvGrpSelfAdapt = 0;
	int                 tps = 0;
	int                 pid = 0;	
	int                 originPid = 0;	
	unsigned int        amsPid = 0;
	//unsigned int        originTellerId = 0;
	//unsigned char       originVtaNo[AMS_MAX_TELLER_NO_LEN + 1] = { 0 };	
	unsigned char       callIdLen = 0;   
	unsigned int        vtmId = 0;	
	//unsigned char       vtmNo[AMS_MAX_VTM_NO_LEN + 1] = { 0 };	
	unsigned int        terminalType = 0;	
	//unsigned char       targetVtaNo[AMS_MAX_TELLER_NO_LEN + 1] = { 0 };			
	unsigned int        srvGrpId = 0;	
	unsigned int        serviceType = 0;	
	unsigned int        serviceTypeRsvd = 0;
	unsigned short      callType = 0;
	unsigned int        i = 0;
	unsigned char       *p;	
	unsigned char 		telleridlen=0,srvgrpidlen=0,srvtypelen=0;
	unsigned char		tellerId[AMS_MAX_TELLER_ID_LEN+1] = { 0 };
	unsigned char		srvgrpid[AMS_MAX_SERVICE_GROUP_NAME_LEN+1] = { 0 };
	unsigned char		srvtype[AMS_MAX_SERVICE_NAME_LEN+1] = { 0};
	TELLER_INFO_NODE	*tellinfonode=NULL;
	TELLER_REGISTER_INFO_NODE *regtellinfonode=NULL;
	DWORD				CallOutType=0;


#ifdef AMS_TEST_LT
		time_t				currentTime;
#endif

	if(AmsMsgTrace)
	{
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"recv A_VTA_CALLOUT_REQ msg \n");	
		AmsTraceToFile(pMsg->s_ReceiverPid,pMsg->s_SenderPid,"A_VTA_CALLOUT_REQ",description,
						descrlen,pMsg->cMessageBody,pMsg->iMessageLength,"ams");		
	}	
	
	//get remote pid
	pid = pMsg->s_SenderPid.iProcessId;

	//检查接受进程号
	if(pMsg->s_ReceiverPid.iProcessId != 0)
	{
		dbgprint("VtaGetReqProc[%d] Pid:%d Err", pid, pMsg->s_ReceiverPid.iProcessId);
		iret = AMS_CMS_CALLOUT_PARA_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,NULL);
		return AMS_ERROR;
	}
	
	p = pMsg->cMessageBody;
	//流水号检查
	callIdLen = *p++;
	if(callIdLen > AMS_MAX_CALLID_LEN)
	{
		dbgprint("VtaGetReqProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_CALLOUT_VTA_CALL_ID_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,NULL);	
		return AMS_ERROR;		
	}
	p += callIdLen;

	//tellerid 检查
	telleridlen=*p++;
	if(telleridlen>AMS_MAX_TELLER_ID_LEN)
	{
		dbgprint("VtaGetReqProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_CALLOUT_VTA_TELLER_ID_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,NULL);	
		return AMS_ERROR;

	}
	memcpy(tellerId,p,telleridlen);
	p+=telleridlen;

	//srvGrpId
	srvgrpidlen=*p++;
	if(srvgrpidlen>AMS_MAX_SERVICE_GROUP_NAME_LEN)
	{
		dbgprint("VtaGetReqProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_CALLOUT_VTA_SRVGRP_ID_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,NULL);	
		return AMS_ERROR;
	}
	memcpy(srvgrpid,p,srvgrpidlen);
	p+=srvgrpidlen;

	//servicetype
	srvtypelen=*p++;
	if(srvtypelen > AMS_MAX_SERVICE_NAME_LEN)
	{
		dbgprint("VtaGetReqProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_CALLOUT_VTA_SRVTYPE_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,NULL);	
		return AMS_ERROR;		
	}
	memcpy(srvtype,p,srvtypelen);
	p+=srvtypelen;

	BEGETSHORT(CallOutType,p);
	p+=2;

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
		iret = AMS_CMS_CALLOUT_VTA_TELLER_ID_NOTCFG_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,CallOutType);	
		return AMS_ERROR;
	}

	//tellerid in regnode or not
	regtellinfonode=AmsSearchRegTellerInfoHash(tellerId,telleridlen);
	if(NULL == regtellinfonode && AmsCfgSrvGroup(srvGrpId).isAutoFlag == AMS_SRVGRP_TYPE_HUMAN)
	{
			dbgprint("VtaGetReqProc[%d] CallIdLen[%d]Err", pid, callIdLen);
			iret = AMS_CMS_CALLOUT_VTA_TELLER_ID_NOT_REG_ERR;
			AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,CallOutType);	
			return AMS_ERROR;		
	} 

	pVtaNode = AmsSearchVtaNode(srvGrpId,tellerId,telleridlen);
	if(NULL == pVtaNode)
	{
		//dbgprint("VtaGetReqProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_CALLOUT_VTA_TELLER_ID_NOTLOGIN_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,CallOutType);	
		return AMS_ERROR;
	}
	
	//get local pid
	pid = pVtaNode->amsPid & 0xffff;
	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		//dbgprint("VtaGetReqProc Teller[%s]CallType[%d] Pid:%d Err", 
		//	pVtaNode->tellerId,  
		//	callType, pid);
		iret = AMS_CMS_CALLOUT_AMS_PID_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,CallOutType);
		return AMS_ERROR;
	}
	
	lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
	//tellerid state in work or not
	if(AMS_VTA_STATE_BUSY == pVtaNode->state)
	{
		/*dbgprint("VtaGetReqProc Teller[%s][%u] Vtm[%s][%u] CallType[%d] TargetTeller[%s][%u]Pid:%d Err", 
					originVtaNo, originTellerId, 
					lpOriginAmsData->vtmNo, lpOriginAmsData->vtmId, 
					callType, targetVtaNo, callTarget.targetTellerId, pid);*/
		iret = AMS_CMS_CALLOUT_VTAREPEAT_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,CallOutType);
		return AMS_ERROR;
	}

	if(AMS_CALL_STATE_NULL != pVtaNode->callState)
	{
		/*dbgprint("VtaGetReqProc Teller[%s][%u] Vtm[%s][%u] CallType[%d] TargetTeller[%s][%u]Pid:%d Err", 
					originVtaNo, originTellerId, 
					lpOriginAmsData->vtmNo, lpOriginAmsData->vtmId, 
					callType, targetVtaNo, callTarget.targetTellerId, pid);*/
		iret = AMS_CMS_CALLOUT_STATE_ERR;
		AmsSendCmsVtaCalloutRsp(NULL,pMsg,iret,CallOutType);
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
	AmsSendCmsVtaCalloutRsp(lpAmsData,pMsg,iret,CallOutType);	

	//send vta busy
	if(CallOutType == 1)
	{
		AmsSendVtaStateOperateInd(lpAmsData,pMsg,VTA_STATE_OPERATE_IND_BUSY,VTA_STATE_OPERATE_IND_BUSY_CALLOUTSOLO);
	}
	//send vta busy
	else if(CallOutType == 2)
	{
		AmsSendVtaStateOperateInd(lpAmsData,pMsg,VTA_STATE_OPERATE_IND_BUSY,VTA_STATE_OPERATE_IND_BUSY_CALLOUTDB);
	}
	else
	{
		AmsSendVtaStateOperateInd(lpAmsData,pMsg,VTA_STATE_OPERATE_IND_BUSY,VTA_STATE_OPERATE_IND_BUSY_IND);
	}

	//delete vtanode from freevatlist
	Sem_wait(&AmsSrvData(srvGrpId).freevtaCtrl);
	lstDelete(&AmsSrvData(srvGrpId).freevtaList, (NODE *)pVtaNode);
	Sem_post(&AmsSrvData(srvGrpId).freevtaCtrl);
	
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
	unsigned int		iret = 0;
	unsigned int		srvGrpIdPos=0;
	int					tellerCfgPos=0;


	
	if(AmsMsgTrace)
	{
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"recv A_AUTHINFO_QUERY_REQ msg \n");	
		AmsTraceToFile(pMsg->s_ReceiverPid,pMsg->s_SenderPid,"A_AUTHINFO_QUERY_REQ",description,
						descrlen,pMsg->cMessageBody,pMsg->iMessageLength,"ams");		
	}	
	
	//get remote pid
	pid = pMsg->s_SenderPid.iProcessId;
	
	//检查接收进程号
	if(pMsg->s_ReceiverPid.iProcessId != 0)
	{
		dbgprint("VtaAuthinfoReqProc[%d] Pid:%d Err", pid, pMsg->s_ReceiverPid.iProcessId);
		iret = AMS_CMS_AUTHINFO_PARA_ERR;
		AmsSendCmsAuthinfoRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}
	p = pMsg->cMessageBody;

	BEGETSHORT(QueryType,p);
	p+=2;

	//tellerid 检查
	tellerIdLen=*p++;
	if(tellerIdLen>AMS_MAX_TELLER_ID_LEN)
	{
		dbgprint("VtaAuthinfoReqProc[%d] TellerIdLen[%d]Err", pid, tellerIdLen);
		iret = AMS_CMS_AUTHINFO_TELLER_ID_LEN_ERR;
		AmsSendCmsAuthinfoRsp(NULL,pMsg,iret);	
		return AMS_ERROR;

	}
	memcpy(tellerId,p,tellerIdLen);
	p+=tellerIdLen;

	//是否在cfg
	
	/*get teller cfg pos*/
	tellinfonode=AmsSearchTellerInfoHash(tellerId,tellerIdLen);
	if( NULL != tellinfonode)
	{
		tellerCfgPos = tellinfonode->tellerInfopos;
		if(AmsCfgTeller(tellerCfgPos).flag != AMS_TELLER_INSTALL)
		{
			tellerCfgPos = -1;
		}
	}
	
	if(tellerCfgPos == -1)
	{
		dbgprint("VtaAuthinfoReqProc[%d] TellerId[%s][%d]not Find", pid, tellerId, tellerIdLen);
		iret = AMS_CMS_AUTHINFO_TELLER_ID_NOTCFG_ERR;
		AmsSendCmsAuthinfoRsp(NULL,pMsg,iret);	
		return AMS_ERROR;
	}

	/*判断是否已注册*/
	regtellinfonode=AmsSearchRegTellerInfoHash(tellerId,tellerIdLen);
	if(NULL != regtellinfonode)
	{
		dbgprint("VtaAuthinfoReqProc[%d] Tellerid[%s] Registed Before",pid,tellerId);
		iret = AMS_CMS_AUTHINFO_TELLER_ID_REPEAT_ERR;
		AmsSendCmsAuthinfoRsp(NULL,pMsg,iret);	
		return AMS_ERROR;
	}


	AmsSendCmsAuthinfoRsp(&AmsCfgTeller(tellinfonode->tellerInfopos),pMsg,iret);

	return AMS_OK;
	
}


int CallEventNoticeProc(int iThreadId, MESSAGE_t *pMsg)
{
	int 				iret = AMS_CMS_PRCOESS_SUCCESS;
	LP_AMS_DATA_t		*lpAmsData = NULL;			//进程数据区指针
	//LP_QUEUE_DATA_t 	*lpQueueData = NULL;		//排队进程数据区指针	
	LP_AMS_DATA_t		*lpOriginAmsData = NULL;	//进程数据区指针	
	VTA_NODE			*pVtaNode = NULL;	
	//VTM_NODE			*pVtmNode = NULL;	
	VTA_NODE			*pOriginVtaNode = NULL; 		
	int 				pid = 0;
	int 				originPid = 0;		
	unsigned int		amsPid = 0;
	unsigned char		callIdLen = 0;	
	unsigned int		callEventNotice = 0;
	unsigned int		tellerIdLen = 0;
	unsigned char		tellerId[AMS_MAX_TELLER_ID_LEN + 1] = { 0 };		
	unsigned int		originId = 0;
	unsigned char		vtmIdLen = 0;		
	unsigned char		vtmId[AMS_MAX_VTM_ID_LEN + 1] = { 0 };	
	//unsigned int		originTellerId = 0; 
	//unsigned char		originVtaNo[AMS_MAX_TELLER_NO_LEN + 1] = { 0 }; 		
	unsigned int		newState = 0;
	unsigned int		i = 0;
	unsigned char		*p;
	DWORD				termType;
		
#ifdef AMS_TEST_LT
		time_t				currentTime;
#endif

	if(AmsMsgTrace)
	{
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"recv A_AMS_CALL_EVENT_NOTICE msg \n");	
		AmsTraceToFile(pMsg->s_ReceiverPid,pMsg->s_SenderPid,"A_AMS_CALL_EVENT_NOTICE",description,
						descrlen,pMsg->cMessageBody,pMsg->iMessageLength,"ams");		
	}

	//检查接收进程号
	pid = pMsg->s_ReceiverPid.iProcessId;
	if(0 == pid)
	{
		dbgprint("CallEventNoticeProc[%d] Err", pMsg->s_SenderPid.iProcessId);
		iret = AMS_CMS_EVENT_NOTICE_PARA_ERR;
		AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);			
		return AMS_ERROR;
	}

	//流水线检查
	p = pMsg->cMessageBody;
	callIdLen = *p++;
	if(callIdLen > AMS_MAX_CALLID_LEN)
	{
		dbgprint("CallEventNoticeProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_EVENT_NOTICE_CALL_ID_ERR;
		AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  			
		return AMS_ERROR;		
	}
	p+=callIdLen;

	//获取事件通知码
	BEGETLONG(callEventNotice, p);
	p += 4;

	//获取AMS进程号
	BEGETLONG(amsPid,p);
	p+=4;

	//获取坐席号
	tellerIdLen = *p++;
	if(tellerIdLen > AMS_MAX_TELLER_ID_LEN)
	{
		dbgprint("CallEventNoticeProc[%d] TellerIdLen[%d]Err", pid, tellerIdLen);
		iret = AMS_CMS_EVENT_NOTICE_CALL_ID_ERR;
		AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  			
		return AMS_ERROR;	
	}
	memcpy(tellerId,p,tellerIdLen);
	p+=tellerIdLen;

	//获取TERMTYPE
	BEGETLONG(termType,p);
	p+=4;

	vtmIdLen=*p++;
	if(vtmIdLen > AMS_MAX_VTM_ID_LEN)
	{
		dbgprint("CallEventNoticeProc[%d] TellerIdLen[%d]Err", pid, tellerIdLen);
		iret = AMS_CMS_EVENT_NOTICE_CALL_ID_ERR;
		AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);			
		return AMS_ERROR;	
	}
	memcpy(vtmId,p,vtmIdLen);
	p+=vtmIdLen;

	/*if(		CMS_CALL_EVENT_NOTICE_VTA_RING == callEventNotice
		||	CMS_CALL_EVENT_NOTICE_VTA_ANSWER == callEventNotice
		||  CMS_CALL_EVENT_NOTICE_VTA_RELEASE == callEventNotice)
	{
		//查看tellerid 是否登陆
		for(i = 0;i < AMS_MAX_SERVICE_GROUP_NUM;i++)
		{
			pOriginVtaNode = AmsSearchVtaNode(i,tellerId,tellerIdLen);
			if(NULL != pOriginVtaNode)
			{
				break;
			}
		}
		if(NULL == pOriginVtaNode)
		{
			dbgprint("CallEventNoticeProc[%d] Event[%d] OriginTeller[%s]Id[%u] not Login.",
				pid, callEventNotice, originVtaNo, originTellerId);
			iret = AMS_CMS_EVENT_NOTICE_ORIGIN_TELLER_ID_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret); 
			return AMS_ERROR;		
		}

		//检查进程号
		originPid = pOriginVtaNode->amsPid & 0xffff;
		if((0 == originPid) || (originPid >= LOGIC_PROCESS_SIZE))
		{
			dbgprint("CallEventNoticeProc[%d] Event[%d] OriginTeller[%s][%u]targetPid[0x%x][%d]Err", 
				pid, callEventNotice, originVtaNo, originTellerId, 
				pOriginVtaNode->amsPid, originPid);
			iret = AMS_CMS_EVENT_NOTICE_ORIGIN_TELLER_AMS_PID_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret); 
			return AMS_ERROR;
		}

		lpOriginAmsData=(LP_AMS_DATA_t *)ProcessData[originPid];
				
		//进程号匹配性检查
		if(lpOriginAmsData->myPid.iProcessId != originPid)
		{
			dbgprint("CallEventNoticeProc[%d] Event[%d] OriginTeller[%s][%u] PID[%d][%d] Not Equal", 
				pid, callEventNotice, originVtaNo, originTellerId,
				lpOriginAmsData->myPid.iProcessId, originPid);
			iret = AMS_CMS_EVENT_NOTICE_ORIGIN_TELLER_AMS_PID_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret); 
			return AMS_ERROR;
		}

		//坐席业务组编号检查
		if(lpOriginAmsData->srvGrpIdPos > AMS_MAX_SERVICE_GROUP_NUM) 
		{
			dbgprint("CallEventNoticeProc[%d] Event[%d] OriginTeller[%s][%u] SrvGrpId[%u]Err", 
 				pid, callEventNotice, originVtaNo, originTellerId, 
 				lpOriginAmsData->srvGrpId);
			iret = AMS_CMS_EVENT_NOTICE_ORIGIN_TELLER_SERVICE_GROUP_ID_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret); 
			return AMS_ERROR;		
		}	

		//坐席业务状态检查
		if(AMS_SERVICE_ACTIVE != AmsSrvData(lpOriginAmsData->srvGrpIdPos).serviceState)
		{
			/*dbgprint("CallEventNoticeProc[%d] Event[%d] OriginTeller[%s][%d] ServiceState[%d]Err", 
				pid, callEventNotice, originVtaNo, originTellerId, 
				AmsSrvData(lpOriginAmsData->srvGrpIdPos).serviceState);*/
			//iret = AMS_CMS_EVENT_NOTICE_ORIGIN_TELLER_SERVICE_STATE_ERR;
			//AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret); 
			//return AMS_ERROR;			
		/*}

		//坐席状态检查
		if(pOriginVtaNode->state >= AMS_VTA_STATE_OFFLINE)
		{
			/*dbgprint("CallEventNoticeProc[%d] Event[%d] OriginTeller[%s][%d] State[%d]Err", 
				pid, callEventNotice, originVtaNo, originTellerId, 
				pOriginVtaNode->state);*/
		/*	iret = AMS_CMS_EVENT_NOTICE_ORIGIN_TELLER_STATE_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);
			return AMS_ERROR;		
		}
	}
	if(		CMS_CALL_EVENT_NOTICE_VTM_RING == callEventNotice
		||	CMS_CALL_EVENT_NOTICE_VTM_ANSWER == callEventNotice
		||  CMS_CALL_EVENT_NOTICE_VTM_RELEASE == callEventNotice)
	{
			//ams->crm 坐席事件指示	
	}*/

	lpAmsData = (LP_AMS_DATA_t *)ProcessData[pid];

	//流水号检查
	p = pMsg->cMessageBody;
	if(callIdLen != lpAmsData->callIdLen)
	{
		dbgprint("CallEventNoticeProc[%d] Event[%d] Teller[%s] Term[%s]CallIdLen[%d][%d]Err", 
			pid, callEventNotice, lpAmsData->tellerId, 
			lpAmsData->termId, 
			callIdLen, lpAmsData->callIdLen); 
		iret = AMS_CMS_EVENT_NOTICE_CALL_ID_ERR;
	    AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  			
		return AMS_ERROR;		
	}

	p++;
	if(0 != strcmp(lpAmsData->callId, p))
	{
		dbgprint("CallEventNoticeProc[%d] Event[%d] Teller[%s] Term[%s]CallIdErr", 
			pid, callEventNotice, lpAmsData->tellerId, 
			lpAmsData->termId); 
		iret = AMS_CMS_EVENT_NOTICE_CALL_ID_ERR;
	    AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  			
		return AMS_ERROR;		
	}
	p += callIdLen;

	//事件通知码检查
	if(    callEventNotice < CMS_CALL_EVENT_NOTICE_VTA_RING 
		|| callEventNotice >= CMS_CALL_EVENT_NOTICE_MAX)
	{
		dbgprint("CallEventNoticeProc[%d] Teller[%s] Term[%s]EventCode[%d]Err", 
			pid,lpAmsData->tellerId, 
			lpAmsData->termId, callEventNotice); 
		iret = AMS_CMS_EVENT_NOTICE_CODE_ERR;
	    AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  			
		return AMS_ERROR;			
	}

	//坐席号检查
	if(		CMS_CALL_EVENT_NOTICE_VTA_RING == callEventNotice
		||	CMS_CALL_EVENT_NOTICE_VTA_ANSWER == callEventNotice
		||	CMS_CALL_EVENT_NOTICE_VTA_RELEASE == callEventNotice)
	{
		if(0 != strcmp(lpAmsData->tellerId,tellerId))
		{
			dbgprint("CallEventNoticeProc[%d] Event[%d] Teller[%s]Err", 
				pid, callEventNotice, 
				lpAmsData->tellerId);
			iret = AMS_CMS_EVENT_NOTICE_TELLER_ID_ERR;
	    	AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  				
			return AMS_ERROR;	
		}
	}

	//用户信息检查
	/*if(		CMS_CALL_EVENT_NOTICE_VTM_RING == callEventNotice
		||	CMS_CALL_EVENT_NOTICE_VTM_ANSWER == callEventNotice
		||	CMS_CALL_EVENT_NOTICE_VTM_RELEASE == callEventNotice)
	{
		if(0 != strcmp(lpAmsData->vtmId,vtmId))
		{
			dbgprint("CallEventNoticeProc[%d] Event[%d] Teller[%s]Vtm[%s]Err", 
				pid, callEventNotice, 
				lpAmsData->tellerId, 
			lpAmsData->vtmId, vtmId);
			iret = AMS_CMS_EVENT_NOTICE_VTM_ID_ERR;
		    AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  	
			return AMS_ERROR;			
		}
	}*/

	//业务组编号检查
	if(lpAmsData->srvGrpIdPos > AMS_MAX_SERVICE_GROUP_NUM)
	{
		dbgprint("CallEventNoticeProc[%d] Event[%d] Teller[%s] Term[%s]SrvGrpId[%u]Err", 
			pid, callEventNotice, 
			lpAmsData->tellerId, 
			lpAmsData->termId, lpAmsData->srvGrpIdPos);
		iret = AMS_CMS_EVENT_NOTICE_SERVICE_GROUP_ID_ERR;
		AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  	
		return AMS_ERROR;		
	}

    //业务状态检查
	if(AMS_SERVICE_ACTIVE != AmsSrvData(lpAmsData->srvGrpIdPos).serviceState)
	{
		dbgprint("CallEventNoticeProc[%d] Event[%d] Teller[%s] Term[%s]ServiceState[%d]Err", 
			pid, callEventNotice, 
			lpAmsData->tellerId, 
			lpAmsData->termId, 
			AmsSrvData(lpAmsData->srvGrpIdPos).serviceState);
		iret = AMS_CMS_EVENT_NOTICE_SERVICE_STATE_ERR;
		AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  	
		return AMS_ERROR;		
	}

	//update vta state
	if( CMS_CALL_EVENT_NOTICE_VTA_RING == callEventNotice
		|| CMS_CALL_EVENT_NOTICE_VTA_ANSWER == callEventNotice
		|| CMS_CALL_EVENT_NOTICE_VTA_RELEASE == callEventNotice)
	{
		/*find vta node*/
		pVtaNode = AmsSearchVtaNode(lpAmsData->srvGrpIdPos,lpAmsData->tellerId,lpAmsData->tellerIdLen);
		if(NULL == pVtaNode)
		{
			dbgprint("CallEventNoticeProc[%d] Event[%d] Term[%s] Teller[%s]Err", 
				pid, callEventNotice, 
				lpAmsData->termId,
				lpAmsData->tellerId);	
			iret = AMS_CMS_EVENT_NOTICE_TELLER_ID_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  	
			return AMS_ERROR;		
		}

		//坐席状态查询
		if( pVtaNode->state >= AMS_VTA_STATE_OFFLINE
			&& CMS_CALL_EVENT_NOTICE_VTA_RELEASE != callEventNotice)
		{
			dbgprint("CallEventNoticeProc[%d] Event[%d] Term[%s]Teller[%s]State[%d]Err", 
				pid, callEventNotice, 
				lpAmsData->termId, 
				lpAmsData->tellerId, pVtaNode->state);		
			iret = AMS_CMS_EVENT_NOTICE_VTA_STATE_ERR;
	    	AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  				
			return AMS_ERROR;				
		}
		if(CMS_CALL_EVENT_NOTICE_VTA_RING == callEventNotice)
		{
			newState = AMS_CALL_STATE_WAIT_ANSWER;
		}
		//update vta callstate,not check vta state and callstate
		if(CMS_CALL_EVENT_NOTICE_VTA_ANSWER == callEventNotice)
		{
			newState = AMS_CALL_STATE_ANSWER;

			//AmsTellerStatProc(lpAmsData->tellerId, lpAmsData->tellerPos, AMS_TELLER_CALL_CONNECT, iret);
			pVtaNode->vtaWorkInfo.connectNum++;

			//reset callTransferNum
			pVtaNode->callTransferNum = 0;			
		}
		
		if(CMS_CALL_EVENT_NOTICE_VTA_RELEASE == callEventNotice)
		{
			//坐席示闲操作在收到示闲指示证实后才正式置闲
			//reset vta call state
			newState = AMS_CALL_STATE_NULL;
			pVtaNode->setstate = AMS_VTA_STATE_IDLE;

			//杀掉定时器
			//			AmsKillVtaAllTimer(lpAmsData, pid);

			//仅杀掉呼叫相关定时器，包括消息、文件收发
			//AmsKillVtaAllCallTimer(lpAmsData, pid);
			
			//update time
			//memset(&pVtaNode->callStateStartLocalTime, 0, sizeof(TIME_INFO)); 
			//memset(&pVtaNode->callStateStartTime, 0, sizeof(time_t));	

			//reset callTransferNum
			//			pVtaNode->callTransferNum = 0;

#ifdef AMS_TEST_LT
			//calc vta workInfo
			//time(&currentTime);    
			//AmsUpdateSingleVtaWorkInfo(pVtaNode, currentTime);
	
			//set Vta State and State Start Time
			//AmsSetVtaState(iThreadId, lpAmsData, pVtaNode, AMS_VTA_STATE_IDLE, 0);
#endif			
			//AmsInsertDbServiceSDR(iThreadId, AMS_SDR_ITEM_BASE, lpAmsData, NULL, 0, 0, NULL);
			//reset sessStat
			//memset(&lpAmsData->sessStat, 0, sizeof(AMS_SESSION_STAT));	

			//send vta ind
			AmsSendVtaStateOperateInd(lpAmsData,pMsg,VTA_STATE_OPERATE_IND_IDLE,VTA_STATE_OPERATE_IND_IDLE_REL);

		}
		AmsSetVtaCallState(lpAmsData, pVtaNode, newState);
	}

	//update vtm state and state start time
	if( CMS_CALL_EVENT_NOTICE_VTM_RING == callEventNotice
		|| CMS_CALL_EVENT_NOTICE_VTM_ANSWER == callEventNotice
		|| CMS_CALL_EVENT_NOTICE_VTM_RELEASE == callEventNotice)
	{
		// ams->crm 
		AmsSendTellerEventInd(lpAmsData,callEventNotice,vtmId,vtmIdLen,termType,iret);
	}

	return AMS_OK;
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

	AmsMsgStatProc(AMS_CMS_MSG, s_Msg.iMessageType);
	//AmsResultStatProc(AMS_CMS_VTA_REG_RESULT, iret);	
	
	if(AmsMsgTrace)
	{	
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"send A_VTA_REG_RSP msg \n");	
		{
			AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_VTA_REG_RSP",description,
				descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,"ams");	
		}
	}
	return SUCCESS;
}

int AmsSendCmsVtaCalloutRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret,DWORD callouttype)
{
	MESSAGE_t           s_Msg;
	unsigned char       *p;
	unsigned char		tellidlen=0,callidlen=0;
	
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
		*p++=lpAmsData->tellerIdLen;
		if(lpAmsData->tellerIdLen<= AMS_MAX_TELLER_ID_LEN)
		{
			memcpy(p,lpAmsData->tellerId,lpAmsData->tellerIdLen);
		}
		p+= lpAmsData->tellerIdLen;
		s_Msg.iMessageLength +=  lpAmsData->tellerIdLen+1;			
	}
	else
	{
		callidlen = pMsg->cMessageBody[0];
		tellidlen = pMsg->cMessageBody[1+callidlen];
		memcpy(p,pMsg->cMessageBody,2+callidlen+tellidlen);
		p+=2+callidlen+tellidlen;
		s_Msg.iMessageLength = 2+callidlen+tellidlen;
	}

	BEPUTSHORT(callouttype, p);	
	p += 2;

	BEPUTLONG(iret, p);
	p += 4;

	s_Msg.iMessageLength += 6;

	SendMsgBuff(&s_Msg,0);

	if(AmsMsgTrace)
	{	
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"send A_VTA_CALLOUT_RSP msg \n");	
		{
			AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_VTA_CALLOUT_RSP",description,
				descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,"ams");	
		}
	}

	AmsMsgStatProc(AMS_CMS_MSG, s_Msg.iMessageType);
	AmsResultStatProc(AMS_CMS_VTA_CALLOUT_RESULT, iret);
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
	
	if(AmsMsgTrace)
	{	
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"send A_AUTHINFO_QUERY_RSP msg \n");	
		{
			AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_AUTHINFO_QUERY_RSP",description,
				descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,"ams");	
		}
	}

	AmsMsgStatProc(AMS_CMS_MSG, s_Msg.iMessageType);

	return AMS_SUCCESS;
}
/*
CallId	STRING	流水号
Result	DWORD	参见5原因值定义
AmsPid	DWORD	AMS进程号，注A
TellerId	STRING	注B
TermNetInfo	O	注C
*/
int AmsSendCmsVtaGetRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret,VTA_NODE *pVtaNode,TERM_NODE *pTermNode)
{
	MESSAGE_t			s_Msg;
	unsigned char		*p;
	
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
	else if(NULL != pTermNode)
	{
		s_Msg.s_SenderPid.iProcessId = pTermNode->customerPid&0xffff;
	}
	else
	{
		s_Msg.s_SenderPid.iProcessId = pMsg->s_ReceiverPid.iProcessId;
	}
	
	s_Msg.iMessageType = A_VTA_GET_RSP;
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

	BEPUTLONG(iret, p);
	p += 4;
	
	s_Msg.iMessageLength += 4;
	if(AMS_VTA_QUEUE_MNG_SUCCESS == iret)
	 {
		 if(NULL != pVtaNode)
		 {
			 BEPUTLONG(pVtaNode->amsPid, p);
			 p += 4;
	
			 //pack TellerId
			 if(pVtaNode->vtaInfo.tellerIdLen > AMS_MAX_TELLER_ID_LEN)
			 {
				 pVtaNode->vtaInfo.tellerIdLen = AMS_MAX_TELLER_ID_LEN;
			 }
			 *p++ = pVtaNode->vtaInfo.tellerIdLen;
			 memcpy(p, pVtaNode->vtaInfo.tellerId, pVtaNode->vtaInfo.tellerIdLen);
			 p += pVtaNode->vtaInfo.tellerIdLen;
	
			 s_Msg.iMessageLength += (1 + pVtaNode->vtaInfo.tellerIdLen);
			 
			 //Opart
			 *p++ = AMS_TERM_NET_INFO_ID;
	
			 BEPUTSHORT(6, p);	 
			 p += 2;
	
			 BEPUTLONG(pVtaNode->vtaInfo.vtaIp, p);  
			 p += 4;
	
			 BEPUTSHORT(pVtaNode->vtaInfo.vtaPort, p);	 
	
			 //add Opart Len
			 s_Msg.iMessageLength += 9;
		 }
		 else
		 {
			 s_Msg.iMessageLength += 9;
		 }
	 }
	 else if(AMS_CMS_GET_VTA_SERVICE_IN_QUEUE == iret)
	 {
		 if(NULL != pTermNode)
		 {
			 BEPUTLONG(pTermNode->customerPid, p);
			 p += 4;
		 }
		 
		 s_Msg.iMessageLength += 9;
	 }	 
	 else
	 {
		 s_Msg.iMessageLength += 9;
	 }
	 
	 SendMsgBuff(&s_Msg,0);
	
	 if(AmsMsgTrace)
	 {	 
		 unsigned char description [1024];
		 int descrlen;
		 memset(description,0,sizeof(description));
		 descrlen=snprintf(description,1024,"send A_VTA_GET_RSP msg \n");	 
		 if(NULL != lpAmsData)
		 {
			 AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_VTA_GET_RSP",description,
							 descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,lpAmsData->sTraceName);				 
		 }
		 else
		 {
			 AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_VTA_GET_RSP",description,
				 descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,"ams"); //lpQueueData->sTraceName
		 }
	 }
	
	 AmsMsgStatProc(AMS_CMS_MSG, s_Msg.iMessageType);
	 AmsResultStatProc(AMS_CMS_GET_VTA_RESULT, iret);
	 
	 return SUCCESS;


}





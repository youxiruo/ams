#include "amsfunc.h"

int VtaLoginReqProc(int iThreadId, MESSAGE_t *pMsg)
{
	int					 iret = AMS_VTA_QUEUE_MNG_SUCCESS;
	LP_AMS_DATA_t		 *lpAmsData = NULL;   //进程数据区指针
	VTA_NODE             *pVtaNode = NULL;
	PID_t				 Pid;	
	int                  tellerIdLen = 0;
	unsigned char        tellerId[AMS_MAX_TELLER_ID_LEN] = {0};
	int                  tellerPwdLen = 0;
	unsigned char        tellerPwd[AMS_MAX_PWD_LEN] = {0};
	unsigned int         tellerCfgPos = -1;	
	unsigned int         orgCfgPos = 0;
	unsigned int         tellerLoginNum = 0;
	unsigned int		 tellerNum = 0;
	int                  pid = 0;	
	int                  i = 0;
	unsigned char        *p;	
	int					 telleridhash=0;
	TELLER_INFO_NODE	*pTellerInfoNode = NULL;


	pid = pMsg->s_SenderPid.iProcessId;

	//检查接受进程号
	if(pMsg->s_ReceiverPid.iProcessId != 0)
	{
		dbgprint("VtaLoginReqProc[%d] Pid:%d Err", pid, pMsg->s_ReceiverPid.iProcessId);
		//iret = AMS_VTA_LOGIN_PARA_ERR;
		//AmsSendVtaLoginRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}

	//消息长度检查


	//个数获取
	p = pMsg->cMessageBody;
	BEGETSHORT(tellerLoginNum, p);
	p++;
	
	for(i=0;i<tellerLoginNum;i++)
	{
		//检查登陆信息
		//check teller id
		tellerIdLen=*p++;
		if(tellerIdLen > AMS_MAX_TELLER_ID_LEN)
		{
			dbgprint("VtaLoginReqProc[%d] TellerIdLen[%d] Err", pid, tellerIdLen);
			iret = AMS_VTA_LOGIN_TELLER_NO_ERR;
			AmsSendVtaLoginRsp(NULL,pMsg,iret);
			return AMS_ERROR;
		}
		memcpy(tellerId,p,tellerIdLen);
		p += tellerIdLen;
		
		//check teller pwd
		tellerPwdLen = *p++;
		if(tellerPwdLen > AMS_MAX_PWD_LEN)
		{
			dbgprint("VtaLoginReqProc[%d] TellerIdLen[%d] Err", pid, tellerIdLen);
			iret = AMS_VTA_LOGIN_TELLER_PWD_ERR;
			AmsSendVtaLoginRsp(NULL,pMsg,iret);
			return AMS_ERROR;			
		}
		memcpy(tellerPwd,p,tellerPwdLen);
		
		//check len


		/*check teller in cfg or not*/
		/*get teller cfg pos*/
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

		/*check teller in regist or not*/
		

		//比较配置的pwd和登陆的pwd是否一致
		if(0 != memcmp(AmsCfgTeller(tellerCfgPos).tellerPwd, tellerPwd, tellerPwdLen))
		{
			dbgprint("VtaLoginReqProc[%d] Pwd[%s]Err", pid, tellerPwd);
			iret = AMS_VTA_LOGIN_TELLER_PWD_ERR;
			AmsSendVtaLoginRsp(NULL,pMsg,iret);
			return AMS_ERROR;		
		}

		//检查已登陆柜员数量
		for(i = 0; i< AMS_MAX_SERVICE_GROUP_NUM;i++)
		{
			tellerNum += lstCount(&AmsSrvData(i).vtaList);
		}


		//检查cfg and sys vtanum
		if(tellerNum >= SystemData.AmsPriData.amsCfgData.maxVtaNum || tellerNum >= AMS_MAX_VTA_NODES)
		{
			dbgprint("VtaLoginReqProc[%d] Teller[%s]TelleNum[%d]-[%d][%d] Err", 
				pid, tellerId, tellerNum, SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NODES);
			iret = AMS_VTA_LOGIN_TELLER_NUM_ERR;
			AmsSendVtaLoginRsp(NULL,pMsg,iret);
			return AMS_ERROR;	
		}

		//check teller type

		/* check teller in process or not*/
		if(AMS_SERVICE_ACTIVE == AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpId).serviceState)
		{
			pVtaNode = AmsSearchVtaNode(AmsCfgTeller(tellerCfgPos).srvGrpId, AmsCfgTeller(tellerCfgPos).tellerId);
			if(NULL != pVtaNode)
			{
				dbgprint("VtaLoginReqProc[%d] TellerNo[%s]Id[%u]has been Logined[%d]", 
					pid, tellerId, AmsCfgTeller(tellerCfgPos).tellerId, tellerCfgPos);		
				iret = AMS_VTA_LOGIN_TELLER_LOGIN_REPEATEDLY;
				AmsSendVtaLoginRsp(NULL,pMsg,iret);		
				return AMS_ERROR;
			}	
		}

		//分配服务进程号
		memset(&Pid,0,sizeof(PID_t));
		iret = AmsAllocPid(&Pid);
		if(-1 == iret)
		{
			dbgprint("VtaLoginReqProc[%d] Teller[%s] AmsAllocPid: SysBusy", pid, tellerNo);
			iret = AMS_VTA_LOGIN_LP_RESOURCE_LIMITED;
			AmsSendVtaLoginRsp(NULL,pMsg,iret);
			return AMS_ERROR;
		}

	    //分配TELLER结点
		pVtaNode = VtaNodeGet();
		if(NULL == pVtaNode)
		{
	 		dbgprint("VtaLoginReqProc[%d] Teller[%s] VtaNodeGet Failed", pid, tellerNo);
			iret = AMS_VTA_LOGIN_NODE_RESOURCE_LIMITED;
			AmsSendVtaLoginRsp(NULL,pMsg,iret);
			AmsReleassPid(Pid, FAILURE);
			return AMS_ERROR;
		}
		
		pMsg->s_ReceiverPid.iProcessId = Pid.iProcessId;

		lpAmsData=(LP_AMS_DATA_t *)ProcessData[pMsg->s_ReceiverPid.iProcessId];

		/* 进程数据区初始化  */
		memset(lpAmsData, 0, sizeof(LP_AMS_DATA_t));

		//record amsPid
		lpAmsData->amsPid = ((((unsigned int)pMsg->s_SenderPid.cModuleId) << 24)&0xff000000) | 
		                 ((((unsigned int)pMsg->s_ReceiverPid.cModuleId) << 16)&0x00ff0000) |
		                 (((unsigned int)pMsg->s_ReceiverPid.iProcessId)&0x0000ffff);
	
		//record tellerId
		lpAmsData->tellerIdLen = AmsCfgTeller(tellerCfgPos).tellerIdLen;
		memcpy(lpAmsData->tellerId,AmsCfgTeller(tellerCfgPos).tellerId,AmsCfgTeller(tellerCfgPos).tellerIdLen);

		//record srvGrpId
		lpAmsData->srvGrpIdLen = AmsCfgTeller(tellerCfgPos).srvGrpIdLen;
		memcpy(lpAmsData->srvGrpId,AmsCfgTeller(tellerCfgPos).srvGrpId,AmsCfgTeller(tellerCfgPos).srvGrpIdLen);

		//record tellerPost
		lpAmsData->tellerPos = tellerCfgPos;

		//init StateOp
		lpAmsData->currStateOp = VTA_STATE_OPERATE_RSVD;

		//init timer
		lpAmsData->iTimerId             = -1;
		lpAmsData->callTimerId          = -1;
		lpAmsData->volumeCtrlTimerId    = -1;
		lpAmsData->rcasRemoteCoopTimerId= -1;
		lpAmsData->vtmRemoteCoopTimerId = -1;	
		lpAmsData->snapTimerId          = -1;
		lpAmsData->restTimerId          = -1;

		lpAmsData->sendMsgToVtaTimerId  = -1;
		lpAmsData->sendMsgToVtmTimerId  = -1;
		lpAmsData->sendFileToVtaTimerId = -1;
		lpAmsData->sendFileToVtmTimerId = -1;

		lpAmsData->multiSessTimerId     = -1;
		lpAmsData->monitorTimerId       = -1;
		lpAmsData->vtaParaCfgTimerId    = -1;	

		//record myPid
		lpAmsData->myPid.cModuleId	 = SystemData.cMid;
		lpAmsData->myPid.cFunctionId = FID_AMS;
		lpAmsData->myPid.iProcessId  = Pid.iProcessId;

		//record rPid //VtaPid
		lpAmsData->rPid.cModuleId	 = pMsg->s_SenderPid.cModuleId;
		lpAmsData->rPid.cFunctionId  = pMsg->s_SenderPid.cFunctionId;
		lpAmsData->rPid.iProcessId   = pMsg->s_SenderPid.iProcessId;

		//init cmsPid
		lpAmsData->cmsPid.cModuleId   = AmsCfgData.cmsInfo.cModuleId;
		lpAmsData->cmsPid.cFunctionId = FID_CMS;
		lpAmsData->cmsPid.iProcessId  = 0;

		/* VTA结点数据初始化 */
	    //record amsPid
		pVtaNode->amsPid = lpAmsData->amsPid;

		//init Start time
		time(&pVtaNode->startTime);

		//stateStartTime & callStateStartTime later 
		
		//init Handshake time
		time(&pVtaNode->handshakeTime);
		
		//init WorkInfo Update time
		time(&pVtaNode->workInfoUpdateTime);

		//init call Transfer Freeze Time
		time(&pVtaNode->callTransferFreezeTime);

		//init update Score Expect Time
		time(&pVtaNode->updateScoreExpectTime);
		
		//get vtaInfo
		memset(&pVtaNode->vtaInfo, 0, sizeof(TELLER_INFO));
		memcpy(&pVtaNode->vtaInfo, &AmsCfgTeller(tellerCfgPos), sizeof(TELLER_INFO));

		//init work info
		memset(&pVtaNode->vtaWorkInfo, 0, sizeof(TELLER_WORK_INFO)); 

		//init call call Transfer Num
		pVtaNode->callTransferNum = 0;
				
		//init Score and Expect Score
		//pVtaNode->dailyAvgScore = AMS_TELLER_SERVICE_QUALITY_THRESHOLD;
		//pVtaNode->dailyAvgScoreExpect = AMS_TELLER_SERVICE_QUALITY_THRESHOLD;

		//recored tellerCfgPos
		pVtaNode->tellerCfgPos = tellerCfgPos;

		//set Vta State and State Start Time
		AmsSetVtaState(iThreadId, lpAmsData, pVtaNode, AMS_VTA_STATE_IDLE, 0);
		
		//set Vta Call State and State Start Time 
		AmsSetVtaCallState(lpAmsData, pVtaNode, AMS_CALL_STATE_NULL);

		/* Add Vta Node to List */
	    Sem_wait(&AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpId).vtaCtrl);
		lstAdd(&AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpId).vtaList, (NODE *)pVtaNode);
		Sem_post(&AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpId).vtaCtrl);

		//send Login Rsp to Vta
		AmsSendVtaLoginRsp(lpAmsData,pMsg,iret);

		//send TermIdNo to Vta
		//AmsSendTermIdNoInfo(pMsg,pVtaNode,NULL);

		//send Term Cfg Info, not include TermIdNo
		//AmsSendTermCfgInfo(pMsg,pVtaNode,NULL);
		
	}

	

	return iret;

}


int AmsSendVtaLoginRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret)
{
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
	s_Msg.s_SenderPid.iProcessId = pMsg->s_ReceiverPid.iProcessId;
	s_Msg.iMessageType = A_VTA_LOGIN_RSP;
	s_Msg.iMessageLength = 0;
	
	p = &s_Msg.cMessageBody[0];
	BEPUTLONG(iret, p);

	if(AMS_VTA_QUEUE_MNG_SUCCESS == iret &&
	   NULL != lpAmsData)		
	{
		unsigned int amsTellerLoginState;
		unsigned int amsTellerCallRelState;

		BEPUTSHORT(lpAmsData->amsTellerNum, p);
		p+=2;

		BEPUTLONG(lpAmsData->amsPid,p)
		p+=4;

		*p=lpAmsData->tellerIdLen;
		p+=1;
		memcpy(p,lpAmsData->tellerId,lpAmsData->tellerIdLen);
		p+=lpAmsData->tellerIdLen;

		//pack Login State
		if(AMS_OK == AmsTransformVtc2AmsTellerState(AmsCfgData.tellerLoginState, &amsTellerLoginState))
		{
			*p++ = AMS_LOGIN_STATE_ID;
			BEPUTSHORT(2, p);
			p += 2;
			
			BEPUTSHORT(amsTellerLoginState, p);
			p += 2;
			
			s_Msg.iMessageLength += (1 + 2 + 2);	
		}

		//pack Call Rel State
		if(AMS_OK == AmsTransformVtc2AmsTellerState(AmsCfgData.tellerCallRelState, &amsTellerCallRelState))
		{
			*p++ = AMS_CALL_REL_STATE_ID;
			BEPUTSHORT(2, p);
			p += 2;		
			
			BEPUTSHORT(amsTellerCallRelState, p);
			p += 2;

			s_Msg.iMessageLength += (1 + 2 + 2);	
		}	

		//pack Answer Type
		*p++ = AMS_ANSWER_TYPE_ID;
		BEPUTSHORT(2, p);
		p += 2;
		
		BEPUTSHORT(AmsCfgData.tellerAnswerType, p);
//		p += 2;
		
		s_Msg.iMessageLength += (1 + 2 + 2);
	}
	else
	{
		p += 10;
		BEPUTLONG(AMS_SERVICE_RSVD_VALUE, p);
		
		s_Msg.iMessageLength += 22;
	}

	SendMsgBuff(&s_Msg,0);
}



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
			pVtaNode = AmsSearchVtaNode(AmsCfgTeller(tellerCfgPos).srvGrpIdPos, AmsCfgTeller(tellerCfgPos).tellerId,AmsCfgTeller(tellerCfgPos).tellerIdLen);
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
		lpAmsData->srvGrpIdPos = AmsCfgTeller(tellerCfgPos).srvGrpIdPos;

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
	    Sem_wait(&AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpIdpos).vtaCtrl);
		lstAdd(&AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpIdpos).vtaList, (NODE *)pVtaNode);
		Sem_post(&AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpIdpos).vtaCtrl);

		//send Login Rsp to Vta
		AmsSendVtaLoginRsp(lpAmsData,pMsg,iret);

		//send TermIdNo to Vta
		//AmsSendTermIdNoInfo(pMsg,pVtaNode,NULL);

		//send Term Cfg Info, not include TermIdNo
		//AmsSendTermCfgInfo(pMsg,pVtaNode,NULL);
		
	}

	

	return iret;

}



int VtaStateOperateReqProc(int iThreadId, MESSAGE_t *pMsg)
{
	int					iret = AMS_VTA_QUEUE_MNG_SUCCESS;
	LP_AMS_DATA_t		*lpAmsData = NULL;   //进程数据区指针
    VTA_NODE            *pVtaNode = NULL;		
	int                 pid = 0;
	unsigned int        amsPid = 0;
	unsigned char        tellerIdLen = 0;
	unsigned char       tellerId[AMS_MAX_TELLER_ID_LEN + 1] = { 0 };		
	unsigned short      stateOperate = 0;
	STATE_OP_INFO       stateOpInfo;
	unsigned char       timerPara[AMS_MAX_TIMER_PARA_LEN];
	unsigned char       stateOpInfoSet = 0;
	unsigned char       *p;

	//进程号有效检查
	pid = pMsg->s_ReceiverPid.iProcessId;
	/if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		dbgprint("VtaStateOperateReqProc Pid:%d Err", pid);
		iret = AMS_VTA_STATE_OPERATE_PARA_ERR;
		AmsSendVtaStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}
	
	//消息长度检查


	lpAmsData = (LP_AMS_DATA_t *)ProcessData[pid];

	//amsPid检查
	p = pMsg->cMessageBody;
	BEGETLONG(amsPid,p);
	if(lpAmsData->amsPid != amsPid)
	{
		dbgprint("VtaStateOperateReqProc[%d] AmsPid[0x%x][0x%x] Err",pid,amsPid,lpAmsData->amsPid);
		iret = AMS_VTA_STATE_OPERATE_AMS_PID_ERR;
		AmsSendVtaStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}

	p += 4;

	//进程号匹配性检查
	if(lpAmsData->myPid.iProcessId != pid)
	{
		dbgprint("VtaStateOperateReqProc PID[%d][%d] Not Equal", lpAmsData->myPid.iProcessId, pid);
		iret = AMS_VTA_STATE_OPERATE_PARA_ERR;
		AmsSendVtaStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}

	//坐席检查
	tellerIdLen = *p++;
	if(tellerIdLen > AMS_MAX_TELLER_ID_LEN && tellerIdLen != lpAmsData->tellerIdLen)
	{
		dbgprint("VtaStateOperateReqProc[%d] TellerId[%u][%u]Err", 
			pid, lpAmsData->tellerId, tellerId);
		iret = AMS_VTA_STATE_OPERATE_TELLER_ID_ERR;
		AmsSendVtaStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;	
	}

	memcpy(tellerId,p,tellerIdLen);
	if(0 != (memcmp(tellerId,lpAmsData->tellerId,tellerIdLen)))
	{
		dbgprint("VtaStateOperateReqProc[%d] TellerId[%u][%u]Err", 
			pid, lpAmsData->tellerId, tellerId);
		iret = AMS_VTA_STATE_OPERATE_TELLER_ID_ERR;
		AmsSendVtaStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;			
	}
	p+=tellerIdLen;

	//操作码检查
	BEGETSHORT(stateOperate,p);
	if(stateOperate < VTA_STATE_OPERATE_IDLE || stateOperate >= VTA_STATE_OPERATE_MAX)
	{
		dbgprint("VtaStateOperateReqProc[%d] Teller[%s]StateOperateCode[%d]Err", 
			pid, tellerId, stateOperate);
		iret = AMS_VTA_STATE_OPERATE_CODE_ERR;
		AmsSendVtaStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;					
	}
	p+=2;

	lpAmsData->currStateOp = stateOperate;

	if( *p == 0x02)
	{
		if(stateOperate != VTA_STATE_OPERATE_BUSY && stateOperate != VTA_STATE_OPERATE_REST)
		{
			dbgprint("VtaStateOperateReqProc[%d] Teller[%s] Opart Len[%d]Err", 
				pid, tellerId, pMsg->iMessageLength);
			iret = AMS_VTA_STATE_OPERATE_LEN_ERR;
			AmsSendVtaStateOperateRsp(lpAmsData,pMsg,iret);
			return AMS_ERROR;				
		}
		//unpack OPart,e.g :stateOpInfo
		memset(&stateOpInfo,0,sizeof(STATE_OP_INFO));
		iret = AmsUnpackStateOperateReqOpartPara(p ,pMsg->iMessageLength-tellerIdLen-7,&stateOpInfo);
		if(AMS_OK != iret)
		{
			dbgprint("VtaStateOperateReqProc[%d] Teller[%s] UnpackOpartParaErr", pid, tellerNo);
			if(AMS_ERROR == iret)
			{
				iret = AMS_VTA_STATE_OPERATE_PARA_ERR;
			}
			
			AmsSendVtaStateOperateRsp(lpAmsData,pMsg,iret);
			return AMS_ERROR;	
		}
		stateOpInfoSet = 1;
	}
	else
	{
		if(stateOperate == VTA_STATE_OPERATE_BUSY && stateOperate == VTA_STATE_OPERATE_REST)
		{
			dbgprint("VtaStateOperateReqProc[%d] Teller[%s] Opart Len[%d]Err", 
				pid, tellerId, pMsg->iMessageLength);
			iret = AMS_VTA_STATE_OPERATE_LEN_ERR;
			AmsSendVtaStateOperateRsp(lpAmsData,pMsg,iret);
			return AMS_ERROR;				
		}		
	}

	//业务组编号检查
	if(lpAmsData->srvGrpIdPos > AMS_MAX_SERVICE_GROUP_NUM)
	{
		dbgprint("VtaStateOperateReqProc[%d] Teller[%s] SrvGrpId[%u]Err", 
			pid, tellerId, lpAmsData->srvGrpId);
		iret = AMS_VTA_STATE_OPERATE_SERVICE_GROUP_ID_ERR;
		AmsSendVtaStateOperateRsp(lpAmsData,pMsg,iret);
		return AMS_ERROR;		
	}

	//业务状态检查
	if(AMS_SERVICE_ACTIVE != AmsSrvData(lpAmsData->srvGrpIdPos).serviceState)
	{
		dbgprint("VtaStateOperateReqProc[%d] Teller[%s] ServiceState[%u]Err", 
			pid, tellerId, AmsSrvData(lpAmsData->srvGrpId).serviceState);
		iret = AMS_VTA_STATE_OPERATE_SERVICE_STATE_ERR;
		AmsSendVtaStateOperateRsp(lpAmsData,pMsg,iret);
		return AMS_ERROR;		
	}

	/*find vta Node*/
	pVtaNode = AmsSearchVtaNode(lpAmsData->srvGrpIdPos, lpAmsData->tellerId,lpAmsData->tellerIdLen);
	if(NULL == pVtaNode)
	{
		dbgprint("VtaStateOperateReqProc[%d] Teller[%s]Id[%u]Err", 
			pid, tellerId, lpAmsData->tellerId);		
		iret = AMS_VTA_STATE_OPERATE_TELLER_ID_ERR;
		AmsSendVtaStateOperateRsp(lpAmsData,pMsg,iret);
		return AMS_ERROR;		
	}

	//坐席状态检查
	if(pVtaNode->state >= AMS_VTA_STATE_OFFLINE)
	{
		dbgprint("VtaStateOperateReqProc[%d] Teller[%s][%u]State[%d]Err", 
			pid, tellerId, lpAmsData->tellerId, pVtaNode->state);
		iret = AMS_VTA_STATE_OPERATE_STATE_ERR;
		AmsSendVtaStateOperateRsp(lpAmsData,pMsg,iret);
		return AMS_ERROR;		
	}

	//update rpid
	lpAmsData->rPid.cModuleId   = pMsg->s_SenderPid.cModuleId;
	lpAmsData->rPid.cFunctionId = pMsg->s_SenderPid.cFunctionId;
	lpAmsData->rPid.iProcessId  = pMsg->s_SenderPid.iProcessId;

	//update vta state and state start time
	if(1 == stateOpInfoSet)
	{
		if(AMS_OK != AmsUpdateVtaState(iThreadId,lpAmsData,pVtaNode,stateOperate,stateOpInfo.reason))
		{
			dbgprint("VtaStateOperateReqProc[%d] Teller[%s][%u]UpdateVtaState[%d]Err!", 
				pid, tellerId, lpAmsData->tellerId, pVtaNode->state);
			iret = AMS_VTA_STATE_OPERATE_UPDATE_STATE_ERR;
			AmsSendVtaStateOperateRsp(lpAmsData,pMsg,iret);
			return AMS_ERROR;					
		}
	}
	else
	{
		if(AMS_OK != AmsUpdateVtaState(iThreadId, lpAmsData, pVtaNode, stateOperate, 0))
		{
			dbgprint("VtaStateOperateReqProc[%d] Teller[%s][%u]UpdateVtaState[%d]Err", 
				pid, tellerId, lpAmsData->tellerId, pVtaNode->state);
			iret = AMS_VTA_STATE_OPERATE_UPDATE_STATE_ERR;
			AmsSendVtaStateOperateRsp(lpAmsData,pMsg,iret);
			return AMS_ERROR;					
		}	
	}

	if(stateOperate != VTA_STATE_OPERATE_REST)
	{
		/* 杀掉定时器 */
		if(lpAmsData->restTimerId >= 0)
		{
			AmsKillTimer(lpAmsData->myPid.iProcessId, &lpAmsData->restTimerId);
			//AmsTimerStatProc(T_AMS_REST_TIMER, AMS_KILL_TIMER); 
		}		
	}

	if(VTA_STATE_OPERATE_REST == stateOperate && 1 == stateOpInfoSet)
	{
		if(stateOpInfo.timeLen > AmsCfgData.vtaRestTimeLength)
		{
			stateOpInfo.timeLen = AmsCfgData.vtaRestTimeLength;
		}
		
		if(stateOpInfo.timeLen > 0)
		{
			//wait rest timeout ,default: 5s
			if(    AmsCfgData.vtaRestTimeLength > 0 
				&& AmsCfgData.vtaRestTimeLength <= T_AMS_REST_TIMER_LENGTH_MAX)
			{
				memset(timerPara, 0, PARA_LEN);
			    if(AMS_OK != AmsCreateTimerPara(lpAmsData->myPid.iProcessId,
						                        &lpAmsData->restTimerId, 
												B_AMS_REST_TIMEOUT, 
												stateOpInfo.timeLen,
												timerPara))
			    {
					dbgprint("VtaStateOperateReqProc[%d] Teller[%s][%u] CreateTimer Err",
						pid, tellerId, lpAmsData->tellerId);
					iret = AMS_VTA_STATE_OPERATE_START_TIMER_ERR;
					AmsSendVtaStateOperateRsp(lpAmsData,pMsg,iret);
					return AMS_ERROR;
			    }

				//AmsTimerStatProc(T_AMS_REST_TIMER, AMS_CREATE_TIMER);

			    if(lpAmsData->commonTrace)
			    {
			        dbgprint("Ams[%d] Create T_AMS_REST_TIMER Timer:timerId=%d.",
						pid, lpAmsData->restTimerId);
			    }
			}
		}
	}
	AmsSendVtaStateOperateRsp(lpAmsData,pMsg,iret);

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

int AmsSendVtaStateOperateRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret)
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
	s_Msg.iMessageType = A_VTA_STATE_OPERATE_RSP;
	s_Msg.iMessageLength = 0;

    p = &s_Msg.cMessageBody[0];
	if(lpAmsData != NULL)
	{
		BEPUTLONG(lpAmsData->amsPid, p);
		p += 4;
		
		p = lpAmsData->tellerIdLen;
		p += 1;
		memcpy(p,lpAmsData->tellerId,lpAmsData->tellerIdLen);
		p += lpAmsData->tellerIdLen;
		
		BEPUTSHORT(lpAmsData->currStateOp, p);
		p += 2;
	}
	else
	{
		memcpy(p, pMsg->cMessageBody, 4);
		p[4]=pMsg->cMessageBody[4];
		p+=5;
		memcpy(p, pMsg->cMessageBody[5],pMsg->cMessageBody[4]);
		p+=pMsg->cMessageBody[4];
		memcpy(p,pMsg->cMessageBody[5+pMsg->cMessageBody[4]],2);
		p+=2;
	}
	
	BEPUTLONG(iret, p);

	s_Msg.iMessageLength = 11+pMsg->cMessageBody[4];

	SendMsgBuff(&s_Msg,0);

	if(lpAmsData != NULL)
	{
		/*switch(lpAmsData->currStateOp)
		{				
		case VTA_STATE_OPERATE_IDLE: 
			AmsTellerStatProc(lpAmsData->tellerId, lpAmsData->tellerPos, AMS_TELLER_SET_IDLE, iret);
			break;
			
		case VTA_STATE_OPERATE_BUSY: 
			AmsTellerStatProc(lpAmsData->tellerId, lpAmsData->tellerPos, AMS_TELLER_SET_BUSY, iret);
			break;

		case VTA_STATE_OPERATE_REST: 
			AmsTellerStatProc(lpAmsData->tellerId, lpAmsData->tellerPos, AMS_TELLER_SET_REST, iret);
			break;

		case VTA_STATE_OPERATE_PREPARE: 
			AmsTellerStatProc(lpAmsData->tellerId, lpAmsData->tellerPos, AMS_TELLER_SET_PREPARA, iret);
			break;
			
		default:
			AmsTellerStatProc(lpAmsData->tellerId, lpAmsData->tellerPos, AMS_TELLER_STAT_TYPE_MAX, iret);
			return AMS_ERROR;
		}*/
	}

	//AmsMsgStatProc(AMS_VTA_MSG, s_Msg.iMessageType);
    //AmsResultStatProc(AMS_VTA_STATE_OPERATE_RESULT, iret);
	
	return SUCCESS;
}

int AmsSendTellerEventInd(LP_AMS_DATA_t *lpAmsData,unsigned int tellerEventInd, unsigned char vtmid[],unsigned char vtmidlen,unsigned int vtmtype,int iret)
{
	MESSAGE_t			s_Msg;
	unsigned char		*p;

	memset(&s_Msg,0,sizeof(MESSAGE_t));

	if(NULL == lpAmsData)
	{
		return AMS_ERROR;
	}
	
	s_Msg.eMessageAreaId = A;
	memcpy(&s_Msg.s_ReceiverPid,&lpAmsData->rPid,sizeof(PID_t));
	s_Msg.s_SenderPid.cModuleId = SystemData.cMid;
	s_Msg.s_SenderPid.cFunctionId = FID_AMS;
	s_Msg.s_SenderPid.iProcessId = lpAmsData->myPid.iProcessId;
	s_Msg.iMessageType = A_VTA_EVENT_IND;
	s_Msg.iMessageLength = 0;

	p = &s_Msg.cMessageBody[0];
	BEPUTLONG(tellerEventInd,p);
	p += 4;
	
	BEPUTLONG(lpAmsData->amsPid, p);
	p += 4;

	*p++= lpAmsData->tellerIdLen;
	memcpy(p,lpAmsData->tellerId,lpAmsData->tellerIdLen);
	p += lpAmsData->tellerIdLen;

	BEPUTLONG(vtmtype,p);
	p += 4;
	
	*p++= vtmidlen;
	memcpy(p,vtmid,vtmidlen);
	p += vtmidlen;

	BEPUTLONG(iret,p);
	p += 4;

	s_Msg.iMessageLength = 14+vtmidlen+lpAmsData->tellerIdLen;

	SendMsgBuff(&s_Msg,0);

	return AMS_OK;
}


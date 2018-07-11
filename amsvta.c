#include "amsfunc.h"

int VtaLoginReqProc(int iThreadId, MESSAGE_t *pMsg)
{
	int					 iret = AMS_VTA_QUEUE_MNG_SUCCESS;
	int					 ret = AMS_VTA_QUEUE_MNG_SUCCESS;
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
	int                  i = 0,j = 0;
	unsigned char        *p;	
	int					 telleridhash=0;
	TELLER_INFO_NODE	*pTellerInfoNode = NULL;
	MESSAGE_t           s_Msg;
	unsigned int		packlen=0;
	TELLER_PERSONAL_INFO tellerpesonalinfo;
	TELLER_REGISTER_INFO_NODE  *pRegTellerInfoNode = NULL;

	memset(&s_Msg,0,sizeof(MESSAGE_t));

	if(AmsMsgTrace)
	{
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"recv A_VTA_LOGIN_REQ msg \n");	
		AmsTraceToFile(pMsg->s_ReceiverPid,pMsg->s_SenderPid,"A_VTA_LOGIN_REQ",description,
						descrlen,pMsg->cMessageBody,pMsg->iMessageLength,"ams");		
	}

	pid = pMsg->s_SenderPid.iProcessId;

	//检查接受进程号
	if(pMsg->s_ReceiverPid.iProcessId != 0)
	{
		dbgprint("VtaLoginReqProc[%d] Pid:%d Err", pid, pMsg->s_ReceiverPid.iProcessId);
		ret = AMS_VTA_LOGIN_PID_ERR;
		s_Msg.iMessageLength = 5;
		AmsSendVtaLoginRsp(pMsg,ret,&s_Msg,0);
		return AMS_ERROR;
	}

	//个数获取
	p = pMsg->cMessageBody;
	BEGETSHORT(tellerLoginNum, p);
	p+=2;

	
	//检查已登陆柜员数量
	for(j = 0; j< AMS_MAX_SERVICE_GROUP_NUM;j++)
	{
			tellerNum += lstCount(&AmsSrvData(j).vtaList);
	}
	
	for(i=0;i<tellerLoginNum;i++)
	{
		iret = AMS_VTA_QUEUE_MNG_SUCCESS;
		//检查登陆信息
		//check teller id
		tellerIdLen=*p++;
		if(tellerIdLen > AMS_MAX_TELLER_ID_LEN)
		{
			dbgprint("VtaLoginReqProc[%d] TellerIdLen[%d] Err", pid, tellerIdLen);
			iret = AMS_VTA_LOGIN_TELLER_LEN_ERR;
			ret = AMS_VTA_LOGIN_PARA_ERR;
			p+=tellerIdLen;
			packlen+=AmsPackVtaLoginBase(tellerIdLen,p,iret,&s_Msg.cMessageBody[6+packlen],NULL);			
			continue;
		}
		memcpy(tellerId,p,tellerIdLen);
		p += tellerIdLen;
		
		//check teller pwd
		tellerPwdLen = *p++;
		if(tellerPwdLen > AMS_MAX_PWD_LEN)
		{
			dbgprint("VtaLoginReqProc[%d] TellerIdLen[%d] Err", pid, tellerIdLen);
			iret = AMS_VTA_LOGIN_TELLER_PWD_ERR;
			ret = AMS_VTA_LOGIN_PARA_ERR;
			p+=tellerPwdLen;
			packlen+=AmsPackVtaLoginBase(tellerIdLen,tellerId,iret,&s_Msg.cMessageBody[6+packlen],NULL);			
			continue;			
		}
		memcpy(tellerPwd,p,tellerPwdLen);
		p+=tellerPwdLen;

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
			ret = AMS_VTA_LOGIN_PARA_ERR;
			packlen+=AmsPackVtaLoginBase(tellerIdLen,tellerId,iret,&s_Msg.cMessageBody[6+packlen],NULL);
			continue;
		}

		/*check teller in regist or not*/
		

		//比较配置的pwd和登陆的pwd是否一致
		if(0 != strcmp(AmsCfgTeller(tellerCfgPos).tellerPwd, tellerPwd))
		{
			dbgprint("VtaLoginReqProc[%d] Pwd[%s]Err", pid, tellerPwd);
			iret = AMS_VTA_LOGIN_TELLER_PWD_ERR;
			ret = AMS_VTA_LOGIN_PARA_ERR;
			packlen+=AmsPackVtaLoginBase(tellerIdLen,tellerId,iret,&s_Msg.cMessageBody[6+packlen],NULL);
			continue;	
		}

		//检查cfg and sys vtanum
		if(tellerNum >= SystemData.AmsPriData.amsCfgData.maxVtaNum || tellerNum >= AMS_MAX_VTA_NODES)
		{
			dbgprint("VtaLoginReqProc[%d] Teller[%s]TelleNum[%d]-[%d][%d] Err", 
				pid, tellerId, tellerNum, SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NODES);
			iret = AMS_VTA_LOGIN_TELLER_NUM_ERR;
			ret = AMS_VTA_LOGIN_PARA_ERR;
			packlen+=AmsPackVtaLoginBase(tellerIdLen,tellerId,iret,&s_Msg.cMessageBody[6+packlen],NULL);
			continue;
		}

		//check teller type
		if(AmsCfgSrvGroup(AmsCfgTeller(tellerCfgPos).srvGrpIdPos).isAutoFlag == AMS_SRVGRP_TYPE_HUMAN)
		{
			if(	 tellerLoginNum > 1)
			{
				dbgprint("VtaLoginReqProc[%d] teller[] Num[] Err Only one humanteller pertime login",pid,tellerId,tellerLoginNum);
				iret = AMS_VTA_LOGIN_TELLER_NUM_ERR;
				ret  = AMS_VTA_LOGIN_PARA_ERR;
				packlen+=AmsPackVtaLoginBase(tellerIdLen,tellerId,iret,&s_Msg.cMessageBody[6+packlen],NULL);
				break;
			}
			else
			{
				memset(&tellerpesonalinfo,0,sizeof(TELLER_PERSONAL_INFO));
				iret = AmsUnpackTellerpersionalinfo(p,pMsg->iMessageLength-4-tellerIdLen-tellerPwdLen,&tellerpesonalinfo);
				break;
			}

			/* chcek teller in registnode or not*/
			pRegTellerInfoNode=AmsSearchRegTellerInfoHash(tellerId,tellerIdLen);
			if(NULL == pRegTellerInfoNode)
			{
				dbgprint("VtaLoginReqProc[%d] TellerId[%s] not registed",pid,tellerId);
				iret = AMS_VTA_LOGIN_TELLER_LOGIN_NOTREGSITER;
				ret = AMS_VTA_LOGIN_PARA_ERR;
				packlen+=AmsPackVtaLoginBase(tellerIdLen,tellerId,iret,&s_Msg.cMessageBody[6+packlen],NULL);
				break;
			}
		}
		
		/* check teller in process or not*/
		if(AMS_SERVICE_ACTIVE == AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpIdPos).serviceState)
		{
			pVtaNode = AmsSearchVtaNode(AmsCfgTeller(tellerCfgPos).srvGrpIdPos, AmsCfgTeller(tellerCfgPos).tellerId,AmsCfgTeller(tellerCfgPos).tellerIdLen);
			if(NULL != pVtaNode)
			{
				dbgprint("VtaLoginReqProc[%d] TellerNo[%s]Idhas been Logined[%d]", 
					pid, tellerId, tellerCfgPos);		
				iret = AMS_VTA_LOGIN_TELLER_LOGIN_REPEATEDLY;
				ret = AMS_VTA_LOGIN_PARA_ERR;
				packlen+=AmsPackVtaLoginBase(tellerIdLen,tellerId,iret,&s_Msg.cMessageBody[6+packlen],NULL);
				continue;
			}	
		}

		//分配服务进程号
		memset(&Pid,0,sizeof(PID_t));
		iret = AmsAllocPid(&Pid);
		if(-1 == iret)
		{
			dbgprint("VtaLoginReqProc[%d] Teller[%s] AmsAllocPid: SysBusy", pid, tellerId);
			iret = AMS_VTA_LOGIN_LP_RESOURCE_LIMITED;
			ret = AMS_VTA_LOGIN_PARA_ERR;
			packlen+=AmsPackVtaLoginBase(tellerIdLen,tellerId,iret,&s_Msg.cMessageBody[6+packlen],NULL);
			continue;
		}

	    //分配TELLER结点
		pVtaNode = VtaNodeGet();
		if(NULL == pVtaNode)
		{
	 		dbgprint("VtaLoginReqProc[%d] Teller[%s] VtaNodeGet Failed", pid, tellerId);
			iret = AMS_VTA_LOGIN_NODE_RESOURCE_LIMITED;
			ret = AMS_VTA_LOGIN_PARA_ERR;
			packlen+=AmsPackVtaLoginBase(tellerIdLen,tellerId,iret,&s_Msg.cMessageBody[6+packlen],NULL);
			AmsReleassPid(Pid, FAILURE);
			continue;
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

		//消息跟踪开关配置
		lpAmsData->debugTrace  = (unsigned char)AmsDebugTrace;	
		lpAmsData->commonTrace = (unsigned char)AmsCommonTrace;
		lpAmsData->msgTrace    = (unsigned char)AmsMsgTrace;
		lpAmsData->stateTrace  = (unsigned char)AmsStateTrace;
		lpAmsData->timerTrace  = (unsigned char)AmsTimerTrace;
		lpAmsData->errorTrace  = (unsigned char)AmsErrorTrace;
		lpAmsData->alarmTrace  = (unsigned char)AmsAlarmTrace;	

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
	    Sem_wait(&AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpIdPos).vtaCtrl);
		lstAdd(&AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpIdPos).vtaList, (NODE *)pVtaNode);
		Sem_post(&AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpIdPos).vtaCtrl);

		/* Add Vta Node to free List */
	    Sem_wait(&AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpIdPos).freevtaCtrl);
		lstAdd(&AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpIdPos).freevtaList, (NODE *)pVtaNode);
		Sem_post(&AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpIdPos).freevtaCtrl);

		packlen+=AmsPackVtaLoginBase(tellerIdLen,tellerId,iret,&s_Msg.cMessageBody[6+packlen],lpAmsData);

		tellerNum+=1;
	}

	//pack 人工参数

	s_Msg.iMessageLength = packlen + 6;
	//send Login Rsp to Vta
	AmsSendVtaLoginRsp(pMsg,ret,&s_Msg,i);		
	
	return AMS_OK;

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


	if(AmsMsgTrace)
	{
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"recv A_VTA_STATE_OPERATE_REQ msg \n");	
		AmsTraceToFile(pMsg->s_ReceiverPid,pMsg->s_SenderPid,"A_VTA_STATE_OPERATE_REQ",description,
						descrlen,pMsg->cMessageBody,pMsg->iMessageLength,"ams");		
	}	

	//进程号有效检查
	pid = pMsg->s_ReceiverPid.iProcessId;
	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
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
	if(0 != (strcmp(tellerId,lpAmsData->tellerId)))
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
			dbgprint("VtaStateOperateReqProc[%d] Teller[%s] UnpackOpartParaErr", pid, tellerId);
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
			pid, tellerId, AmsSrvData(lpAmsData->srvGrpIdPos).serviceState);
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
			AmsTimerStatProc(T_AMS_REST_TIMER, AMS_KILL_TIMER); 
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

				AmsTimerStatProc(T_AMS_REST_TIMER, AMS_CREATE_TIMER);

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

int VtaStateOperateCnfProc(int iThreadId, MESSAGE_t *pMsg)
{
	int					iret = AMS_VTA_QUEUE_MNG_SUCCESS;
	LP_AMS_DATA_t		*lpAmsData = NULL;          //进程数据区指针
	LP_AMS_DATA_t		*lpAmsManagerData = NULL;   //进程数据区指针	
	VTA_NODE            *pVtaNode = NULL;		
	VTA_NODE            *pManagerNode = NULL;			
	int                 pid = 0;
	int                 managerPid = 0;	
	unsigned int        amsPid = 0;
	unsigned char       tellerIdLen = 0;
	unsigned char       tellerId[AMS_MAX_TELLER_ID_LEN + 1] = { 0 };	
//	unsigned int        managerTellerId = 0;	
//	unsigned char       managerNo[AMS_MAX_TELLER_NO_LEN + 1] = { 0 };		
	unsigned int      stateOpInd = 0;
	unsigned int        stateSet = VTA_STATE_OPERATE_IND_RSVD;
	time_t              currentTime;	
	unsigned int        i = 0;
	unsigned char       *p;
	unsigned short      stateOperate = 0;

	if(AmsMsgTrace)
	{
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"recv A_VTA_STATE_OPERATE_CNF msg \n");	
		AmsTraceToFile(pMsg->s_ReceiverPid,pMsg->s_SenderPid,"A_VTA_STATE_OPERATE_CNF",description,
						descrlen,pMsg->cMessageBody,pMsg->iMessageLength,"ams");		
	}

	//进程号有效性检查
	pid = pMsg->s_ReceiverPid.iProcessId;
	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		dbgprint("VtaStateOperateCnfProc Pid:%d Err", pid);
		iret = AMS_VTA_STATE_OPERATE_IND_CNF_PARA_ERR;
		//AmsResultStatProc(AMS_MANAGER_SET_VTA_STATE_RESULT, iret);
		return AMS_ERROR;
	}

	lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
	
	//amsPid检查
	p = pMsg->cMessageBody;
	BEGETLONG(amsPid,p);
	if(lpAmsData->amsPid != amsPid)
	{
		dbgprint("VtaStateOperateCnfProc[%d] AmsPid[0x%x][0x%x] Err",pid,amsPid,lpAmsData->amsPid);
		iret = AMS_VTA_STATE_OPERATE_IND_CNF_AMS_PID_ERR;
		AmsSendVtaStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}

	p += 4;

	//进程号匹配性检查
	if(lpAmsData->myPid.iProcessId != pid)
	{
		dbgprint("VtaStateOperateCnfProc PID[%d][%d] Not Equal", lpAmsData->myPid.iProcessId, pid);
		iret = AMS_VTA_STATE_OPERATE_PARA_ERR;
		AmsSendVtaStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}

	//坐席检查
	tellerIdLen = *p++;
	if(tellerIdLen > AMS_MAX_TELLER_ID_LEN && tellerIdLen != lpAmsData->tellerIdLen)
	{
		dbgprint("VtaStateOperateCnfProc[%d] TellerId[%u][%u]Err", 
			pid, lpAmsData->tellerId, tellerId);
		iret = AMS_VTA_STATE_OPERATE_IND_CNF_TELLER_ID_ERR;
		AmsSendVtaStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;	
	}

	memcpy(tellerId,p,tellerIdLen);
	if(0 != (strcmp(tellerId,lpAmsData->tellerId)))
	{
		dbgprint("VtaStateOperateCnfProc[%d] TellerId[%u][%u]Err", 
			pid, lpAmsData->tellerId, tellerId);
		iret = AMS_VTA_STATE_OPERATE_IND_CNF_TELLER_ID_ERR;
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
		iret = AMS_VTA_STATE_OPERATE_IND_CNF_CODE_ERR;
		AmsSendVtaStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;					
	}
	p+=2;

	if(lpAmsData->iTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->iTimerId);
		AmsTimerStatProc(T_AMS_VTA_STATE_OPERATE_IND_TIMER, AMS_KILL_TIMER);
	}
	
	//iret
	BEGETLONG(iret, p);//原因值统一编码待定
	
	if(AMS_VTA_QUEUE_MNG_SUCCESS == iret)
	{
		if(stateOperate == VTA_STATE_OPERATE_IDLE)
		{
			//AmsKillVtaAllTimer(lpAmsData, pid);
	
			//仅杀掉呼叫相关定时器，包括消息、文件收发
			AmsKillVtaAllCallTimer(lpAmsData, pid);
			
			//update time
			memset(&pVtaNode->callStateStartLocalTime, 0, sizeof(TIME_INFO)); 
			memset(&pVtaNode->callStateStartTime, 0, sizeof(time_t)); 
	
			//reset callTransferNum
			//			pVtaNode->callTransferNum = 0;
	
#ifdef AMS_TEST_LT
			//calc vta workInfo
			time(&currentTime);  
			AmsUpdateSingleVtaWorkInfo(pVtaNode, currentTime);
	
			//set Vta State and State Start Time
			AmsSetVtaState(iThreadId, lpAmsData, pVtaNode, AMS_VTA_STATE_IDLE, 0);
#endif		

			//add vtanode to freevtanode
			Sem_wait(&AmsSrvData(lpAmsData->srvGrpIdPos).freevtaCtrl);
			lstAdd(&AmsSrvData(lpAmsData->srvGrpIdPos).freevtaList, (NODE *)pVtaNode);
			Sem_post(&AmsSrvData(lpAmsData->srvGrpIdPos).freevtaCtrl);

		}
	}

	return AMS_OK;
}

int AmsSendVtaLoginRsp(MESSAGE_t *pMsg,int iret,MESSAGE_t *s_Msg,int num)
{
	unsigned char       *p;
	int	vtanum=0;

	if(NULL == pMsg || NULL == s_Msg)
	{
		return AMS_ERROR;
	}
	
	s_Msg->eMessageAreaId = A;
	memcpy(&s_Msg->s_ReceiverPid,&pMsg->s_SenderPid,sizeof(PID_t));
	s_Msg->s_SenderPid.cModuleId = SystemData.cMid;
	s_Msg->s_SenderPid.cFunctionId = FID_AMS;
	s_Msg->s_SenderPid.iProcessId = pMsg->s_ReceiverPid.iProcessId;
	s_Msg->iMessageType = A_VTA_LOGIN_RSP;
	
	p = &s_Msg->cMessageBody[0];
	BEPUTLONG(iret, p);
	p+=4;

	BEPUTSHORT(num,p);
	
	SendMsgBuff(s_Msg,0);

	if(AmsMsgTrace)
	{	
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"send A_VTA_LOGIN_RSP msg \n");	
		{
			AmsTraceToFile(s_Msg->s_ReceiverPid,s_Msg->s_SenderPid,"A_VTA_LOGIN_RSP",description,
				descrlen,s_Msg->cMessageBody,s_Msg->iMessageLength,"ams");	
		}
	}
	AmsMsgStatProc(AMS_VTA_MSG, s_Msg->iMessageType); 
	AmsResultStatProc(AMS_VTA_LOGIN_RESULT, iret);
	return AMS_OK;
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
		
		*p++= lpAmsData->tellerIdLen;
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
		memcpy(p,&pMsg->cMessageBody[5],pMsg->cMessageBody[4]);
		p+=pMsg->cMessageBody[4];
		memcpy(p,&pMsg->cMessageBody[5+pMsg->cMessageBody[4]],2);
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

	AmsMsgStatProc(AMS_VTA_MSG, s_Msg.iMessageType);
    AmsResultStatProc(AMS_VTA_STATE_OPERATE_RESULT, iret);

	if(AmsMsgTrace)
	{	
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"send A_VTA_STATE_OPERATE_RSP msg \n");	
		{
			AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_VTA_STATE_OPERATE_RSP",description,
				descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,"ams");	
		}
	}
	
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

	if(AmsMsgTrace)
	{	
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"send A_VTA_EVENT_IND msg \n");	
		{
			AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_VTA_EVENT_IND",description,
				descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,"ams");	
		}
	}
	AmsMsgStatProc(AMS_VTA_MSG, s_Msg.iMessageType); 

	return AMS_OK;
}



int AmsSendVtaStateOperateInd(LP_AMS_DATA_t *lpAmsData, MESSAGE_t *pMsg, unsigned short stateOpInd,unsigned short statesubOpInde)
{
	MESSAGE_t           s_Msg;
	unsigned char       *p;
	unsigned char       timerPara[AMS_MAX_TIMER_PARA_LEN];

	memset(&s_Msg,0,sizeof(MESSAGE_t));

	if(NULL == pMsg)
	{
		return AMS_ERROR;
	}
	
	s_Msg.eMessageAreaId = A;
	memcpy(&s_Msg.s_ReceiverPid,&lpAmsData->rPid,sizeof(PID_t));
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
	s_Msg.iMessageType = A_VTA_STATE_OPERATE_IND;
	s_Msg.iMessageLength = 0;
	
	p = &s_Msg.cMessageBody[0];
	if(lpAmsData != NULL)
	{
		BEPUTLONG(lpAmsData->amsPid, p);
		p += 4;
		
		//BEPUTLONG(lpAmsData->tellerId, p);
		//p += 4;
		*p++=lpAmsData->tellerIdLen;
		memcpy(p,lpAmsData->tellerId,lpAmsData->tellerIdLen);
		p+=lpAmsData->tellerIdLen;
		s_Msg.iMessageLength += 5+lpAmsData->tellerIdLen;
	}
	else
	{
		memcpy(p, pMsg->cMessageBody, 8);
		p += 8;
		//待改
	}
	
	BEPUTSHORT(stateOpInd, p);
	p += 2;
	s_Msg.iMessageLength += 2;

	if(stateOpInd == VTA_STATE_OPERATE_IND_IDLE || stateOpInd == VTA_STATE_OPERATE_IND_BUSY)
	{
		*p++ = AMS_STATE_OP_INFO_ID;

		BEPUTSHORT(4,p);
		p += 4;

		BEPUTSHORT(statesubOpInde,p);
		p +=2;

		BEPUTSHORT(0,p);
		p +=2;
		s_Msg.iMessageLength += 9;
	}
	
	SendMsgBuff(&s_Msg,0);

	 if(    AmsCfgData.vtaStateOperateIndTimeLength > 0 
	    && AmsCfgData.vtaStateOperateIndTimeLength <= T_VTA_OPERATE_IND_TIMER_LENGTH_MAX)
    {
    	memset(timerPara, 0, PARA_LEN);
		p = timerPara;
		
		BEPUTLONG(lpAmsData->amsPid, p);

        if(AMS_OK != AmsCreateTimerPara(lpAmsData->myPid.iProcessId,
			                        &lpAmsData->iTimerId, 
									B_AMS_VTA_STATE_OP_IND_TIMEOUT, 
									AmsCfgData.vtaStateOperateIndTimeLength,
									timerPara))
        {
			dbgprint("VtaStateOperateReqProc Teller[%s]CreateTimer Err",
						lpAmsData->tellerId);
        }

		AmsTimerStatProc(T_AMS_VTA_STATE_OPERATE_IND_TIMER, AMS_CREATE_TIMER);

        if(lpAmsData->commonTrace)
        {
            dbgprint("Ams Create T_AMS_VTA_STATE_OPERATE_IND_TIMER Timer:timerId=%d.",
				 lpAmsData->iTimerId);
        }
    }
	

	if(AmsMsgTrace)
	{	
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"send A_VTA_STATE_OPERATE_IND msg \n");	
		{
			AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_VTA_STATE_OPERATE_IND",description,
				descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,"ams");	
		}
	}
	AmsMsgStatProc(AMS_VTA_MSG, s_Msg.iMessageType); 

	return SUCCESS;
}

//坐席示闲指示，超时，不改变当前坐席状态	
//坐席示忙指示，超时，坐席置忙
int VtaStateOperateIndTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg)
{
	//int					iret = AMS_MANAGER_SET_VTA_STATE_VTA_TIMEOUT;
	LP_AMS_DATA_t		*lpAmsData = NULL;   //进程数据区指针
	VTA_NODE            *pVtaNode = NULL;		
	int                 pid = 0;
	MESSAGE_t           msg;
	unsigned char       *p;
	unsigned int        amsPid = 0;

	
#ifdef AMS_TEST_LT
		time_t				currentTime;
#endif


	if(AmsMsgTrace)
	{
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"recv B_AMS_VTA_STATE_OP_IND_TIMEOUT msg[%d] \n",pTmMsg->iTimerId);	
		AmsTraceToFile(pTmMsg->s_ReceiverPid,pTmMsg->s_SenderPid,"B_AMS_VTA_STATE_OP_IND_TIMEOUT",description,
						descrlen,pTmMsg->cTimerParameter,PARA_LEN,"ams");
	}
	
	//进程号有效性检查
	pid = pTmMsg->s_ReceiverPid.iProcessId;
	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		dbgprint("VtaStateOperateIndTimeoutProc Pid:%d Err", pid);
		//iret = AMS_MANAGER_SET_VTA_STATE_TIMEOUT_PARA_ERR;
		//AmsResultStatProc(AMS_MANAGER_SET_VTA_STATE_RESULT, iret);   		
		return AMS_ERROR;
	}
	lpAmsData = (LP_AMS_DATA_t *)ProcessData[pid];

	//amsPid检查
	p = pTmMsg->cTimerParameter;
	BEGETLONG(amsPid,p);
	if(lpAmsData->amsPid != amsPid)
	{
		dbgprint("VtaStateOperateIndTimeoutProc[%d] AmsPid[0x%x][0x%x] Err",pid,amsPid,lpAmsData->amsPid);
		return AMS_ERROR;
	}

	/* 杀掉定时器 */
	if(lpAmsData->iTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->iTimerId);
		AmsTimerStatProc(T_AMS_VTA_STATE_OPERATE_IND_TIMER, AMS_KILL_TIMER);
		pTmMsg->iTimerId = -1;
	} 

	
	pVtaNode = AmsSearchVtaNode(lpAmsData->srvGrpIdPos, lpAmsData->tellerId,lpAmsData->tellerIdLen);
	if(NULL == pVtaNode)
	{
		dbgprint("VtaStateOperateIndTimeoutProc[%d] Teller[%s]Err", 
			pid,lpAmsData->tellerId);		
		return AMS_ERROR;		
	}
	
	return AMS_OK;
}


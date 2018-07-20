#include "amsfunc.h"

int CrmLoginReqProc(int iThreadId, MESSAGE_t *pMsg)
{
	int					 iret = AMS_CRM_QUEUE_MNG_SUCCESS;
	int					 ret = AMS_CRM_QUEUE_MNG_SUCCESS;
	LP_AMS_DATA_t		 *lpAmsData = NULL;   //进程数据区指针
	CRM_NODE             *pCrmNode = NULL;
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
		descrlen=snprintf(description,1024,"recv A_TELLER_LOGIN_REQ msg \n");	
		AmsTraceToFile(pMsg->s_ReceiverPid,pMsg->s_SenderPid,"A_TELLER_LOGIN_REQ",description,
						descrlen,pMsg->cMessageBody,pMsg->iMessageLength,"ams");		
	}

	pid = pMsg->s_SenderPid.iProcessId;

	//检查接受进程号
	if(pMsg->s_ReceiverPid.iProcessId != 0)
	{
		dbgprint("CrmLoginReqProc[%d] Pid:%d Err", pid, pMsg->s_ReceiverPid.iProcessId);
		ret = AMS_CRM_LOGIN_PID_ERR;
		s_Msg.iMessageLength = 5;
		AmsSendCrmLoginRsp(pMsg,ret,&s_Msg,0);
		return AMS_ERROR;
	}

	//个数获取
	p = pMsg->cMessageBody;
	BEGETSHORT(tellerLoginNum, p);
	p+=2;

	
	//检查已登陆柜员数量
	for(j = 0; j< AMS_MAX_SERVICE_GROUP_NUM;j++)
	{
			tellerNum += lstCount(&AmsSrvData(j).crmList);
	}
	
	for(i=0;i<tellerLoginNum;i++)
	{
		iret = AMS_CRM_QUEUE_MNG_SUCCESS;
		//检查登陆信息
		//check teller id
		tellerIdLen=*p++;
		if(tellerIdLen > AMS_MAX_TELLER_ID_LEN)
		{
			dbgprint("CrmLoginReqProc[%d] TellerIdLen[%d] Err", pid, tellerIdLen);
			iret = AMS_CRM_LOGIN_TELLER_LEN_ERR;
			ret = AMS_CRM_LOGIN_PARA_ERR;
			p+=tellerIdLen;
			packlen+=AmsPackCrmLoginBase(tellerIdLen,p,iret,&s_Msg.cMessageBody[6+packlen],NULL);			
			continue;
		}
		memcpy(tellerId,p,tellerIdLen);
		p += tellerIdLen;
		
		//check teller pwd
		tellerPwdLen = *p++;
		if(tellerPwdLen > AMS_MAX_PWD_LEN)
		{
			dbgprint("CrmLoginReqProc[%d] TellerIdLen[%d] Err", pid, tellerIdLen);
			iret = AMS_CRM_LOGIN_TELLER_PWD_ERR;
			ret = AMS_CRM_LOGIN_PARA_ERR;
			p+=tellerPwdLen;
			packlen+=AmsPackCrmLoginBase(tellerIdLen,tellerId,iret,&s_Msg.cMessageBody[6+packlen],NULL);			
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
			dbgprint("CrmLoginReqProc[%d] TellerId[%s][%d]not Find", pid, tellerId, tellerIdLen);
			iret = AMS_CRM_LOGIN_TELLER_NO_ERR;
			ret = AMS_CRM_LOGIN_PARA_ERR;
			packlen+=AmsPackCrmLoginBase(tellerIdLen,tellerId,iret,&s_Msg.cMessageBody[6+packlen],NULL);
			continue;
		}

		/*check teller in regist or not*/
		

		//比较配置的pwd和登陆的pwd是否一致
		if(0 != strcmp(AmsCfgTeller(tellerCfgPos).tellerPwd, tellerPwd))
		{
			dbgprint("CrmLoginReqProc[%d] Pwd[%s]Err", pid, tellerPwd);
			iret = AMS_CRM_LOGIN_TELLER_PWD_ERR;
			ret = AMS_CRM_LOGIN_PARA_ERR;
			packlen+=AmsPackCrmLoginBase(tellerIdLen,tellerId,iret,&s_Msg.cMessageBody[6+packlen],NULL);
			continue;	
		}

		//检查cfg and sys crmnum
		if(tellerNum >= SystemData.AmsPriData.amsCfgData.maxCrmNum || tellerNum >= AMS_MAX_CRM_NODES)
		{
			dbgprint("CrmLoginReqProc[%d] Teller[%s]TelleNum[%d]-[%d][%d] Err", 
				pid, tellerId, tellerNum, SystemData.AmsPriData.amsCfgData.maxCrmNum, AMS_MAX_CRM_NODES);
			iret = AMS_CRM_LOGIN_TELLER_NUM_ERR;
			ret = AMS_CRM_LOGIN_PARA_ERR;
			packlen+=AmsPackCrmLoginBase(tellerIdLen,tellerId,iret,&s_Msg.cMessageBody[6+packlen],NULL);
			continue;
		}

		//check teller type
		if(AmsCfgSrvGroup(AmsCfgTeller(tellerCfgPos).srvGrpIdPos).isAutoFlag == AMS_SRVGRP_TYPE_HUMAN)
		{
			if(	 tellerLoginNum > 1)
			{
				dbgprint("CrmLoginReqProc[%d] teller[] Num[] Err Only one humanteller pertime login",pid,tellerId,tellerLoginNum);
				iret = AMS_CRM_LOGIN_TELLER_NUM_ERR;
				ret  = AMS_CRM_LOGIN_PARA_ERR;
				packlen+=AmsPackCrmLoginBase(tellerIdLen,tellerId,iret,&s_Msg.cMessageBody[6+packlen],NULL);
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
				dbgprint("CrmLoginReqProc[%d] TellerId[%s] not registed",pid,tellerId);
				iret = AMS_CRM_LOGIN_TELLER_LOGIN_NOTREGSITER;
				ret = AMS_CRM_LOGIN_PARA_ERR;
				packlen+=AmsPackCrmLoginBase(tellerIdLen,tellerId,iret,&s_Msg.cMessageBody[6+packlen],NULL);
				break;
			}
		}
		
		/* check teller in process or not*/
		if(AMS_SERVICE_ACTIVE == AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpIdPos).serviceState)
		{
			pCrmNode = AmsSearchCrmNode(AmsCfgTeller(tellerCfgPos).srvGrpIdPos, AmsCfgTeller(tellerCfgPos).tellerId,AmsCfgTeller(tellerCfgPos).tellerIdLen);
			if(NULL != pCrmNode)
			{
				dbgprint("CrmLoginReqProc[%d] TellerNo[%s]Idhas been Logined[%d]", 
					pid, tellerId, tellerCfgPos);		
				iret = AMS_CRM_LOGIN_TELLER_LOGIN_REPEATEDLY;
				ret = AMS_CRM_LOGIN_PARA_ERR;
				packlen+=AmsPackCrmLoginBase(tellerIdLen,tellerId,iret,&s_Msg.cMessageBody[6+packlen],NULL);
				continue;
			}	
		}

		//分配服务进程号
		memset(&Pid,0,sizeof(PID_t));
		iret = AmsAllocPid(&Pid);
		if(-1 == iret)
		{
			dbgprint("CrmLoginReqProc[%d] Teller[%s] AmsAllocPid: SysBusy", pid, tellerId);
			iret = AMS_CRM_LOGIN_LP_RESOURCE_LIMITED;
			ret = AMS_CRM_LOGIN_PARA_ERR;
			packlen+=AmsPackCrmLoginBase(tellerIdLen,tellerId,iret,&s_Msg.cMessageBody[6+packlen],NULL);
			continue;
		}

	    //分配TELLER结点
		pCrmNode = CrmNodeGet();
		if(NULL == pCrmNode)
		{
	 		dbgprint("CrmLoginReqProc[%d] Teller[%s] CrmNodeGet Failed", pid, tellerId);
			iret = AMS_CRM_LOGIN_NODE_RESOURCE_LIMITED;
			ret = AMS_CRM_LOGIN_PARA_ERR;
			packlen+=AmsPackCrmLoginBase(tellerIdLen,tellerId,iret,&s_Msg.cMessageBody[6+packlen],NULL);
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
		lpAmsData->currStateOp = CRM_STATE_OPERATE_RSVD;

		//init timer
		lpAmsData->iTimerId             = -1;
		lpAmsData->callTimerId          = -1;
		lpAmsData->volumeCtrlTimerId    = -1;
		lpAmsData->rcasRemoteCoopTimerId= -1;
		lpAmsData->vtmRemoteCoopTimerId = -1;	
		lpAmsData->restTimerId          = -1;

		lpAmsData->multiSessTimerId     = -1;
		lpAmsData->monitorTimerId       = -1;
		lpAmsData->crmParaCfgTimerId    = -1;	

		//record myPid
		lpAmsData->myPid.cModuleId	 = SystemData.cMid;
		lpAmsData->myPid.cFunctionId = FID_AMS;
		lpAmsData->myPid.iProcessId  = Pid.iProcessId;

		//record rPid //CrmPid
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
		lpAmsData->cmsgTrace   = (unsigned char)AmsCMsgTrace;

		/* CRM结点数据初始化 */
	    //record amsPid
		pCrmNode->amsPid = lpAmsData->amsPid;

		//init Start time
		time(&pCrmNode->startTime);

		//stateStartTime & callStateStartTime later 
		
		//init Handshake time
		time(&pCrmNode->handshakeTime);
		
		//init WorkInfo Update time
		time(&pCrmNode->workInfoUpdateTime);

		//init call Transfer Freeze Time
		time(&pCrmNode->callTransferFreezeTime);
		
		//get crmInfo
		memset(&pCrmNode->crmInfo, 0, sizeof(TELLER_INFO));
		memcpy(&pCrmNode->crmInfo, &AmsCfgTeller(tellerCfgPos), sizeof(TELLER_INFO));

		//init work info
		memset(&pCrmNode->crmWorkInfo, 0, sizeof(TELLER_WORK_INFO)); 

		//init call call Transfer Num
		pCrmNode->callTransferNum = 0;
				
		//init Score and Expect Score
		//pCrmNode->dailyAvgScore = AMS_TELLER_SERVICE_QUALITY_THRESHOLD;
		//pCrmNode->dailyAvgScoreExpect = AMS_TELLER_SERVICE_QUALITY_THRESHOLD;

		//recored tellerCfgPos
		pCrmNode->tellerCfgPos = tellerCfgPos;

		//set Crm State and State Start Time
		AmsSetCrmState(iThreadId, lpAmsData, pCrmNode, AMS_CRM_STATE_IDLE, 0);
		
		//set Crm Call State and State Start Time 
		AmsSetCrmCallState(lpAmsData, pCrmNode, AMS_CALL_STATE_NULL);

		/* Add Crm Node to List */
	    Sem_wait(&AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpIdPos).crmCtrl);
		lstAdd(&AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpIdPos).crmList, (NODE *)pCrmNode);
		Sem_post(&AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpIdPos).crmCtrl);

		/* Add Crm Node to free List */
	    Sem_wait(&AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpIdPos).freecrmCtrl);
		lstAdd(&AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpIdPos).freecrmList, (NODE *)pCrmNode);
		Sem_post(&AmsSrvData(AmsCfgTeller(tellerCfgPos).srvGrpIdPos).freecrmCtrl);

		packlen+=AmsPackCrmLoginBase(tellerIdLen,tellerId,iret,&s_Msg.cMessageBody[6+packlen],lpAmsData);

		tellerNum+=1;
	}

	//pack 人工参数

	s_Msg.iMessageLength = packlen + 6;
	//send Login Rsp to Crm
	AmsSendCrmLoginRsp(pMsg,ret,&s_Msg,i);		
	
	return AMS_OK;

}



int CrmStateOperateReqProc(int iThreadId, MESSAGE_t *pMsg)
{
	int					iret = AMS_CRM_QUEUE_MNG_SUCCESS;
	LP_AMS_DATA_t		*lpAmsData = NULL;   //进程数据区指针
    CRM_NODE            *pCrmNode = NULL;		
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
		descrlen=snprintf(description,1024,"recv A_TELLER_STATE_OPERATE_REQ msg \n");	
		AmsTraceToFile(pMsg->s_ReceiverPid,pMsg->s_SenderPid,"A_TELLER_STATE_OPERATE_REQ",description,
						descrlen,pMsg->cMessageBody,pMsg->iMessageLength,"ams");		
	}	

	//进程号有效检查
	pid = pMsg->s_ReceiverPid.iProcessId;
	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		dbgprint("CrmStateOperateReqProc Pid:%d Err", pid);
		iret = AMS_CRM_STATE_OPERATE_PARA_ERR;
		AmsSendCrmStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}
	
	//消息长度检查


	lpAmsData = (LP_AMS_DATA_t *)ProcessData[pid];

	//amsPid检查
	p = pMsg->cMessageBody;
	BEGETLONG(amsPid,p);
	if(lpAmsData->amsPid != amsPid)
	{
		dbgprint("CrmStateOperateReqProc[%d] AmsPid[0x%x][0x%x] Err",pid,amsPid,lpAmsData->amsPid);
		iret = AMS_CRM_STATE_OPERATE_AMS_PID_ERR;
		AmsSendCrmStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}

	p += 4;

	//进程号匹配性检查
	if(lpAmsData->myPid.iProcessId != pid)
	{
		dbgprint("CrmStateOperateReqProc PID[%d][%d] Not Equal", lpAmsData->myPid.iProcessId, pid);
		iret = AMS_CRM_STATE_OPERATE_PARA_ERR;
		AmsSendCrmStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}

	//坐席检查
	tellerIdLen = *p++;
	if(tellerIdLen > AMS_MAX_TELLER_ID_LEN && tellerIdLen != lpAmsData->tellerIdLen)
	{
		dbgprint("CrmStateOperateReqProc[%d] TellerId[%u][%u]Err", 
			pid, lpAmsData->tellerId, tellerId);
		iret = AMS_CRM_STATE_OPERATE_TELLER_ID_ERR;
		AmsSendCrmStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;	
	}

	memcpy(tellerId,p,tellerIdLen);
	if(0 != (strcmp(tellerId,lpAmsData->tellerId)))
	{
		dbgprint("CrmStateOperateReqProc[%d] TellerId[%u][%u]Err", 
			pid, lpAmsData->tellerId, tellerId);
		iret = AMS_CRM_STATE_OPERATE_TELLER_ID_ERR;
		AmsSendCrmStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;			
	}
	p+=tellerIdLen;

	//操作码检查
	BEGETSHORT(stateOperate,p);
	if(stateOperate < CRM_STATE_OPERATE_IDLE || stateOperate >= CRM_STATE_OPERATE_MAX)
	{
		dbgprint("CrmStateOperateReqProc[%d] Teller[%s]StateOperateCode[%d]Err", 
			pid, tellerId, stateOperate);
		iret = AMS_CRM_STATE_OPERATE_CODE_ERR;
		AmsSendCrmStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;					
	}
	p+=2;

	lpAmsData->currStateOp = stateOperate;

	if( *p == 0x02)
	{
		if(stateOperate != CRM_STATE_OPERATE_BUSY && stateOperate != CRM_STATE_OPERATE_REST)
		{
			dbgprint("CrmStateOperateReqProc[%d] Teller[%s] Opart Len[%d]Err", 
				pid, tellerId, pMsg->iMessageLength);
			iret = AMS_CRM_STATE_OPERATE_LEN_ERR;
			AmsSendCrmStateOperateRsp(lpAmsData,pMsg,iret);
			return AMS_ERROR;				
		}
		//unpack OPart,e.g :stateOpInfo
		memset(&stateOpInfo,0,sizeof(STATE_OP_INFO));
		iret = AmsUnpackStateOperateReqOpartPara(p ,pMsg->iMessageLength-tellerIdLen-7,&stateOpInfo);
		if(AMS_OK != iret)
		{
			dbgprint("CrmStateOperateReqProc[%d] Teller[%s] UnpackOpartParaErr", pid, tellerId);
			if(AMS_ERROR == iret)
			{
				iret = AMS_CRM_STATE_OPERATE_PARA_ERR;
			}
			
			AmsSendCrmStateOperateRsp(lpAmsData,pMsg,iret);
			return AMS_ERROR;	
		}
		stateOpInfoSet = 1;
	}
	else
	{
		if(stateOperate == CRM_STATE_OPERATE_BUSY && stateOperate == CRM_STATE_OPERATE_REST)
		{
			dbgprint("CrmStateOperateReqProc[%d] Teller[%s] Opart Len[%d]Err", 
				pid, tellerId, pMsg->iMessageLength);
			iret = AMS_CRM_STATE_OPERATE_LEN_ERR;
			AmsSendCrmStateOperateRsp(lpAmsData,pMsg,iret);
			return AMS_ERROR;				
		}		
	}

	//业务组编号检查
	if(lpAmsData->srvGrpIdPos > AMS_MAX_SERVICE_GROUP_NUM)
	{
		dbgprint("CrmStateOperateReqProc[%d] Teller[%s] SrvGrpId[%u]Err", 
			pid, tellerId, lpAmsData->srvGrpId);
		iret = AMS_CRM_STATE_OPERATE_SERVICE_GROUP_ID_ERR;
		AmsSendCrmStateOperateRsp(lpAmsData,pMsg,iret);
		return AMS_ERROR;		
	}

	//业务状态检查
	if(AMS_SERVICE_ACTIVE != AmsSrvData(lpAmsData->srvGrpIdPos).serviceState)
	{
		dbgprint("CrmStateOperateReqProc[%d] Teller[%s] ServiceState[%u]Err", 
			pid, tellerId, AmsSrvData(lpAmsData->srvGrpIdPos).serviceState);
		iret = AMS_CRM_STATE_OPERATE_SERVICE_STATE_ERR;
		AmsSendCrmStateOperateRsp(lpAmsData,pMsg,iret);
		return AMS_ERROR;		
	}

	/*find crm Node*/
	pCrmNode = AmsSearchCrmNode(lpAmsData->srvGrpIdPos, lpAmsData->tellerId,lpAmsData->tellerIdLen);
	if(NULL == pCrmNode)
	{
		dbgprint("CrmStateOperateReqProc[%d] Teller[%s]Id[%u]Err", 
			pid, tellerId, lpAmsData->tellerId);		
		iret = AMS_CRM_STATE_OPERATE_TELLER_ID_ERR;
		AmsSendCrmStateOperateRsp(lpAmsData,pMsg,iret);
		return AMS_ERROR;		
	}

	//坐席状态检查
	if(pCrmNode->state >= AMS_CRM_STATE_OFFLINE)
	{
		dbgprint("CrmStateOperateReqProc[%d] Teller[%s][%u]State[%d]Err", 
			pid, tellerId, lpAmsData->tellerId, pCrmNode->state);
		iret = AMS_CRM_STATE_OPERATE_STATE_ERR;
		AmsSendCrmStateOperateRsp(lpAmsData,pMsg,iret);
		return AMS_ERROR;		
	}

	//update rpid
	lpAmsData->rPid.cModuleId   = pMsg->s_SenderPid.cModuleId;
	lpAmsData->rPid.cFunctionId = pMsg->s_SenderPid.cFunctionId;
	lpAmsData->rPid.iProcessId  = pMsg->s_SenderPid.iProcessId;

	//update crm state and state start time
	if(1 == stateOpInfoSet)
	{
		if(AMS_OK != AmsUpdateCrmState(iThreadId,lpAmsData,pCrmNode,stateOperate,stateOpInfo.reason))
		{
			dbgprint("CrmStateOperateReqProc[%d] Teller[%s][%u]UpdateCrmState[%d]Err!", 
				pid, tellerId, lpAmsData->tellerId, pCrmNode->state);
			iret = AMS_CRM_STATE_OPERATE_UPDATE_STATE_ERR;
			AmsSendCrmStateOperateRsp(lpAmsData,pMsg,iret);
			return AMS_ERROR;					
		}
	}
	else
	{
		if(AMS_OK != AmsUpdateCrmState(iThreadId, lpAmsData, pCrmNode, stateOperate, 0))
		{
			dbgprint("CrmStateOperateReqProc[%d] Teller[%s][%u]UpdateCrmState[%d]Err", 
				pid, tellerId, lpAmsData->tellerId, pCrmNode->state);
			iret = AMS_CRM_STATE_OPERATE_UPDATE_STATE_ERR;
			AmsSendCrmStateOperateRsp(lpAmsData,pMsg,iret);
			return AMS_ERROR;					
		}	
	}

	if(stateOperate != CRM_STATE_OPERATE_REST)
	{
		/* 杀掉定时器 */
		if(lpAmsData->restTimerId >= 0)
		{
			AmsKillTimer(lpAmsData->myPid.iProcessId, &lpAmsData->restTimerId);
			AmsTimerStatProc(T_AMS_REST_TIMER, AMS_KILL_TIMER); 
		}		
	}

	if(CRM_STATE_OPERATE_REST == stateOperate && 1 == stateOpInfoSet)
	{
		if(stateOpInfo.timeLen > AmsCfgData.crmRestTimeLength)
		{
			stateOpInfo.timeLen = AmsCfgData.crmRestTimeLength;
		}
		
		if(stateOpInfo.timeLen > 0)
		{
			//wait rest timeout ,default: 5s
			if(    AmsCfgData.crmRestTimeLength > 0 
				&& AmsCfgData.crmRestTimeLength <= T_AMS_REST_TIMER_LENGTH_MAX)
			{
				memset(timerPara, 0, PARA_LEN);
			    if(AMS_OK != AmsCreateTimerPara(lpAmsData->myPid.iProcessId,
						                        &lpAmsData->restTimerId, 
												B_AMS_TELLER_REST_TIMEOUT, 
												stateOpInfo.timeLen,
												timerPara))
			    {
					dbgprint("CrmStateOperateReqProc[%d] Teller[%s][%u] CreateTimer Err",
						pid, tellerId, lpAmsData->tellerId);
					iret = AMS_CRM_STATE_OPERATE_START_TIMER_ERR;
					AmsSendCrmStateOperateRsp(lpAmsData,pMsg,iret);
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
	AmsSendCrmStateOperateRsp(lpAmsData,pMsg,iret);

	return iret;
}

int CrmStateOperateCnfProc(int iThreadId, MESSAGE_t *pMsg)
{
	int					iret = AMS_CRM_QUEUE_MNG_SUCCESS;
	LP_AMS_DATA_t		*lpAmsData = NULL;          //进程数据区指针
	LP_AMS_DATA_t		*lpAmsManagerData = NULL;   //进程数据区指针	
	CRM_NODE            *pCrmNode = NULL;		
	CRM_NODE            *pManagerNode = NULL;			
	int                 pid = 0;
	int                 managerPid = 0;	
	unsigned int        amsPid = 0;
	unsigned char       tellerIdLen = 0;
	unsigned char       tellerId[AMS_MAX_TELLER_ID_LEN + 1] = { 0 };	
//	unsigned int        managerTellerId = 0;	
//	unsigned char       managerNo[AMS_MAX_TELLER_NO_LEN + 1] = { 0 };		
	unsigned int      stateOpInd = 0;
	unsigned int        stateSet = CRM_STATE_OPERATE_IND_RSVD;
	time_t              currentTime;	
	unsigned int        i = 0;
	unsigned char       *p;
	unsigned short      stateOperate = 0;

	if(AmsMsgTrace)
	{
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"recv A_TELLER_STATE_OPERATE_CNF msg \n");	
		AmsTraceToFile(pMsg->s_ReceiverPid,pMsg->s_SenderPid,"A_TELLER_STATE_OPERATE_CNF",description,
						descrlen,pMsg->cMessageBody,pMsg->iMessageLength,"ams");		
	}

	//进程号有效性检查
	pid = pMsg->s_ReceiverPid.iProcessId;
	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		dbgprint("CrmStateOperateCnfProc Pid:%d Err", pid);
		iret = AMS_CRM_STATE_OPERATE_IND_CNF_PARA_ERR;
		//AmsResultStatProc(AMS_MANAGER_SET_CRM_STATE_RESULT, iret);
		return AMS_ERROR;
	}

	lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
	
	//amsPid检查
	p = pMsg->cMessageBody;
	BEGETLONG(amsPid,p);
	if(lpAmsData->amsPid != amsPid)
	{
		dbgprint("CrmStateOperateCnfProc[%d] AmsPid[0x%x][0x%x] Err",pid,amsPid,lpAmsData->amsPid);
		iret = AMS_CRM_STATE_OPERATE_IND_CNF_AMS_PID_ERR;
		AmsSendCrmStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}

	p += 4;

	//进程号匹配性检查
	if(lpAmsData->myPid.iProcessId != pid)
	{
		dbgprint("CrmStateOperateCnfProc PID[%d][%d] Not Equal", lpAmsData->myPid.iProcessId, pid);
		iret = AMS_CRM_STATE_OPERATE_PARA_ERR;
		AmsSendCrmStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}

	//坐席检查
	tellerIdLen = *p++;
	if(tellerIdLen > AMS_MAX_TELLER_ID_LEN && tellerIdLen != lpAmsData->tellerIdLen)
	{
		dbgprint("CrmStateOperateCnfProc[%d] TellerId[%u][%u]Err", 
			pid, lpAmsData->tellerId, tellerId);
		iret = AMS_CRM_STATE_OPERATE_IND_CNF_TELLER_ID_ERR;
		AmsSendCrmStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;	
	}

	memcpy(tellerId,p,tellerIdLen);
	if(0 != (strcmp(tellerId,lpAmsData->tellerId)))
	{
		dbgprint("CrmStateOperateCnfProc[%d] TellerId[%u][%u]Err", 
			pid, lpAmsData->tellerId, tellerId);
		iret = AMS_CRM_STATE_OPERATE_IND_CNF_TELLER_ID_ERR;
		AmsSendCrmStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;			
	}
	p+=tellerIdLen;

	//操作码检查
	BEGETSHORT(stateOperate,p);
	if(stateOperate < CRM_STATE_OPERATE_IDLE || stateOperate >= CRM_STATE_OPERATE_MAX)
	{
		dbgprint("CrmStateOperateReqProc[%d] Teller[%s]StateOperateCode[%d]Err", 
			pid, tellerId, stateOperate);
		iret = AMS_CRM_STATE_OPERATE_IND_CNF_CODE_ERR;
		AmsSendCrmStateOperateRsp(NULL,pMsg,iret);
		return AMS_ERROR;					
	}
	p+=2;

	if(lpAmsData->iTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->iTimerId);
		AmsTimerStatProc(T_AMS_CRM_STATE_OPERATE_IND_TIMER, AMS_KILL_TIMER);
	}
	
	//iret
	BEGETLONG(iret, p);//原因值统一编码待定
	
	if(AMS_CRM_QUEUE_MNG_SUCCESS == iret)
	{
		if(stateOperate == CRM_STATE_OPERATE_IDLE)
		{
			//AmsKillCrmAllTimer(lpAmsData, pid);
	
			//仅杀掉呼叫相关定时器，包括消息、文件收发
			AmsKillCrmAllCallTimer(lpAmsData, pid);
			
			//update time
			memset(&pCrmNode->callStateStartLocalTime, 0, sizeof(TIME_INFO)); 
			memset(&pCrmNode->callStateStartTime, 0, sizeof(time_t)); 
	
			//reset callTransferNum
			//			pCrmNode->callTransferNum = 0;
	
#ifdef AMS_TEST_LT
			//calc crm workInfo
			time(&currentTime);  
			AmsUpdateSingleCrmWorkInfo(pCrmNode, currentTime);
	
			//set Crm State and State Start Time
			AmsSetCrmState(iThreadId, lpAmsData, pCrmNode, AMS_CRM_STATE_IDLE, 0);
#endif		

			//add crmnode to freecrmnode
			Sem_wait(&AmsSrvData(lpAmsData->srvGrpIdPos).freecrmCtrl);
			lstAdd(&AmsSrvData(lpAmsData->srvGrpIdPos).freecrmList, (NODE *)pCrmNode);
			Sem_post(&AmsSrvData(lpAmsData->srvGrpIdPos).freecrmCtrl);

		}
	}

	return AMS_OK;
}

int CrmLoginOutProc(int iThreadId, MESSAGE_t *pMsg)
{
	int 				iret = AMS_CRM_QUEUE_MNG_SUCCESS;
	LP_AMS_DATA_t		*lpAmsData = NULL;			//进程数据区指针
	CRM_NODE			*pCrmNode = NULL;	
	int 				pid = 0;	
	unsigned int		amsPid = 0; 
	unsigned char		tellerIdLen = 0;
	unsigned char		tellerId[AMS_MAX_TELLER_ID_LEN+1]={0};
	time_t				currentTime;
	unsigned char		*p;
	
	if(AmsMsgTrace)
	{
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"recv A_TELLER_LOGOUT_REQ msg \n"); 
		AmsTraceToFile(pMsg->s_ReceiverPid,pMsg->s_SenderPid,"A_TELLER_LOGOUT_REQ",description,
						descrlen,pMsg->cMessageBody,pMsg->iMessageLength,"ams");		
	}
	
	//进程号有效性检查
	pid = pMsg->s_ReceiverPid.iProcessId;
	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		dbgprint("CrmLogoutReqProc Pid:%d Err", pid);
		iret = AMS_CRM_LOGOUT_PARA_ERR;
		AmsSendCrmLogoutRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}

	lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];

	/* 杀掉定时器 */
	AmsKillCrmAllTimer(lpAmsData, pid); 
	//amsPid检查
	p = pMsg->cMessageBody;
	BEGETLONG(amsPid,p);
	if(lpAmsData->amsPid != amsPid)
	{
		dbgprint("CrmLogoutReqProc[%d] AmsPid[0x%x][0x%x] Err",pid,amsPid,lpAmsData->amsPid);
		iret = AMS_CRM_LOGOUT_AMS_PID_ERR;
		AmsSendCrmLogoutRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}

	p += 4;

	//进程号匹配性检查
	if(lpAmsData->myPid.iProcessId != pid)
	{
		dbgprint("CrmLogoutReqProc PID[%d][%d] Not Equal", lpAmsData->myPid.iProcessId, pid);
		iret = AMS_CRM_LOGOUT_PARA_ERR;
		AmsSendCrmLogoutRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}

	//坐席检查
	tellerIdLen = *p++;
	if(tellerIdLen > AMS_MAX_TELLER_ID_LEN && tellerIdLen != lpAmsData->tellerIdLen)
	{
		dbgprint("CrmLogoutReqProc[%d] TellerId[%s][%s]Err", 
			pid, lpAmsData->tellerId, tellerId);
		iret = AMS_CRM_LOGOUT_TELLER_ID_ERR;
		AmsSendCrmLogoutRsp(NULL,pMsg,iret);
		return AMS_ERROR;	
	}

	memcpy(tellerId,p,tellerIdLen);
	if(0 != (strcmp(tellerId,lpAmsData->tellerId)))
	{
		dbgprint("CrmLogoutReqProc[%d] TellerId[%s][%s]Err", 
			pid, lpAmsData->tellerId, tellerId);
		iret = AMS_CRM_LOGOUT_TELLER_ID_ERR;
		AmsSendCrmLogoutRsp(NULL,pMsg,iret);
		return AMS_ERROR;			
	}
	p+=tellerIdLen;

	
	//业务组编号检查
	if(lpAmsData->srvGrpIdPos > AMS_MAX_SERVICE_GROUP_NUM)
	{
		dbgprint("CrmLogoutReqProc[%d] Teller[%s] SrvGrpId[%u]Err", 
			pid, tellerId, lpAmsData->srvGrpIdPos);
		iret = AMS_CRM_LOGOUT_SERVICE_GROUP_ID_ERR;
		AmsSendCrmLogoutRsp(lpAmsData,pMsg,iret);
		return AMS_ERROR;		
	}

	//业务状态检查
	if(AMS_SERVICE_ACTIVE != AmsSrvData(lpAmsData->srvGrpIdPos).serviceState)
	{
		dbgprint("CrmLogoutReqProc[%d] Teller[%s] ServiceState[%u]Err", 
			pid, tellerId, AmsSrvData(lpAmsData->srvGrpIdPos).serviceState);
		iret = AMS_CRM_LOGOUT_SERVICE_STATE_ERR;
		AmsSendCrmLogoutRsp(lpAmsData,pMsg,iret);
		return AMS_ERROR;		
	}

	/*find crm Node*/
	pCrmNode = AmsSearchCrmNode(lpAmsData->srvGrpIdPos, lpAmsData->tellerId,lpAmsData->tellerIdLen);
	if(NULL == pCrmNode)
	{
		dbgprint("CrmLogoutReqProc[%d] Teller[%s]Id[%u]Err", 
			pid, tellerId, lpAmsData->tellerId);		
		iret = AMS_CRM_LOGOUT_TELLER_ID_ERR;
		AmsSendCrmLogoutRsp(lpAmsData,pMsg,iret);
		return AMS_ERROR;		
	}

	//坐席状态检查
	if(pCrmNode->state >= AMS_CRM_STATE_OFFLINE)
	{
		dbgprint("CrmLogoutReqProc[%d] Teller[%s][%u]State[%d]Err", 
			pid, tellerId, lpAmsData->tellerId, pCrmNode->state);
		iret = AMS_CRM_LOGOUT_STATE_ERR;
		AmsSendCrmLogoutRsp(lpAmsData,pMsg,iret);
		return AMS_ERROR;		
	}

	//notify cms to release call
	if(    pCrmNode->callState > AMS_CALL_STATE_NULL 
		&& pCrmNode->callState < AMS_CALL_STATE_RSVD)
	{
		AmsSendCmsCallEventInd(lpAmsData,AMS_CALL_EVENT_IND_TELLER_LOGOUT);
	}
	
    //calc crm workInfo
	time(&currentTime);	   
	AmsUpdateSingleCrmWorkInfo(pCrmNode, currentTime);
	
	//set Crm State and State Start Time
	AmsSetCrmState(iThreadId, lpAmsData, pCrmNode, AMS_CRM_STATE_OFFLINE, 0);

	if(AMS_CALL_STATE_NULL != pCrmNode->callState)
	{
		//set Crm Call State and State Start Time 
		AmsSetCrmCallState(lpAmsData, pCrmNode, AMS_CALL_STATE_NULL);
	}

	//crm登出后，cms vtm 挂机通知失效，提前设置vtm状态
	AmsResetTermState(iThreadId, lpAmsData->srvGrpIdPos, lpAmsData->termId, lpAmsData->termIdLen);
	
    //update time
    memset(&pCrmNode->stateStartLocalTime, 0, sizeof(TIME_INFO));
    memset(&pCrmNode->callStateStartLocalTime, 0, sizeof(TIME_INFO)); 
    memset(&pCrmNode->stateStartTime, 0, sizeof(time_t)); 
    memset(&pCrmNode->callStateStartTime, 0, sizeof(time_t));
    memset(&pCrmNode->startTime, 0, sizeof(time_t)); 		

	//Reset SIU preSrvTellerId
/*	if(pCrmNode->crmInfo.tellerId == AmsSrvData(lpAmsData->srvGrpId).preSrvTellerId)
	{
		AmsSrvData(lpAmsData->srvGrpId).preSrvTellerId = 0;
	}
		*/
    /* Del Crm Node from List */
    Sem_wait(&AmsSrvData(lpAmsData->srvGrpIdPos).crmCtrl);
	lstDelete(&AmsSrvData(lpAmsData->srvGrpIdPos).crmList, (NODE *)pCrmNode);
	Sem_post(&AmsSrvData(lpAmsData->srvGrpIdPos).crmCtrl);

	
    Sem_wait(&AmsSrvData(lpAmsData->srvGrpIdPos).freecrmCtrl);
	lstDelete(&AmsSrvData(lpAmsData->srvGrpIdPos).freecrmList, (NODE *)pCrmNode);
	Sem_post(&AmsSrvData(lpAmsData->srvGrpIdPos).freecrmCtrl);

	//update rPid
	lpAmsData->rPid.cModuleId	= pMsg->s_SenderPid.cModuleId;
	lpAmsData->rPid.cFunctionId = pMsg->s_SenderPid.cFunctionId;
	lpAmsData->rPid.iProcessId  = pMsg->s_SenderPid.iProcessId;

    AmsSendCrmLogoutRsp(lpAmsData,pMsg,iret);

	//release lpAmsData Pid
	AmsReleassPid(lpAmsData->myPid, END);

	CrmNodeFree(pCrmNode);

	return iret;	
}

int CrmModifyPswdReq(int iThreadId, MESSAGE_t *pMsg)
{
	return AMS_OK;
}
int CrmForceLoginReq(int iThreadId, MESSAGE_t *pMsg)
{
	return AMS_OK;
}

/*
3	CallId	STRING
4	TermType	DWORD	终端类型	参见3.2.4注B
5	TermId	STRING	终端标识	参见3.2.4注C
6	SrvGrpId	STRING	
7	ServiceType	STRING	
8	TellerState	WORD	
9	CurStateTimeLen	DWORD	
10	TellerCallState	WORD	
11	CurCallStateTimeLen	DWORD	
*/
int AmsCrmQueryCurTellerWorkInfoProc(LP_AMS_DATA_t *lpAmsData, CRM_NODE *pCurrCrmNode, MESSAGE_t *pMsg)
{
	int					iret = AMS_OK;	
	unsigned short      queryType = AMS_CRM_QUERY_CUR_TELLER_WORK_INFO;	
	unsigned char       queryBuf[MSG_BODY_LEN] = {0};	
	unsigned short      queryBufLen = 0;
	unsigned short      seq = 0;	
	unsigned short      tellerNum = 0;	
	time_t              currentTime;		
	int                 i = 0;
	int                 j = 0;
	unsigned char       *p;	

	if(NULL == lpAmsData || NULL == pCurrCrmNode || NULL == pMsg)
	{
		iret = AMS_ERROR;
		return iret;			
	}

	//calc crm workInfo
	time(&currentTime);	  
	AmsUpdateSingleCrmWorkInfo(pCurrCrmNode,currentTime);
	
	p = queryBuf;

	*p++=lpAmsData->callIdLen;
	memcpy(p,lpAmsData->callId,lpAmsData->callIdLen);
	p += lpAmsData->callIdLen;

	BEPUTLONG(lpAmsData->termType,p);
	p += 4;

	*p++ = lpAmsData->srvGrpIdLen;
	memcpy(p,lpAmsData->srvGrpId,lpAmsData->srvGrpIdLen);
	p += lpAmsData->srvGrpIdLen;

	*p++ = lpAmsData->srvTypeLen;
	memcpy(p,lpAmsData->srvType,lpAmsData->srvTypeLen);
	p += lpAmsData->srvTypeLen;

	BEPUTSHORT(pCurrCrmNode->crmInfo.idState,p);
	p += 2;

	BEPUTLONG(pCurrCrmNode->startTime,p);
	p += 4;

	BEPUTSHORT(pCurrCrmNode->callState,p);
	p += 2;

	BEPUTLONG(pCurrCrmNode->callStateStartTime,p);
	p += 4;

	queryBufLen = p - queryBuf;
	
	AmsSendCrmQueryInfoRsp(lpAmsData,pMsg,queryType,iret,queryBuf,queryBufLen);	
	
	return iret;	
}


int CrmQueryInfoReq(int iThreadId, MESSAGE_t *pMsg)
{
	int					iret = AMS_CRM_QUEUE_MNG_SUCCESS;
	LP_AMS_DATA_t		*lpAmsData = NULL;         //进程数据区指针	
	CRM_NODE            *pCrmNode = NULL;	
	int                 pid = 0;
	unsigned int        amsPid = 0;
	unsigned char       tellerIdLen = 0;
	unsigned char       tellerId[AMS_MAX_TELLER_ID_LEN + 1] = { 0 };			
	unsigned short      queryType = 0;
	unsigned int        serviceId = 0;
	unsigned int        srvGrpId = 0;                
	char                tellerNameKey[AMS_MAX_NAME_LEN + 1];	
	unsigned char       *p;
	
	if(AmsMsgTrace)
	{
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"recv A_TELLER_QUERY_INFO_REQ msg \n");	
		AmsTraceToFile(pMsg->s_ReceiverPid,pMsg->s_SenderPid,"A_TELLER_QUERY_INFO_REQ",description,
						descrlen,pMsg->cMessageBody,pMsg->iMessageLength,"ams");		
	}

	//进程号有效性检查
	pid = pMsg->s_ReceiverPid.iProcessId;
	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		dbgprint("CrmQueryInfoReq Pid:%d Err", pid);
		iret = AMS_CRM_QUERY_INFO_PARA_ERR;
		AmsSendCrmQueryInfoRsp(NULL,pMsg,queryType,iret,NULL,0);
		return AMS_ERROR;
	}

	lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
	
	//amsPid检查
	p = pMsg->cMessageBody;
	BEGETLONG(amsPid,p);
	if(lpAmsData->amsPid != amsPid)
	{
		dbgprint("CrmQueryInfoReq[%d] AmsPid[0x%x][0x%x] Err",pid,amsPid,lpAmsData->amsPid);
		iret = AMS_CRM_QUERY_INFO_AMS_PID_ERR;
		AmsSendCrmQueryInfoRsp(NULL,pMsg,queryType,iret,NULL,0);
		return AMS_ERROR;
	}

	p += 4;

	//进程号匹配性检查
	if(lpAmsData->myPid.iProcessId != pid)
	{
		dbgprint("CrmQueryInfoReq PID[%d][%d] Not Equal", lpAmsData->myPid.iProcessId, pid);
		iret = AMS_CRM_QUERY_INFO_PARA_ERR;
		AmsSendCrmQueryInfoRsp(NULL,pMsg,queryType,iret,NULL,0);
		return AMS_ERROR;
	}

	//坐席检查
	tellerIdLen = *p++;
	if(tellerIdLen > AMS_MAX_TELLER_ID_LEN && tellerIdLen != lpAmsData->tellerIdLen)
	{
		dbgprint("CrmQueryInfoReq[%d] TellerId[%s][%s]Err", 
			pid, lpAmsData->tellerId, tellerId);
		iret = AMS_CRM_QUERY_INFO_TELLER_ID_ERR;
		AmsSendCrmQueryInfoRsp(NULL,pMsg,queryType,iret,NULL,0);
		return AMS_ERROR;	
	}

	memcpy(tellerId,p,tellerIdLen);
	if(0 != (strcmp(tellerId,lpAmsData->tellerId)))
	{
		dbgprint("CrmQueryInfoReq[%d] TellerId[%s][%s]Err", 
			pid, lpAmsData->tellerId, tellerId);
		iret = AMS_CRM_QUERY_INFO_TELLER_ID_ERR;
		AmsSendCrmQueryInfoRsp(NULL,pMsg,queryType,iret,NULL,0);
		return AMS_ERROR;			
	}
	p+=tellerIdLen;	

	BEGETSHORT(queryType, p);
	if(queryType < AMS_CRM_QUERY_ALL_TELLER_STATE_INFO || queryType >= AMS_CRM_QUERY_TYPE_MAX)
	{
		dbgprint("CrmQueryInfoReq[%d] Teller[%s] QueryType[%d]Err", 
			pid,tellerId, queryType);
		iret = AMS_CRM_QUERY_INFO_CODE_ERR;
		AmsSendCrmQueryInfoRsp(lpAmsData,pMsg,queryType,iret,NULL,0);
		return AMS_ERROR;		
	}
	
	//业务组编号检查
	if(lpAmsData->srvGrpIdPos > AMS_MAX_SERVICE_GROUP_NUM)
	{
		dbgprint("CrmQueryInfoReq[%d] Teller[%s] SrvGrpId[%u]Err", 
			pid, tellerId, lpAmsData->srvGrpIdPos);
		iret = AMS_CRM_QUERY_INFO_SERVICE_GROUP_ID_ERR;
		AmsSendCrmQueryInfoRsp(lpAmsData,pMsg,queryType,iret,NULL,0);	
		return AMS_ERROR;		
	}

	//业务状态检查
	if(AMS_SERVICE_ACTIVE != AmsSrvData(lpAmsData->srvGrpIdPos).serviceState)
	{
		dbgprint("CrmQueryInfoReq[%d] Teller[%s] ServiceState[%u]Err", 
			pid, tellerId, AmsSrvData(lpAmsData->srvGrpIdPos).serviceState);
		iret = AMS_CRM_QUERY_INFO_SERVICE_STATE_ERR;
		AmsSendCrmQueryInfoRsp(lpAmsData,pMsg,queryType,iret,NULL,0);	
		return AMS_ERROR;		
	}

	/*find crm Node*/
	pCrmNode = AmsSearchCrmNode(lpAmsData->srvGrpIdPos, lpAmsData->tellerId,lpAmsData->tellerIdLen);
	if(NULL == pCrmNode)
	{
		dbgprint("CrmQueryInfoReq[%d] Teller[%s]Id[%u]Err", 
			pid, tellerId, lpAmsData->tellerId);		
		iret = AMS_CRM_QUERY_INFO_TELLER_ID_ERR;
		AmsSendCrmQueryInfoRsp(lpAmsData,pMsg,queryType,iret,NULL,0);	
		return AMS_ERROR;		
	}

	//坐席状态检查
	if(pCrmNode->state >= AMS_CRM_STATE_OFFLINE)
	{
		dbgprint("CrmQueryInfoReq[%d] Teller[%s][%u]State[%d]Err", 
			pid, tellerId, lpAmsData->tellerId, pCrmNode->state);
		iret = AMS_CRM_QUERY_INFO_STATE_ERR;
		AmsSendCrmQueryInfoRsp(lpAmsData,pMsg,queryType,iret,NULL,0);	
		return AMS_ERROR;		
	}

	//update rPid
	lpAmsData->rPid.cModuleId	= pMsg->s_SenderPid.cModuleId;
	lpAmsData->rPid.cFunctionId = pMsg->s_SenderPid.cFunctionId;
	lpAmsData->rPid.iProcessId  = pMsg->s_SenderPid.iProcessId;
	
	switch(queryType)
	{
	case AMS_CRM_QUERY_CUR_TELLER_WORK_INFO: 
		AmsCrmQueryCurTellerWorkInfoProc(lpAmsData,pCrmNode,pMsg);
		break;
		
	default:
		break;
	}

	return AMS_OK;
}


int CrmEventNoticeTeller(unsigned char *p,unsigned int crmEventNotice)
{
	int					iret = AMS_CRM_QUEUE_MNG_SUCCESS;
	LP_AMS_DATA_t		*lpAmsData = NULL;   //进程数据区指针
	CRM_NODE            *pCrmNode = NULL;				
	int                 pid = 0;
	unsigned int        amsPid = 0;
	unsigned char       tellerIdLen = 0;
	unsigned char       tellerId[AMS_MAX_TELLER_ID_LEN + 1] = { 0 };		
	unsigned char       callIdLen = 0; 
	unsigned char       callId[AMS_MAX_CALLID_LEN + 1] = { 0 };			
	int					unpackRet = AMS_ERROR;	
	int                 ret = 0;	
	int                 len = 0;
	
	if(NULL == p)
	{
		dbgprint("CrmEventNoticeTeller Err");
		return AMS_ERROR;		
	}

	BEGETLONG(amsPid,p);
	pid = amsPid & 0xffff;
	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		dbgprint("CrmEventNoticeTeller Err Pid[%d]",pid);
		return AMS_ERROR;
	}
	lpAmsData = (LP_AMS_DATA_t*)ProcessData[pid];
	if(lpAmsData->amsPid != amsPid)
	{
		dbgprint("CrmEventNoticeTeller Err AmsPid[%d]",amsPid);
		return AMS_ERROR;
	}
	p += 4;
	

	tellerIdLen = *p++;
	if(tellerIdLen > AMS_MAX_TELLER_ID_LEN)
	{
		dbgprint("CrmEventNoticeTeller Err TellIdLen [%d]",tellerIdLen);
		return AMS_ERROR;
	}
	memcpy(tellerId,p,tellerIdLen);
	p += tellerIdLen;

	callIdLen = *p++;
	if(callIdLen > AMS_MAX_CALLID_LEN)
	{
		dbgprint("CrmEventNoticeTeller Err CallIdLen [%d]",callIdLen);
		return AMS_ERROR;
	}
	memcpy(callId,p,callIdLen);
	
	BEGETLONG(ret,p);
	p += 4;

	len = 4+1+tellerIdLen+1+callIdLen+4;

	AmsSendCmsCallEventInd(lpAmsData,AMS_CALL_EVENT_IND_TELLER_ERR);

	AmsClearHeartbeatErrCrm(lpAmsData,tellerId,tellerIdLen);

	return len;
}

int CrmEventNoticeMsg(int iThreadId, MESSAGE_t *pMsg)
{
	int					iret = AMS_CRM_QUEUE_MNG_SUCCESS;
	LP_AMS_DATA_t		*lpAmsData = NULL;   //进程数据区指针
	CRM_NODE            *pCrmNode = NULL;				
	int                 pid = 0;
	unsigned int        amsPid = 0;
	unsigned int        crmEventNotice = 0;
	unsigned char       callIdLen = 0; 
	unsigned char       callId[AMS_MAX_CALLID_LEN + 1] = { 0 };	
	unsigned short      targetType = 0;
	unsigned int        targetId = 0;		
	unsigned short      originType = 0;
	unsigned int        originId = 0;			
	FILE_INFO           fileInfo;	
	int					unpackRet = AMS_ERROR;	
	int                 i = 0;	
	unsigned char       *p;
	unsigned short      tellernum=0;
	
	if(AmsMsgTrace)
	{
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"recv A_TELLER_EVENT_NOTICE msg \n");	
		AmsTraceToFile(pMsg->s_ReceiverPid,pMsg->s_SenderPid,"A_TELLER_EVENT_NOTICE",description,
						descrlen,pMsg->cMessageBody,pMsg->iMessageLength,"ams");		
	}
	
	//先提取crmEventNotice
	p = pMsg->cMessageBody;
	BEGETLONG(crmEventNotice, p);
	p += 4;

	BEGETSHORT(tellernum,p);
	p += 2;

	for(i=0;i<tellernum;i++)
	{
		iret = CrmEventNoticeTeller(p,crmEventNotice);
	}	

	return AMS_OK;
}
	

int AmsSendCrmLoginRsp(MESSAGE_t *pMsg,int iret,MESSAGE_t *s_Msg,int num)
{
	unsigned char       *p;
	int	crmnum=0;

	if(NULL == pMsg || NULL == s_Msg)
	{
		return AMS_ERROR;
	}
	
	s_Msg->eMessageAreaId = A;
	memcpy(&s_Msg->s_ReceiverPid,&pMsg->s_SenderPid,sizeof(PID_t));
	s_Msg->s_SenderPid.cModuleId = SystemData.cMid;
	s_Msg->s_SenderPid.cFunctionId = FID_AMS;
	s_Msg->s_SenderPid.iProcessId = pMsg->s_ReceiverPid.iProcessId;
	s_Msg->iMessageType = A_TELLER_LOGIN_RSP;
	
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
		descrlen=snprintf(description,1024,"send A_TELLER_LOGIN_RSP msg \n");	
		{
			AmsTraceToFile(s_Msg->s_ReceiverPid,s_Msg->s_SenderPid,"A_TELLER_LOGIN_RSP",description,
				descrlen,s_Msg->cMessageBody,s_Msg->iMessageLength,"ams");	
		}
	}
	AmsMsgStatProc(AMS_CRM_MSG, s_Msg->iMessageType); 
	AmsResultStatProc(AMS_CRM_LOGIN_RESULT, iret);
	return AMS_OK;
}

int AmsSendCrmStateOperateRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret)
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
	s_Msg.iMessageType = A_TELLER_STATE_OPERATE_RSP;
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
		case CRM_STATE_OPERATE_IDLE: 
			AmsTellerStatProc(lpAmsData->tellerId, lpAmsData->tellerPos, AMS_TELLER_SET_IDLE, iret);
			break;
			
		case CRM_STATE_OPERATE_BUSY: 
			AmsTellerStatProc(lpAmsData->tellerId, lpAmsData->tellerPos, AMS_TELLER_SET_BUSY, iret);
			break;

		case CRM_STATE_OPERATE_REST: 
			AmsTellerStatProc(lpAmsData->tellerId, lpAmsData->tellerPos, AMS_TELLER_SET_REST, iret);
			break;

		case CRM_STATE_OPERATE_PREPARE: 
			AmsTellerStatProc(lpAmsData->tellerId, lpAmsData->tellerPos, AMS_TELLER_SET_PREPARA, iret);
			break;
			
		default:
			AmsTellerStatProc(lpAmsData->tellerId, lpAmsData->tellerPos, AMS_TELLER_STAT_TYPE_MAX, iret);
			return AMS_ERROR;
		}*/
	}

	AmsMsgStatProc(AMS_CRM_MSG, s_Msg.iMessageType);
    AmsResultStatProc(AMS_CRM_STATE_OPERATE_RESULT, iret);

	if(AmsMsgTrace)
	{	
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"send A_TELLER_STATE_OPERATE_RSP msg \n");	
		{
			AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_TELLER_STATE_OPERATE_RSP",description,
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
	s_Msg.iMessageType = A_TELLER_EVENT_IND;
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
		descrlen=snprintf(description,1024,"send A_TELLER_EVENT_IND msg \n");	
		{
			AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_TELLER_EVENT_IND",description,
				descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,"ams");	
		}
	}
	AmsMsgStatProc(AMS_CRM_MSG, s_Msg.iMessageType); 

	return AMS_OK;
}



int AmsSendCrmStateOperateInd(LP_AMS_DATA_t *lpAmsData, MESSAGE_t *pMsg, unsigned short stateOpInd,unsigned short statesubOpInde)
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
	s_Msg.iMessageType = A_TELLER_STATE_OPERATE_IND;
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

	if(stateOpInd == CRM_STATE_OPERATE_IND_IDLE || stateOpInd == CRM_STATE_OPERATE_IND_BUSY)
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

	 if(    AmsCfgData.crmStateOperateIndTimeLength > 0 
	    && AmsCfgData.crmStateOperateIndTimeLength <= T_CRM_OPERATE_IND_TIMER_LENGTH_MAX)
    {
    	memset(timerPara, 0, PARA_LEN);
		p = timerPara;
		
		BEPUTLONG(lpAmsData->amsPid, p);

        if(AMS_OK != AmsCreateTimerPara(lpAmsData->myPid.iProcessId,
			                        &lpAmsData->iTimerId, 
									B_AMS_TELLER_STATE_OP_IND_TIMEOUT, 
									AmsCfgData.crmStateOperateIndTimeLength,
									timerPara))
        {
			dbgprint("CrmStateOperateReqProc Teller[%s]CreateTimer Err",
						lpAmsData->tellerId);
        }

		AmsTimerStatProc(T_AMS_CRM_STATE_OPERATE_IND_TIMER, AMS_CREATE_TIMER);

        if(lpAmsData->commonTrace)
        {
            dbgprint("Ams Create T_AMS_CRM_STATE_OPERATE_IND_TIMER Timer:timerId=%d.",
				 lpAmsData->iTimerId);
        }
    }
	

	if(AmsMsgTrace)
	{	
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"send A_TELLER_STATE_OPERATE_IND msg \n");	
		{
			AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_TELLER_STATE_OPERATE_IND",description,
				descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,"ams");	
		}
	}
	AmsMsgStatProc(AMS_CRM_MSG, s_Msg.iMessageType); 

	return SUCCESS;
}


int AmsSendCrmLogoutRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret)
{
	MESSAGE_t           s_Msg;
	unsigned char       *p;
	unsigned char       telleridlen;

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
	s_Msg.iMessageType = A_TELLER_LOGOUT_RSP;
	s_Msg.iMessageLength = 0;

    p = &s_Msg.cMessageBody[0];
	if(lpAmsData != NULL)
	{
		BEPUTLONG(lpAmsData->amsPid, p);
		p += 4;

		*p++ = lpAmsData->tellerIdLen;
		memcpy(p,lpAmsData->tellerId,lpAmsData->tellerIdLen);
		p += lpAmsData->tellerIdLen;
	}
	else
	{
		memcpy(p,pMsg->cMessageBody,4);
		telleridlen = pMsg->cMessageBody[4];
		memcpy(p+4,&pMsg->cMessageBody[4],telleridlen+1);
		p += 5+telleridlen;
	}

	BEPUTLONG(iret,p);
	p += 1;

	s_Msg.iMessageLength = p - &s_Msg.cMessageBody[0];
	
	SendMsgBuff(&s_Msg,0);


	if(AmsMsgTrace)
	{	
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"send A_TELLER_LOGOUT_RSP msg \n"); 
		if(NULL != lpAmsData)
		{
			AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_TELLER_LOGOUT_RSP",description,
							descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,lpAmsData->sTraceName);				
		}
		else
		{
			AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_TELLER_LOGOUT_RSP",description,
				descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,"ams");	
		}
	}

	if(lpAmsData != NULL)
	{
		//AmsTellerStatProc(lpAmsData->tellerId, lpAmsData->tellerPos, AMS_TELLER_LOGOUT, iret);
	}	

	AmsMsgStatProc(AMS_CRM_MSG, s_Msg.iMessageType); 
    AmsResultStatProc(AMS_CRM_LOGOUT_RESULT, iret);

	return SUCCESS;
}

int AmsGetQueryResultIdByQueryType(unsigned short  queryType, unsigned char *pQueryResultId)
{
	int					iret = AMS_OK;
	
    switch(queryType)
	{
	case AMS_CRM_QUERY_CUR_TELLER_WORK_INFO:
    	*pQueryResultId = AMS_TELLER_WORK_SHEET_ID;			
		break;	
		
	default:
		dbgprint("Ams GetQueryResultIdByQueryType queryType[%d]Err", queryType);
		iret = AMS_ERROR;
	}

	return iret;	
}


int AmsSendCrmQueryInfoRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,unsigned short queryType,int iret,unsigned char *queryBuf, unsigned short queryBufLen)
{
	MESSAGE_t           s_Msg;
	unsigned char       *p;
	unsigned char       queryResultId = 0;
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
	s_Msg.iMessageType = A_TELLER_QUERY_INFO_RSP;
	s_Msg.iMessageLength = 0;

    p = &s_Msg.cMessageBody[0];
	if(lpAmsData != NULL)
	{
		BEPUTLONG(lpAmsData->amsPid, p);
		p += 4;

		*p++ = lpAmsData->tellerIdLen;
		memcpy(p,lpAmsData->tellerId,lpAmsData->tellerIdLen);
		p += lpAmsData->tellerIdLen;
	}
	else
	{
		memcpy(p,pMsg->cMessageBody,4);
		telleridlen = pMsg->cMessageBody[4];
		memcpy(p+4,&pMsg->cMessageBody[4],telleridlen+1);
		p += 5+telleridlen;
	}

	BEPUTSHORT(queryType, p);
	p += 2;
	
	BEPUTLONG(iret, p);
	p += 4;	

	s_Msg.iMessageLength = p - &s_Msg.cMessageBody[0];
	
	if(AMS_CRM_QUEUE_MNG_SUCCESS == iret && queryBuf != NULL)
	{
		if(queryBufLen + s_Msg.iMessageLength <= MSG_BODY_LEN)
		{		
			if(AMS_OK != AmsGetQueryResultIdByQueryType(queryType, &queryResultId))
			{
				return AMS_ERROR;				
			}

			*p++ = queryResultId;
		    BEPUTSHORT(queryBufLen, p);
		    p += 2;
			
			queryBufLen += 3;	
			
			memcpy(p, queryBuf, queryBufLen);

			s_Msg.iMessageLength += queryBufLen;			
		}
	}

	SendMsgBuff(&s_Msg,0);
	
	if(AmsMsgTrace)
	{	
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"send A_TELLER_QUERY_INFO_RSP msg \n"); 
		if(NULL != lpAmsData)
		{
			AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_TELLER_QUERY_INFO_RSP",description,
							descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,lpAmsData->sTraceName);				
		}
		else
		{
			AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_TELLER_QUERY_INFO_RSP",description,
				descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,"ams");	
		}
	}
	
	AmsMsgStatProc(AMS_CRM_MSG, s_Msg.iMessageType);
	AmsResultStatProc(AMS_CRM_QUERY_INFO_RESULT, iret);

	return SUCCESS;
}

//坐席示闲指示，超时，不改变当前坐席状态	
//坐席示忙指示，超时，坐席置忙
int CrmStateOperateIndTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg)
{
	//int					iret = AMS_MANAGER_SET_CRM_STATE_CRM_TIMEOUT;
	LP_AMS_DATA_t		*lpAmsData = NULL;   //进程数据区指针
	CRM_NODE            *pCrmNode = NULL;		
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
		descrlen=snprintf(description,1024,"recv B_AMS_TELLER_STATE_OP_IND_TIMEOUT msg[%d] \n",pTmMsg->iTimerId);	
		AmsTraceToFile(pTmMsg->s_ReceiverPid,pTmMsg->s_SenderPid,"B_AMS_TELLER_STATE_OP_IND_TIMEOUT",description,
						descrlen,pTmMsg->cTimerParameter,PARA_LEN,"ams");
	}
	
	//进程号有效性检查
	pid = pTmMsg->s_ReceiverPid.iProcessId;
	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		dbgprint("CrmStateOperateIndTimeoutProc Pid:%d Err", pid);
		//iret = AMS_MANAGER_SET_CRM_STATE_TIMEOUT_PARA_ERR;
		//AmsResultStatProc(AMS_MANAGER_SET_CRM_STATE_RESULT, iret);   		
		return AMS_ERROR;
	}
	lpAmsData = (LP_AMS_DATA_t *)ProcessData[pid];

	//amsPid检查
	p = pTmMsg->cTimerParameter;
	BEGETLONG(amsPid,p);
	if(lpAmsData->amsPid != amsPid)
	{
		dbgprint("CrmStateOperateIndTimeoutProc[%d] AmsPid[0x%x][0x%x] Err",pid,amsPid,lpAmsData->amsPid);
		return AMS_ERROR;
	}

	/* 杀掉定时器 */
	if(lpAmsData->iTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->iTimerId);
		AmsTimerStatProc(T_AMS_CRM_STATE_OPERATE_IND_TIMER, AMS_KILL_TIMER);
		pTmMsg->iTimerId = -1;
	} 

	
	pCrmNode = AmsSearchCrmNode(lpAmsData->srvGrpIdPos, lpAmsData->tellerId,lpAmsData->tellerIdLen);
	if(NULL == pCrmNode)
	{
		dbgprint("CrmStateOperateIndTimeoutProc[%d] Teller[%s]Err", 
			pid,lpAmsData->tellerId);		
		return AMS_ERROR;		
	}
	
	return AMS_OK;
}


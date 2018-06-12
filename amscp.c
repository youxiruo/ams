/******************************************************************
*
* FILE NAME						                           VERSION
*	
*	amscp.c	                                                v1.0.0
*
* DESCRIPTION
*
*	AMS应用管理服务器与CMS交互处理函数
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


int VtaRegReqProc(int iThreadId, MESSAGE_t *pMsg)
{
	int					 iret = AMS_CMS_PRCOESS_SUCCESS;
	unsigned int         tellerId = 0;	
	unsigned char        tellerNo[AMS_MAX_TELLER_NO_LEN + 1] = { 0 };	
	TERM_NET_INFO        tellerNetInfo;
	VTA_NODE             *pVtaNode = NULL;	
	unsigned char		 newTransIpLen = 0;
	unsigned char        newTransIp[AMS_MAX_TRANS_IP_LEN + 1]; //柜员IP地址字符串
	unsigned int         vtaIp = 0;
	unsigned char        transIpChange = 0;
	unsigned int         tellerCfgPos = 0;
	int                  pid = 0;	
	int                  pos = 0;
	int                  i = 0;
	int                  j = 0;
	unsigned char        *p;	

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
	
	//检查接收进程号
	if(pMsg->s_ReceiverPid.iProcessId != 0)
	{
		dbgprint("VtaRegReqProc[%d] Pid:%d Err", pid, pMsg->s_ReceiverPid.iProcessId);
		iret = AMS_CMS_VTA_REG_PARA_ERR;
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);

		return AMS_ERROR;
	}
	
	//消息长度检查
	if(pMsg->iMessageLength < 13)
	{
		dbgprint("VtaRegReqProc[%d] Len[%d] Err", pid, pMsg->iMessageLength);
		iret = AMS_CMS_VTA_REG_LEN_ERR;
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}
	
	p = pMsg->cMessageBody;
	
	//获取柜员工号
	BEGETLONG(tellerId, p);
	if(0 == tellerId)
	{
		dbgprint("VtaRegReqProc[%d] TellerId[%u] Err.", pid, tellerId);
		iret = AMS_CMS_VTA_REG_TELLER_ID_ERR;
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}
	AmsGetTellerNoById(tellerId, tellerNo);	
	p += 4;	
	
	//unpack OPart, e.g.:tellerNetInfo
	memset(&tellerNetInfo, 0, sizeof(TERM_NET_INFO));
	iret = AmsUnpackVtaRegReqOpartPara(p, pMsg->iMessageLength - 4, &tellerNetInfo);
    if(AMS_OK != iret)
	{
		dbgprint("VtaRegReqProc[%d][%d][%d] Teller[%s][%u] UnpackVtaRegReqPara Err", 
			pMsg->s_SenderPid.cModuleId,
			pMsg->s_SenderPid.cFunctionId,
			pMsg->s_SenderPid.iProcessId,
			tellerNo, tellerId);
		
		if(AMS_ERROR == iret)
		{
			iret = AMS_CMS_VTA_REG_PARA_ERR;
		}
		
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;		
	}
	
	//柜员工号检查
	//查看tellerId是否登录
    /* find Vta Node in process */
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		pVtaNode = AmsSearchVtaNode(i, tellerId);
		if(NULL != pVtaNode)
		{
			break;
		}
	}
	if(NULL == pVtaNode)
	{
		dbgprint("VtaRegReqProc[%d] Teller[%s]Id[%u] not Login.", 
			pid, tellerNo, tellerId);
		iret = AMS_CMS_VTA_REG_STATE_ERR;
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	} 
	
    /* check RegTeller in cfg or not */
	tellerCfgPos = pVtaNode->tellerCfgPos;
	if(tellerCfgPos >= AMS_MAX_VTA_NUM)
	{
		dbgprint("VtaRegReqProc[%d] Teller[%s][%u] CfgPos[%d]Err.",
			pid, tellerNo, tellerId, tellerCfgPos);
		iret = AMS_CMS_VTA_REG_TELLER_ID_ERR;
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;		
	}
	if(    AMS_TELLER_INSTALL != AmsCfgTeller(tellerCfgPos).flag 
		|| tellerId != AmsCfgTeller(tellerCfgPos).tellerId)
	{
		dbgprint("VtaRegReqProc[%d] Teller[%s]Id[%u][%u]Err-Flag[%d]Pos[%d].",
			pid, tellerNo, tellerId, AmsCfgTeller(tellerCfgPos).tellerId, 
			AmsCfgTeller(tellerCfgPos).flag, tellerCfgPos);
		
		iret = AMS_CMS_VTA_REG_TELLER_ID_ERR;
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;		
	}
	
    /* check teller has been registered or not */
	//已经注册，则更新注册信息
	for(i = 0; i < AMS_MAX_VTA_NUM; i++)
	{
		if(AmsRegTeller(i).flag != AMS_TELLER_REGISTER)
		{
			continue;
		}
		
		if(tellerId == AmsRegTeller(i).tellerId)
		{	
			pos = i;

			break;			
		}	
	}

	//尚未注册，则记录注册信息
	if(i >= AMS_MAX_VTA_NUM)
	{
		for(j = 0; j < AMS_MAX_VTA_NUM; j++)
		{
			if(AMS_TELLER_UNREGISTER == AmsRegTeller(j).flag)
			{
				break;
			}
		}

		if(j >= AMS_MAX_VTA_NUM)
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

		AmsRegTeller(j).flag = AMS_TELLER_REGISTER;	
		AmsRegTeller(j).tellerId = tellerId;	
		
		pos = j;
		
	}

    //记录或更新注册信息	
    if(tellerNetInfo.ip != AmsRegTeller(pos).vtaIp)
    {
		AmsRegTeller(pos).vtaIp = tellerNetInfo.ip;		
		transIpChange = 1;
    }
	AmsRegTeller(pos).vtaPort = tellerNetInfo.port;	

	AmsRegTeller(pos).cmsPid.iProcessId  = pMsg->s_SenderPid.cModuleId;
	AmsRegTeller(pos).cmsPid.cFunctionId = pMsg->s_SenderPid.cFunctionId;
	AmsRegTeller(pos).cmsPid.cModuleId   = pMsg->s_SenderPid.iProcessId;
	
	AmsRegTeller(pos).myPid.iProcessId  = pMsg->s_ReceiverPid.cModuleId;
	AmsRegTeller(pos).myPid.cFunctionId = pMsg->s_ReceiverPid.cFunctionId;
	AmsRegTeller(pos).myPid.cModuleId   = pMsg->s_ReceiverPid.iProcessId;   

	//record tellerRegPos
	pVtaNode->tellerRegPos = pos;

	//update vtaIp if changed
    if(1 == transIpChange)
    {
		pVtaNode->vtaInfo.vtaIp = AmsRegTeller(pos).vtaIp;	
		AmsCfgTeller(tellerCfgPos).vtaIp = AmsRegTeller(pos).vtaIp;

		vtaIp = htonl(AmsRegTeller(pos).vtaIp);
		memset(newTransIp, 0, AMS_MAX_TRANS_IP_LEN + 1);
		snprintf(newTransIp,sizeof(newTransIp),"%s",inet_ntoa(*((struct in_addr *)&vtaIp)));	
		
		newTransIpLen = strlen(newTransIp);
		if(newTransIpLen > 0 && newTransIpLen <= AMS_MAX_TRANS_IP_LEN)
		{
			strcpy((char *)pVtaNode->vtaInfo.transIp, newTransIp); 	
			pVtaNode->vtaInfo.transIp[newTransIpLen] = '\0';
			pVtaNode->vtaInfo.transIpLen = newTransIpLen;

			strcpy((char *)AmsCfgTeller(tellerCfgPos).transIp, newTransIp); 	
			AmsCfgTeller(tellerCfgPos).transIp[newTransIpLen] = '\0';
			AmsCfgTeller(tellerCfgPos).transIpLen = newTransIpLen;
		}
		else
		{
			memset(pVtaNode->vtaInfo.transIp, 0, AMS_MAX_TRANS_IP_LEN + 1);
			pVtaNode->vtaInfo.transIpLen = 0;	

			memset(AmsCfgTeller(tellerCfgPos).transIp, 0, AMS_MAX_TRANS_IP_LEN + 1);
			AmsCfgTeller(tellerCfgPos).transIpLen = 0;	
		}
    }	

	//record vtaPort
	pVtaNode->vtaInfo.vtaPort = AmsRegTeller(pos).vtaPort;
	AmsCfgTeller(tellerCfgPos).vtaPort = AmsRegTeller(pos).vtaPort;

	//send Vta Reg Rsp to CMS
	AmsSendCmsVtaRegRsp(&AmsRegTeller(pos),pMsg,iret);

	return iret;
}

int VtmRegReqProc(int iThreadId, MESSAGE_t *pMsg)
{
	int					 iret = AMS_CMS_PRCOESS_SUCCESS;
	unsigned int         vtmId = 0;	
	unsigned char        vtmNo[AMS_MAX_VTM_NO_LEN + 1] = { 0 };	
	TERM_NET_INFO        vtmNetInfo;
	VTM_NODE             *pVtmNode = NULL;	
	unsigned char		 newTransIpLen = 0;
	unsigned char        newTransIp[AMS_MAX_TRANS_IP_LEN + 1]; //柜员IP地址字符串
	unsigned int         vtmIp = 0;
	unsigned char        transIpChange = 0;
	unsigned int         vtmCfgPos = 0;
	int                  pid = 0;		
	int                  pos = 0;
	int                  i = 0;
	int                  j = 0;
	unsigned char        *p;	

	if(AmsMsgTrace)
	{
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"recv A_VTM_REG_REQ msg \n");	
		AmsTraceToFile(pMsg->s_ReceiverPid,pMsg->s_SenderPid,"A_VTM_REG_REQ",description,
						descrlen,pMsg->cMessageBody,pMsg->iMessageLength,"ams");		
	}
	
	//get remote pid
	pid = pMsg->s_SenderPid.iProcessId;
	
	//检查接收进程号
	if(pMsg->s_ReceiverPid.iProcessId != 0)
	{
		dbgprint("VtmRegReqProc[%d] Pid:%d Err", pid, pMsg->s_ReceiverPid.iProcessId);
		iret = AMS_CMS_VTM_REG_PARA_ERR;
		AmsSendCmsVtmRegRsp(NULL,pMsg,iret);

		return AMS_ERROR;
	}
	
	//消息长度检查
	if(pMsg->iMessageLength < 13)
	{
		dbgprint("VtmRegReqProc[%d] Len[%d] Err", pid, pMsg->iMessageLength);
		iret = AMS_CMS_VTM_REG_LEN_ERR;
		AmsSendCmsVtmRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}
	
	p = pMsg->cMessageBody;
	
	//获取柜员工号
	BEGETLONG(vtmId, p);
	if(0 == vtmId)
	{
		dbgprint("VtmRegReqProc[%d] VtmId[%u] Err.", pid, vtmId);
		iret = AMS_CMS_VTM_REG_VTM_ID_ERR;
		AmsSendCmsVtmRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}
	AmsGetVtmNoById(vtmId, vtmNo);	
	p += 4;	
	
	//unpack OPart, e.g.:vtmNetInfo
	memset(&vtmNetInfo, 0, sizeof(TERM_NET_INFO));
	iret = AmsUnpackVtmRegReqOpartPara(p, pMsg->iMessageLength - 4, &vtmNetInfo);
    if(AMS_OK != iret)
	{
		dbgprint("VtmRegReqProc[%d][%d][%d] Vtm[%s][%d] UnpackVtaRegReqPara Err", 
			pMsg->s_SenderPid.cModuleId,
			pMsg->s_SenderPid.cFunctionId,
			pMsg->s_SenderPid.iProcessId,
			vtmNo, vtmId);
		
		if(AMS_ERROR == iret)
		{
			iret = AMS_CMS_VTM_REG_PARA_ERR;
		}
		
		AmsSendCmsVtmRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;		
	}
	
	//柜员机标识检查
	//查看vtmId是否登录
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
		dbgprint("VtmRegReqProc[%d] Vtm[%s]Id[%u] not Login.", 
			pid, vtmNo, vtmId);
		iret = AMS_CMS_VTM_REG_STATE_ERR;
		AmsSendCmsVtmRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	} 
	
    /* check Reg Vtm in cfg or not */
	vtmCfgPos = pVtmNode->vtmCfgPos;
	if(vtmCfgPos >= AMS_MAX_VTM_NUM)
	{
		dbgprint("VtmRegReqProc[%d] Vtm[%s][%u]CfgPos[%d]Err.", 
			pid, vtmNo, vtmId, vtmCfgPos);
		iret = AMS_CMS_VTM_REG_VTM_ID_ERR;
		AmsSendCmsVtmRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;		
	}
	if(    AMS_VTM_INSTALL != AmsCfgVtm(vtmCfgPos).flag 
		|| vtmId != AmsCfgVtm(vtmCfgPos).vtmId)
	{
		dbgprint("VtmRegReqProc[%d] Vtm[%s]Id[%u][%u]Err-Flag[%d]Pos[%d].",
			pid, vtmNo, vtmId, AmsCfgVtm(vtmCfgPos).vtmId, 
			AmsCfgVtm(vtmCfgPos).flag, vtmCfgPos);
		
		iret = AMS_CMS_VTM_REG_VTM_ID_ERR;
		AmsSendCmsVtmRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;		
	}
	
    /* check vtm has been registered or not */
	//已经注册，则更新注册信息
	for(i = 0; i < AMS_MAX_VTM_NUM; i++)
	{
		if(AmsRegVtm(i).flag != AMS_VTM_REGISTER)
		{
			continue;
		}
		
		if(vtmId == AmsRegVtm(i).vtmId)
		{	
			pos = i;

			break;			
		}	
	}

	//尚未注册，则记录注册信息
	if(i >= AMS_MAX_VTM_NUM)
	{
		for(j = 0; j < AMS_MAX_VTM_NUM; j++)
		{
			if(AMS_VTM_UNREGISTER == AmsRegVtm(j).flag)
			{
				break;
			}
		}

		if(j >= AMS_MAX_VTM_NUM)
		{
			dbgprint("VtmRegReqProc[%d][%d][%d] Vtm[%s][%u] ResourceLimited", 
				pMsg->s_SenderPid.cModuleId,
			    pMsg->s_SenderPid.cFunctionId,
			    pMsg->s_SenderPid.iProcessId,
				vtmNo, vtmId);	
			
			iret = AMS_CMS_VTM_REG_RESOURCE_LIMITED;
			AmsSendCmsVtmRegRsp(NULL,pMsg,iret);		
			return AMS_ERROR;	
		}

		AmsRegVtm(j).flag = AMS_VTM_REGISTER;	
		AmsRegVtm(j).vtmId = vtmId;	
		
		pos = j;
		
	}

    //记录或更新注册信息	
    if(vtmNetInfo.ip != AmsRegVtm(pos).vtmIp)
    {
		AmsRegVtm(pos).vtmIp = vtmNetInfo.ip;		
		transIpChange = 1;
    }
	AmsRegVtm(pos).vtmPort = vtmNetInfo.port;	

	AmsRegVtm(pos).cmsPid.iProcessId  = pMsg->s_SenderPid.cModuleId;
	AmsRegVtm(pos).cmsPid.cFunctionId = pMsg->s_SenderPid.cFunctionId;
	AmsRegVtm(pos).cmsPid.cModuleId   = pMsg->s_SenderPid.iProcessId;
	
	AmsRegVtm(pos).myPid.iProcessId  = pMsg->s_ReceiverPid.cModuleId;
	AmsRegVtm(pos).myPid.cFunctionId = pMsg->s_ReceiverPid.cFunctionId;
	AmsRegVtm(pos).myPid.cModuleId   = pMsg->s_ReceiverPid.iProcessId;   

	//record vtmRegPos
	pVtmNode->vtmRegPos = pos;

	//update vtmIp if changed
    if(1 == transIpChange)
    {
		pVtmNode->vtmInfo.vtmIp = AmsRegVtm(pos).vtmIp;	
		AmsCfgVtm(vtmCfgPos).vtmIp = AmsRegVtm(pos).vtmIp;

		vtmIp = htonl(AmsRegVtm(pos).vtmIp);
		memset(newTransIp, 0, AMS_MAX_TRANS_IP_LEN + 1);
		snprintf(newTransIp,sizeof(newTransIp),"%s",inet_ntoa(*((struct in_addr *)&vtmIp)));	
		
		newTransIpLen = strlen(newTransIp);
		if(newTransIpLen > 0 && newTransIpLen <= AMS_MAX_TRANS_IP_LEN)
		{
			strcpy((char *)pVtmNode->vtmInfo.transIp, newTransIp); 	
			pVtmNode->vtmInfo.transIp[newTransIpLen] = '\0';
			pVtmNode->vtmInfo.transIpLen = newTransIpLen;

			strcpy((char *)AmsCfgVtm(vtmCfgPos).transIp, newTransIp); 	
			AmsCfgVtm(vtmCfgPos).transIp[newTransIpLen] = '\0';
			AmsCfgVtm(vtmCfgPos).transIpLen = newTransIpLen;
		}
		else
		{
			memset(pVtmNode->vtmInfo.transIp, 0, AMS_MAX_TRANS_IP_LEN + 1);
			pVtmNode->vtmInfo.transIpLen = 0;	

			memset(AmsCfgVtm(vtmCfgPos).transIp, 0, AMS_MAX_TRANS_IP_LEN + 1);
			AmsCfgVtm(vtmCfgPos).transIpLen = 0;	
		}
    }	

	//record vtmPort
	pVtmNode->vtmInfo.vtmPort = AmsRegVtm(pos).vtmPort;
	AmsCfgVtm(vtmCfgPos).vtmPort = AmsRegVtm(pos).vtmPort;

	//send Vta Reg Rsp to CMS
	AmsSendCmsVtmRegRsp(&AmsRegVtm(pos),pMsg,iret);

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

#ifdef AMS_TEST_LT
	time_t              currentTime;
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
	
	//检查接收进程号
	if(pMsg->s_ReceiverPid.iProcessId != 0)
	{
		dbgprint("VtaGetReqProc[%d] Pid:%d Err", pid, pMsg->s_ReceiverPid.iProcessId);
		iret = AMS_CMS_GET_VTA_PARA_ERR;
		AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);

		return AMS_ERROR;
	}
	
	//消息长度检查
	if(pMsg->iMessageLength < 23)
	{
		dbgprint("VtaGetReqProc[%d] Len[%d] Err", pid, pMsg->iMessageLength);
		iret = AMS_CMS_GET_VTA_LEN_ERR;
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
	
	//终端类型检查
	BEGETLONG(terminalType, p);
	if(terminalType < AMS_TERMINAL_VTA || terminalType >= AMS_TERMINAL_MAX)
	{
		dbgprint("VtaGetReqProc[%d] TerminalType[%d]Err", pid, terminalType);
		iret = AMS_CMS_GET_VTA_TERMINAL_TYPE_ERR;
		AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
		return AMS_ERROR;		
	}	
	p += 4;

	if(AMS_TERMINAL_VTA == terminalType)
	{
		//柜员标识检查
		BEGETLONG(originTellerId, p);
		if(0 == originTellerId)
		{
			dbgprint("VtaGetReqProc[%d] OriginTellerId[%u]Err", pid, originTellerId);
			iret = AMS_CMS_GET_VTA_ORIGIN_TELLER_ID_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
			return AMS_ERROR;		
		}
		AmsGetTellerNoById(originTellerId, originVtaNo);
		p += 4;

		//查看origin tellerId是否登录
	    /* find Origin Vta Node in process */
		for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
		{
			pOriginVtaNode = AmsSearchVtaNode(i, originTellerId);
			if(NULL != pOriginVtaNode)
			{
				break;
			}
		}
		if(NULL == pOriginVtaNode)
		{
			dbgprint("VtaGetReqProc[%d] OriginTeller[%s]Id[%u] not Login.",
				pid, originVtaNo, originTellerId);
			iret = AMS_CMS_GET_VTA_ORIGIN_TELLER_ID_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
			return AMS_ERROR;
		} 

		//检查进程号
		originPid = pOriginVtaNode->amsPid & 0xffff;
		if((0 == originPid) || (originPid >= LOGIC_PROCESS_SIZE))
		{
			dbgprint("VtaGetReqProc[%d] OriginTeller[%s][%u] TargetPid[0x%x][%d]Err", 
				pid, originVtaNo, originTellerId, 
				pOriginVtaNode->amsPid, originPid);
			iret = AMS_CMS_GET_VTA_ORIGIN_TELLER_AMS_PID_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
			return AMS_ERROR;
		}
		
		lpOriginAmsData=(LP_AMS_DATA_t *)ProcessData[originPid];
				
		//进程号匹配性检查
		if(lpOriginAmsData->myPid.iProcessId != originPid)
		{
			dbgprint("VtaGetReqProc[%d] OriginTeller[%s][%u] OriginPID[%d][%d] Not Equal", 
				pid, originVtaNo, originTellerId, 
				lpOriginAmsData->myPid.iProcessId, originPid);
			iret = AMS_CMS_GET_VTA_ORIGIN_TELLER_AMS_PID_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
			return AMS_ERROR;
		}
		
		//柜员业务组编号检查
		if(lpOriginAmsData->srvGrpId > AMS_SERVICE_GROUP_ID_MAX) 
		{
			dbgprint("VtaGetReqProc[%d] OriginTeller[%s][%u] SrvGrpId[%u]Err", 
				pid, originVtaNo, originTellerId, lpOriginAmsData->srvGrpId);
			iret = AMS_CMS_GET_VTA_ORIGIN_TELLER_SERVICE_GROUP_ID_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
			return AMS_ERROR;		
		}
		
	    //柜员业务状态检查
		if(AMS_SERVICE_ACTIVE != AmsSrvData(lpOriginAmsData->srvGrpId).serviceState)
		{
			dbgprint("VtaGetReqProc[%d] OriginTeller[%s][%u] ServiceState[%d]Err", 
				pid, originVtaNo, originTellerId,  
				AmsSrvData(lpOriginAmsData->srvGrpId).serviceState);
			iret = AMS_CMS_GET_VTA_ORIGIN_TELLER_SERVICE_STATE_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
			return AMS_ERROR;			
		}

		//柜员状态检查
		if(pOriginVtaNode->state >= AMS_VTA_STATE_OFFLINE)
		{
			dbgprint("VtaGetReqProc[%d] OriginTeller[%s][%u] State[%d]Err", 
				pid, originVtaNo, originTellerId, pOriginVtaNode->state);
			iret = AMS_CMS_GET_VTA_ORIGIN_TELLER_STATE_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
			return AMS_ERROR;		
		}

		//业务组编号
		BEGETLONG(srvGrpId, p);
		p += 4;
		
		//业务类型
		BEGETLONG(serviceTypeRsvd, p);
		p += 4;
		
		BEGETLONG(serviceType, p);
		p += 4;

	}
	else //AMS_TERMINAL_VTM
	{
		//柜员机终端设备号检查
		BEGETLONG(vtmId, p);
		if(0 == vtmId)
		{
			dbgprint("VtaGetReqProc[%d] VtmId[%u]Err", pid, vtmId);
			iret = AMS_CMS_GET_VTA_VTM_ID_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
			return AMS_ERROR;		
		}	
		AmsGetVtmNoById(vtmId, vtmNo);		
		p += 4;
		
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
			dbgprint("VtaGetReqProc[%d] Vtm[%s]Id[%u]Err", pid, vtmNo, vtmId);
			iret = 	AMS_CMS_GET_VTA_VTM_ID_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
			return AMS_ERROR;		
		}

		//VTM 状态异常保护
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
				if(pVtmNode->customerPid > 0 && pVtmNode->customerPid < LOGIC_PROCESS_SIZE)
				{
					if(pVtmNode->customerPid > 0 && pVtmNode->customerPid < LOGIC_PROCESS_SIZE)
					{
						lpQueueData=(LP_QUEUE_DATA_t *)ProcessData[pVtmNode->customerPid];
						
						/* 杀掉定时器 */
						if(lpQueueData->iTimerId >= 0)
						{
							AmsQueueKillTimer(pVtmNode->customerPid, &lpQueueData->iTimerId);//或 lpQueueData->myPid.iProcessId
							AmsTimerStatProc(T_AMS_CUSTOMER_IN_QUEUE_TIMER, AMS_KILL_TIMER);
						} 

						//release lpQueueData Pid
						AmsReleassPid(lpQueueData->myPid, END);
					}
				}					
			}		
			
			//set Vtm Service State and State Start Time
			AmsSetVtmServiceState(pVtmNode, AMS_CUSTOMER_SERVICE_NULL);	
		}
		
		//reset amsPid
		pVtmNode->amsPid = 0;

		//reset customerPid
		pVtmNode->customerPid = 0;
			
		//业务组编号检查
		BEGETLONG(srvGrpId, p);
		if(srvGrpId > AMS_SERVICE_GROUP_ID_MAX)
		{
			dbgprint("VtaGetReqProc[%d] Vtm[%s][%u] SrvGrpId[%u]Err", 
				pid, vtmNo, vtmId, srvGrpId);
			iret = AMS_CMS_GET_VTA_SERVICE_GROUP_ID_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
			return AMS_ERROR;		
		}
		p += 4;
		
		//业务类型检查
		BEGETLONG(serviceTypeRsvd, p);
		if(serviceTypeRsvd != AMS_SERVICE_RSVD_VALUE)
		{
			dbgprint("VtaGetReqProc[%d] Vtm[%s][%u] ServiceTypeRsvd[0x%x]Err", 
				pid, vtmNo, vtmId, serviceTypeRsvd);
			iret = 	AMS_CMS_GET_VTA_SERVICE_TYPE_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
			return AMS_ERROR;		
		}
		p += 4;
		
		BEGETLONG(serviceType, p);
		if(AMS_OK != AmsCheckTellerServiceAbility(serviceType, AMS_SERVICE_VALUE_TOTAL))//AMS_SERVICE_TOTAL
		{
			dbgprint("VtaGetReqProc[%d] Vtm[%s][%u] ServiceType[0x%x]-[0x%x]Err", 
				pid, vtmNo, vtmId, serviceType, AMS_SERVICE_VALUE_TOTAL);//AMS_SERVICE_TOTAL
			iret = 	AMS_CMS_GET_VTA_SERVICE_TYPE_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
			return AMS_ERROR;		
		}
		p += 4;

		//检查License有效期
		if(LIC_VALID != AmsLicData.licState)
		{
			dbgprint("VtaGetReqProc[%d] Vtm[%s][%u] LicTimeout", 
				pid, vtmNo, vtmId, tps, AmsLicData.tps);
			iret = 	AMS_CMS_GET_VTA_LICENSE_TIMEOUT;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
			return AMS_ERROR;		
		}

		//检查License Tps
		if(AMS_OK != AmsCheckTps(&tps))
		{
			dbgprint("VtaGetReqProc[%d] Vtm[%s][%u] Tps[%d]-LicTps[%d]Err", 
				pid, vtmNo, vtmId, tps, AmsLicData.tps);
			iret = 	AMS_CMS_GET_VTA_TPS_BEYOND_LIC;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
			return AMS_ERROR;				
		}
	}

	//呼叫类型检查
	BEGETSHORT(callType, p);
	if(callType >= AMS_CALL_TYPE_MAX)
	{
		dbgprint("VtaGetReqProc[%d] Teller[%s][%u] Vtm[%s][%u] CallType[%d]Err", 
			pid, originVtaNo, originTellerId, vtmNo, vtmId, callType);
		iret = 	AMS_CMS_GET_VTA_CALL_TYPE_ERR;
		AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
		return AMS_ERROR;		
	}		
	p += 2;
		
	if(pMsg->iMessageLength - 22 - 1 - callIdLen > 0)
	{
		if(AMS_CALL_INNER == callType || AMS_CALL_TRANSFER == callType || AMS_CALL_MONITOR == callType)
		{
			//unpack OPart, e.g.:callTarget
			memset(&callTarget, 0, sizeof(CALL_TARGET));		
			iret = AmsUnpackVtaGetReqOpartPara(p, pMsg->iMessageLength - 22 - 1 - callIdLen, &callTarget);
		    if(AMS_OK != iret)
			{
				dbgprint("VtaGetReqProc[%d] Teller[%s][%u] Vtm[%s][%u] CallType[%d]UnpackOpart Err", 
					pid, originVtaNo, originTellerId, 
					vtmNo, vtmId, callType);
				if(AMS_ERROR == iret)
				{
					iret = AMS_CMS_GET_VTA_PARA_ERR;
				}
				
				AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
				return AMS_ERROR;		
			}

			//检查srvGrpId与callTarget是否匹配
			//呼叫转移目标的业务组编号可能比客户的业务组编号高
/*			if(AMS_CALL_GROUP == callTarget.callTargetType)
			{
				if(srvGrpId < callTarget.targetTellerGroupId)
				{
					dbgprint("VtaGetReqProc[%d] Teller[%s][%u] Vtm[%s][%u] SrvGrpId[%u]TargetTellerGroupId[%d]notMatch", 
						pid, originVtaNo, originTellerId, vtmNo, vtmId, 
						srvGrpId, callTarget.targetTellerGroupId);
					iret = AMS_CMS_GET_VTA_SERVICE_GROUP_ID_AND_TARGET_GROUP_NO_MATCH;
					
					AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
					return AMS_ERROR;					
				}
			}
*/			
			if(   (AMS_CALL_INNER == callType || AMS_CALL_MONITOR == callType) 
				&& AMS_CALL_TELLER != callTarget.callTargetType)
			{
				dbgprint("VtaGetReqProc[%d] Teller[%s][%u] Vtm[%s][%u] CallType[%d] TargetTellerType[%u]Err", 
					pid, originVtaNo, originTellerId, vtmNo, vtmId, 
					callType, callTarget.callTargetType);
				iret = AMS_CMS_GET_VTA_TARGET_GROUP_ERR;
				
				AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
				return AMS_ERROR;
			}

			if(AMS_CALL_TRANSFER == callType && AMS_CALL_GROUP == callTarget.callTargetType)
			{
				//targetTellerGroupId has been checked			
				if(srvGrpId != callTarget.targetTellerGroupId)
				{
					srvGrpId = callTarget.targetTellerGroupId;
				}
			}

			AmsGetTellerNoById(callTarget.targetTellerId, targetVtaNo);
				
			//AMS_CALL_TELLER
			if(    (   AMS_CALL_INNER == callType 
				    || AMS_CALL_TRANSFER == callType 
				    || AMS_CALL_MONITOR == callType) 
				&& AMS_CALL_TELLER == callTarget.callTargetType)
			{				
				for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
				{
					pTargetVtaNode = AmsSearchVtaNode(i, callTarget.targetTellerId);
					if(NULL != pTargetVtaNode)
					{
						break;
					}
				}
				if(NULL == pTargetVtaNode)
				{
					dbgprint("VtaGetReqProc[%d] Teller[%s][%u] Vtm[%s][%u] SrvGrp[%u]TargetTeller[%s][%u]notLogin", 
						pid, originVtaNo, originTellerId, vtmNo, vtmId, 
						srvGrpId, targetVtaNo, callTarget.targetTellerId);
					iret = AMS_CMS_GET_VTA_TARGET_TELLER_STATE_ERR;
					
					AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
					return AMS_ERROR;		
				}	
				
				//get srvGrpId
	            srvGrpId = i;
			}
			
		}
	}

	//先处理内部呼叫和监听呼叫
	if(AMS_CALL_INNER == callType || AMS_CALL_MONITOR == callType)
	{
		if(AMS_TERMINAL_VTA != terminalType || NULL == lpOriginAmsData)
		{
			dbgprint("VtaGetReqProc[%d] OriginTeller[%s][%u] Vtm[%s][%u] CallType[%d][%d]Err", 
				pid, originVtaNo, originTellerId, vtmNo, vtmId, callType, terminalType);
			iret = 	AMS_CMS_GET_VTA_CALL_TYPE_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
			return AMS_ERROR;	
		}

		//内部呼叫和监听呼叫不关心orgId,orgCfgPos,填0
		
		//业务智能路由
		pVtaNode = AmsServiceIntelligentSelectVta(lpOriginAmsData->vtmNo,lpOriginAmsData->vtmId,srvGrpId,serviceType,callType,callTarget,0,0,&iret);
		if(NULL == pVtaNode)
		{
			dbgprint("VtaGetReqProc[%d] OriginTeller[%s][%u] Vtm[%s][%u] CallType[%d] TargetTeller[%s][%u]SISelectVta Failed",
				pid, originVtaNo, originTellerId, 
				lpOriginAmsData->vtmNo, lpOriginAmsData->vtmId, 
				callType, targetVtaNo, callTarget.targetTellerId);
			iret = AMS_CMS_GET_VTA_SERVICE_INTELLIGENT_ROUTING_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);		
			return AMS_ERROR;	
		}
		else
		{
			//检查进程号
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

			//更新进程数据
			//record vtmId
			lpAmsData->vtmId = lpOriginAmsData->vtmId;
			
			//record vtmNo
			if(lpOriginAmsData->vtmNoLen <= AMS_MAX_VTM_NO_LEN)
			{	
				memset(lpAmsData->vtmNo, 0, AMS_MAX_VTM_NO_LEN + 1);
				memcpy(lpAmsData->vtmNo, lpOriginAmsData->vtmNo, lpOriginAmsData->vtmNoLen);
				lpAmsData->vtmNoLen = lpOriginAmsData->vtmNoLen;
			}
		
			//record vtmPos
			lpAmsData->vtmPos = lpOriginAmsData->vtmPos;
			
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
	        memcpy(&lpAmsData->vtmPid,&lpOriginAmsData->vtmPid,sizeof(PID_t));
			
			//record callId
		    lpAmsData->callIdLen = callIdLen;
			memcpy(lpAmsData->callId, &pMsg->cMessageBody[1], callIdLen);
	
			//update cmsPid
			lpAmsData->cmsPid.cModuleId	   = pMsg->s_SenderPid.cModuleId;
			lpAmsData->cmsPid.cFunctionId  = pMsg->s_SenderPid.cFunctionId;
			lpAmsData->cmsPid.iProcessId   = pMsg->s_SenderPid.iProcessId;
			
			//lpAmsData 可为NULL
    		AmsSendCmsVtaGetRsp(lpAmsData,pMsg,iret,pVtaNode,NULL);	

			return AMS_OK;
		}
	}


	//VTM呼叫VTA普通呼叫或呼叫转移

	//检查业务组编号与业务类型组合
	if (0 == srvGrpId && 0 == serviceType)
	{
		dbgprint("VtaGetReqProc[%d] Vtm[%s][%u] SrvGrpId[%u]ServiceType[0x%x]Err", 
			pid, vtmNo, vtmId, srvGrpId, serviceType);
		iret = 	AMS_CMS_GET_VTA_NO_VALID_SERVICE_GROUP_ID_OR_TYPE;
		AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
		return AMS_ERROR;
	}

	//仅携带业务类型，没有指明业务组编号
	if (0 == srvGrpId && 0 != serviceType)
	{
		//根据业务类型选择业务组
		iret = AmsSelectSrvGrpIdByServiceType(vtmNo,vtmId,serviceType,&srvGrpId);
		
		if(AMS_OK != iret)
		{
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);		
			return AMS_ERROR;			
		}	

		srvGrpSelfAdapt = 1;
	}
	
	//新呼叫添加到链表尾
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
	
	//业务智能路由
	pVtaNode = AmsServiceIntelligentSelectVta(vtmNo,vtmId,srvGrpId,serviceType,callType,callTarget,pVtmNode->vtmInfo.orgId,pVtmNode->orgCfgPos,&iret);
	if(NULL == pVtaNode)
	{
		if(AMS_CMS_GET_VTA_SERVICE_IN_QUEUE != iret)
		{
			dbgprint("VtaGetReqProc[%d] Vtm[%s][%u] OriginTeller[%s][%u] SISelectVta Failed", 
				pid, vtmNo, vtmId, originVtaNo, originTellerId);
			
			if(AMS_ERROR == iret)
			{
				iret = AMS_CMS_GET_VTA_SERVICE_INTELLIGENT_ROUTING_ERR;
			}
					
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);		
			return AMS_ERROR;			
		}

		iret = AmsStartCustomerQueueProcess(pMsg,pVtmNode,srvGrpId,serviceType,callType,callTarget,callIdLen,srvGrpSelfAdapt);
		
		if(AMS_OK != iret)
		{
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);		
			return AMS_ERROR;			
		}
		
		//Customer In Queue...
		iret = AMS_CMS_GET_VTA_SERVICE_IN_QUEUE;
		
		//update Customer Service State
		AmsSetVtmServiceState(pVtmNode, AMS_CUSTOMER_IN_QUEUE);

		//init enterQueueTime
		time(&pVtmNode->enterQueueTime);

		//record serviceType	
		pVtmNode->serviceType = serviceType;
		
		if(AmsStateTrace)
		{
			unsigned char description [256];
			int descrlen;
			memset(description,0,sizeof(description));
			descrlen=snprintf(description,256,"Ams Customer of Vtm[%s][%u]-[%d][%d] is in Queue...",
				pVtmNode->vtmInfo.vtmNo, pVtmNode->vtmInfo.vtmId, srvGrpId, serviceType);
			
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
			dbgprint("VtaGetReqProc Vtm[%s][%u] Teller[%s][%u] Pid[0x%x][%d]Err", 
				vtmNo, vtmId, pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId,
				pVtaNode->amsPid, pid);
			iret = AMS_CMS_GET_VTA_AMS_PID_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,pVtaNode,NULL);
			return AMS_ERROR;
		}
		
		lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];

		//更新进程数据
		//record vtmId
		lpAmsData->vtmId = vtmId;

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
		time(&currentTime);	   
		AmsUpdateSingleVtaWorkInfo(pVtaNode, currentTime);
	
		//set Vta State and State Start Time
		AmsSetVtaState(iThreadId, lpAmsData, pVtaNode, AMS_VTA_STATE_BUSY, 0);
#endif

		//record vtmPid
        memcpy(&lpAmsData->vtmPid,&pVtmNode->rPid,sizeof(PID_t));
		
		//record callId
	    lpAmsData->callIdLen = callIdLen;
		memcpy(lpAmsData->callId, &pMsg->cMessageBody[1], callIdLen);

		//record amsPid
		pVtmNode->amsPid = pVtaNode->amsPid;

		//record serviceType	
		pVtmNode->serviceType = serviceType;
		
	    //update Customer Service State
		AmsSetVtmServiceState(pVtmNode, AMS_CUSTOMER_IN_SERVICE);

		//update cmsPid
		lpAmsData->cmsPid.cModuleId	   = pMsg->s_SenderPid.cModuleId;
		lpAmsData->cmsPid.cFunctionId  = pMsg->s_SenderPid.cFunctionId;
		lpAmsData->cmsPid.iProcessId   = pMsg->s_SenderPid.iProcessId;
		
	}
	
	//update cmsPid
	pVtmNode->cmsPid.cModuleId	  = pMsg->s_SenderPid.cModuleId;
	pVtmNode->cmsPid.cFunctionId  = pMsg->s_SenderPid.cFunctionId;
	pVtmNode->cmsPid.iProcessId   = pMsg->s_SenderPid.iProcessId;
	
    //set Vtm State and State Start Time
	AmsSetVtmState(iThreadId, pVtmNode, AMS_VTM_STATE_BUSY);

	//send Vta Get Rsp to CMS
	if(AMS_CUSTOMER_IN_SERVICE == pVtmNode->serviceState)
	{
		//lpAmsData 可为NULL
    	AmsSendCmsVtaGetRsp(lpAmsData,pMsg,iret,pVtaNode,NULL);
	}
	
	if(AMS_CUSTOMER_IN_QUEUE == pVtmNode->serviceState)
	{
		//lpAmsData 可为NULL
    	AmsSendCmsVtaGetRsp(lpAmsData,pMsg,iret,NULL,pVtmNode);
	}
	
	return iret;
}

int AmsQueryTermNetInfoReqProc(int iThreadId, MESSAGE_t *pMsg)
{
	int					 iret = AMS_CMS_PRCOESS_SUCCESS;
	unsigned int         termType = AMS_TERMINAL_RSVD;	
	unsigned int         termId = 0;	
	unsigned int         tellerId = 0;
	unsigned char        tellerNo[AMS_MAX_TELLER_NO_LEN + 1] = { 0 };		
	unsigned int         vtmId = 0;	
	unsigned char        vtmNo[AMS_MAX_VTM_NO_LEN + 1] = { 0 };	
	VTA_NODE             *pVtaNode = NULL;	
	VTM_NODE             *pVtmNode = NULL;	
	unsigned int         tellerRegPos = 0;	
	unsigned int         vtmRegPos = 0;
	int                  pid = 0;		
	int                  i = 0;
	unsigned char        *p;	

	if(AmsMsgTrace)
	{
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"recv A_TERM_NETINFO_QUERY_REQ msg \n");	
		AmsTraceToFile(pMsg->s_ReceiverPid,pMsg->s_SenderPid,"A_TERM_NETINFO_QUERY_REQ",description,
						descrlen,pMsg->cMessageBody,pMsg->iMessageLength,"ams");		
	}

	//get remote pid
	pid = pMsg->s_SenderPid.iProcessId;
	
	//检查接收进程号	
	if(pMsg->s_ReceiverPid.iProcessId != 0)
	{
		dbgprint("AmsQueryTermNetInfoReqProc[%d] Pid:%d Err", pid, pMsg->s_ReceiverPid.iProcessId);
		iret = AMS_CMS_QUERY_TERM_NETINFO_PARA_ERR;
		AmsSendQueryTermNetInfoRsp(pMsg,iret,NULL,NULL);

		return AMS_ERROR;
	}
	
	//消息长度检查
	if(pMsg->iMessageLength < 8)
	{
		dbgprint("AmsQueryTermNetInfoReqProc[%d] Len[%d] Err", pid, pMsg->iMessageLength);
		iret = AMS_CMS_QUERY_TERM_NETINFO_LEN_ERR;
		AmsSendQueryTermNetInfoRsp(pMsg,iret,NULL,NULL);
		return AMS_ERROR;
	}
	
	p = pMsg->cMessageBody;
	
	//终端类型检查
	BEGETLONG(termType, p);
	if(termType < AMS_TERMINAL_VTA || termType >= AMS_TERMINAL_MAX)
	{
		dbgprint("AmsQueryTermNetInfoReqProc[%d] TermType[%d]Err", pid, termType);
		iret = AMS_CMS_QUERY_TERM_NETINFO_TERMINAL_TYPE_ERR;
		AmsSendQueryTermNetInfoRsp(pMsg,iret,NULL,NULL);
		return AMS_ERROR;		
	}	
	p += 4;
	
	//获取终端标识
	BEGETLONG(termId, p);
	if(0 == termId)
	{
		dbgprint("AmsQueryTermNetInfoReqProc[%d] TermId[%u] Err.", pid, termId);
		iret = AMS_CMS_QUERY_TERM_NETINFO_VTM_ID_ERR;
		AmsSendQueryTermNetInfoRsp(pMsg,iret,NULL,NULL);
		return AMS_ERROR;
	}

	if(AMS_TERMINAL_VTM == termType)
	{
		//柜员机标识检查		
		vtmId = termId;
		AmsGetVtmNoById(vtmId, vtmNo);		
		
		//查看vtmId是否登录
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
			dbgprint("AmsQueryTermNetInfoReqProc[%d] Vtm[%s]Id[%u] not Login.",
				pid, vtmNo, vtmId);
			iret = AMS_CMS_QUERY_TERM_NETINFO_STATE_ERR;
			AmsSendQueryTermNetInfoRsp(pMsg,iret,NULL,NULL);
			return AMS_ERROR;
		} 
		
	    /* check Reg Vtm in Reg or not */
		vtmRegPos = pVtmNode->vtmRegPos;
		if(vtmRegPos >= AMS_MAX_VTM_NUM)
		{
			dbgprint("AmsQueryTermNetInfoReqProc[%d] Vtm[%s][%u]RegPos[%d]Err.",
				pid, vtmNo, vtmId, vtmRegPos);
			iret = AMS_CMS_QUERY_TERM_NETINFO_VTM_ID_ERR;
			AmsSendQueryTermNetInfoRsp(pMsg,iret,NULL,NULL);
			return AMS_ERROR;		
		}
		if(    AMS_VTM_REGISTER != AmsRegVtm(vtmRegPos).flag 
			|| vtmId != AmsRegVtm(vtmRegPos).vtmId)
		{
			dbgprint("AmsQueryTermNetInfoReqProc[%d] Vtm[%s]Id[%u][%u]Err-Flag[%d]Pos[%d].",
				pid, vtmNo, vtmId, AmsRegVtm(vtmRegPos).vtmId, 
				AmsRegVtm(vtmRegPos).flag, vtmRegPos);
			iret = AMS_CMS_QUERY_TERM_NETINFO_VTM_ID_ERR;
			AmsSendQueryTermNetInfoRsp(pMsg,iret,NULL,NULL);
			return AMS_ERROR;		
		}
		
		//send VTM Net Info Query Rsp to CMS
		AmsSendQueryTermNetInfoRsp(pMsg,iret,NULL,pVtmNode);
		
	}
	else
	{
		//柜员标识检查		
		tellerId = termId;
		AmsGetTellerNoById(tellerId, tellerNo);	
		
		//查看tellerId是否登录
	    /* find Vta Node in process */
		for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
		{
			pVtaNode = AmsSearchVtaNode(i, tellerId);
			if(NULL != pVtaNode)
			{
				break;
			}
		}
		if(NULL == pVtaNode)
		{
			dbgprint("AmsQueryTermNetInfoReqProc[%d] Teller[%s]Id[%u] not Login.",
				pid, tellerNo, tellerId);
			iret = AMS_CMS_QUERY_TERM_NETINFO_STATE_ERR;
			AmsSendQueryTermNetInfoRsp(pMsg,iret,NULL,NULL);
			return AMS_ERROR;
		} 
		
	    /* check teller in Reg or not */
		tellerRegPos = pVtaNode->tellerRegPos;
		if(tellerRegPos >= AMS_MAX_VTA_NUM)
		{
			dbgprint("AmsQueryTermNetInfoReqProc[%d] Teller[%s][%u]RegPos[%d]Err.",
				pid, tellerNo, tellerId, tellerRegPos);
			iret = AMS_CMS_QUERY_TERM_NETINFO_TELLER_ID_ERR;
			AmsSendQueryTermNetInfoRsp(pMsg,iret,NULL,NULL);
			return AMS_ERROR;		
		}
		if(    AMS_TELLER_REGISTER != AmsRegTeller(tellerRegPos).flag 
			|| tellerId != AmsRegTeller(tellerRegPos).tellerId)
		{
			dbgprint("VtmRegReqProc[%d] Teller[%s]Id[%u][%u]Err-Flag[%d]Pos[%d].",
				pid, tellerNo, tellerId, AmsRegTeller(tellerRegPos).tellerId, 
				AmsRegTeller(tellerRegPos).flag, tellerRegPos);
			iret = AMS_CMS_QUERY_TERM_NETINFO_TELLER_ID_ERR;
			AmsSendQueryTermNetInfoRsp(pMsg,iret,NULL,NULL);
			return AMS_ERROR;		
		}

		//send VTA Net Info Query Rsp to CMS
		AmsSendQueryTermNetInfoRsp(pMsg,iret,pVtaNode,NULL);		
	}

	return iret;
}

int CallEventNoticeProc(int iThreadId, MESSAGE_t *pMsg)
{
	int					iret = AMS_CMS_PRCOESS_SUCCESS;
	LP_AMS_DATA_t		*lpAmsData = NULL;          //进程数据区指针
	LP_QUEUE_DATA_t     *lpQueueData = NULL;        //排队进程数据区指针	
	LP_AMS_DATA_t		*lpOriginAmsData = NULL;    //进程数据区指针	
	VTA_NODE            *pVtaNode = NULL;	
	VTM_NODE            *pVtmNode = NULL;	
	VTA_NODE            *pOriginVtaNode = NULL;			
	int                 pid = 0;
	int                 originPid = 0;		
	unsigned int        amsPid = 0;
	unsigned char       callIdLen = 0;  
	unsigned int        callEventNotice = 0;
	unsigned int        tellerId = 0;
	unsigned char       tellerNo[AMS_MAX_TELLER_NO_LEN + 1] = { 0 };		
	unsigned int        originId = 0;
	unsigned int        vtmId = 0;		
	unsigned char       vtmNo[AMS_MAX_VTM_NO_LEN + 1] = { 0 };	
	unsigned int        originTellerId = 0;	
	unsigned char       originVtaNo[AMS_MAX_TELLER_NO_LEN + 1] = { 0 };			
	unsigned int        newState = 0;
	unsigned int        i = 0;
	unsigned char       *p;
	
#ifdef AMS_TEST_LT
	time_t              currentTime;
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
	//get local pid
	pid = pMsg->s_ReceiverPid.iProcessId;
	if(0 == pid)
	{
		dbgprint("CallEventNoticeProc[%d] Err", pMsg->s_SenderPid.iProcessId);
		iret = AMS_CMS_EVENT_NOTICE_PARA_ERR;
	    AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  			
		return AMS_ERROR;
	}

	//消息长度检查
	if(pMsg->iMessageLength < 15)
	{
		dbgprint("CallEventNoticeProc[%d] Len[%d] Err", pid, pMsg->iMessageLength);
		iret = AMS_CMS_EVENT_NOTICE_LEN_ERR;
	    AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  			
		return AMS_ERROR;
	}


	//流水号检查
	p = pMsg->cMessageBody;
	callIdLen = *p++;
	if(callIdLen > AMS_MAX_CALLID_LEN)
	{
		dbgprint("CallEventNoticeProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_EVENT_NOTICE_CALL_ID_ERR;
		AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  			
		return AMS_ERROR;		
	}
	p += callIdLen;
	
	//获取AMS进程号
	BEGETLONG(amsPid, p);
	p += 4;
	
	//获取事件通知码
	BEGETSHORT(callEventNotice, p);
	p += 2;
	
	//获取柜员工号
	BEGETLONG(tellerId, p);
	AmsGetTellerNoById(tellerId, tellerNo);	
	p += 4;
	
	//获取源标识
	BEGETLONG(originId, p);
	
	if(    CMS_CALL_EVENT_NOTICE_INNER_CALL_ANSWER == callEventNotice 
		|| CMS_CALL_EVENT_NOTICE_INNER_CALL_RELEASE == callEventNotice 
		|| CMS_CALL_EVENT_NOTICE_MONITOR_CALL_ANSWER == callEventNotice 
		|| CMS_CALL_EVENT_NOTICE_MONITOR_CALL_RELEASE == callEventNotice)
	{
		originTellerId = originId;
		if(0 == originTellerId)
		{
			dbgprint("CallEventNoticeProc[%d] Event[%d] OriginTellerId[%u]Err", 
				pid, callEventNotice, originTellerId);
			iret = AMS_CMS_EVENT_NOTICE_ORIGIN_TELLER_ID_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret); 
			return AMS_ERROR;		
		}
		AmsGetTellerNoById(originTellerId, originVtaNo);	
		p += 4;

		//查看origin tellerId是否登录
	    /* find Origin Vta Node in process */
		for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
		{
			pOriginVtaNode = AmsSearchVtaNode(i, originTellerId);
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
		
		//柜员业务组编号检查
		if(lpOriginAmsData->srvGrpId > AMS_SERVICE_GROUP_ID_MAX) 
		{
			dbgprint("CallEventNoticeProc[%d] Event[%d] OriginTeller[%s][%u] SrvGrpId[%u]Err", 
 				pid, callEventNotice, originVtaNo, originTellerId, 
 				lpOriginAmsData->srvGrpId);
			iret = AMS_CMS_EVENT_NOTICE_ORIGIN_TELLER_SERVICE_GROUP_ID_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret); 
			return AMS_ERROR;		
		}
		
	    //柜员业务状态检查
		if(AMS_SERVICE_ACTIVE != AmsSrvData(lpOriginAmsData->srvGrpId).serviceState)
		{
			dbgprint("CallEventNoticeProc[%d] Event[%d] OriginTeller[%s][%d] ServiceState[%d]Err", 
				pid, callEventNotice, originVtaNo, originTellerId, 
				AmsSrvData(lpOriginAmsData->srvGrpId).serviceState);
			iret = AMS_CMS_EVENT_NOTICE_ORIGIN_TELLER_SERVICE_STATE_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret); 
			return AMS_ERROR;			
		}

		//柜员状态检查
		if(pOriginVtaNode->state >= AMS_VTA_STATE_OFFLINE)
		{
			dbgprint("CallEventNoticeProc[%d] Event[%d] OriginTeller[%s][%d] State[%d]Err", 
				pid, callEventNotice, originVtaNo, originTellerId, 
				pOriginVtaNode->state);
			iret = AMS_CMS_EVENT_NOTICE_ORIGIN_TELLER_STATE_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);
			return AMS_ERROR;		
		}

	}
	
	if(    CMS_CALL_EVENT_NOTICE_DEQUEUE == callEventNotice 
		|| CMS_CALL_EVENT_NOTICE_VTM_RELEASE == callEventNotice)
	{
		vtmId = originId;
		AmsGetVtmNoById(vtmId, vtmNo);		
	}
	
	//amsPid检查
	pid = amsPid & 0xffff;
	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		dbgprint("CallEventNoticeProc Event[%d] Pid:%d Err", callEventNotice, pid);
		iret = AMS_CMS_EVENT_NOTICE_AMS_PID_ERR;
	    AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  			
		return AMS_ERROR;
	}	

	//是否是排队中收到VTM挂机事件通知，或者是收到取消排队通知
	if(    CMS_CALL_EVENT_NOTICE_DEQUEUE == callEventNotice 
		|| CMS_CALL_EVENT_NOTICE_VTM_RELEASE == callEventNotice)
	{
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
			dbgprint("CallEventNoticeProc[%d] Event[%d] Vtm[%s]Id[%u]Err.", 
				pid, callEventNotice, vtmNo, vtmId);
			iret = AMS_CMS_EVENT_NOTICE_VTM_ID_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  			
			return AMS_ERROR;	
		}

		if(AMS_CUSTOMER_SERVICE_NULL == pVtmNode->serviceState)
		{
			AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret); 
			return AMS_OK;	
		}
		
		if(pVtmNode->serviceState > AMS_CUSTOMER_IN_SERVICE)
		{
			dbgprint("CallEventNoticeProc[%d] Event[%d] Vtm[%s][%u]State[%d]Err.", 
				pid , callEventNotice, vtmNo, vtmId, 
				pVtmNode->serviceState);
			iret = AMS_CMS_EVENT_NOTICE_VTM_STATE_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  						
			return AMS_ERROR;	
		}

		if(    CMS_CALL_EVENT_NOTICE_DEQUEUE == callEventNotice 
			&& AMS_CUSTOMER_IN_QUEUE != pVtmNode->serviceState)
		{
			dbgprint("CallEventNoticeProc[%d] Event[%d] Vtm[%s][%u]State[%d]Err", 
				pid , callEventNotice, vtmNo, vtmId, 
				pVtmNode->serviceState);			
			iret = AMS_CMS_EVENT_NOTICE_VTM_STATE_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  						
			return AMS_ERROR;	
		}
		
		//排队中收到VTM挂机呼叫事件通知，或收到取消排队通知
		if(AMS_CUSTOMER_IN_QUEUE == pVtmNode->serviceState)
		{
			lpQueueData=(LP_QUEUE_DATA_t *)ProcessData[pid];
		
			//进程号匹配性检查
			if(lpQueueData->myPid.iProcessId != pid)
			{
				dbgprint("CallEventNoticeProc Event[%d] PID[%d][%d]-SendPid[%d] Not Equal.",
					callEventNotice, lpQueueData->myPid.iProcessId, pid, 
					pMsg->s_SenderPid.iProcessId);
				iret = AMS_CMS_EVENT_NOTICE_PARA_ERR;
				AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  			
				return AMS_ERROR;
			}
			
			/* 杀掉定时器 */
			if(lpQueueData->iTimerId >= 0)
			{
				AmsQueueKillTimer(lpQueueData->myPid.iProcessId, &lpQueueData->iTimerId);
				AmsTimerStatProc(T_AMS_CUSTOMER_IN_QUEUE_TIMER, AMS_KILL_TIMER);
			} 
			
			//流水号检查
			if(callIdLen != lpQueueData->callIdLen)
			{
				dbgprint("CallEventNoticeProc[%d] Event[%d] Vtm[%s][%d] CallIdLen[%d]Err.", 
					pid, callEventNotice, lpQueueData->vtmNo, lpQueueData->vtmId, callIdLen);
				iret = AMS_CMS_EVENT_NOTICE_CALL_ID_ERR;
				AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  			
				return AMS_ERROR;		
			}

			p = &pMsg->cMessageBody[1];
			if(0 != memcmp(lpQueueData->callId, p, callIdLen))
			{
				dbgprint("CallEventNoticeProc[%d] Event[%d] Vtm[%s][%d] CallIdErr.", 
					pid, callEventNotice, lpQueueData->vtmNo, lpQueueData->vtmId);
				iret = AMS_CMS_EVENT_NOTICE_CALL_ID_ERR;
				AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  			
				return AMS_ERROR;		
			}
			
			//AMS进程号检查
			if(lpQueueData->myPid.iProcessId != pid) // (amsPid & 0xffff)
			{
				dbgprint("CallEventNoticeProc Event[%d] Vtm[%s][%d] AmsPid[0x%x][0x%x] Err.",
					callEventNotice, lpQueueData->vtmNo, lpQueueData->vtmId, 
					amsPid, lpQueueData->myPid.iProcessId);
				iret = AMS_CMS_EVENT_NOTICE_AMS_PID_ERR;
				AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  			
				return AMS_ERROR;
			}

			if(lpQueueData->vtmId != vtmId || 0 == vtmId)
			{
				dbgprint("CallEventNoticeProc[%d] Event[%d] Vtm[%s]Id[%u][%u]Err.", 
					pid, callEventNotice, lpQueueData->vtmNo, lpQueueData->vtmId, vtmId);
				iret = AMS_CMS_EVENT_NOTICE_VTM_ID_ERR;
			    AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  	
				return AMS_ERROR;	
			}
		
			//update cmsPid
			lpQueueData->cmsPid.cModuleId	= pMsg->s_SenderPid.cModuleId;
			lpQueueData->cmsPid.cFunctionId = pMsg->s_SenderPid.cFunctionId;
			lpQueueData->cmsPid.iProcessId  = pMsg->s_SenderPid.iProcessId;

			//set Vtm State and State Start Time
			AmsSetVtmState(iThreadId, pVtmNode, AMS_VTM_STATE_IDLE);

			if(AMS_CALL_STATE_NULL != pVtmNode->callState)
			{
				//set Vtm Call State and State Start Time
				AmsSetVtmCallState(pVtmNode, AMS_CALL_STATE_NULL);
			}		
			
			//reset vtm serviceState
			AmsSetVtmServiceState(pVtmNode, AMS_CUSTOMER_SERVICE_NULL);		

			//reset amsPid
			pVtmNode->amsPid = 0;

			//reset customerPid
			pVtmNode->customerPid = 0;
			
			//release lpQueueData Pid
			AmsReleassPid(lpQueueData->myPid, END);
	
			AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret); 	

			return iret;
		}

	}

	
	lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
	
    //流水号检查
	p = pMsg->cMessageBody;
	if(callIdLen != lpAmsData->callIdLen)
	{
		dbgprint("CallEventNoticeProc[%d] Event[%d] Teller[%s][%u] Vtm[%s][%u]CallIdLen[%d][%d]Err", 
			pid, callEventNotice, lpAmsData->tellerNo, lpAmsData->tellerId, 
			lpAmsData->vtmNo, lpAmsData->vtmId, 
			callIdLen, lpAmsData->callIdLen); 
		iret = AMS_CMS_EVENT_NOTICE_CALL_ID_ERR;
	    AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  			
		return AMS_ERROR;		
	}

	p++;
	if(0 != memcmp(lpAmsData->callId, p, callIdLen))
	{
		dbgprint("CallEventNoticeProc[%d] Event[%d] Teller[%s][%u] Vtm[%s][%u]CallIdErr", 
			pid, callEventNotice, lpAmsData->tellerNo, lpAmsData->tellerId, 
			lpAmsData->vtmNo, lpAmsData->vtmId); 
		iret = AMS_CMS_EVENT_NOTICE_CALL_ID_ERR;
	    AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  			
		return AMS_ERROR;		
	}
	p += callIdLen;

	//事件通知码检查
	if(    callEventNotice < CMS_CALL_EVENT_NOTICE_VTA_ANSWER 
		|| callEventNotice >= CMS_CALL_EVENT_NOTICE_MAX)
	{
		dbgprint("CallEventNoticeProc[%d] Teller[%s][%u] Vtm[%s][%u] EventCode[%d]Err", 
			pid, lpAmsData->tellerNo, lpAmsData->tellerId, 
			lpAmsData->vtmNo, lpAmsData->vtmId, callEventNotice); 
		iret = AMS_CMS_EVENT_NOTICE_CODE_ERR;
	    AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  			
		return AMS_ERROR;			
	}
	
	//柜员工号检查
    if(    CMS_CALL_EVENT_NOTICE_VTA_ANSWER == callEventNotice 
		|| CMS_CALL_EVENT_NOTICE_VTA_RELEASE == callEventNotice 
		|| CMS_CALL_EVENT_NOTICE_VTA_HOLD == callEventNotice 
		|| CMS_CALL_EVENT_NOTICE_VTA_UNHOLD == callEventNotice 
		|| CMS_CALL_EVENT_NOTICE_INNER_CALL_ANSWER == callEventNotice 
		|| CMS_CALL_EVENT_NOTICE_INNER_CALL_RELEASE == callEventNotice 
		|| CMS_CALL_EVENT_NOTICE_MONITOR_CALL_ANSWER == callEventNotice 
		|| CMS_CALL_EVENT_NOTICE_MONITOR_CALL_RELEASE == callEventNotice)
    {
		if(lpAmsData->tellerId != tellerId || 0 == tellerId)
		{
			dbgprint("CallEventNoticeProc[%d] Event[%d] Teller[%s]Id[%u][%u]Err", 
				pid, callEventNotice, 
				lpAmsData->tellerNo, lpAmsData->tellerId, tellerId);
			iret = AMS_CMS_EVENT_NOTICE_TELLER_ID_ERR;
	    	AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  				
			return AMS_ERROR;	
		}
	}

	//柜员机设备号检查
	if(CMS_CALL_EVENT_NOTICE_VTM_RELEASE == callEventNotice)
    {
		if(lpAmsData->vtmId != vtmId || 0 == vtmId)
		{
			dbgprint("CallEventNoticeProc[%d] Event[%d] Teller[%s][%u] Vtm[%s]Id[%u][%u]Err", 
				pid, callEventNotice, 
				lpAmsData->tellerNo, lpAmsData->tellerId, 
				lpAmsData->vtmNo, lpAmsData->vtmId, vtmId);
			iret = AMS_CMS_EVENT_NOTICE_VTM_ID_ERR;
		    AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  	
			return AMS_ERROR;	
		}
	}

	//业务组编号检查
	if(lpAmsData->srvGrpId > AMS_SERVICE_GROUP_ID_MAX)
	{
		dbgprint("CallEventNoticeProc[%d] Event[%d] Teller[%s][%u] Vtm[%s][%u] SrvGrpId[%u]Err", 
			pid, callEventNotice, 
			lpAmsData->tellerNo, lpAmsData->tellerId, 
			lpAmsData->vtmNo, lpAmsData->vtmId, lpAmsData->srvGrpId);
		iret = AMS_CMS_EVENT_NOTICE_SERVICE_GROUP_ID_ERR;
		AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  	
		return AMS_ERROR;		
	}

    //业务状态检查
	if(AMS_SERVICE_ACTIVE != AmsSrvData(lpAmsData->srvGrpId).serviceState)
	{
		dbgprint("CallEventNoticeProc[%d] Event[%d] Teller[%s][%u] Vtm[%s][%u] ServiceState[%d]Err", 
			pid, callEventNotice, 
			lpAmsData->tellerNo, lpAmsData->tellerId, 
			lpAmsData->vtmNo, lpAmsData->vtmId, 
			AmsSrvData(lpAmsData->srvGrpId).serviceState);
		iret = AMS_CMS_EVENT_NOTICE_SERVICE_STATE_ERR;
		AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  	
		return AMS_ERROR;		
	}
	
	//update Vta State and State Start Time
    if(    CMS_CALL_EVENT_NOTICE_VTA_ANSWER == callEventNotice		
		|| CMS_CALL_EVENT_NOTICE_VTA_RELEASE == callEventNotice 
		|| CMS_CALL_EVENT_NOTICE_VTA_HOLD == callEventNotice 
	    || CMS_CALL_EVENT_NOTICE_VTA_UNHOLD == callEventNotice 
	    || CMS_CALL_EVENT_NOTICE_INNER_CALL_ANSWER == callEventNotice 
	    || CMS_CALL_EVENT_NOTICE_INNER_CALL_RELEASE == callEventNotice 
	    || CMS_CALL_EVENT_NOTICE_MONITOR_CALL_ANSWER == callEventNotice 
	    || CMS_CALL_EVENT_NOTICE_MONITOR_CALL_RELEASE == callEventNotice)
    {
		/* find Vta Node */
	    pVtaNode = AmsSearchVtaNode(lpAmsData->srvGrpId, lpAmsData->tellerId);
		if(NULL == pVtaNode)
		{
			dbgprint("CallEventNoticeProc[%d] Event[%d] Vtm[%s][%u] Teller[%s]Id[%u]Err", 
				pid, callEventNotice, 
				lpAmsData->vtmNo, lpAmsData->vtmId,
				lpAmsData->tellerNo, lpAmsData->tellerId);	
			iret = AMS_CMS_EVENT_NOTICE_TELLER_ID_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  	
			return AMS_ERROR;		
		}

		//柜员状态检查
		if(    pVtaNode->state >= AMS_VTA_STATE_OFFLINE 
			&& CMS_CALL_EVENT_NOTICE_VTA_RELEASE != callEventNotice)
		{
			dbgprint("CallEventNoticeProc[%d] Event[%d] Vtm[%s][%u]Teller[%s][%u]State[%d]Err", 
				pid, callEventNotice, 
				lpAmsData->vtmNo, lpAmsData->vtmId, 
				lpAmsData->tellerNo, lpAmsData->tellerId, pVtaNode->state);		
			iret = AMS_CMS_EVENT_NOTICE_VTA_STATE_ERR;
	    	AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  				
			return AMS_ERROR;		
		}

		//update vta callState,not check vta state and callState
		if(CMS_CALL_EVENT_NOTICE_VTA_ANSWER == callEventNotice)
		{
			newState = AMS_CALL_STATE_ANSWER;

			AmsTellerStatProc(lpAmsData->tellerId, lpAmsData->tellerPos, AMS_TELLER_CALL_CONNECT, iret);
			pVtaNode->vtaWorkInfo.connectNum++;

			//reset callTransferNum
			pVtaNode->callTransferNum = 0;			
		}
		if(CMS_CALL_EVENT_NOTICE_VTA_RELEASE == callEventNotice)
		{
			//reset vta call state
			newState = AMS_CALL_STATE_NULL;

			//杀掉定时器
//			AmsKillVtaAllTimer(lpAmsData, pid);

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
			AmsInsertDbServiceSDR(iThreadId, AMS_SDR_ITEM_BASE, lpAmsData, NULL, 0, 0, NULL);

			//reset sessStat
			memset(&lpAmsData->sessStat, 0, sizeof(AMS_SESSION_STAT));	
			
		}
		if(CMS_CALL_EVENT_NOTICE_VTA_HOLD == callEventNotice)
		{
			newState = AMS_CALL_STATE_HOLD;

			//ignore vtm call hold
		}
		if(CMS_CALL_EVENT_NOTICE_VTA_UNHOLD == callEventNotice)
		{
			newState = AMS_CALL_STATE_ANSWER;

			//ignore vtm call unhold			
		}
		if(CMS_CALL_EVENT_NOTICE_INNER_CALL_ANSWER == callEventNotice)
		{
			newState = AMS_CALL_STATE_INNER_CALL;
			
			//Update Origin TellerState
			if(AMS_CALL_STATE_HOLD == pOriginVtaNode->callState)//pOriginVtaNode has been checked before
			{
				AmsSetVtaCallState(lpOriginAmsData, pOriginVtaNode, AMS_CALL_STATE_INNER_CALL);
			}			
		}
		if(CMS_CALL_EVENT_NOTICE_INNER_CALL_RELEASE == callEventNotice)
		{
			newState = AMS_CALL_STATE_NULL;

			//Update Origin TellerState
			if(AMS_CALL_STATE_INNER_CALL == pOriginVtaNode->callState)//pOriginVtaNode has been checked before
			{
				AmsSetVtaCallState(lpOriginAmsData, pOriginVtaNode, AMS_CALL_STATE_HOLD);
			}
		}

		if(CMS_CALL_EVENT_NOTICE_MONITOR_CALL_ANSWER == callEventNotice)
		{
			newState = AMS_CALL_STATE_MONITOR_CALL;
			
			//Update Origin TellerState
			if(AMS_CALL_STATE_ANSWER == pOriginVtaNode->callState)//pOriginVtaNode has been checked before
			{
				AmsSetVtaCallState(lpOriginAmsData, pOriginVtaNode, AMS_CALL_STATE_MONITOR_CALL);
			}	
		}
		if(CMS_CALL_EVENT_NOTICE_MONITOR_CALL_RELEASE == callEventNotice)
		{
			newState = AMS_CALL_STATE_NULL;

			//Update Origin TellerState
			if(AMS_CALL_STATE_MONITOR_CALL == pOriginVtaNode->callState)//pOriginVtaNode has been checked before
			{
				AmsSetVtaCallState(lpOriginAmsData, pOriginVtaNode, AMS_CALL_STATE_ANSWER);
			}
		}
		
		AmsSetVtaCallState(lpAmsData, pVtaNode, newState);
		
    }
	
	//update Vtm State and State Start Time
	if(    CMS_CALL_EVENT_NOTICE_VTA_ANSWER == callEventNotice 
		|| CMS_CALL_EVENT_NOTICE_VTM_RELEASE == callEventNotice)
    {
        pVtmNode = AmsSearchVtmNode(lpAmsData->srvGrpId, lpAmsData->vtmId);
		if(NULL == pVtmNode)
		{
			dbgprint("CallEventNoticeProc[%d] Event[%d] Teller[%s][%u] Vtm[%s]Id[%u]Err", 
				pid, callEventNotice, 
				lpAmsData->tellerNo, lpAmsData->tellerId, 
				lpAmsData->vtmNo, lpAmsData->vtmId);		
			iret = AMS_CMS_EVENT_NOTICE_VTM_ID_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  	
			return AMS_ERROR;		
		}

		//柜员机状态检查
		if(    pVtmNode->state >= AMS_VTA_STATE_OFFLINE 
			&& CMS_CALL_EVENT_NOTICE_VTA_RELEASE != callEventNotice)
		{
			dbgprint("CallEventNoticeProc[%d] Event[%d] Teller[%s][%u] Vtm[%s]Id[%u]State[%d]Err", 
				pid, callEventNotice, 
				lpAmsData->tellerNo, lpAmsData->tellerId, 
				lpAmsData->vtmNo, lpAmsData->vtmId, 
				pVtmNode->state);
			iret = AMS_CMS_EVENT_NOTICE_VTM_STATE_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  				
			return AMS_ERROR;		
		}

		//update vtm call state,not check vtm state and callState
		if(CMS_CALL_EVENT_NOTICE_VTA_ANSWER == callEventNotice)//即CMS_CALL_EVENT_NOTICE_VTM_ANSWER
		{
			newState = AMS_CALL_STATE_ANSWER;
		}

		if(CMS_CALL_EVENT_NOTICE_VTM_RELEASE == callEventNotice)
		{
			//reset vtm call state
			newState = AMS_CALL_STATE_NULL;

			//杀掉定时器，包括消息、文件收发
			AmsKillVtmAllCallTimer(pVtmNode, pid);

			//reset vtm serviceState
			AmsSetVtmServiceState(pVtmNode, AMS_CUSTOMER_SERVICE_NULL);

			//set Vtm State and State Start Time
			AmsSetVtmState(iThreadId, pVtmNode, AMS_VTM_STATE_IDLE);

			//reset amsPid
			pVtmNode->amsPid = 0;

			//reset customerPid
			pVtmNode->customerPid = 0;
		}

		AmsSetVtmCallState(pVtmNode, newState);

    }
	
	//update cmsPid
	lpAmsData->cmsPid.cModuleId	  = pMsg->s_SenderPid.cModuleId;
	lpAmsData->cmsPid.cFunctionId = pMsg->s_SenderPid.cFunctionId;
	lpAmsData->cmsPid.iProcessId  = pMsg->s_SenderPid.iProcessId;

	AmsResultStatProc(AMS_CMS_EVENT_NOTICE_RESULT, iret);  	

	return iret;
}

int CallEventIndCnfProc(int iThreadId, MESSAGE_t *pMsg)
{
	int					iret = AMS_CMS_PRCOESS_SUCCESS;
	LP_AMS_DATA_t		*lpAmsData = NULL;               //进程数据区指针
	LP_QUEUE_DATA_t     *lpQueueData = NULL;             //排队进程数据区指针
	VTA_NODE            *pVtaNode = NULL;
	VTM_NODE            *pVtmNode = NULL;
	int                 pid = 0;
	PID_t               cmsSendPid;	
	unsigned int        amsPid = 0;
	unsigned char       callIdLen = 0;  
	unsigned int        callEventInd = 0;
	unsigned char       *p;

	if(AmsMsgTrace)
	{
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"recv A_AMS_CALL_EVENT_IND_CNF msg \n");	
		AmsTraceToFile(pMsg->s_ReceiverPid,pMsg->s_SenderPid,"A_AMS_CALL_EVENT_IND_CNF",description,
						descrlen,pMsg->cMessageBody,pMsg->iMessageLength,"ams");		
	}
	
	//进程号有效性检查
	//get local pid
	pid = pMsg->s_ReceiverPid.iProcessId;
	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		dbgprint("CallEventIndCnfProc Pid:%d Err", pid);
		iret = AMS_CMS_EVENT_IND_CNF_PARA_ERR;
		AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);  			
		return AMS_ERROR;
	}

	//消息长度检查
	if(pMsg->iMessageLength < 11)
	{
		dbgprint("CallEventIndCnfProc[%d] Len:%d Err", pid, pMsg->iMessageLength);
		iret = AMS_CMS_EVENT_IND_CNF_LEN_ERR;
		AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);  			
		return AMS_ERROR;
	}

	//流水号检查
	p = pMsg->cMessageBody;
	callIdLen = *p++;
	if(callIdLen > AMS_MAX_CALLID_LEN)
	{
		dbgprint("CallEventIndCnfProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_EVENT_IND_CNF_CALL_ID_ERR;
		AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);  			
		return AMS_ERROR;		
	}
	p += callIdLen;
	
	//获取AMS进程号
	BEGETLONG(amsPid, p);
	p += 4;	
	
	//获取事件指示码
	BEGETSHORT(callEventInd, p);
	p += 2;	
	
	//获取事件指示结果
	BEGETLONG(iret, p); //原因值统一编码

	if(AMS_CMS_PRCOESS_SUCCESS != iret)
	{
		dbgprint("CallEventIndCnfProc[%d] Cms ReportFail[0x%x]", pid, iret);

		if(iret < AMS_CMS_REASON_BASE || iret >= AMS_RCAS_REASON_BASE)
		{
			iret = AMS_CMS_EVENT_IND_CNF_CMS_REPORT_FAIL; //转换了失败原因值
		}
	}

	if(    CMS_CALL_EVENT_IND_VTM_OFFLINE_DEQUEUE == callEventInd
		|| CMS_CALL_EVENT_IND_VTM_FORCE_LOGIN_DEQUEUE == callEventInd
		|| CMS_CALL_EVENT_IND_VTM_LOGOUT_DEQUEUE == callEventInd)
	{
		lpQueueData=(LP_QUEUE_DATA_t *)ProcessData[pid];

		//timer has been killed, lpQueueDataPid and VtmNode has been freed!
		if(    CMS_CALL_EVENT_IND_VTM_OFFLINE_DEQUEUE == callEventInd
			|| CMS_CALL_EVENT_IND_VTM_FORCE_LOGIN_DEQUEUE == callEventInd
			|| CMS_CALL_EVENT_IND_VTM_LOGOUT_DEQUEUE == callEventInd)
		{
			AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);
			return AMS_OK;	
		}

		//not used

		//进程号匹配性检查
		if(lpQueueData->myPid.iProcessId != pid)
		{
			dbgprint("CallEventIndCnfProc PID[%d][%d] Not Equal.", 
				lpQueueData->myPid.iProcessId, pid);
			iret = AMS_CMS_EVENT_IND_CNF_PARA_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);  			
			return AMS_ERROR;
		}
		
		/* 杀掉定时器 */
		if(lpQueueData->iTimerId >= 0)
		{
			AmsQueueKillTimer(lpQueueData->myPid.iProcessId, &lpQueueData->iTimerId);
			AmsTimerStatProc(T_AMS_CUSTOMER_IN_QUEUE_TIMER, AMS_KILL_TIMER);
		} 
				
		//流水号检查
		if(callIdLen != lpQueueData->callIdLen)
		{
			dbgprint("CallEventIndCnfProc[%d] Vtm[%s][%u]CallIdLen[%d]Err.", 
				pid, lpQueueData->vtmNo, lpQueueData->vtmId, callIdLen);
			iret = AMS_CMS_EVENT_IND_CNF_CALL_ID_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);  			
			return AMS_ERROR;		
		}

		p = &pMsg->cMessageBody[1];
		if(0 != memcmp(lpQueueData->callId, p, callIdLen))
		{
			dbgprint("CallEventIndCnfProc[%d] Vtm[%s][%u]CallIdErr.", 
				pid, lpQueueData->vtmNo, lpQueueData->vtmId);
			iret = AMS_CMS_EVENT_IND_CNF_CALL_ID_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);  			
			return AMS_ERROR;		
		}
		
		//AMS进程号检查
		if(lpQueueData->myPid.iProcessId != (amsPid & 0xffff))
		{
			dbgprint("CallEventIndCnfProc[%d] Vtm[%s][%u] AmsPid[0x%x][0x%x] Err.",
				pid, lpQueueData->vtmNo, lpQueueData->vtmId, 
				amsPid, lpQueueData->myPid.iProcessId);
			iret = AMS_CMS_EVENT_IND_CNF_AMS_PID_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);  			
			return AMS_ERROR;
		}
		
		//update cmsPid
		lpQueueData->cmsPid.cModuleId	= pMsg->s_SenderPid.cModuleId;
		lpQueueData->cmsPid.cFunctionId = pMsg->s_SenderPid.cFunctionId;
		lpQueueData->cmsPid.iProcessId  = pMsg->s_SenderPid.iProcessId;

		AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret); 	
		
	}
	else
	{
		lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
		
		//查看VTA是否有效
		if(lpAmsData->srvGrpId <= AMS_SERVICE_GROUP_ID_MAX &&
		   lpAmsData->tellerId != 0) 
		{
			/* find Vta Node */
		    pVtaNode = AmsSearchVtaNode(lpAmsData->srvGrpId, lpAmsData->tellerId);
		}

		//timer has been killed, CallState has been reset,lpAmsDataPid and VtaNode has been freed!
		if(    CMS_CALL_EVENT_IND_VTA_OFFLINE == callEventInd 
			|| CMS_CALL_EVENT_IND_VTA_FORCE_LOGIN_OFFLINE == callEventInd
		    || CMS_CALL_EVENT_IND_VTA_LOGOUT_OFFLINE == callEventInd
		    || CMS_CALL_EVENT_IND_VTA_CALL_STATE_ABORT == callEventInd
		    || CMS_CALL_EVENT_IND_MNG_REL_VTA == callEventInd 
		    || CMS_CALL_EVENT_IND_MNG_FORCE_LOGOUT_VTA == callEventInd 
		    || CMS_CALL_EVENT_IND_MNG_DEL_VTA == callEventInd )
		{
			AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);
			return AMS_OK;	
		}

		//Call timer has been killed, CallState has been reset
		if(CMS_CALL_EVENT_IND_MNG_FORCE_REL_CALL_VTA == callEventInd)
		{
			AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);
			return AMS_OK;	
		}
		
		//timer has been killed, CallState has been reset,VtmNode has been freed!
		if(    CMS_CALL_EVENT_IND_VTM_OFFLINE == callEventInd
			|| CMS_CALL_EVENT_IND_VTM_FORCE_LOGIN_OFFLINE == callEventInd
			|| CMS_CALL_EVENT_IND_VTM_LOGOUT_OFFLINE)
		{
			AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);
			return AMS_OK;	
		}
		
		//进程号匹配性检查
		if(lpAmsData->myPid.iProcessId != pid)
		{
			dbgprint("CallEventIndCnfProc EventInd[%d] PID[%d][%d] Not Equal", 
				callEventInd, lpAmsData->myPid.iProcessId, pid);
			iret = AMS_CMS_EVENT_IND_CNF_PARA_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);  			
			return AMS_ERROR;
		}
			
		/* 杀掉定时器 */
		if(lpAmsData->callTimerId >= 0)
		{
		    AmsKillTimer(pid, &lpAmsData->callTimerId);
			AmsTimerStatProc(T_AMS_CALL_EVENT_IND_TIMER, AMS_KILL_TIMER);
		} 
		
		//get Pid
		memset(&cmsSendPid,0,sizeof(PID_t));
		memcpy(&cmsSendPid,&pMsg->s_SenderPid,sizeof(PID_t));	   //cmsPid
		memcpy(&pMsg->s_SenderPid,&lpAmsData->rPid,sizeof(PID_t)); //vtaPid	
		
		//流水号检查
		if(callIdLen != lpAmsData->callIdLen)
		{
			dbgprint("CallEventIndCnfProc[%d] EventInd[%d] Teller[%s][%u] Vtm[%s][%u]CallIdLen[%d][%d]Err", 
				pid, callEventInd, lpAmsData->tellerNo, lpAmsData->tellerId, 
				lpAmsData->vtmNo, lpAmsData->vtmId, 
				callIdLen, lpAmsData->callIdLen);
			iret = AMS_CMS_EVENT_IND_CNF_CALL_ID_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);  

			if(CMS_CALL_EVENT_IND_CALL_TRANSFER == callEventInd)
			{	
				if(NULL != pVtaNode)
				{
					if(AMS_CALL_STATE_NULL != pVtaNode->callState)
					{
						//Reset Vta Call State and State Start Time 
						AmsSetVtaCallState(lpAmsData, pVtaNode, AMS_CALL_STATE_NULL);
					}
				}
					
				//send Transfer Call Rsp to Vta
				iret = AMS_VTA_TRANSFER_CALL_LEN_ERR;
				AmsSendVtaTransferCallRsp(lpAmsData,pMsg,iret);		
			}	
			
			return AMS_ERROR;		
		}

		p = &pMsg->cMessageBody[1];
		if(0 != memcmp(lpAmsData->callId, p, callIdLen))
		{
			dbgprint("CallEventIndCnfProc[%d] EventInd[%d] Teller[%s][%u]Vtm[%s][%u]CallIdErr", 
				pid, callEventInd, lpAmsData->tellerNo, lpAmsData->tellerId, 
				lpAmsData->vtmNo, lpAmsData->vtmId);
			iret = AMS_CMS_EVENT_IND_CNF_CALL_ID_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret); 
			
			if(CMS_CALL_EVENT_IND_CALL_TRANSFER == callEventInd)
			{			
				if(NULL != pVtaNode)
				{
					if(AMS_CALL_STATE_NULL != pVtaNode->callState)
					{
						//Reset Vta Call State and State Start Time 
						AmsSetVtaCallState(lpAmsData, pVtaNode, AMS_CALL_STATE_NULL);
					}
				}
				
				//send Transfer Call Rsp to Vta
				iret = AMS_VTA_TRANSFER_CALL_CALLID_ERR;
				AmsSendVtaTransferCallRsp(lpAmsData,pMsg,iret);		
			}	
						
			return AMS_ERROR;		
		}
		
		//AMS进程号检查
		if(lpAmsData->amsPid != amsPid)
		{
			dbgprint("CallEventIndCnfProc[%d] EventInd[%d] Teller[%s][%u] Vtm[%s][%u] AmsPid[0x%x][0x%x] Err",
				pid, callEventInd, lpAmsData->tellerNo, lpAmsData->tellerId, 
				lpAmsData->vtmNo, lpAmsData->vtmId, 
				amsPid, lpAmsData->amsPid);
			iret = AMS_CMS_EVENT_IND_CNF_AMS_PID_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);  

			if(CMS_CALL_EVENT_IND_CALL_TRANSFER == callEventInd)
			{			
				if(NULL != pVtaNode)
				{
					if(AMS_CALL_STATE_NULL != pVtaNode->callState)
					{
						//Reset Vta Call State and State Start Time 
						AmsSetVtaCallState(lpAmsData, pVtaNode, AMS_CALL_STATE_NULL);
					}
				}
				
				//send Transfer Call Rsp to Vta
				iret = AMS_VTA_TRANSFER_CALL_AMS_PID_ERR;
				AmsSendVtaTransferCallRsp(lpAmsData,pMsg,iret);		
			}
			
			return AMS_ERROR;
		}
		
		//事件指示码检查
		if(callEventInd < CMS_CALL_EVENT_IND_VTA_OFFLINE || callEventInd >= CMS_CALL_EVENT_IND_MAX)
		{
			dbgprint("CallEventIndCnfProc[%d] Teller[%s][%u] Vtm[%s][%u] CallEventInd[%d]Err", 
				pid, lpAmsData->tellerNo, lpAmsData->tellerId, 
				lpAmsData->vtmNo, lpAmsData->vtmId, callEventInd);
			iret = AMS_CMS_EVENT_IND_CNF_CODE_ERR;
			AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);  				
			
			return AMS_ERROR;	
		}

		//update cmsPid
		lpAmsData->cmsPid.cModuleId	  = cmsSendPid.cModuleId;
		lpAmsData->cmsPid.cFunctionId = cmsSendPid.cFunctionId;
		lpAmsData->cmsPid.iProcessId  = cmsSendPid.iProcessId;

		if(CMS_CALL_EVENT_IND_CALL_TRANSFER == callEventInd)
		{			
			if(NULL != pVtaNode)
			{
				if(AMS_CALL_STATE_NULL != pVtaNode->callState)
				{
					//Reset Vta Call State and State Start Time 
					AmsSetVtaCallState(lpAmsData, pVtaNode, AMS_CALL_STATE_NULL);
				}
			}
			
			//send Transfer Call Rsp to Vta
			AmsSendVtaTransferCallRsp(lpAmsData,pMsg,iret);		
		}
		
		//Other...
		
		//cms event ind result
		AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret); 
		
	}
	
	return iret;
}


int AmsSendCmsVtaRegRsp(TELLER_REGISTER_INFO *tellerRegisterInfo,MESSAGE_t *pMsg,int iret)
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
	s_Msg.iMessageType = A_VTA_REG_RSP;
	s_Msg.iMessageLength = 0;
		
    p = &s_Msg.cMessageBody[0];
	if(tellerRegisterInfo != NULL)
	{
		BEPUTLONG(tellerRegisterInfo->tellerId, p);
	}
	else
	{
		memcpy(p, pMsg->cMessageBody, 4);
	}
	p += 4;
	
	BEPUTLONG(iret, p);

	s_Msg.iMessageLength = 8;
	
	SendMsgBuff(&s_Msg,0);

	if(AmsMsgTrace)
	{	
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"send A_VTA_REG_RSP msg \n");	
		AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_VTA_REG_RSP",description,
			descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,"ams");	
	}

	AmsMsgStatProc(AMS_CMS_MSG, s_Msg.iMessageType);
	AmsResultStatProc(AMS_CMS_VTA_REG_RESULT, iret);
	
	return SUCCESS;
}

int AmsSendCmsVtmRegRsp(VTM_REGISTER_INFO *vtmRegisterInfo,MESSAGE_t *pMsg,int iret)
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
	s_Msg.iMessageType = A_VTM_REG_RSP;
	s_Msg.iMessageLength = 0;
		
    p = &s_Msg.cMessageBody[0];
	if(vtmRegisterInfo != NULL)
	{
		BEPUTLONG(vtmRegisterInfo->vtmId, p);
	}
	else
	{
		memcpy(p, pMsg->cMessageBody, 4);
	}
	p += 4;
	
	BEPUTLONG(iret, p);

	s_Msg.iMessageLength = 8;
	
	SendMsgBuff(&s_Msg,0);

	if(AmsMsgTrace)
	{	
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"send A_VTM_REG_RSP msg \n");	
		AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_VTM_REG_RSP",description,
			descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,"ams");	
	}

	AmsMsgStatProc(AMS_CMS_MSG, s_Msg.iMessageType);
	AmsResultStatProc(AMS_CMS_VTM_REG_RESULT, iret);
	
	return SUCCESS;
}

int AmsSendCmsVtaGetRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret,VTA_NODE *pVtaNode,VTM_NODE *pVtmNode)
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
	
	if(NULL != lpAmsData)
	{
		s_Msg.s_SenderPid.iProcessId = lpAmsData->myPid.iProcessId;
		
	}
	else if(NULL != pVtmNode)
	{
		s_Msg.s_SenderPid.iProcessId = pVtmNode->customerPid&0xffff;
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
			
			BEPUTLONG(pVtaNode->vtaInfo.tellerId, p);	
			p += 4;

			s_Msg.iMessageLength += 8;

			//pack TellerNo
			if(pVtaNode->vtaInfo.tellerNoLen > AMS_MAX_TELLER_NO_LEN)
			{
				pVtaNode->vtaInfo.tellerNoLen = AMS_MAX_TELLER_NO_LEN;
			}
			*p++ = pVtaNode->vtaInfo.tellerNoLen;
			memcpy(p, pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerNoLen);
			p += pVtaNode->vtaInfo.tellerNoLen;

			s_Msg.iMessageLength += (1 + pVtaNode->vtaInfo.tellerNoLen);
			
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
		if(NULL != pVtmNode)
		{
			BEPUTLONG(pVtmNode->customerPid, p);
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

int AmsSendQueryTermNetInfoRsp(MESSAGE_t *pMsg,int iret,VTA_NODE *pVtaNode,VTM_NODE *pVtmNode)
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
	
	s_Msg.iMessageType = A_TERM_NETINFO_QUERY_RSP;
	s_Msg.iMessageLength = 0;

    p = &s_Msg.cMessageBody[0];

	memcpy(p, pMsg->cMessageBody, 8);//termType termId
	p += 8;
	
	BEPUTLONG(iret, p);
	p += 4;
	
	s_Msg.iMessageLength += 12;
	
    if(AMS_VTA_QUEUE_MNG_SUCCESS == iret)
    {
		if(NULL != pVtaNode && NULL == pVtmNode)
		{
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
		else if(NULL != pVtmNode && NULL == pVtaNode)
		{
			//Opart
			*p++ = AMS_TERM_NET_INFO_ID;

			BEPUTSHORT(6, p);	
			p += 2;

			BEPUTLONG(pVtmNode->vtmInfo.vtmIp, p);	
			p += 4;

			BEPUTSHORT(pVtmNode->vtmInfo.vtmPort, p);	

			//add Opart Len			
			s_Msg.iMessageLength += 9;
		}
    }	
	
	SendMsgBuff(&s_Msg,0);

	if(AmsMsgTrace)
	{	
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"send A_TERM_NETINFO_QUERY_RSP msg \n");	
		AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_TERM_NETINFO_QUERY_RSP",description,
			descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,"ams");
	}

	AmsMsgStatProc(AMS_CMS_MSG, s_Msg.iMessageType);
	AmsResultStatProc(AMS_CMS_QUERY_TERM_NETINFO_RESULT, iret);
	
	return SUCCESS;
}

int AmsSendCmsCallEventInd(LP_AMS_DATA_t *lpAmsData,unsigned short callEventInd,CALL_TARGET *pCallTarget)
{
	MESSAGE_t           s_Msg;
	unsigned short      callTargetLen = 0;
	unsigned char       *p;
	
	memset(&s_Msg,0,sizeof(MESSAGE_t));

	if(NULL == lpAmsData)
	{
		return AMS_ERROR;
	}
	
	s_Msg.eMessageAreaId = A;
	
	s_Msg.s_ReceiverPid.cModuleId   = lpAmsData->cmsPid.cModuleId;  
	s_Msg.s_ReceiverPid.cFunctionId = FID_CMS;
	s_Msg.s_ReceiverPid.iProcessId  = lpAmsData->cmsPid.iProcessId; 
	
	s_Msg.s_SenderPid.cModuleId     = SystemData.cMid;
	s_Msg.s_SenderPid.cFunctionId   = FID_AMS;
	s_Msg.s_SenderPid.iProcessId    = lpAmsData->myPid.iProcessId;
	s_Msg.iMessageType = A_AMS_CALL_EVENT_IND;
	s_Msg.iMessageLength = 0;

    p = &s_Msg.cMessageBody[0];

	//Pack Call Id
	if(lpAmsData->callIdLen > AMS_MAX_CALLID_LEN)
	{
		lpAmsData->callIdLen = AMS_MAX_CALLID_LEN;
	}
    *p++ = lpAmsData->callIdLen;
    memcpy(p, lpAmsData->callId, lpAmsData->callIdLen);
	p += lpAmsData->callIdLen;
	
	BEPUTLONG(lpAmsData->amsPid, p);
	p += 4;
	
	BEPUTSHORT(callEventInd, p);
	p += 2;

	s_Msg.iMessageLength += (1 + lpAmsData->callIdLen + 4 + 2);	
	
	if(CMS_CALL_EVENT_IND_CALL_TRANSFER == callEventInd && pCallTarget != NULL)
	{
		//Pack Call Target
	    *p++ = AMS_CALL_TARGET_ID;
		callTargetLen = 8;
		BEPUTSHORT(callTargetLen, p);
		p += 2;

		BEPUTSHORT(pCallTarget->callTargetType, p);
		p += 2;

		BEPUTSHORT(pCallTarget->targetTellerGroupId, p);
		p += 2;

		BEPUTLONG(pCallTarget->targetTellerId, p);
		p += 4;
	
	    s_Msg.iMessageLength += (3 + callTargetLen);		
	}
	
	SendMsgBuff(&s_Msg,0);

	if(AmsMsgTrace)
	{	
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"send A_AMS_CALL_EVENT_IND msg \n");	
		AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_AMS_CALL_EVENT_IND",description,
			           descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,lpAmsData->sTraceName);				
	}

	AmsMsgStatProc(AMS_CMS_MSG, s_Msg.iMessageType);

	return SUCCESS;
}


//Only used customer in queue
int AmsSendCmsCallQueueEventInd(LP_QUEUE_DATA_t *lpQueueData,unsigned short callEventInd,CALL_TARGET *pCallTarget)
{
	MESSAGE_t           s_Msg;
	unsigned short      callTargetLen = 0;
	unsigned short      queueRuleLen = 0;
	DWORD               amsTempPid = 0;
	unsigned char       *p;
	
	memset(&s_Msg,0,sizeof(MESSAGE_t));

	if(NULL == lpQueueData)
	{
		return AMS_ERROR;
	}
	
	s_Msg.eMessageAreaId = A;
	
	s_Msg.s_ReceiverPid.cModuleId   = lpQueueData->cmsPid.cModuleId;  
	s_Msg.s_ReceiverPid.cFunctionId = FID_CMS;
	s_Msg.s_ReceiverPid.iProcessId  = lpQueueData->cmsPid.iProcessId; 
	
	s_Msg.s_SenderPid.cModuleId     = SystemData.cMid;
	s_Msg.s_SenderPid.cFunctionId   = FID_AMS;
	s_Msg.s_SenderPid.iProcessId    = lpQueueData->myPid.iProcessId;
	s_Msg.iMessageType = A_AMS_CALL_EVENT_IND;
	s_Msg.iMessageLength = 0;

    p = &s_Msg.cMessageBody[0];

	//Pack Call Id
	if(lpQueueData->callIdLen > AMS_MAX_CALLID_LEN)
	{
		lpQueueData->callIdLen = AMS_MAX_CALLID_LEN;
	}
    *p++ = lpQueueData->callIdLen;
    memcpy(p, lpQueueData->callId, lpQueueData->callIdLen);
	p += lpQueueData->callIdLen;
	
	amsTempPid = ((((unsigned int)s_Msg.s_SenderPid.cModuleId) << 24)&0xff000000) | 
	               ((((unsigned int)s_Msg.s_SenderPid.cFunctionId) << 16)&0x00ff0000) |
	               (((unsigned int)s_Msg.s_SenderPid.iProcessId)&0x0000ffff);	
	BEPUTLONG(amsTempPid, p);
	p += 4;
	
	BEPUTSHORT(callEventInd, p);
	p += 2;

	s_Msg.iMessageLength += (1 + lpQueueData->callIdLen + 4 + 2);	

	SendMsgBuff(&s_Msg,0);

	if(AmsMsgTrace)
	{	
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"send A_AMS_CALL_EVENT_IND msg\n");	
		AmsTraceToFile(s_Msg.s_ReceiverPid,s_Msg.s_SenderPid,"A_AMS_CALL_EVENT_IND",description,
			           descrlen,s_Msg.cMessageBody,s_Msg.iMessageLength,lpQueueData->sTraceName);	
	}

	//other not need result stat

	AmsMsgStatProc(AMS_CMS_MSG, s_Msg.iMessageType);

	return SUCCESS;
}


//Onle Handle TransferCall/VolumeCtrl/ModifyQueueRule Timeout
//not include AmsClearInactiveVta/AmsClearInactiveVtm, because the timer is killed
int AmsCallEventIndTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg)
{
	int					iret = AMS_CMS_PRCOESS_SUCCESS;
	LP_AMS_DATA_t		*lpAmsData = NULL;               //进程数据区指针
	VTA_NODE            *pVtaNode = NULL;		
	int                 pid = 0;
	unsigned int        callEventInd = 0;
	unsigned short      volumeCtrlType = 0;	
	MESSAGE_t           msg;
	unsigned char       *p;

	if(AmsMsgTrace)
	{
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"recv B_AMS_CALL_EVENT_IND_TIMEOUT msg[%d] \n",pTmMsg->iTimerId);	
		AmsTraceToFile(pTmMsg->s_ReceiverPid,pTmMsg->s_SenderPid,"B_AMS_CALL_EVENT_IND_TIMEOUT",description,
						descrlen,pTmMsg->cTimerParameter,PARA_LEN,"ams");
	}
		
	//进程号有效性检查
	//get local pid	
	pid = pTmMsg->s_ReceiverPid.iProcessId;
	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		dbgprint("AmsCallEventIndTimeoutProc Pid:%d Err", pid);
		iret = AMS_CMS_EVENT_IND_TIMEOUT_PARA_ERR;
	    AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);  		
		return AMS_ERROR;
	}

	//消息长度检查
	if(pTmMsg->iMessageLength > (PARA_LEN + sizeof(char) + sizeof(int)))
	{
		dbgprint("AmsCallEventIndTimeoutProc[%d] Len[%d] Err", pid, pTmMsg->iMessageLength);
		iret = AMS_CMS_EVENT_IND_TIMEOUT_LEN_ERR;
	    AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);  			
		return AMS_ERROR;
	}

	lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];

	/* 杀掉定时器 */
	if(lpAmsData->callTimerId >= 0)
	{
	    AmsKillTimer(pid, &lpAmsData->callTimerId);
		AmsTimerStatProc(T_AMS_CALL_EVENT_IND_TIMER, AMS_KILL_TIMER);
		pTmMsg->iTimerId = -1;		
	} 

	//进程号匹配性检查
	if(lpAmsData->myPid.iProcessId != pid)
	{
		dbgprint("AmsCallEventIndTimeoutProc Teller[%s][%u] Vtm[%s][%u] PID[%d][%d] Not Equal", 
			lpAmsData->tellerNo, lpAmsData->tellerId, 
			lpAmsData->vtmNo, lpAmsData->vtmId, 
			lpAmsData->myPid.iProcessId, pid);
		iret = AMS_CMS_EVENT_IND_TIMEOUT_PARA_ERR;
	    AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);  			
		return AMS_ERROR;
	}
	
	//fill vta Pid
	memset(&msg, 0, sizeof(MESSAGE_t));
	memcpy(&msg.s_SenderPid, &lpAmsData->rPid, sizeof(PID_t));
	msg.s_ReceiverPid.iProcessId = lpAmsData->myPid.iProcessId;
	
	//呼叫事件指示码检查
	p = pTmMsg->cTimerParameter;
	BEGETSHORT(callEventInd, p);
	if(callEventInd < CMS_CALL_EVENT_IND_VTA_OFFLINE || callEventInd >= CMS_CALL_EVENT_IND_MAX)
	{
		dbgprint("AmsCallEventIndTimeoutProc[%d] Teller[%s][%u] Vtm[%s][%u] CallEventInd[%d]Err",
			pid, lpAmsData->tellerNo, lpAmsData->tellerId, 
			lpAmsData->vtmNo, lpAmsData->vtmId, 
			callEventInd);
		iret = AMS_CMS_EVENT_IND_TIMEOUT_CODE_ERR;
	    AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);  			
		return AMS_ERROR;	
	}

	//业务组编号检查
	if(lpAmsData->srvGrpId > AMS_SERVICE_GROUP_ID_MAX)
	{
		dbgprint("AmsCallEventIndTimeoutProc[%d] Teller[%s][%u] Vtm[%s][%u] SrvGrpId[%u]Err", 
			pid, lpAmsData->tellerNo, lpAmsData->tellerId, 
			lpAmsData->vtmNo, lpAmsData->vtmId, 
			lpAmsData->srvGrpId);
		iret = AMS_CMS_EVENT_IND_TIMEOUT_SERVICE_GROUP_ID_ERR;
	    AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);  	        
		return AMS_ERROR;		
	}

    //业务状态检查
	if(AMS_SERVICE_ACTIVE != AmsSrvData(lpAmsData->srvGrpId).serviceState)
	{
		dbgprint("AmsCallEventIndTimeoutProc[%d] Teller[%s][%u] Vtm[%s][%u] ServiceState[%d]Err", 
			pid, lpAmsData->tellerNo, lpAmsData->tellerId, 
			lpAmsData->vtmNo, lpAmsData->vtmId, 
			AmsSrvData(lpAmsData->srvGrpId).serviceState);
		iret = AMS_CMS_EVENT_IND_TIMEOUT_SERVICE_STATE_ERR;
	    AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);  	        
		return AMS_ERROR;			
	}
	
	/* find Vta Node */
    pVtaNode = AmsSearchVtaNode(lpAmsData->srvGrpId, lpAmsData->tellerId);
	if(NULL == pVtaNode)
	{
		dbgprint("AmsCallEventIndTimeoutProc[%d] Vtm[%s][%u] Teller[%s]Id[%u]Err", 
			pid, lpAmsData->vtmNo, lpAmsData->vtmId, 
			lpAmsData->tellerNo, lpAmsData->tellerId);		
		iret = AMS_CMS_EVENT_IND_TIMEOUT_TELLER_ID_ERR;
	    AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);  	        
		return AMS_ERROR;			
	}

	//柜员状态检查
	if(pVtaNode->state > AMS_VTA_STATE_OFFLINE)
	{
		dbgprint("AmsCallEventIndTimeoutProc[%d] Vtm[%s][%u] Teller[%s][%u]State[%d]Err", 
			pid, lpAmsData->vtmNo, lpAmsData->vtmId, 
			lpAmsData->tellerNo, lpAmsData->tellerId, 
			pVtaNode->state);
		iret = AMS_CMS_EVENT_IND_TIMEOUT_TELLER_STATE_ERR;
	    AmsResultStatProc(AMS_CMS_EVENT_IND_RESULT, iret);  	        
		return AMS_ERROR;		
	}

	//it would not be happened
	if(    CMS_CALL_EVENT_IND_VTA_OFFLINE == callEventInd 
		|| CMS_CALL_EVENT_IND_VTA_FORCE_LOGIN_OFFLINE == callEventInd
		|| CMS_CALL_EVENT_IND_VTA_LOGOUT_OFFLINE == callEventInd
		|| CMS_CALL_EVENT_IND_VTA_CALL_STATE_ABORT == callEventInd)
	{
       //do nothing
	}

	//it would not be happened
	if(    CMS_CALL_EVENT_IND_VTM_OFFLINE == callEventInd 
		|| CMS_CALL_EVENT_IND_VTM_FORCE_LOGIN_OFFLINE == callEventInd
		|| CMS_CALL_EVENT_IND_VTM_LOGOUT_OFFLINE == callEventInd)
	{
       //do nothing
	}	

	
	//it would not be happened
	if(    CMS_CALL_EVENT_IND_VTM_OFFLINE_DEQUEUE == callEventInd
		|| CMS_CALL_EVENT_IND_VTM_FORCE_LOGIN_DEQUEUE == callEventInd
		|| CMS_CALL_EVENT_IND_VTM_LOGOUT_DEQUEUE == callEventInd)
	{
       //do noting
	}

	//it would not be happened
	if(    CMS_CALL_EVENT_IND_MNG_REL_VTA == callEventInd 
		|| CMS_CALL_EVENT_IND_MNG_FORCE_LOGOUT_VTA == callEventInd 
		|| CMS_CALL_EVENT_IND_MNG_DEL_VTA == callEventInd)
	{
        //do noting
	}
	
	//it would not be happened
	if(CMS_CALL_EVENT_IND_MNG_FORCE_REL_CALL_VTA == callEventInd)
	{
        //do noting
	}
	
	if(CMS_CALL_EVENT_IND_CALL_TRANSFER == callEventInd)
	{
		if(AMS_CALL_STATE_NULL != pVtaNode->callState)
		{
			//Reset Vta Call State and State Start Time 
			AmsSetVtaCallState(lpAmsData, pVtaNode, AMS_CALL_STATE_NULL);
		}
						
		//send Transfer Call Rsp to Vta
		iret = AMS_VTA_TRANSFER_CALL_TIMEOUT;
		AmsSendVtaTransferCallRsp(lpAmsData,&msg,iret);		
	}

	return iret;

}

//zry added for scc 2018
int SeatRegReqProc(int iThreadId,MESSAGE_t *pMsg)
{
	int				iret = AMS_CMS_PROCESS_SUCCESS;
	unsigned char   tellidlen=0;
	unsigned char	tellid[AMS_MAX_TELLER_ID_LEN + 1];
	TERM_NET_INFO	tellerNetInfo;
	SEAT_NODE		*pSeatNode = NULL;
	//unsigned char	newTransIpLen = 0;
	//unsigned char
	unsigned int	vtaIp = 0;
	unsigned int	tellerCfgPos = 0;
	int				pid = 0;
	int 			pos = 0;
	int 			i   = 0;
	int				j	= 0;
	unsigned char	*p;

	//获取远端pid
	pid	= pMsg->s_SenderPid.iProcessId;

	//检查接收进程号
	if(pMsg->s_ReceiverPid.iProcessId != 0)
	{
		dbgprint("VtaRegReqProc[%d] Pid:%d Err", pid, pMsg->s_ReceiverPid.iProcessId);
		iret = AMS_CMS_VTA_REG_PARA_ERR;
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
		
		return AMS_ERROR;
	}

	//消息长度检查  待改
	if(pMsg->iMessageLength < 13)
	{
		dbgprint("VtaRegReqProc[%d] Len[%d] Err", pid, pMsg->iMessageLength);
		iret = AMS_CMS_VTA_REG_LEN_ERR;
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}

	p = pMsg->cMessageBody;

	for(i = 0;i < AMS_MAX_TELLER_ID_LEN;i++)
	{
		if(p[i] == TERMINFO_ID) // �?? if(p[i] == '\0')
		{
			break;
		}
	}

	if(i >= AMS_MAX_TELLER_ID_LEN)
	{
		dbgprint("VtaRegReqProc[%d] Len[%d] Err",pid,pMsg->iMessageLength);
		iret = AMS_CMS_VTA_REG_LEN_ERR;
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}
	
	tellidlen = i; //tellid长度
	memcpy(tellid,p,tellidlen);

	p += i;

	//unpack v part TermNetInfo
	memset(&tellerNetInfo,0,sizeof(TERM_NET_INFO));
	iret = AmsUnpackVtaRegReqOpartPart(p,pMsg->iMessageLength - tellidlen,&tellerNetInfo);
	if(AMS_OK != iret)
	{
		dbgprint("VtaRegReqProc[%d][%d][%d] Teller[%s] UnpackVtaRegReqPara Err", 
			pMsg->s_SenderPid.cModuleId,
			pMsg->s_SenderPid.cFunctionId,
			pMsg->s_SenderPid.iProcessId,
			tellid);

		if(AMS_ERROR == iret)
		{
			iret = AMS_CMS_VTA_REG_PARA_ERR;
		}
		
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;	
	}

	//工号检查
	//查看该工号是否登陆
	/*A find Crm Node in process*/
	for(i=0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		pSeatNode = AmsSearchSeatNode(i,tellid,tellidlen);
		if(NULL != pSeatNode)
		{
			break;
		}
	}

	if(NULL == pSeatNode)
	{
		dbgprint("VtaRegReqProc[%d] Teller[%s] not Login",
			pid,tellid);
		iret = AMS_CMS_VTA_REG_STATE_ERR;
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}

	/*B check RegTeller in cfg or not*/
	tellerCfgPos = pSeatNode->tellerCfgPos;
	if(tellerCfgPos >= AMS_MAX_SEAT_NUM)
	{
		dbgprint("VtaReqReqProc[%d] Teller[%s] CfgPos[%d] Err",
			pid,tellerid,tellerCfgPos);
		iret = AMS_CMS_VTA_REG_TELLER_ID_ERR;
				AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}

	if(AMS_TELLER_INSTALL != AmsCfgTeller(tellerCfgPos).flag 
		|| 0 != strcmp(AmsCfgTeller(tellerCfgPos).tellerId,tellid))
	{
		dbgprint("VtaRegReqProc[%d] Teller[%s]Id[%u][%u]Err-Flag[%d]Pos[%d].",
			pid, tellerNo, tellerId, AmsCfgTeller(tellerCfgPos).tellerId, 
			AmsCfgTeller(tellerCfgPos).flag, tellerCfgPos);
		
		iret = AMS_CMS_VTA_REG_TELLER_ID_ERR;
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;		
	}

	/*C check teller has been registered or not */.
	//以及注册，则更新注册信息
	for(i = 0; i < AMS_MAX_SEAT_NUM; i++)
	{
		if(AmsRegTeller(i).flag != AMS_TELLER_REGISTER)
		{
			continue;
		}
		
		if(0 == strcmp(tellid,AmsRegTeller(i).tellerId))
		{	
			pos = i;

			break;			
		}	
	}

	//尚未注册，则记录注册信息
	if(i >= AMS_MAX_SEAT_NUM)
	{
		for(j = 0; j < AMS_MAX_SEAT_NUM; j++)
		{
			if(AMS_TELLER_UNREGISTER == AmsRegTeller(j).flag)
			{
				break;
			}
		}

		if(j >= AMS_MAX_SEAT_NUM)
		{
			dbgprint("VtaRegReqProc[%d][%d][%d] Teller[%s]ResourceLimited", 
				pMsg->s_SenderPid.cModuleId,
			    pMsg->s_SenderPid.cFunctionId,
			    pMsg->s_SenderPid.iProcessId, tellid);		
			
			iret = AMS_CMS_VTA_REG_RESOURCE_LIMITED;
			AmsSendCmsVtaRegRsp(NULL,pMsg,iret);		
			return AMS_ERROR;	
		}

		AmsRegTeller(j).flag = AMS_TELLER_REGISTER;	
		strcpy(AmsRegTeller(j).tellerId ,tellid);	
		
		pos = j;
		
	}

	//记录或更新注册信息
	if(tellerNetInfo.ip != AmsRegTeller(pos).vtaIp)
    {
		AmsRegTeller(pos).seatIp = tellerNetInfo.ip;		
		transIpChange = 1;
    }

	AmsRegTeller(pos).seatPort = tellerNetInfo.port;	

	AmsRegTeller(pos).cmsPid.iProcessId  = pMsg->s_SenderPid.cModuleId;
	AmsRegTeller(pos).cmsPid.cFunctionId = pMsg->s_SenderPid.cFunctionId;
	AmsRegTeller(pos).cmsPid.cModuleId   = pMsg->s_SenderPid.iProcessId;
	
	AmsRegTeller(pos).myPid.iProcessId  = pMsg->s_ReceiverPid.cModuleId;
	AmsRegTeller(pos).myPid.cFunctionId = pMsg->s_ReceiverPid.cFunctionId;
	AmsRegTeller(pos).myPid.cModuleId   = pMsg->s_ReceiverPid.iProcessId;   

	//record tellerRegPos
	pSeatNode->tellerRegPos = pos;

	
	//update vtaIp if changed
	if(1 == transIpChange)
	{
		pSeatNode->seatInfo.seatIp = AmsRegTeller(pos).seatIp;	
		AmsCfgTeller(tellerCfgPos).seatIp = AmsRegTeller(pos).seatIp;

		vtaIp = htonl(AmsRegTeller(pos).vtaIp);
		memset(newTransIp, 0, AMS_MAX_TRANS_IP_LEN + 1);
		snprintf(newTransIp,sizeof(newTransIp),"%s",inet_ntoa(*((struct in_addr *)&vtaIp)));	
		
		newTransIpLen = strlen(newTransIp);
		if(newTransIpLen > 0 && newTransIpLen <= AMS_MAX_TRANS_IP_LEN)
		{
			strcpy((char *)pSeatNode->seatInfo.transIp, newTransIp);	
			pSeatNode->seatInfo.transIp[newTransIpLen] = '\0';
			pSeatNode->seatInfo.transIpLen = newTransIpLen;

			strcpy((char *)AmsCfgSeat(tellerCfgPos).transIp, newTransIp); 	
			AmsCfgSeat(tellerCfgPos).transIp[newTransIpLen] = '\0';
			AmsCfgSeat(tellerCfgPos).transIpLen = newTransIpLen;
		}
		else
		{
			memset(pSeatNode->seatInfo.transIp, 0, AMS_MAX_TRANS_IP_LEN + 1);
			pSeatNode->seatInfo.transIpLen = 0;	

			memset(AmsCfgSeat(tellerCfgPos).transIp, 0, AMS_MAX_TRANS_IP_LEN + 1);
			AmsCfgSeat(tellerCfgPos).transIpLen = 0;	
		}
	}	
		
	//record vtaPort
	pSeatNode->seatInfo.seatPort = AmsRegTeller(pos).seatPort;
	AmsCfgSeat(tellerCfgPos).seatPort = AmsRegTeller(pos).seatPort;

	//send Vta Reg Rsp to CMS
	AmsSendCmsVtaRegRsp(&AmsRegTeller(pos),pMsg,iret);
		
}

int SeatGetReqProc(int iThreadId,MESSAGE_t *pMsg)
{
	int				iret = AMS_CMS_PROCESS_SUCCESS;
	LP_AMS_DATA_t	*lpAmsData = NULL;       //进程数据区指针
	LP_AMS_DATA_t	*lpOriginAmsData = NULL; //进程数据区指针
	LP_QUEUE_DATA_t	*lpQueueData = NULL;	 //排队进程数据区指针
	SEAT_NODE		*pSeatNode = NULL;
	SEAT_NODE		*pOriginSeatNode = NULL;
	SEAT_NODE		*pTargetSeatNode = NULL;
	USER_NODE		*pUserNode = NULL;
	CALL_TARGET		callTarget;
	unsigned char	srvGrpSelfAdapt = 0;
	int				tps = 0;
	int				pid = 0;
	int				originPid = 0;
	unsigned int	amsPid = 0;
	unsigned char	userIdLen=0;
	unsigned char	userId[AMS_MAX_USER_ID_LEN + 1] = {0};
	unsigned char	callIdLen=0;
	unsigned char   callId[AMS_MAX_CALL_ID_LEN + 1]={0};
	unsigned int	terminalType=-0;
	unsigned char	targetTellerId[AMS_MAX_TELLER_ID_LEN + 1] = {0};
	unsigned char   srvgrpidlen=0;
	unsigned char	srvGrpId[AMS_MAX_GRPID_LEN + 1]={0};
	unsigned char   servicetypelen=0;
	unsigned char	serviceType[AMS_MAX_SERVICETYPE_LEN + 1] = {0};
	unsigned char   servicetypersvdlen=0;
	unsigned char	serviceTypeRsvd[AMS_MAX_SERVICETYPE_LEN + 1]={0};
	unsigned short	callType = 0;
	unsigned int	i=0;
	unsigned char   *p;

#ifdef AMS_TEST_LT
	time_t			currentTime;
#endif


	//get remote pid
	pid = pMsg->s_SenderPid.iProcessId;

	//检查接收进程号
	if(pMsg->s_ReceiverPid.iProcessId != 0)
	{
		dbgprint("SeatGetReqProc[%d] Pid:%d Err", pid, pMsg->s_ReceiverPid.iProcessId);
		iret = AMS_CMS_GET_VTA_PARA_ERR;
		AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);

		return AMS_ERROR;
	}

	//消息长度检查
	if(pMsg->iMessageLength < 23)
	{
		dbgprint("SeatGetReqProc[%d] Len[%d] Err", pid, pMsg->iMessageLength);
		iret = AMS_CMS_GET_VTA_LEN_ERR;
		AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
		return AMS_ERROR;
	}

	p = pMsg->cMessageBody;

	//流水号检查
	//callid的格式� LV格式 string都是LV格式 第一个字节是len 后面是字符串
	callIdLen=*p++;
	if(callIdLen > AMS_MAX_CALLID_LEN)
	{
		dbgprint("SeatGetReqProc[%d] CallIdLen[%d]Err", pid, callIdLen);
		iret = AMS_CMS_GET_VTA_CALL_ID_ERR;
		AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);	
		return AMS_ERROR;
	}
	memcpy(callId,p+1,callIdLen);
	p+=callIdLen;

	//终端类型检查
	BEGETLONG(terminalType, p);
	if(terminalType < AMS_TERMINAL_ROBOT || terminalType >= AMS_TERMINAL_MAX)
	{
		dbgprint("SeatGetReqProc[%d] TerminalType[%d]Err", pid, terminalType);
		iret = AMS_CMS_GET_VTA_TERMINAL_TYPE_ERR;
		AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
		return AMS_ERROR;		
	}	
	p += 4;


	if(AMS_TERMINAL_MOBILEPHONE == terminalType)
	{
		userIdLen=*p++;
		if(userIdLen > AMS_MAX_USERID_LEN)
		{
			dbgprint("SeatGetReqProc[%d] termidlen[%u]Err", 
				pid, userIdLen);
			iret = AMS_CMS_GET_VTA_CALL_ID_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
			return AMS_ERROR;		
		}
		memcpy(userId,p,userIdLen);
		p+=userIdLen;

		/*find user node in process */
		for(i = 0;i < AMS_MAX_SERVICE_GROUP_NUM;i++)
		{
			pUserNode = AmsSearchUserNode(i,userId);
			if(NULL != pUserNode)
			{
				break;
			}
		}

		if(NULL == pUserNode)
		{
			dbgprint("SeatGetReqProc[%d] User[%s]Err", pid, userId);
			iret = 	AMS_CMS_GET_VTA_VTM_ID_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
			return AMS_ERROR;		
		}

		//user 状态异常保护
		if(AMS_USER_STATE_IDLE != pUserNode->state)
		{
			//set user state and state start time
			AmsSetUserState(iThreadId,pUserNode,AMS_USER_STATE_IDLE);
			
		}

		if(AMS_CALL_STATE_NULL != pUserNode->callState)
		{
			//set user call state and state start time
			AmsSetUserCallState(pUserNode,AMS_CALL_STATE_NULL);
		}

		if(AMS_CUSTOMER_SERVICE_NULL != pUserNode->servicestate)
		{
			if(AMS_CUSTOMER_IN_QUEUE == pUserNode->serviceState)
			{
				if(pUserNode->customerPid > 0 && pUserNode->customerPid < LOGIC_PROCESS_SIZE)
				{
					if(pUserNode->customerPid > 0 && pUserNode->customerPid < LOGIC_PROCESS_SIZE)
					{
						lpQueueData=(LP_QUEUE_DATA_t *)ProcessData[pUserNode->customerPid];
						
						/* 杀掉定时器 */
						if(lpQueueData->iTimerId >= 0)
						{
							AmsQueueKillTimer(pUserNode->customerPid, &lpQueueData->iTimerId);//或 lpQueueData->myPid.iProcessId
							AmsTimerStatProc(T_AMS_CUSTOMER_IN_QUEUE_TIMER, AMS_KILL_TIMER);
						} 

						//release lpQueueData Pid
						AmsReleassPid(lpQueueData->myPid, END);
					}
				}					
			}		

			//set Vtm Service State and State Start Time
			AmsSetUserServiceState(pUserNode, AMS_CUSTOMER_SERVICE_NULL);	
		}

		//reset amsPid
		pUserNode->amsPid = 0;

		//reset customerPid
		pUserNode->customerPid = 0;		

		//业务组编号
		srvgrpidlen=*p++;
		if(srvgrpidlen > AMS_MAX_GRPID_LEN)
		{
			dbgprint("SeatGetReqProc[%d] srvgrpidlen[%u]Err", pid, srvgrpidlen);
			iret = AMS_CMS_GET_VTA_ORIGIN_TELLER_ID_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
			return AMS_ERROR;		
		}
		memcpy(srvGrpId,p,srvgrpidlen);
		p += srvgrpidlen;
		
		//业务类型		
		servicetypelen=*p++;
		if(servicetypelen > AMS_MAX_SERVICETYPE_LEN)
		{
			dbgprint("SeatGetReqProc[%d] servicetypelen[%u]Err", pid, servicetypelen);
			iret = AMS_CMS_GET_VTA_ORIGIN_TELLER_ID_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
			return AMS_ERROR;

		}
		memcpy(serviceType,p,servicetypelen);
		p += servicetypersvdlen;

	}

	//检查业务组编号与业务类型组合
	if(0 == srvgrpidlen && 0 == servicetypelen)
	{
		dbgprint("SeatGetReqProc[%d] User[%s] SrvGrpId[%s]ServiceType[%s]Err", 
			pid, userId, srvGrpId, serviceType);
		iret = 	AMS_CMS_GET_VTA_NO_VALID_SERVICE_GROUP_ID_OR_TYPE;
		AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);
		return AMS_ERROR;
	}

	//仅携带业务类型，没有指明业务组编号
	if(0 == srvgrpidlen && 0 != servicetypelen)
	{
		//根据业务类型选择业务组
		iret = AmsSelectSrvGrpIdByServiceType(userId,serviceType,srvGrpId);
		
		if(AMS_OK != iret)
		{
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);		
			return AMS_ERROR;			
		}	

		srvGrpSelfAdapt = 1;
	}

	//新呼叫添加到链表尾
	/*del user node from origin list */
	Sem_wait(&AmsSrvData(pUserNode->userInfo.srvGrpNo).userCtrl);
	lstDelete(&AmsSrvData(pUserNode->userInfo.srvGrpNo).userList, (NODE *)pUserNode);
	Sem_post(&AmsSrvData(pUserNode->userInfo.srvGrpNo).userCtrl);

	/* add user node to new list */
	Sem_wait(&AmsSrvData(srvGrpNo).userCtrl);
	lstAdd(&AmsSrvData(srvGrpNo).userList, (NODE *)pUserNode);
	Sem_post(&AmsSrvData(srvGrpNo).userCtrl);

	//更新当前用户的业务组编号
	if(0 != strcmp(pUserNode->userInfo.srvGrpId,srvGrpId))
	{
		//update srvgrpid
		memset(pUserNode->userInfo.srvGrpId,0,AMS_MAX_SRVGRPID_LEN);
		memcpy(pUserNode->userInfo.srvGrpId,srvGrpId,srvgrpidlen);
	}

	//业务智能路由
	pSeatNode = AmsServiceIntelligentSelectSeat(userId,userno,srvGrpNo,serviceType,servicetypelen,pUserNode->userInfo.orgNo,pUserNode->orgCfgPos,&iret);
	if(NULL == pSeatNode)
	{
		if(AMS_CMS_GET_VTA_SERVICE_IN_QUEUE != iret)
		{
			dbgprint("SeatGetReqProc[%d] Vtm[%s][%u] OriginTeller[%s][%u] SISelectVta Failed", 
				pid, userId, userNo, originSeatId, originTellerNo);
			
			if(AMS_ERROR == iret)
			{
				iret = AMS_CMS_GET_VTA_SERVICE_INTELLIGENT_ROUTING_ERR;
			}
					
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);		
			return AMS_ERROR;	
		}

		iret = AmsStartCustomerQueueProcess(pMsg,pUserNode,srvGrpNo,serviceType,servicetypelen,callIdLen,srvGrpSelfAdapt);
		if(AMS_OK != iret)
		{
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,NULL,NULL);		
			return AMS_ERROR;			
		}

		//customer in queue...
		iret = AMS_CMS_GET_VTA_SERVICE_IN_QUEUE;

		//update Customer Service State
		AmsSetUserServiceState(pUserNode,AMS_CUSTOMER_IN_QUEUE);

		//init enterqueuetime
		time(&pUserNode->enterQueueTime);

		//record servicetype
		memcpy(pUserNode->serviceType,serviceType,servicetypelen);

		
	}
	else
	{
		//检查进程号
		//get local pid
		pid = pSeatNode->amsPid & 0xffff;
		if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
		{
			dbgprint("SeatGetReqProc Vtm[%s][%u] Teller[%s][%u] Pid[0x%x][%d]Err", 
				vtmNo, vtmId, pVtaNode->vtaInfo.tellerNo, pVtaNode->vtaInfo.tellerId,
				pSeatNode->amsPid, pid);
			iret = AMS_CMS_GET_VTA_AMS_PID_ERR;
			AmsSendCmsVtaGetRsp(NULL,pMsg,iret,pSeatNode,NULL);
			return AMS_ERROR;
		}

		lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];

		//更新进程数据
		//record userid
		if(pUserNode->userinfo.useridlen <= AMS_MAX_USERID_LEN)
		{
			memset(lpAmsData->userid, 0, (AMS_MAX_USERID_LEN + 1));
			memcpy(lpAmsData->userid, pUserNode->userInfo.userId, pUserNode->userInfo.userIdLen);
			lpAmsData->useridLen = pUserNode->userInfo.useridLen;
		}

		//record vtmPos
		lpAmsData->vtmPos = pUserNode->vtmCfgPos;
		
		//Set seat call State, only one pthread!!!
		AmsSetSeatCallState(lpAmsData, pSeatNode, AMS_CALL_STATE_WAIT_ANSWER);

#ifdef AMS_TEST_LT
	    //calc vta workInfo
		time(&currentTime);	   
		AmsUpdateSingleSeatWorkInfo(pSeatNode, currentTime);
	
		//set Vta State and State Start Time
		AmsSetSeatState(iThreadId, lpAmsData, pSeatNode, AMS_SEAT_STATE_BUSY, 0);
#endif

		//record vtmPid
        memcpy(&lpAmsData->vtmPid,&pUserNode->rPid,sizeof(PID_t));
		
		//record callId
	    lpAmsData->callIdLen = callIdLen;
		memcpy(lpAmsData->callId, &pMsg->cMessageBody[1], callIdLen);

		//record amsPid
		pUserNode->amsPid = pSeatNode->amsPid;

		//record serviceType	
		memcpy(pUserNode->serviceType,serviceType,servicetypelen);
		
	    //update Customer Service State
		AmsSetUserServiceState(pUserNode, AMS_CUSTOMER_IN_SERVICE);

		//update cmsPid
		lpAmsData->cmsPid.cModuleId	   = pMsg->s_SenderPid.cModuleId;
		lpAmsData->cmsPid.cFunctionId  = pMsg->s_SenderPid.cFunctionId;
		lpAmsData->cmsPid.iProcessId   = pMsg->s_SenderPid.iProcessId;

	}

	//update cmsPid
	pUserNode->cmsPid.cModuleId	  = pMsg->s_SenderPid.cModuleId;
	pUserNode->cmsPid.cFunctionId  = pMsg->s_SenderPid.cFunctionId;
	pUserNode->cmsPid.iProcessId   = pMsg->s_SenderPid.iProcessId;
	
    //set Vtm State and State Start Time
	AmsSetUserState(iThreadId, pUserNode, AMS_USER_STATE_BUSY);

	//send Vta Get Rsp to CMS
	if(AMS_CUSTOMER_IN_SERVICE == pUserNode->serviceState)
	{
		//lpAmsData 可为NULL
    	AmsSendCmsVtaGetRsp(lpAmsData,pMsg,iret,pSeatNode,NULL);
	}
	
	if(AMS_CUSTOMER_IN_QUEUE == pUserNode->serviceState)
	{
		//lpAmsData 可为NULL
    	AmsSendCmsVtaGetRsp(lpAmsData,pMsg,iret,NULL,pUserNode);
	}
	
	return iret;
}

//added end

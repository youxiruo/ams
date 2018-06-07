strcpint CrmRegReqProc(int iThreadId,MESSAGE_t *pMsg)
{
	int				iret = AMS_CMS_PROCESS_SUCCESS;
	unsigned char   tellidlen=0;
	unsigned char	tellid[AMS_MAX_TELLER_ID_LEN + 1];
	TERM_NET_INFO	tellerNetInfo;
	CRM_NODE		*pCrmNode = NULL;
	//unsigned char	newTransIpLen = 0;
	//unsigned char
	unsigned int	vtaIp = 0;
	unsigned int	tellerCfgPos = 0;
	int				pid = 0;
	int 			pos = 0;
	int 			i   = 0;
	int				j	= 0;
	unsigned char	*p;

	//»ñÈ¡Ô¶¶Ëpid
	pid	= pMsg->s_SenderPid.iProcessId;

	//¼ì²é½ÓÊÕ½ø³ÌºÅ
	if(pMsg->s_ReceiverPid.iProcessId != 0)
	{
		dbgprint("VtaRegReqProc[%d] Pid:%d Err", pid, pMsg->s_ReceiverPid.iProcessId);
		iret = AMS_CMS_VTA_REG_PARA_ERR;
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
		
		return AMS_ERROR;
	}

	//ÏûÏ¢³¤¶È¼ì²é  ´ý¸Ä
	if(pMsg->iMessageLength < 13)
	{
		dbgprint("VtaRegReqProc[%d] Len[%d] Err", pid, pMsg->iMessageLength);
		iret = AMS_CMS_VTA_REG_LEN_ERR;
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}

	p = pMsg->cMessageBody;

	for(i = 0;i < AMS_MAX_TELLERID_LEN;i++)
	{
		if(p[i] == TERMINFO_ID) // å?? if(p[i] == '\0')
		{
			break;
		}
	}

	if(i >= AMS_MAX_TELLERID_LEN)
	{
		dbgprint("VtaRegReqProc[%d] Len[%d] Err",pid,pMsg->iMessageLength);
		iret = AMS_CMS_VTA_REG_LEN_ERR;
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}
	
	tellidlen = i; //tellid³¤¶È
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

	//¹¤ºÅ¼ì²é
	//²é¿´¸Ã¹¤ºÅÊÇ·ñµÇÂ½
	/*A find Crm Node in process*/
	for(i=0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		pCrmNode = AmsSearchCrmNode(i,tellid,tellidlen);
		if(NULL != pCrmNode)
		{
			break;
		}
	}

	if(NULL == pCrmNode)
	{
		dbgprint("VtaRegReqProc[%d] Teller[%s] not Login",
			pid,tellid);
		iret = AMS_CMS_VTA_REG_STATE_ERR;
		AmsSendCmsVtaRegRsp(NULL,pMsg,iret);
		return AMS_ERROR;
	}

	/*B check RegTeller in cfg or not*/
	tellerCfgPos = pCrmNode->tellerCfgPos;
	if(tellerCfgPos >= AMS_MAX_MBPHONE_NUM)
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
	//ÒÔ¼°×¢²á£¬Ôò¸üÐÂ×¢²áÐÅÏ¢
	for(i = 0; i < AMS_MAX_MBPHONE_NUM; i++)
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

	//ÉÐÎ´×¢²á£¬Ôò¼ÇÂ¼×¢²áÐÅÏ¢
	if(i >= AMS_MAX_MBPHONE_NUM)
	{
		for(j = 0; j < AMS_MAX_MBPHONE_NUM; j++)
		{
			if(AMS_TELLER_UNREGISTER == AmsRegTeller(j).flag)
			{
				break;
			}
		}

		if(j >= AMS_MAX_MBPHONE_NUM)
		{
			dbgprint("VtaRegReqProc[%d][%d][%d] Teller[%s]ResourceLimited", 
				pMsg->s_SenderPid.cModuleId,
			    pMsg->s_SenderPid.cFunctionId,
			    pMsg->s_SenderPid.iProcessId, tellerId);		
			
			iret = AMS_CMS_VTA_REG_RESOURCE_LIMITED;
			AmsSendCmsVtaRegRsp(NULL,pMsg,iret);		
			return AMS_ERROR;	
		}

		AmsRegTeller(j).flag = AMS_TELLER_REGISTER;	
		strcpy(AmsRegTeller(j).tellerId ,tellerId);	
		
		pos = j;
		
	}

	//¼ÇÂ¼»ò¸üÐÂ×¢²áÐÅÏ¢
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
	pCrmNode->tellerRegPos = pos;

	
	//update vtaIp if changed
			if(1 == transIpChange)
			{
				pCrmNode->vtaInfo.vtaIp = AmsRegTeller(pos).vtaIp;	
				AmsCfgTeller(tellerCfgPos).vtaIp = AmsRegTeller(pos).vtaIp;
		
				vtaIp = htonl(AmsRegTeller(pos).vtaIp);
				memset(newTransIp, 0, AMS_MAX_TRANS_IP_LEN + 1);
				snprintf(newTransIp,sizeof(newTransIp),"%s",inet_ntoa(*((struct in_addr *)&vtaIp)));	
				
				newTransIpLen = strlen(newTransIp);
				if(newTransIpLen > 0 && newTransIpLen <= AMS_MAX_TRANS_IP_LEN)
				{
					strcpy((char *)pVtaNode->vtaInfo.transIp, newTransIp);	
					pCrmNode->vtaInfo.transIp[newTransIpLen] = '\0';
					pCrmNode->vtaInfo.transIpLen = newTransIpLen;
		
					strcpy((char *)AmsCfgTeller(tellerCfgPos).transIp, newTransIp); 	
					AmsCfgTeller(tellerCfgPos).transIp[newTransIpLen] = '\0';
					AmsCfgTeller(tellerCfgPos).transIpLen = newTransIpLen;
				}
				else
				{
					memset(pCrmNode->vtaInfo.transIp, 0, AMS_MAX_TRANS_IP_LEN + 1);
					pCrmNode->vtaInfo.transIpLen = 0;	
		
					memset(AmsCfgTeller(tellerCfgPos).transIp, 0, AMS_MAX_TRANS_IP_LEN + 1);
					AmsCfgTeller(tellerCfgPos).transIpLen = 0;	
				}
			}	
		
		//record vtaPort
		pCrmNode->vtaInfo.vtaPort = AmsRegTeller(pos).vtaPort;
		AmsCfgTeller(tellerCfgPos).vtaPort = AmsRegTeller(pos).vtaPort;
	
		//send Vta Reg Rsp to CMS
		AmsSendCmsVtaRegRsp(&AmsRegTeller(pos),pMsg,iret);
		
}

#include "amsfunc.h"

int AmsTimerStatProc(int code, int action)
{
	//code: timer type; action: 0-create 1-kill 2-timeout
	if(action < AMS_CREATE_TIMER || action > AMS_TIMER_TIMEOUT)
	{
		return AMS_ERROR;
	}
	
	switch(code)
	{
	case T_AMS_CRM_STATE_OPERATE_IND_TIMER: 
		AmsTimerStat.WaitCrmOperateIndRsp[action]++;
		break;  

	case T_AMS_CUSTOMER_IN_QUEUE_TIMER: 
		AmsTimerStat.WaitCustomerGetTeller[action]++;
		break;	

	case T_AMS_REST_TIMER: 
		AmsTimerStat.WaitRestRsp[action]++;
		break;

	default:
		AmsTimerStat.UnknownTimer[action]++;
		return AMS_ERROR;
	}
	
	return AMS_OK;
}

int AmsCommonResultStatProc(int resultCode)
{
	switch(resultCode)
	{
	case AMS_SUCCESS: //AMS_COMMON_REASON_BASE
		AmsResultStat.amsSuccess++;
		break;
		
	case AMS_PARA_ERR: 
		AmsResultStat.amsParaErr++;
		break;
		
	case AMS_STATE_ERR: 
		AmsResultStat.amsStateErr++;
		break;
		
	case AMS_ALLOC_MEM_FAILED: 
		AmsResultStat.amsAllocMemFailed++;
		break;		
		
	case AMS_ALLOC_LP_FAILED: 
		AmsResultStat.amsAllocLpFailed++;
		break;
		
	case AMS_ALLOC_LT_FAILED: 
		AmsResultStat.amsAllocLtFailed++;
		break;
	
	default:
		AmsResultStat.amsUnknownResult++;
		return AMS_ERROR;
	}
	
	return AMS_OK;
}

int AmsCrmLoginResultStatProc(int resultCode)
{
	switch(resultCode)
	{
	case AMS_CRM_QUEUE_MNG_SUCCESS:
		AmsResultStat.crmLoginSuccess++;
		break;
		
	case AMS_CRM_LOGIN_PARA_ERR: //AMS_CRM_QUEUE_MNG_REASON_BASE
		AmsResultStat.crmLoginParaErr++;
		break; 
		
	case AMS_CRM_LOGIN_LP_RESOURCE_LIMITED: 
		AmsResultStat.crmLoginLpResourceLimited++;
		break;		
		
	case AMS_CRM_LOGIN_NODE_RESOURCE_LIMITED: 
		AmsResultStat.crmLoginNodeResourceLimited++;
		break;
		
	case AMS_CRM_LOGIN_TELLER_LEN_ERR: 
		AmsResultStat.crmLoginTellerLenErr++;
		break;

	case AMS_CRM_LOGIN_TELLER_NO_ERR: 
		AmsResultStat.crmLoginTellerNoErr++;
		break;
		
	case AMS_CRM_LOGIN_TELLER_PWD_ERR: 
		AmsResultStat.crmLoginTellerPwdErr++;
		break;	
		
	case AMS_CRM_LOGIN_TELLER_LOGIN_REPEATEDLY: 
		AmsResultStat.crmLoginTellerLoginRepeatedly++;
		break;
 
	case AMS_CRM_LOGIN_TELLER_NUM_ERR: 
		AmsResultStat.crmLoginTellerNumErr++;
		break;	
	case AMS_CRM_LOGIN_TELLER_LOGIN_NOTREGSITER:
		AmsResultStat.crmlogintellerloginnotresiter++;
		break;
	default:
		AmsResultStat.amsUnknownResult++;
		return AMS_ERROR;
	}
	
	return AMS_OK;
}


int AmsCrmStateOperateResultStatProc(int resultCode)
{
	switch(resultCode)
	{
	case AMS_CRM_QUEUE_MNG_SUCCESS:
		AmsResultStat.crmStateOperateSuccess++;
		break;
				
	case AMS_CRM_STATE_OPERATE_PARA_ERR:  
		AmsResultStat.crmStateOperateParaErr++;
		break;
		
	case AMS_CRM_STATE_OPERATE_STATE_ERR: 
		AmsResultStat.crmStateOperateStateErr++;
		break;
		
	case AMS_CRM_STATE_OPERATE_LEN_ERR: 
		AmsResultStat.crmStateOperateLenErr++;
		break;
		
	case AMS_CRM_STATE_OPERATE_AMS_PID_ERR: 
		AmsResultStat.crmStateOperateAmsPidErr++;
		break;		
		
	case AMS_CRM_STATE_OPERATE_TELLER_ID_ERR: 
		AmsResultStat.crmStateOperateTellerIdErr++;
		break;
		
	case AMS_CRM_STATE_OPERATE_SERVICE_GROUP_ID_ERR: 
		AmsResultStat.crmStateOperateSrvGrpIdErr++;
		break;

	case AMS_CRM_STATE_OPERATE_SERVICE_STATE_ERR: 
		AmsResultStat.crmStateOperateServiceStateErr++;
		break;
		
	case AMS_CRM_STATE_OPERATE_CODE_ERR: 
		AmsResultStat.crmStateOperateCodeErr++;
		break;	

	case AMS_CRM_STATE_OPERATE_STATE_OP_REASON_ERR: 
		AmsResultStat.crmStateOperateOpReasonErr++;
		break;
		
	case AMS_CRM_STATE_OPERATE_STATE_OP_TIMELEN_ERR: 
		AmsResultStat.crmStateOperateOpTimeLenErr++;
		break;
		
	case AMS_CRM_STATE_OPERATE_UPDATE_STATE_ERR: 
		AmsResultStat.crmStateOperateUpdateStateErr++;
		break;
		
	case AMS_CRM_STATE_OPERATE_START_TIMER_ERR: 
		AmsResultStat.crmStateOperateStartTimerErr++;
		break;
		
	case AMS_CRM_STATE_OPERATE_REST_TIMEOUT: 
		AmsResultStat.crmStateOperateRestTimeout++;
		break;
		
	case AMS_CRM_STATE_OPERATE_REST_TIMEOUT_PARA_ERR: 
		AmsResultStat.crmStateOpRestTimeoutParaErr++;
		break;
	
	case AMS_CRM_STATE_OPERATE_REST_TIMEOUT_STATE_ERR: 
		AmsResultStat.crmStateOpRestTimeoutStateErr++;
		break;
		
	case AMS_CRM_STATE_OPERATE_REST_TIMEOUT_LEN_ERR: 
		AmsResultStat.crmStateOpRestTimeoutLenErr++;
		break;
		
	case AMS_CRM_STATE_OPERATE_REST_TIMEOUT_AMS_PID_ERR: 
		AmsResultStat.crmStateOpRestTimeoutAmsPidErr++;
		break;
		
	case AMS_CRM_STATE_OPERATE_REST_TIMEOUT_TELLER_ID_ERR: 
		AmsResultStat.crmStateOpRestTimeoutTellerIdErr++;
		break;

	case AMS_CRM_STATE_OPERATE_REST_TIMEOUT_SERVICE_GROUP_ID_ERR: 
		AmsResultStat.crmStateOpRestTimeoutSrvGrpIdErr++;
		break;
		
	case AMS_CRM_STATE_OPERATE_REST_TIMEOUT_SERVICE_STATE_ERR: 
		AmsResultStat.crmStateOpRestTimeoutSrvGrpIdErr++;
		break;
		
	default:
		AmsResultStat.amsUnknownResult++;
		return AMS_ERROR;	
	}
	return AMS_OK;
}

int AmsCrmEventNoticeResultStatProc(int resultCode)
{		
	switch(resultCode)
	{
	case AMS_CRM_QUEUE_MNG_SUCCESS:
		AmsResultStat.crmEventNoticeSuccess++;
		break;
		
	case AMS_CRM_EVENT_NOTICE_PARA_ERR: 
		AmsResultStat.crmEventNoticeParaErr++;
		break;
		
	case AMS_CRM_EVENT_NOTICE_STATE_ERR:
		AmsResultStat.crmEventNoticeStateErr++;
		break;
		
	case AMS_CRM_EVENT_NOTICE_LEN_ERR: 
		AmsResultStat.crmEventNoticeLenErr++;
		break;
		
	case AMS_CRM_EVENT_NOTICE_SERVICE_GROUP_ID_ERR: 
		AmsResultStat.crmEventNoticeSrvGrpIdErr++;
		break;
		
	case AMS_CRM_EVENT_NOTICE_SERVICE_STATE_ERR: 
		AmsResultStat.crmEventNoticeServiceStateErr++;
		break;		

	case AMS_CRM_EVENT_NOTICE_CALL_ID_ERR: 
		AmsResultStat.crmEventNoticeCallIdErr++;
		break;	

	case AMS_CRM_EVENT_NOTICE_CALL_STATE_ERR: 
		AmsResultStat.crmEventNoticeCallStateErr++;
		break;
		
	case AMS_CRM_EVENT_NOTICE_AMS_PID_ERR: 
		AmsResultStat.crmEventNoticeAmsPidErr++;
		break;

	case AMS_CRM_EVENT_NOTICE_TELLER_ID_ERR: 
		AmsResultStat.crmEventNoticeTellerIdErr++;
		break;
		
	case AMS_CRM_EVENT_NOTICE_TERM_ID_ERR: 
		AmsResultStat.crmEventNoticeVtmIdErr++;
		break;	 

	case AMS_CRM_EVENT_NOTICE_TYPE_ERR: 
		AmsResultStat.crmEventNoticeTypeErr++;
		break;	 

	case AMS_CRM_EVENT_NOTICE_LACK_FILE_INFO: 
		AmsResultStat.crmEventNoticeLackFileInfo++;
		break;	
		
	case AMS_CRM_EVENT_NOTICE_FILE_PATH_LEN_ERR: 
		AmsResultStat.crmEventNoticeFilePathLenErr++;
		break;	 
		
	case AMS_CRM_EVENT_NOTICE_FILE_NAME_LEN_ERR: 
		AmsResultStat.crmEventNoticeFileNameLenErr++;
		break;	 
	
	default:
		AmsResultStat.amsUnknownResult++;
		return AMS_ERROR;
	}

	return AMS_OK;
}


int AmsCmsCrmRegResultStatProc(int resultCode)
{
	switch(resultCode)
	{
	case AMS_CMS_PRCOESS_SUCCESS:
		AmsResultStat.cmsCrmRegSuccess++;
		break;
				
	case AMS_CMS_CRM_REG_PARA_ERR: 
		AmsResultStat.cmsCrmRegParaErr++;
		break;
		
	case AMS_CMS_CRM_REG_STATE_ERR: 
		AmsResultStat.cmsCrmRegStateErr++;
		break;
		
	case AMS_CMS_CRM_REG_LEN_ERR: 
		AmsResultStat.cmsCrmRegLenErr++;
		break;
		
	case AMS_CMS_CRM_REG_TELLER_ID_ERR: 
		AmsResultStat.cmsCrmRegTellerIdErr++;
		break;		
		
	case AMS_CMS_CRM_REG_RESOURCE_LIMITED: 
		AmsResultStat.cmsCrmRegResourceLimited++;
		break;
		 
	default:
		AmsResultStat.amsUnknownResult++;
		return AMS_ERROR;
	}
	
	return AMS_OK;
}

int AmsCmsGetCrmResultStatProc(int resultCode)
{
	switch(resultCode)
	{
	case AMS_CMS_PRCOESS_SUCCESS:
		AmsResultStat.cmsGetCrmSuccess++;
		break;
 	
	case AMS_CMS_GET_CRM_PARA_ERR: 
		AmsResultStat.cmsGetCrmParaErr++;
		break;
		
	case AMS_CMS_GET_CRM_LEN_ERR: 
		AmsResultStat.cmsGetCrmLenErr++;
		break;		
		
	case AMS_CMS_GET_CRM_AMS_PID_ERR: 
		AmsResultStat.cmsGetCrmAmsPidErr++;
		break;
		
	case AMS_CMS_GET_CRM_CALL_ID_ERR: 
		AmsResultStat.cmsGetCrmCallIdErr++;
		break;

	case AMS_CMS_GET_CRM_TERMINAL_TYPE_ERR: 
		AmsResultStat.cmsGetCrmTerminalTypeErr++;
		break;
		
	case AMS_CMS_GET_CRM_SERVICE_GROUP_ID_ERR: 
		AmsResultStat.cmsGetCrmSrvGrpIdErr++;
		break;	
		
	case AMS_CMS_GET_CRM_SERVICE_TYPE_ERR: 
		AmsResultStat.cmsGetCrmServiceTypeErr++;
		break;

	case AMS_CMS_GET_CRM_NO_VALID_SERVICE_GROUP_ID_OR_TYPE: 
		AmsResultStat.cmsGetCrmNoValidSrvGrpOrType++;
		break;
		
	case AMS_CMS_GET_CRM_LICENSE_TIMEOUT: 
		AmsResultStat.cmsGetCrmLicenseTimeout++;
		break;	
		
	case AMS_CMS_GET_CRM_TPS_BEYOND_LIC: 
		AmsResultStat.cmsGetCrmTpsBeyondLic++;
		break;	
		
	case AMS_CMS_GET_CRM_SERVICE_IN_QUEUE: 
		AmsResultStat.cmsGetCrmServiceInQueue++;
		break;
		
	case AMS_CMS_GET_CRM_SERVICE_INTELLIGENT_ROUTING_ERR: 
		AmsResultStat.cmsGetCrmSIRErr++;
		break;

	case AMS_CMS_GET_CRM_SIR_TERMNO_ERR: 
		AmsResultStat.cmsGetCrmSIRVtmNoErr++;
		break;
		
	case AMS_CMS_GET_CRM_SIR_SERVICE_GROUP_ID_ERR: 
		AmsResultStat.cmsGetCrmSIRSrvGrpIdErr++;
		break;
		
	case AMS_CMS_GET_CRM_SIR_CALL_TYPE_ERR: 
		AmsResultStat.cmsGetCrmSIRCallTypeErr++;
		break;
		
	case AMS_CMS_GET_CRM_SIR_CRM_EMPTY: 
		AmsResultStat.cmsGetCrmSIRCrmEmpty++;
		break;
		
	case AMS_CMS_GET_CRM_SIR_CALC_QUEUE_INFO_ERR: 
		AmsResultStat.cmsGetCrmSIRCalcQueueInfoErr++;
		break;
		
	case AMS_CMS_GET_CRM_SIR_TOO_MANY_CUSTOMER_IN_QUEUE: 
		AmsResultStat.cmsGetCrmSIRTooManyCustInQueue++;
		break;
		
	case AMS_CMS_GET_CRM_SIR_TARGET_TELLER_NOT_LOGIN: 
		AmsResultStat.cmsGetCrmSIRTargetTellerNotLogin++;
		break;
		
	case AMS_CMS_GET_CRM_SIR_TARGET_TELLER_NOT_REGISTERED: 
		AmsResultStat.cmsGetCrmSIRTargetTellerNotReg++;
		break;
		
	case AMS_CMS_GET_CRM_SIR_TARGET_TELLER_STATE_ERR: 
		AmsResultStat.cmsGetCrmSIRTargetTellerStateErr++;
		break;
	
	case AMS_CMS_GET_CRM_SIR_CALL_TRANSFER_TOO_MANY_TIMES: 
		AmsResultStat.cmsGetCrmSIRCallTransTooManyTimes++;
		break;
		
	case AMS_CMS_GET_CRM_ORG_ID_ERR: 
		AmsResultStat.cmsGetCrmOrgIdErr++;
		break;
	
	case AMS_CMS_GET_CRM_ORG_POS_ERR: 
		AmsResultStat.cmsGetCrmOrgPosErr++;
		break;
		
	case AMS_CMS_GET_CRM_PARENT_ORG_ID_ERR: 
		AmsResultStat.cmsGetCrmParentOrgIdErr++;
		break;
	
	case AMS_CMS_GET_CRM_SELECT_SERVICE_GROUP_ID_ERR: 
		AmsResultStat.cmsGetCrmSelectSrvGrpIdErr++;
		break;	
		
	case AMS_CMS_GET_CRM_LP_RESOURCE_LIMITED: 
		AmsResultStat.cmsGetCrmLpResourceLimited++;
		break;
		
	case AMS_CMS_GET_CRM_CREATE_TIMER_ERR: 
		AmsResultStat.cmsGetCrmCreateTimerErr++;
		break;
		
	case AMS_CMS_GET_CRM_TIMEOUT: 
		AmsResultStat.cmsGetCrmTimeout++;
		break;
		
	case AMS_CMS_GET_CRM_TIMEOUT_PARA_ERR: 
		AmsResultStat.cmsGetCrmTimeoutParaErr++;
		break;
		
	case AMS_CMS_GET_CRM_TIMEOUT_STATE_ERR: 
		AmsResultStat.cmsGetCrmTimeoutStateErr++;
		break;

	case AMS_CMS_GET_CRM_TIMEOUT_LEN_ERR: 
		AmsResultStat.cmsGetCrmTimeoutLenErr++;
		break;
		
	case AMS_CMS_GET_CRM_TIMEOUT_TERM_ID_ERR: 
		AmsResultStat.cmsGetCrmTimeoutVtmIdErr++;
		break;
		
	case AMS_CMS_GET_CRM_TIMEOUT_SERVICE_STATE_ERR: 
		AmsResultStat.cmsGetCrmTimeoutServiceStateErr++;
		break;	
		
	default:
		AmsResultStat.amsUnknownResult++;
		return AMS_ERROR;
	}
	
	return AMS_OK;
}


int AmsCmsEventNoticeResultStatProc(int resultCode)
{
	switch(resultCode)
	{
	case AMS_CMS_PRCOESS_SUCCESS:
		AmsResultStat.cmsEventNoticeSuccess++;
		break;
 			
	case AMS_CMS_EVENT_NOTICE_PARA_ERR: 
		AmsResultStat.cmsEventNoticeParaErr++;
		break;
		
	case AMS_CMS_EVENT_NOTICE_STATE_ERR: 
		AmsResultStat.cmsEventNoticeStateErr++;
		break;
		
	case AMS_CMS_EVENT_NOTICE_SERVICE_GROUP_ID_ERR: 
		AmsResultStat.cmsEventNoticeSrvGrpIdErr++;
		break;
		
	case AMS_CMS_EVENT_NOTICE_SERVICE_STATE_ERR: 
		AmsResultStat.cmsEventNoticeServiceStateErr++;
		break;		
		
	case AMS_CMS_EVENT_NOTICE_LEN_ERR: 
		AmsResultStat.cmsEventNoticeLenErr++;
		break;
		
	case AMS_CMS_EVENT_NOTICE_AMS_PID_ERR: 
		AmsResultStat.cmsEventNoticeAmsPidErr++;
		break;

	case AMS_CMS_EVENT_NOTICE_CALL_ID_ERR: 
		AmsResultStat.cmsEventNoticeCallIdErr++;
		break;
		
	case AMS_CMS_EVENT_NOTICE_TELLER_ID_ERR: 
		AmsResultStat.cmsEventNoticeTellerIdErr++;
		break;	
		
	case AMS_CMS_EVENT_NOTICE_TERM_ID_ERR: 
		AmsResultStat.cmsEventNoticeVtmIdErr++;
		break;
		
	case AMS_CMS_EVENT_NOTICE_CODE_ERR: 
		AmsResultStat.cmsEventNoticeCodeErr++;
		break;
		
	case AMS_CMS_EVENT_NOTICE_CRM_STATE_ERR: 
		AmsResultStat.cmsEventNoticeCrmStateErr++;
		break;
		
	case AMS_CMS_EVENT_NOTICE_TERM_STATE_ERR: 
		AmsResultStat.cmsEventNoticeVtmStateErr++;
		break;

	case AMS_CMS_EVENT_NOTICE_ORIGIN_TELLER_ID_ERR: 
		AmsResultStat.cmsEventNoticeOrgnTellerIdErr++;
		break;
		
	case AMS_CMS_EVENT_NOTICE_ORIGIN_TELLER_STATE_ERR: 
		AmsResultStat.cmsEventNoticeOrgnTellerStateErr++;
		break;
		
	case AMS_CMS_EVENT_NOTICE_ORIGIN_TELLER_AMS_PID_ERR: 
		AmsResultStat.cmsEventNoticeOrgnTellerAmsPidErr++;
		break;
		
	case AMS_CMS_EVENT_NOTICE_ORIGIN_TELLER_SERVICE_GROUP_ID_ERR: 
		AmsResultStat.cmsEventNoticeOrgnTellerSrvGrpIdErr++;
		break;
		
	case AMS_CMS_EVENT_NOTICE_ORIGIN_TELLER_SERVICE_STATE_ERR: 
		AmsResultStat.cmsEventNoticeOrgnTellerSrvStateErr++;
		break;

	case AMS_CMS_HANDSHAKE_CLEAR_INACTIVTE_CALL: 
		AmsResultStat.cmsHandshakeClearInactiveCall++;
		break;
		
	default:
		AmsResultStat.amsUnknownResult++;
		return AMS_ERROR;
	}
	
	return AMS_OK;
}

int AmsCmsCrmCalloutStatProc(int resultCode)
{
	switch(resultCode)
	{
	case AMS_CMS_PRCOESS_SUCCESS:
		AmsResultStat.cmscrmCalloutSuccess++;
		break;
	case AMS_CMS_CALLOUT_AMS_PID_ERR:
		AmsResultStat.cmsCalloutAmspidErr++;
		break;
	case AMS_CMS_CALLOUT_PARA_ERR:
		AmsResultStat.cmsCalloutParaErr++;
		break;
	case AMS_CMS_CALLOUT_STATE_ERR:
		AmsResultStat.cmsCalloutStateErr++;
		break;
	case AMS_CMS_CALLOUT_CRM_CALL_ID_ERR:
		AmsResultStat.cmsCalloutcrmCallidErr++;
		break;
	case AMS_CMS_CALLOUT_CRM_TELLER_ID_ERR:
		AmsResultStat.cmsCalloutcrmTelleridErr++;
		break;
	case AMS_CMS_CALLOUT_CRM_SRVGRP_ID_ERR:
		AmsResultStat.cmsCalloutcrmSrvgrpidErr++;
		break;
	case AMS_CMS_CALLOUT_CRM_SRVTYPE_ERR:
		AmsResultStat.cmsCalloutcrmSrvtypeErr++;
		break;
	case AMS_CMS_CALLOUT_CRM_TELLER_ID_NOTCFG_ERR:
		AmsResultStat.cmsCalloutcrmTelleridNotcfgErr++;
		break;
	case AMS_CMS_CALLOUT_CRM_TELLER_ID_NOT_REG_ERR:
		AmsResultStat.cmsCalloutcrmTelleridNotregErr++;
		break;
	case AMS_CMS_CALLOUT_CRM_TELLER_ID_NOTLOGIN_ERR:
		AmsResultStat.cmsCalloutcrmTelleridNotloginErr++;
		break;
	case AMS_CMS_CALLOUT_CRMREPEAT_ERR:
		AmsResultStat.cmsCalloutcrmRepeatErr++;
		break;
	default:
		AmsResultStat.amsUnknownResult++;
		return AMS_ERROR;
	}
}

//Only One pThread !!!
int AmsResultStatProc(int resultType, int resultCode)
{ 
	switch(resultType)
	{
	// ams <-> crm
	case AMS_COMMON_RESULT: 
		AmsCommonResultStatProc(resultCode);
		break;
		
	case AMS_CRM_LOGIN_RESULT: 
		AmsCrmLoginResultStatProc(resultCode);
		break;
		
	case AMS_CRM_LOGOUT_RESULT: 
		//AmsCrmLogoutResultStatProc(resultCode);
		break;	
		
	case AMS_CRM_STATE_OPERATE_RESULT: 
		AmsCrmStateOperateResultStatProc(resultCode);
		break;
		
	case AMS_CRM_MODIFY_PASSWORD_RESULT: 
		//AmsCrmModifyPasswordResultStatProc(resultCode);
		break;	
		
	case AMS_CRM_FORCE_LOGIN_RESULT: 
//		AmsCrmForceLoginResultStatProc(resultCode);
		break;
		
	case AMS_CRM_QUERY_INFO_RESULT: 
//		AmsCrmQueryInfoResultStatProc(resultCode);
		break;

	case AMS_MANAGER_SET_CRM_STATE_RESULT: 
//		AmsManagerSetCrmStateResultStatProc(resultCode);
		break;	
		
	case AMS_CRM_EVENT_NOTICE_RESULT: 
		AmsCrmEventNoticeResultStatProc(resultCode);
		break;	

	//	ams <-> cms
	case AMS_CMS_CRM_REG_RESULT: 
		AmsCmsCrmRegResultStatProc(resultCode);
		break;

	case AMS_CMS_GET_CRM_RESULT: 
		AmsCmsGetCrmResultStatProc(resultCode);
		break;

	case AMS_CMS_CRM_CALLOUT_RESULT:
		AmsCmsCrmCalloutStatProc(resultCode);
		break;
	case AMS_CMS_CRM_AUTHINFO_RESULT:
		//AmsCmsCrmAuthinfoStatProc(resultCode);
		break;
	case AMS_CMS_EVENT_NOTICE_RESULT: 
		AmsCmsEventNoticeResultStatProc(resultCode);
		break;
		
	case AMS_CMS_EVENT_IND_RESULT: 
		//AmsCmsEventIndResultStatProc(resultCode);
		break;
		
	default:
		AmsResultStat.amsUnknownResultType++;
		return AMS_ERROR;
	}

	return AMS_OK;
}


//case is too many ...
int AmsCrmMsgStatProc(int msgCode)
{
	switch(msgCode)
	{
	//ams <-> crm msg begin
	case A_TELLER_LOGIN_REQ: 
		AmsMsgStat.recvCrmLoginReq++;
		break;
		
	case A_TELLER_LOGIN_RSP: 
		AmsMsgStat.sendCrmLoginRsp++;
		break;		      

	case A_TELLER_LOGOUT_REQ: 
		AmsMsgStat.recvCrmLogoutReq++;
		break;	
		
	case A_TELLER_LOGOUT_RSP: 
		AmsMsgStat.sendCrmLogoutRsp++;
		break;

	case A_TELLER_STATE_OPERATE_REQ: 
		AmsMsgStat.recvCrmStateOperateReq++;
		break;
		
	case A_TELLER_STATE_OPERATE_RSP: 
		AmsMsgStat.sendCrmStateOperateRsp++;
		break;		      

	case A_TELLER_STATE_OPERATE_IND: 
		AmsMsgStat.sendCrmStateOperateInd++;
		break;	
		
	case A_TELLER_STATE_OPERATE_CNF: 
		AmsMsgStat.recvCrmStateOperateCnf++;
		break;
		
	case A_TELLER_MODIFY_PASSWORD_REQ: 
		AmsMsgStat.recvCrmModifyPasswordReq++;
		break;	
		
	case A_TELLER_MODIFY_PASSWORD_RSP: 
		AmsMsgStat.sendCrmModifyPasswordRsp++;
		break;
		
	case A_TELLER_FORCE_LOGIN_REQ: 
		AmsMsgStat.recvCrmForceLoginReq++;
		break;
		
	case A_TELLER_FORCE_LOGIN_RSP: 
		AmsMsgStat.sendCrmForceLoginRsp++;
		break;	
		
	case A_TELLER_QUERY_INFO_REQ: 
		AmsMsgStat.recvCrmQueryInfoReq++;
		break;
		
	case A_TELLER_QUERY_INFO_RSP: 
		AmsMsgStat.sendCrmQueryInfoRsp++;
		break;
		
	case A_TELLER_EVENT_NOTICE: 
		AmsMsgStat.recvCrmEventNotice++;
		break;	

	case A_TELLER_EVENT_IND: 
		AmsMsgStat.sendCrmEventInd++;
		break;	
	
    //ams <-> crm msg end

	default:
		AmsMsgStat.amsUnknownMsg++;
		return AMS_ERROR;
	}
	
	return AMS_OK;
}

int AmsCmsMsgStatProc(int msgCode)
{
	switch(msgCode)
	{
	//ams <-> cms msg begin
	case A_TELLER_REG_REQ: 
		AmsMsgStat.recvCrmRegReq++;
		break;
		
	case A_TELLER_REG_RSP: 
		AmsMsgStat.sendCrmRegRsp++;
		break;	
		
	case A_TELLER_GET_REQ: 
		AmsMsgStat.recvCrmGetReq++;
		break;
		
	case A_TELLER_GET_RSP: 
		AmsMsgStat.sendCrmGetRsp++;
		break;

	case A_TELLER_CALLOUT_REQ:
		AmsMsgStat.recvCrmCalloutReq++;
		break;

	case A_TELLER_CALLOUT_RSP:
		AmsMsgStat.sendCrmCalloutRsp++;
		break;

	case A_TELLER_AUTH_INFO_QUERY_REQ:
		AmsMsgStat.recvCrmAuthinfoReq++;
		break;

	case A_TELLER_AUTH_INFO_QUERY_RSP:
		AmsMsgStat.sendCrmAuthinfoRsp++;
		break;
		
	case A_AMS_CALL_EVENT_NOTICE: 
		AmsMsgStat.recvAmsCallEventNoticeReq++;
		break;
		
	case A_AMS_CALL_EVENT_IND: 
		AmsMsgStat.sendAmsCallEventInd++;
		break;
	
	//ams <-> cms msg end

	default:
		AmsMsgStat.amsUnknownMsg++;
		return AMS_ERROR;
	}
	
	return AMS_OK;
}


int AmsBMsgStatProc(int msgCode)
{
	switch(msgCode)
	{
	//ams B Area msg begin
	case B_AMS_TELLER_STATE_OP_IND_TIMEOUT: 
		AmsMsgStat.crmStateOperateIndTimeout++;
		break;
		
	case B_AMS_TELLER_REST_TIMEOUT: 
		AmsMsgStat.restTimeout++;
		break;
		
	case B_AMS_CUSTOMER_IN_QUEUE_TIMEOUT: 
		AmsMsgStat.customerInQueueTimeout++;
		break;

	//ams B Area msg end
	
	default:
		AmsMsgStat.amsUnknownBMsg++;
		return AMS_ERROR;
	}
	
	return AMS_OK;
}


//Only One pThread !!!
int AmsMsgStatProc(int msgType, int msgCode)
{
	switch(msgType)
	{
	case AMS_CRM_MSG: 
		AmsCrmMsgStatProc(msgCode);
		break;
		
	case AMS_CMS_MSG: 
		AmsCmsMsgStatProc(msgCode);
		break;	
		
	case AMS_B_MSG: 
		AmsBMsgStatProc(msgCode);
		break;
		
	default:
		AmsMsgStat.amsUnknownMsgType++;
		return AMS_ERROR;
	}

	return AMS_OK;
}


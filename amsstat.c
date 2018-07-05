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
	case T_AMS_VTA_STATE_OPERATE_IND_TIMER: 
		AmsTimerStat.WaitVtaOperateIndRsp[action]++;
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

int AmsVtaLoginResultStatProc(int resultCode)
{
	switch(resultCode)
	{
	case AMS_VTA_QUEUE_MNG_SUCCESS:
		AmsResultStat.vtaLoginSuccess++;
		break;
		
	case AMS_VTA_LOGIN_PARA_ERR: //AMS_VTA_QUEUE_MNG_REASON_BASE
		AmsResultStat.vtaLoginParaErr++;
		break; 
		
	case AMS_VTA_LOGIN_LP_RESOURCE_LIMITED: 
		AmsResultStat.vtaLoginLpResourceLimited++;
		break;		
		
	case AMS_VTA_LOGIN_NODE_RESOURCE_LIMITED: 
		AmsResultStat.vtaLoginNodeResourceLimited++;
		break;
		
	case AMS_VTA_LOGIN_TELLER_LEN_ERR: 
		AmsResultStat.vtaLoginTellerLenErr++;
		break;

	case AMS_VTA_LOGIN_TELLER_NO_ERR: 
		AmsResultStat.vtaLoginTellerNoErr++;
		break;
		
	case AMS_VTA_LOGIN_TELLER_PWD_ERR: 
		AmsResultStat.vtaLoginTellerPwdErr++;
		break;	
		
	case AMS_VTA_LOGIN_TELLER_LOGIN_REPEATEDLY: 
		AmsResultStat.vtaLoginTellerLoginRepeatedly++;
		break;
 
	case AMS_VTA_LOGIN_TELLER_NUM_ERR: 
		AmsResultStat.vtaLoginTellerNumErr++;
		break;	
	default:
		AmsResultStat.amsUnknownResult++;
		return AMS_ERROR;
	}
	
	return AMS_OK;
}


int AmsVtaStateOperateResultStatProc(int resultCode)
{
	switch(resultCode)
	{
	case AMS_VTA_QUEUE_MNG_SUCCESS:
		AmsResultStat.vtaStateOperateSuccess++;
		break;
				
	case AMS_VTA_STATE_OPERATE_PARA_ERR:  
		AmsResultStat.vtaStateOperateParaErr++;
		break;
		
	case AMS_VTA_STATE_OPERATE_STATE_ERR: 
		AmsResultStat.vtaStateOperateStateErr++;
		break;
		
	case AMS_VTA_STATE_OPERATE_LEN_ERR: 
		AmsResultStat.vtaStateOperateLenErr++;
		break;
		
	case AMS_VTA_STATE_OPERATE_AMS_PID_ERR: 
		AmsResultStat.vtaStateOperateAmsPidErr++;
		break;		
		
	case AMS_VTA_STATE_OPERATE_TELLER_ID_ERR: 
		AmsResultStat.vtaStateOperateTellerIdErr++;
		break;
		
	case AMS_VTA_STATE_OPERATE_SERVICE_GROUP_ID_ERR: 
		AmsResultStat.vtaStateOperateSrvGrpIdErr++;
		break;

	case AMS_VTA_STATE_OPERATE_SERVICE_STATE_ERR: 
		AmsResultStat.vtaStateOperateServiceStateErr++;
		break;
		
	case AMS_VTA_STATE_OPERATE_CODE_ERR: 
		AmsResultStat.vtaStateOperateCodeErr++;
		break;	

	case AMS_VTA_STATE_OPERATE_STATE_OP_REASON_ERR: 
		AmsResultStat.vtaStateOperateOpReasonErr++;
		break;
		
	case AMS_VTA_STATE_OPERATE_STATE_OP_TIMELEN_ERR: 
		AmsResultStat.vtaStateOperateOpTimeLenErr++;
		break;
		
	case AMS_VTA_STATE_OPERATE_UPDATE_STATE_ERR: 
		AmsResultStat.vtaStateOperateUpdateStateErr++;
		break;
		
	case AMS_VTA_STATE_OPERATE_START_TIMER_ERR: 
		AmsResultStat.vtaStateOperateStartTimerErr++;
		break;
		
	case AMS_VTA_STATE_OPERATE_REST_TIMEOUT: 
		AmsResultStat.vtaStateOperateRestTimeout++;
		break;
		
	case AMS_VTA_STATE_OPERATE_REST_TIMEOUT_PARA_ERR: 
		AmsResultStat.vtaStateOpRestTimeoutParaErr++;
		break;
	
	case AMS_VTA_STATE_OPERATE_REST_TIMEOUT_STATE_ERR: 
		AmsResultStat.vtaStateOpRestTimeoutStateErr++;
		break;
		
	case AMS_VTA_STATE_OPERATE_REST_TIMEOUT_LEN_ERR: 
		AmsResultStat.vtaStateOpRestTimeoutLenErr++;
		break;
		
	case AMS_VTA_STATE_OPERATE_REST_TIMEOUT_AMS_PID_ERR: 
		AmsResultStat.vtaStateOpRestTimeoutAmsPidErr++;
		break;
		
	case AMS_VTA_STATE_OPERATE_REST_TIMEOUT_TELLER_ID_ERR: 
		AmsResultStat.vtaStateOpRestTimeoutTellerIdErr++;
		break;

	case AMS_VTA_STATE_OPERATE_REST_TIMEOUT_SERVICE_GROUP_ID_ERR: 
		AmsResultStat.vtaStateOpRestTimeoutSrvGrpIdErr++;
		break;
		
	case AMS_VTA_STATE_OPERATE_REST_TIMEOUT_SERVICE_STATE_ERR: 
		AmsResultStat.vtaStateOpRestTimeoutSrvGrpIdErr++;
		break;
		
	default:
		AmsResultStat.amsUnknownResult++;
		return AMS_ERROR;	
	}
	return AMS_OK;
}

int AmsVtaEventNoticeResultStatProc(int resultCode)
{		
	switch(resultCode)
	{
	case AMS_VTA_QUEUE_MNG_SUCCESS:
		AmsResultStat.vtaEventNoticeSuccess++;
		break;
		
	case AMS_VTA_EVENT_NOTICE_PARA_ERR: 
		AmsResultStat.vtaEventNoticeParaErr++;
		break;
		
	case AMS_VTA_EVENT_NOTICE_STATE_ERR:
		AmsResultStat.vtaEventNoticeStateErr++;
		break;
		
	case AMS_VTA_EVENT_NOTICE_LEN_ERR: 
		AmsResultStat.vtaEventNoticeLenErr++;
		break;
		
	case AMS_VTA_EVENT_NOTICE_SERVICE_GROUP_ID_ERR: 
		AmsResultStat.vtaEventNoticeSrvGrpIdErr++;
		break;
		
	case AMS_VTA_EVENT_NOTICE_SERVICE_STATE_ERR: 
		AmsResultStat.vtaEventNoticeServiceStateErr++;
		break;		

	case AMS_VTA_EVENT_NOTICE_CALL_ID_ERR: 
		AmsResultStat.vtaEventNoticeCallIdErr++;
		break;	

	case AMS_VTA_EVENT_NOTICE_CALL_STATE_ERR: 
		AmsResultStat.vtaEventNoticeCallStateErr++;
		break;
		
	case AMS_VTA_EVENT_NOTICE_AMS_PID_ERR: 
		AmsResultStat.vtaEventNoticeAmsPidErr++;
		break;

	case AMS_VTA_EVENT_NOTICE_TELLER_ID_ERR: 
		AmsResultStat.vtaEventNoticeTellerIdErr++;
		break;
		
	case AMS_VTA_EVENT_NOTICE_VTM_ID_ERR: 
		AmsResultStat.vtaEventNoticeVtmIdErr++;
		break;	 

	case AMS_VTA_EVENT_NOTICE_TYPE_ERR: 
		AmsResultStat.vtaEventNoticeTypeErr++;
		break;	 

	case AMS_VTA_EVENT_NOTICE_LACK_FILE_INFO: 
		AmsResultStat.vtaEventNoticeLackFileInfo++;
		break;	
		
	case AMS_VTA_EVENT_NOTICE_FILE_PATH_LEN_ERR: 
		AmsResultStat.vtaEventNoticeFilePathLenErr++;
		break;	 
		
	case AMS_VTA_EVENT_NOTICE_FILE_NAME_LEN_ERR: 
		AmsResultStat.vtaEventNoticeFileNameLenErr++;
		break;	 
	
	default:
		AmsResultStat.amsUnknownResult++;
		return AMS_ERROR;
	}

	return AMS_OK;
}


int AmsCmsVtaRegResultStatProc(int resultCode)
{
	switch(resultCode)
	{
	case AMS_CMS_PRCOESS_SUCCESS:
		AmsResultStat.cmsVtaRegSuccess++;
		break;
				
	case AMS_CMS_VTA_REG_PARA_ERR: 
		AmsResultStat.cmsVtaRegParaErr++;
		break;
		
	case AMS_CMS_VTA_REG_STATE_ERR: 
		AmsResultStat.cmsVtaRegStateErr++;
		break;
		
	case AMS_CMS_VTA_REG_LEN_ERR: 
		AmsResultStat.cmsVtaRegLenErr++;
		break;
		
	case AMS_CMS_VTA_REG_TELLER_ID_ERR: 
		AmsResultStat.cmsVtaRegTellerIdErr++;
		break;		
		
	case AMS_CMS_VTA_REG_RESOURCE_LIMITED: 
		AmsResultStat.cmsVtaRegResourceLimited++;
		break;
		 
	default:
		AmsResultStat.amsUnknownResult++;
		return AMS_ERROR;
	}
	
	return AMS_OK;
}

int AmsCmsGetVtaResultStatProc(int resultCode)
{
	switch(resultCode)
	{
	case AMS_CMS_PRCOESS_SUCCESS:
		AmsResultStat.cmsGetVtaSuccess++;
		break;
 	
	case AMS_CMS_GET_VTA_PARA_ERR: 
		AmsResultStat.cmsGetVtaParaErr++;
		break;
		
	case AMS_CMS_GET_VTA_LEN_ERR: 
		AmsResultStat.cmsGetVtaLenErr++;
		break;		
		
	case AMS_CMS_GET_VTA_AMS_PID_ERR: 
		AmsResultStat.cmsGetVtaAmsPidErr++;
		break;
		
	case AMS_CMS_GET_VTA_CALL_ID_ERR: 
		AmsResultStat.cmsGetVtaCallIdErr++;
		break;

	case AMS_CMS_GET_VTA_TERMINAL_TYPE_ERR: 
		AmsResultStat.cmsGetVtaTerminalTypeErr++;
		break;
		
	case AMS_CMS_GET_VTA_SERVICE_GROUP_ID_ERR: 
		AmsResultStat.cmsGetVtaSrvGrpIdErr++;
		break;	
		
	case AMS_CMS_GET_VTA_SERVICE_TYPE_ERR: 
		AmsResultStat.cmsGetVtaServiceTypeErr++;
		break;

	case AMS_CMS_GET_VTA_NO_VALID_SERVICE_GROUP_ID_OR_TYPE: 
		AmsResultStat.cmsGetVtaNoValidSrvGrpOrType++;
		break;
	
	case AMS_CMS_GET_VTA_CALL_TYPE_ERR: 
		AmsResultStat.cmsGetVtaCallTypeErr++;
		break;
		
	case AMS_CMS_GET_VTA_ORIGIN_TELLER_ID_ERR: 
		AmsResultStat.cmsGetVtaOriginTellerIdErr++;
		break;

	case AMS_CMS_GET_VTA_ORIGIN_TELLER_STATE_ERR: 
		AmsResultStat.cmsGetVtaOriginTellerStateErr++;
		break;
		
	case AMS_CMS_GET_VTA_ORIGIN_TELLER_AMS_PID_ERR: 
		AmsResultStat.cmsGetVtaOriginTellerAmsPidErr++;
		break;

	case AMS_CMS_GET_VTA_ORIGIN_TELLER_SERVICE_GROUP_ID_ERR: 
		AmsResultStat.cmsGetVtaOriginTellerSrvGrpIdErr++;
		break;
		
	case AMS_CMS_GET_VTA_ORIGIN_TELLER_SERVICE_STATE_ERR: 
		AmsResultStat.cmsGetVtaOriginTellerSrvStateErr++;
		break;
		
	case AMS_CMS_GET_VTA_TARGET_TYPE_ERR: 
		AmsResultStat.cmsGetVtaTargetTypeErr++;
		break;
		
	case AMS_CMS_GET_VTA_TARGET_GROUP_ERR: 
		AmsResultStat.cmsGetVtaTargetGroupErr++;
		break;
		
	case AMS_CMS_GET_VTA_TARGET_TELLER_ID_ERR: 
		AmsResultStat.cmsGetVtaTargetTellerIdErr++;
		break;
			
	case AMS_CMS_GET_VTA_TARGET_TELLER_STATE_ERR: 
		AmsResultStat.cmsGetVtaTargetTellerStateErr++;
		break;
		
	case AMS_CMS_GET_VTA_VTM_ID_ERR: 
		AmsResultStat.cmsGetVtaVtmIdErr++;
		break;
		
	case AMS_CMS_GET_VTA_ASSOC_CALL_ID_ERR: 
		AmsResultStat.cmsGetVtaAssocCallIdErr++;
		break;	
		
	case AMS_CMS_GET_VTA_LICENSE_TIMEOUT: 
		AmsResultStat.cmsGetVtaLicenseTimeout++;
		break;	
		
	case AMS_CMS_GET_VTA_TPS_BEYOND_LIC: 
		AmsResultStat.cmsGetVtaTpsBeyondLic++;
		break;	
		
	case AMS_CMS_GET_VTA_SERVICE_IN_QUEUE: 
		AmsResultStat.cmsGetVtaServiceInQueue++;
		break;
		
	case AMS_CMS_GET_VTA_SERVICE_INTELLIGENT_ROUTING_ERR: 
		AmsResultStat.cmsGetVtaSIRErr++;
		break;

	case AMS_CMS_GET_VTA_SIR_VTMNO_ERR: 
		AmsResultStat.cmsGetVtaSIRVtmNoErr++;
		break;
		
	case AMS_CMS_GET_VTA_SIR_SERVICE_GROUP_ID_ERR: 
		AmsResultStat.cmsGetVtaSIRSrvGrpIdErr++;
		break;
		
	case AMS_CMS_GET_VTA_SIR_CALL_TYPE_ERR: 
		AmsResultStat.cmsGetVtaSIRCallTypeErr++;
		break;
		
	case AMS_CMS_GET_VTA_SIR_VTA_EMPTY: 
		AmsResultStat.cmsGetVtaSIRVtaEmpty++;
		break;
		
	case AMS_CMS_GET_VTA_SIR_CALC_QUEUE_INFO_ERR: 
		AmsResultStat.cmsGetVtaSIRCalcQueueInfoErr++;
		break;
		
	case AMS_CMS_GET_VTA_SIR_TOO_MANY_CUSTOMER_IN_QUEUE: 
		AmsResultStat.cmsGetVtaSIRTooManyCustInQueue++;
		break;
		
	case AMS_CMS_GET_VTA_SIR_TARGET_TELLER_NOT_LOGIN: 
		AmsResultStat.cmsGetVtaSIRTargetTellerNotLogin++;
		break;
		
	case AMS_CMS_GET_VTA_SIR_TARGET_TELLER_NOT_REGISTERED: 
		AmsResultStat.cmsGetVtaSIRTargetTellerNotReg++;
		break;
		
	case AMS_CMS_GET_VTA_SIR_TARGET_TELLER_STATE_ERR: 
		AmsResultStat.cmsGetVtaSIRTargetTellerStateErr++;
		break;
	
	case AMS_CMS_GET_VTA_SIR_CALL_TRANSFER_TOO_MANY_TIMES: 
		AmsResultStat.cmsGetVtaSIRCallTransTooManyTimes++;
		break;
		
	case AMS_CMS_GET_VTA_ORG_ID_ERR: 
		AmsResultStat.cmsGetVtaOrgIdErr++;
		break;
	
	case AMS_CMS_GET_VTA_ORG_POS_ERR: 
		AmsResultStat.cmsGetVtaOrgPosErr++;
		break;
		
	case AMS_CMS_GET_VTA_PARENT_ORG_ID_ERR: 
		AmsResultStat.cmsGetVtaParentOrgIdErr++;
		break;
	
	case AMS_CMS_GET_VTA_SELECT_SERVICE_GROUP_ID_ERR: 
		AmsResultStat.cmsGetVtaSelectSrvGrpIdErr++;
		break;	
		
	case AMS_CMS_GET_VTA_LP_RESOURCE_LIMITED: 
		AmsResultStat.cmsGetVtaLpResourceLimited++;
		break;
		
	case AMS_CMS_GET_VTA_CREATE_TIMER_ERR: 
		AmsResultStat.cmsGetVtaCreateTimerErr++;
		break;
		
	case AMS_CMS_GET_VTA_TIMEOUT: 
		AmsResultStat.cmsGetVtaTimeout++;
		break;
		
	case AMS_CMS_GET_VTA_TIMEOUT_PARA_ERR: 
		AmsResultStat.cmsGetVtaTimeoutParaErr++;
		break;
		
	case AMS_CMS_GET_VTA_TIMEOUT_STATE_ERR: 
		AmsResultStat.cmsGetVtaTimeoutStateErr++;
		break;

	case AMS_CMS_GET_VTA_TIMEOUT_LEN_ERR: 
		AmsResultStat.cmsGetVtaTimeoutLenErr++;
		break;
		
	case AMS_CMS_GET_VTA_TIMEOUT_VTM_ID_ERR: 
		AmsResultStat.cmsGetVtaTimeoutVtmIdErr++;
		break;
		
	case AMS_CMS_GET_VTA_TIMEOUT_SERVICE_STATE_ERR: 
		AmsResultStat.cmsGetVtaTimeoutServiceStateErr++;
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
		
	case AMS_CMS_EVENT_NOTICE_VTM_ID_ERR: 
		AmsResultStat.cmsEventNoticeVtmIdErr++;
		break;
		
	case AMS_CMS_EVENT_NOTICE_CODE_ERR: 
		AmsResultStat.cmsEventNoticeCodeErr++;
		break;
		
	case AMS_CMS_EVENT_NOTICE_VTA_STATE_ERR: 
		AmsResultStat.cmsEventNoticeVtaStateErr++;
		break;
		
	case AMS_CMS_EVENT_NOTICE_VTM_STATE_ERR: 
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

int AmsCmsVtaCalloutStatProc(int resultCode)
{
	switch(resultCode)
	{
	case AMS_CMS_PRCOESS_SUCCESS:
		AmsResultStat.cmsvtaCalloutSuccess++;
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
	case AMS_CMS_CALLOUT_VTA_CALL_ID_ERR:
		AmsResultStat.cmsCalloutvtaCallidErr++;
		break;
	case AMS_CMS_CALLOUT_VTA_TELLER_ID_ERR:
		AmsResultStat.cmsCalloutvtaTelleridErr++;
		break;
	case AMS_CMS_CALLOUT_VTA_SRVGRP_ID_ERR:
		AmsResultStat.cmsCalloutvtaSrvgrpidErr++;
		break;
	case AMS_CMS_CALLOUT_VTA_SRVTYPE_ERR:
		AmsResultStat.cmsCalloutvtaSrvtypeErr++;
		break;
	case AMS_CMS_CALLOUT_VTA_TELLER_ID_NOTCFG_ERR:
		AmsResultStat.cmsCalloutvtaTelleridNotcfgErr++;
		break;
	case AMS_CMS_CALLOUT_VTA_TELLER_ID_NOT_REG_ERR:
		AmsResultStat.cmsCalloutvtaTelleridNotregErr++;
		break;
	case AMS_CMS_CALLOUT_VTA_TELLER_ID_NOTLOGIN_ERR:
		AmsResultStat.cmsCalloutvtaTelleridNotloginErr++;
		break;
	case AMS_CMS_CALLOUT_VTAREPEAT_ERR:
		AmsResultStat.cmsCalloutvtaRepeatErr++;
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
	// ams <-> vta
	case AMS_COMMON_RESULT: 
		AmsCommonResultStatProc(resultCode);
		break;
		
	case AMS_VTA_LOGIN_RESULT: 
		AmsVtaLoginResultStatProc(resultCode);
		break;
		
	case AMS_VTA_LOGOUT_RESULT: 
		//AmsVtaLogoutResultStatProc(resultCode);
		break;	
		
	case AMS_VTA_STATE_OPERATE_RESULT: 
		AmsVtaStateOperateResultStatProc(resultCode);
		break;
		
	case AMS_VTA_MODIFY_PASSWORD_RESULT: 
		//AmsVtaModifyPasswordResultStatProc(resultCode);
		break;	
		
	case AMS_VTA_FORCE_LOGIN_RESULT: 
//		AmsVtaForceLoginResultStatProc(resultCode);
		break;
		
	case AMS_VTA_QUERY_INFO_RESULT: 
//		AmsVtaQueryInfoResultStatProc(resultCode);
		break;

	case AMS_MANAGER_SET_VTA_STATE_RESULT: 
//		AmsManagerSetVtaStateResultStatProc(resultCode);
		break;	
		
	case AMS_VTA_EVENT_NOTICE_RESULT: 
		AmsVtaEventNoticeResultStatProc(resultCode);
		break;	

	//	ams <-> cms
	case AMS_CMS_VTA_REG_RESULT: 
		AmsCmsVtaRegResultStatProc(resultCode);
		break;

	case AMS_CMS_GET_VTA_RESULT: 
		AmsCmsGetVtaResultStatProc(resultCode);
		break;

	case AMS_CMS_VTA_CALLOUT_RESULT:
		AmsCmsVtaCalloutStatProc(resultCode);
		break;
	case AMS_CMS_VTA_AUTHINFO_RESULT:
		//AmsCmsVtaAuthinfoStatProc(resultCode);
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
int AmsVtaMsgStatProc(int msgCode)
{
	switch(msgCode)
	{
	//ams <-> vta msg begin
	case A_VTA_LOGIN_REQ: 
		AmsMsgStat.recvVtaLoginReq++;
		break;
		
	case A_VTA_LOGIN_RSP: 
		AmsMsgStat.sendVtaLoginRsp++;
		break;		      

	case A_VTA_LOGOUT_REQ: 
		AmsMsgStat.recvVtaLogoutReq++;
		break;	
		
	case A_VTA_LOGOUT_RSP: 
		AmsMsgStat.sendVtaLogoutRsp++;
		break;

	case A_VTA_STATE_OPERATE_REQ: 
		AmsMsgStat.recvVtaStateOperateReq++;
		break;
		
	case A_VTA_STATE_OPERATE_RSP: 
		AmsMsgStat.sendVtaStateOperateRsp++;
		break;		      

	case A_VTA_STATE_OPERATE_IND: 
		AmsMsgStat.sendVtaStateOperateInd++;
		break;	
		
	case A_VTA_STATE_OPERATE_CNF: 
		AmsMsgStat.recvVtaStateOperateCnf++;
		break;
		
	case A_VTA_MODIFY_PASSWORD_REQ: 
		AmsMsgStat.recvVtaModifyPasswordReq++;
		break;	
		
	case A_VTA_MODIFY_PASSWORD_RSP: 
		AmsMsgStat.sendVtaModifyPasswordRsp++;
		break;
		
	case A_VTA_FORCE_LOGIN_REQ: 
		AmsMsgStat.recvVtaForceLoginReq++;
		break;
		
	case A_VTA_FORCE_LOGIN_RSP: 
		AmsMsgStat.sendVtaForceLoginRsp++;
		break;	
		
	case A_VTA_QUERY_INFO_REQ: 
		AmsMsgStat.recvVtaQueryInfoReq++;
		break;
		
	case A_VTA_QUERY_INFO_RSP: 
		AmsMsgStat.sendVtaQueryInfoRsp++;
		break;
		
	case A_VTA_EVENT_NOTICE: 
		AmsMsgStat.recvVtaEventNotice++;
		break;	

	case A_VTA_EVENT_IND: 
		AmsMsgStat.sendVtaEventInd++;
		break;	
	
    //ams <-> vta msg end

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
	case A_VTA_REG_REQ: 
		AmsMsgStat.recvVtaRegReq++;
		break;
		
	case A_VTA_REG_RSP: 
		AmsMsgStat.sendVtaRegRsp++;
		break;	
		
	case A_VTA_GET_REQ: 
		AmsMsgStat.recvVtaGetReq++;
		break;
		
	case A_VTA_GET_RSP: 
		AmsMsgStat.sendVtaGetRsp++;
		break;

	case A_VTA_CALLOUT_REQ:
		AmsMsgStat.recvVtaCalloutReq++;
		break;

	case A_VTA_CALLOUT_RSP:
		AmsMsgStat.sendVtaCalloutRsp++;
		break;

	case A_AUTHINFO_QUERY_REQ:
		AmsMsgStat.recvVtaAuthinfoReq++;
		break;

	case A_AUTHINFO_QUERY_RSP:
		AmsMsgStat.sendVtaAuthinfoRsp++;
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
	case B_AMS_VTA_STATE_OP_IND_TIMEOUT: 
		AmsMsgStat.vtaStateOperateIndTimeout++;
		break;
		
	case B_AMS_REST_TIMEOUT: 
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
	case AMS_VTA_MSG: 
		AmsVtaMsgStatProc(msgCode);
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


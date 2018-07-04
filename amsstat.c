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

//Only One pThread !!!
int AmsResultStatProc(int resultType, int resultCode)
{ 
/*	switch(resultType)
	{
	// ams <-> vta
	case AMS_COMMON_RESULT: 
		AmsCommonResultStatProc(resultCode);
		break;
		
	case AMS_VTA_LOGIN_RESULT: 
		AmsVtaLoginResultStatProc(resultCode);
		break;
		
	case AMS_VTA_LOGOUT_RESULT: 
		AmsVtaLogoutResultStatProc(resultCode);
		break;	

	case AMS_VTA_HANDSHAKE_RESULT: 
		AmsVtaHandshakeResultStatProc(resultCode);
		break;
		
	case AMS_VTA_STATE_OPERATE_RESULT: 
		AmsVtaStateOperateResultStatProc(resultCode);
		break;
		
	case AMS_VTA_MODIFY_PASSWORD_RESULT: 
		AmsVtaModifyPasswordResultStatProc(resultCode);
		break;	

	case AMS_VTA_SERVICE_REG_RESULT: 
		AmsVtaServiceRegResultStatProc(resultCode);
		break;

	case AMS_VTA_TRANSFER_CALL_RESULT: 
		AmsVtaTransferCallResultStatProc(resultCode);
		break;
		
	case AMS_VTA_VOLUME_CTRL_RESULT: 
		AmsVtaVolumeCtrlResultStatProc(resultCode);
		break;	

	case AMS_VTA_AUDIO_RECORD_RESULT: 
		AmsVtaAudioRecordResultStatProc(resultCode);
		break;

	case AMS_VTA_SCREEN_RECORD_RESULT: 
		AmsVtaScreenRecordResultStatProc(resultCode);
		break;
		
	case AMS_VTA_REMOTE_COOPERATIVE_RESULT: 
		AmsVtaRemoteCooperativeResultStatProc(resultCode);
		break;	
		
	case AMS_VTA_SNAP_RESULT: 
		AmsVtaSnapResultStatProc(resultCode);
		break;

	case AMS_VTA_SEND_MSG_RESULT: 
		AmsVtaSendMsgResultStatProc(resultCode);
		break;

	case AMS_VTA_RECV_MSG_RESULT:
		AmsVtaRecvMsgResultStatProc(resultCode);
		break;
		
	case AMS_VTA_SEND_FILE_RESULT: 
		AmsVtaSendFileResultStatProc(resultCode);
		break;

	case AMS_VTA_RECV_FILE_RESULT: 
		AmsVtaRecvFileResultStatProc(resultCode);
		break;
		
	case AMS_VTA_MULTI_SESS_RESULT: 
		AmsVtaMultiSessResultStatProc(resultCode);
		break;
		
	case AMS_VTA_FORCE_LOGIN_RESULT: 
		AmsVtaForceLoginResultStatProc(resultCode);
		break;
		
	case AMS_VTA_QUERY_INFO_RESULT: 
		AmsVtaQueryInfoResultStatProc(resultCode);
		break;

	// ams <-> vta manager
	case AMS_MANAGER_ADD_VTA_RESULT: 
		AmsManagerAddVtaResultStatProc(resultCode);
		break;
		
	case AMS_MANAGER_DEL_VTA_RESULT: 
		AmsManagerDelVtaResultStatProc(resultCode);
		break;
		
	case AMS_MANAGER_MODIFY_VTA_RESULT: 
		AmsManagerModifyVtaResultStatProc(resultCode);
		break;	

	case AMS_MANAGER_QUERY_VTA_RESULT: 
		AmsManagerQueryVtaResultStatProc(resultCode);
		break;	
		
	case AMS_MANAGER_FORCE_LOGOUT_VTA_RESULT: 
		AmsManagerForceLogoutVtaResultStatProc(resultCode);
		break;
		
	case AMS_MANAGER_SET_VTA_STATE_RESULT: 
		AmsManagerSetVtaStateResultStatProc(resultCode);
		break;	

	case AMS_MANAGER_QUERY_VTA_STATE_RESULT: 
		AmsManagerQueryVtaStateResultStatProc(resultCode);
		break;	
		
	case AMS_MANAGER_MODIFY_QUEUE_RULE_RESULT: 
		AmsManagerModifyQueueRuleResultStatProc(resultCode);
		break;	
		
	case AMS_MANAGER_FORCE_REL_CALL_RESULT:
		AmsManagerForceRelCallResultStatProc(resultCode);
		break;

	// ams <-> vta inspector
	case AMS_INSPECTOR_MONITOR_RESULT: 
		AmsInspectorMonitorVtaResultStatProc(resultCode);
		break;
		
	case AMS_VTA_BANDWIDTH_NOTICE_RESULT: 
		AmsVtaBandwidthNoticeResultStatProc(resultCode);
		break;
		
	case AMS_VTA_EVENT_NOTICE_RESULT: 
		AmsVtaEventNoticeResultStatProc(resultCode);
		break;	

	case AMS_VTA_PARA_CFG_RESULT: 
		AmsVtaParaCfgResultStatProc(resultCode);
		break;

	//	ams <-> vtm
	case AMS_VTM_LOGIN_RESULT: 
		AmsVtmLoginResultStatProc(resultCode);
		break;
		
	case AMS_VTM_LOGOUT_RESULT: 
		AmsVtmLogoutResultStatProc(resultCode);
		break;
		
	case AMS_VTM_HANDSHAKE_RESULT: 
		AmsVtmHandshakeResultStatProc(resultCode);
		break;		
	
	case AMS_VTM_SEND_MSG_RESULT: 
		AmsVtmSendMsgResultStatProc(resultCode);
		break;

	case AMS_VTM_RECV_MSG_RESULT: 
		AmsVtmRecvMsgResultStatProc(resultCode);
		break;

	case AMS_VTM_SEND_FILE_RESULT:
		AmsVtmSendFileResultStatProc(resultCode);
		break;

	case AMS_VTM_RECV_FILE_RESULT: 
		AmsVtmRecvFileResultStatProc(resultCode);
		break;

	case AMS_VTM_FORCE_LOGIN_RESULT: 
		AmsVtmForceLoginResultStatProc(resultCode);
		break;
		
	case AMS_VTM_QUERY_INFO_RESULT: 
		AmsVtmQueryInfoResultStatProc(resultCode);
		break;

	case AMS_VTM_BANDWIDTH_NOTICE_RESULT: 
		AmsVtmBandwidthNoticeResultStatProc(resultCode);
		break;
		
	case AMS_VTM_EVENT_NOTICE_RESULT: 
		AmsVtmEventNoticeResultStatProc(resultCode);
		break;	

	case AMS_VTM_PARA_CFG_RESULT: 
		AmsVtmParaCfgResultStatProc(resultCode);
		break;

	//	ams <-> cms
	case AMS_CMS_VTA_REG_RESULT: 
		AmsCmsVtaRegResultStatProc(resultCode);
		break;

	case AMS_CMS_VTM_REG_RESULT: 
		AmsCmsVtmRegResultStatProc(resultCode);
		break;
		
	case AMS_CMS_GET_VTA_RESULT: 
		AmsCmsGetVtaResultStatProc(resultCode);
		break;
		
	case AMS_CMS_QUERY_TERM_NETINFO_RESULT: 
		AmsCmsQueryTermNetInfoResultStatProc(resultCode);
		break;
		
	case AMS_CMS_EVENT_NOTICE_RESULT: 
		AmsCmsEventNoticeResultStatProc(resultCode);
		break;
		
	case AMS_CMS_EVENT_IND_RESULT: 
		AmsCmsEventIndResultStatProc(resultCode);
		break;

	//  ams <-> rcas
	case AMS_RCAS_HANDSHAKE_RESULT: 
		AmsRcasHandshakeResultStatProc(resultCode);
		break;

	case AMS_RCAS_LOADCAPC_NOTICE_RESULT:
		AmsRcasLoadCapcNoticeResultStatProc(resultCode);
		break;
		
	default:
		AmsResultStat.amsUnknownResultType++;
		return AMS_ERROR;
	}
*/	
	return AMS_OK;
}


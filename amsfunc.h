/******************************************************************
*
* FILE NAME						                           VERSION
*	
*	amsfunc.h	                                            v1.0.0
*
* DESCRIPTION
*
*	AMS应用管理服务器的函数定义文件
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
#ifndef AMSFUNC_H
#define AMSFUNC_H


#include "amspri.h"

#ifdef __cplusplus
extern "C" {
#endif

//func defined in ams.c
void *ServiceProcTask(void *pThreadId);
int ProcessAmsMessage(int iThreadId,MESSAGE_t *pMsg);

int AmsProcAMsg(int iThreadId,MESSAGE_t *pMsg);
int AmsProcVtaMsg(int iThreadId, MESSAGE_t *pMsg);
int AmsProcVtmMsg(int iThreadId, MESSAGE_t *pMsg);
int AmsProcCmsMsg(int iThreadId, MESSAGE_t *pMsg);
int AmsProcRcasMsg(int iThreadId, MESSAGE_t *pMsg);

int AmsProcBMsg(int iThreadId,MESSAGE_t *pMsg);
int AmsProcCMsg(int iThreadId,MESSAGE_t *pMsg);

//func defined in amslic.c
int AmsGetLicData();
int LicCheckLastRunTimeAndUsedTime(time_t currentTime, char rsvdData1Flag, char rsvdData2Flag, char *pLastRunTimeFlag, char *pUsedTimeFlag, unsigned int *pUsedTime);
int LicProcLastRunTimeAndUsedTime(time_t currentTime);
int LicProcFirstRunTime(time_t currentTime);
extern int AmsCheckLicTime();
int AmsCheckTps(int *pCurrTps);

extern int encrypt_3des_ecb(char *data, int dataLen, char *sKey, int mode, char *output);
extern int decrypt_3des_ecb(char *data, int dataLen, char *sKey, int mode, char *output);

//func defined in amscfg.c
int ConfigAmsSrv(char *cFileName);
int SrvDivSen(char *s,WORD_t *word);
int SrvGetAWord(char *string,WORD_t *word);
int ConfigSrv(WORD_t *word,int wordcount,int section,unsigned  int *pCurrId);

int AmsSrvServiceSenten(WORD_t *word,int wordcount,unsigned int *pCurrId);
int AmsSrvServiceGroupSenten(WORD_t *word,int wordcount,unsigned int *pCurrId);
int AmsSrvTellerSenten(WORD_t *word,int wordcount,unsigned int *pCurrId);
int AmsSrvAuthoritySenten(WORD_t *word,int wordcount,unsigned int *pCurrId);
int AmsSrvVtmSenten(WORD_t *word,int wordcount,unsigned int *pCurrId);
int AmsSrvRcasSenten(WORD_t *word,int wordcount,unsigned int *pCurrId);
int AmsSrvOrgSenten(WORD_t *word,int wordcount,unsigned int *pCurrId);
int AmsSrvQueueSenten(WORD_t *word,int wordcount,unsigned int *pCurrId);

int amsGetTellerType(char *stringword, char *pTellerType);
int amsGetSrvGrpId(char *stringword, char *pSrvGrpId);
int amsSetServiceStateByServiceName(char *serviceName);
int amsGetServiceTypeByServiceName(char *serviceName, unsigned int *pSrvType, unsigned int *pSrvRsvdType);
int amsGetServiceNameByServiceId(unsigned int serviceId, char *pServiceName);
int amsCheckServiceGroupServiceType(unsigned int srvGrpId, unsigned int serviceType, unsigned int srvTypeRsvd);
int amsGetTerminalType(char *stringword, char *pTerminalType);

int AmsParaCfgTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg);

//func defined in amsfunc.c
int AmsAllocPid(PID_t *spid);
int AmsReleassPid(PID_t sPid,int ret);
int AmsCreateTimer(int iPid,int *timerId, int timerType,int tmcnt);
int AmsCreateTimerPara(int iPid,int *timerId,int timerType,int tmcnt,unsigned char *para);
int AmsKillTimer(int iPid, int *timerId);
int AmsQueueCreateTimerPara(int iPid,int *timerId,int timerType,int tmcnt,unsigned char *para);
int AmsQueueKillTimer(int iPid, int *timerId);

char *AmsGetTimerName(int code);
char *AmsGetStateTypeString(int type,int state);

void AmsSetVtaState(int iThreadId,LP_AMS_DATA_t *lpAmsData,VTA_NODE *pVtaNode,int state,int stateReason);
void AmsManagerSetVtaState(int iThreadId,LP_AMS_DATA_t *lpAmsManagerData,LP_AMS_DATA_t *lpAmsData,VTA_NODE *pVtaNode,int state);
void AmsSetVtaCallState(LP_AMS_DATA_t *lpAmsData,VTA_NODE *pVtaNode,int state);
void AmsSetVtmState(int iThreadId,VTM_NODE *pVtmNode,int state);
void AmsSetVtmCallState(VTM_NODE *pVtmNode,int state);
void AmsSetVtmServiceState(VTM_NODE *pVtmNode,int state);

VTA_NODE *AmsSearchVtaNode(unsigned int srvGrpId, unsigned int tellerId);
int AmsUpdateVtaState(int iThreadId, LP_AMS_DATA_t *lpAmsData, VTA_NODE *pVtaNode, int stateOperate, int stateReason);
VTM_NODE *AmsSearchVtmNode(unsigned int srvGrpId, unsigned int vtmId);

int AmsManagerUpdateVtaState(int iThreadId, LP_AMS_DATA_t *lpAmsManagerData, LP_AMS_DATA_t *lpAmsData, VTA_NODE *pVtaNode, unsigned int stateSet);
int AmsGetStateOpIndFromStateSet(unsigned int stateSet,unsigned short *stateOpInd);
int AmsGetNewStateFromStateOpInd(unsigned short stateOpInd,unsigned int *newState);
int AmsUpdateRemoteCoopState(int iThreadId, LP_AMS_DATA_t *lpAmsData, unsigned char remoteCoopTarget, unsigned short remoteOp, int iret);
int AmsKillVtaAllTimer(LP_AMS_DATA_t *lpAmsData, int pid);
int AmsKillVtaAllCallTimer(LP_AMS_DATA_t *lpAmsData, int pid);
int AmsKillVtmAllTimer(VTM_NODE *pVtmNode, int pid);
int AmsKillVtmAllCallTimer(VTM_NODE *pVtmNode, int pid);


//zry added for scc 2018
void AmsSetSeatCallState(LP_AMS_DATA_t *lpAmsData,SEAT_NODE *pSeatNode,int state);
void AmsSetSeatState(int iThreadId,LP_AMS_DATA_t *lpAmsData,SEAT_NODE *pSeatNode,int state,int stateReason);
void AmsSetUserState(int iThreadId,USER_NODE *pUserNode,int state);
void AmsSetUserCallState(USER_NODE *pUserNode,int state);
void AmsSetUserServiceState(USER_NODE *pUserNode,int state);

SEAT_NODE *AmsSearchSeatNode(unsigned char srvgrpid,unsigned char tellerId[],unsigned char tellerIdlen);

//added end
int VtaIdListInit();
int VtmIdListInit();
int OrgIdListInit();

VTA_ID_NODE  *AmsGetVtaIdNode();
int AmsFreeVtaIdNode(VTA_ID_NODE *pNode);
static int AmsCalcVtaIdHashIdx(DWORD tellerId);
VTA_ID_NODE *AmsSearchVtaIdHash(DWORD tellerId);
void AmsInsertVtaIdHash(VTA_ID_NODE *pVtaIdNode);
void AmsDeleteVtaIdHash(VTA_ID_NODE *pVtaIdNode);

VTM_ID_NODE  *AmsGetVtmIdNode();
int AmsFreeVtmIdNode(VTM_ID_NODE *pNode);
static int AmsCalcVtmIdHashIdx(DWORD vtmId);
VTM_ID_NODE *AmsSearchVtmIdHash(DWORD vtmId);
void AmsInsertVtmIdHash(VTM_ID_NODE *pVtmIdNode);
void AmsDeleteVtmIdHash(VTM_ID_NODE *pVtmIdNode);

int AmsGetTellerNoById(unsigned int tellerId, unsigned char *pTellerNo);
int AmsGetVtmNoById(unsigned int vtmId, unsigned char *pVtmNo);

ORG_ID_NODE  *AmsGetOrgIdNode();
int AmsFreeOrgIdNode(ORG_ID_NODE *pNode);
static int AmsCalcOrgIdHashIdx(DWORD orgId);
ORG_ID_NODE *AmsSearchOrgIdHash(DWORD orgId);
void AmsInsertOrgIdHash(ORG_ID_NODE *pOrgIdNode);
void AmsDeleteOrgIdHash(ORG_ID_NODE *pOrgIdNode);


//func defined in amsvta.c
int VtaLoginReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaLogoutReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaHandshakeReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaStateOperateReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaStateOperateCnfProc(int iThreadId, MESSAGE_t *pMsg);
int VtaModifyPasswordReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaServiceRegReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaTransferCallReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaVolumeCtrlReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaAudioRecordReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaScreenRecordReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaRemoteCoopReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaSnapReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaSendMsgReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaRecvMsgCnfProc(int iThreadId, MESSAGE_t *pMsg);
int VtaSendFileReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaRecvFileCnfProc(int iThreadId, MESSAGE_t *pMsg);
int VtaInnerCallReqProc(int iThreadId, MESSAGE_t *pMsg);

int VtaMultiSessReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaMultiSessIndCnfProc(int iThreadId, MESSAGE_t *pMsg);

int VtaForceLoginReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaQueryInfoReqProc(int iThreadId, MESSAGE_t *pMsg);

int ManagerAddVtaReqProc(int iThreadId, MESSAGE_t *pMsg);
int ManagerDelVtaReqProc(int iThreadId, MESSAGE_t *pMsg);
int ManagerModifyVtaReqProc(int iThreadId, MESSAGE_t *pMsg);
int ManagerQueryVtaReqProc(int iThreadId, MESSAGE_t *pMsg);
int ManagerForceLogoutVtaReqProc(int iThreadId, MESSAGE_t *pMsg);
int ManagerSetVtaStateReqProc(int iThreadId, MESSAGE_t *pMsg);
int ManagerQueryVtaStateReqProc(int iThreadId, MESSAGE_t *pMsg);
int ManagerModifyQueueRuleReqProc(int iThreadId, MESSAGE_t *pMsg);
int ManagerForceRelCallReqProc(int iThreadId, MESSAGE_t *pMsg);

int InspectorMonitorReqProc(int iThreadId, MESSAGE_t *pMsg);
int InspectorMonitorIndCnfProc(int iThreadId, MESSAGE_t *pMsg);

int VtaBandWidthNoticeProc(int iThreadId, MESSAGE_t *pMsg);
int VtaEventNoticeProc(int iThreadId, MESSAGE_t *pMsg);
int VtaParaCfgIndCnfProc(int iThreadId, MESSAGE_t *pMsg);

int AmsSendVtaLoginRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);
int AmsSendVtaLogoutRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);
int AmsSendVtaHandshakeRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);
int AmsSendVtaStateOperateRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);
int AmsSendVtaStateOperateInd(LP_AMS_DATA_t *lpAmsData, MESSAGE_t *pMsg, unsigned short stateOpInd);
int AmsSendVtaModifyPasswordRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);
int AmsSendVtaServiceRegRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);
int AmsSendVtaTransferCallRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);
int AmsSendVtaVolumeCtrlRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,unsigned short volumeCtrlType,int iret);
int AmsSendVtaAudioRecordRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret,FILE_INFO *pFileInfo);
int AmsSendVtaScreenRecordRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret,FILE_INFO *pFileInfo);
int AmsSendVtaRemoteCooperativeRsp(LP_AMS_DATA_t *lpAmsData,
	                                     	   MESSAGE_t *pMsg,
	                                     	   int iret,
	                                     	   unsigned short rspType,
	                                     	   unsigned short remoteOp,
	                                     	   TRANS_PROTO_PARA *pTransProtoPara);
int AmsSendVtaSnapRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,unsigned short snapOp,int iret);
int AmsSendVtaSendMsgRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);
int AmsSendVtaRecvMsgInd(LP_AMS_DATA_t *lpTargetAmsData,unsigned short originType,unsigned int originId,unsigned int msgLen,unsigned char *pMsgBody);
int AmsSendVtaSendFileRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,unsigned short fileSendOp,int iret);
int AmsSendVtaRecvFileInd(LP_AMS_DATA_t *lpTargetAmsData,unsigned short originType,unsigned int originId,unsigned short fileRecvOp,unsigned short fileSrvType,unsigned short fileUser,FILE_INFO *pFileInfo);
int AmsSendVtaInnerCallRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);
int AmsSendVtaMultiSessRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,unsigned short multiSessOp,int iret);
int AmsSendVtaMultiSessInd(LP_AMS_DATA_t *lpTargetAmsData,unsigned short originType,unsigned int originId,unsigned short assocTargetType,unsigned int assocTargetId,unsigned short multiSessOp);

int AmsSendVtaForceLoginRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);
int AmsSendVtaQueryInfoRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,unsigned short queryType,int iret,unsigned char *queryBuf, unsigned short queryBufLen);

int AmsSendManagerAddVtaRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);
int AmsSendManagerDelVtaRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);
int AmsSendManagerModifyVtaRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);
int AmsSendManagerQueryVtaRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret,TELLER_INFO *pQueryTellerInfo);
int AmsSendManagerForceLogoutVtaRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);
int AmsSendManagerSetVtaStateRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);
int AmsSendManagerQueryVtaStateRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret,TELLER_QUERY_RESULT *pTellerQueryResult);
int AmsSendManagerModifyQueueRuleRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);
int AmsSendManagerForceRelCallRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);

int AmsSendInspectorMonitorRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,unsigned short monitorOp,int iret);
int AmsSendVtaMonitorInd(LP_AMS_DATA_t *lpTargetAmsData,unsigned short originType,unsigned int originId,unsigned short monitorOp);

int AmsSendTellerEventInd(LP_AMS_DATA_t *lpAmsData,unsigned short originType,unsigned int originId,unsigned int tellerEventInd,int iret);
int AmsSendVtaParaCfgInd(VTA_NODE *pVtaNode,MESSAGE_t *pMsg,OA_AMS_TERMPARA_CONFIG_t *termConfigPara,FILE_SERVER_PARA *fileServerPara,unsigned char *pBuf,unsigned short bufLen,unsigned short configInd);
int AmsSendTermIdNoInfo(MESSAGE_t *pMsg, VTA_NODE *pCurrVtaNode, VTM_NODE *pCurrVtmNode);
int AmsSendTermCfgInfo(MESSAGE_t *pMsg, VTA_NODE *pVtaNode, VTM_NODE *pVtmNode);

int AmsVtaQueryAllTellerStateInfoProc(LP_AMS_DATA_t *lpAmsData, VTA_NODE *pCurrVtaNode, MESSAGE_t *pMsg);
int AmsVtaQueryTellerStateInfoProc(LP_AMS_DATA_t *lpAmsData, VTA_NODE *pCurrVtaNode, MESSAGE_t *pMsg, unsigned char tellerNoLen, char *pTellerNo);
int AmsVtaQueryTellerServiceListProc(LP_AMS_DATA_t *lpAmsData, VTA_NODE *pCurrVtaNode, MESSAGE_t *pMsg, unsigned char tellerNoLen, char *pTellerNo);
int AmsVtaQueryServiceTellerListProc(LP_AMS_DATA_t *lpAmsData, VTA_NODE *pCurrVtaNode, MESSAGE_t *pMsg, unsigned int serviceId);
int AmsVtaQueryServiceQueueInfoProc(LP_AMS_DATA_t *lpAmsData, VTA_NODE *pCurrVtaNode, MESSAGE_t *pMsg, unsigned int serviceId);
int AmsVtaQuerySrvGrpIdQueueInfoProc(LP_AMS_DATA_t *lpAmsData, VTA_NODE *pCurrVtaNode, MESSAGE_t *pMsg, unsigned int srvGrpId);
int AmsVtaQueryCurTellerServiceListProc(LP_AMS_DATA_t *lpAmsData, VTA_NODE *pCurrVtaNode, MESSAGE_t *pMsg);
int AmsVtaQueryCurTellerWorkInfoProc(LP_AMS_DATA_t *lpAmsData, VTA_NODE *pCurrVtaNode, MESSAGE_t *pMsg);
int AmsVtaQueryTellerListByNoKeyProc(LP_AMS_DATA_t *lpAmsData, VTA_NODE *pCurrVtaNode, MESSAGE_t *pMsg, unsigned char tellerNoKeyLen, char *pTellerNoKey);
int AmsVtaQueryTellerListByNameKeyProc(LP_AMS_DATA_t *lpAmsData, VTA_NODE *pCurrVtaNode, MESSAGE_t *pMsg, unsigned char tellerNameKeyLen, char *pTellerNameKey);

int AmsGetQueryResultIdByQueryType(unsigned short  queryType, unsigned char *pQueryResultId);

int VtaAudioRecEventNoticeProc(int iThreadId, LP_AMS_DATA_t *lpAmsData, int iret);
int VtaAudioRecFileUploadEventNoticeProc(int iThreadId, LP_AMS_DATA_t *lpAmsData, int iret, unsigned char callId[], FILE_INFO fileInfo);
int VtaScreenRecEventNoticeProc(int iThreadId, LP_AMS_DATA_t *lpAmsData, int iret);
int VtaScreenRecFileUploadEventNoticeProc(int iThreadId, LP_AMS_DATA_t *lpAmsData, int iret, unsigned char callId[], FILE_INFO fileInfo);
int VtaSnapEventNoticeProc(int iThreadId, LP_AMS_DATA_t *lpAmsData, int iret, int pid);
int VtaSnapFileDownloadEventNoticeProc(int iThreadId, LP_AMS_DATA_t *lpAmsData, int iret, int pid);
int VtaFileUploadEventNoticeProc(int iThreadId, 
										LP_AMS_DATA_t *lpAmsData, 
										int iret, 
										int pid,
										unsigned short targetType,
										unsigned int targetId,
										unsigned int tellerId,
										unsigned char tellerNo[],
										FILE_INFO fileInfo);
int VtaFileDownloadEventNoticeProc(int iThreadId,
											LP_AMS_DATA_t *lpAmsData, 
											int iret, 
											MESSAGE_t *pMsg, 
											int pid,
											unsigned short originType,
											unsigned int originId,	
											unsigned int tellerId,
											unsigned char tellerNo[]);
int VtaCancelRecvFileEventNoticeProc(int iThreadId,
											 LP_AMS_DATA_t *lpAmsData, 
											 int iret, 
											 MESSAGE_t *pMsg, 
											 int pid,
											 unsigned short originType,
											 unsigned int originId,	
											 unsigned int tellerId,
											 unsigned char tellerNo[]);
int VtaScreenShareEventNoticeProc(int iThreadId, LP_AMS_DATA_t *lpAmsData, int iret);

int AmsCheckVtaOrg(unsigned int vtaCfgPos, unsigned int *orgCfgPos);

int VtaStateOperateIndTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg);
int AmsRestTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg);
int AmsVtaRecvMsgTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg);
int AmsVtaRecvFileTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg);
int AmsVtaMultiSessTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg);
int AmsVtaMonitorTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg);
int AmsVtaParaCfgTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg);
int AmsVtmParaCfgTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg);

//func defined in amsvtm.c
int VtmLoginReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtmLogoutReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtmHandshakeReqProc(int iThreadId, MESSAGE_t *pMsg);

int VtmVolumeCtrlCnfProc(int iThreadId, MESSAGE_t *pMsg);

int VtmRemoteCoopCnfProc(int iThreadId, MESSAGE_t *pMsg);
int VtmSnapCnfProc(int iThreadId, MESSAGE_t *pMsg);
int VtmSendMsgReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtmRecvMsgCnfProc(int iThreadId, MESSAGE_t *pMsg);
int VtmSendFileReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtmRecvFileCnfProc(int iThreadId, MESSAGE_t *pMsg);
int VtmForceLoginReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtmQueryInfoReqProc(int iThreadId, MESSAGE_t *pMsg);

int VtmBandWidthNoticeProc(int iThreadId, MESSAGE_t *pMsg);
int VtmEventNoticeProc(int iThreadId, MESSAGE_t *pMsg);
int VtmParaCfgIndCnfProc(int iThreadId, MESSAGE_t *pMsg);
	
int AmsSendVtmLoginRsp(VTM_NODE *pVtmNode,MESSAGE_t *pMsg,int iret);
int AmsSendVtmLogoutRsp(VTM_NODE *pVtmNode,MESSAGE_t *pMsg,int iret);
int AmsSendVtmHandshakeRsp(VTM_NODE *pVtmNode,MESSAGE_t *pMsg,int iret);

int AmsSendVtmVolumeCtrlInd(LP_AMS_DATA_t *lpAmsData,unsigned short volumeCtrlInd);
int AmsSendVtmRemoteCoopInd(LP_AMS_DATA_t *lpAmsData,unsigned short remoteOpInd);
int AmsSendVtmSnapInd(LP_AMS_DATA_t *lpAmsData,unsigned short snapOpInd,FILE_INFO *pFileInfo);

int AmsSendVtmSendMsgRsp(VTM_NODE *pVtmNode,MESSAGE_t *pMsg,int iret);
int AmsSendVtmRecvMsgInd(VTM_NODE *pVtmNode,unsigned short originType,unsigned int originId,unsigned int msgLen,unsigned char *pMsgBody);
int AmsSendVtmSendFileRsp(VTM_NODE *pVtmNode,MESSAGE_t *pMsg,unsigned short fileSendOp,int iret);
int AmsSendVtmRecvFileInd(VTM_NODE *pVtmNode,unsigned short originType,unsigned int originId,unsigned short fileRecvOp,unsigned short fileSrvType,unsigned short fileUser,FILE_INFO *pFileInfo);
int AmsSendVtmForceLoginRsp(VTM_NODE *pVtmNode,MESSAGE_t *pMsg,int iret);
int AmsSendVtmQueryInfoRsp(VTM_NODE *pVtmNode,MESSAGE_t *pMsg,unsigned short queryType,int iret,unsigned char *queryBuf, unsigned short queryBufLen);

int AmsSendVtmEventInd(VTM_NODE *pVtmNode,unsigned short originType,unsigned int originId,unsigned short vtmEventInd,int iret);
int AmsSendVtmParaCfgInd(VTM_NODE *pVtmNode,MESSAGE_t *pMsg,OA_AMS_TERMPARA_CONFIG_t *termConfigPara,FILE_SERVER_PARA *fileServerPara,unsigned char *pBuf,unsigned short bufLen,unsigned short configInd);

int VtaEventNoticeProc(int iThreadId, MESSAGE_t *pMsg);
int VtmCustScoringEventNoticeProc(int iThreadId, LP_AMS_DATA_t *lpAmsData, VTA_NODE *pVtaNode, int iret, SERVICE_QUALITY serviceQuality);
int VtmScreenShareEventNoticeProc(int iThreadId, LP_AMS_DATA_t *lpAmsData, int iret);
int VtmSnapEventNoticeProc(int iThreadId, LP_AMS_DATA_t *lpAmsData, int iret, int pid);
int VtmSnapFileUploadEventNoticeProc(int iThreadId, 
	                                           LP_AMS_DATA_t *lpAmsData, 
	                                           int iret,
	                                           unsigned int vtmId,
	                                           unsigned char vtmNo[],
	                                           unsigned char callId[],
	                                           FILE_INFO fileInfo);
int VtmFileUploadEventNoticeProc(int iThreadId, 
	                                     LP_AMS_DATA_t *lpAmsData, 
	                                     VTM_NODE *pVtmNode,
	                                     int iret, 
	                                     int pid,
	                                     unsigned short targetType,
	                                     unsigned int targetId,	
	                                     unsigned int vtmId,
	                                     unsigned char vtmNo[],
	                                     FILE_INFO fileInfo);
int VtmFileDownloadEventNoticeProc(int iThreadId,
											LP_AMS_DATA_t *lpAmsData, 
											VTM_NODE *pVtmNode,
											int iret, 
											int pid,
											unsigned short originType,
											unsigned int originId,	
											unsigned int vtmId,
											unsigned char vtmNo[]);
int VtmCancelRecvFileEventNoticeProc(int iThreadId,
											  LP_AMS_DATA_t *lpAmsData, 
											  VTM_NODE *pVtmNode,
											  int iret, 
											  int pid,
											  unsigned short originType,
											  unsigned int originId,	
											  unsigned int vtmId,
											  unsigned char vtmNo[]);

int AmsCheckVtmOrg(unsigned int vtmCfgPos, unsigned int *orgCfgPos);

int AmsVolumeCtrlTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg);
int AmsVtmRemoteCoopTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg);
int AmsSnapTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg);
int AmsVtmRecvMsgTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg);
int AmsVtmRecvFileTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg);

//func defined in amscp.c
int VtaRegReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtmRegReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaGetReqProc(int iThreadId, MESSAGE_t *pMsg);
int AmsQueryTermNetInfoReqProc(int iThreadId, MESSAGE_t *pMsg);
int CallEventNoticeProc(int iThreadId, MESSAGE_t *pMsg);
int CallEventIndCnfProc(int iThreadId, MESSAGE_t *pMsg);

int AmsSendCmsVtaRegRsp(TELLER_REGISTER_INFO *tellerRegisterInfo,MESSAGE_t *pMsg,int iret);
int AmsSendCmsVtmRegRsp(VTM_REGISTER_INFO *vtmRegisterInfo,MESSAGE_t *pMsg,int iret);
int AmsSendCmsVtaGetRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret,VTA_NODE *pVtaNode,VTM_NODE *pVtmNode);
int AmsSendQueryTermNetInfoRsp(MESSAGE_t *pMsg,int iret,VTA_NODE *pVtaNode,VTM_NODE *pVtmNode);
int AmsSendCmsCallEventInd(LP_AMS_DATA_t *lpAmsData,unsigned short callEventInd,CALL_TARGET *pCallTarget);
int AmsSendCmsCallQueueEventInd(LP_QUEUE_DATA_t *lpQueueData,unsigned short callEventInd,CALL_TARGET *pCallTarget);

int AmsCallEventIndTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg);

//func defined in asmrcp.c
int RcasStartScreenShareRspProc(int iThreadId, MESSAGE_t *pMsg);
int RcasStartScreenShareCnfProc(int iThreadId, MESSAGE_t *pMsg);
int RcasStopScreenShareCnfProc(int iThreadId, MESSAGE_t *pMsg);
int RcasControlScreenCnfProc(int iThreadId, MESSAGE_t *pMsg);
int RcasCancelControlScreenCnfProc(int iThreadId, MESSAGE_t *pMsg);
int RcasHandshakeReqProc(int iThreadId, MESSAGE_t *pMsg);
int RcasLoadCapacityNoticeProc(int iThreadId, MESSAGE_t *pMsg);

int RcasScreenShareEventNoticeProc(int iThreadId, MESSAGE_t *pMsg);

int AmsSendRcasStartScreenShareReq(LP_AMS_DATA_t *lpAmsData, VTM_NODE *pVtmNode);
int AmsSendRcasStopScreenShareReq(LP_AMS_DATA_t *lpAmsData);
int AmsSendRcasControlScreenReq(LP_AMS_DATA_t *lpAmsData);
int AmsSendRcasCancelControlScreenReq(LP_AMS_DATA_t *lpAmsData);
int AmsSendRcasHandshakeRsp(AMS_RCAS_MANAGE *pRcasMngData,MESSAGE_t *pMsg,int iret);

int AmsSendRcasEventInd(LP_AMS_DATA_t *lpAmsData,unsigned int rcasEventInd,int iret);

int AmsSelectRcasModule(int *pRcasPos);
void AmsRecordRcasCap(int pid, unsigned int rcasId, unsigned int rcasPos, RCAS_CAP_PARA *pRcasCap, int *pRet);
void AmsUpdateRcasCncrNum(LP_AMS_DATA_t *lpAmsData, int type);

int AmsRcasRemoteCoopTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg);


//func defined in amssrvmng.c
int AmsDataInit();

int VtaListInit();
VTA_NODE * VtaNodeGet(void);
void VtaNodeFree(VTA_NODE *pNode);

int VtmListInit();
VTM_NODE * VtmNodeGet(void);
void VtmNodeFree(VTM_NODE *pNode);


void AmsClearInactiveVta(int iThreadId, time_t currentTime);
void AmsClearInactiveVtm(int iThreadId, time_t currentTime);
int AmsClearPreLoginVta(int iThreadId, unsigned int srvGrpId, VTA_NODE *pVtaNode, TERM_NET_INFO termNetInfo);
int AmsClearPreLoginVtm(int iThreadId, unsigned int srvGrpId, VTM_NODE *pVtmNode, TERM_NET_INFO termNetInfo);

void AmsClearInactiveRcas(int iThreadId, time_t currentTime);
void AmsClearInactiveCmsCall(int iThreadId, time_t currentTime);

void AmsResetVtmState(int iThreadId, unsigned int srvGrpId, unsigned int vtmId);

void AmsResetTellerRegInfo(unsigned int tellerId);
void AmsResetVtmRegInfo(unsigned int vtmId);

void AmsGetVtaInServiceProcTask(int iThreadId, time_t currentTime);

int AmsSelectSrvGrpIdByServiceType(unsigned char *pVtmNo, 
												 unsigned int vtmId,						                          
												 unsigned int serviceType,
												 unsigned int *pSrvGrpId);

VTA_NODE *AmsSelectVta(unsigned char *pVtmNo,
						   unsigned int vtmId,
						   unsigned int srvGrpId, 
						   unsigned int serviceType,
						   unsigned int vtaNum,
						   unsigned int orgId,
						   unsigned int selectMode,
						   int *pResult);

int AmsGetParentOrg(unsigned int orgId, unsigned int orgCfgPos, unsigned int *pParentOrgCfgPos);

VTA_NODE *AmsSelectVtaByOrg(unsigned char *pVtmNo,
								  unsigned int vtmId,
								  unsigned int srvGrpId, 
								  unsigned int serviceType,
								  unsigned int vtaNum,
								  unsigned int orgId,
								  unsigned int orgCfgPos,
								  int *pResult);

VTA_NODE *AmsSelectVtaByTeller(unsigned char *pVtmNo,
									unsigned int vtmId,
									unsigned int srvGrpId, 
									unsigned int serviceType,
									CALL_TARGET callTarget,
									int *pResult);

VTA_NODE *AmsServiceIntelligentSelectVta(unsigned char *pVtmNo,
											   unsigned int vtmId,
		                                       unsigned int srvGrpId, 
	                                           unsigned int serviceType,
	                                           unsigned int callType,  
	                                           CALL_TARGET callTarget,
	                                           unsigned int orgId,
	                                           unsigned int orgCfgPos,
	                                           int *pResult);
/*
int AmsCheckTellerServiceInfo(VTA_NODE *pVtaNode, 
	                                unsigned int srvGrpId, 
	                                unsigned int serviceType, 
	                                time_t currentTime);*/
int AmsCheckTellerServiceAbility(unsigned int serviceType, unsigned int tellerSrvAuth);
int AmsStartCustomerQueueProcess(MESSAGE_t *pMsg,
	                                       VTM_NODE *pVtmNode,
	                                       unsigned int srvGrpId,
	                                       unsigned int serviceType,
                                           unsigned int callType,  
                                           CALL_TARGET callTarget,	                                       
	                                       unsigned char callIdLen,
										   unsigned char srvGrpSelfAdapt);

VTM_NODE * AmsGetIdleLongestVtm(unsigned int srvGrpId, int vtmNum, time_t currentTime);

int AmsSendCmsGetVtaTimeoutRsp(LP_QUEUE_DATA_t *lpQueueData,MESSAGE_t *pMsg,int iret);

int AmsUpdateSingleVtaWorkInfo(VTA_NODE *pVtaNode, time_t currentTime);
void AmsUpdateVtaWorkInfoProc(time_t currentTime);

//zry added for scc 2018
int AmsUpdateSingleSeatWorkInfo(SEAT_NODE *pSeatNode, time_t currentTime);
//added end

int AmsSendServiceProcMsg();
int AmsProcServiceProcMsg(int iThreadId, MESSAGE_t *pMsg);

int AmsCalcServiceQueueInfo(unsigned int tellerId, unsigned int serviceId, QUEUE_INFO *pQueueInfo);
int AmsTellerCalcSrvGrpIdQueueInfo(unsigned int tellerId, unsigned int srvGrpId, QUEUE_INFO *pQueueInfo);
int AmsCustCalcSrvGrpIdQueueInfo(unsigned int vtmId, unsigned int srvGrpId, QUEUE_INFO *pQueueInfo);
void AmsGenerateVncAuthPwd(LP_AMS_DATA_t *lpAmsData, VTM_NODE *pVtmNode);

int AmsCustomerInQueueTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg);


//func defined in amspack.c
int	AmsUnPackParaIdAndLenLE(unsigned char body[], int bodyLen, unsigned char *pParaID, int *lenValue);
int AmsUnPackParaIdAndLenBE(unsigned char body[], int bodyLen, unsigned char *pParaID, int *lenValue);

int AmsUnpackStateOperateReqOpartPara(unsigned char body[], int bodyLen, STATE_OP_INFO *pStateOpInfo);
int AmsUnpackTransferCallReqOpartPara(unsigned char body[], int bodyLen, CALL_TARGET *pCallTarget);
int AmsUnpackStartScreenShareReqOpartPara(unsigned char body[], int bodyLen, TRANS_PROTO_PARA *pTransProtoPara);
int AmsUnpackStartScreenShareRspOpartPara(unsigned char body[], int bodyLen, TRANS_PROTO_PARA *pTransProtoPara);
int AmsUnpackStartScreenShareCnfOpartPara(unsigned char body[], 
	                                                   int bodyLen, 
	                                                   TRANS_PROTO_PARA *pTransProtoPara,
	                                                   PIXEL_FORMAT_PARA *pPixelFormatPara,
	                                                   ENCODING_PARA *pEncodingPara);
int AmsUnpackVtaSendFileReqOpartPara(unsigned char body[], int bodyLen, FILE_INFO *pFileInfo);
int AmsUnpackVtaForceLoginReqPara(unsigned char body[], int bodyLen, TERM_NET_INFO *pTermNetInfo);
int AmsUnpackModifyQueueRuleReqOpartPara(unsigned char body[], int bodyLen, QUEUE_RULE *pQueueRule);
int AmsUnpackVtaEventNoticeOpartPara(unsigned int vtaEventNotice, unsigned char body[], int bodyLen, FILE_INFO *pFileInfo);

int AmsCheckVtmNoPwd(unsigned char body[], int bodyLen, unsigned int *vtmCfgPos);
int AmsUnpackVtmSendFileReqOpartPara(unsigned char body[], int bodyLen, FILE_INFO *pFileInfo);
int AmsUnpackVtmForceLoginReqPara(unsigned char body[], int bodyLen, TERM_NET_INFO *pTermNetInfo);
int AmsUnpackVtmEventNoticeOpartPara(unsigned int vtmEventNotice, unsigned char body[], int bodyLen, SERVICE_QUALITY *pServiceQuality, FILE_INFO *pFileInfo);

int AmsUnpackVtaRegReqOpartPara(unsigned char body[], int bodyLen, TERM_NET_INFO *pTellerNetInfo);
int AmsUnpackVtmRegReqOpartPara(unsigned char body[], int bodyLen, TERM_NET_INFO *pVtmNetInfo);
int AmsUnpackVtaGetReqOpartPara(unsigned char body[], int bodyLen, CALL_TARGET *pCallTarget);


int AmsUnpackRcasHandshakeReqOpartPara(unsigned char body[], int bodyLen, RCAS_CAP_PARA *pRcasCap);

int AmsUnpackBandWidthPara(unsigned char body[], int bodyLen, BANDWIDTH_PARA *pBandwidthPara, int *pLenValue);
int AmsUnpackTermConfigPara(unsigned char body[], int bodyLen, OA_AMS_TERMPARA_CONFIG_t *termConfigPara);

int	AmsPackTermConfigPara(OA_AMS_TERMPARA_CONFIG_t *termConfigPara, char p[], int pbufLen);
int	AmsPackFileserverPara(FILE_SERVER_PARA *fileServerPara, char p[], int pbufLen);

int	AmsPackSingleTermIdNo(unsigned char termType, unsigned int termId, unsigned char termNoLen, unsigned char *pTermNo, char p[], int pbufLen);
int	AmsPackTermIdNo(char p[], int pbufLen, unsigned char *pSrcBuf, unsigned short srcBufLen);
int	AmsPackOrdinaryFilePathPara(char p[], int pbufLen);
int	AmsPackEncryptInfo(char p[], int pbufLen);
int	AmsPackNetTraversalInfo(char p[], int pbufLen);

//func defined in amsdb.c
int AmsInsertDbFileUploaded(int iThreadId,unsigned char *pCallId,FILE_INFO *pFileInfo);
int AmsUpdateDbTellerState(int iThreadId,LP_AMS_DATA_t *lpAmsData,unsigned short amsTellerStateItem,int tellerState,time_t *pStateStartTime);
int AmsInsertDbTellerSerialState(int iThreadId,LP_AMS_DATA_t *lpAmsData,unsigned int tellerState,TIME_INFO *pTimeInfo, int lastStateKeepTime);
int AmsUpdateDbVtmState(int iThreadId,VTM_NODE *pVtmNode,int vtmState);
int AmsInsertDbTellerNetFlow(int iThreadId,unsigned char *pCallId,unsigned int tellerId,BANDWIDTH_PARA *pTellerBw);
int AmsUpdateDbTellerRealNetFlow(int iThreadId,unsigned char *pCallId,unsigned int tellerId,BANDWIDTH_PARA *pTellerBw);
int AmsInsertDbVtmNetFlow(int iThreadId,unsigned char *pCallId,unsigned int vtmId,BANDWIDTH_PARA *pVtmBw);
int AmsUpateDbVtmRealNetFlow(int iThreadId,unsigned char *pCallId,unsigned int vtmId,BANDWIDTH_PARA *pVtmBw);
int AmsInsertDbServiceSDR(int iThreadId,unsigned short amsSdrItem,LP_AMS_DATA_t *lpAmsData,unsigned char *pCallId,unsigned char callIdLen,unsigned int termId,BANDWIDTH_PARA *pTermBw);
int AmsGetTime2Str(TIME_INFO *pTimeInfo,char *pStrBuf);
int AmsTransformTime2Str(time_t	*ttime,char *pStrBuf);
int AmsTransform2VtcTellerState(unsigned int amsTellerState,unsigned int *pVtcTellerState);
int AmsTransformVtc2AmsTellerState(unsigned int vtcTellerState,unsigned int *pAmsTellerState);
int AmsGetFileSeqFromFileInfo(FILE_INFO *pFileInfo, WORD *pSeq);

int AmsSendtellerStateToOms(int iThreadId, TELLER_STATE_DB_INFO *pTellerStateDbInfo);
int AmsSendvtmStateToOms(int iThreadId, VTM_STATE_DB_INFO *pVtmStateDbInfo);
int AmsSendTellerRealNetFlowToOms(int iThreadId, TELLER_RNF_DB_INFO *ptellerRealNetFlowDbInfo);
int AmsSendVtmRealNetFlowToOms(int iThreadId, VTM_RNF_DB_INFO *pvtmRealNetFlowDbInfo);
int AmsSendSDRBaseItem(int iThreadId,LP_AMS_DATA_t *lpAmsData,unsigned char *pCallId,unsigned char callIdLen,unsigned int termId,BANDWIDTH_PARA *pTermBw);
int AmsSendSDRTellerNetFlow(int iThreadId,unsigned char *pCallId,unsigned char callIdLen,unsigned int termId,BANDWIDTH_PARA *pTermBw);
int AmsSendSDRVtmNetFlow(int iThreadId,unsigned char *pCallId,unsigned char callIdLen,unsigned int termId,BANDWIDTH_PARA *pTermBw);
int AmsSendSdrToOms(int iThreadId, AMS_SDR *pAmsSdr);

//zry added for scc 2018
int AmsUpdateDbUserState(int iThreadId,USER_NODE *pUserNode,int userState);
//added end

//func defined in amsstat.c 
int AmsVtaMsgStatProc(int msgCode);
int AmsVtmMsgStatProc(int msgCode);
int AmsCmsMsgStatProc(int msgCode);
int AmsRcasMsgStatProc(int msgCode);
int AmsBMsgStatProc(int msgCode);
int AmsCMsgStatProc(int msgCode);

int AmsMsgStatProc(int msgType, int msgCode);
int AmsTimerStatProc(int code, int action);
int AmsQueueSystemStatProc(int statType, int msgType);
int AmsServiceStatProc(int msgType);
int AmsTellerStatProc(DWORD tellerId, DWORD tellerPos, int statType, int result);
int AmsVtmStatProc(DWORD vtmId, DWORD vtmPos, int statType, int result);

int AmsSessionStatProc(LP_AMS_DATA_t *lpAmsData, int statType, int value);

int AmsCommonResultStatProc(int resultCode);
int AmsVtaLoginResultStatProc(int resultCode);
int AmsVtaLogoutResultStatProc(int resultCode);
int AmsVtaHandshakeResultStatProc(int resultCode);
int AmsVtaStateOperateResultStatProc(int resultCode);
int AmsVtaModifyPasswordResultStatProc(int resultCode);
int AmsVtaServiceRegResultStatProc(int resultCode);
int AmsVtaTransferCallResultStatProc(int resultCode);
int AmsVtaVolumeCtrlResultStatProc(int resultCode);
int AmsVtaAudioRecordResultStatProc(int resultCode);
int AmsVtaScreenRecordResultStatProc(int resultCode);
int AmsVtaRemoteCooperativeResultStatProc(int resultCode);
int AmsVtaSnapResultStatProc(int resultCode);

int AmsVtaSendMsgResultStatProc(int resultCode);
int AmsVtaRecvMsgResultStatProc(int resultCode);
int AmsVtaSendFileResultStatProc(int resultCode);
int AmsVtaRecvFileResultStatProc(int resultCode);
int AmsVtaMultiSessResultStatProc(int resultCode);
int AmsVtaForceLoginResultStatProc(int resultCode);
int AmsVtaQueryInfoResultStatProc(int resultCode);

int AmsManagerAddVtaResultStatProc(int resultCode);
int AmsManagerDelVtaResultStatProc(int resultCode);
int AmsManagerModifyVtaResultStatProc(int resultCode);
int AmsManagerQueryVtaResultStatProc(int resultCode);
int AmsManagerForceLogoutVtaResultStatProc(int resultCode);
int AmsManagerSetVtaStateResultStatProc(int resultCode);
int AmsManagerQueryVtaStateResultStatProc(int resultCode);
int AmsManagerModifyQueueRuleResultStatProc(int resultCode);
int AmsManagerForceRelCallResultStatProc(int resultCode);

int AmsInspectorMonitorVtaResultStatProc(int resultCode);

int AmsVtaBandwidthNoticeResultStatProc(int resultCode);

int AmsVtaEventNoticeResultStatProc(int resultCode);
int AmsVtaParaCfgResultStatProc(int resultCode);

int AmsVtmLoginResultStatProc(int resultCode);
int AmsVtmLogoutResultStatProc(int resultCode);
int AmsVtmHandshakeResultStatProc(int resultCode);

int AmsVtmSendMsgResultStatProc(int resultCode);
int AmsVtmRecvMsgResultStatProc(int resultCode);
int AmsVtmSendFileResultStatProc(int resultCode);
int AmsVtmRecvFileResultStatProc(int resultCode);
int AmsVtmForceLoginResultStatProc(int resultCode);
int AmsVtmQueryInfoResultStatProc(int resultCode);
int AmsVtmBandwidthNoticeResultStatProc(int resultCode);

int AmsVtmEventNoticeResultStatProc(int resultCode);
int AmsVtmParaCfgResultStatProc(int resultCode);
	
int AmsCmsVtaRegResultStatProc(int resultCode);
int AmsCmsVtmRegResultStatProc(int resultCode);
int AmsCmsGetVtaResultStatProc(int resultCode);
int AmsCmsQueryTermNetInfoResultStatProc(int resultCode);

int AmsCmsEventNoticeResultStatProc(int resultCode);
int AmsCmsEventIndResultStatProc(int resultCode);

int AmsRcasHandshakeResultStatProc(int resultCode);
int AmsRcasLoadCapcNoticeResultStatProc(int resultCode);

int AmsResultStatProc(int resultType, int resultCode);

//func defined in amsmml.c

int AmsTraceToFile(PID_t hPid,PID_t rPid,unsigned char *MsgCode,
						unsigned char *description,unsigned int descrlen,
					    unsigned char *body,unsigned int length,
					    unsigned char *logName);
int AmsTraceInfoToFile(int pid,int body,
						    unsigned char *description,unsigned int descrlen,
						    unsigned char *logName);

int SetAmsPara(MMLCMD_t r[],int SocketId,int Source);
int DisplayAmsPara(MMLCMD_t r[],int SocketId,int Source);
int SetAmsTrace(MMLCMD_t r[],int SocketId,int Source);
int DisplayAmsTrace(MMLCMD_t r[],int SocketId,int Source);

int SetAmsTimer(MMLCMD_t r[],int SocketId,int Source);
int DisplayAmsTimer(MMLCMD_t r[],int SocketId,int Source);
int DisplayAmsMsgStat(int SocketId,int Source);
int DisplayAmsTimerStat(int SocketId,int Source);

int DisplayAmsQueueSystemStat(int SocketId,int Source);
int DisplayAmsServiceStat(int SocketId,int Source);
int DisplayAmsTellerStat(int SocketId, int Source, unsigned int tellerId);
int DisplayAmsVtmStat(int SocketId, int Source, unsigned int vtmId);
int DisplayAmsRcasStat(int SocketId, int Source, unsigned int rcasId);

int DisplayAmsCommonResultStat(int SocketId,int Source);
int DisplayAmsVtaLoginResultStat(int SocketId,int Source);
int DisplayAmsVtaLogoutResultStat(int SocketId,int Source);
int DisplayAmsVtaHandshakeResultStat(int SocketId,int Source);
int DisplayAmsVtaStateOperateResultStat(int SocketId,int Source);
int DisplayAmsVtaModifyPasswordResultStat(int SocketId,int Source);
int DisplayAmsVtaServiceRegResultStat(int SocketId,int Source);
int DisplayAmsVtaTransferCallResultStat(int SocketId,int Source);
int DisplayAmsVtaVolumeCtrlResultStat(int SocketId,int Source);
int DisplayAmsVtaAudioRecordResultStat(int SocketId,int Source);
int DisplayAmsVtaScreenRecordResultStat(int SocketId,int Source);
int DisplayAmsVtaRemoteCooperativeResultStat(int SocketId,int Source);
int DisplayAmsVtaSnapResultStat(int SocketId,int Source);
int DisplayAmsVtaSendMsgResultStat(int SocketId,int Source);
int DisplayAmsVtaRecvMsgResultStat(int SocketId,int Source);
int DisplayAmsVtaSendFileResultStat(int SocketId,int Source);
int DisplayAmsVtaRecvFileResultStat(int SocketId,int Source);
int DisplayAmsVtaMultiSessResultStat(int SocketId,int Source);
int DisplayAmsVtaForceLoginResultStat(int SocketId,int Source);
int DisplayAmsVtaQueryInfoResultStat(int SocketId,int Source);

int DisplayAmsManagerAddVtaResultStat(int SocketId,int Source);
int DisplayAmsManagerDelVtaResultStat(int SocketId,int Source);
int DisplayAmsManagerModifyVtaResultStat(int SocketId,int Source);
int DisplayAmsManagerQueryVtaResultStat(int SocketId,int Source);
int DisplayAmsManagerForceLogoutVtaResultStat(int SocketId,int Source);
int DisplayAmsManagerSetVtaStateResultStat(int SocketId,int Source);
int DisplayAmsManagerQueryVtaStateResultStat(int SocketId,int Source);
int DisplayAmsManagerModifyQueueRuleResultStat(int SocketId,int Source);
int DisplayAmsManagerForceRelCallResultStat(int SocketId,int Source);

int DisplayAmsInspectorMonitorVtaResultStat(int SocketId,int Source);
int DisplayAmsVtaBandwidthNoticeResultStat(int SocketId,int Source);
int DisplayAmsVtaEventNoticeResultStat(int SocketId,int Source);
int DisplayAmsVtaParaCfgResultStat(int SocketId,int Source);

int DisplayAmsVtmLoginResultStat(int SocketId,int Source);
int DisplayAmsVtmLogoutResultStat(int SocketId,int Source);
int DisplayAmsVtmHandshakeResultStat(int SocketId,int Source);
int DisplayAmsVtmSendMsgResultStatProc(int SocketId,int Source);
int DisplayAmsVtmRecvMsgResultStatProc(int SocketId,int Source);
int DisplayAmsVtmSendFileResultStatProc(int SocketId,int Source);	
int DisplayAmsVtmRecvFileResultStatProc(int SocketId,int Source);
int DisplayAmsVtmForceLoginResultStat(int SocketId,int Source);
int DisplayAmsVtmQueryInfoResultStat(int SocketId,int Source);

int DisplayAmsVtmBandwidthNoticeResultStat(int SocketId,int Source);
int DisplayAmsVtmEventNoticeResultStat(int SocketId,int Source);
int DisplayAmsVtmParaCfgResultStat(int SocketId,int Source);

int DisplayAmsCmsVtaRegResultStat(int SocketId,int Source);
int DisplayAmsCmsVtmRegResultStatProc(int SocketId,int Source);
int DisplayAmsCmsGetVtaResultStat(int SocketId,int Source);
int DisplayAmsCmsQueryTermNetInfoResultStat(int SocketId,int Source);
int DisplayAmsCmsEventNoticeResultStat(int SocketId,int Source);
int DisplayAmsCmsEventIndResultStat(int SocketId,int Source);

int DisplayAmsRcasHandshakeResultStat(int SocketId,int Source);
int DisplayAmsRcasLoadCapcNoticeResultStat(int SocketId,int Source);

int DisplayAmsResultStat(int SocketId,int Source);
int DisplayAmsStat(MMLCMD_t r[],int SocketId,int Source);
int ResetAmsStat(MMLCMD_t r[],int SocketId,int Source);

int DisplayAmsService(MMLCMD_t r[],int SocketId,int Source);
int DisplayAmsServiceGroup(MMLCMD_t r[],int SocketId,int Source);
int DisplayAmsTeller(MMLCMD_t r[],int SocketId,int Source);
int DisplayAmsAuthority(MMLCMD_t r[],int SocketId,int Source);
int DisplayAmsVtm(MMLCMD_t r[],int SocketId,int Source);
//int DisplayAmsScreenRec(MMLCMD_t r[],int SocketId,int Source);
int DisplayAmsRcas(MMLCMD_t r[],int SocketId,int Source);
int DisplayAmsOrg(MMLCMD_t r[],int SocketId,int Source);
int DisplayAmsQueue(MMLCMD_t r[],int SocketId,int Source);
int DisplayAmsTask(MMLCMD_t r[],int SocketId,int Source);
int DisplayAmsLic(MMLCMD_t r[],int SocketId,int Source);

int RecvAmsMsg(MMLCMD_t r[],int SocketId,int Source);
int DisplayVtcVersion(MMLCMD_t r[],int SocketId,int Source);
int StartAmsTest(MMLCMD_t r[],int SocketId,int Source);
int DisplayAmsSysCfg(MMLCMD_t r[],int SocketId,int Source);
int AddAmsTeller(MMLCMD_t r[],int SocketId,int Source);
int ModifyAmsTeller(MMLCMD_t r[],int SocketId,int Source);
int DeleteAmsTeller(MMLCMD_t r[],int SocketId,int Source);
int AddAmsVtm(MMLCMD_t r[],int SocketId,int Source);
int ModifyAmsVtm(MMLCMD_t r[],int SocketId,int Source);
int DeleteAmsVtm(MMLCMD_t r[],int SocketId,int Source);
int AddServiceAuth(MMLCMD_t r[],int SocketId,int Source);
int DeleteServiceAuth(MMLCMD_t r[],int SocketId,int Source);
int SetFilePath(MMLCMD_t r[],int SocketId,int Source);


int InstallAmsMMLCommand();

#ifdef __cplusplus
}
#endif

#endif//AMSFUNC_H


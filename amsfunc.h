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
int AmsProcCrmMsg(int iThreadId, MESSAGE_t *pMsg);
int AmsProcCmsMsg(int iThreadId, MESSAGE_t *pMsg);



//func defined in amscfg.c
int ConfigAmsSrv(char *cFileName);
int SrvDivSen(char *s,WORD_t *word);
int SrvGetAWord(char *string,WORD_t *word);
int ConfigSrv(WORD_t *word,int wordcount,int section,unsigned  char pCurrId[]);

int AmsSrvServiceSenten(WORD_t *word,int wordcount,unsigned char pCurrId[]);
int AmsSrvServiceGroupSenten(WORD_t *word,int wordcount,unsigned char pCurrId[]);
int AmsSrvTellerSenten(WORD_t *word,int wordcount,unsigned char pCurrId[]);
int AmsSrvVtmSenten(WORD_t *word,int wordcount,unsigned char pCurrId[]);
int AmsSrvQueueSenten(WORD_t *word,int wordcount,unsigned char pCurrId[]);


//func defined int amspack.c
int	AmsUnPackParaIdAndLenLE(unsigned char body[], int bodyLen, unsigned char *pParaID, int *lenValue);
int AmsUnPackParaIdAndLenBE(unsigned char body[], int bodyLen, unsigned char *pParaID, int *lenValue);
int AmsUnpackStateOperateReqOpartPara(unsigned char body[], int bodyLen, STATE_OP_INFO *pStateOpInfo);
int AmsUnpackTellerpersionalinfo(unsigned char body[], int bodyLen, TELLER_PERSONAL_INFO *pTellerPersonalInfo);

int AmsPackCrmLoginBase(unsigned char tellerIdLen,unsigned char tellerId[],int iret,unsigned char p[],LP_AMS_DATA_t	*lpAmsData);



//func defined in amssrvmng.c
int AmsCfgDataInit();
int CrmListInit();
CRM_NODE * CrmNodeGet(void);
void CrmNodeFree(CRM_NODE *pNode);

int TermListInit();
TERM_NODE * TermNodeGet(void);
void TermNodeFree(TERM_NODE *pNode);

int AmsCfgDataInit();
int AmsUpdateSingleCrmWorkInfo(CRM_NODE *pCrmNode, time_t currentTime);
int AmsSendServiceProcMsg();
void AmsResetTermState(int iThreadId, unsigned int srvGrpIdPos, unsigned char termId[],unsigned char termIdLen);
int AmsCustCalcSrvGrpIdQueueInfo(unsigned char termId[], unsigned int srvGrpId, QUEUE_INFO *pQueueInfo);
int AmsSeleteGrpId(unsigned int srvGrpId,unsigned char termId[],unsigned char serviceType[],unsigned int *srvGrpSelect);
int AmsSelectSrvGrpIdByServiceType(unsigned char termId[],						                          
											 unsigned char serviceType[],
											 unsigned int *pSrvGrpId);
CRM_NODE *AmsServiceIntelligentSelectCrm(
									   unsigned char termId[],
									   unsigned int srvGrpId, 
									   unsigned char serviceType[],
									   int *pResult);

CRM_NODE *AmsSelectCrm(	unsigned char termId[],
						   unsigned int srvGrpId, 
						   unsigned char serviceType[],
						   unsigned int crmNum,
						   int *pResult);
int AmsStartCustomerQueueProcess(MESSAGE_t *pMsg,
								TERM_NODE *pTermNode,
								unsigned int srvGrpId,
								unsigned char servicetypelen,
								unsigned char serviceType[],                                     
								unsigned char callIdLen,
								unsigned char srvGrpSelfAdapt);

int AmsSendServiceProcMsg();
TERM_NODE * AmsGetIdleLongestTerm(unsigned int srvGrpId, int termNum, time_t currentTime);
void AmsGetCrmInServiceProcTask(int iThreadId, time_t currentTime);
void AmsClearInactiveCrm(int iThreadId, time_t currentTime);
void AmsClearInactiveTerm(int iThreadId, time_t currentTime);
void AmsClearInactiveCmsCall(int iThreadId, time_t currentTime);
void AmsUpdateCrmWorkInfoProc(time_t currentTime);
int AmsProcServiceProcMsg(int iThreadId, MESSAGE_t *pMsg);
int AmsSendCmsGetCrmTimeoutRsp(LP_QUEUE_DATA_t *lpQueueData,MESSAGE_t *pMsg,int iret);
int AmsCustomerInQueueTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg);
int AmsClearHeartbeatErrCrm(LP_AMS_DATA_t *lpAmsData,unsigned char tellid[],unsigned char tellidlen);
void AmsResetTermState(int iThreadId, unsigned int srvGrpIdPos, unsigned char termId[],unsigned char termIdLen);




//func defined in amsfunc.c
int AmsAllocPid(PID_t *spid);
int AmsReleassPid(PID_t sPid,int ret);
int AmsCreateTimer(int iPid,int *timerId, int timerType,int tmcnt);
int AmsCreateTimerPara(int iPid,int *timerId,int timerType,int tmcnt,unsigned char *para);
int AmsKillTimer(int iPid, int *timerId);

void AmsSetCrmState(int iThreadId,LP_AMS_DATA_t *lpAmsData,CRM_NODE *pCrmNode,int state,int stateReason);
void AmsSetCrmCallState(LP_AMS_DATA_t *lpAmsData,CRM_NODE *pCrmNode,int state);

CRM_NODE *AmsSearchCrmNode(unsigned int srvGrpId, unsigned char tellerId[],unsigned int tellerIdLen);
int AmsUpdateCrmState(int iThreadId, LP_AMS_DATA_t *lpAmsData, CRM_NODE *pCrmNode, int stateOperate, int stateReason);
int AmsKillCrmAllCallTimer(LP_AMS_DATA_t *lpAmsData, int pid);

int TellerInfoListInit();
int RegTellerInfoListInit();
int VtmInfoListInit();
TELLER_INFO_NODE  *AmsGetTellerInfoNode();
TELLER_REGISTER_INFO_NODE  *AmsGetRegTellerInfoNode();
VTM_INFO_NODE  *AmsGetVtmInfoNode();
static int AmsCalcTellerInfoHashIdx(unsigned char tellerId[],unsigned char len);
TELLER_INFO_NODE *AmsSearchTellerInfoHash(unsigned char tellerId[],unsigned char len);
void AmsInsertTellerInfoHash(TELLER_INFO_NODE *pTellerInfoNode);
TELLER_REGISTER_INFO_NODE *AmsSearchRegTellerInfoHash(unsigned char tellerId[],unsigned char len);
void AmsInsertRegTellerInfoHash(TELLER_REGISTER_INFO_NODE *pTellerInfoNode);
static int AmsCalcVtmInfoHashIdx(unsigned char vtmId[],unsigned char len);
VTM_INFO_NODE *AmsSearchVtmInfoHash(unsigned char vtmId[],unsigned char len);
void AmsInsertVtmInfoHash(VTM_INFO_NODE *pVtmInfoNode);
TERM_NODE *AmsSearchTermNode(unsigned int srvGrpId, unsigned char termId[],unsigned char termIdLen);



//func defined in amscp.c
int CrmRegReqProc(int iThreadId, MESSAGE_t *pMsg);
int CrmGetReqProc(int iThreadId, MESSAGE_t *pMsg);
int CrmCalloutReqProc(int iThreadId, MESSAGE_t *pMsg);
int CrmAuthinfoReqProc(int iThreadId, MESSAGE_t *pMsg);
int CallEventNoticeProc(int iThreadId, MESSAGE_t *pMsg);
int AmsSendCmsCrmRegRsp(TELLER_REGISTER_INFO_NODE *tellerRegisterInfo,MESSAGE_t *pMsg,int iret);
int AmsSendCmsCrmCalloutRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret,DWORD callouttype);
int AmsSendCmsAuthinfoRsp(TELLER_INFO *tellcfginfo, MESSAGE_t *pMsg,int iret);








//func defined in amsvta.c
int CrmLoginReqProc(int iThreadId, MESSAGE_t *pMsg);
int CrmStateOperateReqProc(int iThreadId,MESSAGE_t *pMsg);
int AmsSendCrmLoginRsp(MESSAGE_t *pMsg,int iret,MESSAGE_t *d_Msg,int num);
int AmsSendCrmStateOperateRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);
int AmsSendTellerEventInd(LP_AMS_DATA_t *lpAmsData,unsigned int tellerEventInd, unsigned char vtmid[],unsigned char vtmidlen,unsigned int vtmtype,int iret);
int AmsSendCrmStateOperateInd(LP_AMS_DATA_t *lpAmsData, MESSAGE_t *pMsg, unsigned short stateOpInd,unsigned short statesubOpInde);
int AmsSendCrmQueryInfoRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,unsigned short queryType,int iret,unsigned char *queryBuf, unsigned short queryBufLen);
int CrmEventNoticeTeller(unsigned char *p,unsigned int crmEventNotice);







#endif

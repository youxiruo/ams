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

int AmsPackVtaLoginBase(unsigned char tellerIdLen,unsigned char tellerId[],int iret,unsigned char p[],LP_AMS_DATA_t	*lpAmsData);



//func defined in amssrvmng.c
int AmsCfgDataInit();
int VtaListInit();
VTA_NODE * VtaNodeGet(void);
void VtaNodeFree(VTA_NODE *pNode);

int TermListInit();
TERM_NODE * TermNodeGet(void);
void TermNodeFree(TERM_NODE *pNode);

int AmsCfgDataInit();
int AmsUpdateSingleVtaWorkInfo(VTA_NODE *pVtaNode, time_t currentTime);
int AmsSendServiceProcMsg();



//func defined in amsfunc.c
int AmsAllocPid(PID_t *spid);
int AmsReleassPid(PID_t sPid,int ret);
int AmsCreateTimer(int iPid,int *timerId, int timerType,int tmcnt);
int AmsCreateTimerPara(int iPid,int *timerId,int timerType,int tmcnt,unsigned char *para);
int AmsKillTimer(int iPid, int *timerId);

void AmsSetVtaState(int iThreadId,LP_AMS_DATA_t *lpAmsData,VTA_NODE *pVtaNode,int state,int stateReason);
void AmsSetVtaCallState(LP_AMS_DATA_t *lpAmsData,VTA_NODE *pVtaNode,int state);

VTA_NODE *AmsSearchVtaNode(unsigned int srvGrpId, unsigned char tellerId[],unsigned int tellerIdLen);
int AmsUpdateVtaState(int iThreadId, LP_AMS_DATA_t *lpAmsData, VTA_NODE *pVtaNode, int stateOperate, int stateReason);
int AmsKillVtaAllCallTimer(LP_AMS_DATA_t *lpAmsData, int pid);

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


//func defined in amscp.c
int VtaRegReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaGetReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaCalloutReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaAuthinfoReqProc(int iThreadId, MESSAGE_t *pMsg);
int CallEventNoticeProc(int iThreadId, MESSAGE_t *pMsg);
int AmsSendCmsVtaRegRsp(TELLER_REGISTER_INFO_NODE *tellerRegisterInfo,MESSAGE_t *pMsg,int iret);
int AmsSendCmsVtaCalloutRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret,DWORD callouttype);
int AmsSendCmsAuthinfoRsp(TELLER_INFO *tellcfginfo, MESSAGE_t *pMsg,int iret);








//func defined in amsvta.c
int VtaLoginReqProc(int iThreadId, MESSAGE_t *pMsg);
int VtaStateOperateReqProc(int iThreadId,MESSAGE_t *pMsg);
int AmsSendVtaLoginRsp(MESSAGE_t *pMsg,int iret,MESSAGE_t *d_Msg,int num);
int AmsSendVtaStateOperateRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);
int AmsSendTellerEventInd(LP_AMS_DATA_t *lpAmsData,unsigned int tellerEventInd, unsigned char vtmid[],unsigned char vtmidlen,unsigned int vtmtype,int iret);
int AmsSendVtaStateOperateInd(LP_AMS_DATA_t *lpAmsData, MESSAGE_t *pMsg, unsigned short stateOpInd,unsigned short statesubOpInde);







#endif

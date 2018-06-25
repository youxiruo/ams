#ifndef AMSFUNC_H
#define AMSFUNC_H

//func defined in ams.c
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


//func defined in amssrvmng.c
int AmsCfgDataInit();
int VtaListInit();
VTA_NODE * VtaNodeGet(void);
void VtaNodeFree(VTA_NODE *pNode);
int AmsCfgDataInit();
int AmsUpdateSingleVtaWorkInfo(VTA_NODE *pVtaNode, time_t currentTime);



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
static int AmsCalcTellerInfoHashIdx(unsigned char tellerId[]);
TELLER_INFO_NODE *AmsSearchTellerInfoHash(unsigned char tellerId[]);
void AmsInsertTellerInfoHash(TELLER_INFO_NODE *pTellerInfoNode);
static int AmsCalcVtmInfoHashIdx(unsigned char vtmId[]);
TELLER_REGISTER_INFO_NODE *AmsSearchRegTellerInfoHash(unsigned char tellerId[],,unsigned int len);
void AmsInsertRegTellerInfoHash(TELLER_REGISTER_INFO_NODE *pTellerInfoNode);
static int AmsCalcVtmInfoHashIdx(unsigned char vtmId[],unsigned int len);
VTM_INFO_NODE *AmsSearchVtmInfoHash(unsigned char vtmId[]);
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
int AmsSendVtaLoginRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);
int AmsSendVtaStateOperateRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);
int AmsSendTellerEventInd(LP_AMS_DATA_t *lpAmsData,unsigned int tellerEventInd, unsigned char vtmid[],unsigned char vtmidlen,unsigned int vtmtype,int iret);







#endif

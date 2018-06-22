#ifndef AMSFUNC_H
#define AMSFUNC_H

//func defined in ams.c
int ProcessAmsMessage(int iThreadId,MESSAGE_t *pMsg);
int AmsProcAMsg(int iThreadId,MESSAGE_t *pMsg);
int AmsProcVtaMsg(int iThreadId, MESSAGE_t *pMsg);
int AmsProcVtmMsg(int iThreadId, MESSAGE_t *pMsg);
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


//func defined in amsfunc.c
int AmsAllocPid(PID_t *spid);
int AmsReleassPid(PID_t sPid,int ret);
int AmsCreateTimer(int iPid,int *timerId, int timerType,int tmcnt);
int AmsCreateTimerPara(int iPid,int *timerId,int timerType,int tmcnt,unsigned char *para);
int AmsKillTimer(int iPid, int *timerId);

void AmsSetVtaState(int iThreadId,LP_AMS_DATA_t *lpAmsData,VTA_NODE *pVtaNode,int state,int stateReason);
void AmsSetVtaCallState(LP_AMS_DATA_t *lpAmsData,VTA_NODE *pVtaNode,int state);

VTA_NODE *AmsSearchVtaNode(unsigned int srvGrpId, unsigned char tellerId[],unsigned int tellerIdLen);


int TellerInfoListInit();
int VtmInfoListInit();

static int AmsCalcTellerInfoHashIdx(unsigned char tellerId[]);
TELLER_INFO_NODE *AmsSearchTellerIdHash(unsigned char tellerId[]);
void AmsInsertTellerInfoHash(TELLER_INFO_NODE *pTellerInfoNode);
static int AmsCalcVtmInfoHashIdx(unsigned char vtmId[]);
VTM_INFO_NODE *AmsSearchVtmIdHash(unsigned char vtmId[]);
void AmsInsertVtmInfoHash(VTM_INFO_NODE *pVtmInfoNode);


//func defined in amscp.c
int VtaRegReqProc(int iThreadId, MESSAGE_t *pMsg);



//func defined in amsvta.c
int VtaLoginReqProc(int iThreadId, MESSAGE_t *pMsg);
int AmsSendVtaLoginRsp(LP_AMS_DATA_t *lpAmsData,MESSAGE_t *pMsg,int iret);





#endif

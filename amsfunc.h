#ifndef AMSFUNC_H
#define AMSFUNC_H
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


//func defined in amsfunc.c
int TellerInfoListInit();
int VtmInfoListInit();

static int AmsCalcTellerInfoHashIdx(unsigned char tellerId[]);
TELLER_INFO_NODE *AmsSearchTellerIdHash(unsigned char tellerId[]);
void AmsInsertTellerInfoHash(TELLER_INFO_NODE *pTellerInfoNode);
static int AmsCalcVtmInfoHashIdx(unsigned char vtmId[]);
VTM_INFO_NODE *AmsSearchVtmIdHash(unsigned char vtmId[]);
void AmsInsertVtmInfoHash(VTM_INFO_NODE *pVtmInfoNode);







#endif

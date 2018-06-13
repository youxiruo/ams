#ifndef AMSFUNC_H
#define AMSFUNC_H
//func defined in amscfg.c
int ConfigAmsSrv(char *cFileName);
int SrvDivSen(char *s,WORD_t *word);
int SrvGetAWord(char *string,WORD_t *word);
int ConfigSrv(WORD_t *word,int wordcount,int section,unsigned  int *pCurrId);

int AmsSrvServiceSenten(WORD_t *word,int wordcount,unsigned int *pCurrId);
int AmsSrvServiceGroupSenten(WORD_t *word,int wordcount,unsigned int *pCurrId);
int AmsSrvTellerSenten(WORD_t *word,int wordcount,unsigned int *pCurrId);
int AmsSrvVtmSenten(WORD_t *word,int wordcount,unsigned int *pCurrId);
int AmsSrvQueueSenten(WORD_t *word,int wordcount,unsigned int *pCurrId);


#endif

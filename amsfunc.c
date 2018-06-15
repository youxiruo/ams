#include "amsfunc.h"



static int			freeAmsTellerInfoListInitialled = 0;
LIST				freeAmsTellerInfoList;
TELLER_INFO_NODE	*freeAmsTellerInfoListBufPtr = NULL;

static int			freeAmsVtmInfoListInitialled = 0;
LIST				freeAmsVtmInfoList;
VTM_INFO_NODE		*freeAmsVtmInfoListBufPtr = NULL;

int TellerInfoListInit()
{
	TELLER_INFO_NODE			*pNode;
	int							size;
	int							i;

	if(0 != amsTellerInfoListInitialled)
	{
		return AMS_SUCCESS;
	}

	lstInit(&freeAmsTellerInfoList);

	size = sizeof(TELLER_INFO_NODE) * AMS_MAX_VTA_NODES;
	freeAmsTellerInfoListBufPtr = (TELLER_INFO_NODE *)malloc(size);
	pNode = (TELLER_INFO_NODE *)freeAmsTellerInfoListBufPtr;
	if(NULL == pNode)
	{
		dbgprint("TellerInfoListInit AllocMem[%d]Err\r\n", size);
		return AMS_ALLOC_MEM_FAILED;
	}

	for(i = 0; i < AMS_MAX_VTA_NODES;i++,pNode++)
	{
		lstAdd(&freeAmsTellerInfoList,(NODE *)pNode);
	}

	freeAmsTellerInfoListInitialled = 1;

	return AMS_SUCCESS;
}

int VtmInfoListInit()
{
	VTM_INFO_NODE			*pNode;
	int							size;
	int							i;

	if(0 != freeAmsVtmInfoListInitialled)
	{
		return AMS_SUCCESS;
	}

	lstInit(&freeAmsVtmInfoList);

	size = sizeof(VTM_INFO_NODE) * AMS_MAX_VTM_NODES;
	freeAmsVtmInfoListBufPtr = (VTM_INFO_NODE *)malloc(size);
	pNode = (VTM_INFO_NODE *)freeAmsVtmInfoListBufPtr;
	if(NULL == pNode)
	{
		dbgprint("TellerInfoListInit AllocMem[%d]Err\r\n", size);
		return AMS_ALLOC_MEM_FAILED;
	}

	for(i = 0; i < AMS_MAX_VTA_NODES;i++,pNode++)
	{
		lstAdd(&freeAmsVtmInfoList,(NODE *)pNode);
	}

	freeAmsVtmInfoListInitialled = 1;

	return AMS_SUCCESS;
}

//---------------------------------------------------------------
//功能： get an idle teller info node
//入口： 
//返回： pNode
//说明： 
//----------------------------------------------------------------
TELLER_INFO_NODE  *AmsGetTellerInfoNode()
{
	TELLER_INFO_NODE         *pNode = NULL;
	
    if(!freeAmsTellerInfoListInitialled)
    {
		dbgprint("AmsGetTellerInfoNode InitFlag[%d]Err\r\n",freeAmsTellerInfoListInitialled);
        return NULL;
    }
	
    pNode = (TELLER_INFO_NODE *)lstGet(&freeAmsTellerInfoList);
	
	return pNode;
}

//---------------------------------------------------------------
//功能： get an idle vtm info node
//入口： 
//返回： pNode
//说明： 
//----------------------------------------------------------------
VTM_INFO_NODE  *AmsGetVtmInfoNode()
{
	VTM_INFO_NODE         *pNode = NULL;
	
    if(!freeAmsVtmInfoListInitialled)
    {
		dbgprint("AmsGetTellerInfoNode InitFlag[%d]Err\r\n",freeAmsVtmInfoListInitialled);
        return NULL;
    }
	
    pNode = (VTM_INFO_NODE *)lstGet(&freeAmsVtmInfoList);
	
	return pNode;
}

/*******************************************************************************
* AmsCalcTellerInfoHashIdx - get teller info hash table index
*
* RETURNS: teller info hash table index
*/
static int AmsCalcTellerInfoHashIdx(unsigned char tellerId[],unsigned int len)
{
	return __BKDRHash(tellerId,len)%AMS_VTA_ID_HASH_SIZE;
}

/*******************************************************************************
* AmsSearchTellerInfoHash - search teller info node via tellerId
*
* RETURNS: a pointer to teller info node
*/
TELLER_INFO_NODE *AmsSearchTellerInfoHash(unsigned char tellerId[],,unsigned int len)
{
	int                 hashIdx;
	TELLER_INFO_NODE    *pTellerInfoNode = NULL;

	hashIdx = AmsCalcTellerInfoHashIdx(tellerId,len);
	pTellerInfoNode= AmsCfgTellerHashTbl[hashIdx];

	while(pTellerInfoNode)
	{
	    if(0 == strcmp(pTellerInfoNode->tellerId,tellerId))
	    {
	        break;
	    }
		
	    pTellerInfoNode = pTellerInfoNode->hashNext;
	}

	return pTellerInfoNode;
	
}


/*******************************************************************************
* AmsInsertTellerInfoHash - insert a node to teller info hash table 
*
* RETURNS: N/A
*/
void AmsInsertTellerInfoHash(TELLER_INFO_NODE *pTellerInfoNode)
{
	int                 hashIdx;

	if(NULL == pTellerInfoNode)
	{
		return;
	}

	hashIdx = AmsCalcTellerInfoHashIdx(pTellerInfoNode->tellerId);
	pTellerInfoNode->hashNext = AmsCfgTellerHashTbl[hashIdx];
	AmsCfgTellerHashTbl[hashIdx] = pTellerInfoNode;
}

/*******************************************************************************
* AmsCalcVtmInfoHashIdx - get vtm info hash table index
*
* RETURNS: vtm info hash table index
*/
static int AmsCalcVtmInfoHashIdx(unsigned char vtmId[],unsigned int len)
{
	return __BKDRHash(vtmId,len)%AMS_VTM_ID_HASH_SIZE;
}

/*******************************************************************************
* AmsSearchTellerInfoHash - search teller info node via tellerId
*
* RETURNS: a pointer to teller info node
*/
VTM_INFO_NODE *AmsSearchVtmInfoHash(unsigned char vtmId[],unsigned int len)
{
	int                 hashIdx;
	VTM_INFO_NODE    *pVtmInfoNode = NULL;

	hashIdx = AmsCalcVtmInfoHashIdx(vtmId,len);
	pVtmInfoNode= AmsCfgVtmHashTbl[hashIdx];

	while(pVtmInfoNode)
	{
	    if(0 == strcmp(pVtmInfoNode->vtmId,vtmId))
	    {
	        break;
	    }
		
	    pVtmInfoNode = pVtmInfoNode->hashNext;
	}

	return pVtmInfoNode;
	
}


/*******************************************************************************
* AmsInsertVtmInfoHash - insert a node to vtm info hash table 
*
* RETURNS: N/A
*/
void AmsInsertVtmInfoHash(VTM_INFO_NODE *pVtmInfoNode)
{
	int                 hashIdx;

	if(NULL == pVtmInfoNode)
	{
		return;
	}

	hashIdx = AmsCalcVtmInfoHashIdx(pVtmInfoNode->vtmId);
	pVtmInfoNode->hashNext = AmsCfgVtmHashTbl[hashIdx];
	AmsCfgVtmHashTbl[hashIdx] = pVtmInfoNode;
}



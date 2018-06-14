#include "amsfunc.h"

static int 			amsDataInitialised = 0;
static int			amsCfgDataInitialised = 0;

static int			amsTellerInfoListInitialled = 0;
LIST				amsTellerInfoList;
TELLER_INFO_NODE	*amsTellerInfoListBufPtr = NULL;

static int			amsVtmInfoListInitialled = 0;
LIST				amsVtmInfoList;
VTM_INFO_NODE		*amsVtmInfoListBufPtr = NULL;

static int          freeAmsVtaListInitialled = 0;
pthread_mutex_t     freeAmsVtaListMtx;
LIST 				freeAmsVtaList;
VTA_NODE        	*freeAmsVtaListBufPtr = NULL;

static int          freeAmsVtmListInitialled = 0;
pthread_mutex_t     freeAmsVtmListMtx;
LIST 				freeAmsVtmList;
VTM_NODE            *freeAmsVtmListBufPtr = NULL;


int AmsCfgDataInit()
{
	int i = 0,ret = 0;
	wchar_t	param[1024];

	if(0 != amsCfgDataInitialised)
	{
		return AMS_SUCCESS;
	}

	//init teller info list
	if(AMS_SUCCESS != TellerInfoListInit())
	{
		Display("TellerInfoListInit:Error-tellerinfo nodes error!\r\n");
		return AMS_ERROR;
	}

	//init vtm info list
	if(AMS_SUCCESS != VtmInfoListInit())
	{
		Display("VtmInfoListInit:Error-vtminfo nodes error!\r\n");
		return AMS_ERROR;
	}

	amsCfgDataInitialised = 1;
	
	Display("AmsCfgDataInit Ok!\r\n");
	
	return AMS_SUCCESS;
}


int TellerInfoListInit()
{
	TELLER_INFO_NODE			*pNode;
	int							size;
	int							i;

	if(0 != amsTellerInfoListInitialled)
	{
		return AMS_SUCCESS;
	}

	lstInit(&amsTellerInfoList);

	size = sizeof(TELLER_INFO_NODE) * AMS_MAX_VTA_NODES;
	amsTellerInfoListBufPtr = (TELLER_INFO_NODE *)malloc(size);
	pNode = (TELLER_INFO_NODE *)amsTellerInfoListBufPtr;
	if(NULL == pNode)
	{
		dbgprint("TellerInfoListInit AllocMem[%d]Err\r\n", size);
		return AMS_ALLOC_MEM_FAILED;
	}

	for(i = 0; i < AMS_MAX_VTA_NODES;i++,pNode++)
	{
		lstAdd(&amsTellerInfoList,(NODE *)pNode);
	}

	amsTellerInfoListInitialled = 1;

	return AMS_SUCCESS;
}

int VtmInfoListInit()
{
	VTM_INFO_NODE			*pNode;
	int							size;
	int							i;

	if(0 != amsVtmInfoListInitialled)
	{
		return AMS_SUCCESS;
	}

	lstInit(&amsVtmInfoList);

	size = sizeof(VTM_INFO_NODE) * AMS_MAX_VTM_NODES;
	amsVtmInfoListBufPtr = (VTM_INFO_NODE *)malloc(size);
	pNode = (VTM_INFO_NODE *)amsVtmInfoListBufPtr;
	if(NULL == pNode)
	{
		dbgprint("TellerInfoListInit AllocMem[%d]Err\r\n", size);
		return AMS_ALLOC_MEM_FAILED;
	}

	for(i = 0; i < AMS_MAX_VTA_NODES;i++,pNode++)
	{
		lstAdd(&amsVtmInfoList,(NODE *)pNode);
	}

	amsVtmInfoListInitialled = 1;

	return AMS_SUCCESS;
}
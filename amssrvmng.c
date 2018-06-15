#include "amsfunc.h"


static int 			amsDataInitialised = 0;
static int			amsCfgDataInitialised = 0;


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
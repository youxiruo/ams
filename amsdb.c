#include "amsfunc.h"

int AmsTransformVtc2AmsTellerState(unsigned int vtcTellerState,unsigned int *pAmsTellerState)
{
	int					iret = AMS_OK;

	if(vtcTellerState >= AMS_VTC_TELLER_STATE_RSVD)
	{
		dbgprint("Ams TransformVtc2AmsTellerState Para[%d] Err", vtcTellerState);
		iret = AMS_ERROR;
		return iret;
	}
	
	switch(vtcTellerState)
	{
	case AMS_VTC_TELLER_STATE_IDLE:
 		*pAmsTellerState = AMS_VTA_STATE_IDLE;	
		break;	
	case AMS_VTC_TELLER_STATE_BUSY:
 		*pAmsTellerState = AMS_VTA_STATE_BUSY;
		break;
	case AMS_VTC_TELLER_STATE_REST:
		*pAmsTellerState = AMS_VTA_STATE_REST;			
		break;	
	case AMS_VTC_TELLER_STATE_PREPARE:
		*pAmsTellerState = AMS_VTA_STATE_PREPARE;			
		break;			
	case AMS_VTC_TELLER_STATE_OFFLINE:
		*pAmsTellerState = AMS_VTA_STATE_OFFLINE;			
		break;	
				
	default:
		dbgprint("Ams TransformVtc2AmsTellerState[%d]Err", vtcTellerState);
		iret = AMS_ERROR;
		break;
	}

	return iret;
	
}


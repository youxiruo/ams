#include "amsfunc.h"


//低位在前的长度
int	AmsUnPackParaIdAndLenLE(unsigned char body[], int bodyLen, unsigned char *pParaID, int *lenValue)
{
	if(bodyLen < 3)
	{
		return AMS_ERROR;
	}
	
	*pParaID = body[0];
	*lenValue = body[1]+body[2]*256;
	
	if(bodyLen < (3 + (*lenValue)))
	{
		return AMS_ERROR;
	}
	
	return AMS_OK;
}


//高位在前的长度
int AmsUnPackParaIdAndLenBE(unsigned char body[], int bodyLen, unsigned char *pParaID, int *lenValue)
{
    if(bodyLen <3)
    {
        return AMS_ERROR;
    }

    *pParaID = body[0];
    *lenValue = body[1]*256+body[2];

    if(bodyLen < (3+(*lenValue)))
    {
        return AMS_ERROR;
    }

    return AMS_OK;
}



int AmsUnpackStateOperateReqOpartPara(unsigned char body[], int bodyLen, STATE_OP_INFO *pStateOpInfo)
{
	unsigned char       paraID = 0;
	int                 pos = 0;
	int                 lenValue = 0;
	int                 ret = AMS_ERROR;
	unsigned char       *p;
	
	if(NULL == body || NULL == pStateOpInfo)
	{
		dbgprint("Ams UnpackStateOperateReqOpartPara[0x%x][0x%x]Err!\r\n", body, pStateOpInfo);	
		return AMS_ERROR;		
	}
	
	if(bodyLen < 3)
	{
		dbgprint("Ams UnpackStateOperateReqOpartPara BodyLen[%d] Err!\r\n", bodyLen);	
		return AMS_ERROR;
	}
	
	while(pos < bodyLen)
	{
		paraID = body[pos];
		if(AMS_STATE_OP_INFO_ID == paraID)
		{
			ret = AmsUnPackParaIdAndLenBE(&(body[pos]), bodyLen-pos, &paraID, &lenValue);
			if(AMS_ERROR == ret || lenValue <= 0)
			{
				dbgprint("Ams UnpackStateOperateReqOpartPara Id Len[%d][%d][%d] Err!\r\n",
					bodyLen, pos, lenValue);	
				return AMS_ERROR;
			}
			pos += 3;
		}
		else
		{
			dbgprint("Ams UnpackStateOperateReqOpartPara ParaID[%d] Err!\r\n", paraID);	
			return AMS_ERROR;
		}

		switch(paraID)
		{
		case AMS_STATE_OP_INFO_ID: 
			p = &body[pos];
            BEGETSHORT(pStateOpInfo->reason, p);

			p += 2;
			
			BEGETSHORT(pStateOpInfo->timeLen, p);
			pos += lenValue;
			break;			
	
		default:
			dbgprint("Ams UnpackStateOperateReqOpartPara ParaID[%d] Err.\r\n", paraID);	
			return AMS_ERROR;
		}
	}
	
	if(pos > bodyLen)
	{
		dbgprint("Ams UnpackStateOperateReqOpartPara Len[%d][%d] Err!\r\n", bodyLen, pos);	
		return AMS_ERROR;
	}

	return AMS_OK;
}

/*IP
IPV6
PORT
*/
int AmsUnpackVtaRegReqVpartPara(unsigned char body[], int bodyLen, TERM_NET_INFO *pTermNetInfo)
{
	unsigned char       paraID = 0;
	int                 pos = 0;
	int                 lenValue = 0;
	int                 ret = AMS_ERROR;
	unsigned char       *p;

	if(NULL == body || NULL == pTermNetInfo)
	{
		dbgprint("AmsUnpackVtaRegReqVpartPara Err1");
		return AMS_ERROR;
	}

	if(bodyLen < 3)
	{
		dbgprint("AmsUnpackVtaRegReqVpartPara Err2");
		return AMS_ERROR;
	}

	paraID = body[pos];
	if(paraID != AMS_TERM_NET_INFO_ID)
	{
		dbgprint("AmsUnpackVtaRegReqVpartPara Err3");
		return AMS_ERROR;
	}
	pos+=3;

	p = &body[pos];
	BEGETLONG(pTermNetInfo->ip,p);
	p+=4;

	memcpy(pTermNetInfo->ipv6,p,16);
	p+=16;

	BEGETSHORT(pTermNetInfo->port,p);
	p+=2;

	return AMS_OK;
	
}
int AmsUnpackTellerpersionalinfo(unsigned char body[], int bodyLen, TELLER_PERSONAL_INFO *pTellerPersonalInfo)
{
	unsigned char       paraID = 0;
	int                 pos = 0;
	int                 lenValue = 0;
	int                 ret = AMS_ERROR;
	unsigned char       *p;
	

	if(NULL == body || NULL == pTellerPersonalInfo)
	{
		dbgprint("AmsUnpackTellerpersionalinfo Err");
		return AMS_ERROR;
	}

	if(bodyLen < 3)
	{
		dbgprint("AmsUnpackTellerpersionalinfo Err");
		return AMS_ERROR;
	}

	paraID = body[pos];
	if(paraID != AMS_TELLER_PERSIONALDATA_ID)
	{
		dbgprint("AmsUnpackTellerpersionalinfo Err");
		return AMS_ERROR;
	}

	pos+=2;
	pTellerPersonalInfo->tellerUserNameLen=body[pos++];
	if(pTellerPersonalInfo->tellerUserNameLen > AMS_MAX_NAME_LEN)
	{
		dbgprint("AmsUnpackTellerpersionalinfo usernamelen Err");
		return AMS_ERROR;		
	}
	memcpy(pTellerPersonalInfo->tellerUserName,&body[pos],pTellerPersonalInfo->tellerUserNameLen);
	pos+=pTellerPersonalInfo->tellerUserNameLen;

	pTellerPersonalInfo->tellerNickNameLen=body[pos++];
	if(pTellerPersonalInfo->tellerNickNameLen > AMS_MAX_NAME_LEN)
	{
		dbgprint("AmsUnpackTellerpersionalinfo Err");
		return AMS_ERROR;
	}
	memcpy(pTellerPersonalInfo->tellerNickName,&body[pos],pTellerPersonalInfo->tellerNickNameLen);
	pos+=pTellerPersonalInfo->tellerNickNameLen;

	p = &body[pos];
	
	BEGETSHORT(pTellerPersonalInfo->tellertype,p);
	pos+=2;

	return pos;		
}


int AmsPackVtaLoginBase(unsigned char tellerIdLen,unsigned char tellerId[],int iret,unsigned char p[],LP_AMS_DATA_t	*lpAmsData)
{
	int len=0;
	
	if(tellerId == NULL || p == NULL)
	{
		dbgprint("AmsPackVtaLoginBase Error");
		return 0;
	}

	if(NULL == lpAmsData)
	{
		BEPUTLONG(0,p);
	}
	else
	{
		BEPUTLONG(lpAmsData->amsPid,p);
	}
	p+=4;
	*p++=tellerIdLen;
	memcpy(p,tellerId,tellerIdLen);	
	p+=tellerIdLen;

	if(iret != AMS_VTA_QUEUE_MNG_SUCCESS)
	{
		*p++=AMS_VTA_LOGIN_ERR_ID;
		BEPUTSHORT(4,p);
		p+=2;

		BEPUTLONG(iret,p);
		p+=4;

		return tellerIdLen+12;	
	}
	else
	{
		return tellerIdLen+5;
	}
}
/******************************************************************
*
* FILE NAME						                           VERSION
*	
*	amscfg.c	                                            v1.0.0
*
* DESCRIPTION
*
*	AMS应用管理服务器的配置功能
*
* COPYRIGHT
*	
*	Switch & Network Division, Eastern Communications Company Limited	
*
* HISTORY                                                        
*																  
*     NAME       DATE              REMARKS		            TARGET    
*    edward  	 2015-12-18        Created version 1.0.0    for VTC
******************************************************************/

#include "amsfunc.h"


int ConfigAmsSrv(char *cFileName)
{
	FILE          *fp = NULL;
	char          sItem[8192];
	int           section = CONFIG_AMS;
	int           index = 0;	
	WORD_t        word[MAXWORDNUMALINE];
	int			  wordCount = 0;
	int           count = 0; 
	unsigned int  currId = 0;

	memset(sItem, 0, sizeof(sItem));
	
	if(NULL == (fp = Fopen(cFileName,"r")))
	{
		return -1;
	}

	while(Fgets(sItem, 8192, fp) != NULL)
	{
		memset(&word, 0, 10 * sizeof(WORD_t));//10?
		wordCount = SrvDivSen(sItem,word);
		if(wordCount <= 0) 
		{
			continue;
		}

		section = ConfigSrv(word, wordCount, section, &currId);
		memset(sItem, 0, sizeof(sItem));

		count++;
	}

	Fclose(fp);
	
	if(0 == count)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}


int SrvDivSen(char *s,WORD_t *word)
{
	int         i = 0;
	int         j = 0;
	char        c = 0;
	char        d = 0;
	char        *p = NULL;
	char        *q = NULL;

	DelSpaces(s);
	for(i = 0; (c = s[i]) != 0; i++)
	{
		if(0 == IsSpace(c)) 
		{
			break;
		}
	}

	p=&s[i];

	if((q = strchr(p , '/' )) != NULL)
	{
		if(q[1] == '/')  
		{
			*q='\0';
		}
	}
	
	c = *p;
	
	for(i = 0; (c = p[i]) != 0; i++)
	{
		if('\'' == c || '\"' == c)			
		{
			for(; (d = p[i + 1]) != 0; i++)
			{
				if(d == c)		
				{
					i++;
					break;
				}
			}
		}
		
		LowCase(&p[i]);
	}
	
	for(i = 0; i < MAXWORDNUMALINE; i++)
	{
		if(MMLKEY_NULL == SrvGetAWord(p, &word[i])) 
		{
			break;
		}
		
		if((MMLKEY_STRING == word[i].Code) || (MMLKEY_SECTION == word[i].Code))
		{
			j = 1;
		}
		else
		{
			j = 0;
		}
		
		p = (char *)&word[i].Body[word[i].Len+j];
	}
	
	return i;
	
}


int SrvGetAWord(char *string,WORD_t *word)
{
	char        c = 0;
	char        d = 0;
	char        *s;
	int         p = 0;
	int         i = 0;
	int         j = 0;
	int         k = 0;

	s = string;

	for(i = 0; (c = s[i]) != 0; i++)
	{
		if(0 == IsSpace(c))
		{
			break;
		}
	}
	if(0 == c)
	{
		return MMLKEY_NULL;
	}
	p = i;
	s = &string[p];
	c = *s;

	if ('\'' == c || '\"' == c)			
    {
		for(j = 0; (d = s[j+1]) != 0; j++)
		{
			if(d == c)
			{
				break;
			}
		}
		p++;	
		k = MMLKEY_STRING;			
    }
	else if('[' == c)
	{
		for(j = 0; (d = s[j+1]) != 0; j++)
		{
			if(']' == d)
			{
				break;
			}
		}
		p++;	
		k = MMLKEY_SECTION;			
	}
    else if(IsDecDigit(c))				
    {
		d = s[1];
		if ('0' == c)
		{
			if('x' == d || 'X' == d)
			{
				k = MMLKEY_XNUMBER;
				i += 2;
			}
			else if('o' == d || 'O' == d)
			{
				k = MMLKEY_ONUMBER;
				i += 2;
			}
			else if('b' == d || 'B' == d)
			{
				k = MMLKEY_BNUMBER;
				i += 2;
			}
			else
			{
				k = MMLKEY_HNUMBER;
			}
		}
		else
		{
			k = MMLKEY_HNUMBER;	
		}
		for (j = i; (d = s[j]) != 0; j++)
		{
			if(0 == IsHexDigit(d)) 
			{
				break;
			}
		}

    }
    else if(IsAlpha(c))				
    {
		for(j = 1; (d = s[j]) != 0; j++)
		{
			if(0 == IsHexDigit(d) && 0 == IsAlpha(d))
			{
				break;
			}
		}
		
		k = MMLKEY_ID;				
    }
    else if ('=' == c)
    {
		j = 1;
		k = MMLKEY_EQUAL;
    }
    else if('&' == c)
	{
		d = s[1];
		if ('&' == d)
		{
			j = 2;
			k = MMLKEY_AND;
		}
		else
		{
			j = 1;
			k = MMLKEY_TO;
		}
	}
	else if('.' == c)
	{
		j = 1;
		k = MMLKEY_UNION;
	}
	else if(':' == c)
	{
		for(j = 1; (d = s[j]) != 0; j++)
		{
			if(IsTermChar(d))
			{
				j++;
				break;
			}
		}
		
		k = MMLKEY_CMD;
	}
	else
    {
		k = MMLKEY_NULL;
    }
	
	word->Code = k;			
    word->Body = (unsigned char *)&string[p];
    word->Len = j;		
	
	return k;
	
}


int ConfigSrv(WORD_t *word,int wordcount,int section,unsigned  int *pCurrId)
{
	char			     stringword[100];
	unsigned char        c = 0;
	
	switch(word[0].Code)
	{
	case MMLKEY_SECTION:
		if(wordcount > 1)
		{
			break;
		}
		
		memset(stringword, 0, sizeof(stringword));
		memcpy(stringword, word[0].Body, word[0].Len);
		
		if(0 == strcmp(stringword,"service"))
		{
			section = CONFIG_SERVICE;
		}
		else if(0 == strcmp(stringword,"servicegroup"))
		{
			section = CONFIG_SERVICE_GROUP;
		}	
		else if(0 == strcmp(stringword,"teller"))
		{
			section = CONFIG_TELLER;
		}		
		else if(0 == strcmp(stringword,"authority"))
		{
			section = CONFIG_AUTHORITY;
		}
		else if(0 == strcmp(stringword,"vtm"))
		{
			section = CONFIG_VTM;
		}
		else if(0 == strcmp(stringword,"rcas"))
		{
			section = CONFIG_RCAS;
		}
		else if(0 == strcmp(stringword,"org"))
		{
			section = CONFIG_ORG;
		}		
		else if(0 == strcmp(stringword,"queue"))
		{
			section = CONFIG_QUEUE;
		}			
		break;
		
	case MMLKEY_ID:
		if(wordcount < 3)
		{
			return section;
		}
		
		if(-1 == IsLegSen(word, wordcount))
		{
			return section;
		}
		
		switch(section)
		{
		case CONFIG_SERVICE:
			AmsSrvServiceSenten(word,wordcount,pCurrId);
			break;
		case CONFIG_SERVICE_GROUP:
			AmsSrvServiceGroupSenten(word,wordcount,pCurrId);
			break;	
		case CONFIG_TELLER:
			AmsSrvTellerSenten(word,wordcount,pCurrId);
			break;			
		case CONFIG_AUTHORITY:
			AmsSrvAuthoritySenten(word,wordcount,pCurrId);
			break;
		case CONFIG_VTM:
			AmsSrvVtmSenten(word,wordcount,pCurrId);
			break;
		case CONFIG_RCAS:
			AmsSrvRcasSenten(word,wordcount,pCurrId);
			break;	
		case CONFIG_ORG:
			AmsSrvOrgSenten(word,wordcount,pCurrId);
			break;			
		case CONFIG_QUEUE:
			AmsSrvQueueSenten(word,wordcount,pCurrId);
			break;				
		default:
			break;
		}
		default:
			break;
	}
	
	return section;
	
}

int AmsSrvServiceSenten(WORD_t *word,int wordcount,unsigned int *pCurrId)
{
	char                 stringword[AMS_MAX_STRING_WORD_LEN];
	unsigned char        serviceNameLen;
	int                  i = 0;	
	int                  j = 0;	
	unsigned int         serviceId = 0;

	if(NULL == pCurrId)
	{
		return FAILURE;
	}

	memset(stringword, 0, sizeof(stringword));
	memcpy(stringword, word[0].Body, word[0].Len);
	
	if(0 == strcmp(stringword,"serviceid"))
	{
		serviceId= atoi((const char *)word[2].Body);

		if(0 == serviceId || serviceId > AMS_MAX_SERVICE_ID_VALUE)
		{
			Display("Service Senten serviceId[%u] Err!\r\n",serviceId);
			return FAILURE;
		}
	
		/* record serviceId */
		for(i = 0; i < AMS_MAX_SERVICE_NUM; i++)
		{
			if(AMS_SERVICE_UNINSTALL == AmsCfgService(i).flag)
			{
				AmsCfgService(i).service = serviceId;
				AmsCfgService(i).flag = AMS_SERVICE_INSTALL;
				break;
			}
		}
		
		if(i >= AMS_MAX_SERVICE_NUM)
		{
			Display("All Service has been Installed[%d]!\r\n", serviceId);
			
			*pCurrId = 0x7FFFFFFF;
			return FAILURE;
		}
		
		*pCurrId = serviceId;

		return SUCCESS;
	}

	if(*pCurrId > AMS_MAX_SERVICE_ID_VALUE)
	{
		Display("Service Senten ServiceId[%u] Err!\r\n",*pCurrId);
		return FAILURE;
	}
	
	/* find service in cfg or not */
	for(i = 0; i < AMS_MAX_SERVICE_NUM; i++)
	{
		if(AMS_SERVICE_INSTALL == AmsCfgService(i).flag && AmsCfgService(i).service == *pCurrId)
		{
			break;
		}
	}
	if(i >= AMS_MAX_SERVICE_NUM)
	{
		Display("Service[%d]Senten not find Id!\r\n", *pCurrId);

		*pCurrId = 0x7FFFFFFF;
		return FAILURE;
	}
	
    if(0 == strcmp(stringword,"servicename"))
	{	
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Service[%d]Senten servicename[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, word[2].Len);
			return FAILURE;
		}

		serviceNameLen = strlen(stringword);
        if(serviceNameLen > 0 && serviceNameLen <= AMS_MAX_SERVICE_NAME_LEN)
        {
			/* check service name is used or not */
			for(j = 0; j < AMS_MAX_SERVICE_NUM; j++)
			{
				if(j == i)
				{
					continue;
				}
				
				if(AmsCfgService(j).serviceNameLen == serviceNameLen)
				{
					if(0 == memcmp(AmsCfgService(j).serviceName, stringword, serviceNameLen))
					{
						Display("Service[%d]Senten serviceName[%s]has been Used[%d]!\r\n",
							*pCurrId, stringword, j);
						return FAILURE;
					}
				}
			}

			strcpy((char *)AmsCfgService(i).serviceName, stringword); 	
			AmsCfgService(i).serviceName[serviceNameLen] = '\0';
			AmsCfgService(i).serviceNameLen = serviceNameLen;

//			amsSetServiceStateByServiceName((char *)AmsCfgService(i).serviceName);

			Display("Service[%d][%s]Installed!\r\n", 
				AmsCfgService(i).service, AmsCfgService(i).serviceName);
        }
		else
		{
			Display("Service[%d]Senten servicename[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, serviceNameLen);
			return FAILURE;
		}
	}
	else
	{
		return FAILURE;	
	}

	return SUCCESS;
}

int AmsSrvServiceGroupSenten(WORD_t *word,int wordcount,unsigned int *pCurrId)
{
	char                 stringword[AMS_MAX_STRING_WORD_LEN];
	unsigned char        srvGroupNameLen;
	int                  i = 0;	
	int                  j = 0;	
	unsigned int         srvGrpId = 0;
	unsigned int         serviceId = 0;
	unsigned int         serviceType = 0;	
	unsigned int         srvRsvdType = 0xffffffff;		
	char                 serviceName[AMS_MAX_SERVICE_NAME_LEN];
	unsigned char        isHighLevel = 0;
	
	if(NULL == pCurrId)
	{
		return FAILURE;
	}

	memset(stringword, 0, sizeof(stringword));
	memcpy(stringword, word[0].Body, word[0].Len);
	
	if(0 == strcmp(stringword,"srvgrpid"))
	{
		srvGrpId = atoi((const char *)word[2].Body);

		/* record srvGrpId */
		for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
		{
			if(AMS_SERVICE_GROUP_UNINSTALL == AmsCfgSrvGroup(i).flag)
			{
				AmsCfgSrvGroup(i).srvGrpId = srvGrpId;
				AmsCfgSrvGroup(i).flag = AMS_SERVICE_GROUP_INSTALL;
				break;
			}
		}
		
		if(i >= AMS_MAX_SERVICE_GROUP_NUM)
		{
			Display("All Service Group has been Installed[%d]!\r\n", srvGrpId);
			
			*pCurrId = 0x7FFFFFFF;
			return FAILURE;
		}
		
		*pCurrId = srvGrpId;

		return SUCCESS;
	}

	if(*pCurrId >= AMS_MAX_SERVICE_GROUP_NUM)
	{
		Display("ServiceGroup Senten srvGrpId[%u] Err!\r\n", *pCurrId);
		return FAILURE;
	}
	
	/* find service group in cfg or not */
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		if(AMS_SERVICE_GROUP_INSTALL == AmsCfgSrvGroup(i).flag && AmsCfgSrvGroup(i).srvGrpId == *pCurrId)
		{
			break;
		}
	}
	if(i >= AMS_MAX_SERVICE_GROUP_NUM)
	{
		Display("ServiceGroup[%d]Senten not find Id!\r\n", *pCurrId);

		*pCurrId = 0x7FFFFFFF;
		return FAILURE;
	}
	
    if(0 == strcmp(stringword,"servicegroupname"))
	{	
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("ServiceGroup[%d]Senten serviceGroupName[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, word[2].Len);
			return FAILURE;
		}

		srvGroupNameLen = strlen(stringword);
        if(srvGroupNameLen > 0 && srvGroupNameLen <= AMS_MAX_SERVICE_GROUP_NAME_LEN)
        {
			/* check service group name is used or not */
			for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
			{
				if(j == i)
				{
					continue;
				}
				if(AmsCfgSrvGroup(j).srvGroupNameLen == srvGroupNameLen)
				{
					if(0 == memcmp(AmsCfgSrvGroup(j).srvGroupName, stringword, srvGroupNameLen))
					{
						Display("ServiceGroup[%d]Senten serviceGroupName[%s]has been Used[%d]!\r\n",
							*pCurrId, stringword, j);
						return FAILURE;
					}
				}
			}

			strcpy((char *)AmsCfgSrvGroup(i).srvGroupName, stringword); 	
			AmsCfgSrvGroup(i).srvGroupName[srvGroupNameLen] = '\0';
			AmsCfgSrvGroup(i).srvGroupNameLen = srvGroupNameLen;

//			amsSetServiceStateByServiceName((char *)AmsCfgSrvGroup(i).srvGroupName);

			AmsSrvData(AmsCfgSrvGroup(i).srvGrpId).serviceState = AMS_SERVICE_ACTIVE;
			
			Display("ServiceGroup[%d][%s]Installed!\r\n",
				AmsCfgSrvGroup(i).srvGrpId, AmsCfgSrvGroup(i).srvGroupName);
        }
		else
		{
			Display("ServiceGroup[%d]Senten serviceGroupName[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, srvGroupNameLen);
			return FAILURE;
		}
	}
    if(0 == strcmp(stringword,"serviceid"))
	{
		serviceId = atoi((const char *)word[2].Body);

		if(0 == serviceId || serviceId > AMS_MAX_SERVICE_ID_VALUE)
		{
			Display("ServiceGroup Senten serviceId[%u] Err!\r\n",serviceId);
			return FAILURE;
		}
			
        if(AMS_ERROR == amsGetServiceNameByServiceId(serviceId, serviceName))
        {
			Display("ServiceGroup[%u]Senten GetServiceNameByServiceId[%d]Err!\r\n",
				*pCurrId, serviceId);
			return FAILURE;	
        }

		serviceType = (1 << (serviceId - 1));
		
		AmsCfgSrvGroup(i).srvTypeRsvd |= srvRsvdType;
		AmsCfgSrvGroup(i).serviceType |= serviceType;

		Display("ServiceGroup[%u]Service[%d][0x%x] Installed!\r\n", 
			AmsCfgSrvGroup(i).srvGrpId, serviceId, AmsCfgSrvGroup(i).serviceType);
	}	
    if(0 == strcmp(stringword,"ishighlevel"))
	{
		isHighLevel = atoi((const char *)word[2].Body);

		if(isHighLevel > 1)
		{
			Display("ServiceGroup[%d] Senten isHighLevel[%d] Err!\r\n",*pCurrId, isHighLevel);
			return FAILURE;
		}

		AmsCfgSrvGroup(i).isHighLevel = isHighLevel;
	}		
	else
	{
		return FAILURE;	
	}

	return SUCCESS;
}

int AmsSrvTellerSenten(WORD_t *word,int wordcount,unsigned int *pCurrId)
{
	char                 stringword[AMS_MAX_STRING_WORD_LEN];
	unsigned char        tellerNoLen;
	unsigned char        tellerNameLen;
	unsigned char        tellerPwdLen;
	unsigned char        tellerTypeLen;
//	unsigned char        srvGrpIdLen;	
	unsigned char        srvGrpId;
	unsigned char        tellerDepartmentNameLen;
	unsigned char        areaCodeLen;                   //柜员电话区号长度	
	unsigned char        phoneNumLen;                   //柜员电话号码长度	
	unsigned char        tellerVNCAuthPwdLen;	
	unsigned char        transIpLen = 0;
	unsigned int         vtaIp = 0;
	unsigned int         orgId = 0;
//	unsigned char		 transIpAddr[AMS_MAX_TRANS_IP_LEN + 1];	
	int                  i = 0;
	int                  j = 0;	
	unsigned int         id = 0;
	int                  idPos = -1;
	int                  vtaNum = 0;
	int                  result = FAILURE;

	if(NULL == pCurrId)
	{
		return FAILURE;
	}
	
	memset(stringword, 0, sizeof(stringword));
	memcpy(stringword, word[0].Body, word[0].Len);
	vtaNum = Min(SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NUM);
	
	if(0 == strcmp(stringword,"id"))
	{
		id = atoi((const char *)word[2].Body);

		if(0 == id)
		{
			Display("Teller[%u]Senten id[%u] Err!\r\n",*pCurrId, id);
			return FAILURE;			
		}
		
		/* check teller in cfg or not */
		for(i = 0; i < vtaNum; i++)
		{
			if(AmsCfgTeller(i).tellerId == id)
			{
				Display("Teller[%u]Senten id[%u] has been used!\r\n",*pCurrId, id);

				memset(&AmsCfgTeller(i), 0, sizeof(TELLER_INFO));

				AmsCfgTeller(i).tellerId = id;
				AmsCfgTeller(i).flag = AMS_TELLER_INSTALL;
					
				idPos = i;
				break;
				
//				return FAILURE;
			}
		}

		if(-1 == idPos)
		{
			/* record tellerId */
			for(i = 0; i < vtaNum; i++)
			{
				if(AMS_TELLER_UNINSTALL == AmsCfgTeller(i).flag)
				{
					AmsCfgTeller(i).tellerId = id;
					AmsCfgTeller(i).flag = AMS_TELLER_INSTALL;
					break;
				}
			}
			
			if(i >= vtaNum)
			{
				Display("All Teller has been Installed[%u]!\r\n", id);
				*pCurrId = 0;
				return FAILURE;
			}
		}
		
		*pCurrId = id;

		Display("Teller[%u] Installed!\r\n", id);
			
		return SUCCESS;
	}

	/* find teller in cfg or not */
	for(i = 0; i < vtaNum; i++)
	{
		if(AMS_TELLER_INSTALL == AmsCfgTeller(i).flag && AmsCfgTeller(i).tellerId == *pCurrId)
		{
			break;
		}
	}
	if(i >= vtaNum)
	{
		Display("Teller[%u]Senten not find Id!\r\n", *pCurrId);

		*pCurrId = 0;
		return FAILURE;
	}
	
    if(0 == strcmp(stringword,"tellerno"))
	{
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Teller[%u]Senten tellerNo[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, word[2].Len);
			return FAILURE;
		}

		tellerNoLen = strlen(stringword);
        if(tellerNoLen > 0 && tellerNoLen <= AMS_MAX_TELLER_NO_LEN)
        {
			/* check tellerNo is used or not */
			for(j = 0; j < vtaNum; j++)
			{
				if(j == i)
				{
					continue;
				}
				
				if(AmsCfgTeller(j).tellerNoLen == tellerNoLen)
				{
					if(0 == memcmp(AmsCfgTeller(j).tellerNo, stringword, tellerNoLen))
					{
						Display("Teller[%u]Senten tellerNo[%s]has been Used[%d]!\r\n",
							*pCurrId, stringword, j);
						return FAILURE;
					}
				}
			}
		
			strcpy((char *)AmsCfgTeller(i).tellerNo, stringword); 	
			AmsCfgTeller(i).tellerNo[tellerNoLen] = '\0';
			AmsCfgTeller(i).tellerNoLen = tellerNoLen;
			        }
		else
		{
			if(0 == tellerNoLen)
			{
				Display("Teller[%u]Senten tellerNo is Null!\r\n",*pCurrId);
			}
			else
			{
				Display("Teller[%u]Senten tellerNo[%s] len[%d]Err!\r\n",
					*pCurrId, stringword, tellerNoLen);
			}

			return FAILURE;
		}		
	}	
    else if(0 == strcmp(stringword,"password"))
	{
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Teller[%u]Senten password[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, word[2].Len);
			return FAILURE;
		}

		tellerPwdLen = strlen(stringword);
        if(tellerPwdLen >= 0 && tellerPwdLen <= AMS_MAX_PWD_LEN)
        {
			strcpy((char *)AmsCfgTeller(i).tellerPwd, stringword); 	
			AmsCfgTeller(i).tellerPwd[tellerPwdLen] = '\0';
			AmsCfgTeller(i).tellerPwdLen = tellerPwdLen;
        }
		else
		{
			Display("Teller[%u]Senten Pwd[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, tellerPwdLen);
			return FAILURE;
		}
	}
    else if(0 == strcmp(stringword,"type"))
	{
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Teller[%u]Senten type[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, word[2].Len);
			return FAILURE;
		}

		tellerTypeLen = strlen(stringword);
        if(tellerTypeLen > 0 && tellerTypeLen <= AMS_MAX_TELLER_TYPE_LEN)
        {
			result = amsGetTellerType(stringword, (char *)&AmsCfgTeller(i).tellerType);		

			if(SUCCESS != result)//SUCCESS = ASM_OK
			{
				Display("Teller[%u]Senten Type[%s] Err!\r\n",*pCurrId, stringword);
			}
			
			return FAILURE;			
        }
		else
		{
			Display("Teller[%u]Senten Type[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, tellerTypeLen);
			return FAILURE;
		}			
	}
	else if(0 == strcmp(stringword,"srvgrpid"))
	{
		srvGrpId = atoi((const char *)word[2].Body);
		
		if(srvGrpId > AMS_SERVICE_GROUP_ID_MAX)
		{
			Display("Teller[%u]Senten srvGrpId[%d] Err!\r\n",*pCurrId, srvGrpId);
			return FAILURE;
		}		

		AmsCfgTeller(i).srvGrpId = srvGrpId;
/*		
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Teller[%u]Senten srvGrpId[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, word[2].Len);
			return FAILURE;
		}

		srvGrpIdLen = strlen(stringword);
        if(srvGrpIdLen > 0 && srvGrpIdLen <= AMS_MAX_SERVICE_GROUP_NUM_LEN)
        {
			result = amsGetSrvGrpId(stringword, (char *)&AmsCfgTeller(i).srvGrpId);		

			if(SUCCESS != result)//SUCCESS = ASM_OK
			{
				Display("Teller[%u]Senten SrvGrpId[%s] Err!\r\n",*pCurrId, stringword);
			}
			
			return FAILURE;			
        }
		else if(0 == srvGrpIdLen || srvGrpIdLen > AMS_MAX_SERVICE_GROUP_NUM_LEN)
		{
			Display("Teller[%u]Senten SrvGrpId[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, srvGrpIdLen);
			return FAILURE;
		}*/
	}	
    else if(0 == strcmp(stringword,"name"))
	{
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Teller[%u]Senten name[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, word[2].Len);
			return FAILURE;
		}

		tellerNameLen = strlen(stringword);
        if(tellerNameLen > 0 && tellerNameLen <= AMS_MAX_NAME_LEN)
        {
			/* check teller name is used or not */
			for(j = 0; j < vtaNum; j++)
			{
				if(j == i)
				{
					continue;
				}
				
				if(AmsCfgTeller(j).tellerNameLen == tellerNameLen)
				{
					if(0 == memcmp(AmsCfgTeller(j).tellerName, stringword, tellerNameLen))
					{
						Display("Teller[%u]Senten name[%s]has been Used[%d]!\r\n",
							*pCurrId, stringword, j);
						return FAILURE;
					}
				}
			}
		
			strcpy((char *)AmsCfgTeller(i).tellerName, stringword); 	
			AmsCfgTeller(i).tellerName[tellerNameLen] = '\0';
			AmsCfgTeller(i).tellerNameLen = tellerNameLen;
			        }
		else
		{
			if(0 == tellerNameLen)
			{
				Display("Teller[%u]Senten Name is Null!\r\n",*pCurrId);
			}
			else
			{
				Display("Teller[%u]Senten Name[%s] len[%d]Err!\r\n",
					*pCurrId, stringword, tellerNameLen);
			}

			return FAILURE;
		}		
	}
    else if(0 == strcmp(stringword,"idstate"))
	{
        AmsCfgTeller(i).idState = atoi((const char *)word[2].Body);
	}	
	else if(0 == strcmp(stringword,"department"))
	{
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Teller[%u]Senten department[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, word[2].Len);
			return FAILURE;
		}

		tellerDepartmentNameLen = strlen(stringword);
        if(tellerDepartmentNameLen >= 0 && tellerDepartmentNameLen <= AMS_MAX_DEPARTMENT_NAME_LEN)
        {
			strcpy((char *)AmsCfgTeller(i).tellerDepartmentName, stringword); 
			AmsCfgTeller(i).tellerDepartmentName[tellerDepartmentNameLen] = '\0';
			AmsCfgTeller(i).tellerDepartmentNameLen = tellerDepartmentNameLen;
        }
		else
		{
			Display("Teller[%u]Senten Department[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, tellerDepartmentNameLen);
			return FAILURE;
		}
	}	
    else if(0 == strcmp(stringword,"areacode"))
	{
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Teller[%u]Senten areaCode[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, word[2].Len);
			return FAILURE;
		}

		areaCodeLen = strlen(stringword);
        if(areaCodeLen >= 0 && areaCodeLen <= AMS_MAX_AREA_CODE_NUM)
        {
			strcpy((char *)AmsCfgTeller(i).areaCode, stringword); 
			AmsCfgTeller(i).areaCode[areaCodeLen] = '\0';
			AmsCfgTeller(i).areaCodeLen = areaCodeLen;
        }
		else
		{
			Display("Teller[%u]Senten AreaCode[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, areaCodeLen);
			return FAILURE;
		}
	}	
    else if(0 == strcmp(stringword,"phone"))
	{
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Teller[%u]Senten phone[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, word[2].Len);
			return FAILURE;
		}

		phoneNumLen = strlen(stringword);
        if(phoneNumLen >= 0 && phoneNumLen <= AMS_MAX_PHONE_NUM)
        {
			strcpy((char *)AmsCfgTeller(i).phoneNum, stringword); 
			AmsCfgTeller(i).phoneNum[phoneNumLen] = '\0';
			AmsCfgTeller(i).phoneNumLen = phoneNumLen;
        }
		else
		{
			Display("Teller[%u]Senten Phone[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, phoneNumLen);
			return FAILURE;
		}
	}
/*	else if(0 == strcmp(stringword,"vncauthpwd"))
	{
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Teller[%u]Senten VncAuthPwd[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, word[2].Len);
			return FAILURE;
		}

		tellerVNCAuthPwdLen = strlen(stringword);
        if(tellerVNCAuthPwdLen >= 0 && tellerVNCAuthPwdLen <= AMS_MAX_PWD_LEN)
        {
			strcpy((char *)AmsCfgTeller(i).vncAuth.password, stringword); 
			AmsCfgTeller(i).vncAuth.password[tellerVNCAuthPwdLen] = '\0';
			AmsCfgTeller(i).vncAuth.passwordLen = tellerVNCAuthPwdLen;
			AmsCfgTeller(i).vncAuth.flag = 1;
        }
		else
		{
			Display("Teller[%u]Senten VncAuthPwd[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, tellerVNCAuthPwdLen);
			return FAILURE;
		}
	}		*/
	else if(0 == strcmp(stringword,"transip"))
	{
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Teller[%u]Senten transip[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, word[2].Len);
			return FAILURE;
		}

		transIpLen = strlen(stringword);
        if(transIpLen >= 0 && transIpLen <= AMS_MAX_TRANS_IP_LEN)//配置上允许为空，但业务上可能不允许
        {
			strcpy((char *)AmsCfgTeller(i).transIp, stringword); 	
			AmsCfgTeller(i).transIp[transIpLen] = '\0';
			AmsCfgTeller(i).transIpLen = transIpLen;
//			AmsCfgTeller(i).vtaIp = inet_addr((const char FAR *)transIpAddr);			
//			AmsCfgTeller(i).vtaIp = inet_addr(AmsCfgTeller(i).transIp);


//			AmsCfgTeller(i).vtaIp = inet_network((const char FAR *)AmsCfgTeller(i).transIp);
//			AmsCfgTeller(i).vtaIp = inet_network(AmsCfgTeller(i).transIp);

			vtaIp = inet_addr(AmsCfgTeller(i).transIp);
			AmsCfgTeller(i).vtaIp = htonl(vtaIp);
			
        }
		else
		{
			Display("Teller[%u]Senten TransIp[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, transIpLen);
			return FAILURE;
		}
	}	
	else if(0 == strcmp(stringword,"orgid"))
	{
		orgId = atoi((const char *)word[2].Body);

		AmsCfgTeller(i).orgId = orgId;
	}
    else
	{
        return FAILURE;
	}

	return SUCCESS;
}

int AmsSrvAuthoritySenten(WORD_t *word,int wordcount,unsigned int *pCurrId)
{
	char                 stringword[AMS_MAX_STRING_WORD_LEN];
	int                  i = 0;
	unsigned int         tellerId = 0;
	unsigned int         serviceId = 0;
	unsigned int         serviceType = 0;	
	unsigned int         srvRsvdType = 0xffffffff;		
	char                 serviceName[AMS_MAX_SERVICE_NAME_LEN];
	int                  vtaNum = 0;

	if(NULL == pCurrId)
	{
		return FAILURE;
	}
		
	memset(stringword, 0, sizeof(stringword));
	memcpy(stringword, word[0].Body, word[0].Len);
	vtaNum = Min(SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NUM);
		
	if(0 == strcmp(stringword,"tellerid"))
	{
		tellerId = atoi((const char *)word[2].Body);

		if(0 == tellerId)
		{
			Display("Authority[%u]Senten id[%u] Err!\r\n",*pCurrId, tellerId);
			return FAILURE;			
		}
			
		/* find teller in cfg or not */
		for(i = 0; i < vtaNum; i++)
		{
			if(AmsCfgTeller(i).tellerId == tellerId)
			{
				break;
			}
		}
		if(i >= vtaNum)
		{
			Display("Authority Senten not Find TellerId[%u]Err!\r\n",tellerId);
			*pCurrId = 0;
			return FAILURE;
		}

		*pCurrId = tellerId;
		
		return SUCCESS;
	}


	/* find teller in cfg or not */
	for(i = 0; i < vtaNum; i++)
	{
		if(AMS_TELLER_INSTALL == AmsCfgTeller(i).flag && AmsCfgTeller(i).tellerId == *pCurrId)
		{
			break;
		}
	}
	if(i >= vtaNum)
	{
		Display("Authority[%u]Senten not find Id!\r\n", *pCurrId);

		*pCurrId = 0;
		return FAILURE;
	}
	
    if(0 == strcmp(stringword,"serviceid"))
	{
		serviceId = atoi((const char *)word[2].Body);

		if(0 == serviceId || serviceId > AMS_MAX_SERVICE_ID_VALUE)
		{
			Display("Authority Senten serviceId[%u] Err!\r\n",serviceId);
			return FAILURE;
		}
			
        if(AMS_ERROR == amsGetServiceNameByServiceId(serviceId, serviceName))
        {
			Display("Authority[%u]Senten GetServiceNameByServiceId[%d]Err!\r\n",
				*pCurrId, serviceId);
			return FAILURE;	
        }

		serviceType = (1 << (serviceId - 1));

	    if(AMS_ERROR == amsCheckServiceGroupServiceType(AmsCfgTeller(i).srvGrpId, serviceType, srvRsvdType))
        {
			Display("Authority[%u]Senten CheckServiceGroup[%d]ServiceType[0x%x]Err!\r\n",
				*pCurrId, AmsCfgTeller(i).srvGrpId, serviceType);
			return FAILURE;	
        }	
		
  /*		
        if(AMS_ERROR == amsGetServiceTypeByServiceName(serviceName, &serviceType, &srvRsvdType))
        {
			Display("Authority[%u]Senten GetServiceTypeByServiceName[%s]Err!\r\n",*pCurrId, serviceName);
			return FAILURE;	
        }

		if(AMS_TELLER_MANAGER != AmsCfgTeller(i).tellerType &&
		   AMS_TELLER_INSPECTOR != AmsCfgTeller(i).tellerType)
		{
			if(AMS_SERVICE_SYSTEM_MAINTENANCE == serviceType)
			{
				Display("Authority[%u]Senten tellerType[%d] Cannot cfg system maintenance Auth!\r\n",
					*pCurrId, AmsCfgTeller(i).tellerType);
				return FAILURE;					
			}
		}

		if(AMS_TELLER_INSPECTOR != AmsCfgTeller(i).tellerType)
		{
			if(AMS_SERVICE_MONITOR == serviceType)
			{
				Display("Authority[%u]Senten tellerType[%d] Cannot cfg monitor Auth!\r\n",
					*pCurrId, AmsCfgTeller(i).tellerType);
				return FAILURE;					
			}
		}*/
		
		AmsCfgTeller(i).tellerSrvAuthRsvd |= srvRsvdType;
		AmsCfgTeller(i).tellerSrvAuth |= serviceType;

		Display("Teller[%u]Authority[%d][0x%x] Installed!\r\n", 
			AmsCfgTeller(i).tellerId, serviceId, AmsCfgTeller(i).tellerSrvAuth);
	}
	else
	{
		return FAILURE;	
	}
	
	return SUCCESS;
}

int AmsSrvVtmSenten(WORD_t *word,int wordcount,unsigned int *pCurrId)
{
	char                 stringword[AMS_MAX_STRING_WORD_LEN];
	unsigned char        vtmNoLen;
	unsigned char        vtmNameLen;
	unsigned char        vtmPwdLen;
	unsigned char        vtmTypeLen;
	unsigned char        vtmVNCAuthPwdLen;
	unsigned char        transIpLen = 0;
	unsigned int         vtmIp = 0;
	unsigned char        remoteCoopType = 0;	
	unsigned int         orgId = 0;
//	unsigned char		 transIpAddr[AMS_MAX_TRANS_IP_LEN + 1];
	int                  i = 0;
	int                  j = 0;	
	unsigned int         id = 0;
	int                  idPos = -1;	
	int                  vtmNum = 0;
	int                  result = 0;
	
	if(NULL == pCurrId)
	{
		return FAILURE;
	}

	memset(stringword, 0, sizeof(stringword));
	memcpy(stringword, word[0].Body, word[0].Len);
	vtmNum = Min(SystemData.AmsPriData.amsCfgData.maxVtmNum, AMS_MAX_VTM_NUM);

	if(0 == strcmp(stringword,"id"))
	{
		id = atoi((const char *)word[2].Body);

		if(0 == id)
		{
			Display("Vtm[%u]Senten id[%u]Err!\r\n",*pCurrId, id);
			return FAILURE;
		}

		/* check vtm in cfg or not */
		for(i = 0; i < vtmNum; i++)
		{
			if(AmsCfgVtm(i).vtmId == id)
			{
				Display("Vtm[%u]Senten id[%u] has been used!\r\n",*pCurrId, id);

				memset(&AmsCfgVtm(i), 0, sizeof(VTM_INFO));
				
				AmsCfgVtm(i).vtmId = id;
				AmsCfgVtm(i).flag = AMS_VTM_INSTALL;
					
				idPos = i;
				break;
//				return FAILURE;
			}
		}

		if(-1 == idPos)
		{
			/* record vtm Id */
			for(i = 0; i < vtmNum; i++)
			{
				if(AMS_VTM_UNINSTALL == AmsCfgVtm(i).flag)
				{
					AmsCfgVtm(i).vtmId = id;
					AmsCfgVtm(i).flag = AMS_VTM_INSTALL;
					break;
				}
			}
			
			if(i >= vtmNum)
			{
				Display("All Vtm has been Installed[%d]!\r\n", id);
				*pCurrId = 0;
				return FAILURE;
			}
		}
		
		*pCurrId = id;

		Display("Vtm[%u] Installed!\r\n", id);

		return SUCCESS;
	}

	/* find vtm in cfg or not */
	for(i = 0; i < vtmNum; i++)
	{
		if(AMS_VTM_INSTALL == AmsCfgVtm(i).flag && AmsCfgVtm(i).vtmId == *pCurrId)
		{
			break;
		}
	}
	if(i >= vtmNum)
	{
		Display("Vtm[%u]Senten not find Id!\r\n", *pCurrId);

		*pCurrId = 0;
		return FAILURE;
	}

	if(0 == strcmp(stringword,"vtmno"))
	{
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Vtm[%u]Senten vtmNo[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, word[2].Len);
			return FAILURE;
		}

		vtmNoLen = strlen(stringword);
        if(vtmNoLen > 0 && vtmNoLen <= AMS_MAX_NAME_LEN)
        {
			/* check vtmNo is used or not */
			for(j = 0; j < vtmNum; j++)
			{
				if(j == i)
				{
					continue;
				}
				
				if(AmsCfgVtm(j).vtmNoLen == vtmNoLen)
				{
					if(0 == memcmp(AmsCfgVtm(j).vtmNo, stringword, vtmNoLen))
					{
						Display("Vtm[%u]Senten vtmNo[%s]has been Used[%d]!\r\n",
							*pCurrId, stringword, j);
						return FAILURE;
					}
				}
			}

			strcpy(AmsCfgVtm(i).vtmNo, stringword); 	
			AmsCfgVtm(i).vtmNo[vtmNoLen] = '\0';
			AmsCfgVtm(i).vtmNoLen = vtmNoLen;
        }
		else
		{
			if(0 == vtmNoLen)
			{
				Display("Vtm[%u]Senten vtmNo is Null!\r\n",*pCurrId);
			}
			else
			{
				Display("Vtm[%u]Senten vtmNo[%s] len[%d]Err!\r\n",
					*pCurrId, stringword, vtmNoLen);
			}
			
			return FAILURE;
		}
	}	
    else if(0 == strcmp(stringword,"password"))
	{	
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Vtm[%u]Senten password[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, word[2].Len);
			return FAILURE;
		}

		vtmPwdLen = strlen(stringword);
        if(vtmPwdLen >= 0 && vtmPwdLen <= AMS_MAX_PWD_LEN)
        {
			strcpy((char *)AmsCfgVtm(i).vtmPwd, stringword); 
			AmsCfgVtm(i).vtmPwd[vtmPwdLen] = '\0';
			AmsCfgVtm(i).vtmPwdLen = vtmPwdLen;
        }
		else
		{
			Display("Vtm[%u]Senten Pwd[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, vtmPwdLen);
			return FAILURE;
		}
	}
    else if(0 == strcmp(stringword,"type"))
	{
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Vtm[%u]Senten type[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, word[2].Len);
			return FAILURE;
		}

		vtmTypeLen = strlen(stringword);
        if(vtmTypeLen > 0 && vtmTypeLen <= AMS_MAX_TERMINAL_TYPE_LEN)
        {
			result = amsGetTerminalType(stringword, (char *)&AmsCfgVtm(i).terminalType);	

			if(AMS_OK != result || AMS_TERMINAL_VTM != AmsCfgVtm(i).terminalType)//SUCCESS = ASM_OK
			{
				Display("Vtm[%u]Senten Type[%s] Err!\r\n",*pCurrId, stringword);
			}
			return FAILURE;
        }
		else
		{
			Display("Vtm[%u]Senten Type[%s] len[%d]Err!\r\n",*pCurrId, stringword, vtmTypeLen);
			return FAILURE;
		}
	}	
	else if(0 == strcmp(stringword,"name"))
	{
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Vtm[%u]Senten name[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, word[2].Len);
			return FAILURE;
		}

		vtmNameLen = strlen(stringword);
        if(vtmNameLen > 0 && vtmNameLen <= AMS_MAX_NAME_LEN)
        {
			/* check vtm name is used or not */
			for(j = 0; j < vtmNum; j++)
			{
				if(j == i)
				{
					continue;
				}
				
				if(AmsCfgVtm(j).vtmNameLen == vtmNameLen)
				{
					if(0 == memcmp(AmsCfgVtm(j).vtmName, stringword, vtmNameLen))
					{
						Display("Vtm[%u]Senten name[%s]has been Used[%d]!\r\n",
							*pCurrId, stringword, j);
						return FAILURE;
					}
				}
			}

			strcpy(AmsCfgVtm(i).vtmName, stringword); 	
			AmsCfgVtm(i).vtmName[vtmNameLen] = '\0';
			AmsCfgVtm(i).vtmNameLen = vtmNameLen;
        }
		else
		{
			if(0 == vtmNameLen)
			{
				Display("Vtm[%u]Senten Name is Null!\r\n",*pCurrId);
			}
			else
			{
				Display("Vtm[%u]Senten Name[%s] len[%d]Err!\r\n",
					*pCurrId, stringword, vtmNameLen);
			}
			
			return FAILURE;
		}	
	}	
/*	else if(0 == strcmp(stringword,"vncauthpwd"))
	{
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Vtm[%u]Senten vncauthpwd[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, word[2].Len);
			return FAILURE;
		}

		vtmVNCAuthPwdLen = strlen(stringword);
        if(vtmVNCAuthPwdLen >= 0 && vtmVNCAuthPwdLen <= AMS_MAX_PWD_LEN)
        {
			strcpy((char *)AmsCfgVtm(i).vncAuth.password, stringword);
			AmsCfgVtm(i).vncAuth.password[vtmVNCAuthPwdLen] = '\0';
			AmsCfgVtm(i).vncAuth.passwordLen = vtmVNCAuthPwdLen;
			AmsCfgVtm(i).vncAuth.flag = 1;
        }
		else
		{
			Display("Vtm[%u]Senten VncAuthPwd[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, vtmVNCAuthPwdLen);
			return FAILURE;
		}
	}	*/
	else if(0 == strcmp(stringword,"transip"))
	{
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Vtm[%u]Senten transip[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, word[2].Len);
			return FAILURE;
		}

		transIpLen = strlen(stringword);
        if(transIpLen >= 0 && transIpLen <= AMS_MAX_TRANS_IP_LEN)//配置上允许为空，但业务上可能不允许
        {
			strcpy((char *)AmsCfgVtm(i).transIp, stringword); 	
			AmsCfgVtm(i).transIp[transIpLen] = '\0';
			AmsCfgVtm(i).transIpLen = transIpLen;
//			AmsCfgVtm(i).vtmIp = inet_addr((const char FAR *)transIpAddr);
//			AmsCfgVtm(i).vtmIp = inet_addr(AmsCfgVtm(i).transIp);

//			AmsCfgVtm(i).vtmIp = inet_network((const char FAR *)AmsCfgVtm(i).transIp);
//			AmsCfgVtm(i).vtmIp = inet_network(AmsCfgVtm(i).transIp);

			vtmIp = inet_addr(AmsCfgVtm(i).transIp);
			AmsCfgVtm(i).vtmIp = htonl(vtmIp);
        }
		else
		{
			Display("Vtm[%u]Senten TransIp[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, transIpLen);
			return FAILURE;
		}
	}
	else if(0 == strcmp(stringword,"remotecooptype"))//Remote Cooperative
	{
		remoteCoopType = atoi((const char *)word[2].Body);
		if(remoteCoopType >= AMS_TRANS_TYPE_MAX)
		{
			Display("Vtm[%u]Senten RemoteCoopType[%d]Err!\r\n", *pCurrId, remoteCoopType);
			return FAILURE;
		}
		
		AmsCfgVtm(i).vtmRemoteCoopType = remoteCoopType;		
	}
	else if(0 == strcmp(stringword,"remotecoopport"))//Remote Cooperative
	{
		AmsCfgVtm(i).vtmRemoteCoopPort = atoi((const char *)word[2].Body);
	}
	else if(0 == strcmp(stringword,"orgid"))
	{
		AmsCfgVtm(i).orgId = atoi((const char *)word[2].Body);
	}	
	else
	{
		return FAILURE;	
	}
	
	return SUCCESS;
}

int AmsSrvRcasSenten(WORD_t *word,int wordcount,unsigned int *pCurrId)
{
	char             stringword[AMS_MAX_STRING_WORD_LEN];
	unsigned int     securitiesType = 0;
	unsigned int     transType = 0;
	unsigned int     encoding = 0;
	unsigned int     id = 0;
	int              idPos = -1;	
	int              rcasNum = 0;
	int              i = 0;
//	int              j = 0;
	
	memset(stringword, 0, sizeof(stringword));
	memcpy(stringword, word[0].Body, word[0].Len);
	rcasNum = Min(SystemData.AmsPriData.amsCfgData.maxRcasNum, AMS_MAX_RCAS_NUM);
#if 0
	if(0 == strcmp(stringword,"id"))
	{
		id = atoi((const char *)word[2].Body);

		if(0 == id)
		{
			Display("Rcas[%u]Senten id[%u]Err!\r\n",*pCurrId, id);
			return FAILURE;
		}

		/* check rcas in cfg or not */
		for(i = 0; i < rcasNum; i++)
		{
			if(AmsCfgRcas(i).cModuleId == id)
			{
				Display("Rcas[%u]Senten id[%u] has been used!\r\n",*pCurrId, id);

				memset(&AmsCfgRcas(i), 0, sizeof(RCAS_INFO));
				
				AmsCfgRcas(i).cModuleId = id;
				AmsCfgRcas(i).flag = AMS_RCAS_INSTALL;
					
				idPos = i;
				break;
//				return FAILURE;
			}
		}

		if(-1 == idPos)
		{
			/* record rcas Id */
			for(i = 0; i < rcasNum; i++)
			{
				if(AMS_RCAS_UNINSTALL == AmsCfgRcas(i).flag)
				{
					AmsCfgRcas(i).cModuleId = id;
					AmsCfgRcas(i).flag = AMS_RCAS_INSTALL;
					break;
				}
			}
			
			if(i >= rcasNum)
			{
				Display("All Rcas has been Installed[%d]!\r\n", id);
				*pCurrId = 0;
				return FAILURE;
			}
		}
		
		*pCurrId = id;

		Display("Rcas[%u] Installed!\r\n", id);

		return SUCCESS;
	}

	/* find rcas in cfg or not */
	for(i = 0; i < rcasNum; i++)
	{
		if(AMS_RCAS_INSTALL == AmsCfgRcas(i).flag && AmsCfgRcas(i).cModuleId == *pCurrId)
		{
			break;
		}
	}
	if(i >= rcasNum)
	{
		Display("Rcas[%u]Senten not find Id!\r\n", *pCurrId);

		*pCurrId = 0;
		return FAILURE;
	}
#endif
/*	if(0 == strcmp(stringword,"screenrecinterval"))//Screen Record
	{
		AmsCfgRcas.screenRecMode.interval = atoi((const char *)word[2].Body);
	}
	else if(0 == strcmp(stringword,"sharedesktop"))//Screen Record
	{
		AmsCfgRcas.rfbInitPara.shareDesktopFlag = atoi((const char *)word[2].Body);
	}
    else if(0 == strcmp(stringword,"vtascreenrectranstype"))//Screen Record
	{	
		transType = atoi((const char *)word[2].Body);
		if(transType >= AMS_TRANS_TYPE_MAX )
		{
			Display("Rcas Senten Vtascreenrectranstype[%s]Err!\r\n", transType);
			return FAILURE;
		}
		
		AmsCfgRcas.rfbInitPara.transProto.transType = transType;
	}
	else if(0 == strcmp(stringword,"vtascreenrectransport"))//Screen Record
	{
		AmsCfgRcas.rfbInitPara.transProto.transPort = atoi((const char *)word[2].Body);
	}		
	else if(0 == strcmp(stringword,"vtascreenrecsecutype"))//可多个 //Screen Record
	{
		securitiesType = atoi((const char *)word[2].Body);
		if(AmsCfgRcas.rfbInitPara.securities.num >= AMS_MAX_SECURITIES_NUM)
		{
			Display("Rcas Senten VtaScreenRecSecuNum[%d]Err[%d]!\r\n",
				AmsCfgRcas.rfbInitPara.securities.num, securitiesType);
			return FAILURE;
		}

		for(i = 0; i < AmsCfgRcas.rfbInitPara.securities.num; i++)
		{
			if(securitiesType == AmsCfgRcas.rfbInitPara.securities.type[i])
			{
				Display("Rcas Senten VtaScreenRecSecuType[%d]has been cfg[%d]!\r\n",
					securitiesType, i);
				return FAILURE;
			}
		}
		
		AmsCfgRcas.rfbInitPara.securities.type[AmsCfgRcas.rfbInitPara.securities.num] = securitiesType;
		
		AmsCfgRcas.rfbInitPara.securities.num++;

		if(0 == AmsCfgRcas.rfbInitPara.securities.flag)
		{
			AmsCfgRcas.rfbInitPara.securities.flag = 1;
		}	

		Display("Rcas Senten VtaScreenRecSecu[%d]is Type[%d]!\r\n",
			AmsCfgRcas.rfbInitPara.securities.num, securitiesType);
	}*/
//	else 
	if(0 == strcmp(stringword,"id"))
	{
		if(NULL == pCurrId)
		{
			return FAILURE;
		}
		
		id = atoi((const char *)word[2].Body);

		if(0 == id)
		{
			Display("Rcas[%u]Senten id[%u]Err!\r\n",*pCurrId, id);
			return FAILURE;
		}

		/* check rcas in cfg or not */
		for(i = 0; i < rcasNum; i++)
		{
			if(AmsCfgRcas(i).cModuleId == id)
			{
				Display("Rcas[%u]Senten id[%u] has been used!\r\n",*pCurrId, id);

				memset(&AmsCfgRcas(i), 0, sizeof(RCAS_INFO));

				//update rcas id
				AmsCfgRcas(i).cModuleId = id;
				AmsCfgRcas(i).flag = AMS_RCAS_INSTALL;
					
				idPos = i;
				break;
//				return FAILURE;
			}
		}

		if(-1 == idPos)
		{
			/* record rcas Id */
			for(i = 0; i < rcasNum; i++)
			{
				if(AMS_RCAS_UNINSTALL == AmsCfgRcas(i).flag)
				{
					AmsCfgRcas(i).cModuleId = id;
					AmsCfgRcas(i).flag = AMS_RCAS_INSTALL;
					break;
				}
			}
			
			if(i >= rcasNum)
			{
				Display("All Rcas has been Installed[%d]!\r\n", id);
				*pCurrId = 0;
				return FAILURE;
			}
		}
		
		*pCurrId = id;

		Display("Rcas[%u] Installed!\r\n", id);

		return SUCCESS;
	}
	else if(0 == strcmp(stringword,"vtasharedesktop"))//Remote Cooperative
	{
		AmsCfgData.rfbInitParaForVta.shareDesktopFlag = atoi((const char *)word[2].Body);
	}	
	else if(0 == strcmp(stringword,"vtaremotecoopsecutype"))//可多个 //Remote Cooperative
	{
		securitiesType = atoi((const char *)word[2].Body);
		if(AmsCfgData.rfbInitParaForVta.securities.num >= AMS_MAX_SECURITIES_NUM)
		{
			Display("Rcas Senten VtaRemoteCoopSecuNum[%d]Err[%d]!\r\n",
				AmsCfgData.rfbInitParaForVta.securities.num, securitiesType);			
			return FAILURE;
		}
		
		for(i = 0; i < AmsCfgData.rfbInitParaForVta.securities.num; i++)
		{
			if(securitiesType == AmsCfgData.rfbInitParaForVta.securities.type[i])
			{
				Display("Rcas Senten VtaRemoteCoopSecuType[%d]has been cfg[%d]!\r\n",
					securitiesType, i);
				return FAILURE;
			}
		}
		
		AmsCfgData.rfbInitParaForVta.securities.type[AmsCfgData.rfbInitParaForVta.securities.num] = securitiesType;
		
		AmsCfgData.rfbInitParaForVta.securities.num++;
			
		if(0 == AmsCfgData.rfbInitParaForVta.securities.flag)
		{
			AmsCfgData.rfbInitParaForVta.securities.flag = 1;
		}		

		Display("Rcas Senten VtaRemoteCoopSecu[%d]is Type[%d]!\r\n",
			AmsCfgData.rfbInitParaForVta.securities.num, securitiesType);
	}
	else if(0 == strcmp(stringword,"vtmsharedesktop"))//Remote Cooperative
	{
		AmsCfgData.rfbInitParaForVtm.shareDesktopFlag = atoi((const char *)word[2].Body);
	}	
	else if(0 == strcmp(stringword,"vtmremotecoopsecutype"))//可多个 //Remote Cooperative
	{	
		securitiesType = atoi((const char *)word[2].Body);
		if(AmsCfgData.rfbInitParaForVtm.securities.num >= AMS_MAX_SECURITIES_NUM)
		{
			Display("Rcas Senten VtmRemoteCoopSecuNum[%d]Err[%d]!\r\n",
				AmsCfgData.rfbInitParaForVtm.securities.num, securitiesType);				
			return FAILURE;
		}
		
		for(i = 0; i < AmsCfgData.rfbInitParaForVtm.securities.num; i++)
		{
			if(securitiesType == AmsCfgData.rfbInitParaForVtm.securities.type[i])
			{
				Display("Rcas Senten VtmRemoteCoopSecuType[%d]has been cfg[%d]!\r\n",
					securitiesType, i);				
				return FAILURE;
			}
		}
		
		AmsCfgData.rfbInitParaForVtm.securities.type[AmsCfgData.rfbInitParaForVtm.securities.num] = securitiesType;		

		AmsCfgData.rfbInitParaForVtm.securities.num++;

		if(0 == AmsCfgData.rfbInitParaForVtm.securities.flag)
		{
			AmsCfgData.rfbInitParaForVtm.securities.flag = 1;
		}

		Display("Rcas Senten VtmRemoteCoopSecu[%d]is Type[%d]!\r\n",
			AmsCfgData.rfbInitParaForVtm.securities.num, securitiesType);
		
	}	
/*	else if(0 == strcmp(stringword,"bitsperpixel"))//Screen Record
	{
		AmsCfgRcas.pixelFormatPara.bitsPerPixel = atoi((const char *)word[2].Body);
		if(0 == AmsCfgRcas.pixelFormatPara.flag)
		{
			AmsCfgRcas.pixelFormatPara.flag = 1;
		}
	}	
	else if(0 == strcmp(stringword,"depth"))//Screen Record
	{
		AmsCfgRcas.pixelFormatPara.depth = atoi((const char *)word[2].Body);
		if(0 == AmsCfgRcas.pixelFormatPara.flag)
		{
			AmsCfgRcas.pixelFormatPara.flag = 1;
		}
	}
	else if(0 == strcmp(stringword,"bigendianflag"))//Screen Record
	{
		AmsCfgRcas.pixelFormatPara.bigEndianFlag = atoi((const char *)word[2].Body);
		if(0 == AmsCfgRcas.pixelFormatPara.flag)
		{
			AmsCfgRcas.pixelFormatPara.flag = 1;
		}		
	}	
	else if(0 == strcmp(stringword,"truecolorflag"))//Screen Record
	{
		AmsCfgRcas.pixelFormatPara.trueColorFlag = atoi((const char *)word[2].Body);
		if(0 == AmsCfgRcas.pixelFormatPara.flag)
		{
			AmsCfgRcas.pixelFormatPara.flag = 1;
		}		
	}	
	else if(0 == strcmp(stringword,"redmax"))//Screen Record
	{
		AmsCfgRcas.pixelFormatPara.redMax = atoi((const char *)word[2].Body);
		if(0 == AmsCfgRcas.pixelFormatPara.flag)
		{
			AmsCfgRcas.pixelFormatPara.flag = 1;
		}		
	}	
	else if(0 == strcmp(stringword,"greenmax"))//Screen Record
	{
		AmsCfgRcas.pixelFormatPara.greenMax = atoi((const char *)word[2].Body);
		if(0 == AmsCfgRcas.pixelFormatPara.flag)
		{
			AmsCfgRcas.pixelFormatPara.flag = 1;
		}		
	}	
	else if(0 == strcmp(stringword,"bluemax"))//Screen Record
	{
		AmsCfgRcas.pixelFormatPara.blueMax = atoi((const char *)word[2].Body);
		if(0 == AmsCfgRcas.pixelFormatPara.flag)
		{
			AmsCfgRcas.pixelFormatPara.flag = 1;
		}		
	}
	else if(0 == strcmp(stringword,"redshift"))//Screen Record
	{
		AmsCfgRcas.pixelFormatPara.redShift = atoi((const char *)word[2].Body);
		if(0 == AmsCfgRcas.pixelFormatPara.flag)
		{
			AmsCfgRcas.pixelFormatPara.flag = 1;
		}		
	}	
	else if(0 == strcmp(stringword,"greenshift"))//Screen Record
	{
		AmsCfgRcas.pixelFormatPara.greenShift = atoi((const char *)word[2].Body);
		if(0 == AmsCfgRcas.pixelFormatPara.flag)
		{
			AmsCfgRcas.pixelFormatPara.flag = 1;
		}		
	}	
	else if(0 == strcmp(stringword,"blueshift"))//Screen Record
	{
		AmsCfgRcas.pixelFormatPara.blueShift = atoi((const char *)word[2].Body);
		if(0 == AmsCfgRcas.pixelFormatPara.flag)
		{
			AmsCfgRcas.pixelFormatPara.flag = 1;
		}		
	}	*/
/*	else if(0 == strcmp(stringword,"encodings"))//可多个 //Screen Record
	{	
		encoding = atoi((const char *)word[2].Body);
		if(AmsCfgRcas.encodingPara.num > AMS_MAX_ENCODING_NUM)
		{
			Display("Rcas Senten EncodingsNum[%d]Err[%d]!\r\n", 
				AmsCfgRcas.encodingPara.num, encoding);				
			return FAILURE;
		}
			
		for(i = 0; i < AmsCfgRcas.encodingPara.num; i++)
		{
			if(encoding == AmsCfgRcas.encodingPara.encodings[i])
			{
				Display("Rcas Senten Encodings[%d]has been cfg[%d]!\r\n",encoding, i);				
				return FAILURE;
			}
		}
		
		AmsCfgRcas.encodingPara.encodings[AmsCfgRcas.encodingPara.num] = encoding;		

		AmsCfgRcas.encodingPara.num++;

		if(0 == AmsCfgRcas.encodingPara.flag)
		{
			AmsCfgRcas.encodingPara.flag = 1;
		}

		Display("Rcas Senten Encodings[%d]is [%d]!\r\n",
			AmsCfgRcas.encodingPara.num, encoding);		
	}		*/
	else
	{
		return FAILURE;	
	}
	
	return SUCCESS;
}

int AmsSrvOrgSenten(WORD_t *word,int wordcount,unsigned int *pCurrId)
{
	char                 stringword[AMS_MAX_STRING_WORD_LEN];
	int                  i = 0;
	unsigned int         id = 0;
	int                  idPos = -1;	
	unsigned char        orgCodeLen;                //机构代码长度
	unsigned char        orgNameLen;                //机构名称长度

	memset(stringword, 0, sizeof(stringword));
	memcpy(stringword, word[0].Body, word[0].Len);

	if(0 == strcmp(stringword,"id"))
	{
		if(NULL == pCurrId)
		{
			return FAILURE;
		}
		
		id = atoi((const char *)word[2].Body);

		if(0 == id)
		{
			Display("Org[%u]Senten id[%u]Err!\r\n",*pCurrId, id);
			return FAILURE;
		}

		/* check org in cfg or not */
		for(i = 0; i < AMS_MAX_ORG_NUM; i++)
		{
			if(AmsCfgOrg(i).orgId == id)
			{
				Display("Org[%u]Senten id[%u] has been used!\r\n",*pCurrId, id);

				memset(&AmsCfgOrg(i), 0, sizeof(ORG_INFO));

				//update org id
				AmsCfgOrg(i).orgId = id;
				AmsCfgOrg(i).flag = AMS_ORG_INSTALL;
					
				idPos = i;
				break;
//				return FAILURE;
			}
		}

		if(-1 == idPos)
		{
			/* record org Id */
			for(i = 0; i < AMS_MAX_ORG_NUM; i++)
			{
				if(AMS_ORG_UNINSTALL == AmsCfgOrg(i).flag)
				{					
					AmsCfgOrg(i).orgId = id;
					AmsCfgOrg(i).flag = AMS_ORG_INSTALL;
					break;
				}
			}
			
			if(i >= AMS_MAX_ORG_NUM)
			{
				Display("All Org has been Installed[%d]!\r\n", id);
				*pCurrId = 0;
				return FAILURE;
			}
		}
		
		*pCurrId = id;

		Display("Org[%u] Installed!\r\n", id);

		return SUCCESS;
	}

	/* find org in cfg or not */
	for(i = 0; i < AMS_MAX_ORG_NUM; i++)
	{
		if(AMS_ORG_INSTALL == AmsCfgOrg(i).flag && AmsCfgOrg(i).orgId == *pCurrId)
		{
			break;
		}
	}
	if(i >= AMS_MAX_ORG_NUM)
	{
		Display("Org[%u]Senten not find Id!\r\n", *pCurrId);

		*pCurrId = 0;
		return FAILURE;
	}
	
	if(0 == strcmp(stringword,"code"))
	{
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Org[%u]Senten code[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, word[2].Len);
			return FAILURE;
		}

		orgCodeLen = strlen(stringword);
        if(orgCodeLen > 0 && orgCodeLen <= AMS_MAX_ORG_CODE_LEN)
        {
			strcpy(AmsCfgOrg(i).orgCode, stringword); 	
			AmsCfgOrg(i).orgCode[orgCodeLen] = '\0';
			AmsCfgOrg(i).orgCodeLen = orgCodeLen;
        }
		else
		{
			if(0 == orgCodeLen)
			{
				Display("Org[%u]Senten Name is Null!\r\n",*pCurrId);
			}
			else
			{
				Display("Org[%u]Senten Name[%s] len[%d]Err!\r\n",
					*pCurrId, stringword, orgCodeLen);
			}
			
			return FAILURE;
		}	
	}		
	else if(0 == strcmp(stringword,"type"))
	{
		AmsCfgOrg(i).orgType = atoi((const char *)word[2].Body);
	}
	else if(0 == strcmp(stringword,"name"))
	{
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Org[%u]Senten name[%s] len[%d]Err!\r\n",
				*pCurrId, stringword, word[2].Len);
			return FAILURE;
		}

		orgNameLen = strlen(stringword);
        if(orgNameLen > 0 && orgNameLen <= AMS_MAX_ORG_NAME_LEN)
        {

			strcpy(AmsCfgOrg(i).orgName, stringword); 	
			AmsCfgOrg(i).orgName[orgNameLen] = '\0';
			AmsCfgOrg(i).orgNameLen = orgNameLen;
        }
		else
		{
			if(0 == orgNameLen)
			{
				Display("Org[%u]Senten Name is Null!\r\n",*pCurrId);
			}
			else
			{
				Display("Org[%u]Senten Name[%s] len[%d]Err!\r\n",
					*pCurrId, stringword, orgNameLen);
			}
			
			return FAILURE;
		}	
	}		
	else if(0 == strcmp(stringword,"level"))
	{
		AmsCfgOrg(i).orgLevel = atoi((const char *)word[2].Body);
	}	
	else if(0 == strcmp(stringword,"parentid"))
	{	
		AmsCfgOrg(i).parentOrgId = atoi((const char *)word[2].Body);
		AmsCfgOrg(i).parentOrgIdFlag = 1;
	}	
	else if(0 == strcmp(stringword,"state"))
	{	
		AmsCfgOrg(i).orgState = atoi((const char *)word[2].Body);
		AmsCfgOrg(i).orgStateFlag = 1;
	}	
	else
	{
		return FAILURE;	
	}
	
	return SUCCESS;
}

int AmsSrvQueueSenten(WORD_t *word,int wordcount,unsigned int *pCurrId)
{
	char                 stringword[AMS_MAX_STRING_WORD_LEN];
	int                  i = 0;
	unsigned int         srvGrpId = 0;
	int                  idPos = -1;	
	unsigned short       maxQueLen = 0;
	unsigned short       avgSrvTime = 0;
	int                  result = 0;
	
	if(NULL == pCurrId)
	{
		return FAILURE;
	}

	memset(stringword, 0, sizeof(stringword));
	memcpy(stringword, word[0].Body, word[0].Len);
	
	if(0 == strcmp(stringword,"srvgrpid"))
	{
		srvGrpId = atoi((const char *)word[2].Body);

		/* check QueueSys in cfg or not */
		for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
		{
			if(AMS_QUEUE_CFG == AmsCfgQueueSys(i).flag && AmsCfgQueueSys(i).srvGrpId == srvGrpId)
			{
				Display("Queue[%u]Senten srvGrpId[%u] has been used!\r\n",
					*pCurrId, srvGrpId);

				memset(&AmsCfgQueueSys(i), 0, sizeof(QUEUE_SYS_INFO));
				
				AmsCfgQueueSys(i).srvGrpId = srvGrpId;
				AmsCfgQueueSys(i).flag = AMS_QUEUE_CFG;
					
				idPos = i;
				break;
			}
		}

		if(-1 == idPos)
		{
			/* record queue srvGrpId */
			for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
			{
				if(AMS_QUEUE_NOT_CFG == AmsCfgQueueSys(i).flag)
				{
					AmsCfgQueueSys(i).srvGrpId = srvGrpId;
					AmsCfgQueueSys(i).flag = AMS_QUEUE_CFG;
					break;
				}
			}
			
			if(i >= AMS_MAX_SERVICE_GROUP_NUM)
			{
				Display("All Queue has been Configed[%d]!\r\n", srvGrpId);
				*pCurrId = 0;
				return FAILURE;
			}
		}
		
		*pCurrId = srvGrpId;

		Display("Queue[%u] Configed!\r\n", srvGrpId);

		return SUCCESS;
	}

	if(*pCurrId >= AMS_MAX_SERVICE_GROUP_NUM)
	{
		Display("Queue Senten srvGrpId[%u] Err!\r\n",*pCurrId);
		return FAILURE;
	}

	/* find QueueSys in cfg or not */
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		if(AMS_QUEUE_CFG == AmsCfgQueueSys(i).flag && AmsCfgQueueSys(i).srvGrpId == *pCurrId)
		{
			break;
		}
	}
	if(i >= AMS_MAX_SERVICE_GROUP_NUM)
	{
		Display("Queue[%u]Senten not find srvGrpId!\r\n", *pCurrId);

		*pCurrId = 0x7FFFFFFF;
		return FAILURE;
	}

    if(0 == strcmp(stringword,"maxquelen"))
	{
		maxQueLen = atoi((const char *)word[2].Body);
		if(maxQueLen > AMS_MAX_VTM_NODES)
		{
			Display("Queue[%u]Senten MaxQueLen[%d]Err!\r\n", *pCurrId, maxQueLen);
			return FAILURE;
		}
		
		AmsCfgQueueSys(i).maxQueLen = maxQueLen;		
	}
	else if(0 == strcmp(stringword,"avgsrvtime"))
	{
		avgSrvTime = atoi((const char *)word[2].Body);
		if(avgSrvTime > AMS_MAX_AVG_SERVICE_TIME)
		{
			Display("Queue[%u]Senten AvgSrvTime[%d]Err!\r\n", *pCurrId, avgSrvTime);
			return FAILURE;
		}
		
		AmsCfgQueueSys(i).avgSrvTime = avgSrvTime;		
	}
	else
	{
		return FAILURE;	
	}
	
	return SUCCESS;
}

int amsGetTellerType(char *stringword, char *pTellerType)
{
	if(NULL == stringword || NULL == pTellerType)
	{
		return AMS_ERROR;
	}
	
	if(0 == strcmp(stringword,"vta"))
	{
		*pTellerType = AMS_TELLER_VTA;
	}
	else if(0 == strcmp(stringword,"manager"))
	{
		*pTellerType = AMS_TELLER_MANAGER;
	}
	else if(0 == strcmp(stringword,"inspector"))
	{
		*pTellerType = AMS_TELLER_INSPECTOR;
	}
	else
	{
		Display("Get TellerType[%s]not support!\r\n", stringword);
		*pTellerType = AMS_TELLER_MAX;
		return AMS_ERROR;
	}

	return AMS_OK;
	
}

/*
int amsGetSrvGrpId(char *stringword, char *pSrvGrpId)
{
	if(NULL == stringword || NULL == pSrvGrpId)
	{
		return AMS_ERROR;
	}
	
	if(0 == strcmp(stringword,"sysmaintenance"))
	{
		*pSrvGrpId = AMS_SERVICE_GROUP_SYS_MAINTENANCE;
	}
	else if(0 == strcmp(stringword,"vip"))
	{
		*pSrvGrpId = AMS_SERVICE_GROUP_VIP;
	}
	else if(0 == strcmp(stringword,"financialmanagement"))
	{
		*pSrvGrpId = AMS_SERVICE_GROUP_FINANCIAL_MANAGEMENT;
	}
	else if(0 == strcmp(stringword,"common"))
	{
		*pSrvGrpId = AMS_SERVICE_GROUP_COMMON;
	}	
	else
	{
		Display("Get SrvGrpId[%s]not support!\r\n", stringword);	
		*pSrvGrpId = AMS_SERVICE_GROUP_ID_MAX;
		return AMS_ERROR;
	}

	return AMS_OK;
	
}
*/

/*
int amsSetServiceStateByServiceName(char *serviceName)
{
	if(NULL == serviceName)
	{
		return AMS_ERROR;
	}
	
	if(0 == strcmp(serviceName,"systemmaintenance"))
	{
		if(AMS_SERVICE_INACTIVE == AmsSrvData(AMS_SERVICE_GROUP_SYS_MAINTENANCE).serviceState)
		{
			AmsSrvData(AMS_SERVICE_GROUP_SYS_MAINTENANCE).serviceState = AMS_SERVICE_ACTIVE;
			Display("ServiceSenten [%d][%s]is Active!\r\n", AMS_SERVICE_GROUP_SYS_MAINTENANCE, serviceName);
		}
	}
	else if(0 == strcmp(serviceName,"activatecards"))
	{
		if(AMS_SERVICE_INACTIVE == AmsSrvData(AMS_SERVICE_GROUP_COMMON).serviceState)
		{
			AmsSrvData(AMS_SERVICE_GROUP_COMMON).serviceState = AMS_SERVICE_ACTIVE;
			Display("ServiceSenten [%d][%s]is Active!\r\n", AMS_SERVICE_GROUP_COMMON, serviceName);
		}
	}
	else if(0 == strcmp(serviceName,"financialmanagement"))
	{
		if(AMS_SERVICE_INACTIVE == AmsSrvData(AMS_SERVICE_GROUP_FINANCIAL_MANAGEMENT).serviceState)
		{
			AmsSrvData(AMS_SERVICE_GROUP_FINANCIAL_MANAGEMENT).serviceState = AMS_SERVICE_ACTIVE;
			Display("ServiceSenten [%d][%s]is Active!\r\n", AMS_SERVICE_GROUP_FINANCIAL_MANAGEMENT, serviceName);
		}
		if(AMS_SERVICE_INACTIVE == AmsSrvData(AMS_SERVICE_GROUP_COMMON).serviceState)
		{
			AmsSrvData(AMS_SERVICE_GROUP_COMMON).serviceState = AMS_SERVICE_ACTIVE;
			Display("ServiceSenten [%d][%s]is Active!\r\n", AMS_SERVICE_GROUP_COMMON, serviceName);
		}		
	}
	else if(0 == strcmp(serviceName,"transferaccounts"))
	{
		if(AMS_SERVICE_INACTIVE == AmsSrvData(AMS_SERVICE_GROUP_COMMON).serviceState)
		{
			AmsSrvData(AMS_SERVICE_GROUP_COMMON).serviceState = AMS_SERVICE_ACTIVE;
			Display("ServiceSenten [%d][%s]is Active!\r\n", AMS_SERVICE_GROUP_COMMON, serviceName);
		}
	}
	else if(0 == strcmp(serviceName,"loans"))
	{
		if(AMS_SERVICE_INACTIVE == AmsSrvData(AMS_SERVICE_GROUP_VIP).serviceState)
		{
			AmsSrvData(AMS_SERVICE_GROUP_VIP).serviceState = AMS_SERVICE_ACTIVE;
			Display("ServiceSenten [%d][%s]is Active!\r\n", AMS_SERVICE_GROUP_VIP, serviceName);
		}
		if(AMS_SERVICE_INACTIVE == AmsSrvData(AMS_SERVICE_GROUP_FINANCIAL_MANAGEMENT).serviceState)
		{
			AmsSrvData(AMS_SERVICE_GROUP_FINANCIAL_MANAGEMENT).serviceState = AMS_SERVICE_ACTIVE;
			Display("ServiceSenten [%d][%s]is Active!\r\n", AMS_SERVICE_GROUP_FINANCIAL_MANAGEMENT, serviceName);
		}
		if(AMS_SERVICE_INACTIVE == AmsSrvData(AMS_SERVICE_GROUP_COMMON).serviceState)
		{
			AmsSrvData(AMS_SERVICE_GROUP_COMMON).serviceState = AMS_SERVICE_ACTIVE;
			Display("ServiceSenten [%d][%s]is Active!\r\n", AMS_SERVICE_GROUP_COMMON, serviceName);
		}		
	}
	else if(0 == strcmp(serviceName,"creditcard"))
	{
		if(AMS_SERVICE_INACTIVE == AmsSrvData(AMS_SERVICE_GROUP_COMMON).serviceState)
		{
			AmsSrvData(AMS_SERVICE_GROUP_COMMON).serviceState = AMS_SERVICE_ACTIVE;
			Display("ServiceSenten [%d][%s]is Active!\r\n", AMS_SERVICE_GROUP_COMMON, serviceName);
		}
	}	
	else if(0 == strcmp(serviceName,"monitor"))
	{
		//
	}	
	else
	{
		Display("ServiceSenten serviceName[%s]not support!\r\n", serviceName);
		return AMS_ERROR;
	}

	return AMS_OK;
	
}
*/

/*
int amsGetServiceTypeByServiceName(char *serviceName, unsigned int *pSrvType, unsigned int *pSrvRsvdType)
{
	if(NULL == serviceName || NULL == pSrvType || NULL == pSrvRsvdType)
	{
		return AMS_ERROR;
	}
	
	if(0 == strcmp(serviceName,"systemmaintenance"))
	{
		*pSrvType = AMS_SERVICE_SYSTEM_MAINTENANCE;
	}
	else if(0 == strcmp(serviceName,"activatecards"))
	{
		*pSrvType = AMS_SERVICE_ACTIVATE_CARDS;
	}
	else if(0 == strcmp(serviceName,"financialmanagement"))
	{
		*pSrvType = AMS_SERVICE_FINANCIAL_MANAGEMENT;
	}
	else if(0 == strcmp(serviceName,"transferaccounts"))
	{
		*pSrvType = AMS_SERVICE_TRANSFER_ACCOUNTS;
	}
	else if(0 == strcmp(serviceName,"loans"))
	{
		*pSrvType = AMS_SERVICE_LOADS;
	}
	else if(0 == strcmp(serviceName,"creditcard"))
	{
		*pSrvType = AMS_SERVICE_CREDIT_CARD;
	}	
	else if(0 == strcmp(serviceName,"monitor"))
	{
		*pSrvType = AMS_SERVICE_MONITOR;
	}	
	else
	{
		Display("Get ServiceType Service[%s]not support!\r\n", serviceName);
		*pSrvType = AMS_SERVICE_MAX;
		return AMS_ERROR;
	}

	return AMS_OK;
	
}
*/


int amsGetServiceNameByServiceId(unsigned int serviceId, char *pServiceName)
{
    int                  i = 0;	

	if(NULL == pServiceName)
	{
		return AMS_ERROR;
	}
	
	for(i = 0; i < AMS_MAX_SERVICE_NUM; i++)
	{
		if(AMS_SERVICE_INSTALL == AmsCfgService(i).flag &&
			AmsCfgService(i).service == serviceId)
		{
			break;
		}
	}
	
	if(i >= AMS_MAX_SERVICE_NUM)
	{
		Display("AuthoritySenten serviceId[%d]not Install!\r\n", serviceId);
		return AMS_ERROR;
	}

	if(AmsCfgService(i).serviceNameLen > AMS_MAX_SERVICE_NAME_LEN)
	{
		Display("AuthoritySenten serviceId[%d]serviceNameLen[%d]Err!\r\n", 
			serviceId, AmsCfgService(i).serviceNameLen);
		return AMS_ERROR;		
	}
	
    memcpy(pServiceName, AmsCfgService(i).serviceName, AmsCfgService(i).serviceNameLen);
	pServiceName[AmsCfgService(i).serviceNameLen] = '\0';
	
	return AMS_OK;
	
}

int amsCheckServiceGroupServiceType(unsigned int srvGrpId, unsigned int serviceType, unsigned int srvTypeRsvd)
{
	unsigned int    i = 0;
	int             iret = AMS_ERROR;
	
	if(srvGrpId > AMS_SERVICE_GROUP_ID_MAX)
	{
		if(AmsErrorTrace)
		{
			dbgprint("Ams CheckServiceGroupServiceType srvGrpId[%u]Err", srvGrpId);
		}
		return iret;		
	}

	/* find service group in cfg or not */
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		if(AMS_SERVICE_GROUP_INSTALL == AmsCfgSrvGroup(i).flag && 
			AmsCfgSrvGroup(i).srvGrpId == srvGrpId)
		{
			break;
		}
	}
	if(i >= AMS_MAX_SERVICE_GROUP_NUM)
	{
		Display("Ams CheckServiceGroupServiceType not find Service Group[%d] !\r\n", srvGrpId);

		return iret;
	}

	if(0 == (AmsCfgSrvGroup(i).serviceType & serviceType))
	{
		Display("Ams CheckServiceGroupServiceType not find Service[0x%x] in Group[%d][0x%x]!\r\n", 
			serviceType, srvGrpId, AmsCfgSrvGroup(i).serviceType);		
		return iret;
	}
	
	return AMS_OK;
}

int amsGetTerminalType(char *stringword, char *pTerminalType)
{
	if(NULL == stringword || NULL == pTerminalType)
	{
		return AMS_ERROR;
	}
	
	if(0 == strcmp(stringword,"vta"))
	{
		*pTerminalType = AMS_TERMINAL_VTA;
	}
	else if(0 == strcmp(stringword,"vtm"))
	{
		*pTerminalType = AMS_TERMINAL_VTM;
	}	
	else if(0 == strcmp(stringword,"mobilephone"))
	{
		*pTerminalType = AMS_TERMINAL_MOBILEPHONE;
	}
	else if(0 == strcmp(stringword,"pc"))
	{
		*pTerminalType = AMS_TERMINAL_PC;
	}
	else
	{
		Display("Get TerminalType[%s]not support!\r\n", stringword);
		*pTerminalType = AMS_TERMINAL_MAX;		
		return AMS_ERROR;
	}

	return AMS_OK;
	
}
////////////////////////////////////////////////////////////////////////////////
//OMSAGENT接口函数 zhuyn added 20160620
int AmsGetSysConfig(OA_AMS_SYS_CONFIG_t *pConf, int *pResultCode, char *pResultDescription, int descriptionBufLen)
{
    int len;

    if (!pConf || !pResultCode || !pResultDescription)
    {
        if (pConf)        
        {
            memset(pConf->flag, 0, AMSSYSCFG_ITEM_MAX);
        }
        if (pResultCode)
        {
            *pResultCode = OA_FAIL_UNKNOWN_ERROR;
        }
        return OA_RET_FAIL;
    }
    pConf->Mid = SystemData.cMid;
    OmsagentCopystr(pConf->SysName, OA_MAX_SYS_NAME_LEN, SystemData.cSysName, strlen(SystemData.cSysName));
    pConf->SysNo = SystemData.cSysNo;
    OmsagentCopystr(pConf->AlmFlag, OA_MAX_ALARM_ITEM, SystemData.AlmTable.cOpenFlag, SystemData.AlmTable.iOpenFlagLen);
    pConf->CmsMid = SystemData.AmsPriData.amsCfgData.cmsInfo.cModuleId;
    pConf->RcasMid = SystemData.AmsPriData.amsCfgData.rcasInfo[0].cModuleId;//rcas扩容
    memset(pConf->flag, 1, AMSSYSCFG_ITEM_MAX);

    *pResultCode = OA_FAIL_NONE;
    OmsagentCopystr(pResultDescription, descriptionBufLen-1, OKStr, strlen(OKStr));
    return OA_RET_SUC;
}

unsigned int AmsCheckOATellerParaValue(OA_AMS_TELLER_CONFIG_t *pteller)
{
    if ( pteller->flag[AMSTELLERCFG_ITEM_TELLERID] && pteller->TellerId < 1)
    {
        return AMSTELLERCFG_ITEM_TELLERID;
    }
    else if (pteller->flag[AMSTELLERCFG_ITEM_TELLERNO] && strlen(pteller->TellerNo) < 1)
    {
        return AMSTELLERCFG_ITEM_TELLERNO;
    }
    else if ( pteller->flag[AMSTELLERCFG_ITEM_TELLERTYPE] && (pteller->TellerType >= AMS_TELLER_MAX || pteller->TellerType < 0))
    {
        return AMSTELLERCFG_ITEM_TELLERTYPE;
    }
    else if ( pteller->flag[AMSTELLERCFG_ITEM_TELLERSRVGRPID] && (pteller->TellerSrvGrpId >= AMS_MAX_SERVICE_GROUP_NUM || pteller->TellerType < 0))
    {
        return AMSTELLERCFG_ITEM_TELLERSRVGRPID;
    }
    else if (pteller->flag[AMSTELLERCFG_ITEM_TELLERNAME] && strlen(pteller->TellerName) < 1)
    {
        return AMSTELLERCFG_ITEM_TELLERNAME;
    }
    return AMSTELLERCFG_ITEM_MAX;
}
    
int AmsSetTellerInfoFromOA(unsigned int i, OA_AMS_TELLER_CONFIG_t *pteller)
{
    if (pteller->flag[AMSTELLERCFG_ITEM_TELLERNO])
    {
        AmsCfgTeller(i).tellerNoLen = OmsagentCopystr(AmsCfgTeller(i).tellerNo, AMS_MAX_TELLER_NO_LEN,
            pteller->TellerNo, strlen(pteller->TellerNo));
    }
    if (pteller->flag[AMSTELLERCFG_ITEM_TELLERTYPE])
    {
        AmsCfgTeller(i).tellerType = pteller->TellerType;
    }
    if (pteller->flag[AMSTELLERCFG_ITEM_TELLERSRVGRPID])
    {
        AmsCfgTeller(i).srvGrpId = pteller->TellerSrvGrpId;
        if (AmsCfgTeller(i).srvGrpId < AMS_MAX_SERVICE_GROUP_NUM)
        {
            AmsCfgTeller(i).tellerSrvAuth = AmsCfgSrvGroup(AmsCfgTeller(i).srvGrpId).serviceType;
            //AmsCfgTeller(i).tellerSrvAuthRsvd = AmsCfgTeller(i).tellerSrvAuth;
            AmsCfgTeller(i).tellerSrvAuthRsvd = 0xffffffff; //暂时为全开放 20170123
        }
    }
    if (pteller->flag[AMSTELLERCFG_ITEM_TELLERNAME])
    {
        AmsCfgTeller(i).tellerNameLen = OmsagentCopystr(AmsCfgTeller(i).tellerName, AMS_MAX_NAME_LEN,
            pteller->TellerName, strlen(pteller->TellerName));
    }
    if (pteller->flag[AMSTELLERCFG_ITEM_TELLERPWD])
    {
        AmsCfgTeller(i).tellerPwdLen = OmsagentCopystr(AmsCfgTeller(i).tellerPwd, AMS_MAX_PWD_LEN,
            pteller->TellerPwd, strlen(pteller->TellerPwd));
    }
    /*if (pteller->flag[AMSTELLERCFG_ITEM_TELLERDEPT])
    {
        AmsCfgTeller(i).tellerDepartmentNameLen = OmsagentCopystr(AmsCfgTeller(i).tellerDepartmentName, AMS_MAX_DEPARTMENT_NAME_LEN,
            pteller->TellerDept, strlen(pteller->TellerDept));
    }*/
    if (pteller->flag[AMSTELLERCFG_ITEM_TELLERAREACODE])
    {
        AmsCfgTeller(i).areaCodeLen = OmsagentCopystr(AmsCfgTeller(i).areaCode, AMS_MAX_AREA_CODE_NUM,
            pteller->TellerAreaCode, strlen(pteller->TellerAreaCode));
    }
    if (pteller->flag[AMSTELLERCFG_ITEM_TELLERPHONENAME])
    {
        AmsCfgTeller(i).phoneNumLen = OmsagentCopystr(AmsCfgTeller(i).phoneNum, AMS_MAX_PHONE_NUM,
            pteller->TellerPhoneNum, strlen(pteller->TellerPhoneNum));
    }
    if (pteller->flag[AMSTELLERCFG_ITEM_TELLERIP])
    {
        DWORD uiIp;
        AmsCfgTeller(i).transIpLen = OmsagentCopystr(AmsCfgTeller(i).transIp, AMS_MAX_TRANS_IP_LEN,
            pteller->TellerIp, strlen(pteller->TellerIp));
        uiIp = inet_addr(AmsCfgTeller(i).transIp);
		AmsCfgTeller(i).vtaIp = htonl(uiIp);
    }
/*  if (pteller->flag[AMSTELLERCFG_ITEM_TELLERREMCOOPPWD])
    {
        AmsCfgTeller(i).vncAuth.passwordLen = OmsagentCopystr(AmsCfgTeller(i).vncAuth.password, AMS_MAX_PWD_LEN,
            pteller->TellerRemCoopPwd, strlen(pteller->TellerRemCoopPwd));
        AmsCfgTeller(i).vncAuth.flag = 1;
    }*/
    if (pteller->flag[AMSTELLERCFG_ITEM_TELLERORGIND])
    {
        AmsCfgTeller(i).orgId = pteller->TellerOrgInd;     
    }
    return 0;
}
int AmsAddTeller(OA_AMS_TELLER_CONFIG_t *pteller, int *pResultCode, char *pResultDescription, int descriptionBufLen)
{
	char                 *parastr = NULL;
	int                  len;
	static unsigned char AddTellerNecessaryFlag[AMSTELLERCFG_ITEM_MAX]={1,1,1,1,1,0,0,0,0,0};
	LP_AMS_DATA_t		 *lpAmsData = NULL;         //进程数据区指针		
	VTA_NODE             *pVtaNode = NULL;
	VTM_NODE             *pVtmNode = NULL;	
	unsigned int         vtaNum = 0;
	unsigned int         vtmNum = 0;
	MESSAGE_t            msg;	
	int                  pid = 0;
	unsigned char        buf[AMS_MAX_SINGLE_TERM_ID_NO_LEN] = {0};
	unsigned short       bufLen = 0;	
	int                  ret = AMS_ERROR;
	VTA_ID_NODE	         *pVtaIdNode;
	unsigned int         tellerPos = 0;	
	unsigned int         i;
	unsigned int         j;
    char                 *reasonstr = NULL;
	
    //检查函数入参
    if (!pteller || !pResultCode || !pResultDescription)
    {
        if (pResultCode)
        {
            *pResultCode = OA_FAIL_UNKNOWN_ERROR;
        }
        return OA_RET_FAIL;
    }
    
    *pResultCode = OA_FAIL_NONE;
    
    //检查必带参数
    for (i = 0; i < AMSTELLERCFG_ITEM_MAX; i++)
    {
        if (AddTellerNecessaryFlag[i] && !pteller->flag[i])
        {
            *pResultCode = OA_FAIL_PARA_MISS;
            OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsTellerParaName[i],NULL);
            return OA_RET_FAIL;        
        }
    }

    //检查参数范围
    if ((i = AmsCheckOATellerParaValue(pteller)) <  AMSTELLERCFG_ITEM_MAX)
    {
        *pResultCode = OA_FAIL_PARAVAL_INV;
        OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsTellerParaName[i],NULL);
        return OA_RET_FAIL;        
    }

    //检查teller是否已存在
    vtaNum = Min(SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NUM);
	for(i = 0; i < Min(SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NUM); i++)
	{
		if(AmsCfgTeller(i).tellerId == pteller->TellerId && AmsCfgTeller(i).flag == AMS_TELLER_INSTALL)
		{
	            *pResultCode = AMS_OAREASON_TELLER_ADDED_BEFORE;
                    reasonstr = OmsagentGetReasonStr(*pResultCode);
                    OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));                    
            return OA_RET_FAIL;     
		}
	}

    //查找空闲项并记录
	for(i = 0; i < Min(SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NUM); i++)
	{
		if(AMS_TELLER_UNINSTALL == AmsCfgTeller(i).flag)
		{
		    memset(&AmsCfgTeller(i), 0, sizeof(TELLER_INFO));
    		AmsCfgTeller(i).tellerId = pteller->TellerId;
    		AmsCfgTeller(i).flag = AMS_TELLER_INSTALL;
			
			memset(&AmsTellerStat(i), 0, sizeof(AMS_TELLER_STAT));
			AmsTellerStat(i).tellerId = pteller->TellerId;
			
            AmsSetTellerInfoFromOA(i, pteller);

			tellerPos = i;
			
			pVtaIdNode = AmsGetVtaIdNode();
			if(NULL != pVtaIdNode)
			{
				pVtaIdNode->tellerId = AmsCfgTeller(i).tellerId;
				pVtaIdNode->tellerPos = i;
				
				AmsInsertVtaIdHash(pVtaIdNode);
			}
	
            OmsagentCopystr(pResultDescription, descriptionBufLen-1, OKStr, strlen(OKStr));
//          return OA_RET_SUC;
			ret = AMS_OK;
			break;
		}
	}

	//发给在线Teller
//	vtaNum = Min(SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NUM);
	for(i = 0; i < vtaNum; i++)
	{
		if(AmsCfgTeller(i).flag != AMS_TELLER_INSTALL)
		{
			continue;
		}
		
	    //检查teller是否在线
    	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
    	{
    		if (NULL == (pVtaNode = AmsSearchVtaNode(j, AmsCfgTeller(i).tellerId)))
    		{
				continue;
    		}

			pid = pVtaNode->amsPid & 0xffff;
			if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
			{
				//...
				continue;
			}

			lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
			
			if(    lpAmsData->amsPid != pVtaNode->amsPid 
				|| lpAmsData->myPid.iProcessId != pid 
				|| lpAmsData->tellerId != pVtaNode->vtaInfo.tellerId)
			{
				//...
				continue;
			}

			bufLen = AmsPackSingleTermIdNo(AMS_OPERATE_ORIGIN_VTA, AmsCfgTeller(tellerPos).tellerId, AmsCfgTeller(tellerPos).tellerNoLen, AmsCfgTeller(tellerPos).tellerNo, buf, AMS_MAX_SINGLE_TERM_ID_NO_LEN);
				
		    //向终端发送添加请求
		    if (AMS_SUCCESS!= AmsSendVtaParaCfgInd(pVtaNode, &msg, NULL, NULL, buf, bufLen, CONFIGIND_ADD_ID_NO))
		    {
				//...
		        continue;     
		    }

    	}		

	}

	//发给在线Vtm
    vtmNum = Min(SystemData.AmsPriData.amsCfgData.maxVtmNum, AMS_MAX_VTM_NUM);	 	
	for(i = 0; i < vtmNum; i++)
	{
		if(AmsCfgVtm(i).flag != AMS_VTM_INSTALL)
		{
			continue;
		}
		
	    //检查vtm是否在线
    	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
    	{
    		if (NULL == (pVtmNode = AmsSearchVtmNode(j, AmsCfgVtm(i).vtmId)))
    		{
				continue;
    		}

			bufLen = AmsPackSingleTermIdNo(AMS_OPERATE_ORIGIN_VTA, AmsCfgTeller(tellerPos).tellerId, AmsCfgTeller(tellerPos).tellerNoLen, AmsCfgTeller(tellerPos).tellerNo, buf, AMS_MAX_SINGLE_TERM_ID_NO_LEN);
			
		    //向终端发送添加请求
		    if (AMS_SUCCESS!= AmsSendVtmParaCfgInd(pVtmNode, &msg, NULL, NULL, buf, bufLen, CONFIGIND_ADD_ID_NO))
		    {			
		        continue;       
		    }										
    	}			
	}

	if(AMS_OK == ret)
	{
		return OA_RET_SUC;
	}
	
    *pResultCode = AMS_OAREASON_TELLER_FULL;
    reasonstr = OmsagentGetReasonStr(*pResultCode);
    OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));       
    return OA_RET_FAIL;
}


int AmsModifyTeller(OA_AMS_TELLER_CONFIG_t *pteller, int *pResultCode, char *pResultDescription, int descriptionBufLen)
{
	char                 *parastr = NULL;
	int                  len;
	static unsigned char ModifyTellerNecessaryFlag[AMSTELLERCFG_ITEM_MAX]={1,0,0,0,0,0,0,0,0,0};
	LP_AMS_DATA_t		 *lpAmsData = NULL;         //进程数据区指针		
	VTA_NODE             *pVtaNode = NULL;
	VTM_NODE             *pVtmNode = NULL;	
	unsigned int         vtaNum = 0;
	unsigned int         vtmNum = 0;
	MESSAGE_t            msg;	
	int                  pid = 0;
	unsigned char        buf[AMS_MAX_SINGLE_TERM_ID_NO_LEN] = {0};
	unsigned short       bufLen = 0;	
	unsigned int         tellerPos = 0;	
	unsigned int         i;
	unsigned int         j;
    char                 *reasonstr = NULL;
	
    //检查函数入参
    if (!pteller || !pResultCode || !pResultDescription)
    {
        if (pResultCode)
        {
            *pResultCode = OA_FAIL_UNKNOWN_ERROR;
        }
        return OA_RET_FAIL;
    }
    
    *pResultCode = OA_FAIL_NONE;
    
    //检查必带参数
    for (i = 0; i < AMSTELLERCFG_ITEM_MAX; i++)
    {
        if (ModifyTellerNecessaryFlag[i] && !pteller->flag[i])
        {
            *pResultCode = OA_FAIL_PARA_MISS;
            OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsTellerParaName[i],NULL);
            return OA_RET_FAIL;        
        }
    }

    //检查参数范围
    if ((i = AmsCheckOATellerParaValue(pteller)) <  AMSTELLERCFG_ITEM_MAX)
    {
        *pResultCode = OA_FAIL_PARAVAL_INV;
        OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsTellerParaName[i],NULL);
        return OA_RET_FAIL;        
    }

    //检查teller是否已存在
    vtaNum = Min(SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NUM);
	for(i = 0; i < vtaNum; i++)
	{
		if(AmsCfgTeller(i).tellerId == pteller->TellerId && AmsCfgTeller(i).flag == AMS_TELLER_INSTALL)
		{
		    break;
		}
	}
    if (i >= vtaNum)
    {
        *pResultCode = AMS_OAREASON_TELLER_NOT_EXIST;
        reasonstr = OmsagentGetReasonStr(*pResultCode);
        OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));       
    
        return OA_RET_FAIL;     
    }

    //更新记录
    AmsSetTellerInfoFromOA(i, pteller);
	tellerPos = i;

 	//发给在线Teller
//	vtaNum = Min(SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NUM);
	for(i = 0; i < vtaNum; i++)
	{
		if(AmsCfgTeller(i).flag != AMS_TELLER_INSTALL)
		{
			continue;
		}
		
	    //检查teller是否在线
    	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
    	{
    		if (NULL == (pVtaNode = AmsSearchVtaNode(j, AmsCfgTeller(i).tellerId)))
    		{
				continue;
    		}

			pid = pVtaNode->amsPid & 0xffff;
			if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
			{
				//...
				continue;
			}

			lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
			
			if(    lpAmsData->amsPid != pVtaNode->amsPid 
				|| lpAmsData->myPid.iProcessId != pid 
				|| lpAmsData->tellerId != pVtaNode->vtaInfo.tellerId)
			{
				//...
				continue;
			}

			bufLen = AmsPackSingleTermIdNo(AMS_OPERATE_ORIGIN_VTA, AmsCfgTeller(tellerPos).tellerId, AmsCfgTeller(tellerPos).tellerNoLen, AmsCfgTeller(tellerPos).tellerNo, buf, AMS_MAX_SINGLE_TERM_ID_NO_LEN);
				
		    //向终端发送修改请求
		    if (AMS_SUCCESS!= AmsSendVtaParaCfgInd(pVtaNode, &msg, NULL, NULL, buf, bufLen, CONFIGIND_MODIFY_ID_NO))
		    {
				//...
		        continue;     
		    }

    	}		

	}

 	//发给在线Vtm
    vtmNum = Min(SystemData.AmsPriData.amsCfgData.maxVtmNum, AMS_MAX_VTM_NUM);	 	
	for(i = 0; i < vtmNum; i++)
	{
		if(AmsCfgVtm(i).flag != AMS_VTM_INSTALL)
		{
			continue;
		}
		
	    //检查vtm是否在线
    	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
    	{
    		if (NULL == (pVtmNode = AmsSearchVtmNode(j, AmsCfgVtm(i).vtmId)))
    		{
				continue;
    		}

			bufLen = AmsPackSingleTermIdNo(AMS_OPERATE_ORIGIN_VTA, AmsCfgTeller(tellerPos).tellerId, AmsCfgTeller(tellerPos).tellerNoLen, AmsCfgTeller(tellerPos).tellerNo, buf, AMS_MAX_SINGLE_TERM_ID_NO_LEN);
			
		    //向终端发送修改请求
		    if (AMS_SUCCESS!= AmsSendVtmParaCfgInd(pVtmNode, &msg, NULL, NULL, buf, bufLen, CONFIGIND_MODIFY_ID_NO))
		    {			
				
		        continue;       
		    }										
    	}			
	}
	
    //检查teller是否在线
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		if (AmsSearchVtaNode(i, pteller->TellerId) != NULL)
		{
		    *pResultCode = AMS_OAREASON_TELLER_NEED_RELOG;
			reasonstr = OmsagentGetReasonStr(*pResultCode);
			OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));           
            return OA_RET_SUC;    
		}
	}

    OmsagentCopystr(pResultDescription, descriptionBufLen-1, OKStr, strlen(OKStr));
    return OA_RET_SUC;
}

int AmsDeleteTeller(OA_AMS_TELLER_CONFIG_t *pteller, int *pResultCode, char *pResultDescription, int descriptionBufLen)
{
	char                 *parastr = NULL;
	int                  len;
	static unsigned char DeleteTellerNecessaryFlag[AMSTELLERCFG_ITEM_MAX]={1,0,0,0,0,0,0,0,0,0};
	LP_AMS_DATA_t		 *lpAmsData = NULL;         //进程数据区指针		
	VTA_NODE             *pVtaNode = NULL;
	VTM_NODE             *pVtmNode = NULL;	
	unsigned int         vtaNum = 0;
	unsigned int         vtmNum = 0;
	MESSAGE_t            msg;	
	int                  pid = 0;
	unsigned char        buf[AMS_MAX_SINGLE_TERM_ID_NO_LEN] = {0};
	unsigned short       bufLen = 0;	
	int                  ret = AMS_ERROR;
	VTA_ID_NODE	         *pVtaIdNode;	
	unsigned int         tellerPos = 0;
	unsigned int         i;
	unsigned int         j;
    char                 *reasonstr = NULL;
	
    //检查函数入参
    if (!pteller || !pResultCode || !pResultDescription)
    {
        if (pResultCode)
        {
            *pResultCode = OA_FAIL_UNKNOWN_ERROR;
        }
        return OA_RET_FAIL;
    }
    
    *pResultCode = OA_FAIL_NONE;
    
    //检查必带参数
    for (i = 0; i < AMSTELLERCFG_ITEM_MAX; i++)
    {
        if (DeleteTellerNecessaryFlag[i] && !pteller->flag[i])
        {
            *pResultCode = OA_FAIL_PARA_MISS;
            OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsTellerParaName[i],NULL);
            return OA_RET_FAIL;        
        }
    }

    //检查参数范围
    //检查teller是否已存在
    vtaNum = Min(SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NUM);
	for(i = 0; i < vtaNum; i++)
	{
		if(AmsCfgTeller(i).tellerId == pteller->TellerId && AmsCfgTeller(i).flag == AMS_TELLER_INSTALL)
		{
		    //检查teller是否在线
        	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
        	{
        		if (AmsSearchVtaNode(j, pteller->TellerId) != NULL)
        		{
             		     *pResultCode = AMS_OAREASON_TELLER_ONLINE;
                          reasonstr = OmsagentGetReasonStr(*pResultCode);
                          OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));                       
                    return OA_RET_FAIL;             
        		}
        	}

			tellerPos = i;
//          memset(&AmsCfgTeller(i), 0, sizeof(TELLER_INFO));
//		    OmsagentCopystr(pResultDescription, descriptionBufLen-1, OKStr, strlen(OKStr));
//          return OA_RET_SUC;
			ret = AMS_OK;
			break;			
		}
	}

 	//发给在线Teller
//	vtaNum = Min(SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NUM);
	for(i = 0; i < vtaNum; i++)
	{
		if(AmsCfgTeller(i).flag != AMS_TELLER_INSTALL)
		{
			continue;
		}
		
	    //检查teller是否在线
    	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
    	{
    		if (NULL == (pVtaNode = AmsSearchVtaNode(j, AmsCfgTeller(i).tellerId)))
    		{
				continue;
    		}

			pid = pVtaNode->amsPid & 0xffff;
			if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
			{
				//...
				continue;
			}

			lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
			
			if(    lpAmsData->amsPid != pVtaNode->amsPid 
				|| lpAmsData->myPid.iProcessId != pid 
				|| lpAmsData->tellerId != pVtaNode->vtaInfo.tellerId)
			{
				//...
				continue;
			}

			bufLen = AmsPackSingleTermIdNo(AMS_OPERATE_ORIGIN_VTA, AmsCfgTeller(tellerPos).tellerId, AmsCfgTeller(tellerPos).tellerNoLen, AmsCfgTeller(tellerPos).tellerNo, buf, AMS_MAX_SINGLE_TERM_ID_NO_LEN);
				
		    //向终端发送删除请求
		    if (AMS_SUCCESS!= AmsSendVtaParaCfgInd(pVtaNode, &msg, NULL, NULL, buf, bufLen, CONFIGIND_DELETE_ID_NO))
		    {
				//...
		        continue;     
		    }

    	}		

	}

 	//发给在线Vtm
    vtmNum = Min(SystemData.AmsPriData.amsCfgData.maxVtmNum, AMS_MAX_VTM_NUM);	 	
	for(i = 0; i < vtmNum; i++)
	{
		if(AmsCfgVtm(i).flag != AMS_VTM_INSTALL)
		{
			continue;
		}
		
	    //检查vtm是否在线
    	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
    	{
    		if (NULL == (pVtmNode = AmsSearchVtmNode(j, AmsCfgVtm(i).vtmId)))
    		{
				continue;
    		}

			bufLen = AmsPackSingleTermIdNo(AMS_OPERATE_ORIGIN_VTA, AmsCfgTeller(tellerPos).tellerId, AmsCfgTeller(tellerPos).tellerNoLen, AmsCfgTeller(tellerPos).tellerNo, buf, AMS_MAX_SINGLE_TERM_ID_NO_LEN);
			
		    //向终端发送删除请求
		    if (AMS_SUCCESS!= AmsSendVtmParaCfgInd(pVtmNode, &msg, NULL, NULL, buf, bufLen, CONFIGIND_DELETE_ID_NO))
		    {			
				
		        continue;       
		    }										
    	}			
	}

	if(AMS_OK == ret)
	{		
		pVtaIdNode = AmsSearchVtaIdHash(AmsCfgTeller(tellerPos).tellerId);
		if(NULL != pVtaIdNode)
		{
			AmsDeleteVtaIdHash(pVtaIdNode);

			AmsFreeVtaIdNode(pVtaIdNode);
		}

		memset(&AmsTellerStat(tellerPos), 0, sizeof(AMS_TELLER_STAT));

		AmsResetTellerRegInfo(AmsCfgTeller(tellerPos).tellerId);
		
		memset(&AmsCfgTeller(tellerPos), 0, sizeof(TELLER_INFO));

		OmsagentCopystr(pResultDescription, descriptionBufLen-1, OKStr, strlen(OKStr));
		return OA_RET_SUC;
	}
	
	*pResultCode = AMS_OAREASON_TELLER_NOT_EXIST;
	reasonstr = OmsagentGetReasonStr(*pResultCode);
	OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));                                
    return OA_RET_FAIL;     
}

unsigned int AmsCheckOAVtmParaValue(OA_AMS_VTM_CONFIG_t *pvtm)
{
    if ( pvtm->flag[AMSVTMCFG_ITEM_VTMID] && pvtm->VtmId < 1)
    {
        return AMSVTMCFG_ITEM_VTMID;
    }
    else if (pvtm->flag[AMSVTMCFG_ITEM_VTMNO] && strlen(pvtm->VtmNo) < 1)
    {
        return AMSVTMCFG_ITEM_VTMNO;
    }
    else if ( pvtm->flag[AMSVTMCFG_ITEM_VTMNAME] && strlen(pvtm->VtmName) < 1)
    {
        return AMSVTMCFG_ITEM_VTMNAME;
    }
    else if ( pvtm->flag[AMSVTMCFG_ITEM_VTMREMCOOPPORT] && (pvtm->VtmRemCoopPort > 0xffff || pvtm->VtmRemCoopPort < 0))
    {
        return AMSVTMCFG_ITEM_VTMREMCOOPPORT;
    }
    
    return AMSVTMCFG_ITEM_MAX;
}

int AmsSetVtmInfoFromOA(unsigned int i, OA_AMS_VTM_CONFIG_t *pvtm)
{
    if (pvtm->flag[AMSVTMCFG_ITEM_VTMNO])
    {
        AmsCfgVtm(i).vtmNoLen = OmsagentCopystr(AmsCfgVtm(i).vtmNo, AMS_MAX_TELLER_NO_LEN,
            pvtm->VtmNo, strlen(pvtm->VtmNo));
    }
    if (pvtm->flag[AMSVTMCFG_ITEM_VTMNAME])
    {
        AmsCfgVtm(i).vtmNameLen = OmsagentCopystr(AmsCfgVtm(i).vtmName, AMS_MAX_NAME_LEN,
            pvtm->VtmName, strlen(pvtm->VtmName));
    }    
    if (pvtm->flag[AMSVTMCFG_ITEM_VTMPWD])
    {
        AmsCfgVtm(i).vtmPwdLen = OmsagentCopystr(AmsCfgVtm(i).vtmPwd, AMS_MAX_PWD_LEN,
            pvtm->VtmPwd, strlen(pvtm->VtmPwd));
    }
    if (pvtm->flag[AMSVTMCFG_ITEM_VTMIP])
    {
        DWORD uiIp;
        AmsCfgVtm(i).transIpLen = OmsagentCopystr(AmsCfgVtm(i).transIp, AMS_MAX_TRANS_IP_LEN,
            pvtm->VtmIp, strlen(pvtm->VtmIp));
        uiIp = inet_addr(AmsCfgVtm(i).transIp);
		AmsCfgVtm(i).vtmIp = htonl(uiIp);
    }
/*  if (pvtm->flag[AMSVTMCFG_ITEM_VTMREMCOOPPWD])
    {
        AmsCfgVtm(i).vncAuth.passwordLen = OmsagentCopystr(AmsCfgVtm(i).vncAuth.password, AMS_MAX_PWD_LEN,
            pvtm->VtmRemCoopPwd, strlen(pvtm->VtmRemCoopPwd));
        AmsCfgVtm(i).vncAuth.flag = 1;
    }*/
    if (pvtm->flag[AMSVTMCFG_ITEM_VTMREMCOOPPORT])
    {
        AmsCfgVtm(i).vtmRemoteCoopPort = pvtm->VtmRemCoopPort;
    }
    if (pvtm->flag[AMSVTMCFG_ITEM_VTMORGID])
    {
        AmsCfgVtm(i).orgId =  pvtm->VtmOrgid;
    }
    return 0;
}


int AmsAddVtm(OA_AMS_VTM_CONFIG_t *pvtm, int *pResultCode, char *pResultDescription, int descriptionBufLen)
{
	int                  len;
	static unsigned char AddVtmNecessaryFlag[AMSVTMCFG_ITEM_MAX]={1,1,1,0,0,0,0};
	LP_AMS_DATA_t		 *lpAmsData = NULL;         //进程数据区指针		
	VTA_NODE             *pVtaNode = NULL;
	VTM_NODE             *pVtmNode = NULL;	
	unsigned int         vtaNum = 0;
	unsigned int         vtmNum = 0;
	MESSAGE_t            msg;	
	int                  pid = 0;
	unsigned char        buf[AMS_MAX_SINGLE_TERM_ID_NO_LEN] = {0};
	unsigned short       bufLen = 0;	
	int                  ret = AMS_ERROR;
	VTM_ID_NODE	         *pVtmIdNode;
	unsigned int         vtmPos;	
	unsigned int         i;
	unsigned int         j;
    char                 *reasonstr = NULL;
	
    //检查函数入参
    if (!pvtm || !pResultCode || !pResultDescription)
    {
        if (pResultCode)
        {
            *pResultCode = OA_FAIL_UNKNOWN_ERROR;
        }
        return OA_RET_FAIL;
    }
    
    *pResultCode = OA_FAIL_NONE;
    
    //检查必带参数
    for (i = 0; i < AMSVTMCFG_ITEM_MAX; i++)
    {
        if (AddVtmNecessaryFlag[i] && !pvtm->flag[i])
        {
            *pResultCode = OA_FAIL_PARA_MISS;
            OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsVtmParaName[i],NULL);            
            return OA_RET_FAIL;        
        }
    }

    //检查参数范围
    if ((i = AmsCheckOAVtmParaValue(pvtm)) <  AMSVTMCFG_ITEM_MAX)
    {
        *pResultCode = OA_FAIL_PARAVAL_INV;
        OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsVtmParaName[i],NULL);            
        return OA_RET_FAIL;        
    }

    //检查vtm是否已存在
    vtmNum = Min(SystemData.AmsPriData.amsCfgData.maxVtmNum, AMS_MAX_VTM_NUM);
	for(i = 0; i < vtmNum; i++)
	{
		if(AmsCfgVtm(i).vtmId == pvtm->VtmId && AMS_VTM_INSTALL == AmsCfgVtm(i).flag)
		{
			*pResultCode = AMS_OAREASON_VTM_ADDED_BEFORE;
			reasonstr = OmsagentGetReasonStr(*pResultCode);
			OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));                                
            return OA_RET_FAIL;     
		}
	}
        
    //查找空闲项并记录
	for(i = 0; i < vtmNum; i++)
	{
		if(AMS_VTM_UNINSTALL == AmsCfgVtm(i).flag)
		{
		    memset(&AmsCfgVtm(i), 0, sizeof(VTM_INFO));
    		AmsCfgVtm(i).vtmId = pvtm->VtmId;
			AmsCfgVtm(i).flag = AMS_VTM_INSTALL;
            AmsCfgVtm(i).terminalType = AMS_TERMINAL_VTM;   //zhuyn added 20160913 数据库目前没有这个字段
            AmsCfgVtm(i).vtmRemoteCoopType = AMS_TRANS_TCP_SERVER;   //zhuyn added 20160921 数据库目前没有这个字段

			memset(&AmsVtmStat(i), 0, sizeof(AMS_VTM_STAT));
			AmsVtmStat(i).vtmId = pvtm->VtmId;

            AmsSetVtmInfoFromOA(i, pvtm);

			vtmPos = i;
			pVtmIdNode = AmsGetVtmIdNode();
			if(NULL != pVtmIdNode)
			{
				pVtmIdNode->vtmId = AmsCfgVtm(i).vtmId;
				pVtmIdNode->vtmPos = i;
				
				AmsInsertVtmIdHash(pVtmIdNode);
			}
			
            OmsagentCopystr(pResultDescription, descriptionBufLen-1, OKStr, strlen(OKStr));
//          return OA_RET_SUC;
			ret = AMS_OK;
			break;			
		}
	}

	//发给在线Teller
	vtaNum = Min(SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NUM);
	for(i = 0; i < vtaNum; i++)
	{
		if(AmsCfgTeller(i).flag != AMS_TELLER_INSTALL)
		{
			continue;
		}
		
	    //检查teller是否在线
    	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
    	{
    		if (NULL == (pVtaNode = AmsSearchVtaNode(j, AmsCfgTeller(i).tellerId)))
    		{
				continue;
    		}

			pid = pVtaNode->amsPid & 0xffff;
			if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
			{
				//...
				continue;
			}

			lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
			
			if(    lpAmsData->amsPid != pVtaNode->amsPid 
				|| lpAmsData->myPid.iProcessId != pid 
				|| lpAmsData->tellerId != pVtaNode->vtaInfo.tellerId)
			{
				//...
				continue;
			}

			bufLen = AmsPackSingleTermIdNo(AMS_OPERATE_ORIGIN_VTM, AmsCfgVtm(vtmPos).vtmId, AmsCfgVtm(vtmPos).vtmNoLen, AmsCfgVtm(vtmPos).vtmNo, buf, AMS_MAX_SINGLE_TERM_ID_NO_LEN);
				
		    //向终端发送添加请求
		    if (AMS_SUCCESS!= AmsSendVtaParaCfgInd(pVtaNode, &msg, NULL, NULL, buf, bufLen, CONFIGIND_ADD_ID_NO))
		    {
				//...
		        continue;     
		    }

    	}		

	}

	//发给在线Vtm
//  vtmNum = Min(SystemData.AmsPriData.amsCfgData.maxVtmNum, AMS_MAX_VTM_NUM);	 	
	for(i = 0; i < vtmNum; i++)
	{
		if(AmsCfgVtm(i).flag != AMS_VTM_INSTALL)
		{
			continue;
		}
		
	    //检查vtm是否在线
    	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
    	{
    		if (NULL == (pVtmNode = AmsSearchVtmNode(j, AmsCfgVtm(i).vtmId)))
    		{
				continue;
    		}

			bufLen = AmsPackSingleTermIdNo(AMS_OPERATE_ORIGIN_VTM, AmsCfgVtm(vtmPos).vtmId, AmsCfgVtm(vtmPos).vtmNoLen, AmsCfgVtm(vtmPos).vtmNo, buf, AMS_MAX_SINGLE_TERM_ID_NO_LEN);
			
		    //向终端发送添加请求
		    if (AMS_SUCCESS!= AmsSendVtmParaCfgInd(pVtmNode, &msg, NULL, NULL, buf, bufLen, CONFIGIND_ADD_ID_NO))
		    {			
				
		        continue;       
		    }										
    	}			
	}

	if(AMS_OK == ret)
	{
		return OA_RET_SUC;
	}
		
	*pResultCode = AMS_OAREASON_VTM_FULL;
	reasonstr = OmsagentGetReasonStr(*pResultCode);
	OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));          
    return OA_RET_FAIL;
}

int AmsModifyVtm(OA_AMS_VTM_CONFIG_t *pvtm, int *pResultCode, char *pResultDescription, int descriptionBufLen)
{
	int                  len;
	static unsigned char ModifyVtmNecessaryFlag[AMSVTMCFG_ITEM_MAX]={1,0,0,0,0,0,0};
	LP_AMS_DATA_t		 *lpAmsData = NULL;         //进程数据区指针		
	VTA_NODE             *pVtaNode = NULL;
	VTM_NODE             *pVtmNode = NULL;	
	unsigned int         vtaNum = 0;
	unsigned int         vtmNum = 0;
	MESSAGE_t            msg;	
	int                  pid = 0;
	unsigned char        buf[AMS_MAX_SINGLE_TERM_ID_NO_LEN] = {0};
	unsigned short       bufLen = 0;	
	unsigned int         vtmPos;	
	unsigned int         i;
	unsigned int         j;
    char                 *reasonstr = NULL;
	
    //检查函数入参
    if (!pvtm || !pResultCode || !pResultDescription)
    {
        if (pResultCode)
        {
            *pResultCode = OA_FAIL_UNKNOWN_ERROR;
        }
        return OA_RET_FAIL;
    }
    
    *pResultCode = OA_FAIL_NONE;
    
    //检查必带参数
    for (i = 0; i < AMSVTMCFG_ITEM_MAX; i++)
    {
        if (ModifyVtmNecessaryFlag[i] && !pvtm->flag[i])
        {
            *pResultCode = OA_FAIL_PARA_MISS;
            OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsVtmParaName[i],NULL);            
            return OA_RET_FAIL;        
        }
    }

    //检查参数范围
    if ((i = AmsCheckOAVtmParaValue(pvtm)) <  AMSVTMCFG_ITEM_MAX)
    {
        *pResultCode = OA_FAIL_PARAVAL_INV;
         OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsVtmParaName[i],NULL);            
        return OA_RET_FAIL;        
    }

    //检查vtm是否已存在
    vtmNum = Min(SystemData.AmsPriData.amsCfgData.maxVtmNum, AMS_MAX_VTM_NUM);
	for(i = 0; i < vtmNum; i++)
	{
		if(AmsCfgVtm(i).vtmId == pvtm->VtmId && AMS_VTM_INSTALL == AmsCfgVtm(i).flag)
		{
		    break;
		}
	}
    if (i >= vtmNum)
    {
        *pResultCode = AMS_OAREASON_VTM_NOT_EXIST;
        reasonstr = OmsagentGetReasonStr(*pResultCode);
        OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));                
        return OA_RET_FAIL;     
    }

    //更新记录
    AmsSetVtmInfoFromOA(i, pvtm);
	vtmPos = i;
	

 	//发给在线Teller
	vtaNum = Min(SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NUM);
	for(i = 0; i < vtaNum; i++)
	{
		if(AmsCfgTeller(i).flag != AMS_TELLER_INSTALL)
		{
			continue;
		}
		
	    //检查teller是否在线
    	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
    	{
    		if (NULL == (pVtaNode = AmsSearchVtaNode(j, AmsCfgTeller(i).tellerId)))
    		{
				continue;
    		}

			pid = pVtaNode->amsPid & 0xffff;
			if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
			{
				//...
				continue;
			}

			lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
			
			if(    lpAmsData->amsPid != pVtaNode->amsPid 
				|| lpAmsData->myPid.iProcessId != pid 
				|| lpAmsData->tellerId != pVtaNode->vtaInfo.tellerId)
			{
				//...
				continue;
			}

			bufLen = AmsPackSingleTermIdNo(AMS_OPERATE_ORIGIN_VTM, AmsCfgVtm(vtmPos).vtmId, AmsCfgVtm(vtmPos).vtmNoLen, AmsCfgVtm(vtmPos).vtmNo, buf, AMS_MAX_SINGLE_TERM_ID_NO_LEN);
				
		    //向终端发送修改请求
		    if (AMS_SUCCESS!= AmsSendVtaParaCfgInd(pVtaNode, &msg, NULL, NULL, buf, bufLen, CONFIGIND_MODIFY_ID_NO))
		    {
				//...
		        continue;     
		    }

    	}		

	}

 	//发给在线Vtm
//  vtmNum = Min(SystemData.AmsPriData.amsCfgData.maxVtmNum, AMS_MAX_VTM_NUM);	 	
	for(i = 0; i < vtmNum; i++)
	{
		if(AmsCfgVtm(i).flag != AMS_VTM_INSTALL)
		{
			continue;
		}
		
	    //检查vtm是否在线
    	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
    	{
    		if (NULL == (pVtmNode = AmsSearchVtmNode(j, AmsCfgVtm(i).vtmId)))
    		{
				continue;
    		}

			bufLen = AmsPackSingleTermIdNo(AMS_OPERATE_ORIGIN_VTM, AmsCfgVtm(vtmPos).vtmId, AmsCfgVtm(vtmPos).vtmNoLen, AmsCfgVtm(vtmPos).vtmNo, buf, AMS_MAX_SINGLE_TERM_ID_NO_LEN);
			
		    //向终端发送修改请求
		    if (AMS_SUCCESS!= AmsSendVtmParaCfgInd(pVtmNode, &msg, NULL, NULL, buf, bufLen, CONFIGIND_MODIFY_ID_NO))
		    {			
				
		        continue;       
		    }										
    	}			
	}

    //检查vtm是否在线
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		if (AmsSearchVtmNode(i, pvtm->VtmId) != NULL)
		{		    
        	      *pResultCode = AMS_OAREASON_VTM_NEED_RELOG;
                    reasonstr = OmsagentGetReasonStr(*pResultCode);
                    OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));                
            return OA_RET_SUC;    
		}
	}

    OmsagentCopystr(pResultDescription, descriptionBufLen-1, OKStr, strlen(OKStr));
    return OA_RET_SUC;
}

int AmsDeleteVtm(OA_AMS_VTM_CONFIG_t *pvtm, int *pResultCode, char *pResultDescription, int descriptionBufLen)
{
	int                  len;
	static unsigned char DeleteVtmNecessaryFlag[AMSVTMCFG_ITEM_MAX]={1,0,0,0,0,0,0};
	LP_AMS_DATA_t		 *lpAmsData = NULL;         //进程数据区指针		
	VTA_NODE             *pVtaNode = NULL;
	VTM_NODE             *pVtmNode = NULL;	
	unsigned int         vtaNum = 0;
	unsigned int         vtmNum = 0;
	MESSAGE_t            msg;	
	int                  pid = 0;
	unsigned char        buf[AMS_MAX_SINGLE_TERM_ID_NO_LEN] = {0};
	unsigned short       bufLen = 0;	
	int                  ret = AMS_ERROR;
	VTM_ID_NODE	         *pVtmIdNode;
	unsigned int         vtmPos;
	unsigned int         i;
	unsigned int         j;
    char                 *reasonstr = NULL;
	
    //检查函数入参
    if (!pvtm || !pResultCode || !pResultDescription)
    {
        if (pResultCode)
        {
            *pResultCode = OA_FAIL_UNKNOWN_ERROR;
        }
        return OA_RET_FAIL;
    }
    
    *pResultCode = OA_FAIL_NONE;
    
    //检查必带参数
    for (i = 0; i < AMSVTMCFG_ITEM_MAX; i++)
    {
        if (DeleteVtmNecessaryFlag[i] && !pvtm->flag[i])
        {
            *pResultCode = OA_FAIL_PARA_MISS;
            OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsVtmParaName[i],NULL);            
            return OA_RET_FAIL;        
        }
    }

    //检查参数范围

    //检查vtm是否已存在
    vtmNum = Min(SystemData.AmsPriData.amsCfgData.maxVtmNum, AMS_MAX_VTM_NUM);
	for(i = 0; i < vtmNum; i++)
	{
		if(AmsCfgVtm(i).vtmId == pvtm->VtmId && AMS_VTM_INSTALL == AmsCfgVtm(i).flag)
		{
		    //检查vtm是否在线
        	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
        	{
        		if (AmsSearchVtmNode(j, pvtm->VtmId) != NULL)
        		{
        		    *pResultCode = AMS_OAREASON_VTM_ONLINE;
                         reasonstr = OmsagentGetReasonStr(*pResultCode);
                         OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));
                    return OA_RET_FAIL;                        		    
        		}
        	}

			vtmPos = i;
//          memset(&AmsCfgVtm(i), 0, sizeof(VTM_INFO));
//		    OmsagentCopystr(pResultDescription, descriptionBufLen-1, OKStr, strlen(OKStr));
//          return OA_RET_SUC;
			ret = AMS_OK;
			break;				
		}
	}
	
 	//发给在线Teller
	vtaNum = Min(SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NUM);
	for(i = 0; i < vtaNum; i++)
	{
		if(AmsCfgTeller(i).flag != AMS_TELLER_INSTALL)
		{
			continue;
		}
		
	    //检查teller是否在线
    	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
    	{
    		if (NULL == (pVtaNode = AmsSearchVtaNode(j, AmsCfgTeller(i).tellerId)))
    		{
				continue;
    		}

			pid = pVtaNode->amsPid & 0xffff;
			if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
			{
				//...
				continue;
			}

			lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
			
			if(lpAmsData->amsPid != pVtaNode->amsPid ||
			   lpAmsData->myPid.iProcessId != pid ||
			   lpAmsData->tellerId != pVtaNode->vtaInfo.tellerId)
			{
				//...
				continue;
			}

			bufLen = AmsPackSingleTermIdNo(AMS_OPERATE_ORIGIN_VTM, AmsCfgVtm(vtmPos).vtmId, AmsCfgVtm(vtmPos).vtmNoLen, AmsCfgVtm(vtmPos).vtmNo, buf, AMS_MAX_SINGLE_TERM_ID_NO_LEN);
				
		    //向终端发送删除请求
		    if (AMS_SUCCESS!= AmsSendVtaParaCfgInd(pVtaNode, &msg, NULL, NULL, buf, bufLen, CONFIGIND_DELETE_ID_NO))
		    {
				//...
		        continue;     
		    }

    	}		

	}

 	//发给在线Vtm
//  vtmNum = Min(SystemData.AmsPriData.amsCfgData.maxVtmNum, AMS_MAX_VTM_NUM);	 	
	for(i = 0; i < vtmNum; i++)
	{
		if(AmsCfgVtm(i).flag != AMS_VTM_INSTALL)
		{
			continue;
		}
		
	    //检查vtm是否在线
    	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
    	{
    		if (NULL == (pVtmNode = AmsSearchVtmNode(j, AmsCfgVtm(i).vtmId)))
    		{
				continue;
    		}

			bufLen = AmsPackSingleTermIdNo(AMS_OPERATE_ORIGIN_VTM, AmsCfgVtm(vtmPos).vtmId, AmsCfgVtm(vtmPos).vtmNoLen, AmsCfgVtm(vtmPos).vtmNo, buf, AMS_MAX_SINGLE_TERM_ID_NO_LEN);
			
		    //向终端发送删除请求
		    if (AMS_SUCCESS!= AmsSendVtmParaCfgInd(pVtmNode, &msg, NULL, NULL, buf, bufLen, CONFIGIND_DELETE_ID_NO))
		    {			
				
		        continue;       
		    }										
    	}			
	}

	if(AMS_OK == ret)
	{
		pVtmIdNode = AmsSearchVtmIdHash(AmsCfgVtm(vtmPos).vtmId);
		if(NULL != pVtmIdNode)
		{
			AmsDeleteVtmIdHash(pVtmIdNode);

			AmsFreeVtmIdNode(pVtmIdNode);
		}

		memset(&AmsVtmStat(vtmPos), 0, sizeof(AMS_VTM_STAT));

		AmsResetVtmRegInfo(AmsCfgVtm(vtmPos).vtmId);
		
		memset(&AmsCfgVtm(vtmPos), 0, sizeof(VTM_INFO));
		
		OmsagentCopystr(pResultDescription, descriptionBufLen-1, OKStr, strlen(OKStr));
		return OA_RET_SUC;
	}
	
	*pResultCode = AMS_OAREASON_VTM_NOT_EXIST;
	reasonstr = OmsagentGetReasonStr(*pResultCode);
	OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));
    return OA_RET_FAIL;     

}

int AmsAddTellerAuth (OA_AMS_TELLERAUTH_CONFIG_t *pauth, int *pResultCode, char *pResultDescription, int descriptionBufLen)
{
    char *reasonstr = NULL;
    
    if (pResultCode)
    {
        *pResultCode = OA_FAIL_UNSUPPORT;
    }
    if (pResultDescription)
    {
         reasonstr = OmsagentGetReasonStr(OA_FAIL_UNSUPPORT);
         OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));
    }
    return OA_RET_FAIL;
}

int AmsDeleteTellerAuth (OA_AMS_TELLERAUTH_CONFIG_t *pauth, int *pResultCode, char *pResultDescription, int descriptionBufLen)
{
    char *reasonstr = NULL;
    
    if (pResultCode)
    {
        *pResultCode = OA_FAIL_UNSUPPORT;
    }
    if (pResultDescription)
    {
        reasonstr = OmsagentGetReasonStr(OA_FAIL_UNSUPPORT);
        OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));
    }
    return OA_RET_FAIL;
}

int AmsSetFileServer(OA_AMS_FILESERVER_CONFIG_t *pfilepath, int *pResultCode, char *pResultDescription, int descriptionBufLen)
{
	LP_AMS_DATA_t		*lpAmsData = NULL;         //进程数据区指针	
    int                 len;
    unsigned int        i;
	unsigned int        j;
	unsigned int        vtaNum = 0;
	unsigned int        vtmNum = 0;	
    VTA_NODE            *pVtaNode = NULL;
    VTM_NODE            *pVtmNode = NULL;	
	unsigned char       timerPara[AMS_MAX_TIMER_PARA_LEN];	
	unsigned char       *p;	
	int                 pid = 0;	
	int                 cfgNum = 0;		
	unsigned int        curTermType;
	unsigned int        curTermPos;	
    MESSAGE_t           msg;
    char                *reasonstr = NULL;
	
    //检查函数入参
    if (!pfilepath || !pResultCode || !pResultDescription)
    {
        if (pResultCode)
        {
            *pResultCode = OA_FAIL_UNKNOWN_ERROR;
        }
        return OA_RET_FAIL;
    }
    
    *pResultCode = OA_FAIL_NONE;

    if (0 != pfilepath->flag[AMSFILESERVER_ITEM_AUDIORECPATH])
    {
        SystemData.AmsPriData.amsCfgData.fileServerInfo.recFilePath.filePathLen 
            = OmsagentCopystr(AmsRecFilePath, AMS_MAX_FILEPATH_LEN, pfilepath->AudioRecFilePath, MAX_QCFILEPATH_LEN);
        SystemData.AmsPriData.amsCfgData.fileServerInfo.recFilePath.flag = 1;  //zhuyn 20161013
        
    }
    if (0 != pfilepath->flag[AMSFILESERVER_ITEM_SCREENRECPATH])
    {
        SystemData.AmsPriData.amsCfgData.fileServerInfo.screenRecFilePath.filePathLen 
            = OmsagentCopystr(AmsScreenRecFilePath, AMS_MAX_FILEPATH_LEN, pfilepath->ScreenRecFilePath, MAX_QCFILEPATH_LEN);
        SystemData.AmsPriData.amsCfgData.fileServerInfo.screenRecFilePath.flag = 1;  //zhuyn 20161013
    }
    if (0 != pfilepath->flag[AMSFILESERVER_ITEM_SNAPPATH])
    {
        SystemData.AmsPriData.amsCfgData.fileServerInfo.snapFilePath.filePathLen 
            = OmsagentCopystr(AmsSnapFilePath, AMS_MAX_FILEPATH_LEN, pfilepath->SnapFilePath, MAX_QCFILEPATH_LEN);
        SystemData.AmsPriData.amsCfgData.fileServerInfo.snapFilePath.flag = 1;  //zhuyn 20161013
    }
    //zhuyn 增加 20160927
    if (0 != pfilepath->flag[AMSFILESERVER_ITEM_ORDINARYPATH])
    {
        SystemData.AmsPriData.amsCfgData.fileServerInfo.ordinaryFilePath.filePathLen 
            = OmsagentCopystr(AmsOrdinaryFilePath, AMS_MAX_FILEPATH_LEN, pfilepath->OrdinaryFilePath, MAX_QCFILEPATH_LEN);
        SystemData.AmsPriData.amsCfgData.fileServerInfo.ordinaryFilePath.flag = 1;  //zhuyn 20161013
    }
    if (0 != pfilepath->flag[AMSFILESERVER_ITEM_IP]) 
    {
        DWORD uiIp;
        uiIp = inet_addr(pfilepath->Ip);
		SystemData.AmsPriData.amsCfgData.fileServerInfo.fileServerPara.serverIp = htonl(uiIp);
        SystemData.AmsPriData.amsCfgData.fileServerInfo.fileServerPara.flag = 1;
    }
    if (0 != pfilepath->flag[AMSFILESERVER_ITEM_PORT])
    {
        SystemData.AmsPriData.amsCfgData.fileServerInfo.fileServerPara.serverPort = (WORD)pfilepath->port;
        SystemData.AmsPriData.amsCfgData.fileServerInfo.fileServerPara.flag = 1;
    }
    if (0 != pfilepath->flag[AMSFILESERVER_ITEM_USER])
    {
        SystemData.AmsPriData.amsCfgData.fileServerInfo.fileServerPara.userNameLen = 
            OmsagentCopystr(SystemData.AmsPriData.amsCfgData.fileServerInfo.fileServerPara.userName,
                AMS_MAX_NAME_LEN, pfilepath->user, strlen(pfilepath->user));
        SystemData.AmsPriData.amsCfgData.fileServerInfo.fileServerPara.flag = 1;
    }
    if (0 != pfilepath->flag[AMSFILESERVER_ITEM_PWD])
    {
        SystemData.AmsPriData.amsCfgData.fileServerInfo.fileServerPara.userPwdLen = 
            OmsagentCopystr(SystemData.AmsPriData.amsCfgData.fileServerInfo.fileServerPara.userPwd,
                AMS_MAX_PWD_LEN, pfilepath->pwd, strlen(pfilepath->pwd));
        SystemData.AmsPriData.amsCfgData.fileServerInfo.fileServerPara.flag = 1;
    }    
    OmsagentCopystr(pResultDescription, descriptionBufLen-1, OKStr, strlen(OKStr));

	//检查teller是否已存在
	vtaNum = Min(SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NUM);
	for(i = 0; i < vtaNum; i++)
	{
		if(AmsCfgTeller(i).flag != AMS_TELLER_INSTALL)
		{
			continue;
		}
		
	    //检查teller是否在线
    	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
    	{
    		if (NULL == (pVtaNode = AmsSearchVtaNode(j, AmsCfgTeller(i).tellerId)))
    		{
				continue;
    		}

			pid = pVtaNode->amsPid & 0xffff;
			if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
			{
				//...
				continue;
			}

			lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
			
			if(lpAmsData->amsPid != pVtaNode->amsPid ||
			   lpAmsData->myPid.iProcessId != pid ||
			   lpAmsData->tellerId != pVtaNode->vtaInfo.tellerId)
			{
				//...
				continue;
			}
			
			//check vtaParaCfgState
			if(lpAmsData->vtaParaCfgState != AMS_TERM_PARA_CFG_NULL)
			{

				//...
				continue;	
			}
			
			//wait vta para cfg Ind cnf,default: 10s
		    if(AmsCfgData.amsVtaParaCfgTimeLength > 0 
			    && AmsCfgData.amsVtaParaCfgTimeLength <= T_AMS_VTA_PARA_CFG_TIMER_LENGTH_MAX)
		    {
				memset(timerPara, 0, PARA_LEN);
				p = timerPara;
				
				BEPUTSHORT(AMS_CONFIG_FILE_SERVER_PARA, p);
				
		        if(AMS_OK != AmsCreateTimerPara(lpAmsData->myPid.iProcessId,
						                        &lpAmsData->vtaParaCfgTimerId, 
												B_AMS_VTA_PARA_CFG_TIMEOUT, 
												AmsCfgData.amsVtaParaCfgTimeLength,
												timerPara))
		        {
					//...
					return AMS_ERROR;
		        }

				AmsTimerStatProc(T_AMS_VTA_PARA_CFG_TIMER, AMS_CREATE_TIMER);

		        if(lpAmsData->commonTrace)
		        {
		            dbgprint("Ams[%d] Create T_AMS_VTA_PARA_CFG_TIMER Timer:timerId=%d.",
						lpAmsData->myPid.iProcessId, lpAmsData->vtaParaCfgTimerId);
		        }
		    }	

			//update vtaParaCfgState
			lpAmsData->vtaParaCfgState = AMS_TERM_PARA_CFG_STARTING;


		    //向终端发送查询请求
		    if (AMS_SUCCESS!= AmsSendVtaParaCfgInd(pVtaNode, &msg, NULL, &AmsFileServerPara, NULL, 0, CONFIGIND_UPDATE_FILESERVER))
		    {
//			        *pResultCode = EXECESULT_FAIL;
//			        OmsagentCopystr(pResultDescription, descriptionBufLen-1, FailStr, strlen(FailStr));

				cfgNum++;	
				if(cfgNum >= AMS_ALLOWED_TERM_ID_NO_NUM)
				{								
					break;
				}
				
		        continue;     
		    }

			cfgNum++;
			if(cfgNum >= AMS_ALLOWED_TERM_ID_NO_NUM)
			{
				break;
			}
    	}		

		if(cfgNum >= AMS_ALLOWED_TERM_ID_NO_NUM)
		{
			break;
		}
	}

	curTermType = AMS_OPERATE_TARGET_VTA;
	curTermPos = i;	
		
	if(cfgNum < AMS_ALLOWED_TERM_ID_NO_NUM)
	{
	 	//检查vtm是否已存在
	    vtmNum = Min(SystemData.AmsPriData.amsCfgData.maxVtmNum, AMS_MAX_VTM_NUM);	 	
		for(i = 0; i < vtmNum; i++)
		{
			if(AmsCfgVtm(i).flag != AMS_VTM_INSTALL)
			{
				continue;
			}
			
		    //检查vtm是否在线
        	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
        	{
        		if (NULL == (pVtmNode = AmsSearchVtmNode(j, AmsCfgVtm(i).vtmId)))
        		{
					continue;
        		}
				
				//check vtmParaCfgState
				if(pVtmNode->vtmParaCfgState != AMS_TERM_PARA_CFG_NULL)
				{

					//...
					continue;	
				}
				
				//wait par cfg Ind cnf,default: 100s
			    if(AmsCfgData.amsVtmParaCfgTimeLength > 0 
				    && AmsCfgData.amsVtmParaCfgTimeLength <= T_AMS_VTM_PARA_CFG_TIMER_LENGTH_MAX)
			    {
					memset(timerPara, 0, PARA_LEN);
					p = timerPara;
					
					BEPUTSHORT(AMS_CONFIG_FILE_SERVER_PARA, p);
					p += 2;
					BEPUTLONG(AmsCfgVtm(i).vtmId, p);	
					
			        if(AMS_OK != AmsCreateTimerPara(pVtmNode->amsPid&0xffff, //not used yet
							                        &pVtmNode->vtmParaCfgTimerId, 
													B_AMS_VTM_PARA_CFG_TIMEOUT, 
													AmsCfgData.amsVtmParaCfgTimeLength,
													timerPara))
			        {

						//...
						return AMS_ERROR;
			        }

					AmsTimerStatProc(T_AMS_VTM_PARA_CFG_TIMER, AMS_CREATE_TIMER);

			        if(AmsCommonTrace)
			        {
			            dbgprint("Ams[%u] Create T_AMS_VTM_PARA_CFG_TIMER Timer:timerId=%d.",
							AmsCfgVtm(i).vtmId, pVtmNode->vtmParaCfgTimerId);
			        }
			    }	
				
				//update vtmParaCfgState
				pVtmNode->vtmParaCfgState = AMS_TERM_PARA_CFG_STARTING;    
						
			    //向终端发送查询请求
			    if (AMS_SUCCESS!= AmsSendVtmParaCfgInd(pVtmNode, &msg, NULL, &AmsFileServerPara, NULL, 0, CONFIGIND_UPDATE_FILESERVER))
			    {			
//			        *pResultCode = EXECESULT_FAIL;
//			        OmsagentCopystr(pResultDescription, descriptionBufLen-1, FailStr, strlen(FailStr));

					cfgNum++;	
					if(cfgNum >= AMS_ALLOWED_TERM_ID_NO_NUM)
					{								
						break;
					}
					
			        continue;       
			    }

				cfgNum++;
				if(cfgNum >= AMS_ALLOWED_TERM_ID_NO_NUM)
				{					
					break;
				}
											
        	}	
			
			if(cfgNum >= AMS_ALLOWED_TERM_ID_NO_NUM)
			{					
				break;
			}	
			
		}

		curTermType = AMS_OPERATE_TARGET_VTM;
		curTermPos = i;					
	}

	if(AMS_OPERATE_TARGET_VTM != curTermType || curTermPos != vtmNum)
	{
		//wait para cfg Ind cnf,default: 100s
	    if(AmsCfgData.amsParaCfgTimeLength > 0 
		    && AmsCfgData.amsParaCfgTimeLength <= T_AMS_PARA_CFG_TIMER_LENGTH_MAX)
	    {
			memset(timerPara, 0, PARA_LEN);
			p = timerPara;
			
			BEPUTSHORT(AMS_CONFIG_FILE_SERVER_PARA, p);
			p += 2;
			BEPUTLONG(curTermType, p);	
			p += 4;			
			BEPUTLONG(curTermPos, p);				
			
	        if(AMS_OK != AmsCreateTimerPara(0, //not used yet
					                        &AmsCfgData.paraCfgTimerId, 
											B_AMS_PARA_CFG_TIMEOUT, 
											AmsCfgData.amsParaCfgTimeLength,
											timerPara))
	        {

				//...
				return AMS_ERROR;
	        }

			AmsTimerStatProc(T_AMS_PARA_CFG_TIMER, AMS_CREATE_TIMER);

	        if(AmsCommonTrace)
	        {
	            dbgprint("Ams[%u][%u] Create T_AMS_PARA_CFG_TIMER Timer:timerId=%d.",
					curTermType, curTermPos, AmsCfgData.paraCfgTimerId);
	        }
	    }	
	}

    return OA_RET_SUC;
}

unsigned int AmsCheckOAGlobalConfigValue(OA_AMS_GLOBAL_CONFIG_t *pGblCfg)
{
    if ( pGblCfg->flag[AMSGBLCFG_ITEM_LOGINSTATE] && (pGblCfg->loginState < AMS_VTC_TELLER_STATE_IDLE || pGblCfg->loginState >= AMS_VTC_TELLER_STATE_RSVD))
    {
        return AMSGBLCFG_ITEM_LOGINSTATE;
    }
    else if ( pGblCfg->flag[AMSGBLCFG_ITEM_RELSTATE] && (pGblCfg->relState < AMS_VTC_TELLER_STATE_IDLE || pGblCfg->relState >= AMS_VTC_TELLER_STATE_RSVD))
    {
        return AMSGBLCFG_ITEM_RELSTATE;
    }
    else if ( pGblCfg->flag[AMSGBLCFG_ITEM_TQUEUELEN] && (pGblCfg->tQueueLen < 1 || pGblCfg->tQueueLen > 3000))
    {
        return AMSGBLCFG_ITEM_TQUEUELEN;
    }
    else if ( pGblCfg->flag[AMSGBLCFG_ITEM_ENCRYPTFLAG] && (pGblCfg->encryptFlag < 0 || pGblCfg->encryptFlag > 1))
    {
        return AMSGBLCFG_ITEM_ENCRYPTFLAG;
    }
    else if ( pGblCfg->flag[AMSGBLCFG_ITEM_BRANCHFLAG] && (pGblCfg->branchFlag < 0 || pGblCfg->branchFlag > 1)) //zhuyn 20161228
    {
        return AMSGBLCFG_ITEM_BRANCHFLAG;
    }
    
    return AMSGBLCFG_ITEM_MAX;
}
int AmsModifyGlobalConfig(OA_AMS_GLOBAL_CONFIG_t *pGblCfg, int *pResultCode, char *pResultDescription, int descriptionBufLen)
{
    int len;
    unsigned int i,vtmNum;

    //检查函数入参
    if (!pGblCfg || !pResultCode || !pResultDescription)
    {
        if (pResultCode)
        {
            *pResultCode = OA_FAIL_UNKNOWN_ERROR;
        }
        return OA_RET_FAIL;
    }

    //检查参数范围
    if ((i = AmsCheckOAGlobalConfigValue(pGblCfg)) <  AMSGBLCFG_ITEM_MAX)
    {
        *pResultCode = OA_FAIL_PARAVAL_INV;
        OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsGllobalCfgParaName[i],NULL);                        
        return OA_RET_FAIL;        
    }

    //赋值        
    if (pGblCfg->flag[AMSGBLCFG_ITEM_LOGINSTATE])
    {
        SystemData.AmsPriData.amsCfgData.tellerLoginState = (unsigned char)pGblCfg->loginState;
    }
    if (pGblCfg->flag[AMSGBLCFG_ITEM_RELSTATE])
    {
        SystemData.AmsPriData.amsCfgData.tellerCallRelState = (unsigned char)pGblCfg->relState;
    }
    if (pGblCfg->flag[AMSGBLCFG_ITEM_TQUEUELEN])
    {
        SystemData.AmsPriData.amsCfgData.amsCustomerInQueueTimeLength = pGblCfg->tQueueLen;
    }
    //zhuyn 增加 20160927
    if (pGblCfg->flag[AMSGBLCFG_ITEM_ENCRYPTFLAG])
    {
        SystemData.AmsPriData.amsCfgData.encryptInfo.encryptFlag = pGblCfg->encryptFlag;
        SystemData.AmsPriData.amsCfgData.encryptInfo.flag = 1;
    }
    if (pGblCfg->flag[AMSGBLCFG_ITEM_BRANCHFLAG])   //zhuyn 20161228
    {
        SystemData.AmsPriData.amsCfgData.branchInfo.branchFlag = (WORD)pGblCfg->branchFlag;
        SystemData.AmsPriData.amsCfgData.branchInfo.flag = 1;
    }

    *pResultCode = OA_FAIL_NONE;

    OmsagentCopystr(pResultDescription, descriptionBufLen-1, OKStr, strlen(OKStr));
    return OA_RET_SUC;
}


unsigned int AmsCheckOANatConfigValue(OA_AMS_NAT_CONFIG_t *pNatCfg)
{
    if ( pNatCfg->flag[AMSNATCFG_ITEM_NATFLAG] && (pNatCfg->natFlag < 0 || pNatCfg->natFlag > 1))
    {
        return AMSNATCFG_ITEM_NATFLAG;
    }
    
    return AMSNATCFG_ITEM_MAX;
}

//zhuyn added 20161009 begin
int AmsModifyNatConfig(OA_AMS_NAT_CONFIG_t *pNatCfg, int *pResultCode, char *pResultDescription, int descriptionBufLen)
{
    int len;
    unsigned int i,vtmNum;

    //检查函数入参
    if (!pNatCfg || !pResultCode || !pResultDescription)
    {
        if (pResultCode)
        {
            *pResultCode = OA_FAIL_UNKNOWN_ERROR;
        }
        return OA_RET_FAIL;
    }

    //检查参数范围
    if ((i = AmsCheckOANatConfigValue(pNatCfg)) <  AMSNATCFG_ITEM_MAX)
    {
        *pResultCode = OA_FAIL_PARAVAL_INV;
        OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsNatCfgParaName[i],NULL);                        
        return OA_RET_FAIL;        
    }

    //赋值        
    if (pNatCfg->flag[AMSNATCFG_ITEM_NATFLAG])
    {
        SystemData.AmsPriData.amsCfgData.netTraversalInfo.netTraversalFlag = pNatCfg->natFlag;
        SystemData.AmsPriData.amsCfgData.netTraversalInfo.flag = 1;
    }
    if (0 != pNatCfg->flag[AMSNATCFG_ITEM_STUNIPFLAG]) 
    {
        DWORD uiIp;
        uiIp = inet_addr(pNatCfg->stunIp);
        SystemData.AmsPriData.amsCfgData.netTraversalInfo.stunServerIp = htonl(uiIp);
        SystemData.AmsPriData.amsCfgData.netTraversalInfo.flag = 1;
    }
    if (0 != pNatCfg->flag[AMSNATCFG_ITEM_STUNPORTFLAG])
    {
        SystemData.AmsPriData.amsCfgData.netTraversalInfo.stunServerPort = (WORD)pNatCfg->stunPort;
        SystemData.AmsPriData.amsCfgData.netTraversalInfo.flag = 1;
    }
    if (0 != pNatCfg->flag[AMSNATCFG_ITEM_STUNUSERFLAG])
    {
        SystemData.AmsPriData.amsCfgData.netTraversalInfo.stunUserNameLen = 
            OmsagentCopystr(SystemData.AmsPriData.amsCfgData.netTraversalInfo.stunUserName,
                AMS_MAX_NAME_LEN, pNatCfg->stunUser, strlen(pNatCfg->stunUser));
        SystemData.AmsPriData.amsCfgData.netTraversalInfo.flag = 1;
    }
    if (0 != pNatCfg->flag[AMSNATCFG_ITEM_STUNPWDFLAG])
    {
        SystemData.AmsPriData.amsCfgData.netTraversalInfo.stunPwdLen = 
            OmsagentCopystr(SystemData.AmsPriData.amsCfgData.netTraversalInfo.stunPwd,
                AMS_MAX_PWD_LEN, pNatCfg->stunPwd, strlen(pNatCfg->stunPwd));
        SystemData.AmsPriData.amsCfgData.netTraversalInfo.flag = 1;
    }  
    if (0 != pNatCfg->flag[AMSNATCFG_ITEM_TURNIPFLAG]) 
    {
        DWORD uiIp;
        uiIp = inet_addr(pNatCfg->turnIp);
        SystemData.AmsPriData.amsCfgData.netTraversalInfo.turnServerIp = htonl(uiIp);
        SystemData.AmsPriData.amsCfgData.netTraversalInfo.flag = 1;
    }
    if (0 != pNatCfg->flag[AMSNATCFG_ITEM_TURNPORTFLAG])
    {
        SystemData.AmsPriData.amsCfgData.netTraversalInfo.turnServerPort = (WORD)pNatCfg->turnPort;
        SystemData.AmsPriData.amsCfgData.netTraversalInfo.flag = 1;
    }
    if (0 != pNatCfg->flag[AMSNATCFG_ITEM_TURNUSERFLAG])
    {
        SystemData.AmsPriData.amsCfgData.netTraversalInfo.turnUserNameLen = 
            OmsagentCopystr(SystemData.AmsPriData.amsCfgData.netTraversalInfo.turnUserName,
                AMS_MAX_NAME_LEN, pNatCfg->turnUser, strlen(pNatCfg->turnUser));
        SystemData.AmsPriData.amsCfgData.netTraversalInfo.flag = 1;
    }
    if (0 != pNatCfg->flag[AMSNATCFG_ITEM_TURNPWDFLAG])
    {
        SystemData.AmsPriData.amsCfgData.netTraversalInfo.turnPwdLen = 
            OmsagentCopystr(SystemData.AmsPriData.amsCfgData.netTraversalInfo.turnPwd,
                AMS_MAX_PWD_LEN, pNatCfg->turnPwd, strlen(pNatCfg->turnPwd));
        SystemData.AmsPriData.amsCfgData.netTraversalInfo.flag = 1;
    }  
    
    *pResultCode = OA_FAIL_NONE;

    OmsagentCopystr(pResultDescription, descriptionBufLen-1, OKStr, strlen(OKStr));
    return OA_RET_SUC;
}
//zhuyn added 20161009 end

int AmsAddRcasConfig(OA_AMS_RCAS_CONFIG_t *pRcasCfg, int *pResultCode, char *pResultDescription, int descriptionBufLen)
{
    int len;
    unsigned int i,vtmNum, index;
    char *reasonstr;

    //检查函数入参
    if (!pRcasCfg || !pResultCode || !pResultDescription)
    {
        if (pResultCode)
        {
            *pResultCode = OA_FAIL_UNKNOWN_ERROR;
        }
        return OA_RET_FAIL;
    }

    //检查必备参数
    if (0 == pRcasCfg->flag[AMSRCASCFG_ITEM_MID])
    {
        *pResultCode = OA_FAIL_PARA_MISS;
         OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsRcasCfgParaName[AMSRCASCFG_ITEM_MID],NULL);            
        return OA_RET_FAIL;    
    }

    //检查参数范围
    if (pRcasCfg->rcasMid > 256)
    {
        *pResultCode = OA_FAIL_PARAVAL_INV;
        OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsRcasCfgParaName[AMSRCASCFG_ITEM_MID],NULL);                        
        return OA_RET_FAIL;        
    }

    //赋值
    index = AMS_MAX_RCAS_NUM;
    for (i = 0; i < AMS_MAX_RCAS_NUM; i++)  //找空闲的记录项
    {
        if (0 == AmsCfgRcas(i).flag)
        {
            if (index == AMS_MAX_RCAS_NUM)
                index = i;
        }
        else
        {
            if (pRcasCfg->rcasMid == AmsCfgRcas(i).cModuleId)
            {
                *pResultCode = AMS_OAREASON_RCAS_ADDED_BEFORE;
                reasonstr = OmsagentGetReasonStr(*pResultCode);
                OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));       
                return OA_RET_FAIL;    
            }
        }
    }
    if(index >= AMS_MAX_RCAS_NUM)   //记录满
    {
        *pResultCode = AMS_OAREASON_RCAS_FULL;
        reasonstr = OmsagentGetReasonStr(*pResultCode);
        OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));       
        return OA_RET_FAIL;    
    }
    AmsCfgRcas(index).flag = 1;
    AmsCfgRcas(index).cModuleId = (unsigned char )pRcasCfg->rcasMid;
    *pResultCode = OA_FAIL_NONE;

    OmsagentCopystr(pResultDescription, descriptionBufLen-1, OKStr, strlen(OKStr));
    return OA_RET_SUC;
}

int AmsDeleteRcasConfig(OA_AMS_RCAS_CONFIG_t *pRcasCfg, int *pResultCode, char *pResultDescription, int descriptionBufLen)
{
    int len;
    unsigned int i,vtmNum;
    char *reasonstr;

    //检查函数入参
    if (!pRcasCfg || !pResultCode || !pResultDescription)
    {
        if (pResultCode)
        {
            *pResultCode = OA_FAIL_UNKNOWN_ERROR;
        }
        return OA_RET_FAIL;
    }

    //检查必备参数
    if (0 == pRcasCfg->flag[AMSRCASCFG_ITEM_MID])
    {
        *pResultCode = OA_FAIL_PARA_MISS;
         OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsRcasCfgParaName[AMSRCASCFG_ITEM_MID],NULL);            
        return OA_RET_FAIL;    
    }

    //检查参数范围
    if (pRcasCfg->rcasMid > 256)
    {
        *pResultCode = OA_FAIL_PARAVAL_INV;
        OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsRcasCfgParaName[AMSRCASCFG_ITEM_MID],NULL);                        
        return OA_RET_FAIL;        
    }

    //找记录
    for (i = 0; i < AMS_MAX_RCAS_NUM; i++)  //找空闲的记录项
    {
        if (0 != AmsCfgRcas(i).flag && pRcasCfg->rcasMid == AmsCfgRcas(i).cModuleId)
            break;
    }
    if(i >= AMS_MAX_RCAS_NUM)   //无此记录
    {
        *pResultCode = AMS_OAREASON_RCAS_NOT_EXIST;
        reasonstr = OmsagentGetReasonStr(*pResultCode);
        OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));       
        return OA_RET_FAIL;    
    }
    AmsCfgRcas(i).flag = 0;
    AmsCfgRcas(i).cModuleId = 0;
    *pResultCode = OA_FAIL_NONE;

    OmsagentCopystr(pResultDescription, descriptionBufLen-1, OKStr, strlen(OKStr));
    return OA_RET_SUC;
}

//zhuyn 20161228
unsigned int AmsCheckOAOrgCfgValue(OA_AMS_ORG_CONFIG_t *pOrgCfg)
{
    int i;
    if ( pOrgCfg->flag[AMSORGCFG_ITEM_ORGTYPE] && (i = atoi(pOrgCfg->orgType)) < 1 || i > 2)
    {
        return AMSORGCFG_ITEM_ORGTYPE;
    }
    else if (pOrgCfg->flag[AMSORGCFG_ITEM_ORGSTATE] && (i = atoi(pOrgCfg->orgState)) < 1 || i > 5)
    {
        return AMSORGCFG_ITEM_ORGSTATE;
    }
    
    return AMSORGCFG_ITEM_MAX;
}

int AmsSetOrgInfoFromOA(unsigned int i, OA_AMS_ORG_CONFIG_t *pOrgCfg)
{
    if (pOrgCfg->flag[AMSORGCFG_ITEM_ORGCODE])
    {
        AmsCfgOrg(i).orgCodeLen = OmsagentCopystr(AmsCfgOrg(i).orgCode, AMS_MAX_ORG_CODE_LEN,
            pOrgCfg->orgCode, strlen(pOrgCfg->orgCode));
    }
    if (pOrgCfg->flag[AMSORGCFG_ITEM_ORGNAME])
    {
        AmsCfgOrg(i).orgNameLen = OmsagentCopystr(AmsCfgOrg(i).orgName, AMS_MAX_ORG_NAME_LEN,
            pOrgCfg->orgName, strlen(pOrgCfg->orgName));
    }
    if (pOrgCfg->flag[AMSORGCFG_ITEM_ORGTYPE])
    {
        AmsCfgOrg(i).orgType = atoi(pOrgCfg->orgType);
    }
    if (pOrgCfg->flag[AMSORGCFG_ITEM_ORGLEVEL])
    {
        AmsCfgOrg(i).orgLevel = pOrgCfg->orgLevel;
    }
    if (pOrgCfg->flag[AMSORGCFG_ITEM_PARENTORGID])
    {
        AmsCfgOrg(i).parentOrgId = pOrgCfg->parentOrgId;
        AmsCfgOrg(i).parentOrgIdFlag = 1;
    }
    if (pOrgCfg->flag[AMSORGCFG_ITEM_ORGSTATE])
    {
        AmsCfgOrg(i).orgState = atoi(pOrgCfg->orgState);
        AmsCfgOrg(i).orgStateFlag = 1;
    }
    
    return 0;
}

int AmsAddOrg(OA_AMS_ORG_CONFIG_t *pOrgCfg, int *pResultCode, char *pResultDescription, int descriptionBufLen)
{
    unsigned int i;
    char *reasonstr;
    ORG_ID_NODE         *pNode = NULL;

    //检查函数入参
    if (!pOrgCfg || !pResultCode || !pResultDescription)
    {
        if (pResultCode)
        {
            *pResultCode = OA_FAIL_UNKNOWN_ERROR;
        }
        return OA_RET_FAIL;
    }

    //检查必备参数
    if (0 == pOrgCfg->flag[AMSORGCFG_ITEM_ORGID])
    {
        *pResultCode = OA_FAIL_PARA_MISS;
         OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsOrgCfgParaName[AMSORGCFG_ITEM_ORGID],NULL);            
        return OA_RET_FAIL;    
    }

    //检查参数范围
    if ((i = AmsCheckOAOrgCfgValue(pOrgCfg)) <  AMSORGCFG_ITEM_MAX)
    {
        *pResultCode = OA_FAIL_PARAVAL_INV;
        OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsOrgCfgParaName[i],NULL);
        return OA_RET_FAIL;        
    }

    //检查org是否已配置
	for(i = 0; i < AMS_MAX_ORG_NUM; i++)
	{
		if(AmsCfgOrg(i).orgId == pOrgCfg->orgId && AmsCfgOrg(i).flag == AMS_ORG_INSTALL)
		{
            *pResultCode = AMS_OAREASON_ORG_ADDED_BEFORE;
            reasonstr = OmsagentGetReasonStr(*pResultCode);
            OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));                    
            return OA_RET_FAIL;     
		}
	}

    //查找空闲项并记录
	for(i = 0; i < AMS_MAX_ORG_NUM; i++)
	{
		if(AMS_ORG_UNINSTALL == AmsCfgOrg(i).flag)
		{
		    if (NULL == (pNode = AmsGetOrgIdNode())) //满
              break;
            pNode->orgId = pOrgCfg->orgId;
            pNode->orgPos = i;
            AmsInsertOrgIdHash(pNode);

		    memset(&AmsCfgOrg(i), 0, sizeof(ORG_INFO));
    		AmsCfgOrg(i).orgId = pOrgCfg->orgId;
    		AmsCfgOrg(i).flag = AMS_ORG_INSTALL;


            AmsSetOrgInfoFromOA(i, pOrgCfg);
            dbgprint("orgId=%u, code=%s, name=%s, type=%u, level=%u, porgid=%u, state=%u",
                AmsCfgOrg(i).orgId,AmsCfgOrg(i).orgCode,AmsCfgOrg(i).orgName,
                AmsCfgOrg(i).orgType,AmsCfgOrg(i).orgLevel,
                AmsCfgOrg(i).parentOrgId,AmsCfgOrg(i).orgState);

            *pResultCode = OA_FAIL_NONE;
            OmsagentCopystr(pResultDescription, descriptionBufLen-1, OKStr, strlen(OKStr));
			return OA_RET_SUC;
		}
	}

    *pResultCode = AMS_OAREASON_ORG_FULL;
    reasonstr = OmsagentGetReasonStr(*pResultCode);
    OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));       
    return OA_RET_FAIL;
}

int AmsModifyOrg(OA_AMS_ORG_CONFIG_t *pOrgCfg, int *pResultCode, char *pResultDescription, int descriptionBufLen)
{
    unsigned int i;
    char *reasonstr;

    //检查函数入参
    if (!pOrgCfg || !pResultCode || !pResultDescription)
    {
        if (pResultCode)
        {
            *pResultCode = OA_FAIL_UNKNOWN_ERROR;
        }
        return OA_RET_FAIL;
    }

    //检查必备参数
    if (0 == pOrgCfg->flag[AMSORGCFG_ITEM_ORGID])
    {
        *pResultCode = OA_FAIL_PARA_MISS;
         OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsOrgCfgParaName[AMSORGCFG_ITEM_ORGID],NULL);            
        return OA_RET_FAIL;    
    }

    //检查参数范围
    if ((i = AmsCheckOAOrgCfgValue(pOrgCfg)) <  AMSORGCFG_ITEM_MAX)
    {
        *pResultCode = OA_FAIL_PARAVAL_INV;
        OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsOrgCfgParaName[i],NULL);
        return OA_RET_FAIL;        
    }

    //找到org项修改配置
	for(i = 0; i < AMS_MAX_ORG_NUM; i++)
	{
		if(AmsCfgOrg(i).orgId == pOrgCfg->orgId && AmsCfgOrg(i).flag == AMS_ORG_INSTALL)
		{
		    AmsSetOrgInfoFromOA(i, pOrgCfg);
            
            *pResultCode = OA_FAIL_NONE;
            OmsagentCopystr(pResultDescription, descriptionBufLen-1, OKStr, strlen(OKStr));
			return OA_RET_SUC;
		}
	}

    *pResultCode = AMS_OAREASON_ORG_NOT_EXIST;
    reasonstr = OmsagentGetReasonStr(*pResultCode);
    OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));       
    return OA_RET_FAIL;
}

int AmsDeleteOrg(OA_AMS_ORG_CONFIG_t *pOrgCfg, int *pResultCode, char *pResultDescription, int descriptionBufLen)
{
    unsigned int i;
    char *reasonstr;
    ORG_ID_NODE *pNode;

    //检查函数入参
    if (!pOrgCfg || !pResultCode || !pResultDescription)
    {
        if (pResultCode)
        {
            *pResultCode = OA_FAIL_UNKNOWN_ERROR;
        }
        return OA_RET_FAIL;
    }

    //检查必备参数
    if (0 == pOrgCfg->flag[AMSORGCFG_ITEM_ORGID])
    {
        *pResultCode = OA_FAIL_PARA_MISS;
         OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsOrgCfgParaName[AMSORGCFG_ITEM_ORGID],NULL);            
        return OA_RET_FAIL;    
    }

    //检查参数范围

    //找到org项删除
	for(i = 0; i < AMS_MAX_ORG_NUM; i++)
	{
		if(AmsCfgOrg(i).orgId == pOrgCfg->orgId && AmsCfgOrg(i).flag == AMS_ORG_INSTALL)
		{
		    if (NULL != (pNode = AmsSearchOrgIdHash(AmsCfgOrg(i).orgId)))
    		{
    			AmsDeleteOrgIdHash(pNode);
    			AmsFreeOrgIdNode(pNode);
    		}
		    memset(&AmsCfgOrg(i), 0,sizeof(AmsCfgOrg(i)));
            
            *pResultCode = OA_FAIL_NONE;
            OmsagentCopystr(pResultDescription, descriptionBufLen-1, OKStr, strlen(OKStr));
			return OA_RET_SUC;
		}
	}

    *pResultCode = AMS_OAREASON_ORG_NOT_EXIST;
    reasonstr = OmsagentGetReasonStr(*pResultCode);
    OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));       
    return OA_RET_FAIL;
}

int AmsGetVtaTermPara(OA_AMS_TERMPARA_CONFIG_t *termPara, int *pResultCode, 
    char *pResultDescription, int descriptionBufLen,
    oa_termpara_cbfun cbFunc,unsigned char *cbpara,int cbparalen)
{
	LP_AMS_DATA_t		*lpAmsData = NULL;         //进程数据区指针	
	unsigned int        vtaNum = 0;
    VTA_NODE            *pVtaNode = NULL;
	unsigned char       timerPara[AMS_MAX_TIMER_PARA_LEN];	
	unsigned char       *p;	
	int                 pid = 0;		
    MESSAGE_t           msg;
    int                 len;
    unsigned int        i;
	unsigned int        j;
    char                *reasonstr = NULL;
    unsigned int        exist = 0;
	
    //检查函数入参
    if (!termPara || !pResultCode || !pResultDescription || cbparalen > OA_MAX_CBPARA_LEN)
    {
        if (pResultCode)
        {
            *pResultCode = OA_FAIL_UNKNOWN_ERROR;
        }
        return OA_RET_FAIL;
    }

    //检查必备参数
    if (0 == termPara->flag[AMSTERMPARACFG_ITEM_TERMID])
    {
        *pResultCode = OA_FAIL_PARA_MISS;
         OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsVtaTermParaName[AMSTERMPARACFG_ITEM_TERMID],NULL);            
        return OA_RET_FAIL;    
    }

    //检查参数范围
    if (termPara->TermId < 1)
    {
        *pResultCode = OA_FAIL_PARAVAL_INV;
         OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsVtaTermParaName[AMSTERMPARACFG_ITEM_TERMID],NULL);            
        return OA_RET_FAIL;        
    }
    
    //检查teller是否已存在
    exist = 0;
    vtaNum = Min(SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NUM);
	for(i = 0; i < vtaNum; i++)
	{
		if(AmsCfgTeller(i).tellerId == termPara->TermId && AmsCfgTeller(i).flag == AMS_TELLER_INSTALL)
		{
		exist = 1;
		    //检查teller是否在线
        	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
        	{
        		if ((pVtaNode = AmsSearchVtaNode(j, termPara->TermId)) != NULL)
        		{
                    break;
        		}
        	}
		}
        if (NULL != pVtaNode)
            break;
	}
    if (NULL == pVtaNode)
    {
        if  (0 == exist)
            *pResultCode = AMS_OAREASON_TELLER_NOT_EXIST;
        else
            *pResultCode = AMS_OAREASON_TELLER_OFFLINE;
        reasonstr = OmsagentGetReasonStr(*pResultCode);
        OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));          
        return OA_RET_FAIL;     
    }

	pid = pVtaNode->amsPid & 0xffff;
	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
            *pResultCode = OA_FAIL_EXEC_FAIL;
             reasonstr = OmsagentGetReasonStr(*pResultCode);
            OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));   
		return OA_RET_FAIL;
	}

	lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
	
	if(lpAmsData->amsPid != pVtaNode->amsPid ||
	   lpAmsData->myPid.iProcessId != pid ||
	   lpAmsData->tellerId != pVtaNode->vtaInfo.tellerId)
	{
	    *pResultCode = OA_FAIL_EXEC_FAIL;
            reasonstr = OmsagentGetReasonStr(*pResultCode);
            OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));   
		return OA_RET_FAIL;
	}
	
	//check vtaParaCfgState
	if(lpAmsData->vtaParaCfgState != AMS_TERM_PARA_CFG_NULL || pVtaNode->oaCbPara.flag != 0)
	{
	    *pResultCode = AMS_OAREASON_TELLER_PROCESSING;
           reasonstr = OmsagentGetReasonStr(*pResultCode);
           OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));   
		return OA_RET_FAIL;		
	}
	
	//wait vta para cfg Ind cnf,default: 100s
    if(AmsCfgData.amsVtaParaCfgTimeLength > 0 
	    && AmsCfgData.amsVtaParaCfgTimeLength <= T_AMS_VTA_PARA_CFG_TIMER_LENGTH_MAX)
    {
		memset(timerPara, 0, PARA_LEN);
		p = timerPara;
		
		BEPUTSHORT(AMS_QUERY_TERM_CONFIG_PARA, p);
		
        if(AMS_OK != AmsCreateTimerPara(lpAmsData->myPid.iProcessId,
				                        &lpAmsData->vtaParaCfgTimerId, 
										B_AMS_VTA_PARA_CFG_TIMEOUT, 
										AmsCfgData.amsVtaParaCfgTimeLength,
										timerPara))
        {
	     *pResultCode = OA_FAIL_EXEC_FAIL;
             reasonstr = OmsagentGetReasonStr(*pResultCode);
            OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));   
			return OA_RET_FAIL;
        }

		AmsTimerStatProc(T_AMS_VTA_PARA_CFG_TIMER, AMS_CREATE_TIMER);

        if(lpAmsData->commonTrace)
        {
            dbgprint("Ams[%d] Create T_AMS_VTA_PARA_CFG_TIMER Timer:timerId=%d.",
				lpAmsData->myPid.iProcessId, lpAmsData->vtaParaCfgTimerId);
        }
    }	

	//update vtaParaCfgState
	lpAmsData->vtaParaCfgState = AMS_TERM_PARA_CFG_STARTING;

    //向终端发送查询请求
    if (AMS_SUCCESS!= AmsSendVtaParaCfgInd(pVtaNode, &msg, termPara, NULL, NULL, 0, CONFIGIND_GET_TERMPARA))
    {
         *pResultCode = OA_FAIL_EXEC_FAIL;
         reasonstr = OmsagentGetReasonStr(*pResultCode);
        OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));               

		/* 杀掉定时器 */
		if(lpAmsData->vtaParaCfgTimerId >= 0)
		{
		    AmsKillTimer(pid, &lpAmsData->vtaParaCfgTimerId);
			AmsTimerStatProc(T_AMS_VTA_PARA_CFG_TIMER, AMS_KILL_TIMER);
		}
		
        return OA_RET_FAIL;     
    }

    //记录回调参数
    pVtaNode->oaCbPara.flag = 1;
    pVtaNode->oaCbPara.configInd = CONFIGIND_GET_TERMPARA;
    pVtaNode->oaCbPara.cbFunc = cbFunc;
    memcpy(pVtaNode->oaCbPara.para,cbpara,cbparalen);

    return OA_RET_PROCESSING;        

}

int AmsModifyVtaTermPara(OA_AMS_TERMPARA_CONFIG_t *termPara, int *pResultCode, 
    char *pResultDescription, int descriptionBufLen,
    oa_termpara_cbfun cbFunc,unsigned char *cbpara,int cbparalen)
{
	LP_AMS_DATA_t		*lpAmsData = NULL;         //进程数据区指针	
    int                 len;
    unsigned int        i;
	unsigned int        j;
	unsigned int        vtaNum = 0;
    VTA_NODE            *pVtaNode = NULL;
	unsigned char       timerPara[AMS_MAX_TIMER_PARA_LEN];	
	unsigned char       *p;	
	int                 pid = 0;		
    MESSAGE_t           msg;
    char                *reasonstr = NULL;
    unsigned int        exist = 0;

    //检查函数入参
    if (!termPara || !pResultCode || !pResultDescription || cbparalen > OA_MAX_CBPARA_LEN)
    {
        if (pResultCode)
        {
            *pResultCode = OA_FAIL_UNKNOWN_ERROR;
        }
        return OA_RET_FAIL;
    }

    //检查必备参数
    for (i = 0; i < AMSTERMPARACFG_ITEM_MAX; i++)
    {
        if (0 == termPara->flag[i])
        {
            *pResultCode = OA_FAIL_PARA_MISS;
            OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsVtaTermParaName[i],NULL);            
            return OA_RET_FAIL;    
        }
    }

    //检查参数范围 除TermId外其余参数透传不判断
    if (termPara->TermId <  1)
    {
        *pResultCode = OA_FAIL_PARAVAL_INV;
            OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsVtaTermParaName[AMSTERMPARACFG_ITEM_TERMID],NULL);            
        return OA_RET_FAIL;        
    }
    
    //检查teller是否已存在
    exist = 0;
    vtaNum = Min(SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NUM);
	for(i = 0; i < vtaNum; i++)
	{
		if(AmsCfgTeller(i).tellerId == termPara->TermId && AmsCfgTeller(i).flag == AMS_TELLER_INSTALL)
		{
		    exist = 1;
		    //检查teller是否在线
        	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
        	{
        		if ((pVtaNode = AmsSearchVtaNode(j, termPara->TermId)) != NULL)
        		{
                    break;
        		}
        	}
		}
        if (NULL != pVtaNode)
            break;
	}
    if (NULL == pVtaNode)
    {
        if  (0 == exist)
            *pResultCode = AMS_OAREASON_TELLER_NOT_EXIST;
        else
            *pResultCode = AMS_OAREASON_TELLER_OFFLINE;
        reasonstr = OmsagentGetReasonStr(*pResultCode);
        OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr)); 
        return OA_RET_FAIL;     
    }

	pid = pVtaNode->amsPid & 0xffff;
	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		*pResultCode = OA_FAIL_EXEC_FAIL;
            reasonstr = OmsagentGetReasonStr(*pResultCode);
            OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));   
		return OA_RET_FAIL;
	}

	lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
	
	if(lpAmsData->amsPid != pVtaNode->amsPid ||
	   lpAmsData->myPid.iProcessId != pid ||
	   lpAmsData->tellerId != pVtaNode->vtaInfo.tellerId)
	{
		*pResultCode = OA_FAIL_EXEC_FAIL;
            reasonstr = OmsagentGetReasonStr(*pResultCode);
            OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));   
		return OA_RET_FAIL;
	}
	
	//check vtaParaCfgState
	if(lpAmsData->vtaParaCfgState != AMS_TERM_PARA_CFG_NULL || pVtaNode->oaCbPara.flag != 0)
	{
	    *pResultCode = AMS_OAREASON_TELLER_PROCESSING;
            reasonstr = OmsagentGetReasonStr(*pResultCode);
            OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));               
		return OA_RET_FAIL;		
	}
	
	//wait vta para cfg Ind cnf,default: 10s
    if(AmsCfgData.amsVtaParaCfgTimeLength > 0 
	    && AmsCfgData.amsVtaParaCfgTimeLength <= T_AMS_VTA_PARA_CFG_TIMER_LENGTH_MAX)
    {
		memset(timerPara, 0, PARA_LEN);
		p = timerPara;
		
		BEPUTSHORT(AMS_CONFIG_TERM_PARA, p);
		
        if(AMS_OK != AmsCreateTimerPara(lpAmsData->myPid.iProcessId,
				                        &lpAmsData->vtaParaCfgTimerId, 
										B_AMS_VTA_PARA_CFG_TIMEOUT, 
										AmsCfgData.amsVtaParaCfgTimeLength,
										timerPara))
        {
    		*pResultCode = OA_FAIL_EXEC_FAIL;
            reasonstr = OmsagentGetReasonStr(*pResultCode);
            OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));
			return OA_RET_FAIL;
        }

		AmsTimerStatProc(T_AMS_VTA_PARA_CFG_TIMER, AMS_CREATE_TIMER);

        if(lpAmsData->commonTrace)
        {
            dbgprint("Ams[%d] Create T_AMS_VTA_PARA_CFG_TIMER Timer:timerId=%d.",
				lpAmsData->myPid.iProcessId, 
				lpAmsData->vtaParaCfgTimerId);
        }
    }	

	//update vtaParaCfgState
	lpAmsData->vtaParaCfgState = AMS_TERM_PARA_CFG_STARTING;
	
    //向终端发送更新请求
    if (AMS_SUCCESS!= AmsSendVtaParaCfgInd(pVtaNode, &msg, termPara, NULL, NULL, 0, CONFIGIND_UPDATE_TERMPARA))
    {
        *pResultCode = OA_FAIL_EXEC_FAIL;
        reasonstr = OmsagentGetReasonStr(*pResultCode);
        OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));

		/* 杀掉定时器 */
		if(lpAmsData->vtaParaCfgTimerId >= 0)
		{
		    AmsKillTimer(pid, &lpAmsData->vtaParaCfgTimerId);
			AmsTimerStatProc(T_AMS_VTA_PARA_CFG_TIMER, AMS_KILL_TIMER);
		}
				
        return OA_RET_FAIL;     
    }

    //记录回调参数
    pVtaNode->oaCbPara.flag = 1;
    pVtaNode->oaCbPara.configInd = CONFIGIND_UPDATE_TERMPARA;
    pVtaNode->oaCbPara.cbFunc = cbFunc;
    memcpy(pVtaNode->oaCbPara.para,cbpara,cbparalen);

    return OA_RET_PROCESSING;        
}

int AmsGetVtmTermPara(OA_AMS_TERMPARA_CONFIG_t *termPara, int *pResultCode, 
    char *pResultDescription, int descriptionBufLen,
    oa_termpara_cbfun cbFunc,unsigned char *cbpara,int cbparalen)
{
    int                 len;
    unsigned int        i;
	unsigned int        j;
	unsigned int        vtmNum = 0;	
    VTM_NODE            *pVtmNode = NULL;
	unsigned char       timerPara[AMS_MAX_TIMER_PARA_LEN];	
	unsigned char       *p;
    MESSAGE_t           msg;
    char                *reasonstr = NULL;
    unsigned int        exist = 0;

    //检查函数入参
    if (!termPara || !pResultCode || !pResultDescription || cbparalen > OA_MAX_CBPARA_LEN)
    {
        if (pResultCode)
        {
            *pResultCode = OA_FAIL_UNKNOWN_ERROR;
        }
        return OA_RET_FAIL;
    }

    //检查必备参数
    if (0 == termPara->flag[AMSTERMPARACFG_ITEM_TERMID])
    {
        *pResultCode = OA_FAIL_PARA_MISS;
         OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsVtmTermParaName[AMSTERMPARACFG_ITEM_TERMID],NULL);            
        return OA_RET_FAIL;    
    }

    //检查参数范围
    if (termPara->TermId <  1)
    {
        *pResultCode = OA_FAIL_PARAVAL_INV;
         OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsVtmTermParaName[AMSTERMPARACFG_ITEM_TERMID],NULL);            
        return OA_RET_FAIL;        
    }
    
    //检查vtm是否已存在
    exist = 0;
    vtmNum = Min(SystemData.AmsPriData.amsCfgData.maxVtmNum, AMS_MAX_VTM_NUM);
	for(i = 0; i < vtmNum; i++)
	{
		if(AmsCfgVtm(i).vtmId == termPara->TermId && AMS_VTM_INSTALL == AmsCfgVtm(i).flag)
		{
		    exist = 1;
		    //检查vtm是否在线
        	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
        	{
        		if ((pVtmNode = AmsSearchVtmNode(j, termPara->TermId)) != NULL)
        		{
                    break;
        		}
        	}
		}
        if (NULL != pVtmNode)
            break;
	}
    
    if (NULL == pVtmNode)
    {
         if  (0 == exist)
            *pResultCode = AMS_OAREASON_VTM_NOT_EXIST;
        else
            *pResultCode = AMS_OAREASON_VTM_OFFLINE;
        reasonstr = OmsagentGetReasonStr(*pResultCode);
        OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr)); 
        return OA_RET_FAIL;     
    }

	//check vtmParaCfgState
	if(pVtmNode->vtmParaCfgState != AMS_TERM_PARA_CFG_NULL || pVtmNode->oaCbPara.flag != 0)
	{
	    *pResultCode = AMS_OAREASON_VTM_PROCESSING;
             reasonstr = OmsagentGetReasonStr(*pResultCode);
            OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));             
	}
	
	//wait par cfg Ind cnf,default: 100s
    if(AmsCfgData.amsVtmParaCfgTimeLength > 0 
	    && AmsCfgData.amsVtmParaCfgTimeLength <= T_AMS_VTM_PARA_CFG_TIMER_LENGTH_MAX)
    {
		memset(timerPara, 0, PARA_LEN);
		p = timerPara;
		
		BEPUTSHORT(AMS_QUERY_TERM_CONFIG_PARA, p);
		p += 2;
		BEPUTLONG(termPara->TermId, p);	
		
        if(AMS_OK != AmsCreateTimerPara(pVtmNode->amsPid&0xffff, //not used yet
				                        &pVtmNode->vtmParaCfgTimerId, 
										B_AMS_VTM_PARA_CFG_TIMEOUT, 
										AmsCfgData.amsVtmParaCfgTimeLength,
										timerPara))
        {
	    *pResultCode = OA_FAIL_EXEC_FAIL;
             reasonstr = OmsagentGetReasonStr(*pResultCode);
            OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));             
			return OA_RET_FAIL;
        }

		AmsTimerStatProc(T_AMS_VTM_PARA_CFG_TIMER, AMS_CREATE_TIMER);

        if(AmsCommonTrace)
        {
            dbgprint("Ams[%u] Create T_AMS_VTM_PARA_CFG_TIMER Timer:timerId=%d.",
				pVtmNode->vtmInfo.vtmId, pVtmNode->vtmParaCfgTimerId);
        }
    }	
	
	//update vtmParaCfgState
	pVtmNode->vtmParaCfgState = AMS_TERM_PARA_CFG_STARTING;    
			
    //向终端发送查询请求
    if (AMS_SUCCESS!= AmsSendVtmParaCfgInd(pVtmNode, &msg, termPara, NULL, NULL, 0, CONFIGIND_GET_TERMPARA))
    {
        *pResultCode = OA_FAIL_EXEC_FAIL;
         reasonstr = OmsagentGetReasonStr(*pResultCode);
        OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr));

		/* 杀掉定时器 */
		if(pVtmNode->vtmParaCfgTimerId >= 0)
		{
		    AmsKillTimer(pVtmNode->amsPid&0xffff, &pVtmNode->vtmParaCfgTimerId);
			AmsTimerStatProc(T_AMS_VTM_PARA_CFG_TIMER, AMS_KILL_TIMER);
		} 
		
        return OA_RET_FAIL;     
    }		

    //记录回调参数
    pVtmNode->oaCbPara.flag = 1;
    pVtmNode->oaCbPara.configInd = CONFIGIND_GET_TERMPARA;
    pVtmNode->oaCbPara.cbFunc = cbFunc;
    memcpy(pVtmNode->oaCbPara.para,cbpara,cbparalen);

    return OA_RET_PROCESSING;        

}


int AmsModifyVtmTermPara(OA_AMS_TERMPARA_CONFIG_t *termPara, int *pResultCode, 
    char *pResultDescription, int descriptionBufLen,
    oa_termpara_cbfun cbFunc,unsigned char *cbpara,int cbparalen)
{
    int                 len;
    unsigned int        i;
	unsigned int        j;
	unsigned int        vtmNum = 0;	
    VTM_NODE            *pVtmNode = NULL;
	unsigned char       timerPara[AMS_MAX_TIMER_PARA_LEN];	
	unsigned char       *p;
    MESSAGE_t           msg;
    char                *reasonstr = NULL;
    unsigned int        exist = 0;

    //检查函数入参
    if (!termPara || !pResultCode || !pResultDescription || cbparalen > OA_MAX_CBPARA_LEN)
    {
        if (pResultCode)
        {
            *pResultCode = OA_FAIL_UNKNOWN_ERROR;
        }
        return OA_RET_FAIL;
    }

    //检查必备参数
    for (i = 0; i < AMSTERMPARACFG_ITEM_MAX; i++)
    {
        if (0 == termPara->flag[i])
        {
            *pResultCode = OA_FAIL_PARA_MISS;
             OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsVtmTermParaName[i],NULL);            
            return OA_RET_FAIL;    
        }
    }

    //检查参数范围 除TermId外其余参数透传不判断
    if (termPara->TermId <  1)
    {
        *pResultCode = OA_FAIL_PARAVAL_INV;
         OmsagentCopy3Descrip(pResultDescription, descriptionBufLen, OmsagentGetReasonStr(*pResultCode), amsVtmTermParaName[AMSTERMPARACFG_ITEM_TERMID],NULL);            
        return OA_RET_FAIL;        
    }
    
    //检查vtm是否已存在
    exist = 0;
    vtmNum = Min(SystemData.AmsPriData.amsCfgData.maxVtmNum, AMS_MAX_VTM_NUM);
	for(i = 0; i < vtmNum; i++)
	{
		if(AmsCfgVtm(i).vtmId == termPara->TermId && AMS_VTM_INSTALL == AmsCfgVtm(i).flag)
		{
		    exist = 1;
		    //检查vtm是否在线
        	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
        	{
        		if ((pVtmNode = AmsSearchVtmNode(j, termPara->TermId)) != NULL)
        		{
                    break;
        		}
        	}
		}
        if (NULL != pVtmNode)
            break;
	}
    
    if (NULL == pVtmNode)
    {
        if  (0 == exist)
            *pResultCode = AMS_OAREASON_VTM_NOT_EXIST;
        else
            *pResultCode = AMS_OAREASON_VTM_OFFLINE;
        reasonstr = OmsagentGetReasonStr(*pResultCode);
        OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr)); 
        return OA_RET_FAIL;     
    }

	//check vtmParaCfgState
	if(pVtmNode->vtmParaCfgState != AMS_TERM_PARA_CFG_NULL || pVtmNode->oaCbPara.flag != 0)
	{
            *pResultCode = AMS_OAREASON_VTM_PROCESSING;
            reasonstr = OmsagentGetReasonStr(*pResultCode);
            OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr)); 
		return OA_RET_FAIL;		
	}
	
	//wait par cfg Ind cnf,default: 100s
    if(AmsCfgData.amsVtmParaCfgTimeLength > 0 
	    && AmsCfgData.amsVtmParaCfgTimeLength <= T_AMS_VTM_PARA_CFG_TIMER_LENGTH_MAX)
    {
		memset(timerPara, 0, PARA_LEN);
		p = timerPara;
		
		BEPUTSHORT(AMS_CONFIG_TERM_PARA, p);
		p += 2;
		BEPUTLONG(termPara->TermId, p);	
		
        if(AMS_OK != AmsCreateTimerPara(pVtmNode->amsPid&0xffff, //not used yet
				                        &pVtmNode->vtmParaCfgTimerId, 
										B_AMS_VTM_PARA_CFG_TIMEOUT, 
										AmsCfgData.amsVtmParaCfgTimeLength,
										timerPara))
        {
            *pResultCode = OA_FAIL_EXEC_FAIL;
            reasonstr = OmsagentGetReasonStr(*pResultCode);
            OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr)); 
			return OA_RET_FAIL;
        }

		AmsTimerStatProc(T_AMS_VTM_PARA_CFG_TIMER, AMS_CREATE_TIMER);

        if(AmsCommonTrace)
        {
            dbgprint("Ams[%u] Create T_AMS_VTM_PARA_CFG_TIMER Timer:timerId=%d.",
				pVtmNode->vtmInfo.vtmId, pVtmNode->vtmParaCfgTimerId);
        }
    }	
	
	//update vtmParaCfgState
	pVtmNode->vtmParaCfgState = AMS_TERM_PARA_CFG_STARTING;    

	//向终端发送更新请求
    if (AMS_SUCCESS!= AmsSendVtmParaCfgInd(pVtmNode, &msg, termPara, NULL, NULL, 0, CONFIGIND_UPDATE_TERMPARA))
    {
        *pResultCode = OA_FAIL_EXEC_FAIL;
        reasonstr = OmsagentGetReasonStr(*pResultCode);
        OmsagentCopystr(pResultDescription, descriptionBufLen-1, reasonstr, strlen(reasonstr)); 

		/* 杀掉定时器 */
		if(pVtmNode->vtmParaCfgTimerId >= 0)
		{
		    AmsKillTimer(pVtmNode->amsPid&0xffff, &pVtmNode->vtmParaCfgTimerId);
			AmsTimerStatProc(T_AMS_VTM_PARA_CFG_TIMER, AMS_KILL_TIMER);
		}
		
        return OA_RET_FAIL;     
    }

    //记录回调参数
    pVtmNode->oaCbPara.flag = 1;
    pVtmNode->oaCbPara.configInd = CONFIGIND_UPDATE_TERMPARA;
    pVtmNode->oaCbPara.cbFunc = cbFunc;
    memcpy(pVtmNode->oaCbPara.para,cbpara,cbparalen);

    return OA_RET_PROCESSING;        
}


int AmsParaCfgTimeoutProc(int iThreadId, TIMEMESSAGE_t *pTmMsg)
{
	int					iret = AMS_PARA_CFG_INF_CNF_TIMEOUT;
	LP_AMS_DATA_t		*lpAmsData = NULL;         //进程数据区指针	
	VTA_NODE            *pVtaNode = NULL;	
	VTM_NODE            *pVtmNode = NULL;	
	unsigned int        vtaNum = 0;
	unsigned int        vtmNum = 0;		
	int                 pid = 0;
	unsigned int        tellerId = 0;	
	unsigned short      configInd = AMS_PARA_CONFIG_RSVD;	
	unsigned short      configState = AMS_TERM_PARA_CFG_NULL;		
	unsigned char       timerPara[AMS_MAX_TIMER_PARA_LEN];	
	int                 cfgNum = 0;			
	unsigned short      termType = 0;
	unsigned int        termPos = 0;
	unsigned int        curTermType = 0;
	unsigned int        curTermPos = 0;
	unsigned char       *p;
	MESSAGE_t           msg;
	unsigned int        i = 0;
	unsigned int        j = 0;
	
	if(AmsMsgTrace)
	{
		unsigned char description [1024];
		int descrlen;
		memset(description,0,sizeof(description));
		descrlen=snprintf(description,1024,"recv B_AMS_PARA_CFG_TIMEOUT msg[%d] \n",pTmMsg->iTimerId);	
		AmsTraceToFile(pTmMsg->s_ReceiverPid,pTmMsg->s_SenderPid,"B_AMS_PARA_CFG_TIMEOUT",description,
						descrlen,pTmMsg->cTimerParameter,PARA_LEN,"ams");
	}
	
	//进程号有效性检查
	pid = pTmMsg->s_ReceiverPid.iProcessId;
/*	if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
	{
		dbgprint("AmsParaCfgTimeoutProc Pid:%d Err", pid);
		return AMS_ERROR;
	}*/

	//消息长度检查
	if(pTmMsg->iMessageLength > (PARA_LEN + sizeof(char) + sizeof(int)))
	{
		dbgprint("AmsParaCfgTimeoutProc[%d] Len:%d Err", pid, pTmMsg->iMessageLength);
		return AMS_ERROR;
	}
	
	/* 杀掉定时器 */
	if(AmsCfgData.paraCfgTimerId >= 0)
	{
	    AmsKillTimer(pid, &AmsCfgData.paraCfgTimerId);
		AmsTimerStatProc(T_AMS_PARA_CFG_TIMER, AMS_KILL_TIMER);
		pTmMsg->iTimerId = -1;		
	} 
	
	//监控操作码检查
	p = pTmMsg->cTimerParameter;
	
	BEGETSHORT(configInd, p);
	if(configInd < AMS_CONFIG_FILE_SERVER_PARA || configInd >= AMS_PARA_CONFIG_MAX)
	{
		dbgprint("AmsParaCfgTimeoutProc[%d] ConfigInd[%d]Err", pid, configInd);
		return AMS_ERROR;	
	}
	p += 2;
	
	BEGETLONG(termType, p);
	if(termType < AMS_OPERATE_TARGET_VTA || termType >= AMS_OPERATE_TARGET_MAX)
	{
		dbgprint("AmsParaCfgTimeoutProc[%d] TermType[%d]Err", pid, termType);
		return AMS_ERROR;	
	}
	p += 4;

	if(AMS_OPERATE_TARGET_VTA == termType)
	{
		vtaNum = Min(SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NUM);
		
		BEGETLONG(termPos, p);
		if(termPos >= vtaNum)
		{
			dbgprint("AmsParaCfgTimeoutProc[%d] ConfigInd[%d]TermType[%d]TermPos[%d][%d]Err", 
				pid, configInd, termType, termPos, vtaNum);
			return AMS_ERROR;	
		}
		p += 4;	
	}
	else//(AMS_OPERATE_TARGET_VTM == termType)
	{
	    vtmNum = Min(SystemData.AmsPriData.amsCfgData.maxVtmNum, AMS_MAX_VTM_NUM);
		
		BEGETLONG(termPos, p);
		if(termPos >= vtmNum)
		{
			dbgprint("AmsParaCfgTimeoutProc[%d] ConfigInd[%d]TermType[%d]TermPos[%d][%d]Err", 
				pid, configInd, termType, termPos, vtmNum);
			return AMS_ERROR;	
		}
		p += 4;	
	}
	
	if(AMS_OPERATE_TARGET_VTA == termType)
	{
	    //检查teller是否已存在
		for(i = termPos; i < vtaNum; i++)
		{
			if(AmsCfgTeller(i).flag != AMS_TELLER_INSTALL)
			{
				continue;
			}

		    //检查teller是否在线
        	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
        	{
        		if (NULL == (pVtaNode = AmsSearchVtaNode(j, AmsCfgTeller(i).tellerId)))
        		{
					continue;
        		}

				pid = pVtaNode->amsPid & 0xffff;
				if((0 == pid) || (pid >= LOGIC_PROCESS_SIZE))
				{
					//...
					continue;
				}

				lpAmsData=(LP_AMS_DATA_t *)ProcessData[pid];
				
				if(lpAmsData->amsPid != pVtaNode->amsPid ||
				   lpAmsData->myPid.iProcessId != pid ||
				   lpAmsData->tellerId != pVtaNode->vtaInfo.tellerId)
				{
					//...
					continue;
				}
				
				//check vtaParaCfgState
				if(lpAmsData->vtaParaCfgState != AMS_TERM_PARA_CFG_NULL)
				{

					//...
					continue;	
				}
				
				//wait vta para cfg Ind cnf,default: 10s
			    if(AmsCfgData.amsVtaParaCfgTimeLength > 0 
				    && AmsCfgData.amsVtaParaCfgTimeLength <= T_AMS_VTA_PARA_CFG_TIMER_LENGTH_MAX)
			    {
					memset(timerPara, 0, PARA_LEN);
					p = timerPara;
					
					BEPUTSHORT(AMS_CONFIG_FILE_SERVER_PARA, p);
					
			        if(AMS_OK != AmsCreateTimerPara(lpAmsData->myPid.iProcessId,
							                        &lpAmsData->vtaParaCfgTimerId, 
													B_AMS_VTA_PARA_CFG_TIMEOUT, 
													AmsCfgData.amsVtaParaCfgTimeLength,
													timerPara))
			        {
						//...
						return AMS_ERROR;
			        }

					AmsTimerStatProc(T_AMS_VTA_PARA_CFG_TIMER, AMS_CREATE_TIMER);

			        if(lpAmsData->commonTrace)
			        {
			            dbgprint("Ams[%d] Create T_AMS_VTA_PARA_CFG_TIMER Timer:timerId=%d.",
							lpAmsData->myPid.iProcessId, lpAmsData->vtaParaCfgTimerId);
			        }
			    }	

				//update vtaParaCfgState
				lpAmsData->vtaParaCfgState = AMS_TERM_PARA_CFG_STARTING;


			    //向终端发送查询请求
			    if (AMS_SUCCESS != AmsSendVtaParaCfgInd(pVtaNode, &msg, NULL, &AmsFileServerPara, NULL, 0, CONFIGIND_UPDATE_FILESERVER))
			    {			
//			        *pResultCode = EXECESULT_FAIL;
//			        OmsagentCopystr(pResultDescription, descriptionBufLen-1, FailStr, strlen(FailStr));

					cfgNum++;						
					if(cfgNum >= AMS_ALLOWED_TERM_ID_NO_NUM)
					{								
						break;
					}
					
			        continue;     
			    }

				cfgNum++;
				if(cfgNum >= AMS_ALLOWED_TERM_ID_NO_NUM)
				{
					break;
				}
        	}	
		
			if(cfgNum >= AMS_ALLOWED_TERM_ID_NO_NUM)
			{
				break;
			}
			
		}

		curTermType = AMS_OPERATE_TARGET_VTA;
		curTermPos = i;	
			
		if(cfgNum < AMS_ALLOWED_TERM_ID_NO_NUM)
		{
		 	//检查vtm是否已存在
			for(i = 0; i < vtmNum; i++)
			{
				if(AmsCfgVtm(i).flag != AMS_VTM_INSTALL)
				{
					continue;
				}
				
			    //检查vtm是否在线
	        	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
	        	{
	        		if (NULL == (pVtmNode = AmsSearchVtmNode(j, AmsCfgVtm(i).vtmId)))
	        		{
						continue;
	        		}
					
					//check vtmParaCfgState
					if(pVtmNode->vtmParaCfgState != AMS_TERM_PARA_CFG_NULL)
					{

						//...
						continue;	
					}
					
					//wait par cfg Ind cnf,default: 100s
				    if(AmsCfgData.amsVtmParaCfgTimeLength > 0 
					    && AmsCfgData.amsVtmParaCfgTimeLength <= T_AMS_VTM_PARA_CFG_TIMER_LENGTH_MAX)
				    {
						memset(timerPara, 0, PARA_LEN);
						p = timerPara;
						
						BEPUTSHORT(AMS_CONFIG_FILE_SERVER_PARA, p);
						p += 2;
						BEPUTLONG(AmsCfgVtm(i).vtmId, p);	
						
				        if(AMS_OK != AmsCreateTimerPara(pVtmNode->amsPid&0xffff, //not used yet
								                        &pVtmNode->vtmParaCfgTimerId, 
														B_AMS_VTM_PARA_CFG_TIMEOUT, 
														AmsCfgData.amsVtmParaCfgTimeLength,
														timerPara))
				        {

							//...
							return AMS_ERROR;
				        }

						AmsTimerStatProc(T_AMS_VTM_PARA_CFG_TIMER, AMS_CREATE_TIMER);

				        if(AmsCommonTrace)
				        {
				            dbgprint("Ams[%u] Create T_AMS_VTM_PARA_CFG_TIMER Timer:timerId=%d.",
								AmsCfgVtm(i).vtmId, pVtmNode->vtmParaCfgTimerId);
				        }
				    }	
					
					//update vtmParaCfgState
					pVtmNode->vtmParaCfgState = AMS_TERM_PARA_CFG_STARTING;    
							
				    //向终端发送查询请求
				    if (AMS_SUCCESS!= AmsSendVtmParaCfgInd(pVtmNode, &msg, NULL, &AmsFileServerPara, NULL, 0, CONFIGIND_UPDATE_FILESERVER))
				    {					
	//			        *pResultCode = EXECESULT_FAIL;
	//			        OmsagentCopystr(pResultDescription, descriptionBufLen-1, FailStr, strlen(FailStr));

						cfgNum++;	
						if(cfgNum >= AMS_ALLOWED_TERM_ID_NO_NUM)
						{								
							break;
						}
						
				        continue;       
				    }

					cfgNum++;
					if(cfgNum >= AMS_ALLOWED_TERM_ID_NO_NUM)
					{						
						break;
					}
												
	        	}	

				if(cfgNum >= AMS_ALLOWED_TERM_ID_NO_NUM)
				{
					break;
				}
			}

			curTermType = AMS_OPERATE_TARGET_VTM;
			curTermPos = i;					
		}		
	}
	else
	{
	 	//检查vtm是否已存在
		for(i = termPos; i < vtmNum; i++)
		{
			if(AmsCfgVtm(i).flag != AMS_VTM_INSTALL)
			{
				continue;
			}
			
		    //检查vtm是否在线
        	for(j = 0; j < AMS_MAX_SERVICE_GROUP_NUM; j++)
        	{
        		if (NULL == (pVtmNode = AmsSearchVtmNode(j, AmsCfgVtm(i).vtmId)))
        		{
					continue;
        		}
				
				//check vtmParaCfgState
				if(pVtmNode->vtmParaCfgState != AMS_TERM_PARA_CFG_NULL)
				{

					//...
					continue;	
				}
				
				//wait par cfg Ind cnf,default: 100s
			    if(AmsCfgData.amsVtmParaCfgTimeLength > 0 
				    && AmsCfgData.amsVtmParaCfgTimeLength <= T_AMS_VTM_PARA_CFG_TIMER_LENGTH_MAX)
			    {
					memset(timerPara, 0, PARA_LEN);
					p = timerPara;
					
					BEPUTSHORT(AMS_CONFIG_FILE_SERVER_PARA, p);
					p += 2;
					BEPUTLONG(AmsCfgVtm(i).vtmId, p);	
					
			        if(AMS_OK != AmsCreateTimerPara(pVtmNode->amsPid&0xffff, //not used yet
							                        &pVtmNode->vtmParaCfgTimerId, 
													B_AMS_VTM_PARA_CFG_TIMEOUT, 
													AmsCfgData.amsVtmParaCfgTimeLength,
													timerPara))
			        {

						//...
						return AMS_ERROR;
			        }

					AmsTimerStatProc(T_AMS_VTM_PARA_CFG_TIMER, AMS_CREATE_TIMER);

			        if(AmsCommonTrace)
			        {
			            dbgprint("Ams[%u] Create T_AMS_VTM_PARA_CFG_TIMER Timer:timerId=%d.",
							AmsCfgVtm(i).vtmId, pVtmNode->vtmParaCfgTimerId);
			        }
			    }	
				
				//update vtmParaCfgState
				pVtmNode->vtmParaCfgState = AMS_TERM_PARA_CFG_STARTING;    
						
			    //向终端发送查询请求
			    if (AMS_SUCCESS!= AmsSendVtmParaCfgInd(pVtmNode, &msg, NULL, &AmsFileServerPara, NULL, 0, CONFIGIND_UPDATE_FILESERVER))
			    {										
//			        *pResultCode = EXECESULT_FAIL;
//			        OmsagentCopystr(pResultDescription, descriptionBufLen-1, FailStr, strlen(FailStr));

					cfgNum++;
					if(cfgNum >= AMS_ALLOWED_TERM_ID_NO_NUM)
					{															
						break;
					}
					
			        continue;       
			    }

				cfgNum++;
				if(cfgNum >= AMS_ALLOWED_TERM_ID_NO_NUM)
				{				
					break;
				}					
        	}		

			if(cfgNum >= AMS_ALLOWED_TERM_ID_NO_NUM)
			{
				break;
			}			
		}
		
		curTermType = AMS_OPERATE_TARGET_VTM;
		curTermPos = i;			
	}
	
	if(AMS_OPERATE_TARGET_VTM != curTermType || curTermPos != vtmNum)
	{
		//wait para cfg Ind cnf,default: 100s
	    if(AmsCfgData.amsParaCfgTimeLength > 0 
		    && AmsCfgData.amsParaCfgTimeLength <= T_AMS_PARA_CFG_TIMER_LENGTH_MAX)
	    {
			memset(timerPara, 0, PARA_LEN);
			p = timerPara;
			
			BEPUTSHORT(AMS_CONFIG_FILE_SERVER_PARA, p);
			p += 2;
			BEPUTLONG(curTermType, p);	
			p += 4;			
			BEPUTLONG(curTermPos, p);				
			
	        if(AMS_OK != AmsCreateTimerPara(0, //not used yet
					                        &AmsCfgData.paraCfgTimerId, 
											B_AMS_PARA_CFG_TIMEOUT, 
											AmsCfgData.amsParaCfgTimeLength,
											timerPara))
	        {

				//...
				return AMS_ERROR;
	        }

			AmsTimerStatProc(T_AMS_PARA_CFG_TIMER, AMS_CREATE_TIMER);

	        if(AmsCommonTrace)
	        {
	            dbgprint("Ams[%u][%u] Create T_AMS_PARA_CFG_TIMER Timer:timerId=%d.",
					curTermType, curTermPos, AmsCfgData.paraCfgTimerId);
	        }
	    }	
	}

	return iret;
}


////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
//数据库接口配置

enum
{
    RESULT_SUC = 0,
    RESULT_ERR_PARA,
    RESULT_ITEM_FULL,
};

char *CfgStateFieldStr = "srvDataCfgState";

//zhuyn 20161228
char *AmsGblCfgKeyStr[] = 
{
    "amsTellerLoginState",
    "amsTellerReleaseState",
    "amsTQueueLen",
    "encryptFlag",
    "branchFlag",    
};

//zhuyn added 20161009
char *NatFieldStr[] =
{
    "natFlag",
    "stunServerIp",
    "stunServerPort",
    "stunUserName",
    "stunPasswd",
    "turnServerIp",
    "turnServerPort",
    "turnUserName",
    "turnPasswd",
};

char *FileServerFieldStr[] =
{
    "fileAudioPath",
    "fileScreenPath",
    "fileSnapPath",
    "fileOrdinaryPath",     //zhuyn  增加 20160927
    "fileServerIp",
    "fileServerPort",
    "fileUserName",
    "fileUserPwd",
};


int AmsJudgeCfgState(int iThread)
{
    char cfgstatus[8] = {'0'};
    int ucCfgstatus;

    int ret;    
    char szSqlStr[128];
    OCIDefine    *definehp[1];
    sb2 defineInd[1];        //OCI_IND_NULL
    char sErrorMsg[512];
	sb4    sb4ErrorCode; 
    ub4    ub4RecordNo;
    
    snprintf(szSqlStr, sizeof(szSqlStr),"select real_value FROM vtc_system_conf where conf_key = '%s'",CfgStateFieldStr);
    ret=OCIStmtPrepare(m_stmthp[iThread], m_errhp[iThread], (text*)szSqlStr, (ub4)strlen(szSqlStr),
        (ub4) OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[0],m_errhp[iThread],1,
        cfgstatus,sizeof(cfgstatus), SQLT_STR,(dvoid *)&defineInd[0], NULL, NULL, OCI_DEFAULT);
    if (OCI_SUCCESS !=(ret=OCIStmtExecute(m_svchp[iThread], m_stmthp[iThread], m_errhp[iThread], 
        (ub4) 0, (ub4) 0,(OCISnapshot *) NULL,(OCISnapshot *) NULL, (ub4)OCI_DEFAULT)))
    {
        OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
			dbgprint("AmsJudgeCfgState sql OCIStmtExecute failed,error msg:%d, %s,...", sb4ErrorCode, sErrorMsg);
        return AMS_ERROR;
    }    
    
    if((ret=OCIStmtFetch2(m_stmthp[iThread],m_errhp[iThread],1,OCI_FETCH_NEXT,1,OCI_DEFAULT))!=OCI_NO_DATA)
    {
        if(ret == OCI_ERROR)
		{
			OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
			dbgprint("AmsLoadServiceInfo sql OCIStmtFetch2 failed,error msg:%d, %s,...", sb4ErrorCode, sErrorMsg);
			return AMS_ERROR;
		}
        ucCfgstatus = atoi(cfgstatus);
        if (ucCfgstatus == 1 || ucCfgstatus == 2)
        {
            return AMS_OK;
        }
        else    //配置状态（0：未配置；1：已配置，未激活；2：已激活）针对由配置文件导入的业务数据，包括：业务组配置、业务配置、柜员的批量配置、柜员机的批量配置、柜员业务权限的批量配置初始状态为未配置；网管将业务数据加载到数据库后，配置状态设置为已配置未激活；AMS将业务数据加载到内存后，将配置状态设置为已激活
        {
            dbgprint("cfgstatus=%d,FAIL", ucCfgstatus);
            return AMS_ERROR;
        }
    }
    dbgprint("no cfgstatus FAIL");
    return AMS_ERROR;
}

int AmsLoadGlobalCfg(int iThread)
{
    char cVal[130], *field;
    int loop;
    OA_AMS_GLOBAL_CONFIG_t gCfg;

    int ret;    
    char szSqlStr[128];
    OCIDefine    *definehp[1];
    sb2 defineInd[1];        //OCI_IND_NULL
    char sErrorMsg[512];
	sb4    sb4ErrorCode; 
    ub4    ub4RecordNo;
    int ResultCode;

    memset(gCfg.flag, 0 ,sizeof(gCfg.flag));
    
    for (loop = AMSGBLCFG_ITEM_LOGINSTATE; loop < AMSGBLCFG_ITEM_MAX; loop++)        //参数，按序为AmsLoginStateFieldStr、AmsRelStateFieldStr、AmsTQueueLenStr
    {
        field = AmsGblCfgKeyStr[loop];        
        snprintf(szSqlStr, sizeof(szSqlStr),"select real_value FROM vtc_system_conf where conf_key = '%s'",field);
        ret=OCIStmtPrepare(m_stmthp[iThread], m_errhp[iThread], (text*)szSqlStr, (ub4)strlen(szSqlStr),
            (ub4) OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);
        ret=OCIDefineByPos(m_stmthp[iThread],&definehp[0],m_errhp[iThread],1,
            cVal,sizeof(cVal), SQLT_STR,(dvoid *)&defineInd[0], NULL, NULL, OCI_DEFAULT);
        if (OCI_SUCCESS !=(ret=OCIStmtExecute(m_svchp[iThread], m_stmthp[iThread], m_errhp[iThread], 
            (ub4) 0, (ub4) 0,(OCISnapshot *) NULL,(OCISnapshot *) NULL, (ub4)OCI_DEFAULT)))
        {
            OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
    			dbgprint("AmsLoadGlobalCfg sql OCIStmtExecute failed,error msg:%d, %s,...\r\nsql:%s", sb4ErrorCode, sErrorMsg,szSqlStr);
            return AMS_ERROR;
        }    
        
        if((ret=OCIStmtFetch2(m_stmthp[iThread],m_errhp[iThread],1,OCI_FETCH_NEXT,1,OCI_DEFAULT))!=OCI_NO_DATA)
        {
            if(ret == OCI_ERROR)
    		{
    			OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
    			dbgprint("AmsLoadGlobalCfg sql OCIStmtFetch2 failed,error msg:%d, %s,...", sb4ErrorCode, sErrorMsg);
    			return AMS_ERROR;
    		}        
            if (-1 == defineInd[0]) //value is NULL zhuyn 20161229
            {
                dbgprint("vtc_system_conf TABLE conf_key=\"%s\" Value is NULL", field);
                continue;
            }
            
            if (AMSGBLCFG_ITEM_LOGINSTATE == loop) 
            {
                gCfg.loginState = (unsigned char)atoi(cVal);
                gCfg.flag[AMSGBLCFG_ITEM_LOGINSTATE] = 1;
            }
            else  if (AMSGBLCFG_ITEM_RELSTATE == loop) 
            {
                gCfg.relState = (unsigned char)atoi(cVal);
                gCfg.flag[AMSGBLCFG_ITEM_RELSTATE] = 1;
            }
            else  if (AMSGBLCFG_ITEM_TQUEUELEN == loop) 
            {
                gCfg.tQueueLen = atoi(cVal);
                gCfg.flag[AMSGBLCFG_ITEM_TQUEUELEN] = 1;
            }
            else  if (AMSGBLCFG_ITEM_ENCRYPTFLAG == loop) 
            {
                gCfg.encryptFlag = atoi(cVal);
                gCfg.flag[AMSGBLCFG_ITEM_ENCRYPTFLAG] = 1;
            }  
            else  if (AMSGBLCFG_ITEM_BRANCHFLAG == loop) 
            {
                gCfg.branchFlag = atoi(cVal);
                gCfg.flag[AMSGBLCFG_ITEM_BRANCHFLAG] = 1;
            }
        }
        else
        {
            dbgprint("vtc_system_conf TABLE conf_key=\"%s\", not exist", field);
            //return AMS_ERROR;
            continue;
        }
    }
    if (OA_RET_SUC != AmsModifyGlobalConfig(&gCfg, &ResultCode, sErrorMsg, sizeof(sErrorMsg)))
    {
        dbgprint("AmsModifyGlobalConfig FAIL: %s",sErrorMsg);
        return AMS_ERROR;
    }
    return AMS_OK;
}

//zhuyn added 20161009
int AmsLoadNatCfg(int iThread)
{
    char cVal[64], *field;
    int loop;
    OA_AMS_NAT_CONFIG_t natCfg;

    int ret;    
    char szSqlStr[128];
    OCIDefine    *definehp[1];
    sb2 defineInd[1];        //OCI_IND_NULL
    char sErrorMsg[512];
	sb4    sb4ErrorCode; 
    ub4    ub4RecordNo;
    int ResultCode;

    memset(natCfg.flag, 0 ,sizeof(natCfg.flag));
    
    for (loop = 0; loop < AMSNATCFG_ITEM_MAX; loop++)        //参数，按序为AmsLoginStateFieldStr、AmsRelStateFieldStr、AmsTQueueLenStr
    {
        field = NatFieldStr[loop];

        snprintf(szSqlStr, sizeof(szSqlStr),"select real_value FROM vtc_system_conf where conf_key = '%s'",field);
        ret=OCIStmtPrepare(m_stmthp[iThread], m_errhp[iThread], (text*)szSqlStr, (ub4)strlen(szSqlStr),
            (ub4) OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);
        ret=OCIDefineByPos(m_stmthp[iThread],&definehp[0],m_errhp[iThread],1,
            cVal,sizeof(cVal), SQLT_STR,(dvoid *)&defineInd[0], NULL, NULL, OCI_DEFAULT);
        if (OCI_SUCCESS !=(ret=OCIStmtExecute(m_svchp[iThread], m_stmthp[iThread], m_errhp[iThread], 
            (ub4) 0, (ub4) 0,(OCISnapshot *) NULL,(OCISnapshot *) NULL, (ub4)OCI_DEFAULT)))
        {
            OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
    			dbgprint("AmsLoadNatCfg sql OCIStmtExecute failed,error msg:%d, %s,...\r\nsql:%s", sb4ErrorCode, sErrorMsg,szSqlStr);
            return AMS_ERROR;
        }    
        
        if((ret=OCIStmtFetch2(m_stmthp[iThread],m_errhp[iThread],1,OCI_FETCH_NEXT,1,OCI_DEFAULT))!=OCI_NO_DATA)
        {
            if(ret == OCI_ERROR)
    		{
    			OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
    			dbgprint("AmsLoadNatCfg sql OCIStmtFetch2 failed,error msg:%d, %s,...", sb4ErrorCode, sErrorMsg);
    			return AMS_ERROR;
    		}        
            if (-1 == defineInd[0]) //value is NULL zhuyn 20161229
            {
                dbgprint("vtc_system_conf TABLE conf_key=\"%s\" Value is NULL", field);
                continue;
            }
            
            if (AMSNATCFG_ITEM_NATFLAG == loop) 
            {
                natCfg.natFlag = atoi(cVal);
                natCfg.flag[loop] = 1;
            }

            else  if (AMSNATCFG_ITEM_STUNIPFLAG == loop) //
            {
                OmsagentCopystr(natCfg.stunIp, OA_MAX_IPSTR_LEN, cVal, strlen(cVal));
                natCfg.flag[loop] = 1;
            }
            else  if (AMSNATCFG_ITEM_STUNPORTFLAG == loop) //
            {
                natCfg.stunPort = atoi(cVal);
                natCfg.flag[loop] = 1;
            }
            else  if (AMSNATCFG_ITEM_STUNUSERFLAG== loop) //
            {
                OmsagentCopystr(natCfg.stunUser, MAX_ORD_USER_NAME_LEN, cVal, strlen(cVal));
                natCfg.flag[loop] = 1;
            }
            else  if (AMSNATCFG_ITEM_STUNPWDFLAG == loop) //
            {
                OmsagentCopystr(natCfg.stunPwd, MAX_ORD_PWD_LEN, cVal, strlen(cVal));
                natCfg.flag[loop] = 1;
            }

            else  if (AMSNATCFG_ITEM_TURNIPFLAG == loop) //
            {
                OmsagentCopystr(natCfg.turnIp, OA_MAX_IPSTR_LEN, cVal, strlen(cVal));
                natCfg.flag[loop] = 1;
            }
            else  if (AMSNATCFG_ITEM_TURNPORTFLAG == loop) //
            {
                natCfg.turnPort = atoi(cVal);
                natCfg.flag[loop] = 1;
            }
            else  if (AMSNATCFG_ITEM_TURNUSERFLAG== loop) //
            {
                OmsagentCopystr(natCfg.turnUser, MAX_ORD_USER_NAME_LEN, cVal, strlen(cVal));
                natCfg.flag[loop] = 1;
            }
            else  if (AMSNATCFG_ITEM_TURNPWDFLAG == loop) //
            {
                OmsagentCopystr(natCfg.turnPwd, MAX_ORD_PWD_LEN, cVal, strlen(cVal));
                natCfg.flag[loop] = 1;
            }
            
        }
        else
        {
            dbgprint("vtc_system_conf TABLE conf_key=\"%s\", not exist", field);
            //return AMS_ERROR;
            continue;
        }
    }
    if (OA_RET_SUC != AmsModifyNatConfig(&natCfg, &ResultCode, sErrorMsg, sizeof(sErrorMsg)))
    {
        dbgprint("AmsModifyNatConfig FAIL: %s",sErrorMsg);
        return AMS_ERROR;
    }
    return AMS_OK;
}

int AmsLoadRcasCfg(int iThread)
{
    int ret;    
    char szSqlStr[128];
    OCIDefine    *definehp[2];
    sb2 defineInd[2];       
    char sErrorMsg[512];
    ub4    ub4RecordNo = 1; 
	sb4    sb4ErrorCode; 
    char    serverType[33];
    OA_AMS_RCAS_CONFIG_t rcasCfg;
    int cfgErr,count;

    snprintf(szSqlStr, sizeof(szSqlStr),"select mid, server_type FROM vtc_server");

    ret=OCIStmtPrepare(m_stmthp[iThread], m_errhp[iThread], (text*)szSqlStr, (ub4)strlen(szSqlStr),
        (ub4) OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);    
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[0],m_errhp[iThread],1,
        &rcasCfg.rcasMid,sizeof(rcasCfg.rcasMid), SQLT_INT,(dvoid *)&defineInd[0], NULL, NULL, OCI_DEFAULT);    
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[1],m_errhp[iThread],2,
        serverType,sizeof(serverType), SQLT_STR,(dvoid *)&defineInd[1], NULL, NULL, OCI_DEFAULT);    
    if (OCI_SUCCESS !=(ret=OCIStmtExecute(m_svchp[iThread], m_stmthp[iThread], m_errhp[iThread], 
        (ub4) 0, (ub4) 0,(OCISnapshot *) NULL,(OCISnapshot *) NULL, (ub4)OCI_DEFAULT)))
    {
        OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
			dbgprint("AmsLoadRcasCfg sql OCIStmtExecute failed,error msg:%d, %s,...\r\nsql:%s", sb4ErrorCode, sErrorMsg,szSqlStr);
        return AMS_ERROR;
    }

    count = 0;    
    while((ret=OCIStmtFetch2(m_stmthp[iThread],m_errhp[iThread],1,OCI_FETCH_NEXT,1,OCI_DEFAULT))!=OCI_NO_DATA)
    {
        if(ret == OCI_ERROR)
		{
			OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
			dbgprint("AmsLoadRcasCfg sql OCIStmtFetch2 failed,error msg:%d, %s,...", sb4ErrorCode, sErrorMsg);
			return AMS_ERROR;
		}     
        if (-1 == defineInd[0] || -1 == defineInd[1])   //valuse is NULL
            continue;
        
        if (0 != strcasecmp(serverType, RCAS_SERVER_TYPE_STR))
            continue;

        rcasCfg.flag[AMSRCASCFG_ITEM_MID] = 1;
        if (AmsAddRcasConfig(&rcasCfg, &cfgErr, sErrorMsg, sizeof(sErrorMsg)) != OA_RET_SUC)
        {
            dbgprint("AmsLoadRcasCfg FAIL err = %u, %s", cfgErr, sErrorMsg);
            continue;
        }
        count++;
    }
    dbgprint("AmsLoadRcasCfg count=%d",count);    
    
    return AMS_OK;
}

int AmsLoadFileServerCfg(int iThread)
{
    char cVal[128];
    int loop;
    OA_AMS_FILESERVER_CONFIG_t cfg;

    int ret;    
    char szSqlStr[128];
    OCIDefine    *definehp[1];
    sb2 defineInd[1];        //OCI_IND_NULL
    char sErrorMsg[512];
	sb4    sb4ErrorCode; 
    ub4    ub4RecordNo;
    int ResultCode;

    memset(cfg.flag, 0 ,sizeof(cfg.flag));

    //7个参数，按序为"fileAudioPath","fileScreenPath","fileSnapPath","fileServerIp","fileServerPort","fileUserName","fileUserPwd",
    for (loop = 0; loop < 8; loop++)   
    {
        snprintf(szSqlStr, sizeof(szSqlStr),"select real_value FROM vtc_system_conf where conf_key = '%s'",FileServerFieldStr[loop]);
        ret=OCIStmtPrepare(m_stmthp[iThread], m_errhp[iThread], (text*)szSqlStr, (ub4)strlen(szSqlStr),
            (ub4) OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);
        ret=OCIDefineByPos(m_stmthp[iThread],&definehp[0],m_errhp[iThread],1,
            cVal,sizeof(cVal), SQLT_STR,(dvoid *)&defineInd[0], NULL, NULL, OCI_DEFAULT);
        if (OCI_SUCCESS !=(ret=OCIStmtExecute(m_svchp[iThread], m_stmthp[iThread], m_errhp[iThread], 
            (ub4) 0, (ub4) 0,(OCISnapshot *) NULL,(OCISnapshot *) NULL, (ub4)OCI_DEFAULT)))
        {
            OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
    			dbgprint("AmsLoadGlobalCfg sql OCIStmtExecute failed,error msg:%d, %s,...\r\nsql:%s", sb4ErrorCode, sErrorMsg,szSqlStr);
            return AMS_ERROR;
        }    
        
        if((ret=OCIStmtFetch2(m_stmthp[iThread],m_errhp[iThread],1,OCI_FETCH_NEXT,1,OCI_DEFAULT))!=OCI_NO_DATA)
        {
            if(ret == OCI_ERROR)
    		{
    			OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
    			dbgprint("AmsLoadGlobalCfg sql OCIStmtFetch2 failed,error msg:%d, %s,...", sb4ErrorCode, sErrorMsg);
    			return AMS_ERROR;
    		}        
            if (-1 == defineInd[0])  //valuse is NULL
            {
                dbgprint("vtc_system_conf TABLE conf_key=\"%s\" Value is NULL", FileServerFieldStr[loop]);
                continue;
            }
            
            if (0 == loop) //fileAudioPath
            {
                OmsagentCopystr(cfg.AudioRecFilePath, MAX_QCFILEPATH_LEN, cVal, strlen(cVal));
                cfg.flag[AMSFILESERVER_ITEM_AUDIORECPATH] = 1;
            }
            else  if (1 == loop) //fileScreenPath
            {
                OmsagentCopystr(cfg.ScreenRecFilePath, MAX_QCFILEPATH_LEN, cVal, strlen(cVal));
                cfg.flag[AMSFILESERVER_ITEM_SCREENRECPATH] = 1;
            }
            else  if (2 == loop) //fileSnapPath
            {
                OmsagentCopystr(cfg.SnapFilePath, MAX_QCFILEPATH_LEN, cVal, strlen(cVal));
                cfg.flag[AMSFILESERVER_ITEM_SNAPPATH] = 1;
            }
            else  if (3 == loop) //OrdinaryFilePath
            {
                OmsagentCopystr(cfg.OrdinaryFilePath, MAX_QCFILEPATH_LEN, cVal, strlen(cVal));
                cfg.flag[AMSFILESERVER_ITEM_ORDINARYPATH] = 1;
            }
            else  if (4 == loop) //fileServerIp
            {
                OmsagentCopystr(cfg.Ip, OA_MAX_IPSTR_LEN, cVal, strlen(cVal));
                cfg.flag[AMSFILESERVER_ITEM_IP] = 1;
            }
            else  if (5 == loop) //fileServerPort
            {
                cfg.port = atoi(cVal);
                cfg.flag[AMSFILESERVER_ITEM_PORT] = 1;
            }
            else  if (6 == loop) //fileUserName
            {
                OmsagentCopystr(cfg.user, MAX_FS_USER_NAME_LEN, cVal, strlen(cVal));
                cfg.flag[AMSFILESERVER_ITEM_USER] = 1;
            }
            else  if (7 == loop) //fileUserPwd
            {
                OmsagentCopystr(cfg.pwd, MAX_FS_PWD_LEN, cVal, strlen(cVal));
                cfg.flag[AMSFILESERVER_ITEM_PWD] = 1;
            }
        }
        else
        {
            dbgprint("vtc_system_conf TABLE conf_key=\"%s\", not exist", FileServerFieldStr[loop]);
            //return AMS_ERROR;
            continue;
        }
    }
    if (OA_RET_SUC != AmsSetFileServer(&cfg, &ResultCode, sErrorMsg, sizeof(sErrorMsg)))
    {
        dbgprint("AmsSetFileServer FAIL: %s",sErrorMsg);
        return AMS_ERROR;
    }
    return AMS_OK;
}

//zhuyn 20121228
int AmsLoadOrgInfo(int iThread)
{
    OA_AMS_ORG_CONFIG_t orgCfg;    
    int addresult,added,item;
    char addRltStr[10];

    int ret;    
    char *szSqlStr = (text *) "select org_id,org_code,org_name,org_type,org_level,parent_org_id,org_state FROM base_org";
    OCIDefine    *definehp[AMSORGCFG_ITEM_MAX];
    sb2 defineInd[AMSORGCFG_ITEM_MAX];        //OCI_IND_NULL
    char sErrorMsg[512];
    ub4    ub4RecordNo = 1; 
	sb4    sb4ErrorCode; 
    
    memset(&orgCfg, 0, sizeof(orgCfg));
    
    ret=OCIStmtPrepare(m_stmthp[iThread], m_errhp[iThread], (text*)szSqlStr, (ub4)strlen(szSqlStr),
        (ub4) OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSORGCFG_ITEM_ORGID],m_errhp[iThread],1,
        &orgCfg.orgId,sizeof(orgCfg.orgId), SQLT_INT,(dvoid *)&defineInd[AMSORGCFG_ITEM_ORGID], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSORGCFG_ITEM_ORGCODE],m_errhp[iThread],2,
        orgCfg.orgCode,sizeof(orgCfg.orgCode), SQLT_STR,(dvoid *)&defineInd[AMSORGCFG_ITEM_ORGCODE], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSORGCFG_ITEM_ORGNAME],m_errhp[iThread],3,
        orgCfg.orgName,sizeof(orgCfg.orgName), SQLT_STR,(dvoid *)&defineInd[AMSORGCFG_ITEM_ORGNAME], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSORGCFG_ITEM_ORGTYPE],m_errhp[iThread],4,
        orgCfg.orgType,sizeof(orgCfg.orgType), SQLT_STR,(dvoid *)&defineInd[AMSORGCFG_ITEM_ORGTYPE], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSORGCFG_ITEM_ORGLEVEL],m_errhp[iThread],5,
        &orgCfg.orgLevel,sizeof(orgCfg.orgLevel), SQLT_INT,(dvoid *)&defineInd[AMSORGCFG_ITEM_ORGLEVEL], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSORGCFG_ITEM_PARENTORGID],m_errhp[iThread],6,
        &orgCfg.parentOrgId,sizeof(orgCfg.parentOrgId), SQLT_INT,(dvoid *)&defineInd[AMSORGCFG_ITEM_PARENTORGID], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSORGCFG_ITEM_ORGSTATE],m_errhp[iThread],7,
        orgCfg.orgState,sizeof(orgCfg.orgState), SQLT_STR,(dvoid *)&defineInd[AMSORGCFG_ITEM_ORGSTATE], NULL, NULL, OCI_DEFAULT);
    if (OCI_SUCCESS !=(ret=OCIStmtExecute(m_svchp[iThread], m_stmthp[iThread], m_errhp[iThread], 
        (ub4) 0, (ub4) 0,(OCISnapshot *) NULL,(OCISnapshot *) NULL, (ub4)OCI_DEFAULT)))
    {
        OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
			dbgprint("AmsLoadOrgInfo sql OCIStmtExecute failed,error msg:%d, %s,...\r\nsql:%s", sb4ErrorCode, sErrorMsg,szSqlStr);
        return AMS_ERROR;
    }
    
    added = 0;    
    while((ret=OCIStmtFetch2(m_stmthp[iThread],m_errhp[iThread],1,OCI_FETCH_NEXT,1,OCI_DEFAULT))!=OCI_NO_DATA)
    {
        if(ret == OCI_ERROR)
		{
			OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
			dbgprint("AmsLoadOrgInfo sql OCIStmtFetch2 failed,error msg:%d, %s,...", sb4ErrorCode, sErrorMsg);
			return AMS_ERROR;
		}
        memset(&orgCfg.flag, 1, sizeof(orgCfg.flag));
        for (item=AMSORGCFG_ITEM_ORGID; item < AMSORGCFG_ITEM_MAX; item++)  //check null value
        {
            if (-1 == defineInd[item])  //The selected value is null, and the value of the output variable is unchanged.
            {
                orgCfg.flag[item] = 0;
            }
        }
        if (orgCfg.flag[AMSORGCFG_ITEM_ORGCODE])
        {
            OAInConv(orgCfg.orgCode, sizeof(orgCfg.orgCode));
        }
        if (orgCfg.flag[AMSORGCFG_ITEM_ORGNAME])
        {
            OAInConv(orgCfg.orgName, sizeof(orgCfg.orgName));
        }
        if (AmsAddOrg(&orgCfg, &addresult, addRltStr, sizeof(addRltStr)) != OA_RET_SUC)
        {
            dbgprint("AmsAddOrg FAIL: orgid=%u, addresult=%d", orgCfg.orgId, addresult);
            return AMS_ERROR;
        }
        else
        {
            added++;
        }
    }
    dbgprint("AmsAddOrg ADDED=%d", added);
    return AMS_OK;
}

int AmsAddService(DWORD serviceId, char *serviceName, int *result)
{
    int i;

    *result = RESULT_SUC;
    if (0 == serviceId || serviceId > AMS_MAX_SERVICE_ID_VALUE
        || strlen(serviceName) > AMS_MAX_SERVICE_NAME_LEN)
    {
        *result = RESULT_ERR_PARA;
        return AMS_ERROR;
    }

    //检查重复
	for(i = 0; i < AMS_MAX_SERVICE_NUM; i++)
	{
		if(AMS_SERVICE_INSTALL == AmsCfgService(i).flag && serviceId == AmsCfgService(i).service)
		{
		    *result = RESULT_ERR_PARA;
			return AMS_ERROR;
		}
	}
    
    /* record serviceId */
	for(i = 0; i < AMS_MAX_SERVICE_NUM; i++)
	{
		if(AMS_SERVICE_UNINSTALL == AmsCfgService(i).flag)
		{
			AmsCfgService(i).service = serviceId;
            strcpy(AmsCfgService(i).serviceName, serviceName);
            AmsCfgService(i).serviceNameLen = strlen(AmsCfgService(i).serviceName);
			AmsCfgService(i).flag = AMS_SERVICE_INSTALL;
			return AMS_OK;
		}
	}
    *result = RESULT_ITEM_FULL;
    return AMS_ERROR;	
}

int AmsLoadServiceInfo(int iThread)
{
    SERVICE_INFO serviceInfo;    
    int addresult,added;

    int ret;    
    char *szSqlStr = (text *) "select skill_id, skill_name FROM vtc_skill_type";
    OCIDefine    *definehp[2];
    sb2 defineInd[2];        //OCI_IND_NULL
    char sErrorMsg[512];
    ub4    ub4RecordNo = 1; 
	sb4    sb4ErrorCode; 
    

    ret=OCIStmtPrepare(m_stmthp[iThread], m_errhp[iThread], (text*)szSqlStr, (ub4)strlen(szSqlStr),
        (ub4) OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[0],m_errhp[iThread],1,
        &serviceInfo.service,sizeof(serviceInfo.service), SQLT_INT,(dvoid *)&defineInd[0], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[1],m_errhp[iThread],2,
        &serviceInfo.serviceName,sizeof(serviceInfo.serviceName), SQLT_STR,(dvoid *)&defineInd[1], NULL, NULL, OCI_DEFAULT);
    if (OCI_SUCCESS !=(ret=OCIStmtExecute(m_svchp[iThread], m_stmthp[iThread], m_errhp[iThread], 
        (ub4) 0, (ub4) 0,(OCISnapshot *) NULL,(OCISnapshot *) NULL, (ub4)OCI_DEFAULT)))
    {
        OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
			dbgprint("AmsLoadServiceInfo sql OCIStmtExecute failed,error msg:%d, %s,...\r\nsql:%s", sb4ErrorCode, sErrorMsg,szSqlStr);
        return AMS_ERROR;
    }
    
    added = 0;
    while((ret=OCIStmtFetch2(m_stmthp[iThread],m_errhp[iThread],1,OCI_FETCH_NEXT,1,OCI_DEFAULT))!=OCI_NO_DATA)
    {
        if(ret == OCI_ERROR)
		{
			OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
			dbgprint("AmsLoadServiceInfo sql OCIStmtFetch2 failed,error msg:%d, %s,...", sb4ErrorCode, sErrorMsg);
			return AMS_ERROR;
		}
        if (-1 == defineInd[0])
        {
            dbgprint("vtc_skill_type TABLE \"skill_id\" Value is NULL");
            continue;
        }
        if (-1 == defineInd[1])
        {
            serviceInfo.serviceName[0] = '\0';
        }
        OAInConv(serviceInfo.serviceName, sizeof(serviceInfo.serviceName));
        if (AmsAddService(serviceInfo.service, (char *)serviceInfo.serviceName,&addresult) != AMS_OK)
        {
            dbgprint("AmsAddService FAIL: service=%d, addresult=%d", serviceInfo.service, addresult);
            return AMS_ERROR;
        }
        else
        {
            added++;
        }
    }
    dbgprint("AmsAddService ADDED=%d", added);
    return AMS_OK;
}

int AmsAddServiceGroup(DWORD srvGrpId, char *srvGroupName, unsigned char isHighLevel, int *result)
{
    int i;

    *result = RESULT_SUC;
    
    if (srvGrpId >= AMS_MAX_SERVICE_GROUP_NUM
        || strlen(srvGroupName) > AMS_MAX_SERVICE_GROUP_NAME_LEN
        || (isHighLevel != 0 && isHighLevel != 1))
    {
        *result = RESULT_ERR_PARA;
        return AMS_ERROR;
    }

    //检查重复
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		if(AMS_SERVICE_GROUP_INSTALL == AmsCfgSrvGroup(i).flag && srvGrpId == AmsCfgSrvGroup(i).srvGrpId)
		{
		    *result = RESULT_ERR_PARA;
			return AMS_ERROR;
		}
	}
    
   /* record srvGrpId */
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		if(AMS_SERVICE_GROUP_UNINSTALL == AmsCfgSrvGroup(i).flag)
		{
			AmsCfgSrvGroup(i).srvGrpId = srvGrpId;
			AmsCfgSrvGroup(i).flag = AMS_SERVICE_GROUP_INSTALL;
            strcpy(AmsCfgSrvGroup(i).srvGroupName, srvGroupName);
            AmsCfgSrvGroup(i).srvGroupNameLen = strlen(AmsCfgSrvGroup(i).srvGroupName);
            AmsCfgSrvGroup(i).serviceType = 0;
            AmsCfgSrvGroup(i).srvTypeRsvd = 0;
            AmsCfgSrvGroup(i).isHighLevel = isHighLevel;

			AmsSrvData(AmsCfgSrvGroup(i).srvGrpId).serviceState = AMS_SERVICE_ACTIVE;
			
			return i;
		}
	}

    *result = RESULT_ITEM_FULL;
    return AMS_ERROR;	
}
int AmsAddServiceGroupServiceId(int i, int  service, int *result)
{
    char serviceName[AMS_MAX_SERVICE_NAME_LEN + 1];
    DWORD   serviceType;
    DWORD srvRsvdType = 0xffffffff;

    *result = RESULT_SUC;
    if (i >= AMS_MAX_SERVICE_GROUP_NUM || service < 1 || service > AMS_MAX_SERVICE_ID_VALUE)
    {
        *result = RESULT_ERR_PARA;
        return AMS_ERROR;
    }
    
    if(AMS_ERROR == amsGetServiceNameByServiceId(service, serviceName))
    {
		return AMS_ERROR;	
    }

	serviceType = (1 << (service - 1));
	
	AmsCfgSrvGroup(i).srvTypeRsvd |= srvRsvdType;
	AmsCfgSrvGroup(i).serviceType |= serviceType;
    
    return AMS_OK;	
}
int AmsLoadServiceGroupInfo(int iThread)
{
    SERVICE_GROUP_INFO serviceGroup;    
    unsigned int groupLevel_array[AMS_MAX_SERVICE_GROUP_NUM];
    int  service; 
    int addresult,added;
    unsigned char flag[AMS_MAX_SERVICE_GROUP_NUM];

    int ret,index;    
    char *szSqlStr = (text *) "select group_id, group_name, is_high_level FROM vtc_skill_group";
    char *szSqlRelateStr = (text *) "select skill_id FROM vtc_skill_group_relate WHERE group_id = :group_id";
    OCIBind *bindhp[1];
    OCIDefine    *definehp[3];
    sb2 bindInd[1], defineInd[3];        //OCI_IND_NULL
    char sErrorMsg[512];
    ub4    ub4RecordNo = 1; 
	sb4    sb4ErrorCode; 

    memset(flag, 0 , sizeof(flag));    

    ret=OCIStmtPrepare(m_stmthp[iThread], m_errhp[iThread], (text*)szSqlStr, (ub4)strlen(szSqlStr),
        (ub4) OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);    
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[0],m_errhp[iThread],1,
        &serviceGroup.srvGrpId,sizeof(serviceGroup.srvGrpId), SQLT_INT,(dvoid *)&defineInd[0], NULL, NULL, OCI_DEFAULT);    
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[1],m_errhp[iThread],2,
        &serviceGroup.srvGroupName,sizeof(serviceGroup.srvGroupName), SQLT_STR,(dvoid *)&defineInd[1], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[2],m_errhp[iThread],3,
        &serviceGroup.isHighLevel,sizeof(serviceGroup.isHighLevel), SQLT_INT,(dvoid *)&defineInd[2], NULL, NULL, OCI_DEFAULT);
    if (OCI_SUCCESS !=(ret=OCIStmtExecute(m_svchp[iThread], m_stmthp[iThread], m_errhp[iThread], 
        (ub4) 0, (ub4) 0,(OCISnapshot *) NULL,(OCISnapshot *) NULL, (ub4)OCI_DEFAULT)))
    {
        OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
			dbgprint("AmsLoadServiceGroupInfo sql OCIStmtExecute failed,error msg:%d, %s,...\r\nsql:%s", sb4ErrorCode, sErrorMsg,szSqlStr);
        return AMS_ERROR;
    }
    
    added = 0;
    while((ret=OCIStmtFetch2(m_stmthp[iThread],m_errhp[iThread],1,OCI_FETCH_NEXT,1,OCI_DEFAULT))!=OCI_NO_DATA)
    {
        if(ret == OCI_ERROR)
		{
			OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
			dbgprint("AmsLoadServiceGroupInfo sql OCIStmtFetch2 failed,error msg:%d, %s,...", sb4ErrorCode, sErrorMsg);
			return AMS_ERROR;
		}
        if (-1 == defineInd[0])
        {
            dbgprint("vtc_skill_group TABLE \"group_id\" Value is NULL");
            continue;
        }
        if (-1 == defineInd[1])
            serviceGroup.srvGroupName[0] = '\0';
        if (-1 == defineInd[2])
            serviceGroup.isHighLevel = 0;
        OAInConv(serviceGroup.srvGroupName, sizeof(serviceGroup.srvGroupName));
        if ((index = AmsAddServiceGroup(serviceGroup.srvGrpId, (char *)serviceGroup.srvGroupName,serviceGroup.isHighLevel,&addresult)) == AMS_ERROR)
        {
            dbgprint("AmsAddServiceGroup FAIL: srvGrpId=%d, addresult=%d", serviceGroup.srvGrpId, addresult);
            return AMS_ERROR;
        }
        else
        {
            flag[index] = 1;
            groupLevel_array[index] = serviceGroup.srvGrpId;
            added++;
        }
    }
    
    added = 0;
    for (index=0; index < AMS_MAX_SERVICE_GROUP_NUM; index++)
    {
        if (flag[index] != 1)
            continue;

        ret=OCIStmtPrepare(m_stmthp[iThread], m_errhp[iThread], (text*)szSqlRelateStr, (ub4)strlen(szSqlRelateStr),
            (ub4) OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);    
        ret=OCIBindByPos(m_stmthp[iThread],&bindhp[0],m_errhp[iThread],1,
                     &groupLevel_array[index],sizeof(groupLevel_array[index]), SQLT_INT,
                     &bindInd[0],NULL,NULL,0,NULL,OCI_DEFAULT);
        ret=OCIDefineByPos(m_stmthp[iThread],&definehp[0],m_errhp[iThread],1,
            &service,sizeof(service), SQLT_INT,(dvoid *)&defineInd[0], NULL, NULL, OCI_DEFAULT);
        if (OCI_SUCCESS !=(ret=OCIStmtExecute(m_svchp[iThread], m_stmthp[iThread], m_errhp[iThread], 
            (ub4) 0, (ub4) 0,(OCISnapshot *) NULL,(OCISnapshot *) NULL, (ub4)OCI_DEFAULT)))
        {
            OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
    			dbgprint("AmsLoadServiceGroupInfoRelate sql OCIStmtExecute failed,error msg:%d, %s,...", sb4ErrorCode, sErrorMsg);
            return AMS_ERROR;
        }

        while((ret=OCIStmtFetch2(m_stmthp[iThread],m_errhp[iThread],1,OCI_FETCH_NEXT,1,OCI_DEFAULT))!=OCI_NO_DATA)
        {
            if(ret == OCI_ERROR)
    		{
    			OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
    			dbgprint("AmsLoadServiceGroupInfo(get service) sql OCIStmtFetch2 failed,error msg:%d, %s,...", sb4ErrorCode, sErrorMsg);
    			return AMS_ERROR;
    		}
            if (-1 == defineInd[0])
            {
                dbgprint("vtc_skill_group_relate TABLE \"skill_id\" Value is NULL");
                continue;
            }
            if (AmsAddServiceGroupServiceId(index, service,&addresult) != AMS_OK)
            {
                dbgprint("AmsAddServiceGroupServiceId failed,error msg:index=%d, service=%d, addresult=%d,...", index, service, addresult);
                return AMS_ERROR;
            } 
        }
        added++;
    }
    dbgprint("AmsLoadServiceGroupInfo ADDED=%d", added);
    return AMS_OK;
}

int AmsLoadTellerInfo(int iThread)
{
    OA_AMS_TELLER_CONFIG_t terllerCfg;    
    int addresult,added;
    char addRltStr[10];

    int ret,item;    
    char *szSqlStr = (text *) "select teller_id,teller_no,teller_type,skill_group_id,teller_name,user_pwd,"
        "area_code,phone_num, ip, org_id FROM vtc_teller";      // 远程登录密码改为非配置, AMS产生
    OCIDefine    *definehp[11];
    sb2 defineInd[11];        //OCI_IND_NULL
    char sErrorMsg[512];
    ub4    ub4RecordNo = 1; 
	sb4    sb4ErrorCode; 
    

    ret=OCIStmtPrepare(m_stmthp[iThread], m_errhp[iThread], (text*)szSqlStr, (ub4)strlen(szSqlStr),
        (ub4) OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);
    
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSTELLERCFG_ITEM_TELLERID],m_errhp[iThread],1,
        &terllerCfg.TellerId,sizeof(terllerCfg.TellerId), SQLT_INT,(dvoid *)&defineInd[AMSTELLERCFG_ITEM_TELLERID], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSTELLERCFG_ITEM_TELLERNO],m_errhp[iThread],2,
        &terllerCfg.TellerNo,sizeof(terllerCfg.TellerNo), SQLT_STR,(dvoid *)&defineInd[AMSTELLERCFG_ITEM_TELLERNO], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSTELLERCFG_ITEM_TELLERTYPE],m_errhp[iThread],3,
        &terllerCfg.TellerType,sizeof(terllerCfg.TellerType), SQLT_INT,(dvoid *)&defineInd[AMSTELLERCFG_ITEM_TELLERTYPE], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSTELLERCFG_ITEM_TELLERSRVGRPID],m_errhp[iThread],4,
        &terllerCfg.TellerSrvGrpId,sizeof(terllerCfg.TellerSrvGrpId), SQLT_INT,(dvoid *)&defineInd[AMSTELLERCFG_ITEM_TELLERSRVGRPID], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSTELLERCFG_ITEM_TELLERNAME],m_errhp[iThread],5,
        &terllerCfg.TellerName,sizeof(terllerCfg.TellerName), SQLT_STR,(dvoid *)&defineInd[AMSTELLERCFG_ITEM_TELLERNAME], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSTELLERCFG_ITEM_TELLERPWD],m_errhp[iThread],6,
        &terllerCfg.TellerPwd,sizeof(terllerCfg.TellerPwd), SQLT_STR,(dvoid *)&defineInd[AMSTELLERCFG_ITEM_TELLERPWD], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSTELLERCFG_ITEM_TELLERAREACODE],m_errhp[iThread],7,
        &terllerCfg.TellerAreaCode,sizeof(terllerCfg.TellerAreaCode), SQLT_STR,(dvoid *)&defineInd[AMSTELLERCFG_ITEM_TELLERAREACODE], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSTELLERCFG_ITEM_TELLERPHONENAME],m_errhp[iThread],8,
        &terllerCfg.TellerPhoneNum,sizeof(terllerCfg.TellerPhoneNum), SQLT_STR,(dvoid *)&defineInd[AMSTELLERCFG_ITEM_TELLERPHONENAME], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSTELLERCFG_ITEM_TELLERIP],m_errhp[iThread],9,
        &terllerCfg.TellerIp,sizeof(terllerCfg.TellerIp), SQLT_STR,(dvoid *)&defineInd[AMSTELLERCFG_ITEM_TELLERIP], NULL, NULL, OCI_DEFAULT);
    /*ret=OCIDefineByPos(m_stmthp[iThread],&definehp[109],m_errhp[iThread],11,
        &terllerCfg.TellerRemCoopPwd,sizeof(terllerCfg.TellerRemCoopPwd), SQLT_STR,(dvoid *)&defineInd[10], NULL, NULL, OCI_DEFAULT);
    terllerCfg.flag[AMSTELLERCFG_ITEM_TELLERREMCOOPPWD] = 1; 远程登录密码改为非配置, AMS产生*/
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSTELLERCFG_ITEM_TELLERORGIND],m_errhp[iThread],10,
        &terllerCfg.TellerOrgInd,sizeof(terllerCfg.TellerOrgInd), SQLT_INT,(dvoid *)&defineInd[AMSTELLERCFG_ITEM_TELLERORGIND], NULL, NULL, OCI_DEFAULT);

    
    if (OCI_SUCCESS !=(ret=OCIStmtExecute(m_svchp[iThread], m_stmthp[iThread], m_errhp[iThread], 
        (ub4) 0, (ub4) 0,(OCISnapshot *) NULL,(OCISnapshot *) NULL, (ub4)OCI_DEFAULT)))
    {
        OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
			dbgprint("AmsLoadTellerInfo sql OCIStmtExecute failed,error msg:%d, %s,...\r\nsql:%s", sb4ErrorCode, sErrorMsg,szSqlStr);
        return AMS_ERROR;
    }
    
    added = 0;
    while((ret=OCIStmtFetch2(m_stmthp[iThread],m_errhp[iThread],1,OCI_FETCH_NEXT,1,OCI_DEFAULT))!=OCI_NO_DATA)
    {
        if(ret == OCI_ERROR)
		{
			OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
			dbgprint("AmsLoadTellerInfo sql OCIStmtFetch2 failed,error msg:%d, %s,...", sb4ErrorCode, sErrorMsg);
			continue;
		}
        memset(terllerCfg.flag,1,sizeof(terllerCfg.flag));
        for (item=AMSTELLERCFG_ITEM_TELLERID; item < AMSTELLERCFG_ITEM_MAX; item++)  //check null value
        {
            if (-1 == defineInd[item])  //The selected value is null, and the value of the output variable is unchanged.
            {
                terllerCfg.flag[item] = 0;
            }
        } 
        if (terllerCfg.flag[AMSTELLERCFG_ITEM_TELLERNAME])
        {
        OAInConv(terllerCfg.TellerName, sizeof(terllerCfg.TellerName));
        }        
        if (AmsAddTeller(&terllerCfg, &addresult,addRltStr, sizeof(addRltStr)) != AMS_OK)
        {
            dbgprint("AmsAddTeller Fail %d, %s",addresult,addRltStr);
        }
        else
        {
            added++;
        }
    }
    dbgprint("AmsLoadTellerInfo ADDED=%d", added);
    return AMS_OK;
}

int AmsLoadVtmInfo(int iThread)
{
    OA_AMS_VTM_CONFIG_t vtmCfg;    
    int addresult,added;
    char addRltStr[10];

    int ret,item;
    char *szSqlStr = (text *) "select vtm_id,vtm_no,vtm_name,vtm_pwd,ip,remote_port,org_id FROM vtc_vtm";// 远程登录密码改为非配置, AMS产生*/
    OCIDefine    *definehp[AMSVTMCFG_ITEM_MAX];
    sb2 defineInd[AMSVTMCFG_ITEM_MAX];        //OCI_IND_NULL
    char sErrorMsg[512];
    ub4    ub4RecordNo = 1; 
	sb4    sb4ErrorCode; 
    

    ret=OCIStmtPrepare(m_stmthp[iThread], m_errhp[iThread], (text*)szSqlStr, (ub4)strlen(szSqlStr),
        (ub4) OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);
    
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSVTMCFG_ITEM_VTMID],m_errhp[iThread],1,
        &vtmCfg.VtmId,sizeof(vtmCfg.VtmId), SQLT_INT,(dvoid *)&defineInd[AMSVTMCFG_ITEM_VTMID], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSVTMCFG_ITEM_VTMNO],m_errhp[iThread],2,
        &vtmCfg.VtmNo,sizeof(vtmCfg.VtmNo), SQLT_STR,(dvoid *)&defineInd[AMSVTMCFG_ITEM_VTMNO], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSVTMCFG_ITEM_VTMNAME],m_errhp[iThread],3,
        &vtmCfg.VtmName,sizeof(vtmCfg.VtmName), SQLT_STR,(dvoid *)&defineInd[AMSVTMCFG_ITEM_VTMNAME], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSVTMCFG_ITEM_VTMPWD],m_errhp[iThread],4,
        &vtmCfg.VtmPwd,sizeof(vtmCfg.VtmPwd), SQLT_STR,(dvoid *)&defineInd[AMSVTMCFG_ITEM_VTMPWD], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSVTMCFG_ITEM_VTMIP],m_errhp[iThread],5,
        &vtmCfg.VtmIp,sizeof(vtmCfg.VtmIp), SQLT_STR,(dvoid *)&defineInd[AMSVTMCFG_ITEM_VTMIP], NULL, NULL, OCI_DEFAULT);
    /*ret=OCIDefineByPos(m_stmthp[iThread],&definehp[5],m_errhp[iThread],6,
        &vtmCfg.VtmRemCoopPwd,sizeof(vtmCfg.VtmRemCoopPwd), SQLT_STR,(dvoid *)&defineInd[5], NULL, NULL, OCI_DEFAULT);
    vtmCfg.flag[AMSVTMCFG_ITEM_VTMREMCOOPPWD] = 1; 远程登录密码改为非配置, AMS产生*/
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSVTMCFG_ITEM_VTMREMCOOPPORT],m_errhp[iThread],6,
        &vtmCfg.VtmRemCoopPort,sizeof(vtmCfg.VtmRemCoopPort), SQLT_INT,(dvoid *)&defineInd[AMSVTMCFG_ITEM_VTMREMCOOPPORT], NULL, NULL, OCI_DEFAULT);
    ret=OCIDefineByPos(m_stmthp[iThread],&definehp[AMSVTMCFG_ITEM_VTMORGID],m_errhp[iThread],7,
        &vtmCfg.VtmOrgid,sizeof(vtmCfg.VtmOrgid), SQLT_INT,(dvoid *)&defineInd[AMSVTMCFG_ITEM_VTMORGID], NULL, NULL, OCI_DEFAULT);
    
    if (OCI_SUCCESS !=(ret=OCIStmtExecute(m_svchp[iThread], m_stmthp[iThread], m_errhp[iThread], 
        (ub4) 0, (ub4) 0,(OCISnapshot *) NULL,(OCISnapshot *) NULL, (ub4)OCI_DEFAULT)))
    {
        OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
        dbgprint("AmsLoadVtmInfo sql OCIStmtExecute failed,error msg:%d, %s,...\r\nsql:%s", sb4ErrorCode, sErrorMsg,szSqlStr);
        return AMS_ERROR;
    }
    
    added = 0;
    while((ret=OCIStmtFetch2(m_stmthp[iThread],m_errhp[iThread],1,OCI_FETCH_NEXT,1,OCI_DEFAULT))!=OCI_NO_DATA)
    {
        if(ret == OCI_ERROR)
		{
			OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
			dbgprint("AmsLoadVtmInfo sql OCIStmtFetch2 failed,error msg:%d, %s,...", sb4ErrorCode, sErrorMsg);
			continue;
		}
        memset(vtmCfg.flag,1,sizeof(vtmCfg.flag));
        for (item=AMSVTMCFG_ITEM_VTMID; item < AMSVTMCFG_ITEM_MAX; item++)  //check null value
        {
            if (-1 == defineInd[item])  //The selected value is null, and the value of the output variable is unchanged.
            {
                vtmCfg.flag[item] = 0;
            }
        } 
        if (vtmCfg.flag[AMSVTMCFG_ITEM_VTMNAME])
        {
        OAInConv(vtmCfg.VtmName, sizeof(vtmCfg.VtmName));
        }
        if (AmsAddVtm(&vtmCfg, &addresult,addRltStr, sizeof(addRltStr)) != AMS_OK)
        {
            dbgprint("AmsAddVtm Fail %d, %s",addresult,addRltStr);
        }
        else
        {
            added++;
        }
    }
    dbgprint("AmsLoadVtmInfo ADDED=%d", added);
    return AMS_OK;
}

int AmsUpdateCfgState(int iThread, int cfgstatus)
{
    int ret;    
    char szSqlStr[128];
    OCIDefine    *definehp[1];
    sword defineInd[1];        //OCI_IND_NULL
    char sErrorMsg[512];
	sb4    sb4ErrorCode; 
    ub4    ub4RecordNo;
    
    snprintf(szSqlStr, sizeof(szSqlStr),"update vtc_system_conf set real_value='%d' where conf_key='%s'", cfgstatus,CfgStateFieldStr);
    ret=OCIStmtPrepare(m_stmthp[iThread], m_errhp[iThread], (text*)szSqlStr, (ub4)strlen(szSqlStr),
        (ub4) OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);
    if (OCI_SUCCESS !=(ret=OCIStmtExecute(m_svchp[iThread], m_stmthp[iThread], m_errhp[iThread], 
        (ub4) 1, (ub4) 0,(OCISnapshot *) NULL,(OCISnapshot *) NULL, (ub4)OCI_COMMIT_ON_SUCCESS)))
    {
        OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
        dbgprint("AmsJudgeCfgState sql OCIStmtExecute failed,error msg:%d, %s,...\r\nsql:%s", sb4ErrorCode, sErrorMsg,szSqlStr);
        return AMS_ERROR;
    }        
    return AMS_OK;
}

//-------------------------------------------------------------------------
int AmsSendtellerStateToOms(int iThreadId, TELLER_STATE_DB_INFO *pTellerStateDbInfo)
{
    unsigned char buf[sizeof(unsigned int)+sizeof(TELLER_STATE_DB_INFO)];
    
    if (-1 == SystemData.AmsPriData.amsDbopr.updateInsertCbCode)
    {
        return AMS_ERROR;
    }

    *((unsigned int*)buf) = TELLERSTATEDBINFO_E;
    memcpy(&buf[sizeof(unsigned int)], pTellerStateDbInfo, sizeof(TELLER_STATE_DB_INFO));
    SendDbOpr(SystemData.AmsPriData.amsDbopr.updateInsertCbCode,
        buf, sizeof(unsigned int)+sizeof(TELLER_STATE_DB_INFO));
    
    return AMS_OK;
}

int AmsSendvtmStateToOms(int iThreadId, VTM_STATE_DB_INFO *pVtmStateDbInfo)
{
    unsigned char buf[sizeof(unsigned int)+sizeof(VTM_STATE_DB_INFO)];
    
    if (-1 == SystemData.AmsPriData.amsDbopr.updateInsertCbCode)
    {
        return AMS_ERROR;
    }

    *((unsigned int*)buf) = VTMSTATEDBINFO_E;
    memcpy(&buf[sizeof(unsigned int)], pVtmStateDbInfo, sizeof(VTM_STATE_DB_INFO));
    SendDbOpr(SystemData.AmsPriData.amsDbopr.updateInsertCbCode,
        buf, sizeof(unsigned int)+sizeof(VTM_STATE_DB_INFO));
    
    return AMS_OK;
}

int AmsSendSdrToOms(int iThreadId, AMS_SDR *pAmsSdr)
{
    unsigned char buf[sizeof(unsigned int)+sizeof(AMS_SDR)];
    
    if (-1 == SystemData.AmsPriData.amsDbopr.updateInsertCbCode)
    {
		dbgprint("AmsSendSdrToOms Para[%d] Err", SystemData.AmsPriData.amsDbopr.updateInsertCbCode);	
        return AMS_ERROR;
    }

    *((unsigned int*)buf) = AMSSDRDBINFO_E;
    memcpy(&buf[sizeof(unsigned int)], pAmsSdr, sizeof(AMS_SDR));
    SendDbOpr(SystemData.AmsPriData.amsDbopr.updateInsertCbCode,
        buf, sizeof(unsigned int)+sizeof(AMS_SDR));
    
    return AMS_OK;
}    

int AmsSendTellerRealNetFlowToOms(int iThreadId, TELLER_RNF_DB_INFO *ptellerRealNetFlowDbInfo)
{
    unsigned char buf[sizeof(unsigned int)+sizeof(TELLER_RNF_DB_INFO)];
    
    if (-1 == SystemData.AmsPriData.amsDbopr.updateInsertCbCode)
    {
        return AMS_ERROR;
    }

    *((unsigned int*)buf) = TELLERRNFDBINFO_E;
    memcpy(&buf[sizeof(unsigned int)], ptellerRealNetFlowDbInfo, sizeof(TELLER_RNF_DB_INFO));
    SendDbOpr(SystemData.AmsPriData.amsDbopr.updateInsertCbCode,
        buf, sizeof(unsigned int)+sizeof(TELLER_RNF_DB_INFO));
    
    return AMS_OK;
}

int AmsSendVtmRealNetFlowToOms(int iThreadId, VTM_RNF_DB_INFO *pvtmRealNetFlowDbInfo)
{
    unsigned char buf[sizeof(unsigned int)+sizeof(VTM_RNF_DB_INFO)];
    
    if (-1 == SystemData.AmsPriData.amsDbopr.updateInsertCbCode)
    {
        return AMS_ERROR;
    }

    *((unsigned int*)buf) = VTMRNFDBINFO_E;
    memcpy(&buf[sizeof(unsigned int)], pvtmRealNetFlowDbInfo, sizeof(VTM_RNF_DB_INFO));
    SendDbOpr(SystemData.AmsPriData.amsDbopr.updateInsertCbCode,
        buf, sizeof(unsigned int)+sizeof(VTM_RNF_DB_INFO));
    
    return AMS_OK;
}

int AmsFillUpdateTellerStateInfo(TELLER_STATE_DB_INFO *info, char *buf, int bufLen)
{
    int pos = 0;
    
    if (0 == info->tellerIdFlag)
        return AMS_ERROR;
    
    pos += snprintf(&buf[pos], bufLen-pos, "update vtc_teller_state set ");
    if (info->tellerStateFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "teller_state=%u,", info->tellerState);
    }
    if (info->audioRecStateFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "audrec_state=%u,", info->audioRecState);
    }
    if (info->screenRecStateFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "scrrec_state=%u,", info->screenRecState);
    }
    if (info->remCoopStateFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "remcoop_state=%u,", info->remCoopState);
    }
    if (info->snapStateFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "snap_state=%u,", info->snapState);
    }
    if (info->startTimeFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "start_time=to_date('%s','YYYY-MM-DD HH24:MI:SS'),", info->startTime);
    }
    if (0 == pos || pos >= bufLen)
        return AMS_ERROR;

    //覆盖最后的,
    pos += snprintf(&buf[pos-1], bufLen-pos+1, " where teller_id=%u",info->tellerId);    

    if (pos >= bufLen)
        return AMS_ERROR;

    return AMS_OK;
}

int AmsFillInsertTellerStateInfo(TELLER_STATE_DB_INFO *info, char *buf, int bufLen)
{
    int pos = 0;
    
    if (0 == info->tellerIdFlag)
        return AMS_ERROR;
    
    pos += snprintf(&buf[pos], bufLen-pos, "insert into vtc_teller_state(teller_id,");
    if (info->tellerStateFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "teller_state,");
    }
    if (info->audioRecStateFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "audrec_state,");
    }
    if (info->screenRecStateFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "scrrec_state,");
    }
    if (info->remCoopStateFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "remcoop_state,");
    }
    if (info->snapStateFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "snap_state,");
    }
    if (info->startTimeFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "start_time,");
    }
    //覆盖最后的,
    if (pos < bufLen)
    {
        pos = pos -1 + snprintf(&buf[pos-1], bufLen-pos+1, ") VALUES(%u,",info->tellerId);
    }

    if (info->tellerStateFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->tellerState);
    }
    if (info->audioRecStateFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->audioRecState);
    }
    if (info->screenRecStateFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->screenRecState);
    }
    if (info->remCoopStateFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->remCoopState);
    }
    if (info->snapStateFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->snapState);
    }
    if (info->startTimeFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "to_date('%s','YYYY-MM-DD HH24:MI:SS'),", info->startTime);
    }
    //覆盖最后的,
    if (pos < bufLen)
    {
        pos = pos -1 + snprintf(&buf[pos-1], bufLen-pos+1, ")");
    }

    if (pos >= bufLen)
        return AMS_ERROR;

    return AMS_OK;
}

int AmsFillUpdateVtmStateInfo(VTM_STATE_DB_INFO *info, char *buf, int bufLen)
{
    int pos = 0;

    if (0 == info->vtmIdFlag)
        return AMS_ERROR;
    
    pos += snprintf(&buf[pos], bufLen-pos, "update vtc_vtm_state set ");
    if (info->vtmStateFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "vtm_state=%u,", info->vtmState);
    }
    if (info->callIdFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "call_id='%s',", info->callId);
    }
    if (info->startTimeFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "start_time=to_date('%s','YYYY-MM-DD HH24:MI:SS'),", info->startTime);
    }
    if (0 == pos || pos >= bufLen)
        return AMS_ERROR;

    //覆盖最后的,
    if (pos < bufLen)
    {
        pos += snprintf(&buf[pos-1], bufLen-pos+1, " where vtm_id=%u",info->vtmId);    
    }

    if (pos >= bufLen)
        return AMS_ERROR;

    return AMS_OK;
}

int AmsFillInsertVtmStateInfo(VTM_STATE_DB_INFO *info, char *buf, int bufLen)
{
    int pos = 0;
    
    if (0 == info->vtmIdFlag)
        return AMS_ERROR;
    
    pos += snprintf(&buf[pos], bufLen-pos, "insert into vtc_vtm_state(vtm_id,");
    if (info->vtmStateFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "vtm_state,");
    }
    if (info->callIdFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "call_id,");
    }
    if (info->startTimeFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "start_time,");
    }
    //覆盖最后的,
    if (pos < bufLen)
    {
        pos = pos -1 + snprintf(&buf[pos-1], bufLen-pos+1, ") VALUES(%u,",info->vtmId);
    }

    if (info->vtmStateFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->vtmState);
    }
    if (info->callIdFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "'%s',", info->callId);
    }
    if (info->startTimeFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "to_date('%s','YYYY-MM-DD HH24:MI:SS'),", info->startTime);
    }
    //覆盖最后的,
    if (pos < bufLen)
    {
        pos = pos -1 + snprintf(&buf[pos-1], bufLen-pos+1, ")");
    }
    if (pos >= bufLen)
        return AMS_ERROR;    

    return AMS_OK;
}

int AmsFillUpdateAmsSdrInfo(AMS_SDR *info, char *buf, int bufLen)
{
    int pos = 0;
    char tmpStr[128];

    if (0 == info->callIdFlag)
        return AMS_ERROR;
    
    pos += snprintf(&buf[pos], bufLen-pos, "update vtc_call_srv_dtl set ");
    if (info->tellerIdFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "teller_id=%u,", info->tellerId);
    }
    if (info->tellerNoFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "teller_no='%s',", info->tellerNo);
    }
    if (info->tellerNameFlag && pos < bufLen)
    {        
        /*snprintf(tmpStr, sizeof(tmpStr), "teller_name='%s',", info->tellerName);
        if (-1 == g2u(tmpStr, strlen(tmpStr), &buf[pos], bufLen-pos))
            return AMS_ERROR;
        pos = strlen(buf);*/
        pos += snprintf(&buf[pos], bufLen-pos, "teller_name='%s',", info->tellerName);
    }
    if (info->tellerIpFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "teller_ip='%s',", info->tellerIp);
    }
    if (info->vtmIdFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "vtm_id=%u,", info->vtmId);
    }
    if (info->vtmNoFlag && pos < bufLen)
    {        
        /*snprintf(tmpStr, sizeof(tmpStr), "vtm_no='%s',", info->vtmNo);
        if (-1 == g2u(tmpStr, strlen(tmpStr), &buf[pos], bufLen-pos))
            return AMS_ERROR;
        pos = strlen(buf);*/
        pos += snprintf(&buf[pos], bufLen-pos, "vtm_no='%s',", info->vtmNo);
    }
    if (info->vtmIpFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "vtm_ip='%s',", info->vtmIp);
    }
    if (info->skillGroupIdFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "skill_group_id=%u,", info->skillGroupId);
    }
    if (info->skillGroupNameFlag && pos < bufLen)
    {        
        /*snprintf(tmpStr, sizeof(tmpStr), "skill_group_name='%s',", info->skillGroupName);
        if (-1 == g2u(tmpStr, strlen(tmpStr), &buf[pos], bufLen-pos))
            return AMS_ERROR;
        pos = strlen(buf);*/
        pos += snprintf(&buf[pos], bufLen-pos, "skill_group_name='%s',", info->skillGroupName);
    }
    if (info->audioRecNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "audrec_num=%u,", info->audioRecNum);
    }
    if (info->audioRecFailNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "audrec_fail_num=%u,", info->audioRecFailNum);
    }
    if (info->screenRecNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "scrrec_num=%u,", info->screenRecNum);
    }
    if (info->screenRecFailNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "scrrec_fail_num=%u,", info->screenRecFailNum);
    }
    if (info->remCoopNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "remcoop_num=%u,", info->remCoopNum);
    }
    if (info->remCoopFailNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "remcoop_fail_num=%u,", info->remCoopFailNum);
    }
    if (info->snapNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "snap_num=%u,", info->snapNum);
    }
    if (info->snapFailNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "snap_fail_num=%u,", info->snapFailNum);
    }
    if (info->scoreFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "cust_score=%u,", info->score);
    }
    if (info->txSessionFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "tx_session_flow=%u,", info->txSessionFlow);
    }
    if (info->rxSessionFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "rx_session_flow=%u,", info->rxSessionFlow);
    }
    if (info->txFileFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "tx_file_flow=%u,", info->txFileFlow);
    }
    if (info->rxFileFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "rx_file_flow=%u,", info->rxFileFlow);
    }
    if (info->txDesktopFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "tx_desktop_flow=%u,", info->txDesktopFlow);
    }
    if (info->rxSessionFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "rx_desktop_flow=%u,", info->rxDesktopFlow);
    }
    if (info->vtmTxSessionFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "vtm_tx_session_flow=%u,", info->vtmTxSessionFlow);
    }
    if (info->vtmRxSessionFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "vtm_rx_session_flow=%u,", info->vtmRxSessionFlow);
    }
    if (info->vtmTxFileFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "vtm_tx_file_flow=%u,", info->vtmTxFileFlow);
    }
    if (info->vtmRxFileFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "vtm_rx_file_flow=%u,", info->vtmRxFileFlow);
    }
    if (info->vtmTxDesktopFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "vtm_tx_desktop_flow=%u,", info->vtmTxDesktopFlow);
    }
    if (info->vtmRxDesktopFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "vtm_rx_desktop_flow=%u,", info->vtmRxDesktopFlow);
    }
    if (0 == pos || pos >= bufLen)
        return AMS_ERROR;

    //覆盖最后的,
    pos += snprintf(&buf[pos-1], bufLen-pos+1, " where call_id='%s'",info->callId);    

    if (pos >= bufLen)
        return AMS_ERROR;

    return AMS_OK;
}

int AmsFillInsertAmsSdrInfo(AMS_SDR *info, char *buf, int bufLen)
{
    int pos = 0;
    char tmpStr[128];
    
    if (0 == info->callIdFlag)
        return AMS_ERROR;
    
    pos += snprintf(&buf[pos], bufLen-pos, "insert into vtc_call_srv_dtl(call_id,");
    if (info->tellerIdFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "teller_id,");
    }
    if (info->tellerNoFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "teller_no,");
    }
    if (info->tellerNameFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "teller_name,");
    }
    if (info->tellerIpFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "teller_ip,");
    }
    if (info->vtmIdFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "vtm_id,");
    }
    if (info->vtmNoFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "vtm_no,");
    }
    if (info->vtmIpFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "vtm_ip,");
    }
    if (info->skillGroupIdFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "skill_group_id,");
    }
    if (info->skillGroupNameFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "skill_group_name,");
    }
    if (info->audioRecNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "audrec_num,");
    }
    if (info->audioRecFailNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "audrec_fail_num,");
    }
    if (info->screenRecNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "scrrec_num,");
    }
    if (info->screenRecFailNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "scrrec_fail_num,");
    }
    if (info->remCoopNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "remcoop_num,");
    }
    if (info->remCoopFailNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "remcoop_fail_num,");
    }
    if (info->snapNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "snap_num,");
    }
    if (info->snapFailNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "snap_fail_num,");
    }
    if (info->scoreFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "cust_score,");
    }
    if (info->txSessionFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "tx_session_flow,");
    }
    if (info->rxSessionFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "rx_session_flow,");
    }
    if (info->txFileFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "tx_file_flow,");
    }
    if (info->rxFileFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "rx_file_flow,");
    }
    if (info->txDesktopFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "tx_desktop_flow,");
    }
    if (info->rxSessionFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "rx_desktop_flow,");
    }
    if (info->vtmTxSessionFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "vtm_tx_session_flow,");
    }
    if (info->vtmRxSessionFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "vtm_rx_session_flow,");
    }
    if (info->vtmTxFileFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "vtm_tx_file_flow,");
    }
    if (info->vtmRxFileFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "vtm_rx_file_flow,");
    }
    if (info->vtmTxDesktopFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "vtm_tx_desktop_flow,");
    }
    if (info->vtmRxDesktopFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "vtm_rx_desktop_flow,");
    }
    //覆盖最后的,
    if (pos < bufLen)
    {
        pos = pos -1 + snprintf(&buf[pos-1], bufLen-pos+1, ") VALUES('%s',",info->callId);
    }

    if (info->tellerIdFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->tellerId);
    }
    if (info->tellerNoFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "'%s',", info->tellerNo);
    }
    if (info->tellerNameFlag && pos < bufLen)
    {        
        /*snprintf(tmpStr, sizeof(tmpStr), "'%s',", info->tellerName);
        if (-1 == g2u(tmpStr, strlen(tmpStr), &buf[pos], bufLen-pos))
            return AMS_ERROR;
        pos = strlen(buf);*/
        pos += snprintf(&buf[pos], bufLen-pos, "'%s',", info->tellerName);
    }
    if (info->tellerIpFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "'%s',", info->tellerIp);
    }
    if (info->vtmIdFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->vtmId);
    }
    if (info->vtmNoFlag && pos < bufLen)
    {        
        /*snprintf(tmpStr, sizeof(tmpStr), "'%s',", info->vtmNo);
        if (-1 == g2u(tmpStr, strlen(tmpStr), &buf[pos], bufLen-pos))
            return AMS_ERROR;
        pos = strlen(buf);*/
        pos += snprintf(&buf[pos], bufLen-pos, "'%s',", info->vtmNo);
    }
    if (info->vtmIpFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "'%s',", info->vtmIp);
    }
    if (info->skillGroupIdFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->skillGroupId);
    }
    if (info->skillGroupNameFlag && pos < bufLen)
    {        
        /*snprintf(tmpStr, sizeof(tmpStr), "'%s',", info->skillGroupName);
        if (-1 == g2u(tmpStr, strlen(tmpStr), &buf[pos], bufLen-pos))
            return AMS_ERROR;
        pos = strlen(buf);*/
        pos += snprintf(&buf[pos], bufLen-pos, "'%s',", info->skillGroupName);
    }
    if (info->audioRecNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->audioRecNum);
    }
    if (info->audioRecFailNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->audioRecFailNum);
    }
    if (info->screenRecNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->screenRecNum);
    }
    if (info->screenRecFailNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->screenRecFailNum);
    }
    if (info->remCoopNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->remCoopNum);
    }
    if (info->remCoopFailNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->remCoopFailNum);
    }
    if (info->snapNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->snapNum);
    }
    if (info->snapFailNumFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->snapFailNum);
    }
    if (info->scoreFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->score);
    }
    if (info->txSessionFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->txSessionFlow);
    }
    if (info->rxSessionFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->rxSessionFlow);
    }
    if (info->txFileFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->txFileFlow);
    }
    if (info->rxFileFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->rxFileFlow);
    }
    if (info->txDesktopFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->txDesktopFlow);
    }
    if (info->rxSessionFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->rxDesktopFlow);
    }
    if (info->vtmTxSessionFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->vtmTxSessionFlow);
    }
    if (info->vtmRxSessionFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->vtmRxSessionFlow);
    }
    if (info->vtmTxFileFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->vtmTxFileFlow);
    }
    if (info->vtmRxFileFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->vtmRxFileFlow);
    }
    if (info->vtmTxDesktopFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->vtmTxDesktopFlow);
    }
    if (info->vtmRxDesktopFlowFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->vtmRxDesktopFlow);
    }

    //覆盖最后的,
    if (pos < bufLen)
    {
        pos = pos -1 + snprintf(&buf[pos-1], bufLen-pos+1, ")");
    }

    if (pos >= bufLen)
        return AMS_ERROR;    

    return AMS_OK;
}

int AmsFillUpdateTellerRealNetFlowInfo(TELLER_RNF_DB_INFO *info, char *buf, int bufLen)
{
    int pos = 0;
    
    if (0 == info->tellerIdFlag)
        return AMS_ERROR;
    
    pos += snprintf(&buf[pos], bufLen-pos, "update vtc_teller_netflow_real set ");
    if (info->callIdFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "call_id='%s',", info->callId);
    }
    if (info->txBitUsedFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "tx_bit_used=%u,", info->txBitUsed);
    }
    if (info->rxBitUsedFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "rx_bit_used=%u,", info->rxBitUsed);
    }
    if (info->durationFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "duration=%u,", info->duration);
    }
    if (info->startTimeFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "start_time=to_date('%s','YYYY-MM-DD HH24:MI:SS'),", info->startTime);
    }
    if (0 == pos || pos >= bufLen)
        return AMS_ERROR;

    //覆盖最后的,
    pos += snprintf(&buf[pos-1], bufLen-pos+1, " where teller_id=%u",info->tellerId);    

    if (pos >= bufLen)
        return AMS_ERROR;

    return AMS_OK;
}

int AmsFillInsertTellerRealNetFlowInfo(TELLER_RNF_DB_INFO *info, char *buf, int bufLen)
{
    int pos = 0;
    
    if (0 == info->tellerIdFlag)
        return AMS_ERROR;
    
    pos += snprintf(&buf[pos], bufLen-pos, "insert into vtc_teller_netflow_real(teller_id,");
    if (info->callIdFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "call_id,");
    }
    if (info->txBitUsedFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "tx_bit_used,");
    }
    if (info->rxBitUsedFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "rx_bit_used,");
    }
    if (info->durationFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "duration,");
    }
    if (info->startTimeFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "start_time,");
    }
    //覆盖最后的,
    if (pos < bufLen)
    {
        pos = pos -1 + snprintf(&buf[pos-1], bufLen-pos+1, ") VALUES(%u,",info->tellerId);
    }

    if (info->callIdFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "'%s',", info->callId);
    }
    if (info->txBitUsedFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->txBitUsed);
    }
    if (info->rxBitUsedFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->rxBitUsed);
    }
    if (info->durationFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->duration);
    }
    if (info->startTimeFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "to_date('%s','YYYY-MM-DD HH24:MI:SS'),", info->startTime);
    }
    //覆盖最后的,
    if (pos < bufLen)
    {
        pos = pos -1 + snprintf(&buf[pos-1], bufLen-pos+1, ")");
    }

    if (pos >= bufLen)
        return AMS_ERROR;

    return AMS_OK;
}

int AmsFillUpdateVtmRealNetFlowInfo(VTM_RNF_DB_INFO *info, char *buf, int bufLen)
{
    int pos = 0;
    
    if (0 == info->vtmIdFlag)
        return AMS_ERROR;
    
    pos += snprintf(&buf[pos], bufLen-pos, "update vtc_vtm_netflow_real set ");
    if (info->callIdFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "call_id='%s',", info->callId);
    }
    if (info->txBitUsedFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "tx_bit_used=%u,", info->txBitUsed);
    }
    if (info->rxBitUsedFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "rx_bit_used=%u,", info->rxBitUsed);
    }
    if (info->durationFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "duration=%u,", info->duration);
    }
    if (info->startTimeFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "start_time=to_date('%s','YYYY-MM-DD HH24:MI:SS'),", info->startTime);
    }
    if (0 == pos || pos >= bufLen)
        return AMS_ERROR;

    //覆盖最后的,
    pos += snprintf(&buf[pos-1], bufLen-pos+1, " where vtm_id=%u",info->vtmId);    

    if (pos >= bufLen)
        return AMS_ERROR;

    return AMS_OK;
}

int AmsFillInsertVtmRealNetFlowInfo(VTM_RNF_DB_INFO *info, char *buf, int bufLen)
{
    int pos = 0;
    
    if (0 == info->vtmIdFlag)
        return AMS_ERROR;
    
    pos += snprintf(&buf[pos], bufLen-pos, "insert into vtc_vtm_netflow_real(vtm_id,");
    if (info->callIdFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "call_id,");
    }
    if (info->txBitUsedFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "tx_bit_used,");
    }
    if (info->rxBitUsedFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "rx_bit_used,");
    }
    if (info->durationFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "duration,");
    }
    if (info->startTimeFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "start_time,");
    }
    //覆盖最后的,
    if (pos < bufLen)
    {
        pos = pos -1 + snprintf(&buf[pos-1], bufLen-pos+1, ") VALUES(%u,",info->vtmId);
    }

    if (info->callIdFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "'%s',", info->callId);
    }
    if (info->txBitUsedFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->txBitUsed);
    }
    if (info->rxBitUsedFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->rxBitUsed);
    }
    if (info->durationFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "%u,", info->duration);
    }
    if (info->startTimeFlag && pos < bufLen)
    {        
        pos += snprintf(&buf[pos], bufLen-pos, "to_date('%s','YYYY-MM-DD HH24:MI:SS'),", info->startTime);
    }
    //覆盖最后的,
    if (pos < bufLen)
    {
        pos = pos -1 + snprintf(&buf[pos-1], bufLen-pos+1, ")");
    }

    if (pos >= bufLen)
        return AMS_ERROR;

    return AMS_OK;
}

int AmsFillUpdateDbInfo(unsigned char *content,unsigned int contentLen,char *buf, int bufLen)
{
    unsigned int type;

    if (contentLen < sizeof(unsigned int))
    {
        return AMS_ERROR;
    }

    type = *((unsigned int*)content);
    switch(type)
    {
    case TELLERSTATEDBINFO_E://实时监控-柜员状态
        if (contentLen < sizeof(unsigned int) + sizeof(TELLER_STATE_DB_INFO))
            return AMS_ERROR;
        return AmsFillUpdateTellerStateInfo((TELLER_STATE_DB_INFO*)&content[sizeof(unsigned int)],buf,bufLen);
        
    case VTMSTATEDBINFO_E://实时监控-柜员机状态
        if (contentLen < sizeof(unsigned int) + sizeof(VTM_STATE_DB_INFO))
            return AMS_ERROR;
        return AmsFillUpdateVtmStateInfo((VTM_STATE_DB_INFO*)&content[sizeof(unsigned int)],buf,bufLen);
         
    case AMSSDRDBINFO_E://统计查询-业务详单
        if (contentLen < sizeof(unsigned int) + sizeof(AMS_SDR))
            return AMS_ERROR;
        return AmsFillUpdateAmsSdrInfo((AMS_SDR*)&content[sizeof(unsigned int)],buf,bufLen);

    case TELLERRNFDBINFO_E://实时监控-柜员实时流量监控
        if (contentLen < sizeof(unsigned int) + sizeof(TELLER_RNF_DB_INFO))
            return AMS_ERROR;
        return AmsFillUpdateTellerRealNetFlowInfo((TELLER_RNF_DB_INFO*)&content[sizeof(unsigned int)],buf,bufLen);

    case VTMRNFDBINFO_E://实时监控-柜员机实时流量监控
        if (contentLen < sizeof(unsigned int) + sizeof(VTM_RNF_DB_INFO))
            return AMS_ERROR;
        return AmsFillUpdateVtmRealNetFlowInfo((VTM_RNF_DB_INFO*)&content[sizeof(unsigned int)],buf,bufLen);        

    default:
        return AMS_ERROR;
    }
}

int AmsFillInsertDbInfo(unsigned char *content,unsigned int contentLen,char *buf, int bufLen)
{
    unsigned int type;

    if (contentLen < sizeof(unsigned int))
    {
        return AMS_ERROR;
    }

    type = *((unsigned int*)content);
    switch(type)
    {
    case TELLERSTATEDBINFO_E://实时监控-柜员状态
        if (contentLen < sizeof(unsigned int) + sizeof(TELLER_STATE_DB_INFO))
            return AMS_ERROR;
        return AmsFillInsertTellerStateInfo((TELLER_STATE_DB_INFO*)&content[sizeof(unsigned int)],buf,bufLen);
        
    case VTMSTATEDBINFO_E://实时监控-柜员机状态
        if (contentLen < sizeof(unsigned int) + sizeof(VTM_STATE_DB_INFO))
            return AMS_ERROR;
        return AmsFillInsertVtmStateInfo((VTM_STATE_DB_INFO*)&content[sizeof(unsigned int)],buf,bufLen);
         
    case AMSSDRDBINFO_E://统计查询-业务详单
        if (contentLen < sizeof(unsigned int) + sizeof(AMS_SDR))
            return AMS_ERROR;
        return AmsFillInsertAmsSdrInfo((AMS_SDR*)&content[sizeof(unsigned int)],buf,bufLen);

    case TELLERRNFDBINFO_E://实时监控-柜员实时流量监控
        if (contentLen < sizeof(unsigned int) + sizeof(TELLER_RNF_DB_INFO))
            return AMS_ERROR;
        return AmsFillInsertTellerRealNetFlowInfo((TELLER_RNF_DB_INFO*)&content[sizeof(unsigned int)],buf,bufLen);

    case VTMRNFDBINFO_E://实时监控-柜员机实时流量监控
        if (contentLen < sizeof(unsigned int) + sizeof(VTM_RNF_DB_INFO))
            return AMS_ERROR;
        return AmsFillInsertVtmRealNetFlowInfo((VTM_RNF_DB_INFO*)&content[sizeof(unsigned int)],buf,bufLen);        

    default:
        return AMS_ERROR;
    }
}
int AmsUpdateInsert2Db_cb(int iThread, unsigned char *content,unsigned int contentLen)
{
    unsigned char type;
    char szSqlStr[1024];
    char sErrorMsg[128];
    ub4    ub4RecordNo = 1; 
	sb4    sb4ErrorCode; 
    int ret;    
    int affectedRows = 0;

    if (AMS_OK != AmsFillUpdateDbInfo(content,contentLen,szSqlStr, sizeof(szSqlStr)))
    {
        return AMS_ERROR;
    }
    ret=OCIStmtPrepare(m_stmthp[iThread], m_errhp[iThread], (text*)szSqlStr, (ub4)strlen(szSqlStr),
        (ub4) OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);
    
    if(SystemData.DbprintFlag >= 2)
    {
        dbgprint("ithread %d, AmsUpdateInsert2Db_cb, execsql sql=%s\n",iThread, szSqlStr);
        dbsqlprint("sql=%s",szSqlStr);
    }
    if (OCI_SUCCESS !=(ret=OCIStmtExecute(m_svchp[iThread], m_stmthp[iThread], m_errhp[iThread], 
        (ub4) 1, (ub4) 0,(OCISnapshot *) NULL,(OCISnapshot *) NULL, (ub4)OCI_COMMIT_ON_SUCCESS)))
    {
        if(SystemData.DbprintFlag >= 1)
        {
        OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
		dbgprint("ithread %d, AmsUpdateInsert2Db_cb update sql failed,type:%d,error msg:%d, %s,...\r\nsql: %s", iThread,content[0],sb4ErrorCode, sErrorMsg ,szSqlStr);
        }
		return -1;
    }
    ret = OCIAttrGet(m_stmthp[iThread], OCI_HTYPE_STMT,&affectedRows, 0, OCI_ATTR_ROW_COUNT, m_errhp[iThread]);
    if (ret != OCI_SUCCESS)
    {
        if(SystemData.DbprintFlag >= 1)
        {
        OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
		dbgprint("ithread %d, AmsUpdateInsert2Db_cb OCIAttrGet failed,type:%d,error msg:%d, %s,...", iThread, content[0],sb4ErrorCode, sErrorMsg);
        }
		return -1;
    }
    if (affectedRows > 0)
    {        
        return 0;
    }
    if(SystemData.DbprintFlag >= 2)
        dbgprint("ithread %d, AmsUpdateInsert2Db_cb update affect %d rows,...", iThread, affectedRows);

    if (AMS_OK != AmsFillInsertDbInfo(content,contentLen,szSqlStr, sizeof(szSqlStr)))
    {
        return AMS_ERROR;
    }    
    ret=OCIStmtPrepare(m_stmthp[iThread], m_errhp[iThread], (text*)szSqlStr, (ub4)strlen(szSqlStr),
        (ub4) OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);

    if(SystemData.DbprintFlag >= 2)
    {
        dbgprint("ithread %d, AmsUpdateInsert2Db_cb, execsql sql=%s\n",iThread, szSqlStr);
        dbsqlprint("sql=%s",szSqlStr);
    }
    ret=OCIStmtExecute(m_svchp[iThread], m_stmthp[iThread], m_errhp[iThread], 
        (ub4) 1, (ub4) 0,(OCISnapshot *) NULL,(OCISnapshot *) NULL, (ub4)OCI_COMMIT_ON_SUCCESS);
    if (OCI_SUCCESS == ret)
    {
        return 0;
    }
    OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
    if (1 != sb4ErrorCode)  
	{
	    if(SystemData.DbprintFlag >= 1)
		dbgprint("ithread %d, AmsUpdateInsert2Db_cb insert sql failed,type:%d,error msg:%d, %s,...\r\nsql: %s", iThread, content[0],sb4ErrorCode, sErrorMsg ,szSqlStr);
		return -1;
    }

    //ORA-00001: unique constraint 可能别的线程抢先插入了，尝试再次update
    AmsFillUpdateDbInfo(content,contentLen,szSqlStr, sizeof(szSqlStr));
    ret=OCIStmtPrepare(m_stmthp[iThread], m_errhp[iThread], (text*)szSqlStr, (ub4)strlen(szSqlStr),
        (ub4) OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);
    
    if(SystemData.DbprintFlag >= 2)
    {
            dbgprint("ithread %d, AmsUpdateInsert2Db_cb, execsql sql=%s\n", iThread, szSqlStr);
        dbsqlprint("sql=%s",szSqlStr);
    }
    if (OCI_SUCCESS !=(ret=OCIStmtExecute(m_svchp[iThread], m_stmthp[iThread], m_errhp[iThread], 
        (ub4) 1, (ub4) 0,(OCISnapshot *) NULL,(OCISnapshot *) NULL, (ub4)OCI_COMMIT_ON_SUCCESS)))
    {
        if(SystemData.DbprintFlag >= 1)
        {
        OCIErrorGet(m_errhp[iThread], ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg, sizeof(sErrorMsg), OCI_HTYPE_ERROR);
		dbgprint("ithread %d, AmsUpdateInsert2Db_cb update sql failed,type:%d,error msg:%d, %s,...\r\nsql: %s", iThread, content[0],sb4ErrorCode, sErrorMsg ,szSqlStr);
        }
		return -1;
    }
    return 0;
}

//-------------------------------------------------------------------------
//初始下载数据库数据，使用第MAXDBCONN个
int AmsLoadDbData()
{
    int iThread = 0;        //zhuyn 20161103
    int iret;
    int cfgstatus;

    if (-1 == (SystemData.AmsPriData.amsDbopr.updateInsertCbCode = DbRegOprFun(AmsUpdateInsert2Db_cb)))
    {
        return -1;
    }        

    if (InitConnectDb(iThread) != 0)
    {
        return -1;
    }

    //判断配置状态设置
    if (AmsJudgeCfgState(iThread) != AMS_OK)
    {
        return -1;
    }
    //写配置状态设置
    if (AmsUpdateCfgState(iThread,2) != AMS_OK)
    {
        return -1;
    }

    //配置管理-全局配置(不包括文件服务器配置)
    if (AmsLoadGlobalCfg(iThread) != AMS_OK)
    {
        return -1;
    }

    //配置管理-穿网配置
    if (AmsLoadNatCfg(iThread) != AMS_OK)
    {
        return -1;
    }

    //配置管理-RCAS配置
    if (AmsLoadRcasCfg(iThread) != AMS_OK)
    {
        return -1;
    }

    //配置管理-文件服务器配置
    if (AmsLoadFileServerCfg(iThread) != AMS_OK)
    {
        return -1;
    }

#if 1
    //配置管理-机构配置
    if (AmsLoadOrgInfo(iThread) != AMS_OK)
    {
        return -1;
    }

#endif

	//配置管理-业务配置查询
    if (AmsLoadServiceInfo(iThread) != AMS_OK)
    {
        return -1;
    }
    
    //配置管理-业务组配置查询
    if (AmsLoadServiceGroupInfo(iThread) != AMS_OK)
    {
        return -1;
    }
	
    //数据库 配置文件同时在的情况， 可能数据有冲突，失败不退出程序，冲突数据取配置文件的，后面只有数据库时要修改 doing
    /*AmsLoadServiceInfo(iThread); 
    AmsLoadServiceGroupInfo(iThread);*/

    //终端管理-柜员配置
    if (AmsLoadTellerInfo(iThread) != AMS_OK)
    {
        return -1;
    }

    //终端管理-柜员机配置
    if (AmsLoadVtmInfo(iThread) != AMS_OK)
    {
        return -1;
    }

    //写配置状态设置
    if (AmsUpdateCfgState(iThread,1) != AMS_OK)
    {
        return -1;
    }

    return 0;
}




//zry added for scc 2018
int AmsSrvSeatSenten(WORD_t *word int wordcount,unsigned char pCurrId[])
{
	char		stringword[AMS_MAX_STRING_WORD_LEN];

	unsigned char 	flag;                          	//是否配置
	unsigned char 	idstate;                       	//坐席工号状态

	unsigned char 	telleridlen;                   	//坐席工号长度
//	unsigned char 	tellerid[AMS_MAX_TELLER_ID_LEN + 1];//坐席工号

	unsigned char 	srvGrpIdlen;                      //业务组类型长度
//	unsigned char 	srvGrpId[AMS_MAX_GRPID_LEN + 1]
	
	DWORD 			mbpIp;
	WORD			mbpPort;	

	int				i = 0;
	int				j = 0;
	unsigned		id = 0;
	int				idPos = -1;
	int				mbphoneNum = 0;
	int             result = FAILURE;

	if(NULL == pCurrId)
	{
		return FAILURE;
	}

	memset(stringword, 0, sizeof(stringword));
	memcpy(stringword, word[0].Body, word[0].Len);
	mbphoneNum = Min(SystemData.AmsPriData.amsCfgData.maxSeatNum, AMS_MAX_SEAT_NUM);

	 if(0 == strcmp(stringword,"tellerid"))
	 {
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Teller Senten tellerNo[%s] len[%d]Err!\r\n",
				stringword, word[2].Len);
			return FAILURE;
		}

		telleridlen = strlen(stringword);
        if(telleridlen > 0 && telleridlen <= AMS_MAX_TELLER_ID_LEN)
        {
			/* check teller in cfg or not */
			for(i = 0;i < mbphoneNum;i++)
			{
				if(0 == strcmp(AmsCfgSeat(i).tellerid,stringword))
				{
					Display("Teller Senten id(%s) has been used!\r\n");
					memset(&(AmsCfgSeat(i),0,sizeof(SEAT_INFO)));

					strcpy(AmsCfgSeat(i).tellerid,stringword);
					AmsCfgSeat(i).flag == AMS_SEAT_INSTALL;

					idPos = i;
					break;
				}
			}

			if(-1 == idPos)
			{
				//record tellerId
				for(i = 0; i < mbphoneNum; i++)
				{
					if(AMS_SEAT_UNINSTALL == AmsCfgSeat(i).flag)
					{
						strcpy(AmsCfgSeat(i).tellerId,stringword);
						AmsCfgSeat(i).tellerIdlen = telleridlen;
						AmsCfgSeat(i).flag == AMS_SEAT_INSTALL;
						break;
					}
				}

				if(i >= mbphoneNum)
				{
					Display("All Teller has been Installed[%s]!\r\n",stringword);
					*pCurrId = 0;
					return FAILURE;
				}
			}
		}
	

		strcpy(pCurrId,stringword);
		Display("Teller [%s] Installed! \r\n",stringword);

		return SUCCESS;	
	}

	/* find teller in cfg or not */
	for(i = 0; i < mbphoneNum; i++)
	{
		if(AMS_SEAT_INSTALL == AmsCfgSeat(i).flag && (strcmp(AmsCfgTeller(i).tellerId ,pCurrId)))
		{
			break;
		}
	}
	if(i >= mbphoneNum)
	{
		Display("Teller[%s]Senten not find Id!\r\n", pCurrId);

		memset(pCurrId,0,sizeof(AMS_MAX_TELLER_ID_LEN));
		return FAILURE;
	}

	if(0 == strcmp(stringword,"srvgrpid"))
	{
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Teller Senten srvGrpId[%s] len[%d]Err!\r\n",
				stringword, word[2].Len);
			return FAILURE;
		}

		srvGrpIdlen = strlen(stringword);
        if(srvGrpIdlen > 0 && srvGrpIdlen <= AMS_MAX_GRPID_LEN)
        {
			strcpy(AmsCfgTeller(i).srvGrpId,stringword);
        }
		else
		{
			Display("Teller[%s]Senten srvGrpId[%s] Err!\r\n",pCurrId, stringword);
			return FAILURE;
		}
	}	

	return SUCCESS;
}

//added end

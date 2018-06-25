#include "amsfunc.h"

int ConfigAmsSrv(char *cFileName)
{
	FILE		  *fp = NULL;
	char		  sItem[8192];
	int 		  section = CONFIG_AMS;
	int 		  index = 0;	
	WORD_t		  word[MAXWORDNUMALINE];
	int 		  wordCount = 0;
	int 		  count = 0; 
	unsigned char currId[MAXWORDNUMALINE] = {0};

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

		section = ConfigSrv(word, wordCount, section, currId);
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


int ConfigSrv(WORD_t *word,int wordcount,int section,unsigned  char pCurrId[])
{
	char			stringword[100];
	unsigned char	c = 0;

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
		else if(0 == strcmp(stringword,"vtm"))
		{
			section = CONFIG_VTM;
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
		case CONFIG_VTM:
			AmsSrvVtmSenten(word,wordcount,pCurrId);
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

int AmsSrvServiceSenten(WORD_t *word,int wordcount,unsigned char pCurrId[])
{
	char			stringword[AMS_MAX_STRING_WORD_LEN];
	unsigned char	serviceNameLen;
	int				i = 0;
	int				j = 0;
	unsigned int	serviceId = 0;

	if(NULL == pCurrId)
	{
		return FAILURE;
	}
	
	memset(stringword, 0, sizeof(stringword));

	if(0 == strcmp(stringword,"servicename"))
	{
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("Service Senten servicename[%s] len[%d]Err!\r\n",
				stringword, word[2].Len);
			return FAILURE;
		}

		serviceNameLen = strlen(stringword);
        if(serviceNameLen > 0 && serviceNameLen <= AMS_MAX_SERVICE_NAME_LEN)
        {       	
			for(i = 0; i < AMS_MAX_SERVICE_NUM; i++)
			{				
				if(AmsCfgService(i).serviceNameLen == serviceNameLen)
				{
					if(0 == memcmp(AmsCfgService(i).serviceName, stringword, serviceNameLen))
					{
						Display("Service Senten serviceName[%s]has been Used[%d]!\r\n",
							stringword, j);
						return FAILURE;
					}
				}
			}
			for(i = 0; i< AMS_MAX_SERVICE_NUM; i++)
			{
				if(AMS_SERVICE_UNINSTALL == AmsCfgService(i).flag)
				{	
					strcpy((char *)AmsCfgService(i).serviceName, stringword); 	
					AmsCfgService(i).serviceName[serviceNameLen] = '\0';
					AmsCfgService(i).serviceNameLen = serviceNameLen;
					AmsCfgService(i).flag = AMS_SERVICE_INSTALL;
					Display("Service[%s]Installed!\r\n", AmsCfgService(i).serviceName);
					break;
				}
			}
			if(i >= AMS_MAX_SERVICE_NUM)
			{
				Display("Service Num Installed over MaxNum[%d]!\r\n", AMS_MAX_SERVICE_NUM);
				
				return FAILURE;
			}
        }
		else
		{
			Display("Service Senten servicename[%s] len[%d]Err!\r\n",
				stringword, serviceNameLen);
			return FAILURE;
		}
	}
	else
	{
		return FAILURE;	
	}

	return SUCCESS;
}

int AmsSrvServiceGroupSenten(WORD_t *word,int wordcount,unsigned char pCurrId[])
{
	char                 stringword[AMS_MAX_STRING_WORD_LEN];
	unsigned char        srvGroupNameLen;
	unsigned char		 srvNameLen;
	int                  i = 0;	
	int                  j = 0;	
	unsigned int         srvGrpId = 0;
	unsigned int         serviceId = 0;
	unsigned int         serviceType = 0;	
	unsigned int		 serviceIdpos = 0;
	unsigned int         srvRsvdType = 0xffffffff;		
	char                 serviceName[AMS_MAX_SERVICE_NAME_LEN];
	unsigned char        isAutoFlag = 0;	

	if(0 == strcmp(stringword,"servicegroupname"))
	{
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("ServiceGroup Senten serviceGroupName[%s] len[%d]Err!\r\n",
				stringword, word[2].Len);
			return FAILURE;
		}

		srvGroupNameLen = strlen(stringword);
        if(srvGroupNameLen > 0 && srvGroupNameLen <= AMS_MAX_SERVICE_GROUP_NAME_LEN)
        {
        	/* check service group name is used or not */
			for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
			{
				if(AmsCfgSrvGroup(i).srvGrpIdLen == srvGroupNameLen)
				{
					if(0 == memcmp(AmsCfgSrvGroup(j).srvGrpId, stringword, srvGroupNameLen))
					{
						Display("ServiceGroup Senten serviceGroupName[%s]has been Used[%d]!\r\n",
							stringword, j);
						return FAILURE;
					}
				}
			}
			for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
			{
				if(AMS_SERVICE_GROUP_UNINSTALL == AmsCfgSrvGroup(i).flag)
				{
					strcpy((char *)AmsCfgSrvGroup(i).srvGrpId, stringword); 	
					AmsCfgSrvGroup(i).srvGrpId[srvGroupNameLen] = '\0';
					AmsCfgSrvGroup(i).srvGrpIdLen = srvGroupNameLen;
					AmsCfgSrvGroup(i).flag = AMS_SERVICE_GROUP_INSTALL;
					strcpy(pCurrId,stringword);
					pCurrId[srvGroupNameLen] = '\0';
				}
			}
        }
		else
		{
			Display("ServiceGroup Senten serviceGroupName[%s] len[%d]Err!\r\n",
				stringword, srvGroupNameLen);
			return FAILURE;
		}
	}

	//判断业务组是否已经安装
	for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
	{
		if( (AMS_SERVICE_GROUP_INSTALL == AmsCfgSrvGroup(i).flag)
			&& (0 == strcmp(AmsCfgSrvGroup(i).srvGrpId,pCurrId)))
		{
			serviceIdpos = i;
			break;
		}
	}
	if(i > AMS_MAX_SERVICE_NUM)
	{
		Display("ServiceGroup[%S]Senten not find!\r\n", pCurrId);
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
			Display("ServiceGroup Senten serviceName[%s] len[%d]Err!\r\n",
				stringword, word[2].Len);
			return FAILURE;
		}

		srvNameLen =strlen (stringword);
		//判断业务类型是不是业务类型列表中
		//不是 返回错误
		//是 将业务类型添加到业务组数组中
		if(AMS_ERROR == amsCheckServiceName(stringword))
		{
			Display("ServiceGroup[%s]Senten amsCheckServiceName[%s]Err!\r\n",
				serviceId);
			return FAILURE;
		}

		for(i = 0; i < AMS_MAX_SERVICE_NUM;i++)
		{		//判断业务类型是否已经安装过
			if(0 == strcmp(AmsCfgSrvGroup(serviceIdpos).srvInfo[i].serviceName,stringword))
			{
				Display("ServiceGroup[%s]Senten Service already installed[%s]!\r\n",
					serviceId,stringword);
				return FAILURE;
			}
		}
		
		j = AmsCfgSrvGroup(serviceIdpos).srvlogpos;
		strcpy(AmsCfgSrvGroup(serviceIdpos).srvInfo[j].serviceName,stringword);
		AmsCfgSrvGroup(i).srvInfo[j].serviceName[srvNameLen] = '\0';
		AmsCfgSrvGroup(i).srvInfo[j].serviceNameLen = srvNameLen;
		AmsCfgSrvGroup(serviceIdpos).srvlogpos++ ;
	}
	if(0 == strcmp(stringword,"isAutoFlag"))
	{
		isAutoFlag = atoi((const char *)word[2].Body);

		if(isAutoFlag > 1)
		{
			Display("ServiceGroup[%s] Senten isAutoFlag[%d] Err!\r\n",*pCurrId, isAutoFlag);
			return FAILURE;
		}

		AmsCfgSrvGroup(i).isAutoFlag = isAutoFlag;
	}
	else
	{
		return FAILURE;
	}
}

int AmsSrvTellerSenten(WORD_t *word,int wordcount,unsigned char pCurrId[])
{
	char				stringword[AMS_MAX_STRING_WORD_LEN];
	unsigned char		tellerIdLen;
	unsigned char		srvGrpIdLen;
	unsigned char		transIpLen;
	unsigned int		vtaIp = 0;
	unsigned int		tellerIdpos = 0;
	int                  i = 0;
	int                  j = 0;	
	unsigned int         id = 0;
	int                  idPos = -1;
	int                  vtaNum = 0;
	int                  result = FAILURE;
	TELLER_INFO_NODE	*pTellerInfoNode = NULL;

	if(NULL == pCurrId)
	{
		return FAILURE;
	}

	memset(stringword, 0, sizeof(stringword));
	memcpy(stringword, word[0].Body, word[0].Len);
	vtaNum = Min(SystemData.AmsPriData.amsCfgData.maxVtaNum, AMS_MAX_VTA_NUM);

	if(0 == strcmp(stringword,"id"))
	{
		//获取id的值
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("AmsSrvTeller Senten id[%s] len[%d]Err!\r\n",
				stringword, word[2].Len);
			return FAILURE;
		}

		tellerIdLen = strlen(stringword);
		
		if(tellerIdLen > 0 && tellerIdLen <= AMS_MAX_TELLER_ID_LEN)
		{
			/* check teller in cfg or not */
			pTellerInfoNode = AmsSearchTellerInfoHash(stringword,tellerIdLen);
			if( NULL != pTellerInfoNode)
			{
				Display("Teller Senten id[%s] has been used!\r\n",id);

				i = pTellerInfoNode->tellerInfopos;
				
				memset(&AmsCfgTeller(i), 0, sizeof(TELLER_INFO));

				strcpy(AmsCfgTeller(i).tellerId,stringword);
				AmsCfgTeller(i).tellerId[tellerIdLen] = '\0';
				AmsCfgTeller(i).tellerIdLen = tellerIdLen;
				AmsCfgTeller(i).flag = AMS_TELLER_INSTALL;
					
				idPos = i;			
			}

			if(-1 == idPos)
			{
				/* record tellerId*/
				for(i = 0;i < vtaNum; i++)
				{
					if(AMS_TELLER_UNINSTALL == AmsCfgTeller(i).flag)
					{
						strcpy(AmsCfgTeller(i).tellerId,stringword);
						AmsCfgTeller(i).tellerId[tellerIdLen] = '\0';
						AmsCfgTeller(i).tellerIdLen = tellerIdLen;
						AmsCfgTeller(i).flag = AMS_TELLER_INSTALL;

						//插入hash
						pTellerInfoNode = AmsGetTellerInfoNode(stringword,tellerIdLen);
						if(NULL != pTellerInfoNode)
						{
							strcpy(pTellerInfoNode->tellerId,stringword);
							pTellerInfoNode->tellerIdLen=tellerIdLen;
							pTellerInfoNode->tellerId[tellerIdLen]='\0';
							pTellerInfoNode->tellerInfopos=i;

							AmsInsertTellerInfoHash(pTellerInfoNode);
						}
					}
				}
				if(i >= vtaNum)
				{
					Display("All Teller has been Installed[%u]!\r\n", id);
					return FAILURE;
				}
			}

			strcpy(pCurrId,stringword);
			pCurrId[tellerIdLen]='\0';

			
			Display("Teller[%s] Installed!\r\n", pCurrId);
				
			return SUCCESS;
		}
		else
		{
			Display("AmsSrvTeller Senten id[%s] len[%d]Err!\r\n",
							stringword, tellerIdLen);
			return FAILURE;
		}

	}
	/*find teller in cfg or not */
	tellerIdLen = strlen(pCurrId);
	pTellerInfoNode = AmsSearchTellerInfoHash(pCurrId,tellerIdLen);
	if(NULL == pTellerInfoNode)
	{
		Display("Teller[%s]Senten not find Id!\r\n", pCurrId);
		memset(pCurrId,0,MAXWORDNUMALINE);
		return FAILURE;		
	}

	i = pTellerInfoNode->tellerInfopos;

	//其他teller涉及到的参数配置

	
	if(0 == strcmp(stringword,"srvgrpid"))
	{
		//获取id的值
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("AmsSrvTeller Senten id[%s] len[%d]Err!\r\n",
				stringword, word[2].Len);
			return FAILURE;
		}
		srvGrpIdLen = strlen(stringword);		
		if(srvGrpIdLen > AMS_MAX_SERVICE_GROUP_NAME_LEN)
		{
			Display("AmsSrvTeller Senten srvGrpId[%s] len[%d]Err!\r\n",
										stringword, srvGrpIdLen);
						return FAILURE;
		}
		else
		{
			for(j=0;j<AMS_MAX_SERVICE_GROUP_NUM;j++)
			{
				if(AmsCfgSrvGroup(j).flag == AMS_SERVICE_GROUP_UNINSTALL)
				{
					continue;
				}
				if( AmsCfgSrvGroup(j).srvGrpIdLen == srvGrpIdLen
					&& 0 == strcmp(AmsCfgSrvGroup(j).srvGrpId,stringword))
				{
					AmsCfgTeller(i).srvGrpIdPos = j;
					strcpy(AmsCfgTeller(i).srvGrpId,stringword);
					AmsCfgTeller(i).srvGrpId[srvGrpIdLen] = '\0';
					AmsCfgTeller(i).srvGrpIdLen = srvGrpIdLen;
					break;
				}
			}
		}
	}



	return SUCCESS;
}

int AmsSrvVtmSenten(WORD_t *word,int wordcount,unsigned char pCurrId[])
{
	char				stringword[AMS_MAX_STRING_WORD_LEN];
	unsigned char		vtmIdLen;
	unsigned char		vtmNameLen;
	unsigned char		vtmPwdLen;
	unsigned char		transIpLen = 0;
	unsigned int		vtmIp=0;
	int					i = 0;
	int 				j = 0;
	unsigned int 		id = 0;
	int					idPos = -1;
	int					vtmNum = 0;
	int					result = 0;
	VTM_INFO_NODE		*pVtmInfoNode=NULL;

	memset(stringword, 0, sizeof(stringword));
	memcpy(stringword, word[0].Body, word[0].Len);
	vtmNum = Min(SystemData.AmsPriData.amsCfgData.maxVtmNum, AMS_MAX_VTM_NUM);

	if(0 == strcmp(stringword,"id"))
	{
		//获取id的值
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("AmsSrvVtmSenten Senten id[%s] len[%d]Err!\r\n",
				stringword, word[2].Len);
			return FAILURE;
		}

		vtmIdLen = strlen(stringword);
		
		if(vtmIdLen > 0 && vtmIdLen <= AMS_MAX_VTM_ID_LEN)
		{
			/* check vtm in cfg or not */
			pVtmInfoNode = AmsSearchVtmInfoHash(stringword,vtmIdLen);
			if( NULL != pVtmInfoNode)
			{
				Display("AmsSrvVtmSenten Senten id[%s] has been used!\r\n",stringword);

				i = pVtmInfoNode->vtmInfopos;
				
				memset(&AmsCfgVtm(i), 0, sizeof(VTM_INFO));
				strcpy(AmsCfgVtm(i).vtmId,stringword);
				AmsCfgVtm(i).vtmId[vtmIdLen] = '\0';
				AmsCfgVtm(i).vtmIdLen = vtmIdLen;
				AmsCfgVtm(i).flag = AMS_VTM_INSTALL;
					
				idPos = i;			
			}

			if(-1 == idPos)
			{
				/* record vtmId*/
				for(i = 0;i < vtmNum; i++)
				{
					if(AMS_VTM_UNINSTALL == AmsCfgVtm(i).flag)
					{
						strcpy(AmsCfgVtm(i).vtmId,stringword);
						AmsCfgVtm(i).vtmId[vtmIdLen] = '\0';
						AmsCfgVtm(i).vtmIdLen = vtmIdLen;
						AmsCfgVtm(i).flag = AMS_VTM_INSTALL;

						//插入hash
						pVtmInfoNode = AmsGetVtmInfoNode(stringword,vtmIdLen);
						if(NULL != pVtmInfoNode)
						{
							strcpy(pVtmInfoNode->vtmId,stringword);
							pVtmInfoNode->vtmIdLen=vtmIdLen;
							pVtmInfoNode->vtmId[vtmIdLen]='\0';
							pVtmInfoNode->vtmInfopos=i;

							AmsInsertVtmInfoHash(pVtmInfoNode);
						}
					}
				}
				if(i >= vtmNum)
				{
					Display("All Teller has been Installed[%u]!\r\n", id);
					return FAILURE;
				}
			}

			strcpy(pCurrId,stringword);
			pCurrId[vtmIdLen]='\0';

			
			Display("Vtm[%s] Installed!\r\n", pCurrId);
				
			return SUCCESS;
		}
		else
		{
			Display("AmsSrvVtmSenten Senten id[%s] len[%d]Err!\r\n",
							stringword, vtmIdLen);
			return FAILURE;
		}
	}

	vtmIdLen = strlen(pCurrId);
	
	/*find vtm in cfg or not */
	pVtmInfoNode = AmsSearchVtmInfoHash(pCurrId,vtmIdLen);
	if(NULL == pVtmInfoNode)
	{
		Display("AmsSrvVtmSenten[%s]Senten not find Id!\r\n", pCurrId);
		memset(pCurrId,0,MAXWORDNUMALINE);
		return FAILURE;		
	}

	i = pVtmInfoNode->vtmInfopos;

	//其他vtm涉及到的参数配置

	return SUCCESS;
}


int AmsSrvQueueSenten(WORD_t *word,int wordcount,unsigned char pCurrId[])
{
	char				stringword[AMS_MAX_STRING_WORD_LEN];
	int					i = 0;
	unsigned char		srvGrpIdLen = 0;
	unsigned char		srvGrpId[AMS_MAX_SERVICE_GROUP_NAME_LEN + 1]={0};
	int					idPos= -1;
	unsigned short		maxQueLen = 0;
	unsigned short		avgSrvTime = 0;
	int					result = 0;

	memset(stringword, 0, sizeof(stringword));
	memcpy(stringword, word[0].Body, word[0].Len);

	if(0 == strcmp(stringword,"srvgrpid"))
	{
		//获取srvgrpid的值
		memset(stringword,0,sizeof(stringword));
		if(word[2].Len <= AMS_MAX_STRING_WORD_LEN)
		{
			memcpy(stringword, word[2].Body, word[2].Len);
		}
		else
		{
			Display("AmsSrvQueueSenten Senten id[%s] len[%d]Err!\r\n",
				stringword, word[2].Len);
			return FAILURE;
		}

		srvGrpIdLen = strlen(stringword);
		
		if(srvGrpIdLen > 0 && srvGrpIdLen <= AMS_MAX_SERVICE_GROUP_NAME_LEN)
		{
			/* check QueueSys in cfg or not */
			for(i = 0; i < AMS_MAX_SERVICE_GROUP_NUM; i++)
			{
				if(AMS_QUEUE_CFG == AmsCfgQueueSys(i).flag 
					&& (0 == strcmp(AmsCfgQueueSys(i).srvGrpId,stringword)))
				{
					Display("Queue Senten srvGrpId[%s] has been used!\r\n",
										pCurrId);
					
					memset(&AmsCfgQueueSys(i), 0, sizeof(QUEUE_SYS_INFO));
					
					strcpy(AmsCfgQueueSys(i).srvGrpId,stringword);
					AmsCfgQueueSys(i).srvGrpId[srvGrpIdLen] = '\0';
					AmsCfgQueueSys(i).srvGrpIdLen = srvGrpIdLen;
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
						memset(&AmsCfgQueueSys(i), 0, sizeof(QUEUE_SYS_INFO));
					
						strcpy(AmsCfgQueueSys(i).srvGrpId,stringword);
						AmsCfgQueueSys(i).srvGrpId[srvGrpIdLen] = '\0';
						AmsCfgQueueSys(i).srvGrpIdLen = srvGrpIdLen;
						AmsCfgQueueSys(i).flag = AMS_QUEUE_CFG;
						break;
					}
				}

				if(i >= AMS_MAX_SERVICE_GROUP_NUM)
				{
					Display("All Queue has been Configed[%d]!\r\n", srvGrpId);
					memset(pCurrId,0,MAXWORDNUMALINE);
					return FAILURE;
				}
			}
		}
		strcpy(pCurrId,stringword);
		pCurrId[srvGrpIdLen] = '\0';

		Display("Queue[%s] Configed!\r\n", pCurrId);
		return SUCCESS;
	}

	return SUCCESS;
}





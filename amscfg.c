int AmsSrvMbphoneSenten(WORD_t *word int wordcount,unsigned char pCurrId[])
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
	mbphoneNum = Min(SystemData.AmsPriData.amsCfgData.maxMbphoneNum, AMS_MAX_MBPHONE_NUM);

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
        if(telleridlen > 0 && telleridlen <= AMS_MAX_TELLID_NO_LEN)
        {
			/* check teller in cfg or not */
			for(i = 0;i < mbphoneNum;i++)
			{
				if(0 == strcmp(AmsCfgTeller(i).tellerid,stringword))
				{
					Display("Teller Senten id(%s) has been used!\r\n");
					memset(&(AmsCfgTeller(i),0,sizeof(MBPHONE_INFO)));

					strcpy(AmsCfgTeller(i).tellerid,stringword);
					AmsCfgTeller(i).flag == AMS_TELLER_INSTALL;

					idPos = i;
					break;
				}
			}

			if(-1 == idPos)
			{
				//record tellerId
				for(i = 0; i < mbphoneNum; i++)
				{
					if(AMS_TELLER_UNISTALL == AmsCfgTeller(i).flag)
					{
						strcpy(AmsCfgTeller(i).tellerId,stringword);
						AmsCfgTeller(i).flag == AMS_TELLER_INSTALL;
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
	 }

	strcpy(pCurrId,stringword);
	Display("Teller [%s] Installed! \r\n",stringword);

	return SUCCESS;	
}

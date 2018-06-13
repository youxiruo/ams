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




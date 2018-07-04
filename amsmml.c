#include "amsfunc.h"

extern char *strAmsTimerName[];

int AmsTraceToFile(PID_t hPid,PID_t rPid,unsigned char *MsgCode,
						unsigned char *description,unsigned int descrlen,
					    unsigned char *body,unsigned int length,
					    unsigned char *logName)
{
	FILE            *fp;
	unsigned char   buf[120];
	time_t          stm;
	struct tm       *ptm;
	unsigned int    fileLen = 0;
 	unsigned int    fileSeq = 0;
	
	memset(buf,0,sizeof(buf));

    if(length > (MSG_BODY_LEN - 100))
    {
        length = MSG_BODY_LEN - 100;
    }

	time(&stm);
    ptm = localtime(&stm);

	//fragment start, add 20161012 
	Pthread_mutex_lock(&SystemData.amsTraceFileProcMtx);	
	
	if(!AmsTraceFileFrgmt)	
	{
		snprintf(buf,sizeof(buf),"%s/%s_trace.log", SYSTRACEPATH, logName);
	
		if(NULL == (fp = fopen(buf,"a+")))
		{
			Pthread_mutex_unlock(&SystemData.amsTraceFileProcMtx);
			return -1;
		}
	}
	else	
	{
		if(!FileFrgmtTime)
		{
			FileFrgmtTime = VTC_TRACE_FILE_FRAGMENT_TIME_LEN;
		}

		if(!FileFrgmtSize)
		{
			FileFrgmtSize = VTC_TRACE_FILE_FRAGMENT_SIZE_LEN;
		}	

		//是否达到生成新文件的时间间隔
		fileSeq=((stm + 28800) % 86400)/(FileFrgmtTime*60); //28800: 东8区, 86400: 24小时

		if(fileSeq != SystemData.amsTraceFileTimeSeq || 0 == SystemData.amsTraceFileSizeSeq)
		{
			//新创建一个文件
			SystemData.amsTraceFileTimeSeq = fileSeq;
			SystemData.amsTraceFileSizeSeq = 1;		
			
	        snprintf(SystemData.amsCurTraceFile, sizeof(SystemData.amsCurTraceFile),
	            "%s/%s_trace_%4d%02d%02d_%03d_%03d.log",
				SYSTRACEPATH, logName, 
				ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, 
				fileSeq, SystemData.amsTraceFileSizeSeq);	

			//打开文件
			if(NULL == (fp = fopen(SystemData.amsCurTraceFile,"a+")))
			{
				Pthread_mutex_unlock(&SystemData.amsTraceFileProcMtx);
				return -1;
			}				
		}
		else
		{
			//打开文件
			if(NULL == (fp = fopen(SystemData.amsCurTraceFile,"a+")))
			{		
				Pthread_mutex_unlock(&SystemData.amsTraceFileProcMtx);
				return -1;
			}
			
			//当前文件是否写满设定大小
			fseek(fp, 0L, SEEK_END);
			fileLen = ftell(fp);		

			if(fileLen >= (FileFrgmtSize*1024*1024))
			{
		        //关闭已写满的文件zhuyn added 20161020
                fclose(fp);
                fp = NULL;
                
				//新创建一个文件
				SystemData.amsTraceFileSizeSeq++;
				if(0 == SystemData.amsTraceFileSizeSeq)
				{
					SystemData.amsTraceFileSizeSeq = 1;
				}
								
		        snprintf(SystemData.amsCurTraceFile, sizeof(SystemData.amsCurTraceFile),
		            "%s/%s_trace_%4d%02d%02d_%03d_%03d.log",
					SYSTRACEPATH, logName, 
					ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, 
					fileSeq, SystemData.amsTraceFileSizeSeq);	

				//打开文件
				if(NULL == (fp = fopen(SystemData.amsCurTraceFile,"a+")))
				{
					Pthread_mutex_unlock(&SystemData.amsTraceFileProcMtx);
					return -1;
				}					
			}
		}
	}
	Pthread_mutex_unlock(&SystemData.amsTraceFileProcMtx);
	
	//fragment end, add 20161012 

	fprintf(fp,"================================================================================");
	fprintf(fp,"\n");
#if SYSTEM != _WIN_32
	{
		static struct timeval  curTime;
		static struct timezone curTz;
		unsigned char buf[50];
		memset(buf,0,50);
		gettimeofday(&curTime,&curTz);
		fprintf(fp,"[%4d-%2d-%2d %2d:%2d:%2d:%d us] ",ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,
			ptm->tm_hour,ptm->tm_min,ptm->tm_sec,curTime.tv_usec);
	} 
#else
	{
		fprintf(fp,"[%4d-%2d-%2d %2d:%2d:%2d] [AMS_TRACE]:\n",
			ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,
			ptm->tm_hour,ptm->tm_min,ptm->tm_sec);
	}
#endif
	fprintf(fp,"description: %s  ,length=%d\n",MsgCode, length);
	fprintf(fp,"host:    MID = %3d   FID = %3d   PID = %5d \n",hPid.cModuleId,hPid.cFunctionId,hPid.iProcessId);
	fprintf(fp,"remote:  MID = %3d   FID = %3d   PID = %5d \n",rPid.cModuleId,rPid.cFunctionId,rPid.iProcessId);
	if(descrlen)
	{
		fprintf(fp,"--------------------------------------------------------------------------------");
		fprintf(fp,"\n");
		fprintf(fp,"detail:\n");
		fwrite(description, 1, descrlen, fp);
		fprintf(fp,"\n");
	}
	if(length)
	{
		unsigned int i;
		fprintf(fp,"--------------------------------------------------------------------------------");
		fprintf(fp,"\n");
		fprintf(fp,"msg body:\n ");
		for(i=0; i < length; i++)
		{
			fprintf(fp,"%02x ",body[i]);
			if( 3*(i+1)%78 ==0 )
				fprintf(fp,"\n ");
		}
		fprintf(fp,"\n");
	}
	fprintf(fp,"================================================================================");
	fprintf(fp,"\n");
	fclose(fp);
	return 0;	
}

int SetAmsTrace(MMLCMD_t r[],int SocketId,int Source)
{
	char             s[2048];
	int              i = 0;
	unsigned char    j = 0;
	int              len = 0;

	memset(s,0,sizeof(s));

	if(r[0].CountOfCell <  1)  
	{
		sprintf(s,"the count of  parameter is wrong!");
		MMLPrint(s,strlen(s),SocketId,Source);
		return -1;
	}

	for(i = 0; i < r[0].CountOfCell; i++)
	{
        if (!CompTwoString(r[0].CmdCell[i][0],"DEBUG"))
		{
			j = 0;
			for(j; j < strlen(r[0].CmdCell[i][1]); j++)
			{
				if(isdigit(r[0].CmdCell[i][1][j]))
				{
					continue;
				}
				else
				{
					len = snprintf(s,sizeof(s),"ParaNameErr-DEBUG");
					MMLErrorPrint(s,len,SocketId,Source);
					return -1;
				}
			}
            
			AmsDebugTrace = atoi(r[0].CmdCell[i][1]);
			continue;
		}
		
        if (!CompTwoString(r[0].CmdCell[i][0],"COMMON"))
		{
			j = 0;
			for(j; j < strlen(r[0].CmdCell[i][1]); j++)
			{
				if(isdigit(r[0].CmdCell[i][1][j]))
				{
					continue;
				}
				else
				{
					len = snprintf(s,sizeof(s),"ParaNameErr-COMMON");
					MMLErrorPrint(s,len,SocketId,Source);
					return -1;
				}
			}
            
			AmsCommonTrace = atoi(r[0].CmdCell[i][1]);
			continue;
		}
                
        if (!CompTwoString(r[0].CmdCell[i][0],"MSG"))
		{
			j = 0;
			for(j; j < strlen(r[0].CmdCell[i][1]); j++)
			{
				if(isdigit(r[0].CmdCell[i][1][j]))
				{
					continue;
				}
				else
				{
					len = snprintf(s,sizeof(s),"ParaNameErr-MSG");
					MMLErrorPrint(s,len,SocketId,Source);
					return -1;
				}
			}
            
			AmsMsgTrace = atoi(r[0].CmdCell[i][1]);
			continue;
		}
        
        if (!CompTwoString(r[0].CmdCell[i][0],"STATE"))
		{
			j = 0;
			for(j; j < strlen(r[0].CmdCell[i][1]); j++)
			{
				if(isdigit(r[0].CmdCell[i][1][j]))
				{
					continue;
				}
				else
				{
					len = snprintf(s,sizeof(s),"ParaNameErr-STATE");
					MMLErrorPrint(s,len,SocketId,Source);
					return -1;
				}
			}
			
			AmsStateTrace = atoi(r[0].CmdCell[i][1]);
			continue;
		} 
        
        if (!CompTwoString(r[0].CmdCell[i][0],"TIMER"))
		{
			j = 0;
			for(j; j < strlen(r[0].CmdCell[i][1]); j++)
			{
				if(isdigit(r[0].CmdCell[i][1][j]))
				{
					continue;
				}
				else
				{
					len = snprintf(s,sizeof(s),"ParaNameErr-TIMER");
					MMLErrorPrint(s,len,SocketId,Source);
					return -1;
				}
			}
			
			AmsTimerTrace = atoi(r[0].CmdCell[i][1]);
			continue;
		}

        if (!CompTwoString(r[0].CmdCell[i][0],"ERROR"))
		{
			j = 0;
			for(j; j < strlen(r[0].CmdCell[i][1]); j++)
			{
				if(isdigit(r[0].CmdCell[i][1][j]))
				{
					continue;
				}
				else
				{
					len = snprintf(s,sizeof(s),"ParaNameErr-ERROR");
					MMLErrorPrint(s,len,SocketId,Source);
					return -1;
				}
			}
			
			AmsErrorTrace = atoi(r[0].CmdCell[i][1]);
			continue;
		}
		
        if (!CompTwoString(r[0].CmdCell[i][0],"ALARM"))
		{
			j = 0;
			for(j; j < strlen(r[0].CmdCell[i][1]); j++)
			{
				if(isdigit(r[0].CmdCell[i][1][j]))
				{
					continue;
				}
				else
				{
					len = snprintf(s,sizeof(s),"ParaNameErr-ALARM");
					MMLErrorPrint(s,len,SocketId,Source);
					return -1;
				}
			}
			
			AmsAlarmTrace = atoi(r[0].CmdCell[i][1]);
			continue;
		}
		
		if(OMP_COMM == Source)
		{
			len = snprintf(s,sizeof(s),"UnvalidParaName");
			MMLErrorPrint(s,len,SocketId,Source);
		}
		else
		{
			sprintf(s,":RESULT:ERRORMSG=\"UnvalidParaName\";");
			MMLPrint(s,strlen(s),SocketId,Source);
		}
		
		return 0;
	}

	memset(s,0,sizeof(s));
	len = snprintf(s,sizeof(s),"SET SUCCESS");
	MMLSuccessPrint(s,len,SocketId,Source);

	return AMS_OK;
}


int DisplayAmsTrace(MMLCMD_t r[],int SocketId,int Source)
{
	char    s[1024];
	int     iLen = 0;
	
	memset(s,0,sizeof(s));
	
	iLen = snprintf(s,sizeof(s),"DEBUG = %d, ", AmsDebugTrace);
	iLen += snprintf(s+iLen,sizeof(s)-iLen,"COMMON = %d, ", AmsCommonTrace);
    iLen += snprintf(s+iLen,sizeof(s)-iLen,"MSG = %d, ", AmsMsgTrace);
    iLen += snprintf(s+iLen,sizeof(s)-iLen,"STATE = %d, ", AmsStateTrace);
    iLen += snprintf(s+iLen,sizeof(s)-iLen,"TIMER = %d, ", AmsTimerTrace);
    iLen += snprintf(s+iLen,sizeof(s)-iLen,"ERROR = %d, ", AmsErrorTrace);
	iLen += snprintf(s+iLen,sizeof(s)-iLen,"ALARM = %d, ", AmsAlarmTrace);
	
	if (OMP_COMM == Source)
	{
		OMSResultPrint(s,iLen,SocketId,Source);
		SendPrompt(SocketId);
	}
	else
	{
		MMLPrint(s,strlen(s),SocketId,Source);
	}
	
	return SUCCESS;
}


int InstallAmsMMLCommand()
{
	MMLInstallCommand("SET","AMS","TRACE",&SetAmsTrace,
			":SET-AMS-TRACE;",
			":SET-AMS-TRACE:COMMON=,MSG=,STATE=,TIMER=,ERROR=,ALARM=;");//Not Show Debug
	
	MMLInstallCommand("DISPLAY","AMS","TRACE",&DisplayAmsTrace,
		":DISPLAY-AMS-TRACE;",
		":DISPLAY-AMS-TRACE:;");
		
	return AMS_SUCCESS;
}



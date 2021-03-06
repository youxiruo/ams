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


int AmsTraceInfoToFile(int pid,int body,
						    unsigned char *description,unsigned int descrlen,
						    unsigned char *logName)
{
	
	FILE            *fp;
	unsigned char   buf[120];
	time_t          stm;
	struct tm       *ptm;
	unsigned int    fileLen = 0;
 	unsigned int    fileSeq = 0;
	
	memset(buf,0,sizeof(buf));

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
		fileSeq = ((stm + 28800) % 86400)/(FileFrgmtTime*60); //28800: 东8区, 86400: 24小时

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

	fprintf(fp,"\ndescription:%s \n",description,body);
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


int DisplayAmsMsgStat(int SocketId,int Source)
{
	char             s[8192];//careful of len!! 
	int              i = 0;
	unsigned char    j = 0;
	int              len = 0;	
	
    memset(s,0,sizeof(s));

    len = snprintf(s,sizeof(s),"\r\nAms Msg Proc Stat:\r\n");
    len += snprintf(s+len,sizeof(s)-len,
"\
recvTellerLoginReq                     =%20ld,\t sendTellerLoginRsp                     =%20ld\r\n\
recvTellerLogoutReq                    =%20ld,\t sendTellerLogoutRsp                    =%20ld\r\n\
recvTellerStateOperateReq              =%20ld,\t sendTellerStateOperateRsp              =%20ld\r\n\
recvTellerStateOperateCnf              =%20ld,\t sendTellerStateOperateInd              =%20ld\r\n\
recvTellerModifyPasswordReq            =%20ld,\t sendTellerModifyPasswordRsp            =%20ld\r\n\
recvTellerForceLoginReq                =%20ld,\t sendTellerForceLoginRsp                =%20ld\r\n\
recvTellerQueryInfoReq                 =%20ld,\t sendTellerQueryInfoRsp                 =%20ld\r\n\r\n\
recvTellerEventNotice                  =%20ld\r\n\
sendTellerEventInd                     =%20ld\r\n\r\n",

	//ams <-> crm msg	
	AmsMsgStat.recvCrmLoginReq,               AmsMsgStat.sendCrmLoginRsp,
	AmsMsgStat.recvCrmLogoutReq,              AmsMsgStat.sendCrmLogoutRsp,
	AmsMsgStat.recvCrmStateOperateReq,        AmsMsgStat.sendCrmStateOperateRsp, 
	AmsMsgStat.recvCrmStateOperateCnf,        AmsMsgStat.sendCrmStateOperateInd, 
	AmsMsgStat.recvCrmModifyPasswordReq,      AmsMsgStat.sendCrmModifyPasswordRsp,
	AmsMsgStat.recvCrmForceLoginReq,          AmsMsgStat.sendCrmForceLoginRsp,
	AmsMsgStat.recvCrmQueryInfoReq,           AmsMsgStat.sendCrmQueryInfoRsp,	
	AmsMsgStat.recvCrmEventNotice,
	AmsMsgStat.sendCrmEventInd);

    if(Source == OMP_COMM)
    {
        OMSResultPrint(s,len,SocketId,Source);
        SendPrompt(SocketId);
    }
    else
    {
        MMLPrint(s,strlen(s),SocketId,Source);
    }

	 memset(s,0,sizeof(s));
	len = snprintf(s,sizeof(s),
	"\
recvTellerRegReq                       =%20ld,\t sendTellerRegRsp                       =%20ld\r\n\
recvTellerGetReq                       =%20ld,\t sendTellerGetRsp                       =%20ld\r\n\
recvTellerCalloutReq                   =%20ld,\t sendTellerCalloutRsp                   =%20ld\r\n\
recvTellerAuthinfoReq                  =%20ld,\t sendTellerAuthinfoRsp                  =%20ld\r\n\
recvTellerCallEventNoticeReq           =%20ld,\t sendAmsCallEventInd                 =%20ld\r\n\r\n",
	
		//ams <-> cms msg
		AmsMsgStat.recvCrmRegReq,				   AmsMsgStat.sendCrmRegRsp,
		AmsMsgStat.recvCrmGetReq,				   AmsMsgStat.sendCrmGetRsp,
		AmsMsgStat.recvCrmCalloutReq,			   AmsMsgStat.sendCrmCalloutRsp,
		AmsMsgStat.recvCrmAuthinfoReq,			   AmsMsgStat.sendCrmAuthinfoRsp,
		AmsMsgStat.recvAmsCallEventNoticeReq,	   AmsMsgStat.sendAmsCallEventInd);
	
		if(Source == OMP_COMM)
		{
			OMSResultPrint(s,len,SocketId,Source);
			SendPrompt(SocketId);
		}
		else
		{
			MMLPrint(s,strlen(s),SocketId,Source);
		}


    memset(s,0,sizeof(s));
    len = snprintf(s,sizeof(s),"\r\nAms B Msg Proc Stat:\r\n");
    len += snprintf(s+len,sizeof(s)-len,
"\
crmStateOperateIndTimeout           =%20ld,\t RestTimeout                 =%20ld\r\n\
customerInQueueTimeout              =%20ld\r\n\r\n",

	//ams B msg
	AmsMsgStat.crmStateOperateIndTimeout,   AmsMsgStat.restTimeout,
	AmsMsgStat.customerInQueueTimeout);  

    if(Source == OMP_COMM)
    {
        OMSResultPrint(s,len,SocketId,Source);
        SendPrompt(SocketId);
    }
    else
    {
        MMLPrint(s,strlen(s),SocketId,Source);
    }


    memset(s,0,sizeof(s));

    len += snprintf(s+len,sizeof(s)-len,"\r\nAms Other Msg Proc Stat:\r\n");
    len += snprintf(s+len,sizeof(s)-len,
"\
amsErrMsg                           =%20ld,\t amsUnknownMsgType                   =%20ld\r\n\
amsUnknownMsg                       =%20ld,\t amsErrBMsg                          =%20ld\r\n\
amsUnknownBMsg                      =%20ld\r\n\r\n", 
	//ams unknown msg
	AmsMsgStat.amsErrMsg,                      AmsMsgStat.amsUnknownMsgType,
	AmsMsgStat.amsUnknownMsg,                  AmsMsgStat.amsErrBMsg,
	AmsMsgStat.amsUnknownBMsg);
	
    if(Source == OMP_COMM)
    {
        OMSResultPrint(s,len,SocketId,Source);
        SendPrompt(SocketId);
    }
    else
    {
        MMLPrint(s,strlen(s),SocketId,Source);
    }

	return SUCCESS;
}

int DisplayAmsTimerStat(int SocketId,int Source)
{
	char             s[8192];//careful of len!! 
	int              i = 0;
	unsigned char    j = 0;
	int              len = 0;	

    memset(s,0,sizeof(s));

    len = snprintf(s,sizeof(s),"\r\nAms Timer Stat:\r\n");                                  
    len += snprintf(s+len,sizeof(s)-len,"%-36s:\r\n\
CreateTimer=%26ld,     KillTimer=%26ld,      TimerOut=%26ld\r\n",
                    strAmsTimerName[T_AMS_CRM_STATE_OPERATE_IND_TIMER],
                    AmsTimerStat.WaitCrmOperateIndRsp[0],
                    AmsTimerStat.WaitCrmOperateIndRsp[1],
                    AmsTimerStat.WaitCrmOperateIndRsp[2]);       
    len += snprintf(s+len,sizeof(s)-len,"%-36s:\r\n\
CreateTimer=%26ld,     KillTimer=%26ld,      TimerOut=%26ld\r\n",
                    strAmsTimerName[T_AMS_CUSTOMER_IN_QUEUE_TIMER],
                    AmsTimerStat.WaitCustomerGetTeller[0],
                    AmsTimerStat.WaitCustomerGetTeller[1],
                    AmsTimerStat.WaitCustomerGetTeller[2]);
    len += snprintf(s+len,sizeof(s)-len,"%-36s:\r\n\
CreateTimer=%26ld,     KillTimer=%26ld,      TimerOut=%26ld\r\n",
                    strAmsTimerName[T_AMS_REST_TIMER],
                    AmsTimerStat.WaitRestRsp[0],
                    AmsTimerStat.WaitRestRsp[1],
                    AmsTimerStat.WaitRestRsp[2]);
   if(Source == OMP_COMM)
    {
        OMSResultPrint(s,len,SocketId,Source);
        SendPrompt(SocketId);
    }
    else
    {
        MMLPrint(s,strlen(s),SocketId,Source);
    }

	return SUCCESS;
}

int DisplayAmsCommonResultStat(int SocketId,int Source)
{
	char             s[2048];//careful of len!! 
	int              i = 0;
	unsigned char    j = 0;
	int              len = 0;
		
    memset(s,0,sizeof(s));

    len = snprintf(s,sizeof(s),"\r\nAms Common Result Stat:\r\n");
    len += snprintf(s + len, sizeof(s) - len,
"\
amsSuccess                          =%20lu,\t amsParaErr                          =%20lu\r\n\
amsStateErr                         =%20lu,\t amsAllocMemFailed                   =%20lu\r\n\
amsAllocLpFailed                    =%20lu,\t amsAllocLtFailed                    =%20lu\r\n\r\n",
   
	AmsResultStat.amsSuccess,               AmsResultStat.amsParaErr,
	AmsResultStat.amsStateErr,              AmsResultStat.amsAllocMemFailed,
	AmsResultStat.amsAllocLpFailed,         AmsResultStat.amsAllocLtFailed);
	
    if(Source == OMP_COMM)
    {
        OMSResultPrint(s,len,SocketId,Source);
        SendPrompt(SocketId);
    }
    else
    {
        MMLPrint(s,strlen(s),SocketId,Source);
    }
	
	return SUCCESS;	
}

int DisplayAmsCrmLoginResultStat(int SocketId,int Source)
{
	char             s[2048];//careful of len!! 
	int              i = 0;
	unsigned char    j = 0;
	int              len = 0;
		
    memset(s,0,sizeof(s));

    len = snprintf(s,sizeof(s),"\r\nAms Crm Login Result Stat:\r\n");
    len += snprintf(s + len, sizeof(s) - len,
"\
tellerLoginSuccess                     =%20lu,\t tellerLoginParaErr                     =%20lu\r\n\
tellerLoginStateErr                    =%20lu,\t tellerLoginLenErr                      =%20lu\r\n\
tellerLoginLpResourceLimited           =%20lu,\t tellerLoginNodeResourceLimited         =%20lu\r\n\
tellerLoginTellerLenErr                =%20lu,\t tellerLoginTellerNoErr                 =%20lu\r\n\
tellerLoginTellerPwdErr                =%20lu,\t tellerLoginTellerVncAuthPwdErr         =%20lu\r\n\
tellerLoginTellerLoginRepeatedly       =%20lu,\t tellerLoginTellerTypeErr               =%20lu\r\n\
tellerLoginTellerNumErr                =%20lu,\t tellerLoginFileServerUsrNameErr        =%20lu\r\n\
tellerLoginFileServerUsrPwdErr         =%20lu,\t tellerLoginOrgIdErr                    =%20lu\r\n\
tellerLoginOrgStateErr                 =%20lu,\t tellerLoginLicenseTimeout              =%20lu\r\n\
tellerlogintellerloginnotresiter       =%20lu,\r tellerLoginTellerNumBeyondLic          =%20lu\r\n\r\n",

	AmsResultStat.crmLoginSuccess,               AmsResultStat.crmLoginParaErr,
	AmsResultStat.crmLoginStateErr,              AmsResultStat.crmLoginLenErr,
	AmsResultStat.crmLoginLpResourceLimited,     AmsResultStat.crmLoginNodeResourceLimited,
	AmsResultStat.crmLoginTellerLenErr,          AmsResultStat.crmLoginTellerNoErr, 
	AmsResultStat.crmLoginTellerPwdErr,          AmsResultStat.crmLoginTellerVncAuthPwdErr, 
	AmsResultStat.crmLoginTellerLoginRepeatedly, AmsResultStat.crmLoginTellerTypeErr,
	AmsResultStat.crmLoginTellerNumErr,          AmsResultStat.crmLoginFileServerUsrNameErr, 
	AmsResultStat.crmLoginFileServerUsrPwdErr,   AmsResultStat.crmLoginOrgIdErr,
	AmsResultStat.crmLoginOrgStateErr,           AmsResultStat.crmLoginLicenseTimeout,
	AmsResultStat.crmlogintellerloginnotresiter, AmsResultStat.crmLoginTellerNumBeyondLic);

    if(Source == OMP_COMM)
    {
        OMSResultPrint(s,len,SocketId,Source);
        SendPrompt(SocketId);
    }
    else
    {
        MMLPrint(s,strlen(s),SocketId,Source);
    }
	
	return SUCCESS;	
}

int DisplayAmsCrmStateOperateResultStat(int SocketId,int Source)
{
	char             s[4096];//careful of len!! 	                         
	int              i = 0;
	unsigned char    j = 0;
	int              len = 0;
		
    memset(s,0,sizeof(s));

    len = snprintf(s,sizeof(s),"\r\nAms Crm State Operate Result Stat:\r\n");
    len += snprintf(s + len, sizeof(s) - len,
"\
tellerStateOperateSuccess              =%20lu,\t tellerStateOperateParaErr              =%20lu\r\n\
tellerStateOperateStateErr             =%20lu,\t tellerStateOperateLenErr               =%20lu\r\n\
tellerStateOperateAmsPidErr            =%20lu,\t tellerStateOperateTellerIdErr          =%20lu\r\n\
tellerStateOperateSrvGrpIdErr          =%20lu,\t tellerStateOperateServiceStateErr      =%20lu\r\n\
tellerStateOperateCodeErr              =%20lu,\t tellerStateOperateOpReasonErr          =%20lu\r\n\
tellerStateOperateOpTimeLenErr         =%20lu,\t tellerStateOperateUpdateStateErr       =%20lu\r\n\
tellerStateOperateStartTimerErr        =%20lu,\t tellerStateOperateRestTimeout          =%20lu\r\n\r\n",

	AmsResultStat.crmStateOperateSuccess,                AmsResultStat.crmStateOperateParaErr,
	AmsResultStat.crmStateOperateStateErr,               AmsResultStat.crmStateOperateLenErr,
	AmsResultStat.crmStateOperateAmsPidErr,              AmsResultStat.crmStateOperateTellerIdErr,
	AmsResultStat.crmStateOperateSrvGrpIdErr,            AmsResultStat.crmStateOperateServiceStateErr, 
	AmsResultStat.crmStateOperateCodeErr,                AmsResultStat.crmStateOperateOpReasonErr, 
	AmsResultStat.crmStateOperateOpTimeLenErr,           AmsResultStat.crmStateOperateUpdateStateErr,
	AmsResultStat.crmStateOperateStartTimerErr,          AmsResultStat.crmStateOperateRestTimeout);

    len += snprintf(s + len, sizeof(s) - len,
"\
tellerStateOpRestTimeoutParaErr        =%20lu,\t tellerStateOpRestTimeoutStateErr       =%20lu\r\n\
tellerStateOpRestTimeoutLenErr         =%20lu,\t tellerStateOpRestTimeoutAmsPidErr      =%20lu\r\n\
tellerStateOpRestTimeoutTellerIdErr    =%20lu,\t tellerStateOpRestTimeoutSrvGrpIdErr    =%20lu\r\n\
tellerStateOpRestTimeoutSrvGrpIdErr    =%20lu\r\n\r\n",

	AmsResultStat.crmStateOpRestTimeoutParaErr,          AmsResultStat.crmStateOpRestTimeoutStateErr,
	AmsResultStat.crmStateOpRestTimeoutLenErr,           AmsResultStat.crmStateOpRestTimeoutAmsPidErr,
	AmsResultStat.crmStateOpRestTimeoutTellerIdErr,      AmsResultStat.crmStateOpRestTimeoutSrvGrpIdErr,
	AmsResultStat.crmStateOpRestTimeoutSrvGrpIdErr);

    if(Source == OMP_COMM)
    {
        OMSResultPrint(s,len,SocketId,Source);
        SendPrompt(SocketId);
    }
    else
    {
        MMLPrint(s,strlen(s),SocketId,Source);
    }
	
	return SUCCESS;	
}

int DisplayAmsCmsEventNoticeResultStat(int SocketId,int Source)
{
	char             s[2048];//careful of len!! 
	int              i = 0;
	unsigned char    j = 0;
	int              len = 0;
		
    memset(s,0,sizeof(s));

    len = snprintf(s,sizeof(s),"\r\nAms Cms Event Notice Result Stat:\r\n");
    len += snprintf(s + len, sizeof(s) - len,
"\
cmsEventNoticeSuccess               =%20lu,\t cmsEventNoticeParaErr               =%20lu\r\n\
cmsEventNoticeStateErr              =%20lu,\t cmsEventNoticeSrvGrpIdErr           =%20lu\r\n\
cmsEventNoticeServiceStateErr       =%20lu,\t cmsEventNoticeLenErr                =%20lu\r\n\
cmsEventNoticeAmsPidErr             =%20lu,\t cmsEventNoticeCallIdErr             =%20lu\r\n\
cmsEventNoticeTellerIdErr           =%20lu,\t cmsEventNoticeVtmIdErr              =%20lu\r\n\
cmsEventNoticeCodeErr               =%20lu,\t cmsEventNoticeCrmStateErr           =%20lu\r\n\
cmsEventNoticeVtmStateErr           =%20lu,\t cmsEventNoticeOrgnTellerIdErr       =%20lu\r\n\
cmsEventNoticeOrgnTellerStateErr    =%20lu,\t cmsEventNoticeOrgnTellerAmsPidErr   =%20lu\r\n\
cmsEventNoticeOrgnTellerSrvGrpIdErr =%20lu,\t cmsEventNoticeOrgnTellerSrvStateErr =%20lu\r\n\
cmsHandshakeClearInactiveCall       =%20lu\r\n\r\n",

	AmsResultStat.cmsEventNoticeSuccess,               AmsResultStat.cmsEventNoticeParaErr,
	AmsResultStat.cmsEventNoticeStateErr,              AmsResultStat.cmsEventNoticeSrvGrpIdErr,
	AmsResultStat.cmsEventNoticeServiceStateErr,       AmsResultStat.cmsEventNoticeLenErr, 
	AmsResultStat.cmsEventNoticeAmsPidErr,             AmsResultStat.cmsEventNoticeCallIdErr, 
	AmsResultStat.cmsEventNoticeTellerIdErr,           AmsResultStat.cmsEventNoticeVtmIdErr,
	AmsResultStat.cmsEventNoticeCodeErr,               AmsResultStat.cmsEventNoticeCrmStateErr,     
	AmsResultStat.cmsEventNoticeVtmStateErr,           AmsResultStat.cmsEventNoticeOrgnTellerIdErr,
	AmsResultStat.cmsEventNoticeOrgnTellerStateErr,    AmsResultStat.cmsEventNoticeOrgnTellerAmsPidErr,
	AmsResultStat.cmsEventNoticeOrgnTellerSrvGrpIdErr, AmsResultStat.cmsEventNoticeOrgnTellerSrvStateErr,
	AmsResultStat.cmsHandshakeClearInactiveCall);

    if(Source == OMP_COMM)
    {
        OMSResultPrint(s,len,SocketId,Source);
        SendPrompt(SocketId);
    }
    else
    {
        MMLPrint(s,strlen(s),SocketId,Source);
    }
	
	return SUCCESS;	
}

int DisplayAmsCmsCrmCalloutResultStat(int SocketId,int Source)
{
	char             s[2048];//careful of len!! 
	int              i = 0;
	unsigned char    j = 0;
	int              len = 0;
		
    memset(s,0,sizeof(s));

    len = snprintf(s,sizeof(s),"\r\nAms Cms Event Notice Result Stat:\r\n");
    len += snprintf(s + len, sizeof(s) - len,
"\
cmsCalloutSuccess                   =%20lu,\t cmsCalloutAmspidErr                 =%20lu\r\n\
cmsCalloutParaErr                   =%20lu,\t cmsCalloutStateErr                  =%20lu\r\n\
cmsCalloutcrmCallidErr              =%20lu,\t cmsCalloutcrmTelleridErr            =%20lu\r\n\
cmsCalloutcrmSrvgrpidErr            =%20lu,\t cmsCalloutcrmSrvtypeErr             =%20lu\r\n\
cmsCalloutcrmTelleridNotcfgErr      =%20lu,\t cmsCalloutcrmTelleridNotregErr      =%20lu\r\n\
cmsCalloutcrmTelleridNotloginErr    =%20lu,\t cmsCalloutcrmRepeatErr              =%20lu\r\n\r\n",	

	AmsResultStat.cmscrmCalloutSuccess,				AmsResultStat.cmsCalloutAmspidErr,
	AmsResultStat.cmsCalloutParaErr,				AmsResultStat.cmsCalloutStateErr,
	AmsResultStat.cmsCalloutcrmCallidErr,			AmsResultStat.cmsCalloutcrmTelleridErr,
	AmsResultStat.cmsCalloutcrmSrvgrpidErr,			AmsResultStat.cmsCalloutcrmSrvtypeErr,
	AmsResultStat.cmsCalloutcrmTelleridNotcfgErr,	AmsResultStat.cmsCalloutcrmTelleridNotregErr,
	AmsResultStat.cmsCalloutcrmTelleridNotloginErr,	AmsResultStat.cmsCalloutcrmRepeatErr);

	if(Source == OMP_COMM)
    {
        OMSResultPrint(s,len,SocketId,Source);
        SendPrompt(SocketId);
    }
    else
    {
        MMLPrint(s,strlen(s),SocketId,Source);
    }
	
	return SUCCESS;	
}

int DisplayAmsCmsGetCrmResultStat(int SocketId,int Source)
{
	char			 s[2048];//careful of len!! 
		int 			 i = 0;
		unsigned char	 j = 0;
		int 			 len = 0;
			
		memset(s,0,sizeof(s));
	
		len = snprintf(s,sizeof(s),"\r\nAms Cms Event Notice Result Stat:\r\n");
		len += snprintf(s + len, sizeof(s) - len,
"\
cmsGetCrmSuccess                  =%20lu,\t cmsGetCrmParaErr                      =%20lu\r\n\
cmsGetCrmLenErr                   =%20lu,\t cmsGetCrmAmsPidErr                    =%20lu\r\n\
cmsGetCrmCallIdErr                =%20lu,\t cmsGetCrmTerminalTypeErr              =%20lu\r\n\
cmsGetCrmSrvGrpIdErr              =%20lu,\t cmsGetCrmServiceTypeErr               =%20lu\r\n\
cmsGetCrmNoValidSrvGrpOrType      =%20lu,\t cmsGetCrmCallTypeErr                  =%20lu\r\n\
cmsGetCrmOriginTellerIdErr        =%20lu,\t cmsGetCrmOriginTellerStateErr         =%20lu\r\n\
cmsGetCrmOriginTellerAmsPidErr    =%20lu,\t cmsGetCrmOriginTellerSrvGrpIdErr      =%20lu\r\n\
cmsGetCrmOriginTellerSrvStateErr  =%20lu,\t	cmsGetCrmTargetTypeErr                =%20lu\r\n\
cmsGetCrmTargetGroupErr           =%20lu,\t cmsGetCrmTargetTellerIdErr            =%20lu\r\n\
cmsGetCrmTargetTellerStateErr     =%20lu,\t	cmsGetCrmVtmIdErr                     =%20lu\r\n\
cmsGetCrmAssocCallIdErr           =%20lu,\t cmsGetCrmLicenseTimeout               =%20lu\r\n\
cmsGetCrmTpsBeyondLic             =%20lu,\t cmsGetCrmServiceInQueue               =%20lu\r\n\
cmsGetCrmSIRErr                   =%20lu,\t cmsGetCrmSIRVtmNoErr                  =%20lu\r\n\
cmsGetCrmSIRSrvGrpIdErr           =%20lu,\t cmsGetCrmSIRCallTypeErr               =%20lu\r\n\
cmsGetCrmSIRCrmEmpty              =%20lu,\t cmsGetCrmSIRCalcQueueInfoErr          =%20lu\r\n\
cmsGetCrmSIRTooManyCustInQueue    =%20lu,\t cmsGetCrmSIRTargetTellerNotLogin      =%20lu\r\n\
cmsGetCrmSIRTargetTellerNotReg    =%20lu,\t cmsGetCrmSIRTargetTellerStateErr      =%20lu\r\n\
cmsGetCrmSIRCallTransTooManyTimes =%20lu,\t cmsGetCrmOrgIdErr                     =%20lu\r\n\
cmsGetCrmOrgPosErr                =%20lu,\t cmsGetCrmParentOrgIdErr               =%20lu\r\n\
cmsGetCrmSelectSrvGrpIdErr        =%20lu,\t cmsGetCrmLpResourceLimited            =%20lu\r\n\
cmsGetCrmCreateTimerErr           =%20lu,\t cmsGetCrmTimeout                      =%20lu\r\n\
cmsGetCrmTimeoutParaErr           =%20lu,\t cmsGetCrmTimeoutStateErr              =%20lu\r\n\
cmsGetCrmTimeoutLenErr            =%20lu,\t cmsGetCrmTimeoutVtmIdErr              =%20lu\r\n\
cmsGetCrmTimeoutServiceStateErr=%20lu\r\n",
			AmsResultStat.cmsGetCrmSuccess,			AmsResultStat.cmsGetCrmParaErr,
			AmsResultStat.cmsGetCrmLenErr,			AmsResultStat.cmsGetCrmAmsPidErr,
			AmsResultStat.cmsGetCrmCallIdErr,			AmsResultStat.cmsGetCrmTerminalTypeErr,
			AmsResultStat.cmsGetCrmSrvGrpIdErr,			AmsResultStat.cmsGetCrmServiceTypeErr,
			AmsResultStat.cmsGetCrmNoValidSrvGrpOrType,			AmsResultStat.cmsGetCrmCallTypeErr,
			AmsResultStat.cmsGetCrmOriginTellerIdErr,			AmsResultStat.cmsGetCrmOriginTellerStateErr,
			AmsResultStat.cmsGetCrmOriginTellerAmsPidErr,			AmsResultStat.cmsGetCrmOriginTellerSrvGrpIdErr,
			AmsResultStat.cmsGetCrmOriginTellerSrvStateErr, 			AmsResultStat.cmsGetCrmTargetTypeErr,
			AmsResultStat.cmsGetCrmTargetGroupErr, 			AmsResultStat.cmsGetCrmTargetTellerIdErr,
			AmsResultStat.cmsGetCrmTargetTellerStateErr,			AmsResultStat.cmsGetCrmVtmIdErr,
			AmsResultStat.cmsGetCrmAssocCallIdErr,			AmsResultStat.cmsGetCrmLicenseTimeout,
			AmsResultStat.cmsGetCrmTpsBeyondLic,			AmsResultStat.cmsGetCrmServiceInQueue,
			AmsResultStat.cmsGetCrmSIRErr,			AmsResultStat.cmsGetCrmSIRVtmNoErr,
			AmsResultStat.cmsGetCrmSIRSrvGrpIdErr,			AmsResultStat.cmsGetCrmSIRCallTypeErr,
			AmsResultStat.cmsGetCrmSIRCrmEmpty,			AmsResultStat.cmsGetCrmSIRCalcQueueInfoErr,
			AmsResultStat.cmsGetCrmSIRTooManyCustInQueue,			AmsResultStat.cmsGetCrmSIRTargetTellerNotLogin,
			AmsResultStat.cmsGetCrmSIRTargetTellerNotReg,			AmsResultStat.cmsGetCrmSIRTargetTellerStateErr,
			AmsResultStat.cmsGetCrmSIRCallTransTooManyTimes,			AmsResultStat.cmsGetCrmOrgIdErr,
			AmsResultStat.cmsGetCrmOrgPosErr,			AmsResultStat.cmsGetCrmParentOrgIdErr,
			AmsResultStat.cmsGetCrmSelectSrvGrpIdErr,			AmsResultStat.cmsGetCrmLpResourceLimited,
			AmsResultStat.cmsGetCrmCreateTimerErr,			AmsResultStat.cmsGetCrmTimeout,
			AmsResultStat.cmsGetCrmTimeoutParaErr,			AmsResultStat.cmsGetCrmTimeoutStateErr,
			AmsResultStat.cmsGetCrmTimeoutLenErr,			AmsResultStat.cmsGetCrmTimeoutVtmIdErr,
			AmsResultStat.cmsGetCrmTimeoutServiceStateErr);

	if(Source == OMP_COMM)
    {
        OMSResultPrint(s,len,SocketId,Source);
        SendPrompt(SocketId);
    }
    else
    {
        MMLPrint(s,strlen(s),SocketId,Source);
    }
	
	return SUCCESS;	


}

int DisplayAmsResultStat(int SocketId,int Source)
{
	char             s[2048];//careful of len!! 
	int              i = 0;
	unsigned char    j = 0;
	int              len = 0;

    memset(s,0,sizeof(s));
	
    len = snprintf(s,sizeof(s),"\r\nAms Result Stat:\r\n");
	
    if(Source == OMP_COMM)
    {
        OMSResultPrint(s,len,SocketId,Source);
        SendPrompt(SocketId);
    }
    else
    {
        MMLPrint(s,strlen(s),SocketId,Source);
    }
	
	DisplayAmsCommonResultStat(SocketId, Source);

	//disp ams <-> crm stat
	DisplayAmsCrmLoginResultStat(SocketId, Source);
	
//	DisplayAmsCrmLogoutResultStat(SocketId, Source);
	
	DisplayAmsCrmStateOperateResultStat(SocketId, Source);
	
//	DisplayAmsCrmModifyPasswordResultStat(SocketId, Source);
	
//	DisplayAmsCrmForceLoginResultStat(SocketId, Source);
	
//	DisplayAmsCrmQueryInfoResultStat(SocketId, Source);	

	//disp ams <-> cms stat
//	DisplayAmsCmsCrmRegResultStat(SocketId, Source);
		
	DisplayAmsCmsGetCrmResultStat(SocketId, Source);

	DisplayAmsCmsCrmCalloutResultStat(SocketId,Source);
	
	DisplayAmsCmsEventNoticeResultStat(SocketId, Source);
	
//	DisplayAmsCmsEventIndResultStat(SocketId, Source); 

    memset(s,0,sizeof(s));
		
    len = snprintf(s,sizeof(s),
"\r\n\
amsUnknownResultType                =%20lu,\t amsUnknownResult                    =%20lu\r\n\r\n",      

	AmsResultStat.amsUnknownResultType,       AmsResultStat.amsUnknownResult );

    if(Source == OMP_COMM)
    {
        OMSResultPrint(s,len,SocketId,Source);
        SendPrompt(SocketId);
    }
    else
    {
        MMLPrint(s,strlen(s),SocketId,Source);
    }
	
	return SUCCESS;	
}


int DisplayAmsStat(MMLCMD_t r[],int SocketId,int Source)
{
	char             s[2048];//careful of len!! 
	int              i = 0;
	unsigned char    j = 0;
	int              len = 0;
	int              type = AMS_STAT_TYPE_NULL;
	unsigned int     tellerId = 0;
	unsigned int     vtmId = 0;
	unsigned int     rcasId = 0;
	
	memset(s,0,sizeof(s));

	if(Source == OMP_COMM)
	{
		for(i = 0; i < r[0].CountOfCell; i++)
		{
			if (!CompTwoString(r[0].CmdCell[i][0],"TYPE"))
			{
				type = atoi(r[0].CmdCell[i][1]);
				continue;
			}
			
			if (!CompTwoString(r[0].CmdCell[i][0],"TELLERID"))
			{
				tellerId = atoi(r[0].CmdCell[i][1]);
				continue;
			}	
		}
	}
	else if(Source == TELNET_COMM)
		{
			for(i = 0; i < r[0].CountOfCell; i++)			
			{
				if (!CompTwoString(r[0].CmdCell[i][0],"TYPE")) 
				{
					for(j = 0; j < strlen(r[0].CmdCell[i][1]); j++)
					{
						if(isdigit(r[0].CmdCell[i][1][j]))
						{
							continue;
						}
						else
						{
							sprintf(s,":RESULT:ERRORMSG=\x22TypeErr\x22;");
							MMLPrint(s,strlen(s),SocketId,Source);
							return -1;
						}
					}
					
					type = atoi(r[0].CmdCell[i][1]);
				}
	
				if (!CompTwoString(r[0].CmdCell[i][0],"TELLERID")) 
				{
					for(j = 0; j < strlen(r[0].CmdCell[i][1]); j++)
					{
						if(isdigit(r[0].CmdCell[i][1][j]))
						{
							continue;
						}
						else
						{
							sprintf(s,":RESULT:ERRORMSG=\x22TellerIdErr\x22;");
							MMLPrint(s,strlen(s),SocketId,Source);
							return -1;
						}
					}
					
					tellerId = atoi(r[0].CmdCell[i][1]);
				}
			}
		}
		switch(type)
		{
		case AMS_STAT_TYPE_NULL:
		case AMS_MSG_STAT_TYPE:
			DisplayAmsMsgStat(SocketId, Source);
				
			if (AMS_MSG_STAT_TYPE == type)
			{
				break;
			}
			
		case AMS_TIMER_STAT_TYPE:
			DisplayAmsTimerStat(SocketId, Source);	
			
			if (AMS_TIMER_STAT_TYPE == type)
			{		
				break;
			}
		case AMS_RESULT_STAT_TYPE:
			DisplayAmsResultStat(SocketId, Source);	 
			break;
		default:
			if (AMS_STAT_TYPE_NULL != type)
			{
				memset(s,0,sizeof(s));
				
				len = snprintf(s,sizeof(s),":RESULT:Type=%d,ERRORMSG=\x22StatTypeErr\x22;",type);
				
				if(Source == OMP_COMM)
				{
					OMSResultPrint(s,len,SocketId,Source);
					SendPrompt(SocketId);
				}
				else
				{
					MMLPrint(s,strlen(s),SocketId,Source);
				}
				
				return 0;	
			}
		}
		
		return SUCCESS;
}

int ResetAmsStat(MMLCMD_t r[],int SocketId,int Source)
{
	char             s[2048];
	int              i = 0;
	unsigned char    j = 0;
	int              len = 0;
	int              type = AMS_STAT_TYPE_NULL;
	TELLER_STATE_INFO tellerStateInfo[AMS_MAX_CRM_NUM]; // init later
	
	memset(s,0,sizeof(s));

	if(Source == OMP_COMM)
	{
		for(i = 0; i < r[0].CountOfCell; i++)
		{
			if (!CompTwoString(r[0].CmdCell[i][0],"TYPE"))
			{
				type = atoi(r[0].CmdCell[i][1]);
				continue;
			}
			
		}
	}
	else if(Source == TELNET_COMM)
	{
		if(r[0].CountOfCell > 0) 
        {
    		if (!CompTwoString(r[0].CmdCell[0][0],"TYPE")) 
    		{
    			for(j = 0; j < strlen(r[0].CmdCell[0][1]); j++)
    			{
    				if(isdigit(r[0].CmdCell[0][1][j]))
    				{
    					continue;
    				}
    				else
    				{
    					sprintf(s,":RESULT:ERRORMSG=\x22TypeErr\x22;");
    					MMLPrint(s,strlen(s),SocketId,Source);
    					return -1;
    				}
    			}
    		}
            type = atoi(r[0].CmdCell[0][1]);
        }
	}
	
	switch(type)
	{
	case AMS_STAT_TYPE_NULL:
	case AMS_MSG_STAT_TYPE:
		memset(&AmsMsgStat, 0, sizeof(AMS_MSG_STAT));
		
		if (AMS_MSG_STAT_TYPE == type)
		{
			break;
		}
	case AMS_TIMER_STAT_TYPE:
		memset(&AmsTimerStat, 0, sizeof(AMS_TIMER_STAT));
		
		if (AMS_TIMER_STAT_TYPE == type)
		{		
			break;
		}
		
/* 	case AMS_TELLER_STAT_TYPE:	
		memset(tellerStateInfo, 0, sizeof(TELLER_STATE_INFO) * AMS_MAX_CRM_NUM);
		
		for(i = 0; i < Min(SystemData.AmsPriData.amsCfgData.maxCrmNum, AMS_MAX_CRM_NUM); i++)
		{
			memcpy(&tellerStateInfo[i], &AmsTellerStat(i).crmStateInfo, sizeof(TELLER_STATE_INFO)); //暂存crmStateInfo
		}

		memset(&AmsTellerStat(0), 0, sizeof(AMS_TELLER_STAT) * AMS_MAX_CRM_NUM);
		for(i = 0; i < Min(SystemData.AmsPriData.amsCfgData.maxCrmNum, AMS_MAX_CRM_NUM); i++)
		{
			AmsTellerStat(i).tellerId = AmsCfgTeller(i).tellerId; //恢复tellerId
			memcpy(&AmsTellerStat(i).crmStateInfo, &tellerStateInfo[i], sizeof(TELLER_STATE_INFO)); //恢复crmStateInfo
		}
		
		if (AMS_TELLER_STAT_TYPE == type)
		{		
			break;
		}  	
*/		
 	case AMS_RESULT_STAT_TYPE:
		memset(&AmsResultStat, 0, sizeof(AMS_RESULT_STAT));
		
		if (AMS_RESULT_STAT_TYPE == type)
		{
			break;
		}  
		
	default:
        if (AMS_STAT_TYPE_NULL != type)
        {
    		memset(s,0,sizeof(s));
			
    		len = snprintf(s,sizeof(s),":RESULT:Type=%d,ERRORMSG=\x22StatTypeErr\x22;",type);
			
            if(Source == OMP_COMM)
            {
                OMSResultPrint(s,len,SocketId,Source);
                SendPrompt(SocketId);
            }
            else
            {
                MMLPrint(s,strlen(s),SocketId,Source);
            }
			
    		return 0;	
        }
    }

    if (OMP_COMM == Source)
	{
		len = snprintf(s,sizeof(s),"Reset Success");
		MMLSuccessPrint(s,len,SocketId,Source);
	}
	else
	{
		sprintf(s,":RESULT:RETURNMSG=\"Reset Success\"");
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

	MMLInstallCommand("DISPLAY","AMS","STAT",&DisplayAmsStat,
		":DISPLAY-AMS-STAT;",
		":DISPLAY-AMS-STAT:TYPE=,TELLERID=;");

    MMLInstallCommand("RESET","AMS","STAT",&ResetAmsStat,
    	":RESET-AMS-STAT;",
    	":RESET-AMS-STAT;");

		
	return AMS_SUCCESS;
}



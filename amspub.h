#ifndef AMSPUB_H
#define	AMSPUB_H

#define AMS_MAX_VTA_NUM				(10000)
#define AMS_MAX_VTM_NUM				(5000)

#define AMS_MAX_VTA_NODES				(10000) //最大坐席节点数
#define AMS_MAX_VTM_NODES				(5000) //最大用户节点数
#define AMS_MAX_SERVICE_TYPE_NUM		(128) //AMS业务类型最大配置数

#define AMS_MAX_SERVICE_NUM				(64)
#define AMS_MAX_SERVICE_USED_NUM		(32)

#define AMS_MAX_STRING_WORD_LEN			(256)

#define AMS_MAX_SERVICE_GROUP_NUM		(64)
#define AMS_MAX_SERVICE_GROUP_ID_MIN	(0)
#define AMS_MAX_SERVICE_GROUP_ID_MAX	(AMS_MAX_SERVICE_GROUP_NUM - 1)
#define AMS_MAX_SERVICE_ID_VALUE		(64)
#define AMS_MAX_SERVICE_GROUP_NAME_LEN	(32)

#define AMS_MAX_NAME_LEN				30
#define AMS_MAX_TELLER_ID_LEN			30
#define AMS_MAX_VTM_ID_LEN				30
#define AMS_MAX_SERVICE_NAME_LEN		32
#define AMS_MAX_TRANS_IP_LEN			16

#define AMS_MAX_PWD_LEN					32

#define AMS_MAX_CALLID_LEN				30
#define	AMS_MAX_FILEPATH_LEN	                       128
#define	AMS_MAX_FILENAME_LEN	                       128


#define T_AMS_REST_TIMER_LENGTH_MAX                    (10000)       //AMS休息最大定时10000s
#define AMS_VTA_ID_HASH_SIZE                           (AMS_MAX_VTA_NUM)
#define AMS_VTM_ID_HASH_SIZE                           (AMS_MAX_VTM_NUM)

#define	AMS_MAX_TIMER_PARA_LEN                         (PARA_LEN)


typedef struct tellerLoginInfo_t
{
	unsigned char	tellerIdLen;	//坐席工号长度
	unsigned char	tellerId[AMS_MAX_TELLER_ID_LEN + 1];

	unsigned char	tellerPwdLen;
	unsigned char   tellerPwd[AMS_MAX_PWD_LEN + 1];
}TELLER_LOGIN_INFO;

typedef struct tellLoginInfoNode_t
{
	NODE	node;
	struct tellLoginInfoNode_t *hashNext;

	TELLER_LOGIN_INFO	tellerLoginInfo;
}TELLER_LOGIN_INFO_NODE;

typedef struct termNetInfo_t
{
	DWORD           ip;
	WORD            port;
	BYTE			ipv6[16];
}TERM_NET_INFO;

typedef struct tellerRegisterInfo_t
{
	unsigned char   flag;	                       //是否注册	
	
	DWORD           tellerIdLen;                      //柜员标识
	unsigned char	tellerId[AMS_MAX_TELLER_ID_LEN + 1];
	
	DWORD           vtaIp;
	WORD            vtaPort;
	
	PID_t	        myPid;
	PID_t	        cmsPid;
	
}TELLER_REGISTER_INFO;


typedef struct tellerRegisterInfoNode_t
{
	NODE	node;
	struct tellLoginInfoNode_t *hashNext;

	TELLER_REGISTER_INFO tellerRegInfo;
}TELLER_REGISTER_INFO_NODE;

//struct tm {
//	int tm_sec;   /* 秒 – 取值区间为[0,59] */ 
//	int tm_min;   /* 分 - 取值区间为[0,59] */ 
//	int tm_hour;  /* 时 - 取值区间为[0,23] */ 
//	int tm_mday;  /* 一个月中的日期 - 取值区间为[1,31] */ 
//	int tm_mon;   /* 月份（从一月开始，0代表一月） - 取值区间为[0,11] */ 
//	int tm_year;  /* 年份，其值等于实际年份减去1900 */ 
//	int tm_wday;  /* 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */ 
//	int tm_yday;  /* 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推 */ 
//	int tm_isdst; /* 夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的进候，tm_isdst为0；不了解情况时，tm_isdst()为负。*/
//}

typedef struct timeInfo_t
{
	WORD           year;     
	BYTE           month;
	BYTE           day;
	BYTE           hour;
	BYTE           minute;
	BYTE           second;
	
}TIME_INFO;

typedef struct fileInfo_t
{
	WORD           fileType;     
	DWORD          serverIp;
	WORD           serverPort;
	DWORD          maxFileSize;
	DWORD          maxFileTimeLen;
	DWORD          fileSize;	
	BYTE           filePathLen;
	BYTE           filePath[AMS_MAX_FILEPATH_LEN + 1];	
	BYTE           fileNameLen;
	BYTE           fileName[AMS_MAX_FILENAME_LEN + 1];
	
}FILE_INFO;


typedef struct tellerWorkInfo_t
{
	WORD          connectNum;           //柜员工作量
	DWORD         workSeconds;          //柜员工作时长
	DWORD         idleSeconds;          //柜员空闲时长

}TELLER_WORK_INFO;

typedef struct tellerStateInfo_t
{
	DWORD           lastState;  	                      //最后更新的状态
	time_t          lastStateStartTime;                   //最后更新状态的时间  用于计算柜员的连续服务时间，离线时间，离开时间

}TELLER_STATE_INFO;

typedef struct stateOperateInfo_t
{
	WORD          reason;
	WORD          timeLen;

}STATE_OP_INFO;


typedef struct tellerInfo_t
{
	unsigned char   flag;                          //是否配置

	unsigned char   idState;                       //柜员工号状态

	DWORD           tellerIdLen;                      //柜员标识
	unsigned char	tellerId[AMS_MAX_TELLER_ID_LEN + 1];

	DWORD           tellerType;                    //柜员类型

	DWORD           srvGrpIdLen;                      //业务组编号
	unsigned char   srvGrpId[AMS_MAX_SERVICE_GROUP_NAME_LEN +1];
	DWORD			srvGrpIdPos;

	DWORD           tellerSrvAuthRsvd;             //Service Authority 业务处理权限预留
	DWORD           tellerSrvAuth;                 //Service Authority 业务处理权限

	//unsigned char   tellerNoLen;                     //柜员工号长度 zhuyn added 20160621 
	//unsigned char   tellerNo[AMS_MAX_TELLER_NO_LEN + 1]; 

	unsigned char   tellerNameLen;                 //柜员姓名长度
	unsigned char   tellerName[AMS_MAX_NAME_LEN + 1];

	unsigned char   tellerPwdLen;                  //柜员密码长度
	unsigned char   tellerPwd[AMS_MAX_PWD_LEN + 1];

	unsigned char   tellerOldPwdLen;               //柜员旧密码长度
	unsigned char   tellerOldPwd[AMS_MAX_PWD_LEN + 1];	

/*	unsigned char   tellerDepartmentNameLen;       //柜员部门名称长度
	unsigned char   tellerDepartmentName[AMS_MAX_DEPARTMENT_NAME_LEN + 1];	

	unsigned char   areaCodeLen;                   //柜员电话区号长度
	unsigned char   areaCode[AMS_MAX_AREA_CODE_NUM + 1];   //柜员电话区号

	unsigned char   phoneNumLen;                   //柜员电话号码长度
	unsigned char   phoneNum[AMS_MAX_PHONE_NUM + 1];   //柜员电话号码	

	DWORD           orgId;                         //机构ID
//	VNC_AUTH_PARA   vncAuth;                       //远程接入鉴权密码*/

	unsigned char   transIpLen;                        //柜员IP地址字符串长度
	unsigned char   transIp[AMS_MAX_TRANS_IP_LEN + 1]; //柜员IP地址字符串	
	DWORD           vtaIp;                         //in_addr_t
	WORD            vtaPort;                       //Sip
	WORD            vtaScreenRecPort; 	
	WORD            vtaRemoteCoopPort;             //vta pack          
	WORD            vtaRemoteCoopType;             //vta pack  
	
}TELLER_INFO;

typedef struct tellerInfoNode_t
{
	NODE	node;
	struct tellerInfoNode_t *hashNext;

	unsigned int	tellerInfopos; //amsCfgData里tellerinfo数组对应的下标
	
	unsigned char	tellerIdLen;
	unsigned char	tellerId[AMS_MAX_TELLER_ID_LEN + 1];
	
}TELLER_INFO_NODE;

typedef struct vtmInfo_t
{
	unsigned char   flag;	                       //是否配置

	DWORD           vtmIdLen;                         //柜员机标识
	unsigned char   vtmId[AMS_MAX_VTM_ID_LEN + 1];
	

	DWORD           terminalType;                  //终端类型

	DWORD           srvGrpIdLen;                      //业务组编号
	unsigned char	srvGrpId[AMS_MAX_SERVICE_GROUP_NAME_LEN + 1];

//	unsigned char   vtmNoLen;                      //柜员机设备号长度 zhuyn added 20160621 
//	unsigned char   vtmNo[AMS_MAX_VTM_NO_LEN + 1];

	unsigned char   vtmNameLen;                    //柜员机名称长度
	char            vtmName[AMS_MAX_NAME_LEN + 1];

	unsigned char   vtmPwdLen;                     //柜员机密码长度
	unsigned char   vtmPwd[AMS_MAX_PWD_LEN + 1];	

	DWORD           orgId;                         //机构ID

//	VNC_AUTH_PARA   vncAuth;                       //远程接入鉴权密码
	
	unsigned char   transIpLen;                        //柜员机IP地址字符串长度
	unsigned char   transIp[AMS_MAX_TRANS_IP_LEN + 1]; //柜员机IP地址字符串	
	DWORD           vtmIp;                         //in_addr_t
	DWORD           vtmPort;                       //Sip
	WORD            vtmRemoteCoopPort; 
	BYTE            vtmRemoteCoopType; 

}VTM_INFO;


typedef struct vtmInfoNode_t
{
	NODE	node;
	struct vtmInfoNode_t *hashNext;

	unsigned int	vtmInfopos; //amsCfgData里vtminfo数组对应的下标
	
	unsigned char	vtmIdLen;
	unsigned char	vtmId[AMS_MAX_VTM_ID_LEN + 1];	
}VTM_INFO_NODE;

typedef struct serviceInfo_t
{
	unsigned char   flag;                          //是否配置

	DWORD           service;                       //业务编号

	unsigned char   serviceNameLen;                //业务名称长度
	unsigned char   serviceName[AMS_MAX_SERVICE_NAME_LEN + 1];	

}SERVICE_INFO;

typedef struct serviceGroupInfo_t
{
	unsigned char   flag;                          //是否配置

	unsigned char   isAutoFlag;                   //是否智能坐席 1 智能坐席 2 非智能坐席
	
	DWORD           srvGrpIdLen;                      //业务组编号
	unsigned char	srvGrpId[AMS_MAX_SERVICE_GROUP_NAME_LEN + 1];
	
	//DWORD           srvTypeRsvd;                   //Service Rsvd 预留业务
	//DWORD           serviceType;                   //Service  业务
	
	unsigned int 	srvlogpos; 						//配置的位置
	SERVICE_INFO    srvInfo[AMS_MAX_SERVICE_NUM]; //业务

//	unsigned char   srvGroupNameLen;               //业务组合名称长度
//	unsigned char   srvGroupName[AMS_MAX_SERVICE_GROUP_NAME_LEN + 1];	
	
}SERVICE_GROUP_INFO;

typedef struct queueSysInfo_t
{
	unsigned char   flag;                          //是否配置

	DWORD           srvGrpIdLen;                      //业务组编号
	unsigned char	srvGrpId[AMS_MAX_SERVICE_GROUP_NAME_LEN + 1];
	
	unsigned short  maxQueLen;                     //此业务组编号允许的最大排队长度
	unsigned short  avgSrvTime;	                   //此业务组编号客户平均服务时长

}QUEUE_SYS_INFO;

/*typedef struct serviceGroupInfoNode_t
{
	NODE	node;
	struct serviceGroupInfoNode_t *hashNext;

	SERVICE_GROUP_INFO	serviceGroupInfo;
}SERVICE_GROUP_INFO_NODE;*/



/* 柜员结点结构体定义 */
typedef struct vtaNode_t
{
	NODE            node; 

	DWORD           amsPid;
	DWORD           state;  
	DWORD           callState;    	
	TIME_INFO       stateStartLocalTime; 
	TIME_INFO       callStateStartLocalTime; 	
	time_t          startTime;		
	time_t          stateStartTime;	
	time_t          callStateStartTime;		
	time_t          handshakeTime;
	time_t          workInfoUpdateTime;	
	time_t          callTransferFreezeTime;	
	time_t          updateScoreExpectTime;
	
	TELLER_INFO     vtaInfo;

	TELLER_WORK_INFO vtaWorkInfo;

	DWORD           callTransferNum;
		
	DWORD           dailyAvgScore;           //enlarge 1000
	DWORD           dailyAvgScoreExpect;     //enlarge 1000   

	DWORD           tellerCfgPos;
	DWORD           tellerRegPos;
	DWORD           orgCfgPos;
	
	//OACBPARA_t      oaCbPara;       //网管回调用参数 zhuyn 20160704
	
}VTA_NODE;


/* 终端结点结构体定义 */
typedef struct vtmNode_t
{
	NODE            node;  

	//Customer In Queue Pid
	DWORD           customerPid;

	DWORD           amsPid;
	DWORD           state;  
	DWORD           callState;  
	DWORD           serviceState; 	

	int             sendMsgToVtaState;
	int             sendMsgToVtmState;
	
	int             sendFileToVtaState;
	int             sendFileToVtmState;
	
	int             vtmParaCfgState;

	TIME_INFO       stateStartLocalTime; 	
	TIME_INFO       callStateStartLocalTime; 
	TIME_INFO       serviceStateStartLocalTime;	
	time_t          stateStartTime;	
	time_t          callStateStartTime;	
	time_t          serviceStateStartTime;			
	time_t          handshakeTime;
	time_t          enterQueueTime;

	DWORD           serviceType;

	//timer
	int             iTimerId; //Not Use Yet
	int             sendMsgToVtaTimerId;  //VTA
	int             sendMsgToVtmTimerId;  //VTM
	int             sendFileToVtaTimerId; //VTA
	int             sendFileToVtmTimerId; //VTM
	int             vtmParaCfgTimerId;    //VTM
//	int		        iTimerType;

	//communication pid
	PID_t	        myPid;
	PID_t	        rPid;     //vtm
	PID_t	        cmsPid;   //cms
	PID_t	        vtaPid;   //VTA	

	VTM_INFO        vtmInfo;

	//VNC_AUTH_PARA   vncAuth;                       //远程接入鉴权密码

	DWORD           vtmCfgPos;
	DWORD           vtmRegPos;
	DWORD           orgCfgPos;
	
	//OACBPARA_t      oaCbPara;       //网管回调用参数 zhuyn 20160704
	
}VTM_NODE;

typedef struct vtaId_t{
	NODE          		node;     
	struct vtaId_t      *hashNext;

	DWORD                tellerIdLen;
	unsigned char		tellerId[AMS_MAX_TELLER_ID_LEN + 1];
	DWORD                tellerPos;	

} VTA_ID_NODE;

typedef struct vtmId_t{
	NODE          		node;     
	struct vtmId_t      *hashNext;

	DWORD                vtmIdLen;
	unsigned char		vtmId[AMS_MAX_VTM_ID_LEN + 1];	
	DWORD                vtmPos;	

} VTM_ID_NODE;

typedef struct cmsInfo_t
{
	unsigned char    cModuleId;

    
}CMS_INFO;



/* struct of ams data sys cfg */
typedef struct amsDataSysCfg_t
{
	unsigned int vtaStateOperateIndTimeLength;
	unsigned int amsCallEventIndTimeLength;
	unsigned int amsCustomerInQueueTimeLength;
	unsigned int vtmVolumeCtrlTimeLength;
	unsigned int rcasRemoteCoopTimeLength;
	unsigned int vtmRemoteCoopTimeLength;	
	unsigned int vtmSnapTimeLength;
	unsigned int vtaRestTimeLength;
	unsigned int amsVtaRecvMsgTimeLength;
	unsigned int amsVtmRecvMsgTimeLength;	
	unsigned int amsVtaRecvFileTimeLength;
	unsigned int amsVtmRecvFileTimeLength;	
	unsigned int amsMultiSessTimeLength;
	unsigned int amsMonitorTimeLength;
	unsigned int amsVtaParaCfgTimeLength;
	unsigned int amsVtmParaCfgTimeLength;	

	unsigned int       amsParaCfgTimeLength;
	int                paraCfgTimerId;       //默认值 -1
	
	unsigned short     tellerLoginState;   //柜员登录后进入默认状态 zhuyn added  20160701  
	unsigned short     tellerCallRelState; //柜员呼叫释放后进入默认状态 zhuyn added 20160701  
	unsigned short     tellerAnswerType;   
	
	unsigned int       maxVtaNum;
	unsigned int       maxVtmNum;
	unsigned int       maxRcasNum;

	CMS_INFO           cmsInfo;
	//RCAS_INFO          rcasInfo[AMS_MAX_RCAS_NUM];
	//RFB_INIT_PARA      rfbInitParaForVtm;   //remote Cooperative, VTA is Server
	//RFB_INIT_PARA      rfbInitParaForVta;   //remote Cooperative, RCAS is Server, transProto get from vta
	
	//FILE_SERVER_INFO   fileServerInfo;
	//ENCRYPT_INFO       encryptInfo;
	//NET_TRAVERSAL_INFO netTraversalInfo;
	//BRANCH_INFO        branchInfo;
	
	SERVICE_INFO       serviceInfo[AMS_MAX_SERVICE_NUM];

	SERVICE_GROUP_INFO srvGroupInfo[AMS_MAX_SERVICE_GROUP_NUM];

	QUEUE_SYS_INFO     queueSysInfo[AMS_MAX_SERVICE_GROUP_NUM];

	//ORG_INFO           orgInfo[AMS_MAX_ORG_NUM];

	unsigned int		vtacfgnum;
	TELLER_INFO        tellerInfo[AMS_MAX_VTA_NUM];
	TELLER_INFO_NODE	*tellerInfoHashTbl[AMS_VTA_ID_HASH_SIZE];

	unsigned int		vtmcfgnum;
	VTM_INFO           vtmInfo[AMS_MAX_VTM_NUM];
	VTM_INFO_NODE		*vtmInfoHashTbl[AMS_VTM_ID_HASH_SIZE];

	VTA_ID_NODE        *VtaIdHashTbl[AMS_VTA_ID_HASH_SIZE];	

	VTM_ID_NODE        *VtmIdHashTbl[AMS_VTM_ID_HASH_SIZE];

	//ORG_ID_NODE        *OrgIdHashTbl[AMS_ORG_ID_HASH_SIZE];



	//zry added for scc 20
	
}AMS_DATA_SYSCFG;

/* struct of ams data register */
typedef struct amsDataRegister_t
{
	unsigned int 	tellerregnum;
    TELLER_REGISTER_INFO_NODE	*tellerRegisterInfoHashTbl[AMS_VTA_ID_HASH_SIZE];
	TELLER_REGISTER_INFO 		tellerRegisterInfo[AMS_MAX_VTA_NUM];


}AMS_DATA_REGISTER;



/* struct of serviec proc */
typedef struct amsServiecProc_t
{
	int              serviceState;
//	DWORD            preSrvTellerId;
	
	sem_t	         vtaCtrl;	
	LIST 		     vtaList;

	sem_t	         vtmCtrl;
	LIST 		     vtmList;
	
}AMS_SERVICE_MANAGE;



/* struct of teller stat */
typedef struct amsTellerStat_t
{
	unsigned char tellerIdLen;                      //柜员工号
	unsigned char tellerId[AMS_MAX_TELLER_ID_LEN];
	
	unsigned long  loginNum;
	unsigned long  logoutNum;
	unsigned long  handshakeNum;
	
	unsigned long  setIdleNum;
	unsigned long  setBusyNum;	
	unsigned long  setRestNum;	
	unsigned long  setCancelRestlNum;	
	unsigned long  setPreparaNum;	
	unsigned long  setPreparaCompletelyNum;
	
	unsigned long  connectTotalNum;	
	unsigned long  transferCallNum;	
	
	unsigned long  volumeCtrlNum;	
	unsigned long  audioRecordNum;	
	unsigned long  screenRecordNum;	
	unsigned long  remoteCoopNum;	
	unsigned long  snapNum;	

	//the stat below are updated after teller logined
	TELLER_WORK_INFO  vtaWorkInfo;                        //柜员工作信息             
	TELLER_STATE_INFO  vtaStateInfo;                      //柜员状态信息
	
	unsigned long  totalScore;                            //柜员客户评分总得分
	unsigned long  averageScore;                          //柜员客户评分均分
	
	unsigned long  unknownStat;	
	
}AMS_TELLER_STAT;

/* struct of timer stat */
typedef struct amsTimerStat_t
{
	unsigned long  WaitVtaOperateIndRsp[3]; 
	unsigned long  WaitCallEventIndRsp[3];
	unsigned long  WaitCustomerGetTeller[3];
	unsigned long  WaitVolumeCtrlRsp[3];	
	unsigned long  WaitRcasRemoteCoopRsp[3];	
	unsigned long  WaitVtmRemoteCoopRsp[3];		
	unsigned long  WaitSnapRsp[3];
	unsigned long  WaitRestRsp[3];
	unsigned long  WaitVtaRecvMsgRsp[3];
	unsigned long  WaitVtmRecvMsgRsp[3];
	unsigned long  WaitVtaRecvFileRsp[3];
	unsigned long  WaitVtmRecvFileRsp[3];	
	unsigned long  WaitMultiSessRsp[3];
	unsigned long  WaitMonitorRsp[3];	
	unsigned long  WaitVtaParaCfgRsp[3];	
	unsigned long  WaitVtmParaCfgRsp[3];		
	unsigned long  WaitParaCfgRsp[3];	
			
	unsigned long  UnknownTimer[3];

}AMS_TIMER_STAT;


/* struct of stat */
typedef struct amsStat_t
{
	//MsgStat
	//AMS_MSG_STAT msgStat;

	//TimerStat
	AMS_TIMER_STAT timerStat;

	//QueueSystemStat
	//AMS_QUEUE_SYSTEM_STAT queueSystemStat;

	//ServiceStat
	//AMS_SERVICE_STAT serviceStat; 

	//TellerStat
	AMS_TELLER_STAT tellerStat[AMS_MAX_VTA_NUM];

	//VtmStat
	//AMS_VTM_STAT vtmStat[AMS_MAX_VTM_NUM];

	//ResultStat
	//AMS_RESULT_STAT resultStat;

}AMS_STAT;



typedef struct
{
	int i;

	AMS_SERVICE_MANAGE amsServiceManageData[AMS_MAX_SERVICE_GROUP_NUM];

	//AMS_RCAS_DATA amsRcasData;

	//AMS_DATA_LIC amsLicData;
	
	AMS_DATA_SYSCFG amsCfgData;

	AMS_DATA_REGISTER amsRegData;
		
	//AMS_ALARM  amsAlarm;
	
	//AMS_DEBUG  amsDebug;
	
	AMS_STAT   amsStat;
	
    //AMS_DBOPR   amsDbopr;   //zhuyn added 
	
}AMS_PRI_AREA_t;

#define AmsCfgData         (SystemData.AmsPriData.amsCfgData)

#define AmsCfgTellerHashTbl	(SystemData.AmsPriData.amsCfgData.tellerInfoHashTbl)
#define AmsCfgVtmHashTbl	(SystemData.AmsPriData.amsCfgData.vtmInfoHashTbl)
#define AmsCfgTeller(i)		(SystemData.AmsPriData.amsCfgData.tellerInfo[i])
#define AmsCfgVtm(i)		(SystemData.AmsPriData.amsCfgData.vtmInfo[i])
#define AmsCfgService(i)	(SystemData.AmsPriData.amsCfgData.serviceInfo[i])
#define AmsCfgSrvGroup(i)	(SystemData.AmsPriData.amsCfgData.srvGroupInfo[i])
#define AmsCfgQueueSys(i)	(SystemData.AmsPriData.amsCfgData.queueSysInfo[i])

#define	AmsVtaIdHashTbl		(SystemData.AmsPriData.amsCfgData.VtaIdHashTbl)
#define AmsVtmIdHashTbl    	(SystemData.AmsPriData.amsCfgData.VtmIdHashTbl)

#define AmsRegTellerHashTbl	(SystemData.AmsPriData.amsRegData.tellerRegisterInfoHashTbl)
#define AmsRegTeller(i)    (SystemData.AmsPriData.amsRegData.tellerRegisterInfo[i])

#define AmsSrvData(i)			(SystemData.AmsPriData.amsServiceManageData[i])

extern int ConfigAmsSrv(char * cFileName);
extern int SrvDivSen(char * s,WORD_t * word);
extern int AmsSrvServiceSenten(WORD_t * word,int wordcount,unsigned char pCurrId[]);
extern int AmsSrvServiceGroupSenten(WORD_t *word,int wordcount,unsigned char pCurrId[]);
extern int AmsSrvTellerSenten(WORD_t *word,int wordcount,unsigned char pCurrId[]);
extern int AmsSrvVtmSenten(WORD_t *word,int wordcount,unsigned char pCurrId[]);
extern int AmsSrvQueueSenten(WORD_t *word,int wordcount,unsigned char pCurrId[]);
extern int AmsCfgDataInit();


extern int ProcessAmsMessage(int iThreadId, MESSAGE_t *pMsg);
extern void *LicenseProcTask(void *pThreadId);
extern void *ServiceProcTask(void *pThreadId);

#endif





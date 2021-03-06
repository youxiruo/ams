#ifndef AMSPUB_H
#define	AMSPUB_H


#define AMS_TEST_LT

#define AMS_MAX_CRM_NUM				(10000)
#define AMS_MAX_VTM_NUM				(5000)

#define AMS_MAX_CRM_NODES				(10000) //最大坐席节点数
#define AMS_MAX_TERM_NODES				(500) //最大用户节点数
#define AMS_MAX_SERVICE_TYPE_NUM		(128) //AMS业务类型最大配置数

#define AMS_MAX_SERVICE_NUM				(64)
#define AMS_MAX_SERVICE_USED_NUM		(32)

#define AMS_MAX_STRING_WORD_LEN			(256)

#define AMS_MAX_SERVICE_GROUP_NUM		(64)
#define AMS_MAX_SERVICE_GROUP_ID_MIN	(0)
#define AMS_MAX_SERVICE_GROUP_ID_MAX	(AMS_MAX_SERVICE_GROUP_NUM - 1)
#define AMS_MAX_SERVICE_ID_VALUE		(64)
#define AMS_MAX_SERVICE_GROUP_NAME_LEN	(30)



#define AMS_MAX_NAME_LEN				30
#define AMS_MAX_TELLER_ID_LEN			30
#define AMS_MAX_VTM_ID_LEN				30
#define AMS_MAX_TERM_ID_LEN				30

#define AMS_MAX_SERVICE_NAME_LEN		30
#define AMS_MAX_TRANS_IP_LEN			16

#define AMS_MAX_PWD_LEN					32

#define AMS_MAX_CALLID_LEN				30
#define	AMS_MAX_FILEPATH_LEN	                       128
#define	AMS_MAX_FILENAME_LEN	                       128
#define AMS_MAX_CLERA_NUM_IN_QUEUE                     5
#define AMS_MAX_UPDATE_WORK_INFO_NUM                   20

#define AMS_CRM_CONNECT_CLEAR_INTERVAL                 (600)// 10 * 60 s 
#define AMS_TERM_CONNECT_CLEAR_INTERVAL                 (600)// 10 * 60 s 
#define AMS_CMS_CALL_CLEAR_INTERVAL                    (300)// 5 * 60 s 
#define AMS_CRM_WORK_INFO_UPDATE_INTERVAL              (300) // 5 * 60 s




#define AMS_MAX_AVG_SERVICE_TIME                       (3600)
#define AMS_AVG_SERVICE_TIME                           (100)


#define T_AMS_REST_TIMER_LENGTH_MAX                    (10000)       //AMS休息最大定时10000s
#define T_CRM_OPERATE_IND_TIMER_LENGTH_MAX             (1000)        //柜员操作指示最大定时1000s
#define T_AMS_CUSTOMER_IN_QUEUE_TIMER_LENGTH_MAX       (30000)       //AMS客户排队最大定时30000s


#define AMS_CRM_ID_HASH_SIZE                           (AMS_MAX_CRM_NUM)
#define AMS_VTM_ID_HASH_SIZE                           (AMS_MAX_VTM_NUM)

#define	AMS_MAX_TIMER_PARA_LEN                         (PARA_LEN)
#define AMS_MAX_PROC_NUM_IN_QUEUE                      20


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


typedef struct tellerPersonalInfo_t
{
	unsigned char tellerUserNameLen;
	unsigned char tellerUserName[AMS_MAX_NAME_LEN + 1];
	unsigned char tellerNickNameLen;
	unsigned char tellerNickName[AMS_MAX_NAME_LEN + 1];
	unsigned char tellertype;
	
}TELLER_PERSONAL_INFO;


typedef struct tellerRegisterInfo_t
{
	unsigned char   flag;	                       //是否注册	
	
	DWORD           tellerIdLen;                      //柜员标识
	unsigned char	tellerId[AMS_MAX_TELLER_ID_LEN + 1];
	
	DWORD           crmIp;
	WORD            crmPort;
	
	PID_t	        myPid;
	PID_t	        cmsPid;

	TELLER_PERSONAL_INFO tellerpersionalinfo;
	
}TELLER_REGISTER_INFO;


typedef struct tellerRegisterInfoNode_t
{
	NODE	node;
	struct tellerRegisterInfoNode_t *hashNext;

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


typedef struct amsQueueInfo_t
{
	DWORD           srvGrpId;
	
	WORD            queuingLen;
	WORD            queuingTime;

}QUEUE_INFO;


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

	DWORD           tellerType;                    	//坐席类型

	DWORD           srvGrpIdLen;                      //业务组编号
	unsigned char   srvGrpId[AMS_MAX_SERVICE_GROUP_NAME_LEN +1];
	DWORD			srvGrpIdPos;

//	DWORD           tellerSrvAuthRsvd;             //Service Authority 业务处理权限预留
//	DWORD           tellerSrvAuth;                 //Service Authority 业务处理权限

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
	DWORD           crmIp;                         //in_addr_t
	WORD            crmPort;                       //Sip
	WORD            crmScreenRecPort; 	
	WORD            crmRemoteCoopPort;             //crm pack          
	WORD            crmRemoteCoopType;             //crm pack  

	TERM_NET_INFO		termInfo;                     //终端网络信息
	TELLER_PERSONAL_INFO tellerPersionalInfo;     //坐席个人信息
	
}TELLER_INFO;

typedef struct tellerInfoNode_t
{
	NODE	node;
	struct tellerInfoNode_t *hashNext;

	unsigned int	tellerInfopos; //amsCfgData里tellerinfo数组对应的下标
	
	unsigned char	tellerIdLen;
	unsigned char	tellerId[AMS_MAX_TELLER_ID_LEN + 1];
	
}TELLER_INFO_NODE;

typedef struct termInfo_t
{
	unsigned char   flag;	                       //是否配置

	DWORD           termIdLen;                         
	unsigned char   termId[AMS_MAX_VTM_ID_LEN + 1];  //终端标识
	

	DWORD           terminalType;                  //终端类型

	DWORD           srvGrpIdLen;                      //业务组编号
	unsigned char	srvGrpId[AMS_MAX_SERVICE_GROUP_NAME_LEN + 1];

	unsigned int    srvGrpIdpos;

}TERM_INFO;


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
	unsigned char   srvGrpIdPos;
	
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
typedef struct crmNode_t
{
	NODE            node; 

	DWORD           amsPid;
	DWORD           state;  
	DWORD			setstate;
	DWORD           callState;    	
	TIME_INFO       stateStartLocalTime; 
	TIME_INFO       callStateStartLocalTime; 	
	time_t          startTime;		
	time_t          stateStartTime;	
	time_t          callStateStartTime;		
	time_t          handshakeTime;
	time_t          workInfoUpdateTime;	
	time_t          callTransferFreezeTime;	
	
	TELLER_INFO     crmInfo;

	TELLER_WORK_INFO crmWorkInfo;

	DWORD           callTransferNum;
		
	DWORD           dailyAvgScore;           //enlarge 1000
	DWORD           dailyAvgScoreExpect;     //enlarge 1000   

	DWORD           tellerCfgPos;
	DWORD           tellerRegPos;
	DWORD           orgCfgPos;
	
	//OACBPARA_t      oaCbPara;       //网管回调用参数 zhuyn 20160704
	
}CRM_NODE;


/* 终端结点结构体定义 */
typedef struct termNode_t
{
	NODE            node;  

	//Customer In Queue Pid
	DWORD           customerPid;

	DWORD           amsPid;
	DWORD           state;  
	DWORD           callState;  
	DWORD           serviceState; 	

	TIME_INFO       stateStartLocalTime; 	
	TIME_INFO       callStateStartLocalTime; 
	TIME_INFO       serviceStateStartLocalTime;	
	time_t          stateStartTime;	
	time_t          callStateStartTime;	
	time_t          serviceStateStartTime;			
	time_t          handshakeTime;
	time_t          enterQueueTime;

	unsigned char   serviceTypeLen;
	unsigned char   serviceType[AMS_MAX_SERVICE_NAME_LEN+1];

	//timer
	int             iTimerId; //Not Use Yet
//	int		        iTimerType;

	//communication pid
	PID_t	        myPid;
	PID_t	        rPid;     //vtm
	PID_t	        cmsPid;   //cms
	PID_t	        crmPid;   //CRM	

	TERM_INFO        termInfo;
	
}TERM_NODE;


typedef struct crmId_t{
	NODE          		node;     
	struct crmId_t      *hashNext;

	DWORD                tellerIdLen;
	unsigned char		tellerId[AMS_MAX_TELLER_ID_LEN + 1];
	DWORD                tellerPos;	

} CRM_ID_NODE;

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
	unsigned int crmStateOperateIndTimeLength;
	unsigned int amsCallEventIndTimeLength;
	unsigned int amsCustomerInQueueTimeLength;
	unsigned int vtmVolumeCtrlTimeLength;
	unsigned int rcasRemoteCoopTimeLength;
	unsigned int vtmRemoteCoopTimeLength;	
	unsigned int vtmSnapTimeLength;
	unsigned int crmRestTimeLength;
	unsigned int amsCrmRecvMsgTimeLength;
	unsigned int amsVtmRecvMsgTimeLength;	
	unsigned int amsCrmRecvFileTimeLength;
	unsigned int amsVtmRecvFileTimeLength;	
	unsigned int amsMultiSessTimeLength;
	unsigned int amsMonitorTimeLength;
	unsigned int amsCrmParaCfgTimeLength;
	unsigned int amsVtmParaCfgTimeLength;	

	unsigned int       amsParaCfgTimeLength;
	int                paraCfgTimerId;       //默认值 -1
	
	unsigned short     tellerLoginState;   //柜员登录后进入默认状态 zhuyn added  20160701  
	unsigned short     tellerCallRelState; //柜员呼叫释放后进入默认状态 zhuyn added 20160701  
	unsigned short     tellerAnswerType;   
	
	unsigned int       maxCrmNum;
	unsigned int       maxVtmNum;
	unsigned int       maxRcasNum;

	CMS_INFO           cmsInfo;
	//RCAS_INFO          rcasInfo[AMS_MAX_RCAS_NUM];
	//RFB_INIT_PARA      rfbInitParaForVtm;   //remote Cooperative, CRM is Server
	//RFB_INIT_PARA      rfbInitParaForCrm;   //remote Cooperative, RCAS is Server, transProto get from crm
	
	//FILE_SERVER_INFO   fileServerInfo;
	//ENCRYPT_INFO       encryptInfo;
	//NET_TRAVERSAL_INFO netTraversalInfo;
	//BRANCH_INFO        branchInfo;
	
	SERVICE_INFO       serviceInfo[AMS_MAX_SERVICE_NUM];

	SERVICE_GROUP_INFO srvGroupInfo[AMS_MAX_SERVICE_GROUP_NUM];

	QUEUE_SYS_INFO     queueSysInfo[AMS_MAX_SERVICE_GROUP_NUM];

	//ORG_INFO           orgInfo[AMS_MAX_ORG_NUM];

	unsigned int		crmcfgnum;
	TELLER_INFO        tellerInfo[AMS_MAX_CRM_NUM];
	TELLER_INFO_NODE	*tellerInfoHashTbl[AMS_CRM_ID_HASH_SIZE];

	unsigned int		vtmcfgnum;
	//VTM_INFO           vtmInfo[AMS_MAX_VTM_NUM];
	VTM_INFO_NODE		*vtmInfoHashTbl[AMS_VTM_ID_HASH_SIZE];

	CRM_ID_NODE        *CrmIdHashTbl[AMS_CRM_ID_HASH_SIZE];	

	VTM_ID_NODE        *VtmIdHashTbl[AMS_VTM_ID_HASH_SIZE];

	//ORG_ID_NODE        *OrgIdHashTbl[AMS_ORG_ID_HASH_SIZE];



	//zry added for scc 20
	
}AMS_DATA_SYSCFG;

/* struct of ams data register */
typedef struct amsDataRegister_t
{
	unsigned int 	tellerregnum;
    TELLER_REGISTER_INFO_NODE	*tellerRegisterInfoHashTbl[AMS_CRM_ID_HASH_SIZE];
	TELLER_REGISTER_INFO 		tellerRegisterInfo[AMS_MAX_CRM_NUM];


}AMS_DATA_REGISTER;



/* struct of serviec proc */
typedef struct amsServiecProc_t
{
	int              serviceState;
//	DWORD            preSrvTellerId;
	
	sem_t	         crmCtrl;	
	LIST 		     crmList; //坐席队列

	sem_t	         termCtrl;
	LIST 		     termList; //客户信息

	sem_t            freecrmCtrl;
	LIST			 freecrmList; //空闲坐席队列
	
}AMS_SERVICE_MANAGE;



/* struct of teller stat */
typedef struct amsTellerStat_t
{
	unsigned char tellerIdLen;                      //柜员工号
	unsigned char tellerId[AMS_MAX_TELLER_ID_LEN];
	
	unsigned long  loginNum;
	unsigned long  logoutNum;
	
	unsigned long  setIdleNum;
	unsigned long  setBusyNum;	
	unsigned long  setRestNum;	
	unsigned long  setPreparaNum;

	//the stat below are updated after teller logined
	TELLER_WORK_INFO  crmWorkInfo;                        //柜员工作信息             
	TELLER_STATE_INFO  crmStateInfo;                      //柜员状态信息
	
	unsigned long  unknownStat;	
	
}AMS_TELLER_STAT;

/* struct of timer stat */
typedef struct amsTimerStat_t
{
	unsigned long  WaitCustomerGetTeller[3];
	unsigned long  WaitCrmOperateIndRsp[3]; 
	unsigned long  WaitRestRsp[3];
			
	unsigned long  UnknownTimer[3];

}AMS_TIMER_STAT;

/* struct of msg stat */
typedef struct amsMsgStat_t
{
	// ams <-> crm msg
	unsigned long  recvCrmLoginReq;
	unsigned long  sendCrmLoginRsp;
	unsigned long  recvCrmLogoutReq;
	unsigned long  sendCrmLogoutRsp;

	unsigned long  recvCrmStateOperateReq;
	unsigned long  sendCrmStateOperateRsp; 

	unsigned long  sendCrmStateOperateInd; 
	unsigned long  recvCrmStateOperateCnf;

	unsigned long  recvCrmModifyPasswordReq;
	unsigned long  sendCrmModifyPasswordRsp;

	unsigned long  recvCrmForceLoginReq;
	unsigned long  sendCrmForceLoginRsp;

	unsigned long  recvCrmQueryInfoReq;
	unsigned long  sendCrmQueryInfoRsp;

	unsigned long  recvCrmEventNotice;
	unsigned long  sendCrmEventInd;

	// ams <-> cms msg
	unsigned long  recvCrmRegReq;
	unsigned long  sendCrmRegRsp;
	unsigned long  recvCrmGetReq;
	unsigned long  sendCrmGetRsp;

	unsigned long  recvCrmCalloutReq;
	unsigned long  sendCrmCalloutRsp;

	unsigned long  recvCrmAuthinfoReq;
	unsigned long  sendCrmAuthinfoRsp;

	
	unsigned long  recvAmsCallEventNoticeReq;
	unsigned long  sendAmsCallEventInd;

	// ams B msg
	unsigned long  customerInQueueTimeout;
	unsigned long  crmStateOperateIndTimeout;
	unsigned long  restTimeout;
	unsigned long  amsUnknownBMsg;

	
	//ams unknow msg
	unsigned long  amsErrMsg;
	unsigned long  amsUnknownMsgType;
	unsigned long  amsUnknownMsg;

	unsigned long  amsErrBMsg;
	
}AMS_MSG_STAT;


/* struct of msg stat */
typedef struct amsResultStat_t
{
	//Common Result	
	unsigned long  amsSuccess;

	unsigned long  amsParaErr;
	unsigned long  amsStateErr;
	unsigned long  amsAllocMemFailed;
	unsigned long  amsAllocLpFailed;
	unsigned long  amsAllocLtFailed;

	//Crm Login Result
	unsigned long  crmLoginSuccess;
	unsigned long  crmLoginParaErr;
	unsigned long  crmLoginStateErr;
	unsigned long  crmLoginLenErr;
	unsigned long  crmLoginLpResourceLimited;
	unsigned long  crmLoginNodeResourceLimited;
	unsigned long  crmLoginTellerLenErr;
	unsigned long  crmLoginTellerNoErr;
	unsigned long  crmLoginTellerPwdErr;
	unsigned long  crmLoginTellerVncAuthPwdErr;
	unsigned long  crmLoginTellerLoginRepeatedly;
	unsigned long  crmLoginTellerTypeErr;
	unsigned long  crmLoginTellerNumErr;
	unsigned long  crmLoginFileServerUsrNameErr;
	unsigned long  crmLoginFileServerUsrPwdErr;
	unsigned long  crmLoginOrgIdErr;
	unsigned long  crmLoginOrgStateErr;	
	unsigned long  crmLoginLicenseTimeout;
	unsigned long  crmLoginTellerNumBeyondLic;
	unsigned long crmlogintellerloginnotresiter;

	//Crm Logout Result
	unsigned long  crmLogoutSuccess;
	unsigned long  crmLogoutParaErr;          
	unsigned long  crmLogoutStateErr;         
	unsigned long  crmLogoutLenErr;           
	unsigned long  crmLogoutAmsPidErr;        
	unsigned long  crmLogoutTellerIdErr;      
	unsigned long  crmLogoutSrvGrpIdErr;  
	unsigned long  crmLogoutServiceStateErr;   
	
	//Crm State Operate Result
	unsigned long  crmStateOperateSuccess;
	unsigned long  crmStateOperateParaErr;
	unsigned long  crmStateOperateStateErr;
	unsigned long  crmStateOperateLenErr;
	unsigned long  crmStateOperateAmsPidErr;
	unsigned long  crmStateOperateTellerIdErr;
	unsigned long  crmStateOperateSrvGrpIdErr;
	unsigned long  crmStateOperateServiceStateErr;
	unsigned long  crmStateOperateCodeErr;
	unsigned long  crmStateOperateOpReasonErr;
	unsigned long  crmStateOperateOpTimeLenErr;
	unsigned long  crmStateOperateUpdateStateErr;
	unsigned long  crmStateOperateStartTimerErr;
	unsigned long  crmStateOperateRestTimeout;	
	
	unsigned long  crmStateOpRestTimeoutParaErr;
	unsigned long  crmStateOpRestTimeoutStateErr;
	unsigned long  crmStateOpRestTimeoutLenErr;
	unsigned long  crmStateOpRestTimeoutAmsPidErr;
	unsigned long  crmStateOpRestTimeoutTellerIdErr;
	unsigned long  crmStateOpRestTimeoutSrvGrpIdErr;
	unsigned long  crmStateOpRestTimeoutSrvStErr;

	//Crm Modify Password Result
	unsigned long  crmModifyPasswordSuccess;
	unsigned long  crmModifyPasswordParaErr;
	unsigned long  crmModifyPasswordStateErr;
	unsigned long  crmModifyPasswordLenErr;
	unsigned long  crmModifyPasswordAmsPidErr;
	unsigned long  crmModifyPasswordTellerIdErr;
	unsigned long  crmModifyPasswordOldPasswordErr;
	unsigned long  crmModifyPasswordNewPasswordErr;
	unsigned long  crmModifyPasswordSrvGrpIdErr;
	unsigned long  crmModifyPasswordServiceStateErr;
	unsigned long  crmModifyPasswordTellerPosErr; 

	//Crm Force Login Result
	unsigned long  crmForceLoginSuccess;
	unsigned long  crmForceLoginParaErr;
	unsigned long  crmForceLoginStateErr;
	unsigned long  crmForceLoginLenErr;
	unsigned long  crmForceLoginLpResourceLimited;
	unsigned long  crmForceLoginNodeResourceLimited;
	unsigned long  crmForceLoginTellerLenErr;
	unsigned long  crmForceLoginTellerNoErr;
	unsigned long  crmForceLoginTellerPwdErr;
	unsigned long  crmForceLoginTermNetInfoErr;
	unsigned long  crmForceLoginTellerVncAuthPwdErr;
	unsigned long  crmForceLoginTellerLoginRepeatedly;
	unsigned long  crmForceLoginTellerTypeErr;
	unsigned long  crmForceLoginTellerNumErr;
	unsigned long  crmForceLoginFileServerUsrNameErr;
	unsigned long  crmForceLoginFileServerUsrPwdErr;
	unsigned long  crmForceLoginLicenseTimeout;
	unsigned long  crmForceLoginTellerNumBeyondLic;

	//Crm Query Info Result	
	unsigned long  crmQueryInfoSuccess;
	unsigned long  crmQueryInfoParaErr;
	unsigned long  crmQueryInfoStateErr;
	unsigned long  crmQueryInfoLenErr;
	unsigned long  crmQueryInfoAmsPidErr;
	unsigned long  crmQueryInfoTellerIdErr;
	unsigned long  crmQueryInfoSrvGrpIdErr;
	unsigned long  crmQueryInfoServiceStateErr;
	unsigned long  crmQueryInfoCodeErr;
	unsigned long  crmQueryInfoTargetSrvIdErr;
	unsigned long  crmQueryInfoTargetSrvNameLenErr;
	unsigned long  crmQueryInfoTargetSrvGrpIdErr;
	unsigned long  crmQueryInfoTargetTellerNoLenErr;
	unsigned long  crmQueryInfoTargetTellerNoErr;
	unsigned long  crmQueryInfoTellerNoKeyLenErr;
	unsigned long  crmQueryInfoTellerNameKeyLenErr; 
 
	//--Crm State Op Ind Cnf Result--
	unsigned long  crmStateOperateIndCnfParaErr;
	unsigned long  crmStateOperateIndCnfStateErr;
	unsigned long  crmStateOperateIndCnfLenErr;
	unsigned long  crmStateOperateIndCnfAmsPidErr;
	unsigned long  crmStateOperateIndCnfTellerIdErr;
	unsigned long  crmStateOperateIndCnfSrvGrpIdErr; 
	unsigned long  crmStateOperateIndCnfServiceStateErr;
	unsigned long  crmStateOperateIndCnfManagerAmsPidErr;
	unsigned long  crmStateOperateIndCnfManagerTellerIdErr;
	unsigned long  crmStateOperateIndCnfManagerSrvGrpIdErr;
	unsigned long  crmStateOperateIndCnfManagerSrvStErr;	
	unsigned long  crmStateOperateIndCnfCodeErr;
	
	unsigned long  managerSetCrmStateTimeoutParaErr;
	unsigned long  managerSetCrmStateTimeoutStateErr;
	unsigned long  managerSetCrmStateTimeoutLenErr;
	unsigned long  managerSetCrmStateTimeoutAmsPidErr;	
	unsigned long  managerSetCrmStateTimeoutTellerIdErr;
	unsigned long  managerSetCrmStateTimeoutSrvGrpIdErr;
	unsigned long  managerSetCrmStateTimeoutServiceStateErr;
	unsigned long  managerSetCrmStateTimeoutStSetStateErr;
 
	//Crm Event Notice Result
	unsigned long  crmEventNoticeSuccess;
	unsigned long  crmEventNoticeParaErr;
	unsigned long  crmEventNoticeStateErr;
	unsigned long  crmEventNoticeLenErr;
	unsigned long  crmEventNoticeSrvGrpIdErr;
	unsigned long  crmEventNoticeServiceStateErr;
	unsigned long  crmEventNoticeCallIdErr;
	unsigned long  crmEventNoticeCallStateErr;
	unsigned long  crmEventNoticeAmsPidErr;
	unsigned long  crmEventNoticeTellerIdErr;
	unsigned long  crmEventNoticeVtmIdErr;
	unsigned long  crmEventNoticeTypeErr;
	unsigned long  crmEventNoticeLackFileInfo;
	unsigned long  crmEventNoticeFilePathLenErr;
	unsigned long  crmEventNoticeFileNameLenErr;

	unsigned long  crmEvntNtceSendFileTrgtTypeErr;
	unsigned long  crmEvntNtceSendFileTrgtTlrIdErr;
	unsigned long  crmEvntNtceSendFileTrgtTlrStErr;
	unsigned long  crmEvntNtceSendFileTrgtTlrAmsPidErr;
	unsigned long  crmEvntNtceSendFileTrgtVtmIdErr;
	unsigned long  crmEvntNtceSendFileTrgtVtmStErr;
	unsigned long  crmEvntNtceSendFileSendStErr;
	unsigned long  crmEvntNtceSendFileStartTimerErr;
	
	unsigned long  crmEvntNtceRecvFileParaErr;
	unsigned long  crmEvntNtceRecvFileOrgnTypeErr;
	unsigned long  crmEvntNtceRecvFileOrgnTlrIdErr;
	unsigned long  crmEvntNtceRecvFileOrgnTlrAmsPidErr;
	unsigned long  crmEvntNtceRecvFileOrgnTlrSrvGrpIdErr;
	unsigned long  crmEvntNtceRecvFileOrgnTlrSrvStErr;
	unsigned long  crmEvntNtceRecvFileOrgnTlrStErr;
	unsigned long  crmEvntNtceRecvFileOrgnVtmIdErr; 
	unsigned long  crmEvntNtceRecvFileOrgnVtmStErr;
	unsigned long  crmEvntNtceRecvFileRecvStErr;

	unsigned long  crmEvntCancRecvFileParaErr;
	unsigned long  crmEvntCancRecvFileOrgnTypeErr;
	unsigned long  crmEvntCancRecvFileOrgnTlrIdErr;
	unsigned long  crmEvntCancRecvFileOrgnTlrAmsPidErr;
	unsigned long  crmEvntCancRecvFileOrgnTlrSrvGrpIdErr;
	unsigned long  crmEvntCancRecvFileOrgnTlrSrvStErr;
	unsigned long  crmEvntCancRecvFileOrgnTlrStErr;
	unsigned long  crmEvntCancRecvFileOrgnVtmIdErr; 
	unsigned long  crmEvntCancRecvFileOrgnVtmStErr;
	unsigned long  crmEvntCancRecvFileRecvStErr;
	
	unsigned long  crmEvntNtceAudioRecCrmRepFail;
	unsigned long  crmEvntNtceUpldAudioFileCrmRepFail;
	unsigned long  crmEvntNtceScrRecCrmRepFail;
	unsigned long  crmEvntNtceUpldScrRecFileCrmRepFail;
	unsigned long  crmEvntNtceSnapCrmRepFail;
	unsigned long  crmEvntNtceDownldSnapFileCrmRepFail;
	unsigned long  crmEvntNtceUpldFileCrmRepFail;
	unsigned long  crmEvntNtceDownldFileCrmRepFail;
	unsigned long  crmEvntNtceCrmCancRecvFileRepFail;
	unsigned long  crmEvntNtceRemCoopCrmRepFail;  
	
	//Cms Crm Reg Result
	unsigned long  cmsCrmRegSuccess;
	unsigned long  cmsCrmRegParaErr;
	unsigned long  cmsCrmRegStateErr;
	unsigned long  cmsCrmRegLenErr;
	unsigned long  cmsCrmRegTellerIdErr;
	unsigned long  cmsCrmRegResourceLimited;
 
	//Cms Get Crm Result
	unsigned long  cmsGetCrmSuccess;
	unsigned long  cmsGetCrmParaErr;
	unsigned long  cmsGetCrmLenErr;
	unsigned long  cmsGetCrmAmsPidErr;
	unsigned long  cmsGetCrmCallIdErr;
	unsigned long  cmsGetCrmTerminalTypeErr;
	unsigned long  cmsGetCrmSrvGrpIdErr;
	unsigned long  cmsGetCrmServiceTypeErr;
	unsigned long  cmsGetCrmNoValidSrvGrpOrType;
	unsigned long  cmsGetCrmCallTypeErr;
	unsigned long  cmsGetCrmOriginTellerIdErr;
	unsigned long  cmsGetCrmOriginTellerStateErr;
	unsigned long  cmsGetCrmOriginTellerAmsPidErr;
	unsigned long  cmsGetCrmOriginTellerSrvGrpIdErr;
	unsigned long  cmsGetCrmOriginTellerSrvStateErr;	
	unsigned long  cmsGetCrmTargetTypeErr;
	unsigned long  cmsGetCrmTargetGroupErr; 
	unsigned long  cmsGetCrmTargetTellerIdErr;
	unsigned long  cmsGetCrmTargetTellerStateErr;	
	unsigned long  cmsGetCrmVtmIdErr;
	unsigned long  cmsGetCrmAssocCallIdErr;
	unsigned long  cmsGetCrmLicenseTimeout;
	unsigned long  cmsGetCrmTpsBeyondLic;

	unsigned long  cmsGetCrmServiceInQueue;
	unsigned long  cmsGetCrmSIRErr;
	unsigned long  cmsGetCrmSIRVtmNoErr;
	unsigned long  cmsGetCrmSIRSrvGrpIdErr;
	unsigned long  cmsGetCrmSIRCallTypeErr;
	unsigned long  cmsGetCrmSIRCrmEmpty;
	unsigned long  cmsGetCrmSIRCalcQueueInfoErr;
	unsigned long  cmsGetCrmSIRTooManyCustInQueue;
	unsigned long  cmsGetCrmSIRTargetTellerNotLogin;
	unsigned long  cmsGetCrmSIRTargetTellerNotReg;
	unsigned long  cmsGetCrmSIRTargetTellerStateErr;
	unsigned long  cmsGetCrmSIRCallTransTooManyTimes;

	unsigned long  cmsGetCrmOrgIdErr;
	unsigned long  cmsGetCrmOrgPosErr;
	unsigned long  cmsGetCrmParentOrgIdErr;
	unsigned long  cmsGetCrmSelectSrvGrpIdErr;
	unsigned long  cmsGetCrmLpResourceLimited;
	unsigned long  cmsGetCrmCreateTimerErr;
	
	unsigned long  cmsGetCrmTimeout;
	unsigned long  cmsGetCrmTimeoutParaErr;
	unsigned long  cmsGetCrmTimeoutStateErr;
	unsigned long  cmsGetCrmTimeoutLenErr;
	unsigned long  cmsGetCrmTimeoutVtmIdErr;
	unsigned long  cmsGetCrmTimeoutServiceStateErr;	 

	//Cms Callout Result
	unsigned long cmscrmCalloutSuccess;
	unsigned long cmsCalloutAmspidErr;
	unsigned long cmsCalloutParaErr;
	unsigned long cmsCalloutStateErr;
	unsigned long cmsCalloutcrmCallidErr;
	unsigned long cmsCalloutcrmTelleridErr;
	unsigned long cmsCalloutcrmSrvgrpidErr;
	unsigned long cmsCalloutcrmSrvtypeErr;
	unsigned long cmsCalloutcrmTelleridNotcfgErr;
	unsigned long cmsCalloutcrmTelleridNotregErr;
	unsigned long cmsCalloutcrmTelleridNotloginErr;
	unsigned long cmsCalloutcrmRepeatErr;

	//Cms Event Notice Result
	unsigned long  cmsEventNoticeSuccess;
	unsigned long  cmsEventNoticeParaErr;
	unsigned long  cmsEventNoticeStateErr;
	unsigned long  cmsEventNoticeSrvGrpIdErr;
	unsigned long  cmsEventNoticeServiceStateErr;
	unsigned long  cmsEventNoticeLenErr;
	unsigned long  cmsEventNoticeAmsPidErr;
	unsigned long  cmsEventNoticeCallIdErr;
	unsigned long  cmsEventNoticeTellerIdErr;
	unsigned long  cmsEventNoticeVtmIdErr;
	unsigned long  cmsEventNoticeCodeErr;
	unsigned long  cmsEventNoticeCrmStateErr;
	unsigned long  cmsEventNoticeVtmStateErr;
	unsigned long  cmsEventNoticeOrgnTellerIdErr;
	unsigned long  cmsEventNoticeOrgnTellerStateErr;
	unsigned long  cmsEventNoticeOrgnTellerAmsPidErr;
	unsigned long  cmsEventNoticeOrgnTellerSrvGrpIdErr;
	unsigned long  cmsEventNoticeOrgnTellerSrvStateErr;
	
	unsigned long  cmsHandshakeClearInactiveCall;
	//ams unknown msg
	unsigned long  amsUnknownResultType;
	unsigned long  amsUnknownResult;
	
}AMS_RESULT_STAT;



/* struct of stat */
typedef struct amsStat_t
{
	//MsgStat
	AMS_MSG_STAT msgStat;

	//TimerStat
	AMS_TIMER_STAT timerStat;

	//QueueSystemStat
	//AMS_QUEUE_SYSTEM_STAT queueSystemStat;

	//ServiceStat
	//AMS_SERVICE_STAT serviceStat; 

	//TellerStat
	AMS_TELLER_STAT tellerStat[AMS_MAX_CRM_NUM];

	//VtmStat
	//AMS_VTM_STAT vtmStat[AMS_MAX_VTM_NUM];

	//ResultStat
	AMS_RESULT_STAT resultStat;

}AMS_STAT;


/* struct of ams license data */
typedef struct amsDataLic_t
{
	pthread_mutex_t    amsLicProcMtx;
	unsigned char      licState;
	
	unsigned int       crmNum;
	unsigned int       vtmNum;
	unsigned int       tps;

	char               licRunTimeCheckStart;
	char               dayPerm;
	char               validTimeLenFlag;
	unsigned int	   validTimeLen;
	
	unsigned char      firstRunTimeRec;
	TIME_INFO          firstRunTime;
	time_t             lastRunTime;            //AmsReserveData1
	unsigned int       usedTime;               //AmsReserveData2
	
}AMS_DATA_LIC;

/* struct of debug */
typedef struct amsDebug_t	     //调试开关
{
	unsigned char debug;         //调试信息
	unsigned char common;        //一般信息
	unsigned char msg;			 //消息
	unsigned char state;         //状态信息
	unsigned char timer;         //定时器信息
	unsigned char error;         //错误信息
	unsigned char alarm;         //告警信息
	unsigned char cmsg;          //c区消息
	
}AMS_DEBUG;


typedef struct
{
	int i;

	AMS_SERVICE_MANAGE amsServiceManageData[AMS_MAX_SERVICE_GROUP_NUM];

	//AMS_RCAS_DATA amsRcasData;

	AMS_DATA_LIC amsLicData;
	
	AMS_DATA_SYSCFG amsCfgData;

	AMS_DATA_REGISTER amsRegData;
		
	//AMS_ALARM  amsAlarm;
	
	AMS_DEBUG  amsDebug;
	
	AMS_STAT   amsStat;
	
    //AMS_DBOPR   amsDbopr;   //zhuyn added 
	
}AMS_PRI_AREA_t;

#define AmsCfgData         (SystemData.AmsPriData.amsCfgData)
#define AmsLicData         (SystemData.AmsPriData.amsLicData)


#define AmsCfgTellerHashTbl	(SystemData.AmsPriData.amsCfgData.tellerInfoHashTbl)
#define AmsCfgVtmHashTbl	(SystemData.AmsPriData.amsCfgData.vtmInfoHashTbl)
#define AmsCfgTeller(i)		(SystemData.AmsPriData.amsCfgData.tellerInfo[i])
#define AmsCfgVtm(i)		(SystemData.AmsPriData.amsCfgData.vtmInfo[i])
#define AmsCfgService(i)	(SystemData.AmsPriData.amsCfgData.serviceInfo[i])
#define AmsCfgSrvGroup(i)	(SystemData.AmsPriData.amsCfgData.srvGroupInfo[i])
#define AmsCfgQueueSys(i)	(SystemData.AmsPriData.amsCfgData.queueSysInfo[i])

#define	AmsCrmIdHashTbl		(SystemData.AmsPriData.amsCfgData.CrmIdHashTbl)
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





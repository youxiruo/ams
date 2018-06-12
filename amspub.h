/******************************************************************
*
* FILE NAME						                           VERSION
*	
*	amspub.h	                                            v1.0.0
*
* DESCRIPTION
*
*	AMS应用管理服务器的公共定义文件
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
#ifndef AMSPUB_H
#define AMSPUB_H

//#define AMS_TEST_LT
#define AMS_TEST_NEED_DB
#define AMS_TEST_NEED_LIC

//zry added for scc 2018
#define AMS_MAX_SEAT_NUM								(10000)
#define AMS_MAX_SEAT_NODES								(1000)
#define AMS_MAX_TELLER_ID_LEN							30
#define AMS_MAX_GRPID_LEN								30
#define AMS_MAX_USERID_LEN								30
#define AMS_MAX_SERVICETYPE_LEN							30
#define IPV6_LEN										16
#define AMS_SEAT_ID_HASH_SIZE							(AMS_MAX_SEAT_NUM)
#define TERMINFO_ID                                     0x03
//added end

#define AMS_MAX_VTA_NUM		                           (2000) // 2000
#define AMS_MAX_VTM_NUM                                (1000) // 1000
#define AMS_MAX_RCAS_NUM                               (64)   // 64

#define AMS_MAX_VTA_NODES	                           (200)  //vta最大坐席节点数 200
#define AMS_MAX_VTM_NODES                              (600)  //vtm最大节点数 600
#define AMS_MAX_SERVICE_TYPE_NUM                       (128)  //AMS业务类型最大配置数
#define AMS_MAX_RCAS_CNCR_NUM                          (200)  //单个RCAS最大并发处理路数

#define AMS_MAX_SERVICE_NUM                            (64)
#define AMS_MAX_SERVICE_USED_NUM                       (32)

#define AMS_MAX_STRING_WORD_LEN                        (256)

#define AMS_MAX_SERVICE_GROUP_NUM                      (64)
#define AMS_SERVICE_GROUP_ID_MIN                       (0)
#define AMS_SERVICE_GROUP_ID_MAX                       (AMS_MAX_SERVICE_GROUP_NUM - 1)
#define AMS_MAX_SERVICE_ID_VALUE                       (64)
#define AMS_MAX_SERVICE_GROUP_NAME_LEN		           (80)

#define AMS_MAX_ORG_NUM                                (256)
#define AMS_MAX_ORG_LEVEL                              (8)
#define AMS_MAX_ORG_CODE_LEN                           (32)
#define AMS_MAX_ORG_NAME_LEN		                   (128)

#define AMS_MAX_NAME_LEN		                       30
#define AMS_MAX_TELLER_NO_LEN                          30
#define AMS_MAX_VTM_NO_LEN                             30
#define AMS_MAX_DEPARTMENT_NAME_LEN		               60
#define AMS_MAX_SERVICE_NAME_LEN		               80
#define AMS_MAX_AREA_CODE_NUM   		               20
#define AMS_MAX_PHONE_NUM   		                   40
#define AMS_MAX_TRANS_IP_LEN                           16 //39

#define	AMS_MAX_VNC_AUTH_PWD_LEN	                   32
#define	AMS_MAX_CALLID_LEN		                       30
#define	AMS_MAX_ENCODING_NUM                           32
#define	AMS_MAX_SECURITIES_NUM                         16
#define	AMS_MAX_FILEPATH_LEN	                       128
#define	AMS_MAX_FILENAME_LEN	                       128
#define	AMS_MAX_FILE_SIZE    	                       104857600 //100*1024*1024,即100M
#define	AMS_MAX_FILE_TIME_LEN    	                   60
#define	AMS_MAX_SCREEN_REC_NUM                         9999
#define	AMS_MAX_TIMER_PARA_LEN                         (PARA_LEN)
#define	AMS_MAX_SERVICE_SCORE                          10
#define	AMS_MAX_TELLER_TYPE_LEN	                       30
#define	AMS_MAX_TERMINAL_TYPE_LEN	                   30
#define	AMS_MAX_SERVICE_GROUP_NUM_LEN	               80
#define	AMS_MAX_PWD_LEN			                       32
#define	AMS_MAX_SEND_MSG_LEN			               3840
#define	AMS_FILE_SEQ_LEN			                   5
#define	AMS_FILE_TIME_LEN			                   14
#define AMS_MAX_LIC_FIRST_RUN_TIME_LEN                 (128)
#define AMS_MAX_LIC_RUN_TIME_LEN                       (128)
#define AMS_MAX_LIC_USED_TIME_LEN                      (128)

#define	AMS_MAX_VNC_AUTH_PWD_HEAD_LEN			       (8)
#define	AMS_MAX_RCAS_CPU_LOAD_LEVEL_VALUE			   (3)
#define	AMS_MAX_RCAS_CPU_LOAD_VALUE			           (100)
#define	AMS_RCAS_ALLOWED_CPU_LOAD			           (90)

//#define AMS_MAX_CUSTOMER_IN_QUEUE                      (AMS_MAX_VTA_NODES*0.2)
#define AMS_MAX_PROC_NUM_IN_QUEUE                      20
#define AMS_MAX_CLERA_NUM_IN_QUEUE                     5
#define AMS_MAX_UPDATE_WORK_INFO_NUM                   20

#define AMS_VTA_CALL_TRANSFER_FREEZE_TIME              (300)// 5 * 60 s
#define AMS_VTA_CONNECT_CLEAR_INTERVAL                 (600)// 10 * 60 s 
#define AMS_VTM_CONNECT_CLEAR_INTERVAL                 (600)// 10 * 60 s 
#define AMS_RCAS_CONNECT_CLEAR_INTERVAL                (600)// 10 * 60 s 
#define AMS_CMS_CALL_CLEAR_INTERVAL                    (300)// 5 * 60 s 

#define AMS_VTA_WORK_INFO_UPDATE_INTERVAL              (300) // 5 * 60 s
#define	AMS_CUSTOMER_SCORE_ENLARGE_NUM                 (1000)
#define	AMS_VTA_IDLE_RATE_ENLARGE_NUM                  (10000)

#define	AMS_ALLOWED_QUERY_INFO_RESULT_NUM              (40)
#define AMS_ALLOWED_TERM_ID_NO_NUM                     (60)

#define AMS_MAX_TELLER_TYPE                            4 //AMS_TELLER_MAX
#define AMS_MAX_VTA_STATE                              5 //AMS_VTA_STATE_RSVD 5
#define AMS_MAX_VTA_CALL_STATE                         7 //AMS_CALL_STATE_RSVD 7
#define AMS_MAX_VTM_STATE                              3 //AMS_VTM_STATE_RSVD 3
#define AMS_MAX_VTA_ANSWER_TYPE                        1 //AMS_AUTO_ANSWER 1

#define T_VTA_OPERATE_IND_TIMER_LENGTH_MAX             (1000)        //柜员操作指示最大定时1000s
#define T_AMS_CALL_EVENT_IND_TIMER_LENGTH_MAX          (1000)        //AMS呼叫事件指示最大定时1000s
#define T_AMS_CUSTOMER_IN_QUEUE_TIMER_LENGTH_MAX       (30000)       //AMS客户排队最大定时30000s
#define T_AMS_VOLUME_CTRL_TIMER_LENGTH_MAX             (1000)        //AMS音量控制最大定时1000s
#define T_AMS_RCAS_REMOTE_COOP_TIMER_LENGTH_MAX        (1000)        //AMS RCAS 远程协作最大定时1000s
#define T_AMS_VTM_REMOTE_COOP_TIMER_LENGTH_MAX         (10000)       //AMS VTM 远程协作最大定时10000s
#define T_AMS_SNAP_TIMER_LENGTH_MAX                    (1000)        //AMS拍照最大定时1000s
#define T_AMS_REST_TIMER_LENGTH_MAX                    (10000)       //AMS休息最大定时10000s
#define T_AMS_MSG_RECV_TIMER_LENGTH_MAX                (1000)        //AMS消息接收最大定时1000s
#define T_AMS_FILE_RECV_TIMER_LENGTH_MAX               (10000)       //AMS文件接收最大定时10000s
#define T_AMS_MULTI_SESS_TIMER_LENGTH_MAX              (1000)        //AMS多方会话最大定时1000s
#define T_AMS_MONITOR_TIMER_LENGTH_MAX                 (1000)        //AMS监听最大定时1000s
#define T_AMS_VTA_PARA_CFG_TIMER_LENGTH_MAX            (1000)        //AMS VTA参数配置最大定时1000s
#define T_AMS_VTM_PARA_CFG_TIMER_LENGTH_MAX            (1000)        //AMS VTM参数配置最大定时1000s
#define T_AMS_PARA_CFG_TIMER_LENGTH_MAX                (1000)        //AMS 参数配置最大定时1000s

#define AMS_SERVICE_TOTAL                              (AMS_SERVICE_SYSTEM_MAINTENANCE | \
				                                        AMS_SERVICE_ACTIVATE_CARDS | \
				                                        AMS_SERVICE_FINANCIAL_MANAGEMENT | \
				                                        AMS_SERVICE_TRANSFER_ACCOUNTS | \
				                                        AMS_SERVICE_LOADS | \
				                                        AMS_SERVICE_CREDIT_CARD | \
				                                        AMS_SERVICE_MONITOR)

#define AMS_SERVICE_VALUE_TOTAL                        (0xFFFFFFFF)
#define AMS_RSVD_SERVICE_VALUE_TOTAL                   (0xFFFFFFFF)

#define AMS_HTTP_FAST_CONNECT                          (0x8000)

#define AMS_MAX_AVG_SERVICE_TIME                       (3600)
#define AMS_AVG_SERVICE_TIME                           (100)

#define AMS_OMS_BUF_LEN_MAX                            (2048)
#define AMS_DB_TIME_LEN_MAX                            (15)
#define AMS_MAX_SINGLE_TERM_ID_NO_LEN                  (100)

#define AMS_VTA_ID_HASH_SIZE                           (AMS_MAX_VTA_NUM)
#define AMS_VTM_ID_HASH_SIZE                           (AMS_MAX_VTM_NUM)
#define AMS_ORG_ID_HASH_SIZE                           (AMS_MAX_ORG_NUM)

#define AMS_VTA_MID_BASE                                33
#define AMS_VTM_MID_BASE                                100


//zry added for smartcallcenter 0608
#ifdef scc
typedef struct tellerLoginInfo_t
{
	unsigned char   tellerIdLen;                   //坐席工号长度
	unsigned char   tellerId[AMS_MAX_TELLER_ID_LEN + 1];

	unsigned char   tellerPwdLen;                  //坐席密码长度
	unsigned char   tellerPwd[AMS_MAX_PWD_LEN + 1];

	DWORD           tellerType;                    //坐席类型 待定参数

}TELLER_LOGIN_INFO;

#else
typedef struct tellerLoginInfo_t
{
	unsigned char   tellerNoLen;                   //柜员工号长度
	unsigned char   tellerNo[AMS_MAX_TELLER_NO_LEN + 1];

	unsigned char   tellerPwdLen;                  //柜员密码长度
	unsigned char   tellerPwd[AMS_MAX_PWD_LEN + 1];

	DWORD           tellerType;                    //柜员类型

}TELLER_LOGIN_INFO;
#endif


typedef struct termNetInfo_t
{
	DWORD           ip;
	WORD            port;
//zry added for scc 2018
	BYTE            ipv6[IPV6_LEN];
//added end
}TERM_NET_INFO;


//zry added for scc 2018
#ifdef scc
typedef struct tellerRegisterInfo_t
{
	unsigned char   flag;	                       //是否注册	
	
	//DWORD           tellerId;                      //柜员标识
	BYTE			tellerIdlen;					//坐席工号长度
	BYTE			tellerId[AMS_MAX_TELLER_ID_LEN + 1];//坐席工号	
	
	DWORD           seatIp;
	WORD            seatPort;
	BYTE			seatIpv6[IPV6_LEN];
	
	PID_t	        myPid;
	PID_t	        cmsPid;
	
}TELLER_REGISTER_INFO;

#else
typedef struct tellerRegisterInfo_t
{
	unsigned char   flag;	                       //是否注册	
	
	DWORD           tellerId;                      //柜员标识
	
	DWORD           vtaIp;
	WORD            vtaPort;
	
	PID_t	        myPid;
	PID_t	        cmsPid;
	
}TELLER_REGISTER_INFO;
#endif


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

typedef struct callTarget_t
{
	WORD          callTargetType;
	WORD          targetTellerGroupId;
	DWORD         targetTellerId;

}CALL_TARGET;

typedef struct amsQueryRule_t
{
	DWORD           sequence;	


}QUEUE_RULE;

typedef struct amsServiceQuality_t
{
	WORD           score;	


}SERVICE_QUALITY;
/*
typedef struct amsAssocCallId_t
{
	BYTE           flag;
	
	BYTE           callIdLen;
	BYTE           callId[AMS_MAX_CALLID_LEN + 1];


}ASSOC_CALLID;
*/
typedef struct amsQueueInfo_t
{
	DWORD           srvGrpId;
	
	WORD            queuingLen;
	WORD            queuingTime;

}QUEUE_INFO;

typedef struct amsFilePathPara_t
{
	//configure flag
	BYTE            flag;
		
	unsigned char   filePathLen;                 //文件路径长度
	unsigned char   filePath[AMS_MAX_FILEPATH_LEN + 1];

}FILE_PATH_PARA;

typedef struct amsFileServerPara_t
{
	//configure flag
	BYTE            flag;
	
	DWORD           serverIp;
	WORD            serverPort;
	
	unsigned char   userNameLen;                 //文件服务器访问用户名长度
	unsigned char   userName[AMS_MAX_NAME_LEN + 1];

	unsigned char   userPwdLen;                  //文件服务器访问密码长度
	unsigned char   userPwd[AMS_MAX_PWD_LEN + 1];

}FILE_SERVER_PARA;

typedef struct amsScreenRecMode_t
{
	WORD           interval;	
	
}SCREEN_REC_MODE;

typedef struct amsTransProtoPara_t
{
	//configure flag
	BYTE            flag;
	
	BYTE            transType;	
	DWORD           transIp;
	WORD            transPort;

}TRANS_PROTO_PARA;

typedef struct amsSecuritiesPara_t
{
	//configure flag
	BYTE            flag;
	
	BYTE            num;
	BYTE            type[AMS_MAX_SECURITIES_NUM];
	
}SECURITIES_PARA;

typedef struct amsVNCAuthPara_t
{
	//configure flag
	BYTE            flag;
	
	BYTE            passwordLen;
	BYTE            password[AMS_MAX_PWD_LEN + 1];
	
}VNC_AUTH_PARA;

typedef struct amsRFBInitPara_t
{
	BYTE               shareDesktopFlag;
	TRANS_PROTO_PARA   transProto;	
	SECURITIES_PARA    securities;
	VNC_AUTH_PARA      vncAuth;
	
}RFB_INIT_PARA;

typedef struct amsPixelFormatPara_t
{
	//configure flag
	BYTE            flag;

	BYTE            bitsPerPixel;
	BYTE            depth;
	BYTE            bigEndianFlag;
	BYTE            trueColorFlag;
	WORD            redMax;
	WORD            greenMax;
	WORD            blueMax;
	WORD            redShift;
	WORD            greenShift;
	WORD            blueShift;

}PIXEL_FORMAT_PARA;

typedef struct amsEncodingPara_t
{
	//configure flag
	BYTE            flag;
	
	WORD            num;
	DWORD           encodings[AMS_MAX_ENCODING_NUM];

}ENCODING_PARA;

	
typedef struct amsScreenRecResultInfo_t
{
	//configure flag
	BYTE            flag;
	
	WORD            num;

}SCREEN_REC_RESULT_INFO;

typedef struct amsRcasCapPara_t
{
	//configure flag
	BYTE            flag;
	
	WORD            cncrNum;

}RCAS_CAP_PARA;

typedef struct amsBandWidthPara_t
{
	DWORD           txTotalBitUsed;
	DWORD           rxTotalBitUsed;
	DWORD           txDurationBitUsed;
	DWORD           rxDurationBitUsed;	
	DWORD           durationValue;
	
	WORD            extendFlag;
	DWORD           txLatestSessionBitUsed;
	DWORD           rxLatestSessionBitUsed;
	DWORD           txLatestFileBitUsed;
	DWORD           rxLatestFileBitUsed;
	DWORD           txLatestRemoteDesktopBitUsed;
	DWORD           rxLatestRemoteDesktopBitUsed;	

}BANDWIDTH_PARA;

typedef struct cmsInfo_t
{
	unsigned char    cModuleId;

    
}CMS_INFO;

typedef struct rcasInfo_t
{
	unsigned char     flag;                //是否配置
	unsigned char     cModuleId;           //模块号

//	SCREEN_REC_MODE   screenRecMode;
//	RFB_INIT_PARA     rfbInitPara;         //screenRec, VTA is Server
//	PIXEL_FORMAT_PARA pixelFormatPara;     //screenRec
//	ENCODING_PARA     encodingPara;        //screenRec

//	RFB_INIT_PARA     rfbInitParaForVtm;   //remote Cooperative, VTA is Server
//	RFB_INIT_PARA     rfbInitParaForVta;   //remote Cooperative, RCAS is Server, transProto get from vta

}RCAS_INFO;


typedef struct fileServerInfo_t
{
	FILE_PATH_PARA      recFilePath;               //录音文件路径
	FILE_PATH_PARA      screenRecFilePath;         //录屏文件路径
	FILE_PATH_PARA      snapFilePath;              //抓拍文件路径
	FILE_PATH_PARA      ordinaryFilePath;          //普通文件路径
	
	FILE_SERVER_PARA    fileServerPara;
	
}FILE_SERVER_INFO;

typedef struct encryptInfo_t
{
	unsigned char   flag;                          //是否配置

	WORD            encryptFlag;                   //加密开关

}ENCRYPT_INFO;

typedef struct netTraversalInfo_t
{
	unsigned char   flag;                          //是否配置

	WORD            netTraversalFlag;              //穿网开关

	DWORD           stunServerIp;
	WORD            stunServerPort;
	unsigned char   stunUserNameLen;               //STUN服务器访问用户名长度
	unsigned char   stunUserName[AMS_MAX_NAME_LEN + 1];	
	unsigned char   stunPwdLen;                    //STUN服务器访问密码长度
	unsigned char   stunPwd[AMS_MAX_PWD_LEN + 1];	
	
	DWORD           turnServerIp;
	WORD            turnServerPort;
	unsigned char   turnUserNameLen;               //TURN服务器访问用户名长度
	unsigned char   turnUserName[AMS_MAX_NAME_LEN + 1];	
	unsigned char   turnPwdLen;                    //TURN服务器访问密码长度
	unsigned char   turnPwd[AMS_MAX_PWD_LEN + 1];	

}NET_TRAVERSAL_INFO;

typedef struct branchInfo_t
{
	unsigned char   flag;                          //是否配置
	
	WORD            branchFlag;                    //分中心配置开关
		
}BRANCH_INFO;

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

	unsigned char   isHighLevel;                   //是否高等级
	
	DWORD           srvGrpId;                      //业务组编号

	DWORD           srvTypeRsvd;                   //Service Rsvd 预留业务
	DWORD           serviceType;                   //Service  业务

	unsigned char   srvGroupNameLen;               //业务组合名称长度
	unsigned char   srvGroupName[AMS_MAX_SERVICE_GROUP_NAME_LEN + 1];	
	
}SERVICE_GROUP_INFO;

typedef struct queueSysInfo_t
{
	unsigned char   flag;                          //是否配置

	DWORD           srvGrpId;                      //业务组编号

	unsigned short  maxQueLen;                     //此业务组编号允许的最大排队长度
	unsigned short  avgSrvTime;	                   //此业务组编号客户平均服务时长

}QUEUE_SYS_INFO;

typedef struct OrgInfo
{
	unsigned char       flag;	                   //是否配置
	
	unsigned int        orgId;                     //机构ID
	
	unsigned char       orgCodeLen;                //机构代码长度
	unsigned char       orgCode[AMS_MAX_ORG_CODE_LEN + 1];	           
	unsigned char       orgNameLen;                //机构名称长度
	unsigned char       orgName[AMS_MAX_ORG_NAME_LEN + 1];    
	
	unsigned int        orgType;	               //机构类型
	unsigned int        orgLevel;                  //机构级别

	unsigned char       parentOrgIdFlag;           //上级机构是否配置
	unsigned int        parentOrgId;               //上级机构ID

	unsigned char       orgStateFlag;              //机构状态是否配置
	unsigned int        orgState;                  //机构状态

}ORG_INFO;


//zry added for scc 2018
//新系统中目前只支持mobilephone的终端类型信息
typedef struct seatInfo_t
{
	unsigned char 	flag;                          	//是否配置
	unsigned char 	idstate;                       	//坐席工号状态

	unsigned char 	tellerIdlen;                   	//坐席工号长度
	unsigned char 	tellerId[AMS_MAX_TELLER_ID_LEN + 1];//坐席工号

	unsigned char 	srvGrpIdlen;                      //业务组类型长度
	unsigned char 	srvGrpId[AMS_MAX_GRPID_LEN + 1]
	
	DWORD 			seatIp;
	WORD			seatPort;	
	BYTE            seatIpv6[16];
	
}SEAT_INFO;
//added end


typedef struct tellerInfo_t
{
	unsigned char   flag;                          //是否配置

	unsigned char   idState;                       //柜员工号状态

	DWORD           tellerId;                      //柜员标识

	DWORD           tellerType;                    //柜员类型

	DWORD           srvGrpId;                      //业务组编号

	DWORD           tellerSrvAuthRsvd;             //Service Authority 业务处理权限预留
	DWORD           tellerSrvAuth;                 //Service Authority 业务处理权限

	unsigned char   tellerNoLen;                     //柜员工号长度 zhuyn added 20160621 
	unsigned char   tellerNo[AMS_MAX_TELLER_NO_LEN + 1]; 

	unsigned char   tellerNameLen;                 //柜员姓名长度
	unsigned char   tellerName[AMS_MAX_NAME_LEN + 1];

	unsigned char   tellerPwdLen;                  //柜员密码长度
	unsigned char   tellerPwd[AMS_MAX_PWD_LEN + 1];

	unsigned char   tellerOldPwdLen;               //柜员旧密码长度
	unsigned char   tellerOldPwd[AMS_MAX_PWD_LEN + 1];	

	unsigned char   tellerDepartmentNameLen;       //柜员部门名称长度
	unsigned char   tellerDepartmentName[AMS_MAX_DEPARTMENT_NAME_LEN + 1];	

	unsigned char   areaCodeLen;                   //柜员电话区号长度
	unsigned char   areaCode[AMS_MAX_AREA_CODE_NUM + 1];   //柜员电话区号

	unsigned char   phoneNumLen;                   //柜员电话号码长度
	unsigned char   phoneNum[AMS_MAX_PHONE_NUM + 1];   //柜员电话号码	

	DWORD           orgId;                         //机构ID
//	VNC_AUTH_PARA   vncAuth;                       //远程接入鉴权密码

	unsigned char   transIpLen;                        //柜员IP地址字符串长度
	unsigned char   transIp[AMS_MAX_TRANS_IP_LEN + 1]; //柜员IP地址字符串	
	DWORD           vtaIp;                         //in_addr_t
	WORD            vtaPort;                       //Sip
	WORD            vtaScreenRecPort; 	
	WORD            vtaRemoteCoopPort;             //vta pack          
	WORD            vtaRemoteCoopType;             //vta pack  
	
}TELLER_INFO;

typedef struct vtmInfo_t
{
	unsigned char   flag;	                       //是否配置

	DWORD           vtmId;                         //柜员机标识

	DWORD           terminalType;                  //终端类型

	DWORD           srvGrpId;                      //业务组编号

	unsigned char   vtmNoLen;                      //柜员机设备号长度 zhuyn added 20160621 
	unsigned char   vtmNo[AMS_MAX_VTM_NO_LEN + 1];

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

typedef struct vtmRegisterInfo_t
{
	unsigned char   flag;	                       //是否注册	
	
	DWORD           vtmId;                         //柜员机标识
	
	DWORD           vtmIp;
	WORD            vtmPort;
	
	PID_t	        myPid;
	PID_t	        cmsPid;
	
}VTM_REGISTER_INFO;


typedef struct tellerQueryResult_t
{
    WORD            state;
    DWORD           stateTime;	
	
    WORD            connectNum;
    DWORD           workSeconds;
    DWORD           idleSeconds;	

}TELLER_QUERY_RESULT;


typedef struct tellerStateDbInfo_t
{
	BYTE            tellerIdFlag;
	DWORD           tellerId;		

	BYTE            tellerStateFlag;
	BYTE            tellerState;	

	BYTE            audioRecStateFlag;
	BYTE            audioRecState;	

	BYTE            screenRecStateFlag;
	BYTE            screenRecState;	

	BYTE            remCoopStateFlag;
	BYTE            remCoopState;	

	BYTE            snapStateFlag;
	BYTE            snapState;	

	BYTE            startTimeFlag;
	BYTE            startTimeLen;
	BYTE            startTime[AMS_DB_TIME_LEN_MAX + 1];	

}TELLER_STATE_DB_INFO;


typedef struct vtmStateDbInfo_t
{
	BYTE            vtmIdFlag;	
	DWORD           vtmId;	
		
	BYTE            vtmStateFlag;
	BYTE            vtmState;

	BYTE            callIdFlag;
	BYTE            callIdLen;
	BYTE            callId[AMS_MAX_CALLID_LEN + 1];	
	
	BYTE            startTimeFlag;
	BYTE            startTimeLen;
	BYTE            startTime[AMS_DB_TIME_LEN_MAX + 1];	
	
}VTM_STATE_DB_INFO;

typedef struct tellerRealNetFlowDbInfo_t
{
	BYTE            tellerIdFlag;
	DWORD           tellerId;	

	BYTE            callIdFlag;
	BYTE            callIdLen;
	BYTE            callId[AMS_MAX_CALLID_LEN + 1];	

	BYTE            txBitUsedFlag;	
	DWORD           txBitUsed;	

	BYTE            rxBitUsedFlag;	
	DWORD           rxBitUsed;	

	BYTE            durationFlag;	
	DWORD           duration;	
  
	BYTE            startTimeFlag;
	BYTE            startTimeLen;
	BYTE            startTime[AMS_DB_TIME_LEN_MAX + 1];	
	
}TELLER_RNF_DB_INFO;

typedef struct vtmRealNetFlowDbInfo_t
{
	BYTE            vtmIdFlag;	
	DWORD           vtmId;	

	BYTE            callIdFlag;
	BYTE            callIdLen;
	BYTE            callId[AMS_MAX_CALLID_LEN + 1];	
	
	BYTE            txBitUsedFlag;	
	DWORD           txBitUsed;	

	BYTE            rxBitUsedFlag;	
	DWORD           rxBitUsed;	

	BYTE            durationFlag;	
	DWORD           duration;	
  	
	BYTE            startTimeFlag;
	BYTE            startTimeLen;
	BYTE            startTime[AMS_DB_TIME_LEN_MAX + 1];	
	
}VTM_RNF_DB_INFO;

typedef struct amsSDR_t
{
	BYTE            callIdFlag;
	BYTE            callIdLen;
	BYTE            callId[AMS_MAX_CALLID_LEN + 1];	

	BYTE            tellerIdFlag;
	DWORD           tellerId;	

	BYTE            tellerNoFlag;
	BYTE            tellerNoLen;
	BYTE            tellerNo[AMS_MAX_TELLER_NO_LEN + 1];	
	
	BYTE            tellerNameFlag;
	BYTE            tellerNameLen;
	BYTE            tellerName[AMS_MAX_NAME_LEN + 1];	

	BYTE            tellerIpFlag;
	BYTE            tellerIp[AMS_MAX_TRANS_IP_LEN + 1];

	BYTE            vtmIdFlag;
	DWORD           vtmId;

	BYTE            vtmNoFlag;
	BYTE            vtmNoLen;
	BYTE            vtmNo[AMS_MAX_VTM_NO_LEN + 1];
	
	BYTE            vtmIpFlag;
	BYTE            vtmIp[AMS_MAX_TRANS_IP_LEN + 1];

	BYTE            skillGroupIdFlag;
	DWORD           skillGroupId;
	
	BYTE            skillGroupNameFlag;
	BYTE            skillGroupNameLen;
	BYTE            skillGroupName[AMS_MAX_SERVICE_GROUP_NAME_LEN + 1];

	BYTE            audioRecNumFlag;
	BYTE            audioRecNum;

	BYTE            audioRecFailNumFlag;
	BYTE            audioRecFailNum;
	
	BYTE            screenRecNumFlag;
	BYTE            screenRecNum;

	BYTE            screenRecFailNumFlag;
	BYTE            screenRecFailNum;

	BYTE            remCoopNumFlag;
	BYTE            remCoopNum;

	BYTE            remCoopFailNumFlag;
	BYTE            remCoopFailNum;

	BYTE            snapNumFlag;
	BYTE            snapNum;

	BYTE            snapFailNumFlag;
	BYTE            snapFailNum;

	BYTE            scoreFlag;
	BYTE            score;

	BYTE            txSessionFlowFlag;
	DWORD           txSessionFlow;

	BYTE            rxSessionFlowFlag;
	DWORD           rxSessionFlow;

	BYTE            txFileFlowFlag;
	DWORD           txFileFlow;

	BYTE            rxFileFlowFlag;
	DWORD           rxFileFlow;

	BYTE            txDesktopFlowFlag;
	DWORD           txDesktopFlow;

	BYTE            rxDesktopFlowFlag;
	DWORD           rxDesktopFlow;

	BYTE            vtmTxSessionFlowFlag;
	DWORD           vtmTxSessionFlow;

	BYTE            vtmRxSessionFlowFlag;
	DWORD           vtmRxSessionFlow;

	BYTE            vtmTxFileFlowFlag;
	DWORD           vtmTxFileFlow;

	BYTE            vtmRxFileFlowFlag;
	DWORD           vtmRxFileFlow;

	BYTE            vtmTxDesktopFlowFlag;
	DWORD           vtmTxDesktopFlow;

	BYTE            vtmRxDesktopFlowFlag;
	DWORD           vtmRxDesktopFlow;

}AMS_SDR;

//---------------------------------------------------------------------------------
//网管回调参数记录 用于对VTA/VTM的配置或查询命令不能立即返回的情况  zhuyn 20160704
//注: flag = 0 无进行中的命令； flag= 1 命令进行,此时若又收到一个命令，返回失败(已有命令在执行)
enum 
{
	CONFIGIND_UPDATE_FILESERVER = 1, //更新文件服务器参数配置
	CONFIGIND_UPDATE_TERMPARA = 2,   // 2：更新终端参数配置
	CONFIGIND_ADD_ID_NO = 3,         // 3:增加标识映射
	CONFIGIND_MODIFY_ID_NO = 4,      // 4:修改标识映射
	CONFIGIND_DELETE_ID_NO = 5,      // 5:删除标识映射

	CONFIGIND_GET_TERMPARA = 0x10,  //0x10：查询终端参数配置
};

typedef struct
{
    unsigned char    flag;
    unsigned char    configInd;   //对应发到终端的命令
    oa_termpara_cbfun    cbFunc;
    unsigned char    para[OA_MAX_CBPARA_LEN];
}OACBPARA_t;
//---------------------------------------------------------------------------------


//zry added for scc 2018
/* 坐席结点结构体定义 */
typedef struct  seatNode_t
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
	
	SEAT_INFO     seatInfo;

	TELLER_WORK_INFO seatWorkInfo;

	DWORD           callTransferNum;
		
	DWORD           dailyAvgScore;           //enlarge 1000
	DWORD           dailyAvgScoreExpect;     //enlarge 1000   

	DWORD           tellerCfgPos;
	DWORD           tellerRegPos;
	DWORD           orgCfgPos;
	
	OACBPARA_t      oaCbPara;       //网管回调用参数 zhuyn 20160704
	
}SEAT_NODE;


typedef struct userNode_t
{
	NODE			node;  
	
	//Customer In Queue Pid
	DWORD			customerPid;

	DWORD			amsPid;
	DWORD			state;	
	DWORD			callState;	
	DWORD			serviceState;	

	int 			sendMsgToVtaState;
	int 			sendMsgToVtmState;
	
	int 			sendFileToVtaState;
	int 			sendFileToVtmState;
	
	int 			vtmParaCfgState;

	TIME_INFO		stateStartLocalTime;	
	TIME_INFO		callStateStartLocalTime; 
	TIME_INFO		serviceStateStartLocalTime; 
	time_t			stateStartTime; 
	time_t			callStateStartTime; 
	time_t			serviceStateStartTime;			
	time_t			handshakeTime;
	time_t			enterQueueTime;

	DWORD			serviceType;

	//timer
	int 			iTimerId; //Not Use Yet
	int 			sendMsgToVtaTimerId;  //VTA
	int 			sendMsgToVtmTimerId;  //VTM
	int 			sendFileToVtaTimerId; //VTA
	int 			sendFileToVtmTimerId; //VTM
	int 			vtmParaCfgTimerId;	  //VTM
//	int 			iTimerType;

	//communication pid
	PID_t			myPid;
	PID_t			rPid;	  //vtm
	PID_t			cmsPid;   //cms
	PID_t			vtaPid;   //VTA 

	VTM_INFO		vtmInfo;

	VNC_AUTH_PARA	vncAuth;					   //远程接入鉴权密码

	DWORD			vtmCfgPos;
	DWORD			vtmRegPos;
	DWORD			orgCfgPos;
	
	OACBPARA_t		oaCbPara;		//网管回调用参数 zhuyn 20160704

}USER_NODE;

//added end


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
	
	OACBPARA_t      oaCbPara;       //网管回调用参数 zhuyn 20160704
	
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

	VNC_AUTH_PARA   vncAuth;                       //远程接入鉴权密码

	DWORD           vtmCfgPos;
	DWORD           vtmRegPos;
	DWORD           orgCfgPos;
	
	OACBPARA_t      oaCbPara;       //网管回调用参数 zhuyn 20160704
	
}VTM_NODE;

//zry added for scc 2018
typedef struct seatId_t{
	NODE				node;
	struct seatId_t		*hashNext;

	BTYE				tellerIdlen;
	BYTE				tellerId[AMS_MAX_TELLER_ID_LEN + 1];
	DWORD				tellerPos;
}SEAT_ID_NODE;

//added end


typedef struct vtaId_t{
	NODE          		node;     
	struct vtaId_t      *hashNext;

	DWORD                tellerId;
	DWORD                tellerPos;	

} VTA_ID_NODE;

typedef struct vtmId_t{
	NODE          		node;     
	struct vtmId_t      *hashNext;

	DWORD                vtmId;
	DWORD                vtmPos;	

} VTM_ID_NODE;

typedef struct orgId_t{
	NODE          		node;     
	struct orgId_t      *hashNext;

	DWORD                orgId;
	DWORD                orgPos;	

} ORG_ID_NODE;

/* struct of ams license data */
typedef struct amsDataLic_t
{
	pthread_mutex_t    amsLicProcMtx;
	unsigned char      licState;
	
	unsigned int       vtaNum;
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
	RCAS_INFO          rcasInfo[AMS_MAX_RCAS_NUM];
	RFB_INIT_PARA      rfbInitParaForVtm;   //remote Cooperative, VTA is Server
	RFB_INIT_PARA      rfbInitParaForVta;   //remote Cooperative, RCAS is Server, transProto get from vta
	
	FILE_SERVER_INFO   fileServerInfo;
	ENCRYPT_INFO       encryptInfo;
	NET_TRAVERSAL_INFO netTraversalInfo;
	BRANCH_INFO        branchInfo;
	
	SERVICE_INFO       serviceInfo[AMS_MAX_SERVICE_NUM];

	SERVICE_GROUP_INFO srvGroupInfo[AMS_MAX_SERVICE_GROUP_NUM];

	QUEUE_SYS_INFO     queueSysInfo[AMS_MAX_SERVICE_GROUP_NUM];

	ORG_INFO           orgInfo[AMS_MAX_ORG_NUM];
	
	TELLER_INFO        tellerInfo[AMS_MAX_VTA_NUM];

	VTM_INFO           vtmInfo[AMS_MAX_VTM_NUM];

	VTA_ID_NODE        *VtaIdHashTbl[AMS_VTA_ID_HASH_SIZE];	

	VTM_ID_NODE        *VtmIdHashTbl[AMS_VTM_ID_HASH_SIZE];

	ORG_ID_NODE        *OrgIdHashTbl[AMS_ORG_ID_HASH_SIZE];

	//zry added for scc 2018
	unsigned int		maxSeatNum;
	SEAT_INFO			seatInfo[AMS_MAX_SEAT_NUM];
	SEAT_ID_NODE		*SeatHashTbl[AMS_SEAT_ID_HASH_SIZE];
	//added end
	
}AMS_DATA_SYSCFG;

/* struct of ams data register */
typedef struct amsDataRegister_t
{
//zry added for scc 2018
#ifdef scc
	TELLER_REGISTER_INFO tellerRegisterInfo[AMS_MAX_SEAT_NUM];
#else
    TELLER_REGISTER_INFO tellerRegisterInfo[AMS_MAX_VTA_NUM];
#endif

	VTM_REGISTER_INFO    vtmRegisterInfo[AMS_MAX_VTM_NUM];




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

//zry added for scc 2018
	sem_t            seatCtrl;
	LIST			 seatList;
//added end
}AMS_SERVICE_MANAGE;


/* struct of rcas Mng */
typedef struct amsRcasMng_t //rcas扩容
{
	int              state;

	unsigned short   currCncrNum;
	unsigned short   cpuLoadLevel;
	unsigned short   cpuLoad;	

	time_t           handshakeTime;
	
	PID_t            rcasPid;
	
	RCAS_CAP_PARA    rcasCap;

	RCAS_INFO        rcasInfo;
	
}AMS_RCAS_MANAGE;

/* struct of rcas Data */
typedef struct amsRcasData_t //rcas扩容
{
	int             currRcasPos;
	
	AMS_RCAS_MANAGE amsRcasMngData[AMS_MAX_RCAS_NUM];
	
}AMS_RCAS_DATA;


/* struct of alarm */
typedef struct amsAlarm_t
{
	unsigned char vtm_node_usage_alarm_threshold;                //柜员机结点使用告警阈值
	unsigned char vta_node_usage_alarm_threshold;                //柜员结点使用告警阈值
	
}AMS_ALARM;


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
	
}AMS_DEBUG;

/* struct of msg stat */
typedef struct amsMsgStat_t
{
	//ams <-> vta msg	
	unsigned long  recvVtaLoginReq;
	unsigned long  sendVtaLoginRsp;
	unsigned long  recvVtaLogoutReq;
	unsigned long  sendVtaLogoutRsp;

	unsigned long  recvVtaHandshakeReq;
	unsigned long  sendVtaHandshakeRsp;

	unsigned long  recvVtaStateOperateReq;
	unsigned long  sendVtaStateOperateRsp; 

	unsigned long  sendVtaStateOperateInd; 
	unsigned long  recvVtaStateOperateCnf;

	unsigned long  recvVtaModifyPasswordReq;
	unsigned long  sendVtaModifyPasswordRsp;
		
	unsigned long  recvVtaServiceRegReq;
	unsigned long  sendVtaServiceRegRsp;

	unsigned long  recvVtaTransferCallReq;
	unsigned long  sendVtaTransferCallRsp;

	unsigned long  recvVtaVolumeCtrlReq;
	unsigned long  sendVtaVolumeCtrlRsp;
	unsigned long  recvVtaAudioRecordReq;
	unsigned long  sendVtaAudioRecordRsp;

	unsigned long  recvVtaScreenRecordReq;
	unsigned long  sendVtaScreenRecordRsp;
	unsigned long  recvVtaRemoteCooperativeReq;
	unsigned long  sendVtaRemoteCooperativeRsp;
    unsigned long  recvVtaSnapReq;    
	unsigned long  sendVtaSnapRsp;

    unsigned long  recvVtaSendMsgReq;
	unsigned long  sendVtaSendMsgRsp;
	unsigned long  sendVtaRecvMsgInd;
	unsigned long  recvVtaRecvMsgCnf;

    unsigned long  recvVtaSendFileReq;
	unsigned long  sendVtaSendFileRsp;
	unsigned long  sendVtaRecvFileInd;
	unsigned long  recvVtaRecvFileCnf;

	unsigned long  recvVtaForceLoginReq;
	unsigned long  sendVtaForceLoginRsp;
    unsigned long  recvVtaQueryInfoReq;
	unsigned long  sendVtaQueryInfoRsp;
	
	unsigned long  recvManagerAddVtaReq;
	unsigned long  sendManagerAddVtaRsp;
	unsigned long  recvManagerDelVtaReq;
	unsigned long  sendManagerDelVtaRsp;

	unsigned long  recvManagerModifyVtaReq;
	unsigned long  sendManagerModifyVtaRsp;
	unsigned long  recvManagerQueryVtaReq;
	unsigned long  sendManagerQueryVtaRsp;
	        
	unsigned long  recvManagerForceLogoutReq;
	unsigned long  sendManagerForceLogoutRsp;
	unsigned long  recvManagerSetVtaStateReq;
	unsigned long  sendManagerSetVtaStateRsp;

	unsigned long  recvManagerQueryVtaStateReq;
	unsigned long  sendManagerQueryVtaStateRsp;

	unsigned long  recvManagerModifyQueueRuleReq;
	unsigned long  sendManagerModifyQueueRuleRsp;

	unsigned long  recvManagerForceRelCallReq;
	unsigned long  sendManagerForceRelCallRsp;
	
	unsigned long  recvInspectorMonitorReq;
	unsigned long  sendInspectorMonitorRsp;

	unsigned long  recvVtaBandwidthNotice;	
	unsigned long  recvVtaEventNotice;

	unsigned long  sendVtaParaCfgInd;
	unsigned long  recvVtaParaCfgIndCnf;
	
	unsigned long  sendVtaEventInd;

	//ams <-> vtm msg
	unsigned long  recvVtmLoginReq;
	unsigned long  sendVtmLoginRsp;
	unsigned long  recvVtmLogoutReq;
	unsigned long  sendVtmLogoutRsp;

	unsigned long  recvVtmHandshakeReq;
	unsigned long  sendVtmHandshakeRsp;

	unsigned long  sendVtmVolumeCtrlInd;
	unsigned long  recvVtmVolumeCtrlCnf;
	unsigned long  sendVtmSnapInd;
	unsigned long  recvVtmSnapCnf;

	unsigned long  recvVtmSendMsgReq;
	unsigned long  sendVtmSendMsgRsp;
	unsigned long  sendVtmRecvMsgInd;
	unsigned long  recvVtmRecvMsgCnf;

	unsigned long  recvVtmSendFileReq;
	unsigned long  sendVtmSendFileRsp;
	unsigned long  sendVtmRecvFileInd;
	unsigned long  recvVtmRecvFileCnf;

	unsigned long  recvVtmForceLoginReq;
	unsigned long  sendVtmForceLoginRsp;	
    unsigned long  recvVtmQueryInfoReq;
	unsigned long  sendVtmQueryInfoRsp;
	
	unsigned long  recvVtmBandwidthNotice;
	unsigned long  recvVtmEventNotice;
	
	unsigned long  sendVtmParaCfgInd;
	unsigned long  recvVtmParaCfgIndCnf;
	
	unsigned long  sendVtmEventInd;

	//ams <-> cms msg
	unsigned long  recvVtaRegReq;
	unsigned long  sendVtaRegRsp;
	unsigned long  recvVtmRegReq;
	unsigned long  sendVtmRegRsp;	
	unsigned long  recvVtaGetReq;
	unsigned long  sendVtaGetRsp;

	unsigned long  recvAmsCallEventNoticeReq;
	unsigned long  sendAmsCallEventInd;
	unsigned long  recvAmsCallEventIndCnf;	

	//ams <-> rcas msg
	unsigned long  sendRcasStartScreenShareReq;
	unsigned long  recvRcasStartScreenShareRsp;
	unsigned long  recvRcasStartScreenShareCnf;
	unsigned long  sendRcasStopScreenShareReq;
	unsigned long  recvRcasStopScreenShareCnf;

	unsigned long  sendRcasControlScreenReq;
	unsigned long  recvRcasControlScreenCnf;
	unsigned long  sendRcasCancelControlScreenReq;
	unsigned long  recvRcasCancelControlScreenCnf;

	unsigned long  recvRcasHandshakeReq;
	unsigned long  sendRcasHandshakeRsp;
	unsigned long  recvRcasLoadCapacityNotice;
	
	unsigned long  recvRcasScreenShareEventNotice;
	unsigned long  sendRcasScreenShareEventInd;

	//ams B msg
	unsigned long  vtaStateOperateIndTimeout;
	unsigned long  callEventIndTimeout;
	unsigned long  customerInQueueTimeout;
	unsigned long  volumeCtrlTimeout;
	unsigned long  rcasRemoteCoopTimeout;
	unsigned long  vtmRemoteCoopTimeout;	
	unsigned long  snapTimeout;
	unsigned long  restTimeout;
	unsigned long  vtaRecvMsgTimeout;
	unsigned long  vtmRecvMsgTimeout;
	unsigned long  vtaRecvFileTimeout;
	unsigned long  vtmRecvFileTimeout;	
	
	unsigned long  vtaMultiSessTimeout;
	unsigned long  vtaMonitorTimeout;	

	unsigned long  vtaParaCfgTimeout;
	unsigned long  vtmParaCfgTimeout;
	
	unsigned long  paraCfgTimeout;	

	//ams C msg
	unsigned long  sendServiceProcReq;
	unsigned long  recvServiceProcReq;
	
	//ams unknown msg
	unsigned long  amsErrMsg;
	unsigned long  amsUnknownMsgType;
	unsigned long  amsUnknownMsg;

	unsigned long  amsErrBMsg;
	unsigned long  amsUnknownBMsg;

}AMS_MSG_STAT;


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


/* struct of queue system stat */
typedef struct amsQueueSystemStat_t
{	
	unsigned long  tellerInSystemNum;                          //系统中的柜员数量
	unsigned long  customerInServiceNum;                       //系统中正在接受服务的客户数量，即正在提供服务的柜员数量
	unsigned long  customerInQueueNum;                         //系统当前的等待队列规模
	unsigned long  customerInSystemNum;                        //系统当前的规模:各业务等待队列中的客户数 + 各业务服务队列中的客户数

	unsigned long  tellerCfgNum;                               //系统配置的柜员数量
	unsigned long  vtmCfgNum;                                  //系统配置的柜员机数量
	unsigned long  tellerTypeCfgNum[AMS_MAX_TELLER_TYPE];      //系统配置的各类型柜员数
	
	unsigned long  customerEnterSystemTotalNum;                //客户总数量
	unsigned long  customerAcceptServiceTotalNum;              //接受服务的客户总数量
	unsigned long  customerEnterQueueTotalNum;                 //进入排队的客户总数量
	unsigned long  customerInQueueTimeoutTotalNum;             //排队超时的客户总数量
	unsigned long  customerDequeueTotalNum;                    //取消排队的客户总数量

	unsigned long  averageCustomerInServiceTime;               //客户接受服务的平均时长
	unsigned long  averageCustomerInQueueTime;                 //客户在等待队列中平均耗时	
	unsigned long  averageCustomerInSystemTime;                //客户在系统内平均耗时
     
	unsigned long  tellerTypeStat[AMS_MAX_TELLER_TYPE];        //各类型柜员数
	unsigned long  tellerStateStat[AMS_MAX_VTA_STATE];         //各状态下柜员数
	unsigned long  tellerCallStateStat[AMS_MAX_VTA_CALL_STATE];//各呼叫状态下柜员数

	unsigned long  vtmInSystemNum;                             //系统中的柜员机数量
	unsigned long  vtmStateStat[AMS_MAX_VTM_STATE];            //各状态下柜员机数
	
}AMS_QUEUE_SYSTEM_STAT;


/* struct of service group stat */
typedef struct amsSrvGrpStat_t
{
	unsigned long  tellerNum;                                  //此业务当前的柜员数量	
	unsigned long  custInServiceNum;                           //此业务正在接受服务的客户数量，即正在提供服务的柜员数量
	unsigned long  custInQueueNum;                             //此业务当前的等待队列规模
	unsigned long  custInSystemNum;                            //此业务当前的客户规模:等待队列中的客户数 + 正在接受服务的客户数

	unsigned long  tellerCfgNum;                               //此业务配置的柜员数量
	unsigned long  vtmCfgNum;                                  //此业务配置的柜员机数量
	unsigned long  tellerTypeCfgNum[AMS_MAX_TELLER_TYPE];      //此业务配置的各类型柜员数
	
	unsigned long  custEnterSystemTotalNum;                    //此业务的办理总次数
	unsigned long  custAcceptServiceTotalNum;                  //接受此业务服务的客户总数量
	unsigned long  custEnterQueueTotalNum;                     //此业务进入排队的客户总数量
	unsigned long  custInQueueTimeoutTotalNum;                 //此业务排队超时的客户总数量
	unsigned long  custDequeueTotalNum;                        //此业务取消排队的客户总数量
	
	unsigned long  averageCustInServiceTime;                   //客户在此业务中接受服务的平均时长	
	unsigned long  averageCustInQueueTime;                     //客户在此业务的等待队列中平均耗时	
	unsigned long  averageCustInSystemTime;                    //客户在此业务中平均耗时

	unsigned long  tellerTypeStat[AMS_MAX_TELLER_TYPE];        //各类型柜员数
	unsigned long  tellerStateStat[AMS_MAX_VTA_STATE];         //各状态下柜员数
	unsigned long  tellerCallStateStat[AMS_MAX_VTA_CALL_STATE];//各呼叫状态下柜员数

	unsigned long  vtmInSystemNum;                             //办理此业务的柜员机数量
	
}AMS_SERVICE_GROUP_STAT;


/* struct of service type stat */
typedef struct amsServiceTypeStat_t
{
	unsigned long  systemMaintenanceNum;                  //系统维护次数
	unsigned long  activateCardsNum;                      //开卡次数
	unsigned long  financialManagementNum;                //理财次数
	unsigned long  transferAccountsNum;                   //转账次数
	unsigned long  loansNum;                              //贷款次数
	unsigned long  creditCardNum;                         //信用卡业务次数
	unsigned long  monitorNum;                            //监听次数

}AMS_SERVICE_TYPE_STAT;


/* struct of service stat */
typedef struct amsServiceStat_t
{
	unsigned long  serviceTotalNum;                       //业务办理总次数

	//业务组统计  
	AMS_SERVICE_GROUP_STAT srvGrpStat[AMS_MAX_SERVICE_GROUP_NUM]; 

	//各业务的激活状态
	//get from AmsSrvData(i).serviceState

	//业务类型统计  
	AMS_SERVICE_TYPE_STAT serviceTypeStat[AMS_MAX_SERVICE_TYPE_NUM]; //Not Used

}AMS_SERVICE_STAT;


/* struct of session stat */
typedef struct amsSessionStat_t
{
	unsigned long  audioRecNum;
	unsigned long  audioRecFailNum;	
	unsigned long  scrRecNum;
	unsigned long  scrRecFailNum;		
	unsigned long  remCoopNum;
	unsigned long  remCoopFailNum;	
	unsigned long  snapNum;
	unsigned long  snapFailNum;	

	unsigned char  scoreFlag;
	unsigned long  score;	

	unsigned long  unknownStat;	
	
}AMS_SESSION_STAT;


/* struct of teller stat */
typedef struct amsTellerStat_t
{
	DWORD          tellerId;                      //柜员工号
	
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

/* struct of vtm stat */
typedef struct amsVtmStat_t
{
	DWORD          vtmId;                         //柜员机标识
	
	unsigned long  loginNum;
	unsigned long  logoutNum;
	unsigned long  handshakeNum;

	unsigned long  screenRecordNum;
	unsigned long  snapNum;

	unsigned long  unknownStat;	

}AMS_VTM_STAT;


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

	//Vta Login Result
	unsigned long  vtaLoginSuccess;
	unsigned long  vtaLoginParaErr;
	unsigned long  vtaLoginStateErr;
	unsigned long  vtaLoginLenErr;
	unsigned long  vtaLoginLpResourceLimited;
	unsigned long  vtaLoginNodeResourceLimited;
	unsigned long  vtaLoginTellerLenErr;
	unsigned long  vtaLoginTellerNoErr;
	unsigned long  vtaLoginTellerPwdErr;
	unsigned long  vtaLoginTellerVncAuthPwdErr;
	unsigned long  vtaLoginTellerLoginRepeatedly;
	unsigned long  vtaLoginTellerTypeErr;
	unsigned long  vtaLoginTellerNumErr;
	unsigned long  vtaLoginFileServerUsrNameErr;
	unsigned long  vtaLoginFileServerUsrPwdErr;
	unsigned long  vtaLoginOrgIdErr;
	unsigned long  vtaLoginOrgStateErr;	
	unsigned long  vtaLoginLicenseTimeout;
	unsigned long  vtaLoginTellerNumBeyondLic;

	//Vta Logout Result
	unsigned long  vtaLogoutSuccess;
	unsigned long  vtaLogoutParaErr;          
	unsigned long  vtaLogoutStateErr;         
	unsigned long  vtaLogoutLenErr;           
	unsigned long  vtaLogoutAmsPidErr;        
	unsigned long  vtaLogoutTellerIdErr;      
	unsigned long  vtaLogoutSrvGrpIdErr;  
	unsigned long  vtaLogoutServiceStateErr;  

	//Vta Handshake Result
	unsigned long  vtaHandshakeSuccess;                                   
	unsigned long  vtaHandshakeParaErr;                                                                          	
	unsigned long  vtaHandshakeSateErr;                                                                           
	unsigned long  vtaHandshakeLenErr;                                                                        
	unsigned long  vtaHandshakeAmsPidErr;                                                                   
	unsigned long  vtaHandshakeTellerIdErr;                                                              
	unsigned long  vtaHandshakeSrvGrpIdErr;                                                          
	unsigned long  vtaHandshakeServiceStateErr;    
	unsigned long  vtaClearInactiveTeller;
	
	//Vta State Operate Result
	unsigned long  vtaStateOperateSuccess;
	unsigned long  vtaStateOperateParaErr;
	unsigned long  vtaStateOperateStateErr;
	unsigned long  vtaStateOperateLenErr;
	unsigned long  vtaStateOperateAmsPidErr;
	unsigned long  vtaStateOperateTellerIdErr;
	unsigned long  vtaStateOperateSrvGrpIdErr;
	unsigned long  vtaStateOperateServiceStateErr;
	unsigned long  vtaStateOperateCodeErr;
	unsigned long  vtaStateOperateOpReasonErr;
	unsigned long  vtaStateOperateOpTimeLenErr;
	unsigned long  vtaStateOperateUpdateStateErr;
	unsigned long  vtaStateOperateStartTimerErr;
	unsigned long  vtaStateOperateRestTimeout;	
	
	unsigned long  vtaStateOpRestTimeoutParaErr;
	unsigned long  vtaStateOpRestTimeoutStateErr;
	unsigned long  vtaStateOpRestTimeoutLenErr;
	unsigned long  vtaStateOpRestTimeoutAmsPidErr;
	unsigned long  vtaStateOpRestTimeoutTellerIdErr;
	unsigned long  vtaStateOpRestTimeoutSrvGrpIdErr;
	unsigned long  vtaStateOpRestTimeoutSrvStErr;

	//Vta Modify Password Result
	unsigned long  vtaModifyPasswordSuccess;
	unsigned long  vtaModifyPasswordParaErr;
	unsigned long  vtaModifyPasswordStateErr;
	unsigned long  vtaModifyPasswordLenErr;
	unsigned long  vtaModifyPasswordAmsPidErr;
	unsigned long  vtaModifyPasswordTellerIdErr;
	unsigned long  vtaModifyPasswordOldPasswordErr;
	unsigned long  vtaModifyPasswordNewPasswordErr;
	unsigned long  vtaModifyPasswordSrvGrpIdErr;
	unsigned long  vtaModifyPasswordServiceStateErr;
	unsigned long  vtaModifyPasswordTellerPosErr;

	//Vta Service Reg Result
	unsigned long  vtaServiceRegSuccess;
	unsigned long  vtaServiceRegParaErr;
	unsigned long  vtaServiceRegStateErr;
	unsigned long  vtaServiceRegLenErr;
	unsigned long  vtaServiceRegAmsPidErr;
	unsigned long  vtaServiceRegTellerIdErr; 
	unsigned long  vtaServiceRegSrvGrpIdErr;
	unsigned long  vtaServiceRegServiceStateErr;
	unsigned long  vtaServiceRegServiceRsvdErr;
	unsigned long  vtaServiceRegServiceUnsupport;
	unsigned long  vtaServiceRegServiceUnmatch;
	unsigned long  vtaServiceRegServiceRegRepeatedly;
	unsigned long  vtaServiceRegTellerPosErr;

	//Vta Transfer Call Result
	unsigned long  vtaTransferCallSuccess;
	unsigned long  vtaTransferCallParaErr;
	unsigned long  vtaTransferCallStateErr;
	unsigned long  vtaTransferCallLenErr; 
	unsigned long  vtaTransferCallAmsPidErr; 
	unsigned long  vtaTransferCallTellerIdErr;
	unsigned long  vtaTransferCallSrvGrpIdErr;
	unsigned long  vtaTransferCallServiceStateErr;
	unsigned long  vtaTransferCallTargetTypeErr;
	unsigned long  vtaTransferCallTargetGroupErr;
	unsigned long  vtaTransferCallTargetTellerErr;
	unsigned long  vtaTransferCallCallIdErr;
	unsigned long  vtaTransferCallStartTimerErr;
	unsigned long  vtaTransferCallTimeout;
	unsigned long  vtaTransferCallCmsReportFail;
		
	//Vta Volume Ctrl Result
	unsigned long  vtaVolumeCtrlSuccess;
	unsigned long  vtaVolumeCtrlParaErr;
	unsigned long  vtaVolumeCtrlStateErr;
	unsigned long  vtaVolumeCtrlLenErr;
	unsigned long  vtaVolumeCtrlAmsPidErr;
	unsigned long  vtaVolumeCtrlTellerIdErr; 
	unsigned long  vtaVolumeCtrlSrvGrpIdErr;
	unsigned long  vtaVolumeCtrlServiceStateErr;
	unsigned long  vtaVolumeCtrlTypeErr;
	unsigned long  vtaVolumeCtrlStartTimerErr;
	unsigned long  vtaVolumeCtrlVtmMuteTimeout;
	unsigned long  vtaVolumeCtrlVtmUnmuteTimeout;

	unsigned long  vtaVolumeCtrlTimeoutParaErr;
	unsigned long  vtaVolumeCtrlTimeoutStateErr;
	unsigned long  vtaVolumeCtrlTimeoutLenErr;
	unsigned long  vtaVolumeCtrlTimeoutAmsPidErr;
	unsigned long  vtaVolumeCtrlTimeoutTellerIdErr;
	unsigned long  vtaVolumeCtrlTimeoutTellerStateErr;
	unsigned long  vtaVolumeCtrlTimeoutSrvGrpIdErr;
	unsigned long  vtaVolumeCtrlTimeoutServiceStateErr;
	unsigned long  vtaVolumeCtrlTimeoutTypeErr;

	unsigned long  vtmVolumeCtrlParaErr;
	unsigned long  vtmVolumeCtrlStateErr;
	unsigned long  vtmVolumeCtrlLenErr;
	unsigned long  vtmVolumeCtrlAmsPidErr;
	unsigned long  vtmVolumeCtrlTellerIdErr;
	unsigned long  vtmVolumeCtrlVtmIdErr;
	unsigned long  vtmVolumeCtrlTypeErr;
	unsigned long  vtmVolumeCtrlVtmReportFail;
	
	//Vta Audio Record Result
	unsigned long  vtaAudioRecordSuccess;
	unsigned long  vtaAudioRecordParaErr;
	unsigned long  vtaAudioRecordStateErr;
	unsigned long  vtaAudioRecordLenErr;
	unsigned long  vtaAudioRecordAmsPidErr; 
	unsigned long  vtaAudioRecordTellerIdErr;
	unsigned long  vtaAudioRecordSrvGrpIdErr;
	unsigned long  vtaAudioRecordServiceStateErr;
	unsigned long  vtaAudioRecordCallIdErr;
	unsigned long  vtaAudioRecordCallStateErr;
	unsigned long  vtaAudioRecordTypeErr;
	unsigned long  vtaAudioRecordRecStateErr;
	unsigned long  vtaAudioRecordFileTypeErr;
	unsigned long  vtaAudioRecordFileNameErr;
	unsigned long  vtaAudioRecordFilePathErr;
	
	//Vta Screen Record Result
	unsigned long  vtaScreenRecordSuccess;
	unsigned long  vtaScreenRecordParaErr;
	unsigned long  vtaScreenRecordStateErr;
	unsigned long  vtaScreenRecordLenErr;
	unsigned long  vtaScreenRecordAmsPidErr;
	unsigned long  vtaScreenRecordTellerIdErr;
	unsigned long  vtaScreenRecordSrvGrpIdErr;
	unsigned long  vtaScreenRecordServiceStateErr;
	unsigned long  vtaScreenRecordCallIdErr;
	unsigned long  vtaScreenRecordCallStateErr;
	unsigned long  vtaScreenRecordTypeErr; 
	unsigned long  vtaScreenRecordFileTypeErr;
	unsigned long  vtaScreenRecordFileNameErr;
	unsigned long  vtaScreenRecordFilePathErr;

	//Vta Remote Cooperative Result
	unsigned long  vtaRemoteCooperativeSuccess;
	unsigned long  vtaRemoteCooperativeParaErr;
	unsigned long  vtaRemoteCooperativeStateErr;
	unsigned long  vtaRemoteCooperativeLenErr;
	unsigned long  vtaRemoteCooperativeAmsPidErr;
	unsigned long  vtaRemoteCooperativeTellerIdErr;
	unsigned long  vtaRemoteCooperativeSrvGrpIdErr;
	unsigned long  vtaRemoteCooperativeServiceStateErr;
	unsigned long  vtaRemoteCooperativeCallIdErr;
	unsigned long  vtaRemoteCooperativeCallStateErr;
	
	unsigned long  vtaRemoteCooperativeOpTypeErr;
	unsigned long  vtaRemoteCooperativeOpStateErr;	
	unsigned long  vtaRemoteCooperativeVtmIdErr;
	unsigned long  vtaRemoteCooperativeVtmStateErr;
	unsigned long  vtaRemoteCooperativeRcasTransProtoParaErr;
	unsigned long  vtaRemoteCooperativeRfbInitParaForVtmErr;
	unsigned long  vtaRemoteCooperativeRfbInitParaForVtaErr;
	unsigned long  vtaRemoteCooperativeVtaTransProtoParaErr;
	unsigned long  vtaRemoteCooperativeStartTimerToVtmErr;	
	
	unsigned long  vtaRemoteCooperativeTimeoutParaErr;
	unsigned long  vtaRemoteCooperativeTimeoutStateErr;
	unsigned long  vtaRemoteCooperativeTimeoutLenErr;
	unsigned long  vtaRemoteCooperativeTimeoutAmsPidErr;
	unsigned long  vtaRemoteCooperativeTimeoutTellerIdErr;
	unsigned long  vtaRemoteCooperativeTimeoutTellerStateErr;
	unsigned long  vtaRemoteCooperativeTimeoutSrvGrpIdErr;
	unsigned long  vtaRemoteCooperativeTimeoutSrvStErr;
	unsigned long  vtaRemoteCooperativeTimeoutVtmIdErr;
	unsigned long  vtaRemoteCooperativeTimeoutVtmStateErr;	
	unsigned long  vtaRemoteCooperativeTimeoutCallIdErr;
	unsigned long  vtaRemoteCooperativeTimeoutOpTypeErr;
	
	unsigned long  rcasStartDesktopShareParaErr;
	unsigned long  rcasStartDesktopShareStateErr;
	unsigned long  rcasStartDesktopShareLenErr; 
	unsigned long  rcasStartDesktopShareTellerIdErr;
	unsigned long  rcasStartDesktopShareSrvGrpIdErr;
	unsigned long  rcasStartDesktopShareSrvStErr;	
	unsigned long  rcasStartDesktopShareCallIdErr;
	unsigned long  rcasStartDesktopShareVtmIdErr;
	unsigned long  rcasStartDesktopShareVtmStateErr;	
	unsigned long  rcasStartDesktopShareVtmTransProtoParaErr;
	unsigned long  rcasStartDesktopShareVtaTransProtoParaErr;
	unsigned long  rcasStartDesktopSharePixelFormatParaErr;
	unsigned long  rcasStartDesktopShareEncodingParaErr;
	unsigned long  rcasStartDesktopShareTimeout;
	
	unsigned long  rcasStopDesktopShareParaErr;
	unsigned long  rcasStopDesktopShareStateErr;
	unsigned long  rcasStopDesktopShareLenErr;
	unsigned long  rcasStopDesktopShareTellerIdErr;
	unsigned long  rcasStopDesktopShareSrvGrpIdErr;
	unsigned long  rcasStopDesktopShareSrvStErr;	
	unsigned long  rcasStopDesktopShareCallIdErr;
	unsigned long  rcasStopDesktopShareVtmIdErr;
	unsigned long  rcasStopDesktopShareVtmStateErr;	
	unsigned long  rcasStopDesktopShareTimeout;
	
	unsigned long  rcasStartDesktopCtrlParaErr;
	unsigned long  rcasStartDesktopCtrlStateErr;	
	unsigned long  rcasStartDesktopCtrlLenErr;
	unsigned long  rcasStartDesktopCtrlTellerIdErr;
	unsigned long  rcasStartDesktopCtrlSrvGrpIdErr;
	unsigned long  rcasStartDesktopCtrlSrvStErr;	
	unsigned long  rcasStartDesktopCtrlCallIdErr;
	unsigned long  rcasStartDesktopCtrlVtmIdErr;
	unsigned long  rcasStartDesktopCtrlVtmStateErr;	
	unsigned long  rcasStartDesktopCtrlTimeout;
	
	unsigned long  rcasStopDesktopCtrlParaErr;
	unsigned long  rcasStopDesktopCtrlStateErr;
	unsigned long  rcasStopDesktopCtrlLenErr; 
	unsigned long  rcasStopDesktopCtrlTellerIdErr;	
	unsigned long  rcasStopDesktopCtrlSrvGrpIdErr;	
	unsigned long  rcasStopDesktopCtrlSrvStErr;		
	unsigned long  rcasStopDesktopCtrlCallIdErr;
	unsigned long  rcasStopDesktopCtrlVtmIdErr;
	unsigned long  rcasStopDesktopCtrlVtmStateErr;	
	unsigned long  rcasStopDesktopCtrlTimeout;
	
	unsigned long  rcasScreenShareEventParaErr;
	unsigned long  rcasScreenShareEventStateErr;
	unsigned long  rcasScreenShareEventLenErr;
	unsigned long  rcasScreenShareEventTellerIdErr;	
	unsigned long  rcasScreenShareEventSrvGrpIdErr;		
	unsigned long  rcasScreenShareEventCallIdErr;
	unsigned long  rcasScreenShareEventVtmIdErr;
	unsigned long  rcasScreenShareEventVtmStateErr;	
	unsigned long  rcasScreenShareEventTypeErr;
	unsigned long  rcasScreenShareEventRcasReportFail;

	unsigned long  vtmRemoteCooperativeParaErr;
	unsigned long  vtmRemoteCooperativeStateErr;
	unsigned long  vtmRemoteCooperativeLenErr;
	unsigned long  vtmRemoteCooperativeAmsPidErr;
	unsigned long  vtmRemoteCooperativeTellerIdErr;
	unsigned long  vtmRemoteCooperativeVtmIdErr;
	unsigned long  vtmRemoteCooperativeVtmStateErr;
	unsigned long  vtmRemoteCooperativeOpTypeErr;
	unsigned long  vtmRemoteCooperativeOpStateErr;
	unsigned long  vtmRemoteCooperativeRcasOpStateErr;

	unsigned long  vtmRemoteCoopTellerPosErr;
	unsigned long  vtmRemoteCoopTellerIdNotMatch;
	unsigned long  vtmRemoteCoopVtmPosErr; 
	unsigned long  vtmRemoteCoopVtmIdNotMatch;
	unsigned long  vtmRemoteCoopRcasTransProtoParaErr;			
	unsigned long  vtmRemoteCoopVtmTransProtoParaErr;
	unsigned long  vtmRemoteCoopVtmSecuritiesParaErr;	
	unsigned long  vtmRemoteCoopVtmVncAuthParaErr;
	unsigned long  vtmRemoteCoopVtaSecuritiesParaErr;
	unsigned long  vtmRemoteCoopVtaVncAuthParaErr;

	unsigned long  vtmRemoteCoopSelectRcasModuleFail;
	unsigned long  vtmRemoteCoopStartTimerToRcasErr;
	unsigned long  vtmRemoteCoopVtmReportFail;
	
	//Vta Snap Result
	unsigned long  vtaSnapSuccess;
	unsigned long  vtaSnapParaErr;
	unsigned long  vtaSnapStateErr;
	unsigned long  vtaSnapLenErr;
	unsigned long  vtaSnapAmsPidErr;
	unsigned long  vtaSnapTellerIdErr;
	unsigned long  vtaSnapSrvGrpIdErr;
	unsigned long  vtaSnapServiceStateErr;
	unsigned long  vtaSnapCallIdErr;
	unsigned long  vtaSnapCallStateErr;
	unsigned long  vtaSnapVtmIdErr;
	unsigned long  vtaSnapVtmStateErr;
	unsigned long  vtaSnapTypeErr; 
	unsigned long  vtaSnapFileTypeErr;
	unsigned long  vtaSnapFileNameErr;
	unsigned long  vtaSnapFilePathErr;
	unsigned long  vtaSnapStartTimerErr;
	unsigned long  vtaSnapTimeout;

	unsigned long  vtaSnapTimeoutParaErr;
	unsigned long  vtaSnapTimeoutStateErr;
	unsigned long  vtaSnapTimeoutLenErr;
	unsigned long  vtaSnapTimeoutAmsPidErr;
	unsigned long  vtaSnapTimeoutTellerIdErr;
	unsigned long  vtaSnapTimeoutTellerStateErr;
	unsigned long  vtaSnapTimeoutSrvGrpIdErr;
	unsigned long  vtaSnapTimeoutServiceStateErr;
	unsigned long  vtaSnapTimeoutCallIdErr;
	unsigned long  vtaSnapTimeoutTypeErr;

	//Vtm Snap Result
	unsigned long  vtmSnapParaErr;
	unsigned long  vtmSnapStateErr;
	unsigned long  vtmSnapLenErr;
	unsigned long  vtmSnapAmsPidErr;
	unsigned long  vtmSnapTellerIdErr;
	unsigned long  vtmSnapVtmIdErr;
	unsigned long  vtmSnapTypeErr;
	unsigned long  vtmSnapVtmReportFail;

	//Vta Msg Operate Result
	unsigned long  vtaSendMsgSuccess;
	unsigned long  vtaSendMsgParaErr;
	unsigned long  vtaSendMsgStateErr;
	unsigned long  vtaSendMsgLenErr;
	unsigned long  vtaSendMsgAmsPidErr;
	unsigned long  vtaSendMsgTellerIdErr;
	unsigned long  vtaSendMsgSrvGrpIdErr;
	unsigned long  vtaSendMsgServiceStateErr;
	unsigned long  vtaSendMsgTargetTypeErr;
	unsigned long  vtaSendMsgTargetTellerIdErr;
	unsigned long  vtaSendMsgTargetTellerStateErr;
	unsigned long  vtaSendMsgTargetTellerAmsPidErr;
	unsigned long  vtaSendMsgTargetVtmIdErr;
	unsigned long  vtaSendMsgTargetVtmStateErr;
	unsigned long  vtaSendMsgMsgLenErr;
	unsigned long  vtaSendMsgMsgSendStateErr;
	unsigned long  vtaSendMsgStartTimerErr;

	unsigned long  vtaRecvMsgSuccess;
	unsigned long  vtaRecvMsgParaErr;
	unsigned long  vtaRecvMsgStateErr;
	unsigned long  vtaRecvMsgLenErr;
	unsigned long  vtaRecvMsgAmsPidErr;
	unsigned long  vtaRecvMsgTellerIdErr;
	unsigned long  vtaRecvMsgSrvGrpIdErr;
	unsigned long  vtaRecvMsgServiceStateErr;
	unsigned long  vtaRecvMsgCallIdErr; 
	unsigned long  vtaRecvMsgOrgnTypeErr;
	unsigned long  vtaRecvMsgOrgnTellerIdErr;
	unsigned long  vtaRecvMsgOrgnTellerStateErr; 
	unsigned long  vtaRecvMsgOrgnTellerAmsPidErr;
	unsigned long  vtaRecvMsgOrgnTellerSrvGrpIdErr;
	unsigned long  vtaRecvMsgOrgnTellerSrvStErr;
	unsigned long  vtaRecvMsgOrgnVtmIdErr; 
	unsigned long  vtaRecvMsgOrgnVtmStateErr;
	unsigned long  vtaRecvMsgMsgRecvState;
	unsigned long  vtaRecvMsgVtaReportFail;
	
	unsigned long  vtaRecvMsgTimeoutParaErr;
	unsigned long  vtaRecvMsgTimeoutLenErr;
	unsigned long  vtaRecvMsgTimeoutSrvGrpIdErr;
	unsigned long  vtaRecvMsgTimeoutSrvStErr;
	unsigned long  vtaRecvMsgTimeoutOrgnTypeErr;
	unsigned long  vtaRecvMsgTimeoutOrgnTellerIdErr;
	unsigned long  vtaRecvMsgTimeoutOrgnTellerStErr;
	unsigned long  vtaRecvMsgTimeoutOrgnTellerAmsPidErr;
	unsigned long  vtaRecvMsgTimeoutOrgnVtmIdErr;
	unsigned long  vtmRecvMsgTimeoutOrgnVtmStErr;
	
	//Vta File Operate Result
	unsigned long  vtaSendFileSuccess;
	unsigned long  vtaSendFileParaErr; 
	unsigned long  vtaSendFileStateErr;
	unsigned long  vtaSendFileLenErr;
	unsigned long  vtaSendFileAmsPidErr;
	unsigned long  vtaSendFileTellerIdErr; 
	unsigned long  vtaSendFileSrvGrpIdErr;
	unsigned long  vtaSendFileServiceStateErr;
	unsigned long  vtaSendFileTargetTypeErr;
	unsigned long  vtaSendFileTargetTellerIdErr;
	unsigned long  vtaSendFileTargetTellerStateErr;
	unsigned long  vtaSendFileTargetTellerAmsPidErr;
	unsigned long  vtaSendFileTargetVtmIdErr;
	unsigned long  vtaSendFileTargetVtmStateErr;
	unsigned long  vtaSendFileOpTypeErr;
	unsigned long  vtaSendFileFileTypeErr;
	unsigned long  vtaSendFileFilePathErr;
	unsigned long  vtaSendFileFileNameErr;
	unsigned long  vtaSendFileStartTimerErr;
	unsigned long  vtaSendFileFileSendStateErr;

	unsigned long  vtaRecvFileSuccess;
	unsigned long  vtaRecvFileParaErr;
	unsigned long  vtaRecvFileStateErr;
	unsigned long  vtaRecvFileLenErr;
	unsigned long  vtaRecvFileAmsPidErr;
	unsigned long  vtaRecvFileTellerIdErr;
	unsigned long  vtaRecvFileSrvGrpIdErr;
	unsigned long  vtaRecvFileServiceStateErr;
	unsigned long  vtaRecvFileCallIdErr;
	unsigned long  vtaRecvFileOrgnTypeErr;
	unsigned long  vtaRecvFileOrgnTellerIdErr; 
	unsigned long  vtaRecvFileOrgnTellerStateErr;
	unsigned long  vtaRecvFileOrgnTellerAmsPidErr;
	unsigned long  vtaRecvFileOrgnTellerSrvGrpIdErr;
	unsigned long  vtaRecvFileOrgnTellerSrvStErr;
	unsigned long  vtaRecvFileOrgnVtmIdErr;
	unsigned long  vtaRecvFileOrgnVtmStateErr;
	unsigned long  vtaRecvFileFileRecvOpType;
	unsigned long  vtaRecvFileFileSrvTypeErr;
	unsigned long  vtaRecvFileFileRecvStateErr;
	unsigned long  vtaRecvSnapFileStateErr;
	unsigned long  vtaRecvFileVtaReportFail;
	unsigned long  vtaRecvSnapFileVtaReportFail;
	unsigned long  vtaRecvFileTimeout; 
	
	unsigned long  vtaRecvFileTimeoutParaErr;
	unsigned long  vtaRecvFileTimeoutLenErr;
	unsigned long  vtaRecvFileTimeoutSrvGrpIdErr;
	unsigned long  vtaRecvFileTimeoutSrvStErr;
	unsigned long  vtaRecvFileTimeoutFileRecvOpTypeErr;
	unsigned long  vtaRecvFileTimeoutFileSrvTypeErr;
	unsigned long  vtaRecvFileTimeoutOrgnTypeErr;
	unsigned long  vtaRecvFileTimeoutOrgnTellerIdErr;
	unsigned long  vtaRecvFileTimeoutOrgnTellerStErr;
	unsigned long  vtaRecvFileTimeoutOrgnTellerAmsPidErr;
	unsigned long  vtaRecvFileTimeoutOrgnVtmIdErr;
	unsigned long  vtaRecvFileTimeoutOrgnVtmStateErr;	
	
	//Multi Sess Result
	unsigned long  vtaMultiSessSuccess;
	unsigned long  vtaMultiSessParaErr;
	unsigned long  vtaMultiSessStateErr;
	unsigned long  vtaMultiSessLenErr;
	unsigned long  vtaMultiSessAmsPidErr;
	unsigned long  vtaMultiSessTellerIdErr;
	unsigned long  vtaMultiSessSrvGrpIdErr;
	unsigned long  vtaMultiSessServiceStateErr;
	unsigned long  vtaMultiSessCallIdErr;
	unsigned long  vtaMultiSessCallStateErr;
	unsigned long  vtaMultiSessVtmIdErr;
	unsigned long  vtaMultiSessVtmStateErr;
	unsigned long  vtaMultiSessTargetTypeErr;
	unsigned long  vtaMultiSessTargetTellerIdErr;
	unsigned long  vtaMultiSessTargetTellerStateErr;
	unsigned long  vtaMultiSessTargetTellerAmsPidErr;
	unsigned long  vtaMultiSessOpTypeErr;
	unsigned long  vtaMultiSessOpStateErr;
	unsigned long  vtaMultiSessStartTimerErr;
	unsigned long  vtaMultiSessTimeout;
	
	unsigned long  vtaMultiSessTimeoutParaErr;
	unsigned long  vtaMultiSessTimeoutLenErr;
	unsigned long  vtaMultiSessTimeoutTellerIdErr;
	unsigned long  vtaMultiSessTimeoutTellerStateErr;
	unsigned long  vtaMultiSessTimeoutSrvGrpIdErr;
	unsigned long  vtaMultiSessTimeoutServiceStateErr;
	unsigned long  vtaMultiSessTimeoutOpTypeErr;
	unsigned long  vtaMultiSessTimeoutTrgtTypeErr;
	unsigned long  vtmMultiSessTimeoutTrgtTlrIdErr;
	unsigned long  vtmMultiSessTimeoutTrgtTlrStateErr;
	unsigned long  vtmMultiSessTimeoutTrgtTlrAmsPidErr;
	unsigned long  vtmMultiSessTimeoutTrgtVtmIdErr;
	unsigned long  vtmMultiSessTimeoutTrgtVtmStateErr;

	unsigned long  vtaMultiSessIndCnfParaErr;
	unsigned long  vtaMultiSessIndCnfStateErr; 
	unsigned long  vtaMultiSessIndCnfLenErr;
	unsigned long  vtaMultiSessIndCnfAmsPidErr; 
	unsigned long  vtaMultiSessIndCnfTellerIdErr;
	unsigned long  vtaMultiSessIndCnfSrvGrpIdErr;
	unsigned long  vtaMultiSessIndCnfSrvStErr; 
	unsigned long  vtaMultiSessIndCnfCallIdErr;
	unsigned long  vtaMultiSessIndCnfCallStateErr;
	unsigned long  vtaMultiSessIndCnfOrgnTypeErr;
	unsigned long  vtaMultiSessIndCnfOrgnTlrIdErr;
	unsigned long  vtaMultiSessIndCnfOrgnTlrStErr;
	unsigned long  vtaMultiSessIndCnfOrgnTlrAmsPidErr;
	unsigned long  vtaMultiSessIndCnfOrgnTlrSrvGrpIdErr; 
	unsigned long  vtaMultiSessIndCnfOrgnTlrSrvStErr;
	unsigned long  vtaMultiSessIndCnfOrgnVtmIdErr; 
	unsigned long  vtaMultiSessIndCnfOrgnVtmStateErr; 
	unsigned long  vtaMultiSessIndCnfOpTypeErr;
	unsigned long  vtaMultiSessIndCnfOpStateErr;
	unsigned long  vtaMultiSessIndCnfVtaReportFail;

	//Vta Force Login Result
	unsigned long  vtaForceLoginSuccess;
	unsigned long  vtaForceLoginParaErr;
	unsigned long  vtaForceLoginStateErr;
	unsigned long  vtaForceLoginLenErr;
	unsigned long  vtaForceLoginLpResourceLimited;
	unsigned long  vtaForceLoginNodeResourceLimited;
	unsigned long  vtaForceLoginTellerLenErr;
	unsigned long  vtaForceLoginTellerNoErr;
	unsigned long  vtaForceLoginTellerPwdErr;
	unsigned long  vtaForceLoginTermNetInfoErr;
	unsigned long  vtaForceLoginTellerVncAuthPwdErr;
	unsigned long  vtaForceLoginTellerLoginRepeatedly;
	unsigned long  vtaForceLoginTellerTypeErr;
	unsigned long  vtaForceLoginTellerNumErr;
	unsigned long  vtaForceLoginFileServerUsrNameErr;
	unsigned long  vtaForceLoginFileServerUsrPwdErr;
	unsigned long  vtaForceLoginLicenseTimeout;
	unsigned long  vtaForceLoginTellerNumBeyondLic;

	//Vta Query Info Result	
	unsigned long  vtaQueryInfoSuccess;
	unsigned long  vtaQueryInfoParaErr;
	unsigned long  vtaQueryInfoStateErr;
	unsigned long  vtaQueryInfoLenErr;
	unsigned long  vtaQueryInfoAmsPidErr;
	unsigned long  vtaQueryInfoTellerIdErr;
	unsigned long  vtaQueryInfoSrvGrpIdErr;
	unsigned long  vtaQueryInfoServiceStateErr;
	unsigned long  vtaQueryInfoCodeErr;
	unsigned long  vtaQueryInfoTargetSrvIdErr;
	unsigned long  vtaQueryInfoTargetSrvNameLenErr;
	unsigned long  vtaQueryInfoTargetSrvGrpIdErr;
	unsigned long  vtaQueryInfoTargetTellerNoLenErr;
	unsigned long  vtaQueryInfoTargetTellerNoErr;
	unsigned long  vtaQueryInfoTellerNoKeyLenErr;
	unsigned long  vtaQueryInfoTellerNameKeyLenErr; 

	//Manager Add Vta Result
	unsigned long  managerAddVtaSuccess;
	unsigned long  managerAddVtaParaErr;
	unsigned long  managerAddVtaStateErr;
	unsigned long  managerAddVtaSrvGrpIdErr;
	unsigned long  managerAddVtaServiceStateErr;
	unsigned long  managerAddVtaLenErr;
	unsigned long  managerAddVtaAmsPidErr;
	unsigned long  managerAddVtaTellerIdErr;
	unsigned long  managerAddVtaAddTellerIdErr; 
	unsigned long  managerAddVtaAddTellerNoErr;
	unsigned long  managerAddVtaAddTellerPwdErr;
	unsigned long  managerAddVtaAddTellerTypeErr;
	unsigned long  managerAddVtaResourceLimited;
	unsigned long  managerAddVtaTellerNumErr;
	
	//Manager Del Vta Result
	unsigned long  managerDelVtaSuccess;
	unsigned long  managerDelVtaParaErr;
	unsigned long  managerDelVtaStateErr;
	unsigned long  managerDelVtaSrvGrpIdErr;
	unsigned long  managerDelVtaServiceStateErr;
	unsigned long  managerDelVtaLenErr;
	unsigned long  managerDelVtaAmsPidErr;
	unsigned long  managerDelVtaTellerIdErr;
	unsigned long  managerDelVtaDelTellerIdErr;
	unsigned long  managerDelVtaDelTellerStateErr;
	unsigned long  managerDelVtaDelTellerUpdateStateErr;
	
	//Manager Modify Vta Result
	unsigned long  managerModifyVtaSuccess;
	unsigned long  managerModifyVtaParaErr;
	unsigned long  managerModifyVtaStateErr;
	unsigned long  managerModifyVtaSrvGrpIdErr;
	unsigned long  managerModifyVtaServiceStateErr;
	unsigned long  managerModifyVtaLenErr;
	unsigned long  managerModifyVtaAmsPidErr;
	unsigned long  managerModifyVtaTellerIdErr;
	unsigned long  managerModifyVtaModifyTellerIdErr; 
	unsigned long  managerModifyVtaModifyTellerNoErr;
	unsigned long  managerModifyVtaModifyTellerPwdErr;
	unsigned long  managerModifyVtaModifyTellerTypeErr; 
	unsigned long  managerModifyVtaModifyTellerStateErr;

	//Manager Query Vta Result
	unsigned long  managerQueryVtaSuccess;
	unsigned long  managerQueryVtaParaErr;
	unsigned long  managerQueryVtaStateErr;
	unsigned long  managerQueryVtaSrvGrpIdErr;
	unsigned long  managerQueryVtaServiceStateErr;
	unsigned long  managerQueryVtaLenErr;
	unsigned long  managerQueryVtaAmsPidErr; 
	unsigned long  managerQueryVtaTellerIdErr;
	unsigned long  managerQueryVtaQueryTellerIdErr;
	unsigned long  managerQueryVtaQueryTellerNameErr;
	unsigned long  managerQueryVtaQueryTellerPwdErr;
	unsigned long  managerQueryVtaQueryTellerTypeErr;

	//Manager Force Logout Vta Result
	unsigned long  managerForceLogoutVtaSuccess;
	unsigned long  managerForceLogoutVtaParaErr;
	unsigned long  managerForceLogoutVtaStateErr;
	unsigned long  managerForceLogoutVtaSrvGrpIdErr;
	unsigned long  managerForceLogoutVtaServiceStateErr;
	unsigned long  managerForceLogoutVtaLenErr;
	unsigned long  managerForceLogoutVtaAmsPidErr;
	unsigned long  managerForceLogoutVtaTellerIdErr;
	unsigned long  managerForceLogoutVtaTellerTypeErr;	
	unsigned long  managerForceLogoutVtaTargetTellerIdErr; 
	unsigned long  managerForceLogoutVtaTargetTellerStateErr;
	unsigned long  managerForceLogoutVtaTargetTellerSrvGrpIdErr;
	unsigned long  managerForceLogoutVtaUpdateStateErr;

	//Manager Set Vta State Result
	unsigned long  managerSetVtaStateSuccess;
	unsigned long  managerSetVtaStateParaErr;
	unsigned long  managerSetVtaStateStateErr;
	unsigned long  managerSetVtaStateLenErr;
	unsigned long  managerSetVtaStateAmsPidErr;
	unsigned long  managerSetVtaStateTellerIdErr;
	unsigned long  managerSetVtaStateTellerTypeErr;
	unsigned long  managerSetVtaStateSrvGrpIdErr;
	unsigned long  managerSetVtaStateServiceStateErr;
	unsigned long  managerSetVtaStateStSetStateErr;	
	unsigned long  managerSetVtaStateStartTimerErr;
	unsigned long  managerSetVtaStateSetTellerAmsPidErr;
	unsigned long  managerSetVtaStateSetTellerIdErr;
	unsigned long  managerSetVtaStateSetTellerSrvGrpIdErr; 
	unsigned long  managerSetVtaStateSetTellerSrvStErr;
	unsigned long  managerSetVtaStateSetTellerStateErr;
	unsigned long  managerSetVtaStateGetOperateIndErr;
	unsigned long  managerSetVtaStateGetStateErr;
	unsigned long  managerSetVtaStateUpdateStateErr;
	unsigned long  managerSetVtaStateVtaReportFail;
	unsigned long  managerSetVtaStateVtaTimeout;	
	
	//--Vta State Op Ind Cnf Result--
	unsigned long  vtaStateOperateIndCnfParaErr;
	unsigned long  vtaStateOperateIndCnfStateErr;
	unsigned long  vtaStateOperateIndCnfLenErr;
	unsigned long  vtaStateOperateIndCnfAmsPidErr;
	unsigned long  vtaStateOperateIndCnfTellerIdErr;
	unsigned long  vtaStateOperateIndCnfSrvGrpIdErr; 
	unsigned long  vtaStateOperateIndCnfServiceStateErr;
	unsigned long  vtaStateOperateIndCnfManagerAmsPidErr;
	unsigned long  vtaStateOperateIndCnfManagerTellerIdErr;
	unsigned long  vtaStateOperateIndCnfManagerSrvGrpIdErr;
	unsigned long  vtaStateOperateIndCnfManagerSrvStErr;	
	unsigned long  vtaStateOperateIndCnfCodeErr;
	
	unsigned long  managerSetVtaStateTimeoutParaErr;
	unsigned long  managerSetVtaStateTimeoutStateErr;
	unsigned long  managerSetVtaStateTimeoutLenErr;
	unsigned long  managerSetVtaStateTimeoutAmsPidErr;	
	unsigned long  managerSetVtaStateTimeoutTellerIdErr;
	unsigned long  managerSetVtaStateTimeoutSrvGrpIdErr;
	unsigned long  managerSetVtaStateTimeoutServiceStateErr;
	unsigned long  managerSetVtaStateTimeoutStSetStateErr;

	//Manager Query Vta State Result
	unsigned long  managerQueryVtaStateSuccess;
	unsigned long  managerQueryVtaStateParaErr;
	unsigned long  managerQueryVtaStateStateErr;
	unsigned long  managerQueryVtaStateLenErr;
	unsigned long  managerQueryVtaStateAmsPidErr;
	unsigned long  managerQueryVtaStateTellerIdErr;
	unsigned long  managerQueryVtaStateTellerTypeErr;
	unsigned long  managerQueryVtaStateSrvGrpIdErr;
	unsigned long  managerQueryVtaStateServiceStateErr;
	unsigned long  managerQueryVtaStateStartTimerErr;
	unsigned long  managerQueryVtaStateQueryTellerAmsPidErr;
	unsigned long  managerQueryVtaStateQueryTellerIdErr;
	unsigned long  managerQueryVtaStateQueryTellerSrvGrpIdErr;
	unsigned long  managerQueryVtaStateQueryTellerSrvStErr;
	unsigned long  managerQueryVtaStateQueryTellerStateErr;

	//Manager Modify Queue Rule Result
	unsigned long  managerModifyQueueRuleSuccess;
	unsigned long  managerModifyQueueRuleParaErr;
	unsigned long  managerModifyQueueRuleStateErr;
	unsigned long  managerModifyQueueRuleLenErr;
	unsigned long  managerModifyQueueRuleAmsPidErr;
	unsigned long  managerModifyQueueRuleTellerIdErr;
	unsigned long  managerModifyQueueRuleSrvGrpIdErr;
	unsigned long  managerModifyQueueRuleServiceStateErr;
	unsigned long  managerModifyQueueRuleStartTimerErr;	 
	unsigned long  managerModifyQueueRuleModifyVtmAmsPidErr;
	unsigned long  managerModifyQueueRuleModifyVtmIdErr;
	unsigned long  managerModifyQueueRuleModifyVtmSrvGrpIdErr;
	unsigned long  managerModifyQueueRuleModifyVtmServiceStateErr;
	unsigned long  managerModifyQueueRuleModifyVtmStateErr;
	unsigned long  managerModifyQueueRuleModifyVtmSequenceErr;
	unsigned long  managerModifyQueueRuleTimeout;	
	
	//Manager Force Rel Call Vta Result
	unsigned long  managerForceRelCallSuccess;
	unsigned long  managerForceRelCallParaErr;
	unsigned long  managerForceRelCallStateErr;
	unsigned long  managerForceRelCallLenErr; 
	unsigned long  managerForceRelCallAmsPidErr;
	unsigned long  managerForceRelCallTellerIdErr;
	unsigned long  managerForceRelCallTellerTypeErr;
	unsigned long  managerForceRelCallSrvGrpIdErr;
	unsigned long  managerForceRelCallServiceStateErr;
	unsigned long  managerForceRelCallTrgtTlrIdErr;
	unsigned long  managerForceRelCallTrgtTlrStErr;
	unsigned long  managerForceRelCallTrgtTlrAmsPidErr;
	unsigned long  managerForceRelCallTrgtTlrSrvGrpIdErr;
	unsigned long  managerForceRelCallTrgtTlrSrvStErr;
	unsigned long  managerForceRelCallStartTimerErr;

	//Inspector Monitor Vta Result
	unsigned long  inspMonitorSuccess;
	unsigned long  inspMonitorParaErr;
	unsigned long  inspMonitorStateErr;
	unsigned long  inspMonitorLenErr;
	unsigned long  inspMonitorAmsPidErr;
	unsigned long  inspMonitorTellerIdErr; 
	unsigned long  inspMonitorTellerTypeErr;	
	unsigned long  inspMonitorSrvGrpIdErr;
	unsigned long  inspMonitorServiceStateErr;
	unsigned long  inspMonitorTargetTypeErr;
	unsigned long  inspMonitorTargetTellerIdErr;
	unsigned long  inspMonitorTargetTellerStateErr;
	unsigned long  inspMonitorVtaMonitorTellerAmsPidErr;
	unsigned long  inspMonitorVtaMonitorTellerSrvGrpIdErr;
	unsigned long  inspMonitorVtaMonitorTellerSrvStErr;
	unsigned long  inspMonitorTargetVtmIdErr;
	unsigned long  inspMonitorTargetVtmStateErr;
	unsigned long  inspMonitorVtaTypeErr;	
	unsigned long  inspMonitorVtaMonitorTellerOpStErr;	
	unsigned long  inspMonitorStartTimerErr;
	unsigned long  inspMonitorVtaReportMonitorFail;
	unsigned long  inspMonitorVtaRpForceInsertCallFail;
	unsigned long  inspMonitorTimeout;
	
	unsigned long  inspMonitorTimeoutParaErr;
	unsigned long  inspMonitorTimeoutLenErr;
	unsigned long  inspMonitorTimeoutTellerIdErr; 
	unsigned long  inspMonitorTimeoutTellerStErr;
	unsigned long  inspMonitorTimeoutSrvGrpIdErr;
	unsigned long  inspMonitorTimeoutSrvStErr;
	unsigned long  inspMonitorTimeoutOpTypeErr;
	unsigned long  inspMonitorTimeoutTrgtTypeErr;
	unsigned long  inspMonitorTimeoutTrgtTlrIdErr;
	unsigned long  inspMonitorTimeoutTrgtTlrStErr; 
	unsigned long  inspMonitorTimeoutTrgtTlrAmsPidErr;
	
	unsigned long  inspMonitorIndCnfParaErr; 
	unsigned long  inspMonitorIndCnfStateErr;
	unsigned long  inspMonitorIndCnfLenErr;
	unsigned long  inspMonitorIndCnfAmsPidErr;
	unsigned long  inspMonitorIndCnfTellerIdErr;
	unsigned long  inspMonitorIndCnfSrvGrpIdErr; 
	unsigned long  inspMonitorIndCnfSrvStErr;
	unsigned long  inspMonitorIndCnfCallIdErr;
	unsigned long  inspMonitorIndCnfCallStateErr;
	unsigned long  inspMonitorIndCnfOrgnTypeErr;
	unsigned long  inspMonitorIndCnfOrgnTlrIdErr;
	unsigned long  inspMonitorIndCnfOrgnTlrStErr;
	unsigned long  inspMonitorIndCnfOrgnTlrAmsPidErr;
	unsigned long  inspMonitorIndCnfOrgnTlrSrvGrpIdErr; 
	unsigned long  inspMonitorIndCnfOrgnTlrSrvStErr;
	unsigned long  inspMonitorIndCnfOpTypeErr;
	unsigned long  inspMonitorIndCnfOpStErr;

	//Vta Bandwidth Notice Result
	unsigned long  vtaBandwidthNoticeSuccess;
	unsigned long  vtaBandwidthNoticeParaErr;
	unsigned long  vtaBandwidthNoticeStateErr;
	unsigned long  vtaBandwidthNoticeLenErr;
	unsigned long  vtaBandwidthNoticeTellerIdErr;
	unsigned long  vtaBandwidthNoticeCallIdErr;
	unsigned long  vtaBandwidthNoticeBwParaErr;

	//Vta Event Notice Result
	unsigned long  vtaEventNoticeSuccess;
	unsigned long  vtaEventNoticeParaErr;
	unsigned long  vtaEventNoticeStateErr;
	unsigned long  vtaEventNoticeLenErr;
	unsigned long  vtaEventNoticeSrvGrpIdErr;
	unsigned long  vtaEventNoticeServiceStateErr;
	unsigned long  vtaEventNoticeCallIdErr;
	unsigned long  vtaEventNoticeCallStateErr;
	unsigned long  vtaEventNoticeAmsPidErr;
	unsigned long  vtaEventNoticeTellerIdErr;
	unsigned long  vtaEventNoticeVtmIdErr;
	unsigned long  vtaEventNoticeTypeErr;
	unsigned long  vtaEventNoticeLackFileInfo;
	unsigned long  vtaEventNoticeFilePathLenErr;
	unsigned long  vtaEventNoticeFileNameLenErr;

	unsigned long  vtaEvntNtceSendFileTrgtTypeErr;
	unsigned long  vtaEvntNtceSendFileTrgtTlrIdErr;
	unsigned long  vtaEvntNtceSendFileTrgtTlrStErr;
	unsigned long  vtaEvntNtceSendFileTrgtTlrAmsPidErr;
	unsigned long  vtaEvntNtceSendFileTrgtVtmIdErr;
	unsigned long  vtaEvntNtceSendFileTrgtVtmStErr;
	unsigned long  vtaEvntNtceSendFileSendStErr;
	unsigned long  vtaEvntNtceSendFileStartTimerErr;
	
	unsigned long  vtaEvntNtceRecvFileParaErr;
	unsigned long  vtaEvntNtceRecvFileOrgnTypeErr;
	unsigned long  vtaEvntNtceRecvFileOrgnTlrIdErr;
	unsigned long  vtaEvntNtceRecvFileOrgnTlrAmsPidErr;
	unsigned long  vtaEvntNtceRecvFileOrgnTlrSrvGrpIdErr;
	unsigned long  vtaEvntNtceRecvFileOrgnTlrSrvStErr;
	unsigned long  vtaEvntNtceRecvFileOrgnTlrStErr;
	unsigned long  vtaEvntNtceRecvFileOrgnVtmIdErr; 
	unsigned long  vtaEvntNtceRecvFileOrgnVtmStErr;
	unsigned long  vtaEvntNtceRecvFileRecvStErr;

	unsigned long  vtaEvntCancRecvFileParaErr;
	unsigned long  vtaEvntCancRecvFileOrgnTypeErr;
	unsigned long  vtaEvntCancRecvFileOrgnTlrIdErr;
	unsigned long  vtaEvntCancRecvFileOrgnTlrAmsPidErr;
	unsigned long  vtaEvntCancRecvFileOrgnTlrSrvGrpIdErr;
	unsigned long  vtaEvntCancRecvFileOrgnTlrSrvStErr;
	unsigned long  vtaEvntCancRecvFileOrgnTlrStErr;
	unsigned long  vtaEvntCancRecvFileOrgnVtmIdErr; 
	unsigned long  vtaEvntCancRecvFileOrgnVtmStErr;
	unsigned long  vtaEvntCancRecvFileRecvStErr;
	
	unsigned long  vtaEvntNtceAudioRecVtaRepFail;
	unsigned long  vtaEvntNtceUpldAudioFileVtaRepFail;
	unsigned long  vtaEvntNtceScrRecVtaRepFail;
	unsigned long  vtaEvntNtceUpldScrRecFileVtaRepFail;
	unsigned long  vtaEvntNtceSnapVtaRepFail;
	unsigned long  vtaEvntNtceDownldSnapFileVtaRepFail;
	unsigned long  vtaEvntNtceUpldFileVtaRepFail;
	unsigned long  vtaEvntNtceDownldFileVtaRepFail;
	unsigned long  vtaEvntNtceVtaCancRecvFileRepFail;
	unsigned long  vtaEvntNtceRemCoopVtaRepFail;
	

	//Vta Para Cfg Result
	unsigned long  vtaParaCfgSuccess;
	unsigned long  vtaParaCfgIndCnfParaErr;
	unsigned long  vtaParaCfgIndCnfStateErr;
	unsigned long  vtaParaCfgIndCnfLenErr;
	unsigned long  vtaParaCfgIndCnfTellerIdErr;
	unsigned long  vtaParaCfgIndCnfSrvGrpIdErr;
	unsigned long  vtaParaCfgIndCnfSrvStErr;
	unsigned long  vtaParaCfgIndCnfOpTypeErr;
	unsigned long  vtaParaCfgIndCnfOpStateErr;
	unsigned long  vtaParaCfgIndCnfTermCfgParaErr;
	unsigned long  vtaParaCfgIndCnfVtaReportFail;
	unsigned long  vtaParaCfgIndCnfTimeout;
	unsigned long  vtaParaCfgTimeoutParaErr;
	unsigned long  vtaParaCfgTimeoutLenErr;
	unsigned long  vtaParaCfgTimeoutTellerIdErr;
	unsigned long  vtaParaCfgTimeoutTellerStateErr;
	unsigned long  vtaParaCfgTimeoutSrvGrpIdErr;
	unsigned long  vtaParaCfgTimeoutServiceStateErr;
	unsigned long  vtaParaCfgTimeoutOpTypeErr;

	//Vtm Login Result
	unsigned long  vtmLoginSuccess;
	unsigned long  vtmLoginParaErr;
	unsigned long  vtmLoginStateErr; 
	unsigned long  vtmLoginNodeResourceLimited;
	unsigned long  vtmLoginLenErr;
	unsigned long  vtmLoginVtmNoErr;
	unsigned long  vtmLoginPwdErr;
	unsigned long  vtmLoginVncAuthPwdErr;
	unsigned long  vtmLoginLoginRepeatedlyErr;
	unsigned long  vtmLoginVtmTypeErr;
	unsigned long  vtmLoginVtmNumErr;
	unsigned long  vtmLoginFileServerUsrNameErr;
	unsigned long  vtmLoginFileServerUsrPwdErr;
	unsigned long  vtmLoginOrgIdErr;
	unsigned long  vtmLoginOrgStateErr;	
	unsigned long  vtmLoginLicenseTimeout;
	unsigned long  vtmLoginVtmNumBeyondLic;
	
	//Vtm Logout Result
	unsigned long  vtmLogoutSuccess;
	unsigned long  vtmLogoutParaErr;
	unsigned long  vtmLogoutStateErr;
	unsigned long  vtmLogoutLenErr;
	unsigned long  vtmLogoutVtmIdErr; 

	//Vtm Handshake Result
	unsigned long  vtmHandshakeSuccess;
	unsigned long  vtmHandshakeParaErr;
	unsigned long  vtmHandshakeStateErr;
	unsigned long  vtmHandshakeLenErr;
	unsigned long  vtmHandshakeVtmIdErr;
	unsigned long  vtmClearInactiveVtm;

	//Vtm Msg Operate Result
	unsigned long  vtmSendMsgSuccess; 
	unsigned long  vtmSendMsgParaErr;
	unsigned long  vtmSendMsgStateErr;
	unsigned long  vtmSendMsgLenErr;
	unsigned long  vtmSendMsgVtmIdErr;
	unsigned long  vtmSendMsgTargetTypeErr;
	unsigned long  vtmSendMsgTargetTellerIdErr;
	unsigned long  vtmSendMsgTargetTellerStateErr;
	unsigned long  vtmSendMsgTargetTellerAmsPidErr;
	unsigned long  vtmSendMsgTargetVtmIdErr;
	unsigned long  vtmSendMsgTargetVtmStateErr;
	unsigned long  vtmSendMsgMsgLenErr;
	unsigned long  vtmSendMsgMsgSendStateErr; 
	unsigned long  vtmSendMsgStartTimerErr;
	
	unsigned long  vtmRecvMsgSuccess;
	unsigned long  vtmRecvMsgParaErr;
	unsigned long  vtmRecvMsgStateErr;
	unsigned long  vtmRecvMsgLenErr;
	unsigned long  vtmRecvMsgVtmIdErr;
	unsigned long  vtmRecvMsgOrgnTypeErr;
	unsigned long  vtmRecvMsgOrgnTellerIdErr;
	unsigned long  vtmRecvMsgOrgnTellerStateErr;
	unsigned long  vtmRecvMsgOrgnTellerAmsPidErr;
	unsigned long  vtmRecvMsgOrgnTellerSrvGrpIdErr;
	unsigned long  vtmRecvMsgOrgnTellerSrvStErr;
	unsigned long  vtmRecvMsgOrgnVtmIdErr;
	unsigned long  vtmRecvMsgOrgnVtmStateErr;
	unsigned long  vtmRecvMsgMsgRecvStateErr;
	unsigned long  vtmRecvMsgVtmReportFail;
	unsigned long  vtmRecvMsgTimeout;
	
	unsigned long  vtmRecvMsgTimeoutParaErr;
	unsigned long  vtmRecvMsgTimeoutLenErr;
	unsigned long  vtmRecvMsgTimeoutOrgnTypeErr;
	unsigned long  vtmRecvMsgTimeoutOrgnTlrIdErr;
	unsigned long  vtmRecvMsgTimeoutOrgnTlrStErr;
	unsigned long  vtmRecvMsgTimeoutOrgnTlrAmsPidErr;
	unsigned long  vtmRecvMsgTimeoutOrgnTlrSrvGrpIdErr;
	unsigned long  vtmRecvMsgTimeoutOrgnTlrSrvStErr;	
	unsigned long  vtmRecvMsgTimeoutOrgnVtmIdErr;
	unsigned long  vtmRecvMsgTimeoutOrgnVtmStateErr;

	//Vtm File Operate Result
	unsigned long  vtmSendFileSuccess;
	unsigned long  vtmSendFileParaErr;
	unsigned long  vtmSendFileStateErr;
	unsigned long  vtmSendFileLenErr;
	unsigned long  vtmSendFileVtmIdErr; 
	unsigned long  vtmSendFileTargetTypeErr;
	unsigned long  vtmSendFileTargetTellerIdErr;
	unsigned long  vtmSendFileTargetTellerStateErr;
	unsigned long  vtmSendFileTargetTellerAmsPidErr;
	unsigned long  vtmSendFileTargetVtmIdErr;
	unsigned long  vtmSendFileTargetVtmStateErr;
	unsigned long  vtmSendFileFileOpTypeErr;
	unsigned long  vtmSendFileFileTypeErr;
	unsigned long  vtmSendFileFilePathErr;
	unsigned long  vtmSendFileFileNameErr;
	unsigned long  vtmSendFileStartTimerErr;
	unsigned long  vtmSendFileFileSendStateErr;
	
	unsigned long  vtmRecvFileSuccess;
	unsigned long  vtmRecvFileParaErr;
	unsigned long  vtmRecvFileStateErr;
	unsigned long  vtmRecvFileLenErr;
	unsigned long  vtmRecvFileVtmIdErr;
	unsigned long  vtmRecvFileOrgnTypeErr;
	unsigned long  vtmRecvFileOrgnTellerIdErr;
	unsigned long  vtmRecvFileOrgnTellerStateErr;
	unsigned long  vtmRecvFileOrgnTellerAmsPidErr;
	unsigned long  vtmRecvFileOrgnTellerSrvGrpIdErr;
	unsigned long  vtmRecvFileOrgnTellerSrvStErr;
	unsigned long  vtmRecvFileOrgnVtmIdErr;
	unsigned long  vtmRecvFileOrgnVtmStateErr;
	unsigned long  vtmRecvFileFileRecvStateErr;
	unsigned long  vtmRecvFileVtmReportFail;
	unsigned long  vtmRecvFileTimeout;
	
	unsigned long  vtmRecvFileTimeoutParaErr;
	unsigned long  vtmRecvFileTimeoutLenErr;
	unsigned long  vtmRecvFileTimeoutFileRecvOpErr;
	unsigned long  vtmRecvFileTimeoutFileSrvTypeErr;
	unsigned long  vtmRecvFileTimeoutOrgnTypeErr;
	unsigned long  vtmRecvFileTimeoutOrgnTlrIdErr;
	unsigned long  vtmRecvFileTimeoutOrgnTlrStErr;
	unsigned long  vtmRecvFileTimeoutOrgnTlrAmsPidErr;
	unsigned long  vtmRecvFileTimeoutOrgnTlrSrvGrpIdErr;
	unsigned long  vtmRecvFileTimeoutOrgnTlrSrvStErr;	
	unsigned long  vtmRecvFileTimeoutOrgnVtmIdErr;
	unsigned long  vtmRecvFileTimeoutOrgnVtmStateErr;

	//Vtm Force Login Result
	unsigned long  vtmForceLoginSuccess;
	unsigned long  vtmForceLoginParaErr;
	unsigned long  vtmForceLoginStateErr;
	unsigned long  vtmForceLoginNodeResourceLimited;
	unsigned long  vtmForceLoginLenErr;
	unsigned long  vtmForceLoginVtmNoErr;
	unsigned long  vtmForceLoginPwdErr;
	unsigned long  vtmForceLoginTermNetInfoErr;
	unsigned long  vtmForceLoginVncAuthPwdErr;
	unsigned long  vtmForceLoginLoginRepeatedlyErr;
	unsigned long  vtmForceLoginVtmTypeErr;
	unsigned long  vtmForceLoginVtmNumErr;
	unsigned long  vtmForceLoginFileServerUsrNameErr;
	unsigned long  vtmForceLoginFileServerUsrPwdErr;
	unsigned long  vtmForceLoginLicenseTimeout;
	unsigned long  vtmForceLoginVtmNumBeyondLic;

	//Vtm Query Info Result
	unsigned long  vtmQueryInfoSuccess;
	unsigned long  vtmQueryInfoParaErr;
	unsigned long  vtmQueryInfoStateErr;
	unsigned long  vtmQueryInfoLenErr;
	unsigned long  vtmQueryInfoVtmIdErr;
	unsigned long  vtmQueryInfoCodeErr;
	unsigned long  vtmQueryInfoServiceStateErr;
	unsigned long  vtmQueryInfoCustomerPidErr;
	unsigned long  vtmQueryInfoCalcQueueInfoErr;

	//Vtm Bandwidth Notice Result
	unsigned long  vtmBandwidthNoticeSuccess;
	unsigned long  vtmBandwidthNoticeParaErr;
	unsigned long  vtmBandwidthNoticeStateErr;
	unsigned long  vtmBandwidthNoticeLenErr; 
	unsigned long  vtmBandwidthNoticeVtmIdErr;
	unsigned long  vtmBandwidthNoticeCallIdErr;
	unsigned long  vtmBandwidthNoticeBwParaErr;	
	
	//Vtm Event Notice Result
	unsigned long  vtmEventNoticeSuccess;
	unsigned long  vtmEventNoticeParaErr;
	unsigned long  vtmEventNoticeStateErr;
	unsigned long  vtmEventNoticeLenErr;
	unsigned long  vtmEventNoticeSrvGrpIdErr;
	unsigned long  vtmEventNoticeAmsPidErr;
	unsigned long  vtmEventNoticeTellerIdErr;
	unsigned long  vtmEventNoticeVtmIdErr;
	unsigned long  vtmEventNoticeVtmStateErr;
	unsigned long  vtmEventNoticeTypeErr;
	unsigned long  vtmEventNoticeCallIdErr;
	unsigned long  vtmEventNoticeLackServiceQuality;
	unsigned long  vtmEventNoticeLackFileInfo;
	unsigned long  vtmEventNoticeServiceScoreErr;
	unsigned long  vtmEventNoticeFilePathLenErr;
	unsigned long  vtmEventNoticeFileNameLenErr;

	unsigned long  vtmEvntNtceSendFileTrgtTypeErr;
	unsigned long  vtmEvntNtceSendFileTrgtTlrIdErr;
	unsigned long  vtmEvntNtceSendFileTrgtTlrStErr;
	unsigned long  vtmEvntNtceSendFileTrgtTlrAmsPidErr;
	unsigned long  vtmEvntNtceSendFileTrgtVtmIdErr;
	unsigned long  vtmEvntNtceSendFileTrgtVtmStErr;
	unsigned long  vtmEvntNtceSendFileSendStErr;
	unsigned long  vtmEvntNtceSendFileStartTimerErr;
	
	unsigned long  vtmEvntNtceRecvFileParaErr;
	unsigned long  vtmEvntNtceRecvFileOrgnTypeErr;
	unsigned long  vtmEvntNtceRecvFileOrgnTlrIdErr;
	unsigned long  vtmEvntNtceRecvFileOrgnTlrStErr;
	unsigned long  vtmEvntNtceRecvFileOrgnTlrAmsPidErr;
	unsigned long  vtmEvntNtceRecvFileOrgnTlrSrvGrpIdErr;
	unsigned long  vtmEvntNtceRecvFileOrgnTlrSrvStErr;
	unsigned long  vtmEvntNtceRecvFileOrgnVtmIdErr;
	unsigned long  vtmEvntNtceRecvFileOrgnVtmStErr;
	unsigned long  vtmEvntNtceRecvFileRecvStErr;

	unsigned long  vtmEvntCancRecvFileParaErr;         
  	unsigned long  vtmEvntCancRecvFileOrgnTypeErr;     
  	unsigned long  vtmEvntCancRecvFileOrgnTlrIdErr;    
  	unsigned long  vtmEvntCancRecvFileOrgnTlrStErr;    
  	unsigned long  vtmEvntCancRecvFileOrgnTlrAmsPidErr;
  	unsigned long  vtmEvntCancRecvFileOrgnTlrSrvGrpIdErr; 
  	unsigned long  vtmEvntCancRecvFileOrgnTlrSrvStErr; 
  	unsigned long  vtmEvntCancRecvFileOrgnVtmIdErr;    
  	unsigned long  vtmEvntCancRecvFileOrgnVtmStErr;    
  	unsigned long  vtmEvntCancRecvFileRecvStErr;  
	
	unsigned long  vtmEvntNtceSnapStartTimerErr;
	
	unsigned long  vtmEvntNtceCustScoringVtmRepFail;
	unsigned long  vtmEvntNtceRemCoopVtmRepFail;
	unsigned long  vtmEvntNtceSnapVtmRepFail;
	unsigned long  vtmEvntNtceUpldSnapFileVtmRepFail;
	unsigned long  vtmEvntNtceUpldFileVtmRepFail;
	unsigned long  vtmEvntNtceDownLdFileVtmRepFail;
	unsigned long  vtmEvntNtceVtmCancRecvFileRepFail;

	//Vtm Para Cfg Result
	unsigned long  vtmParaCfgSuccess;
	unsigned long  vtmParaCfgIndCnfParaErr;
	unsigned long  vtmParaCfgIndCnfStateErr; 
	unsigned long  vtmParaCfgIndCnfLenErr;
	unsigned long  vtmParaCfgIndCnfVtmIdErr;
	unsigned long  vtmParaCfgIndCnfOpTypeErr; 
	unsigned long  vtmParaCfgIndCnfOpStateErr;
	unsigned long  vtmParaCfgIndCnfTermCfgParaErr;
	unsigned long  vtmParaCfgIndCnfVtmReportFail;
	unsigned long  vtmParaCfgIndCnfTimeout;
	
	unsigned long  vtmParaCfgTimeoutParaErr;
	unsigned long  vtmParaCfgTimeoutLenErr;
	unsigned long  vtmParaCfgTimeoutVtmIdErr; 
	unsigned long  vtmParaCfgTimeoutVtmStateErr;
	unsigned long  vtmParaCfgTimeoutOpTypeErr;
	
	//Cms Vta Reg Result
	unsigned long  cmsVtaRegSuccess;
	unsigned long  cmsVtaRegParaErr;
	unsigned long  cmsVtaRegStateErr;
	unsigned long  cmsVtaRegLenErr;
	unsigned long  cmsVtaRegTellerIdErr;
	unsigned long  cmsVtaRegResourceLimited;

	//Cms Vtm Reg Result
	unsigned long  cmsVtmRegSuccess;
	unsigned long  cmsVtmRegParaErr;
	unsigned long  cmsVtmRegStateErr;
	unsigned long  cmsVtmRegLenErr;
	unsigned long  cmsVtmRegVtmIdErr;
	unsigned long  cmsVtmRegResourceLimited;
	
	//Cms Get Vta Result
	unsigned long  cmsGetVtaSuccess;
	unsigned long  cmsGetVtaParaErr;
	unsigned long  cmsGetVtaLenErr;
	unsigned long  cmsGetVtaAmsPidErr;
	unsigned long  cmsGetVtaCallIdErr;
	unsigned long  cmsGetVtaTerminalTypeErr;
	unsigned long  cmsGetVtaSrvGrpIdErr;
	unsigned long  cmsGetVtaServiceTypeErr;
	unsigned long  cmsGetVtaNoValidSrvGrpOrType;
	unsigned long  cmsGetVtaCallTypeErr;
	unsigned long  cmsGetVtaOriginTellerIdErr;
	unsigned long  cmsGetVtaOriginTellerStateErr;
	unsigned long  cmsGetVtaOriginTellerAmsPidErr;
	unsigned long  cmsGetVtaOriginTellerSrvGrpIdErr;
	unsigned long  cmsGetVtaOriginTellerSrvStateErr;	
	unsigned long  cmsGetVtaTargetTypeErr;
	unsigned long  cmsGetVtaTargetGroupErr; 
	unsigned long  cmsGetVtaTargetTellerIdErr;
	unsigned long  cmsGetVtaTargetTellerStateErr;	
	unsigned long  cmsGetVtaVtmIdErr;
	unsigned long  cmsGetVtaAssocCallIdErr;
	unsigned long  cmsGetVtaLicenseTimeout;
	unsigned long  cmsGetVtaTpsBeyondLic;

	unsigned long  cmsGetVtaServiceInQueue;
	unsigned long  cmsGetVtaSIRErr;
	unsigned long  cmsGetVtaSIRVtmNoErr;
	unsigned long  cmsGetVtaSIRSrvGrpIdErr;
	unsigned long  cmsGetVtaSIRCallTypeErr;
	unsigned long  cmsGetVtaSIRVtaEmpty;
	unsigned long  cmsGetVtaSIRCalcQueueInfoErr;
	unsigned long  cmsGetVtaSIRTooManyCustInQueue;
	unsigned long  cmsGetVtaSIRTargetTellerNotLogin;
	unsigned long  cmsGetVtaSIRTargetTellerNotReg;
	unsigned long  cmsGetVtaSIRTargetTellerStateErr;
	unsigned long  cmsGetVtaSIRCallTransTooManyTimes;

	unsigned long  cmsGetVtaOrgIdErr;
	unsigned long  cmsGetVtaOrgPosErr;
	unsigned long  cmsGetVtaParentOrgIdErr;
	unsigned long  cmsGetVtaSelectSrvGrpIdErr;
	unsigned long  cmsGetVtaLpResourceLimited;
	unsigned long  cmsGetVtaCreateTimerErr;
	
	unsigned long  cmsGetVtaTimeout;
	unsigned long  cmsGetVtaTimeoutParaErr;
	unsigned long  cmsGetVtaTimeoutStateErr;
	unsigned long  cmsGetVtaTimeoutLenErr;
	unsigned long  cmsGetVtaTimeoutVtmIdErr;
	unsigned long  cmsGetVtaTimeoutServiceStateErr;	

	//Cms Query Term NetInfo Result
	unsigned long  cmsQueryTermNetInfoSuccess;
	unsigned long  cmsQueryTermNetInfoParaErr;
	unsigned long  cmsQueryTermNetInfoStateErr;
	unsigned long  cmsQueryTermNetInfoLenErr;
	unsigned long  cmsQueryTermNetInfoTellerIdErr;
	unsigned long  cmsQueryTermNetInfoVtmIdErr;
	unsigned long  cmsQueryTermNetInfoTermTypeErr;

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
	unsigned long  cmsEventNoticeVtaStateErr;
	unsigned long  cmsEventNoticeVtmStateErr;
	unsigned long  cmsEventNoticeOrgnTellerIdErr;
	unsigned long  cmsEventNoticeOrgnTellerStateErr;
	unsigned long  cmsEventNoticeOrgnTellerAmsPidErr;
	unsigned long  cmsEventNoticeOrgnTellerSrvGrpIdErr;
	unsigned long  cmsEventNoticeOrgnTellerSrvStateErr;
	
	unsigned long  cmsHandshakeClearInactiveCall;

	//Cms Event Ind Result
	unsigned long  cmsEventIndSuccess;
	unsigned long  cmsEventIndCnfParaErr;          
	unsigned long  cmsEventIndCnfStateErr;         
	unsigned long  cmsEventIndCnfLenErr;           
	unsigned long  cmsEventIndCnfAmsPidErr;        
	unsigned long  cmsEventIndCnfCallIdErr;        
	unsigned long  cmsEventIndCnfTellerIdErr;      
	unsigned long  cmsEventIndCnfVtmIdErr;    
	unsigned long  cmsEventIndCnfTellerStateErr;  
	unsigned long  cmsEventIndCnfTellerUpdateStErr;
	unsigned long  cmsEventIndCnfVtmStateErr; 
	unsigned long  cmsEventIndCnfCodeErr;          
	unsigned long  cmsEventIndCnfCmsReportFail;     
	
	unsigned long  cmsEventIndTimeoutParaErr; 
	unsigned long  cmsEventIndTimeoutStateErr;
	unsigned long  cmsEventIndTimeoutLenErr;
	unsigned long  cmsEventIndTimeoutAmsPidErr;
	unsigned long  cmsEventIndTimeoutCallIdErr; 
	unsigned long  cmsEventIndTimeoutTellerIdErr;
	unsigned long  cmsEventIndTimeoutVtmIdErr;
	unsigned long  cmsEventIndTimeoutTellerStateErr;
	unsigned long  cmsEventIndTimeoutVtmStateErr;
	unsigned long  cmsEventIndTimeoutCodeErr;
	unsigned long  cmsEventIndTimeoutSrvGrpIdErr;
	unsigned long  cmsEventIndTimeoutServiceStateErr;	

	//Rcas Handshake Result
	unsigned long  rcasHandshakeSuccess;                
	unsigned long  rcasHandshakeParaErr;
	unsigned long  rcasHandshakeStateErr;               
	unsigned long  rcasHandshakeLenErr;
	unsigned long  rcasHandshakeRcasMidErr;  
	unsigned long  rcasHandshakeNoRcasCapPara;
	unsigned long  rcasHandshakeRcasCapParaErr;
	unsigned long  rcasClearInactiveRcas;
	
	unsigned long  vtaLogoutRcasRemoteCoopAbort;
	unsigned long  vtaLogoutVtmRemoteCoopAbort;
	unsigned long  vtaForceLoginRcasRemoteCoopAbort;
	unsigned long  vtaForceLoginVtmRemoteCoopAbort;
	unsigned long  vtaHsTimeoutRcasRemoteCoopAbort;
	unsigned long  vtaHsTimeoutVtmRemoteCoopAbort;
	unsigned long  vtmLogoutRcasRemoteCoopAbort;
	unsigned long  vtmLogoutVtaRemoteCoopAbort;
	unsigned long  vtmForceLoginRcasRemoteCoopAbort;
	unsigned long  vtmForceLoginVtaRemoteCoopAbort;
	unsigned long  vtmHsTimeoutRcasRemoteCoopAbort;
	unsigned long  vtmHsTimeoutVtaRemoteCoopAbort;

	unsigned long  rcasHsTimeoutVtaRemoteCoopAbort;
	unsigned long  rcasHsTimeoutVtmRemoteCoopAbort;

	//Rcas LoadCapc Notice Result
	unsigned long  rcasLoadCapcNoticeSuccess;
	unsigned long  rcasLoadCapcNoticeParaErr;
	unsigned long  rcasLoadCapcNoticeStateErr;
	unsigned long  rcasLoadCapcNoticeLenErr;
	unsigned long  rcasLoadCapcNoticeRcasMidErr;
	unsigned long  rcasLoadCapcNoticeCpuLoadLevelErr;
	unsigned long  rcasLoadCapcNoticeCpuLoadErr;

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
	AMS_QUEUE_SYSTEM_STAT queueSystemStat;

	//ServiceStat
	AMS_SERVICE_STAT serviceStat; 

	//TellerStat
	AMS_TELLER_STAT tellerStat[AMS_MAX_VTA_NUM];

	//VtmStat
	AMS_VTM_STAT vtmStat[AMS_MAX_VTM_NUM];

	//ResultStat
	AMS_RESULT_STAT resultStat;

}AMS_STAT;


//[begin] zhuyn added
typedef struct amsDbopr_t
{
    int     updateInsertCbCode;
    
}AMS_DBOPR;
//[end] zhuyn added

typedef struct
{
	int i;

	AMS_SERVICE_MANAGE amsServiceManageData[AMS_MAX_SERVICE_GROUP_NUM];

	AMS_RCAS_DATA amsRcasData;

	AMS_DATA_LIC amsLicData;
	
	AMS_DATA_SYSCFG amsCfgData;

	AMS_DATA_REGISTER amsRegData;
		
	AMS_ALARM  amsAlarm;
	
	AMS_DEBUG  amsDebug;
	
	AMS_STAT   amsStat;
	
    AMS_DBOPR   amsDbopr;   //zhuyn added 
	
}AMS_PRI_AREA_t;

#define AmsLicData         (SystemData.AmsPriData.amsLicData)

#define AmsCfgData         (SystemData.AmsPriData.amsCfgData)
#define AmsCfgTeller(i)    (SystemData.AmsPriData.amsCfgData.tellerInfo[i])
#define AmsCfgVtm(i)       (SystemData.AmsPriData.amsCfgData.vtmInfo[i])
#define AmsCfgService(i)   (SystemData.AmsPriData.amsCfgData.serviceInfo[i])
#define AmsCfgSrvGroup(i)  (SystemData.AmsPriData.amsCfgData.srvGroupInfo[i])
#define AmsCfgOrg(i)       (SystemData.AmsPriData.amsCfgData.orgInfo[i])
#define AmsCfgRcas(i)      (SystemData.AmsPriData.amsCfgData.rcasInfo[i])
#define AmsCfgQueueSys(i)  (SystemData.AmsPriData.amsCfgData.queueSysInfo[i])

#define AmsVtaIdHashTbl    (SystemData.AmsPriData.amsCfgData.VtaIdHashTbl)
#define AmsVtmIdHashTbl    (SystemData.AmsPriData.amsCfgData.VtmIdHashTbl)
#define AmsOrgIdHashTbl    (SystemData.AmsPriData.amsCfgData.OrgIdHashTbl)

#define AmsRegTeller(i)    (SystemData.AmsPriData.amsRegData.tellerRegisterInfo[i])
#define AmsRegVtm(i)       (SystemData.AmsPriData.amsRegData.vtmRegisterInfo[i])
#define AmsSrvData(i)      (SystemData.AmsPriData.amsServiceManageData[i])
#define AmsRcasMngData(i)  (SystemData.AmsPriData.amsRcasData.amsRcasMngData[i])

//zry added for scc 2018
#define AmsCfgSeat(i)		(SystemData.AmsPriData.amsCfgData.seatInfo[i])
#define AmsSeatIdHashTbl		(SystemData.AmsPriData.amsCfgData.SeatHashTbl[i])
//added end

extern void *ServiceProcTask(void *pThreadId);
extern void *LicenseProcTask(void *pThreadId);
extern int ProcessAmsMessage(int iThreadId, MESSAGE_t *pMsg);
extern int AmsDataInit();

extern int ConfigAmsSrv(char *cFileName);
extern int SrvDivSen(char *s,WORD_t *word);
extern int AmsSrvServiceSenten(WORD_t *word,int wordcount,unsigned int *pCurrId);
extern int AmsSrvServiceGroupSenten(WORD_t *word,int wordcount,unsigned int *pCurrId);
extern int AmsSrvTellerSenten(WORD_t *word,int wordcount,unsigned int *pCurrId);
extern int AmsSrvAuthoritytSenten(WORD_t *word,int wordcount,unsigned int *pCurrId);
extern int AmsSrvVtmSenten(WORD_t *word,int wordcount,unsigned int *pCurrId);
extern int AmsSrvRcasSenten(WORD_t *word,int wordcount,unsigned int *pCurrId);
extern int AmsSrvOrgSenten(WORD_t *word,int wordcount,unsigned int *pCurrId);
extern int AmsSrvQueueSenten(WORD_t *word,int wordcount,unsigned int *pCurrId);

#endif//AMSPUB_H 


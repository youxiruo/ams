#define AMS_MAX_MBPHONE_NUM							(10000)


/* 坐席结点结构体定义 */
typedef struct  crmNode_t
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
	
}CRM_NODE;



//新系统中目前只支持mobilephone的终端类型信息
typedef struct mbphoneInfo_t
{
	unsigned char 	flag;                          	//是否配置
	unsigned char 	idstate;                       	//坐席工号状态

	unsigned char 	telleridlen;                   	//坐席工号长度
	unsigned char 	tellerid[AMS_MAX_TELLER_ID_LEN + 1];//坐席工号

	unsigned char 	srvGrpIdlen;                      //业务组类型长度
	unsigned char 	srvGrpId[AMS_MAX_GRPID_LEN + 1]
	
	DWORD 			mbpIp;
	WORD			mbpPort;	
}MBPHONE_INFO;


typedef struct amsDataSysCfg_t
{
	MBPHONE_INFO 	   mbphoneInfo[AMS_MAX_MBPHONE_NUM];

}AMS_DATA_SYSCFG;



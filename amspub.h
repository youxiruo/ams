#define AMS_MAX_MBPHONE_NUM							(10000)


/* ��ϯ���ṹ�嶨�� */
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
	
	OACBPARA_t      oaCbPara;       //���ܻص��ò��� zhuyn 20160704
	
}CRM_NODE;



//��ϵͳ��Ŀǰֻ֧��mobilephone���ն�������Ϣ
typedef struct mbphoneInfo_t
{
	unsigned char 	flag;                          	//�Ƿ�����
	unsigned char 	idstate;                       	//��ϯ����״̬

	unsigned char 	telleridlen;                   	//��ϯ���ų���
	unsigned char 	tellerid[AMS_MAX_TELLER_ID_LEN + 1];//��ϯ����

	unsigned char 	srvGrpIdlen;                      //ҵ�������ͳ���
	unsigned char 	srvGrpId[AMS_MAX_GRPID_LEN + 1]
	
	DWORD 			mbpIp;
	WORD			mbpPort;	
}MBPHONE_INFO;


typedef struct amsDataSysCfg_t
{
	MBPHONE_INFO 	   mbphoneInfo[AMS_MAX_MBPHONE_NUM];

}AMS_DATA_SYSCFG;



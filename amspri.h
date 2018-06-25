#ifndef AMSPRI_H
#define AMSPRI_H


#include "../sys/sysdata.h"
#include "amsenum.h"

#define AMS_OK                          (0)
#define AMS_ERROR                       (-1)

#define AmsTellerStat(i)                (SystemData.AmsPriData.amsStat.tellerStat[i])
#define AmsTimerStat                    (SystemData.AmsPriData.amsStat.timerStat)



//业务处理进程数与VTA数量一致，数量固定
typedef struct
{
	//state
	int     stSetState;
	int     audioRecState;
	int     screenRecState;
	int     rcasRemoteCoopState;
	int     vtmRemoteCoopState;	
	int     snapState;
	
	int     sendMsgToVtaState;
	int     sendMsgToVtmState;
	int     sendFileToVtaState;
	int     sendFileToVtmState;

	int     multiSessState;
	int     monitorState;
	int     vtaParaCfgState;

	WORD	amsTellerNum;

	DWORD   amsPid;
	//DWORD   tellerId;
	DWORD   vtmId[AMS_MAX_VTM_ID_LEN+1];
	DWORD   assocTellerId;
	
	unsigned char srvGrpIdLen;
	unsigned char srvGrpId[AMS_MAX_SERVICE_GROUP_NAME_LEN+1];
	DWORD   srvGrpIdPos;

	DWORD   tellerPos;
	DWORD   vtmPos;
	DWORD   rcasPos;
	
	WORD    currStateOp;
	
	unsigned char   tellerIdLen;                   //柜员工号长度
	unsigned char   tellerId[AMS_MAX_TELLER_ID_LEN + 1];	
	unsigned char   callIdLen;                 
	unsigned char   callId[AMS_MAX_CALLID_LEN + 1];	
	//unsigned char   vtmNoLen;                      //柜员机设备号长度
	//unsigned char   vtmNo[AMS_MAX_VTM_NO_LEN + 1];	
	unsigned char   assocCallIdLen;                 
	unsigned char   assocCallId[AMS_MAX_CALLID_LEN + 1];	

	//timer
	int     iTimerId;             //VTA
	int     callTimerId;          //CMS
	int     volumeCtrlTimerId;    //VTM
	int     rcasRemoteCoopTimerId;//RCAS
	int     vtmRemoteCoopTimerId; //VTM
	int     snapTimerId;          //VTM
	int     restTimerId;          //VTA
	int     sendMsgToVtaTimerId;  //VTA
	int     sendMsgToVtmTimerId;  //VTM
	int     sendFileToVtaTimerId; //VTA
	int     sendFileToVtmTimerId; //VTM
	
	int     multiSessTimerId;     //VTA/VTM
	int     monitorTimerId;       //VTA/VTM
	int     vtaParaCfgTimerId;    //VTA
	
//	int		iTimerType;
	
	//communication pid
	PID_t	myPid;
	PID_t	rPid;    //vta
	PID_t	vtmPid;  //vtm
	PID_t	cmsPid;
	PID_t	rcasPid;	

	//VNC_AUTH_PARA   vncAuth;                       //远程接入鉴权密码
	
	//TRANS_PROTO_PARA transToRcas;

	//AMS_SESSION_STAT sessStat;
	
	//trace
	BYTE	debugTrace;
	BYTE	commonTrace;
	BYTE	msgTrace;
	BYTE	stateTrace;
	BYTE	timerTrace;
	BYTE	errorTrace;
	BYTE	alarmTrace;
	
	BYTE	sTraceName[32];
	BYTE	lTraceNameLen;

	DWORD	CallOutType;
	
}LP_AMS_DATA_t;


#define BEGETSHORT(s, cp) do{ \
s = (cp[0] << 8) \
| cp[1] ; \
}while(0)
 
#define BEGETLONG(l, cp) do{ \
l = ((unsigned int)cp[0] << 24) \
    | ((unsigned int)cp[1] << 16) \
    | ((unsigned int)cp[2] << 8) \
    | ((unsigned int)cp[3]) ;\
}while(0)
 
#define BEPUTSHORT(s, cp) do{ \
cp[0] = (char)((s) >> 8); \
cp[1] = (char)(s); \
}while(0)
 
#define BEPUTLONG(l, cp) do{ \
cp[0] = (char)((l) >> 24); \
cp[1] = (char)((l) >> 16); \
cp[2] = (char)((l) >> 8); \
cp[3] = (char)(l); \
}while(0)
 
#define LEGETSHORT(s, cp) do{ \
s = (cp[1] << 8) \
| cp[0] ; \
}while(0)
 
#define LEGETLONG(l, cp) do{ \
l = ((unsigned int)cp[3] << 24) \
    | ((unsigned int)cp[2] << 16) \
    | ((unsigned int)cp[1] << 8) \
    | ((unsigned int)cp[0]) ;\
}while(0)
 
#define LEPUTSHORT(s, cp) do{ \
cp[1] = (char)((s) >> 8); \
cp[0] = (char)(s); \
}while(0)
 
#define LEPUTLONG(l, cp) do{ \
cp[3] = (char)((l) >> 24); \
cp[2] = (char)((l) >> 16); \
cp[1] = (char)((l) >> 8); \
cp[0] = (char)(l); \
}while(0)

#define MIN(a,b) ((a) < (b) ? (a) : (b))

#endif


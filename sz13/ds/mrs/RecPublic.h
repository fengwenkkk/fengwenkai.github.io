#ifndef REC_PUBLIC_H
#define REC_PUBLIC_H

#include "ds_public.h"
#include "pdb_app.h"
#include "msg.h"
#include "GPublic.h"
#include "CfgDao.h"

typedef INT64       GROUP_ID;

typedef struct
{
    UINT8       aucLocalAddr[4]; //localIp=192.168.3.123
    UINT8       aucServerAddr[4];

    CHAR        acLocalAddr[32];
    CHAR        acServerAddr[32];

    UINT16      usServerPort;
    UINT16      usTransFilePort;
    UINT16      usDCTransServerPort;
    CHAR        acUser[32];
    CHAR        acPwd[32];

    CHAR        acRecRootPath[256];         // 录音根目录，具体录音文件以年月和年月日分2层子目录
    CHAR        acRecTmpPath[256];
    CHAR        acRecOriPath[256];

    UINT32      ulOriSaveDays;
    UINT32      ulRecSaveDays;
    UINT32      ulTempSaveDays;

    UINT32      ulMaxRecFileIdleTime;
}REC_CFG_T;

extern REC_CFG_T    g_stIniCfg;
extern UINT32       g_ulDCUserID;

#define TIMER_INIT          TIMER0
#define EV_INIT_IND         EV_TIMER0

extern VOID SendAllClient(UINT16 usSenderTaskID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);
extern VOID OperLog(OPER_LOG_INFO_T *pstReport);
extern VOID OperLog(UINT16 usInstID, OPER_LOG_INFO_T *pstReport);
extern VOID SendAllUser(UINT16 usSenderTaskID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);

BOOL CheckCallType(UINT32 ulCallType);
BOOL IsAudioCall(UINT32 ulCallTypee);

VOID SetLogin(UINT32 ulDCUserID, PID_T *pstPID);
VOID SetLogout(UINT32 ulDCUserID);
BOOL GetDCPID(UINT32 ulDCUserID, PID_T *pstPID);

VOID InitOperatorInfo(VECTOR<REC_OPER_INFO_T> &vInfo);
VOID GetOperatorInfo(VECTOR<REC_OPER_INFO_T> &vInfo);
BOOL GetOperatorInfo(CHAR *szOperatorID, REC_OPER_INFO_T &stInfo);
VOID AddOperatorInfo(REC_OPER_INFO_T &stInfo);
VOID DelOperatorInfo(CHAR *szOperatorID);
BOOL SetOperatorInfo(REC_OPER_INFO_T &stInfo);
BOOL CheckOperator(CHAR *szOperatorID, CHAR *szPassword, UINT32 &ulOperType, CHAR *szErrInfo);
BOOL SetOperatorPwd(CHAR *szOperatorID, CHAR *szNewPassword);


#endif

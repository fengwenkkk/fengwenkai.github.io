#ifndef TASK_ATS_H
#define TASK_ATS_H

#include "DISPublic.h"
#include "ats.h"

class TaskATS: public GBaseTask
{
public :
    TaskATS(UINT16 usDispatchID);

    BOOL Init();
    virtual VOID TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);

private:
    CfgDao                  *m_pDao;
    UINT32                  m_ulValidAddrIndex;
    CHAR                    *m_szFilePrefix;
    FILE                    *m_fp;
    CHAR                    m_acFile[256];
    CHAR                    m_acZipFile[256];
    GOS_DATETIME_T          m_stLastFileTime;
    UINT32                  m_ulFrameNum;
    UINT32                  m_ulLastFrameIndex;     // 最后一次接收到的FrameIndex
    UINT32                  m_ulRecvMsgLen;
    UINT8                   m_aucRecvBuf[64*1024];
    UINT32                  m_ulMsgDataLen;
    UINT8                   m_aucMsgData[64*1024];

    virtual VOID OnServerDisconnectClient(UINT16 usClientID);

    VOID GetATSMsg(UINT8 *pucFrame, UINT32 ulFrameLen);
    BOOL ParseATSInfo(UINT8 *pucMsg, INT32 iMsgLen, VECTOR<ATS_INFO_T> &vATSInfo);
    BOOL OnATSMsg(SOCKET stSock, UINT8 *pucMsg, UINT32 ulMsgLen, UINT8 *pucAddr);
    BOOL GetATSInfo();

    BOOL SendHeartBeatMsg(SOCKET  stSock);
    VOID OnTimerGetATSInd();
    VOID SaveATSToFile(ATS_REC_T *pstATSRec);
    VOID CheckFile();
    VOID RemoveATSFile();
};

VOID GetATSInfo(VECTOR<ATS_INFO_T> &vATSInfo);
BOOL GetATSInfo(UINT32 ulTrainUnitID, ATS_INFO_T *pstATSInfo);
VOID RemoveFile(CHAR *szDir, CHAR *szFilePrefix, UINT32 ulMaxFileNum);

#endif

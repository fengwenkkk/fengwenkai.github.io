#ifndef G_TRANS_FILE_H
#define G_TRANS_FILE_H

#define TRANS_FILE_MAGIC    (('F'<<24) | ('I'<<16) | ('L'<<8) | 'E')
#define TRANS_FILE_VER      0x0100

#pragma pack(push, 1)
typedef struct
{
    UINT32                  ulMagic;
    UINT32                  ulVer;

    CHAR                    acFile[256];
    UINT32                  ulIndex;
    UINT64                  u64StartPos;

    UINT32                  ulMaxDataLen;
}TRANS_FILE_REQ_T;

#define MAX_TRANS_FILE_DATE_LEN     (60*1024)
typedef struct
{
    UINT32                  ulMagic;
    UINT32                  ulVer;

    UINT8                   ucRet;
    UINT8                   ucIsEnd;
    UINT8                   aucResv[2];

    UINT32                  ulIndex;
    UINT32                  ulDataLen;
    CHAR                    aucData[MAX_TRANS_FILE_DATE_LEN];
}TRANS_FILE_RSP_T;

typedef struct
{
    UINT32                  ulSendTime;
    UINT32                  ulSendNum;
    UINT32                  ulIndex;
    BOOL                    bRecvOver;          // 当recv长度为0时而且已经传输结束，则RecvOver
    UINT32                  ulExpectDataLen;
    UINT32                  ulRecvDataLen;
    CHAR                    aucData[MAX_TRANS_FILE_DATE_LEN];
}TRANS_FILE_RECV_BUF_T;

#pragma pack(pop)

class GTransFile: public GThread
{
public:
    GTransFile(UINT16 usPort, BOOL bAbsolutePath=FALSE);
    BOOL    Init();

    virtual GOS_THREAD_RET ThreadEntry(void* pPara);

private:
    UINT16              m_usPort;
    BOOL                m_bAbsolutePath;
    SOCKET              m_SocketLocal;
    SOCKADDR_IN         m_stLocalAddr;
    TRANS_FILE_RSP_T    m_stRsp;
    UINT8               m_aucMsgBuf[64*1024];

    VOID    Listen();
    VOID    OnTransFile(UINT8 *pucMsgBuf, UINT32 ulMsgLen, SOCKADDR_IN *pstClientAddr);
};

class GTransFileClient
{
public:
    GTransFileClient(UINT8 *pucServerAddr, UINT16 usServerPort);
    ~GTransFileClient();

    VOID SetMaxDataLen(UINT32 ulMaxDataLen);
    VOID SetResendPeriod(UINT32 ulResendPeriod){m_ulResendPeriod = ulResendPeriod;};
    VOID SetBufNum(UINT32 ulBufNum);
    BOOL DownloadFile(CHAR *szFile, CHAR *szNewFile=NULL);
    DOUBLE GetDropRate();       // 1 - 有效收报/有效发包
    DOUBLE GetResendRate();       // 有效发包/总共存储包数 - 1

private:
    BOOL Init();
    VOID InitSocket(UINT8 *pucServerAddr, UINT16 usServerPort);

    VOID GetSendInfo(std::vector<TRANS_FILE_RECV_BUF_T*> &vBuf);
    VOID Send(TRANS_FILE_RECV_BUF_T* pstBuf);
    INT32 Recv();
    BOOL IsOver();
    VOID SaveData();
    VOID TransOver();

    BOOL _DownloadFile(CHAR *szFile, CHAR *szNewFile);

    UINT32                  m_ulMaxDataLen;
    SOCKET                  m_stSocket;
    SOCKADDR_IN             m_stServerAddr;

    UINT32                  m_ulBufNum;
    TRANS_FILE_RECV_BUF_T   m_astRecvBuf[16];

    TRANS_FILE_REQ_T        m_stReq;
    TRANS_FILE_RSP_T        m_stRsp;

    UINT32                  m_ulSendPkgNum;
    UINT32                  m_ulRecvPkgNum;

    HANDLE                  m_hFile;
    UINT32                  m_ulNextWriteIndex;
    UINT32                  m_ulNextSendIndex;

    UINT32                  m_ulLastIndex;
    UINT32                  m_ulResendPeriod;

    UINT32                  m_ulLastSendPkgNum;
    UINT32                  m_ulLastRecvPkgNum;

    DOUBLE                  m_dCurrDropRate;
    DOUBLE                  m_dCurrResendRate;
};

BOOL TransFile(UINT8 *pucServerAddr, UINT16 usServerPort, CHAR *szFile, CHAR *szNewFile=NULL, UINT32 ulMaxDataLen=MAX_TRANS_FILE_DATE_LEN, BOOL *pbCancel=NULL);

#endif

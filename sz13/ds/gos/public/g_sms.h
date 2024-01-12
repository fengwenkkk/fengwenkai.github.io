#ifndef G_SMS_H
#define G_SMS_H

#define MAX_SMS_TEXT_SIZE               (70*4)
#define MAX_SMS_TEXT_SIZE_WITH_INDEX    (MAX_SMS_TEXT_SIZE-5*4)
#define CHAR_CTRL_Z                     0x1A

#define MAX_SMS_BLOCK_NUM               9

typedef struct
{
    UINT32      ulSMSLiminal;       //短信阈值；
    CHAR*       szOverageHead;
    CHAR*       szOverageTail;      //余额查询头尾关键字；
}SMS_CONFIG_T;

typedef struct _GSMInfo
{
    UINT32      ulSMSCapacity;
    UINT32      ulSMSNum;           //短信容量，短信数；
    DOUBLE      dOverage;           //余额值；
    BOOL        bSignalState;       //信号状态；
    BOOL        bModemReady;        //模块状态；
}SMS_INFO_T;

typedef struct
{
    CHAR        acPhoneNo[32+1];
    CHAR        acSMSContent[2048+256];
}SMS_PHONE_INFO_T;

typedef struct Node
{
    UINT32       id;
    Node *       next;
}Node, *pLinklist;

CHAR *StringToUnicode(CHAR *szIn);
BOOL IndexToUnicode(UINT32 ulBlockNum, UINT32 ulBlockNo, CHAR *szIndexUnicode);
UINT32  SMSPackage(CHAR *szPhoneNumber, CHAR *szSMSText, CHAR (*szSMSPcak)[400]);

class GSms
{
public:
    GSms();
    ~GSms();

public:
    VOID SetGSMConfig(UINT32 ulSMSLiminal, const CHAR *pHeadKey, const CHAR *pTailKey);
    BOOL GetModemAck();
    BOOL SetEchoMode(BOOL bShowEcho);
    BOOL CheckSignal();
    BOOL SetSMSMode(BOOL bTextMode, BOOL bIgnore = FALSE);
    BOOL Init(GSerialPort *pSerialPort);

    BOOL CheckMailboxInfo();
    BOOL PurgeMailbox(UINT32 ulTimeOut = 16000);
    BOOL ListAllSMS(CHAR (*szRecvBuf)[350]);
    BOOL ListUnreadSMS(CHAR (*szRecvBuf)[350]);
    BOOL CreateSMSLink();
    BOOL LoadSMSLink();
    BOOL MailboxInit();

    BOOL ReadSMS(CHAR *szRecvBuf, UINT32 SMSNo);
    BOOL SendSMS(CHAR *szPhoneNumber, CHAR *szSMSText);
    BOOL DeleteSMS(UINT32 SMSid = 0);                                              //默认依次删除

    BOOL WaitForSMS(UINT32 ulTimeOut=9000);
    BOOL ReadOverageSMS();
    BOOL CheckOverage();

private:
    CHAR *Send(CHAR *szCmdBuf, UINT32 ulWaitTime=4000, UINT32 ulTimeOut = 5000);

    CHAR            m_acRecvBuf[1000];
    CHAR            szRecvNo_UCS2[32];

    SMS_CONFIG_T    m_stGSMConfig;
    SMS_INFO_T      m_stGSMInfo;

    pLinklist       m_stMailboxHead;        // TODO 去掉，用 vector
    pLinklist       m_stMailboxTail;

    GSerialPort     *m_pSerialPort;     // TODO 成员变量 m_开头
};




#endif

#ifndef G_TELNET_H
#define G_TELNET_H

#define TELNET_PORT     23

#pragma pack(push, 4)

typedef struct
{
    UINT8   aucAddr[4];
    UINT16  usPort;
    UINT32  ulRecvTimeout;      // 单位ms

    CHAR    acUserPrompt[32];
    CHAR    acPwdPrompt[32];
    CHAR    acExtPrompt[32];
    CHAR    acUser[32];
    CHAR    acPwd[32];
    CHAR    acExt[32];

    CHAR    acPrompt[16];       // 登录成功提示符，如 >、#、$

    CHAR    acLoginFail[32];    // 登录失败提示文字
}TELNET_INFO_T;

#pragma pack(pop)

class GTelnet
{
public:
    GTelnet(TELNET_INFO_T *pstTelnetInfo);
    ~GTelnet();

    BOOL Connect();
    VOID Close();
    CHAR* GetPrompt(){return m_stTelnetInfo.acPrompt;};
    VOID SetPrompt(CHAR *szPrompt);
    VOID SetTimeout(UINT32 ulTimeout);

    BOOL Command(UINT8 *pucCmd, UINT32 ulCmdLen, CHAR *szPrompt=NULL);
    BOOL Command(CHAR *szCmd, CHAR *szPrompt=NULL);

    BOOL GetValue(CHAR *szCmd, CHAR *szParam, CHAR *szSymbol, CHAR *szEndSymbol, CHAR *szValue, UINT32 ulMaxLen);
    BOOL GetValue(CHAR *szCmd, CHAR *szParam, CHAR *szSymbol, CHAR *szValue, UINT32 ulMaxLen);
    BOOL GetValue(CHAR *szCmd, CHAR *szParam, CHAR *szSymbol, UINT32 *pulValue);
    BOOL GetValue(CHAR *szCmd, CHAR *szParam, CHAR *szSymbol, INT32 *piValue);
    BOOL GetValue(CHAR *szCmd, CHAR *szParam, CHAR *szSymbol, DOUBLE *pdValue);
    BOOL GetValue(CHAR *szCmd, CHAR *szParam, CHAR *szSymbol, DOUBLE *pdValue, UINT32 ulPrecision);
    BOOL GetIPValue(CHAR *szCmd, CHAR *szParam, CHAR *szSymbol, UINT8 *pucAddr);
    BOOL GetBoolValue(CHAR *szCmd, CHAR *szParam, CHAR *szSymbol, CHAR *szTrue, CHAR *szFalse, BOOL *pbOnOff);
    BOOL GetPercentValue(CHAR *szCmd, CHAR *szParam, CHAR *szSymbol, DOUBLE *pdValue);

    UINT32  GetMaxValueLen(){return sizeof(m_acMsgBuf)-1;};
    UINT32  GetValueLen(){return m_ulMsgLen;};
    VOID*   GetValue(){return m_acMsgBuf;};

    BOOL SendMsg(CHAR *szMsg);
    BOOL RecvMsg();
    BOOL RecvMsg(CHAR *szPrompt);

    VOID InitRecvBuf();
    VOID PrintRecvMsg();

private:
    SOCKET  m_stSocket;
    CHAR    m_acMsgBuf[24*1024];
    UINT32  m_ulMsgLen;
    TELNET_INFO_T   m_stTelnetInfo;

    BOOL SendMsg(UINT8 *pucCmd, UINT32 ulCmdLen);
    BOOL SendNegoiateMsg(CHAR cCmd, CHAR cValue);

    BOOL RecvMsg(CHAR *szPrompt, UINT32 ulWaitTime);
    INT32 RecvMsg(SOCKET stSock, VOID *pMsgBuf, UINT32 ulMsgBufLen, UINT32 ulTimeout, CHAR *szPrompt);

    VOID Negoiate(CHAR cCmd, CHAR cValue);
    VOID Negoiate();
};


#endif

#include "g_include.h"

#ifdef _OSWIN32_
#pragma warning (disable : 4310)
#endif

const CHAR OPTION_SUB_BEGIN = (CHAR)0xFA;   // 250;
const CHAR OPTION_SUB_END   = (CHAR)0xF0;   // 240;
const CHAR OPTION_WILL      = (CHAR)0xfB;   // 希望对方
const CHAR OPTION_WONT      = (CHAR)0xFC;   // 不希望对方;
const CHAR OPTION_DO        = (CHAR)0xFD;   // 253;
const CHAR OPTION_DONT      = (CHAR)0xFE;   // 254
const CHAR OPTION_IAC       = (CHAR)0XFF;   //

const CHAR VALUE_ECHO       = (CHAR)1;
const CHAR VALUE_SGA        = (CHAR)3;
const CHAR VALUE_TERM_TYPE  = (CHAR)0x18;
const CHAR VALUE_TERM_SPEED = (CHAR)0x20;
const CHAR VALUE_X_LOC      = (CHAR)0x23;
const CHAR VALUE_NEW_ENV    = (CHAR)0x27;

#ifdef _OSWIN32_
#pragma warning (default : 4310)
#endif

GTelnet::GTelnet(TELNET_INFO_T *pstTelnetInfo)
{
    memcpy(&m_stTelnetInfo, pstTelnetInfo, sizeof(TELNET_INFO_T));

    if (0 == m_stTelnetInfo.usPort)
    {
        m_stTelnetInfo.usPort = TELNET_PORT;
    }

    m_stSocket = INVALID_SOCKET;

    m_ulMsgLen = 0;
}

GTelnet::~GTelnet()
{
    Close();

    CLOSE_SOCKET(m_stSocket);
}

VOID GTelnet::SetPrompt(CHAR *szPrompt)
{
    strcpy(m_stTelnetInfo.acPrompt, szPrompt);
}

VOID GTelnet::SetTimeout(UINT32 ulTimeout)
{
    m_stTelnetInfo.ulRecvTimeout = ulTimeout;
}

static BOOL HasString(CHAR *szMsgBuf, INT32 iBufSize, CHAR *szUserPrompt)
{
    UINT32  ulCmpLen = strlen(szUserPrompt);

    iBufSize -= ulCmpLen;
    for (INT32 i=0; i<iBufSize; i++)
    {
        if (memcmp(szMsgBuf+i, szUserPrompt, ulCmpLen) == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL GTelnet::Connect()
{
    UINT32  i;
    BOOL    bUser = FALSE;
    BOOL    bPwd = FALSE;
    BOOL    bExt = FALSE;
    CHAR    *szPrompt;

    m_stSocket = gos_connect_tcp_server(m_stTelnetInfo.aucAddr, m_stTelnetInfo.usPort);
    if (m_stSocket == INVALID_SOCKET)
    {
        return FALSE;
    }

    if (m_stTelnetInfo.acUserPrompt[0] == '\0')
    {
        bUser = TRUE;
    }

    if (m_stTelnetInfo.acPwdPrompt[0] == '\0')
    {
        bPwd = TRUE;
    }

    if (m_stTelnetInfo.acExtPrompt[0] == '\0')
    {
        bExt = TRUE;
    }

    for (i=0; i<10; i++)
    {
        szPrompt = NULL;
        if (!bUser)
        {
            szPrompt = m_stTelnetInfo.acUserPrompt;
        }
        else if (!bPwd)
        {
            szPrompt = m_stTelnetInfo.acPwdPrompt;
        }
        else if (!bExt)
        {
            szPrompt = m_stTelnetInfo.acExtPrompt;
        }
        else
        {
            szPrompt = m_stTelnetInfo.acPrompt;
        }

        if (!RecvMsg(szPrompt))
        {
            goto fail;
        }

        Negoiate();

        if (HasString(m_acMsgBuf, m_ulMsgLen, m_stTelnetInfo.acExtPrompt))
        {
            m_ulMsgLen += 0;
        }

        if (!bUser && HasString(m_acMsgBuf, m_ulMsgLen, m_stTelnetInfo.acUserPrompt))
        {
            bUser = TRUE;
            SendMsg(m_stTelnetInfo.acUser);
            SendMsg((CHAR*)"\r\n");
            continue;
        }
        else if (!bPwd && HasString(m_acMsgBuf, m_ulMsgLen, m_stTelnetInfo.acPwdPrompt))
        {
            bPwd = TRUE;
            SendMsg(m_stTelnetInfo.acPwd);
            SendMsg((CHAR*)"\r\n");

            if (!bExt)
            {
                continue;
            }
            else
            {
                break;
            }
        }
        else if (!bExt && HasString(m_acMsgBuf, m_ulMsgLen, m_stTelnetInfo.acExtPrompt))
        {
            bExt = TRUE;
            SendMsg(m_stTelnetInfo.acExt);
            SendMsg((CHAR*)"\r\n");
            break;
        }

        if (bUser && bPwd && bExt && HasString(m_acMsgBuf, m_ulMsgLen, m_stTelnetInfo.acPrompt))
        {
            m_ulMsgLen = 0;
            memset(m_acMsgBuf, 0, sizeof(m_acMsgBuf));
            return TRUE;
        }
    }

    m_ulMsgLen = 0;
    memset(m_acMsgBuf, 0, sizeof(m_acMsgBuf));
    if (!RecvMsg())
    {
        goto fail;
    }

    if (m_ulMsgLen == 0 || HasString(m_acMsgBuf, m_ulMsgLen, m_stTelnetInfo.acLoginFail))
    {
        goto fail;
    }

    return TRUE;

fail:
    Close();

    return FALSE;
}

VOID GTelnet::Negoiate(CHAR cCmd, CHAR cValue)
{
    if (cCmd == OPTION_DO)
    {
        SendNegoiateMsg(cValue == VALUE_ECHO ? OPTION_WILL : OPTION_WONT, cValue);
    }
    else if (cCmd == OPTION_DONT)
    {
        SendNegoiateMsg(OPTION_WONT, cValue);
    }
    else if (cCmd == OPTION_WILL)
    {
        if (cValue == VALUE_SGA ||
            cValue == VALUE_ECHO)
        {
            SendNegoiateMsg(OPTION_DO, cValue);
        }
        else
        {
            SendNegoiateMsg(OPTION_DONT, cValue);
        }
    }
    else if (cCmd == OPTION_WONT)
    {
        SendNegoiateMsg(OPTION_DONT, cValue);
    }
    else
    {
        printf("unknown cmd = %c%c\n", cCmd, cValue);
    }
}

BOOL GTelnet::SendNegoiateMsg(CHAR cCmd, CHAR cValue)
{
    CHAR    acMsg[4];

    acMsg[0] = OPTION_IAC;
    acMsg[1] = cCmd;
    acMsg[2] = cValue;

    return gos_tcp_send(m_stSocket, acMsg, 3);
}

VOID GTelnet::Negoiate()
{
    INT32   iMsgLen = m_ulMsgLen;
    CHAR    *szMsg = m_acMsgBuf;
    BOOL    bBlank = TRUE;
    UINT32  ulStartPos = 0;

    for (INT32 i=0; i<iMsgLen; )
    {
        if (szMsg[i] == OPTION_IAC)
        {
            if ((i+3) > iMsgLen)
            {
                break;
            }

            Negoiate(szMsg[i+1], szMsg[i+2]);
            memset(szMsg+i, 0, 3);
            i += 3;

            if (bBlank)
            {
                ulStartPos = i;
            }
        }
        else
        {
            i++;
            bBlank = FALSE;
        }
    }

    if (ulStartPos != 0)
    {
        m_ulMsgLen -= ulStartPos;
        memmove(m_acMsgBuf, &m_acMsgBuf[ulStartPos], m_ulMsgLen);
    }
}

BOOL GTelnet::Command(UINT8 *pucCmd, UINT32 ulCmdLen, CHAR *szPrompt)
{
    if (m_stSocket == INVALID_SOCKET)
    {
        if (!Connect())
        {
            return FALSE;
        }
    }

    SendMsg(pucCmd, ulCmdLen);

    memset(&m_acMsgBuf, 0, sizeof(m_acMsgBuf));
    m_ulMsgLen = 0;

    if (szPrompt)
    {
        return RecvMsg(szPrompt);
    }
    else
    {
        return RecvMsg();
    }
}

BOOL GTelnet::Command(CHAR *szCmd, CHAR *szPrompt)
{
    if (m_stSocket == INVALID_SOCKET)
    {
        if (!Connect())
        {
            return FALSE;
        }
    }

    SendMsg(szCmd);

    memset(&m_acMsgBuf, 0, sizeof(m_acMsgBuf));
    m_ulMsgLen = 0;

    if (szPrompt)
    {
        return RecvMsg(szPrompt);
    }
    else
    {
        return RecvMsg();
    }
}

VOID GTelnet::Close()
{
    CLOSE_SOCKET(m_stSocket);
    m_ulMsgLen = 0;
    memset(m_acMsgBuf, 0, sizeof(m_acMsgBuf));
}

BOOL GTelnet::SendMsg(UINT8 *pucMsg, UINT32 ulMsgLen)
{
    return gos_tcp_send(m_stSocket, pucMsg, ulMsgLen);
}

BOOL GTelnet::SendMsg(CHAR *szMsg)
{
    return gos_tcp_send(m_stSocket, szMsg, strlen(szMsg));
}

VOID GTelnet::InitRecvBuf()
{
    m_ulMsgLen = 0;
}

INT32 GTelnet::RecvMsg(SOCKET stSock, VOID *pMsgBuf, UINT32 ulMsgBufLen, UINT32 ulTimeout, CHAR *szPrompt)
{
    FD_SET  fds;
    TIMEVAL stTimeout = {0};
    INT32   iRet;
    INT32   iRecvLen;
    INT32   iTotalRecvLen = 0;
    CHAR    *szMsgBuf = (CHAR*)pMsgBuf;

    if (stSock == INVALID_SOCKET)
    {
        return -1;
    }

    if (0 == ulMsgBufLen)
    {
        return 0;
    }

again:
    stTimeout.tv_sec = ulTimeout/1000;
    stTimeout.tv_usec = 1000*MOD(ulTimeout,1000);
    FD_ZERO(&fds);
    FD_SET(stSock, &fds);

    iRet = select(stSock + 1, &fds, NULL, NULL, &stTimeout);
    if (iRet == 0)
    {
        return iTotalRecvLen;
    }

    if (iRet < 0)
    {
        if (errno == EINTR)
        {
            goto again;
        }

        return -1;
    }

    if (FD_ISSET(stSock, &fds))
    {
        iRecvLen = recv(stSock, szMsgBuf+iTotalRecvLen, ulMsgBufLen-iTotalRecvLen, 0);
        if (iRecvLen < 0)
        {
            return -1;
        }
        else if (iRecvLen == 0)
        {
            if (iTotalRecvLen == 0)
            {
                return -1;
            }

            return iTotalRecvLen;
        }
        else
        {
            iTotalRecvLen += iRecvLen;
            if (ulMsgBufLen == (UINT32)iTotalRecvLen)
            {
                return iTotalRecvLen;
            }
            else if (!IS_NULL_STRING(szPrompt) && HasString(m_acMsgBuf, m_ulMsgLen+iTotalRecvLen, szPrompt))
            {
                return iTotalRecvLen;
            }
        }
    }

    goto again;
}

BOOL GTelnet::RecvMsg()
{
    return RecvMsg(m_stTelnetInfo.acPrompt, m_stTelnetInfo.ulRecvTimeout);
}

BOOL GTelnet::RecvMsg(CHAR *szPrompt)
{
    return RecvMsg(szPrompt, m_stTelnetInfo.ulRecvTimeout);
}

BOOL GTelnet::RecvMsg(CHAR *szPrompt, UINT32 ulWaitTime)
{
    INT32   iRecvSize = RecvMsg(m_stSocket, m_acMsgBuf+m_ulMsgLen, sizeof(m_acMsgBuf)-1-m_ulMsgLen, ulWaitTime, szPrompt);

    if (iRecvSize < 0)
    {
        Close();

        return FALSE;
    }

    m_ulMsgLen += iRecvSize;

    return TRUE;
}

VOID GTelnet::PrintRecvMsg()
{
    printf("\n== msg start ==\n");

    for (UINT32 i=0; i<m_ulMsgLen; i++)
    {
        printf("%c", m_acMsgBuf[i]);
    }

    printf("\n== msg end == \n");
}

char *my_strstr(char *szText, UINT32 ulMaxLen, char *szSymbol)
{
    UINT32  ulSymbolLen = strlen(szSymbol);
    INT32   iLen = ulMaxLen - ulSymbolLen+1;

    if (ulSymbolLen == 0)
    {
        return szText;
    }

    for (INT32 i=0; i<iLen; i++)
    {
        if (memcmp(szText+i, szSymbol, ulSymbolLen) == 0)
        {
            return szText+i;
        }
    }

    return NULL;
}

BOOL GTelnet::GetValue(CHAR *szCmd, CHAR *szParam, CHAR *szSymbol, CHAR *szEndSymbol, CHAR *szValue, UINT32 ulMaxLen)
{
    CHAR    *szCurr;
    UINT32  ulLen = 0;
    UINT32  ulEndSymbolLen = szEndSymbol?strlen(szEndSymbol):0;

    szCurr = my_strstr(m_acMsgBuf, m_ulMsgLen, szCmd);
    if (!szCurr)
    {
        return FALSE;
    }

    szCurr = strstr(szCurr+strlen(szCmd), szParam);
    if (!szCurr)
    {
        return FALSE;
    }

    szCurr += strlen(szParam);
    if (szSymbol)
    {
        szCurr = strstr(szCurr, szSymbol);
        if (!szCurr)
        {
            return FALSE;
        }

        szCurr += strlen(szSymbol);
    }

    szCurr = gos_left_trim_string(szCurr);

    memset(szValue, 0, ulMaxLen);
    while(1)
    {
        if (*szCurr == '\0')
        {
            return TRUE;
        }

        if (ulEndSymbolLen > 0)
        {
            if (memcmp(szCurr, szEndSymbol, ulEndSymbolLen) == 0)
            {
                return TRUE;
            }
        }
        else
        {
            if (gos_isspace(*szCurr))
            {
                return TRUE;
            }
        }

        if (ulLen == ulMaxLen)
        {
            return FALSE;
        }

        szValue[ulLen++] = *szCurr++;
    }
}

BOOL GTelnet::GetValue(CHAR *szCmd, CHAR *szParam, CHAR *szSymbol, CHAR *szValue, UINT32 ulMaxLen)
{
    return GetValue(szCmd, szParam, szSymbol, NULL, szValue, ulMaxLen);
}

BOOL GTelnet::GetValue(CHAR *szCmd, CHAR *szParam, CHAR *szSymbol, UINT32 *pulValue)
{
    CHAR    acValue[16];

    if (!GetValue(szCmd, szParam, szSymbol, acValue, sizeof(acValue)-1))
    {
        return FALSE;
    }

    return gos_atou(acValue, pulValue);
}

BOOL GTelnet::GetIPValue(CHAR *szCmd, CHAR *szParam, CHAR *szSymbol, UINT8 *pucAddr)
{
    CHAR    acValue[32];

    if (!GetValue(szCmd, szParam, szSymbol, acValue, sizeof(acValue)-1))
    {
        return FALSE;
    }

    return gos_get_ip(acValue, pucAddr);
}

BOOL GTelnet::GetBoolValue(CHAR *szCmd, CHAR *szParam, CHAR *szSymbol, CHAR *szTrue, CHAR *szFalse, BOOL *pbOnOff)
{
    CHAR    acValue[32];

    if (!GetValue(szCmd, szParam, szSymbol, acValue, sizeof(acValue)-1))
    {
        return FALSE;
    }

    if (strcmp(acValue, szTrue) == 0)
    {
        *pbOnOff = TRUE;
        return TRUE;
    }
    else if (strcmp(acValue, szFalse) == 0)
    {
        *pbOnOff = FALSE;
        return TRUE;
    }

    return FALSE;
}

BOOL GTelnet::GetPercentValue(CHAR *szCmd, CHAR *szParam, CHAR *szSymbol, DOUBLE *pdValue)
{
    CHAR    acValue[64];
    CHAR    *szMark;

    if (!GetValue(szCmd, szParam, szSymbol, acValue, sizeof(acValue)-1))
    {
        return FALSE;
    }

    szMark = strchr(acValue, '%');
    if (szMark)
    {
        *szMark = '\0';
    }

    return gos_atof(acValue, pdValue);
}

BOOL GTelnet::GetValue(CHAR *szCmd, CHAR *szParam, CHAR *szSymbol, INT32 *piValue)
{
    CHAR    acValue[16];

    if (!GetValue(szCmd, szParam, szSymbol, acValue, sizeof(acValue)-1))
    {
        return FALSE;
    }

    return gos_atoi(acValue, piValue);
}

BOOL GTelnet::GetValue(CHAR *szCmd, CHAR *szParam, CHAR *szSymbol, DOUBLE *pdValue)
{
    CHAR    acValue[32];

    if (!GetValue(szCmd, szParam, szSymbol, acValue, sizeof(acValue)-1))
    {
        return FALSE;
    }

    return gos_atof(acValue, pdValue);
}

BOOL GTelnet::GetValue(CHAR *szCmd, CHAR *szParam, CHAR *szSymbol, DOUBLE *pdValue, UINT32 ulPrecision)
{
    if (!GetValue(szCmd, szParam, szSymbol, pdValue))
    {
        return FALSE;
    }

    INT64   i64Value;
    DOUBLE  dMulti = pow((DOUBLE)10, (INT32)ulPrecision);

    i64Value = *pdValue * dMulti;
    *pdValue = i64Value / dMulti;

    return TRUE;
}

#include "g_include.h"
#include "ds_public.h"
#include "rtsp.h"
#include "rtsp_util.h"
//#include "rtsp_client.h"
#include "rtsp_server.h"

//ȫ�ֱ�������
UINT32      g_ulRTSPClientTimeout = 3600 * 24;
UINT32      g_ulRTSPClientConnectTimeout = 30;
UINT8       g_aucRTSPServerAddr[4] = { 0,0,0,0 };//{192,0,1,203};
UINT16      g_usRTSPServerPort = DEFAULT_RTP_PORT;//554
UINT8       g_aucLocalRTSPServerAddr[4] = { 0,0,0,0 };//{192,0,1,203};
UINT16      g_usLocalRTSPServerPort = 0;
UINT16      g_usDefaultRTSPServerPort = DEFAULT_RTP_PORT;
FD_SET      g_fdsAll;

CONN_INFO_T     g_stRTSPClientConnInfo = { 0 };
UINT32          g_ulConnectRTSPServerSuccTime = 0;

static UINT32  g_ulLowerUDPPort = 36500;
static UINT32  g_ulUpperUDPPort = 0xffff;

CHAR        g_acCurrRTSPCmd[32] = { 0 };
UINT32      g_ulMaxConnectRTSPServerTime = 30;      // ������ʱ��δ�ɹ����ӵ�RTSP Server���Զ��˳�

SOCKET      g_stLocalRTSPServerSock = INVALID_SOCKET;       // �ṩ���ͻ��˵ı���RTSP������



/*******************************************************************************
* ��������: NewConnInfo
* ��    ��: ��¼�µĽ��ն���Ϣ��������־��¼
* ��    ��:
* ��������              ����/���   ����
* stClientSocket        IN          accept�Ŀͻ����׽���
* stClientAddr          IN          �ͻ��˵�ַ
* ��������:VOID
* ˵    ��:
*******************************************************************************/
VOID NewConnInfo(SOCKET stClientSocket, SOCKADDR_IN& stClientAddr)
{
    CONN_INFO_T     stConnInfo;
    UINT32          ulBufSize = 64 * 1024;
    
    memset(&stConnInfo, 0, sizeof(stConnInfo));
    stConnInfo.bValid = TRUE;
    stConnInfo.ulMaxRecvBufSize = ulBufSize - 1;
    stConnInfo.ulMaxLocalRecvBufSize = ulBufSize - 1;
    stConnInfo.ulMaxLocalRTPRecvBufSize = ulBufSize - 1;
    stConnInfo.ulMaxLocalRTCPRecvBufSize = ulBufSize - 1;

    stConnInfo.stClientSocket = stClientSocket;
    stConnInfo.stLocalSocket = INVALID_SOCKET;
    stConnInfo.stLocalRTPSocket = INVALID_SOCKET;
    stConnInfo.stLocalRTCPSocket = INVALID_SOCKET;

    memcpy(&stConnInfo.stClientAddr, &stClientAddr, sizeof(stClientAddr));

    stConnInfo.szRecvBuf = (CHAR*)calloc(1, stConnInfo.ulMaxRecvBufSize + 1 + stConnInfo.ulMaxLocalRecvBufSize + 1 + stConnInfo.ulMaxLocalRTPRecvBufSize + 1 + stConnInfo.ulMaxLocalRTCPRecvBufSize + 1);
    stConnInfo.szLocalRecvBuf = stConnInfo.szRecvBuf + stConnInfo.ulMaxRecvBufSize + 1;
    stConnInfo.pucLocalRTPRecvBuf = (UINT8*)(stConnInfo.szLocalRecvBuf + stConnInfo.ulMaxLocalRecvBufSize + 1);
    stConnInfo.pucLocalRTCPRecvBuf = stConnInfo.pucLocalRTPRecvBuf + stConnInfo.ulMaxLocalRTPRecvBufSize + 1;
    
    if (g_stRTSPClientConnInfo.bValid)
    {
        FD_CLR(g_stRTSPClientConnInfo.stClientSocket, &g_fdsAll);
        CLOSE_SOCKET(g_stRTSPClientConnInfo.stClientSocket);
        GOS_FREE(g_stRTSPClientConnInfo.szRecvBuf);
    }
    memcpy(&g_stRTSPClientConnInfo, &stConnInfo, sizeof(stConnInfo));

    FD_SET(stClientSocket, &g_fdsAll);
}

VOID CloseApp()
{
    gos_sleep_1ms(500);
    CLOSE_SOCKET(g_stLocalRTSPServerSock);
    exit(0);
}



/*******************************************************************************
* ��������: GetSIPValue
* ��    ��: ��ȡ���ļ�ֵ
* ��    ��:
* ��������              ����/���   ����
* szSIPText              IN         �ı���Ϣ
* szKey                  IN         ��ֵ����
* szValue                IN         ��ֵֵ������
* ulMaxLen               IN         ����ĳ���
* ��������:BOOL
* ˵    ��:
*******************************************************************************/
BOOL GetSIPValue(CHAR* szSIPText, const CHAR* szKey, UINT32* pulValue)
{
    CHAR    acValue[16];
    CHAR*   szValue= acValue;
    CHAR*   szStart = strstr(szSIPText, szKey); // ��SIP�ı��в��Ҽ�����ʼλ��
    CHAR*   szEnd;
    UINT32  ulLen;
    UINT32  ulMaxLen = sizeof(acValue);

    if (!szStart)
    {
        return FALSE; // ����Ҳ���������ʼλ�ã��򷵻�FALSE��ʾ��ȡʧ��
    }

    szStart += strlen(szKey) + 1; // ����ʼλ������ƶ����Ⱥ�֮���λ��
    szEnd = strstr(szStart, "\r\n"); // ����ʼλ��֮����һ��з���λ�ã���ʾֵ�Ľ���λ��
    if (!szEnd)
    {
        return FALSE; // ����Ҳ���ֵ�Ľ���λ�ã��򷵻�FALSE��ʾ��ȡʧ��
    }

    ulLen = (UINT32)(szEnd - szStart); // ����ֵ�ĳ���
    if (ulLen >= ulMaxLen)
    {
        return FALSE; // ���ֵ�ĳ��ȴ��ڵ�����󳤶ȣ��򷵻�FALSE��ʾ��ȡʧ��
    }

    memcpy(szValue, szStart, ulLen); // ��ֵ���Ƶ�ָ���Ļ�������
    szValue[ulLen] = '\0'; // �ڻ�������ĩβ����ַ���������

    return gos_atou(acValue, pulValue); // ����TRUE��ʾ�ɹ���ȡֵ
}

BOOL ParseRTSPPort(CHAR* szPort, UINT16& usRTPPort, UINT16& usRTCPPort)
{
    CHAR* szPort2 = strchr(szPort, '-');
    if (!szPort2)
    {
        return FALSE;
    }
    *szPort2++ = '\0';
    return gos_get_short(szPort, &usRTPPort) &&
        gos_get_short(szPort2, &usRTCPPort);
}

GString ReplaceRTSPMsg(CHAR* szRTSPMsg, const CHAR* szKey, CHAR* szNewValue)
{
    GString strMsg;
    CHAR* szMsg = strstr(szRTSPMsg, szKey);

    if (!szMsg)
    {
        strMsg.Append(szRTSPMsg);
        return strMsg;
    }

    szMsg += strlen(szKey);
    szMsg = gos_left_trim_string(szMsg);
    if (*szMsg != '=')
    {
        strMsg.Append(szRTSPMsg);
        return strMsg;
    }

    szMsg = gos_left_trim_string(szMsg + 1);

    CHAR* szEnd = strstr(szMsg, "\r\n");
    if (!szEnd)
    {
        strMsg.Append(szRTSPMsg);
        return strMsg;
    }

    CHAR* szEnd2 = strchr(szMsg, ';');

    if (szEnd2 && szEnd2 < szEnd)
    {
        szEnd = szEnd2;
    }

    UINT32      ulLen = szMsg - szRTSPMsg;

    strMsg.Append(szRTSPMsg, ulLen, FALSE);
    strMsg.Append(szNewValue);
    strMsg.Append(szEnd);

    return strMsg;
}

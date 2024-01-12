//---------------------------------------------------------------------------
#include "g_include.h"
#include "GCommon.h"

#include "GTransFile.h"

GTransFile::GTransFile(UINT16 usPort, BOOL bAbsolutePath):GThread(NULL)
{
    m_usPort = usPort;
    m_bAbsolutePath = bAbsolutePath;
    m_SocketLocal = INVALID_SOCKET;

    memset(&m_stLocalAddr, 0, sizeof(m_stLocalAddr));
}

GOS_THREAD_RET GTransFile::ThreadEntry(void* pPara)
{
    while(1)
    {
        Listen();
        gos_sleep_1ms(10);
    }
}

BOOL GTransFile::Init()
{
    UINT32              ulFlag = 1;

    if (m_SocketLocal != INVALID_SOCKET)
    {
        return TRUE;
    }

    gos_init_socket();

    m_stLocalAddr.sin_family = AF_INET;
    m_stLocalAddr.sin_addr.s_addr = INADDR_ANY;
    m_stLocalAddr.sin_port = htons(m_usPort);

    m_SocketLocal = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_SocketLocal == INVALID_SOCKET)
    {
        GosLog(LOG_ERROR, "Create listen socket failed!");
        return FALSE;
    }

    if (setsockopt(m_SocketLocal, SOL_SOCKET, SO_REUSEADDR, (CHAR*)&ulFlag, sizeof(ulFlag)) == SOCKET_ERROR)
    {
        GosLog(LOG_ERROR, "set socket option failed!");
        CLOSE_SOCKET (m_SocketLocal);

        return FALSE;
    }

    if (bind (m_SocketLocal, (SOCKADDR *) &m_stLocalAddr, sizeof (SOCKADDR)) == SOCKET_ERROR)
    {
        GosLog(LOG_ERROR, "bind socket failed!");
        CLOSE_SOCKET(m_SocketLocal);

        return FALSE;
    }

    return TRUE;
}

VOID GTransFile::Listen()
{
    UINT16              usCurrPort;
    UINT32              ulTimeout = 2000;
    SOCKADDR_IN         stClientAddr = {0};
    INT32               iRecvSize;

    if (!Init())
    {
        return;
    }

    while(1)
    {
        usCurrPort = m_stLocalAddr.sin_port;
        if (usCurrPort == 0)
        {
            break;
        }

        memset(m_aucMsgBuf, 0, sizeof(m_aucMsgBuf));

        iRecvSize = gos_recv_udp_msg(m_SocketLocal, m_aucMsgBuf, sizeof(m_aucMsgBuf), ulTimeout, &stClientAddr);

        /* Ê§°Ü */
        if (iRecvSize < 0)
        {
            GosLog(LOG_ERROR, "recv failed when listen on GTransFile");
            break;
        }

        /* ³¬Ê± */
        if (iRecvSize == 0)
        {
            continue;
        }

        OnTransFile(&m_aucMsgBuf[0], iRecvSize, &stClientAddr);
    }

    CLOSE_SOCKET(m_SocketLocal);

    return;
}

VOID GTransFile::OnTransFile(UINT8 *pucMsgBuf, UINT32 ulMsgLen, SOCKADDR_IN *pstClientAddr)
{
    TRANS_FILE_REQ_T            *pstReq = (TRANS_FILE_REQ_T*)pucMsgBuf;
    TRANS_FILE_RSP_T            *pstRsp = &m_stRsp;
    HANDLE                      hFile = NULL;
    INT32                       iSize;
    CHAR                        acFile[256+512];
    INT64                       i64StartPos;
    UINT32                      ulRspLen = STRUCT_OFFSET(TRANS_FILE_RSP_T, aucData);

    memset(pstRsp, 0, sizeof(TRANS_FILE_RSP_T));

    if (ulMsgLen != sizeof(TRANS_FILE_REQ_T))
    {
        GosLog(LOG_ERROR, "invalid msg length(%u) of TRANS_FILE_REQ", ulMsgLen);
        return;
    }

    NTOHL_IT(pstReq->ulMagic);
    NTOHL_IT(pstReq->ulVer);
    NTOHL_IT(pstReq->ulMaxDataLen);

    if (pstReq->ulMaxDataLen > sizeof(pstRsp->aucData))
    {
        pstReq->ulMaxDataLen = sizeof(pstRsp->aucData);
    }
    else if (pstReq->ulMaxDataLen < 64)
    {
        pstReq->ulMaxDataLen = 64;
    }

    if (pstReq->ulMagic != TRANS_FILE_MAGIC)
    {
        return;
    }

    if (pstReq->ulVer != TRANS_FILE_VER)
    {
        return;
    }

    pstRsp->ulMagic = HTONL(pstReq->ulMagic);
    pstRsp->ulVer = HTONL(pstReq->ulVer);

    pstRsp->ulIndex = pstReq->ulIndex;

    pstRsp->ucRet = FALSE;
    pstRsp->ucIsEnd = FALSE;

    i64StartPos = (INT64)NTOHLL(pstReq->u64StartPos);
    if (i64StartPos < 0)
    {
        goto end;
    }

    if (m_bAbsolutePath)
    {
        strcpy(acFile, pstReq->acFile);
    }
    else
    {
        sprintf(acFile, "%s/%s", gos_get_root_path(), pstReq->acFile);
    }

    hFile = gos_fopen(acFile, "rb");
    if (!hFile)
    {
        GosLog(LOG_ERROR, "open file %s failed", acFile);
        goto end;
    }

    iSize = gos_fskip(hFile, i64StartPos);
    if (iSize < 0)
    {
        gos_fclose(hFile);
        goto end;
    }

    iSize = gos_fread(hFile, pstRsp->aucData, pstReq->ulMaxDataLen);

    gos_fclose(hFile);
    if (iSize < 0)
    {
        GosLog(LOG_ERROR, "read file %s failed", acFile);
        goto end;
    }

    pstRsp->ulDataLen = iSize;
    HTONL_IT(pstRsp->ulDataLen);
    pstRsp->ucRet = TRUE;
    if (iSize < (INT32)pstReq->ulMaxDataLen)
    {
        pstRsp->ucIsEnd = TRUE;
    }

    ulRspLen += iSize;

end:
    if (!gos_udp_send(m_SocketLocal, pstRsp, ulRspLen, pstClientAddr))
    {
        GosLog(LOG_ERROR, "udp send failed on TransFile");
    }
}

INT32 UDPSyncSend(SOCKET stSocket, SOCKADDR_IN *pstServerAddr, TRANS_FILE_REQ_T *pstReq, UINT32 ulReqLen, TRANS_FILE_RSP_T *pstRsp, UINT32 ulMaxRspLen, UINT32 ulTimeout)
{
    if (!gos_udp_send(stSocket, pstReq, ulReqLen, pstServerAddr))
    {
        GosLog(LOG_ERROR, "sync send failed: %s", gos_get_socket_err());
        return -1;
    }

    UINT32  ulStartTime = gos_get_uptime_1ms();

recv:
    UINT32  ulCurrTime = gos_get_uptime_1ms();

    if ((ulStartTime + ulTimeout) < ulCurrTime)
    {
        return 0;
    }

    INT32 iRecvLen = gos_recv_udp_msg(stSocket, pstRsp, ulMaxRspLen, 100, NULL);

    if (iRecvLen == 0)
    {
        goto recv;
    }

    if (iRecvLen < 0)
    {
        GosLog(LOG_ERROR, "sync send failed: %s", gos_get_socket_err());
        return iRecvLen;
    }

    if (iRecvLen < STRUCT_OFFSET(TRANS_FILE_RSP_T, aucData))
    {
        GosLog(LOG_ERROR, "sync send rsp data is too short(%u)", iRecvLen);
        return iRecvLen;
    }

    UINT32  ulRspIndex = NTOHL(pstRsp->ulIndex);
    UINT32  ulReqIndex = NTOHL(pstReq->ulIndex);

    if (ulRspIndex < ulReqIndex)
    {
        goto recv;
    }

    return iRecvLen;
}

BOOL TransFile(UINT8 *pucServerAddr, UINT16 usServerPort, CHAR *szFile, CHAR *szNewFile, UINT32 ulMaxDataLen, BOOL *pbCancel)
{
    TRANS_FILE_REQ_T    stReq = {0};
    TRANS_FILE_RSP_T    *pstRsp = NULL;
    CHAR                acFile[256];
    CHAR                acPath[256];
    HANDLE              hFile = NULL;
    CHAR                *szEnd;
    SOCKADDR_IN         stServerAddr = {0};
    SOCKET              stSocket = INVALID_SOCKET;
    BOOL                bRet = FALSE;
    UINT32              aulTimeout[] = {800, 700};
    UINT32              ulSendIndex;

    if (ulMaxDataLen > sizeof(pstRsp->aucData))
    {
        ulMaxDataLen = sizeof(pstRsp->aucData);
    }

    if (strlen(szFile) >= sizeof(stReq.acFile))
    {
        return FALSE;
    }

    pstRsp = (TRANS_FILE_RSP_T*)calloc(1, sizeof(TRANS_FILE_RSP_T));
    if (!pstRsp)
    {
        return FALSE;
    }

    if (!szNewFile)
    {
        strcpy(acFile, szFile);
    }
    else
    {
        strcpy(acFile, szNewFile);
    }

    strcpy(acPath, acFile);
    szEnd = acPath + strlen(acPath)-1;
    for (; szEnd>=acPath; szEnd--)
    {
        if (*szEnd == '\\' || *szEnd == '/')
        {
            *szEnd = '\0';
            break;
        }
    }

    if (!gos_file_exist(acPath))
    {
        gos_create_redir(acPath);
    }

    gos_format_path(acFile);
    hFile = gos_fopen(acFile, "w+b");
    if (!hFile)
    {
        GosLog(LOG_ERROR, "Open file(%s) faild", acFile);

        goto end;
    }

    strcpy(stReq.acFile, szFile);

    // socket
    gos_init_socket();

    stServerAddr.sin_family = AF_INET;
    stServerAddr.sin_port   = htons(usServerPort);
    memcpy(&stServerAddr.sin_addr.s_addr, pucServerAddr, 4);

    stSocket = socket (AF_INET, SOCK_DGRAM, 0);
    if (stSocket == INVALID_SOCKET)
    {
        goto end;
    }

    // file trans
    while(1)
    {
        if (pbCancel && *pbCancel == TRUE)
        {
            goto end;
        }

        stReq.ulMagic = HTONL(TRANS_FILE_MAGIC);
        stReq.ulVer = HTONL(TRANS_FILE_VER);
        stReq.ulMaxDataLen = HTONL(ulMaxDataLen);

        HTONL_IT(stReq.ulIndex);
        HTONLL_IT(stReq.u64StartPos);

        INT32 iRspLen = -1;

        for (ulSendIndex=0; ulSendIndex<ARRAY_SIZE(aulTimeout); ulSendIndex++)
        {
            iRspLen = UDPSyncSend(stSocket, &stServerAddr, &stReq, sizeof(stReq), pstRsp, sizeof(TRANS_FILE_RSP_T), aulTimeout[ulSendIndex]);

            if (iRspLen > 0)
            {
                break;
            }

            GosLog(LOG_ERROR, "sync send to server faild(%u)", aulTimeout[ulSendIndex]);
        }

        if (iRspLen <= 0)
        {
            goto end;
        }

        if (iRspLen < STRUCT_OFFSET(TRANS_FILE_RSP_T, aucData))
        {
            goto end;
        }

        NTOHL_IT(pstRsp->ulIndex);
        NTOHL_IT(pstRsp->ulDataLen);

        NTOHL_IT(stReq.ulIndex);
        NTOHLL_IT(stReq.u64StartPos);

        if (stReq.ulIndex != pstRsp->ulIndex )
        {
            GosLog(LOG_ERROR, "sync send rsp index mismatch(%u :: %u)", stReq.ulIndex, pstRsp->ulIndex);
            goto end;
        }

        if (iRspLen != (STRUCT_OFFSET(TRANS_FILE_RSP_T, aucData) + pstRsp->ulDataLen) )
        {
            GosLog(LOG_ERROR, "sync send rsp data length error(%u)", iRspLen);
            goto end;
        }

        if (pstRsp->ucRet != TRUE )
        {
            GosLog(LOG_ERROR, "sync send rsp failed");
            goto end;
        }

        if (pstRsp->ulDataLen > 0 && (INT32)pstRsp->ulDataLen != gos_fwrite(hFile, &pstRsp->aucData, pstRsp->ulDataLen))
        {
            GosLog(LOG_ERROR, "write to local file failed");
            goto end;
        }

        if (pstRsp->ulDataLen < ulMaxDataLen ||
            pstRsp->ucIsEnd)
        {
            break;
        }

        stReq.ulIndex++;
        stReq.u64StartPos += pstRsp->ulDataLen;
    }

    bRet = TRUE;

end:
    gos_fclose(hFile);

    CLOSE_SOCKET(stSocket);

    GOS_FREE(pstRsp);

    return bRet;
}

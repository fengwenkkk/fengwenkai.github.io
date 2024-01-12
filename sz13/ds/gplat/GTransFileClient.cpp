#include "g_include.h"
#include "GCommon.h"

#include "GTransFile.h"

GTransFileClient::GTransFileClient(UINT8 *pucServerAddr, UINT16 usServerPort)
{
    m_ulMaxDataLen = 1024*4;
    m_ulResendPeriod = 3000;
    m_ulBufNum = ARRAY_SIZE(m_astRecvBuf);

    InitSocket(pucServerAddr, usServerPort);
}

GTransFileClient::~GTransFileClient()
{
    CLOSE_SOCKET(m_stSocket);
}

VOID GTransFileClient::SetMaxDataLen(UINT32 ulMaxDataLen)
{
    m_ulMaxDataLen = ulMaxDataLen;
    if (m_ulMaxDataLen > MAX_TRANS_FILE_DATE_LEN)
    {
        m_ulMaxDataLen = MAX_TRANS_FILE_DATE_LEN;
    }
}

VOID GTransFileClient::SetBufNum(UINT32 ulBufNum)
{
    m_ulBufNum = ulBufNum;
    if (m_ulBufNum > ARRAY_SIZE(m_astRecvBuf))
    {
        m_ulBufNum = ARRAY_SIZE(m_astRecvBuf);
    }
}

BOOL GTransFileClient::Init()
{
    m_hFile = NULL;

    memset(m_astRecvBuf, 0, sizeof(m_astRecvBuf));

    for (UINT32 i=0; i<m_ulBufNum; i++)
    {
        m_astRecvBuf[i].ulIndex = i;
        m_astRecvBuf[i].ulExpectDataLen = m_ulMaxDataLen;
    }

    m_ulNextWriteIndex = 0;
    m_ulNextSendIndex = m_ulBufNum;

    m_ulSendPkgNum = 0;
    m_ulRecvPkgNum = 0;
    m_ulLastIndex = 0xffffffff;

    m_dCurrDropRate = 0;
    m_dCurrResendRate = 0;

    m_ulLastSendPkgNum = 0;
    m_ulLastRecvPkgNum = 0;

    return TRUE;
}

VOID GTransFileClient::InitSocket(UINT8 *pucServerAddr, UINT16 usServerPort)
{
    // socket
    gos_init_socket();

    m_stServerAddr.sin_family = AF_INET;
    m_stServerAddr.sin_port   = htons(usServerPort);
    memcpy(&m_stServerAddr.sin_addr.s_addr, pucServerAddr, 4);

    m_stSocket = socket (AF_INET, SOCK_DGRAM, 0);
}

VOID GTransFileClient::TransOver()
{
    gos_fclose(m_hFile);
    m_hFile = NULL;
}

BOOL GTransFileClient::DownloadFile(CHAR *szFile, CHAR *szNewFile)
{
    Init();

    BOOL    bRet = _DownloadFile(szFile, szNewFile);

    TransOver();

    return bRet;
}

BOOL GTransFileClient::_DownloadFile(CHAR *szFile, CHAR *szNewFile)
{
    CHAR        acPath[260];
    CHAR        acFile[260];
    CHAR        *szEnd;
    BOOL        bRet = FALSE;

    if (strlen(szFile) >= sizeof(m_stReq.acFile))
    {
        return FALSE;
    }

    if (!szNewFile)
    {
        sprintf(acFile, "%s/%s", gos_get_root_path(), szFile);
    }
    else
    {
        sprintf(acFile, "%s/%s", gos_get_root_path(), szNewFile);
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
    m_hFile = gos_fopen(acFile, "w+b");
    if (!m_hFile)
    {
        GosLog(LOG_ERROR, "Open file(%s) faild", acFile);

        return FALSE;
    }

    memset(&m_stReq, 0, sizeof(m_stReq));
    strcpy(m_stReq.acFile, szFile);

    TRANS_FILE_RECV_BUF_T   *pstBuf;
    std::vector<TRANS_FILE_RECV_BUF_T*> vBuf;
    UINT32                  ulDeltaSendPkgNum;
    UINT32                  ulDeltaRecvPkgNum;
    DOUBLE                  dCurrDropRate;
    DOUBLE                  dCurrResendRate;
    UINT32                  i;

    while(1)
    {
        GetSendInfo(vBuf);
        for (i=0; i<vBuf.size(); i++)
        {
            Send(vBuf[i]);
        }

        while (Recv() <= 0)
        {
            break;
        }

        if (IsOver())
        {
            GosLog(LOG_DETAIL, "download file %s over", szFile);
            bRet = TRUE;
            break;
        }

        if (m_hFile == NULL)
        {
            GosLog(LOG_ERROR, "download file %s failed", szFile);
            break;
        }
    }

    TransOver();

    return bRet;
}

BOOL GTransFileClient::IsOver()
{
    BOOL    bOver = TRUE;

    if (m_ulLastIndex == 0xffffffff)
    {
        return FALSE;
    }

    for (UINT32 i=0; i<m_ulBufNum; i++)
    {
        if (m_astRecvBuf[i].ulIndex > m_ulLastIndex)
        {
            continue;
        }

        if (m_astRecvBuf[i].ulIndex != 0xffffffff)
        {
            bOver = FALSE;
            break;
        }
    }

    return bOver;
}

VOID GTransFileClient::GetSendInfo(std::vector<TRANS_FILE_RECV_BUF_T*> &vBuf)
{
    UINT32                  ulCurrTime = gos_get_uptime_1ms();
    TRANS_FILE_RECV_BUF_T   *pstBuf = NULL;

    vBuf.clear();

    for (UINT32 i=0; i<m_ulBufNum; i++)
    {
        if (m_astRecvBuf[i].ulIndex == 0xffffffff)
        {
            continue;
        }

        if (m_astRecvBuf[i].ulSendNum == 0 ||
            (m_astRecvBuf[i].ulExpectDataLen != m_astRecvBuf[i].ulRecvDataLen &&
            (ulCurrTime - m_astRecvBuf[i].ulSendTime) > m_ulResendPeriod) )
        {
            pstBuf = &m_astRecvBuf[i];
            pstBuf->ulSendNum++;
            pstBuf->ulSendTime = ulCurrTime;

            vBuf.push_back(pstBuf);
        }
    }
}

VOID GTransFileClient::Send(TRANS_FILE_RECV_BUF_T* pstBuf)
{
    if (!pstBuf)
    {
        return;
    }

    m_stReq.ulIndex = pstBuf->ulIndex;
    m_stReq.ulMaxDataLen = pstBuf->ulExpectDataLen;
    m_stReq.u64StartPos = ((UINT64)m_stReq.ulIndex)*m_stReq.ulMaxDataLen;

    m_stReq.ulMagic = HTONL(TRANS_FILE_MAGIC);
    m_stReq.ulVer = HTONL(TRANS_FILE_VER);

    HTONL_IT(m_stReq.ulMaxDataLen);
    HTONL_IT(m_stReq.ulIndex);
    HTONLL_IT(m_stReq.u64StartPos);

    gos_udp_send(m_stSocket, &m_stReq, sizeof(m_stReq), &m_stServerAddr);
    m_ulSendPkgNum++;
}

INT32 GTransFileClient::Recv()
{
    UINT32  ulTimeout = 100;
    INT32   iRecvLen = gos_recv_udp_msg(m_stSocket, &m_stRsp, sizeof(m_stRsp), ulTimeout, NULL);

    if (iRecvLen == 0)
    {
        return 1;
    }

    if (iRecvLen < 0)
    {
        return -1;
    }

    if (iRecvLen > 0)
    {
        m_ulRecvPkgNum ++;
    }

    if (iRecvLen < STRUCT_OFFSET(TRANS_FILE_RSP_T, aucData))
    {
        GosLog(LOG_ERROR, "rsp data is too short(%u)", iRecvLen);
        return -1;
    }

    UINT32  ulRspIndex = NTOHL(m_stRsp.ulIndex);

    NTOHL_IT(m_stRsp.ulDataLen);
    NTOHL_IT(m_stRsp.ulIndex);

    if (iRecvLen != (STRUCT_OFFSET(TRANS_FILE_RSP_T, aucData) + m_stRsp.ulDataLen) )
    {
        GosLog(LOG_ERROR, "rsp data length error(%u)", iRecvLen);
        return -1;
    }

    if (m_stRsp.ucRet != TRUE)
    {
        GosLog(LOG_ERROR, "return failed");
        TransOver();

        return -1;
    }

    TRANS_FILE_RECV_BUF_T   *pstBuf = NULL;

    for (UINT32 i=0; i<m_ulBufNum; i++)
    {
        if (m_astRecvBuf[i].ulIndex == ulRspIndex)
        {
            pstBuf = &m_astRecvBuf[i];
            break;
        }
    }

    if (!pstBuf)
    {
        GosLog(LOG_DETAIL, "invalid rsp index %u", m_stRsp.ulIndex);
        return -1;
    }

    if (!m_stRsp.ucIsEnd &&
        m_stRsp.ulDataLen != pstBuf->ulExpectDataLen)
    {
        GosLog(LOG_ERROR, "invalid rsp data len %u", m_stRsp.ulDataLen);
        return -1;
    }

    pstBuf->bRecvOver = TRUE;
    pstBuf->ulRecvDataLen = m_stRsp.ulDataLen;

    if (m_stRsp.ulDataLen > 0)
    {
        memcpy(pstBuf->aucData, m_stRsp.aucData, m_stRsp.ulDataLen);
    }

    if (m_stRsp.ucIsEnd)
    {
        m_ulNextSendIndex = 0;
        m_ulLastIndex = m_stRsp.ulIndex;
    }

    SaveData();

    return 0;
}

VOID GTransFileClient::SaveData()
{
    TRANS_FILE_RECV_BUF_T   *pstBuf;

check:
    pstBuf = NULL;
    for (UINT32 i=0; i<m_ulBufNum; i++)
    {
        if (m_astRecvBuf[i].ulIndex == m_ulNextWriteIndex)
        {
            if (m_astRecvBuf[i].bRecvOver)
            {
                pstBuf = &m_astRecvBuf[i];
            }

            break;
        }
    }

    if (!pstBuf)
    {
        if (m_ulNextWriteIndex > m_ulLastIndex)
        {
            TransOver();
        }

        return;
    }

    if (pstBuf->ulRecvDataLen > 0)
    {
        if ((INT32)pstBuf->ulRecvDataLen != gos_fwrite(m_hFile, pstBuf->aucData, pstBuf->ulRecvDataLen))
        {
            GosLog(LOG_ERROR, "write to local file failed");
            TransOver();

            return;
        }
    }
    else
    {
        if (m_ulNextSendIndex > 0)
        {
            GosLog(LOG_ERROR, "error data len of index %u", pstBuf->ulIndex);
            TransOver();

            return;
        }
    }

    if (m_ulNextSendIndex > 0)
    {
        pstBuf->ulIndex = m_ulNextSendIndex++;
    }
    else
    {
        pstBuf->ulIndex = 0xffffffff;
    }

    pstBuf->ulSendTime = 0;
    pstBuf->ulSendNum = 0;
    pstBuf->bRecvOver = FALSE;
    pstBuf->ulExpectDataLen = m_ulMaxDataLen;
    pstBuf->ulRecvDataLen = 0;

    m_ulNextWriteIndex++;

    goto check;
}

DOUBLE GTransFileClient::GetDropRate()
{
    if (m_ulSendPkgNum == 0)
    {
        return 0;
    }

    return 1 - m_ulRecvPkgNum/((DOUBLE)m_ulSendPkgNum);
}

DOUBLE GTransFileClient::GetResendRate()
{
    if (m_ulLastIndex == 0xffffffff)
    {
        return 0;
    }

    return m_ulSendPkgNum/((DOUBLE)m_ulLastIndex+1) - 1;
}

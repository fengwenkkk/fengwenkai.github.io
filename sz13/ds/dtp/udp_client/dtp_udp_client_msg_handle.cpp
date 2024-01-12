#include "g_include.h"

#include "dtp_public.h"
#include "dtp_common.h"
#include "dtp_func.h"
#include "dtp_udp_client.h"

#define DTPClientImp    DTPUDPClient

BOOL DTPClientImp::HandleLinkCheckRsp(DTP_LINK_CHECK_RSP_T *pstRsp)
{
    gos_mutex(m_pstClient->hLinkStatMutex);

    m_pstClient->ulLinkHelloReqNum = 0;

    gos_unmutex(m_pstClient->hLinkStatMutex);

    return TRUE;
}

BOOL DTPClientImp::HandleLinkCheckReq(DTP_LINK_CHECK_REQ_T *pstReq)
{
    DTP_LINK_CHECK_RSP_T    stRsp = {0};

    stRsp.stMsgHdr.ulMsgMagic= DTP_MSG_MAGIC;
    stRsp.stMsgHdr.usMsgVer  = DTP_MSG_VER;
    stRsp.stMsgHdr.ulMsgLen  = sizeof(stRsp);
    stRsp.stMsgHdr.usMsgType = DTP_EV_LINK_CHECK_RSP;
    stRsp.stMsgHdr.ulSeqID   = pstReq->stMsgHdr.ulSeqID;
    stRsp.stMsgHdr.ucMsgPri  = m_pstClient->ucMsgPri;

    dtp_msg_hton(&stRsp.stMsgHdr);

    SOCKADDR_IN     stServerAddr = {0};

    stServerAddr.sin_family = AF_INET;
    stServerAddr.sin_port   = HTONS(m_pstClient->usServerPort);
    memcpy(&stServerAddr.sin_addr.s_addr, m_pstClient->aucServerAddr, 4);

    if (!SocketSend(m_pstClient->stSocket, &stRsp, sizeof(stRsp), &stServerAddr, m_pstClient->bSendThreadRunning))
    {
        DTPLog(LOG_FATAL, "client_auth_check: send msg failed");
        return FALSE;
    }

    return TRUE;
}

BOOL DTPClientImp::HandleLinkReleaseRsp(DTP_LINK_RELEASE_RSP_T *pstRsp)
{
    if (pstRsp->stMsgHdr.usMsgType != DTP_EV_LINK_RELEASE_RSP ||
        pstRsp->stMsgHdr.ulMsgLen  != sizeof(DTP_LINK_RELEASE_RSP_T) ||
        pstRsp->stMsgHdr.ulSeqID   >  m_pstClient->ulSeqID ||
        pstRsp->ucResult != DTP_LINK_RELEASE_OK)
    {
        DTPLog(LOG_FATAL, "client_release_link: link release failed");
        return FALSE;
    }
    else
    {
        DTPLog(LOG_INFO, "client_release_link: link release successful");
    }

    return TRUE;
}

BOOL DTPClientImp::HandleRecvMsg(DTP_MSG_T *pstMsg)
{
    UINT32              i;
    UINT32              ulMsgLen;

    if (!m_pstClient || !pstMsg || pstMsg->stMsgHdr.ulMsgLen < sizeof(DTP_MSG_HDR_T))
    {
        return FALSE;
    }

    dtp_msg_ntoh(&pstMsg->stMsgHdr);

    if (pstMsg->stMsgHdr.usMsgType == DTP_EV_LINK_CHECK_RSP)
    {
        return HandleLinkCheckRsp((DTP_LINK_CHECK_RSP_T*)pstMsg);
    }
    else if (pstMsg->stMsgHdr.usMsgType == DTP_EV_LINK_CHECK_REQ)
    {
        return HandleLinkCheckReq((DTP_LINK_CHECK_REQ_T*)pstMsg);
    }
    else if (pstMsg->stMsgHdr.usMsgType == DTP_EV_LINK_RELEASE_RSP)
    {
        return HandleLinkReleaseRsp((DTP_LINK_RELEASE_RSP_T*)pstMsg);
    }

    ulMsgLen = pstMsg->stMsgHdr.ulMsgLen - sizeof(DTP_MSG_HDR_T);

    // 处理同步消息
    if (m_pstClient->hSyncMsgMutex)
    {
        if (!gos_mutex(m_pstClient->hSyncMsgMutex))
        {
            return FALSE;
        }

        for (i=0; i<ARRAY_SIZE(m_pstClient->astSyncMsg); i++)
        {
            if (m_pstClient->astSyncMsg[i].bUsed &&
                m_pstClient->astSyncMsg[i].ulMsgSn == pstMsg->stMsgHdr.ulSeqID)
            {
                m_pstClient->astSyncMsg[i].bUsed = FALSE;

                if (ulMsgLen > m_pstClient->astSyncMsg[i].ulMaxRspMsgLen)
                {
                    gos_unmutex(m_pstClient->hSyncMsgMutex);
                    return FALSE;
                }

                memcpy(m_pstClient->astSyncMsg[i].pstRspMsg, pstMsg->aucMsgBody, ulMsgLen);
                if (m_pstClient->astSyncMsg[i].pulRspMsgLen)
                {
                    *m_pstClient->astSyncMsg[i].pulRspMsgLen = ulMsgLen;
                }

                gos_sem_v(m_pstClient->astSyncMsg[i].hSem);

                gos_unmutex(m_pstClient->hSyncMsgMutex);
                return TRUE;
            }
        }

        gos_unmutex(m_pstClient->hSyncMsgMutex);
    }

    m_pMsgHandle->MsgHandle(m_pstClient->usServerID, pstMsg);

    return TRUE;
}

UINT16 DTPClientImp::GetServerID()
{
    return m_pstClient->usServerID;
}

VOID DTPClientImp::SetServerID(UINT16 usServerID)
{
    m_pstClient->usServerID = usServerID;
}

/*******************************************************************************
* 函数名称: AuthLink
* 功    能: 客户端认证链路
* 参    数:
* 参数名称              输入/输出   描述
* 函数返回:
* 说    明:
*******************************************************************************/
BOOL DTPClientImp::AuthLink()
{
    INT32                   iTimeout = 5000;
    INT32                   iRecvMsgLen;
    DTP_UDP_LINK_AUTH_REQ_T stReq;
    DTP_LINK_AUTH_RSP_T     stRsp;
    UINT32                  ulStartTime = gos_get_uptime_1ms();

    memset(&stReq, 0, sizeof(stReq));
    memset(&stRsp, 0, sizeof(stRsp));

    stReq.stMsgHdr.ulMsgMagic= DTP_MSG_MAGIC;
    stReq.stMsgHdr.usMsgVer  = DTP_MSG_VER;
    stReq.stMsgHdr.ulMsgLen  = sizeof(stReq);
    stReq.stMsgHdr.usMsgType = DTP_EV_LINK_AUTH_REQ;
    stReq.stMsgHdr.ulSeqID   = _GetSeqID();
    stReq.stMsgHdr.ucMsgPri  = m_pstClient->ucMsgPri;
    stReq.stMsgHdr.ucSenderType  = m_pstClient->ucSenderType;

    memcpy(stReq.aucClientAddr, m_pstClient->aucClientAddr, sizeof(stReq.aucClientAddr));
    stReq.usClientPort = HTONS(m_pstClient->usClientPort);

    dtp_msg_hton(&stReq.stMsgHdr);

    if (!gos_mutex(m_pstClient->hMsgQueueMutex))
    {
        return FALSE;
    }

    SOCKADDR_IN     stServerAddr = {0};

    stServerAddr.sin_family = AF_INET;
    stServerAddr.sin_port   = HTONS(m_pstClient->usServerPort);
    memcpy(&stServerAddr.sin_addr.s_addr, m_pstClient->aucServerAddr, 4);

    if (!SocketSend(m_pstClient->stSocket, &stReq, sizeof(stReq), &stServerAddr, FALSE))
    {
        DTPLog(LOG_FATAL, "client_auth_link: send msg failed");
        gos_unmutex(m_pstClient->hMsgQueueMutex);
        return FALSE;
    }

again:
    UINT32  ulTime = gos_get_uptime_1ms() - ulStartTime;
    INT32   iWaitTime = iTimeout - ulTime;

    if (iWaitTime <= 0)
    {
        DTPLog(LOG_FATAL, "client_auth_link: recv msg timeout");

        gos_unmutex(m_pstClient->hMsgQueueMutex);
        return FALSE;
    }

    iRecvMsgLen = dtp_udp_recv(m_pstClient->stSocket, &stRsp, sizeof(stRsp), iWaitTime);

    if (0 == iRecvMsgLen)
    {
        goto again;
    }
    else if (iRecvMsgLen < 0)
    {
        DTPLog(LOG_FATAL, "client_auth_link: recv msg failed");

        gos_unmutex(m_pstClient->hMsgQueueMutex);
        return FALSE;
    }

    gos_unmutex(m_pstClient->hMsgQueueMutex);
    if ((UINT32)iRecvMsgLen != sizeof(stRsp))
    {
        DTPLog(LOG_FATAL, "client_auth_link: msg len error: %d", iRecvMsgLen);
        return FALSE;
    }

    dtp_msg_ntoh(&stRsp.stMsgHdr);
    NTOHS_IT(stRsp.usServerID);
    NTOHS_IT(stRsp.usClientID);
    NTOHS_IT(stRsp.usClientPort);

    if (stRsp.stMsgHdr.usMsgType != DTP_EV_LINK_AUTH_RSP )
    {
        DTPLog(LOG_FATAL, "client_auth_link: link auth failed, invalid msg type");
        return FALSE;
    }

    if (stRsp.stMsgHdr.ulMsgLen != sizeof(DTP_LINK_AUTH_RSP_T) )
    {
        DTPLog(LOG_FATAL, "client_auth_link: link auth failed, invalid msg len %u", stRsp.stMsgHdr.ulMsgLen);
        return FALSE;
    }

    if (stRsp.stMsgHdr.ulSeqID   >  m_pstClient->ulSeqID )
    {
        DTPLog(LOG_FATAL, "client_auth_link: link auth failed, invalid seq id(%u, %u)", stRsp.stMsgHdr.ulSeqID, m_pstClient->ulSeqID);
        return FALSE;
    }

    if (stRsp.ucResult != DTP_LINK_AUTH_OK )
    {
        DTPLog(LOG_FATAL, "client_auth_link: link auth failed");
        return FALSE;
    }

    m_pstClient->usClientID = stRsp.usClientID;
    DTPLog(LOG_FATAL, "client_auth_link: link auth successful, client id is %u", m_pstClient->usClientID);

    return TRUE;
}

/*******************************************************************************
* 函数名称: ReleaseLink
* 功    能: 客户端释放链路
* 参    数:
* 参数名称              输入/输出   描述
* 函数返回:
* 说    明:
*******************************************************************************/
BOOL DTPClientImp::ReleaseLink()
{
    if (NULL == m_pstClient || m_pstClient->stSocket == INVALID_SOCKET)
    {
        return FALSE;
    }

    UINT32 ulSeqID = _GetSeqID();

    if (!Send(0xffff, 0xffff, DTP_EV_LINK_RELEASE_REQ, ulSeqID, NULL, 0))
    {
        DTPLog(LOG_FATAL, "client_release_link: send msg failed");
        return FALSE;
    }

    gos_sleep_1ms(500);
    return TRUE;
}

#undef DTPClientImp

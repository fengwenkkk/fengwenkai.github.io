#include "g_include.h"

#include "dtp_public.h"
#include "dtp_common.h"
#include "dtp_func.h"
#include "dtp_tcp_server.h"

#define DTPServerImp    DTPTCPServer

/*******************************************************************************
* 函数名称: _CheckClient
* 功    能: 检验客户端标识
* 参    数:
* 参数名称              输入/输出   描述
* usLoginID             IN          客户端标识
* 函数返回:
* 说    明:
*******************************************************************************/
BOOL DTPServerImp::_CheckClient(UINT16 usLoginID)
{
    return TRUE;
}

// UDP 0x8000 -> 0xffff, TCP 0x100 -> 0x7fff
BOOL DTPServerImp::AllocClientID(DTP_CONN_INFO_T *pstConn)
{
    UINT32              i;
    BOOL                bExist = FALSE;
    UINT32              ulClientNum = 0;
    UINT16              ausClientID[DTP_MAX_CONN_NUM];

    if (pstConn->usClientID != DTP_INVALID_INST_ID)
    {
        return TRUE;
    }

    memset(ausClientID, 0, sizeof(ausClientID));

    for (i=0; i<ARRAY_SIZE(m_pstServer->astConnInfo); i++)
    {
        if (m_pstServer->astConnInfo[i].stSocket != INVALID_SOCKET &&
            m_pstServer->astConnInfo[i].stSocket != pstConn->stSocket )
        {
            ausClientID[ulClientNum++] = m_pstServer->astConnInfo[i].usClientID;
        }
    }

    pstConn->usClientID = (1<<8) | (UINT8)NTOHL(pstConn->stClientSendAddr.sin_addr.s_addr);
    bExist = FALSE;
    for (i=0; i<ulClientNum; i++)
    {
        if (ausClientID[i] == pstConn->usClientID)
        {
            bExist = TRUE;
            break;
        }
    }

    if (!bExist)
    {
        return TRUE;
    }

    for (pstConn->usClientID=512; pstConn->usClientID<0x7fff; pstConn->usClientID++)
    {
        bExist = FALSE;
        for (i=0; i<ulClientNum; i++)
        {
            if (ausClientID[i] == pstConn->usClientID)
            {
                bExist = TRUE;
                break;
            }
        }

        if (!bExist)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*******************************************************************************
* 函数名称: HandleLinkAuthReqMsg
* 功    能: 处理客户端的链路鉴权请求消息
* 参    数:
* 参数名称              输入/输出   描述
* pstConn               IN          客户端连接信息
* pstMsg                IN          请求消息
* 函数返回:
* 说    明:
*******************************************************************************/
BOOL DTPServerImp::HandleLinkAuthReqMsg(DTP_CONN_INFO_T *pstConn, DTP_TCP_LINK_AUTH_REQ_T *pstReq)
{
    DTP_LINK_AUTH_RSP_T     stRsp;
    BOOL                    bRet;

    memset(&stRsp, 0, sizeof(stRsp));

    DTPLog(LOG_INFO, "server_handle_link_auth_req_msg: client = " IP_FMT "(%d)",
        IP_ARG(&pstConn->stClientSendAddr.sin_addr.s_addr), NTOHS(pstConn->stClientSendAddr.sin_port));

    pstConn->ucClientType = pstReq->stMsgHdr.ucSenderType;

    stRsp.stMsgHdr.ulMsgMagic= DTP_MSG_MAGIC;
    stRsp.stMsgHdr.usMsgVer = DTP_MSG_VER;
    stRsp.stMsgHdr.ulMsgLen  = sizeof(stRsp);
    stRsp.stMsgHdr.usMsgType = DTP_EV_LINK_AUTH_RSP;
    stRsp.stMsgHdr.ulSeqID   = pstReq->stMsgHdr.ulSeqID;
    stRsp.usServerID = HTONS(m_pstServer->usInstID);
    memcpy(stRsp.aucClientAddr, &pstConn->stClientSendAddr.sin_addr.s_addr, sizeof(stRsp.aucClientAddr));
    stRsp.usClientPort = pstConn->stClientSendAddr.sin_port; //  HTONS(pstConn->stClientSendAddr.sin_port);
    stRsp.ucResult = DTP_LINK_AUTH_OK;

    if (!AllocClientID(pstConn))
    {
        DTPLog(LOG_INFO, "server_handle_link_auth_req_msg: alloc client id(%u) failed", pstConn->stSocket);
        return FALSE;
    }

    stRsp.usClientID = HTONS(pstConn->usClientID);

    switch (pstConn->enClientStatus)
    {
        case DTP_CLIENT_CONNECTED:
            if (!_CheckClient(pstConn->usClientID))
            {
                stRsp.ucResult = DTP_LINK_AUTH_INVALID_LOGINID;

                DTPLog(LOG_INFO, "server_handle_link_auth_req_msg: invalid login id(%d)", pstConn->usClientID);
            }
            else
            {
                pstConn->enClientStatus = DTP_CLIENT_LINKAUTHED;
            }

            break;

        case DTP_CLIENT_LINKAUTHED:
            break;

        case DTP_CLIENT_IDLE:
        default:
            return FALSE;
    }

    if (stRsp.ucResult == DTP_LINK_AUTH_OK)
    {
        DTPLog(LOG_INFO, "link auth successful, client(%d) = " IP_FMT "(%d)",
            pstConn->usClientID, IP_ARG(&pstConn->stClientSendAddr.sin_addr.s_addr), NTOHS(pstConn->stClientSendAddr.sin_port));
    }
    else
    {
        DTPLog(LOG_FATAL, "link auth failed, client(%d) = " IP_FMT "(%d)",
            pstConn->usClientID, IP_ARG(&pstConn->stClientSendAddr.sin_addr.s_addr), NTOHS(pstConn->stClientSendAddr.sin_port));
    }

    bRet = SendBySocket(pstConn->stSocket, &stRsp.stMsgHdr);

    if (bRet)
    {
        m_pMsgHandle->OnAcceptClient(pstConn->usClientID);
    }

    return bRet;
}

/*******************************************************************************
* 函数名称: HandleLinkCheckReqMsg
* 功    能: 处理客户端的链路检查请求消息
* 参    数:
* 参数名称              输入/输出   描述
* pstConn               IN          客户端连接信息
* pstMsg                IN          请求消息
* 函数返回:
* 说    明:
*******************************************************************************/
BOOL DTPServerImp::HandleLinkCheckReqMsg(DTP_CONN_INFO_T *pstConn, DTP_MSG_T *pstReq)
{
    DTP_LINK_CHECK_RSP_T    stRsp;

    memset(&stRsp, 0, sizeof(stRsp));

    stRsp.stMsgHdr.ulMsgMagic= DTP_MSG_MAGIC;
    stRsp.stMsgHdr.usMsgVer  = DTP_MSG_VER;
    stRsp.stMsgHdr.ulMsgLen  = sizeof(stRsp);
    stRsp.stMsgHdr.usMsgType = DTP_EV_LINK_CHECK_RSP;
    stRsp.stMsgHdr.ulSeqID   = pstReq->stMsgHdr.ulSeqID;

    return SendBySocket(pstConn->stSocket, &stRsp.stMsgHdr);
}

/*******************************************************************************
* 函数名称: HandleLinkReleaseReqMsg
* 功    能: 处理客户端的链路释放请求消息
* 参    数:
* 参数名称              输入/输出   描述
* pstConn               IN          客户端连接信息
* pstMsg                IN          请求消息
* 函数返回:
* 说    明:
*******************************************************************************/
BOOL DTPServerImp::HandleLinkReleaseReqMsg(DTP_CONN_INFO_T *pstConn, DTP_LINK_RELEASE_REQ_T *pstReq)
{
    DTP_LINK_RELEASE_RSP_T  stRsp;

    memset(&stRsp, 0, sizeof(stRsp));

    stRsp.stMsgHdr.ulMsgLen  = sizeof(stRsp);
    stRsp.stMsgHdr.usMsgType = DTP_EV_LINK_RELEASE_RSP;
    stRsp.stMsgHdr.ulSeqID   = pstReq->stMsgHdr.ulSeqID;

    stRsp.ucResult = DTP_LINK_RELEASE_OK;

    SendBySocket(pstConn->stSocket, &stRsp.stMsgHdr);

    DTPLog(LOG_INFO, "server_handle_link_release_req_msg: link release successful, client = " IP_FMT "(%d)",
        IP_ARG(&pstConn->stClientSendAddr.sin_addr.s_addr), NTOHS(pstConn->stClientSendAddr.sin_port));

    gos_sleep_1ms(200);

    _InitConnectInfo(pstConn->stSocket, FALSE);

    return TRUE;
}

/*******************************************************************************
* 函数名称: HandleClientMsg
* 功    能: 处理客户端的请求消息
* 参    数:
* 参数名称              输入/输出   描述
* stClientSocket        IN          accept到底TCP客户端socket
* pstMsgHdr             IN          客户端请求消息
* ulMsgLen              IN          消息长度
* 函数返回:
* 说    明:
*******************************************************************************/
BOOL DTPServerImp::HandleClientMsg(SOCKET stClientSocket, DTP_MSG_T *pstMsg)
{
    UINT16              usMsgType;
    UINT32              i;
    DTP_CONN_INFO_T     *pstConn = NULL;
    UINT32              ulMsgLen;

    usMsgType = pstMsg->stMsgHdr.usMsgType;

    /* 根据socket获取连接信息 */
    for (i=0; i<ARRAY_SIZE(m_pstServer->astConnInfo); i++)
    {
        if (stClientSocket == m_pstServer->astConnInfo[i].stSocket)
        {
            pstConn = &m_pstServer->astConnInfo[i];
            break;
        }
    }

    if (!pstConn)
    {
        DTPLog(LOG_ERROR, "server_handle_client_msg: invalid client socket %u", stClientSocket);
        return FALSE;
    }

    pstConn->ulLastRecvTime = gos_get_uptime();

    pstConn->ucClientType = pstMsg->stMsgHdr.ucSenderType;

    /* 以下处理客户端启动流程中相关的请求消息 */
    switch(usMsgType)
    {
        case DTP_EV_LINK_AUTH_REQ:
            return HandleLinkAuthReqMsg(pstConn, (DTP_TCP_LINK_AUTH_REQ_T*)pstMsg);

        case DTP_EV_LINK_RELEASE_REQ:
            return HandleLinkReleaseReqMsg(pstConn, (DTP_LINK_RELEASE_REQ_T*)pstMsg);
    }

    ulMsgLen = pstMsg->stMsgHdr.ulMsgLen - sizeof(DTP_MSG_HDR_T);
    if (m_pstServer->hSyncMsgMutex)
    {
        if (!gos_mutex(m_pstServer->hSyncMsgMutex))
        {
            return FALSE;
        }

        for (i=0; i<ARRAY_SIZE(m_pstServer->astSyncMsg); i++)
        {
            if (m_pstServer->astSyncMsg[i].bUsed &&
                m_pstServer->astSyncMsg[i].ulMsgSn == pstMsg->stMsgHdr.ulSeqID)
            {
                m_pstServer->astSyncMsg[i].bUsed = FALSE;

                if (ulMsgLen > m_pstServer->astSyncMsg[i].ulMaxRspMsgLen)
                {
                    gos_unmutex(m_pstServer->hSyncMsgMutex);
                    return FALSE;
                }

                memcpy(m_pstServer->astSyncMsg[i].pstRspMsg, pstMsg->aucMsgBody, ulMsgLen);
                if (m_pstServer->astSyncMsg[i].pulRspMsgLen)
                {
                    *m_pstServer->astSyncMsg[i].pulRspMsgLen = ulMsgLen;
                }

                gos_sem_v(m_pstServer->astSyncMsg[i].hSem);

                gos_unmutex(m_pstServer->hSyncMsgMutex);
                return TRUE;
            }
        }

        gos_unmutex(m_pstServer->hSyncMsgMutex);
    }

    /* 以下处理客户端启动后的业务请求消息 */
    if (pstConn->enClientStatus == DTP_CLIENT_CONNECTED ||
        pstConn->enClientStatus == DTP_CLIENT_LINKSETUP )
    {
        switch(usMsgType)
        {
            case DTP_EV_LINK_CHECK_REQ:
                return HandleLinkCheckReqMsg(pstConn, pstMsg);

            case DTP_EV_LINK_CHECK_RSP:
                return TRUE;

            default:
                m_pMsgHandle->MsgHandle(pstConn->usClientID, pstMsg);

                return TRUE;
        }
    }

    DTPLog(LOG_WARN, "handle_client_msg: recv msg type %d under status %u", usMsgType, pstConn->enClientStatus);

    if (DTP_CLIENT_CONNECTED == pstConn->enClientStatus)
    {
        _InitConnectInfo(pstConn->stSocket, FALSE);
    }

    return FALSE;
}

#undef DTPServerImp

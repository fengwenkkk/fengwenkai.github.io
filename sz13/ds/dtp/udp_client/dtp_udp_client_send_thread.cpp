#include "g_include.h"

#include "dtp_public.h"
#include "dtp_common.h"
#include "dtp_func.h"
#include "dtp_udp_client.h"

#define DTPClientImp    DTPUDPClient

UINT32 DTPClientImp::_GetSeqID()
{
    UINT32          ulSeqID = 0;

    if (NULL == m_pstClient)
    {
        return 0;
    }

    gos_mutex(m_pstClient->hSeqIDMutex);

    // TCP序列号为奇数，UDP为偶数
    m_pstClient->ulSeqID += 2;
    if (m_pstClient->ulSeqID < 2)
    {
        m_pstClient->ulSeqID = 2;
    }

    ulSeqID = m_pstClient->ulSeqID;
    gos_unmutex(m_pstClient->hSeqIDMutex);

    return ulSeqID;
}

BOOL DTPClientImp::CheckLink()
{
    if (!IsConnected())
    {
        return FALSE;
    }

    UINT32  ulSeqID = _GetSeqID();

    DTPLog(LOG_DETAIL, "link checking...");

    return Send(0xffff, 0xffff, DTP_EV_LINK_CHECK_REQ, ulSeqID, NULL, 0);
}

/*******************************************************************************
* 函数名称: AllocMsgNode
* 功    能: 申请一个消息发送队列节点
* 参    数:
* 参数名称              输入/输出   描述
* 函数返回:
* 说    明:
*******************************************************************************/
UINT8* DTPClientImp::AllocMsgNode(UINT32 ulMsgLen)
{
    if (ulMsgLen < sizeof(DTP_MSG_HDR_T) ||
        ulMsgLen > sizeof(DTP_MSG_T) )
    {
        return NULL;
    }

    return (UINT8*)m_pMemAlloc->MemAlloc(ulMsgLen);
}

/*******************************************************************************
* 函数名称: dtp_client_free_msg_node
* 功    能: 释放一个消息发送队列节点
* 参    数:
* 参数名称              输入/输出   描述
* 函数返回:
* 说    明:
*******************************************************************************/
VOID DTPClientImp::FreeMsgNode(VOID *pvMsg)
{
    m_pMemAlloc->MemFree(pvMsg);
}

BOOL DTPClientImp::SocketSend(SOCKET stSocket, VOID *pvMsg, UINT32 ulMsgLen, SOCKADDR_IN *pstServerAddr, BOOL bNeedLock)
{
    if (bNeedLock && !gos_mutex(m_pstClient->hMsgQueueMutex))
    {
        return FALSE;
    }

    BOOL bRet = dtp_udp_send(stSocket, pvMsg, ulMsgLen, pstServerAddr);

    if (bNeedLock)
    {
        gos_unmutex(m_pstClient->hMsgQueueMutex);
    }

    return bRet;
}

/*******************************************************************************
* 函数名称: dtp_client_send_ex
* 功    能: 客户端发送消息
* 参    数:
* 参数名称              输入/输出   描述
* 函数返回:
* 说    明:
*******************************************************************************/
BOOL DTPClientImp::SendNow(UINT16 usSenderModule, UINT16 usReceiverModule,
                          UINT16 usMsgType, UINT32 ulSeqID,
                          VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulTimeout)
{
    DTP_MSG_T       *pstMsg = NULL;
    UINT32          ulTotalMsgLen = sizeof(DTP_MSG_HDR_T)+ulMsgLen;

    pstMsg = (DTP_MSG_T*)GOS_ZALLOC(sizeof(DTP_MSG_T));
    if (!pstMsg || ulMsgLen > sizeof(pstMsg->aucMsgBody))
    {
        return FALSE;
    }

    pstMsg->stMsgHdr.ulMsgMagic = DTP_MSG_MAGIC;
    pstMsg->stMsgHdr.usMsgVer = DTP_MSG_VER;

    pstMsg->stMsgHdr.usSenderModule = usSenderModule;
    pstMsg->stMsgHdr.usReceiverModule = usReceiverModule;
    pstMsg->stMsgHdr.ulMsgLen  = ulTotalMsgLen;
    pstMsg->stMsgHdr.ulSeqID   = ulSeqID;
    pstMsg->stMsgHdr.usMsgType = usMsgType;
    pstMsg->stMsgHdr.ucMsgPri  = m_pstClient->ucMsgPri;

    pstMsg->stMsgHdr.ucCompType = DTP_COMPRESS_TYPE_NONE;
    pstMsg->stMsgHdr.ucEncryptType = DTP_ENCRYPT_TYPE_NONE;

    pstMsg->stMsgHdr.usTimeout = ulTimeout;
    if (ulMsgLen > 0)
    {
        memcpy(pstMsg->aucMsgBody, pvMsg, ulMsgLen);
    }

    pstMsg->stMsgHdr.ucSenderType = m_pstClient->ucSenderType;

    dtp_msg_hton(&pstMsg->stMsgHdr);

    SOCKADDR_IN     stServerAddr = {0};

    stServerAddr.sin_family = AF_INET;
    stServerAddr.sin_port   = HTONS(m_pstClient->usServerPort);
    memcpy(&stServerAddr.sin_addr.s_addr, m_pstClient->aucServerAddr, 4);

    BOOL bRet = SocketSend(m_pstClient->stSocket, pstMsg, ulTotalMsgLen, &stServerAddr);

    free(pstMsg);

    return bRet;
}

BOOL DTPClientImp::SendEx(UINT16 usSenderModule, UINT16 usReceiverModule,
                          UINT16 usMsgType, UINT32 ulSeqID,
                          VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulTimeout)
{
    BOOL bSendNow = TRUE;

    if (bSendNow)
    {
        return SendNow(usSenderModule, usReceiverModule, usMsgType, ulSeqID, pvMsg, ulMsgLen, ulTimeout);
    }

    DTP_CLIENT_MSG_NODE_T   stMsgNode = {0};
    GLISTNODE               *pstListNode;
    UINT32                  ulTotalMsgLen;
    DTP_MSG_HDR_T          *pstMsgHdr;
    HANDLE                  hMutex;
    HANDLE                  hQueue;

    if (!m_pstClient)
    {
        return FALSE;
    }

    if (ulMsgLen > DTP_MAX_MSG_LEN)
    {
        return FALSE;
    }

    ulTotalMsgLen = sizeof(DTP_MSG_HDR_T)+ulMsgLen;
    stMsgNode.pucMsg = AllocMsgNode(ulTotalMsgLen);
    if (!stMsgNode.pucMsg)
    {
        return FALSE;
    }

    pstMsgHdr = (DTP_MSG_HDR_T *)stMsgNode.pucMsg;

    pstMsgHdr->ulMsgMagic = DTP_MSG_MAGIC;
    pstMsgHdr->usMsgVer = DTP_MSG_VER;

    pstMsgHdr->usSenderModule = usSenderModule;
    pstMsgHdr->usReceiverModule = usReceiverModule;
    pstMsgHdr->ulMsgLen  = ulTotalMsgLen;
    pstMsgHdr->ulSeqID   = ulSeqID;
    pstMsgHdr->usMsgType = usMsgType;
    pstMsgHdr->ucMsgPri  = m_pstClient->ucMsgPri;

    pstMsgHdr->ucCompType = DTP_COMPRESS_TYPE_NONE;
    pstMsgHdr->ucEncryptType = DTP_ENCRYPT_TYPE_NONE;

    pstMsgHdr->usTimeout = ulTimeout;
    pstMsgHdr->ucSenderType = m_pstClient->ucSenderType;

    dtp_msg_hton(pstMsgHdr);

    if (ulMsgLen > 0)
    {
        if (pvMsg == NULL)
        {
            FreeMsgNode(pstMsgHdr);
            return FALSE;
        }

        memcpy(pstMsgHdr+1, pvMsg, ulMsgLen);
    }

    // 根据消息获取消息处理的类型
    hMutex = m_pstClient->hMsgQueueMutex;
    hQueue = m_pstClient->hMsgQueue;

    if (!gos_mutex(hMutex))
    {
        FreeMsgNode(pstMsgHdr);
        return FALSE;
    }

    stMsgNode.bSendFlag = FALSE;
    stMsgNode.ulSendTime = 0;
    stMsgNode.ulMsgNum = 1;
    stMsgNode.ulMsgLen = ulTotalMsgLen;

    pstListNode = gos_list_add((GLIST*)hQueue, &stMsgNode, sizeof(stMsgNode));
    gos_unmutex(hMutex);

    if (!pstListNode)
    {
        FreeMsgNode(pstMsgHdr);
        return FALSE;
    }
    else
    {
        gos_sem_v(m_pstClient->hMsgQueueSem);
    }

    return TRUE;
}

BOOL DTPClientImp::Send(UINT16 usSenderModule, UINT16 usReceiverModule, UINT16 usMsgType, UINT32 ulSeqID, VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulTimeout)
{
    return SendEx(usSenderModule, usReceiverModule, usMsgType, ulSeqID, pvMsg, ulMsgLen, ulTimeout);
}

BOOL DTPClientImp::SendReq(UINT16 usSenderModule, UINT16 usReceiverModule, UINT16 usMsgType, VOID *pvMsg, UINT32 ulMsgLen)
{
    UINT32  ulSeqID;

    ulSeqID = _GetSeqID();
    return Send(usSenderModule, usReceiverModule, usMsgType, ulSeqID, pvMsg, ulMsgLen);
}

BOOL DTPClientImp::SyncSend(UINT16 usSenderModule, UINT16 usReceiverModule, UINT16 usMsgType, VOID *pvMsg, UINT32 ulMsgLen,
                            VOID *pvRspMsg, UINT32 ulMaxRspMsgLen, UINT32 *pulRspMsgLen, UINT32 ulTimeout)
{
    DTP_SYNC_MSG_NODE_T    *pstMsgNode = NULL;
    UINT32                  i;
    UINT32                  ulCurrTime = gos_get_uptime_1ms();
    UINT32                  ulSyncMsgSn;

    if (!m_pstClient || !m_pstClient->hSyncMsgMutex ||
        ulTimeout == 0)
    {
        return FALSE;
    }

    // 将消息添加到同步消息队列
    if (!gos_mutex(m_pstClient->hSyncMsgMutex))
    {
        return FALSE;
    }

    for (i=0; i<ARRAY_SIZE(m_pstClient->astSyncMsg); i++)
    {
        if (m_pstClient->astSyncMsg[i].bUsed)
        {
            if (m_pstClient->astSyncMsg[i].ulDeadTime <= ulCurrTime )
            {
                m_pstClient->astSyncMsg[i].pstRspMsg->ulMsgLen = 0;
                gos_sem_v(m_pstClient->astSyncMsg[i].hSem);
                m_pstClient->astSyncMsg[i].bUsed = FALSE;
                pstMsgNode = &m_pstClient->astSyncMsg[i];
                break;
            }
        }
        else
        {
            pstMsgNode = &m_pstClient->astSyncMsg[i];
            break;
        }
    }

    if (!pstMsgNode)
    {
        *pulRspMsgLen = 0;
        gos_unmutex(m_pstClient->hSyncMsgMutex);
        return FALSE;
    }

    if (pstMsgNode->hSem == INVALID_SEM)
    {
        pstMsgNode->hSem = gos_sem_init();
    }

    if (!pstMsgNode->hSem)
    {
        *pulRspMsgLen = 0;
        gos_unmutex(m_pstClient->hSyncMsgMutex);
        return FALSE;
    }

    ulSyncMsgSn = _GetSeqID();

    pstMsgNode->bUsed = TRUE;
    pstMsgNode->ulDeadTime = ulCurrTime + ulTimeout*1000;
    pstMsgNode->ulMsgLen   = ulMsgLen;
    pstMsgNode->usMsgType  = usMsgType;
    pstMsgNode->ulMsgSn    = ulSyncMsgSn;

    pstMsgNode->pstRspMsg = (DTP_MSG_HDR_T *)pvRspMsg;
    pstMsgNode->ulMaxRspMsgLen = ulMaxRspMsgLen;
    pstMsgNode->pulRspMsgLen = pulRspMsgLen;

    if (!Send(usSenderModule, usReceiverModule, usMsgType, ulSyncMsgSn, pvMsg, ulMsgLen, ulTimeout))
    {
        *pulRspMsgLen = 0;
        pstMsgNode->bUsed = FALSE;

        return FALSE;
    }

    gos_unmutex(m_pstClient->hSyncMsgMutex);

    // 等待应答消息
    if (!gos_sem_p(pstMsgNode->hSem, ulTimeout*1000))
    {
        gos_mutex(m_pstClient->hSyncMsgMutex);
        pstMsgNode->bUsed = FALSE;

        *pulRspMsgLen = 0;
        gos_unmutex(m_pstClient->hSyncMsgMutex);

        return FALSE;
    }

    return TRUE;
}

GOS_THREAD_RET DTPUDPClientSendThreadEntry(VOID *pvData)
{
    UINT32                  ulMsgLen;
    DTP_CLIENT_MSG_NODE_T   *pstMsgNode = NULL;
    DTPClientImp            *pClient = (DTPClientImp*)pvData;
    DTP_CLIENT_INFO_T       *pstClient = (DTP_CLIENT_INFO_T*)pClient->GetHandle();
    HANDLE                  hMutex;
    GLIST                   *hQueue;
    SOCKADDR_IN             stServerAddr = {0};

    stServerAddr.sin_family = AF_INET;
    stServerAddr.sin_port   = HTONS(pstClient->usServerPort);
    memcpy(&stServerAddr.sin_addr.s_addr, pstClient->aucServerAddr, 4);

    gos_init_socket();

    pstClient->bSendThreadRunning = TRUE;

    while(pstClient->enRunStatus != THREAD_STATE_WORK)
    {
        if (pstClient->enRunStatus == THREAD_STATE_STOPPING)
        {
            goto end;
        }

        gos_sleep_1ms(100);
    }

    while(pstClient->enRunStatus != THREAD_STATE_STOPPING &&
          pstClient->enRunStatus != THREAD_STATE_STOP )
    {
        if (pstClient->enClientStatus != DTP_CLIENT_LINKSETUP ||
            pstClient->stSocket == INVALID_SOCKET )
        {
            gos_sem_p(pstClient->hMsgQueueSem, 100);
            continue;
        }

        pstMsgNode = NULL;
        hMutex = pstClient->hMsgQueueMutex;
        hQueue = (GLIST*)pstClient->hMsgQueue;
        gos_mutex(hMutex);

        while(!gos_list_is_empty(hQueue))
        {
            pstMsgNode = (DTP_CLIENT_MSG_NODE_T*)gos_list_get_head(hQueue);
            if (pstMsgNode)
            {
                break;
            }
            else
            {
                gos_list_remove(hQueue);
            }
        }

        if (NULL == pstMsgNode)
        {
            gos_unmutex(hMutex);
            gos_sem_p(pstClient->hMsgQueueSem, 1);

            continue;
        }

     /*   pstMsg = (DTP_MSG_T *)pstMsgNode->pucMsg;
        usMsgType = NTOHS(pstMsg->stMsgHdr.usMsgType);     */
        ulMsgLen = pstMsgNode->ulMsgLen;

        if (pClient->SocketSend(pstClient->stSocket, pstMsgNode->pucMsg, ulMsgLen, &stServerAddr, FALSE))
        {
            pstClient->ulSocketErrNum = 0;

            pClient->FreeMsgNode(pstMsgNode->pucMsg);
            gos_list_remove(hQueue);

            gos_unmutex(hMutex);

            gos_mutex(pstClient->hLinkStatMutex);
            pstClient->ulLastSendTime = gos_get_uptime();
            gos_unmutex(pstClient->hLinkStatMutex);
        }
        else
        {
            /* 发送失败也删除消息节点 */
            pClient->FreeMsgNode(pstMsgNode->pucMsg);
            gos_list_remove(hQueue);

            gos_unmutex(hMutex);

            pstClient->ulSocketErrNum ++;
            if (pstClient->ulSocketErrNum < pstClient->ulMaxSocketErrNum)
            {
                DTPLog(LOG_WARN, "client send msg failed");
            }
            else
            {
                DTPLog(LOG_WARN, "too many send failure, socket will be closed");

                CLOSE_SOCKET(pstClient->stSocket);
                pstClient->ulSocketErrNum = 0;
            }
        }
    }

end:
    pstClient->bSendThreadRunning = FALSE;

    return 0;
}

#undef DTPClientImp

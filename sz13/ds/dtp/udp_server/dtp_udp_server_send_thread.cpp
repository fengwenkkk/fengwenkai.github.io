#include "g_include.h"

#include "dtp_public.h"
#include "dtp_common.h"
#include "dtp_func.h"
#include "dtp_udp_server.h"

#define DTPServerImp    DTPUDPServer

/*******************************************************************************
* ��������: _GetSeqID
* ��    ��: ����һ����Ϣ��ˮ��
* ��    ��:
* ��������              ����/���   ����
* ��������:
* ˵    ��:
*******************************************************************************/
UINT32 DTPServerImp::_GetSeqID()
{
    UINT32  ulSeqID = 0;

    gos_mutex(m_pstServer->hSeqIDMutex);
    m_pstServer->ulSeqID += 2;
    if (m_pstServer->ulSeqID < 2)
    {
        m_pstServer->ulSeqID = 2;
    }

    ulSeqID = m_pstServer->ulSeqID;

    gos_unmutex(m_pstServer->hSeqIDMutex);

    return ulSeqID;
}
/*******************************************************************************
* ��������: AllocMsgNode
* ��    ��: ����һ����Ϣ���Ͷ��нڵ�
* ��    ��:
* ��������              ����/���   ����
* ��������:
* ˵    ��:
*******************************************************************************/
DTP_MSG_T* DTPServerImp::AllocMsgNode(UINT32 ulMsgLen)
{
    DTP_MSG_T  *pstMsg;

    if (ulMsgLen < sizeof(DTP_MSG_HDR_T) ||
        ulMsgLen > sizeof(DTP_MSG_T) )
    {
        return NULL;
    }

    pstMsg = (DTP_MSG_T*)m_pMemAlloc->MemAlloc(ulMsgLen);
    return pstMsg;
}

/*******************************************************************************
* ��������: FreeMsgNode
* ��    ��: �ͷ�һ����Ϣ���Ͷ��нڵ�
* ��    ��:
* ��������              ����/���   ����
* ��������:
* ˵    ��:
*******************************************************************************/
VOID DTPServerImp::FreeMsgNode(VOID *pvMsg)
{
    m_pMemAlloc->MemFree(pvMsg);
}

BOOL DTPServerImp::GetMsgStat(std::vector<DTP_MSG_STAT_T> &vStat)
{
    if (!gos_mutex(m_pstServer->hMsgQueueMutex))
    {
        return FALSE;
    }

    GHASHNODE   *pNode = gos_hashlist_get_first(m_hMsgStatHash);
    DTP_MSG_STAT_T *pstStat;

    vStat.clear();
    while(pNode)
    {
        pstStat = (DTP_MSG_STAT_T *)gos_hashlist_get_node_data(pNode);
        pNode = gos_hashlist_get_next(m_hMsgStatHash, pNode);

        vStat.push_back(*pstStat);
    }

    gos_unmutex(m_pstServer->hMsgQueueMutex);

    return TRUE;
}

BOOL DTPServerImp::SocketSend(DTP_MSG_HDR_T *pstMsg, SOCKADDR_IN *pstServerAddr, BOOL bNeedLock)
{
    if (bNeedLock && !gos_mutex(m_pstServer->hMsgQueueMutex))
    {
        return FALSE;
    }

    UINT32  ulMsgLen = NTOHL(pstMsg->ulMsgLen);

    BOOL    bRet = dtp_udp_send(m_pstServer->stSocket, pstMsg, ulMsgLen, pstServerAddr);

    if (bRet)
    {
        UINT32          ulMsgID = NTOHS(pstMsg->usMsgType);
        DTP_MSG_STAT_T  stStat = {0};
        DTP_MSG_STAT_T  *pstStat = (DTP_MSG_STAT_T*)gos_hashlist_get_int(m_hMsgStatHash, ulMsgID);

        if (!pstStat)
        {
            stStat.ulMsgID = ulMsgID;
            stStat.u64MsgCount = 1;
            stStat.u64MsgSize = ulMsgLen;
            gos_hashlist_add_int(m_hMsgStatHash, &stStat, sizeof(stStat));
        }
        else
        {
            pstStat->u64MsgCount ++;
            pstStat->u64MsgSize += ulMsgLen;
        }
    }

    if (bNeedLock)
    {
        gos_unmutex(m_pstServer->hMsgQueueMutex);
    }

    return bRet;
}

BOOL DTPServerImp::SendNow(UINT16 usServerModuleID, UINT16 usClientID, UINT16 usClientModuleID,
                          UINT16 usMsgType, UINT32 ulSeqID, VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulTimeout)
{
    UINT32          ulTotalMsgLen = sizeof(DTP_MSG_HDR_T)+ulMsgLen;
    BOOL            bRet = FALSE;
    DTP_MSG_T       *pstMsg = NULL;

    if (!gos_mutex(m_pstServer->hConnInfo))
    {
        return FALSE;
    }

    SOCKADDR_IN     stClientRecvAddr = {0};
    BOOL            bExist = FALSE;

    for (UINT32 i=0; i<ARRAY_SIZE(m_pstServer->astConnInfo); i++)
    {
        if (m_pstServer->astConnInfo[i].usClientRecvPort == 0)
        {
            continue;
        }

        if (m_pstServer->astConnInfo[i].usClientID == usClientID)
        {
            memcpy(&stClientRecvAddr, &m_pstServer->astConnInfo[i].stClientSendAddr, sizeof(stClientRecvAddr));
            stClientRecvAddr.sin_port = HTONS(m_pstServer->astConnInfo[i].usClientRecvPort);
            bExist = TRUE;

            break;
        }
    }

    gos_unmutex(m_pstServer->hConnInfo);

    if (!bExist)
    {
        return FALSE;
    }

    pstMsg = (DTP_MSG_T*)GOS_ZALLOC(sizeof(DTP_MSG_T));
    if (!pstMsg)
    {
        return FALSE;
    }

    pstMsg->stMsgHdr.ulMsgMagic = DTP_MSG_MAGIC;
    pstMsg->stMsgHdr.usMsgVer = DTP_MSG_VER;

    pstMsg->stMsgHdr.usSenderModule = usServerModuleID;
    pstMsg->stMsgHdr.usReceiverModule = usClientModuleID;
    pstMsg->stMsgHdr.ulMsgLen  = ulTotalMsgLen;
    pstMsg->stMsgHdr.ulSeqID   = ulSeqID;
    pstMsg->stMsgHdr.usMsgType = usMsgType;
    pstMsg->stMsgHdr.ucMsgPri  = DTP_MSG_PRI_HIGH;

    pstMsg->stMsgHdr.ucCompType = DTP_COMPRESS_TYPE_NONE;
    pstMsg->stMsgHdr.ucEncryptType = DTP_ENCRYPT_TYPE_NONE;

    pstMsg->stMsgHdr.usTimeout = ulTimeout;

    if (ulMsgLen > 0)
    {
        memcpy(&pstMsg->aucMsgBody, pvMsg, ulMsgLen);
    }

    dtp_msg_hton(&pstMsg->stMsgHdr);

    bRet = SocketSend(&pstMsg->stMsgHdr, &stClientRecvAddr);

    GOS_FREE(pstMsg);

    return bRet;
}

/*******************************************************************************
* ��������: _Send
* ��    ��: ����˷�����Ϣ���ͻ���
* ��    ��:
* ��������              ����/���   ����
* ��������:
* ˵    ��:
*******************************************************************************/
BOOL DTPServerImp::SendEx(UINT16 usServerModuleID, UINT16 usClientID, UINT16 usClientModuleID,
                          UINT16 usMsgType, UINT32 ulSeqID, VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulTimeout)
{
    BOOL bSendNow = TRUE;

    if (bSendNow)
    {
        return SendNow(usServerModuleID, usClientID, usClientModuleID, usMsgType, ulSeqID, pvMsg, ulMsgLen, ulTimeout);
    }

    return FALSE;
}

/*******************************************************************************
* ��������: SyncSend
* ��    ��: ����˷���ͬ����Ϣ
* ��    ��:
* ��������              ����/���   ����
* ��������:
* ˵    ��:
*******************************************************************************/
BOOL DTPServerImp::SyncSend(UINT16 usSenderModule, UINT16 usClientID, UINT16 usClientModuleID, UINT16 usMsgType, VOID *pvMsg, UINT32 ulMsgLen,
                            VOID *pvRspMsg, UINT32 ulMaxRspMsgLen, UINT32 *pulRspMsgLen, UINT32 ulTimeout)
{
    DTP_SYNC_MSG_NODE_T    *pstMsgNode = NULL;
    UINT32                  i;
    UINT32                  ulCurrTime = gos_get_uptime_1ms();
    UINT32                  ulSyncMsgSn;

    if (!m_pstServer || !m_pstServer->hSyncMsgMutex ||
        ulTimeout == 0)
    {
        return FALSE;
    }

    // ����Ϣ��ӵ�ͬ����Ϣ����
    if (!gos_mutex(m_pstServer->hSyncMsgMutex))
    {
        return FALSE;
    }

    for (i=0; i<ARRAY_SIZE(m_pstServer->astSyncMsg); i++)
    {
        if (m_pstServer->astSyncMsg[i].bUsed)
        {
            if (m_pstServer->astSyncMsg[i].ulDeadTime <= ulCurrTime )
            {
                m_pstServer->astSyncMsg[i].pstRspMsg->ulMsgLen = 0;
                gos_sem_v(m_pstServer->astSyncMsg[i].hSem);
                m_pstServer->astSyncMsg[i].bUsed = FALSE;
                pstMsgNode = &m_pstServer->astSyncMsg[i];
                break;
            }
        }
        else
        {
            pstMsgNode = &m_pstServer->astSyncMsg[i];
            break;
        }
    }

    if (!pstMsgNode)
    {
        *pulRspMsgLen = 0;
        gos_unmutex(m_pstServer->hSyncMsgMutex);
        return FALSE;
    }

    if (pstMsgNode->hSem == INVALID_SEM)
    {
        pstMsgNode->hSem = gos_sem_init();
    }

    if (!pstMsgNode->hSem)
    {
        *pulRspMsgLen = 0;
        gos_unmutex(m_pstServer->hSyncMsgMutex);
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

    if (!Send(usSenderModule, usClientID, usClientModuleID, usMsgType, ulSyncMsgSn, pvMsg, ulMsgLen, ulTimeout))
    {
        *pulRspMsgLen = 0;
        pstMsgNode->bUsed = FALSE;

        return FALSE;
    }

    gos_unmutex(m_pstServer->hSyncMsgMutex);

    // �ȴ�Ӧ����Ϣ
    if (!gos_sem_p(pstMsgNode->hSem, ulTimeout*1000))
    {
        gos_mutex(m_pstServer->hSyncMsgMutex);
        pstMsgNode->bUsed = FALSE;

        *pulRspMsgLen = 0;
        gos_unmutex(m_pstServer->hSyncMsgMutex);

        return FALSE;
    }

    return TRUE;
}

/*******************************************************************************
* ��������: HasClient
* ��    ��: �ͻ����Ƿ����
* ��    ��:
* ��������          ����/���       ����
* usClientID        IN              �ͻ��˱�ʶ
* ��������:
* ˵    ��:
*******************************************************************************/
BOOL DTPServerImp::HasClient(UINT16 usClientID)
{
    UINT32              i;

    if (!m_pstServer)
    {
        return FALSE;
    }

    if (!gos_mutex(m_pstServer->hConnInfo))
    {
        return FALSE;
    }

    for (i=0; i<ARRAY_SIZE(m_pstServer->astConnInfo); i++)
    {
        if (m_pstServer->astConnInfo[i].usClientRecvPort != 0 &&
            m_pstServer->astConnInfo[i].usClientID == usClientID)
        {
            gos_unmutex(m_pstServer->hConnInfo);
            return TRUE;
        }
    }

    gos_unmutex(m_pstServer->hConnInfo);
    return FALSE;
}

/*******************************************************************************
* ��������: GetClientAddr
* ��    ��: ��ȡ�ͻ��˵�ַ
* ��    ��:
* ��������          ����/���       ����
* usClientID        IN              �ͻ��˱�ʶ
* ��������:
* ˵    ��:
*******************************************************************************/
BOOL DTPServerImp::GetClientAddr(UINT16 usClientID, UINT8 *pucClientAddr, UINT16 *pusClientPort)
{
    UINT32              i;

    if (!m_pstServer)
    {
        return FALSE;
    }

    if (!gos_mutex(m_pstServer->hConnInfo))
    {
        return FALSE;
    }

    for (i=0; i<ARRAY_SIZE(m_pstServer->astConnInfo); i++)
    {
        if (m_pstServer->astConnInfo[i].usClientRecvPort != 0 &&
            m_pstServer->astConnInfo[i].usClientID == usClientID)
        {
            memcpy(pucClientAddr, &m_pstServer->astConnInfo[i].stClientSendAddr.sin_addr, 4);
            *pusClientPort = ntohs(m_pstServer->astConnInfo[i].stClientSendAddr.sin_port);

            gos_unmutex(m_pstServer->hConnInfo);
            return TRUE;
        }
    }

    gos_unmutex(m_pstServer->hConnInfo);
    return FALSE;
}

BOOL DTPServerImp::Send(UINT16 usServerModuleID, UINT16 usClientID, UINT16 usClientModuleID, UINT16 usMsgType, UINT32 ulSeqID,
                        VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulTimeout)
{
    return SendEx(usServerModuleID, usClientID, usClientModuleID, usMsgType, ulSeqID, pvMsg, ulMsgLen, ulTimeout);
}

BOOL DTPServerImp::SendBySocket(DTP_MSG_HDR_T *pstMsgHdr, SOCKADDR_IN *pstServerAddr)
{
    dtp_msg_hton(pstMsgHdr);

    BOOL bRet = SocketSend(pstMsgHdr, pstServerAddr);

    return bRet;
}

/*******************************************************************************
* ��������: SendReq
* ��    ��: ������Ϣ
* ��    ��:
* ��������          ����/���       ����
* ��������:
* ˵    ��:
*******************************************************************************/
BOOL DTPServerImp::SendReq(UINT16 usServerModuleID, UINT16 usClientID, UINT16 usClientModuleID,
                           UINT16 usMsgType, VOID *pvMsg, UINT32 ulMsgLen)
{
    UINT32 ulSeqID = 0;

    if (!m_pstServer)
    {
        return FALSE;
    }

    ulSeqID = _GetSeqID();

    return Send(usServerModuleID, usClientID, usClientModuleID, usMsgType, ulSeqID, pvMsg, ulMsgLen);
}

/*******************************************************************************
* ��������: SendBroadcastMsg
* ��    ��: �����пͻ��˷��͹㲥��Ϣ
* ��    ��:
* ��������          ����/���       ����
* pvMsg             IN              ��Ϣ��DTP_MSG_T�ṹ��
* ��������:
* ˵    ��:
*******************************************************************************/
VOID DTPServerImp::SendBroadcastMsg(UINT16 usServerModuleID, UINT16 usClientModuleID, UINT16 usMsgType, VOID *pvMsg, UINT32 ulMsgLen)
{
    UINT8 ucClientType = 0;

    return SendGroupMsg(usServerModuleID, usClientModuleID, ucClientType, usMsgType, pvMsg, ulMsgLen);
}

VOID DTPServerImp::SendGroupMsg(UINT16 usServerModuleID, UINT16 usClientModuleID, UINT8 ucClientType, UINT16 usMsgType, VOID *pvMsg, UINT32 ulMsgLen)
{
    UINT32              i;

    if (!m_pstServer)
    {
        return;
    }

    if (!gos_mutex(m_pstServer->hConnInfo))
    {
        return;
    }

    VECTOR<UINT16>      vClientID;

    for (i=0; i<ARRAY_SIZE(m_pstServer->astConnInfo); i++)
    {
        if (m_pstServer->astConnInfo[i].usClientRecvPort != 0 &&
            (ucClientType == 0 || m_pstServer->astConnInfo[i].ucClientType == ucClientType))
        {
            vClientID.push_back(m_pstServer->astConnInfo[i].usClientID);
        }
    }

    gos_unmutex(m_pstServer->hConnInfo);

    for (i=0; i<vClientID.size(); i++)
    {
       SendReq(usServerModuleID, vClientID[i], usClientModuleID, usMsgType, pvMsg, ulMsgLen);
    }
}

VOID DTPServerImp::GetClientList(UINT16 *pusClientID, UINT32 *pulClientNum)
{
    UINT32              i;

    *pulClientNum = 0;
    if (!m_pstServer)
    {
        return;
    }

    if (!gos_mutex(m_pstServer->hConnInfo))
    {
        return;
    }

    for (i=0; i<ARRAY_SIZE(m_pstServer->astConnInfo); i++)
    {
        if (m_pstServer->astConnInfo[i].usClientRecvPort != 0)
        {
            *pusClientID++ = m_pstServer->astConnInfo[i].usClientID;
            pulClientNum[0]++;
        }
    }

    gos_unmutex(m_pstServer->hConnInfo);
}

/*******************************************************************************
* ��������: DTPUDPServerSendThreadEntry
* ��    ��: ������Ϣ�߳����
* ��    ��:
* ��������          ����/���       ����
* pvData            IN              DTP_SERVER_INFO_T�ṹָ��
* ��������:
* ˵    ��:
*******************************************************************************/
GOS_THREAD_RET DTPUDPServerSendThreadEntry(VOID *pvData)
{
    UINT32                  i;
    DTP_SERVER_MSG_NODE_T   *pstMsgNode = NULL;
    DTP_CONN_INFO_T         stConn = {0};
    BOOL                    bExist;
    DTPServerImp            *pServer = (DTPServerImp*)pvData;
    DTP_SERVER_INFO_T       *pstServer = (DTP_SERVER_INFO_T*)pServer->GetHandle();
    GLIST                   *hQueue = (GLIST*)pstServer->hMsgQueue;
    SOCKADDR_IN             stClientRecvAddr;

    gos_init_socket();

    while(1)
    {
        while(1)
        {
            if (pServer->IsRunning())
            {
                break;
            }

            gos_sleep_1ms(100);
        }

        pstMsgNode = NULL;
        gos_mutex(pstServer->hMsgQueueMutex);

        while(!gos_list_is_empty(hQueue))
        {
            pstMsgNode = (DTP_SERVER_MSG_NODE_T*)gos_list_get_head(hQueue);
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
            gos_unmutex(pstServer->hMsgQueueMutex);
            gos_sem_p(pstServer->hMsgQueueSem, 100);

            continue;
        }

        bExist = FALSE;
        if (!gos_mutex(pstServer->hConnInfo))
        {
            continue;
        }

        for (i=0; i<ARRAY_SIZE(pstServer->astConnInfo); i++)
        {
            if (pstServer->astConnInfo[i].usClientRecvPort == 0)
            {
                continue;
            }

            if ((pstMsgNode->stSocket == INVALID_SOCKET && /* ����cleintID��ConnType���� */
                 pstServer->astConnInfo[i].usClientID == pstMsgNode->usClientID))
            {
                bExist = TRUE;
                memcpy(&stConn, &pstServer->astConnInfo[i], sizeof(stConn));

                break;
            }
        }

        gos_unmutex(pstServer->hConnInfo);

        if (bExist)
        {
            memcpy(&stClientRecvAddr, &stConn.stClientSendAddr, sizeof(stClientRecvAddr));
            stClientRecvAddr.sin_port = HTONS(stConn.usClientRecvPort);
        }

        if (!bExist)
        {
            DTPLog(LOG_DETAIL, "server send msg failed, client %d has not logined", pstMsgNode->usClientID);

            pServer->FreeMsgNode(pstMsgNode->pstMsg);
            gos_list_remove(hQueue);
        }
        else if (pServer->SocketSend(&pstMsgNode->pstMsg->stMsgHdr, &stClientRecvAddr, FALSE))
        {
            DTPLog(LOG_DETAIL, "server send seq = %u, time = %u",
                    0xffffff & NTOHL(pstMsgNode->pstMsg->stMsgHdr.ulSeqID), gos_get_uptime_1ms());

            pServer->FreeMsgNode(pstMsgNode->pstMsg);
            gos_list_remove(hQueue);

            pstServer->ulSendErrNum = 0;
        }
        else
        {
            DTPLog(LOG_WARN, "send fail, connection with " IP_FMT "(%d) will be closed",
                         IP_ARG(&stConn.stClientSendAddr.sin_addr.s_addr), NTOHS(stConn.stClientSendAddr.sin_port));

            /* ����ʧ��Ҳɾ���ڵ� */
            pServer->FreeMsgNode(pstMsgNode->pstMsg);
            gos_list_remove(hQueue);
        }

        gos_unmutex(pstServer->hMsgQueueMutex);
    }

//  return 0;
}

#undef DTPServerImp

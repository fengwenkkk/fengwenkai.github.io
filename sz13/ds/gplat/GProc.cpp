#include "GPublic.h"
#include "GBaseTask.h"
#include "GMsgQueue.h"
#include "GMsgDispatch.h"
#include "GProc.h"

extern GBaseTask    *GetTask(UINT16 usTaskID, UINT16 usMsgID);

static GMutex           m_ProcMutex;
static IntMap<GProc>    m_mProc(16);    /* ProcID->Proc */

GProc* GProc::GetProc(UINT16 usProcID)
{
    if (!m_ProcMutex.Mutex())
    {
        return NULL;
    }

    GProc   *pProc = m_mProc.GetValue(usProcID);
    UINT32  ulMaxMsgNodeNum = GosGetMaxMsgNodeNum();

    if (pProc)
    {
        goto end;
    }

    pProc = new GProc(usProcID, ulMaxMsgNodeNum, NULL);
    if (!pProc)
    {
        goto end;
    }

    m_mProc.Add(usProcID, pProc);

    GosLog(LOG_FATAL, "start proc %u", usProcID);

    pProc->Start();

end:
    m_ProcMutex.Unmutex();

    return pProc;
}

GProc::GProc(UINT16 usProcID, UINT32 ulMaxMsgNodeNum, VOID *pPara, UINT32 ulStackSize):GThread(pPara, ulStackSize)
{
    m_hSem = gos_sem_init();
    m_usProcID = usProcID;
    m_ulStatus = PROC_STATUS_RUNNING;
    m_usTaskID = INVALID_TASK_ID;

    m_MsgQueue = new GMsgQueue(usProcID, ulMaxMsgNodeNum);
}

BOOL GProc::InitUniqueMsg(UINT16 usMsgID, GOS_UNIQUE_MSG_CMP_FUNC *pfUniqueMsgCmp)
{
    return m_MsgQueue->InitUniqueMsg(usMsgID, pfUniqueMsgCmp);
}

VOID GProc::Resume()
{
    m_ulStatus = PROC_STATUS_RUNNING;
    gos_sem_v(m_hSem);
}

VOID GProc::Suspend()
{
    m_ulStatus = PROC_STATUS_SUSPEND;
}

BOOL GProc::AddMsg(UINT16 usTaskID, MSG_T *pstMsgHdr, VOID *pvMsgBody, UINT32 ulMsgBodyLen)
{
    return m_MsgQueue->AddMsg(usTaskID, pstMsgHdr, pvMsgBody, ulMsgBodyLen);
}

BOOL GProc::AddMsg(UINT16 usTaskID, MSG_T *pstMsg)
{
    return m_MsgQueue->AddMsg(usTaskID, pstMsg);
}

BOOL GProc::Schedule()
{
    MSG_T           *pstMsg = NULL;
    GBaseTask       *pTask;

    m_usTaskID = INVALID_TASK_ID;

    pstMsg = m_MsgQueue->GetMsg();
    if (!pstMsg)
    {
        return FALSE;
    }

    pTask = GetTask(pstMsg->stReceiver.usTaskID, pstMsg->usMsgID);
    if (!pTask)
    {
        GosLog(MODULE_PLAT, LOG_ERROR, "unknown task msg %d", pstMsg->usMsgID);
        goto end;
    }

    m_usTaskID = pTask->GetTaskID();
    pTask->SetMsgSender(&pstMsg->stSender);
    pTask->SetSenderType(pstMsg->ucSenderType);
    pTask->SetMsgSeqID(pstMsg->ulSeqID);
    pTask->SetMsgID(pstMsg->usMsgID);
    pTask->SetMsgData(pstMsg->aucMsgData);
    pTask->SetMsgLen(pstMsg->ulMsgLen);
    pTask->SetMsgStartTime(gos_get_uptime());
    pTask->SetMsgEndTime(0);

    if (pstMsg->usMsgID == EV_CLIENT_CONNECT_SERVER_IND)
    {
        if (pstMsg->ulMsgLen == (sizeof(CLIENT_CONNECT_SERVER_T) - STRUCT_OFFSET(CLIENT_CONNECT_SERVER_T, usServerID)))
        {
            pTask->OnClientConnectServer((UINT16)GET_SHORT(pstMsg->aucMsgData));
        }
    }
    else if (pstMsg->usMsgID == EV_CLIENT_DISCONNECT_SERVER_IND)
    {
        if (pstMsg->ulMsgLen == (sizeof(CLIENT_DISCONNECT_SERVER_T) - STRUCT_OFFSET(CLIENT_DISCONNECT_SERVER_T, usServerID)))
        {
            pTask->OnClientDisconnectServer((UINT16)GET_SHORT(pstMsg->aucMsgData));
        }
    }
    else if (pstMsg->usMsgID == EV_SERVER_ACCEPT_CLIENT_IND)
    {
        if (pstMsg->ulMsgLen == (sizeof(SERVER_ACCEPT_CLIENT_T) - STRUCT_OFFSET(SERVER_ACCEPT_CLIENT_T, usClientID)))
        {
            pTask->OnServerAcceptClient((UINT16)GET_SHORT(pstMsg->aucMsgData));
        }
    }
    else if (pstMsg->usMsgID == EV_SERVER_DISCONNECT_CLIENT_IND)
    {
        if (pstMsg->ulMsgLen == (sizeof(SERVER_DISCONNECT_CLIENT_T) - STRUCT_OFFSET(SERVER_DISCONNECT_CLIENT_T, usClientID)))
        {
            pTask->OnServerDisconnectClient((UINT16)GET_SHORT(pstMsg->aucMsgData));
        }
    }
    else if (pstMsg->usMsgID == EV_TASK_INIT_REQ && pTask->GetTaskStatus() != TASK_STATUS_IDLE)
    {
        pTask->SendRsp();
    }
    else if (pstMsg->usMsgID == EV_TASK_CHECK_REQ)
    {
        pTask->SendRsp();
    }
    else
    {
        UINT32  ulTime = gos_get_uptime_1ms();

        pTask->TaskEntry(pstMsg->usMsgID, pstMsg->aucMsgData, pstMsg->ulMsgLen);

        ulTime = gos_get_uptime_1ms() - ulTime;

        if (ulTime > 60*1000)
        {
            GosLog(pTask->GetTaskID(), LOG_DETAIL, "consume msg 0x%X in %u ms", pstMsg->usMsgID, ulTime);
        }
    }

    pTask->SetMsgEndTime(gos_get_uptime());

end:
    GosFreeMsg(pstMsg);

    return TRUE;
}

GOS_THREAD_RET GProc::ThreadEntry(void* pPara)
{
    while(1)
    {
        if (m_ulStatus == PROC_STATUS_SUSPEND)
        {
            gos_sem_p(m_hSem, WAIT_FOREVER);
        }

        if (Schedule())
        {
            m_ulStatus = PROC_STATUS_RUNNING;
        }
        else
        {
            m_ulStatus = PROC_STATUS_SUSPEND;
        }
    }
}

UINT16 GProc::GetTaskID()
{
    return m_usTaskID;
}


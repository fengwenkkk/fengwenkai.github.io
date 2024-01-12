#include "UniqueMsgCheck.h"

UniqueMsgCheck* UniqueMsgCheck::GetInstance(UINT32 ulMsgListMaxNum)
{
    static UniqueMsgCheck *pInstance = NULL;

    if (!pInstance)
    {
        pInstance = new UniqueMsgCheck();
        if (!pInstance)
        {
            return NULL;
        }

        if (!pInstance->InitMsgList(ulMsgListMaxNum))
        {
            delete pInstance;

            pInstance = NULL;

            return NULL;
        }
    }

    return pInstance;
}

UniqueMsgCheck::UniqueMsgCheck()
{
    m_vHaskList.clear();
}

UniqueMsgCheck::~UniqueMsgCheck()
{
    UINT32 i = 0;

    for (i=0; i<m_vHaskList.size(); i++)
    {
        if (m_vHaskList[i].pIntHashList)
        {
            delete m_vHaskList[i].pIntHashList;
        }
    }
}

BOOL UniqueMsgCheck::InitMsgList(UINT32 ulListMaxNum)
{
    if (ulListMaxNum == 0)
    {
        ulListMaxNum = UNIQUE_MSG_ID_LIST_MAX_NUM;
    }

    m_hMutex = gos_mutex_init();
    if (m_hMutex == NULL)
    {
        return FALSE;
    }

    m_ulListMaxNum = ulListMaxNum;

    return TRUE;
}

BOOL UniqueMsgCheck::TaskRegister(UINT32 ulTaskID)
{
    UNIQUE_MSG_HASH_LIST_T stHashList;

    Lock();

    stHashList.ulTaskID = ulTaskID;

    stHashList.pIntHashList  = new GIntHashList(m_ulListMaxNum, sizeof(UNIQUE_MSG_SEQ_ID_T));
    if (stHashList.pIntHashList == NULL)
    {
        Unlock();

        return FALSE;
    }

    m_vHaskList.push_back(stHashList);

    Unlock();

    return TRUE;
}

BOOL UniqueMsgCheck::UniqueCheck(UINT32 ulTaskID, UINT32 ulMsgID, UINT32 ulPID)
{
    UNIQUE_MSG_SEQ_ID_T  stMsgID;
    UNIQUE_MSG_SEQ_ID_T *pstMsgID = NULL;
    UINT32               i        = 0;
    UINT32               ulKey    = GET_UNIQUE_MSG_ID(ulPID, ulMsgID);

    stMsgID.ulMsgSeqID = ulKey;
    stMsgID.ulPID      = ulPID;

    return TRUE;

    Lock();

    if (ulMsgID == 0)
    {
        return TRUE;
    }

    for (i=0; i<m_vHaskList.size(); i++)
    {
        if (m_vHaskList[i].ulTaskID == ulTaskID)
        {
            pstMsgID = (UNIQUE_MSG_SEQ_ID_T *)m_vHaskList[i].pIntHashList->Get(ulKey);
            if (pstMsgID != NULL && pstMsgID->ulPID == ulPID)
            {
                Unlock();

                return FALSE;
            }

            if (m_vHaskList[i].pIntHashList->GetUsedNum() >= m_ulListMaxNum)
            {
                m_vHaskList[i].pIntHashList->RemoveNode(m_vHaskList[i].pIntHashList->GetFirstNode());
            }

            m_vHaskList[i].pIntHashList->Add(&stMsgID, sizeof(stMsgID));

            Unlock();

            return TRUE;
        }
    }

    Unlock();

    return TRUE;
}

BOOL UniqueMsgCheck::ReSetList()
{
    UINT32 i = 0;

    Lock();

    for (i=0; i<m_vHaskList.size(); i++)
    {
        if (m_vHaskList[i].pIntHashList)
        {
            m_vHaskList[i].pIntHashList->Clear();
        }
    }

    Unlock();

    return TRUE;
}

BOOL UniqueMsgCheck::Lock()
{
    return gos_mutex(m_hMutex);
}

VOID UniqueMsgCheck::Unlock()
{
    gos_unmutex(m_hMutex);
}

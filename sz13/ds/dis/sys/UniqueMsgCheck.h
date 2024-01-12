#ifndef UNIQUE_MSG_CHECK_H
#define UNIQUE_MSG_CHECK_H

#include "NMSPublic.h"
#include "g_hashlist.h"

#pragma pack(push, 4)

#define UNIQUE_MSG_ID_LIST_MAX_NUM 100

typedef struct
{
    UINT32                      ulMsgSeqID;
    UINT32                      ulPID;
}UNIQUE_MSG_SEQ_ID_T;

typedef struct
{
    UINT32        ulTaskID;
    GIntHashList *pIntHashList;
}UNIQUE_MSG_HASH_LIST_T;

#pragma pack(pop)

class UniqueMsgCheck
{
public:
    static UniqueMsgCheck* GetInstance(UINT32 ulListMaxNum=UNIQUE_MSG_ID_LIST_MAX_NUM);

    BOOL ReSetList();

    BOOL UniqueCheck(UINT32 ulTaskID, UINT32 ulMsgID, UINT32 ulPID=0);
    BOOL TaskRegister(UINT32 ulTaskID);

private:
    VECTOR<UNIQUE_MSG_HASH_LIST_T> m_vHaskList;
    UINT32          m_ulListMaxNum;
    HANDLE          m_hMutex;

    BOOL InitMsgList(UINT32 ulListMaxNum);

    UniqueMsgCheck();
    ~UniqueMsgCheck();

    BOOL Lock();
    VOID Unlock();
};
#endif

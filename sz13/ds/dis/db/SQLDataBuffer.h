#ifndef SQL_DATA_BUFFER_H
#define SQL_DATA_BUFFER_H

#include "DSPublic.h"

#define SQL_LIST_MAX_NUM 10000

typedef struct
{
    UINT32 ulDataLen;
    CHAR  *pchSQL;
}SQL_DATA_T;

class SQLDataBuffer
{
public:
    SQLDataBuffer(UINT32 ulListMaxNum);
    ~SQLDataBuffer();

    UINT32 GetDataNum();
    CHAR*  GetDataByIndex(UINT32 ulIndex);
    BOOL   SaveSQLData(CHAR *pchSQLData, UINT32 ulDataLen);
    BOOL   DeleDataByIndex(UINT32 ulIndex);
    BOOL   ClearDataList();

private:
    VECTOR<SQL_DATA_T> m_vSQLDataList;
    HANDLE             m_hMutex;
    UINT32             m_ulListMaxNum;

    BOOL Lock();
    VOID Unlock();
};

#endif

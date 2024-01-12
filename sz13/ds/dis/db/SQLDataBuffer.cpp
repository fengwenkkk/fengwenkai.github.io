#include "SQLDataBuffer.h"

SQLDataBuffer::SQLDataBuffer(UINT32 ulListMaxNum)
{
    m_hMutex = gos_mutex_init();

    m_ulListMaxNum = ulListMaxNum;

    m_vSQLDataList.clear();
}

SQLDataBuffer::~SQLDataBuffer()
{
    UINT32 i = 0;

    for (i=0; i<m_vSQLDataList.size(); i++)
    {
        if (m_vSQLDataList[i].pchSQL != NULL)
        {
            free(m_vSQLDataList[i].pchSQL);
        }
    }

    m_vSQLDataList.clear();
}

BOOL SQLDataBuffer::ClearDataList()
{
    UINT32 i = 0;

    for (i=0; i<m_vSQLDataList.size(); i++)
    {
        if (m_vSQLDataList[i].pchSQL != NULL)
        {
            free(m_vSQLDataList[i].pchSQL);
        }
    }

    m_vSQLDataList.clear();

    return TRUE;
}

UINT32 SQLDataBuffer::GetDataNum()
{
    UINT32 ulDataNum = 0;

    Lock();

    ulDataNum = m_vSQLDataList.size();

    Unlock();

    return ulDataNum;
}

CHAR* SQLDataBuffer::GetDataByIndex(UINT32 ulIndex)
{
    UINT32 ulDataNum = 0;
    CHAR  *pchData   = NULL;

    Lock();

    ulDataNum = m_vSQLDataList.size();

    if (ulIndex >= ulDataNum)
    {
        Unlock();

        return NULL;
    }

    pchData = m_vSQLDataList[ulIndex].pchSQL;

    Unlock();

    return pchData;
}

BOOL SQLDataBuffer::SaveSQLData(CHAR *pchSQLData, UINT32 ulDataLen)
{
    SQL_DATA_T stData;

    if (pchSQLData == NULL || ulDataLen == 0)
    {
        return FALSE;
    }

    Lock();

    stData.ulDataLen = ulDataLen;
    stData.pchSQL    = NULL;
    stData.pchSQL    = (CHAR *)malloc((ulDataLen+1));

    if (stData.pchSQL == NULL)
    {
        Unlock();

        return FALSE;
    }

    memset(stData.pchSQL, 0, ulDataLen+1);

    snprintf(stData.pchSQL, ulDataLen, pchSQLData);

    if (m_vSQLDataList.size() > m_ulListMaxNum)
    {
        if (m_vSQLDataList[0].pchSQL != NULL)
        {
            free(m_vSQLDataList[0].pchSQL);
        }

        m_vSQLDataList.erase(m_vSQLDataList.begin());
    }

    m_vSQLDataList.push_back(stData);

    Unlock();

    return TRUE;
}

BOOL SQLDataBuffer::DeleDataByIndex(UINT32 ulIndex)
{
    Lock();

    if (m_vSQLDataList.size() <= ulIndex)
    {
        Unlock();

        return FALSE;
    }

    if (m_vSQLDataList[ulIndex].pchSQL)
    {
        free(m_vSQLDataList[ulIndex].pchSQL);
    }

    m_vSQLDataList.erase(m_vSQLDataList.begin()+ulIndex);

    Unlock();

    return TRUE;
}

BOOL SQLDataBuffer::Lock()
{
    return gos_mutex(m_hMutex);
}

VOID SQLDataBuffer::Unlock()
{
    gos_unmutex(m_hMutex);
}

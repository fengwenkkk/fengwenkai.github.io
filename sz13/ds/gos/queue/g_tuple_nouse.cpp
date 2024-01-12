#include "g_include.h"
#include "g_tuple.h"

GTuple::GTuple(UINT32 ulMaxNum, UINT32 ulDataLen, BOOL bLock)
{
    m_hMutex = NULL;
    m_ulMaxNum = MAX(1, ulMaxNum);
    if (ulDataLen == 0)
    {
        ulDataLen = 4;
    }

    m_ulDataNum = 0;
    m_ulDataIndex = m_ulMaxNum-1;
    m_ulDataLen = ulDataLen;
    m_pvData = (UINT8*)calloc(1, m_ulDataLen*m_ulMaxNum);

    if (bLock)
    {
        m_hMutex = gos_mutex_init();
    }
}

GTuple::~GTuple()
{
    gos_mutex_free(m_hMutex);
    free(m_pvData);
}

VOID GTuple::InitData(VOID *pvData)
{
    for (UINT32 i=0; i<m_ulMaxNum; i++)
    {
        if (!Add(pvData))
        {
            break;
        }
    }
}

BOOL GTuple::Add(VOID *pvData, BOOL bRemoveFirst)
{
    UINT8       *pucData;
    UINT32      ulIndex;

    if (m_hMutex)
    {
        if (!gos_mutex(m_hMutex))
        {
            return FALSE;
        }
    }

    if (m_ulDataNum >= m_ulMaxNum)
    {
        if (!bRemoveFirst)
        {
            if (m_hMutex)
            {
                gos_unmutex(m_hMutex);
            }

            return FALSE;
        }
    }

    ulIndex = m_ulDataIndex+m_ulDataNum;
    if (ulIndex >= m_ulMaxNum)
    {
        ulIndex -= m_ulMaxNum;
    }

    if (m_ulDataNum < m_ulMaxNum)
    {
        m_ulDataNum++;
    }

    if (m_ulDataNum == m_ulMaxNum)
    {
        m_ulDataIndex++;
        if (m_ulDataIndex == m_ulMaxNum)
        {
            m_ulDataIndex = 0;
        }
    }

    pucData = m_pvData + ulIndex*m_ulDataLen;
    memcpy(pucData, pvData, m_ulDataLen);

    if (m_hMutex)
    {
        gos_unmutex(m_hMutex);
    }

    return TRUE;
}

// InsertIndex表示在该Index之前插入
BOOL GTuple::Insert(UINT32 ulIndex, VOID *pvData)
{
    UINT8       *pucData;
    INT32       iMoveNum;

    if (ulIndex >= m_ulMaxNum)
    {
        return Add(pvData);
    }

    if (m_hMutex)
    {
        if (!gos_mutex(m_hMutex))
        {
            return FALSE;
        }
    }

    if (m_ulDataNum >= m_ulMaxNum)
    {
        if (m_hMutex)
        {
            gos_unmutex(m_hMutex);
        }

        return FALSE;
    }

    // 挪动数据
    m_ulDataNum++;

    iMoveNum = m_ulDataNum - ulIndex;
    pucData = m_pvData+ulIndex*m_ulDataLen;
    memmove(pucData+m_ulDataLen, pucData, m_ulDataLen*iMoveNum);
    memcpy(pucData, pvData, m_ulDataLen);

    if (m_hMutex)
    {
        gos_unmutex(m_hMutex);
    }

    return TRUE;
}

VOID* GTuple::Get(UINT32 ulIndex)
{
    VOID        *pvData = NULL;
    UINT32      ulDataIndex;

    if (m_hMutex)
    {
        if (!gos_mutex(m_hMutex))
        {
            return NULL;
        }
    }

    if (ulIndex >= m_ulDataNum)
    {
        goto end;
    }

    ulDataIndex = m_ulDataIndex+ulIndex;
    if (ulDataIndex >= m_ulMaxNum)
    {
        ulDataIndex -= m_ulMaxNum;
    }

    pvData = m_pvData + ulDataIndex*m_ulDataLen;

end:
    if (m_hMutex)
    {
        gos_unmutex(m_hMutex);
    }

    return pvData;
}

BOOL GTuple::Get(UINT32 ulIndex, VOID *pvData)
{
    BOOL        bRet = FALSE;
    UINT32      ulDataIndex;

    if (m_hMutex)
    {
        if (!gos_mutex(m_hMutex))
        {
            return FALSE;
        }
    }

    if (ulIndex >= m_ulDataNum)
    {
        goto end;
    }

    ulDataIndex = m_ulDataIndex+ulIndex;
    if (ulDataIndex >= m_ulMaxNum)
    {
        ulDataIndex -= m_ulMaxNum;
    }

    memcpy(pvData, m_pvData + ulDataIndex*m_ulDataLen, m_ulDataLen);

    bRet = TRUE;

end:
    if (m_hMutex)
    {
        gos_unmutex(m_hMutex);
    }

    return bRet;
}

BOOL GTuple::Remove()
{
    BOOL        bRet = FALSE;

    if (m_hMutex)
    {
        if (!gos_mutex(m_hMutex))
        {
            return FALSE;
        }
    }

    if (m_ulDataNum == 0)
    {
        bRet = TRUE;
        goto end;
    }

    m_ulDataIndex++;
    m_ulDataNum--;
    if (m_ulDataIndex >= m_ulMaxNum)
    {
        m_ulDataIndex -= m_ulMaxNum;
    }

    bRet = TRUE;

end:
    if (m_hMutex)
    {
        gos_unmutex(m_hMutex);
    }

    return bRet;
}

VOID* GTuple::GetDesc(UINT32 ulIndex)
{
    VOID        *pvData = NULL;
    UINT32      ulDataIndex;

    if (m_hMutex)
    {
        if (!gos_mutex(m_hMutex))
        {
            return NULL;
        }
    }

    if (ulIndex >= m_ulDataNum)
    {
        goto end;
    }

    ulDataIndex = m_ulDataIndex+m_ulDataNum-1-ulIndex;
    if (ulDataIndex >= m_ulMaxNum)
    {
        ulDataIndex -= m_ulMaxNum;
    }

    pvData = m_pvData + ulDataIndex*m_ulDataLen;

end:
    if (m_hMutex)
    {
        gos_unmutex(m_hMutex);
    }

    return pvData;
}

BOOL GTuple::GetDesc(UINT32 ulIndex, VOID *pvData)
{
    BOOL        bRet = FALSE;
    UINT32      ulDataIndex;

    if (m_hMutex)
    {
        if (!gos_mutex(m_hMutex))
        {
            return FALSE;
        }
    }

    if (ulIndex >= m_ulDataNum)
    {
        goto end;
    }

    ulDataIndex = m_ulDataIndex+m_ulDataNum-1-ulIndex;
    if (ulDataIndex >= m_ulMaxNum)
    {
        ulDataIndex -= m_ulMaxNum;
    }

    memcpy(pvData, m_pvData + ulDataIndex*m_ulDataLen, m_ulDataLen);

    bRet = TRUE;

end:
    if (m_hMutex)
    {
        gos_unmutex(m_hMutex);
    }

    return bRet;
}

BOOL GTuple::RemoveDesc()
{
    BOOL        bRet = FALSE;

    if (m_hMutex)
    {
        if (!gos_mutex(m_hMutex))
        {
            return FALSE;
        }
    }

    if (m_ulDataNum == 0)
    {
        bRet = TRUE;
        goto end;
    }

    m_ulDataNum--;

    bRet = TRUE;

end:
    if (m_hMutex)
    {
        gos_unmutex(m_hMutex);
    }

    return bRet;
}

VOID GTuple::Stat(UINT32 ulStatPointNum, DOUBLE &dAvg, DOUBLE &dVar)
{
    UINT32  i;
    DOUBLE  dValue;

    dAvg = 0;
    dVar = 0;

    for (i=0; i<ulStatPointNum; i++)
    {
        GetDesc(i, (VOID*)&dValue);

        dAvg += dValue;
        dVar += dValue*dValue;
    }

    dAvg /= ulStatPointNum;
    dVar = dVar/ulStatPointNum - dAvg*dAvg;
    if (dVar < 0)
    {
        dVar = 0;
    }
    else
    {
        dVar = sqrt(dVar);
    }
}

VOID GTuple::Stat(UINT32 ulStatPointNum, DOUBLE *pdAvg, DOUBLE *pdVar)
{
    Stat(ulStatPointNum, *pdAvg, *pdVar);
}

VOID GTuple::StatTrend(UINT32 ulStatPointNum, DOUBLE &dTrend, DOUBLE &dTrendVar)
{
    DOUBLE      dSumX = 0;
    DOUBLE      dSumY = 0;
    DOUBLE      dSumXY = 0;
    DOUBLE      dSumX2 = 0;
    DOUBLE      dSumY2 = 0;
    DOUBLE      x, y;
    DOUBLE      a,b;

    for (UINT32 i=0; i<ulStatPointNum; i++)
    {
         GetDesc(i, (VOID*)&y);

         x = ulStatPointNum-1-i;
         dSumX += x;
         dSumY += y;
         dSumXY += x*y;
         dSumX2 += x*x;
         dSumY2 += y*y;
    }

    a = ulStatPointNum*dSumX2 - dSumX*dSumX;
    if (a < 0.000001)
    {
        a = 0;
    }
    else
    {
        a = (ulStatPointNum*dSumXY - dSumX*dSumY)/a;
    }

    b = (dSumY - a*dSumX)/ulStatPointNum;

    dTrend = a;
    dTrendVar = a*a*dSumX2 + 2*a*b*dSumX + ulStatPointNum*b*b + dSumY2 - 2*b*dSumY - 2*a*dSumXY;
    if (dTrendVar < 0)
    {
        dTrendVar = 0;
    }
    else
    {
        dTrendVar = sqrt(dTrendVar);
    }
}

VOID GTuple::StatTrend(UINT32 ulStatPointNum, DOUBLE *pdTrend, DOUBLE *pdTrendVar)
{
    StatTrend(ulStatPointNum, *pdTrend, *pdTrendVar);
}

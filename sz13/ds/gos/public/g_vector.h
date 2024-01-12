#ifndef G_VECTOR_H
#define G_VECTOR_H

#include "g_include.h"

template <class _T> class GVector
{
public:
    GVector()
    {
        for (UINT32 i=0; i<ARRAY_SIZE(m_apstData); i++)
        {
            m_apstData[i] = NULL;
        }

        memset(m_aulNum, 0, sizeof(m_aulNum));
        m_ulTupleIndex = 0;
        m_ulDataNum = 0;

        m_ulMaxTupleNum = ARRAY_SIZE(m_apstData);
        m_ulMaxTupleDataNum = 64*1024;
    }

    ~GVector()
    {
        Clear();
    }

    _T* Add(_T *pstData)
    {
        if (m_ulTupleIndex >= m_ulMaxTupleNum)
        {
            return NULL;
        }

        if (m_apstData[m_ulTupleIndex] && m_aulNum[m_ulTupleIndex] >= m_ulMaxTupleDataNum)
        {
            m_ulTupleIndex++;
            if (m_ulTupleIndex >= m_ulMaxTupleNum)
            {
                return NULL;
            }
        }

        if (m_apstData[m_ulTupleIndex] == NULL)
        {
            m_apstData[m_ulTupleIndex] = (_T*)calloc(1, sizeof(_T)*m_ulMaxTupleDataNum);
            m_aulNum[m_ulTupleIndex] = 0;

            if (!m_apstData[m_ulTupleIndex])
            {
                return NULL;
            }
        }

        _T  *pstNewData = m_apstData[m_ulTupleIndex] + m_aulNum[m_ulTupleIndex];

        memcpy(pstNewData, pstData, sizeof(_T));
        m_aulNum[m_ulTupleIndex]++;
        m_ulDataNum++;

        return pstNewData;
    }

    _T* Get(UINT32 ulIndex, _T *pstData=NULL)
    {
        UINT32      ulTupleIndex = ulIndex>>16;
        UINT32      ulTupleDataIndex = ulIndex & 0x0000ffff;

        if (ulTupleIndex >= m_ulMaxTupleNum)
        {
            return NULL;
        }

        if (ulTupleDataIndex >= m_aulNum[ulTupleIndex])
        {
            return NULL;
        }

        _T  *pstFindData = m_apstData[ulTupleIndex] + ulTupleDataIndex;

        if (pstData)
        {
            memcpy(pstData, pstFindData, sizeof(_T));
            return pstData;
        }

        return pstFindData;
    }

    _T * operator [](UINT32 ulIndex)
    {
        return Get(ulIndex);
    }

    _T* Add(_T &stData)
    {
        return Add(&stData);
    }

    UINT32 Size()
    {
        return m_ulDataNum;
    }

    VOID Clear()
    {
        for (UINT32 i=0; i<ARRAY_SIZE(m_apstData); i++)
        {
            GOS_FREE(m_apstData[i]);
        }

        memset(m_aulNum, 0, sizeof(m_aulNum));
        m_ulTupleIndex = 0;
        m_ulDataNum = 0;
    }

private:
    _T      *m_apstData[64*1024];
    UINT32  m_aulNum[64*1024];
    UINT32  m_ulTupleIndex;

    UINT32  m_ulMaxTupleNum;
    UINT32  m_ulMaxTupleDataNum;
    UINT32  m_ulDataNum;
};

#endif

#ifndef _G_MAP_H
#define _G_MAP_H

#include "g_include.h"

#ifdef WIN32
#pragma warning (disable : 4018 4663 )
#endif

#include <map>
#include <vector>

template <typename KT, typename VT>
class VectorMap
{
typedef std::vector<KT> VectorKey;

public:
    VT* Add(KT Key, VT &Value)
    {
        if (FIND_MAP_KEY(m_mValue, Key))
        {
            m_mValue[Key] = Value;
        }
        else
        {
            m_mValue[Key] = Value;
            m_vKey.push_back(Key);
            m_vValue.push_back(&m_mValue[Key]);
        }

        return &m_mValue[Key];
    }

    BOOL AddAll(VectorMap<KT, VT> &VecMap)
    {
        BOOL bRet = TRUE;

        for(UINT32 i=0; i<VecMap.Size(); i++)
        {
            KT key = VecMap.GetKey(i);
            VT *value = VecMap.GetValue(key);
            if (!Add(key, *value))
            {
                return FALSE;
            }
        }

        return TRUE;
    }

    UINT32 Size()
    {
        return m_vKey.size();
    }

    BOOL HasKey(KT Key)
    {
        return FIND_MAP_KEY(m_mValue, Key);
    }

    VECTOR<KT> &GetKeyList()
    {
        return m_vKey;
    }

    VOID GetKeyList(VECTOR<KT> &vKey)
    {
        vKey.assign(m_vKey.begin(), m_vKey.end());
    }

    VECTOR<VT *> &GetValueList()
    {
        return m_vValue;
    }

    VOID GetValueList(VECTOR<VT *> &vValue)
    {
        vValue.assign(m_vValue.begin(), m_vValue.end());
    }

    KT GetKey(UINT32 ulIndex)
    {
        return m_vKey[ulIndex];
    }

    VT* GetValue(KT Key)
    {
        if (FIND_MAP_KEY(m_mValue, Key))
        {
            return &m_mValue[Key];
        }

        return NULL;
    }

    VT* GetValueByIndex(UINT32 ulIndex)
    {
        if (ulIndex >= m_vValue.size())
        {
            return NULL;
        }

        return m_vValue[ulIndex];
    }

    VT *operator[](UINT32 ulIndex)
    {
        return GetValueByIndex(ulIndex);
    }

    VOID Clear()
    {
        m_mValue.clear();
        m_vKey.clear();
        m_vValue.clear();
    }

    VOID DelByIndex(UINT32 ulIndex)
    {
        if (ulIndex >= m_vKey.size())
        {
            return;
        }

        m_mValue.erase(m_vKey[ulIndex]);
        m_vKey.erase(m_vKey.begin()+ulIndex);
        m_vValue.erase(m_vValue.begin()+ulIndex);
    }

    VOID Del(KT Key)
    {
        m_mValue.erase(Key);

        for (UINT32 i=0; i<m_vKey.size(); i++)
        {
            if (m_vKey[i] == Key)
            {
                m_vKey.erase(m_vKey.begin()+i);
                m_vValue.erase(m_vValue.begin()+i);
                break;
            }
        }
    }

private:
    std::map<KT, VT>    m_mValue;
    std::vector<KT>     m_vKey;
    std::vector<VT*>    m_vValue;
};

typedef struct
{
    UINT32      ulKey;
    VOID        *pvData;
}INT_MAP_RECORD_T;

// 释放模式
typedef enum
{
    FM_NULL,
    FM_FREE,
    FM_DELETE
}FREE_MODE_E;

// Clone模式下，VT必须是简单数据类型，Value内容会Clone一份，删除时自动释放
// 非Clone模式下，VT类型没有限制，Value内容为指针，不Clone，删除时根据释放模式Value
template <class VT>
class IntMap
{
public:
    IntMap(UINT32 ulEntrySize, BOOL bCloneMode, FREE_MODE_E enFreeMode=FM_NULL)
    {
        m_bCloneMode = bCloneMode;
        m_enFreeMode = enFreeMode;
        m_pfFreeFunc = NULL;

        if (bCloneMode)
        {
            m_enFreeMode = FM_FREE;
        }

        m_pHash = new GHashList(ulEntrySize, sizeof(INT_MAP_RECORD_T), gos_hash_int, gos_hash_key_cmp_int);
    }

    IntMap(UINT32 ulEntrySize, FREE_MODE_E enFreeMode=FM_NULL)
    {
        m_bCloneMode = FALSE;
        m_enFreeMode = enFreeMode;
        m_pfFreeFunc = NULL;

        m_pHash = new GHashList(ulEntrySize, sizeof(INT_MAP_RECORD_T), gos_hash_int, gos_hash_key_cmp_int);
    }

    IntMap(UINT32 ulEntrySize, GOS_HASH_FREE_FUNC pfFreeFunc)
    {
        m_bCloneMode = FALSE;
        m_enFreeMode = FM_NULL;
        m_pfFreeFunc = pfFreeFunc;

        m_pHash = new GHashList(ulEntrySize, sizeof(INT_MAP_RECORD_T), gos_hash_int, gos_hash_key_cmp_int);
    }

    ~IntMap()
    {
        if (m_enFreeMode != FM_NULL || m_pfFreeFunc)
        {
            for (GHASHNODE *pNode=m_pHash->GetFirstNode(); pNode; pNode=m_pHash->GetNextNode(pNode))
            {
                INT_MAP_RECORD_T *pstRecord = (INT_MAP_RECORD_T*)m_pHash->GetNodeData(pNode);

                _Delete(pstRecord);
            }
        }

        delete m_pHash;
    }

    VOID _Delete(VT *pValue)
    {
        if (!pValue)
        {
            return;
        }

        if (m_pfFreeFunc)
        {
            m_pfFreeFunc(pValue);
            return;
        }

        if (m_enFreeMode == FM_FREE)
        {
            free(pValue);
        }
        else if (m_enFreeMode == FM_DELETE)
        {
            delete pValue;
        }
    }

    VOID _Delete(INT_MAP_RECORD_T *pstRecord)
    {
        if (!pstRecord)
        {
            return;
        }

        _Delete((VT*)pstRecord->pvData);
    }

    VT* Add(UINT32 ulKey, VT *pValue)
    {
        INT_MAP_RECORD_T    *pstRecord = (INT_MAP_RECORD_T*)m_pHash->Get(&ulKey);
        INT_MAP_RECORD_T    stRecord;
        VT                  *pNewValue = pValue;

        if (m_bCloneMode)
        {
            pNewValue = (VT*)calloc(1, sizeof(VT));

            if (!pNewValue)
            {
                return NULL;
            }

            if (pValue)
            {
                memcpy(pNewValue, pValue, sizeof(VT));
            }
        }

        if (pstRecord)
        {
            if (!pNewValue)
            {
                return NULL;
            }

            _Delete((VT *)pstRecord->pvData);
            pstRecord->pvData = pNewValue;

            return pNewValue;
        }

        stRecord.ulKey = ulKey;
        stRecord.pvData = (VOID*)pNewValue;

        if (!m_pHash->Add(&stRecord, sizeof(stRecord)))
        {
            if (m_bCloneMode)
            {
                free(pNewValue);
            }

            return NULL;
        }

        return pNewValue;
    }

    UINT32 Size()
    {
        return m_pHash->GetUsedNum();
    }

    BOOL Del(UINT32 ulKey)
    {
        VT  *pValue = GetValue(ulKey);

        if (!pValue)
        {
            return FALSE;
        }

        if (!m_pHash->Remove(&ulKey))
        {
            return FALSE;
        }

        _Delete(pValue);

        return TRUE;
    }

    VOID GetKeyList(VECTOR<UINT32> &vKey)
    {
        vKey.clear();

        for (GHASHNODE *pNode=m_pHash->GetFirstNode(); pNode; pNode=m_pHash->GetNextNode(pNode))
        {
            INT_MAP_RECORD_T    *pstRecord = (INT_MAP_RECORD_T*)m_pHash->GetNodeData(pNode);

            vKey.push_back(pstRecord->ulKey);
        }
    }

    VOID GetValueList(VECTOR<VT*> &vValue)
    {
        vValue.clear();

        for (GHASHNODE *pNode=m_pHash->GetFirstNode(); pNode; pNode=m_pHash->GetNextNode(pNode))
        {
            INT_MAP_RECORD_T    *pstRecord = (INT_MAP_RECORD_T*)m_pHash->GetNodeData(pNode);

            vValue.push_back((VT*)pstRecord->pvData);
        }
    }

    VOID GetKeyValueList(VECTOR<UINT32> &vKey, VECTOR<VT*> &vValue)
    {
        vValue.clear();

        for (GHASHNODE *pNode=m_pHash->GetFirstNode(); pNode; pNode=m_pHash->GetNextNode(pNode))
        {
            INT_MAP_RECORD_T    *pstRecord = (INT_MAP_RECORD_T*)m_pHash->GetNodeData(pNode);

            vKey.push_back(pstRecord->ulKey);
            vValue.push_back((VT*)pstRecord->pvData);
        }
    }

    BOOL Has(UINT32 ulKey)
    {
        return (m_pHash->Get(&ulKey) != NULL);
    }

    VT* GetValue(UINT32 ulKey)
    {
        INT_MAP_RECORD_T    *pstRecord = (INT_MAP_RECORD_T*)m_pHash->Get(&ulKey);

        if (!pstRecord)
        {
            return NULL;
        }

        return (VT*)pstRecord->pvData;
    }

    VOID Clear()
    {
        VECTOR<UINT32>  vKey;

        GetKeyList(vKey);

        for (UINT32 i=0; i<vKey.size(); i++)
        {
            Del(vKey[i]);
        }
    }

private:
    FREE_MODE_E         m_enFreeMode;
    BOOL                m_bCloneMode;

    GOS_HASH_FREE_FUNC  m_pfFreeFunc;
    GHashList           *m_pHash;
};

template <class VT>
class RawIntMap : public IntMap<VT>
{
public:
    RawIntMap(UINT32 ulEntrySize=16): IntMap<VT>(ulEntrySize, TRUE, FM_NULL)
    {
    }
};

typedef struct
{
    INT64       i64Key;
    VOID        *pvData;
}INT64_MAP_RECORD_T;

// Clone模式下，VT必须是简单数据类型，Value内容会Clone一份，删除时自动释放
// 非Clone模式下，VT类型没有限制，Value内容为指针，不Clone，删除时根据释放模式Value
template <class VT>
class Int64Map
{
public:
    Int64Map(UINT32 ulEntrySize, BOOL bCloneMode, FREE_MODE_E enFreeMode=FM_NULL)
    {
        m_bCloneMode = bCloneMode;
        m_enFreeMode = enFreeMode;
        m_pfFreeFunc = NULL;

        if (bCloneMode)
        {
            m_enFreeMode = FM_FREE;
        }

        m_pHash = new GHashList(ulEntrySize, sizeof(INT64_MAP_RECORD_T), gos_hash_int64, gos_hash_key_cmp_int64);
    }

    Int64Map(UINT32 ulEntrySize, FREE_MODE_E enFreeMode=FM_NULL)
    {
        m_bCloneMode = FALSE;
        m_enFreeMode = enFreeMode;
        m_pfFreeFunc = NULL;

        m_pHash = new GHashList(ulEntrySize, sizeof(INT64_MAP_RECORD_T), gos_hash_int64, gos_hash_key_cmp_int64);
    }

    Int64Map(UINT32 ulEntrySize, GOS_HASH_FREE_FUNC pfFreeFunc)
    {
        m_bCloneMode = FALSE;
        m_enFreeMode = FM_NULL;
        m_pfFreeFunc = pfFreeFunc;

        m_pHash = new GHashList(ulEntrySize, sizeof(INT64_MAP_RECORD_T), gos_hash_int64, gos_hash_key_cmp_int64);
    }

    ~Int64Map()
    {
        if (m_enFreeMode != FM_NULL || m_pfFreeFunc)
        {
            for (GHASHNODE *pNode=m_pHash->GetFirstNode(); pNode; pNode=m_pHash->GetNextNode(pNode))
            {
                INT64_MAP_RECORD_T *pstRecord = (INT64_MAP_RECORD_T*)m_pHash->GetNodeData(pNode);

                _Delete(pstRecord);
            }
        }

        delete m_pHash;
    }

    VOID _Delete(VT *pValue)
    {
        if (!pValue)
        {
            return;
        }

        if (m_pfFreeFunc)
        {
            m_pfFreeFunc(pValue);
            return;
        }

        if (m_enFreeMode == FM_FREE)
        {
            free(pValue);
        }
        else if (m_enFreeMode == FM_DELETE)
        {
            delete pValue;
        }
    }

    VOID _Delete(INT64_MAP_RECORD_T *pstRecord)
    {
        if (!pstRecord)
        {
            return;
        }

        _Delete((VT*)pstRecord->pvData);
    }

    VT* Add(INT64 i64Key, VT *pValue)
    {
        INT64_MAP_RECORD_T  *pstRecord = (INT64_MAP_RECORD_T*)m_pHash->Get(&i64Key);
        INT64_MAP_RECORD_T  stRecord;
        VT                  *pNewValue = pValue;

        if (m_bCloneMode)
        {
            pNewValue = (VT*)calloc(1, sizeof(VT));

            if (!pNewValue)
            {
                return NULL;
            }

            if (pValue)
            {
                memcpy(pNewValue, pValue, sizeof(VT));
            }
        }

        if (pstRecord)
        {
            if (!pNewValue)
            {
                return NULL;
            }

            _Delete((VT *)pstRecord->pvData);
            pstRecord->pvData = pNewValue;

            return pNewValue;
        }

        stRecord.i64Key = i64Key;
        stRecord.pvData = (VOID*)pNewValue;

        if (!m_pHash->Add(&stRecord, sizeof(stRecord)))
        {
            if (m_bCloneMode)
            {
                free(pNewValue);
            }

            return NULL;
        }

        return pNewValue;
    }

    UINT32 Size()
    {
        return m_pHash->GetUsedNum();
    }

    BOOL Del(INT64 i64Key)
    {
        VT  *pValue = GetValue(i64Key);

        if (!pValue)
        {
            return FALSE;
        }

        if (!m_pHash->Remove(&i64Key))
        {
            return FALSE;
        }

        _Delete(pValue);

        return TRUE;
    }

    VOID GetKeyList(VECTOR<INT64> &vKey)
    {
        vKey.clear();

        for (GHASHNODE *pNode=m_pHash->GetFirstNode(); pNode; pNode=m_pHash->GetNextNode(pNode))
        {
            INT64_MAP_RECORD_T  *pstRecord = (INT64_MAP_RECORD_T*)m_pHash->GetNodeData(pNode);

            vKey.push_back(pstRecord->i64Key);
        }
    }

    VOID GetValueList(VECTOR<VT*> &vValue)
    {
        vValue.clear();

        for (GHASHNODE *pNode=m_pHash->GetFirstNode(); pNode; pNode=m_pHash->GetNextNode(pNode))
        {
            INT64_MAP_RECORD_T  *pstRecord = (INT64_MAP_RECORD_T*)m_pHash->GetNodeData(pNode);

            vValue.push_back((VT*)pstRecord->pvData);
        }
    }

    VT* GetValue(INT64 i64Key)
    {
        INT64_MAP_RECORD_T  *pstRecord = (INT64_MAP_RECORD_T*)m_pHash->Get(&i64Key);

        if (!pstRecord)
        {
            return NULL;
        }

        return (VT*)pstRecord->pvData;
    }

    BOOL Has(INT64 i64Key)
    {
        return m_pHash->Get(&i64Key) != NULL;
    }

    VOID Clear()
    {
        VECTOR<INT64>  vKey;

        GetKeyList(vKey);

        for (UINT32 i=0; i<vKey.size(); i++)
        {
            Del(vKey[i]);
        }
    }

private:
    FREE_MODE_E         m_enFreeMode;
    BOOL                m_bCloneMode;

    GOS_HASH_FREE_FUNC  m_pfFreeFunc;
    GHashList           *m_pHash;
};

template <class VT>
class RawInt64Map : public Int64Map<VT>
{
public:
    RawInt64Map(UINT32 ulEntrySize=32): Int64Map<VT>(ulEntrySize, TRUE, FM_NULL)
    {
    }
};

typedef struct
{
    CHAR        *szKey;
    VOID        *pvData;
}STRING_MAP_RECORD_T;

static UINT32 _hash_string(VOID *pKey)
{
    CHAR    *szKey = *((CHAR**)pKey);
    return gos_hash_string(szKey);
}

static BOOL _hash_key_cmp_string(VOID* pSrcKey, VOID *pDstKey)
{
    CHAR    *szSrc = *((CHAR**)pSrcKey);
    CHAR    *szDst = *((CHAR**)pDstKey);

    return gos_hash_key_cmp_string(szSrc, szDst);
}

template <class VT>
class StringMap
{
public:
    StringMap(UINT32 ulEntrySize/*, BOOL bClearObject*/)
    {
        m_bClearObject = TRUE;//bClearObject;
        m_pfFreeFunc = NULL;

        m_pHash = new GHashList(ulEntrySize, sizeof(STRING_MAP_RECORD_T), _hash_string, _hash_key_cmp_string);
    }

    StringMap(UINT32 ulEntrySize, GOS_HASH_FREE_FUNC pfFreeFunc)
    {
        m_bClearObject = TRUE;
        m_pfFreeFunc = pfFreeFunc;

        m_pHash = new GHashList(ulEntrySize, sizeof(STRING_MAP_RECORD_T), _hash_string, _hash_key_cmp_string);
    }

    VOID Clear()
    {
        if (m_bClearObject)
        {
            for (GHASHNODE *pNode=m_pHash->GetFirstNode(); pNode; pNode=m_pHash->GetNextNode(pNode))
            {
                STRING_MAP_RECORD_T *pstRecord = (STRING_MAP_RECORD_T*)m_pHash->GetNodeData(pNode);

                if (m_pfFreeFunc)
                {
                    m_pfFreeFunc(pstRecord->pvData);
                }
                else
                {
                    VT  *pValue = (VT*)pstRecord->pvData;

                    delete pValue;
                }
            }
        }
    }

    ~StringMap()
    {
        Clear();

        delete m_pHash;
    }

    BOOL Add(CHAR *szKey, VT *pValue)
    {
        STRING_MAP_RECORD_T stRecord;

        stRecord.szKey = szKey;
        stRecord.pvData = new VT;

        memcpy(stRecord.pvData, pValue, sizeof(VT));

        return m_pHash->Add(&stRecord, sizeof(stRecord)) != NULL;
    }

    BOOL Del(CHAR *szKey)
    {
        VT  *pValue = GetValue(szKey);

        if (!pValue)
        {
            return FALSE;
        }

        if (!m_pHash->Remove(&szKey))
        {
            return FALSE;
        }

        delete pValue;
    }

    UINT32 Size()
    {
        return m_pHash->GetUsedNum();
    }

    VOID GetKeyList(VECTOR<CHAR*> &vKey)
    {
        vKey.clear();

        for (GHASHNODE *pNode=m_pHash->GetFirstNode(); pNode; pNode=m_pHash->GetNextNode(pNode))
        {
            STRING_MAP_RECORD_T *pstRecord = (STRING_MAP_RECORD_T*)m_pHash->GetNodeData(pNode);

            vKey.push_back(pstRecord->szKey);
        }
    }

    VOID GetValueList(VECTOR<VT*> &vValue)
    {
        vValue.clear();

        for (GHASHNODE *pNode=m_pHash->GetFirstNode(); pNode; pNode=m_pHash->GetNextNode(pNode))
        {
            INT_MAP_RECORD_T    *pstRecord = (INT_MAP_RECORD_T*)m_pHash->GetNodeData(pNode);

            vValue.push_back((VT*)pstRecord->pvData);
        }
    }

    VT* GetValue(CHAR *szKey)
    {
        STRING_MAP_RECORD_T *pstRecord = (STRING_MAP_RECORD_T*)m_pHash->Get(&szKey);

        if (!pstRecord)
        {
            return NULL;
        }

        return (VT*)pstRecord->pvData;
    }

private:
    BOOL                m_bClearObject;
    GOS_HASH_FREE_FUNC  m_pfFreeFunc;
    GHashList           *m_pHash;
};

//template <class VT> class IntSet
class IntSet
{
public:
    IntSet(UINT32 ulEntrySize=32)
    {
        m_pHash = new GHashList(ulEntrySize, sizeof(UINT32), gos_hash_int, gos_hash_key_cmp_int);
    }

    ~IntSet()
    {
        delete m_pHash;
    }

    VOID Init(IntSet &s)
    {
        VECTOR<UINT32>  vList;

        Clear();
        s.GetList(vList);

        for (UINT32 i=0; i<vList.size(); i++)
        {
            Add(vList[i]);
        }
    }

    UINT32 Size()
    {
        return m_pHash->GetUsedNum();
    }

    BOOL Add(UINT32 ulKey)
    {
        return NULL != m_pHash->Add(&ulKey, sizeof(ulKey));
    }

    VOID Add(VECTOR<UINT32> &vList)
    {
        for (UINT32 i=0; i<vList.size(); i++)
        {
            Add(vList[i]);
        }
    }

    VOID Del(VECTOR<UINT32> &vList)
    {
        for (UINT32 i=0; i<vList.size(); i++)
        {
            Del(vList[i]);
        }
    }

    BOOL Del(UINT32 ulKey)
    {
        return m_pHash->Remove(&ulKey);
    }

    BOOL Has(UINT32 ulKey)
    {
        return NULL != m_pHash->Get(&ulKey);
    }

    UINT32 Get(UINT32 ulIndex)
    {
        UINT32  *pulValue = (UINT32*)m_pHash->Data(ulIndex);

        if (pulValue)
        {
            return *pulValue;
        }

        return 0xffffffff;
    }

    VOID GetList(VECTOR<UINT32> &vList)
    {
        vList.clear();

        for (GHASHNODE *pNode=m_pHash->GetFirstNode(); pNode; pNode=m_pHash->GetNextNode(pNode))
        {
            UINT32  *pulValue = (UINT32*)m_pHash->GetNodeData(pNode);

            vList.push_back(*pulValue);
        }
    }

    VOID Clear()
    {
        m_pHash->Clear();
    }

    UINT32 operator[](UINT32 ulIndex)
    {
        return Get(ulIndex);
    }

private:
    GHashList           *m_pHash;
};

class Int64Set
{
public:
    Int64Set(UINT32 ulEntrySize=32)
    {
        m_pHash = new GHashList(ulEntrySize, sizeof(INT64), gos_hash_int64, gos_hash_key_cmp_int64);
    }

    ~Int64Set()
    {
        delete m_pHash;
    }

    UINT32 Size()
    {
        return m_pHash->GetUsedNum();
    }

    VOID Init(Int64Set &s)
    {
        VECTOR<INT64>   vList;

        Clear();
        s.GetList(vList);

        for (UINT32 i=0; i<vList.size(); i++)
        {
            Add(vList[i]);
        }
    }

    BOOL Add(INT64 i64Key)
    {
        return NULL != m_pHash->Add(&i64Key, sizeof(i64Key));
    }

    VOID Add(VECTOR<INT64> &vKey)
    {
        for (UINT32 i=0; i<vKey.size(); i++)
        {
            Add(vKey[i]);
        }
    }

    BOOL Del(INT64 i64Key)
    {
        return m_pHash->Remove(&i64Key);
    }

    BOOL Has(INT64 i64Key)
    {
        return NULL != m_pHash->Get(&i64Key);
    }

    INT64 Get(UINT32 ulIndex)
    {
        INT64   *pi64Value = (INT64*)m_pHash->Data(ulIndex);

        if (pi64Value)
        {
            return *pi64Value;
        }

        return -1;
    }

    VOID GetList(VECTOR<INT64> &vKey)
    {
        vKey.clear();

        for (GHASHNODE *pNode=m_pHash->GetFirstNode(); pNode; pNode=m_pHash->GetNextNode(pNode))
        {
            INT64   *pi64Key = (INT64*)m_pHash->GetNodeData(pNode);

            vKey.push_back(*pi64Key);
        }
    }

    VOID Clear()
    {
        m_pHash->Clear();
    }

    INT64 operator[](UINT32 ulIndex)
    {
        return Get(ulIndex);
    }

private:
    GHashList           *m_pHash;
};

#endif

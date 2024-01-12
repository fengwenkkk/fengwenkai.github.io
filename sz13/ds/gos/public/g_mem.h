#ifndef _G_MEM_H
#define _G_MEM_H

class GMemAlloc
{
public:
    virtual VOID* MemAlloc(UINT32 ulSize)=0;
    virtual VOID MemFree(VOID* ptr)=0;
};

class GSystemMemAlloc:public GMemAlloc
{
public:
    virtual VOID* MemAlloc(UINT32 ulSize)
    {
        return malloc(ulSize);
    }

    virtual VOID MemFree(VOID* ptr)
    {
        if (ptr)
        {
            free(ptr);
        }
    }
};

class GMemPool:public GMemAlloc
{
public:
    ~GMemPool()
    {
        Free();
    }

    static GMemPool* GetInstance(GOS_MEM_POOL_T *pstMemPool, UINT32 ulMemPoolNum, BOOL bCanDynamicAlloc=TRUE, BOOL bUseLock=FALSE);

    virtual VOID* MemAlloc(UINT32 ulSize);
    virtual VOID MemFree(VOID* ptr);

private:
    HANDLE      m_hPool;

    BOOL Init(GOS_MEM_POOL_T *pstMemPool, UINT32 ulMemPoolNum, BOOL bCanDynamicAlloc, BOOL bUseLock);
    VOID Free();
};

#endif

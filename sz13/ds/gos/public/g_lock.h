#ifndef G_LOCK_H
#define G_LOCK_H

class GLock
{
public:
    GLock();
    ~GLock();

    BOOL Lock(UINT32 ulTimeout=WAIT_FOREVER);
    BOOL Unlock();

private:
    HANDLE      m_hLock;
};

class GMutex
{
public:
    GMutex();
    ~GMutex();

    BOOL Mutex();
    VOID Unmutex();

private:
    HANDLE      m_hMutex;
};

class GAutoLock
{
public:
    GAutoLock(GMutex &Mutex);

public:
    virtual ~GAutoLock(VOID);

protected:
    GMutex  &m_Mutex;
};

#define AUTO_LOCK(Mutex)        GAutoLock __Lock__(Mutex)

class GSem
{
public:
    GSem();
    ~GSem();

    BOOL P(UINT32 ulTimeout=WAIT_FOREVER);
    BOOL V();

private:
    HANDLE      m_hSem;
};

#endif

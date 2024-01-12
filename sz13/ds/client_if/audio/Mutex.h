class   Mutex
{
public:
    Mutex(void);

public:
    virtual ~Mutex(void);

    int   Lock();
    int   UnLock();

protected:
    CRITICAL_SECTION    m_CritSec;
};

 class  AutoLock
{
public:
    AutoLock(Mutex& mutex);

public:
    virtual ~AutoLock(void);

protected:
    Mutex&    m_Mutex;
};

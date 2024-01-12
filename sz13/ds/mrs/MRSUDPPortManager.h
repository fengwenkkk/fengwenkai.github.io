#ifndef MRS_UDP_PORT_MANAGER
#define MRS_UDP_PORT_MANAGER

#include "g_include.h"

typedef struct
{
    UINT16      usPort;
    SOCKET      stSocket;
    BOOL        bAlloced;
}UDP_INFO_T;

class MrsUDPPortManager
{
public:

    static MrsUDPPortManager &GetInstance()
    {
        static MrsUDPPortManager *pInstance = NULL;
        if (pInstance == NULL)
        {
            pInstance = new MrsUDPPortManager();
        }

        return *pInstance;
    }

    UINT16  AllocUDPPort();
    VOID    FreeUDPPort(UINT16 usPort);
    BOOL    IsUDPPortFree(UINT16 usPort);
    BOOL    AllocUDPPort(UINT16 &usPort, SOCKET &stSocket);
    SOCKET  GetSocket(UINT16 usPort, CHAR *szBusinessName);

private:
    MrsUDPPortManager();
    ~MrsUDPPortManager();

    /// �ú�����ʵ�֣���ֹ��������
    MrsUDPPortManager(const MrsUDPPortManager &other);
    /// �ú�����ʵ�֣���ֹ��������
    MrsUDPPortManager &operator=(const MrsUDPPortManager &other);

    RawIntMap<UDP_INFO_T>    *m_pmUDPInfo;
    
};

#endif
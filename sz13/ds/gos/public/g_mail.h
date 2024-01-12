#ifndef G_MAIL_H_
#define G_MAIL_H_

#include "g_include.h"

#define MAIL_ECHO_LEN            1024
#define MAIL_RECV_TIMEOUT        20
#define MAIL_CHECK_TIMEOUT       0

typedef enum
{
    MAIL_SUC = 0,
    MAIL_ERR_CONN,
    MAIL_ERR_LOGIN,
    MAIL_ERR_QUIT,
    MAIL_ERR_USER,
    MAIL_ERR_PWD,
    MAIL_ERR_SEND,
    MAIL_ERR_HELO,
}MAIL_SAT_E;

typedef struct
{
    CHAR    acUserName[256];
    CHAR    acPassword[256];
    UINT8   aucIPAddr[4];
}MAIL_USER_INFO_T;

typedef struct
{
    CHAR    acFrom[64];
    CHAR    acTo[64];
    CHAR    acSubject[128];
    CHAR    acContent[2048 + 256];
}MAIL_INFO_T;

class GMail
{
public:
    static GMail *GetInstance(UINT8 *pucIPAddr=NULL, UINT16 usPort=25);
    GMail(UINT8 *pucIPAddr, UINT16 usPort=25);
    ~GMail();

    BOOL  Init();
    INT32 Connect();
    INT32 Check();
    INT32 Login(MAIL_USER_INFO_T * pstUserInfo);
    INT32 SendMail(MAIL_INFO_T * pstMailInfo);
    INT32 Quit();
    INT32 ReConnect();

    friend INT32 CheckMailServer(GMail *pInst);

private:
    INT32 ConnectServer(SOCKET tSock, SOCKADDR *ptSockAddr, INT32 iAddrLen, UINT32 ulTimeout);
    BOOL  SendMail(CHAR *szMail, UINT32 ulMailLen);
    INT32 RecvMail(CHAR *szMail, UINT32 ulMaxMailLen, UINT32 ulTimeout);

    SOCKET           m_socket;
    SOCKADDR_IN      m_sockAddr;
    BOOL             m_bConnect;
};

extern INT32 CheckMailServer(GMail *pInst);
#endif

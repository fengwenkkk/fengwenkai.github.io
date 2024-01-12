/******************************** 头文件保护开头 *****************************/

#ifndef _GOS_SOCKET_H
#define _GOS_SOCKET_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _OSWIN32_
/// Windows 系统下
typedef struct fd_set           FDSET;

#define GetSockErr              WSAGetLastError
#define SetSockErr              WSASetLastError
#define SEWOULDBLOCK            WSAEWOULDBLOCK
#define SEINPROGRESS            WSAEINPROGRESS
#define SEALREADY               WSAEALREADY
#define SENOTSOCK               WSAENOTSOCK
#define SEDESTADDRREQ           WSAEDESTADDRREQ
#define SEMSGSIZE               WSAEMSGSIZE
#define SEPROTOTYPE             WSAEPROTOTYPE
#define SENOPROTOOPT            WSAENOPROTOOPT
#define SEPROTONOSUPPORT        WSAEPROTONOSUPPORT
#define SESOCKTNOSUPPORT        WSAESOCKTNOSUPPORT
#define SEOPNOTSUPP             WSAEOPNOTSUPP
#define SEPFNOSUPPORT           WSAEPFNOSUPPORT
#define SEAFNOSUPPORT           WSAEAFNOSUPPORT
#define SEADDRINUSE             WSAEADDRINUSE
#define SEADDRNOTAVAIL          WSAEADDRNOTAVAIL
#define SENETDOWN               WSAENETDOWN
#define SENETUNREACH            WSAENETUNREACH
#define SENETRESET              WSAENETRESET
#define SECONNABORTED           WSAECONNABORTED
#define SECONNRESET             WSAECONNRESET
#define SENOBUFS                WSAENOBUFS
#define SEISCONN                WSAEISCONN
#define SENOTCONN               WSAENOTCONN
#define SESHUTDOWN              WSAESHUTDOWN
#define SETOOMANYREFS           WSAETOOMANYREFS
#define SETIMEDOUT              WSAETIMEDOUT
#define SECONNREFUSED           WSAECONNREFUSED
#define SELOOP                  WSAELOOP
#define SEHOSTDOWN              WSAEHOSTDOWN
#define SEHOSTUNREACH           WSAEHOSTUNREACH
#define SEPROCLIM               WSAEPROCLIM
#define SEUSERS                 WSAEUSERS
#define SEDQUOT                 WSAEDQUOT
#define SESTALE                 WSAESTALE
#define SEREMOTE                WSAEREMOTE
#define SEFAULT                 WSAEFAULT
#define SEINVAL                 WSAEINVAL
#define SENOTINITIALISED        WSANOTINITIALISED
#define SEINVALFD               WSAENOTSOCK

#define INVALID_SOCKET          (SOCKET)(~0)
#define SOCKET_ERROR            (-1)


typedef int socklen_t;

#define SOCKADDR                struct sockaddr

#ifdef _BCB_
/// Windows 下 C++ Builder 环境下
typedef struct
{
    union {
        struct {UINT8 s_b1,s_b2,s_b3,s_b4; } S_un_b;
        struct {UINT16 s_w1,s_w2; } S_un_w;
        unsigned long S_addr;
    } S_un;
#define s_addr  S_un.S_addr
                                /* can be used for most tcp & ip code */
#define s_host  S_un.S_un_b.s_b2
                                /* host on imp */
#define s_net   S_un.S_un_b.s_b1
                                /* network */
#define s_imp   S_un.S_un_w.s_w2
                                /* imp */
#define s_impno S_un.S_un_b.s_b4
                                /* imp # */
#define s_lh    S_un.S_un_b.s_b3
                                /* logical host */
}GOS_IN_ADDR;

typedef struct
{
    short           sin_family;
    unsigned short  sin_port;
    GOS_IN_ADDR     sin_addr;
    char            sin_zero[8];
}GOS_SOCKADDR_IN;

typedef struct timeval          TIMEVAL;
#define SOCKADDR_IN             GOS_SOCKADDR_IN
#endif

#else
/// Linux 系统下
typedef fd_set                  FDSET;
typedef struct sockaddr_in      SOCKADDR_IN;
typedef struct timeval          TIMEVAL;
typedef struct sockaddr         SOCKADDR;
typedef struct in_addr          IN_ADDR;
typedef struct ifreq            IFREQ;

#define INVALID_SOCKET          (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#define closesocket             close
#define ioctlsocket(a, b, c)    ioctl(a, b, c)
#define GetSockErr()            errno
#define SetSockErr(err)         {errno = err;}

#define SENOTINITIALISED        0    /* 用于适配, vxworks下无需初始化 */
#define SEWOULDBLOCK            EWOULDBLOCK
#define SEINPROGRESS            EINPROGRESS
#define SEALREADY               EALREADY
#define SENOTSOCK               ENOTSOCK
#define SEDESTADDRREQ           EDESTADDRREQ
#define SEMSGSIZE               EMSGSIZE
#define SEPROTOTYPE             EPROTOTYPE
#define SENOPROTOOPT            ENOPROTOOPT
#define SEPROTONOSUPPORT        EPROTONOSUPPORT
#define SESOCKTNOSUPPORT        ESOCKTNOSUPPORT
#define SEOPNOTSUPP             EOPNOTSUPP
#define SEPFNOSUPPORT           EPFNOSUPPORT
#define SEAFNOSUPPORT           EAFNOSUPPORT
#define SEADDRINUSE             EADDRINUSE
#define SEADDRNOTAVAIL          EADDRNOTAVAIL
#define SENETDOWN               ENETDOWN
#define SENETUNREACH            ENETUNREACH
#define SENETRESET              ENETRESET
#define SECONNABORTED           ECONNABORTED
#define SECONNRESET             ECONNRESET
#define SENOBUFS                ENOBUFS
#define SEISCONN                EISCONN
#define SENOTCONN               ENOTCONN
#define SESHUTDOWN              ESHUTDOWN
#define SETOOMANYREFS           ETOOMANYREFS
#define SETIMEDOUT              ETIMEDOUT
#define SECONNREFUSED           ECONNREFUSED
#define SELOOP                  ELOOP
#define SEHOSTDOWN              EHOSTDOWN
#define SEHOSTUNREACH           EHOSTUNREACH
#define SEPROCLIM               EPROCLIM
#define SEUSERS                 EUSERS
#define SEDQUOT                 EDQUOT
#define SESTALE                 ESTALE
#define SEREMOTE                EREMOTE
#define SEFAULT                 EFAULT
#define SEINVAL                 EINVAL
#define SEINVALFD               ENOTSOCK

#define TCP_NODELAY             1
#endif

#ifdef __cplusplus
}
#endif

#endif

/******************************************************************************
模块名      : public
文件名      : gos_api.h
相关文件    :
实现功能    : GOS模块的接口
修改记录    :
------------------------------------------------------------------------------
日期        修改人      修改内容
2010/05/08  雷杰        创建
******************************************************************************/
#ifndef _GOS_API_H_
#define _GOS_API_H_

#include "gos_typedef.h"
#include "gos_socket.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************/
/* 类型定义 */
/***********************************************************************************/

#define GOS_MALLOC(size)            malloc(size)
#define GOS_ZALLOC(size)            calloc(1, size)
#define GOS_REALLOC(ptr, size)      realloc(ptr, size)
#define GOS_FREE(ptr)                       \
do                                          \
{                                           \
    if (ptr)                                \
    {                                       \
        free(ptr);                          \
        (ptr) = NULL;                       \
    }                                       \
}while(0)

BOOL        gos_init_socket         (VOID);
VOID        gos_init_socket_addr    (SOCKADDR_IN *pstSockAddr, UINT8 *pucAddr, UINT16 usPort);
BOOL        gos_get_socket_addr     (SOCKET stSock, UINT8 *pucAddr, UINT16 *pusPort);
INT32       gos_get_socket_err_code (VOID);
const CHAR* gos_get_socket_err      (VOID);
VOID        gos_close_socket        (SOCKET *pSocket);
SOCKET      gos_connect_tcp_server  (UINT8 *pucServerAddr, UINT16 usPort);
INT32       gos_connect             (SOCKET stSock, SOCKADDR *ptSockAddr, INT32 iAddrLen, UINT32 ulTimeout);
BOOL        gos_tcp_send            (SOCKET stSocket, VOID *pstReqMsg, UINT32 ulReqMsgLen);
BOOL        gos_send_tcp_msg        (UINT8 *pucServerAddr, UINT16 usPort, VOID *pvMsg, UINT32 ulMsgLen);
INT32       gos_recv_tcp_msg        (SOCKET stSock, VOID *pMsgBuf, UINT32 ulMsgBufLen, UINT32 ulTimeout);

BOOL        gos_udp_send            (SOCKET stSocket, VOID *pstReqMsg, UINT32 ulReqMsgLen, SOCKADDR_IN *pstAddr);
SOCKET      gos_create_udp_socket   (VOID);
SOCKET      gos_create_udp_server_socket(UINT8 *pucServerAddr, UINT16 usPort);
INT32       gos_recv_udp_msg        (SOCKET stSock, VOID *pMsgBuf, UINT32 ulMsgBufLen, UINT32 ulTimeout, SOCKADDR_IN *pstClientAddr);

HANDLE      gos_lock_init           (VOID);
BOOL        gos_lock_free           (HANDLE hLock);
BOOL        gos_lock                (HANDLE hLock, UINT32 ulTimeout);
BOOL        gos_unlock              (HANDLE hLock);

HANDLE      gos_mutex_init          (VOID);
VOID        gos_mutex_free          (HANDLE hMutex);
BOOL        gos_mutex               (HANDLE hMutex);
VOID        gos_unmutex             (HANDLE hMutex);

HANDLE      gos_sem_init            (VOID);
BOOL        gos_sem_free            (HANDLE hSem);
BOOL        gos_sem_p               (HANDLE hSem, UINT32 dwTimeout);
BOOL        gos_sem_v               (HANDLE hSem);

UINT32      gos_get_sys_uptime      (VOID);
UINT32      gos_get_sys_uptime_1ms  (VOID);
INT64       gos_get_sys_uptime_1us  (VOID);
UINT32      gos_get_uptime          (VOID);
UINT32      gos_get_uptime_1ms      (VOID);
INT64       gos_get_uptime_1us      (VOID);

VOID        gos_sleep               (UINT32 ulSecond);
VOID        gos_sleep_1ms           (UINT32 ulMillSecond);
UINT32      gos_mktime              (GOS_DATETIME_T *pstTime);
INT32       gos_get_timezone        (VOID);
BOOL        gos_get_abstime         (UINT32 *pulCurrTime, GOS_DATETIME_T *pstTime);
BOOL        gos_get_localtime       (UINT32 *pulCurrTime, GOS_DATETIME_T *pstTime);
UINT32      gos_get_day_from_time   (UINT32 ulTime);
BOOL        gos_get_leapyear        (UINT32 ulYear);
UINT32      gos_get_daycount        (UINT32 ulYear, UINT32 ulMonth);
UINT32      gos_get_dayweek         (UINT16 usYear, UINT8 ucMonth, UINT8 ucDay);
INT32       gos_asctime_ex          (UINT32 ulCurrTime, CHAR *szTime, BOOL bHaveSec);
INT32       gos_asctime             (UINT32 ulCurrTime, CHAR *szTime);
INT32       gos_asctimeval          (TIMEVAL *pstTimeVal, CHAR *szTime);
UINT32      gos_text_time_to_int    (const CHAR *szTime);
UINT32      gos_compact_text_time_to_int(const CHAR *szTime);

BOOL        gos_create_dir_ex       (const CHAR* szDir, UINT32 ulMode);
BOOL        gos_create_dir          (const CHAR* szDir);
BOOL        gos_create_redir_ex     (const CHAR* szDir, UINT32 ulMode);
BOOL        gos_create_redir        (const CHAR* szDir);

BOOL        gos_file_exist          (const CHAR* szFile);
BOOL        gos_delete_file         (const CHAR *szFile);
BOOL        gos_delete_dir          (const CHAR *szDir);
BOOL        gos_delete_dir_full     (const CHAR *szDir);
BOOL        gos_get_dir_info        (const CHAR *szDir, UINT32 *pulFileNum);

BOOL        gos_rename_file         (const CHAR* szOldFile, const CHAR* szNewFile);
BOOL        gos_copy_file           (const CHAR* szOldFile, const CHAR* szNewFile);

THREAD_TOKEN gos_create_thread      (THREAD_ENTRY *pfThreadEntry, UINT32 ulStacksize, VOID *pPara);
BOOL        gos_close_thread        (THREAD_TOKEN ulThreadToken);

VOID        gos_run_daemon          (BOOL bLogToStdout);

VOID        gos_md5                 (const CHAR *szText, UINT8 *pucResult);

CHAR*       gos_get_root_path       (VOID);

VOID*       gos_malloc              (UINT32 ulSize);
VOID        gos_free                (VOID* pMem);

VOID        gos_system              (const CHAR *szCmd);
VOID        gos_reboot              (VOID);

INT32       gos_get_ntp_time        (UINT8 *pucAddr, UINT16 usPort);

VOID        gos_gettimeofday        (TIMEVAL *tp);


#ifdef __cplusplus
}
#endif

#endif /* _GOS_API_H */
/******************************* 头文件结束 **********************************/



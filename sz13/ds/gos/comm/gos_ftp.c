#include "g_include.h"

#define LOG_NULL            0
#define LOG_DETAIL          1
#define LOG_INFO            2
#define LOG_WARN            3
#define LOG_ERROR           4
#define LOG_FAIL            5
#define LOG_FATAL           6

#define GOS_FTP_AGENT_NAME           "ftp_agent"
#define GOS_FTP_RECV_SIZE            (32*1024)

#define GOS_FILE_ISDIR(mode)        ((mode & S_IFMT) == S_IFDIR)     /* directory */
#define GOS_INVALID_FD              ((UINT32)(0xFFFFFFFF))
#define GOS_MAX_OPENED_FILES        60                              /*同时打开的最大文件个数*/

typedef struct
{
    FILE        *pFtpIO;
    INT32       iFtpSocket;
}GOS_FTP_CMD_HANDLER_T;

BOOL gos_ftp_get_dirinfo(CHAR *szUser, CHAR *szPwd, UINT8* pucSvrIP, UINT16 usPort, CHAR *szDir, UINT32 *pulNum, FLOAT *pfSize);


static char     *g_szFtpErrText = NULL;

VOID ftp_log(UINT32 ulLogLevel, CHAR *szLogFormat, ...)
{
#if 1
    CHAR        acLog[4096];
    va_list     ap;

    memset(acLog, 0, sizeof(acLog));

    va_start(ap, szLogFormat);
    vsnprintf(acLog, sizeof(acLog)-1, szLogFormat, ap);

    va_end(ap);

    GOS_FREE(g_szFtpErrText);
    g_szFtpErrText = strdup(acLog);
#endif
}

char* gos_ftp_get_last_err()
{
    return g_szFtpErrText?g_szFtpErrText:(char*)"";
}

BOOL gos_file_stat(const CHAR *szFileName, GOS_FILE_STAT_T *pstStat)
{
#if defined (_OSWIN32_)
    struct _stat stStat;
#else
    struct stat stStat;
#endif

    INT32 lStat;

    /*对传入参数进行检查*/
    if (pstStat == NULL)
    {
        return FALSE;
    }

#if defined (_OSWIN32_)
    lStat = _stat(szFileName, &stStat);
#else
    lStat = stat(szFileName, &stStat);
#endif

    if ( -1 == lStat )
    {
        return FALSE;
    }

    pstStat->usMode   = stStat.st_mode;
    pstStat->ulSize   = stStat.st_size;

    return TRUE;
}

BOOL gos_file_is_dir(const CHAR *szFileName)
{
    GOS_FILE_STAT_T stStat;

    if(szFileName == NULL)
    {
        return FALSE;
    }

    if (!gos_file_stat(szFileName,&stStat))
    {
        return FALSE;
    }

    return GOS_FILE_ISDIR(stStat.usMode);
}

/*******************************************************************************/
#if defined (_OSWIN32_)     /* WINDOWS*/
static UINT32 g_ulOpenedFileNum = 0 ;

UINT32 gos_file_open(const CHAR* szFileName, INT32 lFlag)
{
    CHAR acFileName[ GOS_FS_FILE_ABSPATH_LEN] ;
    INT32 lFileHandle ;

    if(szFileName == NULL)
    {
        return GOS_INVALID_FD ;
    }
    strncpy(acFileName,szFileName, GOS_FS_FILE_ABSPATH_LEN);

    /*
    检查是否超过打开最大文件数
    */
    if(g_ulOpenedFileNum > GOS_MAX_OPENED_FILES)
    {
        return GOS_INVALID_FD ;
    }

    /*
    调用操作系统的实现函数
    */

    /*如果没有定义文本打开方式，转化打开方式为二进制方式*/
    if((lFlag & GOS_FILE_OTEXT) == 0)
    {
        lFlag |= GOS_FILE_OBINARY ;
    }

    /*转化文件名*/
#ifdef _BCB_
    lFileHandle = open(acFileName,lFlag,0644);
#else
    lFileHandle = _open(acFileName,lFlag,0644);
#endif
    if ( lFileHandle == -1 )
    {
        return GOS_INVALID_FD ;
    }

    g_ulOpenedFileNum++ ;
    return (UINT32)lFileHandle;
}

BOOL gos_file_close(UINT32 ulFd)
{
    INT32 lClose = 0;
    lClose = _close(ulFd);
    if ( -1 == lClose )
    {
        return FALSE;
    }

    if (g_ulOpenedFileNum)
    {
        g_ulOpenedFileNum-- ;
    }

    return TRUE;
}

BOOL _gos_get_filename(CHAR *szDirName, CHAR *szFileName)
{
    INT32 lBufLowPtr = 0 ;
    INT32 lBufHighPtr = strlen(szDirName)-1 ;

    /*得到文件名的末指针*/
    while((szDirName[lBufHighPtr] == '\\')||(szDirName[lBufHighPtr] == '/'))
    {
        lBufHighPtr -- ;
        if(lBufHighPtr < 0)
        {
            return FALSE;
        }
    }

    lBufLowPtr = lBufHighPtr ;

    /*得到文件名的始指针*/
    while((szDirName[lBufLowPtr]!='\\')&&(szDirName[lBufLowPtr]!='/'))
    {
        lBufLowPtr-- ;
        if(lBufLowPtr == 0)
        {
            lBufLowPtr = -1 ;
            break ;
        }
    }

    /*拷贝文件名到目的字符串中*/
    strncpy(szFileName,&szDirName[lBufLowPtr+1],lBufHighPtr-lBufLowPtr) ;
    *(szFileName + lBufHighPtr-lBufLowPtr) = 0 ;

    return TRUE;
}

BOOL _gos_path_link(CHAR *szDirPath, CHAR *szDirName, UINT32 ulPathLen)
{
    UINT32 ulStrLen ;
    UINT32 ulSumStrLen;

    ulSumStrLen = strlen(szDirPath)+strlen(szDirName);
    if( ulSumStrLen > ulPathLen)
    {
        return FALSE;
    }

    ulStrLen = strlen(szDirPath) ;

    if(szDirPath[ulStrLen-1] != '\\')
    {
        szDirPath[ulStrLen] = '\\' ;
        ulStrLen++ ;
    }

    strcpy(&szDirPath[ulStrLen], szDirName) ;

    return TRUE ;
}

#ifdef _OSWIN32_
BOOL _gos_ftp_get_svr_file_len(INT32 lCtrlSock, INT32 lDataSock, UINT32 *pulFileLen, CHAR *szServerFile)
{
    WIN32_FIND_DATA stFileInfo ;
    CHAR            acServerFilePath[GOS_FS_FILE_ABSPATH_LEN] ;

    strncpy(acServerFilePath, (CHAR*)szServerFile, GOS_FS_FILE_ABSPATH_LEN) ;

    if(NULL == FtpFindFirstFile((HINTERNET)lDataSock,acServerFilePath,&stFileInfo,0,0))
    {
        return FALSE ;
    }

    if(stFileInfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
    {
        return FALSE ;
    }

    if(stFileInfo.nFileSizeHigh != 0)
    {
        return FALSE ;
    }

    *pulFileLen = stFileInfo.nFileSizeLow ;

    return TRUE ;
}
#endif

/*******************************************************************************
* 函数名称:     _gos_ftp_connect
* 功    能:
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL _gos_ftp_connect(GOS_FTP_CONNECT_T *pstFTPConnect,           /*FTP连接结构指针*/
                      UINT32* pulDownFileLen)
{
    UINT32  ulFileLen ;
    BOOL    bRet;

    /*打开Internet会话*/
    pstFTPConnect->pvInternetSession = InternetOpen(GOS_FTP_AGENT_NAME, LOCAL_INTERNET_ACCESS, NULL,0,0) ;
    if(pstFTPConnect->pvInternetSession == NULL)
    {
        return FALSE;
    }

    /*打开connectiong session会话*/
    pstFTPConnect->pvConnectSession = InternetConnect(pstFTPConnect->pvInternetSession,
                                                      pstFTPConnect->szServerIP,
                                                      pstFTPConnect->usPort,
                                                      pstFTPConnect->szUserName,
                                                      pstFTPConnect->szPasswd,
                                                      INTERNET_SERVICE_FTP,
                                                      INTERNET_FLAG_PASSIVE,
                                                      0) ;
    if(pstFTPConnect->pvConnectSession == NULL)
    {
        InternetCloseHandle(pstFTPConnect->pvInternetSession) ;
        return FALSE ;
    }

    if(NULL != pulDownFileLen)
    {
        bRet = _gos_ftp_get_svr_file_len(0,(INT32)pstFTPConnect->pvConnectSession,&ulFileLen,pstFTPConnect->szServerFileName) ;
        if(!bRet)
        {
            InternetCloseHandle(pstFTPConnect->pvConnectSession) ;
            InternetCloseHandle(pstFTPConnect->pvInternetSession) ;
            return FALSE ;
        }

        *pulDownFileLen = ulFileLen ;
    }

    /*打开FTP会话*/
    if((pstFTPConnect->ulCmd == GOS_FTP_CMD_PUT_FILE)||(pstFTPConnect->ulCmd == GOS_FTP_CMD_PUT_DIR_PACK))
    {
        pstFTPConnect->pvFTPSession = FtpOpenFile(pstFTPConnect->pvConnectSession,
                                                  pstFTPConnect->szServerFileName,
                                                  GENERIC_WRITE,
                                                  FTP_TRANSFER_TYPE_BINARY,
                                                  0) ;
    }
    else if((pstFTPConnect->ulCmd == GOS_FTP_CMD_GET_FILE)||(pstFTPConnect->ulCmd == GOS_FTP_CMD_GET_DIR_PACK))
    {
        pstFTPConnect->pvFTPSession = FtpOpenFile(pstFTPConnect->pvConnectSession,
                                                  pstFTPConnect->szServerFileName,
                                                  GENERIC_READ,
                                                  FTP_TRANSFER_TYPE_BINARY,
                                                  0) ;
    }

    if (pstFTPConnect->ulCmd == GOS_FTP_CMD_PUT_FILE || pstFTPConnect->ulCmd == GOS_FTP_CMD_PUT_DIR_PACK||
        pstFTPConnect->ulCmd == GOS_FTP_CMD_GET_FILE || pstFTPConnect->ulCmd == GOS_FTP_CMD_GET_DIR_PACK)
    {
        /*打开FTP会话失败*/
        if(pstFTPConnect->pvFTPSession == NULL)
        {
            InternetCloseHandle(pstFTPConnect->pvConnectSession) ;
            InternetCloseHandle(pstFTPConnect->pvInternetSession) ;
            return FALSE ;
        }
    }

    return TRUE;
}

/*******************************************************************************
* 函数名称:     _gos_ftp_disconnect
* 功    能:
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
VOID _gos_ftp_disconnect(GOS_FTP_CONNECT_T* pstFTPConnect)
{
    /*关闭internet handle、connection handle、ftp handle*/
    if (pstFTPConnect->pvFTPSession)
    {
        InternetCloseHandle(pstFTPConnect->pvFTPSession);
    }

    InternetCloseHandle(pstFTPConnect->pvConnectSession);
    InternetCloseHandle(pstFTPConnect->pvInternetSession);
}

/*******************************************************************************
* 函数名称:     _gos_ftp_get_win
* 功    能:
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL _gos_ftp_get_win(GOS_FTP_REQ_MSG_T* pstFTPReqMsg)
{
    UINT32  ulBytes = 0 ;
    UINT32  ulBlockRead = 0 ;
    UINT32  ulFileHandle= 0 ;
    UINT8   *pucBuf = NULL;
    CHAR    acClientFileName[GOS_FS_FILE_ABSPATH_LEN] = {0};
    CHAR    acServerFileName[GOS_FS_FILE_ABSPATH_LEN] = {0};
    CHAR    acFileName[GOS_FS_FILENAME_LEN] ;
    CHAR    acServerIp[64] = {0};
    struct in_addr stAddr;
    GOS_FTP_CONNECT_T stFTPConnect ;
    INT32   lStatus = TRUE;
//    INT32   lFtpConn = 0;
    BOOL    bInternetReadFile = FALSE;
    INT32   lWrite = 0;
    BOOL    bIsDir ;
    UINT32  ulCurrFtpLen = 0 ;
    UINT32  ulFtpLenStep = 0 ;
    UINT32  ulDownFileLen = 0 ;

    if((pstFTPReqMsg->ulLen >0 ) &&
       (pstFTPReqMsg->pucMem == NULL)&&
       (pstFTPReqMsg->ulFtpCmd == GOS_FTP_CMD_GET_FILE))
    {
        return FALSE;
    }

    pucBuf = (UINT8*)malloc(GOS_FTP_RECV_SIZE);
    if (!pucBuf)
    {
        return FALSE;
    }

    if(pstFTPReqMsg->ulLen == 0)
    {
        strncpy(acClientFileName, pstFTPReqMsg->acClientFile, GOS_FS_FILE_ABSPATH_LEN);
    }

    strncpy(acServerFileName, pstFTPReqMsg->acServerFile, GOS_FS_FILE_ABSPATH_LEN);

    /*得到字符串形式的IP地址*/
    stAddr.s_addr = pstFTPReqMsg->ulServerIP ;
    strcpy(acServerIp,inet_ntoa(stAddr));

    stFTPConnect.szServerIP = acServerIp ;
    stFTPConnect.usPort = pstFTPReqMsg->usPort;
    stFTPConnect.szServerFileName = acServerFileName ;
    stFTPConnect.szUserName = pstFTPReqMsg->acUserName ;
    stFTPConnect.szPasswd = pstFTPReqMsg->acPasswd ;
    stFTPConnect.ulCmd = pstFTPReqMsg->ulFtpCmd ;

    /*连接FTP服务器*/
    if (!_gos_ftp_connect(&stFTPConnect,&ulDownFileLen))
    {
        GOS_FREE(pucBuf);
        return FALSE;
    }

    /*打开文件*/
    if((pstFTPReqMsg->ulLen == 0)||(pstFTPReqMsg->ulFtpCmd == GOS_FTP_CMD_GET_DIR_PACK))
    {
        bIsDir = gos_file_is_dir(acClientFileName);
        if(TRUE == bIsDir)
        {
            _gos_get_filename(acServerFileName,acFileName);

            /*组织目录名+文件名*/
            if (!_gos_path_link(acClientFileName,acFileName, GOS_FS_FILE_ABSPATH_LEN) )
            {
                lStatus = FALSE ;
            }
        }

        /*在同样的目录下构造一个在源文件名后加上.tmp后缀的临时文件名*/
        if (strlen(acClientFileName)+strlen(".tmp") < GOS_FS_FILE_ABSPATH_LEN)
        {
            strcat(acClientFileName, ".tmp") ;
        }
        else
        {
            lStatus = FALSE ;
        }

        /*以写方式打开文件*/
        ulFileHandle = gos_file_open(acClientFileName, GOS_FILE_ORDWR|GOS_FILE_OCREATE|GOS_FILE_OTRUNC|GOS_FILE_OBINARY) ;
        if(ulFileHandle == GOS_INVALID_FD)
        {
            lStatus = FALSE ;
        }

        if(lStatus == FALSE)
        {
            _gos_ftp_disconnect(&stFTPConnect) ;

            GOS_FREE(pucBuf);

            return FALSE ;
        }
    }

    while(1)
    {
        /*从FTP读取数据*/
        bInternetReadFile = InternetReadFile(stFTPConnect.pvFTPSession,pucBuf,GOS_FTP_RECV_SIZE, (DWORD*)&ulBytes);
        if( bInternetReadFile == TRUE)
        {
            if(ulBytes==0)
            {
                break ;
            }

            /*如果是下载过程，且用户内存大于0，则将读到的数据存在用户传进行来的内存中 */
            if((pstFTPReqMsg->ulLen > 0)&&(pstFTPReqMsg->ulFtpCmd == GOS_FTP_CMD_GET_FILE))
            {
                ulBlockRead = ulBlockRead + ulBytes ;
                if(ulBlockRead > pstFTPReqMsg->ulLen)
                {
                    lStatus = FALSE ;
                    break ;
                }
                memcpy(pstFTPReqMsg->pucMem+ulBlockRead-ulBytes,pucBuf,ulBytes) ;
            }
            else
            {
                /*将数据写到写文件描述符中*/
                lWrite = _write(ulFileHandle,pucBuf,ulBytes);
                if( -1 == lWrite)
                {
                    lStatus = FALSE ;
                    break ;
                }
            }

            ulCurrFtpLen = ulCurrFtpLen + ulBytes;
            ulFtpLenStep = ulFtpLenStep + ulBytes;
        }
        else
        {
            lStatus = FALSE ;
            break ;
        }
    }

    /*释放ftp的资源*/
    _gos_ftp_disconnect(&stFTPConnect) ;

    GOS_FREE(pucBuf);

    /*如果状态为ERROR进行错误处理*/
    if(lStatus == FALSE)
    {
        if((pstFTPReqMsg->ulLen == 0)||(pstFTPReqMsg->ulFtpCmd == GOS_FTP_CMD_GET_DIR_PACK))
        {
            gos_file_close(ulFileHandle) ;
            gos_delete_file(acClientFileName) ;
        }

        return FALSE ;
    }

    /*如果是下载过程，且文件存在本地硬盘，则将存储的临时文件重命名为用户规定的文件名 */
    if((pstFTPReqMsg->ulLen == 0)||(pstFTPReqMsg->ulFtpCmd == GOS_FTP_CMD_GET_DIR_PACK))
    {
        gos_file_close(ulFileHandle) ;

        strncpy(acServerFileName,acClientFileName, GOS_FS_FILE_ABSPATH_LEN) ;

        acServerFileName[strlen(acServerFileName)-strlen(".tmp")] = '\0' ;

        /*如果下载的本地文件名存在则返回失败*/
        if (gos_file_exist(acServerFileName))
        {
            gos_delete_file(acServerFileName) ;
        }

        if (!gos_rename_file(acClientFileName,acServerFileName))
        {
            gos_delete_file(acClientFileName) ;

            return FALSE ;
        }
    }

    /*FTP_GET过程正确完成*/
    return TRUE;
}

/*******************************************************************************
* 函数名称:     _gos_ftp_put_win
* 功    能:
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL _gos_ftp_put_win(GOS_FTP_REQ_MSG_T* pstFTPReqMsg, BOOL *pbPrjDelFlag)
{
    UINT32  ulBytes = 0 ;
    UINT32  ulFileHandle= 0 ;
    UINT8   *pucBuf;
    CHAR    acClientFileName[GOS_FS_FILE_ABSPATH_LEN] ;
    CHAR    acServerFileName[GOS_FS_FILE_ABSPATH_LEN] ;
    CHAR    acFileName[GOS_FS_FILENAME_LEN] ;
    CHAR    acServerIp[64];
    struct in_addr stAddr;
    GOS_FTP_CONNECT_T stFTPConnect ;
    INT32  lStatus = TRUE;
    BOOL    bInternetWriteFile = FALSE;
    INT32   lRead = 0;
    BOOL    bIsDir ;
    UINT32 ulCurrFtpLen = 0 ;
    UINT32 ulFtpLenStep = 0 ;

    if(pbPrjDelFlag && *pbPrjDelFlag)
    {
        return FALSE;
    }

    pstFTPReqMsg->ulLen = 0;

    if((pstFTPReqMsg->ulLen >0 ) &&
       (pstFTPReqMsg->pucMem == NULL)&&
       (pstFTPReqMsg->ulFtpCmd == GOS_FTP_CMD_PUT_FILE))
    {
        return FALSE;
    }

    pucBuf = (UINT8*)malloc(GOS_FTP_RECV_SIZE);
    if (!pucBuf)
    {
        return FALSE;
    }

    if(pstFTPReqMsg->ulLen == 0)
    {
        strncpy(acClientFileName, pstFTPReqMsg->acClientFile, GOS_FS_FILE_ABSPATH_LEN);
    }

    strncpy(acServerFileName, pstFTPReqMsg->acServerFile, GOS_FS_FILE_ABSPATH_LEN);

    /*得到字符串形式的IP地址*/
    stAddr.s_addr = pstFTPReqMsg->ulServerIP ;
    strcpy(acServerIp, inet_ntoa(stAddr));

    stFTPConnect.szServerIP = acServerIp ;
    stFTPConnect.usPort = pstFTPReqMsg->usPort;
    stFTPConnect.szServerFileName = acServerFileName ;
    stFTPConnect.szUserName = pstFTPReqMsg->acUserName ;
    stFTPConnect.szPasswd = pstFTPReqMsg->acPasswd ;
    stFTPConnect.ulCmd = pstFTPReqMsg->ulFtpCmd ;

    /*连接FTP服务器*/
    if (!_gos_ftp_connect(&stFTPConnect, NULL))
    {
        GOS_FREE(pucBuf);
        return FALSE;
    }

    if(pbPrjDelFlag && *pbPrjDelFlag)
    {
        _gos_ftp_disconnect(&stFTPConnect) ;
        GOS_FREE(pucBuf);
        return FALSE;
    }

    /*打开文件*/
    if((pstFTPReqMsg->ulLen == 0)||(pstFTPReqMsg->ulFtpCmd == GOS_FTP_CMD_PUT_DIR_PACK))
    {
        bIsDir = gos_file_is_dir(acClientFileName);
        if(TRUE == bIsDir)
        {
            _gos_get_filename(acServerFileName,acFileName);

            /*组织目录名+文件名*/
            if (!_gos_path_link(acClientFileName,acFileName, GOS_FS_FILE_ABSPATH_LEN) )
            {
                _gos_ftp_disconnect(&stFTPConnect) ;
                GOS_FREE(pucBuf);
                lStatus = FALSE ;
            }
        }

        /*以写方式打开文件*/
        ulFileHandle = gos_file_open(acClientFileName, GOS_FILE_ORDONLY|GOS_FILE_OBINARY) ;
        if(ulFileHandle == GOS_INVALID_FD)
        {
            _gos_ftp_disconnect(&stFTPConnect) ;
            GOS_FREE(pucBuf);
            lStatus = FALSE ;
        }

        if(lStatus == FALSE)
        {
            _gos_ftp_disconnect(&stFTPConnect) ;
            gos_file_close(ulFileHandle);
            GOS_FREE(pucBuf);
            return FALSE ;
        }
    }

    while(1)
    {
        if(pbPrjDelFlag && *pbPrjDelFlag)
        {
            lStatus = FALSE;
            break;
        }
        /*将数据写到写文件描述符中*/
        lRead = _read(ulFileHandle,pucBuf,GOS_FTP_RECV_SIZE);
        if( -1 == lRead)
        {
            lStatus = FALSE ;
            break ;
        }

        if(lRead == 0)
        {
            break ;
        }

        /*从FTP写数据*/
        bInternetWriteFile = InternetWriteFile(stFTPConnect.pvFTPSession,pucBuf,lRead, (DWORD*)&ulBytes);
        if( bInternetWriteFile == FALSE)
        {
            lStatus = FALSE ;
            break;
        }

        if((INT32)(ulBytes) != lRead)
        {
            lStatus = FALSE;
            break ;
        }

        ulCurrFtpLen += ulBytes ;
        ulFtpLenStep += ulBytes ;
    }

    /*释放ftp的资源*/
    _gos_ftp_disconnect(&stFTPConnect) ;

    /*如果状态为ERROR进行错误处理*/
    gos_file_close(ulFileHandle);

    GOS_FREE(pucBuf);

    if(lStatus == FALSE)
    {
        return FALSE ;
    }

    /*FTP_PUT过程正确完成*/
    return TRUE;
}
#endif  /* #if defined (_OSWIN32_)     */

/*******************************************************************************
* 函数名称:     _gos_ftp_init
* 功    能:
* 参    数:
* 参数名称           输入/输出          描述
  pstHandler        IN                  FTP句柄
* 函数返回:
*
* 说    明:    初始化socket，只有在windows环境下才需要
*******************************************************************************/
BOOL _gos_ftp_init(GOS_FTP_CMD_HANDLER_T *pstHandler)
{
    gos_init_socket();

    memset(pstHandler, 0, sizeof(GOS_FTP_CMD_HANDLER_T));
    pstHandler->iFtpSocket = -1;

    return TRUE;
}

/*******************************************************************************
* 函数名称:     _gos_ftp_cmd
* 功    能:     执行FTP命令
* 参    数:
* 参数名称           输入/输出          描述
  pstHandler        IN                  FTP句柄t
  szFormat          IN                  FTP命令
* 函数返回:
*
* 说    明:
*******************************************************************************/
INT32 _gos_ftp_cmd(GOS_FTP_CMD_HANDLER_T *pstHandler, CHAR *szFormat, ...)
{
    va_list vp;
    INT32   iErr = -1;
    INT32   iCmdLen;
    CHAR    *szBuf = (CHAR *)malloc(GOS_FTP_RECV_SIZE+1);

    if (NULL == szBuf)
    {
        return -1;
    }

    if (pstHandler->pFtpIO == NULL)
    {
        if ((pstHandler->pFtpIO = fdopen(pstHandler->iFtpSocket, "r")) == NULL)
        {
            free(szBuf);
            return -1;
        }
    }

    szBuf[0] = '\0';
    if (szFormat)
    {
        va_start(vp, szFormat);
        iCmdLen = vsprintf(szBuf, szFormat, vp);
        szBuf[iCmdLen++] = '\r';
        szBuf[iCmdLen++]='\n';
        /* @lijiancong: 这句话没有写成功的分支没有覆盖 */
        /* 2019-03-07 13:15:12 */
#pragma warning(suppress: 6031)
        write(pstHandler->iFtpSocket, szBuf, iCmdLen);
    }
    else
    {
    }

    do
    {
        if (fgets(szBuf, GOS_FTP_RECV_SIZE+1, pstHandler->pFtpIO) == NULL)
        {
            free(szBuf);
            return -1;
        }
    } while(szBuf[3] == '-');

    /* @lijiancong: 这句话没有读成功的分支没有覆盖，szBuf可能为空 */
    /* 2019-03-07 13:16:16 */
#pragma warning(suppress: 6031)
#pragma warning(suppress: 6387)
    sscanf(szBuf, "%d", &iErr);

    free(szBuf);

    return iErr;
}

BOOL _gos_ftp_get(GOS_FTP_REQ_MSG_T* pstFTPReqMsg)
{
    CHAR    aucServerIp[32];
    int sockdata = -1,sockxfer = -1;
    struct sockaddr_in addr = {0};
    int len,tmp,err;
    int retval = -1;
    int savefd;
    unsigned char *c,*p;
    int delta;
    GOS_FTP_CMD_HANDLER_T stHandler = {0};
    CHAR    buf[1024];

    addr.sin_family = PF_INET;
    addr.sin_port = htons(pstFTPReqMsg->usPort);
    addr.sin_addr.s_addr = pstFTPReqMsg->ulServerIP;

    strcpy(aucServerIp, inet_ntoa(addr.sin_addr));

    if (!_gos_ftp_init(&stHandler))
    {
        ftp_log(LOG_ERROR, "gos_ftp_get: _gos_ftp_init failed");
        return FALSE;
    }

    if (pstFTPReqMsg->ulServerIP == (unsigned long)-1)
    {
        ftp_log(LOG_ERROR, "gos_ftp_get: server ip error");
        return FALSE;
    }

    stHandler.iFtpSocket = socket(AF_INET,SOCK_STREAM,0);
    if (stHandler.iFtpSocket == -1)
    {
        ftp_log(LOG_ERROR, "gos_ftp_get: open ctl socket error");
        goto out;
    }

    if (gos_connect(stHandler.iFtpSocket,(struct sockaddr *)&addr,sizeof(addr), 2) == -1)
    {
        ftp_log(LOG_ERROR, "gos_ftp_get: gos_connect fail");
        goto out;
    }

    err = _gos_ftp_cmd(&stHandler,NULL);
    if (err != 220)
    {
        ftp_log(LOG_ERROR, "gos_ftp_get: NULL cmd error(%d)", err);
        goto out;
    }

    err = _gos_ftp_cmd(&stHandler,"USER %s",pstFTPReqMsg->acUserName);
    if (err != 331)
    {
        ftp_log(LOG_ERROR, "gos_ftp_get: USER cmd error(%d)", err);
        goto out;
    }

    err = _gos_ftp_cmd(&stHandler,"PASS %s",pstFTPReqMsg->acPasswd);
    if (err != 230)
    {
        ftp_log(LOG_ERROR, "gos_ftp_get: PASS cmd error(%d)", err);
        goto out;
    }

    err = _gos_ftp_cmd(&stHandler,"TYPE I");
    if (err != 200)
    {
        ftp_log(LOG_ERROR, "gos_ftp_get: TYPE I cmd error(%d)", err);
        goto out;
    }

    sockdata = socket(AF_INET,SOCK_STREAM,0);
    if (sockdata == -1)
    {
        ftp_log(LOG_ERROR, "gos_ftp_get: open data socket error");
        goto out;
    }

    tmp = sizeof(addr);
    getsockname(stHandler.iFtpSocket,(struct sockaddr *)&addr, (socklen_t*)&tmp);
    addr.sin_port = 0;
    if (bind(sockdata,(struct sockaddr *)&addr,sizeof(addr)) == -1)
    {
        ftp_log(LOG_ERROR, "gos_ftp_get: bind data socket error");
        goto out;
    }

    if (listen(sockdata,1) == -1)
    {
        ftp_log(LOG_ERROR, "gos_ftp_get: listen for xfer socket error");
        goto out;
    }

    tmp = sizeof(addr);
    getsockname(sockdata,(struct sockaddr *)&addr, (socklen_t*)&tmp);
    c = (unsigned char *)&addr.sin_addr;
    p = (unsigned char *)&addr.sin_port;
    err = _gos_ftp_cmd(&stHandler,"PORT %d,%d,%d,%d,%d,%d",
                          c[0],c[1],c[2],c[3],p[0],p[1]);

    if (err != 200)
    {
        ftp_log(LOG_ERROR, "gos_ftp_get: PORT cmd error(%d)", err);
        goto out;
    }

    err = _gos_ftp_cmd(&stHandler,"RETR %s",pstFTPReqMsg->acServerFile);
    if (err != 150 && err != 125)
    {
        ftp_log(LOG_ERROR, "gos_ftp_get: RETR cmd error(%d)", err);
        goto out;
    }

    tmp = sizeof(addr);
    sockxfer = accept(sockdata,(struct sockaddr *)&addr, (socklen_t*)&tmp);
    if (sockxfer == -1)
    {
        ftp_log(LOG_ERROR, "gos_ftp_get: accept error");
        goto out;
    }

    savefd = open(pstFTPReqMsg->acClientFile,O_WRONLY|O_CREAT,0644);
    if (savefd == -1)
    {
        ftp_log(LOG_ERROR, "gos_ftp_get: open local file error");
        goto out;
    }

    retval = 0;
    delta = 0;
    while ((len=read(sockxfer,buf,sizeof(buf)))>0)
    {
        /* @lijiancong:没有写write(savefd,buf,len);成功的分支没有覆盖 */
        /* 2019-03-07 13:16:53 */
#pragma warning(suppress: 6031)
        write(savefd,buf,len);
        retval += len;
        delta += len;
        if (delta > 256*1024)
        {
            delta = 0;
            ftp_log(LOG_DETAIL, "gos_ftp_get: %d kbytes transfered", retval/1024);
        }
    };

    close(savefd);

    if (len == -1)
    {
        ftp_log(LOG_ERROR, "gos_ftp_get: read socket error!");
        retval = -1;
        goto out;
    }
    else
    {
        ftp_log(LOG_DETAIL, "gos_ftp_get: %d bytes transfered, file download succeed", retval);
        goto out;
    }

out:
    if (retval == -1)
    {
        gos_delete_file(pstFTPReqMsg->acClientFile);
    }

    close(sockxfer);
    close(sockdata);
    close(stHandler.iFtpSocket);

    if (stHandler.pFtpIO)
    {
        fclose(stHandler.pFtpIO);
        stHandler.pFtpIO = NULL;
    }

    if (retval < 0)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

/*******************************************************************************
* 函数名称:     _gos_ftp_put
* 功    能:
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL _gos_ftp_put(GOS_FTP_REQ_MSG_T* pstFTPReqMsg, BOOL *pbPrjDelFlag)
{
    CHAR    aucServerIp[32];
    int sockdata = -1,sockxfer = -1;
    struct sockaddr_in addr = {0};
    int len,tmp,err;
    int retval = -1;
    int savefd;
    unsigned char *c,*p;
    int delta;
    GOS_FTP_CMD_HANDLER_T stHandler = {0};
    CHAR    buf[1024];

    if(pbPrjDelFlag && *pbPrjDelFlag)
    {
        return FALSE;
    }

    addr.sin_family = PF_INET;
    addr.sin_port = htons(pstFTPReqMsg->usPort);
    addr.sin_addr.s_addr = pstFTPReqMsg->ulServerIP;

    strcpy(aucServerIp, inet_ntoa(addr.sin_addr));

    if (!_gos_ftp_init(&stHandler))
    {
        ftp_log(LOG_ERROR, "_gos_ftp_put: _gos_ftp_init failed");
        return FALSE;
    }
    if (pstFTPReqMsg->ulServerIP == (unsigned long)-1)
    {
        ftp_log(LOG_ERROR, "_gos_ftp_put: server ip error");
        return FALSE;
    }

    stHandler.iFtpSocket = socket(AF_INET,SOCK_STREAM,0);
    if (stHandler.iFtpSocket == -1)
    {
        ftp_log(LOG_ERROR, "_gos_ftp_put: open ctl socket error");
        goto out;
    }

    if (gos_connect(stHandler.iFtpSocket,(struct sockaddr *)&addr,sizeof(addr), 2) == -1)
    {
        ftp_log(LOG_ERROR, "_gos_ftp_put: gos_connect fail");
        goto out;
    }

    err = _gos_ftp_cmd(&stHandler,NULL);
    if (err != 220)
    {
        ftp_log(LOG_ERROR, "_gos_ftp_put: NULL cmd error(%d)", err);
        goto out;
    }

    err = _gos_ftp_cmd(&stHandler,"USER %s",pstFTPReqMsg->acUserName);
    if (err != 331)
    {
        ftp_log(LOG_ERROR, "_gos_ftp_put: USER cmd error(%d)", err);
        goto out;
    }

    err = _gos_ftp_cmd(&stHandler,"PASS %s",pstFTPReqMsg->acPasswd);
    if (err != 230)
    {
        ftp_log(LOG_ERROR, "_gos_ftp_put: PASS cmd error(%d)", err);
        goto out;
    }

    err = _gos_ftp_cmd(&stHandler,"TYPE I");
    if (err != 200)
    {
        ftp_log(LOG_ERROR, "_gos_ftp_put: TYPE I cmd error(%d)", err);
        goto out;
    }

    sockdata = socket(AF_INET,SOCK_STREAM,0);
    if (sockdata == -1)
    {
        ftp_log(LOG_ERROR, "_gos_ftp_put: open data socket error");
        goto out;
    }

    tmp = sizeof(addr);
    getsockname(stHandler.iFtpSocket,(struct sockaddr *)&addr, (socklen_t*)&tmp);
    addr.sin_port = 0;
    if (bind(sockdata,(struct sockaddr *)&addr,sizeof(addr)) == -1)
    {
        ftp_log(LOG_ERROR, "_gos_ftp_put: bind data socket error");
        goto out;
    }

    if (listen(sockdata,1) == -1)
    {
        ftp_log(LOG_ERROR, "_gos_ftp_put: listen for xfer socket error");
        goto out;
    }

    tmp = sizeof(addr);
    getsockname(sockdata,(struct sockaddr *)&addr, (socklen_t*)&tmp);
    c = (unsigned char *)&addr.sin_addr;
    p = (unsigned char *)&addr.sin_port;
    err = _gos_ftp_cmd(&stHandler,"PORT %d,%d,%d,%d,%d,%d",
                          c[0],c[1],c[2],c[3],p[0],p[1]);

    if (err != 200)
    {
        ftp_log(LOG_ERROR, "_gos_ftp_put: PORT cmd error(%d)", err);
        goto out;
    }

    err = _gos_ftp_cmd(&stHandler,"STOR %s",pstFTPReqMsg->acServerFile);
    if (err != 150)
    {
        ftp_log(LOG_ERROR, "_gos_ftp_put: RETR cmd error(%d)", err);
        goto out;
    }

    tmp = sizeof(addr);
    sockxfer = accept(sockdata,(struct sockaddr *)&addr, (socklen_t*)&tmp);
    if (sockxfer == -1)
    {
        ftp_log(LOG_ERROR, "_gos_ftp_put: accept error");
        goto out;
    }

    savefd = open(pstFTPReqMsg->acClientFile,O_RDONLY);
    if (savefd == -1)
    {
        ftp_log(LOG_ERROR, "_gos_ftp_put: open local file error");
        goto out;
    }

    retval = 0;
    delta = 0;
    while ((len=read(savefd,buf,sizeof(buf)))>0)
    {
        if(pbPrjDelFlag && *pbPrjDelFlag)
        {
            ftp_log(LOG_ERROR, "_gos_ftp_put: project is deleting!");
            len = -1;
            break;
        }
        /*@lijiancong: write(sockxfer,buf,len)没有写成功的分支没有覆盖  */
        /* 2019-03-07 13:25:36 */
#pragma warning(suppress: 6031)
        write(sockxfer,buf,len);
        retval += len;
        delta += len;
        if (delta > 256*1024)
        {
            delta = 0;
            ftp_log(LOG_DETAIL, "_gos_ftp_put: %d kbytes transfered", retval/1024);
        }
    };

    close(savefd);

    if (len == -1)
    {
        ftp_log(LOG_ERROR, "_gos_ftp_put: read socket error!");
        retval = -1;
        goto out;
    }
    else
    {
        ftp_log(LOG_DETAIL, "_gos_ftp_put: %d bytes transfered, file upload succeed", retval);
        goto out;
    }

out:

    close(sockxfer);
    close(sockdata);
    close(stHandler.iFtpSocket);

    if (stHandler.pFtpIO)
    {
        fclose(stHandler.pFtpIO);
        stHandler.pFtpIO = NULL;
    }

    if (retval < 0)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

/*******************************************************************************
* 函数名称:     gos_ftp_get
* 功    能:
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_ftp_get(const CHAR *szUser, const CHAR *szPwd, UINT8* pucSvrIP, UINT16 usPort, const CHAR *szSvrFile, const CHAR *szLocalFile)
{
    GOS_FTP_REQ_MSG_T   stReq;

    memset(&stReq, 0, sizeof(stReq));
    strncpy((CHAR*)stReq.acUserName, szUser, sizeof(stReq.acUserName)-1);
    strncpy((CHAR*)stReq.acPasswd, szPwd, sizeof(stReq.acPasswd)-1);

    memcpy(&stReq.ulServerIP, pucSvrIP, 4);
    stReq.usPort = usPort;

    strncpy((CHAR*)stReq.acServerFile, szSvrFile, sizeof(stReq.acServerFile)-1);
    strncpy((CHAR*)stReq.acClientFile, szLocalFile, sizeof(stReq.acClientFile)-1);

    stReq.ulFtpCmd = GOS_FTP_CMD_GET_FILE;

    gos_delete_file(szLocalFile);

#ifdef _OSWIN32_
    return _gos_ftp_get_win(&stReq);
#else
    return _gos_ftp_get(&stReq);
#endif
}

/*******************************************************************************
* 函数名称:     gos_ftp_put_with_flag
* 功    能:
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_ftp_put_with_flag(const CHAR *szUser, const CHAR *szPwd, UINT8* pucSvrIP, UINT16 usPort, const CHAR *szSvrFile, const CHAR *szLocalFile, BOOL *pbPrjDelFlag)
{
    GOS_FTP_REQ_MSG_T   stReq;

    memset(&stReq, 0, sizeof(stReq));
    strncpy((CHAR*)stReq.acUserName, szUser, sizeof(stReq.acUserName)-1);
    strncpy((CHAR*)stReq.acPasswd, szPwd, sizeof(stReq.acPasswd)-1);

    memcpy(&stReq.ulServerIP, pucSvrIP, 4);
    stReq.usPort = usPort;

    strncpy((CHAR*)stReq.acServerFile, szSvrFile, sizeof(stReq.acServerFile)-1);
    strncpy((CHAR*)stReq.acClientFile, szLocalFile, sizeof(stReq.acClientFile)-1);

    stReq.ulFtpCmd = GOS_FTP_CMD_PUT_FILE;

#ifdef _OSWIN32_
    return _gos_ftp_put_win(&stReq, pbPrjDelFlag);
    //return FALSE;
#else
    return _gos_ftp_put(&stReq, pbPrjDelFlag);
#endif
}

/*******************************************************************************
* 函数名称:     gos_ftp_put
* 功    能:
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_ftp_put(const CHAR *szUser, const CHAR *szPwd, UINT8* pucSvrIP, UINT16 usPort, const CHAR *szSvrFile, const CHAR *szLocalFile)
{
    return gos_ftp_put_with_flag(szUser, szPwd, pucSvrIP, usPort, szSvrFile, szLocalFile, NULL);
}

/*******************************************************************************
* 函数名称:     gos_ftp_create_dir
* 功    能:
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
#ifdef _OSWIN32_
BOOL _gos_ftp_create_dir(GOS_FTP_CONNECT_T *pstFTPConnect, CHAR *szDir)
{
    return FtpCreateDirectory(pstFTPConnect->pvConnectSession, szDir);
}
#endif

BOOL gos_ftp_create_dir(CHAR *szUser, CHAR *szPwd, UINT8* pucSvrIP, UINT16 usPort, CHAR *szDir)
{
#ifdef _OSWIN32_
    GOS_FTP_CONNECT_T   stFTPConnect;
    CHAR                aucServerIp[32];
    BOOL                bRet;

    /*得到字符串形式的IP地址*/
    sprintf(aucServerIp, IP_FMT, IP_ARG(pucSvrIP));

    stFTPConnect.szServerIP = aucServerIp;
    stFTPConnect.usPort = usPort;
    stFTPConnect.szServerFileName = NULL;
    stFTPConnect.szUserName = szUser;
    stFTPConnect.szPasswd = szPwd;
    stFTPConnect.ulCmd = GOS_FTP_CMD_CREATE_DIR;

    /*连接FTP服务器*/
    if (!_gos_ftp_connect(&stFTPConnect, NULL))
    {
        return FALSE;
    }

    bRet = _gos_ftp_create_dir(&stFTPConnect, szDir);

    _gos_ftp_disconnect(&stFTPConnect);

    return bRet;
#else
    return FALSE;
#endif
}

/*******************************************************************************
* 函数名称:     gos_ftp_delete_dir
* 功    能:
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
#ifdef _OSWIN32_
BOOL _gos_ftp_delete_dir(GOS_FTP_CONNECT_T *pstFTPConnect, CHAR *szDir)
{
    return FtpRemoveDirectory(pstFTPConnect->pvConnectSession, szDir);
}
#endif

BOOL gos_ftp_delete_dir(CHAR *szUser, CHAR *szPwd, UINT8* pucSvrIP, UINT16 usPort, CHAR *szDir)
{
#ifdef _OSWIN32_
    GOS_FTP_CONNECT_T   stFTPConnect;
    CHAR                aucServerIp[32];
    BOOL                bRet;

    /*得到字符串形式的IP地址*/
    sprintf(aucServerIp, IP_FMT, IP_ARG(pucSvrIP));

    stFTPConnect.szServerIP = aucServerIp;
    stFTPConnect.usPort = usPort;
    stFTPConnect.szServerFileName = NULL;
    stFTPConnect.szUserName = szUser;
    stFTPConnect.szPasswd = szPwd;
    stFTPConnect.ulCmd = GOS_FTP_CMD_DELETE_DIR;

    /*连接FTP服务器*/
    if (!_gos_ftp_connect(&stFTPConnect, NULL))
    {
        return FALSE;
    }

    bRet = _gos_ftp_delete_dir(&stFTPConnect, szDir);

    _gos_ftp_disconnect(&stFTPConnect);

    return bRet;
#else
    return FALSE;
#endif
}

/*******************************************************************************
* 函数名称:     gos_ftp_delete_dir
* 功    能:
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
#ifdef _OSWIN32_
BOOL _gos_ftp_delete_file(GOS_FTP_CONNECT_T *pstFTPConnect, CHAR *szFile)
{
    return FtpDeleteFile(pstFTPConnect->pvConnectSession, szFile);
}
#endif

BOOL gos_ftp_delete_file(CHAR *szUser, CHAR *szPwd, UINT8* pucSvrIP, UINT16 usPort, CHAR *szFile)
{
#ifdef _OSWIN32_
    GOS_FTP_CONNECT_T   stFTPConnect;
    CHAR                aucServerIp[32];
    BOOL                bRet;

    /*得到字符串形式的IP地址*/
    sprintf(aucServerIp, IP_FMT, IP_ARG(pucSvrIP));

    stFTPConnect.szServerIP = aucServerIp;
    stFTPConnect.usPort = usPort;
    stFTPConnect.szServerFileName = NULL;
    stFTPConnect.szUserName = szUser;
    stFTPConnect.szPasswd = szPwd;
    stFTPConnect.ulCmd = GOS_FTP_CMD_DELETE_FILE;

    /*连接FTP服务器*/
    if (!_gos_ftp_connect(&stFTPConnect, NULL))
    {
        return FALSE;
    }

    bRet = _gos_ftp_delete_file(&stFTPConnect, szFile);

    _gos_ftp_disconnect(&stFTPConnect);

    return bRet;
#else
    return FALSE;
#endif
}

#ifdef _OSWIN32_
/*******************************************************************************
* 函数名称:     _gos_ftp_get_dir_files
* 功    能:
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_ftp_get_dir_files(CHAR *szUser, CHAR *szPwd, UINT8* pucSvrIP, UINT16 usPort,
                            CHAR *szDir, UINT32 ulMaxMallocNum,
                            UINT32 *pulNum, FLOAT *pfSize, GOS_FTP_FILE_T *pstFtpUrl)
{
    UINT32  i;
    FLOAT   fSize = 0;
    CHAR    *szSearch = "*.Kpi";
    HINTERNET hFind;
    WIN32_FIND_DATA stFTPFileData = {0};

    GOS_FTP_CONNECT_T   stFTPConnect;
    CHAR                aucServerIp[32] = {0};
    BOOL                bRet = FALSE;

    *pulNum = 0;

    /*得到字符串形式的IP地址*/
    sprintf(aucServerIp, IP_FMT, IP_ARG(pucSvrIP));

    stFTPConnect.szServerIP = aucServerIp;
    stFTPConnect.usPort = usPort;
    stFTPConnect.szServerFileName = NULL;
    stFTPConnect.szUserName = szUser;
    stFTPConnect.szPasswd = szPwd;
    stFTPConnect.ulCmd = GOS_FTP_CMD_GET_DIRINFO;

    /*连接FTP服务器*/
    if (!_gos_ftp_connect(&stFTPConnect, NULL))
    {
        return FALSE;
    }

    if (TRUE !=FtpSetCurrentDirectory(stFTPConnect.pvConnectSession, szDir))
    {
        goto end;
    }

    hFind = FtpFindFirstFile(stFTPConnect.pvConnectSession, szSearch, &stFTPFileData, 0, 0);
    //free(szSearch);
    if (NULL == hFind)
    {
        goto end;
    }

    i = 0;

    while(TRUE)
    {
        pstFtpUrl[i].acFile[0] = '\0';

        if (strcmp(stFTPFileData.cFileName, ".") != 0 && strcmp(stFTPFileData.cFileName, "..") != 0)
        {
            fSize = 0;

            if (stFTPFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                continue;
            }

            if (0xffffffff == stFTPFileData.nFileSizeLow)
            {
                HINTERNET   hFile;
                DWORD       ulFileSizeHigh = 0;
                DWORD       ulFileSizeLow = 0;

                /* 无法返回大于4G的文件的真实大小，ulFileSizeHigh总是0 !!! */
                hFile = FtpOpenFile(stFTPConnect.pvConnectSession, stFTPFileData.cFileName, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 0);
                ulFileSizeLow = FtpGetFileSize(hFile, &ulFileSizeHigh);
                InternetCloseHandle(hFile);

                fSize += (FLOAT)(ulFileSizeHigh >> 12);
                fSize += ((FLOAT)(ulFileSizeLow) / (FLOAT)(1024*1024));
            }
            else
            {
                fSize += ((FLOAT)(stFTPFileData.nFileSizeLow) / (FLOAT)(1024*1024));
            }

            if (i < ulMaxMallocNum)
            {
                pfSize[i] += fSize;
                sprintf(pstFtpUrl[i].acFile, stFTPFileData.cFileName);
            }

            i++;
        }

        if (!InternetFindNextFile(hFind, &stFTPFileData))
        {
            if (GetLastError() == ERROR_NO_MORE_FILES)
            {
                *pulNum = i;
                bRet = TRUE;
                goto end;
            }
            else
            {
                goto end;
            }
        }
    }

end:

    _gos_ftp_disconnect(&stFTPConnect);
    return bRet;
}
#endif

#ifdef _OSWIN32_
/*******************************************************************************
* 函数名称:     _gos_ftp_get_file_info
* 功    能:
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL _gos_ftp_get_file_info(CHAR *szUser, CHAR *szPwd, UINT8* pucSvrIP, UINT16 usPort,
                            HINTERNET hConnect, CHAR *szDir, UINT32 *pulNum, FLOAT *pfSize)
{
    UINT32  ulNum = 0;
    FLOAT   fSize = 0;
    CHAR    *szSearch = NULL;
    HINTERNET hFind;
    WIN32_FIND_DATA stFTPFileData = {0};

    if (szDir == NULL || szDir[0] == '\0')
    {
        szSearch = strdup("*.*");
    }
    else
    {
        szSearch = (char *)malloc(strlen(szDir)+strlen("\\*.*")+1);
        if (NULL == szSearch)
        {
            return FALSE;
        }
        sprintf(szSearch, "%s\\*.*", szDir);
    }

    if (!szSearch)
    {
        return FALSE;
    }

    hFind = FtpFindFirstFile(hConnect, szSearch, &stFTPFileData, 0, 0);
    free(szSearch);
    if (NULL == hFind)
    {
        return FALSE;
    }

    while(TRUE)
    {
        if (strcmp(stFTPFileData.cFileName, ".") != 0 && strcmp(stFTPFileData.cFileName, "..") != 0)
        {
            ulNum = 0;
            fSize = 0;

            if (stFTPFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (!gos_ftp_get_dirinfo(szUser, szPwd, pucSvrIP, usPort, stFTPFileData.cFileName, &ulNum, &fSize))
                {
                    return FALSE;
                }
            }
            else
            {
                if (0xffffffff == stFTPFileData.nFileSizeLow)
                {
                    HINTERNET   hFile;
                    DWORD       ulFileSizeHigh = 0;
                    DWORD       ulFileSizeLow = 0;

                    /* 无法返回大于4G的文件的真实大小，ulFileSizeHigh总是0 !!! */
                    hFile = FtpOpenFile(hConnect, stFTPFileData.cFileName, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 0);
                    ulFileSizeLow = FtpGetFileSize(hFile, &ulFileSizeHigh);
                    InternetCloseHandle(hFile);

                    fSize += (FLOAT)(ulFileSizeHigh >> 12);
                    fSize += ((FLOAT)(ulFileSizeLow) / (FLOAT)(1024*1024));
                }
                else
                {
                    fSize += ((FLOAT)(stFTPFileData.nFileSizeLow) / (FLOAT)(1024*1024));
                }

                ulNum = 1;
            }

            *pulNum += ulNum;
            *pfSize += fSize;
        }

        if (!InternetFindNextFile(hFind, &stFTPFileData))
        {
            if (GetLastError() == ERROR_NO_MORE_FILES)
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
    }
}
#endif

/*******************************************************************************
* 函数名称:     gos_ftp_get_dirinfo
* 功    能:
* 参    数:
* 参数名称          输入/输出   描述
* pfSize            OUT         单位M
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_ftp_get_dirinfo(CHAR *szUser, CHAR *szPwd, UINT8* pucSvrIP, UINT16 usPort, CHAR *szDir, UINT32 *pulNum, FLOAT *pfSize)
{
#ifdef _OSWIN32_
    GOS_FTP_CONNECT_T   stFTPConnect;
    CHAR                aucServerIp[32] = {0};
    BOOL                bRet;

    /*得到字符串形式的IP地址*/
    sprintf(aucServerIp, IP_FMT, IP_ARG(pucSvrIP));

    stFTPConnect.szServerIP = aucServerIp;
    stFTPConnect.usPort = usPort;
    stFTPConnect.szServerFileName = NULL;
    stFTPConnect.szUserName = szUser;
    stFTPConnect.szPasswd = szPwd;
    stFTPConnect.ulCmd = GOS_FTP_CMD_GET_DIRINFO;

    /*连接FTP服务器*/
    if (!_gos_ftp_connect(&stFTPConnect, NULL))
    {
        return FALSE;
    }

    bRet = _gos_ftp_get_file_info(szUser, szPwd, pucSvrIP, usPort, stFTPConnect.pvConnectSession, szDir, pulNum, pfSize);

    _gos_ftp_disconnect(&stFTPConnect);

    return bRet;
#else
    return FALSE;
#endif
}


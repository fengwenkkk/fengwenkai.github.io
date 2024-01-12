#include "g_include.h"
//#include "gos_base64.h"

GMail * GMail::GetInstance(UINT8 *pucAddr, UINT16 usPort)
{
    static GMail *pInst = NULL;

    if (!pucAddr)
    {
        return pInst;
    }

    if (!pInst)
    {
        pInst = new GMail(pucAddr, usPort);
    }

    return pInst;
}

GMail::GMail(UINT8 *pucIPAddr, UINT16 usPort)
{
    m_socket   = INVALID_SOCKET;
    m_bConnect = FALSE;

    memcpy(&m_sockAddr.sin_addr.s_addr, pucIPAddr, 4);
    m_sockAddr.sin_family = AF_INET;
    m_sockAddr.sin_port   = HTONS(usPort);
}

BOOL GMail::Init()
{
    BOOL bRet = TRUE;

    if (INVALID_SOCKET == m_socket)
    {
        if (!gos_init_socket())
        {
            bRet = FALSE;
            goto end;
        }

        m_socket = socket(AF_INET,SOCK_STREAM, 0);

        if (INVALID_SOCKET == m_socket)
        {
            bRet = FALSE;
            goto end;
        }
    }

end:
    return bRet;
}

INT32 GMail::Connect()
{
    INT32 iRet = MAIL_SUC;
    CHAR  acRecvBuf[1024] = {0};

    if (!m_bConnect)
    {
        if (ConnectServer(m_socket, (SOCKADDR *)&m_sockAddr, sizeof(SOCKADDR), MAIL_CHECK_TIMEOUT) != 0)
        {
            iRet = MAIL_ERR_CONN;
            goto end;
        }

        if (MAIL_ECHO_LEN == RecvMail(acRecvBuf, MAIL_ECHO_LEN, MAIL_RECV_TIMEOUT))
        {
            iRet = MAIL_ERR_CONN;
            goto end;
        }

        if (acRecvBuf[0] != '2' || acRecvBuf[1] != '2' || acRecvBuf[2] != '0')
        {
            iRet = MAIL_ERR_CONN;
            goto end;
        }

        m_bConnect = TRUE;
    }

    return iRet;

end:
    CLOSE_SOCKET(m_socket);
    m_bConnect = FALSE;

    return iRet;
}

INT32 GMail::Check()
{
    INT32 iRet = MAIL_SUC;
    CHAR  acRecvBuf[1024] = {0};
    CHAR  acSendBuf[1024] = {0};

    snprintf(acSendBuf, sizeof(acSendBuf), "HELO %s\r\n", "check");

    if (!SendMail(acSendBuf, strlen(acSendBuf)))
    {
        iRet = MAIL_ERR_SEND;
        goto end;
    }

    if (MAIL_ECHO_LEN == RecvMail(acRecvBuf, MAIL_ECHO_LEN, MAIL_RECV_TIMEOUT))
    {
        iRet = MAIL_ERR_HELO;
        goto end;
    }

    if (acRecvBuf[0] != '2' || acRecvBuf[1] != '5' || acRecvBuf[2] != '0')
    {
        iRet = MAIL_ERR_HELO;
        goto end;
    }

    return iRet;

end:
    CLOSE_SOCKET(m_socket);
    m_bConnect = FALSE;
    return iRet;
}

INT32 GMail::Login(MAIL_USER_INFO_T * pstUserInfo)
{
    INT32   iRet = MAIL_SUC;
    CHAR    acSendBuf[1024] = {0};
    CHAR    acRecvBuf[1024]   = {0};
    UINT8   acEncodeBuf[128]={0};
    INT32   iEncodeBufSize = sizeof(acEncodeBuf);

    if (!SendMail((CHAR*)("AUTH LOGIN\r\n"), strlen("AUTH LOGIN\r\n")))
    {
        iRet = MAIL_ERR_SEND;
        goto end;
    }

    if(MAIL_ECHO_LEN == RecvMail(acRecvBuf, MAIL_ECHO_LEN, MAIL_RECV_TIMEOUT))
    {
        iRet = MAIL_RECV_TIMEOUT;
        goto end;
    }

    if (acRecvBuf[0] != '3' || acRecvBuf[1] != '3' || acRecvBuf[2] != '4')
    {
        iRet = MAIL_ERR_LOGIN;
        goto end;
    }

    gos_base64_encode((unsigned char *)pstUserInfo->acUserName, strlen(pstUserInfo->acUserName), acEncodeBuf, &iEncodeBufSize);

    snprintf(acSendBuf , sizeof(acSendBuf), "%s\r\n", acEncodeBuf);

    if (!SendMail(acSendBuf, strlen(acSendBuf)))
    {
        iRet = MAIL_ERR_SEND;
        goto end;
    }

    if (MAIL_ECHO_LEN == RecvMail(acRecvBuf, MAIL_ECHO_LEN, MAIL_RECV_TIMEOUT))
    {
        iRet = MAIL_ERR_USER;
        goto end;
    }

    if (acRecvBuf[0]!='3' || acRecvBuf[1]!='3' ||acRecvBuf[2]!='4')
    {
        iRet = MAIL_ERR_USER;
        goto end;
    }

    gos_base64_encode((unsigned char *)pstUserInfo->acPassword, strlen(pstUserInfo->acPassword), acEncodeBuf, &iEncodeBufSize);

    snprintf(acSendBuf, sizeof(acSendBuf), "%s\r\n", acEncodeBuf);

    if(!SendMail(acSendBuf, strlen(acSendBuf)))
    {
        iRet = MAIL_ERR_SEND;
        goto end;
    }

    if (MAIL_ECHO_LEN == RecvMail(acRecvBuf, MAIL_ECHO_LEN, MAIL_RECV_TIMEOUT))
    {
        iRet = MAIL_ERR_PWD;
        goto end;
    }

    if (acRecvBuf[0]!='2' || acRecvBuf[1]!='3' ||acRecvBuf[2]!='5')
    {
        iRet = MAIL_ERR_PWD;
        goto end;
    }

end:
    return iRet;
}

INT32 GMail::SendMail(MAIL_INFO_T * pstMailInfo)
{
    INT32 iRet = MAIL_SUC;
    CHAR  acRecvBuf[1024] = {0};
    CHAR  acSendBuf[4096] = {0};

    snprintf(acSendBuf, sizeof(acSendBuf), "MAIL FROM:<%s>\r\n", pstMailInfo->acFrom);

    if (!SendMail(acSendBuf, strlen(acSendBuf)))
    {
        iRet = MAIL_ERR_SEND;
        goto end;
    }

    if (MAIL_ECHO_LEN == RecvMail(acRecvBuf, MAIL_ECHO_LEN, MAIL_RECV_TIMEOUT))
    {
        iRet = MAIL_ERR_SEND;
        goto end;
    }

    if (acRecvBuf[0]!='2' || acRecvBuf[1]!='5' ||acRecvBuf[2]!='0')
    {
        iRet = MAIL_ERR_SEND;
        goto end;
    }

    snprintf(acSendBuf, sizeof(acSendBuf), "RCPT TO:<%s>\r\n", pstMailInfo->acTo);

    if (!SendMail(acSendBuf, strlen(acSendBuf)))
    {
        iRet = MAIL_ERR_SEND;
        goto end;
    }

    if (MAIL_ECHO_LEN == RecvMail(acRecvBuf, MAIL_ECHO_LEN, MAIL_RECV_TIMEOUT))
    {
        iRet = MAIL_ERR_SEND;
        goto end;
    }

    if (acRecvBuf[0]!='2' || acRecvBuf[1]!='5' ||acRecvBuf[2]!='0')
    {
        iRet = MAIL_ERR_SEND;
        goto end;
    }

    snprintf(acSendBuf, sizeof(acSendBuf), "DATA\r\n");

    if (!SendMail(acSendBuf, strlen(acSendBuf)))
    {
        iRet = MAIL_ERR_SEND;
        goto end;
    }

    if (MAIL_ECHO_LEN == RecvMail(acRecvBuf, MAIL_ECHO_LEN, MAIL_RECV_TIMEOUT))
    {
        iRet = MAIL_ERR_SEND;
        goto end;
    }

    if (acRecvBuf[0]!='3' || acRecvBuf[1]!='5' ||acRecvBuf[2]!='4')
    {
        iRet = MAIL_ERR_SEND;
        goto end;
    }

    snprintf(acSendBuf, sizeof(acSendBuf),
             "from: \"%s\"\r\n"
             "to: \"%s\"\r\n"
             "subject: \"%s\"\r\n\r\n %s\r\n.\r\n",
             pstMailInfo->acFrom, pstMailInfo->acTo, pstMailInfo->acSubject, pstMailInfo->acContent);

    if (!SendMail(acSendBuf, strlen(acSendBuf)))
    {
        iRet = MAIL_ERR_SEND;
        goto end;
    }

    if (MAIL_ECHO_LEN == RecvMail(acRecvBuf, MAIL_ECHO_LEN, MAIL_RECV_TIMEOUT))
    {
        iRet = MAIL_ERR_SEND;
        goto end;
    }

    if (acRecvBuf[0]!='2' || acRecvBuf[1]!='5' || acRecvBuf[2]!='0')
    {
        iRet = MAIL_ERR_SEND;
        goto end;
    }

    return iRet;

end:
    CLOSE_SOCKET(m_socket);
    m_bConnect = FALSE;
    return iRet;
}

INT32 GMail::ReConnect()
{
    INT32 iRet = MAIL_SUC;

    if (!Init())
    {
        iRet = MAIL_ERR_CONN;//SOCKET;
        goto end;
    }

    iRet = Connect();

    if (MAIL_SUC != iRet)
    {
        goto end;
    }

    iRet = Check();

    if (MAIL_SUC != iRet)
    {
        goto end;
    }

end:
    return iRet;
}

INT32 GMail::Quit()
{
    INT32  iRet = MAIL_SUC;
    CHAR   acRecvBuf[1024] = {0};

    if (!SendMail((CHAR *)("QUIT\r\n"), strlen("QUIT\r\n")))
    {
        iRet = MAIL_ERR_SEND;
        goto end;
    }

    if (MAIL_ECHO_LEN == RecvMail(acRecvBuf, MAIL_ECHO_LEN, MAIL_RECV_TIMEOUT))
    {
        iRet = MAIL_ERR_QUIT;
        goto end;
    }

    if (acRecvBuf[0]!='2' || acRecvBuf[1]!='2' ||acRecvBuf[2]!='1')
    {
        iRet =  MAIL_ERR_QUIT;
        goto end;
    }

end:
    return iRet;
}

INT32 GMail::ConnectServer(SOCKET tSock, SOCKADDR *ptSockAddr, INT32 iAddrLen, UINT32 ulTimeout)
{
    struct timeval  tTimeout;
    UINT32          dwNonBlock;
    fd_set          tWriteFds;
    SOCKADDR        tPeerSockAddr;
    INT             iLen;
    INT             iResult;
    INT             iRetVal;
    INT             iSockErr;
    INT             iErrno;

    if (0 == ulTimeout)
    {
        return connect(tSock, ptSockAddr, iAddrLen);
    }

    iSockErr = 0;
    dwNonBlock = 1;
    if ((ioctlsocket (tSock, FIONBIO, (u_long*)&dwNonBlock)) == SOCKET_ERROR)
    {
    }

    if (connect (tSock, ptSockAddr, iAddrLen) == SOCKET_ERROR)
    {
        iErrno = errno;
        /*
         * when socket is set to non-blocking mode, connect() will return
         * with WSAEWOULDBLOCK if a connection cannot be setup immediately.
         */

#ifdef _OSWIN32_
        if ((iErrno == SEINPROGRESS) ||
             (iErrno == SETIMEDOUT) ||
             (iErrno == SENOTINITIALISED) ||
             (iErrno == SEWOULDBLOCK) ||
             (iErrno == 2))
#else
        if ((iErrno == SEINPROGRESS) ||
             (iErrno == SETIMEDOUT) ||
             (iErrno == SENOTINITIALISED))
#endif
        {
            /*
             * We do synchronous polling on the write side of this socket
             * using select() with the given <timeVal> timeout value until
             * the connection gets established.
             */
            FD_ZERO (&tWriteFds);
            FD_SET (tSock, &tWriteFds); /* poll write-side of <sock> */
            tTimeout.tv_sec = ulTimeout/1000;
            tTimeout.tv_usec = 1000*MOD(ulTimeout, 1000);
            iResult = select (FD_SETSIZE,
                              (fd_set *) NULL,
                              &tWriteFds,
                              (fd_set *) NULL,
                              &tTimeout
                             );
            if (iResult == SOCKET_ERROR)
            {
                iSockErr = GetSockErr ();
                iRetVal = SOCKET_ERROR;
            }
            else if (iResult == 0) /* time out */
            {
                iSockErr = SETIMEDOUT;
                iRetVal = SOCKET_ERROR;
            }
            else /* if (iResult == SOCKET_ERROR) */
            {
                /*
                 * select() is successful, see if our socket is actually
                 * selected for writing.
                 */
                if (FD_ISSET (tSock, &tWriteFds))
                {
                    /*
                     * connection attempt has completed.  we now see if
                     * it was a successful attempt by trying to get the
                     * remote peer's address.  If getpeername() succeeds
                     * we have a connection, otherwise the connect() has
                     * failed.
                     */
                    iLen = sizeof (tPeerSockAddr);
                    if (getpeername (tSock, &tPeerSockAddr, (socklen_t*)&iLen) != SOCKET_ERROR)
                    {
                        iRetVal = 0;
                    }
                    else
                    {
                        iSockErr = GetSockErr ();
                        iRetVal = SOCKET_ERROR;
                    }
                }
                else /* if (FD_ISSET (tSock, &tWriteFds)) */
                {
                    iSockErr = GetSockErr ();
                    iRetVal = SOCKET_ERROR;
                }
            }
        }
        else /* if (GetSockErr () != SEWOULDBLOCK) */
        {
            iSockErr = GetSockErr ();
            iRetVal = SOCKET_ERROR;
        }
    }
    else /* if (connect (tSock, ptSockAddr, iAddrLen) == SOCKET_ERROR) */
    {
        iRetVal = 0;
    }

    /* 再把 SOCKET 设置为 BLOCK 模式 */
    dwNonBlock = 0;
    if ((ioctlsocket (tSock, FIONBIO, (u_long*)&dwNonBlock)) == SOCKET_ERROR)
    {
        iSockErr = GetSockErr ();
        iRetVal = SOCKET_ERROR;
    }

    if (iSockErr != 0)
    {
        SetSockErr (iSockErr);
    }

    return (iRetVal);
}

BOOL GMail::SendMail(CHAR *szMail, UINT32 ulMailLen)
{
    SOCKET  stSocket = m_socket;
    INT32   iLeftLen = ulMailLen;
    INT32   iSendLen;

    if (stSocket == INVALID_SOCKET)
    {
        return FALSE;
    }

    while(iLeftLen > 0)
    {
        iSendLen = send(stSocket, szMail, iLeftLen, 0);
        if (iSendLen < 0)
        {
            printf("send mail failed: %s\n", gos_get_socket_err());
            return FALSE;
        }

        szMail += iSendLen;
        iLeftLen -= iSendLen;
    }

    return TRUE;
}

INT32 GMail::RecvMail(CHAR *szMail, UINT32 ulMaxMailLen, UINT32 ulTimeout)
{
    SOCKET  stSock = m_socket;
    FD_SET  fds;
    TIMEVAL stTimeout = {0};
    INT32   iRet;
    INT32   iRecvLen;
    INT32   iTotalRecvLen = 0;
    CHAR    *szMsgBuf = (CHAR*)szMail;

    if (stSock == INVALID_SOCKET)
    {
        return -1;
    }

    if (0 == ulMaxMailLen)
    {
        return 0;
    }

    if (ulTimeout == 0)
    {
        while(1)
        {
            iRecvLen = recv(stSock, szMsgBuf+iTotalRecvLen, ulMaxMailLen-iTotalRecvLen, 0);
            if (iRecvLen < 0)
            {
                iTotalRecvLen = -1;
                break;
            }
            else if (iRecvLen == 0)
            {
                break;
            }
            else
            {
                iTotalRecvLen += iRecvLen;
                break;
            }
        }

        return iTotalRecvLen;
    }

again:
    stTimeout.tv_sec = ulTimeout;
    FD_ZERO(&fds);
    FD_SET(stSock, &fds);

    iRet = select(stSock + 1, &fds, NULL, NULL, &stTimeout);
    if (iRet == 0)
    {
        return 0;
    }

    if (iRet < 0)
    {
        if (errno == EINTR)
        {
            goto again;
        }

        return -1;
    }

    if (FD_ISSET(stSock, &fds))
    {
        while(1)
        {
            iRecvLen = recv(stSock, szMsgBuf+iTotalRecvLen, ulMaxMailLen-iTotalRecvLen, 0);
            if (iRecvLen < 0)
            {
                iTotalRecvLen = -1;
                break;
            }
            else if (iRecvLen == 0)
            {
                if (iTotalRecvLen == 0)
                {
                    iTotalRecvLen = -1;
                }

                break;
            }
            else
            {
                iTotalRecvLen += iRecvLen;
                if (iTotalRecvLen >= 2)
                {
                    if (szMsgBuf[iTotalRecvLen-2] == '\r' &&
                        szMsgBuf[iTotalRecvLen-1] == '\n' )
                    {
                        break;
                    }
                }
            }
        }

        FD_CLR(stSock, &fds);

        return iTotalRecvLen;
    }
    else
    {
        FD_CLR(stSock, &fds);
        return -1;
    }
}

GMail::~GMail()
{
    CLOSE_SOCKET(m_socket);
    m_bConnect = FALSE;
}

INT32 CheckMailServer(GMail *pInst)
{
    INT32 iRet = MAIL_SUC;

    if(!pInst->Init())
    {
        iRet = MAIL_ERR_CONN;
        goto end;
    }

    iRet = pInst->Connect();

    if (MAIL_SUC != iRet)
    {
        goto end;
    }

    iRet = pInst->Check();

    if (MAIL_SUC != iRet)
    {
        goto end;
    }

    return iRet;

end:
    CLOSE_SOCKET(pInst->m_socket);
    pInst->m_bConnect = FALSE;

    return iRet;
}

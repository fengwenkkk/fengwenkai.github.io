#include "g_include.h"

GSerialPort::GSerialPort(BOOL bRawMode)
{
#ifdef _OSWIN32_
    m_hComm = INVALID_HANDLE_VALUE;
#else
    m_fd = -1;
#endif

    m_ulTimeout = 10000;

    m_bRawMode = bRawMode;
}

GSerialPort::~GSerialPort()
{
    Close();
}

VOID GSerialPort::Close()
{
#ifdef _OSWIN32_
    if (m_hComm == INVALID_HANDLE_VALUE)
    {
        return;
    }

    CloseHandle(m_hComm);
    m_hComm = INVALID_HANDLE_VALUE;
#else
    if (m_fd < 0)
    {
        return;
    }

    close(m_fd);
    m_fd = -1;
#endif
}

BOOL GSerialPort::Open(SERIAL_PORT_CFG_T *pstCfg)
{
    return Open(pstCfg->ucPortNo, pstCfg->acTTYName, pstCfg->ulBaudRate, pstCfg->ucDataBits, pstCfg->ucParity, pstCfg->ucStopBits, pstCfg->ulReadInterval, pstCfg->ulTotalTimeout);
}

BOOL GSerialPort::Open(UINT8 ucPortNo, CHAR *szTTYName, UINT32 ulBaudRate, UINT8 ucDataBits,
                      UINT8 ucParity, UINT8 ucStopBits, UINT32 ulReadInterval, UINT32 ulTotalTimeout)
{
#ifdef _OSWIN32_
    DCB                 stDCB = {0};
    COMMTIMEOUTS        stTimeout = {0};

    if (ucPortNo < 10)
    {
        sprintf(m_acCommName, "COM%d", ucPortNo);
    }
    else
    {
        sprintf(m_acCommName, "\\\\.\\COM%d", ucPortNo);
    }

    m_hComm = CreateFile(m_acCommName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, NULL );
    if (m_hComm == INVALID_HANDLE_VALUE )
    {
        GetLastError();
        return FALSE;
    }

    if (!SetupComm(m_hComm, sizeof(m_aucRecvMsgBuf), sizeof(m_aucRecvMsgBuf)))
    {
        goto fail;
    }

    stTimeout.ReadIntervalTimeout = ulReadInterval;
    stTimeout.ReadTotalTimeoutMultiplier = ulTotalTimeout;
    stTimeout.ReadTotalTimeoutConstant = ulTotalTimeout;
    stTimeout.WriteTotalTimeoutMultiplier = ulTotalTimeout;
    stTimeout.WriteTotalTimeoutConstant = ulTotalTimeout;

    if (!SetCommTimeouts(m_hComm, &stTimeout))
    {
        goto fail;
    }

    if (!SetCommMask(m_hComm, EV_RXFLAG | EV_RXCHAR))
    {
        goto fail;
    }

    if (!GetCommState(m_hComm, &stDCB))
    {
        goto fail;
    }

    stDCB.BaudRate = ulBaudRate;
    stDCB.ByteSize = ucDataBits;
    stDCB.Parity = ucParity;
    stDCB.StopBits = ucStopBits;
    stDCB.fDtrControl = DTR_CONTROL_DISABLE;
    stDCB.fRtsControl = RTS_CONTROL_DISABLE;

    if (!SetCommState(m_hComm, &stDCB))
    {
        goto fail;
    }

    PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

    return TRUE;

fail:
    Close();
    return FALSE;
#else

    CHAR    acPort[64];
    UINT32  i;
    UINT32  aulSpeed[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300};
    UINT32  aulBaudRate[] = {115200, 38400, 19200, 9600, 4800, 2400, 1200, 300};
    struct termios  stOptions = {0};

    sprintf(acPort, "%s%u", szTTYName, ucPortNo);

    m_fd = open(acPort, O_RDWR);

    if (-1 == m_fd)
    {
        printf("open port %s failed!\n", acPort);
        return FALSE;
    }

    //恢复串口为阻塞状态
    if (fcntl(m_fd, F_SETFL, 0) < 0)
    {
        printf("restore port %u to block mode failed!\n", ucPortNo);
        goto fail;
    }

    if (tcgetattr(m_fd, &stOptions) != 0)
    {
        perror("tcgetattr error!\n");
        goto fail;
    }

    //设置串口输入波特率和输出波特率
    for (i=0; i<ARRAY_SIZE(aulSpeed); i++)
    {
        if (ulBaudRate == aulBaudRate[i])
        {
            cfsetispeed(&stOptions, aulSpeed[i]);
            cfsetospeed(&stOptions, aulSpeed[i]);
        }
    }

    //设置数据流控制
    stOptions.c_cflag &= ~CRTSCTS;

#if 0
    switch(flow_ctrl)
    {
        case 0 ://不使用流控制
            stOptions.c_cflag &= ~CRTSCTS;
            break;

        case 1 ://使用硬件流控制
            stOptions.c_cflag |= CRTSCTS;
            break;

        case 2 ://使用软件流控制
            stOptions.c_cflag |= IXON | IXOFF | IXANY;
            break;
    }
#endif

    //设置数据位
    //屏蔽其他标志位
    stOptions.c_cflag &= ~CSIZE;

    switch (ucDataBits)
    {
        case 5:
            stOptions.c_cflag |= CS5;
            break;

        case 6:
            stOptions.c_cflag |= CS6;
            break;

        case 7:
            stOptions.c_cflag |= CS7;
            break;

        case 8:
            stOptions.c_cflag |= CS8;
            break;

        default:
            perror("set databits error!\n");
            goto fail;
    }

    //设置校验位  ucParity;       // 奇偶校验, 0-4=None,Odd,Even,Mark,Space
    switch (ucParity)
    {
       case SERIAL_PORT_PARITY_NONE:    //无奇偶校验位。
            stOptions.c_cflag &= ~PARENB;
            stOptions.c_iflag &= ~INPCK;
            break;

       case SERIAL_PORT_PARITY_ODD:     //设置为奇校验
            stOptions.c_cflag |= (PARODD | PARENB);
            stOptions.c_iflag |= INPCK;
            break;

       case SERIAL_PORT_PARITY_EVEN:    //设置为偶校验
            stOptions.c_cflag |= PARENB;
            stOptions.c_cflag &= ~PARODD;
            stOptions.c_iflag |= INPCK;
            break;

       case SERIAL_PORT_PARITY_SPACE:   //设置为空格
            stOptions.c_cflag &= ~PARENB;
            stOptions.c_cflag &= ~CSTOPB;
            break;

       default:
           perror("set parity error!\n");
           goto fail;
    }

    // 设置停止位
    switch (ucStopBits)
    {
        case SERIAL_PORT_STOP_BITS_1:
            stOptions.c_cflag &= ~CSTOPB;
            break;

        case SERIAL_PORT_STOP_BITS_2:
            stOptions.c_cflag |= CSTOPB;
            break;

        default:
            perror("set stopbits error!\n");
            goto fail;
    }

    //如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据但是不读
    tcflush(m_fd, TCIFLUSH);

    stOptions.c_iflag = 0;
    stOptions.c_oflag = 0;
    stOptions.c_lflag = 0;

    if (m_bRawMode)
    {
        stOptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /*Input*/
        stOptions.c_oflag &= ~OPOST;                          /*Output*/
    }

    //设置等待时间和最小接收字符
    stOptions.c_cc[VTIME] = 150; /* 读取一个字符等待1*(1/10)s */
    stOptions.c_cc[VMIN]  = 1;   /* 读取字符的最少个数为1 */

    //激活配置 (将修改后的termios数据设置到串口中）
    if (tcsetattr(m_fd, TCSANOW, &stOptions) != 0)
    {
        perror("com set error!\n");
        goto fail;
    }

    return TRUE;

fail:
    Close();
    return FALSE;
#endif
}

BOOL GSerialPort::Write(CHAR *szMsg)
{
    return Write(szMsg, strlen(szMsg));
}

BOOL GSerialPort::Write(VOID *pvMsg, UINT32 ulMsgLen)
{
#ifdef _OSWIN32_
    UINT32          ulWriteLen;
    OVERLAPPED      ov = {0};
    BOOL            bRet = FALSE;
    COMSTAT         stComStat;
    UINT32          ulErrorFlags;
    UINT32          ulErrCode;

    if (m_hComm == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    memset( &ov, 0, sizeof( ov ) );

    if (!WriteFile(m_hComm, pvMsg, ulMsgLen, (DWORD*)&ulWriteLen, &ov ) )
    {
        ulErrCode = GetLastError();
        if (ulErrCode != ERROR_IO_PENDING )
        {
            goto end;
        }

        GetOverlappedResult(m_hComm, &ov, (DWORD*)&ulWriteLen, TRUE);
    }

    if (ulWriteLen == ulMsgLen)
    {
        bRet = TRUE;
    }

end:
    if (ov.hEvent)
    {
        CloseHandle(ov.hEvent);
    }

    ClearCommError(m_hComm, (DWORD*)&ulErrorFlags, &stComStat);
    FlushFileBuffers(m_hComm);

    return bRet;
#else

    INT32 iSendLen = 0;
    INT32 iLeftLen = ulMsgLen;
    UINT8 *pucMsgBuf = NULL;

    if (m_fd < 0)
    {
        return FALSE;
    }

    pucMsgBuf = (UINT8 *)malloc(ulMsgLen);

    if(!pucMsgBuf)
    {
        return FALSE;
    }

    memcpy(pucMsgBuf, pvMsg, ulMsgLen);

    while(iLeftLen > 0)
    {
        iSendLen = write(m_fd, pucMsgBuf + iSendLen, iLeftLen);
        if(iSendLen < 0)
        {
            tcflush(m_fd, TCOFLUSH);
            free(pucMsgBuf);

            return FALSE;
        }
        else
        {
            iLeftLen -= iSendLen;
        }
    }

    free(pucMsgBuf);

    return TRUE;

#endif
}

BOOL GSerialPort::Read(CHAR *szMsg, UINT32 ulMaxMsgLen)
{
    UINT32 ulMsgLen = 0;

    return Read(szMsg, ulMaxMsgLen, &ulMsgLen, m_ulTimeout);
}

BOOL GSerialPort::Read(CHAR *szMsg, UINT32 ulMaxMsgLen, UINT32 ulTimeout)
{
    UINT32 ulMsgLen = 0;

    return Read(szMsg, ulMaxMsgLen, &ulMsgLen, ulTimeout);
}

BOOL GSerialPort::Read(VOID *pvMsg, UINT32 ulMaxMsgLen, UINT32 *pulMsgLen)
{
    return Read(pvMsg, ulMaxMsgLen, pulMsgLen, m_ulTimeout);
}

BOOL GSerialPort::Read(VOID *pvMsg, UINT32 ulMaxMsgLen, UINT32 *pulMsgLen, UINT32 ulTimeout)
{
#ifdef _OSWIN32_

    OVERLAPPED      ov = {0};
    BOOL            bRet = FALSE;
    COMSTAT         stComStat;
    UINT32          ulErrorFlags = 0;
    UINT32          ulErrCode;

    memset(pvMsg, 0, ulMaxMsgLen);
    if (m_hComm == INVALID_HANDLE_VALUE)
    {
        printf("GSerialPort read failed: invalid comm\n");
        return FALSE;
    }

    memset( &ov, 0, sizeof( ov ) );
    ov.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
    if (ov.hEvent == NULL)
    {
        printf("GSerialPort read failed when CreateEvent\n");
        goto end;
    }

    if (ulMaxMsgLen >= sizeof(m_aucRecvMsgBuf))
    {
        ulMaxMsgLen = sizeof(m_aucRecvMsgBuf)-1;
        m_aucRecvMsgBuf[sizeof(m_aucRecvMsgBuf)-1] = '\0';
    }

    try
    {
        if (!ReadFile(m_hComm, m_aucRecvMsgBuf, ulMaxMsgLen, (DWORD*)&m_ulRecvMsgLen, &ov ) )
        {
            ulErrCode = GetLastError();
            if (ulErrCode != ERROR_IO_PENDING)
            {
                printf("GSerialPort read failed, error code is %u\n", ulErrCode);
                goto end;
            }

            if (WAIT_OBJECT_0 != WaitForSingleObject(ov.hEvent, ulTimeout))
            {
                // 在某些情况下，需要注释goto end
                goto end;
            }

            GetOverlappedResult(m_hComm, &ov, (DWORD*)&m_ulRecvMsgLen, TRUE);
        }
    }
    catch(...)
    {
        printf("GSerialPort read exception\n");
        goto end;
    }

    memcpy(pvMsg, m_aucRecvMsgBuf, m_ulRecvMsgLen);
    *pulMsgLen = m_ulRecvMsgLen;
    bRet = TRUE;

end:
    if (ov.hEvent)
    {
        CloseHandle(ov.hEvent);
    }

    ClearCommError(m_hComm, (DWORD*)&ulErrorFlags, &stComStat);
    FlushFileBuffers(m_hComm);

    return bRet;
#else
    INT32   iRet;
    INT32   iRecvLen = 0;
    INT32   iTotalLen = 0;
    INT32   iReadingSize;
    UINT32  ulStartTime = gos_get_uptime_1ms();
    UINT32  ulCurrTime;
    fd_set  fsRead;

    struct timeval stTimeout = {0};

    if (m_fd < 0)
    {
        return FALSE;
    }

    stTimeout.tv_usec = 100*1000;  // 100 ms

    while(1)
    {
        ulCurrTime = gos_get_uptime_1ms();
        if ((UINT32)(ulCurrTime - ulStartTime) > ulTimeout)
        {
            break;
        }

        FD_ZERO(&fsRead);
        FD_SET(m_fd, &fsRead);

        iRet = select(m_fd+1, &fsRead, NULL, NULL, &stTimeout);
        if (iRet <= 0)
        {
            continue;
        }

        if (!FD_ISSET(m_fd, &fsRead))
        {
            continue;
        }

        iRecvLen = read(m_fd, m_aucRecvMsgBuf + iTotalLen, ulMaxMsgLen - iTotalLen);
        if (iRecvLen < 0)
        {
            return FALSE;
        }
        else
        {
            iTotalLen += iRecvLen;
            if (iTotalLen == ulMaxMsgLen)
            {
                break;
            }
        }
    }

    memcpy(pvMsg, m_aucRecvMsgBuf, iTotalLen);
    *pulMsgLen = iTotalLen;

    return TRUE;

#endif
}

BOOL GSerialPort::Execute(VOID *pvMsg, UINT32 ulMsgLen, BOOL bNoWait)
{
    return Execute(pvMsg, ulMsgLen, sizeof(m_aucRecvMsgBuf), bNoWait);
}

BOOL GSerialPort::Execute(VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulMaxRecvMsgLen, BOOL bNoWait)
{
    if (ulMaxRecvMsgLen > sizeof(m_aucRecvMsgBuf))
    {
        ulMaxRecvMsgLen = sizeof(m_aucRecvMsgBuf);
    }

#ifdef _OSWIN32_
    UINT32          ulWriteLen;
    OVERLAPPED      ov = {0};
    BOOL            bRet = FALSE;
    COMSTAT         stComStat;
    UINT32          ulErrorFlags;
    UINT32          ulErrCode;

    if (m_hComm == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    memset( &ov, 0, sizeof( ov ) );

    if (!WriteFile(m_hComm, pvMsg, ulMsgLen, (DWORD*)&ulWriteLen, &ov ) )
    {
        ulErrCode = GetLastError();
        if (ulErrCode != ERROR_IO_PENDING )
        {
            goto end;
        }

        GetOverlappedResult(m_hComm, &ov, (DWORD*)&ulWriteLen, TRUE);
    }

    if (ulWriteLen != ulMsgLen)
    {
        goto end;
    }

    if (!bNoWait)
    {
        memset( &ov, 0, sizeof( ov ) );
        ov.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
        if (ov.hEvent == NULL)
        {
            goto end;
        }

        try
        {
            if (!ReadFile(m_hComm, m_aucRecvMsgBuf, ulMaxRecvMsgLen, (DWORD*)&m_ulRecvMsgLen, &ov ) )
            {
                ulErrCode = GetLastError();
                if (ulErrCode != ERROR_IO_PENDING)
                {
                    goto end;
                }

                if (WAIT_OBJECT_0 != WaitForSingleObject(ov.hEvent, m_ulTimeout))
                {
                    if (m_ulTimeout <= READ_FILE_WAIT_TIME)
                    {
                        m_ulTimeout += 200;
                    }

                    goto end;
                }

                GetOverlappedResult(m_hComm, &ov, (DWORD*)&m_ulRecvMsgLen, TRUE);
            }
        }
        catch(...)
        {
            goto end;
        }
    }

    bRet = TRUE;

end:
    if (ov.hEvent)
    {
        CloseHandle(ov.hEvent);
    }

    ClearCommError(m_hComm, (DWORD*)&ulErrorFlags, &stComStat);
    FlushFileBuffers(m_hComm);

    return bRet;
#else

    if (!Write(pvMsg, ulMsgLen))
    {
        return FALSE;
    }

    if (!bNoWait)
    {
        gos_sleep(1);
        if (!Read(m_aucRecvMsgBuf, ulMaxRecvMsgLen, &m_ulRecvMsgLen))
        {
            return FALSE;
        }
    }

    return TRUE;
#endif
}

UINT32 GSerialPort::GetInQueSize()
{
#ifdef _OSWIN32_
    UINT32      ulErrorFlags = 0;
    COMSTAT     stComStat = {0};

    /* 在调用ReadFile和WriteFile之前,通过本函数清除以前遗留的错误标志 */
    if (!ClearCommError(m_hComm, (DWORD*)&ulErrorFlags, &stComStat))
    {
        return 0;
    }

    /* 获取在输入缓冲区中的字节数 */
    return stComStat.cbInQue;
#else
    return 1;
#endif
}

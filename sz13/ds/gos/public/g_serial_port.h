#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H


#define MAX_SERIAL_PORT_BUF_SIZE        (16*1024)
#define READ_FILE_WAIT_TIME             15000

#define SERIAL_PORT_STOP_BITS_1         0
#define SERIAL_PORT_STOP_BITS_1_5       1
#define SERIAL_PORT_STOP_BITS_2         2

#define SERIAL_PORT_PARITY_NONE         0
#define SERIAL_PORT_PARITY_ODD          1
#define SERIAL_PORT_PARITY_EVEN         2
#define SERIAL_PORT_PARITY_MARK         3
#define SERIAL_PORT_PARITY_SPACE        4

#define MAX_BAUD_RATE                   921600

#pragma pack(push, 4)
typedef struct
{
    UINT8       ucPortNo;       // >=1
    UINT8       ucResv1[3];

    CHAR        acTTYName[64];

    UINT32      ulBaudRate;     // 波特率

    UINT8       ucDataBits;     // 数据位，5~8
    UINT8       ucParity;       // 奇偶校验, 0-4=None,Odd,Even,Mark,Space
    UINT8       ucStopBits;     // 停止位, 0,1,2 = 1, 1.5, 2
    UINT8       ucResv2;

    UINT32      ulReadInterval;
    UINT32      ulTotalTimeout;
}SERIAL_PORT_CFG_T;
#pragma pack(pop)

class GSerialPort
{
public:
    GSerialPort(BOOL bRawMode=FALSE);
    ~GSerialPort();

    BOOL Open(SERIAL_PORT_CFG_T *pstCfg);
    BOOL Open(UINT8 ucPortNo,
                CHAR *szTTYName,
                UINT32 ulBaudRate,
                UINT8 ucDataBits = 8,
                UINT8 ucParity = SERIAL_PORT_PARITY_NONE,
                UINT8 ucStopBits = SERIAL_PORT_STOP_BITS_1,
                UINT32 ulReadInterval=100,
                UINT32 ulTotalTimeout=10000);

    VOID Close();
    BOOL Read(CHAR *szMsg, UINT32 ulMaxMsgLen);
    BOOL Read(CHAR *szMsg, UINT32 ulMaxMsgLen, UINT32 ulTimeout);
    BOOL Read(VOID *pvMsg, UINT32 ulMaxMsgLen, UINT32 *pulMsgLen);
    BOOL Read(VOID *pvMsg, UINT32 ulMaxMsgLen, UINT32 *pulMsgLen, UINT32 ulTimeout);

    BOOL Write(VOID *pvMsg, UINT32 ulMsgLen);
    BOOL Write(CHAR *szMsg);
    BOOL Execute(VOID *pvMsg, UINT32 ulMsgLen, BOOL bNoWait=FALSE);
    BOOL Execute(VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulMaxRecvMsgLen, BOOL bNoWait);

    UINT32 GetInQueSize();

    VOID*   GetRecvMsg(){return m_aucRecvMsgBuf;};
    UINT32  GetRecvMsgLen(){return m_ulRecvMsgLen;};

protected:
    BOOL                    m_bRawMode;
    CHAR                    m_acCommName[64];
    UINT8                   m_aucRecvMsgBuf[MAX_SERIAL_PORT_BUF_SIZE];
    UINT32                  m_ulRecvMsgLen;
    UINT32                  m_ulTimeout;

#ifdef _OSWIN32_
    HANDLE                  m_hComm;
#else
    INT32                   m_fd;
#endif
};

BOOL ParseSerialPortCfg(CHAR *szCfg, SERIAL_PORT_CFG_T *pstCfg);

#endif

//
// PING.C -- Ping program using ICMP and RAW Sockets
//

#include "g_include.h"

#ifdef _OSWIN32_
#pragma warning (disable : 4214)
#endif

#pragma pack(push)
#pragma pack(1)

#define ICMP_MIN        8
#define ICMP_ECHOREPLY  0
#define ICMP_ECHOREQ    8

#define MAX_TTL         64

#define DECODE_ERR_NOR  (-1)
#define DECODE_ERR_SEQ  (-2)

// IP Header -- RFC 791
typedef struct tagIPHDR
{
    u_char  VerHdrLen;
    u_char  TOS;            // Type Of Service
    short   TotLen;         // Total Length
    short   ID;             // Identification
    short   FlagOff;        // Flags and Fragment Offset
    u_char  TTL;            // Time To Live
    u_char  Protocol;       // Protocol
    u_short Checksum;       // Checksum
    struct  in_addr iaSrc;  // Internet Address - Source
    struct  in_addr iaDst;  // Internet Address - Destination
}IPHDR, *PIPHDR;

// ICMP Header - RFC 792
typedef struct tagICMPHDR
{
    u_char  Type;           // Type
    u_char  Code;           // Code
    u_short Checksum;       // Checksum
    u_short ID;             // Identification
    u_short Seq;            // Sequence
//  char    Data;           // Data
}ICMPHDR, *PICMPHDR;

#define REQ_DATASIZE 63*1024        // Echo Request Data size

// ICMP Echo Request
typedef struct tagECHOREQUEST
{
    ICMPHDR icmpHdr;
//  u_long  dwTime;
    char    cData[REQ_DATASIZE];
}ECHOREQUEST, *PECHOREQUEST;

// ICMP Echo Reply
typedef struct tagECHOREPLY
{
    IPHDR           ipHdr;
    ECHOREQUEST     echoRequest;
    char            cFiller[256];
}ECHOREPLY, *PECHOREPLY;

#pragma pack(pop)

//
// Mike Muuss' in_cksum() function
// and his comments from the original
// ping program
//
// * Author -
// *    Mike Muuss
// *    U. S. Army Ballistic Research Laboratory
// *    December, 1983

/*
 *          I N _ C K S U M
 *
 * Checksum routine for Internet Protocol family headers (C Version)
 *
 */
u_short in_cksum(u_short *addr, int len)
{
    register int nleft = len;
    register u_short *w = addr;
    register u_short answer;
    register int sum = 0;

    /*
     *  Our algorithm is simple, using a 32 bit accumulator (sum),
     *  we add sequential 16 bit words to it, and at the end, fold
     *  back all the carry bits from the top 16 bits into the lower
     *  16 bits.
     */
    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    /* mop up an odd byte, if necessary */
    if (nleft == 1)
    {
        u_short u = 0;

        *(u_char *)(&u) = *(u_char *)w ;
        sum += u;
    }

    /*
     * add back carry outs from top 16 bits to low 16 bits
     */
    sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
    sum += (sum >> 16);         /* add carry */
    answer = ~sum;              /* truncate to 16 bits */
    return (answer);
}

// SendEchoRequest()
// Fill in echo request header
// and send to destination
int SendEchoRequest(SOCKET s, SOCKADDR_IN *lpstToAddr, UINT32 ulSize, UINT16 usSeqID)
{
    UINT32          ulTotalSize = STRUCT_OFFSET(ECHOREQUEST, cData) + ulSize;
    ECHOREQUEST     *echoReq = (ECHOREQUEST*)malloc(ulTotalSize);
    int nRet;

    // Fill in echo request
    echoReq->icmpHdr.Type       = ICMP_ECHOREQ;
    echoReq->icmpHdr.Code       = 0;
    echoReq->icmpHdr.Checksum   = 0;
    echoReq->icmpHdr.ID         = htons(usSeqID);//nId++;
    echoReq->icmpHdr.Seq        = htons(usSeqID);//nSeq++;

    // Fill in some data to send
   //   for (nRet = 0; nRet < REQ_DATASIZE; nRet++)
    //  echoReq.cData[nRet] = ' '+nRet;
    memset(echoReq->cData, 0, ulSize);
    SET_INT(echoReq->cData, gos_get_current_time());

    // Save tick count when sent
    //echoReq->dwTime = gos_get_uptime_1ms();

    // Put data in packet and compute checksum
    echoReq->icmpHdr.Checksum = in_cksum((u_short *)echoReq, ulTotalSize);//sizeof(ECHOREQUEST));
//  echoReq->icmpHdr.Checksum = htons(echoReq->icmpHdr.Checksum);

    // Send the echo request
    nRet = sendto(s,                        /* socket */
                 (char*)echoReq,            /* buffer */
                 ulTotalSize, //sizeof(ECHOREQUEST),
                 0,                         /* flags */
                 (SOCKADDR*)lpstToAddr, /* destination */
                 sizeof(SOCKADDR_IN));   /* address length */

    free(echoReq);
    return (nRet);
}

int decode_resp(char *buf, int bytes,struct sockaddr_in *from, UINT16 usSeqID)
{
    IPHDR           *iphdr;
    ICMPHDR         *icmphdr;
    unsigned short  iphdrlen;

    iphdr = (IPHDR *)buf;
    iphdrlen = (iphdr->VerHdrLen & 0x0f) * 4 ; // number of 32-bit words *4 = bytes

    if (bytes < iphdrlen + ICMP_MIN)
    {
        return DECODE_ERR_NOR;
    }

    icmphdr = (ICMPHDR*)(buf + iphdrlen);
    if (icmphdr->Type != ICMP_ECHOREPLY)
    {
        return DECODE_ERR_NOR;
    }

    if (ntohs(icmphdr->ID) != usSeqID)
    {
        return DECODE_ERR_SEQ;
    }

    return 0;
}

// Receive incoming data
// and parse out fields
int RecvEchoReply(SOCKET s, SOCKADDR_IN *lpsaFrom, struct in_addr *pstRecvSrcAddr,UINT16 usSeqID)
{
    ECHOREPLY   *echoReply = (ECHOREPLY*)malloc(sizeof(ECHOREPLY));
    int nRet;
    socklen_t nAddrLen = sizeof(struct sockaddr_in);
    int iTTL;

    memset(echoReply, 0, sizeof(echoReply));
    // Receive the echo reply
    nRet = recvfrom(s,                  // socket
                    (char*)echoReply,   // buffer
                    sizeof(ECHOREPLY),  // size of buffer
                    0,                  // flags
                    (SOCKADDR*)lpsaFrom,// From address
                    &nAddrLen);         // pointer to address len

    if (nRet == SOCKET_ERROR)
    {
        free(echoReply);
        return DECODE_ERR_NOR;
    }

    nRet = decode_resp((char*)echoReply, nRet, (struct sockaddr_in *)lpsaFrom, usSeqID);
    if (nRet < 0)
    {
        free(echoReply);
        return nRet;
    }

    iTTL = echoReply->ipHdr.TTL;

    memcpy(pstRecvSrcAddr, &echoReply->ipHdr.iaDst, sizeof(echoReply->ipHdr.iaDst));

    free(echoReply);
    return iTTL;
}

// WaitForEchoReply()
// Use select() to determine when
// data is waiting to be read
int WaitForEchoReply(SOCKET s, long timeout)
{
    struct timeval stTimeout = {0};
    fd_set readfds;

    /*readfds.fd_count = 1;
    readfds.fd_array[0] = s;*/

    FD_ZERO(&readfds);
    FD_SET(s, &readfds);

    stTimeout.tv_sec = timeout/1000;
    stTimeout.tv_usec = 1000*(timeout % 1000);

//  return select(1, &readfds, NULL, NULL, &stTimeout);
    return select(s+1, &readfds, NULL, NULL, &stTimeout);
}

BOOL gos_ping(UINT8 *pucAddr, UINT32 ulSize, UINT32 ulTimeout, DOUBLE *pdDelay, UINT16 *pusSeq, UINT32 *pulTTL, UINT8 *pucLocalAddr)
{
    SOCKET          stSocket;
    INT32           iRet;
    SOCKADDR_IN     stSrcAddr;
    SOCKADDR_IN     stDstAddr;
    struct in_addr  stRecvSrcAddr;
    UINT64          u64Time;
    UINT32          ulTime;
    INT32           iTimeout;
    UINT16          usSeqID;
    static UINT32   ulSeqID = 0;
    static HANDLE   hMutex = NULL;

    if (ulSize < sizeof(UINT32))
    {
        ulSize = sizeof(UINT32);
    }

    *pdDelay = -1;
    *pulTTL = 0;

    gos_init_socket();
#ifdef _OSWIN32_
    gos_time_init();
#endif

    if (0 == ulSize)
    {
        ulSize = 32;
    }

    if (ulSize > REQ_DATASIZE)//sizeof(((ECHOREQUEST*)0)->cData))
    {
        ulSize = REQ_DATASIZE; //sizeof(((ECHOREQUEST*)0)->cData));
    }

    if (!hMutex)
    {
        hMutex = gos_mutex_init();
    }

    if (!gos_mutex(hMutex))
    {
        return FALSE;
    }

    usSeqID = ++ulSeqID;
    *pusSeq = usSeqID;

    gos_unmutex(hMutex);

    // Create a Raw socket
    stSocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (stSocket == INVALID_SOCKET)
    {
        printf("create raw socke failed:%s\n", gos_get_socket_err());
        /* @lijiancong:  这里好像需要释放这个closesocket */
        /* 2019-03-08 17:14:59 */
        return FALSE;//PING_ERROR;
    }

    // Setup destination socket address
    memset(&stDstAddr, 0, sizeof(stDstAddr));
    memcpy(&stDstAddr.sin_addr, pucAddr, 4);
    stDstAddr.sin_family = AF_INET;
    stDstAddr.sin_port = 0;

    // Send ICMP echo request
    u64Time = gos_get_uptime_1us();
    if (SendEchoRequest(stSocket, &stDstAddr, ulSize, usSeqID) < 0)
    {
        closesocket(stSocket);
        return FALSE;
    }

check_rsp:

    ulTime = (gos_get_uptime_1us() - u64Time)/1000;
    iTimeout = ulTimeout - ulTime;
    if (iTimeout < 0)
    {
        closesocket(stSocket);
        return FALSE;
    }

    // Use select() to wait for data to be received
    iRet = WaitForEchoReply(stSocket, iTimeout);
    if (iRet == SOCKET_ERROR)
    {
        closesocket(stSocket);
        return FALSE;
    }

    if (iRet == 0)
    {
        goto check_rsp;
    }

    iRet = RecvEchoReply(stSocket, &stSrcAddr, &stRecvSrcAddr, usSeqID);
    if (iRet == DECODE_ERR_SEQ)
    {
        goto check_rsp;
    }

    memcpy(pucLocalAddr, &stRecvSrcAddr, 4);

    if (iRet >= 0)
    {
        closesocket(stSocket);

        ulTime = (UINT32)(gos_get_uptime_1us() - u64Time);
        if (ulTime > 1000*ulTimeout)
        {
            return FALSE;
        }

        *pdDelay = ((double)ulTime)/1000;
        *pulTTL = iRet;

        return TRUE;
    }

    closesocket(stSocket);
    return FALSE;
}

BOOL gos_simple_ping(UINT8 *pucAddr, UINT32 ulTimeout)
{
    UINT32  ulSize = 32;
    DOUBLE  dDelay;
    UINT16  usSeq;
    UINT32  ulTTL;
    UINT8   aucLocalAddr[4];

    return gos_ping(pucAddr, ulSize, ulTimeout, &dDelay, &usSeq, &ulTTL, aucLocalAddr);
}

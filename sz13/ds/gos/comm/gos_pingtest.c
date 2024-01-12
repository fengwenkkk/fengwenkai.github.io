/* vi: set sw=4 ts=4: */
/*
 * $Id: ping.c,v 1.56 2004/03/15 08:28:48 andersen Exp $
 * Mini ping implementation for busybox
 *
 * Copyright (C) 1999 by Randolph Chung <tausq@debian.org>
 *
 * Adapted from the ping in netkit-base 0.10:
 * Copyright (c) 1989 The Regents of the University of California.
 * Derived from software contributed to Berkeley by Mike Muuss.
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/times.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <signal.h>

#include <net/if.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

//#include "common.h"

enum
{
    DEFDATALEN = 56,
    MAXIPLEN = 60,
    MAXICMPLEN = 76,
    MAXPACKET = 65468,
    MAX_DUP_CHK = (8 * 128),
    MAXWAIT = 2,
    PINGINTERVAL = 1        /* second */
};

#define O_QUIET         (1 << 0)

#define A(bit)      rcvd_tbl[(bit)>>3]      /* identify byte in array */
#define B(bit)      (1 << ((bit) & 0x07))   /* identify bit in byte */
#define SET(bit)    (A(bit) |= B(bit))
#define CLR(bit)    (A(bit) &= (~B(bit)))
#define TST(bit)    (A(bit) & B(bit))

static char *g_filename;

static void ping(const char *host);

/* common routines */
static int in_cksum(unsigned short *buf, int sz)
{
    int nleft = sz;
    int sum = 0;
    unsigned short *w = buf;
    unsigned short ans = 0;

    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1)
    {
        *(unsigned char *) (&ans) = *(unsigned char *) w;
        sum += ans;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    ans = ~sum;
    return (ans);
}

static int create_icmp_socket(void)
{
    struct protoent *proto;
    int sock;

    proto = getprotobyname("icmp");
    /* if getprotobyname failed, just silently force
     * proto->p_proto to have the correct value for "icmp" */
    if ((sock = socket(AF_INET, SOCK_RAW, (proto ? proto->p_proto : 1))) < 0)
    { /* 1 == ICMP */
            return -1;
    }

    return sock;
}

/* full(er) version */
static struct sockaddr_in pingaddr;
static int pingsock = -1;
static int datalen; /* intentionally uninitialized to work around gcc bug */

static long ntransmitted, nreceived = 0, nrepeats, pingcount;
static int myid, options;
static unsigned long tmin = ULONG_MAX, tmax, tsum;
static char rcvd_tbl[MAX_DUP_CHK / 8];

static struct hostent *hostent;

static void sendping(int);
static void pingstats(int);
static void unpack(char *, int, struct sockaddr_in *);

/**************************************************************************/
static void pingstats(int junk)
{
    int status;
    char cmdString[512];

    signal(SIGINT, SIG_IGN);

    if (nreceived != 0)
    {
        status = EXIT_SUCCESS;
        sprintf (cmdString, "touch %s", g_filename);
        gos_system (cmdString);
    }
    else
    {
        status = EXIT_FAILURE;
        sprintf (cmdString, "rm -f %s", g_filename);
        gos_system (cmdString);
    }

    exit(status);
}

static void sendping(int junk)
{
    struct icmp *pkt;
    int i;
    size_t pack_len = datalen + ICMP_MINLEN;
    char *packet;

    packet = malloc(pack_len);

    pkt = (struct icmp *) packet;

    pkt->icmp_type = ICMP_ECHO;
    pkt->icmp_code = 0;
    pkt->icmp_cksum = 0;
    pkt->icmp_seq = htons(ntransmitted++);
    pkt->icmp_id = myid;
    CLR(ntohs(pkt->icmp_seq) % MAX_DUP_CHK);

    gettimeofday((struct timeval *) &pkt->icmp_dun, NULL);
    pkt->icmp_cksum = in_cksum((unsigned short *) pkt, pack_len);

    i = sendto(pingsock, packet, pack_len, 0,
               (struct sockaddr *) &pingaddr, sizeof(struct sockaddr_in));

    if (i < 0)
    {
        free(packet);
        exit(-1);
    }
    else if ((size_t)i != pack_len)
    {
        free(packet);
        exit(-1);
    }

    signal(SIGALRM, sendping);
    if (pingcount == 0 || ntransmitted < pingcount)
    {   /* schedule next in 1s */
        alarm(PINGINTERVAL);
    }
    else
    {   /* done, wait for the last ping to come back */
        /* todo, don't necessarily need to wait so long... */
        signal(SIGALRM, pingstats);
        alarm(MAXWAIT);
    }
}

static char *icmp_type_name (int id)
{
    switch (id) {
    case ICMP_ECHOREPLY:        return "Echo Reply";
    case ICMP_DEST_UNREACH:     return "Destination Unreachable";
    case ICMP_SOURCE_QUENCH:    return "Source Quench";
    case ICMP_REDIRECT:         return "Redirect (change route)";
    case ICMP_ECHO:             return "Echo Request";
    case ICMP_TIME_EXCEEDED:    return "Time Exceeded";
    case ICMP_PARAMETERPROB:    return "Parameter Problem";
    case ICMP_TIMESTAMP:        return "Timestamp Request";
    case ICMP_TIMESTAMPREPLY:   return "Timestamp Reply";
    case ICMP_INFO_REQUEST:     return "Information Request";
    case ICMP_INFO_REPLY:       return "Information Reply";
    case ICMP_ADDRESS:          return "Address Mask Request";
    case ICMP_ADDRESSREPLY:     return "Address Mask Reply";
    default:                    return "unknown ICMP type";
    }
}

static void unpack(char *buf, int sz, struct sockaddr_in *from)
{
    struct icmp *icmppkt;
    struct iphdr *iphdr;
    struct timeval tv, *tp;
    int hlen, dupflag;
    unsigned long triptime;

    gettimeofday(&tv, NULL);

    /* check IP header */
    iphdr = (struct iphdr *) buf;
    hlen = iphdr->ihl << 2;
    /* discard if too short */
    if (sz < (datalen + ICMP_MINLEN))
        return;

    sz -= hlen;
    icmppkt = (struct icmp *) (buf + hlen);

    if (icmppkt->icmp_id != myid)
        return;             /* not our ping */

    if (icmppkt->icmp_type == ICMP_ECHOREPLY)
    {
        u_int16_t recv_seq = ntohs(icmppkt->icmp_seq);
        ++nreceived;
        tp = (struct timeval *) icmppkt->icmp_data;

        if ((tv.tv_usec -= tp->tv_usec) < 0)
        {
            --tv.tv_sec;
            tv.tv_usec += 1000000;
        }
        tv.tv_sec -= tp->tv_sec;

        triptime = tv.tv_sec * 10000 + (tv.tv_usec / 100);
        tsum += triptime;
        if (triptime < tmin)
            tmin = triptime;
        if (triptime > tmax)
            tmax = triptime;

        if (TST(recv_seq % MAX_DUP_CHK))
        {
            ++nrepeats;
            --nreceived;
            dupflag = 1;
        }
        else
        {
            SET(recv_seq % MAX_DUP_CHK);
            dupflag = 0;
        }

        if (options & O_QUIET)
            return;
    }
    else if (icmppkt->icmp_type != ICMP_ECHO)
            ;
    fflush(stdout);
}

int is_local_ip(unsigned char *pucIP)
{
    int fd, internum;
    struct ifconf ifc;
    struct sockaddr_in *pstSockAddr;
    struct  ifreq inter[16];
    unsigned char aaucInvalidIP[2][4] = {{0,0,0,0},{255,255,255,255}};
    unsigned char aucLocalIP[4] = {127,0,0,1};

    if (memcmp(pucIP, aucLocalIP, 4) == 0 ||
        memcmp(pucIP, aaucInvalidIP[0], 4) == 0 ||
        memcmp(pucIP, aaucInvalidIP[1], 4) == 0 )
    {
        return 1;
    }

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        return 0;
    }

    ifc.ifc_len = sizeof(inter);
    ifc.ifc_buf = (caddr_t) inter;

    /* »ñÈ¡¶Ë¿ÚºÅ*/
    if (0 == ioctl(fd, SIOCGIFCONF, (char *) &ifc))
    {
        internum = ifc.ifc_len / sizeof(struct ifreq);

        while (internum-- > 0)
        {
            if (0 == ioctl(fd, SIOCGIFADDR, (char *) &inter[internum]))
            {
                pstSockAddr = (struct sockaddr_in *) (&inter[internum].ifr_addr);
                if (memcmp(&pstSockAddr->sin_addr.s_addr, pucIP, 4) == 0)
                {
                    close(fd);
                    return 1;
                }
            }
        }
    }

    close(fd);
    return 0;
}

static void ping(const char *host)
{
    int loop;
    int pack_len = datalen + MAXIPLEN + MAXICMPLEN;
    char *packet;
    int sockopt;

    packet = malloc(pack_len);

    pingsock = create_icmp_socket();

    memset(&pingaddr, 0, sizeof(struct sockaddr_in));

    pingaddr.sin_family = AF_INET;
    hostent = gethostbyname(host);
    if (hostent->h_addrtype != AF_INET)
    {
        free(packet);
        exit(-1);
    }

    memcpy(&pingaddr.sin_addr, hostent->h_addr, sizeof(pingaddr.sin_addr));

    if (is_local_ip((unsigned char*)&pingaddr.sin_addr.s_addr))
    {
        nreceived = 1;

        free(packet);
        pingstats(0);

        exit(0);
    }

    /* enable broadcast pings */
    sockopt = 1;
    setsockopt(pingsock, SOL_SOCKET, SO_BROADCAST, (char *) &sockopt,
               sizeof(sockopt));

    /* set recv buf for broadcast pings */
    sockopt = 48 * 1024;
    setsockopt(pingsock, SOL_SOCKET, SO_RCVBUF, (char *) &sockopt,
               sizeof(sockopt));
    signal(SIGINT, pingstats);

    /* start the ping's going ... */
    sendping(0);

    /* listen for replies */
    for (loop=0; loop<pingcount; loop++)
    {
        struct sockaddr_in from;
        int fromlen = (int) sizeof(from);
        int c;

        if ((c = recvfrom(pingsock, packet, pack_len, 0,
                          (struct sockaddr *) &from, &fromlen)) < 0)
        {
            continue;
        }
        unpack(packet, c, &from);
        if (pingcount > 0 && nreceived >= pingcount)
            break;
    }
    pingstats(0);

    free(packet);
}

int main(int argc, char **argv)
{
    char *thisarg;

    datalen = DEFDATALEN; /* initialized here rather than in global scope to work around gcc bug */

    g_filename = argv[argc-1];

    argc--;
    argv++;
    options = 0;
    /* Parse any options */
    while (argc >= 1 && **argv == '-')
    {
        thisarg = *argv;
        thisarg++;
        switch (*thisarg)
        {
        case 'q':
            options |= O_QUIET;
            break;
        case 'c':
            if (--argc <= 0)
            {
                exit(-1);
            }
            argv++;
            pingcount = atoi(*argv);
            break;
        case 's':
            if (--argc <= 0)
            {
                exit(-1);
            }
            argv++;
            datalen = atoi(*argv);
            break;
        default:
            {
                exit(-1);
            }
        }
        argc--;
        argv++;
    }
    if (argc < 1)
    {
        exit(-1);
    }

    pingcount = 1;

    myid = 0x8888/*getpid() & 0xFFFF*/;

    ping(*argv);
    return EXIT_SUCCESS;
}


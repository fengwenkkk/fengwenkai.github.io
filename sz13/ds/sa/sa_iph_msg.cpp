#include "sa_public.h"
/**
 * @note
 * ���ӿ������ڽӿ��ĵ�\\10.1.1.154\00-����u��\������Ⱥ������Ŀ\����14\�ĵ�����\�����ڵ���14����LTEϵͳ�복���ӿ��ļ�V1.2��
 * �ر�ע��ýӿ����Ϻ�18�����нϴ�����
 */

BOOL        g_bNeedSendSelfCheckMsg = TRUE;
UINT32      g_ulTotalCheckPISCount = 0;
UINT32      g_ulRecBrdCfgNum = 0;
ID_VALUE_T  g_astRecBrdCfg[1024];

extern BOOL GetSelfCheckStatus(INT32 *piAppSelfCheckStatus);

extern UINT32  g_ulIPHRunStatus;
extern UINT32  g_ulIPHLinkcheckMaxNum;

// IPH����ʱ����Ҫ�������е�rtsp��ַ�Ͷ˿ڽ�������url�����û�ж˿ڣ�ȱʡ����554�������󽫳����+IPH�豸��� ��rtsp��������ַ���˿ڽ���һ��mapӳ�䣨�������ó����<<16 | IPHDevID����һ��UINT32�������������⣬��Ҫ�����͸�DIS�������е�VideoURL�滻�ɱ��ص�ַ�Ͷ˿ڣ�URLֻ�ظ������ַ����ݲ���
std::map<UINT32, RTSP_INFO_T>     g_mRTSPInfo;

VOID AddRTSPInfo(UINT8 ucCarriageID, UINT8 ucIPHDevID, RTSP_INFO_T &stRtspInfo)
{
    UINT32      ulKey = (((UINT32)ucCarriageID) << 16) | ucIPHDevID;

    GosLog(LOG_INFO, "add RTSP info: %u %u -> rtsp server "IP_FMT ":%u, local rtsp server" IP_FMT ":%u", 
            ucCarriageID, ucIPHDevID,
            IP_ARG(stRtspInfo.aucRTSPServerAddr), stRtspInfo.usRTSPServerPort,
            IP_ARG(stRtspInfo.aucLocalRTSPServerAddr), stRtspInfo.usLocalRTSPServerPort);
  
    g_mRTSPInfo[ulKey] = stRtspInfo;
}

BOOL GetRTSPInfo(UINT8 ucCarriageID, UINT8 ucIPHDevID, RTSP_INFO_T &stRtspInfo)
{
    UINT32      ulKey = (((UINT32)ucCarriageID) << 16) | ucIPHDevID;

    if (g_mRTSPInfo.find(ulKey) == g_mRTSPInfo.end())
    {
        return FALSE;
    }

    memcpy(&stRtspInfo, &g_mRTSPInfo[ulKey], sizeof(stRtspInfo));

    return TRUE;
}

/*
BOOL IsSameRTPPort(UINT16 usPort1, UINT16 usPort2)
{
    if (usPort1 == 0)
    {
        usPort1 = DEFAULT_RTP_PORT;
    }

    if (usPort2 == 0)
    {
        usPort2 = DEFAULT_RTP_PORT;
    }

    return usPort1 == usPort2;
}

BOOL ConvertRtspServerAddr(UINT8 *pucLocalRTSPServerAddr, UINT16 usLocalRTSPServerPort, UINT8 *pucRTSPServerAddr, UINT16 &usRTSPServerPort)
{
    std::map<UINT32, IP_ADDR_T>::iterator it = g_mRTSPInfo.begin();

    for (; it!=g_mRTSPInfo.end(); it++)
    {
        if (memcpy(it->second.aucIP, pucLocalRTSPServerAddr, 4) == 0 &&
            IsSameRTPPort(it->second.usPort, usLocalRTSPServerPort) )
        {
            //memcpy(pucRTSPServerAddr, it->first.
        }
    }

    //TODO
    SET_IP(pucRTSPServerAddr, 192, 0, 1, 133);
    usRTSPServerPort = DEFAULT_RTP_PORT;

    return TRUE;
}
*/

BOOL SendPISMsg(UINT16 usMsgType, VOID *pvMsg, UINT16 usMsgLen)
{
    if (g_stPISTCPClientSock == INVALID_SOCKET)
    {
        GosLog(LOG_ERROR, "send PIS msg failed for invalid socket");
        return FALSE;
    }

    PIS_MSG_HDR_T   *pstMsg = (PIS_MSG_HDR_T*)calloc(1, sizeof(PIS_MSG_HDR_T)+usMsgLen);

    if (!pstMsg)
    {
        GosLog(LOG_ERROR, "malloc failed when sending PIS msg");
        return FALSE;
    }

    pstMsg->usMsgMagic = htons(PIS_MSG_MAGIC);
    pstMsg->usMsgType = htons(usMsgType);
    pstMsg->usMsgLen = htons(usMsgLen);

    if (pvMsg && usMsgLen)
    {
        memcpy(pstMsg+1, pvMsg, usMsgLen);
    }

    if (!gos_tcp_send(g_stPISTCPClientSock, pstMsg, sizeof(PIS_MSG_HDR_T) + usMsgLen))
    {
        GosLog(LOG_ERROR, "send PIS msg(%u) failed", usMsgType);
        InitPISSocket();
        free(pstMsg);

        return FALSE;
    }

    free(pstMsg);
    //g_ulLastPISMsgTime = gos_get_uptime();

    return TRUE;
}

VOID SendPISHeartBeatMsg()
{
    UINT32          ulCurrTime = gos_get_uptime_1ms();
    static UINT32   ulLastPISMsgTime = 0;

    if (ulLastPISMsgTime == 0 ||
        ulCurrTime >= (ulLastPISMsgTime+g_stSACfg.ulPISHeartBeatTime*1000))
    {
        SendPISMsg(EV_PIS_HEART_BEAT);
        ulLastPISMsgTime = ulCurrTime;

        GosLog(LOG_DETAIL, "send pis heartbeat end");
    }
}

VOID SendPISSelfCheckMsg()
{
    static UINT32   ulSendTime = 0;
    INT32           iAppSelfCheckStatus;
    UINT32          ulTime = gos_get_uptime() - ulSendTime;
    UINT32          ulSendPeriod = 3;
    
    if (ulTime >= ulSendPeriod)
    {
        PIS_SELFCHECK_STATUS_REQ_T  stMsg = {0};

        if (!GetSelfCheckStatus(&iAppSelfCheckStatus))
        {
            return;
        }

        if (iAppSelfCheckStatus == APP_SELF_CHECK_STATUS_UNKNOWN)
        {
            GosLog(LOG_DETAIL, "app self check status is unknown");
            return ;
        }

        stMsg.ucCheckStatus = iAppSelfCheckStatus;
        SendPISMsg(EV_PIS_SELFCHECK_STATUS, &stMsg, sizeof(stMsg));

        ulSendTime = gos_get_uptime();
        GosLog(LOG_DETAIL, "send  app self check status to PIS end, app self check status is %u", stMsg.ucCheckStatus);
    }

    return;
}

VOID SendPISGetTrainInfoMsg()
{
    static UINT32   ulLastSendTime = 0;
    UINT32      ulCurrTime = gos_get_uptime();
    GJsonParam  Param;

    if (ulLastSendTime > 0 && ulCurrTime < (5+ulLastSendTime))
    {
        return;
    }

    GosLog(LOG_DETAIL, "send pis get train info msg");
    SendPISMsg(EV_PIS_GET_TRAIN_INFO);

    ulLastSendTime = gos_get_uptime();
    if (ulLastSendTime == 0)
    {
        ulLastSendTime = 1;
    }
}

VOID SendPISRecBrdCfgMsg()
{
    SendPISMsg(EV_PA_REC_CFG);
}

/**
 * @brief           ��IPH���������ͻ�ȡ��ȡԤ¼���㲥��������
 * @return          VOID 
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par ʾ��:
 * @code
 *  
 * @endcode
 * @note
 * �����ĵ�ѡ��
 */
VOID CheckPISRecBrdCfg()
{
    static UINT32   ulLastSendTime = 0;
    UINT32      ulCurrTime = gos_get_uptime();

    if (g_ulTotalCheckPISCount >= 20)
    {
        return;
    }

    if (ulLastSendTime == 0 ||
        ulCurrTime >= (ulLastSendTime+15))
    {
        SendPISRecBrdCfgMsg();
        GosLog(LOG_INFO, "send get pis rec brd cfg msg");

        ulLastSendTime = gos_get_uptime();
        if (ulLastSendTime == 0)
        {
            ulLastSendTime = 1;
        }

        g_ulTotalCheckPISCount++;
    }
}

/**
 * @brief           ����PIS���͵��г���ϢӦ��
 * @param           pvMsg       [in] �յ���Ӧ��
 * @param           ulMsgLen    [in] ��Ϣ�峤��
 * @return          VOID 
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par ʾ��:
 * @code
 *  
 * @endcode
 * @note
 * ���г��ļ�ʻģʽ�͸�״̬���õ�UCI
 * ��״̬��1���� 0���ر�
 * ��ʻģʽ��1�����˼�ʻ 0�����˼�ʻ
 */
/*
static VOID OnPISGetTrainInfoRsp(VOID *pvMsg, UINT32 ulMsgLen)
{
    static UINT8                ucCoverStatus = 0xff;
    static UINT8                ucDriverMode = 0xff;
    PIS_GET_TRAIN_INFO_RSP_T    *pstRsp = (PIS_GET_TRAIN_INFO_RSP_T*)pvMsg;

    GosLog(LOG_DETAIL, "recv PIS_GET_TRAIN_INFO msg");
    if (ulMsgLen != sizeof(PIS_GET_TRAIN_INFO_RSP_T))
    {
        GosLog(LOG_ERROR, "invalid msg len(%u) of PIS_GET_TRAIN_INFO", ulMsgLen);
        return;
    }

    // �洢�豸״̬
    if (pstRsp->ucCoverStatus != ucCoverStatus ||
        pstRsp->ucDriverMode  != ucDriverMode )
    {
        ucCoverStatus = pstRsp->ucCoverStatus;
        ucDriverMode  = pstRsp->ucDriverMode;

        GosLog(LOG_INFO, "cover status is %u, driver mode is %u", ucCoverStatus, ucDriverMode);

        CfgSet(TRAIN_COVER_STATUS, ucCoverStatus);
        CfgSet(TRAIN_DRIVE_MODE, ucDriverMode);
//        CfgCommit();
    }
}
*/ 
/*
static VOID OnPISReportSelfCheckStatusRsp(VOID *pvMsg, UINT32 ulMsgLen)
{
    GosLog(LOG_DETAIL, "recv PIS_SELFCHECK_STATUS msg");

    g_bNeedSendSelfCheckMsg = FALSE;
}
*/

/**
 * @brief           ����IPH�������豸״̬��ϢӦ��
 * @param           pvMsg       [in] �յ���Ӧ����Ϣ
 * @param           ulMsgLen    [in] ��Ϣ�峤��
 * @return          VOID 
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par ʾ��:
 * @code
 *  
 * @endcode
 * @note
 * ������Ϣ��IPH״̬��֯��JSON���͸�DIS
 * IPHϵͳ״̬��0������������ֵ���쳣
 */
static VOID OnPISHeartBeat(VOID *pvMsg, UINT32 ulMsgLen)
{
    IPH_GET_SYS_STATUS_RSP_T    *pstRsp = (IPH_GET_SYS_STATUS_RSP_T*)pvMsg;

    if (ulMsgLen != sizeof(IPH_GET_SYS_STATUS_RSP_T))
    {
        GosLog(LOG_ERROR, "invalid msg len(%u) of IPH heart_beat msgbody", ulMsgLen);
        return ;
    }

    g_ulIPHRunStatus = pstRsp->ucStatus;

    ram_set(TRAIN_IPH_RUNNING_STATUS, g_ulIPHRunStatus);

    g_ulIPHLinkcheckMaxNum = 0;

    GosLog(LOG_DETAIL, "recv IPH heartbeat msg!");
}

// rtsp://admin;admin@10.194.128.68:2203/h264/ch1/av_stream
BOOL GetRtspAddrFromUrl(CHAR *szUrl, UINT8 *pucAddr, UINT16 *pusPort)
{
    CHAR        acVideoURL[32];
    CHAR        *szStart;
    CHAR        *szEnd;
    CHAR        *szTmp;
    UINT32      ulUrlLen;
    
    szStart = strstr(szUrl, "rtsp://");
    if (!szStart)
    {
        return FALSE;
    }

    szStart += strlen("rtsp://");
    szEnd = strchr(szStart, '/');
    if (!szEnd)
    {
        return FALSE;
    }

    szTmp = strchr(szStart, '@');
    if (szTmp)
    {
        szStart = szTmp+1;
    }

    ulUrlLen = szEnd - szStart;
    if (ulUrlLen >= sizeof(acVideoURL))
    {
        return FALSE;
    }

    memset(acVideoURL, 0, sizeof(acVideoURL));
    memcpy(acVideoURL, szStart, ulUrlLen);

    return gos_get_ip_port(acVideoURL, pucAddr, pusPort);
}

// ��ԭʼ��url�滻�ɳ���̨���ص�ַ�Ͷ˿ڵ�url (rtsp://192.168.90.223:55554/h264/ch1/sub/av_stream)
BOOL ReplaceRtspVideoUrl(CHAR *szOriVideoUrl, UINT8 *pucLocalRTSPServerAddr, UINT16 usLocalRTSPServerPort, CHAR *szVideoUrl, UINT32 ulMaxLen)
{
    CHAR    *szTmp = strstr(szOriVideoUrl, "rtsp://");

    if (!szTmp)
    {
        return FALSE;
    }

    szTmp += strlen("rtsp://");
    szTmp = strchr(szTmp, '/');
    if (!szTmp)
    {
        return FALSE;
    }

    UINT32 ulLen;

    if (usLocalRTSPServerPort == DEFAULT_RTP_PORT ||
        usLocalRTSPServerPort == 0)
    {
        ulLen = sprintf(szVideoUrl, "rtsp://" IP_FMT, IP_ARG(pucLocalRTSPServerAddr));
    }
    else
    {
        ulLen = sprintf(szVideoUrl, "rtsp://" IP_FMT":%u", IP_ARG(pucLocalRTSPServerAddr), usLocalRTSPServerPort);
    }

    if ((ulLen+strlen(szTmp)) >= ulMaxLen)
    {
        return FALSE;
    }

    strcpy(szVideoUrl+ulLen, szTmp);
    return TRUE;
}

/**
 * @brief           ����IPH�Խ�����
 * @param           pvMsg      [in]
 * @param           ulMsgLen   [in]
 * @return          VOID 
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par ʾ��:
 * @code
 *  
 * @endcode
 * @note
 */
static VOID OnIPHTalkApplyReq(VOID *pvMsg, UINT32 ulMsgLen)
{
    IPH_TALK_APPLY_REQ_T    *pstReq = (IPH_TALK_APPLY_REQ_T*)pvMsg;

    GosLog(LOG_INFO, "recv IPH_TALK_APPLY_REQ msg");
    if (ulMsgLen < STRUCT_OFFSET(IPH_TALK_APPLY_REQ_T, astApplyReq))
    {
        GosLog(LOG_ERROR, "invalid msg len(%u) of IPH_TALK_APPLY_REQ", ulMsgLen);
        return;
    }

    if (ulMsgLen != (STRUCT_OFFSET(IPH_TALK_APPLY_REQ_T, astApplyReq) + pstReq->ucNum*sizeof(pstReq->astApplyReq[0])))
    {
        GosLog(LOG_ERROR, "invalid msg len(%u) of IPH_TALK_APPLY_REQ", ulMsgLen);
        return;
    }

    if (0 == pstReq->ucNum)
    {
        GosLog(LOG_ERROR, "IPH req number is 0");
        return;
    }

    if (!g_stSACfg.bIPHVideoProxyMode)
    {
        // ������Ϣ��DIS
        SendSAApplyIPHTalkReqMsg(pstReq);
        return;
    }

    RTSP_INFO_T     stRTSPInfo;
    CHAR            acVideoUrl[96];
    GJsonTupleParam Param;

    for (UINT32 i=0; i<pstReq->ucNum; i++)
    {
        GosLog(LOG_INFO, "get IPH talk request %u: %u %u %u %s", i+1,
            ntohs(pstReq->astApplyReq[i].usTrainUnitID),
            pstReq->astApplyReq[i].ucCarriageID,
            pstReq->astApplyReq[i].ucIPHDevID,
            pstReq->astApplyReq[i].acVideoUrl);

        memset(&stRTSPInfo, 0, sizeof(stRTSPInfo));
        stRTSPInfo.stPreAllocedSocket = INVALID_SOCKET;

        if (!GetRtspAddrFromUrl(pstReq->astApplyReq[i].acVideoUrl, stRTSPInfo.aucRTSPServerAddr, &stRTSPInfo.usRTSPServerPort))
        {
            GosLog(LOG_ERROR, "parse IPH video url failed: %s", pstReq->astApplyReq[i].acVideoUrl);
            continue;
        }

        if (!AllocLocalRTSPServerAddr(stRTSPInfo.aucLocalRTSPServerAddr, stRTSPInfo.usLocalRTSPServerPort, stRTSPInfo.stPreAllocedSocket))
        {
            GosLog(LOG_ERROR, "alloc local rtsp addr failed");
            continue;
        }

        stRTSPInfo.ulApplyUptime = gos_get_uptime();
        stRTSPInfo.ulRTSPStatus = RTSP_STATUS_PRE_ALLOC;

        if (!ReplaceRtspVideoUrl(pstReq->astApplyReq[i].acVideoUrl, stRTSPInfo.aucLocalRTSPServerAddr, 
                                 stRTSPInfo.usLocalRTSPServerPort, acVideoUrl, sizeof(acVideoUrl)-1))
        {
            GosLog(LOG_ERROR, "replace iph url of failed");
            continue;
        }
        
        AddRTSPInfo(pstReq->astApplyReq[i].ucCarriageID, pstReq->astApplyReq[i].ucIPHDevID, stRTSPInfo);

        GJsonParam      SubParam;

        pstReq->astApplyReq[i].usTrainUnitID = ntohs(pstReq->astApplyReq[i].usTrainUnitID);

        SubParam.Add("TrainUnitID", pstReq->astApplyReq[i].usTrainUnitID);
        SubParam.Add("SAID", g_acSAID);
        SubParam.Add("CarriageID", pstReq->astApplyReq[i].ucCarriageID);
        SubParam.Add("IPHDevID", pstReq->astApplyReq[i].ucIPHDevID);
        SubParam.AddIP("IPHDevIP", g_stSACfg.stPISAddr.aucIP);
        SubParam.Add("VideoURL", acVideoUrl);
        
        Param.Add(SubParam);
    }

    SendDISMsg(MSG_SAApplyIPHTalk, 0, Param);
}

static VOID OnIPHTalkStopReq(VOID *pvMsg, UINT32 ulMsgLen)
{
    IPH_TALK_STOP_REQ_T *pstReq = (IPH_TALK_STOP_REQ_T*)pvMsg;

    GosLog(LOG_DETAIL, "recv IPH_TALK_STOP_REQ msg");
    if (ulMsgLen != sizeof(IPH_TALK_STOP_REQ_T))
    {
        GosLog(LOG_ERROR, "invalid msg len(%u) of IPH_TALK_STOP_REQ", ulMsgLen);
        return;
    }

    NTOHS_IT(pstReq->usTrainUnitID);

    if (g_stSACfg.bIPHVideoProxyMode)
    {
        StopRTSP(pstReq->ucCarriageID, pstReq->ucIPHDevID);
    }

    // ������Ϣ��DIS
    SendSAStopIPHTalkReqMsg(pstReq);
}

VOID OnPISMsg(PIS_MSG_HDR_T *pstMsg)
{
    VOID    *pvMsgBody = (VOID*)(pstMsg+1);
    UINT32  ulMsgType = ntohs(pstMsg->usMsgType);
    UINT32  ulMsgLen = ntohs(pstMsg->usMsgLen);

//    g_ulLastPISMsgTime = gos_get_uptime_1ms();

    if (ulMsgType == EV_PIS_HEART_BEAT)
    {
        OnPISHeartBeat(pvMsgBody, ulMsgLen);
    }
    else if (ulMsgType == EV_IPH_TALK_APPLY)
    {
        OnIPHTalkApplyReq(pvMsgBody, ulMsgLen);
    }
    else if (ulMsgType == EV_IPH_STOP_TALK)
    {
        OnIPHTalkStopReq(pvMsgBody, ulMsgLen);
    }
    else
    {
        GosLog(LOG_ERROR, "invalid PIS msg type %04X", ulMsgType);
    }
}
#include "sa_public.h"

BOOL    g_bGetDISCfg = FALSE;

static UINT32   g_ulLastDISMsgTime = 0;
static UINT32   g_ulLastHeartBeatMsgTime = 0;
static UINT32   g_ulLastCfgMsgTime = 0;
static UINT32   g_ulLastNTPCfgMsgTime = 0;
static CHAR     g_acNTPServerAddr[32] = {0};
static UINT32   g_ulNTPServerPort = 0;
static UINT32   g_ulNTPSyncPeriod = 0;
//static UINT32   g_ulResetButtonCheckPeriod = 100;
static UINT32   g_ulResetButtonCheckTime = 10;
static UINT32   g_ulResetButtonStatusHighNum = 0;

static UINT32   g_ulResetAPPCheckTime = 0xffffffff;
static UINT32   g_ulLinkCheckAPPFailNum = 0;

static UINT32   g_ulDCUserID = 0;  //保存发起广播的调度台ID

extern SOCKET   g_stIPHUDPClientSock;

extern UINT32  g_ulIPHRunStatus;
extern UINT32  g_ulPAMasterStatus;
extern UINT32  g_ulPARunStatus;

extern BOOL ConvertRtspServerAddr(UINT8 *pucLocalRTSPServerAddr, UINT16 usLocalRTSPServerPort, UINT8 *pucRTSPServerAddr, UINT16 &usRTSPServerPort);
extern BOOL GetRTSPInfo(UINT8 ucCarriageID, UINT8 ucIPHDevID, RTSP_INFO_T &stRtspInfo);

BOOL GetResetButtonStatus()
{
#ifdef _OSWIN32_
    return TRUE;
#else
    CHAR      *szCmd = (CHAR*)"cat /sys/class/gpio/gpio3/value";
    FILE    *fp = popen(szCmd, "r");
    CHAR    acBuf[32];
    CHAR    *szResult;
    BOOL    bResult = TRUE;    // gpio为high时返回FALSE

    if (!fp)
    {
        return TRUE;
    }

    memset(acBuf, 0, sizeof(acBuf));
    if (fgets(acBuf, sizeof(acBuf), fp) == 0)
    {
        goto end;
    }

    if (acBuf[sizeof(acBuf)-1] != 0)
    {
        GosLog(LOG_INFO, "get reset_button_status failed!");
        goto end;
    }

    szResult = gos_trim_string(acBuf);
    GosLog(LOG_DETAIL, "GPIO 3 is %s", szResult);
    if (strcmp(szResult, "1") == 0)
    {
        bResult = FALSE;
    }

end:
    pclose(fp);

    return bResult;
#endif
}

VOID ResetTX()
{
    /*
    FILE    *fp = fopen("/tmp/reset_tx.sh", "w+t");

    if (fp)
    {
        fprintf(fp, "#!/bin/sh\n");
        fprintf(fp, "gpio func 2 0\n");
        fprintf(fp, "gpio status 2 low\n");
        fprintf(fp, "gpio status 12 low\n");
        fprintf(fp, "sleep 2\n");
        fprintf(fp, "gpio status 2 high\n");
        fprintf(fp, "gpio status 12 high\n");
        fclose(fp);

        gos_set_file_executable("/tmp/reset_tx.sh");
        gos_system("/tmp/reset_tx.sh &");
    }
    else
    {
        gos_system((CHAR*)"gpio func 2 0");
        gos_system((CHAR*)"gpio status 2 low");
        gos_system((CHAR*)"gpio status 12 low");
        gos_sleep(2);
        gos_system((CHAR*)"gpio status 2 high");
        gos_system((CHAR*)"gpio status 12 high");
    } */

    GosLog(LOG_FATAL, "reboot");

    gos_system((CHAR*)"echo 0 > /sys/class/gpio/gpio49/value");
    gos_reboot();
}

VOID CheckResetButton()
{
    static UINT32       ulLastCheckTime = 0;
    UINT32              ulCurrTime = gos_get_uptime_1ms();

    if (g_ulResetButtonCheckTime == 0)
    {
        return;
    }

    if (ulLastCheckTime > 0 && ulCurrTime < (ulLastCheckTime+1000))
    {
        return;
    }

    if (ulLastCheckTime == 0)
    {
        ulLastCheckTime = ulCurrTime;
    }
    else
    {
        ulLastCheckTime += 1000;
    }

    if (GetResetButtonStatus())
    {
        GosLog(LOG_DETAIL, "reset button status is low");

        g_ulResetButtonStatusHighNum = 0;
        return;
    }

    GosLog(LOG_FATAL, "reset button status is high");

    g_ulResetButtonStatusHighNum++;
    if (g_ulResetButtonStatusHighNum <= g_ulResetButtonCheckTime)
    {
        return;
    }

    g_ulResetButtonStatusHighNum = 0;

    ResetTX();
}

BOOL PingTX()
{
    UINT8   *pucAddr = g_stSACfg.stAPPAddr.aucIP;
    UINT32  ulTimeout = 300;

    return gos_simple_ping(pucAddr, ulTimeout);
}

void ResetButtonTest()
{
    for (UINT32 i=0; ;i++)
    {
        gos_sleep_1ms(10);

        if (GetResetButtonStatus())
        {
            GosLog(LOG_INFO, ",%u, low", i);
        }
        else
        {
            GosLog(LOG_INFO, ",%u, high", i);
        }
    }
}

VOID CheckAPPLink()
{
    if (g_ulResetAPPCheckTime == 0)
    {
        return;
    }

    if (PingTX())
    {
        g_ulLinkCheckAPPFailNum = 0;
        return;
    }

    g_ulLinkCheckAPPFailNum++;
    if (g_ulLinkCheckAPPFailNum >= 30)
    {
        GosLog(LOG_ERROR, "ping tx failed");
    }

    if (g_ulLinkCheckAPPFailNum < g_ulResetAPPCheckTime)
    {
        return;
    }

    g_ulLinkCheckAPPFailNum = 0;

    ResetTX();
}

VOID GetSACfg()
{
    CfgGet(OMA_NTPCFG_NTPADDR, g_acNTPServerAddr, sizeof(g_acNTPServerAddr));
    CfgGet(OMA_NTPCFG_NTPPORT, &g_ulNTPServerPort);
    if (!CfgGet(OMA_NTPCFG_SYNCPERIOD, &g_ulNTPSyncPeriod))
    {
        g_ulNTPSyncPeriod = 60;
        GosLog(LOG_INFO, "uci get ntp_sync_period failed, now default period is 60");
    }

    GosLog(LOG_INFO, "load NTP cfg: %s:%u %u", g_acNTPServerAddr, g_ulNTPServerPort, g_ulNTPSyncPeriod);
}

BOOL dtp_msg_ntoh(DTP_MSG_HDR_T *pstMsgHdr)
{
    if (!pstMsgHdr)
    {
        return FALSE;
    }

    NTOHL_IT(pstMsgHdr->ulMsgMagic);
    NTOHS_IT(pstMsgHdr->usMsgVer);
    NTOHS_IT(pstMsgHdr->usSenderModule);
    NTOHS_IT(pstMsgHdr->usReceiverModule);
    NTOHL_IT(pstMsgHdr->ulMsgLen);
    NTOHL_IT(pstMsgHdr->ulSeqID);
    NTOHS_IT(pstMsgHdr->usMsgType);
    //NTOHS_IT(pstMsgHdr->usCompType);
    //NTOHS_IT(pstMsgHdr->usEncryptType);
    NTOHS_IT(pstMsgHdr->usTimeout);
    NTOHS_IT(pstMsgHdr->usCRC);

    return TRUE;
}

BOOL dtp_msg_hton(DTP_MSG_HDR_T *pstMsgHdr)
{
    if (!pstMsgHdr)
    {
        return FALSE;
    }

    HTONL_IT(pstMsgHdr->ulMsgMagic);
    HTONS_IT(pstMsgHdr->usMsgVer);
    HTONS_IT(pstMsgHdr->usSenderModule);
    HTONS_IT(pstMsgHdr->usReceiverModule);
    HTONL_IT(pstMsgHdr->ulMsgLen);
    HTONL_IT(pstMsgHdr->ulSeqID);
    HTONS_IT(pstMsgHdr->usMsgType);
    //HTONS_IT(pstMsgHdr->usCompType);
    //HTONS_IT(pstMsgHdr->usEncryptType);
    HTONS_IT(pstMsgHdr->usTimeout);
    HTONS_IT(pstMsgHdr->usCRC);

    return TRUE;
}

VOID OnDCAcceptIPHTalkRsp(CHAR *szMsgInfo, GJson &Json)
{
    IPH_TALK_APPLY_RSP_T    *pstRsp = (IPH_TALK_APPLY_RSP_T*)calloc(1, sizeof(IPH_TALK_APPLY_RSP_T));
    std::vector<GJson*>     &SubJson = Json.GetSubJson();
    IPH_TALK_APPLY_RSP_INFO_T   *pstApplyInfo = &pstRsp->astApplyRsp[0];
    UINT32                      ulMsgLen;

    if (SubJson.size() == 0)
    {
        GosLog(LOG_ERROR, "no valid IPH rsp info: %s", szMsgInfo);
        goto error;
    }

    for (UINT32 i=0; i<SubJson.size(); i++)
    {
        GJson   *pSubJson = SubJson[i];

        if (!pSubJson->GetValue("TrainUnitID", &pstApplyInfo->usTrainUnitID))
        {
            GosLog(LOG_ERROR, "get TrainUnitID of MsgInfo failed: %s", szMsgInfo);
            goto error;
        }

        if (!pSubJson->GetValue("CarriageID", &pstApplyInfo->ucCarriageID))
        {
            GosLog(LOG_ERROR, "get CarriageID of MsgInfo failed: %s", szMsgInfo);
            goto error;
        }

        if (!pSubJson->GetValue("IPHDevID", &pstApplyInfo->ucIPHDevID))
        {
            GosLog(LOG_ERROR, "get IPHDevID of MsgInfo failed: %s", szMsgInfo);
            goto error;
        }

        if (!pSubJson->GetValue("ApplyResult", &pstApplyInfo->ucApplyResult))
        {
            GosLog(LOG_ERROR, "get ApplyResult of MsgInfo failed: %s", szMsgInfo);
            goto error;
        }

        if (!pSubJson->GetIP("AudioRecvAddr", pstApplyInfo->aucAudioRecvAddr))
        {
            GosLog(LOG_ERROR, "get AudioRecvAddr of MsgInfo failed: %s", szMsgInfo);
            goto error;
        }

        if (!pSubJson->GetValue("AudioRecvPort", &pstApplyInfo->usAudioRecvPort))
        {
            GosLog(LOG_ERROR, "get AudioRecvPort of MsgInfo failed: %s", szMsgInfo);
            goto error;
        }

        GosLog(LOG_INFO, "IPH apply result of %u %u %u is %u",
                pstApplyInfo->usTrainUnitID, pstApplyInfo->ucCarriageID,
                pstApplyInfo->ucIPHDevID, pstApplyInfo->ucApplyResult);

        if (pstApplyInfo->usTrainUnitID != g_ulTrainUnitID)
        {
            GosLog(LOG_ERROR, "unknown train unit id(%u) in msg: %s", pstApplyInfo->usTrainUnitID, szMsgInfo);
            break;
        }

        if ((i == 0) && g_stSACfg.bIPHVideoProxyMode)
        {
            if (IPH_APPLY_ACCEPT == pstApplyInfo->ucApplyResult)
            {
               
                CHAR    acRTSPServerAddr[64];
                CHAR    acLocalRTSPServerAddr[64];
                RTSP_INFO_T   stRTSPInfo;

                memset(&stRTSPInfo, 0, sizeof(stRTSPInfo));

                /*
                if (!ParseVideoURL(szVideoURL, aucLocalRTSPServerAddr, usLocalRTSPServerPort))
                {
                    GosLog(LOG_ERROR, "invalid rtsp url %s", szVideoURL);
                    continue;
                }*/

                // 根据server地址获取真正的RTSP流地址

                if (!GetRTSPInfo(pstApplyInfo->ucCarriageID, pstApplyInfo->ucIPHDevID, stRTSPInfo))
                //if (!ConvertRtspServerAddr(aucLocalRTSPServerAddr, usLocalRTSPServerPort, aucRTSPServerAddr, usRTSPServerPort))
                {
                    GosLog(LOG_ERROR, "accept unknown iph %u %u", pstApplyInfo->ucCarriageID, pstApplyInfo->ucIPHDevID);
                    goto error;//return;
                }

               
                /* if (!AllocLocalRTSPServerAddr(aucLocalRTSPServerAddr, usLocalRTSPServerPort))
                {
                    GosLog(LOG_ERROR, "alloc local rtsp addr failed");
                    continue;
                } */

                //sprintf(acRTSPServerAddr, IP_FMT ":%u", IP_ARG(aucRTSPServerAddr), usRTSPServerPort);
                sprintf(acRTSPServerAddr, IP_FMT ":%u", IP_ARG(stRTSPInfo.aucRTSPServerAddr), stRTSPInfo.usRTSPServerPort);
                sprintf(acLocalRTSPServerAddr, IP_FMT ":%u", IP_ARG(stRTSPInfo.aucLocalRTSPServerAddr), stRTSPInfo.usLocalRTSPServerPort);

                CLOSE_SOCKET(stRTSPInfo.stPreAllocedSocket);// 关闭已分配端口
                StartRTSP(pstApplyInfo->ucCarriageID, pstApplyInfo->ucIPHDevID, acRTSPServerAddr, acLocalRTSPServerAddr);

                GosLog(LOG_INFO, "start rtsp proxy with rtsp server %s and local addr %s", acRTSPServerAddr, acLocalRTSPServerAddr);
            }
        }

        pstApplyInfo->usTrainUnitID = htons(pstApplyInfo->usTrainUnitID);
        pstApplyInfo->usAudioRecvPort = htons(pstApplyInfo->usAudioRecvPort);

        pstApplyInfo++;
        pstRsp->ucNum++;
        if (pstRsp->ucNum == ARRAY_SIZE(pstRsp->astApplyRsp))
        {
            break;
        }
    }

    if (pstRsp->ucNum > 0)
    {
        ulMsgLen = STRUCT_OFFSET(IPH_TALK_APPLY_RSP_T, astApplyRsp) + pstRsp->ucNum*sizeof(pstRsp->astApplyRsp[0]);
        SendPISMsg(EV_IPH_TALK_APPLY, pstRsp, ulMsgLen);

        GosLog(LOG_INFO, "send IPH_TALK_APPLY to IPH with msg: %s", szMsgInfo);
    }

    free(pstRsp);
    return;

error:
    free(pstRsp);
    return;
}

VOID OnDCStopIPHTalkRsp(CHAR *szMsgInfo, GJson &Json)
{
    IPH_TALK_STOP_RSP_T         stRsp = {0};

    if (!Json.GetValue("TrainUnitID", &stRsp.usTrainUnitID))
    {
        GosLog(LOG_ERROR, "get TrainUnitID of MsgInfo failed: %s", szMsgInfo);
        goto error;
    }

    if (!Json.GetValue("CarriageID", &stRsp.ucCarriageID))
    {
        GosLog(LOG_ERROR, "get CarriageID of MsgInfo failed: %s", szMsgInfo);
        goto error;
    }

    if (!Json.GetValue("IPHDevID", &stRsp.ucIPHDevID))
    {
        GosLog(LOG_ERROR, "get IPHDevID of MsgInfo failed: %s", szMsgInfo);
        goto error;
    }

    GosLog(LOG_INFO, "DC stop IPH talk %u %u %u",
        stRsp.usTrainUnitID, stRsp.ucCarriageID, stRsp.ucIPHDevID);

    if (stRsp.usTrainUnitID != g_ulTrainUnitID)
    {
        GosLog(LOG_ERROR, "invalid train unit id(%u) in msg: %s", stRsp.usTrainUnitID, szMsgInfo);
        return;
    }

    stRsp.usTrainUnitID = htons(stRsp.usTrainUnitID);

    SendPISMsg(EV_IPH_STOP_TALK, &stRsp, sizeof(stRsp));

    if (g_stSACfg.bIPHVideoProxyMode)
    {
        StopRTSP(stRsp.ucCarriageID, stRsp.ucIPHDevID);
    }

    return;

error:
    return;
}

VOID OnDCStartRecBrdReq(CHAR *szMsgInfo, GJson &Json)
{
    PA_REC_BRD_REQ_T    stReq = {0};

    if (!Json.GetValue("RecID", &stReq.ulRecID))
    {
        GosLog(LOG_ERROR, "get RecID of MsgInfo failed: %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("BrdSeqID", &stReq.ulBrdSeqID))
    {
        GosLog(LOG_ERROR, "get BrdSeqID of MsgInfo failed: %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("BrdTime", &stReq.ucBrdLoopTime))
    {
        GosLog(LOG_ERROR, "get BrdTime of MsgInfo failed: %s", szMsgInfo);
        return;
    }

    GosLog(LOG_INFO, "DC start rec broadcast: %u %u %u", stReq.ulRecID, stReq.ulBrdSeqID, stReq.ucBrdLoopTime);

    HTONL_IT(stReq.ulRecID);
    HTONL_IT(stReq.ulBrdSeqID);

    if (!SendPAMsg(EV_PA_REC_BRD, &stReq, sizeof(stReq)))
    {
        GosLog(LOG_ERROR, "send start rec broadcast %u to PA failed", ntohl(stReq.ulBrdSeqID));
    }
    else
    {
        GosLog(LOG_INFO, "send start rec broadcast %u to PA succ", ntohl(stReq.ulBrdSeqID));
    }

#ifdef TEST_MODE
    PA_REC_BRD_RSP_T    stRsp = {0};

    // 发送消息给DIS
    NTOHL_IT(stReq.ulRecID);
    NTOHL_IT(stReq.ulBrdSeqID);

    stRsp.ulBrdSeqID = stReq.ulBrdSeqID;
    stRsp.ulRecID = stReq.ulRecID;
    stRsp.ucRet = 1;

    SendSAStartRecBrdRspMsg(&stRsp);
#endif
}

VOID SetLocalTime(UINT32 ulSecond)
{
#ifndef _OSWIN32_
    struct timespec stNewTime = {0};

    stNewTime.tv_sec  = ulSecond;
    stNewTime.tv_nsec = 0;

    clock_settime(CLOCK_REALTIME, &stNewTime);
#endif
}

VOID OnDISHeartBeatReq(CHAR *szMsgInfo, GJson &Json)
{
    UINT32      ulCurrTime = gos_get_current_time();
    UINT32      ulDISTime;
    static BOOL bSyncFlag = FALSE;
    BOOL        bNeedSyncTime = FALSE;
    GOS_DATETIME_T  stTime;

    if (!Json.GetValue("Time", &ulDISTime))
    {
        GosLog(LOG_ERROR, "get Time of MsgInfo failed: %s", szMsgInfo);
        return;
    }

    g_ulLastHeartBeatMsgTime = gos_get_uptime();

    gos_get_localtime(&ulCurrTime, &stTime);

    if (stTime.usYear < 2020)
    {
        bNeedSyncTime = TRUE;
    }

    if (bNeedSyncTime || !bSyncFlag)
    {
#ifndef _OSWIN32_
        SetLocalTime(ulDISTime);
#endif
        GosLog(LOG_INFO, "auto sync time with DIS");

        bSyncFlag = TRUE;
    }
}

/*
VOID OnDCGetNTPCfgRsp(CHAR *szMsgInfo, GJson &Json)
{
    UINT8       aucNTPServerAddr[4];
    CHAR        *szNTPServerAddr;
    UINT32      ulNTPServerPort = 0;
    UINT32      ulNTPSyncPeriod = 0;

    g_ulLastNTPCfgMsgTime = gos_get_uptime();

    szNTPServerAddr = Json.GetValue("NTPServerAddr");
    if (!gos_get_ip(szNTPServerAddr, aucNTPServerAddr))
    {
        GosLog(LOG_ERROR, "get NTPServerAddr of MsgInfo failed: %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("NTPServerPort", &ulNTPServerPort) ||
        ulNTPServerPort == 0 || ulNTPServerPort >= 0xffff)
    {
        GosLog(LOG_ERROR, "get NTPServerPort of MsgInfo failed: %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("NTPSyncPeriod", &ulNTPSyncPeriod))
    {
        GosLog(LOG_ERROR, "get NTPSyncPeriod of MsgInfo failed: %s", szMsgInfo);
        return;
    }

    if (ulNTPSyncPeriod < 30)
    {
        ulNTPSyncPeriod = 30;
    }
    else if (ulNTPSyncPeriod > 300)
    {
        ulNTPSyncPeriod = 300;
    }

    if (strcmp(g_szNTPServerAddr, szNTPServerAddr) == 0 &&
        g_ulNTPServerPort == ulNTPServerPort &&
        g_ulNTPSyncPeriod == ulNTPSyncPeriod )
    {
        return;
    }

    CfgSet(OMA_NTPCFG_NTPADDR, szNTPServerAddr);
    CfgSet(OMA_NTPCFG_NTPPORT, ulNTPServerPort);
    CfgSet(OMA_NTPCFG_SYNCPERIOD, ulNTPSyncPeriod);

    CfgCommit();
}
*/

VOID OnDISGetCfgReq(CHAR *szMsgInfo, GJson &Json)
{
    UINT8       aucNTPServerAddr[4];
    CHAR        *szValue;
    CHAR        *szNTPServerAddr;            
    UINT32      ulNTPServerPort = 0;
    UINT32      ulNTPSyncPeriod = 0;
//  UINT32      ulResetButtonCheckPeriod = 0;
    UINT32      ulResetButtonCheckTime = 0;
    UINT32      ulResetAPPCheckTime = 0;
    BOOL        bIPHVideoProxyMode;
    BOOL        bCommit = FALSE;
    UINT32      ulSelfCheckSendMaxTime = 0;

    g_ulLastCfgMsgTime = gos_get_uptime();

    szNTPServerAddr = Json.GetValue("NTPServerAddr");
    if (!gos_get_ip(szNTPServerAddr, aucNTPServerAddr))
    {
        GosLog(LOG_ERROR, "get NTPServerAddr of MsgInfo failed: %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("NTPServerPort", &ulNTPServerPort) ||
        ulNTPServerPort == 0 || ulNTPServerPort >= 0xffff)
    {
        GosLog(LOG_ERROR, "get NTPServerPort of MsgInfo failed: %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("NTPSyncPeriod", &ulNTPSyncPeriod))
    {
        GosLog(LOG_ERROR, "get NTPSyncPeriod of MsgInfo failed: %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("ResetButtonCheckTime", &ulResetButtonCheckTime))
    {
        g_ulResetButtonCheckTime = 10;
        GosLog(LOG_ERROR, "get ResetButtonCheckTime of MsgInfo failed: %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("ResetAPPCheckTime", &ulResetAPPCheckTime))
    {
        GosLog(LOG_ERROR, "get ResetAPPCheckTime of MsgInfo failed: %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("IPHVideoProxyMode", &bIPHVideoProxyMode))
    {
        GosLog(LOG_ERROR, "get IPHVideoProxyMode of MsgInfo failed: %s", szMsgInfo);
        bIPHVideoProxyMode = FALSE; ///默认rtspproxy不打开
    }

    if (g_stSACfg.bIPHVideoProxyMode != bIPHVideoProxyMode)
    {
        GosLog(LOG_INFO, "IPHVideoProxyMode is %u", bIPHVideoProxyMode);
    }

    g_stSACfg.bIPHVideoProxyMode = bIPHVideoProxyMode;

    if (!Json.GetValue("SendSelfCheckMsgPeriod", &ulSelfCheckSendMaxTime))
    {
        GosLog(LOG_ERROR, "get SendSelfCheckMsgPeriod of MsgInfo failed: %s", szMsgInfo);
        g_stSACfg.ulSelfCheckMaxTime = 0; ///默认始终发送
    }
    else
    {
        g_stSACfg.ulSelfCheckMaxTime = ulSelfCheckSendMaxTime;
    }

    if (ulNTPSyncPeriod < 30)
    {
        ulNTPSyncPeriod = 30;
    }
    else if (ulNTPSyncPeriod > 300)
    {
        ulNTPSyncPeriod = 300;
    }

    if (ulResetButtonCheckTime > 0 && ulResetButtonCheckTime < 5)
    {
        ulResetButtonCheckTime = 5;
    }

    if (ulResetAPPCheckTime > 0 && ulResetAPPCheckTime < 30)
    {
        ulResetAPPCheckTime = 30;
    }

    if (strcmp(g_acNTPServerAddr, szNTPServerAddr) != 0 ||
        g_ulNTPServerPort != ulNTPServerPort ||
        g_ulNTPSyncPeriod != ulNTPSyncPeriod )
    {
        CfgSet(OMA_NTPCFG_NTPADDR, szNTPServerAddr);
        CfgSet(OMA_NTPCFG_NTPPORT, ulNTPServerPort);
        CfgSet(OMA_NTPCFG_SYNCPERIOD, ulNTPSyncPeriod);

        bCommit = TRUE;

        strcpy(g_acNTPServerAddr, szNTPServerAddr);
        g_ulNTPServerPort = ulNTPServerPort;
        g_ulNTPSyncPeriod = ulNTPSyncPeriod;

        GosLog(LOG_INFO, "NTP addr is %s:%u, sync period is %u", g_acNTPServerAddr, g_ulNTPServerPort, g_ulNTPSyncPeriod);
    }

    // 关闭脚本
    if (g_ulResetButtonCheckTime != ulResetButtonCheckTime ||
        g_ulResetAPPCheckTime    != ulResetAPPCheckTime )
    {
        g_ulResetButtonCheckTime = ulResetButtonCheckTime;
        g_ulResetAPPCheckTime = ulResetAPPCheckTime;

        GosLog(LOG_INFO, "reset button check time is %u, reset APP check time is %u", g_ulResetButtonCheckTime, g_ulResetAPPCheckTime);
    }

    if (bCommit)
    {
        CfgCommit();
    }

    g_bGetDISCfg = TRUE;
}

VOID OnDCGetRecBrdCfgReq()
{
#ifdef TEST_MODE
    g_ulRecBrdCfgNum = 2;

    g_astRecBrdCfg[0].ulID = 1;
    g_astRecBrdCfg[0].szValue = "12345";

    g_astRecBrdCfg[1].ulID = 1;
    g_astRecBrdCfg[1].szValue = "ancde";

    SendDCGetRecBrdCfgRspMsg();
#endif
        
    SendPARecBrdCfgMsg();

    GosLog(LOG_INFO, "get DC get rec brd cfg req");
}

VOID OnOtherDISMsg(DTP_MSG_HDR_T *pstMsg)
{
    if (pstMsg->usMsgType == DTP_EV_LINK_CHECK_REQ)
    {
        DTP_LINK_CHECK_RSP_T    stRsp;

        memset(&stRsp, 0, sizeof(stRsp));

        stRsp.stMsgHdr.ulMsgMagic = DTP_MSG_MAGIC;
        stRsp.stMsgHdr.usMsgVer = DTP_MSG_VER;
        stRsp.stMsgHdr.ulMsgLen  = sizeof(stRsp);
        stRsp.stMsgHdr.usMsgType = DTP_EV_LINK_CHECK_RSP;
        stRsp.stMsgHdr.ulSeqID   = pstMsg->ulSeqID;

        stRsp.stMsgHdr.usSenderModule = INVALID_INST_ID;
        stRsp.stMsgHdr.usReceiverModule = INVALID_INST_ID;

        stRsp.stMsgHdr.ucCompType = DTP_COMPRESS_TYPE_NONE;
        stRsp.stMsgHdr.ucEncryptType = DTP_ENCRYPT_TYPE_NONE;
        stRsp.stMsgHdr.usTimeout = 0;

        stRsp.stMsgHdr.usCRC = 0;          /* 校验码，对消息体数据每2个字节转换为16位整数相加，如有最后一个字节转化为8位整数相加 */
        stRsp.stMsgHdr.ucMsgPri = DTP_MSG_PRI_HIGH;
        stRsp.stMsgHdr.ucSenderType = DIS_CLIENT_SA;

        dtp_msg_ntoh(&stRsp.stMsgHdr);

        if (!gos_udp_send(g_stDISUDPClientSock, &stRsp, sizeof(stRsp), &g_stDISAddr))
        {
            GosLog(LOG_ERROR, "send link check msg to dis failed");
            return;
        }
    }
    else
    {
        GosLog(LOG_ERROR, "unknown dis msg %u: %s", pstMsg->usMsgType, (CHAR*)(pstMsg+1));
    }
}

VOID OnDCStartRealBrdReq(CHAR *szMsgInfo, GJson &Json)
{
    UINT32    ulBrdSeqID;

    if (!Json.GetValue("BrdSeqID", &ulBrdSeqID))
    {
        GosLog(LOG_ERROR, "get MsgSeqID failed");
        return ;
    }

    if (!Json.GetValue("DCUserID", &g_ulDCUserID))
    {
        GosLog(LOG_ERROR, "get DCUserID failed");
        return ;
    }

    GosLog(LOG_INFO, "get DC start real broadcast %u", ulBrdSeqID);

    PA_REAL_BRD_REQ_T  stReq = {0};

    stReq.ulBrdSeqID = HTONL(ulBrdSeqID);
    if (!SendPAMsg(EV_PA_START_REAL_BRD, &stReq, sizeof(stReq)))
    {
        GosLog(LOG_ERROR, "send DC start real broadcast %u to PA failed", ulBrdSeqID);
    }
    else
    {
        GosLog(LOG_INFO, "send DC start real broadcast %u to PA succ", ulBrdSeqID);
    }

#ifdef TEST_MODE
    PA_REAL_BRD_RSP_T    stRsp = {0};

    // 发送消息给DIS
    stRsp.ulBrdSeqID = ulBrdSeqID;
    stRsp.ucBrdResult = 1;

    SendSAStartRealBrdRspMsg(&stRsp);
#endif
}

/**
 * @brief           处理调度台IPH语音业务
 * @param           szMsgInfo    [in]
 * @param           Json    [in]
 * @return          VOID 
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *  
 * @endcode
 * @note
 */
VOID OnDCSendIPHAudioDataReq(CHAR *szMsgInfo, GJson &Json)
{
    PIS_MSG_HDR_T    *pstMsgHdr = NULL;
    IPH_PCM_DATA_T   *pstPMCData = NULL;
    SOCKADDR_IN      stIPHUDPAddr = {0};

    UINT32      ulTrainUnitID;
    CHAR        *szPcmData;
    UINT32      ulTotalMsgLen;
    UINT32      ulDataIndex;
    UINT32      ulSampleRate;
    UINT32      ulChannelNum;
    UINT32      ulSampleBits;
    UINT32      ulIPHDevID;
    UINT32      ulCarriageID;
    UINT32      ulPCMPkgTime;
    UINT32      ulDataLen;
    UINT8       aucDevIP[4] = {0};

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        GosLog(LOG_ERROR, "get TrainUnitID failed in msg %s", szMsgInfo);
        return;
    }

    if (ulTrainUnitID != g_ulTrainUnitID)
    {
        GosLog(LOG_ERROR, "get train_ID is %u in msg: %s, but the train global ID is %u", ulTrainUnitID, szMsgInfo, g_ulTrainUnitID);
        return ;
    }

    if (!Json.GetValue("CarriageID", &ulCarriageID))
    {
        GosLog(LOG_ERROR, "get CarriageID failed in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("IPHDevID", &ulIPHDevID))
    {
        GosLog(LOG_ERROR, "get IPHDevID failed in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("DataIndex", &ulDataIndex))
    {
        GosLog(LOG_ERROR, "get DataIndex failed in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("SampleRate", &ulSampleRate))
    {
        GosLog(LOG_ERROR, "get SampleRate failed in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("ChannelNum", &ulChannelNum))
    {
        GosLog(LOG_ERROR, "get ChannelNum failed in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("SampleBits", &ulSampleBits))
    {
        GosLog(LOG_ERROR, "get SampleBits failed in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("PCMPkgTime", &ulPCMPkgTime))
    {
        GosLog(LOG_ERROR, "get PCMPkgTime failed in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("PCMDataLen", &ulDataLen))
    {
        GosLog(LOG_ERROR, "get PCMDataLen failed in msg %s", szMsgInfo);
        return;
    }

    //if (!Json.GetIP("DevAddr", aucDevIP))
    //{
    //    GosLog(LOG_ERROR, "get DevAddr failed in msg %s", szMsgInfo);
    //    return;
    //}

    szPcmData = Json.GetValue("PCMData");
    if (!szPcmData)
    {
        GosLog(LOG_ERROR, "no PCMData failed in msg %s", szMsgInfo);
        return;
    }

    UINT32          ulPcmPkgSize = (strlen(szPcmData) / 2);

    GosLog(LOG_DETAIL, "PCM data(%u) size is %u", ulDataIndex, ulPcmPkgSize);

    if (ulPcmPkgSize == 0 || ulPcmPkgSize > 64*1024)
    {
        GosLog(LOG_ERROR, "invalid PCM data length %u of iph audio", ulPcmPkgSize);
        return;
    }

    ulTotalMsgLen = sizeof(PIS_MSG_HDR_T) + STRUCT_OFFSET(IPH_PCM_DATA_T, aucPCMData) + ulPcmPkgSize;
    UINT16 usMsgBodyLen = STRUCT_OFFSET(IPH_PCM_DATA_T, aucPCMData) + ulPcmPkgSize;/// 消息体长度

    pstMsgHdr = (PIS_MSG_HDR_T*)calloc(1, ulTotalMsgLen);
    if (!pstMsgHdr)
    {
        GosLog(LOG_ERROR, "calloc PCM data fail of send iph audio");
        return;
    }

    pstPMCData = (IPH_PCM_DATA_T*)(pstMsgHdr+1);
    if (!gos_hexstr_to_byte(szPcmData, pstPMCData->aucPCMData))
    {
        GosLog(LOG_ERROR, "convert PCM data fail of iph audio");
        free(pstMsgHdr);

        return;
    }

    /// 消息头填充
    pstMsgHdr->usMsgMagic = htons(PIS_MSG_MAGIC);
    pstMsgHdr->usMsgType = htons(EV_IPH_AUDIO_DOWNLOAD);
    pstMsgHdr->usMsgLen = htons(usMsgBodyLen); ///STRUCT_OFFSET(IPH_PCM_DATA_T, aucPCMData) + ulCurrPcmDataLen;

    /// 消息体填充
    pstPMCData->usTrainUnitID = htons(ulTrainUnitID);
    pstPMCData->ucCarriageID = ulCarriageID;
    pstPMCData->ucIPHDevID = ulIPHDevID;
    pstPMCData->ulPCMPkgIndex = htonl(ulDataIndex);  // PCM数据包序号，第一个数据包为0，后续递增
    pstPMCData->ulPCMPkgTime = htonl(ulPCMPkgTime);
    pstPMCData->usSampleRate = htons(ulSampleRate);
    pstPMCData->ucChanNum = ulChannelNum;
    pstPMCData->ucSampleBits = ulSampleBits;
    pstPMCData->usPCMDataLen = htons(ulDataLen);

    gos_init_socket_addr(&stIPHUDPAddr, g_stSACfg.stPISAddr.aucIP, IPH_RECV_DC_PORT);
    
    if (!gos_udp_send(g_stIPHUDPClientSock, pstMsgHdr, ulTotalMsgLen, &stIPHUDPAddr))
    {
        GosLog(LOG_ERROR, "send PCM data to iph fail");
    }

    free(pstMsgHdr);

    return ;
}

VOID OnDCSendRealBrdAudioDataReq(CHAR *szMsgInfo, GJson &Json)
{
    PA_MSG_HDR_T    *pstMsgHdr = NULL;
    PA_PCM_DATA_T   *pstPMCData = NULL;
    CHAR        *szPcmData;
    UINT32      ulTotalMsgLen;
    UINT32      ulPAIndex;
    UINT32      ulDataIndex;
    UINT32      ulSampleRate;
    UINT32      ulChannelNum;
    UINT32      ulSampleBits;

    if (!Json.GetValue("BrdSeqID", &ulPAIndex))
    {
        GosLog(LOG_ERROR, "get BrdSeqID failed in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("DataIndex", &ulDataIndex))
    {
        GosLog(LOG_ERROR, "get DataIndex failed in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("SampleRate", &ulSampleRate))
    {
        GosLog(LOG_ERROR, "get SampleRate failed in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("ChannelNum", &ulChannelNum))
    {
        GosLog(LOG_ERROR, "get ChannelNum failed in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("SampleBits", &ulSampleBits))
    {
        GosLog(LOG_ERROR, "get SampleBits failed in msg %s", szMsgInfo);
        return;
    }

    szPcmData = Json.GetValue("PCMData");
    if (!szPcmData)
    {
        GosLog(LOG_ERROR, "no PCMData failed in msg %s", szMsgInfo);
        return;
    }

    UINT32          ulPcmPkgSize = (strlen(szPcmData) / 2);

    GosLog(LOG_DETAIL, "PCM data(%u) size is %u", ulDataIndex, ulPcmPkgSize);

    if (ulPcmPkgSize == 0 || ulPcmPkgSize > 64*1024)
    {
        GosLog(LOG_ERROR, "invalid PCM data length %u", ulPcmPkgSize);
        return;
    }

    ulTotalMsgLen = sizeof(PA_MSG_HDR_T) + STRUCT_OFFSET(PA_PCM_DATA_T, aucPCMData) + ulPcmPkgSize;
    UINT16  usMsgBodyLen = STRUCT_OFFSET(PA_PCM_DATA_T, aucPCMData) + ulPcmPkgSize;

    pstMsgHdr = (PA_MSG_HDR_T*)calloc(1, ulTotalMsgLen);
    if (!pstMsgHdr)
    {
        GosLog(LOG_ERROR, "alloc PCM data fail");
        return;
    }

    pstPMCData = (PA_PCM_DATA_T*)(pstMsgHdr+1);    
    if (!gos_hexstr_to_byte(szPcmData, pstPMCData->aucPCMData))
    {
        GosLog(LOG_ERROR, "convert PCM data fail");
        free(pstMsgHdr);

        return;
    }

    pstMsgHdr->usMsgMagic = htons(PIS_MSG_MAGIC);
    pstMsgHdr->usMsgType  = htons(EV_PA_PUSH_AUDIO_DATA);
    pstMsgHdr->usMsgLen = htons(usMsgBodyLen);

    pstPMCData->ulBrdSeqID = htonl(ulPAIndex);
    pstPMCData->ulPCMPkgIndex = htonl(ulDataIndex);
    pstPMCData->usSampleRate = htons(ulSampleRate);
    pstPMCData->ucChanNum = ulChannelNum;
    pstPMCData->ucSampleBits = ulSampleBits;
    pstPMCData->usPCMDataLen = htons(ulPcmPkgSize);

    if (!gos_udp_send(g_stPAUDPClientSock, pstMsgHdr, ulTotalMsgLen, &g_stPAUDPAddr))
    {
        GosLog(LOG_ERROR, "send PCM data to PA fail");
    }

    free(pstMsgHdr);

    return ;
}

VOID OnDCStopRealBrdReq(CHAR *szMsgInfo, GJson &Json)
{
    UINT32    ulBrdSeqID;

    if (!Json.GetValue("BrdSeqID", &ulBrdSeqID))
    {
        GosLog(LOG_ERROR, "get MsgSeqID failed");
        return ;
    }

    GosLog(LOG_INFO, "get DC stop real broadcast %u", ulBrdSeqID);

    PA_STOP_REAL_BRD_REQ_T  stReq = {0};

    stReq.ulBrdSeqID = HTONL(ulBrdSeqID);
    if (!SendPAMsg(EV_PA_STOP_REAL_BRD, &stReq, sizeof(stReq)))
    {
        GosLog(LOG_ERROR, "send DC stop real broadcast failed");
    }
    else
    {
        GosLog(LOG_INFO, "send DC stop real broadcast succ");
    }
}

VOID OnDISMsg(DTP_MSG_HDR_T *pstMsg)
{
    CHAR        *szReq = (CHAR*)(pstMsg+1);
    GJson       Json;
    GJson       SubJson;
    BOOL        bRet;
    CHAR        *szMsgType;
    CHAR        *szMsgInfo;
    UINT32      ulSeqID;

    pstMsg->usMsgType = ntohs(pstMsg->usMsgType);

    g_ulLastDISMsgTime = gos_get_uptime();

    bRet = Json.Parse(szReq);
    if (!bRet)
    {
        goto error;
    }

    szMsgType = Json.GetValue("MsgName");

    if (!szMsgType)
    {
        GosLog(LOG_ERROR, "get MsgName failed: %s", szReq);
        goto error;
    }

    if (!Json.GetValue("MsgSeqID", &ulSeqID))
    {
        GosLog(LOG_ERROR, "get MsgSeqID failed: %s", szReq);
        goto error;
    }

    szMsgInfo = Json.GetValue("MsgInfo");
    if (!szMsgInfo)
    {
        GosLog(LOG_ERROR, "get MsgInfo failed: %s", szReq);
        goto error;
    }

    if (!SubJson.Parse(szMsgInfo))
    {
        GosLog(LOG_ERROR, "parse MsgInfo failed: %s", szReq);
        goto error;
    }

    if (strcmp(szMsgType, MSG_DCAcceptIPHTalk) == 0)
    {
        OnDCAcceptIPHTalkRsp(szReq, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCStopIPHTalk) == 0)
    {
        OnDCStopIPHTalkRsp(szReq, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCStartRecBrd) == 0)
    {
        OnDCStartRecBrdReq(szReq, SubJson);
    }
    else if (strcmp(szMsgType, MSG_SAHeartBeat) == 0)
    {
        OnDISHeartBeatReq(szReq, SubJson);
    }
    else if (strcmp(szMsgType, MSG_SAGetCfg) == 0)
    {
        OnDISGetCfgReq(szReq, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCGetRecBrdCfg) == 0)
    {
        OnDCGetRecBrdCfgReq();
    }
    else if (strcmp(szMsgType, MSG_DCStartRealBrd) == 0)
    {
        OnDCStartRealBrdReq(szReq, SubJson);
    }
    else if(strcmp(szMsgType, MSG_DCSendRealBrdAudioData) == 0)
    {
        OnDCSendRealBrdAudioDataReq(szReq, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCStopRealBrd) == 0)
    {
        OnDCStopRealBrdReq(szReq, SubJson);
    }
    else if(strcmp(szMsgType, MSG_DCSendIPHAudioData) == 0)
    {
        OnDCSendIPHAudioDataReq(szReq, SubJson);
    }
    else
    {
        OnOtherDISMsg(pstMsg);
    }

    return;

error:
    return;
}

BOOL SendDISMsg(const CHAR *szMsg)
{
    UINT32          ulMsgLen = strlen(szMsg)+1;
//  DTP_MSG_HDR_T   stMsg = {0};
    UINT32          ulTotalMsgLen = sizeof(DTP_MSG_HDR_T) + ulMsgLen;
    DTP_MSG_HDR_T   *pstMsg = (DTP_MSG_HDR_T*)calloc(1, ulTotalMsgLen);

    if (!pstMsg)
    {
        GosLog(LOG_ERROR, "calloc failed for func SendDISMsg()");
        
        return FALSE;
    }

    pstMsg->ulMsgMagic = DTP_MSG_MAGIC;
    pstMsg->usMsgVer = DTP_MSG_VER;
    pstMsg->usMsgType = EV_SA_DIS_REQ;      /* 消息类型 */

    pstMsg->ulMsgLen = sizeof(DTP_MSG_HDR_T)+ulMsgLen;       /* 消息总长度(含消息头及消息体) */
    pstMsg->ulSeqID = gos_get_uptime();

    pstMsg->usSenderModule = INVALID_INST_ID;
    pstMsg->usReceiverModule = INVALID_INST_ID;

    pstMsg->ucCompType = DTP_COMPRESS_TYPE_NONE;
    pstMsg->ucEncryptType = DTP_ENCRYPT_TYPE_NONE;
    pstMsg->usTimeout = 0;

    pstMsg->usCRC = 0;          /* 校验码，对消息体数据每2个字节转换为16位整数相加，如有最后一个字节转化为8位整数相加 */
    pstMsg->ucMsgPri = DTP_MSG_PRI_HIGH;
    pstMsg->ucSenderType = DIS_CLIENT_SA;

    memcpy(pstMsg+1, szMsg, ulMsgLen);

    dtp_msg_ntoh(pstMsg);

    if  (!gos_udp_send(g_stDISUDPClientSock, pstMsg, ulTotalMsgLen, &g_stDISAddr))
    {
        free(pstMsg);

        GosLog(LOG_ERROR, "gos_udp_send to dis failed ,msg is %s", szMsg);

        return FALSE;
    }

    free(pstMsg);
    g_ulLastDISMsgTime = gos_get_uptime();
    return TRUE;
}

BOOL SendDISMsg(const CHAR *szMsgName, UINT32 ulSeqID, GJsonParam &Param)
{
    GJsonParam      ReqParam;

    ReqParam.Add("MsgName", szMsgName);
    ReqParam.Add("MsgSeqID", ulSeqID);
    ReqParam.Add("MsgInfo", Param);
    ReqParam.Add("SAID", g_acSAID);
    ReqParam.Add("TrainUnitID", g_ulTrainUnitID);

    CHAR    *szMsg = ReqParam.GetString();

    return SendDISMsg(szMsg);
}

BOOL SendDISMsg(const CHAR *szMsgName, UINT32 ulSeqID, GJsonTupleParam &Param)
{
    GJsonParam      ReqParam;

    ReqParam.Add("MsgName", szMsgName);
    ReqParam.Add("MsgSeqID", ulSeqID);
    ReqParam.Add("MsgInfo", Param);
    ReqParam.Add("SAID", g_acSAID);
    ReqParam.Add("TrainUnitID", g_ulTrainUnitID);

    CHAR    *szMsg = ReqParam.GetString();

    return SendDISMsg(szMsg);
}

VOID SendDISHeartBeatMsg()
{
    static UINT32   ulLastSendTime = 0;
    UINT32      ulCurrTime = gos_get_uptime();
    GJsonParam  Param;

    if (ulLastSendTime > 0 && ulCurrTime < (2+ulLastSendTime))
    {
        return;
    }

    if (g_ulLastHeartBeatMsgTime > 0 &&
        ulCurrTime < (g_stSACfg.ulDISHeartBeatTime+g_ulLastHeartBeatMsgTime))
    {
        return;
    }

    Param.Add("TrainUnitID", g_ulTrainUnitID);
    Param.Add("SAID", g_acSAID);

    GosLog(LOG_DETAIL, "send heart beat check msg");

    SendDISMsg(MSG_SAHeartBeat, gos_get_current_time(), Param);

#if 0
    for (UINT32 j=0; j<20; j++)
    {
        IPH_TALK_APPLY_REQ_T    stReq;

        g_stSACfg.ulDISHeartBeatTime = 1;

        stReq.ucNum = 20;
        for (UINT32 i=0; i<20; i++)
        {
            stReq.astApplyReq[i].usTrainUnitID = htons(18001);
            stReq.astApplyReq[i].ucCarriageID = i/5+1;
            stReq.astApplyReq[i].ucIPHDevID = MOD(i, 5)+1;
            strcpy(stReq.astApplyReq[i].acVideoUrl, "rtsp://10.61.46.54:11001/h264/ch1/sub/av_stream");
        }

        SendSAApplyIPHTalkReqMsg(&stReq);
    }
#endif

    ulLastSendTime = gos_get_uptime();
    if (ulLastSendTime == 0)
    {
        ulLastSendTime = 1;
    }
}

VOID SendDISGetCfgMsg()
{
    static UINT32   ulLastSendTime = 0;
    UINT32      ulCurrTime = gos_get_uptime();
    UINT32      ulTimeout = 300;

    if (ulLastSendTime > 0)
    {
        if (ulCurrTime < (ulLastSendTime + 2))
        {
            return;
        }

        if (ulCurrTime < (g_ulLastCfgMsgTime + ulTimeout))
        {
            return;
        }
    }

    GJsonParam  Param;

    Param.Add("TrainUnitID", g_ulTrainUnitID);

    if (SendDISMsg(MSG_SAGetCfg, gos_get_current_time(), Param) == FALSE)
    {
        //g_ulResetButtonCheckTime = 10;
        GosLog(LOG_ERROR, "send to dis get cfg failed, now reset button check time:%u", g_ulResetButtonCheckTime);
    }

    ulLastSendTime = gos_get_uptime();
    if (ulLastSendTime == 0)
    {
        ulLastSendTime = 1;
    }

    GosLog(LOG_DETAIL, "send get cfg msg");
}

/*
    {"MsgName":"SAApplyIPHTalkReq",  // 接口名称
        "MsgSeqID":"123",          // 接口消息序列号
        "MsgInfo":                 // 接口信息
    [{"TrainUnitID":"123",
        "CarriageID":"2",
        "IPHDevID":"3",
        "VideoURL":"rtsp://192.168.30.189:11003/h264/ch1/main/av_stream"},
    {"TrainUnitID":"123",
    "CarriageID":"2",
    "IPHDevID":"4",
    "VideoURL":"rtsp://192.168.30.189:11004/h264/ch1/main/av_stream"}]
    }*/

BOOL SendSAApplyIPHTalkReqMsg(IPH_TALK_APPLY_REQ_T *pstReq)
{
    GJsonTupleParam Param;

    for (UINT32 i=0; i<pstReq->ucNum; i++)
    {
        GJsonParam      SubParam;

        pstReq->astApplyReq[i].usTrainUnitID = ntohs(pstReq->astApplyReq[i].usTrainUnitID);

        SubParam.Add("TrainUnitID", pstReq->astApplyReq[i].usTrainUnitID);
        SubParam.Add("SAID", g_acSAID);
        SubParam.Add("CarriageID", pstReq->astApplyReq[i].ucCarriageID);
        SubParam.Add("IPHDevID", pstReq->astApplyReq[i].ucIPHDevID);
        SubParam.AddIP("IPHDevIP", g_stSACfg.stPISAddr.aucIP);
        SubParam.Add("VideoURL", pstReq->astApplyReq[i].acVideoUrl);

        GosLog(LOG_INFO, "get IPH talk request %u: %u %u %u", i+1,
            pstReq->astApplyReq[i].usTrainUnitID,
            pstReq->astApplyReq[i].ucCarriageID,
            pstReq->astApplyReq[i].ucIPHDevID);

        Param.Add(SubParam);
    }

    return SendDISMsg(MSG_SAApplyIPHTalk, 0, Param);
}

BOOL SendSAStopIPHTalkReqMsg(IPH_TALK_STOP_REQ_T *pstReq)
{
    GJsonParam      Param;

    Param.Add("TrainUnitID", pstReq->usTrainUnitID);
    Param.Add("SAID", g_acSAID);
    Param.Add("CarriageID", pstReq->ucCarriageID);
    Param.Add("IPHDevID", pstReq->ucIPHDevID);

    GosLog(LOG_INFO, "get IPH stop talk request: %u %u %u",
        pstReq->usTrainUnitID,
        pstReq->ucCarriageID,
        pstReq->ucIPHDevID);

    return SendDISMsg(MSG_SAStopIPHTalk, 0, Param);
}

BOOL SendSAStopRealBrdRspMsg(PA_STOP_REAL_BRD_RSP_T *pstRsp)
{
    GJsonParam      Param;

    Param.Add("TrainUnitID", g_ulTrainUnitID);
    Param.Add("SAID", g_acSAID);
    Param.Add("BrdSeqID", pstRsp->ulBrdSeqID);

    GosLog(LOG_INFO, "send stop real broadcast response:  %u ", pstRsp->ulBrdSeqID);

    return SendDISMsg(MSG_DCStopRealBrd, gos_get_current_time(), Param);
}

BOOL SendSAStartRealBrdRspMsg(PA_REAL_BRD_RSP_T *pstRsp)
{
    GJsonParam      Param;

    Param.Add("TrainUnitID", g_ulTrainUnitID);
    Param.Add("SAID", g_acSAID);
    Param.Add("BrdSeqID", pstRsp->ulBrdSeqID);
    Param.Add("DCUserID", g_ulDCUserID);
    Param.Add("Result", pstRsp->ucBrdResult);

    GosLog(LOG_INFO, "send start real broadcast %u to DIS, PA result is %u",
        pstRsp->ulBrdSeqID, pstRsp->ucBrdResult);

    return SendDISMsg(MSG_DCStartRealBrd, gos_get_current_time(), Param);
}

BOOL SendSAPAHeartBeatRspMsg(PA_GET_SYS_STATUS_RSP_T *pstRsp)
{
    GJsonParam      Param;

    Param.Add("TrainUnitID", g_ulTrainUnitID);
    Param.Add("SAID", g_acSAID);
    Param.Add("Master", pstRsp->ucIsMaster);
    Param.Add("Running", pstRsp->ucStatus);

    GosLog(LOG_INFO, "send PA heart beat response: Master:%u Status:%u",
        pstRsp->ucIsMaster, pstRsp->ucStatus);

    return SendDISMsg(MSG_PAHeartBeat, 0, Param);
}

BOOL SendSAIPHHeartBeatRspMsg(IPH_GET_SYS_STATUS_RSP_T *pstRsp)
{
    GJsonParam      Param;

    Param.Add("TrainUnitID", g_ulTrainUnitID);
    Param.Add("SAID", g_acSAID);
    Param.Add("Status", pstRsp->ucStatus);
    
    GosLog(LOG_INFO, "send IPH heart beat response: Status:%u",
        pstRsp->ucStatus);

    return SendDISMsg(MSG_IPHHeartBeat, 0, Param);
}

BOOL SendSAStartRecBrdRspMsg(PA_REC_BRD_RSP_T *pstRsp)
{
    GJsonParam      Param;

    Param.Add("TrainUnitID", g_ulTrainUnitID);
    Param.Add("SAID", g_acSAID);
    Param.Add("RecID", pstRsp->ulRecID);
    Param.Add("BrdSeqID", pstRsp->ulBrdSeqID);
    Param.Add("Result", pstRsp->ucRet);

    GosLog(LOG_INFO, "send start rec broadcast response: %u %u %u",
           pstRsp->ulRecID, pstRsp->ulBrdSeqID, pstRsp->ucRet);

    return SendDISMsg(MSG_DCStartRecBrd, 0, Param);
}

VOID SendDCGetRecBrdCfgRspMsg()
{
    GJsonParam      Param;
    GJsonTupleParam TupleParam;
    GJsonParam      SubParam;

    for (UINT32 i=0; i<g_ulRecBrdCfgNum; i++)
    {
        SubParam.Clear();
        SubParam.Add("RecID", g_astRecBrdCfg[i].ulID);
        SubParam.Add("RecInfo", g_astRecBrdCfg[i].szValue);

        TupleParam.Add(SubParam);
    }

    Param.Add("TrainUnitID", g_ulTrainUnitID);
    Param.Add("RecBrdCfg", TupleParam);

    SendDISMsg(MSG_SAGetRecBrdCfg, 0, Param);

    GosLog(LOG_INFO, "send GetRecBrdCfg to DIS");
}

#include "dtp_client.h"

static HANDLE  g_hClient = NULL;

static DTP_MSG_T      g_stSyncRsp;

static DTP_CLIENT_CFG_T    g_stClientCfg = {0};

#define BLOCK_NUM       1024//*5

static DTP_MSG_POOL_T g_astMsgPool[] = {
                            {512,  BLOCK_NUM},
                            {1024, BLOCK_NUM},
                            {4*1024, BLOCK_NUM},
                            {16*1024, BLOCK_NUM},
                            {64*1024, BLOCK_NUM},
                            };

extern BOOL dtp_tcp_send(SOCKET stSocket, VOID *pstReqMsg, UINT32 ulReqMsgLen);

VOID dtp_utest_init_client_cfg(VOID)
{
    memset(&g_stClientCfg, 0, sizeof(g_stClientCfg));

    SET_IP(g_stClientCfg.aucServerAddr, 127,0,0,1);
    g_stClientCfg.usServerPort = 0;
    strcpy(g_stClientCfg.acLoginID, "Account1");
    g_stClientCfg.bAllowSyncMsg = FALSE;

    g_stClientCfg.ulLinkHelloInterval = 3;
}

VOID dtp_client_utest_msg_handle(DTP_SERVER_ID_T *pstServerID, DTP_MSG_T *pstMsg)
{
    UINT32  i;
#if 0
    switch(pstMsg->stMsgHdr.usMsgType)
    {
        case EV_SCG_OMS_ABIS_LINK_ALARM_REQ:
        {
            SCG_OMS_ABIS_LINK_ALARM_REQ_T   *pstReq = pstMsg->aucMsgBody;

            NTOHL_IT(pstReq->ulNum);
            for (i=0; i<pstReq->ulNum; i++)
            {
                dtp_log(LOG_INFO, "ABIS_LINK_ALARM: %s %u %u %u %u %u %u %u %u %u",
                    pstReq->astLink[i].ucNewLink?"new":"remove",
                    pstReq->astLink[i].ucCardId,
                    pstReq->astLink[i].ucPortNo,
                    pstReq->astLink[i].ucE1,
                    pstReq->astLink[i].ucSTM1,
                    pstReq->astLink[i].ucTS,
                    pstReq->astLink[i].ucDiretion,
                    pstReq->astLink[i].ucPortType,
                    pstReq->astLink[i].ucRate,
                    NTOHL(pstReq->astLink[i].ulCellID));
            }

            break;
        }

        case EV_SCG_OMS_A_LINK_INFORM_REQ:
        {
            SCG_OMS_A_LINK_INFORM_REQ_T   *pstReq = pstMsg->aucMsgBody;

            NTOHL_IT(pstReq->ulNum);
            for (i=0; i<pstReq->ulNum; i++)
            {
                dtp_log(LOG_INFO, "A_LINK_INFORM: %s %u %u %02X%02X%02X %02X%02X%02X %u %u",
                    pstReq->astLink[i].ucNewLink?"new":"remove",
                    pstReq->astLink[i].ucCardId,
                    pstReq->astLink[i].ucPortNo,
                    pstReq->astLink[i].aucOpc[0],pstReq->astLink[i].aucOpc[1],pstReq->astLink[i].aucOpc[2],
                    pstReq->astLink[i].aucDpc[0],pstReq->astLink[i].aucDpc[1],pstReq->astLink[i].aucDpc[2],
                    pstReq->astLink[i].ucLinkID,
                    pstReq->astLink[i].ucLinkDirect);
            }

            break;
        }

        case EV_SCG_OMS_ABIS_LINK_INFORM_REQ:
        {
            SCG_OMS_ABIS_LINK_INFORM_REQ_T   *pstReq = pstMsg->aucMsgBody;

            NTOHL_IT(pstReq->ulNum);
            for (i=0; i<pstReq->ulNum; i++)
            {
                dtp_log(LOG_INFO, "ABIS_LINK_INFORM: %s %u %u %u %u %u %u %u %u %u",
                    pstReq->astLink[i].ucNewLink?"new":"remove",
                    pstReq->astLink[i].ucCardId,
                    pstReq->astLink[i].ucPortNo,
                    pstReq->astLink[i].ucE1,
                    pstReq->astLink[i].ucSTM1,
                    pstReq->astLink[i].ucTS,
                    pstReq->astLink[i].ucDiretion,
                    pstReq->astLink[i].ucPortType,
                    pstReq->astLink[i].ucRate,
                    NTOHL(pstReq->astLink[i].ulCellID));
            }

            break;
        }

        case EV_SCG_OMS_ALARM_REPORT_REQ:
        {
            SCG_OMS_ALARM_REPORT_REQ_T   *pstReq = pstMsg->aucMsgBody;

            NTOHL_IT(pstReq->stAlarmInfo.ulAlarmTime);
            dtp_log(LOG_INFO, "ALARM_REPORT: %u %u %s",
                    NTOHL(pstReq->stAlarmInfo.ulAlarmID),
                    NTOHL(pstReq->stAlarmInfo.ulAlarmState),
                    gos_get_text_time(&pstReq->stAlarmInfo.ulAlarmTime));
            break;
        }

        case EV_SCG_OMS_E1_LINK_CHECK_RSP:
             break;

        default:
            dtp_log(LOG_INFO, "unknow msg type = %d", pstMsg->stMsgHdr.usMsgType);
            break;
    }
#endif
    return;
}

VOID dtp_utest_scg(VOID)
{
    HANDLE          hClient = NULL;
    UINT16          usRspMsgLen = 0;
    UINT32          ulTimeout = 3;
    UINT32          ulLen;
    UINT32          i;
    FILE            *fp;
    UINT32          ulSize;
    UINT8           aucReq[64];
    UINT8           aucRsp[64];

    /* Õý³£µÄscgid */
    dtp_utest_init_client_cfg();
    SET_IP(g_stClientCfg.aucServerAddr, 192,168,0,58)
    g_stClientCfg.usServerPort = DTP_DEFAULT_SERVER_PORT+1;
    g_stClientCfg.bAllowSyncMsg = TRUE;
    g_stClientCfg.ulLinkHelloInterval = 60;
    g_stClientCfg.ulLinkHelloFailNum = 10;
    hClient = dtp_client_init(&g_stClientCfg, dtp_client_utest_msg_handle, ARRAY_SIZE(g_astMsgPool), g_astMsgPool);
    dtp_client_run(hClient);

    while (!dtp_client_is_connected(hClient))
    {
        gos_sleep_1ms(100);
    }

    dtp_client_ssend(hClient, 20549,
                              aucReq, sizeof(aucReq),
                              aucRsp, sizeof(aucRsp),
                              &usRspMsgLen, ulTimeout);

#if 0
    while (0)
    {
        static SCG_OMS_GET_ALL_CONFIG_RSP_T        stGetAllCfgRsp;

        if (dtp_client_ssend(hClient, EV_SCG_OMS_GET_ALL_CONFIG_REQ,
                              NULL, 0,
                              &stGetAllCfgRsp, sizeof(stGetAllCfgRsp),
                              &usRspMsgLen, ulTimeout))
        {
            fp = fopen("c:\\cfg.xml", "wb");
            ulSize = NTOHL(stGetAllCfgRsp.ulLen);

            ulSize = fwrite(stGetAllCfgRsp.acConfig, 1, ulSize, fp);
            fclose(fp);
            break;
        }
    }

    while (0)
    {
        static SCG_OMS_GET_IF1_STAT_RSP_T           stRsp;

        if (dtp_client_ssend(hClient, EV_SCG_OMS_GET_IF1_STAT_REQ,
                             NULL, 0,
                              &stRsp, sizeof(stRsp),
                              &usRspMsgLen, ulTimeout))
        {
            break;
        }
    }

    while (0)
    {
        static SCG_OMS_CLOSE_IF1_REQ_T           stReq;
        static SCG_OMS_CLOSE_IF1_RSP_T           stRsp;

        stReq.ucReason = 1;
        if (dtp_client_ssend(hClient, EV_SCG_OMS_CLOSE_IF1_REQ,
                              (VOID*)&stReq, sizeof(stReq),
                              &stRsp, sizeof(stRsp),
                              &usRspMsgLen, ulTimeout))
        {
            break;
        }
    }

    while (0)
    {
        static SCG_OMS_SET_ALL_CONFIG_REQ_T         stSetAllCfgReq;
        SCG_OMS_SET_ALL_CONFIG_RSP_T               stSetAllCfgRsp;

        fp = fopen("c:\\cfg.xml", "rb");
        fseek(fp, 0, SEEK_END);
        stSetAllCfgReq.ulLen = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        stSetAllCfgReq.ulLen = fread(stSetAllCfgReq.acConfig, 1, stSetAllCfgReq.ulLen, fp);
        stSetAllCfgReq.acConfig[stSetAllCfgReq.ulLen++] = 0;
        fclose(fp);

        ulLen = STRUCT_OFFSET(SCG_OMS_SET_ALL_CONFIG_REQ_T, acConfig) + stSetAllCfgReq.ulLen;
        HTONL_IT(stSetAllCfgReq.ulLen);
        if (dtp_client_ssend(hClient, EV_SCG_OMS_SET_ALL_CONFIG_REQ,
                              &stSetAllCfgReq, ulLen,
                              &stSetAllCfgRsp, sizeof(stSetAllCfgRsp),
                              &usRspMsgLen, ulTimeout) &&
            stSetAllCfgRsp.ulResult == 0)
        {
            break;
        }
    }

    if (0)
    {
        for (i=0; i<1000; i++)
        {
            static SCG_OMS_GET_DEVICE_INFO_RSP_T        stGetDeviceInfoRsp;

            if (dtp_client_ssend(hClient, EV_SCG_OMS_GET_DEVICE_INFO_REQ,
                                  NULL, 0,
                                  &stGetDeviceInfoRsp, sizeof(stGetDeviceInfoRsp),
                                  &usRspMsgLen, ulTimeout))
            {
                NTOHLL_IT(stGetDeviceInfoRsp.stDeviceInfo.u64CpuFreq);
                NTOHLL_IT(stGetDeviceInfoRsp.stDeviceInfo.u64MemTotalSize);
                NTOHLL_IT(stGetDeviceInfoRsp.stDeviceInfo.u64MemFreeSize);
                NTOHLL_IT(stGetDeviceInfoRsp.stDeviceInfo.u64DiskTotalSize);
                NTOHLL_IT(stGetDeviceInfoRsp.stDeviceInfo.u64DiskFreeSize);

                dtp_log(LOG_INFO, "scg id = %s, result = %u, "
                        " man = %s, code = %s, "
                        "cpu feq = "FORMAT_U64 ", "
                        "disk code= %s, "
                        "mem total = "FORMAT_U64 ", "
                        "mem free = "FORMAT_U64 ", "
                        "disk total = "FORMAT_U64 ", "
                        "disk free = "FORMAT_U64 ", "
                        "mem usage = %.2f "
                        "disk usage = %.2f ",
                        stGetDeviceInfoRsp.acSCGID,
                        NTOHL(stGetDeviceInfoRsp.ulResult),
                        stGetDeviceInfoRsp.stDeviceInfo.acHostProducer,
                        stGetDeviceInfoRsp.stDeviceInfo.acIdCode,
                        stGetDeviceInfoRsp.stDeviceInfo.u64CpuFreq,
                        stGetDeviceInfoRsp.stDeviceInfo.acDiskIdCode,
                        stGetDeviceInfoRsp.stDeviceInfo.u64MemTotalSize,
                        stGetDeviceInfoRsp.stDeviceInfo.u64MemFreeSize,
                        stGetDeviceInfoRsp.stDeviceInfo.u64DiskTotalSize,
                        stGetDeviceInfoRsp.stDeviceInfo.u64DiskFreeSize,
                        (DOUBLE)(INT64)(100-stGetDeviceInfoRsp.stDeviceInfo.u64MemFreeSize*100/stGetDeviceInfoRsp.stDeviceInfo.u64MemTotalSize),
                        (DOUBLE)(INT64)(100-stGetDeviceInfoRsp.stDeviceInfo.u64DiskFreeSize*100/stGetDeviceInfoRsp.stDeviceInfo.u64DiskTotalSize));

            }

            gos_sleep(10);
        }
    }

    while (0)
    {
        static SCG_OMS_GET_ADAPTER_INFO_RSP_T        stGetAdapterInfoRsp;

        if (dtp_client_ssend(hClient, EV_SCG_OMS_GET_ADAPTER_INFO_REQ,
                              NULL, 0,
                              &stGetAdapterInfoRsp, sizeof(stGetAdapterInfoRsp),
                              &usRspMsgLen, 10))
        {
            break;
        }
    }

    while (0)
    {
        SCG_OMS_RESET_IND_T stMsg = {0};

        stMsg.ulDelayTime = HTONL(1000);
        if (dtp_client_send_req(hClient, EV_SCG_OMS_RESET_IND, &stMsg, sizeof(stMsg)))
        {
            break;
        }
    }

    while (1)
    {
        SCG_OMS_E1_LINK_CHECK_REQ_T stMsg = {0};
        static BOOL bStart = FALSE;
        static BOOL bStop = FALSE;

        if (bStart)
        {
            stMsg.ulCheckPeriod = HTONL(10);
            dtp_client_send_req(hClient, EV_SCG_OMS_E1_LINK_CHECK_REQ, &stMsg, sizeof(stMsg));
            bStart = FALSE;
        }

        if (bStop)
        {
            stMsg.ulCheckPeriod = HTONL(0);
            dtp_client_send_req(hClient, EV_SCG_OMS_E1_LINK_CHECK_REQ, &stMsg, sizeof(stMsg));
            bStop = FALSE;
        }

        gos_sleep(1);
    }
#endif

    dtp_client_close(hClient);
}

VOID dtp_utest(VOID)
{
    dtp_set_log_level(LOG_INFO);

    dtp_utest_scg();

    getchar();
}

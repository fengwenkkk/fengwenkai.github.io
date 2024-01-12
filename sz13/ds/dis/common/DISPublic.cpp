#include "DISPublic.h"
#include "pis.h"

static GMutex   g_CfgMutex;
static GMutex   g_DCMutex;
static GMutex   g_DCLoginMutex;
static GMutex   g_ATSMutex;
static GMutex   g_StationMutex;
static GMutex   g_SectionMutex;
static GMutex   g_UserMutex;
static GMutex   g_BrdZoneMutex;
static GMutex   g_TrainMutex;
static GMutex   g_RegisterMutex;
static GMutex   g_PredefinedSMSMutex;
static GMutex   g_MultiGroupMutex;
static GMutex   g_APPBasicCfgMutex;
static GMutex   g_LineCfgMutex;
static GMutex   g_DestInfoMutex;
static GMutex   g_RecBrdCfgMutex;

LOCAL_CFG_T                     g_stLocalCfg;     // 初始化时赋值，后续不会修改

GROUP_CFG_T                     g_stGroupCfg = {0};

VectorMap<UINT32, ATS_INFO_T>   g_mATSInfo;
VECTOR<DEPOT_INFO_T>            g_vDepotInfo;
VECTOR<STATION_INFO_T>          g_vStationInfo;
VECTOR<STATION_UE_INFO_T>       g_vStationUEInfo;
VECTOR<TRAIN_UE_INFO_T>         g_vTrainUEInfo;
VECTOR<PORTAL_UE_INFO_T>        g_vPortalUEInfo;
VECTOR<BRD_ZONE_INFO_T>         g_vBrdZoneInfo;
VECTOR<DC_USER_INFO_T>          g_vDCUserInfo;
VECTOR<DC_OPERATOR_INFO_T>      g_vDCOperatorInfo;
VECTOR<PREDEFINED_SMS_INFO_T>   g_vPredefinedSMS;
VECTOR<REC_BRD_CFG_T>           g_vRecBrdCfg;
VECTOR<MULTI_GROUP_INFO_T>      g_vMultiGroupInfo;
VectorMap<UINT32, DEST_INFO_T>  g_mDestInfo;

RawIntMap<SECTION_INFO_T>           g_mSectionInfo(1024);
std::map<UINT32, SECTION_INFO_T*>   g_mUpsideSectionInfo;
std::map<UINT32, SECTION_INFO_T*>   g_mDownsideSectionInfo;

APP_BASIC_CFG_T                 g_stAPPBasicCfg = {0};

VectorMap<UINT32, TRAIN_INFO_T> g_mTrainInfo;

RawIntMap<REGISTER_INFO_T>      g_mAPPRegisterInfo(256);
RawIntMap<PID_T>                g_mLoginedDCUser(16);

UINT32                          g_ulLastATSDataTime = 0;
static BOOL                     g_bATSValid = TRUE;

extern CHAR                     *g_szConfigFile;

BOOL ReplaceString(CHAR *szInfo, UINT32 ulMaxLen, const CHAR *szFrom, const CHAR *szTo)
{
    if (!szInfo || !szFrom || !szTo)
    {
        return FALSE;
    }

    CHAR    *szOri = szInfo;
    UINT32  ulFromLen = strlen(szFrom);
    UINT32  ulToLen = strlen(szTo);

    if (ulFromLen == 0 ||
        ulToLen == 0)
    {
        return TRUE;
    }

    CHAR    *szNew = (CHAR*)calloc(1, ulMaxLen+1);
    UINT32  ulLen = 0;

    if (!szNew)
    {
        return FALSE;
    }

    while(*szInfo)
    {
        if (strncmp(szInfo, szFrom, ulFromLen) != 0)
        {
            if ((ulLen+1) >= ulMaxLen)
            {
                free(szNew);
                return FALSE;
            }

            szNew[ulLen++] = *szInfo++;
        }
        else
        {
            if ((ulLen+ulToLen) >= ulMaxLen)
            {
                free(szNew);
                return FALSE;
            }

            strcpy(szNew+ulLen, szTo);
            szInfo += ulFromLen;
            ulLen += ulToLen;
        }
    }

    strcpy(szOri, szNew);
    free(szNew);

    return TRUE;
}

// Encode 由GJson完成
BOOL EncodeJsonMsg(CHAR *szInfo, UINT32 ulMaxLen)
{
    return TRUE;//ReplaceString(szInfo, ulMaxLen, "\"", "&quot;");
}

BOOL DecodeJsonMsg(CHAR *szInfo, UINT32 ulMaxLen)
{
    return TRUE;//ReplaceString(szInfo, ulMaxLen, "&quot;", "\"");
}

LOCAL_CFG_T& GetLocalCfg()
{
    return g_stLocalCfg;
}

VOID GetBoolCfg(GConfFile &Conf, const CHAR *szKey, BOOL &bValue, BOOL bDefaultValue)
{
    bValue = bDefaultValue;

    CHAR    *szValue = Conf.Get(szKey);

    if (szValue)
    {
        if (bDefaultValue == FALSE) // 如果缺省值为FALSE
        {
            if (strcasecmp(szValue, "true") == 0 ||
                strcmp(szValue, "1") == 0 )
            {
                bValue = TRUE;
            }
        }
        else
        {
            if (strcasecmp(szValue, "false") == 0 ||
                strcmp(szValue, "0") == 0 )
            {
                bValue = FALSE;
            }
        }
    }

    GosLog(LOG_INFO, "%s is %u", szKey, bValue);
}

BOOL GetIntCfg(GConfFile &Conf, const CHAR *szKey, UINT32 &ulValue, UINT32 ulDefaultValue, UINT32 ulMinValue=0, UINT32 ulMaxValue=0xffffffff)
{
    CHAR    *szValue;

    ulValue = ulDefaultValue;
    szValue = Conf.Get(szKey);//, ulDefaultValue, &ulValue))
    if (szValue)
    {
        if (!gos_atou(szValue, &ulValue))
        {
            GosLog(LOG_ERROR, "invalid %s in %s", szKey, g_szConfigFile);
            return FALSE;
        }
    }

    if (ulMinValue >  0 &&
        ulValue < ulMinValue )
    {
        ulValue = ulMinValue;
    }

    if (ulMaxValue != 0xffffffff &&
        ulValue > ulMaxValue )
    {
        ulValue = ulMaxValue;
    }

    GosLog(LOG_INFO, "%s is %u", szKey, ulValue);

    return TRUE;
}

BOOL LoadLocalCfg()
{
    memset(&g_stLocalCfg, 0, sizeof(g_stLocalCfg));

    // ATS配置
    g_stLocalCfg.bSaveATSToFile = TRUE;        // 0:no 1:all 2:only different
    g_stLocalCfg.ulMaxATSFileNum = 366;        // ATS文件最大个数
    g_stLocalCfg.ulGetATSTime = 3;             // 获取ATS信息的周期（秒）

    // APP业务配置
    g_stLocalCfg.iTransferTrackReqCallMode = -1;   // 转换轨呼叫模式0:全呼 1：呼叫当前归属区 -1：呼叫待进入归属区
    g_stLocalCfg.ulMaxAppSelfCheckNum = 1;         // APP自检最大次数

    g_stLocalCfg.ulRadioResetButtonCheckTime = 5;   // 车载台主机监测重启按钮时间，单位秒，0表示不检测
    g_stLocalCfg.ulRadioResetAPPCheckTime = 0;      // 车载台主机ping终端失败达到本次数后重启，0表示不检测（目前SA未做处理）
    g_stLocalCfg.bAPPMsgUseAnsiString = FALSE;      // APP消息是否采用ANSI编码格式

    g_stLocalCfg.ulDCMinTmpPatchGroupNum = 1;       // 每个调度台自动创建的临时派接组个数
    g_stLocalCfg.ulDCUserIDForDepotIPH = INVALID_USER_ID;

    // GPS
    g_stLocalCfg.usGPSRecvPort = DEFAULT_GPS_UDP_PORT;

    // 系统管理
    g_stLocalCfg.ulClearDataHour = 2;          // 数据清理时间
    g_stLocalCfg.ulResvDataDays = 366;         // 数据(库）保留天数
    g_stLocalCfg.ulBackupFileNum = 366;        // 数据备份文件个数

    g_stLocalCfg.iAutoCloseHour = -1;          // dis自动重启整点时刻，-1表示不自动重启

    // 集群配置
    g_stLocalCfg.ulClusterDefaultStatus = CLUSTER_STATUS_STAND_ALONE;

    // OCC、NOCC接口配置
    g_stLocalCfg.ulLineID = INVALID_LINE_ID;
    g_stLocalCfg.usOCCPort = DEFAULT_OCC_UDP_PORT;

    CHAR        acFile[512];
    CHAR        *szValue = NULL;
    UINT16      usDefaultATSPort = ATS_SERVER_PORT;

    sprintf(acFile, "%s/%s", gos_get_root_path(), g_szConfigFile);

    if (!gos_file_exist(acFile))
    {
        GosLog(LOG_ERROR, "file %s does not exist", acFile);
        return FALSE;
    }

    GConfFile   Conf(acFile);

    GosLog(LOG_INFO, "read local cfg from %s", g_szConfigFile);

    // 地址配置
    // mdc
    szValue = Conf.Get("mdc_addr");
    if (!szValue)
    {
        GosLog(LOG_ERROR, "no mdc_addr in %s", g_szConfigFile);
        return FALSE;
    }

    if (!gos_get_ip(szValue, g_stLocalCfg.aucMdcAddr))
    {
        GosLog(LOG_ERROR, "invalid mdc_addr in %s", g_szConfigFile);
        return FALSE;
    }

    GosLog(LOG_INFO, "mdc addr is "IP_FMT, IP_ARG(g_stLocalCfg.aucMdcAddr));

    // omc
    szValue = Conf.Get("omc_addr");
    if (!szValue)
    {
        GosLog(LOG_ERROR, "no omc_addr in %s", g_szConfigFile);
        memset(g_stLocalCfg.aucOmcAddr, 0, sizeof(g_stLocalCfg.aucOmcAddr));
    }
    else if (!gos_get_ip(szValue, g_stLocalCfg.aucOmcAddr))
    {
        GosLog(LOG_ERROR, "invalid omc_addr in %s", g_szConfigFile);
        return FALSE;
    }

    GosLog(LOG_INFO, "omc addr is "IP_FMT, IP_ARG(g_stLocalCfg.aucOmcAddr));

    // ATS配置
    GetBoolCfg(Conf, "auto_close_ats", g_stLocalCfg.bAutoCloseATS, FALSE);

    szValue = Conf.Get("ats_addr1");
    if (!szValue)
    {
        GosLog(LOG_ERROR, "no ats_addr1 in %s", g_szConfigFile);
        return FALSE;
    }

    g_stLocalCfg.astATSAddr[0].usPort = usDefaultATSPort;
    if (!gos_get_ip_port(szValue, g_stLocalCfg.astATSAddr[0].aucIP, &g_stLocalCfg.astATSAddr[0].usPort))
    {
        GosLog(LOG_ERROR, "invalid ats_addr1 in %s", g_szConfigFile);
        return FALSE;
    }

    if (0 == g_stLocalCfg.astATSAddr[0].usPort)
    {
        g_stLocalCfg.astATSAddr[0].usPort = usDefaultATSPort;
    }

    GosLog(LOG_INFO, "ats_addr1 is "IP_FMT ":%u", IP_ARG(g_stLocalCfg.astATSAddr[0].aucIP), g_stLocalCfg.astATSAddr[0].usPort);

    szValue = Conf.Get("ats_addr2");
    if (!szValue)
    {
        memset(&g_stLocalCfg.astATSAddr[1], 0, sizeof(g_stLocalCfg.astATSAddr[1]));
    }
    else
    {
        g_stLocalCfg.astATSAddr[1].usPort = usDefaultATSPort;
        if (!gos_get_ip_port(szValue, g_stLocalCfg.astATSAddr[1].aucIP, &g_stLocalCfg.astATSAddr[1].usPort))
        {
            GosLog(LOG_ERROR, "invalid ats_addr2 in %s", g_szConfigFile);
            return FALSE;
        }

        if (0 == g_stLocalCfg.astATSAddr[1].usPort)
        {
            g_stLocalCfg.astATSAddr[1].usPort = usDefaultATSPort;
        }

        GosLog(LOG_INFO, "ats_addr2 is "IP_FMT ":%u", IP_ARG(g_stLocalCfg.astATSAddr[1].aucIP), g_stLocalCfg.astATSAddr[1].usPort);
    }

    if (memcmp(&g_stLocalCfg.astATSAddr[0], &g_stLocalCfg.astATSAddr[1], sizeof(g_stLocalCfg.astATSAddr[0])) == 0)
    {
        memset(&g_stLocalCfg.astATSAddr[1], 0, sizeof(g_stLocalCfg.astATSAddr[0]));
    }

    szValue = Conf.Get("save_ats");
    if (szValue)
    {
        if (strcmp(szValue, "0") == 0 ||
            strcasecmp(szValue, "false") == 0)
        {
            g_stLocalCfg.bSaveATSToFile = FALSE;
        }
    }

    GosLog(LOG_INFO, "save_ats is %u", g_stLocalCfg.bSaveATSToFile);

    GetIntCfg(Conf, "max_ats_file_num", g_stLocalCfg.ulMaxATSFileNum, 366, 30, 3*366);
    /*
    szValue = Conf.Get("max_ats_file_num");
    if (szValue)
    {
        gos_atou(szValue, &g_stLocalCfg.ulMaxATSFileNum);
    }
    else
    {
        g_stLocalCfg.ulMaxATSFileNum = g_stLocalCfg.ulBackupFileNum;
    }

    if (g_stLocalCfg.ulMaxATSFileNum < 30)
    {
        g_stLocalCfg.ulMaxATSFileNum = 30;
    }
    else if (g_stLocalCfg.ulMaxATSFileNum > 3*366)
    {
        g_stLocalCfg.ulMaxATSFileNum = 3*366;
    }

    GosLog(LOG_INFO, "max_ats_file_num is %u", g_stLocalCfg.ulMaxATSFileNum); */

#if 0 // 14号线ATS不需要本配置项
    szValue = Conf.Get("get_ats_period");
    if (szValue)
    {
        gos_atou(szValue, &g_stLocalCfg.ulGetATSTime);
        if (g_stLocalCfg.ulGetATSTime < 1)
        {
            g_stLocalCfg.ulGetATSTime = 1;
        }
        else if (g_stLocalCfg.ulGetATSTime > 10)
        {
            g_stLocalCfg.ulGetATSTime = 10;
        }
    }

    GosLog(LOG_INFO, "get_ats_period is %u", g_stLocalCfg.ulGetATSTime);
#endif
    GetIntCfg(Conf, "ats_valid_period", g_stLocalCfg.ulATSValidPeriod, 10, 5);

    // APP业务配置
    GetBoolCfg(Conf, "allow_emergency_call", g_stLocalCfg.bAllowEmergencyCall, FALSE);

    GetBoolCfg(Conf, "allow_tx_p2p_call", g_stLocalCfg.bAllowTXP2PCall, FALSE);
    GetBoolCfg(Conf, "allow_fx_p2p_call", g_stLocalCfg.bAllowFXP2PCall, TRUE);

    GetBoolCfg(Conf, "direct_send_app", g_stLocalCfg.bDirectSendApp, FALSE);

  //  GetBoolCfg(Conf, "app_use_ansi_string", g_stLocalCfg.bAPPMsgUseAnsiString, FALSE);

    g_stLocalCfg.iTransferTrackReqCallMode = -1;
    if (!Conf.Get("transfer_track_reqcall_mode", g_stLocalCfg.iTransferTrackReqCallMode, &g_stLocalCfg.iTransferTrackReqCallMode))
    {
        GosLog(LOG_ERROR, "invalid transfer_track_reqcall_mode in %s", g_szConfigFile);
        return FALSE;
    }

    GosLog(LOG_INFO, "transfer_track_reqcall_mode is %d", g_stLocalCfg.iTransferTrackReqCallMode);

    GetIntCfg(Conf, "app_self_check_num", g_stLocalCfg.ulMaxAppSelfCheckNum, 5, 1);

 /*   g_stLocalCfg.ulMaxAppSelfCheckNum = 1;
    Conf.Get("app_self_check_num", 1, &g_stLocalCfg.ulMaxAppSelfCheckNum);
    if (g_stLocalCfg.ulMaxAppSelfCheckNum == 0)
    {
        g_stLocalCfg.ulMaxAppSelfCheckNum = 1;
    }

    GosLog(LOG_INFO, "app_self_check_num is %u", g_stLocalCfg.ulMaxAppSelfCheckNum); */

    if (!Conf.Get("radio_reset_btn_check_time", g_stLocalCfg.ulRadioResetButtonCheckTime, &g_stLocalCfg.ulRadioResetButtonCheckTime))
    {
        GosLog(LOG_ERROR, "invalid radio_reset_btn_check_time in %s", g_szConfigFile);
        return FALSE;
    }

    if (g_stLocalCfg.ulRadioResetButtonCheckTime > 0 &&
        g_stLocalCfg.ulRadioResetButtonCheckTime < 5 )
    {
        g_stLocalCfg.ulRadioResetButtonCheckTime = 5;
    }

    GosLog(LOG_INFO, "radio_reset_btn_check_time is %u", g_stLocalCfg.ulRadioResetButtonCheckTime);

/*    if (!Conf.Get("radio_reset_app_check_time", g_stLocalCfg.ulRadioResetAPPCheckTime, &g_stLocalCfg.ulRadioResetAPPCheckTime))
    {
        GosLog(LOG_ERROR, "invalid radio_reset_app_check_time in %s", g_szConfFile);
        return FALSE;
    }*/

    GosLog(LOG_INFO, "radio_reset_app_check_time is %u", g_stLocalCfg.ulRadioResetAPPCheckTime);

    GetBoolCfg(Conf, "iph_video_proxy_mode", g_stLocalCfg.bIPHVideoProxyMode, FALSE);

    GetIntCfg(Conf, "iph_video_open_timeout", g_stLocalCfg.ulIPHVideoOpenTimeout, 1, 1, 30);

    GetIntCfg(Conf, "send_self_check_msg_period", g_stLocalCfg.ulSendSelfCheckMsgPeriod, 180, 180);

    GetIntCfg(Conf, "tx_data_capture_mode", g_stLocalCfg.ulTXDataCaptureMode, 1);
/*
    g_stLocalCfg.ulSendSelfCheckMsgPeriod = 180;
    if (!Conf.Get("send_self_check_msg_period", g_stLocalCfg.ulSendSelfCheckMsgPeriod, &g_stLocalCfg.ulSendSelfCheckMsgPeriod))
    {
        GosLog(LOG_ERROR, "invalid send_self_check_msg_period in %s", g_szConfigFile);
        return FALSE;
    }

    if (g_stLocalCfg.ulSendSelfCheckMsgPeriod > 0 &&
        g_stLocalCfg.ulSendSelfCheckMsgPeriod < 180 )
    {
        g_stLocalCfg.ulSendSelfCheckMsgPeriod = 180;
    }

    if (0 == g_stLocalCfg.ulSendSelfCheckMsgPeriod)
    {
        g_stLocalCfg.ulSendSelfCheckMsgPeriod = 0xffffffff;
    }

    GosLog(LOG_INFO, "send_self_check_msg_period is %u", g_stLocalCfg.ulSendSelfCheckMsgPeriod); */

    GetIntCfg(Conf, "tx_auto_lock_timeout", g_stLocalCfg.ulTXAutoLockTimeout, 120, 30, 3600);
/*
    g_stLocalCfg.ulTXAutoLockTimeout = 120;
    if (!Conf.Get("tx_auto_lock_timeout", g_stLocalCfg.ulTXAutoLockTimeout, &g_stLocalCfg.ulTXAutoLockTimeout))
    {
        GosLog(LOG_ERROR, "invalid tx_auto_lock_timeout in %s", g_szConfigFile);
        return FALSE;
    }

    if (g_stLocalCfg.ulTXAutoLockTimeout < 30 )
    {
        g_stLocalCfg.ulTXAutoLockTimeout = 30;
    }
    else if (g_stLocalCfg.ulTXAutoLockTimeout > 3600)
    {
        g_stLocalCfg.ulTXAutoLockTimeout = 3600;
    }

    GosLog(LOG_INFO, "tx_auto_lock_timeout is %u", g_stLocalCfg.ulTXAutoLockTimeout); */

    g_stLocalCfg.acTXLockPwd[0] = '\0';
    szValue = Conf.Get("tx_lock_pwd");
    if (szValue)
    {
        if (strlen(szValue) >= sizeof(g_stLocalCfg.acTXLockPwd))
        {
            GosLog(LOG_ERROR, "tx_lock_pwd(%s) is too long", szValue);
            return FALSE;
        }

        strcpy(g_stLocalCfg.acTXLockPwd, szValue);
    }

    std::string strPwd = gos::base64::EncodeString(g_stLocalCfg.acTXLockPwd);

    GosLog(LOG_INFO, "tx_lock_pwd is %s", strPwd.c_str());

    // DC业务配置
    GetBoolCfg(Conf, "allow_kickoff_dc", g_stLocalCfg.bAllowKickOffDC, TRUE);
    GetBoolCfg(Conf, "allow_dc_reload_cfg", g_stLocalCfg.bAllowDCReloadCfg, TRUE);
    GetBoolCfg(Conf, "allow_half_duplex_p2pcall", g_stLocalCfg.bAllowHalfDuplexP2PCall, FALSE);

    GetBoolCfg(Conf, "manage_one_depot", g_stLocalCfg.bManageOneDepot, FALSE);
    GetBoolCfg(Conf, "allow_video_wall", g_stLocalCfg.bAllowVideoWall, FALSE);
    GetBoolCfg(Conf, "allow_gis_sub", g_stLocalCfg.bAllowGISSub, FALSE);
    GetBoolCfg(Conf, "allow_p2p_video_call", g_stLocalCfg.bAllowP2PVideoCall, TRUE);
    GetBoolCfg(Conf, "auto_close_multicast_call", g_stLocalCfg.bAutoCloseMulticastCall, TRUE);
    GetBoolCfg(Conf, "send_sms_by_realmsg", g_stLocalCfg.bSendSMSByRealMsg, TRUE);  // 14号线用realMsg，机场线用彩信
    GetBoolCfg(Conf, "simple_sms_mode", g_stLocalCfg.bSimpleSMSMode, TRUE);      // 14号线采用简单短信模式

    GetIntCfg(Conf, "min_tmp_patch_group_num", g_stLocalCfg.ulDCMinTmpPatchGroupNum, 1, 1, 8);

    // 发起车辆段IPH时，接听者调度台配置，如果配置则发送给该指定的调度台
    g_stLocalCfg.ulDCUserIDForDepotIPH = INVALID_USER_ID;
    if (!Conf.Get("dc_for_depot_iph", g_stLocalCfg.ulDCUserIDForDepotIPH, &g_stLocalCfg.ulDCUserIDForDepotIPH))
    {
        GosLog(LOG_ERROR, "invalid dc_for_depot_iph in %s", g_szConfigFile);
        return FALSE;
    }

    GosLog(LOG_INFO, "dc_for_depot_iph is %u", g_stLocalCfg.ulDCUserIDForDepotIPH);

    GetBoolCfg(Conf, "iph_video_tcp_mode", g_stLocalCfg.bIPHVideoTcpMode, FALSE);
    GetBoolCfg(Conf, "iph_rec_merge_mode", g_stLocalCfg.bIPHRecMergeMode, FALSE);
    GetBoolCfg(Conf, "direct_send_iph_audio", g_stLocalCfg.bDirectSendIPHAudio, TRUE);

    GetIntCfg(Conf, "iph_audio_sample_rate", g_stLocalCfg.ulIPHAudioSampleRate, 16000, 8000, 32000);

    /*
    g_stLocalCfg.ulIPHAudioSampleRate = 16000;
    if (!Conf.Get("iph_audio_sample_rate", g_stLocalCfg.ulIPHAudioSampleRate, &g_stLocalCfg.ulIPHAudioSampleRate))
    {
        GosLog(LOG_ERROR, "invalid iph_audio_sample_rate in %s", g_szConfigFile);
        return FALSE;
    }

    if (g_stLocalCfg.ulIPHAudioSampleRate < 8000 )
    {
        g_stLocalCfg.ulIPHAudioSampleRate = 8000;
    }
    else if (g_stLocalCfg.ulIPHAudioSampleRate > 32000 )
    {
        g_stLocalCfg.ulIPHAudioSampleRate = 32000;
    }

    GosLog(LOG_INFO, "iph_audio_sample_rate is %u", g_stLocalCfg.ulIPHAudioSampleRate); */

    GetIntCfg(Conf, "iph_audio_bits_per_sample", g_stLocalCfg.ulIPHAudioBitsPerSample, 16, 8, 16);
    /*
    g_stLocalCfg.ulIPHAudioBitsPerSample = 16;
    if (!Conf.Get("iph_audio_bits_per_sample", g_stLocalCfg.ulIPHAudioBitsPerSample, &g_stLocalCfg.ulIPHAudioBitsPerSample))
    {
        GosLog(LOG_ERROR, "invalid iph_audio_bits_per_sample in %s", g_szConfigFile);
        return FALSE;
    }

    if (g_stLocalCfg.ulIPHAudioBitsPerSample < 8 )
    {
        g_stLocalCfg.ulIPHAudioBitsPerSample = 8;
    }
    else if (g_stLocalCfg.ulIPHAudioBitsPerSample > 16 )
    {
        g_stLocalCfg.ulIPHAudioBitsPerSample = 16;
    }

    GosLog(LOG_INFO, "iph_audio_bits_per_sample is %u", g_stLocalCfg.ulIPHAudioBitsPerSample); */

    g_stLocalCfg.bDirectSendPAAudio = FALSE;  //  目前不支持调度台直接发送音频给PA
  //  GetBoolCfg(Conf, "direct_send_pa_audio", g_stLocalCfg.bDirectSendPAAudio, FALSE);

    GetIntCfg(Conf, "pa_audio_sample_rate", g_stLocalCfg.ulPAAudioSampleRate, 16000, 8000, 16000);

    GetIntCfg(Conf, "pa_audio_bits_per_sample", g_stLocalCfg.ulPAAudioBitsPerSample, 16, 8, 16);

    // 目前不支持
    g_stLocalCfg.bPAAECMode = FALSE;
   // GetBoolCfg(Conf, "allow_pa_aec_mode", g_stLocalCfg.bPAAECMode, FALSE);

    GetIntCfg(Conf, "pa_max_pkg_size", g_stLocalCfg.ulPAMaxPkgSize, 1000, 320, 2000);

    GetBoolCfg(Conf, "allow_iph_aec_mode", g_stLocalCfg.bIPHAECMode, FALSE);

    GetIntCfg(Conf, "iph_max_pkg_size", g_stLocalCfg.ulIPHMaxPkgSize, 1000, 320, 2000);


    GetIntCfg(Conf, "max_iph_talk_time", g_stLocalCfg.ulMaxIPHTalkTime, 60*30, 30, 86400);
    GetIntCfg(Conf, "max_iph_wait_accept_time", g_stLocalCfg.ulMAXIPHWaitAcceptTime, 60*5, 60, 86400);

    // GPS
#ifdef HAS_TX_GPS
    g_stLocalCfg.usGPSRecvPort = DEFAULT_GPS_UDP_PORT;
    if (!Conf.Get("gps_udp_port", g_stLocalCfg.usGPSRecvPort, &g_stLocalCfg.usGPSRecvPort) ||
        g_stLocalCfg.usGPSRecvPort == 0)
    {
        GosLog(LOG_ERROR, "invalid gps_udp_port in %s", g_szConfigFile);
        return FALSE;
    }

    GosLog(LOG_INFO, "gps_udp_port is %u", g_stLocalCfg.usGPSRecvPort);
#endif

    // 系统管理
    szValue = Conf.Get("clear_data_hour");
    if (szValue)
    {
        gos_atou(szValue, &g_stLocalCfg.ulClearDataHour);
        if (g_stLocalCfg.ulClearDataHour >= 24)
        {
            g_stLocalCfg.ulClearDataHour = 2;
        }
    }

    GosLog(LOG_INFO, "clear_data_hour is %u", g_stLocalCfg.ulClearDataHour);

    GetIntCfg(Conf, "data_resv_day", g_stLocalCfg.ulResvDataDays, 366, 30, 3*366);

    GetIntCfg(Conf, "backup_file_num", g_stLocalCfg.ulBackupFileNum, 366, 30, 3*366);

    g_stLocalCfg.iAutoCloseHour = -1;
    szValue = Conf.Get("auto_close_hour");
    if (szValue)
    {
        if (!gos_atoi(szValue, &g_stLocalCfg.iAutoCloseHour))
        {
            g_stLocalCfg.iAutoCloseHour = -1;
        }
        else if (g_stLocalCfg.iAutoCloseHour >= 24)
        {
            g_stLocalCfg.iAutoCloseHour = -1;
        }
    }

    GosLog(LOG_INFO, "auto_close_hour is %d", g_stLocalCfg.iAutoCloseHour);

    g_stLocalCfg.usNOCCPort = DEFAULT_NOCC_UDP_PORT;
    szValue = Conf.Get("nocc_addr");
    if (szValue)
    {
        if (!gos_get_ip_port(szValue, g_stLocalCfg.aucNOCCAddr, &g_stLocalCfg.usNOCCPort))
        {
            GosLog(LOG_ERROR, "invalid nocc_addr in %s", g_szConfigFile);
            return FALSE;
        }

        if (g_stLocalCfg.usNOCCPort == 0)
        {
            g_stLocalCfg.usNOCCPort = DEFAULT_NOCC_UDP_PORT;
        }
    }

    if (GET_INT(g_stLocalCfg.aucNOCCAddr) != 0)
    {
        GosLog(LOG_INFO, "nocc_addr is "IP_FMT ":%u", IP_ARG(g_stLocalCfg.aucNOCCAddr), g_stLocalCfg.usNOCCPort);

        g_stLocalCfg.ulLineID = METRO_LINE_ID;  // 每个线路取值不一样
        if (Conf.Get("line_id") &&
            !Conf.Get("line_id", g_stLocalCfg.ulLineID, &g_stLocalCfg.ulLineID) )
        {
            GosLog(LOG_ERROR, "invalid line_id in %s", g_szConfigFile);
            return FALSE;
        }

        if (g_stLocalCfg.ulLineID == 0 ||
            g_stLocalCfg.ulLineID == INVALID_LINE_ID)
        {
            GosLog(LOG_ERROR, "invalid line_id in %s", g_szConfigFile);
            return FALSE;
        }

        GosLog(LOG_INFO, "line_id is %d", g_stLocalCfg.ulLineID);

        g_stLocalCfg.usOCCPort = DEFAULT_OCC_UDP_PORT;
        if (!Conf.Get("occ_port", g_stLocalCfg.usOCCPort, &g_stLocalCfg.usOCCPort) )
        {
            GosLog(LOG_ERROR, "invalid occ_port in %s", g_szConfigFile);
            return FALSE;
        }

        if (g_stLocalCfg.usOCCPort == 0)
        {
            g_stLocalCfg.usOCCPort = DEFAULT_OCC_UDP_PORT;
        }

        GosLog(LOG_INFO, "occ_port is %d", g_stLocalCfg.usOCCPort);
    }

    // 集群配置
    SET_IP(g_stLocalCfg.aucClusterMasterAddr, 127, 0, 0, 1);
    if (!Conf.Get("Cluster", "default_status", CLUSTER_STATUS_STAND_ALONE, &g_stLocalCfg.ulClusterDefaultStatus))
    {
        GosLog(LOG_ERROR, "invalid default_status in [Cluster] of %s", g_szConfigFile);
        return FALSE;
    }

    GosLog(LOG_INFO, "default_status of [Cluster] is %u", g_stLocalCfg.ulClusterDefaultStatus);

    if (g_stLocalCfg.ulClusterDefaultStatus != CLUSTER_STATUS_STAND_ALONE)
    {
        szValue = Conf.Get("Cluster", "peer_addr");
        if (szValue)
        {
            if (!gos_get_ip(szValue, g_stLocalCfg.aucClusterPeerAddr))
            {
                GosLog(LOG_ERROR, "invalid peer_addr in [Cluster] of %s", g_szConfigFile);
                return FALSE;
            }
        }

        GosLog(LOG_INFO, "peer_addr of [Cluster] is "IP_FMT, IP_ARG(g_stLocalCfg.aucClusterPeerAddr));

        szValue = Conf.Get("Cluster", "master_addr");
        if (!szValue)
        {
            GosLog(LOG_ERROR, "no master_addr in [Cluster] of %s", g_szConfigFile);
            return FALSE;
        }

        if (!gos_get_ip(szValue, g_stLocalCfg.aucClusterMasterAddr))
        {
            GosLog(LOG_ERROR, "invalid master_addr in [Cluster] of %s", g_szConfigFile);
            return FALSE;
        }

        GosLog(LOG_INFO, "master_addr of [Cluster] is "IP_FMT, IP_ARG(g_stLocalCfg.aucClusterMasterAddr));
    }

    szValue = Conf.Get("status_file");
    if (szValue)
    {
        if (strlen(szValue) >= sizeof(g_stLocalCfg.acStatusFile))
        {
            GosLog(LOG_INFO, "status_file is too long");
            return FALSE;
        }

        strcpy(g_stLocalCfg.acStatusFile, szValue);

        GosLog(LOG_INFO, "status_file is %s", g_stLocalCfg.acStatusFile);
    }

    GetIntCfg(Conf, "refresh_status_file_period", g_stLocalCfg.ulRefreshStatusFilePeriod, 2, 1, 86400);

    return TRUE;
}

static int CompareFile(const void *t1,const void *t2)
{
    CHAR    **s = (CHAR**)t1;
    CHAR    **d = (CHAR**)t2;

    return strcmp(*s, *d);
}

BOOL IsATSValid()
{
    return g_bATSValid;
}

VOID SetATSValid(BOOL bValid)
{
    g_bATSValid = bValid;

    //写status_file
    if (g_stLocalCfg.acStatusFile[0])
    {
        static UINT32   ulLastTime = 0;
        UINT32          ulTime = gos_get_uptime_1ms();
        CHAR            acFile[256];
        FILE            *fp = NULL;

        sprintf(acFile, "%s/%s", gos_get_root_path(), g_stLocalCfg.acStatusFile);

        if (bValid)
        {
            if (gos_get_file_size(acFile) != 0) // 非0（文件不存在或非空）需要创建或清空文件
            {
                fp = fopen(acFile, "wb");
                if (!fp)
                {
                    GosLog(LOG_ERROR, "open file %s failed", acFile);
                    return;
                }

                fclose(fp);

                GosLog(LOG_FATAL, "init file %s", acFile);
            }
        }
        else if ((UINT32)(ulTime - ulLastTime) >= g_stLocalCfg.ulRefreshStatusFilePeriod*1000 )
        {
            fp = fopen(acFile, "wt");
            if (!fp)
            {
                GosLog(LOG_ERROR, "open file %s failed", acFile);
                return;
            }

            fprintf(fp, "ATS error\n");
            fclose(fp);

            GosLog(LOG_FATAL, "write ATS error to file %s", acFile);

            ulLastTime = ulTime;
        }
    }
}

VOID RemoveFile(CHAR *szDir, CHAR *szFilePrefix, UINT32 ulMaxFileNum)
{
    BOOL        bIsDir;
    CHAR        acFile[260];
    CHAR        *szFile;
    CHAR        acFilePrefix[260];
    HANDLE      hDir = gos_open_dir(szDir);
    VECTOR<CHAR*>   vFile;

    if (!hDir)
    {
        return;
    }

    sprintf(acFilePrefix, "%s/%s", szDir, szFilePrefix);
    gos_format_path(acFilePrefix);

    while(gos_get_next_file(hDir, acFile, &bIsDir))
    {
        if (!bIsDir)
        {
            gos_format_path(acFile);
            if (gos_str_like(acFile, acFilePrefix))
            {
                vFile.push_back(strdup(acFile));
            }
        }
    }

    gos_close_dir(hDir);

    UINT32  i;
    UINT32  ulFileNum = vFile.size();

    if (ulFileNum <= ulMaxFileNum)
    {
        for (UINT32 i=0; i<vFile.size(); ++i)
        {
            free(vFile.at(i));
        }

        return;
    }

    CHAR    **aszFile = (CHAR**)calloc(sizeof(CHAR*), ulFileNum);

    for (i=0; i<ulFileNum; i++)
    {
        aszFile[i] = vFile[i];
    }

    qsort(aszFile, ulFileNum, sizeof(CHAR*), CompareFile);

    for (i=0; i<ulFileNum; i++)
    {
        szFile = aszFile[i];
        if (i < (ulFileNum - ulMaxFileNum))
        {
            gos_delete_file(szFile);
            GosLog(LOG_INFO, "remove file %s", szFile);
        }

        free(szFile);
    }

    free(aszFile);
}

BOOL CharArrayToInt(CHAR *szText, UINT32 ulLen, UINT32 *pulValue)
{
    CHAR    acText[32];

    if (!pulValue)
    {
        return FALSE;
    }

    *pulValue = 0;
    if (!szText)
    {
        return FALSE;
    }

    if (ulLen >= sizeof(acText))
    {
        return FALSE;
    }

    memset(acText, 0, sizeof(acText));
    memcpy(acText, szText, ulLen);

    return gos_atou(acText, pulValue);
}

BOOL CharArrayToInt(CHAR *szText, UINT32 ulLen, UINT16 *pusValue)
{
    UINT32      ulValue = 0;

    if (!pusValue)
    {
        return FALSE;
    }

    *pusValue = 0;

    if (!CharArrayToInt(szText, ulLen, &ulValue) ||
        ulValue > 0xffff)
    {
        return FALSE;
    }

    *pusValue = (UINT16)ulValue;
    return TRUE;
}

//=============== group cfg  ========================
VOID SetGroupCfg(GROUP_CFG_T *pstCfg)
{
    AUTO_LOCK(g_CfgMutex);

    memcpy(&g_stGroupCfg, pstCfg, sizeof(g_stGroupCfg));
}

BOOL GetGroupCfg(GROUP_CFG_T *pstCfg)
{
    AUTO_LOCK(g_CfgMutex);

    if (g_stGroupCfg.i64UpLineTrainGroupID == INVALID_GROUP_ID)
    {
        return FALSE;
    }

    memcpy(pstCfg, &g_stGroupCfg, sizeof(g_stGroupCfg));
    return TRUE;
}

//=============== logined DC user ========================
VOID SetDCLogin(UINT32 ulDCUserID, PID_T *pstPID)
{
    g_DCLoginMutex.Mutex();
    g_mLoginedDCUser.Add(ulDCUserID, pstPID);
    g_DCLoginMutex.Unmutex();
}

VOID SetDCLogout(UINT32 ulDCUserID)
{
    g_DCLoginMutex.Mutex();
    g_mLoginedDCUser.Del(ulDCUserID);
    g_DCLoginMutex.Unmutex();
}

BOOL GetDCPID(UINT32 ulDCUserID, PID_T *pstPID)
{
    pstPID->usInstID = INVALID_INST_ID;
    pstPID->usTaskID = INVALID_TASK_ID;

    g_DCLoginMutex.Mutex();

    PID_T   *pstTmp = g_mLoginedDCUser.GetValue(ulDCUserID);

    if (pstTmp)
    {
        memcpy(pstPID, pstTmp, sizeof(PID_T));
    }

    g_DCLoginMutex.Unmutex();

    return (pstTmp != NULL);
}

BOOL IsDCLogined(UINT32 ulDCUserID)
{
    g_DCLoginMutex.Mutex();

    BOOL bHas = g_mLoginedDCUser.Has(ulDCUserID);

    g_DCLoginMutex.Unmutex();

    return bHas;
}

VOID GetLoginedDCUser(VECTOR<UINT32> &vUser)
{
    g_DCLoginMutex.Mutex();

    g_mLoginedDCUser.GetKeyList(vUser);

    g_DCLoginMutex.Unmutex();
}

//=============== ATS ========================
VOID GetTrainPosition(TRAIN_INFO_T *pstATSInfo, UINT32 *pulDepotID, CHAR *szPositionName)
{
    STATION_INFO_T  stStationInfo = {0};
    DEPOT_INFO_T    stDepotInfo;

    *pulDepotID = DEPOT_ID_MAINLINE;
    strcpy(szPositionName, "");

    if (GetDepotInfo(pstATSInfo->ulRegisterDepotID, stDepotInfo))
    {
        *pulDepotID = pstATSInfo->ulRegisterDepotID;
        strcpy(szPositionName, stDepotInfo.acDepotName);
        return;
    }

    if (!GetStationInfo(pstATSInfo->ulStationID, &stStationInfo))
    {
        return;
    }

    if (GetDepotInfo(stStationInfo.ulDepotID, stDepotInfo))
    {
        *pulDepotID = stStationInfo.ulDepotID;
        strcpy(szPositionName, stDepotInfo.acDepotName);
        return;
    }
}

BOOL GetTrainRegisterDepotID(UINT32 ulTrainUnitID, UINT32 *pulDepotID)
{
    TRAIN_INFO_T    stATSInfo;
    STATION_INFO_T  stStationInfo;

    if (!GetTrainInfo(ulTrainUnitID, stATSInfo))
    {
        return FALSE;
    }

    if (!GetStationInfo(stATSInfo.ulStationID, &stStationInfo))
    {
        return FALSE;
    }

    *pulDepotID = stStationInfo.ulDepotID;

    return TRUE;
}

//=============== depot ========================
VOID InitDepotInfo(VECTOR<DEPOT_INFO_T> &vDepotInfo)
{
    AUTO_LOCK(g_StationMutex);

    g_vDepotInfo.clear();
    g_vDepotInfo.assign(vDepotInfo.begin(), vDepotInfo.end());

}

VOID GetDepotInfo(VECTOR<DEPOT_INFO_T> &vDepotInfo)
{
    AUTO_LOCK(g_StationMutex);

    vDepotInfo.clear();
    vDepotInfo.assign(g_vDepotInfo.begin(), g_vDepotInfo.end());

}

BOOL GetDepotInfo(UINT32 ulDepotID, DEPOT_INFO_T &stDepotInfo)
{
    AUTO_LOCK(g_StationMutex);

    for (UINT32 i=0; i<g_vDepotInfo.size(); i++)
    {
        if (g_vDepotInfo[i].ulDepotID == ulDepotID)
        {
            memcpy(&stDepotInfo, &g_vDepotInfo[i], sizeof(stDepotInfo));
            return TRUE;
        }
    }

    return FALSE;
}

VOID AddDepotInfo(DEPOT_INFO_T &stDepotInfo)
{
    AUTO_LOCK(g_StationMutex);

    for (UINT32 i=0; i<g_vDepotInfo.size(); i++)
    {
        if (g_vDepotInfo[i].ulDepotID == stDepotInfo.ulDepotID)
        {
            memcpy(&g_vDepotInfo[i], &stDepotInfo, sizeof(stDepotInfo));
            return;
        }
    }

    g_vDepotInfo.push_back(stDepotInfo);
}

VOID DelDepotInfo(UINT32 ulDepotID)
{
    AUTO_LOCK(g_StationMutex);

    for (UINT32 i=0; i<g_vDepotInfo.size(); i++)
    {
        if (g_vDepotInfo[i].ulDepotID == ulDepotID)
        {
            g_vDepotInfo.erase(g_vDepotInfo.begin()+i);
            return;
        }
    }
}

BOOL SetDepotInfo(DEPOT_INFO_T &stDepotInfo)
{
    AUTO_LOCK(g_StationMutex);

    for (UINT32 i=0; i<g_vDepotInfo.size(); i++)
    {
        if (g_vDepotInfo[i].ulDepotID == stDepotInfo.ulDepotID)
        {
            memcpy(&g_vDepotInfo[i], &stDepotInfo, sizeof(stDepotInfo));
            return TRUE;
        }
    }

    return FALSE;
}

//=============== station ========================
VOID InitDestInfo(VECTOR<DEST_INFO_T> &vInfo)
{
    AUTO_LOCK(g_DestInfoMutex);

    g_mDestInfo.Clear();

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        g_mDestInfo.Add(vInfo[i].ulDestID, vInfo[i]);
    }
}

VOID AddDestInfo(DEST_INFO_T &stInfo)
{
    AUTO_LOCK(g_DestInfoMutex);

    g_mDestInfo.Add(stInfo.ulDestID, stInfo);
}

VOID DelDestInfo(UINT32 ulDestID)
{
    AUTO_LOCK(g_DestInfoMutex);

    g_mDestInfo.Del(ulDestID);
}

VOID SetDestInfo(DEST_INFO_T &stInfo)
{
    AUTO_LOCK(g_DestInfoMutex);

    g_mDestInfo.Add(stInfo.ulDestID, stInfo);
}

BOOL GetDestInfo(UINT32 ulDestID, DEST_INFO_T *pstDestInfo)
{
    AUTO_LOCK(g_DestInfoMutex);

    DEST_INFO_T *pstInfo = g_mDestInfo.GetValue(ulDestID);

    if (!pstInfo)
    {
        return FALSE;
    }

    memcpy(pstDestInfo, pstInfo, sizeof(DEST_INFO_T));
    return TRUE;
}

BOOL GetDestInfo(CHAR *szDestID, DEST_INFO_T *pstInfo)
{
    UINT32  ulDestID;

    if (!gos_atou(szDestID, &ulDestID))
    {
        return FALSE;
    }

    return GetDestInfo(ulDestID, pstInfo);
}

VOID GetDestInfo(VECTOR<DEST_INFO_T> &vDestInfo)
{
    AUTO_LOCK(g_DestInfoMutex);

    VECTOR<DEST_INFO_T*> &vInfo = g_mDestInfo.GetValueList();

    vDestInfo.clear();
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        vDestInfo.push_back(*vInfo[i]);
    }
}

VOID InitStationInfo(VECTOR<STATION_INFO_T> &vStationInfo)
{
    g_StationMutex.Mutex();

    g_vStationInfo.clear();
    g_vStationInfo.assign(vStationInfo.begin(), vStationInfo.end());
    g_StationMutex.Unmutex();
}

VOID AddStationInfo(STATION_INFO_T &stInfo)
{
    g_StationMutex.Mutex();
    g_vStationInfo.push_back(stInfo);
    g_StationMutex.Unmutex();
}

VOID DelStationInfo(UINT32 ulStationID)
{
    g_StationMutex.Mutex();
    for (UINT32 i=0; i<g_vStationInfo.size(); i++)
    {
        if (g_vStationInfo[i].ulStationID == ulStationID)
        {
            g_vStationInfo.erase(g_vStationInfo.begin()+i);
            break;
        }
    }

    g_StationMutex.Unmutex();
}

VOID SetStationInfo(STATION_INFO_T &stInfo)
{
    g_StationMutex.Mutex();

    for (UINT32 i=0; i<g_vStationInfo.size(); i++)
    {
        if (g_vStationInfo[i].ulStationID == stInfo.ulStationID)
        {
            strcpy(g_vStationInfo[i].acStationName, stInfo.acStationName);
            g_vStationInfo[i].ulStationType = stInfo.ulStationType;
            g_vStationInfo[i].ulInterLockStationID = stInfo.ulInterLockStationID;
            g_vStationInfo[i].i64StationGroupID = stInfo.i64StationGroupID;
            g_StationMutex.Unmutex();
            return;
        }
    }

    g_vStationInfo.push_back(stInfo);
    g_StationMutex.Unmutex();
}

BOOL GetStationInfo(UINT32 ulStationID, STATION_INFO_T *pstStationInfo)
{
    g_StationMutex.Mutex();

    for (UINT32 i=0; i<g_vStationInfo.size(); i++)
    {
        if (ulStationID == g_vStationInfo[i].ulStationID)
        {
            memcpy(pstStationInfo, &g_vStationInfo[i], sizeof(STATION_INFO_T));
            g_StationMutex.Unmutex();
            return TRUE;
        }
    }

    g_StationMutex.Unmutex();
    return FALSE;
}

VOID GetStationInfo(VECTOR<STATION_INFO_T> &vStationInfo)
{
    g_StationMutex.Mutex();

    vStationInfo.clear();
    vStationInfo.assign(g_vStationInfo.begin(), g_vStationInfo.end());

    g_StationMutex.Unmutex();
}

VOID GetDepotStationInfo(VECTOR<STATION_INFO_T> &vStationInfo)
{
    g_StationMutex.Mutex();

    vStationInfo.clear();

    for (UINT32 i=0; i<g_vStationInfo.size(); i++)
    {
        if (g_vStationInfo[i].ulStationType != STATION_TYPE_NORMAL)
        {
            vStationInfo.push_back(g_vStationInfo[i]);
        }
    }

    g_StationMutex.Unmutex();
}

BOOL GetStationName(UINT32 ulStationID, CHAR *szStationName)
{
    STATION_INFO_T  stStationInfo = {0};

    *szStationName = '\0';
    if (!GetStationInfo(ulStationID, &stStationInfo))
    {
        return FALSE;
    }

    strcpy(szStationName, stStationInfo.acStationName);
    return TRUE;
}

//=============== section ========================
VOID InitSectionInfo(VECTOR<SECTION_INFO_T> &vInfo)
{
    AUTO_LOCK(g_SectionMutex);

    g_mSectionInfo.Clear();
    g_mUpsideSectionInfo.clear();
    g_mDownsideSectionInfo.clear();

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        SECTION_INFO_T  *pstInfo = g_mSectionInfo.Add(vInfo[i].ulSectionID, &vInfo[i]);

        if (pstInfo && pstInfo->bInPlatform)
        {
            if (pstInfo->ulDirection == TRAIN_DIRECTION_UPSIDE)
            {
                g_mUpsideSectionInfo[pstInfo->ulStationID] = pstInfo;
            }
            else if (pstInfo->ulDirection == TRAIN_DIRECTION_DOWNSIDE)
            {
                g_mDownsideSectionInfo[pstInfo->ulStationID] = pstInfo;
            }
        }
    }
}

VOID GetSectionInfo(BOOL bInDepot, VECTOR<SECTION_INFO_T> &vInfo)
{
    VECTOR<STATION_INFO_T>  vStationInfo;

    if (bInDepot)
    {
        GetDepotStationInfo(vStationInfo);
    }
    else
    {
        GetStationInfo(vStationInfo);
    }

    AUTO_LOCK(g_SectionMutex);

    VECTOR<SECTION_INFO_T *>    vValue;
    SECTION_INFO_T              *pstSection;
    UINT32                      i,j;

    g_mSectionInfo.GetValueList(vValue);

    vInfo.clear();
    for (i=0; i<vValue.size(); i++)
    {
        pstSection = vValue[i];
        for (j=0; j<vStationInfo.size(); j++)
        {
            if (vStationInfo[j].ulStationID == pstSection->ulStationID)
            {
                vInfo.push_back(*pstSection);
                break;
            }
        }
    }
}

BOOL GetSectionInfo(UINT32 ulSectionID, SECTION_INFO_T &stInfo)
{
    AUTO_LOCK(g_SectionMutex);

    SECTION_INFO_T *pstInfo = g_mSectionInfo.GetValue(ulSectionID);

    if (!pstInfo)
    {
        memset(&stInfo, 0, sizeof(stInfo));
        return FALSE;
    }

    memcpy(&stInfo, pstInfo, sizeof(stInfo));
    return TRUE;
}

// 某方向站台的section Index
UINT32 GetPlatformIndexInStation(UINT32 ulDirection, UINT32 ulStationID)
{
    AUTO_LOCK(g_SectionMutex);

    SECTION_INFO_T *pstInfo = NULL;

    if (ulDirection == TRAIN_DIRECTION_UPSIDE)
    {
        pstInfo = g_mUpsideSectionInfo[ulStationID];
    }
    else if (ulDirection == TRAIN_DIRECTION_DOWNSIDE)
    {
        pstInfo = g_mDownsideSectionInfo[ulStationID];
    }
    else
    {
        return 0;
    }

    if (!pstInfo)
    {
        return 0;
    }

    return pstInfo->iIndexInStation;
}

// 下行 02 04 06
// 上行 01 03 05
INT32 GetTrainOrder(TRAIN_INFO_T *pstATSInfo)
{
    SECTION_INFO_T  stSectionInfo;

    if (!GetSectionInfo(pstATSInfo->usSectionID, stSectionInfo))
    {
        if (pstATSInfo->usSectionID == 0)
        {
            GosLog(LOG_DETAIL, "unknown section %u", pstATSInfo->usSectionID);
        }
        else
        {
            GosLog(LOG_ERROR, "unknown section %u", pstATSInfo->usSectionID);
        }

        return 0;
    }

    UINT32      ulPlatformIndexInStation = GetPlatformIndexInStation(pstATSInfo->ulDirection, pstATSInfo->ulStationID);
    INT32       iTrainOrder = stSectionInfo.iIndexInStation - ulPlatformIndexInStation; // 扣除本车站列车方向的Platform所在的iIndexInStation

    if (pstATSInfo->ulDirection == TRAIN_DIRECTION_DOWNSIDE)
    {
        iTrainOrder = -iTrainOrder;
    }

    iTrainOrder = (iTrainOrder*256) + pstATSInfo->ucIndexInSection;

    return iTrainOrder;
}

//=============== brdzone ========================
VOID InitBrdZoneInfo(VECTOR<BRD_ZONE_INFO_T> &vInfo)
{
    AUTO_LOCK(g_BrdZoneMutex);

    g_vBrdZoneInfo.clear();
    g_vBrdZoneInfo.assign(vInfo.begin(), vInfo.end());
}

VOID AddBrdZoneInfo(BRD_ZONE_INFO_T &stInfo)
{
    AUTO_LOCK(g_BrdZoneMutex);
    g_vBrdZoneInfo.push_back(stInfo);
}

VOID DelBrdZoneInfo(UINT32 ulStationID, UINT32 ulZoneID)
{
    AUTO_LOCK(g_BrdZoneMutex);
    for (UINT32 i=0; i<g_vBrdZoneInfo.size(); i++)
    {
        if (g_vBrdZoneInfo[i].ulStationID == ulStationID &&
            g_vBrdZoneInfo[i].ulZoneID == ulZoneID )
        {
            g_vBrdZoneInfo.erase(g_vBrdZoneInfo.begin()+i);
            break;
        }
    }
}

VOID SetBrdZoneInfo(BRD_ZONE_INFO_T &stInfo)
{
    AUTO_LOCK(g_BrdZoneMutex);

    for (UINT32 i=0; i<g_vBrdZoneInfo.size(); i++)
    {
        if (g_vBrdZoneInfo[i].ulStationID == stInfo.ulStationID &&
            g_vBrdZoneInfo[i].ulZoneID    == stInfo.ulZoneID )
        {
            memcpy(&g_vBrdZoneInfo[i], &stInfo, sizeof(stInfo));
            return;
        }
    }

    g_vBrdZoneInfo.push_back(stInfo);
}

VOID GetBrdZoneInfo(UINT32 ulStationID, VECTOR<BRD_ZONE_INFO_T> &vInfo)
{
    AUTO_LOCK(g_BrdZoneMutex);

    vInfo.clear();
    for (UINT32 i=0; i<g_vBrdZoneInfo.size(); i++)
    {
        if (g_vBrdZoneInfo[i].ulStationID == ulStationID)
        {
            vInfo.push_back(g_vBrdZoneInfo[i]);
        }
    }
}

BOOL IsBrdZoneExist(UINT32 ulStationID, UINT32 ulZoneID)
{
    AUTO_LOCK(g_BrdZoneMutex);

    for (UINT32 i=0; i<g_vBrdZoneInfo.size(); i++)
    {
        if (g_vBrdZoneInfo[i].ulStationID == ulStationID &&
            g_vBrdZoneInfo[i].ulZoneID == ulZoneID )
        {
            return TRUE;
        }
    }

    return FALSE;
}

//=============== train ========================
VOID InitTrainInfo(VECTOR<TRAIN_T> &vTrain)
{
    TRAIN_INFO_T    stTrainInfo;

    AUTO_LOCK(g_TrainMutex);

    g_mTrainInfo.Clear();
    for (UINT32 i=0; i<vTrain.size(); i++)
    {
        memset(&stTrainInfo, 0, sizeof(stTrainInfo));
        stTrainInfo.ulTrainUnitID = vTrain[i].ulTrainUnitID;
        stTrainInfo.ulTrainType = vTrain[i].ulTrainType;
        strcpy(stTrainInfo.acTrainName, vTrain[i].acTrainName);
        stTrainInfo.i64DefaultGroupID = vTrain[i].i64DefaultGroupID;

        g_mTrainInfo.Add(stTrainInfo.ulTrainUnitID, stTrainInfo);
    }
}

// 将ATS 驾驶模式转换成NOCC接口中的驾驶模式
// am是有人的自动驾驶，cm是人工驾驶，rm是限速25的人工驾驶，eum是切除信号驾驶，fam是全自动驾驶，cam和rrm都是限速25的全自动驾驶，但是rrm的功能比cam更少
UINT32 ConvertDriveMode(UINT8 ucATSDriveMode)
{
    if (ucATSDriveMode == ATS_DRIVE_MODE_AM ||
        ucATSDriveMode == ATS_DRIVE_MODE_FAM ||
        ucATSDriveMode == ATS_DRIVE_MODE_CAM ||
        ucATSDriveMode == ATS_DRIVE_MODE_RRM )
    {
        return DRIVE_MODE_AUTO;
    }

    return DRIVE_MODE_MANUAL;
}

VOID SetATSInfo(ATS_INFO_T *pstATS, TRAIN_INFO_T *pstTrain)
{
    UINT32      ulDestStationID;

    pstTrain->ulUptime = gos_get_uptime();
    pstTrain->ulDriveMode = pstATS->ucDriveMode;

    if (0 == pstTrain->ulParkTime)
    {
        pstTrain->ulParkTime = gos_get_current_time();
    }

    if (pstATS->usStationID == pstATS->usUpsideStationID &&
        pstATS->usStationID == pstATS->usDownsideStationID )  // 停在站区
    {
        if (pstTrain->ulStationID       != pstATS->usStationID || // 站区变化
            pstTrain->ulUpsideStationID != pstATS->usStationID ||
            pstTrain->ulDownsideStationID != pstATS->usStationID )
        {
            pstTrain->ulParkTime = gos_get_current_time();
            goto next;
        }
    }

    if (pstTrain->ulStationID == pstTrain->ulUpsideStationID &&
        pstTrain->ulStationID == pstTrain->ulDownsideStationID )
    {
        if (pstTrain->ulStationID != pstATS->usStationID ||
            pstTrain->ulStationID != pstATS->usUpsideStationID ||
            pstTrain->ulStationID != pstATS->usDownsideStationID )
        {
            pstTrain->ulLeaveTime = gos_get_current_time();
            goto next;
        }
    }

    if (pstATS->usStationID != pstTrain->ulStationID)
    {
        if (pstATS->usStationID != pstATS->usUpsideStationID ||
            pstATS->usStationID != pstATS->usDownsideStationID )
        {
            pstTrain->ulComeTime = gos_get_current_time();
            goto next;
        }
    }

next:
    memset(pstTrain->acTrainServiceNumber, 0, sizeof(pstTrain->acTrainServiceNumber));
    memset(pstTrain->acTrainSerialNumber, 0, sizeof(pstTrain->acTrainSerialNumber));
    memset(pstTrain->acTrainID, 0, sizeof(pstTrain->acTrainID));

    if (pstATS->ucServiceNumberLen)
    {
        memcpy(pstTrain->acTrainServiceNumber, pstATS->acServiceNumber, pstATS->ucServiceNumberLen);
        memcpy(pstTrain->acTrainID, pstATS->acServiceNumber, pstATS->ucServiceNumberLen);
    }

    if (pstATS->ucOrderNumberLen)
    {
        memcpy(pstTrain->acTrainSerialNumber, pstATS->acOrderNumber, pstATS->ucOrderNumberLen);
        memcpy(pstTrain->acTrainID+pstATS->ucServiceNumberLen, pstATS->acOrderNumber, pstATS->ucOrderNumberLen);
    }

    pstTrain->ulDriverID = pstATS->usDriverNumber;

    ulDestStationID = INVALID_STATION_ID;
    if (pstATS->ucDestIDLen > 0)
    {
        strncpy(pstTrain->acDestID, pstATS->acDestID, pstATS->ucDestIDLen);
    }

    pstTrain->ulRollingStockNum = pstATS->ucCompartmentNum;
    if (pstATS->ucDirection == ATS_DIRECTION_UP)
    {
        pstTrain->ulDirection = TRAIN_DIRECTION_UPSIDE;
    }
    else if (pstATS->ucDirection == ATS_DIRECTION_DOWN)
    {
        pstTrain->ulDirection = TRAIN_DIRECTION_DOWNSIDE;
    }
    else
    {
        pstTrain->ulDirection = TRAIN_DIRECTION_UNKNOWN;
    }

    pstTrain->ulStationID = pstATS->usStationID;
    pstTrain->ulUpsideStationID = pstATS->usUpsideStationID;
    pstTrain->ulDownsideStationID = pstATS->usDownsideStationID;

    if (pstATS->ucOnTurnBackTrackFlag == ATS_ON_TURNBACK_TRACK_NO)
    {
        pstTrain->ulTurnbackTrackFlag = TRAIN_ON_TURNBACK_TRACK_NO;
    }
    else if (pstATS->ucOnTurnBackTrackFlag == ATS_ON_TURNBACK_TRACK_YES)
    {
        pstTrain->ulTurnbackTrackFlag = TRAIN_ON_TURNBACK_TRACK_YES;
    }
    else
    {
        pstTrain->ulTurnbackTrackFlag = TRAIN_ON_TURNBACK_TRACK_UNKNOWN;
    }

    if (pstATS->ucIsOnPlatform == ATS_ON_PLATFORM_NO)
    {
        pstTrain->ulPlatformID = TRAIN_ON_PLATFORM_NO;
    }
    else
    {
        pstTrain->ulPlatformID = TRAIN_ON_PLATFORM_YES;
    }

    if (pstATS->ucOnTransferTrackFlag == ATS_ON_TRANSFER_NO)
    {
        pstTrain->ulTransferTrackFlag = FALSE;
    }
    else
    {
        pstTrain->ulTransferTrackFlag = TRUE;
    }

    if (pstATS->ucInOutDepot == ATS_INOUT_DEPOT_STATE_OUT)
    {
        pstTrain->ulInOutDepot = FALSE;
    }
    else
    {
        pstTrain->ulInOutDepot = TRUE;
    }

    // 14号线转换轨标识与存车线标识相同
    SECTION_INFO_T stSectionInfo;

    // 14号线采用section来判断
    if (GetSectionInfo(pstTrain->usSectionID, stSectionInfo))
    {
        pstTrain->ulDepotTrackFlag = stSectionInfo.bInDepotTrack;
    }
    else
    {
        pstTrain->ulDepotTrackFlag = pstTrain->ulTurnbackTrackFlag;
    }

    if (pstATS->ucLostRealLocation == ATS_LOST_REAL_LOCATION_NO)
    {
        pstTrain->ulLostRealLocation = FALSE;
    }
    else
    {
        pstTrain->ulLostRealLocation = TRUE;
    }

    strcpy(pstTrain->acTrackName, "");

    pstTrain->usSectionID = pstATS->usPhysicSectionID;
    pstTrain->ucIndexInSection = pstATS->ucIndexInSection;
}

void TestATSData(VECTOR<ATS_INFO_T> &vATSInfo)
{
    vATSInfo.clear();

    //ATS_INFO_T  astATS[] =
    UINT8   aucATS[] =

    {
            0x04,0x31,0x34,0x31,0x30,0x03,0x32,0x30,0x31,0x02,0x30,0x32,0x00,0x00,0x03,0x32,0x36,0x32,0x06,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x6B,0x01,0x7A,0x00,0x01,0x08,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0xAA,0xAA,0x01,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x30,0x37,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x15,0x00,0x00,0x00,0x15,0x00,0x02,0x00,0x00,0x00,0x01,0x01,0x00,0xAB,0x01,0x8B,0x00,0x01,0x08,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0xAA,0xAA,0x01,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    /*        0x04,0x31,0x34,0x32,0x31,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x01,0x1D,0x03,0x42,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0xAA,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x31,0x38,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x01,0x18,0x03,0x3E,0x01,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0xAA,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x31,0x35,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x01,0x00,0x00,0x01,0x00,0x01,0xBC,0x02,0xFD,0x00,0x01,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0xAA,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x31,0x34,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x01,0x00,0x00,0x01,0x00,0x01,0xC1,0x02,0x01,0x01,0x01,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0xAA,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x35,0x35,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x01,0x00,0x00,0x01,0x00,0x01,0xE5,0x02,0x1E,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0x55,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x35,0x32,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x03,0x2E,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0xAA,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x35,0x31,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x01,0x00,0x00,0x01,0x00,0x01,0xA0,0x02,0xEE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0x55,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x31,0x33,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x01,0x00,0x00,0x01,0x00,0x01,0xD4,0x02,0x10,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0x55,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x30,0x35,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x01,0x00,0x00,0x01,0x00,0x01,0x27,0x03,0x47,0x01,0x01,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0xAA,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x32,0x30,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x01,0xF0,0x02,0x25,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0xAA,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x30,0x34,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x01,0x00,0x00,0x01,0x00,0x01,0xD9,0x02,0x14,0x01,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0x55,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x31,0x31,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x01,0x00,0x00,0x01,0x00,0x01,0xE2,0x02,0x1B,0x01,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0x55,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x31,0x39,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x01,0x00,0x00,0x01,0x00,0x01,0x36,0x03,0x54,0x01,0x01,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0xAA,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x30,0x32,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x01,0x00,0x00,0x01,0x00,0x01,0x33,0x03,0x51,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0x55,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x31,0x36,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x01,0x00,0x00,0x01,0x00,0x01,0xBD,0x02,0xFE,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0x55,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x31,0x37,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x01,0x17,0x03,0x3D,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0xAA,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x35,0x30,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x01,0x00,0x00,0x01,0x00,0x01,0x2A,0x03,0x4A,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0x55,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x30,0x33,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x01,0x00,0x00,0x01,0x00,0x01,0x30,0x03,0x4E,0x01,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0x55,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x30,0x39,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x01,0x00,0x00,0x01,0x00,0x01,0xD7,0x02,0x12,0x01,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0x55,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x30,0x31,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x01,0x00,0x00,0x01,0x00,0x01,0xD1,0x02,0x0D,0x01,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0x55,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x30,0x38,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x1D,0x00,0x1D,0x00,0x1D,0x00,0xFF,0x01,0x00,0x00,0x01,0x00,0x01,0xDC,0x02,0x17,0x01,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0xAA,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x30,0x36,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0B,0x00,0x1E,0x00,0x1E,0x00,0x1E,0x00,0xFF,0x01,0x00,0x00,0x01,0x00,0x01,0xA9,0x03,0xA7,0x01,0x01,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0xAA,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x04,0x31,0x34,0x31,0x32,0x03,0x30,0x30,0x30,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x0B,0x00,0x1E,0x00,0x1E,0x00,0x1E,0x00,0xFF,0x01,0x00,0x00,0x01,0x00,0x01,0x92,0x03,0x90,0x01,0x01,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xAA,0xAA,0xFF,0x00,0x55,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    */
    };

    ATS_INFO_T      *pstATSInfo = (ATS_INFO_T*)aucATS;

    for (UINT32 i=0; i<sizeof(aucATS)/sizeof(ATS_INFO_T); i++)
    {
        vATSInfo.push_back(pstATSInfo[i]);
    }

    /*
    vATSInfo.resize(4);

    int iIndex = 0;

    vATSInfo[iIndex].ucTrainUnitIDLen = 4;
    memcpy(vATSInfo[iIndex].acTrainUnitID, "1410", 4);
    vATSInfo[iIndex].usStationID = 21;
    vATSInfo[iIndex].ucDirection = 0;
    vATSInfo[iIndex].ucOnTransferTrackFlag = 0;
    vATSInfo[iIndex].ucOnTurnBackTrackFlag = 0;
    vATSInfo[iIndex].ucIsOnPlatform = 0;
    vATSInfo[iIndex].ucLostRealLocation = 0;
    vATSInfo[iIndex].usPhysicSectionID = 122;
    vATSInfo[iIndex].ucIndexInSection = 1;

    iIndex++;

    vATSInfo[iIndex].ucTrainUnitIDLen = 4;
    memcpy(vATSInfo[iIndex].acTrainUnitID, "1407", 4);
    vATSInfo[iIndex].usStationID = 21;
    vATSInfo[iIndex].ucDirection = 0;
    vATSInfo[iIndex].ucOnTransferTrackFlag = 0;
    vATSInfo[iIndex].ucOnTurnBackTrackFlag = 1;
    vATSInfo[iIndex].ucIsOnPlatform = 1;
    vATSInfo[iIndex].ucLostRealLocation = 0;
    vATSInfo[iIndex].usPhysicSectionID = 139;
    vATSInfo[iIndex].ucIndexInSection = 1;

    iIndex++;

    vATSInfo[iIndex].ucTrainUnitIDLen = 4;
    memcpy(vATSInfo[iIndex].acTrainUnitID, "1411", 4);
    vATSInfo[iIndex].usStationID = 17;
    vATSInfo[iIndex].ucDirection = 0;
    vATSInfo[iIndex].ucOnTransferTrackFlag = 0;
    vATSInfo[iIndex].ucOnTurnBackTrackFlag = 1;
    vATSInfo[iIndex].ucIsOnPlatform = 1;
    vATSInfo[iIndex].ucLostRealLocation = 0;
    vATSInfo[iIndex].usPhysicSectionID = 81;
    vATSInfo[iIndex].ucIndexInSection = 1;

    iIndex++;

    vATSInfo[iIndex].ucTrainUnitIDLen = 4;
    memcpy(vATSInfo[iIndex].acTrainUnitID, "1403", 4);
    vATSInfo[iIndex].usStationID = 17;
    vATSInfo[iIndex].ucDirection = 1;
    vATSInfo[iIndex].ucOnTransferTrackFlag = 0;
    vATSInfo[iIndex].ucOnTurnBackTrackFlag = 1;
    vATSInfo[iIndex].ucIsOnPlatform = 1;
    vATSInfo[iIndex].ucLostRealLocation = 0;
    vATSInfo[iIndex].usPhysicSectionID = 94;
    vATSInfo[iIndex].ucIndexInSection = 1;

    iIndex++;

    */
}

VOID SetATSDataTime()
{
    g_ulLastATSDataTime = gos_get_sys_uptime();
}

UINT32 GetATSDataTimeDiff()
{
    return gos_get_sys_uptime() - g_ulLastATSDataTime;
}

// 是否超时未收到ATS数据
BOOL IsATSDataTimeout(UINT32 ulTrainATSUptime)
{
    UINT32  ulTimeDiff = GetATSDataTimeDiff();

    if (ulTimeDiff > g_stLocalCfg.ulATSValidPeriod)
    {
        return TRUE;
    }

    ulTimeDiff = gos_get_uptime() - ulTrainATSUptime;
    if (ulTimeDiff > g_stLocalCfg.ulATSValidPeriod)
    {
        return TRUE;
    }

    return FALSE;
}

UINT32 GetLostRealLocation(UINT32 ulLostRealLocation, UINT32 ulTrainATSUptime)
{
    if (ulLostRealLocation == TRUE)
    {
        return TRUE;
    }

    if (IsATSDataTimeout(ulTrainATSUptime))
    {
        return TRUE;
    }

    return FALSE;
}

VOID InitATSInfo(VECTOR<ATS_INFO_T> &vATSInfo)
{
    AUTO_LOCK(g_TrainMutex);

    SetATSDataTime();

#if 0
    TestATSData(vATSInfo);
#endif

    g_mATSInfo.Clear();

    for (UINT32 i=0; i<vATSInfo.size(); i++)
    {
        UINT32          ulTrainUnitID;

        if (!CharArrayToInt(vATSInfo[i].acTrainUnitID, sizeof(vATSInfo[i].acTrainUnitID), &ulTrainUnitID))
        {
            continue;
        }

        g_mATSInfo.Add(ulTrainUnitID, vATSInfo[i]);

        TRAIN_INFO_T    *pstInfo = g_mTrainInfo.GetValue(ulTrainUnitID);

        if (!pstInfo)
        {
            GosLog(LOG_ERROR, "unknown train : %u", ulTrainUnitID);
        }
        else
        {
            SetATSInfo(&vATSInfo[i], pstInfo);
            pstInfo->ulUptime = gos_get_uptime();
//            pstInfo->ulUpdateTime = ulUpdateTime;
            pstInfo->bRegister = FALSE;
            pstInfo->ulRegisterDepotID = DEPOT_ID_UNKNOWN;
        }
    }
}

VOID GetTrainInfo(VECTOR<TRAIN_INFO_T> &vTrainInfo)
{
    g_TrainMutex.Mutex();

    vTrainInfo.clear();
    for (UINT32 i=0; i<g_mTrainInfo.Size(); i++)
    {
        vTrainInfo.push_back(*g_mTrainInfo[i]);
    }

    g_TrainMutex.Unmutex();

    g_RegisterMutex.Mutex();

    REGISTER_INFO_T     *pstInfo;

    for (UINT32 i=0; i<vTrainInfo.size(); i++)
    {
        vTrainInfo[i].bRegister = FALSE;
        vTrainInfo[i].ulRegisterDepotID = DEPOT_ID_UNKNOWN;

        pstInfo = g_mAPPRegisterInfo.GetValue(vTrainInfo[i].ulTrainUnitID);
        if (pstInfo)
        {
            vTrainInfo[i].bRegister = pstInfo->bRegister;
            vTrainInfo[i].ulRegisterDepotID = pstInfo->ulRegisterDepotID;
        }
    }

    g_RegisterMutex.Unmutex();
}

BOOL GetTrainInfo(UINT32 ulTrainUnitID, TRAIN_INFO_T &stTrainInfo)
{
    AUTO_LOCK(g_TrainMutex);

    TRAIN_INFO_T    *pstInfo = g_mTrainInfo.GetValue(ulTrainUnitID);

    if (pstInfo)
    {
        memcpy(&stTrainInfo, pstInfo, sizeof(stTrainInfo));
        return TRUE;
    }

    return FALSE;
}

VOID GetTrainInfoByStationID(UINT32 ulStationID, VECTOR<TRAIN_INFO_T> &vTrainInfo)
{
    AUTO_LOCK(g_TrainMutex);

    vTrainInfo.clear();
    for (UINT32 i=0; i<g_mTrainInfo.Size(); i++)
    {
        TRAIN_INFO_T    *pstTrain = g_mTrainInfo[i];

        if (pstTrain->ulStationID == ulStationID)
        {
            vTrainInfo.push_back(*pstTrain);
        }
    }
}

BOOL GetTrainInfoByTrainID(const std::string& strTrainID, TRAIN_INFO_T& stTrainInfo)
{
    AUTO_LOCK(g_TrainMutex);

    for (UINT32 i = 0; i < g_mTrainInfo.Size(); i++)
    {
        // 不要在底层函数中随便加上打印，会产生大量的打印
        //GosLog(LOG_DETAIL, "ATS TrainID: %s", g_mTrainInfo[i]->acTrainID);
        if (std::string (g_mTrainInfo[i]->acTrainID) == strTrainID)
        {
            memcpy(&stTrainInfo, g_mTrainInfo[i], sizeof(stTrainInfo));
            return TRUE;
        }
    }

    return FALSE;
}

BOOL GetTrainInfoByTrainUnitID(const int& iTrainUnitID, TRAIN_INFO_T& stTrainInfo)
{
 /*   AUTO_LOCK(g_TrainMutex);

    for(UINT32 i = 0; i < g_mTrainInfo.Size(); i++)
    {
        if (g_mTrainInfo[i]->ulTrainUnitID == (UINT32)iTrainUnitID)
        {
            memcpy(&stTrainInfo, g_mTrainInfo[i], sizeof(stTrainInfo));
            return TRUE;
        }
    }

    return FALSE; */

    return GetTrainInfo((UINT32)iTrainUnitID, stTrainInfo);
}

VOID GetMainLineTrainUnitID(VECTOR<UINT32> &vTrainUnitID)
{
    AUTO_LOCK(g_TrainMutex);

    vTrainUnitID.clear();

    STATION_INFO_T  stStation;
    TRAIN_INFO_T    *pstTrain;

    for (UINT32 i=0; i<g_mTrainInfo.Size(); i++)
    {
        pstTrain = g_mTrainInfo[i];
        if (!GetStationInfo(pstTrain->ulStationID, &stStation))
        {
            continue;
        }

        if (stStation.ulDepotID == DEPOT_ID_MAINLINE)
        {
            vTrainUnitID.push_back(pstTrain->ulTrainUnitID);
        }
    }
}

VOID GetUpLineTrainUnitID(VECTOR<UINT32> &vTrainUnitID)
{
    AUTO_LOCK(g_TrainMutex);

    vTrainUnitID.clear();

    STATION_INFO_T  stStation;
    TRAIN_INFO_T    *pstTrain;

    for (UINT32 i=0; i<g_mTrainInfo.Size(); i++)
    {
        pstTrain = g_mTrainInfo[i];
        if (!GetStationInfo(pstTrain->ulStationID, &stStation))
        {
            continue;
        }

        if (stStation.ulDepotID == DEPOT_ID_MAINLINE &&
            pstTrain->ulDirection == TRAIN_DIRECTION_UPSIDE )
        {
            vTrainUnitID.push_back(pstTrain->ulTrainUnitID);
        }
    }
}

VOID GetDownLineTrainUnitID(VECTOR<UINT32> &vTrainUnitID)
{
    AUTO_LOCK(g_TrainMutex);

    vTrainUnitID.clear();

    STATION_INFO_T  stStation;
    TRAIN_INFO_T    *pstTrain;

    for (UINT32 i=0; i<g_mTrainInfo.Size(); i++)
    {
        pstTrain = g_mTrainInfo[i];
        if (!GetStationInfo(pstTrain->ulStationID, &stStation))
        {
            continue;
        }

        if (stStation.ulDepotID == DEPOT_ID_MAINLINE &&
            pstTrain->ulDirection == TRAIN_DIRECTION_DOWNSIDE )
        {
            vTrainUnitID.push_back(pstTrain->ulTrainUnitID);
        }
    }
}

BOOL SetIPHAudioAddr(UINT32 ulTrainUnitID, IPH_AUDIO_NET_INFO_T &stInfo)
{
    AUTO_LOCK(g_TrainMutex);

    TRAIN_INFO_T    *pstInfo = g_mTrainInfo.GetValue(ulTrainUnitID);

    if (!pstInfo)
    {
        GosLog(LOG_ERROR, "invalid train %u when update train info", ulTrainUnitID);
        return FALSE;
    }

    if (memcmp(pstInfo->stIPHAudioNetInfo.aucIPHAddr, stInfo.aucIPHAddr, 4) != 0)
    {
        GosLog(LOG_INFO, "train(%u) set iph audio addr "IP_FMT, ulTrainUnitID, IP_ARG(stInfo.aucIPHAddr));
    }

    pstInfo->stIPHAudioNetInfo.ulMasterStatus = stInfo.ulMasterStatus;
    pstInfo->stIPHAudioNetInfo.ulIPHStatus    = stInfo.ulIPHStatus;
    memcpy(pstInfo->stIPHAudioNetInfo.aucIPHAddr, stInfo.aucIPHAddr, 4);

    return TRUE;
}

BOOL SetAppTrainInfo(APP_TRAIN_INFO_T &stInfo)
{
    AUTO_LOCK(g_TrainMutex);

    TRAIN_INFO_T    *pstInfo = g_mTrainInfo.GetValue(stInfo.ulTrainUnitID);

    if (!pstInfo)
    {
        GosLog(LOG_ERROR, "invalid train %u when update train info", stInfo.ulTrainUnitID);
        return FALSE;
    }

    pstInfo->ulAppReportTime = gos_get_uptime();
/*    if (stInfo.ulUserID == pstInfo->astAppTrainInfo[1].ulUserID)
    {
        memcpy(&pstInfo->astAppTrainInfo[1], &stInfo, sizeof(stInfo));
    }
    else // 其他情况一律覆盖0
    {
        memcpy(&pstInfo->astAppTrainInfo[0], &stInfo, sizeof(stInfo));
    }*/

    if (stInfo.ulTC == ATS_TC1)
    {
        memcpy(&pstInfo->astAppTrainInfo[0], &stInfo, sizeof(stInfo));
    }
    else if (stInfo.ulTC == ATS_TC2)
    {
        memcpy(&pstInfo->astAppTrainInfo[1], &stInfo, sizeof(stInfo));
    }
    else
    {
        GosLog(LOG_ERROR, "invalid TC %u of train %u when update train info", stInfo.ulTC, stInfo.ulTrainUnitID);
        return FALSE;
    }

    return TRUE;
}

BOOL SetTrainPos(UINT32 ulTrainUnitID, UINT32 ulDepotID)
{
    AUTO_LOCK(g_TrainMutex);

    TRAIN_INFO_T    *pstInfo = g_mTrainInfo.GetValue(ulTrainUnitID);

    if (!pstInfo)
    {
        return FALSE;
    }

    pstInfo->bRegister = TRUE;
    pstInfo->ulRegisterDepotID = ulDepotID;

    return TRUE;
}

BOOL SetTrainPos(VECTOR<UINT32> &vTrain, UINT32 ulDepotID, CHAR *szDestID, UINT32 ulDirection)
{
    AUTO_LOCK(g_TrainMutex);

    TRAIN_INFO_T    *pstTrain;

    for (UINT32 i=0; i<vTrain.size(); i++)
    {
        pstTrain = g_mTrainInfo.GetValue(vTrain[i]);
        if (!pstTrain)
        {
            GosLog(MODULE_CFG, LOG_ERROR, "set train position of %u failed: unknown train", vTrain[i]);
            continue;
        }

        pstTrain->bRegister = TRUE;
        pstTrain->ulRegisterDepotID = ulDepotID;
        strncpy(pstTrain->acDestID, szDestID, sizeof(pstTrain->acDestID)-1);
        pstTrain->ulDirection = ulDirection;
    }

    return TRUE;
}

BOOL SetTrainPos(UINT32 ulTrainUnitID, UINT32 ulDepotID, CHAR *szTrainID, CHAR *szDestID, UINT32 ulDirection, UINT32 ulStationID,UINT32 ulDriverID)
{
    AUTO_LOCK(g_TrainMutex);

    TRAIN_INFO_T    *pstInfo = g_mTrainInfo.GetValue(ulTrainUnitID);

    if (!pstInfo)
    {
        return FALSE;
    }

    pstInfo->ulRegisterDepotID = ulDepotID;
    strncpy(pstInfo->acTrainID, szTrainID, sizeof(pstInfo->acTrainID));
    strncpy(pstInfo->acDestID, szDestID, sizeof(pstInfo->acDestID)-1);
    pstInfo->ulDirection = ulDirection;
    pstInfo->ulStationID = ulStationID;
    pstInfo->ulUpsideStationID = ulStationID;
    pstInfo->ulDownsideStationID = ulStationID;
    pstInfo->ulDriverID = ulDriverID;
    pstInfo->ulLostRealLocation = TRUE;

    return TRUE;
}

VOID AddTrainInfo(TRAIN_T &stInfo)
{
    AUTO_LOCK(g_TrainMutex);

    TRAIN_INFO_T    stTrainInfo;

    memset(&stTrainInfo, 0, sizeof(stTrainInfo));
    stTrainInfo.ulTrainUnitID = stInfo.ulTrainUnitID;
    stTrainInfo.ulTrainType = stInfo.ulTrainType;
    strcpy(stTrainInfo.acTrainName, stInfo.acTrainName);
    stTrainInfo.i64DefaultGroupID = stInfo.i64DefaultGroupID;

    g_mTrainInfo.Add(stTrainInfo.ulTrainUnitID, stTrainInfo);
}

VOID DelTrainInfo(UINT32 ulTrainUnitID)
{
    AUTO_LOCK(g_TrainMutex);

    g_mTrainInfo.Del(ulTrainUnitID);
}

VOID SetTrainInfo(TRAIN_T &stTrain)
{
    AUTO_LOCK(g_TrainMutex);

    TRAIN_INFO_T    *pstInfo = g_mTrainInfo.GetValue(stTrain.ulTrainUnitID);

    if (pstInfo)
    {
        pstInfo->ulTrainType = stTrain.ulTrainType;
        strcpy(pstInfo->acTrainName, stTrain.acTrainName);
        pstInfo->i64DefaultGroupID = stTrain.i64DefaultGroupID;
    }
}

VOID GetStationTrainInfo(UINT32 ulStationID, UINT32 &ulUplineTrain, CHAR *szUplineTrainID, UINT32 &ulDownlineTrain, CHAR *szDownlineTrainID)
{
    AUTO_LOCK(g_TrainMutex);

    ulUplineTrain = INVALID_TRAIN_UNIT_ID;
    *szUplineTrainID = '\0';
    ulDownlineTrain = INVALID_TRAIN_UNIT_ID;
    *szDownlineTrainID = '\0';

    for (UINT32 i=0; i<g_mTrainInfo.Size(); i++)
    {
        TRAIN_INFO_T    *pstTrain = g_mTrainInfo[i];

        if (pstTrain->ulStationID == ulStationID &&
            pstTrain->ulUpsideStationID == ulStationID &&
            pstTrain->ulDownsideStationID == ulStationID )
        {
            if (pstTrain->ulDirection == TRAIN_DIRECTION_UPSIDE)
            {
                ulUplineTrain = pstTrain->ulTrainUnitID;
                strcpy(szUplineTrainID, pstTrain->acTrainID);
            }
            else if (pstTrain->ulDirection == TRAIN_DIRECTION_DOWNSIDE)
            {
                ulDownlineTrain = pstTrain->ulTrainUnitID;
                strcpy(szDownlineTrainID, pstTrain->acTrainID);
            }
        }
    }
}

//=============== dc opertor ========================
VOID InitDCOperatorInfo(VECTOR<DC_OPERATOR_INFO_T> &vInfo)
{
    AUTO_LOCK(g_UserMutex);

    g_vDCOperatorInfo.clear();
    g_vDCOperatorInfo.assign(vInfo.begin(), vInfo.end());
}

VOID AddDCOperatorInfo(DC_OPERATOR_INFO_T &stOperatorInfo)
{
    AUTO_LOCK(g_UserMutex);
    g_vDCOperatorInfo.push_back(stOperatorInfo);
}

VOID DelDCOperatorInfo(CHAR *szOperatorID)
{
    AUTO_LOCK(g_UserMutex);

    for (UINT32 i=0; i<g_vDCOperatorInfo.size(); i++)
    {
        if (strcmp(g_vDCOperatorInfo[i].acOperatorID, szOperatorID) == 0)
        {
            g_vDCOperatorInfo.erase(g_vDCOperatorInfo.begin()+i);
            break;
        }
    }
}

BOOL SetDCOperatorInfo(DC_OPERATOR_INFO_T &stOperatorInfo)
{
    AUTO_LOCK(g_UserMutex);

    for (UINT32 i=0; i<g_vDCOperatorInfo.size(); i++)
    {
        if (strcmp(g_vDCOperatorInfo[i].acOperatorID, stOperatorInfo.acOperatorID) == 0)
        {
            memcpy(&g_vDCOperatorInfo[i], &stOperatorInfo, sizeof(stOperatorInfo));

            return TRUE;
        }
    }

    return FALSE;
}

BOOL SetDCOperatorPwd(CHAR *szOperatorID, CHAR *szNewPassword)
{
    AUTO_LOCK(g_UserMutex);

    for (UINT32 i=0; i<g_vDCOperatorInfo.size(); i++)
    {
        if (strcmp(g_vDCOperatorInfo[i].acOperatorID, szOperatorID) == 0)
        {
            strcpy(g_vDCOperatorInfo[i].acPassword, szNewPassword);

            return TRUE;
        }
    }

    return FALSE;
}

VOID GetDCOperatorInfo(VECTOR<DC_OPERATOR_INFO_T> &vInfo)
{
    AUTO_LOCK(g_UserMutex);

    vInfo.clear();
    vInfo.assign(g_vDCOperatorInfo.begin(), g_vDCOperatorInfo.end());
}

BOOL CheckDCOperator(CHAR *szOperatorID, CHAR *szPassword, UINT32 ulDCType, CHAR *szErrInfo)
{
    AUTO_LOCK(g_UserMutex);

    *szErrInfo = '\0';
    for (UINT32 i=0; i<g_vDCOperatorInfo.size(); i++)
    {
        if (strcmp(g_vDCOperatorInfo[i].acOperatorID, szOperatorID) == 0)
        {
            if (strcmp(g_vDCOperatorInfo[i].acPassword, szPassword) != 0)
            {
                strcpy(szErrInfo, "密码错误");
                return FALSE;
            }

            if (g_vDCOperatorInfo[i].ulDCType & ulDCType)
            {
                return TRUE;
            }

            strcpy(szErrInfo, "没有调度权限");
            return FALSE;
        }
    }

    strcpy(szErrInfo, "未知用户");
    return FALSE;
}

BOOL GetDCOperatorInfo(CHAR *szOperatorID, DC_OPERATOR_INFO_T &stInfo)
{
    AUTO_LOCK(g_UserMutex);

    for (UINT32 i=0; i<g_vDCOperatorInfo.size(); i++)
    {
        if (strcmp(g_vDCOperatorInfo[i].acOperatorID, szOperatorID) == 0)
        {
            memcpy(&stInfo, &g_vDCOperatorInfo[i], sizeof(stInfo));
            return TRUE;
        }
    }

    return FALSE;
}

//=============== user ========================
VOID InitDCUserInfo(VECTOR<DC_USER_INFO_T> &vUserInfo)
{
    g_UserMutex.Mutex();

    g_vDCUserInfo.clear();
    g_vDCUserInfo.assign(vUserInfo.begin(), vUserInfo.end());
    g_UserMutex.Unmutex();
}

VOID AddDCUserInfo(DC_USER_INFO_T &stUserInfo)
{
    g_UserMutex.Mutex();
    g_vDCUserInfo.push_back(stUserInfo);
    g_UserMutex.Unmutex();
}

VOID DelDCUserInfo(UINT32 ulUserID)
{
    g_UserMutex.Mutex();
    for (UINT32 i=0; i<g_vDCUserInfo.size(); i++)
    {
        if (g_vDCUserInfo[i].ulUserID == ulUserID)
        {
            g_vDCUserInfo.erase(g_vDCUserInfo.begin()+i);
            break;
        }
    }

    g_UserMutex.Unmutex();
}

BOOL SetDCUserInfo(DC_USER_INFO_T &stUserInfo)
{
    g_UserMutex.Mutex();

    for (UINT32 i=0; i<g_vDCUserInfo.size(); i++)
    {
        if (g_vDCUserInfo[i].ulUserID == stUserInfo.ulUserID)
        {
            strcpy(g_vDCUserInfo[i].acName, stUserInfo.acName);
            g_vDCUserInfo[i].ulDCType = stUserInfo.ulDCType;
            g_vDCUserInfo[i].ulStationNum = stUserInfo.ulStationNum;
            memcpy(g_vDCUserInfo[i].aulStationList, stUserInfo.aulStationList, sizeof(stUserInfo.aulStationList));

            g_UserMutex.Unmutex();

            return TRUE;
        }
    }

    g_UserMutex.Unmutex();
    return FALSE;
}

BOOL CheckDCUser(UINT32 ulDCUserID, UINT32 ulDCType)
{
    AUTO_LOCK(g_UserMutex);

    for (UINT32 i=0; i<g_vDCUserInfo.size(); i++)
    {
        if (g_vDCUserInfo[i].ulUserID == ulDCUserID)
        {
            if (g_vDCUserInfo[i].ulDCType & ulDCType)
            {
                return TRUE;
            }

            return FALSE;
        }
    }

    return FALSE;
}

VOID GetDCUserInfo(VECTOR<DC_USER_INFO_T> &vUserInfo)
{
    AUTO_LOCK(g_UserMutex);

    vUserInfo.clear();
    for (UINT32 i=0; i<g_vDCUserInfo.size(); i++)
    {
        if (vUserInfo.size() == MAX_DC_NUM)
        {
            GosLog(LOG_ERROR, "too many DC user");
            break;
        }
        else
        {
            vUserInfo.push_back(g_vDCUserInfo[i]);
        }
    }
}

VOID GetTrainAndDepotDCUserInfo(VECTOR<DC_USER_INFO_T> &vUserInfo)
{
    AUTO_LOCK(g_UserMutex);

    vUserInfo.clear();
    for (UINT32 i=0; i<g_vDCUserInfo.size(); i++)
    {
        if (g_vDCUserInfo[i].ulDCType == DC_TYPE_TRAIN ||
            g_vDCUserInfo[i].ulDCType == DC_TYPE_FIX ||
            g_vDCUserInfo[i].ulDCType == DC_TYPE_DEPOT )
        {
            if (vUserInfo.size() == MAX_DC_NUM)
            {
                GosLog(LOG_ERROR, "too many DC user");
                break;
            }
            else
            {
                vUserInfo.push_back(g_vDCUserInfo[i]);
            }
        }
    }
}

//=============== Train UE ========================
VOID InitTrainUEInfo(VECTOR<TRAIN_UE_INFO_T> &vUserInfo)
{
    g_UserMutex.Mutex();

    g_vTrainUEInfo.clear();
    g_vTrainUEInfo.assign(vUserInfo.begin(), vUserInfo.end());
    g_UserMutex.Unmutex();
}

VOID GetTrainUEInfo(VECTOR<TRAIN_UE_INFO_T> &vInfo)
{
    g_UserMutex.Mutex();

    vInfo.clear();
    vInfo.assign(g_vTrainUEInfo.begin(), g_vTrainUEInfo.end());
    g_UserMutex.Unmutex();
}

BOOL GetTrainUEInfo(UINT32 ulUserID, TRAIN_UE_INFO_T *pstUserInfo)
{
    BOOL    bRet = FALSE;

    g_UserMutex.Mutex();
    for (UINT32 i=0; i<g_vTrainUEInfo.size(); i++)
    {
        if (g_vTrainUEInfo[i].ulUserID == ulUserID)
        {
            memcpy(pstUserInfo, &g_vTrainUEInfo[i], sizeof(TRAIN_UE_INFO_T));
            bRet = TRUE;
            break;
        }
    }

    g_UserMutex.Unmutex();

    return bRet;
}

VOID GetTrainUserID(UINT32 ulTrainUnitID, UINT32 *pulUserID1, UINT32 *pulUserID2)
{
    *pulUserID1 = INVALID_USER_ID;
    *pulUserID2 = INVALID_USER_ID;

    g_UserMutex.Mutex();
    for (UINT32 i=0; i<g_vTrainUEInfo.size(); i++)
    {
        if (g_vTrainUEInfo[i].ulTrainUnitID == ulTrainUnitID)
        {
            if (*pulUserID1 == INVALID_USER_ID)
            {
                *pulUserID1 = g_vTrainUEInfo[i].ulUserID;
            }
            else if (*pulUserID2 == INVALID_USER_ID)
            {
                *pulUserID2 = g_vTrainUEInfo[i].ulUserID;
                break;
            }
        }
    }

    g_UserMutex.Unmutex();
}

VOID AddTrainUEInfo(TRAIN_UE_INFO_T &stUserInfo)
{
    BOOL    bExist = FALSE;

    g_UserMutex.Mutex();
    for (UINT32 i=0; i<g_vTrainUEInfo.size(); i++)
    {
        if (g_vTrainUEInfo[i].ulUserID == stUserInfo.ulUserID)
        {
            memcpy(&g_vTrainUEInfo[i], &stUserInfo, sizeof(stUserInfo));
            bExist = TRUE;
            break;
        }
    }

    if (!bExist)
    {
        g_vTrainUEInfo.push_back(stUserInfo);
    }

    g_UserMutex.Unmutex();
}

VOID DelTrainUEInfo(UINT32 ulUserID)
{
    g_UserMutex.Mutex();
    for (UINT32 i=0; i<g_vTrainUEInfo.size(); i++)
    {
        if (g_vTrainUEInfo[i].ulUserID == ulUserID)
        {
            g_vTrainUEInfo.erase(g_vTrainUEInfo.begin()+i);
            break;
        }
    }

    g_UserMutex.Unmutex();
}

VOID SetTrainUEInfo(TRAIN_UE_INFO_T &stUserInfo)
{
    BOOL    bExist = FALSE;

    g_UserMutex.Mutex();
    for (UINT32 i=0; i<g_vTrainUEInfo.size(); i++)
    {
        if (g_vTrainUEInfo[i].ulUserID == stUserInfo.ulUserID)
        {
            memcpy(&g_vTrainUEInfo[i], &stUserInfo, sizeof(stUserInfo));
            bExist = TRUE;
            break;
        }
    }

    if (!bExist)
    {
        g_vTrainUEInfo.push_back(stUserInfo);
    }

    g_UserMutex.Unmutex();
}

UINT32 GetAnotherTrainUser(UINT32 ulUserID)
{
    UINT32  ulTrainUnitID = INVALID_TRAIN_UNIT_ID;

    AUTO_LOCK(g_UserMutex);

    for (UINT32 i=0; i<g_vTrainUEInfo.size(); i++)
    {
        if (g_vTrainUEInfo[i].ulUserID == ulUserID)
        {
            ulTrainUnitID = g_vTrainUEInfo[i].ulTrainUnitID;
            break;
        }
    }

    if (ulTrainUnitID == INVALID_TRAIN_UNIT_ID)
    {
        return INVALID_USER_ID;
    }

    for (UINT32 i=0; i<g_vTrainUEInfo.size(); i++)
    {
        if (g_vTrainUEInfo[i].ulTrainUnitID == ulTrainUnitID &&
            g_vTrainUEInfo[i].ulUserID != ulUserID )
        {
            return g_vTrainUEInfo[i].ulUserID;
        }
    }

    return INVALID_USER_ID;
}

//=============== Station UE ========================
VOID InitStationUEInfo(VECTOR<STATION_UE_INFO_T> &vUserInfo)
{
    g_UserMutex.Mutex();

    g_vStationUEInfo.clear();
    g_vStationUEInfo.assign(vUserInfo.begin(), vUserInfo.end());
    g_UserMutex.Unmutex();
}

VOID GetStationUEInfo(VECTOR<STATION_UE_INFO_T> &vInfo)
{
    g_UserMutex.Mutex();

    vInfo.clear();
    vInfo.assign(g_vStationUEInfo.begin(), g_vStationUEInfo.end());
    g_UserMutex.Unmutex();
}

BOOL GetStationUEInfo(UINT32 ulUserID, STATION_UE_INFO_T *pstUserInfo)
{
    BOOL    bRet = FALSE;

    g_UserMutex.Mutex();
    for (UINT32 i=0; i<g_vStationUEInfo.size(); i++)
    {
        if (g_vStationUEInfo[i].ulUserID == ulUserID)
        {
            memcpy(pstUserInfo, &g_vStationUEInfo[i], sizeof(STATION_UE_INFO_T));
            bRet = TRUE;
            break;
        }
    }

    g_UserMutex.Unmutex();

    return bRet;
}

VOID AddStationUEInfo(STATION_UE_INFO_T &stUserInfo)
{
    BOOL    bExist = FALSE;

    g_UserMutex.Mutex();
    for (UINT32 i=0; i<g_vStationUEInfo.size(); i++)
    {
        if (g_vStationUEInfo[i].ulUserID == stUserInfo.ulUserID)
        {
            memcpy(&g_vStationUEInfo[i], &stUserInfo, sizeof(stUserInfo));
            bExist = TRUE;
            break;
        }
    }

    if (!bExist)
    {
        g_vStationUEInfo.push_back(stUserInfo);
    }

    g_UserMutex.Unmutex();
}

VOID DelStationUEInfo(UINT32 ulUserID)
{
    g_UserMutex.Mutex();
    for (UINT32 i=0; i<g_vStationUEInfo.size(); i++)
    {
        if (g_vStationUEInfo[i].ulUserID == ulUserID)
        {
            g_vStationUEInfo.erase(g_vStationUEInfo.begin()+i);
            break;
        }
    }

    g_UserMutex.Unmutex();
}

VOID SetStationUEInfo(STATION_UE_INFO_T &stUserInfo)
{
    BOOL    bExist = FALSE;

    g_UserMutex.Mutex();
    for (UINT32 i=0; i<g_vStationUEInfo.size(); i++)
    {
        if (g_vStationUEInfo[i].ulUserID == stUserInfo.ulUserID)
        {
            memcpy(&g_vStationUEInfo[i], &stUserInfo, sizeof(stUserInfo));
            bExist = TRUE;
            break;
        }
    }

    if (!bExist)
    {
        g_vStationUEInfo.push_back(stUserInfo);
    }

    g_UserMutex.Unmutex();
}

//=============== Portal UE ========================
VOID InitPortalUEInfo(VECTOR<PORTAL_UE_INFO_T> &vUserInfo)
{
    g_UserMutex.Mutex();

    g_vPortalUEInfo.clear();
    g_vPortalUEInfo.assign(vUserInfo.begin(), vUserInfo.end());
    g_UserMutex.Unmutex();
}

VOID GetPortalUEInfo(VECTOR<PORTAL_UE_INFO_T> &vInfo)
{
    g_UserMutex.Mutex();

    vInfo.clear();
    vInfo.assign(g_vPortalUEInfo.begin(), g_vPortalUEInfo.end());
    g_UserMutex.Unmutex();
}

BOOL GetPortalUEInfo(UINT32 ulUserID, PORTAL_UE_INFO_T *pstUserInfo)
{
    BOOL    bRet = FALSE;

    g_UserMutex.Mutex();
    for (UINT32 i=0; i<g_vPortalUEInfo.size(); i++)
    {
        if (g_vPortalUEInfo[i].ulUserID == ulUserID)
        {
            memcpy(pstUserInfo, &g_vPortalUEInfo[i], sizeof(PORTAL_UE_INFO_T));
            bRet = TRUE;
            break;
        }
    }

    g_UserMutex.Unmutex();

    return bRet;
}

VOID AddPortalUEInfo(PORTAL_UE_INFO_T &stUserInfo)
{
    BOOL    bExist = FALSE;

    g_UserMutex.Mutex();
    for (UINT32 i=0; i<g_vPortalUEInfo.size(); i++)
    {
        if (g_vPortalUEInfo[i].ulUserID == stUserInfo.ulUserID)
        {
            memcpy(&g_vPortalUEInfo[i], &stUserInfo, sizeof(stUserInfo));
            bExist = TRUE;
            break;
        }
    }

    if (!bExist)
    {
        g_vPortalUEInfo.push_back(stUserInfo);
    }

    g_UserMutex.Unmutex();
}

VOID DelPortalUEInfo(UINT32 ulUserID)
{
    g_UserMutex.Mutex();
    for (UINT32 i=0; i<g_vPortalUEInfo.size(); i++)
    {
        if (g_vPortalUEInfo[i].ulUserID == ulUserID)
        {
            g_vPortalUEInfo.erase(g_vPortalUEInfo.begin()+i);
            break;
        }
    }

    g_UserMutex.Unmutex();
}

VOID SetPortalUEInfo(PORTAL_UE_INFO_T &stUserInfo)
{
    BOOL    bExist = FALSE;

    g_UserMutex.Mutex();
    for (UINT32 i=0; i<g_vPortalUEInfo.size(); i++)
    {
        if (g_vPortalUEInfo[i].ulUserID == stUserInfo.ulUserID)
        {
            memcpy(&g_vPortalUEInfo[i], &stUserInfo, sizeof(stUserInfo));
            bExist = TRUE;
            break;
        }
    }

    if (!bExist)
    {
        g_vPortalUEInfo.push_back(stUserInfo);
    }

    g_UserMutex.Unmutex();
}

VOID SetPortalUEGISInfo(UINT32 ulUserID, DOUBLE dLon, DOUBLE dLat)
{
    g_UserMutex.Mutex();
    for (UINT32 i=0; i<g_vPortalUEInfo.size(); i++)
    {
        if (g_vPortalUEInfo[i].ulUserID == ulUserID)
        {
            g_vPortalUEInfo[i].dLon = dLon;
            g_vPortalUEInfo[i].dLat = dLat;
            break;
        }
    }

    g_UserMutex.Unmutex();
}

//=============== Rec Brd ========================
VOID InitRecBrdCfg(VECTOR<REC_BRD_CFG_T> &vInfo)
{
    AUTO_LOCK(g_RecBrdCfgMutex);

    g_vRecBrdCfg.clear();
    g_vRecBrdCfg.assign(vInfo.begin(), vInfo.end());
}

VOID GetRecBrdCfg(VECTOR<REC_BRD_CFG_T> &vInfo)
{
    AUTO_LOCK(g_RecBrdCfgMutex);

    vInfo.clear();
    vInfo.assign(g_vRecBrdCfg.begin(), g_vRecBrdCfg.end());
}

BOOL GetRecBrdCfg(UINT32 ulRecID, REC_BRD_CFG_T &stCfg)
{
    AUTO_LOCK(g_RecBrdCfgMutex);

    for (UINT32 i=0; i<g_vRecBrdCfg.size(); i++)
    {
        if (g_vRecBrdCfg[i].ulRecID == ulRecID)
        {
            memcpy(&stCfg, &g_vRecBrdCfg[i], sizeof(stCfg));
            return TRUE;
        }
    }

    return FALSE;
}

VOID AddRecBrdCfg(REC_BRD_CFG_T &stInfo)
{
    AUTO_LOCK(g_RecBrdCfgMutex);

    BOOL    bExist = FALSE;

    for (UINT32 i=0; i<g_vRecBrdCfg.size(); i++)
    {
        if (g_vRecBrdCfg[i].ulRecID == stInfo.ulRecID)
        {
            memcpy(&g_vRecBrdCfg[i], &stInfo, sizeof(stInfo));
            bExist = TRUE;
            break;
        }
    }

    if (!bExist)
    {
        g_vRecBrdCfg.push_back(stInfo);
    }
}

VOID DelRecBrdCfg(UINT32 ulRecID)
{
    AUTO_LOCK(g_RecBrdCfgMutex);

    for (UINT32 i=0; i<g_vRecBrdCfg.size(); i++)
    {
        if (g_vRecBrdCfg[i].ulRecID == ulRecID)
        {
            g_vRecBrdCfg.erase(g_vRecBrdCfg.begin()+i);
            break;
        }
    }
}

VOID SetRecBrdCfg(REC_BRD_CFG_T &stInfo)
{
    AUTO_LOCK(g_RecBrdCfgMutex);

    BOOL    bExist = FALSE;

    for (UINT32 i=0; i<g_vPortalUEInfo.size(); i++)
    {
        if (g_vRecBrdCfg[i].ulRecID == stInfo.ulRecID)
        {
            memcpy(&g_vRecBrdCfg[i], &stInfo, sizeof(stInfo));
            bExist = TRUE;
            break;
        }
    }

    if (!bExist)
    {
        g_vRecBrdCfg.push_back(stInfo);
    }
}

//=============== Predefined SMS ========================
VOID InitPredefinedSMS(VECTOR<PREDEFINED_SMS_INFO_T> &vInfo)
{
    g_PredefinedSMSMutex.Mutex();

    g_vPredefinedSMS.clear();
    g_vPredefinedSMS.assign(vInfo.begin(), vInfo.end());
    g_PredefinedSMSMutex.Unmutex();
}

VOID GetPredefinedSMS(VECTOR<PREDEFINED_SMS_INFO_T> &vInfo)
{
    g_PredefinedSMSMutex.Mutex();

    vInfo.clear();
    vInfo.assign(g_vPredefinedSMS.begin(), g_vPredefinedSMS.end());
    g_PredefinedSMSMutex.Unmutex();
}

VOID DelPredefinedSMS(UINT32 ulSMSID)
{
    g_PredefinedSMSMutex.Mutex();
    for (UINT32 i=0; i<g_vPredefinedSMS.size(); i++)
    {
        if (g_vPredefinedSMS[i].ulSMSID == ulSMSID)
        {
            g_vPredefinedSMS.erase(g_vPredefinedSMS.begin()+i);
            break;
        }
    }

    g_PredefinedSMSMutex.Unmutex();
}

UINT32 GetMaxSMSID()
{
    UINT32  ulMax = 0;

    g_PredefinedSMSMutex.Mutex();
    for (UINT32 i=0; i<g_vPredefinedSMS.size(); i++)
    {
        if (ulMax < g_vPredefinedSMS[i].ulSMSID)
        {
            ulMax = g_vPredefinedSMS[i].ulSMSID;
        }
    }

    g_PredefinedSMSMutex.Unmutex();

    return ulMax;
}

VOID SetPredefinedSMS(PREDEFINED_SMS_INFO_T &stInfo)
{
    BOOL    bExist = FALSE;

    g_PredefinedSMSMutex.Mutex();
    for (UINT32 i=0; i<g_vPredefinedSMS.size(); i++)
    {
        if (g_vPredefinedSMS[i].ulSMSID == stInfo.ulSMSID)
        {
            memcpy(&g_vPredefinedSMS[i], &stInfo, sizeof(stInfo));
            bExist = TRUE;
            break;
        }
    }

    if (!bExist)
    {
        g_vPredefinedSMS.push_back(stInfo);
    }

    g_PredefinedSMSMutex.Unmutex();
}

//=============== user ========================
VOID InitRegisterInfo(VECTOR<REGISTER_INFO_T> &vInfo)
{
    VECTOR<DC_USER_INFO_T>  vDCUser;

    GetTrainAndDepotDCUserInfo(vDCUser);

    g_RegisterMutex.Mutex();
    g_mAPPRegisterInfo.Clear();

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        REGISTER_INFO_T     &stInfo = vInfo[i];

        // 注册通知未结束、注销（注销要重新通知DC）
        stInfo.ulDCNum = 0;
        if (!stInfo.bRegisterOver) // || !stInfo.bRegister)
        {
            stInfo.ulDCNum = MIN(vDCUser.size(), ARRAY_SIZE(stInfo.aulDCUser));
            for (UINT32 j=0; j<stInfo.ulDCNum; j++)
            {
                stInfo.aulDCUser[j] = vDCUser[j].ulUserID;
            }
        }

        g_mAPPRegisterInfo.Add(vInfo[i].ulTrainUnitID, &vInfo[i]);
    }

    g_RegisterMutex.Unmutex();
}

BOOL GetRegisterInfo(UINT32 ulTrainUnitID, REGISTER_INFO_T *pstInfo)
{
    g_RegisterMutex.Mutex();

    REGISTER_INFO_T *pstTmp = g_mAPPRegisterInfo.GetValue(ulTrainUnitID);

    if (pstTmp)
    {
        memcpy(pstInfo, pstTmp, sizeof(REGISTER_INFO_T));
    }

    g_RegisterMutex.Unmutex();

    return pstInfo != NULL;
}

VOID GetRegisterInfo(VECTOR<REGISTER_INFO_T> &vInfo)
{
    VECTOR<REGISTER_INFO_T*>    vTmp;

    g_RegisterMutex.Mutex();

    g_mAPPRegisterInfo.GetValueList(vTmp);
    vInfo.clear();
    for (UINT32 i=0; i<vTmp.size(); i++)
    {
        vInfo.push_back(*vTmp[i]);
    }

    g_RegisterMutex.Unmutex();
}

BOOL SetRegisterInfo(UINT32 ulTrainUnitID, BOOL bRegister, UINT32 ulRegisterDepotID, BOOL bAutoRegister)
{
    VECTOR<DC_USER_INFO_T>      vDCUser;

    GetTrainAndDepotDCUserInfo(vDCUser);

    AUTO_LOCK(g_RegisterMutex);

    REGISTER_INFO_T *pstInfo = g_mAPPRegisterInfo.GetValue(ulTrainUnitID);

    if (!pstInfo)
    {
        REGISTER_INFO_T stInfo = {0};

        stInfo.ulTrainUnitID = ulTrainUnitID;
        pstInfo = g_mAPPRegisterInfo.Add(ulTrainUnitID, &stInfo);
        if (!pstInfo)
        {
            return FALSE;
        }
    }
    else if (pstInfo->bRegister == bRegister &&
             pstInfo->ulRegisterDepotID == ulRegisterDepotID)
    {
        return TRUE;
    }

    pstInfo->bRegister = bRegister;
    pstInfo->ulRegisterDepotID = ulRegisterDepotID;
    pstInfo->bAutoRegister = bAutoRegister;
    pstInfo->ulTime = gos_get_current_time();
    pstInfo->ulDCNum = MIN(vDCUser.size(), ARRAY_SIZE(pstInfo->aulDCUser));
    for (UINT32 j=0; j<pstInfo->ulDCNum; j++)
    {
        pstInfo->aulDCUser[j] = vDCUser[j].ulUserID;
    }

    pstInfo->bRegisterOver = FALSE;

    return TRUE;
}

VOID SetRegisterOverInfo(UINT32 ulTrainUnitID, BOOL bRegister, UINT32 ulRegisterDepotID, UINT32 ulDCUserID)
{
    AUTO_LOCK(g_RegisterMutex);

    REGISTER_INFO_T *pstInfo = g_mAPPRegisterInfo.GetValue(ulTrainUnitID);

    if (!pstInfo)
    {
        return;
    }

    if (pstInfo->bRegister != bRegister ||
        pstInfo->ulRegisterDepotID != ulRegisterDepotID )
    {
        return;
    }

    for (UINT32 i=0; i<pstInfo->ulDCNum; i++)
    {
        if (pstInfo->aulDCUser[i] == ulDCUserID)
        {
            for (UINT32 j=i+1; j<pstInfo->ulDCNum; j++)
            {
                pstInfo->aulDCUser[j-1] = pstInfo->aulDCUser[j];
            }

            pstInfo->ulDCNum--;
            if (pstInfo->ulDCNum == 0)
            {
                pstInfo->bRegisterOver = TRUE;
            }
            break;
        }
    }
}

BOOL IsRegisterOver(UINT32 ulTrainUnitID)
{
    AUTO_LOCK(g_RegisterMutex);

    REGISTER_INFO_T *pstInfo = g_mAPPRegisterInfo.GetValue(ulTrainUnitID);

    if (!pstInfo)
    {
        return TRUE;
    }

    return pstInfo->bRegisterOver;
}

//=============== Multi Group ==============================================
VOID InitMultiGroupInfo(VECTOR<MULTI_GROUP_INFO_T> &vInfo)
{
    AUTO_LOCK(g_MultiGroupMutex);

    g_vMultiGroupInfo.clear();
    g_vMultiGroupInfo.assign(vInfo.begin(), vInfo.end());
}

VOID GetMultiGroupInfo(VECTOR<MULTI_GROUP_INFO_T> &vInfo)
{
    AUTO_LOCK(g_MultiGroupMutex);

    vInfo.clear();
    vInfo.assign(g_vMultiGroupInfo.begin(), g_vMultiGroupInfo.end());
}

BOOL GetMultiGroupInfo(INT64 i64GroupID, MULTI_GROUP_INFO_T &stInfo)
{
    AUTO_LOCK(g_MultiGroupMutex);

    for (INT32 i=0; i<g_vMultiGroupInfo.size(); i++)
    {
        if (g_vMultiGroupInfo[i].i64GroupID == i64GroupID)
        {
            memcpy(&stInfo, &g_vMultiGroupInfo[i], sizeof(stInfo));
            return TRUE;
        }
    }

    return FALSE;
}

VOID ClearMultiGroupInfo(UINT32 ulDCUserID)
{
    AUTO_LOCK(g_MultiGroupMutex);

    for (INT32 i=g_vMultiGroupInfo.size()-1; i>=0; i--)
    {
        if (g_vMultiGroupInfo[i].ulDCUserID == ulDCUserID)
        {
            g_vMultiGroupInfo.erase(g_vMultiGroupInfo.begin()+i);
        }
    }
}

VOID SetMultiGroupInfo(VECTOR<MULTI_GROUP_INFO_T> &vInfo)
{
    AUTO_LOCK(g_MultiGroupMutex);

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        g_vMultiGroupInfo.push_back(vInfo[i]);
    }
}

//=============== APP Basic Cfg  ==============================================
VOID InitAPPBasicCfg(APP_BASIC_CFG_T &stCfg)
{
    AUTO_LOCK(g_APPBasicCfgMutex);

    memcpy(&g_stAPPBasicCfg, &stCfg, sizeof(stCfg));
}

VOID GetAPPBasicCfg(APP_BASIC_CFG_T &stCfg)
{
    AUTO_LOCK(g_APPBasicCfgMutex);

    memcpy(&stCfg, &g_stAPPBasicCfg, sizeof(stCfg));
}

// ==========================================================================
VOID ConvertGroupIDToJson(VECTOR<INT64> &vGroup, GJsonTupleParam &JsonTupleParam)
{
    JsonTupleParam.Clear();

    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        GJsonParam      JsonParam;

        JsonParam.Add("GroupID", vGroup[i]);

        JsonTupleParam.Add(JsonParam);
    }
}

VOID ConvertUserIDToJson(VECTOR<UINT32> &vUserID, GJsonTupleParam &JsonTupleParam)
{
    JsonTupleParam.Clear();

    for (UINT32 i=0; i<vUserID.size(); i++)
    {
        GJsonParam      JsonParam;

        JsonParam.Add("UserID", vUserID[i]);

        JsonTupleParam.Add(JsonParam);
    }
}
/*
VOID ConvertStationToJson(VECTOR<STATION_INFO_T> &vStation, GJsonTupleParam &JsonTupleParam)
{
    JsonTupleParam.Clear();

    for (UINT32 i=0; i<vStation.size(); i++)
    {
        STATION_INFO_T  &stStation = vStation[i];
        GJsonParam      JsonParam;

        JsonParam.Add("StationIndex", stStation.ulStationIndex);
        JsonParam.Add("StationID", stStation.ulStationID);
        JsonParam.Add("StationName", stStation.acStationName);
        JsonParam.Add("StationType", stStation.ulStationType);
        JsonParam.Add("DepotID", stStation.ulDepotID);
        JsonParam.Add("InterLockStationID", stStation.ulInterLockStationID);
        JsonParam.Add("StationGroupID", stStation.i64StationGroupID);

        JsonTupleParam.Add(JsonParam);
    }
}*/

UINT32 GetTrainStatus(TRAIN_INFO_T  &stTrain)
{
    UINT32  ulCurrTime = gos_get_uptime();
    UINT32  ulTimeout = 30;
    UINT32  ulTrainStatus = TRAIN_STATUS_ONLINE;
    UINT32  ulTimeDiff = (UINT32)(ulCurrTime - stTrain.ulUptime);

    if (ulTimeDiff >= ulTimeout ||
        ulCurrTime >= (stTrain.ulAppReportTime + ulTimeout))
    {
        ulTrainStatus = TRAIN_STATUS_OFFLINE;
    }
    else
    {
        if (stTrain.astAppTrainInfo[0].ulCoverStatus == COVER_STATUS_CLOSE &&
            stTrain.astAppTrainInfo[1].ulCoverStatus == COVER_STATUS_CLOSE )
        {
            ulTrainStatus = TRAIN_STATUS_SILENT;
        }
    }

    return ulTrainStatus;
}

VOID ConvertTrainToJson(VECTOR<TRAIN_INFO_T> &vTrain, GJsonTupleParam &JsonTupleParam)
{
    UINT32      ulCurrTime = gos_get_uptime();
    UINT32      ulTimeDiff = 0xffffffff;
    UINT32      ulTrainStatus;

    JsonTupleParam.Clear();

    for (UINT32 i=0; i<vTrain.size(); i++)
    {
        TRAIN_INFO_T    &stTrain = vTrain[i];
        GJsonParam      JsonParam;
        INT32           iTrainOrder = GetTrainOrder(&stTrain);
        UINT32          ulLostRealLocation = GetLostRealLocation(stTrain.ulLostRealLocation, stTrain.ulUptime); 

        ulTimeDiff = (UINT32)(ulCurrTime - stTrain.ulUptime);
        ulTrainStatus = GetTrainStatus(stTrain);

        if (i == 0)
        {
            JsonParam.Add("ATSDataTimeDiff", GetATSDataTimeDiff());
        }

        JsonParam.Add("TimeDiff", ulTimeDiff);
//        JsonParam.Add("UpdateTime", stTrain.ulUpdateTime);
        JsonParam.Add("UnitID", stTrain.ulTrainUnitID);
        JsonParam.Add("Type", stTrain.ulTrainType);
        JsonParam.Add("Name", stTrain.acTrainName);
        JsonParam.Add("TrainID", stTrain.acTrainID);
        JsonParam.Add("TrainServiceNumber", 0); // TODO stTrain.acTrainSerialNumber);
        JsonParam.Add("TrainSerialNumber", 0); // TODO stTrain.acTrainSerialNumber);
        JsonParam.Add("DriverID", stTrain.ulDriverID);
        JsonParam.Add("ComeTime", stTrain.ulComeTime);
        JsonParam.Add("ParkTime", stTrain.ulParkTime);
        JsonParam.Add("LeaveTime", stTrain.ulLeaveTime);
        JsonParam.Add("DestID", stTrain.acDestID);
        JsonParam.Add("RollingStockNum", stTrain.ulRollingStockNum);
        JsonParam.Add("Direction", stTrain.ulDirection);
        JsonParam.Add("DriveMode", stTrain.ulDriveMode);
        JsonParam.Add("StationID", stTrain.ulStationID);
        JsonParam.Add("PrevStationID", stTrain.ulUpsideStationID);
        JsonParam.Add("NextStationID", stTrain.ulDownsideStationID);
        JsonParam.Add("TurnbackTrackFlag", stTrain.ulTurnbackTrackFlag);
        JsonParam.Add("DepotTrackFlag", stTrain.ulDepotTrackFlag);
        JsonParam.Add("SectionID", stTrain.usSectionID);
        JsonParam.Add("IndexInSection", stTrain.ucIndexInSection);
        JsonParam.Add("PlatformID", stTrain.ulPlatformID);
        JsonParam.Add("TransferTrackFlag", stTrain.ulTransferTrackFlag);
        JsonParam.Add("InOutDepot", stTrain.ulInOutDepot);
        JsonParam.Add("LostRealLocation", ulLostRealLocation);
        JsonParam.Add("TrainOrder", iTrainOrder);
        JsonParam.Add("TrackName", stTrain.acTrackName);
        JsonParam.Add("TrainStationPos", stTrain.iTrainStationPos);
        JsonParam.Add("DefaultGroupID", stTrain.i64DefaultGroupID);
        JsonParam.Add("Register", stTrain.bRegister);
        JsonParam.Add("RegisterDepotID", stTrain.ulRegisterDepotID);
        JsonParam.Add("MasterStatus", stTrain.stIPHAudioNetInfo.ulMasterStatus);
        JsonParam.Add("IPHStatus", stTrain.stIPHAudioNetInfo.ulIPHStatus);
        JsonParam.AddIP("IPHAddr", stTrain.stIPHAudioNetInfo.aucIPHAddr);

        JsonParam.Add("Lon", stTrain.dLon);
        JsonParam.Add("Lat", stTrain.dLat);
        JsonParam.Add("TrainStatus", ulTrainStatus);

        for (UINT32 j=0; j<ARRAY_SIZE(stTrain.astAppTrainInfo); j++)
        {
            GJsonParam          AppJsonParam;
            APP_TRAIN_INFO_T    &stTrainInfo = stTrain.astAppTrainInfo[j];

            AppJsonParam.Add("TrainID",         stTrainInfo.acTrainID);
            AppJsonParam.Add("TC",              stTrainInfo.ulTC);
            AppJsonParam.Add("CurrentGroupID",  stTrainInfo.i64CurrentGroupID);
            AppJsonParam.Add("StationID",       stTrainInfo.ulStationID);
            AppJsonParam.Add("CellID",          stTrainInfo.ulCellID);
            AppJsonParam.Add("CallType",        stTrainInfo.ulCallType);
            AppJsonParam.Add("CallID",          stTrainInfo.i64CallID);
            AppJsonParam.Add("CurrentTime",     stTrainInfo.ulCurrentTime);
            AppJsonParam.Add("Uptime",          stTrainInfo.ulUptime);
            AppJsonParam.Add("SerialNumber",    stTrainInfo.acSerialNumber);
            AppJsonParam.Add("APPVer",          stTrainInfo.acAPPVer);
            AppJsonParam.Add("IsIPHOnline",     stTrainInfo.bIsIPHOnline);
            AppJsonParam.Add("IPHStatus",       stTrainInfo.ulIPHStatus);
            AppJsonParam.Add("IsPAOnline",      stTrainInfo.bIsPAOnline);
            AppJsonParam.Add("PAStatus",        stTrainInfo.ulPAStatus);
            AppJsonParam.Add("IsMasterPA",      stTrainInfo.bIsMasterPA);

            JsonParam.Add(j==0?"AppTrainInfo1":"AppTrainInfo2", AppJsonParam);
        }

        JsonTupleParam.Add(JsonParam);
    }

    GosLog(LOG_DETAIL, "ATS info from %u seconds ago", ulTimeDiff);
}

/*
VOID ConvertPredefinedSMSToJson(VECTOR<PREDEFINED_SMS_INFO_T> &vInfo, GJsonTupleParam &JsonTupleParam)
{
    JsonTupleParam.Clear();

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        PREDEFINED_SMS_INFO_T   &stInfo = vInfo[i];
        GJsonParam              JsonParam;

        JsonParam.Add("SMSID", stInfo.ulSMSID);
        JsonParam.Add("SMSTemplateType", stInfo.ulSMSTemplateType);
        JsonParam.Add("SMSInfo", stInfo.acSMSInfo);

        JsonTupleParam.Add(JsonParam);
    }
}*/

UINT32 GetSafeQueryRecLen()
{
    UINT32  ulLen = GosGetSafeMsgSize() - 2*1024;

    return ulLen;
}

VOID ConvertIPHTalkToJson(VECTOR<IPH_TALK_INFO_T> &vInfo, BOOL bActiveIPHTalk, GJsonTupleParam &JsonTupleParam)
{
    JsonTupleParam.Clear();

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        IPH_TALK_INFO_T     &stInfo = vInfo[i];
        GJsonParam          JsonParam;

        if (bActiveIPHTalk && stInfo.ulStopTime > 0)
        {
            continue;
        }

        JsonParam.Add("ApplyTime", stInfo.ulApplyTime);
        JsonParam.Add("TrainUnitID", stInfo.ulTrainUnitID);
        JsonParam.Add("TrainID", stInfo.acTrainID);
        JsonParam.Add("CarriageID", stInfo.ulCarriageID);
        JsonParam.Add("IPHDevID", stInfo.ulIPHDevID);
        JsonParam.Add("AcceptDCUserID", stInfo.ulAcceptDCUserID);
        JsonParam.Add("AcceptTime", stInfo.ulAcceptTime);
        JsonParam.Add("StopTime", stInfo.ulStopTime);

        if (bActiveIPHTalk)
        {
            JsonParam.Add("Status", stInfo.ulStatus);
            JsonParam.AddIP("IPHDevIP", stInfo.aucIPHDevIP);
            JsonParam.Add("VideoURL", stInfo.acVideoURL);
        }

        JsonTupleParam.Add(JsonParam);

        if (JsonTupleParam.GetStringSize() >= GetSafeQueryRecLen())
        {
            break;
        }
    }
}
/*
VOID ConvertMultiGroupToJson(VECTOR<MULTI_GROUP_INFO_T> &vInfo, GJsonTupleParam &JsonTupleParam)
{
    JsonTupleParam.Clear();

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        MULTI_GROUP_INFO_T      &stInfo = vInfo[i];
        GJsonParam              JsonParam;

        JsonParam.Add("DCUserID", stInfo.ulDCUserID);
        JsonParam.Add("GroupID", stInfo.i64GroupID);
        JsonParam.Add("GroupName", stInfo.acGroupName);
        JsonParam.Add("GroupMember", stInfo.acGroupMember);

        JsonTupleParam.Add(JsonParam);
    }
}*/

/*
$GPGGA,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,M,<10>,M,<11>,<12>*hh
    <1> UTC时间，hhmmss（时分秒）格式
    <2> 纬度ddmm.mmmm（度分）格式（前面的0也将被传输）
    <3> 纬度半球N（北半球）或S（南半球）
    <4> 经度dddmm.mmmm（度分）格式（前面的0也将被传输）
    <5> 经度半球E（东经）或W（西经）
    <6> GPS状态：0=未定位，1=非差分定位，2=差分定位，6=正在估算
    <7> 正在使用解算位置的卫星数量（00~12）（前面的0也将被传输）
    <8> HDOP水平精度因子（0.5~99.9）
    <9> 海拔高度（-9999.9~99999.9）
    <10> 地球椭球面相对大地水准面的高度
    <11> 差分时间（从最近一次接收到差分信号开始的秒数，如果不是差分定位将为空）
    <12> 差分站ID号0000~1023（前面的0也将被传输，如果不是差分定位将为空）

    $GPGGA,053620.101,,,,,0,0,,,M,,M,,*4A$GPRMC
*/

// $GPGGA,053620.101,,,,,0,0,,,M,,M,,*4A$GPRMC,053620.101,V,,,,,0.00,0.00,260315,,,N*4C$GPVTG,0.00,T,,M,0.00,N,0.0,K,N*02
// $GPGGA,023351.00,2218.64996,N,11413.56335,E,1,06,1.27,119.6,M,-1.5,M,,,1000*6E
BOOL ParseGPSGGAData(CHAR *szGPS, GPS_INFO_T *pstGPS)
{
    CHAR    *szNewGPS;
    CHAR    *szStart;
    CHAR    *szEnd;
    DOUBLE  dTmp;
    BOOL    bRet = FALSE;

    if (*szGPS == '$')
    {
        szGPS ++;
    }

    szStart = strstr(szGPS, "GPGGA");
    if (!szStart)
    {
        return FALSE;
    }

    szNewGPS = (CHAR*)malloc(strlen(szStart)+2);
    sprintf(szNewGPS, "%s%c", szStart, ',');
    szStart = szNewGPS;

    for (UINT32 i=0; i<=15; i++)
    {
        szEnd = strchr(szStart, ',');
        if (!szEnd)
        {
            return FALSE;
        }

        *szEnd++ = '\0';
        if (i == 0 && strcmp(szStart, "GPGGA") != 0)
        {
            return FALSE;
        }
        else if (i == 2)    // 纬度ddmm.mmmm（度分）格式
        {
            if (!gos_atof(szStart, &dTmp))
            {
                goto end;
            }

            dTmp /= 100;

            pstGPS->dLat = dTmp;
        }
        else if (i == 3)    // 纬度半球N（北半球）或S（南半球）
        {
            if (*szStart == 'N')
            {
                pstGPS->ucNorthLat = TRUE;
            }
            else if (*szStart == 'S')
            {
                pstGPS->ucNorthLat = FALSE;
            }
            else
            {
                goto end;
            }
        }
        else if (i == 4)    // 经度
        {
            if (!gos_atof(szStart, &dTmp))
            {
                goto end;
            }

            dTmp /= 100;

            pstGPS->dLon = dTmp;
        }
        else if (i == 5)    // 经度半球E（东经）或W（西经）
        {
            if (*szStart == 'E')
            {
                pstGPS->ucEastLon = TRUE;
            }
            else if (*szStart == 'W')
            {
                pstGPS->ucEastLon = FALSE;
            }
            else
            {
                goto end;
            }
        }
        else if (i == 9)
        {
            if (!gos_atof(szStart, &dTmp))
            {
                return FALSE;
            }

            pstGPS->fHeight = (FLOAT)dTmp;
        }
        else if (i == 15)    // DeviceID, 1000*56
        {
            szEnd = strchr(szStart, '*');
            if (!szEnd)
            {
                goto end;
            }

            UINT32  ulLen = szEnd - szStart;
            CHAR    acDevID[32];

            if (ulLen > 10)
            {
                goto end;
            }

            memset(acDevID, 0, sizeof(acDevID));
            memcpy(acDevID, szStart, ulLen);

            if (!gos_atou(acDevID, &pstGPS->ulDevID))
            {
                goto end;
            }

            bRet = TRUE;
            break;
        }

        szStart = szEnd;
    }

end:
    GOS_FREE(szNewGPS);
    return bRet;
}

/*
$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*hh
    <1> UTC时间，hhmmss（时分秒）格式
    <2> 定位状态，A=有效定位，V=无效定位
    <3> 纬度ddmm.mmmm（度分）格式（前面的0也将被传输）
    <4> 纬度半球N（北半球）或S（南半球）
    <5> 经度dddmm.mmmm（度分）格式（前面的0也将被传输）
    <6> 经度半球E（东经）或W（西经）
    <7> 地面速率（000.0~999.9节，前面的0也将被传输）
    <8> 地面航向（000.0~359.9度，以真北为参考基准，前面的0也将被传输）
    <9> UTC日期，ddmmyy（日月年）格式
    <10> 磁偏角（000.0~180.0度，前面的0也将被传输）
    <11> 磁偏角方向，E（东）或W（西）
    <12> 模式指示（仅NMEA0183 3.00版本输出，A=自主定位，D=差分，E=估算，N=数据无效）
    <13> DeviceID
*/
// GPS数据格式: $GPRMC,053620.101,V,,,,,0.00,0.00,260315,,,N*4C
// $GPRMC,023351.00,A,2218.64996,N,11413.56335,E,0.020,,111021,,,A,1000*56

BOOL ParseGPSRMCData(CHAR *szGPS, GPS_INFO_T *pstGPS)
{
    BOOL    bRet = FALSE;
    CHAR    *szNewGPS = NULL;
    CHAR    *szStart;
    CHAR    *szEnd;
    DOUBLE  dTmp;
    INT32   iTime;
    INT32   iDate;
    INT32   aiTmp[3];
    INT32   iTimeZone = gos_get_timezone();
    GOS_DATETIME_T  stTime = {0};

   // strcpy(szGPS, "GPRMC,053620.101,A,1234.5678,N,2345.6789,E,345.6,290.3,260315,<10>,<11>,<12>*hh");
    if (*szGPS == '$')
    {
        szGPS ++;
    }

    szStart = strstr(szGPS, "GPRMC");
    if (!szStart)
    {
        return FALSE;
    }

    szNewGPS = (CHAR*)malloc(strlen(szStart)+2);
    sprintf(szNewGPS, "%s%c", szStart, ',');
    szStart = szNewGPS;

    for (UINT32 i=0; i<=13; i++)
    {
        szEnd = strchr(szStart, ',');
        if (!szEnd)
        {
            goto end;
        }

        *szEnd++ = '\0';
        if (i == 0 && strcmp(szStart, "GPRMC") != 0)
        {
            goto end;
        }
        else if (i == 1)    // UTC时间，hhmmss（时分秒）格式
        {
            if (!gos_atof(szStart, &dTmp))
            {
                goto end;
            }

            iTime = (INT32)dTmp;

            aiTmp[0] = iTime/10000;
            aiTmp[1] = MOD(iTime/100, 100);
            aiTmp[2] = MOD(iTime, 100);
            if (aiTmp[0] > 23 || aiTmp[1] > 59 || aiTmp[2] > 59)
            {
                goto end;
            }

            stTime.ucHour = (UINT8)aiTmp[0];
            stTime.ucMinute = (UINT8)aiTmp[1];
            stTime.ucSecond = (UINT8)aiTmp[2];
        }
        else if (i == 2)    // 定位状态，A=有效定位，V=无效定位
        {
            if (*szStart != 'A')
            {
                goto end;
            }
        }
        else if (i == 3)    // 纬度ddmm.mmmm（度分）格式
        {
            if (!gos_atof(szStart, &dTmp))
            {
                goto end;
            }

            dTmp /= 100;

            pstGPS->dLat = dTmp;
        }
        else if (i == 4)    // 纬度半球N（北半球）或S（南半球）
        {
            if (*szStart == 'N')
            {
                pstGPS->ucNorthLat = TRUE;
            }
            else if (*szStart == 'S')
            {
                pstGPS->ucNorthLat = FALSE;
            }
            else
            {
                goto end;
            }
        }
        else if (i == 5)    // 经度
        {
            if (!gos_atof(szStart, &dTmp))
            {
                goto end;
            }

            dTmp /= 100;

            pstGPS->dLon = dTmp;
        }
        else if (i == 6)    // 经度半球E（东经）或W（西经）
        {
            if (*szStart == 'E')
            {
                pstGPS->ucEastLon = TRUE;
            }
            else if (*szStart == 'W')
            {
                pstGPS->ucEastLon = FALSE;
            }
            else
            {
                goto end;
            }
        }
        else if (i == 7)    // 地面速率（000.0~999.9节）
        {
            if (!gos_atof(szStart, &dTmp))
            {
                goto end;
            }

            pstGPS->fSpeed = (FLOAT)dTmp;
        }
        else if (i == 8)    // 地面航向（000.0~359.9度，以真北为参考基准
        {
            if (*szStart == '\0')
            {
                dTmp = 0;
            }
            else if (!gos_atof(szStart, &dTmp))
            {
                goto end;
            }

            pstGPS->fAzimuth = (FLOAT)dTmp;
        }
        else if (i == 9)    // UTC日期，ddmmyy（日月年）格式
        {
            if (!gos_atoi(szStart, &iDate))
            {
                goto end;
            }

            aiTmp[0] = MOD(iDate, 100);     // 年
            aiTmp[1] = MOD(iDate/100, 100); // 月
            aiTmp[2] = iDate/10000;         // 日
            if (aiTmp[0] < 0 ||
                aiTmp[1] <= 0 || aiTmp[1] > 12 ||
                aiTmp[2] <= 0 || aiTmp[2] > 31 )
            {
                return FALSE;
            }

            stTime.usYear = (UINT16)(2000+aiTmp[0]);
            stTime.ucMonth = (UINT8)aiTmp[1];
            stTime.ucDay = (UINT8)aiTmp[2];

            pstGPS->ulTime = gos_mktime(&stTime);
            pstGPS->ulTime += iTimeZone;
        }
        else if (i == 13)    // DeviceID, 1000*56
        {
            szEnd = strchr(szStart, '*');
            if (!szEnd)
            {
                goto end;
            }

            UINT32  ulLen = szEnd - szStart;
            CHAR    acDevID[32];

            if (ulLen > 10)
            {
                goto end;
            }

            memset(acDevID, 0, sizeof(acDevID));
            memcpy(acDevID, szStart, ulLen);

            if (!gos_atou(acDevID, &pstGPS->ulDevID))
            {
                goto end;
            }

            bRet = TRUE;
            break;
        }

        szStart = szEnd;
    }

end:
    GOS_FREE(szNewGPS);
    return bRet;
}

UINT32 GetSecOfToday()
{
    UINT32  ulTime = gos_get_current_time();
    GOS_DATETIME_T  stTime;

    gos_get_localtime(&ulTime, &stTime);
    stTime.ucHour = 0;
    stTime.ucSecond = 0;
    stTime.ucMinute = 0;

    ulTime = gos_mktime(&stTime);

    return ulTime;
}

// ">RPV10481+2231090+1142259100000002;ID=1801;*74<
// 10481(当天的秒）， 2231090（小数点后5位置）11422591（小数点后5位置），000（速度），000（），0（源），2（0：无效）
BOOL ParseRPVData(CHAR *szGPS, GPS_INFO_T &stGPS)
{
    BOOL    bRet = FALSE;
    CHAR    *szStart = strstr(szGPS, "RPV");

    if (!szStart)
    {
        return FALSE;
    }

    szStart += strlen("RPV");

    CHAR    *szNewGPS = strdup(szStart);
    CHAR    *szTmp;
    CHAR    *szID;
    CHAR    *szLat;
    CHAR    *szLon;
    CHAR    *szTime;
    CHAR    *szSpeed;

    szStart = szNewGPS;
    szTime = gos_left_trim_string(szStart);
    szTmp = strchr(szTime, ';');
    if (!szTmp)
    {
        goto end;
    }

    *szTmp++ = '\0';
    szID = gos_left_trim_string(szTmp);

    szLat = strchr(szStart, '+');
    if (!szLat)
    {
        goto end;
    }

    *szLat++ = '\0';

    if (!gos_atou(szTime, &stGPS.ulTime))
    {
        goto end;
    }

    stGPS.ulTime += GetSecOfToday();

    szTmp = strchr(szLat, '+');
    if (!szTmp)
    {
        goto end;
    }

    *szTmp = '\0';
    szLon = szTmp+1;

    if (!gos_atof(szLat, &stGPS.dLat))
    {
        goto end;
    }

    stGPS.dLat /= 100000;
    stGPS.ucNorthLat = TRUE;

    if (strlen(szLon) < 10)
    {
        goto end;
    }

    szSpeed = szLon + strlen(szLon) - 8;
    *szSpeed = '\0';

    if (szSpeed[7] == '0')
    {
        goto end;
    }

    if (!gos_atof(szLon, &stGPS.dLon))
    {
        goto end;
    }

    stGPS.dLon /= 100000;
    stGPS.ucEastLon = TRUE;

    // ID
    if (!gos_str_like(szID, "ID"))
    {
        goto end;
    }

    szID = gos_left_trim_string(szID+2);
    if (*szID != '=')
    {
        goto end;
    }

    szID ++;
    szTmp = strchr(szID, ';');
    if (!szTmp)
    {
        goto end;
    }
    *szTmp = '\0';

    if (!gos_atou(szID, &stGPS.ulDevID))
    {
        goto end;
    }

    bRet = TRUE;

end:
    free(szNewGPS);
    return bRet;
}

// >RCP10481+223109+114225902;ID=1801;*60<
BOOL ParseRCPData(CHAR *szGPS, GPS_INFO_T &stGPS)
{
    BOOL    bRet = FALSE;
    CHAR    *szStart = strstr(szGPS, "RCP");

    if (!szStart)
    {
        return FALSE;
    }

    szStart += strlen("RCP");

    CHAR    *szNewGPS = strdup(szStart);
    CHAR    *szTmp;
    CHAR    *szID;
    CHAR    *szLat;
    CHAR    *szLon;
    CHAR    *szTime;

    szStart = szNewGPS;
    szTime = gos_left_trim_string(szStart);
    szTmp = strchr(szTime, ';');
    if (!szTmp)
    {
        goto end;
    }

    *szTmp++ = '\0';
    szID = gos_left_trim_string(szTmp);

    szLat = strchr(szStart, '+');
    if (!szLat)
    {
        goto end;
    }

    *szLat++ = '\0';

    if (!gos_atou(szTime, &stGPS.ulTime))
    {
        goto end;
    }

    stGPS.ulTime += GetSecOfToday();

    szTmp = strchr(szLat, '+');
    if (!szTmp)
    {
        goto end;
    }

    *szTmp = '\0';
    szLon = szTmp+1;

    if (!gos_atof(szLat, &stGPS.dLat))
    {
        goto end;
    }

    stGPS.dLat /= 10000;
    stGPS.ucNorthLat = TRUE;

    if (!gos_atof(szLon, &stGPS.dLon))
    {
        goto end;
    }

    stGPS.dLon /= 1000000;
    stGPS.ucEastLon = TRUE;

    // ID
    if (!gos_str_like(szID, "ID"))
    {
        goto end;
    }

    szID = gos_left_trim_string(szID+2);
    if (*szID != '=')
    {
        goto end;
    }

    szID ++;
    szTmp = strchr(szID, ';');
    if (!szTmp)
    {
        goto end;
    }
    *szTmp = '\0';

    if (!gos_atou(szID, &stGPS.ulDevID))
    {
        goto end;
    }

    bRet = TRUE;

end:
    free(szNewGPS);
    return bRet;
}

BOOL ParseGPSInfo(CHAR *szGPS, GPS_INFO_T &stGPS)
{
    memset(&stGPS, 0, sizeof(stGPS));

    if (strstr(szGPS, "GPGGA") &&
        ParseGPSGGAData(szGPS, &stGPS))
    {
        return TRUE;
    }

    if (strstr(szGPS, "GPRMC") &&
        ParseGPSRMCData(szGPS, &stGPS))
    {
        return TRUE;
    }

    // ">RPV10481+2231090+1142259100000002;ID=1801;*74<>RCP10481+223109+114225902;ID=1801;*60<";
    if (strstr(szGPS, "RPV") &&
        ParseRPVData(szGPS, stGPS))
    {
        return TRUE;
    }

    if (strstr(szGPS, "RCP") &&
        ParseRCPData(szGPS, stGPS))
    {
        return TRUE;
    }

    return FALSE;
}

VOID AddGPSInfo(GPS_INFO_T &stGPS)
{
    AUTO_LOCK(g_TrainMutex);

    TRAIN_INFO_T    *pstTrain = g_mTrainInfo.GetValue(stGPS.ulDevID);

    if (!pstTrain)
    {
        return;
    }

    pstTrain->dLon = stGPS.dLon;
    pstTrain->dLat = stGPS.dLat;
}

/*
BOOL GetGPSInfo(UINT32 ulDevID, GPS_INFO_T &stGPS)
{
    AUTO_LOCK(g_GPSMutex);

    GPS_INFO_T  *pstGPS = g_mGPSInfo.GetValue(ulDevID);

    if (!pstGPS)
    {
        return FALSE;
    }

    memcpy(&stGPS, pstGPS, sizeof(stGPS));
    return TRUE;
}*/

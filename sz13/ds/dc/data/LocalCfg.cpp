#include "ClientInclude.h"
#include "ClientData.h"

#ifndef DC_VER
#define DC_VER      "V2.0.0b"
#endif

#ifndef METRO_NAME
#define METRO_NAME  "深圳市地铁13号线"
#endif

LocalCfg::LocalCfg()
{
    m_bIsTcpMode = true;
    m_bIsMrsTcpMode = true;
    m_bIsShowCallWin = true;
    m_bShowTitle = false;
    m_bTestMode = false;
    m_bAutoLoginDIS = false;

    m_ulDataReserveDay = 366;    // 1年
    m_ulAutoLockPeriod = 3600;   // 1小时

    /// 设置配置文件的路径
    m_strLocalConfFileName.sprintf("%s/%s", gos_get_root_path(), "dc.ini");
    /// 设置日志模块的默认配置
    m_LogCfg.bLogToFile = TRUE;
    m_LogCfg.bLogToStdout = FALSE;
    m_LogCfg.ulLogBufTime = 0;
    m_LogCfg.ulLogFileNum = 30;
    m_LogCfg.ulLogFileSize = 4 * 1024 * 1024;
    m_LogCfg.ulLogLevel = LOG_DETAIL;
    m_LogCfg.bLogDevInfo = FALSE;

    m_strFontName = "";
    m_iFontSize = 11;

    /// 设置版本和应用标题
    m_strVersion = DC_VER;
    m_strAppTitle = AnsiString(METRO_NAME) + "无线集群调度系统";

    /// 设置铃声文件路径
    m_strEmergencyCallAlarmFile.sprintf("%s/%s", gos_get_root_path(), "alarm.wav");
    m_strIPHAlarmFile.sprintf("%s/%s", gos_get_root_path(), "iph.wav");
    m_strReqCallAlarmFile.sprintf("%s/%s", gos_get_root_path(), "reqcall.wav");
    m_strSDSAlarmFile.sprintf("%s/%s", gos_get_root_path(), "sds.wav");
}

BOOL LocalCfg::Load(AnsiString strLocalConfFileName)
{
    m_strLocalConfFileName = strLocalConfFileName;

    CHAR        *szValue;
    INT32       iValue;
    UINT32      ulValue;
    GConfFile   Conf(m_strLocalConfFileName.c_str());

    std::string strConfFile = Conf.GetString();
    GosLog(LOG_INFO, "%s:\n%s", m_strLocalConfFileName.c_str(), strConfFile.c_str());

    szValue = Conf.Get("Sys", "dc_type");
    if (szValue)
    {
        if (gos_atou(szValue, &ulValue))
        {
            APP::SetDCType(ulValue);
        }
    }

    szValue = Conf.Get("Sys", "depot");
    if (szValue)
    {
        if (gos_atou(szValue, &ulValue))
        {
            APP::SetDepotID(ulValue);
        }
    }

    szValue = Conf.Get("Sys", "dc_number");
    if (!szValue)
    {
        GosLog(LOG_ERROR, "get dc_number of Sys failed");
        return FALSE;
    }

    if (!gos_atou(szValue, &ulValue))
    {
        GosLog(LOG_ERROR, "invalid dc_number(%s) of Sys", szValue);
        return FALSE;
    }

    APP::SetSDKAccount(ulValue);

    szValue = Conf.Get("Sys", "mdc_ip");
    memset(&m_stMdcIP, 0, sizeof(m_stMdcIP));
    if (!gos_get_ip_port(szValue, m_stMdcIP.aucIP, &m_stMdcIP.usPort))
    {
        GosLog(LOG_ERROR, "get mdc_ip of Sys failed");
        return FALSE;
    }

    if (m_stMdcIP.usPort == 0)
    {
        m_stMdcIP.usPort = DEFAULT_MDC_PORT;
    }

    szValue = Conf.Get("Sys", "omc_ip");
    memset(&m_stOmcIP, 0, sizeof(m_stOmcIP));
    if (szValue)
    {
        if (!gos_get_ip(szValue, m_stOmcIP.aucIP) )
        {
            GosLog(LOG_ERROR, "get omc_ip of Sys failed");
            return FALSE;
        }
    }

    szValue = Conf.Get("Sys", "mrs_ip");
    memset(&m_stMrsIP, 0, sizeof(m_stMrsIP));
    if (szValue)
    {
        if (!gos_get_ip_port(szValue, m_stMrsIP.aucIP, &m_stMrsIP.usPort))
        {
            GosLog(LOG_ERROR, "get mrs_ip of Sys failed");
            return FALSE;
        }
    }

    szValue = Conf.Get("Sys", "dis_ip");
    memset(&m_stDisIP, 0, sizeof(m_stDisIP));
    if (!gos_get_ip_port(szValue, m_stDisIP.aucIP, &m_stDisIP.usPort))
    {
        GosLog(LOG_ERROR, "get dis_ip of Sys failed");
        return FALSE;
    }

    if (m_stDisIP.usPort == 0)
    {
        m_stDisIP.usPort = DEFAULT_DIS_PORT;
    }

    Conf.Get("Sys", "tcp_mode", true, m_bIsTcpMode);
    if (!Conf.Get("Sys", "mrs_tcp_mode"))
    {
        m_bIsMrsTcpMode = m_bIsTcpMode;
    }
    else
    {
        Conf.Get("Sys", "mrs_tcp_mode", true, m_bIsMrsTcpMode);
    }

    // 自动锁屏时间
    szValue = Conf.Get("Sys", "lock_window_time");
    if (szValue &&
        gos_atoi(szValue, &iValue))
    {
        if (iValue > 0 && iValue < 60)
        {
            iValue = 60;
        }
        else if (iValue > 86400*30 || iValue < 0)
        {
            iValue = 0;
        }

        m_ulAutoLockPeriod = iValue;
    }

    // 数据保留天数
    szValue = Conf.Get("Sys", "data_resv_day");
    if (szValue)
    {
        gos_atoi(szValue, &iValue);
        if (iValue < 30)
        {
            iValue = 30;
        }
        else if (iValue > 3*366)
        {
            iValue = 3*366;
        }

        m_ulDataReserveDay = iValue;
    }

    // font_size
    szValue = Conf.Get("Sys", "font_size");
    if (szValue)
    {
        gos_atoi(szValue, &iValue);
        if (iValue < 9)
        {
            iValue = 9;
        }
        else if (iValue > 14)
        {
            iValue = 14;
        }

        m_iFontSize = iValue;
    }

    // font_name
    szValue = Conf.Get("Sys", "font_name");
    if (szValue)
    {
        m_strFontName = szValue;
    }

    szValue = Conf.Get("Sys", "show_title");
    if (szValue && strcasecmp(szValue, "true") == 0)
    {
        m_bShowTitle = TRUE;
    }

    return GetLocalLogCfg(Conf);
}

/**
 * @brief           读取调度台日志模块的配置
 * @param           Conf    [in]
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-08-27 10:45:54
 * @note
 */
BOOL LocalCfg::GetLocalLogCfg(GConfFile &Conf)
{
    LOG_CFG_T st;

    memcpy(&st, &m_LogCfg, sizeof(st));

    if (!Conf.Get("Log", "log_level", LOG_INFO, &st.ulLogLevel))
    {
        GosLog(LOG_ERROR, "get log_level of cfg failed");
        return FALSE;
    }

    if (st.ulLogLevel > LOG_FATAL)
    {
        st.ulLogLevel = LOG_FATAL;
    }

    if (!Conf.Get("Log", "log_file_num", 90, &st.ulLogFileNum))
    {
        GosLog(LOG_ERROR, "get log_file_num of cfg failed!");
        return FALSE;
    }

    if (st.ulLogFileNum < 2)
    {
        st.ulLogFileNum = 2;
    }
    else if (st.ulLogFileNum > 100)
    {
        st.ulLogFileNum = 100;
    }

    if (!Conf.Get("Log", "log_file_size", 4096, &st.ulLogFileSize))
    {
        GosLog(LOG_ERROR, "get log_file_size of cfg failed!");
        return FALSE;
    }

    if (st.ulLogFileSize < 1024)
    {
        st.ulLogFileSize = 1024;
    }
    else if (st.ulLogFileSize > 102400)
    {
        st.ulLogFileSize = 102400;
    }

    st.ulLogFileSize *= 1024;

    if (!Conf.Get("Log", "log_buf_time", 0, &st.ulLogBufTime))
    {
        GosLog(LOG_ERROR, "get log_buf_time of cfg failed!");
        return FALSE;
    }

    // 缺省不打印研发调试信息
    Conf.GetBool("log_dev_info", FALSE, &st.bLogDevInfo);

    memcpy(&m_LogCfg, &st, sizeof(st));

    return TRUE;
}

IP_ADDR_T& LocalCfg::GetMdcIP()
{
    return m_stMdcIP;
}

IP_ADDR_T& LocalCfg::GetMrsIP()
{
    return m_stMrsIP;
}

IP_ADDR_T& LocalCfg::GetDisIP()
{
    return m_stDisIP;
}

IP_ADDR_T& LocalCfg::GetOmcIP()
{
    return m_stOmcIP;
}

bool LocalCfg::IsOmcIPEmpty()
{
    return GET_INT(m_stOmcIP.aucIP) == 0;
}

bool LocalCfg::GetTcpMode()
{
    return m_bIsTcpMode;
}

bool LocalCfg::GetMrsTcpMode()
{
    return m_bIsMrsTcpMode;
}

LOG_CFG_T& LocalCfg::GetLogCfg()
{
    return m_LogCfg;
}

AnsiString LocalCfg::GetDCVersion()
{
    return m_strVersion;
}

AnsiString LocalCfg::GetAppTitle()
{
    return m_strAppTitle;
}

const char* LocalCfg::GetAlarmFileEmergencyCall()
{
    return m_strEmergencyCallAlarmFile.c_str();
}

const char* LocalCfg::GetAlarmFileReqCall()
{
    return m_strReqCallAlarmFile.c_str();
}

const char* LocalCfg::GetAlarmFileSDS()
{
    return m_strSDSAlarmFile.c_str();
}

const char* LocalCfg::GetAlarmFileIPH()
{
    return m_strIPHAlarmFile.c_str();
}

void LocalCfg::SetLocalConfFileName(AnsiString strConfFileName)
{
    m_strLocalConfFileName = strConfFileName;
}

const char* LocalCfg::GetLocalConfFileName()
{
    return m_strLocalConfFileName.c_str();
}

void LocalCfg::SetAppTitle(AnsiString strAppTitle)
{
    m_strAppTitle = strAppTitle;
}

bool LocalCfg::GetAutoLoginDIS()
{
    return m_bAutoLoginDIS;
}

void LocalCfg::SetAutoLoginDIS(bool bAutoLoginDIS)
{
    m_bAutoLoginDIS = bAutoLoginDIS;
}

bool LocalCfg::NeedLogin()
{
    return GET_INT(m_stMdcIP.aucIP) != 0;
}

bool LocalCfg::GetTestMode()
{
    return m_bTestMode;
}

void LocalCfg::SetTestMode(bool bTestMode)
{
    m_bTestMode = bTestMode;
}

bool LocalCfg::GetIsShowCallWin()
{
    return m_bIsShowCallWin;
}

void LocalCfg::SetIsShowCallWin(bool b)
{
    m_bIsShowCallWin = b;
}

void LocalCfg::SaveLoginInfo(CHAR *szUserName, UINT32 ulDCType, UINT32 ulDepotID)
{
    if (!gos_file_exist(m_strLocalConfFileName.c_str()))
    {
        return;
    }

    GConfFile Conf(m_strLocalConfFileName.c_str());

    Conf.Set("Sys", "dc_type", ulDCType);
    Conf.Set("Sys", "depot", ulDepotID);

    Conf.Save();
}

// 数据保留最大时间
UINT32 LocalCfg::GetDataReserveDay()
{
    return m_ulDataReserveDay;
}

UINT32 LocalCfg::GetAutoLockPeriod()
{
    return m_ulAutoLockPeriod;
}


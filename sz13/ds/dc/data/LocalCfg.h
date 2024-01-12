#ifndef DC_DATA_LOCAL_CFG_H__
#define DC_DATA_LOCAL_CFG_H__

#include "g_include.h"

class LocalCfg
{
public:
    static LocalCfg &GetInstance()
    {
        static LocalCfg *pInstance = NULL;
        if (pInstance == NULL)
        {
            pInstance = new LocalCfg();
        }
        return *pInstance;
    }

    BOOL Load(AnsiString strLocalConfFileName);

     /**
     * @brief           获取核心网地址
     * @return          IPV4_INFO_C
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-08-26 18:17:29
     * @note
     */
    IP_ADDR_T& GetMdcIP();
    IP_ADDR_T& GetMrsIP();

    /**
     * @brief           获取调度服务器地址
     * @return          IPV4_INFO_C
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-08-26 18:18:14
     * @note
     */
    IP_ADDR_T& GetDisIP();

    /**
     * @brief           获取 Omc 地址
     * @return          IPV4_INFO_C
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-01 09:29:22
     * @note
     */
    IP_ADDR_T& GetOmcIP();

    /**
     * @brief           获取 omc 地址是否为空
     * @return          true    为空
     * @return          false   不为空
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-01 10:01:47
     * @note
     */
    bool IsOmcIPEmpty();

    /**
     * @brief           获取 tcp 模式
     * @return          true
     * @return          false
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-08-27 09:23:00
     * @note
     */
    bool GetTcpMode();
    bool GetMrsTcpMode();

    /**
     * @brief           获取日志配置
     * @return          LOG_CFG_T
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-08-27 09:43:44
     * @note
     */
    LOG_CFG_T& GetLogCfg();

    /**
     * @brief           获取调度台版本
     * @return          std::string
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-08-27 14:16:18
     * @note
     */
    AnsiString GetDCVersion();

    /**
     * @brief           获取整个 App 的标题
     * @return          std::string
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-08-27 14:23:38
     * @note
     */
    AnsiString GetAppTitle();

    /**
     * @brief           获取紧急呼叫的铃声文件（绝对路径加文件名称）
     * @return          std::string
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-08-31 17:07:18
     * @note
     */
    const char* GetAlarmFileEmergencyCall();

    /**
     * @brief           获取请呼的铃声文件（绝对路径加文件名称）
     * @return          std::string
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-08-31 17:08:12
     * @note
     */
    const char* GetAlarmFileReqCall();

    /**
     * @brief           获取 SDS 的铃声文件（绝对路径加文件名称）
     * @return          std::string
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-08-31 17:08:34
     * @note
     */
    const char* GetAlarmFileSDS();
    const char* GetAlarmFileIPH();

    /**
     * @brief           设置配置文件的名称
     * @param           strConfFileName    [in]
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-01 10:14:46
     * @note
     */
    void SetLocalConfFileName(AnsiString strConfFileName);

    /**
     * @brief           获取配置文件的名称
     * @return          std::string
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-01 10:16:28
     * @note
     */
    const char* GetLocalConfFileName();

    /**
     * @brief           设置 APP 的标题
     * @param           strAppTitle    [in]
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-06 09:22:22
     * @note
     */
    void SetAppTitle(AnsiString strAppTitle);

    bool NeedLogin();
    bool GetAutoLoginDIS();
    void SetAutoLoginDIS(bool bAutoLoginDIS);

    /**
     * @brief           获取是否是测试模式
     * @return          true    是测试模式
     * @return          false   不是测试模式
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-08 10:09:42
     * @note
     */
    bool GetTestMode();

    /**
     * @brief           设置测试模式的值
     * @param           bTestMode    [in]
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-08 10:10:41
     * @note
     */
    void SetTestMode(bool bTestMode);

    /**
     * @brief           获取是否显示呼叫窗口配置项
     * @return          true
     * @return          false
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-08 16:15:40
     * @note
     */
    bool GetIsShowCallWin();

    /**
     * @brief           设置是否显示呼叫窗口
     * @return          true
     * @return          false
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-08 16:17:43
     * @note
     */
    void SetIsShowCallWin(bool b);

    /**
     * @brief           在配置文件中记录上次登录星系
     * @param           szName    [in]  SDK 短号码
     * @param           ulDCType    [in]    调度台类型
     * @param           ulDepotID    [in]
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-15 11:09:41
     * @note
     */
    void SaveLoginInfo(CHAR *szUserName, UINT32 ulDCType, UINT32 ulDepotID);

    UINT32 GetDataReserveDay();
    UINT32 GetAutoLockPeriod();

    bool ShowTitle(){return m_bShowTitle;}

    int GetFontSize(){return m_iFontSize;};
    AnsiString GetFontName(){return m_strFontName;};

private:
    LocalCfg();

    BOOL GetLocalLogCfg(GConfFile &Conf);

    /// 该函数不实现，防止拷贝操作
    LocalCfg(const LocalCfg &other);
    /// 该函数不实现，防止拷贝操作
    LocalCfg &operator=(const LocalCfg &other);

    AnsiString m_strLocalConfFileName;        ///< 配置文件全路径和名称

    bool m_bIsTcpMode;                        ///< 是否使用 tcp 模式与 dis 通信， true: tcp, false: udp
    bool m_bIsMrsTcpMode;                     ///< 是否使用 tcp 模式与 mrs 通信， true: tcp, false: udp
    bool m_bTestMode;                         ///< 是否是测试模式, true: 测试模式， false: 非测试模式
    bool m_bShowTitle;                        // 是否显示程序的标题行
    bool m_bAutoLoginDIS;                     // 自动登录DIS

    AnsiString m_strFontName;
    int     m_iFontSize;
    UINT32  m_ulDataReserveDay;               /// 本地录音文件等保留天数，单位天
    UINT32  m_ulAutoLockPeriod;               /// 自动锁屏时间，单位秒，0表示不锁屏

    LOG_CFG_T m_LogCfg;                       ///< 日志配置项

    IP_ADDR_T   m_stMdcIP;                        ///< 核心网地址
    IP_ADDR_T   m_stDisIP;                        ///< 二开调度服务器地址
    IP_ADDR_T   m_stMrsIP;                        ///< 录音录像服务器地址
    IP_ADDR_T   m_stOmcIP;                        ///< OMC 地址

    AnsiString m_strVersion;                  ///< 本调度台软件版本
    AnsiString m_strAppTitle;                 ///< 本调度台标题文字

    AnsiString m_strEmergencyCallAlarmFile;  ///< 紧急呼叫请呼铃声文件
    AnsiString m_strReqCallAlarmFile;        ///< 普通请呼铃声文件
    AnsiString m_strSDSAlarmFile;            ///< SDS铃声文件
    AnsiString m_strIPHAlarmFile;            ///< IPH请求铃声文件

    bool m_bIsShowCallWin;                    ///< 是否显示呼叫窗口
};

#endif  ///< DC_DATA_LOCAL_CFG_H__

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
     * @brief           ��ȡ��������ַ
     * @return          IPV4_INFO_C
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-08-26 18:17:29
     * @note
     */
    IP_ADDR_T& GetMdcIP();
    IP_ADDR_T& GetMrsIP();

    /**
     * @brief           ��ȡ���ȷ�������ַ
     * @return          IPV4_INFO_C
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-08-26 18:18:14
     * @note
     */
    IP_ADDR_T& GetDisIP();

    /**
     * @brief           ��ȡ Omc ��ַ
     * @return          IPV4_INFO_C
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-01 09:29:22
     * @note
     */
    IP_ADDR_T& GetOmcIP();

    /**
     * @brief           ��ȡ omc ��ַ�Ƿ�Ϊ��
     * @return          true    Ϊ��
     * @return          false   ��Ϊ��
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-01 10:01:47
     * @note
     */
    bool IsOmcIPEmpty();

    /**
     * @brief           ��ȡ tcp ģʽ
     * @return          true
     * @return          false
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-08-27 09:23:00
     * @note
     */
    bool GetTcpMode();
    bool GetMrsTcpMode();

    /**
     * @brief           ��ȡ��־����
     * @return          LOG_CFG_T
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-08-27 09:43:44
     * @note
     */
    LOG_CFG_T& GetLogCfg();

    /**
     * @brief           ��ȡ����̨�汾
     * @return          std::string
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-08-27 14:16:18
     * @note
     */
    AnsiString GetDCVersion();

    /**
     * @brief           ��ȡ���� App �ı���
     * @return          std::string
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-08-27 14:23:38
     * @note
     */
    AnsiString GetAppTitle();

    /**
     * @brief           ��ȡ�������е������ļ�������·�����ļ����ƣ�
     * @return          std::string
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-08-31 17:07:18
     * @note
     */
    const char* GetAlarmFileEmergencyCall();

    /**
     * @brief           ��ȡ����������ļ�������·�����ļ����ƣ�
     * @return          std::string
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-08-31 17:08:12
     * @note
     */
    const char* GetAlarmFileReqCall();

    /**
     * @brief           ��ȡ SDS �������ļ�������·�����ļ����ƣ�
     * @return          std::string
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-08-31 17:08:34
     * @note
     */
    const char* GetAlarmFileSDS();
    const char* GetAlarmFileIPH();

    /**
     * @brief           ���������ļ�������
     * @param           strConfFileName    [in]
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-01 10:14:46
     * @note
     */
    void SetLocalConfFileName(AnsiString strConfFileName);

    /**
     * @brief           ��ȡ�����ļ�������
     * @return          std::string
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-01 10:16:28
     * @note
     */
    const char* GetLocalConfFileName();

    /**
     * @brief           ���� APP �ı���
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
     * @brief           ��ȡ�Ƿ��ǲ���ģʽ
     * @return          true    �ǲ���ģʽ
     * @return          false   ���ǲ���ģʽ
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-08 10:09:42
     * @note
     */
    bool GetTestMode();

    /**
     * @brief           ���ò���ģʽ��ֵ
     * @param           bTestMode    [in]
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-08 10:10:41
     * @note
     */
    void SetTestMode(bool bTestMode);

    /**
     * @brief           ��ȡ�Ƿ���ʾ���д���������
     * @return          true
     * @return          false
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-08 16:15:40
     * @note
     */
    bool GetIsShowCallWin();

    /**
     * @brief           �����Ƿ���ʾ���д���
     * @return          true
     * @return          false
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-08 16:17:43
     * @note
     */
    void SetIsShowCallWin(bool b);

    /**
     * @brief           �������ļ��м�¼�ϴε�¼��ϵ
     * @param           szName    [in]  SDK �̺���
     * @param           ulDCType    [in]    ����̨����
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

    /// �ú�����ʵ�֣���ֹ��������
    LocalCfg(const LocalCfg &other);
    /// �ú�����ʵ�֣���ֹ��������
    LocalCfg &operator=(const LocalCfg &other);

    AnsiString m_strLocalConfFileName;        ///< �����ļ�ȫ·��������

    bool m_bIsTcpMode;                        ///< �Ƿ�ʹ�� tcp ģʽ�� dis ͨ�ţ� true: tcp, false: udp
    bool m_bIsMrsTcpMode;                     ///< �Ƿ�ʹ�� tcp ģʽ�� mrs ͨ�ţ� true: tcp, false: udp
    bool m_bTestMode;                         ///< �Ƿ��ǲ���ģʽ, true: ����ģʽ�� false: �ǲ���ģʽ
    bool m_bShowTitle;                        // �Ƿ���ʾ����ı�����
    bool m_bAutoLoginDIS;                     // �Զ���¼DIS

    AnsiString m_strFontName;
    int     m_iFontSize;
    UINT32  m_ulDataReserveDay;               /// ����¼���ļ��ȱ�����������λ��
    UINT32  m_ulAutoLockPeriod;               /// �Զ�����ʱ�䣬��λ�룬0��ʾ������

    LOG_CFG_T m_LogCfg;                       ///< ��־������

    IP_ADDR_T   m_stMdcIP;                        ///< ��������ַ
    IP_ADDR_T   m_stDisIP;                        ///< �������ȷ�������ַ
    IP_ADDR_T   m_stMrsIP;                        ///< ¼��¼���������ַ
    IP_ADDR_T   m_stOmcIP;                        ///< OMC ��ַ

    AnsiString m_strVersion;                  ///< ������̨����汾
    AnsiString m_strAppTitle;                 ///< ������̨��������

    AnsiString m_strEmergencyCallAlarmFile;  ///< ����������������ļ�
    AnsiString m_strReqCallAlarmFile;        ///< ��ͨ��������ļ�
    AnsiString m_strSDSAlarmFile;            ///< SDS�����ļ�
    AnsiString m_strIPHAlarmFile;            ///< IPH���������ļ�

    bool m_bIsShowCallWin;                    ///< �Ƿ���ʾ���д���
};

#endif  ///< DC_DATA_LOCAL_CFG_H__

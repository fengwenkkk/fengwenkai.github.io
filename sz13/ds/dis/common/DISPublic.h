#ifndef DIS_PUBLIC_H
#define DIS_PUBLIC_H

#include "g_include.h"
#include "GPublic.h"
#include "ds_public.h"
#include "pdb_app.h"
#include "msg.h"
#include "ats.h"
#include "CfgDao.h"

#define TIMER_INIT          TIMER0
#define EV_INIT_IND         EV_TIMER0

// DIS本地配置文件
typedef struct
{
    // 地址配置
    UINT8           aucMdcAddr[4];
    UINT8           aucOmcAddr[4];

    // ATS配置
    BOOL            bAutoCloseATS;                  // 自动关闭连接ATS的socket
    IP_ADDR_T       astATSAddr[2];                  // ATS服务器地址
    BOOL            bSaveATSToFile;                 // 是否保存ATS文件
    UINT32          ulMaxATSFileNum;                // ATS文件最大个数
    UINT32          ulGetATSTime;                   // 获取ATS信息的周期（秒）
    UINT32          ulATSValidPeriod;               // 判断ATS是否正常的超时时间（秒）

    // 车载台业务配置
    BOOL            bAllowEmergencyCall;            // 车载台、固定台是否支持紧急呼叫
    BOOL            bAllowTXP2PCall;                // 车载台是否支持个呼
    BOOL            bAllowFXP2PCall;                // 固定台是否支持个呼
    BOOL            bDirectSendApp;
    BOOL            bAPPMsgUseAnsiString;           // APP消息是否采用ANSI编码格式

    INT32           iTransferTrackReqCallMode;      // 转换轨呼叫模式0:全呼 1：呼叫当前归属区 -1：呼叫待进入归属区
    UINT32          ulMaxAppSelfCheckNum;           // APP自检最大次数

    UINT32          ulRadioResetButtonCheckTime;    // 车载台主机监测重启按钮时间，单位秒，0表示不检测
    UINT32          ulRadioResetAPPCheckTime;       // 车载台主机监测重启按钮时间，单位秒，0表示不检测

    BOOL            bIPHVideoProxyMode;             // IPH视频是否通过车载台代理，缺省FALSE（不代理）
    UINT32          ulSendSelfCheckMsgPeriod;       // 车载台主机启动后发送自检消息的时间段（超过该时间段将停止发送），0表示无限制，单位秒

    UINT32          ulTXDataCaptureMode;             // 车载台实时抓包模式：0：不抓包，1：抓包

    UINT32          ulTXAutoLockTimeout;            // 终端自动闭锁时间(单位秒）
    CHAR            acTXLockPwd[32];                // 解锁密码(如果为空表示不需要输入密码，长按自动解锁），发送的是经过base64加密的文本

    // DC业务配置
    BOOL            bManageOneDepot;                // TRUE：段场调度台只能管理一个段场 FALSE：管理所有段场
    BOOL            bAllowKickOffDC;                // 允许DC重复登录时（或断开链接恢复时），自动踢出先登录的DC
    BOOL            bAllowDCReloadCfg;              // DC是否可以在启动后重新加载配置数据
    BOOL            bAllowHalfDuplexP2PCall;        // DC是否支持半双工点呼

    BOOL            bAllowVideoWall;                // DC是否支持视频投大屏功能
    BOOL            bAllowGISSub;                   // DC是否支持GIS订阅
    BOOL            bAllowP2PVideoCall;             // DC是否支持视频个呼
    BOOL            bAutoCloseMulticastCall;        // 通播组呼叫采用广播模式(释放话权后立即强拆)

    BOOL            bSendSMSByRealMsg;              // 采用实时信息发送短信（否则采用短数据方式）
    BOOL            bSimpleSMSMode;                 // 去除电子工单和运营消息

    UINT32          ulDCMinTmpPatchGroupNum;        // 每个调度台自动创建的临时派接组个数
    UINT32          ulDCUserIDForDepotIPH;          // 支持车辆段列车IPH功能的调度台，主要用于测试

    BOOL            bIPHVideoTcpMode;               // IPH视频数据传输模式，TRUE：TCP，FALSE：UDP
    UINT32          ulIPHVideoOpenTimeout;          // 播放IPH视频时协议协商时间（打开视频源时间）
    BOOL            bIPHRecMergeMode;               // IPH录音是否合并，TRUE：合并，FALSE：不合并
    BOOL            bDirectSendIPHAudio;            // 是否直接发送对讲语音给IPH设备
    UINT32          ulIPHAudioSampleRate;           // IPH音频采样频率，通常16000/8000
    UINT32          ulIPHAudioBitsPerSample;        // IPH音频采样位数，通常16或8

    BOOL            bDirectSendPAAudio;             // 是否直接发送对讲语音给PA设备
    UINT32          ulPAAudioSampleRate;            // PA音频采样频率，通常16000/8000
    UINT32          ulPAAudioBitsPerSample;         // PA音频采样位数，通常16或8

    BOOL            bPAAECMode;                     // 广播语音采用回声（麦克风）抑制方式，缺省不支持
    UINT32          ulPAMaxPkgSize;                 // 广播语音包最大长度（字节数），缺省1000
//    UINT32          ulPADelaySendTime;              // 广播语音包发送延迟时间，单位毫秒，不大于500ms，0表示不延迟

    BOOL            bIPHAECMode;                    // IPH语音采用回声（麦克风）抑制方式，缺省不支持
    UINT32          ulIPHMaxPkgSize;                // IPH语音包最大长度（字节数），缺省1000
    UINT32          ulMaxIPHTalkTime;               // IPH发起请求后最大等待时间
    UINT32          ulMAXIPHWaitAcceptTime;         // IPH等待接听的最大时间

    // GPS
    UINT16          usGPSRecvPort;                  // GPS数据接收现场（港铁项目）

    // 系统管理
    UINT32          ulClearDataHour;                // 数据清理时间
    UINT32          ulResvDataDays;                 // 数据(库）保留天数
    UINT32          ulBackupFileNum;                // 数据备份文件个数

    INT32           iAutoCloseHour;                 // dis自动重启整点时刻，-1表示不自动重启

    // 集群配置
    UINT32          ulClusterDefaultStatus;         // 主备运行状态
    UINT8           aucClusterPeerAddr[4];          // 对端地址（内部心跳地址）
    UINT8           aucClusterMasterAddr[4];        // 主用状态下的地址（对外地址）

    CHAR            acStatusFile[256];              // 被监控进程状态文件（文件内容为空表示正常，其他表示异常）
    UINT32          ulRefreshStatusFilePeriod;      // 更新状态检测文件周期，确省15秒，最小5秒

    // OCC、NOCC接口配置
    UINT32          ulLineID;                       // 线路ID
    UINT16          usOCCPort;                      // OCC与NOCC通信端口(本地UDP侦听端口）
    UINT8           aucNOCCAddr[4];                 // NOCC DIS地址，当地址为0时，OCC调度台不接入NOCC
    UINT16          usNOCCPort;                     // NOCC DIS UDP端口，用于与OCC的DIS的UDP接口通信

}LOCAL_CFG_T;

extern VOID SendAllClient(UINT16 usSenderTaskID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);
extern VOID OperLog(OPER_LOG_INFO_T *pstReport);
extern VOID OperLog(UINT16 usInstID, OPER_LOG_INFO_T *pstReport);
extern VOID SendAllUser(UINT16 usSenderTaskID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);

BOOL ReplaceString(CHAR *szInfo, UINT32 ulMaxLen, const CHAR *szFrom, const CHAR *szTo);
BOOL EncodeJsonMsg(CHAR *szInfo, UINT32 ulMaxLen);
BOOL DecodeJsonMsg(CHAR *szInfo, UINT32 ulMaxLen);

BOOL LoadLocalCfg();
LOCAL_CFG_T& GetLocalCfg();

VOID RemoveFile(CHAR *szDir, CHAR *szFilePrefix, UINT32 ulMaxFileNum);

VOID SetGroupCfg(GROUP_CFG_T *pstCfg);
BOOL GetGroupCfg(GROUP_CFG_T *pstCfg);

VOID SetDCLogin(UINT32 ulDCUserID, PID_T *pstPID);
VOID SetDCLogout(UINT32 ulDCUserID);
BOOL GetDCPID(UINT32 ulDCUserID, PID_T *pstPID);

VOID InitDepotInfo(VECTOR<DEPOT_INFO_T> &vDepotInfo);
VOID GetDepotInfo(VECTOR<DEPOT_INFO_T> &vDepotInfo);
BOOL GetDepotInfo(UINT32 ulDepotID, DEPOT_INFO_T &stDepotInfo);
VOID AddDepotInfo(DEPOT_INFO_T &stInfo);
VOID DelDepotInfo(UINT32 ulDepotID);
BOOL SetDepotInfo(DEPOT_INFO_T &stInfo);

VOID InitStationInfo(VECTOR<STATION_INFO_T> &vStationInfo);
VOID AddStationInfo(STATION_INFO_T &stInfo);
VOID DelStationInfo(UINT32 ulStationID);
VOID SetStationInfo(STATION_INFO_T &stInfo);
VOID GetStationInfo(VECTOR<STATION_INFO_T> &vStationInfo);
VOID GetDepotStationInfo(VECTOR<STATION_INFO_T> &vStationInfo);
BOOL GetStationInfo(UINT32 ulStationID, STATION_INFO_T *pstStationInfo);
VOID GetTrainPosition(TRAIN_INFO_T *pstATSInfo, UINT32 *pulPosition, CHAR *szPositionName);
UINT16 GetNextStationID(TRAIN_INFO_T *pstATSInfo);
BOOL GetStationName(UINT32 ulStationID, CHAR *szStationName);
BOOL GetTrainRegisterDepotID(UINT32 ulTrainUnitID, UINT32 *pulDepotID);

VOID InitSectionInfo(VECTOR<SECTION_INFO_T> &vInfo);
VOID GetSectionInfo(BOOL bInDepot, VECTOR<SECTION_INFO_T> &vInfo);
BOOL GetSectionInfo(UINT32 ulSectionID, SECTION_INFO_T &stInfo);
INT32 GetTrainOrder(TRAIN_INFO_T *pstATSInfo);

VOID InitDCUserInfo(VECTOR<DC_USER_INFO_T> &vUserInfo);
VOID GetDCUserInfo(VECTOR<DC_USER_INFO_T> &vUserInfo);
VOID GetTrainAndDepotDCUserInfo(VECTOR<DC_USER_INFO_T> &vUserInfo);
VOID AddDCUserInfo(DC_USER_INFO_T &stUserInfo);
VOID DelDCUserInfo(UINT32 ulDCUserID);
BOOL SetDCUserInfo(DC_USER_INFO_T &stUserInfo);
BOOL CheckDCUser(UINT32 ulDCUserID, UINT32 ulDCType);

VOID InitDCOperatorInfo(VECTOR<DC_OPERATOR_INFO_T> &vInfo);
VOID GetDCOperatorInfo(VECTOR<DC_OPERATOR_INFO_T> &vInfo);
BOOL GetDCOperatorInfo(CHAR *szOperatorID, DC_OPERATOR_INFO_T &stInfo);
VOID AddDCOperatorInfo(DC_OPERATOR_INFO_T &stInfo);
VOID DelDCOperatorInfo(CHAR *szOperatorID);
BOOL SetDCOperatorInfo(DC_OPERATOR_INFO_T &stInfo);
BOOL CheckDCOperator(CHAR *szOperatorID, CHAR *szPassword, UINT32 ulDCType, CHAR *szErrInfo);
BOOL SetDCOperatorPwd(CHAR *szOperatorID, CHAR *szNewPassword);

VOID InitTrainUEInfo(VECTOR<TRAIN_UE_INFO_T> &vUserInfo);
VOID GetTrainUEInfo(VECTOR<TRAIN_UE_INFO_T> &vUserInfo);
VOID GetMainLineTrainUnitID(VECTOR<UINT32> &vTrainUnitID);
VOID GetUpLineTrainUnitID(VECTOR<UINT32> &vTrainUnitID);
VOID GetDownLineTrainUnitID(VECTOR<UINT32> &vTrainUnitID);
BOOL GetTrainUEInfo(UINT32 ulUserID, TRAIN_UE_INFO_T *pstUserInfo);
VOID GetTrainUserID(UINT32 ulTrainUnitID, UINT32 *pulUserID1, UINT32 *pulUserID2);
VOID AddTrainUEInfo(TRAIN_UE_INFO_T &stUserInfo);
VOID SetTrainUEInfo(TRAIN_UE_INFO_T &stUserInfo);
VOID DelTrainUEInfo(UINT32 ulUserID);
UINT32 GetAnotherTrainUser(UINT32 ulUserID);

VOID InitStationUEInfo(VECTOR<STATION_UE_INFO_T> &vUserInfo);
VOID GetStationUEInfo(VECTOR<STATION_UE_INFO_T> &vUserInfo);
BOOL GetStationUEInfo(UINT32 ulUserID, STATION_UE_INFO_T *pstUserInfo);
VOID AddStationUEInfo(STATION_UE_INFO_T &stUserInfo);
VOID SetStationUEInfo(STATION_UE_INFO_T &stUserInfo);
VOID DelStationUEInfo(UINT32 ulUserID);

VOID InitPortalUEInfo(VECTOR<PORTAL_UE_INFO_T> &vUserInfo);
VOID GetPortalUEInfo(VECTOR<PORTAL_UE_INFO_T> &vUserInfo);
BOOL GetPortalUEInfo(UINT32 ulUserID, PORTAL_UE_INFO_T *pstUserInfo);
VOID AddPortalUEInfo(PORTAL_UE_INFO_T &stUserInfo);
VOID SetPortalUEInfo(PORTAL_UE_INFO_T &stUserInfo);
VOID DelPortalUEInfo(UINT32 ulUserID);
VOID SetPortalUEGISInfo(UINT32 ulUserID, DOUBLE dLon, DOUBLE dLat);

VOID InitBrdZoneInfo(VECTOR<BRD_ZONE_INFO_T> &vInfo);
VOID AddBrdZoneInfo(BRD_ZONE_INFO_T &stInfo);
VOID DelBrdZoneInfo(UINT32 ulStationID, UINT32 ulZoneID);
VOID SetBrdZoneInfo(BRD_ZONE_INFO_T &stInfo);
VOID GetBrdZoneInfo(UINT32 ulStationID, VECTOR<BRD_ZONE_INFO_T> &vInfo);
BOOL IsBrdZoneExist(UINT32 ulStationID, UINT32 ulZoneID);

UINT32 ConvertDriveMode(UINT8 ucATSDriveMode);
UINT32 GetTrainStatus(TRAIN_INFO_T  &stTrain);

UINT32 GetLostRealLocation(UINT32 ulLostRealLocation, UINT32 ulTrainATSUptime);
VOID InitTrainInfo(VECTOR<TRAIN_T> &vTrainInfo);
VOID InitATSInfo(VECTOR<ATS_INFO_T> &vATSInfo);
UINT32 GetATSDataTimeDiff();
VOID GetTrainInfo(VECTOR<TRAIN_INFO_T> &vTrainInfo);
BOOL GetTrainInfo(UINT32 ulTrainUnitID, TRAIN_INFO_T &stTrainInfo);
VOID GetTrainInfoByStationID(UINT32 ulStationID, VECTOR<TRAIN_INFO_T> &vTrainInfo);
VOID AddTrainInfo(TRAIN_T &stInfo);
VOID DelTrainInfo(UINT32 ulTrainUnitID);
VOID SetTrainInfo(TRAIN_T &stInfo);
BOOL SetAppTrainInfo(APP_TRAIN_INFO_T &stInfo);
BOOL SetIPHAudioAddr(UINT32 ulTrainUnitID, IPH_AUDIO_NET_INFO_T &stInfo);
BOOL SetTrainPos(UINT32 ulTrainUnitID, UINT32 ulDepotID);
BOOL SetTrainPos(UINT32 ulTrainUnitID, UINT32 ulDepotID, CHAR *szTrainID, CHAR *szDestID, UINT32 ulDirection, UINT32 ulStationID,UINT32 ulDriverID);
BOOL SetTrainPos(VECTOR<UINT32> &vTrain, UINT32 ulDepotID, CHAR *szDestID, UINT32 ulDirection);
VOID GetStationTrainInfo(UINT32 ulStationID, UINT32 &ulUplineTrain, CHAR *szUplineTrainID, UINT32 &ulDownlineTrain, CHAR *szDownlineTrainID);

VOID InitPredefinedSMS(VECTOR<PREDEFINED_SMS_INFO_T> &vInfo);
VOID GetPredefinedSMS(VECTOR<PREDEFINED_SMS_INFO_T> &vInfo);
UINT32 GetMaxSMSID();
VOID SetPredefinedSMS(PREDEFINED_SMS_INFO_T &stInfo);
VOID DelPredefinedSMS(UINT32 ulSMSID);

VOID InitRegisterInfo(VECTOR<REGISTER_INFO_T> &vInfo);
VOID GetRegisterInfo(VECTOR<REGISTER_INFO_T> &vInfo);
BOOL GetRegisterInfo(UINT32 ulTrainUnitID, REGISTER_INFO_T *pstInfo);
BOOL SetRegisterInfo(UINT32 ulTrainUnitID, BOOL bRegister, UINT32 ulRegisterDepotID, BOOL bAutoRegister);
VOID SetRegisterOverInfo(UINT32 ulTrainUnitID, BOOL bRegister, UINT32 ulRegisterDepotID, UINT32 ulDCUserID);
BOOL IsRegisterOver(UINT32 ulTrainUnitID);

VOID InitRecBrdCfg(VECTOR<REC_BRD_CFG_T> &vInfo);
VOID GetRecBrdCfg(VECTOR<REC_BRD_CFG_T> &vInfo);
BOOL GetRecBrdCfg(UINT32 ulRecID, REC_BRD_CFG_T &stInfo);
VOID AddRecBrdCfg(REC_BRD_CFG_T &stInfo);
VOID DelRecBrdCfg(UINT32 ulRecID);
VOID SetRecBrdCfg(REC_BRD_CFG_T &stInfo);

VOID InitMultiGroupInfo(VECTOR<MULTI_GROUP_INFO_T> &vInfo);
VOID GetMultiGroupInfo(VECTOR<MULTI_GROUP_INFO_T> &vInfo);
BOOL GetMultiGroupInfo(INT64 i64GroupID, MULTI_GROUP_INFO_T &stInfo);
VOID ClearMultiGroupInfo(UINT32 ulDCUserID);
VOID SetMultiGroupInfo(VECTOR<MULTI_GROUP_INFO_T> &vInfo);

VOID InitAPPBasicCfg(APP_BASIC_CFG_T &stCfg);
VOID GetAPPBasicCfg(APP_BASIC_CFG_T &stCfg);

BOOL ParseGPSInfo(CHAR *szGPS, GPS_INFO_T &stGPS);
VOID AddGPSInfo(GPS_INFO_T &stGPS);

VOID GetDestInfo(VECTOR<DEST_INFO_T> &vInfo);
BOOL GetDestInfo(UINT32 ulDestID, DEST_INFO_T *pstInfo);
BOOL GetDestInfo(CHAR *szDestID, DEST_INFO_T *pstInfo);
VOID SetDestInfo(DEST_INFO_T &stInfo);
VOID DelDestInfo(UINT32 ulDestID);
VOID AddDestInfo(DEST_INFO_T &stInfo);
VOID InitDestInfo(VECTOR<DEST_INFO_T> &vStInfo);

/**
 * @brief           通过车次号获取车辆信息
 * @param           strTrainID    [in]  车次号
 * @param           stTrainInfo   [out] 车辆信息
 * @return          BOOL    获取成功或失败
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
BOOL GetTrainInfoByTrainID(const std::string& strTrainID, TRAIN_INFO_T& stTrainInfo);

/**
 * @brief           通过车体号获取车辆信息
 * @param           iTrainUnitID    [in]
 * @param           stTrainInfo     [out]
 * @return          BOOL    获取成功或失败
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
BOOL GetTrainInfoByTrainUnitID(const int& iTrainUnitID, TRAIN_INFO_T& stTrainInfo);

UINT32 GetSafeQueryRecLen();

// ==================================================
VOID ConvertGroupIDToJson(VECTOR<INT64> &vGroup, GJsonTupleParam &JsonTupleParam);
VOID ConvertUserIDToJson(VECTOR<UINT32> &vUserID, GJsonTupleParam &JsonTupleParam);
VOID ConvertTrainToJson(VECTOR<TRAIN_INFO_T> &vTrain, GJsonTupleParam &JsonTupleParam);
VOID ConvertIPHTalkToJson(VECTOR<IPH_TALK_INFO_T> &vInfo, BOOL bActiveIPHTalk, GJsonTupleParam &JsonTupleParam);

#endif

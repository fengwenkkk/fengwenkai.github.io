#ifndef DS_PUBLIC_H
#define DS_PUBLIC_H

#include "g_include.h"

#define METRO_LINE_ID       13  // 每个线路取值不一样


#define MODULE_SYS          20
#define MODULE_LOG          21
#define MODULE_DBMON        23
#define MODULE_DBSYNC       24
#define MODULE_OMA          25
#define MODULE_OMT          26
#define MODULE_FILESYNC     27

#define MODULE_CFG          40
#define MODULE_ATS          41
#define MODULE_APP          42
#define MODULE_SA           43
#define MODULE_TPC          44  // Train Pos Call

// OMC
#define MODULE_TX_AGENT     50
#define MODULE_FX_AGENT     51

#define MODULE_EOMC_AGENT   70

#define MODULE_OCC          91
#define MODULE_NOCC         92

#define MODULE_TEST         201

//#define SNMP_V1                       0

#define STORE_NULL                  0
#define STORE_ON_FILE               1
#define STORE_ON_MYSQL              2
#define STORE_ON_SQLSERVER          3
#define STORE_ON_ORACLE             4

#define ODBC_NULL                   0
#define ODBC_DRIVER_CONNECT         1
#define ODBC_DSN_CONNECT            2

//#define DEFAULT_QUERY_INTERVAL      5000

#define DIS_CLIENT_DC       1   // 调度台
#define DIS_CLIENT_SA       6   // 车载台业务代理
#define DIS_CLIENT_APP      8   // 车载台APP

#define DEFAULT_RTP_PORT            554

#define DEFAULT_DIS_MPS_UDP_PORT    6308    // DIS与巡更服务器对接的端口

#define DEFAULT_DIS_TCP_PORT        63001
#define DEFAULT_REC_TCP_PORT        63101
#define DEFAULT_NOCC_UDP_PORT       63201       // NOCC DIS与OCC DIS对接的UDP服务端口
#define DEFAULT_OCC_UDP_PORT        63202       // OCC DIS与NOCC DIS对接的UDP服务端口
#define DEFAULT_APP_UDP_PORT        64001
#define DEFAULT_DBA_UDP_PORT        64002
#define DEFAULT_SA_UDP_PORT         64003

#define DEFAULT_OMC_UDP_PORT        64008
#define DEFAULT_OMA_UDP_PORT        64009

#define DEFAULT_GPS_UDP_PORT        40800       // 接收终端GPS数据上报的端口

#define PIS_TCP_PORT                50000       // IPH服务器作为TCP服务端口
#define PA_TCP_PORT                 50001       // PA服务器作为TCP服务端口
#define DC_RECV_IPH_PORT            51000       // DC接收音频数据的端口
#define IPH_RECV_DC_PORT            52000       // IPH对讲音频数据的端口

#define DEFAULT_SERVER_TRANS_FILE_PORT   50221  // 服务器TransFile端口
#define DEFAULT_CLIENTT_TRANS_FILE_PORT  50222  // 客服端TransFile端口

#define PA_RECV_DC_PORT             53000       // PA 接收广播音频的端口
#define SA_SERVER_PORT              54000       // SA 作为UDPserver的端口

#define DEFAULT_RTSP_START_PORT     55400       // 缺省RTSP中继服务器地址

#define PIS_MULTICAST_PORT          57000       // 组播端口
#define PIS_MULTICAST_ADDR          "224.8.0.0" // 组播地址

#define CLUSTER_MULTICAST_PORT      43001
#define CLUSTER_MULTICAST_ADDR      "224.9.0.0" // 组播地址

// 调度台类型
#define DC_TYPE_DEFAULT         0x00    // 缺省调度台
#define DC_TYPE_NONE            0x00    // 未知调度台

#define DC_TYPE_ALL             0xffff  // 全功能调度台
#define DC_TYPE_TRAIN           0x01    // 行车调度台
#define DC_TYPE_PASSENGER       0x02    // 乘客调度台
#define DC_TYPE_DEPOT           0x04    // 车辆段调度台
#define DC_TYPE_FIX             0x08    // 维修段调度台
#define DC_TYPE_ENV             0x10    // 环控调度台
#define DC_TYPE_POWER           0x20    // 电力调度台
#define DC_TYPE_SHUNT           0x40    // 车辆调度台
#define DC_TYPE_MASTER          0x80    // 总控调度台
#define DC_TYPE_MANAGER         0x8000  // 维护调度台

#define UE_REGISTER             1   // 注册
#define UE_UNREGISTER           0   // 注销

#define REGISTER_STATE_DOING    0   // 注册/注销中
#define REGISTER_STATE_OVER     1   // 注册/注销结束

// 请呼类型
#define INVALID_REQ_CALL                0xff
#define TRAIN_REQ_CALL_DC               0   // 调度台（车载台特有）
#define TRAIN_REQ_CALL_STATION          1   // 车站（车载台特有）
#define TRAIN_REQ_CALL_FRONT_TRAIN      2   // 前车（车载台特有）
#define TRAIN_REQ_CALL_BACK_TRAIN       3   // 后车（车载台特有）
#define TRAIN_EMERGENCY_REQ_CALL_DC     9   // 紧急请呼调度台（车载台特有）

#define STATION_REQ_CALL_DC             10  // 呼叫调度台（固定台特有）
#define STATION_REQ_CALL_TRAIN          11  // 站管区列车（固定台特有）
#define STATION_EMERGENCY_REQ_CALL_DC   19  // 紧急请呼调度台（固定台特有）

#define APP_OK                          1   // 成功
#define APP_ERR                         0   // 失败

#define FORMAL_DISPATCH_SDS_INDEX_MIN   1
#define FORMAL_DISPATCH_SDS_INDEX_MAX   99
#define ORAL_DISPATCH_SDS_INDEX_MIN     101
#define ORAL_DISPATCH_SDS_INDEX_MAX     199
#define OPERATE_SDS_INDEX_MIN           201

// 文件数据库名称
#define DB_FILE_APP_STATUS              "app_status"

// JSon 消息: SA->DIS

#define MSG_SAHeartBeat                 "SAHeartBeat"               // SA->DIS
#define MSG_SAApplyIPHTalk              "SAApplyIPHTalk"            // SA->DIS  DIS->DC
#define MSG_SAStopIPHTalk               "SAStopIPHTalk"             // SA->DIS  DIS->DC
#define MSG_SAIPHApplyACKMsg            "SAIPHApplyAckMsg"          // SA->DIS  DIS->DC
#define MSG_SAIPHPauseRspMsg            "SAIPHPauseRspMsg"          // SA->DIS  DIS->DC
#define MSG_SAIPHDevRestReqMsg          "SAIPHDevRestReqMsg"        // SA->DIS  DIS->DC
#define MSG_SAIPHAudioUploadMsg         "SAIPHAudioUploadMsg"       // SA->DIS  DIS->DC

#define MSG_SAGetNTPCfg                 "SAGetNTPCfg"               // SA->DIS  DIS->DC
#define MSG_SAGetCfg                    "SAGetCfg"                  // SA->DIS  DIS->DC
#define MSG_SAGetRecBrdCfg              "SAGetRecBrdCfg"            // SA->DIS  DIS->DC

// PA心跳
#define MSG_PAHeartBeat                 "PAHeartBeat"               //DC<->DIS SA->DIS

// IPH心跳
#define MSG_IPHHeartBeat                "IPHHeartBeat"              //DC<->DIS SA->DIS

// DC->DIS  DIS->SA
#define MSG_DCAcceptIPHTalk             "DCAcceptIPHTalk"           // DC->DIS  DIS->SA
#define MSG_DCStopIPHTalk               "DCStopIPHTalk"             // DC->DIS  DIS->SA
#define MSG_DCOneKeyResetIPH            "DCOneKeyResetIPH"          // DC->DIS  DIS->SA
#define MSG_DCPauseIPHTalkReq           "DCPauseIPHTalkReq"         // DC->DIS  DIS->SA
#define MSG_DCIPHDevResetRsp            "DCIPHDevRsetRsp"           // DC->DIS  DIS->SA

#define MSG_DCStartRecBrd               "DCStartRecBrd"             // DC->DIS  DIS->SA

#define MSG_DCGetActiveIPHTalk          "DCGetActiveIPHTalk"        // DC->DIS

#define MSG_DCGetRecBrdCfg              "DCGetRecBrdCfg"            // DC->DIS

#define MSG_DCStartRealBrd              "DCStartRealBrd"            // DC->DIS  DIS->SA  采用发送PCM码流给广播系统的实时广播模式
#define MSG_DCStopRealBrd               "DCStopRealBrd"             // DC->DIS  DIS->SA
#define MSG_DCSendRealBrdAudioData      "DCSendRealBrdAudioData"    // DC->DIS  DIS->SA
#define MSG_DCSendIPHAudioData          "DCSendIPHAudioData"        // DC->DIS  DIS->SA

// JSon 消息: DC->DIS
#define MSG_DCRegisterDIS               "DCRegisterDIS"             // DC->DIS
#define MSG_DISKickOffDC                "DISKickOffDC"              // DIS->DC

#define MSG_DCConfirmSave               "DCConfirmSave"             // DIS->DC
#define MSG_DCLoadAPPCfg                "DCLoadAPPCfg"              // DC->DIS
#define MSG_DCLoadGroupCfg              "DCLoadGroupCfg"
#define MSG_DCLoadDCInfo                "DCLoadDCInfo"
#define MSG_DCLoadDCUserInfo            "DCLoadDCUserInfo"
#define MSG_DCLoadDCOperatorInfo        "DCLoadDCOperatorInfo"
#define MSG_DCLoadLineCfg               "DCLoadLineCfg"
#define MSG_DCLoadTrainUEInfo           "DCLoadTrainUEInfo"
#define MSG_DCLoadStationUEInfo         "DCLoadStationUEInfo"
#define MSG_DCLoadPortalUEInfo          "DCLoadPortalUEInfo"
#define MSG_DCLoadGroupInfo             "DCLoadGroupInfo"           // DC->DIS
#define MSG_DCLoadAutoJoinGroupInfo     "DCLoadAutoJoinGroupInfo"   // DC->DIS
#define MSG_DCLoadSubGroupInfo          "DCLoadSubGroupInfo"        // DC->DIS
#define MSG_DCLoadSubVideoGroupInfo     "DCLoadSubVideoGroupInfo"   // DC->DIS
#define MSG_DCLoadSubUserInfo           "DCLoadSubUserInfo"         // DC->DIS
#define MSG_DCLoadSubVideoUserInfo      "DCLoadSubVideoUserInfo"    // DC->DIS
#define MSG_DCLoadBrdZoneInfo           "DCLoadBrdZoneInfo"         // DC->DIS
#define MSG_DCLoadTrainInfo             "DCLoadTrainInfo"           // DC->DIS
#define MSG_DCLoadTrainRegisterInfo     "DCLoadTrainRegisterInfo"   // DC->DIS
#define MSG_DCLoadDepotInfo             "DCLoadDepotInfo"           // DC->DIS
#define MSG_DCLoadStationInfo           "DCLoadStationInfo"         // DC->DIS
#define MSG_DCLoadMultiGroupInfo        "DCLoadMultiGroupInfo"      // DC->DIS
#define MSG_DCLoadStationPlatformInfo   "DCLoadStationPlatformInfo" // DC->DIS
#define MSG_DCLoadDestInfo              "DCLoadDestInfo"            // DC->DIS
#define MSG_DCLoadSectionInfo           "DCLoadSectionInfo"         // DC->DIS

#define MSG_DCLoadP2PCallInfo           "DCLoadP2PCallInfo"         // DC->DIS
#define MSG_DCLoadGroupCallInfo         "DCLoadGroupCallInfo"       // DC->DIS
#define MSG_DCLoadP2PVideoCallInfo      "DCLoadP2PVideoCallInfo"    // DC->DIS
#define MSG_DCLoadReqCallInfo           "DCLoadReqCallInfo"         // DC->DIS
#define MSG_DCLoadRecBrdInfo            "DCLoadRecBrdInfo"          // DC->DIS
#define MSG_DCLoadRealBrdInfo           "DCLoadRealBrdInfo"         // DC->DIS
#define MSG_DCLoadIPHTalkInfo           "DCLoadIPHTalkInfo"         // DC->DIS

#define MSG_DCLoadPredefinedSMS         "DCLoadPredefinedSMS"
#define MSG_DCLoadSDSTemplate           "DCLoadSDSTemplate"
#define MSG_DCLoadRecBrdCfg             "DCLoadRecBrdCfg"
#define MSG_DCLoadLineCfg               "DCLoadLineCfg"

#define MSG_DCLoadOperLogInfo           "DCLoadOperLogInfo"         // DC->DIS

#define MSG_DCSaveGroupStatus           "DCSaveGroupStatus"         // DC<->DIS
#define MSG_DCSaveSubGroupInfo          "DCSaveSubGroupInfo"        // DC<->DIS
#define MSG_DCSaveSubVideoGroupInfo     "DCSaveSubVideoGroupInfo"   // DC<->DIS

#define MSG_DCSaveSubUserInfo           "DCSaveSubUserInfo"         // DC<->DIS
#define MSG_DCSaveSubVideoUserInfo      "DCSaveSubVideoUserInfo"    // DC<->DIS

#define MSG_DCSaveP2PCallInfo           "DCSaveP2PCallInfo"         // DC<->DIS
#define MSG_DCSaveGroupCallInfo         "DCSaveGroupCallInfo"       // DC<->DIS
#define MSG_DCSaveP2PVideoCallInfo      "DCSaveP2PVideoCallInfo"    // DC<->DIS
#define MSG_DCSaveGroupVideoCallInfo    "DCSaveGroupVideoCallInfo"  // DC<->DIS
#define MSG_DCSaveRealBrdInfo           "DCSaveRealBrdInfo"         // DC<->DIS
#define MSG_DCSaveGroupInfo             "DCSaveGroupInfo"           // DC<->DIS

#define MSG_DCBroadcastStart            "DCBroadcastStart"          // DC->DIS  SA->DIS 采用组呼模式的实时广播
#define MSG_DCBroadcastEnd              "DCBroadcastEnd"            // DC->DIS  SA->DIS

#define MSG_DCGetSDSInfo                "DCGetSDSInfo"
#define MSG_DCGetLatestSDSInfo          "DCGetLatestSDSInfo"
#define MSG_DCAddSDSInfo                "DCAddSDSInfo"
#define MSG_DCSetSDSInfo                "DCSetSDSInfo"
#define MSG_DCDelSDSInfo                "DCDelSDSInfo"
#define MSG_DCGetSDSReply               "DCGetSDSReply"
#define MSG_DCSetTrainDepotID           "DCSetTrainDepotID"
#define MSG_DCSetUESDSReply             "DCSetUESDSReply"
#define MSG_DCSetUESDSRecv              "DCSetUESDSRecv"
#define MSG_DCSaveNewSDS                "DCSaveNewSDS"
#define MSG_DCSaveNewSMS                "DCSaveNewSMS"
#define MSG_DCSetOperator               "DCSetOperator"
#define MSG_DCSaveGISInfo               "DCSaveGISInfo"
#define MSG_DCSaveMultiGroupInfo        "DCSaveMultiGroupInfo"

// 请呼
#define MSG_DCGetReqCallInfo            "DCGetReqCallInfo"          // DC->DIS
#define MSG_DCApplyReqCall              "DCApplyReqCall"            // DC请求受理请呼
#define MSG_DCCloseReqCall              "DCCloseReqCall"            // DC结束请呼

#define MSG_DCSetTrainGroup             "DCSetTrainGroup"           // 列车转组
#define MSG_DCSetTrainPos               "DCSetTrainPos"             // 设置列车位置

// 配置管理
#define MSG_DCLogin                     "DCLogin"
#define MSG_DCLogout                    "DCLogout"

#define MSG_DCOperatorLogin             "DCOperatorLogin"
#define MSG_DCOperatorLogout            "DCOperatorLogout"

#define MSG_DCSetPwd                    "DCSetPwd"

#define MSG_DCAddGroupCfg               "DCAddGroupCfg"
#define MSG_DCSetGroupCfg               "DCSetGroupCfg"
#define MSG_DCDelGroupCfg               "DCDelGroupCfg"

#define MSG_DCGetCfgDataVer             "DCGetCfgDataVer"

#define MSG_DCAddDCUser                 "DCAddDCUser"               // DC<->DIS
#define MSG_DCDelDCUser                 "DCDelDCUser"               // DC<->DIS
#define MSG_DCSetDCUser                 "DCSetDCUser"               // DC<->DIS

#define MSG_DCAddDCOperator             "DCAddDCOperator"
#define MSG_DCDelDCOperator             "DCDelDCOperator"
#define MSG_DCSetDCOperator             "DCSetDCOperator"

#define MSG_DCSaveOperLog               "DCSaveOperLog"

#define MSG_DCAddDepotCfg               "DCAddDepotCfg"
#define MSG_DCDelDepotCfg               "DCDelDepotCfg"
#define MSG_DCSetDepotCfg               "DCSetDepotCfg"

#define MSG_DCAddLineCfg                "DCAddLineCfg"
#define MSG_DCDelLineCfg                "DCDelLineCfg"
#define MSG_DCSetLineCfg                "DCSetLineCfg"

#define MSG_DCAddStation                "DCAddStation"              // DC<->DIS
#define MSG_DCDelStation                "DCDelStation"              // DC<->DIS
#define MSG_DCSetStation                "DCSetStation"              // DC<->DIS

#define MSG_DCAddDestInfo               "DCAddDestInfo"             // DC<->DIS
#define MSG_DCDelDestInfo               "DCDelDestInfo"             // DC<->DIS
#define MSG_DCSetDestInfo               "DCSetDestInfo"             // DC<->DIS

#define MSG_DCAddStationUE              "DCAddStationUE"            // DC<->DIS
#define MSG_DCDelStationUE              "DCDelStationUE"            // DC<->DIS
#define MSG_DCSetStationUE              "DCSetStationUE"            // DC<->DIS

#define MSG_DCAddPortalUE               "DCAddPortalUE"             // DC<->DIS
#define MSG_DCDelPortalUE               "DCDelPortalUE"             // DC<->DIS
#define MSG_DCSetPortalUE               "DCSetPortalUE"             // DC<->DIS

#define MSG_DCAddBrdZone                "DCAddBrdZone"              // DC<->DIS
#define MSG_DCDelBrdZone                "DCDelBrdZone"              // DC<->DIS
#define MSG_DCSetBrdZone                "DCSetBrdZone"              // DC<->DIS

#define MSG_DCAddTrain                  "DCAddTrain"                // DC<->DIS
#define MSG_DCDelTrain                  "DCDelTrain"                // DC<->DIS
#define MSG_DCSetTrain                  "DCSetTrain"                // DC<->DIS

#define MSG_DCAddTrainUE                "DCAddTrainUE"              // DC<->DIS
#define MSG_DCDelTrainUE                "DCDelTrainUE"              // DC<->DIS
#define MSG_DCSetTrainUE                "DCSetTrainUE"              // DC<->DIS

#define MSG_DCAddPredefinedSMS          "DCAddPredefinedSMS"
#define MSG_DCDelPredefinedSMS          "DCDelPredefinedSMS"
#define MSG_DCSetPredefinedSMS          "DCSetPredefinedSMS"

#define MSG_DCAddSDSTemplate            "DCAddSDSTemplate"
#define MSG_DCDelSDSTemplate            "DCDelSDSTemplate"
#define MSG_DCSetSDSTemplate            "DCSetSDSTemplate"

#define MSG_DCAddRecBrdCfg              "DCAddRecBrdCfg"
#define MSG_DCDelRecBrdCfg              "DCDelRecBrdCfg"
#define MSG_DCSetRecBrdCfg              "DCSetRecBrdCfg"

#define MSG_DCGetMaxRealBrdSeqID        "DCGetMaxRealBrdSeqID"

#define MSG_DCGetRecFileListReq         "DCGetRecFileListReq"       // DC -> DIS, DIS -> DC
#define MSG_DCGetRecFileListRsp         "DCGetRecFileListRsp"       // DC -> DIS, DIS -> DC

// 录音录像终端
#define MSG_REC_Login                   "REC_Login"
#define MSG_REC_Logout                  "REC_Logout"

#define MSG_REC_ConfirmSave             "REC_ConfirmSave"

#define MSG_REC_SetPwd                  "REC_SetPwd"

#define MSG_REC_LoadNtpCfg              "REC_LoadNTPCfg"     /// SPT<->MRS

#define MSG_REC_GetOperator             "REC_GETOperator"
#define MSG_REC_AddOperator             "REC_AddOperator"
#define MSG_REC_DelOperator             "REC_DelOperator"
#define MSG_REC_SetOperator             "REC_SetOperator"

#define MSG_REC_GetRecFileList          "REC_GetRecFileList"

#define MSG_REC_GetRecFile              "REC_GetRecFile"            // 文件下载
#define MSG_REC_DCUpFileReq             "REC_DCUpFileReq"

#define MSG_DCSetDynamicGroup           "DCSetDynamicGroup"

// APP消息
#define MSG_APP_GetDSCfgInfoReq         "GetDSCfgInfoReq"
#define MSG_APP_GetDSCfgInfoRsp         "GetDSCfgInfoRsp"

#define MSG_APP_GetNTPCfgReq            "GetNTPCfgReq"
#define MSG_APP_GetNTPCfgRsp            "GetNTPCfgRsp"

#define MSG_APP_GetCfgInfoReq           "GetCfgInfoReq"
#define MSG_APP_GetCfgInfoRsp           "GetCfgInfoRsp"

#define MSG_APP_RegisterReq             "RegisterReq"       // 注册
#define MSG_APP_RegisterRsp             "RegisterRsp"       // 注册
#define MSG_APP_UnregisterReq           "UnregisterReq"     // 注销
#define MSG_APP_UnregisterRsp           "UnregisterRsp"     // 注销

#define MSG_APP_RequestCallReq          "RequestCallReq"
#define MSG_APP_RequestCallRsp          "RequestCallRsp"

#define MSG_APP_GroupCallEndReq         "GroupCallEndReq"   // 调度台通知车载台组呼结束

#define MSG_APP_GetATSInfoReq           "GetATSInfoReq"
#define MSG_APP_GetATSInfoRsp           "GetATSInfoRsp"

#define MSG_APP_GetStationATSInfoReq    "GetStationATSInfoReq"
#define MSG_APP_GetStationATSInfoRsp    "GetStationATSInfoRsp"

#define MSG_APP_ReportTrainInfo         "ReportTrainInfo"

#define MSG_APP_SetTrainGroupReq        "SetTrainGroupReq"  // 列车转组
#define MSG_APP_SetTrainGroupRsp        "SetTrainGroupRsp"

#define MSG_APP_SetTrainIDReq           "SetTrainIDReq"     // 设置列车车次号
#define MSG_APP_SetTrainIDRsp           "SetTrainIDRsp"

#define MSG_APP_SetTrainPos             "SetTrainPos"    // 设置列车位置
#define MSG_APP_SetTrainPosReq          "SetTrainPosReq"    // 设置列车位置
#define MSG_APP_SetTrainPosRsp          "SetTrainPosRsp"

#define MSG_APP_BroadcastStartReq       "DCBroadcastStartReq"
#define MSG_APP_BroadcastStartRsp       "DCBroadcastStartRsp"

#define MSG_APP_BroadcastEndReq         "DCBroadcastEndReq"

#define MSG_APP_SDSReplyReq             "SDSReplyReq"
#define MSG_APP_SDSReplyRsp             "SDSReplyRsp"

#define MSG_APP_AutoSDSReplyReq         "AutoSDSReplyReq"   // 如果一台车载台已经发送应答消息，则DIS自动发送此消息到另外一台车载台
#define MSG_APP_AutoSDSReplyRsp         "AutoSDSReplyRsp"   //

#define MSG_APP_EmergencyCallReq        "EmergencyCallReq"
#define MSG_APP_EmergencyCallRsp        "EmergencyCallRsp"

#define MSG_APP_AddDynamicGroupReq      "AddDynamicGroupReq"
#define MSG_APP_DelDynamicGroupReq      "DelDynamicGroupReq"

// OCC <-> NOCC消息
#define MSG_OCC_HeartBeatReq                "HeartBeatReq"              // 心跳请求
#define MSG_OCC_HeartBeatRsp                "HeartBeatRsp"              // 心跳应答
#define MSG_OCC_GetLineInfoReq              "GetLineInfoReq"            // 获取线路基本信息请求
#define MSG_OCC_GetLineInfoRsp              "GetLineInfoRsp"            // 获取线路基本信息应答
#define MSG_OCC_GetATSInfoReq               "GetATSInfoReq"             // 查询ATS信息请求
#define MSG_OCC_GetATSInfoRsp               "GetATSInfoRsp"             // 查询ATS信息应答
#define MSG_OCC_StartTrainRealPAReq         "StartTrainRealPAReq"       // 实时广播请求
#define MSG_OCC_StartTrainRealPARsp         "StartTrainRealPARsp"       // 实时广播应答
#define MSG_OCC_PushTrainRealPAAudioReq     "PushTrainRealPAAudioReq"   // 实时广播语音下发
#define MSG_OCC_StopTrainRealPAReq          "StopTrainRealPAReq"        // 实时广播结束请求
#define MSG_OCC_StopTrainRealPARsp          "StopTrainRealPARsp"        // 实时广播结束应答
#define MSG_OCC_GetPARecCfgReq              "GetPARecCfgReq"            // 查询预录音广播配置信息请求
#define MSG_OCC_GetPARecCfgRsp              "GetPARecCfgRsp"            // 查询预录音广播配置信息应答
#define MSG_OCC_StartTrainRecPAReq          "StartTrainRecPAReq"        // 预录音广播请求
#define MSG_OCC_StartTrainRecPARsp          "StartTrainRecPARsp"        // 预录音广播应答

// NVRAM参数
#define CFG_LOCAL_IP                        "network.lan.ipaddr"
#define CFG_LOCAL_NETMASK                   "network.lan.netmask"
#define CFG_LOCAL_GW                        "network.lan.gateway"

#define FACTORY_BOOT                        "BOOT_VERSION"
#define FACTORY_SW                          "running"
#define FACTORY_MAC                         "dev_mac"
#define FACTORY_SN                          "SN"

#define CFG_STATION_ID                      "cfg_station_id"
#define CFG_TRAIN_UNIT_ID                   "cfg_train_unit_id"
#define CFG_DIS_IP                          "cfg_dis_ip"
#define CFG_PIS_IP                          "cfg_pis_ip"
#define CFG_PA_IP                           "cfg_pa_ip"
#define CFG_APP_IP                          "cfg_app_ip"
#define CFG_OMC_IP                          "cfg_omc_ip"

#define CFG_DIS_HEARTBEAT_TIME              "cfg_dis_heartbeat_time"
#define CFG_PIS_HEARTBEAT_TIME              "cfg_pis_heartbeat_time"
#define CFG_APP_HEARTBEAT_TIME              "cfg_app_heartbeat_time"
#define CFG_PIS_DISCOVER_AUTO               "cfg_pis_discover_auto"
#define CFG_DBA_LOG_LEVEL                   "cfg_dba_log_level"
#define CFG_SA_LOG_LEVEL                    "cfg_sa_log_level"
#define CFG_RTSP_PROXY_LOG_LEVEL            "cfg_rtsp_proxy_log_level"
#define CFG_OMA_LOG_LEVEL                   "cfg_oma_log_level"
#define CFG_OMA_LOG_STD_OUT                 "cfg_oma_log_std_out"
#define CFG_TC_ID                           "cfg_tc_id"    /// 车载台安装位置1：TC1， 2：TC2
#define CFG_TRAIN_TYPE                      "cfg_train_type"  /// 列车类型1：普通车， 2：工程车， 3：作业车
#define CFG_PIS_IF_IP                       "cfg_pis_if_ip"
#define CFG_PIS_IF_MASK                     "cfg_pis_if_mask"
#define CFG_DGU_STATUS                      "cfg_dgu_status"

// 业务数据文件键值
#define TRAIN_COVER_STATUS                  "train_cover_status"
#define TRAIN_DRIVE_MODE                    "train_drive_mode"
#define TRAIN_IPH_LINK_STATUS               "train_iph_link_status"

#define TRAIN_PA_RUNNING_STATUS             "train_pa_running_status"
#define TRAIN_IPH_RUNNING_STATUS            "train_iph_running_status"
#define TRAIN_PA_MASTER_STATUS              "train_pa_master_status"

// DBA NVRAM
#define APP_UPDATE_COMMAND                  "app_update_command"
#define APP_UPDATE_STATUS                   "app_update_status"
#define APP_UPLOAD_COMMAND                  "app_upload_command"

#define APP_SET_NETWORK_COMMAND             "app_set_net_command"
#define APP_SET_NETWORK_IP                  "app_set_net_ip"
#define APP_SET_NETWORK_NETMASK             "app_set_net_netmask"
#define APP_SET_NETWORK_GATEWAY             "app_set_net_gw"

//#define APP_GETFILE_FILETYPE
#define APP_GETFILE_STARTTIME               "app_get_file_start_time"
#define APP_GETFILE_INTERVAL                "app_get_file_interval"

#define APP_GETLOG_STARTTIME                "app_get_log_start_time"
#define APP_GETLOG_INTERVAL                 "app_get_log_interval"

#define APP_REBOOT_RESETTYPE                "app_reboot_command"
#define APP_REBOOT_WAITTIME                 "app_reboot_wait_time"
#define APP_REBOOT_CMD_SWITCH               "app_reboot_cmd_switch"

#define APP_FILE_LOAD_FLAG                  "app_file_load_flag"
#define APP_FILE_FILENAME                   "app_file_filename"
#define APP_FILE_FILETYPE                   "app_file_filetype"
#define APP_FILE_TRANSPORT_PROTOCOL         "app_file_transport_protocol"
#define APP_FILE_SERVER_ADDR                "app_file_server_addr"
#define APP_FILE_SERVER_PORT                "app_file_server_port"
#define APP_FILE_SERVER_USERNAME            "app_file_server_username"
#define APP_FILE_SERVER_PASSWD              "app_file_server_passwd"
#define APP_FILE_TRANS_STATUS               "app_file_trans_status"
#define APP_FILE_FAIL_REASON                "app_file_fail_reason"
#define APP_FILE_FILE_VERSION               "app_file_file_version"

#define OMA_NTPCFG_NTPADDR                  "oma_ntpcfg_ntpaddr"
#define OMA_NTPCFG_NTPPORT                  "oma_ntpcfg_ntpport"
#define OMA_NTPCFG_SYNCPERIOD               "oma_ntpcfg_syncperiod"

#define OMA_CFG_ResetButtonCheckTime        "oma_cfg_reset_button_check_time"
#define OMA_CFG_ResetAPPCheckTime           "oma_cfg_reset_app_check_time"

#define APP_STATUS_DeviceId                 "app_status_device_id"
#define APP_STATUS_FwVersion                "app_status_fw_version"
#define APP_STATUS_AppVersion               "app_status_app_version"
#define APP_STATUS_NetMask                  "app_status_netmask"
#define APP_STATUS_Gateway                  "app_status_gateway"
#define APP_STATUS_LteAttachStatus          "app_status_lte_attach_status"
#define APP_STATUS_LteRSSI                  "app_status_lte_rssi"
#define APP_STATUS_LteRSRP                  "app_status_lte_rsrp"
#define APP_STATUS_LteRSRQ                  "app_status_lte_rsrq"
#define APP_STATUS_LteSINR                  "app_status_lte_sinr"
#define APP_STATUS_CellID                   "app_status_lte_cellid"
#define APP_STATUS_GatewayReachable         "app_status_gateway_reachable"
#define APP_STATUS_MDCRegStatus             "app_status_mdc_reg_status"
#define APP_STATUS_PISLinkStatus            "app_status_pis_link_status"
#define APP_STATUS_PALinkStatus             "app_status_pa_link_status"
#define APP_STATUS_DISLinkStatus            "app_status_dis_link_status"
#define APP_STATUS_ProcessStatus            "app_status_proc_status"
#define APP_STATUS_SelfCheckStatus          "app_status_self_check_status"
#define APP_STATUS_SelfCheckFailReason      "app_status_self_check_fail_reason"
#define APP_STATUS_Time                     "app_status_time"
#define APP_STATUS_Uptime                   "app_status_uptime"
#define APP_STATUS_CpuUsage                 "app_status_cpu_usage"
#define APP_STATUS_RamUsage                 "app_status_ram_usage"
#define APP_STATUS_DiskUsage                "app_status_disk_usage"
#define APP_STATUS_EmergencyCallStatus      "app_status_emergency_call_status"
#define APP_STATUS_EmergencyBraceStatus     "app_status_emergency_brace_status"
#define APP_STATUS_StationID                "app_status_station_id"
#define APP_STATUS_DevTemp                  "app_status_dev_temp"
#define APP_STATUS_RadioVersion             "app_status_radio_version"
#define APP_STATUS_AndroidVersion           "app_status_android_version"
#define APP_FILE_LIST_PATH_MNT              "/mnt/emmc/logsave/Log_Filelist"
#define APP_OPER_LOG_PATH_MNT               "/mnt/emmc/logsave/Log_Operlog"
#define APP_FILE_LIST_PATH_TMP              "/tmp/Log_Filelist"
#define APP_OPER_LOG_PATH_TMP               "/tmp/Log_Operlog"
#define APP_LAST_GET_FILE_LIST_TIME         "/mnt/emmc/logsave/LastGetFilelistTime"
#define APP_LAST_GET_OPER_LOG_TIME          "/mnt/emmc/logsave/LastGetOperLogTime"

#define APP_LOG_PATH                        "/custom/metro/log/"
#define APP_VOICE_PATH                      "/custom/metro/voice/"

#define SNMP_FILE_LIST_DIR                  "/tmp/Log_Filelist"
#define SNMP_OPER_LOG_DIR                   "/tmp/Log_Operlog"

// 阈值设置
#define APP_STATUS_CPU_Threshold            "app_status_cpu_threshold"
#define APP_STATUS_RAM_Threshold            "app_status_ram_threshold"
#define APP_STATUS_Disk_Threshold           "app_status_disk_threshold"
#define APP_STATUS_Temp_Threshold           "app_status_temp_threshold"

#define NVRAM_SNMP_MANAGE_IP                "ManageIp"
#endif

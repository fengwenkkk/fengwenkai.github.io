#ifndef OID_H
#define OID_H

static SNMP_OID SNMP_TRAP[] = {SNMP_TRAP_OID};

static SNMP_OID oid_AuthFailed      [] = {1,3,6,1,6,3,1,1,5,5,0}; // 鉴权失败

//告警内容
static SNMP_OID oid_AlarmSn         [] = {1,3,6,1,4,1,28723,1,1,0};
static SNMP_OID oid_AlarmNeName     [] = {1,3,6,1,4,1,28723,1,2,0};
static SNMP_OID oid_AlarmLevel      [] = {1,3,6,1,4,1,28723,1,3,0};
static SNMP_OID oid_AlarmType       [] = {1,3,6,1,4,1,28723,1,4,0};
static SNMP_OID oid_AlarmReasonID   [] = {1,3,6,1,4,1,28723,1,5,0};
static SNMP_OID oid_AlarmReason     [] = {1,3,6,1,4,1,28723,1,6,0};
static SNMP_OID oid_AlarmRaiseTime  [] = {1,3,6,1,4,1,28723,1,7,0};
static SNMP_OID oid_AlarmStatus     [] = {1,3,6,1,4,1,28723,1,8,0};
static SNMP_OID oid_AlarmTitle      [] = {1,3,6,1,4,1,28723,1,9,0};
static SNMP_OID oid_AlarmInfo       [] = {1,3,6,1,4,1,28723,1,10,0};

// 告警
static SNMP_OID oid_CpuUsageTooHighTrap         [] = {1,3,6,1,4,1,28723,12,10,1,3,0};   // 设备CPU利用率过高告警
static SNMP_OID oid_MemUsageTooHighTrap         [] = {1,3,6,1,4,1,28723,12,10,1,5,0};   // 设备内存利用率过高告警
static SNMP_OID oid_DiscoveryTrap               [] = {1,3,6,1,4,1,28723,12,10,1,7,0};   // 上线报文
static SNMP_OID oid_OffLineTrap                 [] = {1,3,6,1,4,1,28723,12,10,1,10,0};  // 设备离线告警 ZWJ
static SNMP_OID oid_HeartBeatTrap               [] = {1,3,6,1,4,1,28723,12,10,1,11,0};  // 心跳报文
static SNMP_OID oid_LteLanIDTrap                [] = {1,3,6,1,4,1,28723,12,10,1,30,0};  // TAU LTE 小区切换告警
static SNMP_OID oid_LanStatusTrap               [] = {1,3,6,1,4,1,28723,12,10,1,31,0};  // 端口状态告警/TAU(LAN口插拔告警)
static SNMP_OID oid_LteWeakSignalTrap           [] = {1,3,6,1,4,1,28723,12,10,1,32,0};  // TAU LTE 信号弱告警告警
static SNMP_OID oid_PowerDownTrap               [] = {1,3,6,1,4,1,28723,12,10,1,33,0};  // 掉电告警
static SNMP_OID oid_RebootTrap                  [] = {1,3,6,1,4,1,28723,12,10,1,34,0};  // 设备重启告警
static SNMP_OID oid_DevTempTrap                 [] = {1,3,6,1,4,1,28723,12,10,1,35,0};  // 设备温度告警

static SNMP_OID oid_DiskUsageTooHighTrap        [] = {1,3,6,1,4,1,28723,70,3,1,0};      // 磁盘占用率过高告警
static SNMP_OID oid_EmergencyCallTrap           [] = {1,3,6,1,4,1,28723,70,3,3,0};      // 紧急呼叫告警
static SNMP_OID oid_EmergencyBraceTrap          [] = {1,3,6,1,4,1,28723,70,3,4,0};      // 紧急停车告警
static SNMP_OID oid_SelfCheckFailTrap           [] = {1,3,6,1,4,1,28723,70,3,5,0};      // 自检失败告警
//static SNMP_OID oid_CoverOpenedTrap               [] = {1,3,6,1,4,1,28723,70,3,6 };     // 设备盖打开告警 只查看状态不产生告警
static SNMP_OID oid_SwitchToMasterTrap          [] = {1,3,6,1,4,1,28723,70,3,7,0};      // 切换到主用状态告警
static SNMP_OID oid_SwitchToSlaveTrap           [] = {1,3,6,1,4,1,28723,70,3,8,0};      // 切换到备用状态告警
static SNMP_OID oid_GetATSDataFailTrap          [] = {1,3,6,1,4,1,28723,70,3,9,0};      // 获取ATS数据失告警
static SNMP_OID oid_SyncTimeFailTrap            [] = {1,3,6,1,4,1,28723,70,3,10,0};     // 时间同步失败告警
static SNMP_OID oid_LinkToLteFailTrap           [] = {1,3,6,1,4,1,28723,70,3,11,0};     // 连接核心网失败告警
static SNMP_OID oid_IPHLinkFailTrap             [] = {1,3,6,1,4,1,28723,70,3,12,0};     // IPH 连接失败告警
static SNMP_OID oid_PALinkFailTrap              [] = {1,3,6,1,4,1,28723,70,3,13,0};     // PA 连接失败告警
static SNMP_OID oid_HardDiskMountFailTrap       [] = {1,3,6,1,4,1,28723,70,3,14,0};     // 硬盘挂载失败告警(TX/FX特有)
static SNMP_OID oid_FileStorageTooHighTrap      [] = {1,3,6,1,4,1,28723,70,3,15,0};     // 文件占用存储空间过高告警(MRS特有)

// 告警码
#define SNMP_ALARM_CODE_CpuUsageTooHigh         1003        // CPU利用率过高
#define SNMP_ALARM_CODE_MemUsageTooHigh         1004        // 内存利用率过高
#define SNMP_ALARM_CODE_PowerDown               3004        // 设备掉电告警
#define SNMP_ALARM_CODE_DiskUsageTooHigh        7001        // 磁盘占用率过高
#define SNMP_ALARM_CODE_HardDiskMountFail       7002        // 硬盘挂载失败
#define SNMP_ALARM_CODE_EmergencyCall           7003        // 紧急呼叫告警
#define SNMP_ALARM_CODE_EmergencyBrace          7004        // 紧急停车告警
#define SNMP_ALARM_CODE_SelfCheckFailed         7005        // 自检失败告警
#define SNMP_ALARM_CODE_CoverOpened             7006        // 设备盖打开告警
#define SNMP_ALARM_CODE_SwitchToMaster          7007        // 切换到主用状态
#define SNMP_ALARM_CODE_SwitchToSlave           7008        // 切换到备用状态
#define SNMP_ALARM_CODE_GetATSDataFail          7009        // ATS 信息获取失败
#define SNMP_ALARM_CODE_SyncTimeFail            7010        // NTP 信息获取失败
#define SNMP_ALARM_CODE_LinkToLteFail           7011        // LTE 信息获取失败
#define SNMP_ALARM_CODE_LanStatus               7012        // LAN口插拔告警告
#define SNMP_ALARM_CODE_DevOffLine              7013        // 设备离线告警
#define SNMP_ALARM_CODE_Reboot                  7014        // 设备重启告警
#define SNMP_ALARM_CODE_DevTempTooHigh          7015        // 设备温度过高告警
#define SNMP_ALARM_CODE_PISLinkFail             7016        // IPH 连接失败告警
#define SNMP_ALARM_CODE_PALinkFail              7017        // PA 连接失败告警
#define SNMP_ALARM_CODE_FileStorageTooHigh      7018        // 文件占用存储空间过高告警
#define SNMP_ALARM_CODE_All                     7099        // 所有告警

static SNMP_OID oid_channelUsing                [] = {1,3,6,1,4,1,28723,11,10,2,3};
static SNMP_OID oid_transmitPower               [] = {1,3,6,1,4,1,28723,11,10,3,4};
static SNMP_OID oid_wifi5ChannelUsing           [] = {1,3,6,1,4,1,28723,11,15,2,3};
static SNMP_OID oid_wifi5TransmitPower          [] = {1,3,6,1,4,1,28723,11,15,3,4};

static SNMP_OID oid_ipMode                      [] = {1,3,6,1,4,1,28723,11,1,4,10,1,2}; // 矢量表
static SNMP_OID oid_ipAddress                   [] = {1,3,6,1,4,1,28723,11,1,4,10,1,3}; // 矢量表
static SNMP_OID oid_ipNetMask                   [] = {1,3,6,1,4,1,28723,11,1,4,10,1,4}; // 矢量表
static SNMP_OID oid_ipGatewayAddress            [] = {1,3,6,1,4,1,28723,11,1,4,10,1,5}; // 矢量表

static SNMP_OID oid_sysMacAddress               [] = {1,3,6,1,4,1,28723,11,1,1,1,0};
static SNMP_OID oid_sysUptime                   [] = {1,3,6,1,4,1,28723,11,1,1,2,0};    //  设备运行时长
static SNMP_OID oid_sysRegetPeriod              [] = {1,3,6,1,4,1,28723,11,1,1,3,0};    //  SNMP 数据刷新时间(TAU)
static SNMP_OID oid_sysRestart                  [] = {1,3,6,1,4,1,28723,11,1,1,6,0};    //  normal(0),restart(1)
static SNMP_OID oid_sysIpaddress                [] = {1,3,6,1,4,1,28723,11,1,1,9,0};
static SNMP_OID oid_sysManageState              [] = {1,3,6,1,4,1,28723,11,1,1,10,0};
static SNMP_OID oid_modelId                     [] = {1,3,6,1,4,1,28723,11,1,1,11,0};
static SNMP_OID oid_modelName                   [] = {1,3,6,1,4,1,28723,11,1,1,12,0};
static SNMP_OID oid_sysModel                    [] = {1,3,6,1,4,1,28723,11,1,1,13,0};
static SNMP_OID oid_devType                     [] = {1,3,6,1,4,1,28723,11,1,1,16,0};
static SNMP_OID oid_sysName                     [] = {1,3,6,1,4,1,28723,11,1,1,17,0};
static SNMP_OID oid_sysHotId                    [] = {1,3,6,1,4,1,28723,11,1,1,18,0};
static SNMP_OID oid_RunState                    [] = {1,3,6,1,4,1,28723,11,1,1,19,0};   // 运行状态
static SNMP_OID oid_TCID                        [] = {1,3,6,1,4,1,28723,11,1,1,20,0};   // TC端信息
static SNMP_OID oid_sysMaxStaNum                [] = {1,3,6,1,4,1,28723,11,1,1,27,0};

static SNMP_OID oid_currentVersion              [] = {1,3,6,1,4,1,28723,11,1,2,1,0};    //  设备当前版本
static SNMP_OID oid_targetVersion               [] = {1,3,6,1,4,1,28723,11,1,2,2,0};    //  设备目标版本(TAU)
static SNMP_OID oid_softwareVersion             [] = {1,3,6,1,4,1,28723,11,1,2,3,0};    //  设备软件版本

static SNMP_OID oid_memorySize                  [] = {1,3,6,1,4,1,28723,11,1,3,3,0};
static SNMP_OID oid_hardwareVersion             [] = {1,3,6,1,4,1,28723,11,1,3,5,0};    // 设备硬件版本
static SNMP_OID oid_hardwareType                [] = {1,3,6,1,4,1,28723,11,1,3,6,0};
static SNMP_OID oid_boardVersion                [] = {1,3,6,1,4,1,28723,11,1,3,7,0};

static SNMP_OID oid_CpuTemp                     [] = {1,3,6,1,4,1,28723,60,1,3,3,0};
static SNMP_OID oid_CpuUsage                    [] = {1,3,6,1,4,1,28723,60,1,3,4,0};
static SNMP_OID oid_MemUsage                    [] = {1,3,6,1,4,1,28723,60,1,3,5,0};
static SNMP_OID oid_DiskUsage                   [] = {1,3,6,1,4,1,28723,60,1,3,6,0};
static SNMP_OID oid_DiskLogsaveUsage            [] = {1,3,6,1,4,1,28723,60,1,3,7,0};    // TAU特有
static SNMP_OID oid_DiskPackageUsage            [] = {1,3,6,1,4,1,28723,60,1,3,8,0};    // TAU特有
static SNMP_OID oid_DiskModuleUsage             [] = {1,3,6,1,4,1,28723,60,1,3,9,0};    // TAU特有

static SNMP_OID oid_sysHardwareVer              [] = {1,3,6,1,4,1,28723,70,4,1,0};
static SNMP_OID oid_sysSoftwareVer              [] = {1,3,6,1,4,1,28723,70,4,2,0};
static SNMP_OID oid_sysRadioVer                 [] = {1,3,6,1,4,1,28723,70,4,3,0};
static SNMP_OID oid_sysAndroidVer               [] = {1,3,6,1,4,1,28723,70,4,4,0};

// 设备状态信息
static SNMP_OID oid_GatewayReachable            [] = {1,3,6,1,4,1,28723,70,1,2,0};   // 网关连通状态 0:不通; 1:连通;
static SNMP_OID oid_MDCRegStatus                [] = {1,3,6,1,4,1,28723,70,1,3,0};   // 注册到原装调度服务器状态 0:未注册; 1:已注册;
static SNMP_OID oid_OMCLinkStatus               [] = {1,3,6,1,4,1,28723,70,1,4,0};   // 网管服务器连接状态 0:未连接; 1:连接;
static SNMP_OID oid_DISLinkStatus               [] = {1,3,6,1,4,1,28723,70,1,5,0};   // 二开调度服务器连接状态 0:未连接; 1:连接;
static SNMP_OID oid_SelfCheckStatus             [] = {1,3,6,1,4,1,28723,70,1,6,0};   // 终端自检状态 0:失败; 1:成功;
static SNMP_OID oid_DevCoverStatus              [] = {1,3,6,1,4,1,28723,70,1,7,0};   // 设备盖状态 0:关闭; 1:打开;
static SNMP_OID oid_DriveMode                   [] = {1,3,6,1,4,1,28723,70,1,8,0};   // 驾驶模式 0:无人驾驶; 1:有人驾驶;
static SNMP_OID oid_EmergencyCallStatus         [] = {1,3,6,1,4,1,28723,70,1,9,0};   // 紧急呼叫状态 0:否; 1:是;
static SNMP_OID oid_EmergencyBraceStatus        [] = {1,3,6,1,4,1,28723,70,1,10,0};  // 紧急停车状态 0:否; 1:是;
static SNMP_OID oid_PISLinkStatus               [] = {1,3,6,1,4,1,28723,70,1,11,0};  // PIS连接状态 0:异常; 1:正常;
static SNMP_OID oid_NTPAddr                     [] = {1,3,6,1,4,1,28723,70,1,12,0};  // NTP服务器地址
static SNMP_OID oid_NTPPort                     [] = {1,3,6,1,4,1,28723,70,1,13,0};  // NTP服务端口
static SNMP_OID oid_NTPSyncTime                 [] = {1,3,6,1,4,1,28723,70,1,14,0};  // NTP同步间隔
static SNMP_OID oid_PALinkStatus                [] = {1,3,6,1,4,1,28723,70,1,15,0};  // PA连接状态 0:异常; 1:正常;
static SNMP_OID oid_StationID                   [] = {1,3,6,1,4,1,28723,70,1,16,0};  // 列车当前站(TX/FX特有)
static SNMP_OID oid_NTPStatus                   [] = {1,3,6,1,4,1,28723,70,1,17,0};  // NTP连接状态 0:未连接; 1:连接;
static SNMP_OID oid_LTEStatus                   [] = {1,3,6,1,4,1,28723,70,1,18,0};  // LTE连接状态 0:未连接; 1:连接;
static SNMP_OID oid_ATSStatus                   [] = {1,3,6,1,4,1,28723,70,1,19,0};  // ATS连接状态 0:未连接; 1:连接;
static SNMP_OID oid_ClusterStatus               [] = {1,3,6,1,4,1,28723,70,1,20,0};  // 主备状态 0:独立运行;1:主用;2:备用;

// 文件列表 矢量表
static SNMP_OID oid_FileListTable               [] = {1,3,6,1,4,1,28723,70,2};
static SNMP_OID oid_FileListEntry               [] = {1,3,6,1,4,1,28723,70,2,1};
static SNMP_OID oid_DevFileIndex                [] = {1,3,6,1,4,1,28723,70,2,1,1};
static SNMP_OID oid_DevMacAddress               [] = {1,3,6,1,4,1,28723,70,2,1,2};
static SNMP_OID oid_DevType                     [] = {1,3,6,1,4,1,28723,70,2,1,3};
static SNMP_OID oid_DevFileType                 [] = {1,3,6,1,4,1,28723,70,2,1,4};  // 1(voice),2(log)
static SNMP_OID oid_DevFileName                 [] = {1,3,6,1,4,1,28723,70,2,1,5};
static SNMP_OID oid_DevFileSize                 [] = {1,3,6,1,4,1,28723,70,2,1,6};  // 带单位
static SNMP_OID oid_DevFileTime                 [] = {1,3,6,1,4,1,28723,70,2,1,7};  // 时间戳
static SNMP_OID oid_LastDevFileIndex            [] = {1,3,6,1,4,1,28723,70,2,1,8};  // 0:否 1：是

// 操作日志 矢量表
static SNMP_OID oid_OperLogTable                [] = {1,3,6,1,4,1,28723,70,5};
static SNMP_OID oid_OperLogEntry                [] = {1,3,6,1,4,1,28723,70,5,1};
static SNMP_OID oid_OperLogIndex                [] = {1,3,6,1,4,1,28723,70,5,1,1};
static SNMP_OID oid_DevID                       [] = {1,3,6,1,4,1,28723,70,5,1,2};
static SNMP_OID oid_DevOperation                [] = {1,3,6,1,4,1,28723,70,5,1,3};
static SNMP_OID oid_DevOperTime                 [] = {1,3,6,1,4,1,28723,70,5,1,4};  // 时间戳
static SNMP_OID oid_LastOperLogIndex            [] = {1,3,6,1,4,1,28723,70,5,1,5};  // 0:否 1:是

static SNMP_OID oid_SIMIMSI                     [] = {1,3,6,1,4,1,28723,60,2,4,1,2,0};  // SIM 卡信息（IMSI）
// LTE信息
static SNMP_OID oid_LteRSSI                     [] = {1,3,6,1,4,1,28723,60,2,4,3,1,0};
static SNMP_OID oid_LteRSRP                     [] = {1,3,6,1,4,1,28723,60,2,4,3,2,0};
static SNMP_OID oid_LteRSRQ                     [] = {1,3,6,1,4,1,28723,60,2,4,3,3,0};
static SNMP_OID oid_LteSINR                     [] = {1,3,6,1,4,1,28723,60,2,4,3,4,0};
static SNMP_OID oid_LtePCI                      [] = {1,3,6,1,4,1,28723,60,2,4,4,3,0};    // 物理小区 ID

// 系统操作
static SNMP_OID oid_SoftwareUpdate              [] = {1,3,6,1,4,1,28723,14,1,1,0};  // normal(0),update(1)
static SNMP_OID oid_SysRestart                  [] = {1,3,6,1,4,1,28723,14,1,2,0};  // normal(0),restart(1)
static SNMP_OID oid_SysReset                    [] = {1,3,6,1,4,1,28723,14,1,3,0};  // normal(0),reset(1)
static SNMP_OID oid_CfgDataFileImpExp           [] = {1,3,6,1,4,1,28723,14,1,4,0};  // normal(0),import(1),export(2)
static SNMP_OID oid_GetLastFileListIndex        [] = {1,3,6,1,4,1,28723,14,1,5,0};  // normal(0),import(1),export(2)
static SNMP_OID oid_GetLastOperLogIndex         [] = {1,3,6,1,4,1,28723,14,1,6,0};  // normal(0),import(1),export(2)
static SNMP_OID oid_sendOfflineConfigFile       [] = {1,3,6,1,4,1,28723,14,1,7,0};

// 文件传输
static SNMP_OID oid_FileTrans_LoadFlag          [] = {1,3,6,1,4,1,28723,14,2,1,0};   // download(1), upload(2)
static SNMP_OID oid_FileTrans_FileName          [] = {1,3,6,1,4,1,28723,14,2,2,0};
static SNMP_OID oid_FileTrans_FileType          [] = {1,3,6,1,4,1,28723,14,2,3,0};   // software(1), config(2), diagnose(3), other(4)
static SNMP_OID oid_FileTrans_TransProtocol     [] = {1,3,6,1,4,1,28723,14,2,4,0};   // ftp(1), tftp(2), http(3), https(4)
static SNMP_OID oid_FileTrans_ServerAddr        [] = {1,3,6,1,4,1,28723,14,2,5,0};
static SNMP_OID oid_FileTrans_ServerPort        [] = {1,3,6,1,4,1,28723,14,2,6,0};
static SNMP_OID oid_FileTrans_ServerUserName    [] = {1,3,6,1,4,1,28723,14,2,7,0};
static SNMP_OID oid_FileTrans_ServerPasswd      [] = {1,3,6,1,4,1,28723,14,2,8,0};
static SNMP_OID oid_FileTrans_TransStatus       [] = {1,3,6,1,4,1,28723,14,2,9,0};   // success(1), get-file-fail(2), file-wrong(3), flash-error(4)
static SNMP_OID oid_FileTrans_FailReason        [] = {1,3,6,1,4,1,28723,14,2,10,0};
static SNMP_OID oid_FileTrans_FileVersion       [] = {1,3,6,1,4,1,28723,14,2,11,0};

// TAU 状态信息-端口流量统计
static SNMP_OID oid_WANTxBytes                  [] = {1,3,6,1,4,1,28723,60,4,1,1,0};   //  wan口发送字节数
static SNMP_OID oid_WANRxBytes                  [] = {1,3,6,1,4,1,28723,60,4,1,2,0};   //  wan口接收字节数
static SNMP_OID oid_LAN1TxBytes                 [] = {1,3,6,1,4,1,28723,60,4,1,3,0};   //  lan1口发送字节数
static SNMP_OID oid_LAN1RxBytes                 [] = {1,3,6,1,4,1,28723,60,4,1,4,0};   //  lan1口发送字节数
static SNMP_OID oid_LAN2TxBytes                 [] = {1,3,6,1,4,1,28723,60,4,1,5,0};   //  lan2口发送字节数
static SNMP_OID oid_LAN2RxBytes                 [] = {1,3,6,1,4,1,28723,60,4,1,6,0};   //  lan2口发送字节数
static SNMP_OID oid_LAN3TxBytes                 [] = {1,3,6,1,4,1,28723,60,4,1,7,0};   //  lan3口发送字节数
static SNMP_OID oid_LAN3RxBytes                 [] = {1,3,6,1,4,1,28723,60,4,1,8,0};   //  lan3口发送字节数
static SNMP_OID oid_LAN4TxBytes                 [] = {1,3,6,1,4,1,28723,60,4,1,9,0};   //  lan4口发送字节数
static SNMP_OID oid_LAN4RxBytes                 [] = {1,3,6,1,4,1,28723,60,4,1,10,0};  //  lan4口发送字节数


/**********************告警处理意见ZWJ*************************************************/
#define DiskUsageTooHighAlarmInfo        "请查看设备中存储空间占用情况,清理不需要的文件"
#define DevRebootAlarmInfo               "请确认是否为人为重启或者车辆日常断电,如设备经常无故重启,可能有异常"

#define DevSwitchToMasterAlarmInfo       "调度服务器已经切换到主用状态"
#define DevSwitchToSlaveAlarmInfo        "调度服务器已经切换到备用状态"

#define DevGetATSDataFailAlarmInfo       "请尽快确认获取ATS数据失败原因"
#define DevSyncTimeFailAlarmInfo         "请尽快确认时间同步失败原因"
#define DevLinkToLteFailAlarmInfo        "请尽快确认连接核心网失败原因"

#define DevCpuUsageTooHighAlarmInfo      "短时间的CPU利用率过高属于正常,长时间利用率过高,可能设备运行异常,请重启后再次排查"  //TX/FX
#define DevMemUsageTooHighAlarmInfo      "短时间的内存利用率过高属于正常,长时间利用率过高,可能设备运行异常,请重启后再次排查" //Tx/FX

#define DevEmergencyCallAlarmInfo        "请尽快确认紧急呼叫详情,并排查解决紧急情况"   // 紧急呼叫告警
#define DevEmergencyBraceAlarmInfo       "请尽快确认发生紧急停车的列车状况" // 紧急停车告警
#define DevSelfCheckFailedAlarmInfo      "请尽快解决自检失败项的问题,以保证设备正常运行"   // 自检失败告警
#define DevCoverOpenedAlarmInfo          "请确认是否为人为动作所致,本条目会检查列车一端共5个设备盖的状态，有任一盖子被打开即会告警"    // 设备盖打开告警
#define DevIfStatusAlarmInfo             "请检查设备是否有线缆松动情况,或者重启设备后再次排查" // 端口状态告警
#define DevOffLineAlarmInfo              "设备离线,请确认是否为正常运营下线"  //设备离线
/**********************告警处理意见END********************************************/


/***********************告警标题ZWJ*************************************************/
#define DevOffLineAlarmTitle             "设备离线"
/***********************告警标题END*************************************************/


/***********************告警原因ZWJ*************************************************/
#define DevOffLineAlarmRason             "设备离线"
/***********************告警原因END*************************************************/

#endif

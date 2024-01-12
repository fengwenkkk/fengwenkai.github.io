#ifndef OID_H
#define OID_H

static SNMP_OID SNMP_TRAP[] = {SNMP_TRAP_OID};

static SNMP_OID oid_AuthFailed      [] = {1,3,6,1,6,3,1,1,5,5,0}; // ��Ȩʧ��

//�澯����
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

// �澯
static SNMP_OID oid_CpuUsageTooHighTrap         [] = {1,3,6,1,4,1,28723,12,10,1,3,0};   // �豸CPU�����ʹ��߸澯
static SNMP_OID oid_MemUsageTooHighTrap         [] = {1,3,6,1,4,1,28723,12,10,1,5,0};   // �豸�ڴ������ʹ��߸澯
static SNMP_OID oid_DiscoveryTrap               [] = {1,3,6,1,4,1,28723,12,10,1,7,0};   // ���߱���
static SNMP_OID oid_OffLineTrap                 [] = {1,3,6,1,4,1,28723,12,10,1,10,0};  // �豸���߸澯 ZWJ
static SNMP_OID oid_HeartBeatTrap               [] = {1,3,6,1,4,1,28723,12,10,1,11,0};  // ��������
static SNMP_OID oid_LteLanIDTrap                [] = {1,3,6,1,4,1,28723,12,10,1,30,0};  // TAU LTE С���л��澯
static SNMP_OID oid_LanStatusTrap               [] = {1,3,6,1,4,1,28723,12,10,1,31,0};  // �˿�״̬�澯/TAU(LAN�ڲ�θ澯)
static SNMP_OID oid_LteWeakSignalTrap           [] = {1,3,6,1,4,1,28723,12,10,1,32,0};  // TAU LTE �ź����澯�澯
static SNMP_OID oid_PowerDownTrap               [] = {1,3,6,1,4,1,28723,12,10,1,33,0};  // ����澯
static SNMP_OID oid_RebootTrap                  [] = {1,3,6,1,4,1,28723,12,10,1,34,0};  // �豸�����澯
static SNMP_OID oid_DevTempTrap                 [] = {1,3,6,1,4,1,28723,12,10,1,35,0};  // �豸�¶ȸ澯

static SNMP_OID oid_DiskUsageTooHighTrap        [] = {1,3,6,1,4,1,28723,70,3,1,0};      // ����ռ���ʹ��߸澯
static SNMP_OID oid_EmergencyCallTrap           [] = {1,3,6,1,4,1,28723,70,3,3,0};      // �������и澯
static SNMP_OID oid_EmergencyBraceTrap          [] = {1,3,6,1,4,1,28723,70,3,4,0};      // ����ͣ���澯
static SNMP_OID oid_SelfCheckFailTrap           [] = {1,3,6,1,4,1,28723,70,3,5,0};      // �Լ�ʧ�ܸ澯
//static SNMP_OID oid_CoverOpenedTrap               [] = {1,3,6,1,4,1,28723,70,3,6 };     // �豸�Ǵ򿪸澯 ֻ�鿴״̬�������澯
static SNMP_OID oid_SwitchToMasterTrap          [] = {1,3,6,1,4,1,28723,70,3,7,0};      // �л�������״̬�澯
static SNMP_OID oid_SwitchToSlaveTrap           [] = {1,3,6,1,4,1,28723,70,3,8,0};      // �л�������״̬�澯
static SNMP_OID oid_GetATSDataFailTrap          [] = {1,3,6,1,4,1,28723,70,3,9,0};      // ��ȡATS����ʧ�澯
static SNMP_OID oid_SyncTimeFailTrap            [] = {1,3,6,1,4,1,28723,70,3,10,0};     // ʱ��ͬ��ʧ�ܸ澯
static SNMP_OID oid_LinkToLteFailTrap           [] = {1,3,6,1,4,1,28723,70,3,11,0};     // ���Ӻ�����ʧ�ܸ澯
static SNMP_OID oid_IPHLinkFailTrap             [] = {1,3,6,1,4,1,28723,70,3,12,0};     // IPH ����ʧ�ܸ澯
static SNMP_OID oid_PALinkFailTrap              [] = {1,3,6,1,4,1,28723,70,3,13,0};     // PA ����ʧ�ܸ澯
static SNMP_OID oid_HardDiskMountFailTrap       [] = {1,3,6,1,4,1,28723,70,3,14,0};     // Ӳ�̹���ʧ�ܸ澯(TX/FX����)
static SNMP_OID oid_FileStorageTooHighTrap      [] = {1,3,6,1,4,1,28723,70,3,15,0};     // �ļ�ռ�ô洢�ռ���߸澯(MRS����)

// �澯��
#define SNMP_ALARM_CODE_CpuUsageTooHigh         1003        // CPU�����ʹ���
#define SNMP_ALARM_CODE_MemUsageTooHigh         1004        // �ڴ������ʹ���
#define SNMP_ALARM_CODE_PowerDown               3004        // �豸����澯
#define SNMP_ALARM_CODE_DiskUsageTooHigh        7001        // ����ռ���ʹ���
#define SNMP_ALARM_CODE_HardDiskMountFail       7002        // Ӳ�̹���ʧ��
#define SNMP_ALARM_CODE_EmergencyCall           7003        // �������и澯
#define SNMP_ALARM_CODE_EmergencyBrace          7004        // ����ͣ���澯
#define SNMP_ALARM_CODE_SelfCheckFailed         7005        // �Լ�ʧ�ܸ澯
#define SNMP_ALARM_CODE_CoverOpened             7006        // �豸�Ǵ򿪸澯
#define SNMP_ALARM_CODE_SwitchToMaster          7007        // �л�������״̬
#define SNMP_ALARM_CODE_SwitchToSlave           7008        // �л�������״̬
#define SNMP_ALARM_CODE_GetATSDataFail          7009        // ATS ��Ϣ��ȡʧ��
#define SNMP_ALARM_CODE_SyncTimeFail            7010        // NTP ��Ϣ��ȡʧ��
#define SNMP_ALARM_CODE_LinkToLteFail           7011        // LTE ��Ϣ��ȡʧ��
#define SNMP_ALARM_CODE_LanStatus               7012        // LAN�ڲ�θ澯��
#define SNMP_ALARM_CODE_DevOffLine              7013        // �豸���߸澯
#define SNMP_ALARM_CODE_Reboot                  7014        // �豸�����澯
#define SNMP_ALARM_CODE_DevTempTooHigh          7015        // �豸�¶ȹ��߸澯
#define SNMP_ALARM_CODE_PISLinkFail             7016        // IPH ����ʧ�ܸ澯
#define SNMP_ALARM_CODE_PALinkFail              7017        // PA ����ʧ�ܸ澯
#define SNMP_ALARM_CODE_FileStorageTooHigh      7018        // �ļ�ռ�ô洢�ռ���߸澯
#define SNMP_ALARM_CODE_All                     7099        // ���и澯

static SNMP_OID oid_channelUsing                [] = {1,3,6,1,4,1,28723,11,10,2,3};
static SNMP_OID oid_transmitPower               [] = {1,3,6,1,4,1,28723,11,10,3,4};
static SNMP_OID oid_wifi5ChannelUsing           [] = {1,3,6,1,4,1,28723,11,15,2,3};
static SNMP_OID oid_wifi5TransmitPower          [] = {1,3,6,1,4,1,28723,11,15,3,4};

static SNMP_OID oid_ipMode                      [] = {1,3,6,1,4,1,28723,11,1,4,10,1,2}; // ʸ����
static SNMP_OID oid_ipAddress                   [] = {1,3,6,1,4,1,28723,11,1,4,10,1,3}; // ʸ����
static SNMP_OID oid_ipNetMask                   [] = {1,3,6,1,4,1,28723,11,1,4,10,1,4}; // ʸ����
static SNMP_OID oid_ipGatewayAddress            [] = {1,3,6,1,4,1,28723,11,1,4,10,1,5}; // ʸ����

static SNMP_OID oid_sysMacAddress               [] = {1,3,6,1,4,1,28723,11,1,1,1,0};
static SNMP_OID oid_sysUptime                   [] = {1,3,6,1,4,1,28723,11,1,1,2,0};    //  �豸����ʱ��
static SNMP_OID oid_sysRegetPeriod              [] = {1,3,6,1,4,1,28723,11,1,1,3,0};    //  SNMP ����ˢ��ʱ��(TAU)
static SNMP_OID oid_sysRestart                  [] = {1,3,6,1,4,1,28723,11,1,1,6,0};    //  normal(0),restart(1)
static SNMP_OID oid_sysIpaddress                [] = {1,3,6,1,4,1,28723,11,1,1,9,0};
static SNMP_OID oid_sysManageState              [] = {1,3,6,1,4,1,28723,11,1,1,10,0};
static SNMP_OID oid_modelId                     [] = {1,3,6,1,4,1,28723,11,1,1,11,0};
static SNMP_OID oid_modelName                   [] = {1,3,6,1,4,1,28723,11,1,1,12,0};
static SNMP_OID oid_sysModel                    [] = {1,3,6,1,4,1,28723,11,1,1,13,0};
static SNMP_OID oid_devType                     [] = {1,3,6,1,4,1,28723,11,1,1,16,0};
static SNMP_OID oid_sysName                     [] = {1,3,6,1,4,1,28723,11,1,1,17,0};
static SNMP_OID oid_sysHotId                    [] = {1,3,6,1,4,1,28723,11,1,1,18,0};
static SNMP_OID oid_RunState                    [] = {1,3,6,1,4,1,28723,11,1,1,19,0};   // ����״̬
static SNMP_OID oid_TCID                        [] = {1,3,6,1,4,1,28723,11,1,1,20,0};   // TC����Ϣ
static SNMP_OID oid_sysMaxStaNum                [] = {1,3,6,1,4,1,28723,11,1,1,27,0};

static SNMP_OID oid_currentVersion              [] = {1,3,6,1,4,1,28723,11,1,2,1,0};    //  �豸��ǰ�汾
static SNMP_OID oid_targetVersion               [] = {1,3,6,1,4,1,28723,11,1,2,2,0};    //  �豸Ŀ��汾(TAU)
static SNMP_OID oid_softwareVersion             [] = {1,3,6,1,4,1,28723,11,1,2,3,0};    //  �豸����汾

static SNMP_OID oid_memorySize                  [] = {1,3,6,1,4,1,28723,11,1,3,3,0};
static SNMP_OID oid_hardwareVersion             [] = {1,3,6,1,4,1,28723,11,1,3,5,0};    // �豸Ӳ���汾
static SNMP_OID oid_hardwareType                [] = {1,3,6,1,4,1,28723,11,1,3,6,0};
static SNMP_OID oid_boardVersion                [] = {1,3,6,1,4,1,28723,11,1,3,7,0};

static SNMP_OID oid_CpuTemp                     [] = {1,3,6,1,4,1,28723,60,1,3,3,0};
static SNMP_OID oid_CpuUsage                    [] = {1,3,6,1,4,1,28723,60,1,3,4,0};
static SNMP_OID oid_MemUsage                    [] = {1,3,6,1,4,1,28723,60,1,3,5,0};
static SNMP_OID oid_DiskUsage                   [] = {1,3,6,1,4,1,28723,60,1,3,6,0};
static SNMP_OID oid_DiskLogsaveUsage            [] = {1,3,6,1,4,1,28723,60,1,3,7,0};    // TAU����
static SNMP_OID oid_DiskPackageUsage            [] = {1,3,6,1,4,1,28723,60,1,3,8,0};    // TAU����
static SNMP_OID oid_DiskModuleUsage             [] = {1,3,6,1,4,1,28723,60,1,3,9,0};    // TAU����

static SNMP_OID oid_sysHardwareVer              [] = {1,3,6,1,4,1,28723,70,4,1,0};
static SNMP_OID oid_sysSoftwareVer              [] = {1,3,6,1,4,1,28723,70,4,2,0};
static SNMP_OID oid_sysRadioVer                 [] = {1,3,6,1,4,1,28723,70,4,3,0};
static SNMP_OID oid_sysAndroidVer               [] = {1,3,6,1,4,1,28723,70,4,4,0};

// �豸״̬��Ϣ
static SNMP_OID oid_GatewayReachable            [] = {1,3,6,1,4,1,28723,70,1,2,0};   // ������ͨ״̬ 0:��ͨ; 1:��ͨ;
static SNMP_OID oid_MDCRegStatus                [] = {1,3,6,1,4,1,28723,70,1,3,0};   // ע�ᵽԭװ���ȷ�����״̬ 0:δע��; 1:��ע��;
static SNMP_OID oid_OMCLinkStatus               [] = {1,3,6,1,4,1,28723,70,1,4,0};   // ���ܷ���������״̬ 0:δ����; 1:����;
static SNMP_OID oid_DISLinkStatus               [] = {1,3,6,1,4,1,28723,70,1,5,0};   // �������ȷ���������״̬ 0:δ����; 1:����;
static SNMP_OID oid_SelfCheckStatus             [] = {1,3,6,1,4,1,28723,70,1,6,0};   // �ն��Լ�״̬ 0:ʧ��; 1:�ɹ�;
static SNMP_OID oid_DevCoverStatus              [] = {1,3,6,1,4,1,28723,70,1,7,0};   // �豸��״̬ 0:�ر�; 1:��;
static SNMP_OID oid_DriveMode                   [] = {1,3,6,1,4,1,28723,70,1,8,0};   // ��ʻģʽ 0:���˼�ʻ; 1:���˼�ʻ;
static SNMP_OID oid_EmergencyCallStatus         [] = {1,3,6,1,4,1,28723,70,1,9,0};   // ��������״̬ 0:��; 1:��;
static SNMP_OID oid_EmergencyBraceStatus        [] = {1,3,6,1,4,1,28723,70,1,10,0};  // ����ͣ��״̬ 0:��; 1:��;
static SNMP_OID oid_PISLinkStatus               [] = {1,3,6,1,4,1,28723,70,1,11,0};  // PIS����״̬ 0:�쳣; 1:����;
static SNMP_OID oid_NTPAddr                     [] = {1,3,6,1,4,1,28723,70,1,12,0};  // NTP��������ַ
static SNMP_OID oid_NTPPort                     [] = {1,3,6,1,4,1,28723,70,1,13,0};  // NTP����˿�
static SNMP_OID oid_NTPSyncTime                 [] = {1,3,6,1,4,1,28723,70,1,14,0};  // NTPͬ�����
static SNMP_OID oid_PALinkStatus                [] = {1,3,6,1,4,1,28723,70,1,15,0};  // PA����״̬ 0:�쳣; 1:����;
static SNMP_OID oid_StationID                   [] = {1,3,6,1,4,1,28723,70,1,16,0};  // �г���ǰվ(TX/FX����)
static SNMP_OID oid_NTPStatus                   [] = {1,3,6,1,4,1,28723,70,1,17,0};  // NTP����״̬ 0:δ����; 1:����;
static SNMP_OID oid_LTEStatus                   [] = {1,3,6,1,4,1,28723,70,1,18,0};  // LTE����״̬ 0:δ����; 1:����;
static SNMP_OID oid_ATSStatus                   [] = {1,3,6,1,4,1,28723,70,1,19,0};  // ATS����״̬ 0:δ����; 1:����;
static SNMP_OID oid_ClusterStatus               [] = {1,3,6,1,4,1,28723,70,1,20,0};  // ����״̬ 0:��������;1:����;2:����;

// �ļ��б� ʸ����
static SNMP_OID oid_FileListTable               [] = {1,3,6,1,4,1,28723,70,2};
static SNMP_OID oid_FileListEntry               [] = {1,3,6,1,4,1,28723,70,2,1};
static SNMP_OID oid_DevFileIndex                [] = {1,3,6,1,4,1,28723,70,2,1,1};
static SNMP_OID oid_DevMacAddress               [] = {1,3,6,1,4,1,28723,70,2,1,2};
static SNMP_OID oid_DevType                     [] = {1,3,6,1,4,1,28723,70,2,1,3};
static SNMP_OID oid_DevFileType                 [] = {1,3,6,1,4,1,28723,70,2,1,4};  // 1(voice),2(log)
static SNMP_OID oid_DevFileName                 [] = {1,3,6,1,4,1,28723,70,2,1,5};
static SNMP_OID oid_DevFileSize                 [] = {1,3,6,1,4,1,28723,70,2,1,6};  // ����λ
static SNMP_OID oid_DevFileTime                 [] = {1,3,6,1,4,1,28723,70,2,1,7};  // ʱ���
static SNMP_OID oid_LastDevFileIndex            [] = {1,3,6,1,4,1,28723,70,2,1,8};  // 0:�� 1����

// ������־ ʸ����
static SNMP_OID oid_OperLogTable                [] = {1,3,6,1,4,1,28723,70,5};
static SNMP_OID oid_OperLogEntry                [] = {1,3,6,1,4,1,28723,70,5,1};
static SNMP_OID oid_OperLogIndex                [] = {1,3,6,1,4,1,28723,70,5,1,1};
static SNMP_OID oid_DevID                       [] = {1,3,6,1,4,1,28723,70,5,1,2};
static SNMP_OID oid_DevOperation                [] = {1,3,6,1,4,1,28723,70,5,1,3};
static SNMP_OID oid_DevOperTime                 [] = {1,3,6,1,4,1,28723,70,5,1,4};  // ʱ���
static SNMP_OID oid_LastOperLogIndex            [] = {1,3,6,1,4,1,28723,70,5,1,5};  // 0:�� 1:��

static SNMP_OID oid_SIMIMSI                     [] = {1,3,6,1,4,1,28723,60,2,4,1,2,0};  // SIM ����Ϣ��IMSI��
// LTE��Ϣ
static SNMP_OID oid_LteRSSI                     [] = {1,3,6,1,4,1,28723,60,2,4,3,1,0};
static SNMP_OID oid_LteRSRP                     [] = {1,3,6,1,4,1,28723,60,2,4,3,2,0};
static SNMP_OID oid_LteRSRQ                     [] = {1,3,6,1,4,1,28723,60,2,4,3,3,0};
static SNMP_OID oid_LteSINR                     [] = {1,3,6,1,4,1,28723,60,2,4,3,4,0};
static SNMP_OID oid_LtePCI                      [] = {1,3,6,1,4,1,28723,60,2,4,4,3,0};    // ����С�� ID

// ϵͳ����
static SNMP_OID oid_SoftwareUpdate              [] = {1,3,6,1,4,1,28723,14,1,1,0};  // normal(0),update(1)
static SNMP_OID oid_SysRestart                  [] = {1,3,6,1,4,1,28723,14,1,2,0};  // normal(0),restart(1)
static SNMP_OID oid_SysReset                    [] = {1,3,6,1,4,1,28723,14,1,3,0};  // normal(0),reset(1)
static SNMP_OID oid_CfgDataFileImpExp           [] = {1,3,6,1,4,1,28723,14,1,4,0};  // normal(0),import(1),export(2)
static SNMP_OID oid_GetLastFileListIndex        [] = {1,3,6,1,4,1,28723,14,1,5,0};  // normal(0),import(1),export(2)
static SNMP_OID oid_GetLastOperLogIndex         [] = {1,3,6,1,4,1,28723,14,1,6,0};  // normal(0),import(1),export(2)
static SNMP_OID oid_sendOfflineConfigFile       [] = {1,3,6,1,4,1,28723,14,1,7,0};

// �ļ�����
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

// TAU ״̬��Ϣ-�˿�����ͳ��
static SNMP_OID oid_WANTxBytes                  [] = {1,3,6,1,4,1,28723,60,4,1,1,0};   //  wan�ڷ����ֽ���
static SNMP_OID oid_WANRxBytes                  [] = {1,3,6,1,4,1,28723,60,4,1,2,0};   //  wan�ڽ����ֽ���
static SNMP_OID oid_LAN1TxBytes                 [] = {1,3,6,1,4,1,28723,60,4,1,3,0};   //  lan1�ڷ����ֽ���
static SNMP_OID oid_LAN1RxBytes                 [] = {1,3,6,1,4,1,28723,60,4,1,4,0};   //  lan1�ڷ����ֽ���
static SNMP_OID oid_LAN2TxBytes                 [] = {1,3,6,1,4,1,28723,60,4,1,5,0};   //  lan2�ڷ����ֽ���
static SNMP_OID oid_LAN2RxBytes                 [] = {1,3,6,1,4,1,28723,60,4,1,6,0};   //  lan2�ڷ����ֽ���
static SNMP_OID oid_LAN3TxBytes                 [] = {1,3,6,1,4,1,28723,60,4,1,7,0};   //  lan3�ڷ����ֽ���
static SNMP_OID oid_LAN3RxBytes                 [] = {1,3,6,1,4,1,28723,60,4,1,8,0};   //  lan3�ڷ����ֽ���
static SNMP_OID oid_LAN4TxBytes                 [] = {1,3,6,1,4,1,28723,60,4,1,9,0};   //  lan4�ڷ����ֽ���
static SNMP_OID oid_LAN4RxBytes                 [] = {1,3,6,1,4,1,28723,60,4,1,10,0};  //  lan4�ڷ����ֽ���


/**********************�澯�������ZWJ*************************************************/
#define DiskUsageTooHighAlarmInfo        "��鿴�豸�д洢�ռ�ռ�����,������Ҫ���ļ�"
#define DevRebootAlarmInfo               "��ȷ���Ƿ�Ϊ��Ϊ�������߳����ճ��ϵ�,���豸�����޹�����,�������쳣"

#define DevSwitchToMasterAlarmInfo       "���ȷ������Ѿ��л�������״̬"
#define DevSwitchToSlaveAlarmInfo        "���ȷ������Ѿ��л�������״̬"

#define DevGetATSDataFailAlarmInfo       "�뾡��ȷ�ϻ�ȡATS����ʧ��ԭ��"
#define DevSyncTimeFailAlarmInfo         "�뾡��ȷ��ʱ��ͬ��ʧ��ԭ��"
#define DevLinkToLteFailAlarmInfo        "�뾡��ȷ�����Ӻ�����ʧ��ԭ��"

#define DevCpuUsageTooHighAlarmInfo      "��ʱ���CPU�����ʹ�����������,��ʱ�������ʹ���,�����豸�����쳣,���������ٴ��Ų�"  //TX/FX
#define DevMemUsageTooHighAlarmInfo      "��ʱ����ڴ������ʹ�����������,��ʱ�������ʹ���,�����豸�����쳣,���������ٴ��Ų�" //Tx/FX

#define DevEmergencyCallAlarmInfo        "�뾡��ȷ�Ͻ�����������,���Ų����������"   // �������и澯
#define DevEmergencyBraceAlarmInfo       "�뾡��ȷ�Ϸ�������ͣ�����г�״��" // ����ͣ���澯
#define DevSelfCheckFailedAlarmInfo      "�뾡�����Լ�ʧ���������,�Ա�֤�豸��������"   // �Լ�ʧ�ܸ澯
#define DevCoverOpenedAlarmInfo          "��ȷ���Ƿ�Ϊ��Ϊ��������,����Ŀ�����г�һ�˹�5���豸�ǵ�״̬������һ���ӱ��򿪼���澯"    // �豸�Ǵ򿪸澯
#define DevIfStatusAlarmInfo             "�����豸�Ƿ��������ɶ����,���������豸���ٴ��Ų�" // �˿�״̬�澯
#define DevOffLineAlarmInfo              "�豸����,��ȷ���Ƿ�Ϊ������Ӫ����"  //�豸����
/**********************�澯�������END********************************************/


/***********************�澯����ZWJ*************************************************/
#define DevOffLineAlarmTitle             "�豸����"
/***********************�澯����END*************************************************/


/***********************�澯ԭ��ZWJ*************************************************/
#define DevOffLineAlarmRason             "�豸����"
/***********************�澯ԭ��END*************************************************/

#endif

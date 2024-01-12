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

// DIS���������ļ�
typedef struct
{
    // ��ַ����
    UINT8           aucMdcAddr[4];
    UINT8           aucOmcAddr[4];

    // ATS����
    BOOL            bAutoCloseATS;                  // �Զ��ر�����ATS��socket
    IP_ADDR_T       astATSAddr[2];                  // ATS��������ַ
    BOOL            bSaveATSToFile;                 // �Ƿ񱣴�ATS�ļ�
    UINT32          ulMaxATSFileNum;                // ATS�ļ�������
    UINT32          ulGetATSTime;                   // ��ȡATS��Ϣ�����ڣ��룩
    UINT32          ulATSValidPeriod;               // �ж�ATS�Ƿ������ĳ�ʱʱ�䣨�룩

    // ����̨ҵ������
    BOOL            bAllowEmergencyCall;            // ����̨���̶�̨�Ƿ�֧�ֽ�������
    BOOL            bAllowTXP2PCall;                // ����̨�Ƿ�֧�ָ���
    BOOL            bAllowFXP2PCall;                // �̶�̨�Ƿ�֧�ָ���
    BOOL            bDirectSendApp;
    BOOL            bAPPMsgUseAnsiString;           // APP��Ϣ�Ƿ����ANSI�����ʽ

    INT32           iTransferTrackReqCallMode;      // ת�������ģʽ0:ȫ�� 1�����е�ǰ������ -1�����д����������
    UINT32          ulMaxAppSelfCheckNum;           // APP�Լ�������

    UINT32          ulRadioResetButtonCheckTime;    // ����̨�������������ťʱ�䣬��λ�룬0��ʾ�����
    UINT32          ulRadioResetAPPCheckTime;       // ����̨�������������ťʱ�䣬��λ�룬0��ʾ�����

    BOOL            bIPHVideoProxyMode;             // IPH��Ƶ�Ƿ�ͨ������̨����ȱʡFALSE��������
    UINT32          ulSendSelfCheckMsgPeriod;       // ����̨�������������Լ���Ϣ��ʱ��Σ�������ʱ��ν�ֹͣ���ͣ���0��ʾ�����ƣ���λ��

    UINT32          ulTXDataCaptureMode;             // ����̨ʵʱץ��ģʽ��0����ץ����1��ץ��

    UINT32          ulTXAutoLockTimeout;            // �ն��Զ�����ʱ��(��λ�룩
    CHAR            acTXLockPwd[32];                // ��������(���Ϊ�ձ�ʾ����Ҫ�������룬�����Զ������������͵��Ǿ���base64���ܵ��ı�

    // DCҵ������
    BOOL            bManageOneDepot;                // TRUE���γ�����ֻ̨�ܹ���һ���γ� FALSE���������жγ�
    BOOL            bAllowKickOffDC;                // ����DC�ظ���¼ʱ����Ͽ����ӻָ�ʱ�����Զ��߳��ȵ�¼��DC
    BOOL            bAllowDCReloadCfg;              // DC�Ƿ���������������¼�����������
    BOOL            bAllowHalfDuplexP2PCall;        // DC�Ƿ�֧�ְ�˫�����

    BOOL            bAllowVideoWall;                // DC�Ƿ�֧����ƵͶ��������
    BOOL            bAllowGISSub;                   // DC�Ƿ�֧��GIS����
    BOOL            bAllowP2PVideoCall;             // DC�Ƿ�֧����Ƶ����
    BOOL            bAutoCloseMulticastCall;        // ͨ������в��ù㲥ģʽ(�ͷŻ�Ȩ������ǿ��)

    BOOL            bSendSMSByRealMsg;              // ����ʵʱ��Ϣ���Ͷ��ţ�������ö����ݷ�ʽ��
    BOOL            bSimpleSMSMode;                 // ȥ�����ӹ�������Ӫ��Ϣ

    UINT32          ulDCMinTmpPatchGroupNum;        // ÿ������̨�Զ���������ʱ�ɽ������
    UINT32          ulDCUserIDForDepotIPH;          // ֧�ֳ������г�IPH���ܵĵ���̨����Ҫ���ڲ���

    BOOL            bIPHVideoTcpMode;               // IPH��Ƶ���ݴ���ģʽ��TRUE��TCP��FALSE��UDP
    UINT32          ulIPHVideoOpenTimeout;          // ����IPH��ƵʱЭ��Э��ʱ�䣨����ƵԴʱ�䣩
    BOOL            bIPHRecMergeMode;               // IPH¼���Ƿ�ϲ���TRUE���ϲ���FALSE�����ϲ�
    BOOL            bDirectSendIPHAudio;            // �Ƿ�ֱ�ӷ��ͶԽ�������IPH�豸
    UINT32          ulIPHAudioSampleRate;           // IPH��Ƶ����Ƶ�ʣ�ͨ��16000/8000
    UINT32          ulIPHAudioBitsPerSample;        // IPH��Ƶ����λ����ͨ��16��8

    BOOL            bDirectSendPAAudio;             // �Ƿ�ֱ�ӷ��ͶԽ�������PA�豸
    UINT32          ulPAAudioSampleRate;            // PA��Ƶ����Ƶ�ʣ�ͨ��16000/8000
    UINT32          ulPAAudioBitsPerSample;         // PA��Ƶ����λ����ͨ��16��8

    BOOL            bPAAECMode;                     // �㲥�������û�������˷磩���Ʒ�ʽ��ȱʡ��֧��
    UINT32          ulPAMaxPkgSize;                 // �㲥��������󳤶ȣ��ֽ�������ȱʡ1000
//    UINT32          ulPADelaySendTime;              // �㲥�����������ӳ�ʱ�䣬��λ���룬������500ms��0��ʾ���ӳ�

    BOOL            bIPHAECMode;                    // IPH�������û�������˷磩���Ʒ�ʽ��ȱʡ��֧��
    UINT32          ulIPHMaxPkgSize;                // IPH��������󳤶ȣ��ֽ�������ȱʡ1000
    UINT32          ulMaxIPHTalkTime;               // IPH������������ȴ�ʱ��
    UINT32          ulMAXIPHWaitAcceptTime;         // IPH�ȴ����������ʱ��

    // GPS
    UINT16          usGPSRecvPort;                  // GPS���ݽ����ֳ���������Ŀ��

    // ϵͳ����
    UINT32          ulClearDataHour;                // ��������ʱ��
    UINT32          ulResvDataDays;                 // ����(�⣩��������
    UINT32          ulBackupFileNum;                // ���ݱ����ļ�����

    INT32           iAutoCloseHour;                 // dis�Զ���������ʱ�̣�-1��ʾ���Զ�����

    // ��Ⱥ����
    UINT32          ulClusterDefaultStatus;         // ��������״̬
    UINT8           aucClusterPeerAddr[4];          // �Զ˵�ַ���ڲ�������ַ��
    UINT8           aucClusterMasterAddr[4];        // ����״̬�µĵ�ַ�������ַ��

    CHAR            acStatusFile[256];              // ����ؽ���״̬�ļ����ļ�����Ϊ�ձ�ʾ������������ʾ�쳣��
    UINT32          ulRefreshStatusFilePeriod;      // ����״̬����ļ����ڣ�ȷʡ15�룬��С5��

    // OCC��NOCC�ӿ�����
    UINT32          ulLineID;                       // ��·ID
    UINT16          usOCCPort;                      // OCC��NOCCͨ�Ŷ˿�(����UDP�����˿ڣ�
    UINT8           aucNOCCAddr[4];                 // NOCC DIS��ַ������ַΪ0ʱ��OCC����̨������NOCC
    UINT16          usNOCCPort;                     // NOCC DIS UDP�˿ڣ�������OCC��DIS��UDP�ӿ�ͨ��

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
 * @brief           ͨ�����κŻ�ȡ������Ϣ
 * @param           strTrainID    [in]  ���κ�
 * @param           stTrainInfo   [out] ������Ϣ
 * @return          BOOL    ��ȡ�ɹ���ʧ��
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
BOOL GetTrainInfoByTrainID(const std::string& strTrainID, TRAIN_INFO_T& stTrainInfo);

/**
 * @brief           ͨ������Ż�ȡ������Ϣ
 * @param           iTrainUnitID    [in]
 * @param           stTrainInfo     [out]
 * @return          BOOL    ��ȡ�ɹ���ʧ��
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

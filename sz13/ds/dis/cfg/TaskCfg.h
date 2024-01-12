#ifndef TASK_CFG_H
#define TASK_CFG_H

#include "DISPublic.h"
#include "CfgDao.h"


class TaskCfg: public GBaseTask
{
public :
    TaskCfg(UINT16 usDispatchID);

    BOOL Init();
    virtual VOID TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);

private:
    UINT32      m_ulLoadAPPCfg;
    UINT32      m_ulLoadGroupCfg;
    UINT32      m_ulLoadStationInfo;
    UINT32      m_ulLoadSectionInfo;
    UINT32      m_ulLoadDCUserInfo;
    UINT32      m_ulLoadDCOperatorInfo;
    UINT32      m_ulLoadStationUEInfo;
    UINT32      m_ulLoadPortalUEInfo;
    UINT32      m_ulLoadBrdZoneInfo;
    UINT32      m_ulLoadTrainInfo;
    UINT32      m_ulLoadTrainUEInfo;
    UINT32      m_ulLoadRecBrdCfg;
    UINT32      m_ulLoadDepotInfo;
    UINT32      m_ulLoadMultiGroupInfo;
    UINT32      m_ulLoadPredefinedSMS;
    UINT32      m_ulLoadSDSTemplate;

    UINT32      m_ulLoadGroupInfo;
    UINT32      m_ulLoadAutoJoinGroupInfo;
    UINT32      m_ulLoadSubGroupInfo;
    UINT32      m_ulLoadSubVideoGroupInfo;
    UINT32      m_ulLoadSubUserInfo;
    UINT32      m_ulLoadSubVideoUserInfo;
  //  UINT32      m_ulLoadStationPlatformInfo;
    UINT32      m_ulLoadDestInfo;
    //BOOL        m_bGetRealBrdSeqID;

    BOOL        m_bCfgDataChanged;

    UINT32      m_ulDCUserID;
    CHAR        m_acDCOperatorID[32];

    //UINT32      m_ulRealBrdSeqID;
    std::map<std::string, UINT32*>    m_mCfgDataVer;

    CfgDao      *m_pDao;

    virtual VOID OnServerDisconnectClient(UINT16 usClientID);

    BOOL AddOperLog(UINT32 ulLogTime, CHAR *szLog);
    BOOL AddOperLog(const CHAR *szOperLog, ...);

    VOID KickOffDC(PID_T *pstPID);

    VOID OnGetClientVer(VOID *pvMsg, UINT32 ulMsgLen);

    BOOL LoadStation(CHAR *szData, UINT32 ulMaxDataLen);

    VOID OnCheckTimer();
    VOID InitCfgDataVer();
    VOID InitCfgDataVerMap();
    VOID SetCfgDataVer(UINT32 &ulCfgVer);
    VOID SendGetCfgDataVerMsg(BOOL bSendAllDC, UINT32 ulSeqID);

    VOID OnSwitchToMaster();
    VOID CheckAutoClose();
    VOID CheckClusterStatus();

    VOID OnDCDISReq(CHAR *szReq, UINT32 ulMsgLen);

    VOID SendConfirmSaveRsp(UINT32 ulSeqID);

    BOOL SendLoadCfgDataRsp(const CHAR *szMsg, UINT32 ulSeqID, CHAR *szJsonMsg);

    VOID OnDCRegisterDISReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadAPPCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadGroupCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadDCUserInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadDCOperatorInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadTrainUEInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadStationUEInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadPortalUEInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadGroupInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadAutoJoinGroupInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadSubGroupInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadSubVideoGroupInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadSubUserInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadSubVideoUserInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadBrdZoneInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadTrainInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadStationInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadMultiGroupInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCLoadP2PCallInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadGroupCallInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadP2PVideoCallInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadReqCallInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadRecBrdInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadRealBrdInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadIPHTalkInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadOperLogInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCLoadPredefinedSMSReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadSDSTemplateReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCSaveGroupStatusReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSaveSubGroupInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSaveSubUserInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSaveSubVideoUserInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSaveSubVideoGroupInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);

    VOID OnDCSaveP2PCallInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSaveGroupCallInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSaveP2PVideoCallInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSaveRealBrdInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSaveGroupInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);

    VOID OnDCGetSDSInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCGetLatestSDSInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCAddSDSInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSetSDSInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCDelSDSInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCGetSDSReplyReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);

    VOID OnDCLoadRecBrdCfgReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadDepotInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);

    VOID OnDCSetTrainDepotIDReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSetUESDSReplyReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSetUESDSRecvReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSaveNewSDSReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSaveNewSMSReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSetOperatorReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSaveGISInfo(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSaveMultiGroupInfo(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);

    VOID OnDCGetRecFileListReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCGetRecFileListRsp(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);

    VOID OnDCCfgReq(CHAR *szReq, UINT32 ulMsgLen);

    VOID OnDCLogin(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLogout(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCOperatorLogin(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCOperatorLogout(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCGetCfgDataVer(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCSetPwdReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSetGroupCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCAddDCUserReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCDelDCUserReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSetDCUserReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCAddDCOperatorReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCDelDCOperatorReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSetDCOperatorReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCAddDepotCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCDelDepotCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSetDepotCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCAddStationReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCDelStationReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSetStationReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCAddDestInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCDelDestInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSetDestInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCAddStationUEReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCDelStationUEReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSetStationUEReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCAddPortalUEReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCDelPortalUEReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSetPortalUEReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCAddBrdZoneReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCDelBrdZoneReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSetBrdZoneReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCAddTrainReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCDelTrainReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSetTrainReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCAddTrainUEReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCDelTrainUEReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSetTrainUEReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCAddPredefinedSMSReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCDelPredefinedSMSReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSetPredefinedSMSReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCAddSDSTemplateReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCDelSDSTemplateReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSetSDSTemplateReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCAddRecBrdCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCDelRecBrdCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSetRecBrdCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCSaveOperLogReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCAddStaticGroupReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCDelStaticGroupReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCLoadDestInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCLoadSectionInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);

    BOOL SendDC(UINT32 ulDCUserID, UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &Param);

};

#endif

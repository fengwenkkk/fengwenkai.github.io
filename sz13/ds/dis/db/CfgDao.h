#ifndef CFG_DAO_H
#define CFG_DAO_H

#include "DISPublic.h"

#define SYNC_ADD        1
#define SYNC_DEL        (-1)
#define SYNC_NULL       0

#define SYNC_UPDATE     SYNC_ADD

#define DaoLog(ulLogLevel, szFormat, ...)      { GosLog("DAO", ulLogLevel, szFormat, ##__VA_ARGS__); }

class CfgDao
{
public :
    CfgDao();
    ~CfgDao();

    BOOL TransBegin();
    BOOL TransRollback();
    BOOL TransCommit();

    BOOL Init(PDBEnvBase *pPDBEnv);

    BOOL CheckConn();

    BOOL Update(const CHAR *szSQL);

    BOOL GetGroupCfg(GROUP_CFG_T &stCfg, INT32 &iSyncFlag);
    BOOL SetGroupCfg(GROUP_CFG_T &stCfg, INT32 iSyncFlag);
    BOOL SyncOverGroupCfg();

    BOOL GetAppCfg(APP_BASIC_CFG_T *pstCfg);
    BOOL SaveAppCfg(APP_BASIC_CFG_T *pstCfg);

    BOOL GetDCDevInfo(VECTOR<DC_DEV_INFO_T> &vInfo);

    BOOL GetDCUser(VECTOR<DC_USER_INFO_T> &vInfo);
    BOOL AddDCUser(DC_USER_INFO_T &stRec);
    BOOL SetDCUser(DC_USER_INFO_T &stRec, INT32 iSyncFlag);
    BOOL DelDCUser(UINT32 ulUserID);
    BOOL GetSyncingDCUser(UINT32 ulMaxRecNum, VECTOR<DC_USER_INFO_T> &vAddRec, VECTOR<DC_USER_INFO_T> &vDelRec);
    BOOL SyncOverDCUser(DC_USER_INFO_T &stRec, INT32 iSyncFlag);

    BOOL GetDCOperator(VECTOR<DC_OPERATOR_INFO_T> &vInfo);
    BOOL AddDCOperator(DC_OPERATOR_INFO_T &stInfo);
    BOOL DelDCOperator(CHAR *szOperatorID);
    BOOL SetDCOperator(DC_OPERATOR_INFO_T &stInfo);
    BOOL SetDCOperator(DC_OPERATOR_INFO_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingDCOperator(UINT32 ulMaxRecNum, VECTOR<DC_OPERATOR_INFO_T> &vAddRec, VECTOR<DC_OPERATOR_INFO_T> &vDelRec);
    BOOL SyncOverDCOperator(DC_OPERATOR_INFO_T &stRec, INT32 iSyncFlag);
    BOOL SetDCOperatorPwd(CHAR *szUser, CHAR *szOldPwd, CHAR *szNewPwd);

    BOOL GetPredefinedSMS(VECTOR<PREDEFINED_SMS_INFO_T> &vSMS);
    BOOL AddPredefinedSMS(PREDEFINED_SMS_INFO_T &stInfo);
    BOOL DelPredefinedSMS(UINT32 ulSMSID);
    BOOL SetPredefinedSMS(PREDEFINED_SMS_INFO_T &stInfo);
    BOOL SetPredefinedSMS(PREDEFINED_SMS_INFO_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingPredefinedSMS(UINT32 ulMaxRecNum, VECTOR<PREDEFINED_SMS_INFO_T> &vAddRec, VECTOR<PREDEFINED_SMS_INFO_T> &vDelRec);
    BOOL SyncOverPredefinedSMS(PREDEFINED_SMS_INFO_T &stRec, INT32 iSyncFlag);

    BOOL GetSDSTemplate(VECTOR<SDS_TEMPLATE_T> &vInfo);
    BOOL GetMaxSDSTemplateID(UINT32 *pulID);
    BOOL AddSDSTemplate(SDS_TEMPLATE_T &pstInfo);
    BOOL DelSDSTemplate(UINT32 ulSMSID);
    BOOL SetSDSTemplate(SDS_TEMPLATE_T &stInfo);
    BOOL SetSDSTemplate(SDS_TEMPLATE_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingSDSTemplate(UINT32 ulMaxRecNum, VECTOR<SDS_TEMPLATE_T> &vAddRec, VECTOR<SDS_TEMPLATE_T> &vDelRec);
    BOOL SyncOverSDSTemplate(SDS_TEMPLATE_T &stRec, INT32 iSyncFlag);

    BOOL GetStationUE(VECTOR<STATION_UE_INFO_T> &vInfo);
    BOOL AddStationUE(STATION_UE_INFO_T &stInfo);
    BOOL DelStationUE(UINT32 ulUserID);
    BOOL SetStationUE(STATION_UE_INFO_T &stInfo);
    BOOL SetStationUE(STATION_UE_INFO_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingStationUE(UINT32 ulMaxRecNum, VECTOR<STATION_UE_INFO_T> &vAddRec, VECTOR<STATION_UE_INFO_T> &vDelRec);
    BOOL SyncOverStationUE(STATION_UE_INFO_T &stRec, INT32 iSyncFlag);

    BOOL GetTrainUE(VECTOR<TRAIN_UE_INFO_T> &vInfo);
    BOOL AddTrainUE(TRAIN_UE_INFO_T &stInfo);
    BOOL DelTrainUE(UINT32 ulUserID);
    BOOL SetTrainUE(TRAIN_UE_INFO_T &stInfo);
    BOOL SetTrainUE(TRAIN_UE_INFO_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingTrainUE(UINT32 ulMaxRecNum, VECTOR<TRAIN_UE_INFO_T> &vAddRec, VECTOR<TRAIN_UE_INFO_T> &vDelRec);
    BOOL SyncOverTrainUE(TRAIN_UE_INFO_T &stRec, INT32 iSyncFlag);

    BOOL GetPortalUE(VECTOR<PORTAL_UE_INFO_T> &vInfo);
    BOOL AddPortalUE(PORTAL_UE_INFO_T &stInfo);
    BOOL DelPortalUE(UINT32 ulUserID);
    BOOL SetPortalUE(PORTAL_UE_INFO_T &stInfo);
    BOOL SetPortalUE(PORTAL_UE_INFO_T &stRec, INT32 iSyncFlag);
    BOOL SetPortalUEGISInfo(UINT32 ulUserID, DOUBLE dLon, DOUBLE dLat);
    BOOL GetSyncingPortalUE(UINT32 ulMaxRecNum, VECTOR<PORTAL_UE_INFO_T> &vAddRec, VECTOR<PORTAL_UE_INFO_T> &vDelRec);
    BOOL SyncOverPortalUE(PORTAL_UE_INFO_T &stRec, INT32 iSyncFlag);

    BOOL GetAutoJoinGroup(UINT32 ulDCUserID, VECTOR<INT64> &vGroupID);
    BOOL IsAutoJoinGroupExist(UINT32 ulDCUserID, INT64 i64GroupID);
    BOOL SaveGroupStatus(UINT32 ulDCUserID, INT64 i64GroupID, UINT32 ulGroupStatus);
    BOOL SetAutoJoinGroup(SUB_GROUP_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingAutoJoinGroup(UINT32 ulMaxRecNum, VECTOR<SUB_GROUP_T> &vAddRec, VECTOR<SUB_GROUP_T> &vDelRec);
    BOOL SyncOverAutoJoinGroup(SUB_GROUP_T &stRec, INT32 iSyncFlag);

    BOOL GetGroupInfo(VECTOR<GROUP_INFO_T> &vGroup);
    BOOL SaveGroupInfo(GROUP_INFO_T *pstInfo);
    BOOL SetGroupInfo(GROUP_INFO_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingGroupInfo(UINT32 ulMaxRecNum, VECTOR<GROUP_INFO_T> &vAddRec, VECTOR<GROUP_INFO_T> &vDelRec);
    BOOL SyncOverGroupInfo(GROUP_INFO_T &stRec, INT32 iSyncFlag);

    BOOL GetSubGroup(UINT32 ulDCUserID, VECTOR<INT64> &vGroupID);
    BOOL SaveSubGroup(UINT32 ulDCUserID, VECTOR<INT64> &vDelGroup, VECTOR<INT64> &vAddGroup);
    BOOL SetSubGroup(SUB_GROUP_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingSubGroup(UINT32 ulMaxRecNum, VECTOR<SUB_GROUP_T> &vAddRec, VECTOR<SUB_GROUP_T> &vDelRec);
    BOOL SyncOverSubGroup(SUB_GROUP_T &stRec, INT32 iSyncFlag);

    BOOL GetSubVideoGroup(UINT32 ulDCUserID, VECTOR<INT64> &vGroupID);
    BOOL SaveSubVideoGroup(UINT32 ulDCUserID, VECTOR<INT64> &vDelGroup, VECTOR<INT64> &vAddGroup);
    BOOL SetSubVideoGroup(SUB_GROUP_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingSubVideoGroup(UINT32 ulMaxRecNum, VECTOR<SUB_GROUP_T> &vAddRec, VECTOR<SUB_GROUP_T> &vDelRec);
    BOOL SyncOverSubVideoGroup(SUB_GROUP_T &stRec, INT32 iSyncFlag);

    BOOL GetSubUser(UINT32 ulDCUserID, VECTOR<UINT32> &vUserID);
    BOOL SaveSubUser(UINT32 ulDCUserID, VECTOR<UINT32> &vDelUser, VECTOR<UINT32> &vAddUser);
    BOOL SetSubUser(SUB_USER_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingSubUser(UINT32 ulMaxRecNum, VECTOR<SUB_USER_T> &vAddRec, VECTOR<SUB_USER_T> &vDelRec);
    BOOL SyncOverSubUser(SUB_USER_T &stRec, INT32 iSyncFlag);

    BOOL GetSubVideoUser(UINT32 ulDCUserID, VECTOR<UINT32> &vUserID);
    BOOL SaveSubVideoUser(UINT32 ulDCUserID, VECTOR<UINT32> &vDelUser, VECTOR<UINT32> &vAddUser);
    BOOL SetSubVideoUser(SUB_USER_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingSubVideoUser(UINT32 ulMaxRecNum, VECTOR<SUB_USER_T> &vAddRec, VECTOR<SUB_USER_T> &vDelRec);
    BOOL SyncOverSubVideoUser(SUB_USER_T &stRec, INT32 iSyncFlag);

    BOOL GetBrdZoneInfo(UINT32 ulStationID, VECTOR<BRD_ZONE_INFO_T> &vBrdZoneInfo);
    BOOL GetBrdZoneInfo(VECTOR<BRD_ZONE_INFO_T> &vBrdZoneInfo);
    BOOL AddBrdZoneInfo(BRD_ZONE_INFO_T &stInfo);
    BOOL DelBrdZoneInfo(UINT32 ulStationID, UINT32 ulZoneID);
    BOOL SetBrdZoneInfo(BRD_ZONE_INFO_T &stInfo);
    BOOL SetBrdZoneInfo(BRD_ZONE_INFO_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingBrdZoneInfo(UINT32 ulMaxRecNum, VECTOR<BRD_ZONE_INFO_T> &vAddRec, VECTOR<BRD_ZONE_INFO_T> &vDelRec);
    BOOL SyncOverBrdZoneInfo(BRD_ZONE_INFO_T &stRec, INT32 iSyncFlag);

    BOOL GetDepotCfg(VECTOR<DEPOT_INFO_T> &vDepot);
    BOOL AddDepotCfg(DEPOT_INFO_T &stInfo);
    BOOL DelDepotCfg(UINT32 ulDepotID);
    BOOL SetDepotCfg(DEPOT_INFO_T &stInfo);
    BOOL SetDepotCfg(DEPOT_INFO_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingDepotCfg(UINT32 ulMaxRecNum, VECTOR<DEPOT_INFO_T> &vAddRec, VECTOR<DEPOT_INFO_T> &vDelRec);
    BOOL SyncOverDepotCfg(DEPOT_INFO_T &stRec, INT32 iSyncFlag);

    BOOL GetStation(VECTOR<STATION_INFO_T> &vStation);
    BOOL AddStation(STATION_INFO_T &stInfo);
    BOOL DelStation(UINT32 ulStationID);
    BOOL SetStation(STATION_INFO_T &stInfo, INT32 iSyncFlag);
    BOOL GetSyncingStation(UINT32 ulMaxRecNum, VECTOR<STATION_INFO_T> &vAddRec, VECTOR<STATION_INFO_T> &vDelRec);
    BOOL SyncOverStation(STATION_INFO_T &stRec, INT32 iSyncFlag);

    BOOL GetSection(VECTOR<SECTION_INFO_T> &vInfo);
    BOOL AddSection(SECTION_INFO_T &stInfo);
    BOOL DelSection(UINT32 ulSectionID);
    BOOL SetSection(SECTION_INFO_T &stInfo, INT32 iSyncFlag);
    BOOL GetSyncingSection(UINT32 ulMaxRecNum, VECTOR<SECTION_INFO_T> &vAddRec, VECTOR<SECTION_INFO_T> &vDelRec);
    BOOL SyncOverSection(SECTION_INFO_T &stRec, INT32 iSyncFlag);

    BOOL GetDestInfo(VECTOR<DEST_INFO_T> &vStInfo);
    BOOL AddDestInfo(DEST_INFO_T &stInfo);
    BOOL DelDestInfo(UINT32 ulDestID);
    BOOL SetDestInfo(DEST_INFO_T &stInfo, INT32 iSyncFlag);
    BOOL GetSyncingDestInfo(UINT32 ulMaxRecNum, VECTOR<DEST_INFO_T> &vAddRec, VECTOR<DEST_INFO_T> &vDelRec);
    BOOL SyncOverDestInfo(DEST_INFO_T &stRec, INT32 iSyncFlag);

    BOOL GetTrainInfo(VECTOR<TRAIN_T> &vTrain);
    BOOL AddTrainInfo(TRAIN_T &stInfo);
    BOOL SetTrainInfo(TRAIN_T &stInfo);
    BOOL DelTrainInfo(UINT32 ulTrainUnitID);
    BOOL SetTrainInfo(TRAIN_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingTrainInfo(UINT32 ulMaxRecNum, VECTOR<TRAIN_T> &vAddRec, VECTOR<TRAIN_T> &vDelRec);
    BOOL SyncOverTrainInfo(TRAIN_T &stRec, INT32 iSyncFlag);

    BOOL GetP2PCallInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulCallType, UINT32 ulUserID, UINT32 ulMaxNum, GJsonTupleParam &JsonTupleParam);
    BOOL SaveP2PCallInfo(P2P_CALL_INFO_T &stInfo);
    BOOL SetP2PCallInfo(P2P_CALL_INFO_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingP2PCallInfo(UINT32 ulMaxRecNum, VECTOR<P2P_CALL_INFO_T> &vAddRec, VECTOR<P2P_CALL_INFO_T> &vDelRec);
    BOOL SyncOverP2PCallInfo(P2P_CALL_INFO_T &stRec, INT32 iSyncFlag);

    BOOL GetGroupCallInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulCallType, UINT32 ulDCUserID, INT64 i64GroupID, UINT32 ulMaxNum, GJsonTupleParam &JsonTupleParam);
    BOOL SaveGroupCallInfo(GROUP_CALL_INFO_T &stInfo);
    BOOL SetGroupCallInfo(GROUP_CALL_INFO_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingGroupCallInfo(UINT32 ulMaxRecNum, VECTOR<GROUP_CALL_INFO_T> &vAddRec, VECTOR<GROUP_CALL_INFO_T> &vDelRec);
    BOOL SyncOverGroupCallInfo(GROUP_CALL_INFO_T &stRec, INT32 iSyncFlag);

    BOOL GetMaxSDSID(UINT32 *pulMaxSDSID);
    BOOL GetSDSIndex(SDS_INDEX_T &stInfo);
    BOOL SetSDSIndex(SDS_INDEX_T &stRec);
    BOOL GetSyncingSDSIndex(SDS_INDEX_T &stInfo);

    BOOL GetSDSIDBySDSIndex(UINT32 ulSDSIndex, UINT32 *pulSDSID);
    BOOL UpdateSDSIndex(UINT32 ulSDSDay, UINT32 ulFormalDispatchSDSIndex, UINT32 ulOralDispatchSDSIndex, UINT32 ulOperateSDSIndex);

    BOOL GetSDSInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulSDSType,
                    CHAR *szSendUserName, CHAR *szKeyword, UINT32 ulMaxNum, VECTOR<SDS_INFO_T> &vInfo);
    BOOL GetLatestSDSInfo(UINT32 ulMaxNum, UINT32 ulDCUserID, VECTOR<SDS_INFO_T> &vInfo);
    BOOL AddSDSInfo(SDS_INFO_T &stInfo);
    BOOL UpdateSDSRecv(UINT32 ulSDSID, UINT32 ulUserID, UINT32 ulSendResult, UINT32 ulTime);
    BOOL SetSDSInfo(UINT32 ulSDSID, BOOL bSendFlag);
    BOOL DelSDSInfo(UINT32 ulSDSID);
    BOOL SetSDSInfo(SDS_INFO_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingSDSInfo(UINT32 ulMaxRecNum, VECTOR<SDS_INFO_T> &vAddRec, VECTOR<SDS_INFO_T> &vDelRec);
    BOOL SyncOverSDSInfo(SDS_INFO_T &stRec, INT32 iSyncFlag);

    BOOL GetSDSReply(UINT32 ulSDSID, VECTOR<SDS_REPLY_T> &vInfo);
    BOOL AddSDSReply(UINT32 ulSDSID, VECTOR<UINT32> &vUserID);
    BOOL SetSDSReply(SDS_REPLY_T &stRec, BOOL bSendFlag);
    BOOL UpdateSDSReply(UINT32 ulSDSID, UINT32 ulUserID, UINT32 ulTime);
    BOOL GetSyncingSDSReply(UINT32 ulMaxRecNum, VECTOR<SDS_REPLY_T> &vAddRec, VECTOR<SDS_REPLY_T> &vDelRec);
    BOOL SyncOverSDSReply(SDS_REPLY_T &stRec, INT32 iSyncFlag);

    BOOL GetRecBrdCfg(VECTOR<REC_BRD_CFG_T> &vInfo);
    //BOOL SetRecBrdCfg(VECTOR<REC_BRD_CFG_T> &vCfg);
    BOOL AddRecBrdCfg(REC_BRD_CFG_T &pstInfo);
    BOOL DelRecBrdCfg(UINT32 ulSMSID);
    BOOL SetRecBrdCfg(REC_BRD_CFG_T &stInfo);
    BOOL SetRecBrdCfg(REC_BRD_CFG_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingRecBrdCfg(UINT32 ulMaxRecNum, VECTOR<REC_BRD_CFG_T> &vAddRec, VECTOR<REC_BRD_CFG_T> &vDelRec);
    BOOL SyncOverRecBrdCfg(REC_BRD_CFG_T &stRec, INT32 iSyncFlag);

    BOOL GetMaxRecBrdSeqID(UINT32 *pulMaxSeqID);
    BOOL GetRecBrdInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulMaxNum, GJsonTupleParam &JsonTupleParam);

    BOOL AddRecBrdInfo(REC_BRD_INFO_T *pstInfo, VECTOR<UINT32> &vTrainUnitID);
    BOOL SetRecBrdInfo(REC_BRD_INFO_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingRecBrdInfo(UINT32 ulMaxRecNum, VECTOR<REC_BRD_INFO_T> &vAddRec, VECTOR<REC_BRD_INFO_T> &vDelRec);
    BOOL SyncOverRecBrdInfo(REC_BRD_INFO_T &stRec, INT32 iSyncFlag);

    BOOL UpdateRecBrdReply(REC_BRD_REPLY_T &stRec);
    BOOL SetRecBrdReply(REC_BRD_REPLY_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingRecBrdReply(UINT32 ulMaxRecNum, VECTOR<REC_BRD_REPLY_T> &vAddRec, VECTOR<REC_BRD_REPLY_T> &vDelRec);
    BOOL SyncOverRecBrdReply(REC_BRD_REPLY_T &stRec, INT32 iSyncFlag);

    BOOL GetMaxRealBrdSeqID(UINT32 *pulMaxSeqID);
    BOOL GetRealBrdInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulMaxNum, GJsonTupleParam &JsonTupleParam);
    BOOL SaveRealBrdInfo(REAL_BRD_INFO_T &stInfo);
    BOOL SetRealBrdInfo(REAL_BRD_INFO_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingRealBrdInfo(UINT32 ulMaxRecNum, VECTOR<REAL_BRD_INFO_T> &vAddRec, VECTOR<REAL_BRD_INFO_T> &vDelRec);
    BOOL SyncOverRealBrdInfo(REAL_BRD_INFO_T &stRec, INT32 iSyncFlag);

    BOOL GetOperLogInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulDCUserID, UINT32 ulMaxNum, GJsonTupleParam &JsonTupleParam);
    BOOL AddOperLog(UINT32 ulTime, UINT32 ulDCUserID, CHAR *szOperatorID, CHAR *szLogInfo);
    BOOL GetSyncingOperLog(UINT32 ulMaxRecNum, VECTOR<OPER_LOG_INFO_T> &vAddRec, VECTOR<OPER_LOG_INFO_T> &vDelRec);
    BOOL SetOperLog(OPER_LOG_INFO_T &stRec, INT32 iSyncFlag);
    BOOL SyncOverOperLog(OPER_LOG_INFO_T &stRec, INT32 iSyncFlag);

    BOOL NewIPHTalk(VECTOR<IPH_TALK_INFO_T> &vInfo);
    BOOL UpdateIPHTalk(IPH_TALK_INFO_T *pstInfo);
    BOOL GetIPHTalkInfo(UINT32 ulStartTime, UINT32 ulEndTime, GJsonTupleParam &JsonTupleParam);
    BOOL SetIPHTalkInfo(IPH_TALK_INFO_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingIPHTalkInfo(UINT32 ulMaxRecNum, VECTOR<IPH_TALK_INFO_T> &vAddRec, VECTOR<IPH_TALK_INFO_T> &vDelRec);
    BOOL SyncOverIPHTalkInfo(IPH_TALK_INFO_T &stRec, INT32 iSyncFlag);

    BOOL GetReqCallInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulReqCallType, UINT32 ulMaxNum, GJsonTupleParam &JsonTupleParam);
    BOOL AddReqCallInfo(REQ_CALL_INFO_T &stInfo);
    BOOL UpdateReqCallInfo(UINT32 ulReqTime, UINT32 ulUserID, UINT32 ulDCUserID);
    BOOL SetReqCallInfo(REQ_CALL_INFO_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingReqCallInfo(UINT32 ulMaxRecNum, VECTOR<REQ_CALL_INFO_T> &vAddRec, VECTOR<REQ_CALL_INFO_T> &vDelRec);
    BOOL SyncOverReqCallInfo(REQ_CALL_INFO_T &stRec, INT32 iSyncFlag);

    BOOL AddCellInfo(VECTOR<CELL_INFO_T> &vCellInfo);
    BOOL GetCellInfo(UINT32 ulStartTime, UINT32 ulMaxNum, VECTOR<CELL_INFO_T> &vInfo);
    BOOL SetCellInfo(CELL_INFO_T &stRec, INT32 iSyncFlag);
    BOOL GetSyncingCellInfo(UINT32 ulMaxRecNum, VECTOR<CELL_INFO_T> &vAddRec, VECTOR<CELL_INFO_T> &vDelRec);
    BOOL SyncOverCellInfo(CELL_INFO_T &stRec, INT32 iSyncFlag);

    BOOL SetTrainID(UINT32 ulTrainUnitID, CHAR *szTrainID, CHAR *szDestID, UINT32 ulDirection);

    BOOL GetMultiGroupInfo(VECTOR<MULTI_GROUP_INFO_T> &vRec);
    BOOL SetMultiGroupInfo(UINT32 ulDCUserID, VECTOR<MULTI_GROUP_INFO_T> &vInfo);
    BOOL SetMultiGroupInfo(MULTI_GROUP_INFO_T &vRec, INT32 iSyncFlag);
    BOOL GetSyncingMultiGroupInfo(UINT32 ulMaxRecNum, VECTOR<MULTI_GROUP_INFO_T> &vAddRec, VECTOR<MULTI_GROUP_INFO_T> &vDelRec);
    BOOL SyncOverMultiGroupInfo(MULTI_GROUP_INFO_T &stRec, INT32 iSyncFlag);

    INT32 GetRecNum(CHAR *szTable);
    BOOL ClearTable(CHAR *szTable, CHAR *szFieldName, UINT32 ulMaxResvNum);

    BOOL ClearCellInfo(UINT32 ulDeadTime);
    BOOL ClearGroupCallInfo(UINT32 ulDeadTime);
    BOOL ClearIPHTalkInfo(UINT32 ulDeadTime);
    BOOL ClearOperLog(UINT32 ulDeadTime);
    BOOL ClearP2PCallInfo(UINT32 ulDeadTime);
    BOOL ClearRealBrdInfo(UINT32 ulDeadTime);
    BOOL ClearRecBrdInfo(UINT32 ulDeadTime);
    BOOL ClearRecBrdReply(UINT32 ulDeadTime);
    BOOL ClearReqCallInfo(UINT32 ulDeadTime);
    BOOL ClearSDSInfo(UINT32 ulDeadTime);
    BOOL ClearSDSReply(UINT32 ulDeadTime);

private:
    PDBApp  *m_pPDBApp;

    BOOL   IsRecordExist(const CHAR *szTable, const CHAR *szKey, UINT32 ulKey);
};

extern CfgDao* GetCfgDao();
extern CfgDao* GetCfgDao(PDBEnvBase *pPDBEnv);

BOOL ConvertStationList(CHAR *szStationList, UINT32 *pulStationList, UINT32 ulMaxStationNum, UINT32 &ulStationNum);
VOID ConvertStationList(UINT32 *pulStationList, UINT32 ulStationNum, CHAR *szStationList);

#endif

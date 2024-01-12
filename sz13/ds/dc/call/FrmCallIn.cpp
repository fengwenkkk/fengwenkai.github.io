//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "ClientData.h"
#include "UICfg.h"
#include "AudioDev.h"
#include "FrmCallIn.h"
#include "FrmGroup.h"
#include "DlgNewSDS.h"
#include "main.h"
#include "Group.h"
#include "DynamicGroup.h"
#include "AudioDev.h"
#include "FrmVideoCall.h"
#include "FrmGroupCallWin.h"
#include "FrmP2PCallWin.h"
#include "FrmHalfDuplexP2PCallWin.h"
#include "DlgSetTrainGroup.h"
#include "DlgSetTrainID.h"
#include "FrmIPH.h"
#include "GPublic.h"
#include "FrmGroup.h"
#include "FrmUser.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxCustomData"
#pragma link "cxDropDownEdit"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
#pragma link "cxInplaceContainer"
#pragma link "cxLabel"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxMemo"
#pragma link "cxStyles"
#pragma link "cxTextEdit"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "dxSkinBlue"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "MDIForm"
#pragma link "dxBar"
#pragma link "cxProgressBar"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxClasses"
#pragma link "cxImageComboBox"
#pragma link "cxImageList"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxCheckBox"
#pragma link "AdvSplitter"
#pragma link "cxScrollBox"
#pragma link "dxBevel"
#pragma link "cxSplitter"
#pragma link "AdvSplitter"
#pragma link "dxBevel"
#pragma link "cxScrollBox"
#pragma link "WMPLib_OCX"
#pragma resource "*.dfm"
TFormCallIn *FormCallIn;

#define VOICE_ALARM_EMERGENCY_CALL      1
#define VOICE_ALARM_IPH                 2
#define VOICE_ALARM_EMERGENCY_REQ_CALL  3
#define VOICE_ALARM_TRAIN_REQ           4
#define VOICE_ALARM_REQ_CALL            5
#define VOICE_ALARM_SDS                 6
#define VOICE_ALARM_NULL                0xffffffff

static TColor  g_CallColorRed = clMaroon;
static TColor  g_CallColorGray = clGray;
static TColor  g_CallColorOrange = (TColor)0x80D0;
static TColor  g_CallColorGreen = (TColor)RGB(0, 128, 0);

static VectorMap<UINT32, SET_TRAIN_POS_T>  g_mSetTrainPos;

static bool     g_bAutoHideClosed = true;
static bool     g_bPlaying = false;

static BOOL     g_bAllowAlarm = TRUE;

static UINT32   g_ulVoiceAlarmTimeout = 60;
static UINT32   g_ulVoiceAlarmTime = 0;
static UINT32   g_ulVoiceAlarm = VOICE_ALARM_NULL;
static UINT32   g_ulCurrVoiceAlarm = VOICE_ALARM_NULL;

static INT64    g_i64MainListenGroupID = INVALID_GROUP_ID;

static bool     g_bAutoAcceptP2PEmergencyCall = true;

extern void ShowHalfDuplexP2PCallWin(UINT32 ulUserID);

static AnsiString GetCallStateText(UINT32 ulState)
{
    if (ulState == CALL_STATE_IDLE)             return "空闲";
    if (ulState == CALL_STATE_WAIT)             return "等待";
    if (ulState == CALL_STATE_ACCEPT)           return "通话中";
    if (ulState == CALL_STATE_CLOSED)           return "结束";
    if (ulState == CALL_STATE_NOT_CONFIRMED)    return "未确认";
    if (ulState == CALL_STATE_CONFIRMED)        return "已确认";

    return "";
}

static void StopMainListen()
{
    if (g_i64MainListenGroupID == INVALID_GROUP_ID ||
        g_i64MainListenGroupID == Group::GetMainGroupID() )
    {
        return;
    }

    AssignGroupSpeaker(g_i64MainListenGroupID, GetESDKMonSpeaker());
    g_i64MainListenGroupID = INVALID_GROUP_ID;

    if (FormCallIn)
    {
        FormCallIn->tlCallInfo->BeginUpdate();
        FormCallIn->tlCallInfo->EndUpdate();
    }
}

static void StartMainListen(INT64 i64GroupID)
{
    if (Group::GetMainGroupID() != INVALID_GROUP_ID ||
        i64GroupID == INVALID_GROUP_ID)
    {
        return;
    }

    if (g_i64MainListenGroupID != i64GroupID &&
        g_i64MainListenGroupID != INVALID_GROUP_ID)
    {
        AssignGroupSpeaker(g_i64MainListenGroupID, GetESDKMonSpeaker());
        g_i64MainListenGroupID = INVALID_GROUP_ID;
    }

    if (AssignGroupSpeaker(i64GroupID, GetESDKMainSpeaker()))
    {
        g_i64MainListenGroupID = i64GroupID;
    }

    if (FormCallIn)
    {
        FormCallIn->tlCallInfo->BeginUpdate();
        FormCallIn->tlCallInfo->EndUpdate();
    }
}

// 组呼结束后触发
void CheckMainListenOnPTTDial()
{
/*  if (g_i64MainGroupID == INVALID_GROUP_ID)
    {
        if (g_i64MainListenGroupID != INVALID_GROUP_ID)
        {
            if (!AssignGroupSpeaker(g_i64MainListenGroupID, GetESDKMainSpeaker()))
            {
                g_i64MainListenGroupID = INVALID_GROUP_ID;
            }
        }
    }
    else   */
    {
        if (Group::GetMainGroupID() != g_i64MainListenGroupID &&
            g_i64MainListenGroupID != INVALID_GROUP_ID)
        {
            AssignGroupSpeaker(g_i64MainListenGroupID, GetESDKMonSpeaker());
        }

        AssignGroupSpeaker(Group::GetMainGroupID(), GetESDKMainSpeaker());
    }

    if (FormCallIn)
    {
        FormCallIn->tlCallInfo->BeginUpdate();
        FormCallIn->tlCallInfo->EndUpdate();
    }
}

// 组呼结束后触发
void CheckMainListenOnGroupCallOver()
{
    if (Group::GetMainGroupID() == g_i64MainListenGroupID)
    {
        return;
    }

    if (g_i64MainListenGroupID != INVALID_GROUP_ID &&
        !AssignGroupSpeaker(g_i64MainListenGroupID, GetESDKMainSpeaker()))
    {
        g_i64MainListenGroupID = INVALID_GROUP_ID;
    }

    AssignGroupSpeaker(Group::GetMainGroupID(), GetESDKMonSpeaker());

    if (FormCallIn)
    {
        FormCallIn->tlCallInfo->BeginUpdate();
        FormCallIn->tlCallInfo->EndUpdate();
    }
}

// TODO 是否没有要求只能有一个请呼被处理？
static INT64 InitReqCallGroup(CHAR *szGroupPrefix, VECTOR<UINT32> &vUserID)
{
    INT64           i64GroupID = DynamicGroup::GetReqCallGroupID();
    VECTOR<UINT32>  vCurrUser;
    VECTOR<INT64>   vCurrGroup;

    if (i64GroupID == INVALID_GROUP_ID ||
        !DynamicGroup::GetDynamicGroup(i64GroupID))
    {
        i64GroupID = DynamicGroup::CreateDynamicGroup(szGroupPrefix, vUserID);
        //if (!CreateReqCallGroup(i64GroupID, vUserID))
        if (i64GroupID == INVALID_GROUP_ID)
        {
            return INVALID_GROUP_ID;
        }
        else
        {
            DynamicGroup::SetReqCallGroupID(i64GroupID);
            return i64GroupID;
        }
    }

    if (!eSDK_GetGroupMember(i64GroupID, vCurrUser, vCurrGroup))
    {
        GosLog(LOG_ERROR, "get group member of " FORMAT_I64 " failed", i64GroupID);
        return INVALID_GROUP_ID;
    }

    Group::SubJoinGroup(i64GroupID);

#if 1
    RemoveCommonMember(vCurrUser, vUserID);
    RemoveMember(vCurrUser, APP::GetCurrDCUserID());
    if (vCurrUser.size() == 0 && vUserID.size() == 0)
    {
        return i64GroupID;
    }

    if (!eSDK_SetDynamicGroupUser(i64GroupID, vUserID, vCurrUser))
    {
        GosLog(LOG_ERROR, "set group member of " FORMAT_I64 " failed", i64GroupID);
        return INVALID_GROUP_ID;
    }
#else
    //  RemoveCommonMember(vCurrUser, vUserID);

    VECTOR<UINT32> vEmpty;

    RemoveMember(vCurrUser, APP::GetCurrDCUserID());
    for (UINT32 i = 0; i < vCurrUser.size(); i++)
    {
        GosLog(LOG_ERROR, "remove group user %u", vCurrUser[i]);
    }

    for (UINT32 i = 0; i < vUserID.size(); i++)
    {
        GosLog(LOG_ERROR, "add group user %u", vUserID[i]);
    }

    if (!eSDK_SetDynamicGroupUser(i64GroupID, vCurrUser, vEmpty))  // vCurrUser))
    {
        GosLog(LOG_ERROR, "set group member of " FORMAT_I64 " failed", i64GroupID);
        return INVALID_GROUP_ID;
    }

    if (!eSDK_SetDynamicGroupUser(i64GroupID, vEmpty, vUserID))
    {
        GosLog(LOG_ERROR, "set group member of " FORMAT_I64 " failed", i64GroupID);
        return INVALID_GROUP_ID;
    }
#endif

    return i64GroupID;
}

/**
 * @brief           返回业务请求排序优先级请求
 * @param           ulCallType       [in]
 * @param           ulReqCallType    [in]
 * @return          UINT32  优先级的数值
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
UINT32 GetSortOrder(UINT32 ulCallType, UINT32 ulReqCallType)
{
    if (ulCallType == CALL_TYPE_EMERGENCY_CALL)         return 0;
    if (ulCallType == CALL_TYPE_IPH)                    return 1;
    if (ulCallType == TRAIN_REGISTER)                   return 10;
    if (ulCallType == TRAIN_SET_TRAINID)                return 11;
    if (ulCallType == CALL_TYPE_P2P_EMERGENCY_CALL)     return 11;
    if (ulCallType == CALL_TYPE_HALF_DUPLEX_P2P_CALL)   return 40;
    if (ulCallType == CALL_TYPE_P2P)                    return 40;
    if (ulCallType == CALL_TYPE_P2P_TRANSFER)           return 40;
    if (ulCallType == CALL_TYPE_LRBUS_REQ_CALL)         return 12;
    if (ulCallType == CALL_TYPE_VIDEO_RECV)             return 30;
    if (ulCallType == CALL_TYPE_VIDEO_PTT)              return 30;

    if (ulCallType == CALL_TYPE_REQ_CALL)
    {
        if (ulReqCallType == TRAIN_EMERGENCY_REQ_CALL_DC)   return 10;
        if (ulReqCallType == TRAIN_REQ_CALL_DC)             return 12;
        if (ulReqCallType == TRAIN_REQ_CALL_STATION)        return 13;
        if (ulReqCallType == TRAIN_REQ_CALL_FRONT_TRAIN)    return 13;
        if (ulReqCallType == TRAIN_REQ_CALL_BACK_TRAIN)     return 13;

        if (ulReqCallType == STATION_EMERGENCY_REQ_CALL_DC) return 10;
        if (ulReqCallType == STATION_REQ_CALL_DC)           return 12;
        if (ulReqCallType == STATION_REQ_CALL_TRAIN)        return 13;

        return 10;
    }

    return 99;
}

static AnsiString GetSortOrder(TcxTreeListNode *Node)
{
    UINT32 ulTime = Node->Values[0];
    UINT32 ulCallType = Node->Values[1];
    UINT32 ulReqCallType = Node->Values[5].IsNull() ? 0 : (UINT32)Node->Values[5];
    UINT32 ulCallState = (UINT32)Node->Values[4];
    UINT32 ulSortOrder = GetSortOrder(ulCallType, ulReqCallType);
    CHAR   acText[32];

    //  按照呼叫优先级、时间倒序
    sprintf(acText, "%u%03u%012u", ulCallState==CALL_STATE_CLOSED?0:1, 999-ulSortOrder, ulTime);
    return acText;
}

static void ActivateGroupCall(INT64 i64GroupID)
{
    FormGroup->FocusGroupCall(i64GroupID);
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::StartP2PEmergencyCallAlarm()
{
    if (!g_bAutoAcceptP2PEmergencyCall)
    {
        return;
    }

    if (g_ulVoiceAlarm < VOICE_ALARM_EMERGENCY_CALL)
    {
        return;
    }

    g_ulVoiceAlarm = VOICE_ALARM_EMERGENCY_CALL;
    StartVoiceAlarm();
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::StartTrainReqAlarm(UINT32 ulTrainUnitID)
{
    if (g_ulVoiceAlarm < VOICE_ALARM_TRAIN_REQ)
    {
        return;
    }

    g_ulVoiceAlarm = VOICE_ALARM_TRAIN_REQ;
    StartVoiceAlarm();
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::StopTrainReqAlarm(UINT32 ulTrainUnitID)
{
    if (g_ulVoiceAlarm < VOICE_ALARM_TRAIN_REQ)
    {
        return;
    }

    g_ulVoiceAlarm = VOICE_ALARM_NULL;
    StopVoiceAlarm();
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::StartEmergencyCallAlarm(INT64 i64GroupID)
{
    if (g_ulVoiceAlarm < VOICE_ALARM_EMERGENCY_CALL)
    {
        return;
    }

    g_ulVoiceAlarm = VOICE_ALARM_EMERGENCY_CALL;
    StartVoiceAlarm();
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::StopEmergencyCallAlarm(INT64 i64GroupID)
{
    g_ulVoiceAlarm = VOICE_ALARM_NULL;
    StopVoiceAlarm();
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::StartIPHTalkAlarm(UINT32 ulTrainUnitID)
{
    if (g_ulVoiceAlarm < VOICE_ALARM_IPH)
    {
        return;
    }

    g_ulVoiceAlarm = VOICE_ALARM_IPH;
    StartVoiceAlarm();
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::StopIPHTalkAlarm(UINT32 ulTrainUnitID)
{
    if (g_ulVoiceAlarm < VOICE_ALARM_IPH)
    {
        return;
    }

    StopVoiceAlarm();
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::StartReqCallAlarm(BOOL bEmergencyReqCall, UINT32 ulUserID)
{
    GosLog(LOG_DETAIL, "curr voice alarm is %u", g_ulVoiceAlarm);

    if (bEmergencyReqCall)
    {
        if (g_ulVoiceAlarm < VOICE_ALARM_EMERGENCY_REQ_CALL)
        {
            return;
        }

        g_ulVoiceAlarm = VOICE_ALARM_EMERGENCY_REQ_CALL;
    }
    else
    {
        if (g_ulVoiceAlarm < VOICE_ALARM_REQ_CALL)
        {
            return;
        }

        g_ulVoiceAlarm = VOICE_ALARM_REQ_CALL;
    }

    GosLog(LOG_DETAIL, "new voice alarm is %u", g_ulVoiceAlarm);

    StartVoiceAlarm();
}

//---------------------------------------------------------------------------
void __fastcall TFormCallIn::StopReqCallAlarm(UINT32 ulReqCallType, UINT32 ulUserID)
{
    if (ulReqCallType == TRAIN_EMERGENCY_REQ_CALL_DC ||
        ulReqCallType == STATION_EMERGENCY_REQ_CALL_DC )
    {
        if (g_ulVoiceAlarm < VOICE_ALARM_EMERGENCY_REQ_CALL)
        {
            return;
        }
    }
    else
    {
        if (g_ulVoiceAlarm < VOICE_ALARM_REQ_CALL)
        {
            return;
        }
    }

    g_ulVoiceAlarm = VOICE_ALARM_NULL;
    StopVoiceAlarm();
}

//---------------------------------------------------------------------------
void __fastcall TFormCallIn::StartSDSAlarm()
{
    if (g_ulVoiceAlarm < VOICE_ALARM_SDS)
    {
        return;
    }

    g_ulVoiceAlarm = VOICE_ALARM_SDS;
    StartVoiceAlarm();
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::StartVoiceAlarm()
{
    if (!g_bAllowAlarm)
    {
        return;
    }

    AnsiString strFile = "";

    if (g_ulVoiceAlarm == VOICE_ALARM_EMERGENCY_CALL)
    {
        strFile = LocalCfg::GetInstance().GetAlarmFileEmergencyCall();
    }
    else if (g_ulVoiceAlarm == VOICE_ALARM_IPH)
    {
        strFile = LocalCfg::GetInstance().GetAlarmFileIPH();
        if (!FileExists(strFile))
        {
            strFile = LocalCfg::GetInstance().GetAlarmFileEmergencyCall();
        }
    }
    else if (g_ulVoiceAlarm == VOICE_ALARM_EMERGENCY_REQ_CALL)
    {
        strFile = LocalCfg::GetInstance().GetAlarmFileEmergencyCall();
    }
    else if (g_ulVoiceAlarm == VOICE_ALARM_TRAIN_REQ)
    {
        strFile = LocalCfg::GetInstance().GetAlarmFileReqCall();
    }
    else if (g_ulVoiceAlarm == VOICE_ALARM_REQ_CALL)
    {
        strFile = LocalCfg::GetInstance().GetAlarmFileReqCall();
    }
    else if (g_ulVoiceAlarm == VOICE_ALARM_SDS)
    {
        strFile = LocalCfg::GetInstance().GetAlarmFileSDS();
    }
    else
    {
        return;
    }

    if (!FileExists(strFile))
    {
        GosLog(LOG_ERROR, "file %s not exist", strFile.c_str());
        return;
    }

    if (g_ulCurrVoiceAlarm == g_ulVoiceAlarm &&
        WindowsMediaPlayer->playState == WMPPlayState::wmppsPlaying)
    {
        GosLog(LOG_DETAIL, "alread playing alarm %u", g_ulCurrVoiceAlarm);
        return;
    }

    if (g_ulCurrVoiceAlarm != VOICE_ALARM_NULL &&
        g_ulCurrVoiceAlarm != g_ulVoiceAlarm)
    {
        WindowsMediaPlayer->controls->stop();
    }

    WindowsMediaPlayer->URL = String(strFile);  // edtURL->Text;
    WindowsMediaPlayer->controls->play();

    WindowsMediaPlayer->settings->volume = GetVoiceAlarmVolume();

    g_ulCurrVoiceAlarm = g_ulVoiceAlarm;
    if (g_ulVoiceAlarmTime == 0)
    {
        g_ulVoiceAlarmTime = gos_get_uptime();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::StopVoiceAlarm()
{
    if (g_ulCurrVoiceAlarm != 0xffffffff)
    {
        GosLog(LOG_DETAIL, "stop voice alarm %u", g_ulCurrVoiceAlarm);
    }

    WindowsMediaPlayer->controls->stop();

    g_ulVoiceAlarm = VOICE_ALARM_NULL;
    g_ulCurrVoiceAlarm = VOICE_ALARM_NULL;
    g_ulVoiceAlarmTime = 0;

    WindowsMediaPlayer->settings->volume = GetMonVolume();

 // CheckVoiceAlarm();
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::PauseVoiceAlarm()
{
    g_bAllowAlarm = FALSE;

    g_ulCurrVoiceAlarm = VOICE_ALARM_NULL;
    WindowsMediaPlayer->controls->stop();

    WindowsMediaPlayer->settings->volume = GetVoiceAlarmVolume();

//  GosLog(LOG_INFO, "pause voice alarm");
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::ResumeVoiceAlarm()
{
    g_bAllowAlarm = TRUE;
    CheckVoiceAlarm();
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::CheckVoiceAlarm()
{
    UINT32 ulCurrTime = gos_get_uptime();

    if (g_ulVoiceAlarmTime > 0 &&
        (g_ulVoiceAlarmTime + g_ulVoiceAlarmTimeout) < ulCurrTime)
    {
        GosLog(LOG_ERROR, "voice alarm timeout");
        StopVoiceAlarm();
    }

    StartVoiceAlarm();
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::InitSkin()
{
    tlCallIn->OptionsSelection->CellSelect = true;
}

//---------------------------------------------------------------------------
__fastcall TFormCallIn::TFormCallIn(TComponent* Owner)
    : TSubForm(Owner)
{
//  InitMsgHandler(EV_DC_DIS_APP_RSP, MSG_DCGetReqCallInfo, OnDCGetReqCallInfoRsp);
    TColor  ColorBackground = UICfg::GetBackgroundColor();

    SplitterTop->Appearance->Color = ColorBackground;
    SplitterTop->Appearance->ColorTo = ColorBackground;
    SplitterTop->Appearance->ColorHot = ColorBackground;
    SplitterTop->Appearance->ColorHotTo = ColorBackground;

    InitMsgHandler(EV_DIS_DC_IND, MSG_APP_RequestCallRsp, OnDCAcceptReqCallInd);

    InitMsgHandler(EV_DC_DIS_APP_RSP, MSG_DCSetTrainPos, OnDCSetTrainPosInd);
    InitMsgHandler(EV_DC_DIS_APP_RSP, MSG_DCSetTrainGroup, OnDCSetTrainDepotIDInd);

    InitMsgHandler(EV_DIS_DC_IND, MSG_APP_EmergencyCallReq, OnAppEmergencyCallReq);
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::cxTreeListColumn10GetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[0].IsNull())
    {
        return;
    }

    UINT32 ulTime = (UINT32)ANode->Values[0];

    Value = GetTextTime(ulTime, NULL).SubString(12, 10);
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::tlCallInColumnCallTypeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[1].IsNull())
    {
        return;
    }

    UINT32 ulCallType = (UINT32)ANode->Values[1];
    UINT32 ulReqCallType = 0;

    if (!ANode->Values[5].IsNull())
    {
        ulReqCallType = (UINT32)ANode->Values[5];
    }

    Value = GetCallTypeText(ulCallType, ulReqCallType);
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::TrainRegister(UINT32 ulTrainUnitID, UINT32 ulDepotID)
{
    for (int i=0; i<tlCallIn->AbsoluteCount; i++)
    {
        if ((UINT32)tlCallIn->AbsoluteItems[i]->Values[1] == TRAIN_REGISTER &&
            (UINT32)tlCallIn->AbsoluteItems[i]->Values[2] == ulTrainUnitID)
        {
            tlCallIn->AbsoluteItems[i]->Delete();
            break;
        }
    }

//    if (ulDepotID != APP::GetDepotID())
    if (!Depot::IsSameDepot(ulDepotID))
    {
        return;
    }

    TcxTreeListNode *Node = tlCallIn->Add();

    Node->Values[0] = gos_get_current_time();
    Node->Values[1] = TRAIN_REGISTER;
    Node->Values[2] = ulTrainUnitID;
    Node->Values[4] = CALL_STATE_NOT_CONFIRMED;
    Node->Values[5] = INVALID_REQ_CALL;
    Node->Values[6] = GetSortOrder(Node);
    Node->Values[7] = ulDepotID;
    Node->Values[9] = 0;
    Node->Values[10] = -1;
    Node->Values[11] = INVALID_USER_ID;

    if (tlCallIn->SelectionCount == 0)
    {
        Node->Focused = true;
    }

    StartTrainReqAlarm(ulTrainUnitID);
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::OnSetTrainPosReq(GJson &Json, UINT32 ulSeqID)
{
    SET_TRAIN_POS_T     stPos = {0};
    CHAR                *szMsg = Json.GetText();

    if (!Json.GetValue("TrainUnitID", &stPos.ulTrainUnitID))
    {
        GosLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szMsg);
        return;
    }

    if (!Json.GetValue("DepotID", &stPos.ulDepotID))
    {
        GosLog(LOG_ERROR, "invalid DepotID in msg %s", szMsg);
        return;
    }

    if (!Json.GetValue("TrainID", stPos.acTrainID, sizeof(stPos.acTrainID)))
    {
        GosLog(LOG_ERROR, "invalid TrainID in msg %s", szMsg);
        return;
    }

    if (!Json.GetValue("DestID", stPos.acDestID, sizeof(stPos.acDestID)))
    {
        GosLog(LOG_ERROR, "invalid DestID in msg %s", szMsg);
        return;
    }

    if (!Json.GetValue("Direction", &stPos.ulDirection))
    {
        GosLog(LOG_ERROR, "invalid Direction in msg %s", szMsg);
        return;
    }

    if (!Json.GetValue("StationID", &stPos.ulStationID))
    {
        GosLog(LOG_ERROR, "invalid StationID in msg %s", szMsg);
        return;
    }

    if (!Json.GetValue("DriverID", &stPos.ulDriverID))
    {
        GosLog(LOG_ERROR, "invalid DriverID in msg %s", szMsg);
        return;
    }

    g_mSetTrainPos.Add(stPos.ulTrainUnitID, stPos);
    GosLog(LOG_INFO, "train %u set train pos: DepotID=%u, TrainID=%s, DestID=%s, Direction=%u, StationID=%u, DriverID=%u",
           stPos.ulTrainUnitID, stPos.ulDepotID, stPos.acTrainID, stPos.acDestID,
           stPos.ulDirection, stPos.ulStationID, stPos.ulDriverID);

//    if (stPos.ulDepotID != APP::GetDepotID())
    if (!Depot::IsSameDepot(stPos.ulDepotID))
    {
        return;
    }

    TcxTreeList     *tl = tlCallIn;

    for (int i=0; i<tl->AbsoluteCount; i++)
    {
        if ((UINT32)tl->AbsoluteItems[i]->Values[1] == TRAIN_SET_TRAINID &&
            (UINT32)tl->AbsoluteItems[i]->Values[2] == stPos.ulTrainUnitID )
        {
            tl->AbsoluteItems[i]->Delete();
            break;
        }
    }

    TcxTreeListNode     *Node = tl->Add();

    Node->Values[0] = gos_get_current_time();
    Node->Values[1] = TRAIN_SET_TRAINID;
    Node->Values[2] = stPos.ulTrainUnitID;
    Node->Values[4] = CALL_STATE_NOT_CONFIRMED;
    Node->Values[5] = INVALID_REQ_CALL;
    Node->Values[6] = GetSortOrder(Node);
    Node->Values[9] = 0;
    Node->Values[10] = -1;
    Node->Values[11] = INVALID_USER_ID;

    if (tl->SelectionCount == 0)
    {
        Node->Focused = true;
    }

    StartTrainReqAlarm(stPos.ulTrainUnitID);
}
//---------------------------------------------------------------------------
VOID __fastcall TFormCallIn::OnRequestCallReq(GJson &Json, UINT32 ulSeqID)
{
    UINT32      ulReqTime;
    UINT32      ulUserID;
    UINT32      ulReqCallType;
    UINT32      ulDCType;

    if (!Json.GetValue("ReqTime", &ulReqTime))
    {
        GosLog(LOG_ERROR, "invalid ReqTime in msg %s", Json.GetText());
        return;
    }

    if (!Json.GetValue("UserID", &ulUserID))
    {
        GosLog(LOG_ERROR, "invalid UserID in msg %s", Json.GetText());
        return;
    }

    if (!Json.GetValue("CallType", &ulReqCallType))
    {
        GosLog(LOG_ERROR, "invalid CallType in msg %s", Json.GetText());
        return;
    }

    if (!Json.GetValue("DCType", &ulDCType))
    {
        GosLog(LOG_ERROR, "invalid DCType in msg %s", Json.GetText());
        return;
    }

    // 被叫调度台类型匹配
    if (!APP::HasDCType(ulDCType))
    {
        GosLog(LOG_WARN, "DC has no type %u", ulDCType);
        return;
    }

    AnsiString  strTrainList = "";

    if (ulReqCallType == STATION_REQ_CALL_TRAIN)
    {
        CHAR    *szTrainList = Json.GetValue("TrainList");
        UINT32  ulTrainUnitID;

        if (!szTrainList)
        {
            GosLog(LOG_ERROR, "no TrainList in msg %s", Json.GetText());
            return;
        }

        GJson       TrainJson;

        TrainJson.Parse(szTrainList);

        VECTOR<GJson*>      &vSubJson = TrainJson.GetSubJson();

        for (UINT32 i=0; i<vSubJson.size(); i++)
        {
            if (!vSubJson[i]->GetValue("TrainUnitID", &ulTrainUnitID))
            {
                GosLog(LOG_ERROR, "invalid TrainUnitID in msg %s", Json.GetText());
                return;
            }

            if (i > 0)
            {
                strTrainList += ",";
            }

            strTrainList += ulTrainUnitID;
        }
    }

    GosLog(LOG_INFO, "get APP request call type(%u) from user %u, req time is %s",
           ulReqCallType, ulUserID, gos_get_text_time(&ulReqTime));

    TcxTreeList     *tl = NULL;

    if (ulReqCallType == TRAIN_REQ_CALL_DC ||
        ulReqCallType == TRAIN_EMERGENCY_REQ_CALL_DC ||
        ulReqCallType == TRAIN_REQ_CALL_STATION ||
        ulReqCallType == TRAIN_REQ_CALL_FRONT_TRAIN ||
        ulReqCallType == TRAIN_REQ_CALL_BACK_TRAIN )
    {
        tl = tlCallIn;
    }
    else if (ulReqCallType == STATION_REQ_CALL_DC ||
             ulReqCallType == STATION_EMERGENCY_REQ_CALL_DC ||
             ulReqCallType == STATION_REQ_CALL_TRAIN )
    {
        tl = tlCallIn;
    }
    else
    {
        GosLog(LOG_ERROR, "invalid request call type(%u) from user %u", ulReqCallType, ulUserID);
        return;
    }

    for (int i=0; i<tl->AbsoluteCount; i++)
    {
        if ((UINT32)tl->AbsoluteItems[i]->Values[1] == CALL_TYPE_REQ_CALL &&
            (UINT32)tl->AbsoluteItems[i]->Values[2] == ulUserID &&
            (UINT32)tl->AbsoluteItems[i]->Values[5] == ulReqCallType)
        {
            if ((UINT32)tl->AbsoluteItems[i]->Values[4] == CALL_STATE_WAIT ||
                (UINT32)tl->AbsoluteItems[i]->Values[4] == CALL_STATE_CONFIRMED )
            {
                tl->AbsoluteItems[i]->Delete();
                break;
            }
            else if ((UINT32)tl->AbsoluteItems[i]->Values[4] == CALL_STATE_CLOSED)
            {
                if (g_bAutoHideClosed)
                {
                    tl->AbsoluteItems[i]->Delete();
                    i = 0;
                    continue;
                }
            }
            else if ((UINT32)tl->AbsoluteItems[i]->Values[4] == CALL_STATE_ACCEPT)
            {
                tl->AbsoluteItems[i]->Values[15] = ulReqTime;
                return;
            }
        }
    }

    if (ulReqCallType == TRAIN_REQ_CALL_DC ||
        ulReqCallType == TRAIN_EMERGENCY_REQ_CALL_DC ||
        ulReqCallType == TRAIN_REQ_CALL_STATION ||
        ulReqCallType == TRAIN_REQ_CALL_FRONT_TRAIN ||
        ulReqCallType == TRAIN_REQ_CALL_BACK_TRAIN )
    {
        TRAIN_UE_INFO_T     *pstUser = TrainUE::GetTrainUE(ulUserID);

        if (!pstUser)
        {
            GosLog(LOG_ERROR, "unknown user(%u) in msg APP_REQUEST_CALL", ulUserID);
            return;
        }

        TRAIN_INFO_T    *pstTrain = Train::GetTrainInfo(pstUser->ulTrainUnitID);

        if (!pstTrain)
        {
            GosLog(LOG_ERROR, "invalid train unit id of user(%u) in msg APP_REQUEST_CALL", ulUserID);
            return;
        }

        if (ulReqCallType != TRAIN_REQ_CALL_DC &&
            ulReqCallType != TRAIN_EMERGENCY_REQ_CALL_DC)
        {
            if (!Station::IsTrainInDepot(pstTrain))
            {
                return;
            }
        }

        INT64 i64GroupID = INVALID_GROUP_ID;

        if (ulReqCallType == TRAIN_REQ_CALL_DC ||
            ulReqCallType == TRAIN_EMERGENCY_REQ_CALL_DC )
        {
            i64GroupID = pstTrain->i64DefaultGroupID;
        }

        /// 在 tlCallIn 表中插入一条记录
        InsertCallInInfoList(ulUserID, pstTrain->ulTrainUnitID, ulReqCallType, TRAIN_UE, ulReqTime,
                             i64GroupID, GIS::GetGISText(pstTrain->dLon, pstTrain->dLat));

        StartReqCallAlarm(ulReqCallType == TRAIN_EMERGENCY_REQ_CALL_DC, ulUserID);
    }
    else if (ulReqCallType == STATION_REQ_CALL_DC ||
             ulReqCallType == STATION_EMERGENCY_REQ_CALL_DC ||
             ulReqCallType == STATION_REQ_CALL_TRAIN )
    {
        if (!APP::HasDCType(DC_TYPE_TRAIN))
        {
            return;
        }

        STATION_UE_INFO_T       *pstUser = StationUE::GetStationUE(ulUserID);

        if (!pstUser)
        {
            GosLog(LOG_ERROR, "unknown user(%u) in msg APP_REQUEST_CALL", ulUserID);
            return;
        }

        INT64 i64GroupID = INVALID_GROUP_ID;

        if (ulReqCallType == STATION_REQ_CALL_DC ||
            ulReqCallType == STATION_EMERGENCY_REQ_CALL_DC )
        {
            i64GroupID = pstUser->i64DefaultGroupID;
        }

        /// 在 tlCallIn 表中插入一条记录
        InsertCallInInfoList(ulUserID, pstUser->ulStationID, ulReqCallType, STATION_UE, ulReqTime,
                             i64GroupID, "");

        StartReqCallAlarm(ulReqCallType == STATION_EMERGENCY_REQ_CALL_DC, ulUserID);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::NewEmergencyCall(INT64 i64GroupID, UINT32 ulSpeaker)
{
    TcxTreeList     *tl = tlCallIn;

    for (int i=0; i<tl->AbsoluteCount; i++)
    {
        if ((UINT32)tl->AbsoluteItems[i]->Values[1] == CALL_TYPE_EMERGENCY_CALL &&
             (INT64)tl->AbsoluteItems[i]->Values[2] == i64GroupID )
        {
            if ((UINT32)tl->AbsoluteItems[i]->Values[4] == CALL_STATE_NOT_CONFIRMED ||
                (UINT32)tl->AbsoluteItems[i]->Values[4] == CALL_STATE_CONFIRMED)
            {
                tl->AbsoluteItems[i]->Delete();
                break;
            }

            if ((UINT32)tl->AbsoluteItems[i]->Values[4] == CALL_STATE_ACCEPT)
            {
                return;
            }

            if ((UINT32)tl->AbsoluteItems[i]->Values[4] == CALL_STATE_CLOSED)
            {
                if (g_bAutoHideClosed)
                {
                    tl->AbsoluteItems[i]->Delete();
                    i = 0;
                    continue;
                }
            }
        }
    }

    TcxTreeListNode     *Node = tl->Add();

    Node->Values[0] = gos_get_current_time();
    Node->Values[1] = CALL_TYPE_EMERGENCY_CALL;
    Node->Values[2] = i64GroupID;
    Node->Values[4] = CALL_STATE_NOT_CONFIRMED;
    Node->Values[5] = INVALID_REQ_CALL;
    Node->Values[6] = GetSortOrder(Node);
    Node->Values[9] = 0;
    Node->Values[10] = -1;
    Node->Values[11] = INVALID_USER_ID;
    Node->Values[16] = GIS::GetGISText(ulSpeaker);

    if (tl->SelectionCount == 0)
    {
        Node->Focused = true;
    }

    StartEmergencyCallAlarm(i64GroupID);

    GosLog(LOG_INFO, "user %u initiate an emergency group call to "FORMAT_I64, ulSpeaker, i64GroupID);
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::CloseEmergencyCall(INT64 i64GroupID)
{
    TcxTreeList     *tl = tlCallIn;

    for (int i=0; i<tl->AbsoluteCount; i++)
    {
        if ((UINT32)tl->AbsoluteItems[i]->Values[1] == CALL_TYPE_EMERGENCY_CALL &&
             (INT64)tl->AbsoluteItems[i]->Values[2] == i64GroupID)
        {
            if (g_bAutoHideClosed)
            {
                StopEmergencyCallAlarm(i64GroupID);
                tl->AbsoluteItems[i]->Delete();
            }
            else
            {
                tl->AbsoluteItems[i]->Values[4] = CALL_STATE_CLOSED;
            }

            break;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::NewP2PEmergencyCall(EVENT_P2PCALL_STATUS_T &stEvent)
{
    if (stEvent.ulCaller == APP::GetCurrDCUserID())
    {
        return;
    }

    TcxTreeList     *tl = tlCallIn;

    for (int i=0; i<tl->AbsoluteCount; i++)
    {
        if ((UINT32)tl->AbsoluteItems[i]->Values[1] == CALL_TYPE_P2P_EMERGENCY_CALL &&
            (UINT32)tl->AbsoluteItems[i]->Values[2] == stEvent.ulCaller )
        {
        /*  if ((UINT32)tl->AbsoluteItems[i]->Values[4] == CALL_STATE_WAIT ||
                (UINT32)tl->AbsoluteItems[i]->Values[4] == CALL_STATE_ACCEPT )
            {
                break;
            }

            if (g_bAutoHideClosed)
            {
                tl->AbsoluteItems[i]->Delete();
                break;
            }  */

            tl->AbsoluteItems[i]->Values[16] = GIS::GetGISText(stEvent.ulCaller);

            return;
        }
    }

    BOOL    ulCallState = CALL_STATE_TALKING;

    if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_EMERGENCY_BEGIN  ||
        stEvent.ulP2PCallStatus == P2P_IND_STATUS_EMERGENCY_WAIT)
  //        stEvent.ulP2PCallStatus == P2P_IND_STATUS_RINGING )
    {
        TcxTreeListNode *Node = tl->Add();

        if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_EMERGENCY_WAIT)
        {
            ulCallState = CALL_STATE_WAIT;
        }

        if (g_bAutoAcceptP2PEmergencyCall)
        {
            ulCallState = CALL_STATE_ACCEPT;
        }

        Node->Values[0] = gos_get_current_time();
        Node->Values[1] = CALL_TYPE_P2P_EMERGENCY_CALL;
        Node->Values[2] = stEvent.ulCaller;
        Node->Values[4] = ulCallState;
        Node->Values[5] = INVALID_REQ_CALL;
        Node->Values[6] = GetSortOrder(Node);
        Node->Values[9] = 0;
        Node->Values[10] = 0;
        Node->Values[11] = INVALID_USER_ID;
        Node->Values[16] = GIS::GetGISText(stEvent.ulCaller);

        if (tl->SelectionCount == 0)
        {
            Node->Focused = true;
        }

        StartP2PEmergencyCallAlarm();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::NewHalfDuplexP2PCall(EVENT_P2PCALL_STATUS_T &stEvent)
{
    if (stEvent.ulCaller == APP::GetCurrDCUserID())
    {
        return;
    }

    TcxTreeList     *tl = tlCallIn;

    for (int i=0; i<tl->AbsoluteCount; i++)
    {
        if ((UINT32)tl->AbsoluteItems[i]->Values[1] == CALL_TYPE_HALF_DUPLEX_P2P_CALL &&
            (UINT32)tl->AbsoluteItems[i]->Values[2] == stEvent.ulCaller )
        {
            tl->AbsoluteItems[i]->Values[16] = GIS::GetGISText(stEvent.ulCaller);
            return;
        }
    }

    BOOL    ulCallState = CALL_STATE_TALKING;

    TcxTreeListNode *Node = tl->Add();

    Node->Values[0] = gos_get_current_time();
    Node->Values[1] = CALL_TYPE_HALF_DUPLEX_P2P_CALL;
    Node->Values[2] = stEvent.ulCaller;
    Node->Values[4] = ulCallState;
    Node->Values[5] = INVALID_REQ_CALL;
    Node->Values[6] = GetSortOrder(Node);
    Node->Values[9] = 0;
    Node->Values[10] = 0;
    Node->Values[11] = INVALID_USER_ID;
    Node->Values[16] = GIS::GetGISText(stEvent.ulCaller);

    if (tl->SelectionCount == 0)
    {
        Node->Focused = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::NewP2PCall(EVENT_P2PCALL_STATUS_T &stEvent)
{
    if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_EMERGENCY ||
        stEvent.ulP2PCallStatus == P2P_IND_STATUS_EMERGENCY_BEGIN||
        stEvent.ulP2PCallStatus == P2P_IND_STATUS_EMERGENCY_WAIT)
    {
        NewP2PEmergencyCall(stEvent);
        return;
    }

    if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_HALFDUPLEX_BEGIN)
    {
        NewHalfDuplexP2PCall(stEvent);
        return;
    }

    if (stEvent.ulCaller == APP::GetCurrDCUserID())
    {
        return;
    }

    for (int i=0; i<tlCallIn->AbsoluteCount; i++)
    {
        if ((UINT32)tlCallIn->AbsoluteItems[i]->Values[1] == CALL_TYPE_P2P &&
            (UINT32)tlCallIn->AbsoluteItems[i]->Values[2] == stEvent.ulCaller )
        {
            if ((UINT32)tlCallIn->AbsoluteItems[i]->Values[4] == CALL_STATE_WAIT ||
                (UINT32)tlCallIn->AbsoluteItems[i]->Values[4] == CALL_STATE_ACCEPT )
            {
                tlCallIn->AbsoluteItems[i]->Values[16] = GIS::GetGISText(stEvent.ulCaller);
                return;
            }

            if (g_bAutoHideClosed)
            {
                tlCallIn->AbsoluteItems[i]->Delete();
                break;
            }
        }
    }

    if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_RECEIVED ||
        stEvent.ulP2PCallStatus == P2P_IND_STATUS_RINGING )
    {
        TcxTreeListNode *Node = tlCallIn->Add();

        Node->Values[0] = gos_get_current_time();
        Node->Values[1] = CALL_TYPE_P2P;
        Node->Values[2] = stEvent.ulCaller;
        Node->Values[4] = CALL_STATE_WAIT;
        Node->Values[5] = INVALID_REQ_CALL;
        Node->Values[6] = GetSortOrder(Node);
        Node->Values[9] = 0;
        Node->Values[10] = 0;
        Node->Values[11] = INVALID_USER_ID;
        Node->Values[16] = GIS::GetGISText(stEvent.ulCaller);

        if (tlCallIn->SelectionCount == 0)
        {
            Node->Focused = true;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::NewP2PVideoCall(EVENT_P2PVIDEO_STATUS_T &stEvent)
{
    if (stEvent.ulCaller == APP::GetCurrDCUserID())
    {
        return;
    }

    TcxTreeList     *tl = tlCallIn;

    for (int i=0; i<tl->AbsoluteCount; i++)
    {
        if (((UINT32)tl->AbsoluteItems[i]->Values[1] == CALL_TYPE_VIDEO_RECV ||
             (UINT32)tl->AbsoluteItems[i]->Values[1] == CALL_TYPE_VIDEO_P2P ) &&
            (UINT32)tl->AbsoluteItems[i]->Values[2] == stEvent.ulCaller )
        {
            if ((UINT32)tl->AbsoluteItems[i]->Values[4] == CALL_STATE_WAIT ||
                (UINT32)tl->AbsoluteItems[i]->Values[4] == CALL_STATE_ACCEPT )
            {
                break;
            }

            if (g_bAutoHideClosed)
            {
                tl->AbsoluteItems[i]->Delete();
                break;
            }
        }
    }

    if (stEvent.ulStatus == P2PVIDEO_IND_STATUS_RECEIVED || // 终端发起的视频点呼
        stEvent.ulStatus == P2PVIDEO_IND_STATUS_MONITOR  )  // 终端发起的视频回传
    {
        UINT32              ulCallType;
        TcxTreeListNode     *Node = tl->Add();

        if (stEvent.bIsP2PVideoDial)
        {
            ulCallType = CALL_TYPE_VIDEO_P2P;
        }
        else
        {
            ulCallType = CALL_TYPE_VIDEO_RECV;
        }

        Node->Values[0] = gos_get_current_time();
        Node->Values[1] = ulCallType;
        Node->Values[2] = stEvent.ulCaller;
        Node->Values[4] = CALL_STATE_WAIT;
        Node->Values[5] = INVALID_REQ_CALL;
        Node->Values[6] = GetSortOrder(Node);
        Node->Values[7] = stEvent.ulVideoFormat;
        Node->Values[8] = stEvent.iCameraType;
        Node->Values[9] = 0;
        Node->Values[10] = 0;
        Node->Values[11] = INVALID_USER_ID;

        if (tl->SelectionCount == 0)
        {
            Node->Focused = true;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::NewIPHTalk(UINT32 ulTrainUnitID)
{
    if (!APP::HasDCType(DC_TYPE_PASSENGER) )
    {
//      GosLog(LOG_DETAIL, "dc type is %u, ignore the IPH req", g_ulDCType);
        return;
    }

    TcxTreeList *tl = tlCallIn;

    for (int i=0; i<tl->AbsoluteCount; i++)
    {
        if ((UINT32)tl->AbsoluteItems[i]->Values[1] == CALL_TYPE_IPH &&
            (UINT32)tl->AbsoluteItems[i]->Values[2] == ulTrainUnitID )
        {
            if ((UINT32)tl->AbsoluteItems[i]->Values[4] == CALL_STATE_WAIT ||
                (UINT32)tl->AbsoluteItems[i]->Values[4] == CALL_STATE_CONFIRMED ||
                (UINT32)tl->AbsoluteItems[i]->Values[4] == CALL_STATE_ACCEPT )
            {
                return;
            }

            if (g_bAutoHideClosed)
            {
                tl->AbsoluteItems[i]->Delete();
                break;
            }
        }
    }

    TcxTreeListNode *Node = tl->Add();

    Node->Values[0] = gos_get_current_time();
    Node->Values[1] = CALL_TYPE_IPH;
    Node->Values[2] = ulTrainUnitID;
    Node->Values[4] = CALL_STATE_WAIT;
    Node->Values[5] = INVALID_REQ_CALL;
    Node->Values[6] = GetSortOrder(Node);
    Node->Values[9] = 0;
    Node->Values[10] = 0;
    Node->Values[11] = INVALID_USER_ID;

    if (tl->SelectionCount == 0)
    {
        Node->Focused = true;
    }

    StartIPHTalkAlarm(ulTrainUnitID);
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::UpdateIPHTalk(IntSet &sTrain)
{
    IntSet      sExistInfo;
    UINT32      ulTrainUnitID;
    UINT32      ulDCUserID;
    TcxTreeList *tl = tlCallIn;

    if (sTrain.Size() == 0)//&& tl->AbsoluteCount == 0)
    {
        StopIPHTalkAlarm(INVALID_TRAIN_UNIT_ID);
       // return;
    }

    // 删除已经不存在的记录
    for (int i=tl->AbsoluteCount-1; i>=0; i--)
    {
        if ((UINT32)tl->AbsoluteItems[i]->Values[1] != CALL_TYPE_IPH)
        {
            continue;
        }

        ulTrainUnitID = (UINT32)tl->AbsoluteItems[i]->Values[2];
        // sTrain是未接听的列车,需要显示，否则删除
        if (!sTrain.Has(ulTrainUnitID))
        {
            tl->AbsoluteItems[i]->Delete();
            GosLog(LOG_INFO, "remove IPH talk of train %u", ulTrainUnitID);
        }
        else
        {
            sTrain.Del(ulTrainUnitID);
        }
  /*      else
        {
            tl->AbsoluteItems[i]->Values[4] = CALL_STATE_ACCEPT;
            tl->AbsoluteItems[i]->Values[11] = INVALID_USER_ID;//mInfo[ulTrainUnitID];
            sExistInfo.Add(ulTrainUnitID);
        }   */
    }

    // 插入新记录
    for (UINT32 i=0; i<sTrain.Size(); i++)
    {
        ulTrainUnitID = sTrain.Get(i);

        GosLog(LOG_INFO, "new IPH talk of train %u", ulTrainUnitID);

     /*   if (sExistInfo.Has(ulTrainUnitID))
        {
            continue;
        } */

        TcxTreeListNode *Node = tl->Add();

        Node->Values[0] = gos_get_current_time();
        Node->Values[1] = CALL_TYPE_IPH;
        Node->Values[2] = ulTrainUnitID;
        Node->Values[4] = CALL_STATE_WAIT;
        Node->Values[5] = INVALID_REQ_CALL;
        Node->Values[6] = GetSortOrder(Node);
        Node->Values[9] = 0;
        Node->Values[10] = 0;
        Node->Values[11] = INVALID_USER_ID;//it->second;
    }

    /*
    BOOL    bAllAccepted = TRUE;

    for (int i=tl->AbsoluteCount-1; i>=0; i--)
    {
        if ((UINT32)tl->AbsoluteItems[i]->Values[1] != CALL_TYPE_IPH)
        {
            continue;
        }

        ulTrainUnitID = (UINT32)tl->AbsoluteItems[i]->Values[2];
        ulDCUserID = (UINT32)tl->AbsoluteItems[i]->Values[11];

        // 删除已经接听的记录
        if (ulDCUserID != INVALID_USER_ID)
        {
            tl->AbsoluteItems[i]->Delete();
        }
        else
        {
            bAllAccepted = FALSE;
        }
    }

    if (bAllAccepted)
    {
        StopIPHTalkAlarm(INVALID_TRAIN_UNIT_ID);
    }   */
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::AcceptIPHTalk(UINT32 ulTrainUnitID)
{
    TcxTreeList *tl = tlCallIn;

    for (int i=0; i<tl->AbsoluteCount; i++)
    {
        if ((UINT32)tl->AbsoluteItems[i]->Values[1] == CALL_TYPE_IPH &&
            (UINT32)tl->AbsoluteItems[i]->Values[2] == ulTrainUnitID )
        {
            tl->AbsoluteItems[i]->Delete();
            break;
        }
    }

    StopIPHTalkAlarm(INVALID_TRAIN_UNIT_ID);

    return;
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::StopIPHTalk(UINT32 ulTrainUnitID)
{
    TcxTreeList *tl = tlCallIn;

    GosLog(LOG_INFO, "stop IPH talk of train %u", ulTrainUnitID);

    for (int i=0; i<tl->AbsoluteCount; i++)
    {
        if ((UINT32)tl->AbsoluteItems[i]->Values[1] == CALL_TYPE_IPH &&
            (UINT32)tl->AbsoluteItems[i]->Values[2] == ulTrainUnitID )
        {
            if (g_bAutoHideClosed)
            {
                tl->AbsoluteItems[i]->Delete();
            }
            else
            {
                tl->AbsoluteItems[i]->Values[4] = CALL_STATE_CLOSED;
            }

            break;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::SendRequestCallRsp(UINT32 ulReqTime, UINT32 ulUserID, UINT32 ulCallType,
                                                INT64 i64DynamicGroupID, UINT32 ulStationUserID,
                                                VECTOR<UINT32> &vTrainUserID)
{
    GJsonParam      Param;

    Param.Add("ReqTime", ulReqTime);   // ReqTime在数据库中是主键，是请呼的唯一标识
    Param.Add("UserID", ulUserID);
    Param.Add("DCUserID", APP::GetCurrDCUserID());
    Param.Add("CallType", ulCallType);
    Param.Add("Result", TRUE);
    Param.Add("GroupID", i64DynamicGroupID);

    if (ulCallType == TRAIN_REQ_CALL_STATION)
    {
        Param.Add("StationUserID", ulStationUserID);
    }
    else if (ulCallType == STATION_REQ_CALL_TRAIN)
    {
        GJsonTupleParam TupleParam;

        for (UINT32 i=0; i<vTrainUserID.size(); i++)
        {
            GJsonParam  SubParam;

            SubParam.Add("UserID", vTrainUserID[i]);
            TupleParam.Add(SubParam);
        }
        Param.Add("TrainUserID", TupleParam);
    }

    SendServer(EV_APP_DIS_DC_RSP, MSG_APP_RequestCallRsp, Param);

    GosLog(LOG_INFO, "send request call rsp to %u", ulUserID);
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::CloseRequestCall(UINT32 ulUserID, UINT32 ulCallType)
{
    GJsonParam      Param;

    Param.Add("UserID", ulUserID);
    Param.Add("CallType", ulCallType);
    Param.Add("DCUserID", APP::GetCurrDCUserID());

    SendServer(EV_DC_DIS_APP_REQ, MSG_DCCloseReqCall, Param);
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::ApplyReqCall(UINT32 ulUserID, UINT32 ulCallType)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::FormCreate(TObject *Sender)
{
    SetImage(pmiAccept, IMG_ACCEPT_CALL);
    SetImage(pmiConfirm, IMG_OK);
    SetImage(pmiHangup, IMG_REJECT_CALL);
}

//---------------------------------------------------------------------------
void __fastcall TFormCallIn::ShowGroupCallWin(INT64 i64GroupID, AnsiString strCaption)
{
    AnsiString      strGroupName = Group::GetGroupName(i64GroupID);
    TRAIN_INFO_T    *pstTrain = Train::GetTrainInfo(i64GroupID);

    if (pstTrain)
    {
        strGroupName = Train::GetTrainAlias(pstTrain->ulTrainUnitID, pstTrain->acTrainID);
    }
    else
    {
        STATION_INFO_T      *pstStation = Station::GetStationByGroupID(i64GroupID);

        if (pstStation)
        {
            strGroupName = pstStation->acStationName;
        }
    }

    if (FormMain != FormGroupCallWin->Parent)
    {
        FormGroupCallWin->Parent = FormMain;
        FormGroupCallWin->Left = FormMain->PanelMon->Left - FormGroupCallWin->Width - 150;
        FormGroupCallWin->Top  = FormMain->PanelMon->Top + FormMain->PanelMon->Height/2 - FormGroupCallWin->Height/2 - 50;

        FormGroupCallWin->m_iLeft = FormGroupCallWin->Left;
        FormGroupCallWin->m_iTop = FormGroupCallWin->Top;
    }
    else
    {
        FormGroupCallWin->Left = FormGroupCallWin->m_iLeft;
        FormGroupCallWin->Top = FormGroupCallWin->m_iTop;
    }

    if (FormGroupCallWin->Left > FormMain->Width)
    {
        FormGroupCallWin->Left = FormMain->Width - FormGroupCallWin->Width;
    }

    if (FormGroupCallWin->Left < 0)
    {
        FormGroupCallWin->Left = 10;
    }

    if (FormGroupCallWin->Top > FormMain->Height)
    {
        FormGroupCallWin->Top = FormMain->Height - FormGroupCallWin->Height;
    }

    if (FormGroupCallWin->Top < 0)
    {
        FormGroupCallWin->Top = 10;
    }

    Group::SubJoinGroup(i64GroupID);

    FormGroupCallWin->BringToFront();

    FormGroupCallWin->Init(i64GroupID, strGroupName);

    if (strCaption.IsEmpty())
    {
        strCaption = "组呼";
    }

    FormGroupCallWin->Caption = strCaption;

    FormGroupCallWin->Show();
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::ShowP2PCallWin(UINT32 ulUserID, bool bEmergencyCall, bool bAutoAccept)
{
    if (P2P::IsCalling())
    {
        InfoMsg("正在通话中");
        return;
    }

    if (FormMain != FormP2PCallWin->Parent)
    {
        FormP2PCallWin->Parent = FormMain; //gbMain;
        FormP2PCallWin->Left = FormMain->PanelMon->Left + 50;
        FormP2PCallWin->Top = FormMain->PanelMon->Top + FormMain->PanelCallIn->Height - FormP2PCallWin->Height - 80;
    }

    FormUser->StopListen();

    if (!bAutoAccept &&
        !P2P::Recv(ulUserID))
    {
        InfoMsg("接听失败");
        return;  //      TODO
    }

    /// SetP2PCalling(TRUE);

    FormP2PCallWin->Init(ulUserID, bEmergencyCall);
    FormP2PCallWin->BringToFront();
    FormP2PCallWin->Show();
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::PickUpEmergencyCall()
{
    TcxTreeListNode *Node = tlCallIn->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    INT64 i64GroupID = INVALID_GROUP_ID;
    i64GroupID = (INT64)Node->Values[2];

    if (Node->Values[4] == CALL_STATE_ACCEPT ||
        Node->Values[4] == CALL_STATE_CLOSED )
    {
        return;
    }

    Node->Values[4] = CALL_STATE_ACCEPT;

    Node->Values[9] = -1;
    Node->Values[10] = 0;
    Node->Values[11] = INVALID_USER_ID;

    StopEmergencyCallAlarm(i64GroupID);

    ShowGroupCallWin(i64GroupID, "紧急呼叫");
}

void __fastcall TFormCallIn::PickUpP2PCall(bool bEmergencyCall)
{
    UINT32 ulUserID = INVALID_USER_ID;
    TcxTreeListNode *Node = tlCallIn->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    ulUserID = (UINT32)Node->Values[2];

    if (Node->Values[4] == CALL_STATE_ACCEPT ||
        Node->Values[4] == CALL_STATE_CLOSED )
    {
        return;
    }

    Node->Values[4] = CALL_STATE_ACCEPT;

    Node->Values[9] = -1;
    Node->Values[10] = 0;
    Node->Values[11] = INVALID_USER_ID;

    ShowP2PCallWin(ulUserID, bEmergencyCall, false);
}

void __fastcall TFormCallIn::PickUpP2PVideo()
{
    UINT32 ulUserID = INVALID_USER_ID;
    FormMain->ActivePage(PAGE_VIDEO);

    TcxTreeListNode *Node = tlCallIn->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }
    ulUserID = (UINT32)Node->Values[2];

    UINT32 ulVideoFormat = (UINT32)Node->Values[7];
    INT32 iCameraType = (INT32)Node->Values[8];

    if (Node->Values[4] == CALL_STATE_ACCEPT ||
        Node->Values[4] == CALL_STATE_CLOSED )
    {
        return;
    }

    Node->Values[4] = CALL_STATE_ACCEPT;

    Node->Values[9] = -1;
    Node->Values[10] = 0;
    Node->Values[11] = INVALID_USER_ID;

    UINT32 ulCallType = (UINT32)Node->Values[1];

    if (FormVideoCall->RecvVideoCall(ulCallType, ulUserID, ulVideoFormat, iCameraType))
    {
    }
}

void __fastcall TFormCallIn::PickUpIPH()
{
    TcxTreeListNode *Node = tlCallIn->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    UINT32 ulTrainUnitID = (UINT32)Node->Values[2];

    StopIPHTalkAlarm(ulTrainUnitID);

    Node->Values[4] = CALL_STATE_CONFIRMED;

    FormMain->ActivePage(PAGE_IPH);
    FormIPH->AutoAccept();
}

void __fastcall TFormCallIn::PickUpTrainReqOrTrainEmergencyCallDC(INT64 &i64DynamicGroupID)
{
    TcxTreeListNode *Node = tlCallIn->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }
    UINT32 ulTrainUnitID = (UINT32)Node->Values[3];
    UINT32 ulRequestCallType = (UINT32)Node->Values[5];
    TRAIN_INFO_T *pstTrain = Train::GetTrainInfo(ulTrainUnitID);
    UINT32 ulUserID = (UINT32)Node->Values[2];
    if (!pstTrain)
    {
        GosLog(LOG_ERROR, "unknown train %u", ulTrainUnitID);
        ErrMsg("未知列车");

        Node->Delete();

        StopReqCallAlarm(ulRequestCallType, ulUserID);
        return;
    }

    INT64 i64GroupID = Train::GetTrainGroupID(pstTrain);
    if (i64GroupID == INVALID_GROUP_ID)
    {
        ErrMsg("未知列车");
        Node->Delete();

        StopReqCallAlarm(ulRequestCallType, ulUserID);
        return;
    }

    i64DynamicGroupID = i64GroupID;
    ActivateGroupCall(i64DynamicGroupID);

    Node->Values[4] = CALL_STATE_ACCEPT;
    StopReqCallAlarm(ulRequestCallType, ulUserID);
}

void __fastcall TFormCallIn::PickUpStationReqCallOrStationEmergencyCallDC(INT64 &i64DynamicGroupID)
{
    TcxTreeListNode *Node = tlCallIn->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    UINT32 ulStationID = (UINT32)Node->Values[3];
    STATION_UE_INFO_T *pstStationUE = StationUE::GetStationUEByStationID(ulStationID);
    UINT32 ulRequestCallType = (UINT32)Node->Values[5];
    UINT32 ulUserID = (UINT32)Node->Values[2];

    if (!pstStationUE)
    {
        GosLog(LOG_ERROR, "unknown station %u", ulStationID);
        ErrMsg("该车站未配置固定台");

        Node->Delete();
        StopReqCallAlarm(ulRequestCallType, ulUserID);

        return;
    }

    i64DynamicGroupID = pstStationUE->i64DefaultGroupID;
    ActivateGroupCall(i64DynamicGroupID);

    Node->Values[4] = CALL_STATE_ACCEPT;
    StopReqCallAlarm(ulRequestCallType, ulUserID);
}

void __fastcall TFormCallIn::PickUpTrainReqCallStation(INT64 &i64DynamicGroupID)
{
    VECTOR<UINT32> vUserID;

    TcxTreeListNode *Node = tlCallIn->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    UINT32 ulTrainUnitID = (UINT32)Node->Values[3];
    TRAIN_INFO_T *pstTrain = Train::GetTrainInfo(ulTrainUnitID);
    UINT32 ulRequestCallType = (UINT32)Node->Values[5];
    UINT32 ulUserID = (UINT32)Node->Values[2];
    if (!pstTrain)
    {
        GosLog(LOG_ERROR, "unknown train %u", ulTrainUnitID);
        ErrMsg("动态重组失败");

        StopReqCallAlarm(ulRequestCallType, ulUserID);

        return;
    }

    STATION_UE_INFO_T *pstStationUE = StationUE::GetStationUEByStationID(pstTrain->ulStationID);
    if (!pstStationUE)
    {
        GosLog(LOG_ERROR, "unknown station %u", pstTrain->ulStationID);
        StopReqCallAlarm(ulRequestCallType, ulUserID);
        ErrMsg("该车站未配置固定台");
        Node->Delete();

        return;
    }

    /// UINT32 ulStationUserID = pstStationUE->ulUserID;

    vUserID.push_back(pstStationUE->ulUserID);
    vUserID.push_back(ulUserID);
    vUserID.push_back(APP::GetCurrDCUserID());

    i64DynamicGroupID = InitReqCallGroup(GROUP_NAME_REQ_CALL, vUserID);
    if (i64DynamicGroupID == INVALID_GROUP_ID)
    {
        GosLog(LOG_ERROR, "init req call dynamic group failed");
        StopReqCallAlarm(ulRequestCallType, ulUserID);
        ErrMsg("动态重组失败");

        return;
    }

    ActivateGroupCall(i64DynamicGroupID);

    Node->Values[4] = CALL_STATE_ACCEPT;
    StopReqCallAlarm(ulRequestCallType, ulUserID);
}

void __fastcall TFormCallIn::PickUpStationReqCallTrain(INT64 &i64DynamicGroupID)
{
    AnsiString strTrainList = "";
    TcxTreeListNode *Node = tlCallIn->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    UINT32 ulStationID = (UINT32)Node->Values[3];
    STATION_UE_INFO_T *pstStationUE = StationUE::GetStationUEByStationID(ulStationID);
    UINT32 ulRequestCallType = (UINT32)Node->Values[5];
    UINT32 ulUserID = (UINT32)Node->Values[2];

    if (!pstStationUE)
    {
        GosLog(LOG_ERROR, "unknown station %u", ulStationID);
        ErrMsg("该车站未配置固定台");

        Node->Delete();
        StopReqCallAlarm(ulRequestCallType, ulUserID);
        return;
    }

    VECTOR<UINT32> vUserID;
    if (!Node->Values[13].IsNull())
    {
        strTrainList = Node->Values[13];
    }

    if (strTrainList.IsEmpty())
    {
        TrainUE::GetTrainUserIDOfStation(ulStationID, vUserID);
    }
    else
    {
        AnsiString strTrain;
        UINT32 ulUserID1;
        UINT32 ulUserID2;

        strTrainList += ",";
        while (!strTrainList.IsEmpty())
        {
            int iPos = strTrainList.Pos(",");

            if (iPos > 0)
            {
                strTrain = strTrainList.SubString(1, iPos - 1);
                strTrainList = strTrainList.SubString(iPos + 1, strTrainList.Length());

                UINT32 ulTrainUnitID = strTrain.ToIntDef(INVALID_TRAIN_UNIT_ID);
                Train::GetTrainUserID(ulTrainUnitID, &ulUserID1, &ulUserID2);
                if (ulUserID1 != INVALID_TRAIN_UNIT_ID)
                {
                    vUserID.push_back(ulUserID1);
                }
                else if (ulUserID2 != INVALID_TRAIN_UNIT_ID)
                {
                    vUserID.push_back(ulUserID2);
                }
            }
            else
            {
                break;
            }
        }
    }

    VECTOR<UINT32> vTrainUserID;

    vTrainUserID.assign(vUserID.begin(), vUserID.end());
    vUserID.push_back(ulUserID);

    i64DynamicGroupID = InitReqCallGroup(GROUP_NAME_REQ_CALL, vUserID);
    if (i64DynamicGroupID == INVALID_GROUP_ID)
    {
        GosLog(LOG_ERROR, "init req call dynamic group failed");
        StopReqCallAlarm(ulRequestCallType, ulUserID);
        ErrMsg("动态重组失败");

        return;
    }

    ActivateGroupCall(i64DynamicGroupID);

    Node->Values[4] = CALL_STATE_ACCEPT;
    StopReqCallAlarm(ulRequestCallType, ulUserID);
}

void __fastcall TFormCallIn::PickUpReqCall()
{
    UINT32 ulUserID = INVALID_USER_ID;
    TcxTreeListNode *Node = tlCallIn->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }
    ulUserID = (UINT32)Node->Values[2];

    INT64 i64DynamicGroupID = INVALID_GROUP_ID;
    /// TRAIN_INFO_T *pstTrain = NULL;
    /// STATION_UE_INFO_T *pstStationUE = NULL;
    UINT32 ulTrainUnitID;
    UINT32 ulStationID;
    UINT32 ulReqTime;

    if (Node->Values[4] == CALL_STATE_ACCEPT || Node->Values[4] == CALL_STATE_CLOSED)
    {
        return;
    }

    ulReqTime = Node->Values[15];

    Node->Values[9] = -1;
    Node->Values[10] = -1;
    Node->Values[11] = INVALID_USER_ID;
    /// UINT32 ulCallType = (UINT32)Node->Values[1];
    UINT32 ulStationUserID;
    VECTOR<UINT32> vTrainUserID;
    UINT32 ulRequestCallType = (UINT32)Node->Values[5];

    if (ulRequestCallType == TRAIN_REQ_CALL_DC ||
        ulRequestCallType == TRAIN_EMERGENCY_REQ_CALL_DC)
    {
        PickUpTrainReqOrTrainEmergencyCallDC(i64DynamicGroupID);
    }
    else if (ulRequestCallType == STATION_REQ_CALL_DC ||
             ulRequestCallType == STATION_EMERGENCY_REQ_CALL_DC)
    {
        PickUpStationReqCallOrStationEmergencyCallDC(i64DynamicGroupID);
    }
    else if (ulRequestCallType == TRAIN_REQ_CALL_STATION)
    {
        PickUpTrainReqCallStation(i64DynamicGroupID);
    }
    else if (ulRequestCallType == STATION_REQ_CALL_TRAIN)
    {
        PickUpStationReqCallTrain(i64DynamicGroupID);
    }
    else
    {
        GosLog(LOG_ERROR, "unknow request call type %u", ulRequestCallType);
        return;
    }

    if (i64DynamicGroupID != INVALID_GROUP_ID)
    {
        Node->Values[9] = -1;
        Node->Values[10] = 0;
        Node->Values[11] = APP::GetCurrDCUserID();
        Node->Values[12] = i64DynamicGroupID;

        SendRequestCallRsp(ulReqTime, ulUserID, ulRequestCallType, i64DynamicGroupID, ulStationUserID, vTrainUserID);

        PTT::Dial(i64DynamicGroupID);
        gos_sleep_1ms(300);
        PTT::Release();

        ShowGroupCallWin(i64DynamicGroupID, "请呼");
    }
}

/**
 * @brief           请呼窗口的接听的按钮触发事件
 * @param           Sender    [in]
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
void __fastcall TFormCallIn::btnAcceptClick(TObject *Sender)
{
    if (tlCallIn->SelectionCount == 0)
    {
        InfoMsg("未选定记录");
        return;
    }

    TcxTreeListNode *Node = tlCallIn->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    /// UINT32 ulReqTime = 0;
    UINT32 ulCallType = (UINT32)Node->Values[1];

    if (ulCallType == CALL_TYPE_EMERGENCY_CALL)
    {
        PickUpEmergencyCall();
    }
    else if (ulCallType == CALL_TYPE_P2P ||
             ulCallType == CALL_TYPE_P2P_TRANSFER ||
             ulCallType == CALL_TYPE_P2P_EMERGENCY_CALL)
    {
        BOOL    bEmergencyCall = (ulCallType == CALL_TYPE_P2P_EMERGENCY_CALL);

        PickUpP2PCall(bEmergencyCall);
    }
    else if (ulCallType == CALL_TYPE_HALF_DUPLEX_P2P_CALL )
    {
        UINT32 ulUserID = (UINT32)Node->Values[2];

        Node->Values[4] = CALL_STATE_ACCEPT;

        Node->Values[9] = -1;
        Node->Values[10] = 0;
        Node->Values[11] = INVALID_USER_ID;

        ShowHalfDuplexP2PCallWin(ulUserID);
    }
    else if (ulCallType == CALL_TYPE_VIDEO_P2P ||
             ulCallType == CALL_TYPE_VIDEO_RECV)
    {
        PickUpP2PVideo();
    }
    else if (ulCallType == CALL_TYPE_IPH)
    {
        PickUpIPH();
    }
    else if (ulCallType == CALL_TYPE_REQ_CALL)
    {
        PickUpReqCall();
    }
    else
    {
        GosLog(LOG_ERROR, "unknow call type(%u)", ulCallType);
        InfoMsg("未知呼叫类型(%d)", ulCallType);
    }
}
//---------------------------------------------------------------------------
/**
 * @brief           请呼窗口挂断按钮点击事件
 * @param           Sender    [in]
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
void __fastcall TFormCallIn::btnHangupClick(TObject *Sender)
{
    TcxTreeList     *tl = tlCallIn;

    if (tl->SelectionCount == 0)
    {
        InfoMsg("未选定记录");
        return;
    }

    TcxTreeListNode *Node = tl->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    UINT32 ulCallType = (UINT32)Node->Values[1];

    if (ulCallType == CALL_TYPE_P2P ||
        ulCallType == CALL_TYPE_P2P_TRANSFER ||
        ulCallType == CALL_TYPE_HALF_DUPLEX_P2P_CALL)
    {
        UINT32 ulUserID = (UINT32)Node->Values[2];

        if (!Node->Values[4].IsNull() &&
            (UINT32)Node->Values[4] == CALL_STATE_ACCEPT)
        {
            P2P::Hangup(ulUserID);
        }
        else if (!P2P::Reject(ulUserID))
        {
            WarnMsg("拒接失败");
            return;
        }
    }
    else if (ulCallType == CALL_TYPE_P2P_EMERGENCY_CALL)
    {
        UINT32  ulUserID = (UINT32)Node->Values[2];

        if (!Node->Values[4].IsNull() &&
            (UINT32)Node->Values[4] == CALL_STATE_ACCEPT)
        {
            P2P::Hangup(ulUserID);
        }
    }
    else if (ulCallType == CALL_TYPE_VIDEO_P2P ||
             ulCallType == CALL_TYPE_VIDEO_RECV )
    {
        UINT32 ulUserID = (UINT32)Node->Values[2];

        if (!Node->Values[4].IsNull() &&
            (UINT32)Node->Values[4] == CALL_STATE_ACCEPT)
        {
            P2P::StopVideo(ulUserID);
        }
        else if (!P2P::RejectVideo(ulUserID))
        {
            WarnMsg("拒接失败");
            return;
        }
    }
    else if (ulCallType == CALL_TYPE_REQ_CALL)
    {
        UINT32  ulRequestCallType = (UINT32)Node->Values[5];
        UINT32  ulUserID = (UINT32)Node->Values[2];

        if (!Node->Values[11].IsNull() &&
            (UINT32)Node->Values[11] == (UINT32)APP::GetCurrDCUserID())
        {
            CloseRequestCall(ulUserID, ulRequestCallType);
            Node->Delete();
        }
    }
    else
    {
        GosLog(LOG_DETAIL, "not handle call type %u", ulCallType);
    }
}
//---------------------------------------------------------------------------
/**
 * @brief           为请呼列表特定列添加颜色的函数
 * @param           Sender      [in]
 * @param           ACanvas     [in]
 * @param           AViewInfo   [in]
 * @param           ADone       [in]
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
void __fastcall TFormCallIn::tlCallInCustomDrawDataCell(TcxCustomTreeList *Sender, TcxCanvas *ACanvas,
          TcxTreeListEditCellViewInfo *AViewInfo, bool &ADone)
{
    if (!AViewInfo->Node ||
        AViewInfo->Node->Values[1].IsNull() ||
        AViewInfo->Column->ItemIndex != 1 )
    {
        return;
    }

    UINT32  ulCallType = AViewInfo->Node->Values[1];
    UINT32  ulCallStatus = AViewInfo->Node->Values[4];

    // 紧急呼叫、IPH(红色)、紧急请呼(橙色)、请呼、转组/设置列车位置(蓝色)
    if (ulCallStatus == CALL_STATE_CLOSED)
    {
        ACanvas->Canvas->Brush->Color = g_CallColorGray;
    }
    else
    {
        if (ulCallType == CALL_TYPE_EMERGENCY_CALL ||
            ulCallType == CALL_TYPE_IPH ||
            ulCallType == CALL_TYPE_P2P_EMERGENCY_CALL)
        {
            ACanvas->Canvas->Brush->Color = g_CallColorRed;
        }
        else if (ulCallType == CALL_TYPE_REQ_CALL)
        {
            UINT32  ulReqCallType = AViewInfo->Node->Values[5];

            if (ulReqCallType == TRAIN_EMERGENCY_REQ_CALL_DC ||
                ulReqCallType == STATION_EMERGENCY_REQ_CALL_DC )
            {
                ACanvas->Canvas->Brush->Color = g_CallColorOrange;
            }
        }
        else if (ulCallType == TRAIN_REGISTER ||
                 ulCallType == TRAIN_SET_TRAINID )
        {
            ACanvas->Canvas->Brush->Color = g_CallColorGreen;
        }
    }
}
//---------------------------------------------------------------------------
/**
 * @brief           为请呼列表表格中的通话状态转换成文字的事件
 * @param           Sender   [in]
 * @param           ANode    [in]
 * @param           Value    [out]
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
void __fastcall TFormCallIn::cxTreeListColumn13GetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (!ANode || ANode->Values[4].IsNull())
    {
        return;
    }

    UINT32  ulState = ANode->Values[4];

    Value = GetCallStateText(ulState);
}
//---------------------------------------------------------------------------
/**
 * @brief           请呼列表确认按钮点击事件
 * @param           Sender    [in]
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
void __fastcall TFormCallIn::btnConfirmClick(TObject *Sender)
{
    TcxTreeList     *tl = tlCallIn;

    if (tl->SelectionCount == 0)
    {
        InfoMsg("未选定记录");
        return;
    }

    TcxTreeListNode *Node = tl->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    UINT32  ulCallType = (UINT32)Node->Values[1];
    INT64   i64GroupID = INVALID_GROUP_ID;
    UINT32  ulTrainUnitID;
    UINT32  ulUserID = INVALID_USER_ID;

    if (ulCallType == TRAIN_REGISTER)
    {
        ulTrainUnitID = (UINT32)Node->Values[2];

        UINT32          ulTrainPos = (UINT32)Node->Values[7];
        TRAIN_INFO_T    *pstTrain = Train::GetTrainInfo(ulTrainUnitID);

        StopTrainReqAlarm(ulTrainUnitID);

        if (!pstTrain)
        {
            ErrMsg("未知列车");
            Node->Delete();
            return;
        }

        if (SetTrainPos(pstTrain, ulTrainPos))
        {
            if (g_bAutoHideClosed)
            {
                Node->Delete();
            }
            else
            {
                Node->Values[4] = CALL_STATE_CONFIRMED;
            }
        }
    }
    else if (ulCallType == TRAIN_SET_TRAINID)
    {
        ulTrainUnitID = (UINT32)Node->Values[2];

        UINT32          ulTrainPos;
        TRAIN_INFO_T    *pstTrain = Train::GetTrainInfo(ulTrainUnitID);

        StopTrainReqAlarm(ulTrainUnitID);

        if (!pstTrain)
        {
            ErrMsg("未知列车");
            Node->Delete();
            return;
        }

        SET_TRAIN_POS_T *pstPos = g_mSetTrainPos.GetValue(ulTrainUnitID);

        if (SetTrainPos(pstTrain, pstPos))
        {
            if (g_bAutoHideClosed)
            {
                Node->Delete();
            }
            else
            {
                Node->Values[4] = CALL_STATE_CONFIRMED;
            }
        }
    }
    else if (ulCallType == CALL_TYPE_EMERGENCY_CALL)
    {
        i64GroupID = (INT64)Node->Values[2];
        StopEmergencyCallAlarm(i64GroupID);

        Node->Values[4] = CALL_STATE_CONFIRMED;
    }
    else if (ulCallType == CALL_TYPE_REQ_CALL)
    {
        UINT32  ulReqCallType = Node->Values[5];

        ulUserID = (UINT32)Node->Values[2];
        StopReqCallAlarm(ulReqCallType, ulUserID);

        Node->Values[4] = CALL_STATE_CONFIRMED;
    }
    else if (ulCallType == CALL_TYPE_IPH)
    {
        ulTrainUnitID = (UINT32)Node->Values[2];
        StopIPHTalkAlarm(ulTrainUnitID);

        Node->Values[4] = CALL_STATE_CONFIRMED;
    }
    else if (ulCallType == CALL_TYPE_P2P_EMERGENCY_CALL)
    {
        ulUserID = (UINT32)Node->Values[2];
        StopEmergencyCallAlarm(ulUserID);

        if (!g_bAutoAcceptP2PEmergencyCall)
        {
            Node->Values[4] = CALL_STATE_CONFIRMED;
        }
    }
    else
    {
        GosLog(LOG_DETAIL, "not handle the confirm click, call type is %u", ulCallType);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::HandleReqCall(TcxTreeListNode *Node, UINT32 ulUserID, UINT32 ulReqCallType)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::TimerCheckTimer(TObject *Sender)
{
    CheckVoiceAlarm();
}
//---------------------------------------------------------------------------
/**
 * @brief           在其他调度台已经处理该消息的时候，
 * @param           ulSeqID     [in]
 * @param           Json        [in]
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
void __fastcall TFormCallIn::OnDCAcceptReqCallInd(UINT32 ulSeqID, GJson &Json)
{
    INT64       i64GroupID = 0;
    UINT32      ulUserID = 0;
    UINT32      ulDCUserID = 0;
    UINT32      ulReqCallType = 0;
    UINT32      ulResult;
    GJsonParam  Param;

    if (!Json.GetValue("UserID", &ulUserID))
    {
        GosLog(LOG_ERROR, "invalid UserID in DCAcceptReqCallInd");
        return;
    }

    if (!Json.GetValue("DCUserID", &ulDCUserID))
    {
        GosLog(LOG_ERROR, "invalid DCUserID in DCAcceptReqCallInd");
        return;
    }

    if (!Json.GetValue("CallType", &ulReqCallType))
    {
        GosLog(LOG_ERROR, "invalid CallType in DCAcceptReqCallInd");
        return;
    }

    if (!Json.GetValue("Result", &ulResult))
    {
        GosLog(LOG_ERROR, "invalid Result in DCAcceptReqCallInd");
        return;
    }

    if (!Json.GetValue("GroupID", &i64GroupID))
    {
        GosLog(LOG_ERROR, "invalid GroupID in DCAcceptReqCallInd");
        return;
    }

    TcxTreeList     *tl = NULL;

    if (ulReqCallType == TRAIN_REQ_CALL_DC ||
        ulReqCallType == TRAIN_EMERGENCY_REQ_CALL_DC ||
        ulReqCallType == TRAIN_REQ_CALL_STATION ||
        ulReqCallType == TRAIN_REQ_CALL_FRONT_TRAIN ||
        ulReqCallType == TRAIN_REQ_CALL_BACK_TRAIN )
    {
        tl = tlCallIn;
    }
    else if (ulReqCallType == STATION_REQ_CALL_DC ||
             ulReqCallType == STATION_EMERGENCY_REQ_CALL_DC ||
             ulReqCallType == STATION_REQ_CALL_TRAIN )
    {
        tl = tlCallIn;
    }
    else
    {
        GosLog(LOG_ERROR, "invalid request call type(%u) from user %u", ulReqCallType, ulUserID);
        return;
    }

    for (int i=0; i<tl->AbsoluteCount; i++)
    {
        TcxTreeListNode * Node = tl->AbsoluteItems[i];

        if ((UINT32)Node->Values[1] == CALL_TYPE_REQ_CALL &&
            (INT64)Node->Values[2] == (INT64)ulUserID &&
            !Node->Values[5].IsNull() &&
            (UINT32)Node->Values[5] == ulReqCallType)
        {
            StopReqCallAlarm(ulReqCallType, ulUserID);

            Node->Values[4] = CALL_STATE_ACCEPT;
            Node->Values[11] = ulDCUserID;

            Node->Delete();
            break;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::OnDCSetTrainPosInd(UINT32 ulSeqID, GJson &Json)
{
    UINT32      ulTrainUnitID = 0;
    CHAR        *szResult;
    GJsonParam  Param;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        GosLog(LOG_ERROR, "invalid TrainUnitID in DCSetTrainPos");
        return;
    }

    szResult = Json.GetValue("Result");
    if (!szResult)
    {
        GosLog(LOG_ERROR, "invalid Result in DCSetTrainPos");
        return;
    }

    if (strcasecmp(szResult, "succ") != 0)
    {
        return;
    }

    TcxTreeList     *tl = tlCallIn;

    for (int i=0; i<tl->AbsoluteCount; i++)
    {
        TcxTreeListNode * Node = tl->AbsoluteItems[i];

        if ((UINT32)Node->Values[1] == TRAIN_SET_TRAINID &&
            (INT64)Node->Values[2] == (INT64)ulTrainUnitID)
        {
            StopTrainReqAlarm(ulTrainUnitID);

            Node->Delete();
            break;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::OnDCSetTrainDepotIDInd(UINT32 ulSeqID, GJson &Json)
{
    UINT32      ulTrainUnitID = 0;
    GJsonParam  Param;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        GosLog(LOG_ERROR, "invalid TrainUnitID in DCSetTrainPos");
        return;
    }

    TcxTreeList     *tl = tlCallIn;

    for (int i=0; i<tl->AbsoluteCount; i++)
    {
        TcxTreeListNode * Node = tl->AbsoluteItems[i];

        if ((UINT32)Node->Values[1] == TRAIN_REGISTER &&
            (INT64)Node->Values[2] == (INT64)ulTrainUnitID)
        {
            StopTrainReqAlarm(ulTrainUnitID);

            Node->Delete();
            break;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::OnAppEmergencyCallReq(UINT32 ulSeqID, GJson &Json)
{
    UINT32              ulUserID;
    UINT32              ulTime;
    UINT32              ulEB;

    if (!APP::HasDCType(DC_TYPE_TRAIN))
    {
        return;
    }

    if (!Json.GetValue("UserID", &ulUserID))
    {
        GosLog(LOG_ERROR, "invalid UserID in msg %s", Json.GetText());
        return;
    }

    if (!Json.GetValue("Time", &ulTime))
    {
        GosLog(LOG_ERROR, "invalid Time in msg %s", Json.GetText());
        return;
    }

    if (!Json.GetValue("EB", &ulEB))
    {
        GosLog(LOG_ERROR, "invalid EB in msg %s", Json.GetText());
        return;
    }

    GosLog(LOG_FATAL, "emergency call req from %u%s", ulUserID, (ulEB != 0) ? " by EB" : "");

    TRAIN_UE_INFO_T     *pstUser = TrainUE::GetTrainUE(ulUserID);

    if (!pstUser)
    {
        GosLog(LOG_ERROR, "unknown user(%u) in msg %s", Json.GetText());
        return;
    }

    TRAIN_INFO_T    *pstTrain = Train::GetTrainInfo(pstUser->ulTrainUnitID);

    NewEmergencyCall(pstTrain->i64DefaultGroupID, INVALID_USER_ID);

    GJsonParam      Param;

    Param.Add("UserID", ulUserID);

    SendServer(EV_DC_DIS_APP_REQ, MSG_APP_EmergencyCallRsp, Param);
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::FormShow(TObject *Sender)
{
    gbMain->Style->Font->Size = UICfg::GetGroupBoxFontSize();
    gbMain->Style->Font->Name = "宋体";

    gbCallInfo->Style->Font->Size = UICfg::GetGroupBoxFontSize();
    gbCallInfo->Style->Font->Name = "宋体";
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::btnCallClick(TObject *Sender)
{
    TcxTreeList     *tl = tlCallIn;

    if (tl->SelectionCount == 0)
    {
        InfoMsg("未选定记录");
        return;
    }

    TcxTreeListNode *Node = tl->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    UINT32  ulCallType = (UINT32)Node->Values[1];
    INT64   i64GroupID = INVALID_GROUP_ID;
    //UINT32    ulUserID = INVALID_USER_ID;

    if (ulCallType == CALL_TYPE_EMERGENCY_CALL)
    {
        i64GroupID = (INT64)Node->Values[2];

        ShowGroupCallWin(i64GroupID);
    }
    else if (ulCallType == CALL_TYPE_P2P ||
             ulCallType == CALL_TYPE_P2P_TRANSFER )
    {
        //ulUserID = (UINT32)Node->Values[2];
    }
    else if (ulCallType == CALL_TYPE_VIDEO_P2P ||
             ulCallType == CALL_TYPE_VIDEO_RECV )
    {
        //ulUserID = (UINT32)Node->Values[2];
    }
    else if (ulCallType == CALL_TYPE_REQ_CALL)
    {
        INT64   i64DynamicGroupID = INVALID_GROUP_ID;

        if (Node->Values[4] == CALL_STATE_ACCEPT &&
            !Node->Values[12].IsNull())
        {
            i64DynamicGroupID = (INT64)Node->Values[12];
            ShowGroupCallWin(i64DynamicGroupID, "请呼");
            return;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::tlCallInFocusedNodeChanged(TcxCustomTreeList *Sender, TcxTreeListNode *APrevFocusedNode,
          TcxTreeListNode *AFocusedNode)
{
    tlCallInDataChanged(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::tlCallInDataChanged(TObject *Sender)
{
    if (!Visible || !btnAccept)
    {
        return;
    }

    TcxTreeList     *tl = (TcxTreeList*)Sender;

    if (tl->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tl->Selections[0];

    if (!Node || Node->Values[4].IsNull())
    {
        return;
    }

    UINT32  ulCallType = (UINT32)Node->Values[1];
    UINT32  ulCallState = (UINT32)Node->Values[4];

    if (ulCallType == TRAIN_REGISTER)
    {
        return;
    }
    else if (ulCallType == TRAIN_SET_TRAINID)
    {
        return;
    }
    else if (ulCallType == CALL_TYPE_EMERGENCY_CALL)
    {
        if (ulCallState == CALL_STATE_NOT_CONFIRMED)
        {
            btnConfirm->Enabled = true;
        }
        else if (ulCallState == CALL_STATE_ACCEPT)
        {
            btnCall->Enabled = true;
        }
        else if (ulCallState == CALL_STATE_CLOSED)
        {
            btnCall->Enabled = true;
        }
    }
    else if (ulCallType == CALL_TYPE_IPH)
    {
        if (ulCallState == CALL_STATE_WAIT)
        {
            btnAccept->Enabled = true;
            btnConfirm->Enabled = true;
        }
        else if (ulCallState == CALL_STATE_ACCEPT)
        {
            btnHangup->Enabled = true;
        }
        else if (ulCallState == CALL_STATE_CONFIRMED)
        {
            btnAccept->Enabled = true;
        }

        FormMain->ActivePage(PAGE_IPH);
    }
    else if (ulCallType == CALL_TYPE_P2P ||
             ulCallType == CALL_TYPE_P2P_TRANSFER)
    {
        btnAccept->Enabled = true;
        btnHangup->Enabled = true;
        FormMain->ActivePage(PAGE_USER);

        if (ulCallState == CALL_STATE_CLOSED)
        {
            btnCall->Enabled = true;
        }
    }
    else if (ulCallType == CALL_TYPE_VIDEO_P2P  ||
             ulCallType == CALL_TYPE_VIDEO_RECV )
    {
        btnAccept->Enabled = true;
        btnHangup->Enabled = true;
        FormMain->ActivePage(PAGE_VIDEO);
    }
    else if (ulCallType == CALL_TYPE_VIDEO_PTT)
    {
        FormMain->ActivePage(PAGE_VIDEO);
    }
    else if (ulCallType == CALL_TYPE_REQ_CALL)
    {
        if (ulCallState == CALL_STATE_WAIT)
        {
            btnAccept->Enabled = true;
            btnConfirm->Enabled = true;
        }
        else if (ulCallState == CALL_STATE_CONFIRMED)
        {
            btnAccept->Enabled = true;
        }
        else if (ulCallState == CALL_STATE_ACCEPT)
        {
            btnHangup->Enabled = true;
            btnCall->Enabled = true;
        }
        else if (ulCallState == CALL_STATE_CLOSED)
        {
            btnCall->Enabled = true;
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormCallIn::UpdateGroupCallStatus(EVENT_GROUP_STATUS_T &stEvent)
{
    TcxTreeListNode     *Node = NULL;

    if (stEvent.i64GroupID == GetCurrBrdGroupID())
    {
        return;
    }

    for (int i=0; i<tlCallInfo->AbsoluteCount; i++)
    {
        if ((UINT32)tlCallInfo->AbsoluteItems[i]->Values[1] == CALL_TYPE_PTT ||
            (UINT32)tlCallInfo->AbsoluteItems[i]->Values[1] == CALL_TYPE_EMERGENCY_CALL)
        {
            if ((INT64)tlCallInfo->AbsoluteItems[i]->Values[2] == stEvent.i64GroupID )
            {
                Node = tlCallInfo->AbsoluteItems[i];
                break;
            }
        }
    }

    if (stEvent.ulGroupCallStatus == PTT_IND_STATUS_SNATCH ||   // 组呼建立成功
        stEvent.ulGroupCallStatus == PTT_IND_STATUS_TX_BEGIN )  // 话权变更
    {
        if (!Node)
        {
            Node = tlCallInfo->Add();
            Node->Values[0] = gos_get_current_time();
            Node->Values[1] = CALL_TYPE_PTT;
            Node->Values[2] = stEvent.i64GroupID;
            Node->Values[3] = stEvent.ulSpeaker;
            Node->Values[4] = CALL_STATE_TALKING;
            return;
        }

        Node->Values[3] = stEvent.ulSpeaker;
        Node->Values[4] = CALL_STATE_TALKING;
        if (stEvent.ulGroupCallStatus == PTT_IND_STATUS_SNATCH)
        {
            Node->Values[0] = gos_get_current_time();
        }
    }
    else if (stEvent.ulGroupCallStatus == PTT_IND_STATUS_TX_IDLE)
    {
        if (Node)
        {
            Node->Values[3] = 0;
            Node->Values[4] = CALL_STATE_IDLE;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::StopGroupCall(INT64 i64GroupID)
{
    TcxTreeList     *tl = tlCallIn;

    for (INT32 i=0; i<tl->AbsoluteCount; i++)
    {
        if ((UINT32)tl->AbsoluteItems[i]->Values[1] == CALL_TYPE_EMERGENCY_CALL &&
            (INT64)tl->AbsoluteItems[i]->Values[2] == i64GroupID)
        {
            if (g_bAutoHideClosed)
            {
                tl->AbsoluteItems[i]->Delete();
            }
            else
            {
                tl->AbsoluteItems[i]->Values[4] = CALL_STATE_CLOSED;
            }

            return;
        }

        if ((UINT32)tl->AbsoluteItems[i]->Values[1] == CALL_TYPE_REQ_CALL &&
            !tl->AbsoluteItems[i]->Values[12].IsNull() &&
            (INT64)tl->AbsoluteItems[i]->Values[12] == i64GroupID )
        {
            if (g_bAutoHideClosed)
            {
                tl->AbsoluteItems[i]->Delete();
            }
            else
            {
                tl->AbsoluteItems[i]->Values[4] = CALL_STATE_CLOSED;
            }

            return;
        }
    }

    TcxTreeListNode     *Node = NULL;

    for (int i=0; i<tlCallInfo->AbsoluteCount; i++)
    {
        Node = tlCallInfo->AbsoluteItems[i];
        if ((UINT32)Node->Values[1] == CALL_TYPE_PTT ||
            (UINT32)Node->Values[1] == CALL_TYPE_EMERGENCY_CALL)
        {
            if ((INT64)Node->Values[2] == i64GroupID &&
                (UINT32)Node->Values[4] != CALL_STATE_CLOSED )
            {
                Node->Values[3] = 0;
                Node->Values[4] = CALL_STATE_CLOSED;
                break;
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::StopP2PCall(UINT32 ulUserID)
{
    for (int i=0; i<tlCallIn->AbsoluteCount; i++)
    {
        if (((UINT32)tlCallIn->AbsoluteItems[i]->Values[1] == CALL_TYPE_P2P ||
             (UINT32)tlCallIn->AbsoluteItems[i]->Values[1] == CALL_TYPE_P2P_EMERGENCY_CALL ||
             (UINT32)tlCallIn->AbsoluteItems[i]->Values[1] == CALL_TYPE_HALF_DUPLEX_P2P_CALL ) &&
            (UINT32)tlCallIn->AbsoluteItems[i]->Values[2] == ulUserID)
        {
            if (g_bAutoHideClosed)
            {
                tlCallIn->AbsoluteItems[i]->Delete();
            }
            else
            {
                tlCallIn->AbsoluteItems[i]->Values[4] = CALL_STATE_CLOSED;
            }

            break;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::StopP2PVideoCall(UINT32 ulUserID)
{
    TcxTreeList     *tl = tlCallIn;

    for (INT32 i=0; i<tl->AbsoluteCount; i++)
    {
        if ((UINT32)tl->AbsoluteItems[i]->Values[1] == CALL_TYPE_VIDEO_P2P ||
            (UINT32)tl->AbsoluteItems[i]->Values[1] == CALL_TYPE_VIDEO_RECV )
        {
            if ((UINT32)tl->AbsoluteItems[i]->Values[2] == ulUserID)
            {
                if (g_bAutoHideClosed)
                {
                    tl->AbsoluteItems[i]->Delete();
                }
                else
                {
                    tl->AbsoluteItems[i]->Values[4] = CALL_STATE_CLOSED;
                }

                break;
            }
        }
    }
}
//---------------------------------------------------------------------------
/**
 * @brief           判断请呼列表中有没有超过200条记录如果超过200条，则从头删除至100条
 *                  判断每一条记录是否是一天前生成的，如果是则直接删除记录
 * @param           Sender    [in]
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
void __fastcall TFormCallIn::TimerCheckCallTimer(TObject *Sender)
{
    TcxTreeList     *tl = tlCallIn;

    if (tl->AbsoluteCount >= 200)
    {
        tl->BeginUpdate();
        while(tl->AbsoluteCount > 100)
        {
            tl->AbsoluteItems[0]->Delete();
        }

        tl->EndUpdate();
    }

    if (tlCallIn->AbsoluteCount == 0 )
    {
        g_ulVoiceAlarm = VOICE_ALARM_NULL;
        StopVoiceAlarm();
    }

    UINT32  ulTime = gos_get_current_time() - 24*3600;

    for (int i=tlCallInfo->AbsoluteCount-1; i>=0; i--)
    {
        if ((UINT32)tlCallInfo->AbsoluteItems[i]->Values[0] < ulTime)
        {
            tlCallInfo->AbsoluteItems[i]->Delete();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::AutoSelectNode(TcxTreeListNode *Node)
{
    if (Node->TreeList->SelectionCount == 0)
    {
        Node->Selected = true;
        Node->MakeVisible();
    }
}
void __fastcall TFormCallIn::ColumnTrainOrStationGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    // 界面不显示
/*    if (ANode->Values[3].IsNull() || ANode->Values[14].IsNull())
    {
        return;
    }

    UINT32  ulUEType = (UINT32)ANode->Values[14];
    UINT32  ulID = (UINT32)ANode->Values[3];

    if (ulUEType == TRAIN_UE)
    {
        TRAIN_INFO_T    *pstTrain = Train::GetTrainInfo(ulID);

        if (pstTrain)
        {
            Value = pstTrain->acTrainID;
        }
    }
    else if (ulUEType == STATION_UE)
    {
        STATION_INFO_T    *pstStation = Station::GetStationInfo(ulID);

        if (pstStation)
        {
            Value = pstStation->acStationName;
        }
    }   */
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::ColumnDCUserGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[11].IsNull())
    {
        return;
    }

    UINT32  ulValue = ANode->Values[11];

    if (ulValue == INVALID_USER_ID)
    {
        Value = "";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::ColumnCallerGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[1].IsNull() ||
        ANode->Values[2].IsNull() )
    {
        return;
    }

    UINT32  ulCallType = ANode->Values[1];
    CHAR    acTrainInfo[256];

    if (ulCallType == TRAIN_REGISTER ||
        ulCallType == TRAIN_SET_TRAINID ||
        ulCallType == CALL_TYPE_IPH )
    {
        UINT32          ulTrainUnitID = ANode->Values[2];
        TRAIN_INFO_T    *pstTrain = Train::GetTrainInfo(ulTrainUnitID);

        if (pstTrain)
        {
            Value = Train::GetTrainAlias(ulTrainUnitID, pstTrain->acTrainID);
        }
    }
    else if (ulCallType == CALL_TYPE_EMERGENCY_CALL)
    {
        INT64       i64GroupID = ANode->Values[2];

        Value = Group::GetGroupDisplayName(i64GroupID);
    }
    else if (ulCallType == CALL_TYPE_P2P ||
             ulCallType == CALL_TYPE_VIDEO_P2P ||
             ulCallType == CALL_TYPE_VIDEO_RECV )
    {
        UINT32      ulUserID = ANode->Values[2];

        Value = User::GetUserDisplayName(ulUserID);
    }
    else if (ulCallType == CALL_TYPE_REQ_CALL)
    {
        if (ANode->Values[5].IsNull() ||
            ANode->Values[3].IsNull() )
        {
            return;
        }

        UINT32      ulUserID = ANode->Values[2];
        UINT32      ulReqCallType = ANode->Values[5];

        if (ulReqCallType == TRAIN_REQ_CALL_DC ||
            ulReqCallType == TRAIN_EMERGENCY_REQ_CALL_DC ||
            ulReqCallType == TRAIN_REQ_CALL_STATION ||
            ulReqCallType == TRAIN_REQ_CALL_FRONT_TRAIN ||
            ulReqCallType == TRAIN_REQ_CALL_BACK_TRAIN )
        {
            UINT32          ulTrainUnitID = ANode->Values[3];
            TRAIN_INFO_T    *pstTrain = Train::GetTrainInfo(ulTrainUnitID);

            if (pstTrain)
            {
                Value = Train::GetTrainAlias(ulTrainUnitID, pstTrain->acTrainID);
            }
        }
        else if (ulReqCallType == STATION_REQ_CALL_DC ||
                 ulReqCallType == STATION_EMERGENCY_REQ_CALL_DC ||
                 ulReqCallType == STATION_REQ_CALL_TRAIN )
        {
            UINT32  ulStationID = ANode->Values[3];

            Value = Station::GetStationName(ulStationID);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::MainPanelResize(TObject *Sender)
{
    int     iHeight = MainPanel->Height;
    int     iTopHeight = iHeight*50/100;

    sbMain->Height = iTopHeight;
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::pmiAcceptClick(TObject *Sender)
{
    btnAcceptClick(btnAccept);
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::pmiConfirmClick(TObject *Sender)
{
    btnConfirmClick(btnConfirm);
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::pmiHangupClick(TObject *Sender)
{
    btnHangupClick(btnHangup);
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::pmiCallTransferClick(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::btnJoinGroupClick(TObject *Sender)
{
    if (tlCallInfo->SelectionCount == 0)
    {
        InfoMsg("未选定记录");
        return;
    }

    TcxTreeListNode *Node = tlCallInfo->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    INT64   i64GroupID = (INT64)Node->Values[2];

    ShowGroupCallWin(i64GroupID);
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::btnExitGroupClick(TObject *Sender)
{
    if (tlCallInfo->SelectionCount == 0)
    {
        InfoMsg("未选定记录");
        return;
    }

    TcxTreeListNode *Node = tlCallInfo->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    INT64   i64GroupID = (INT64)Node->Values[2];

    Group::AutoExitGroup(i64GroupID);

    Node->Delete();
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::tlCallInfoColumn5GetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[2].IsNull())
    {
        return;
    }

    INT64   i64GroupID = (INT64)ANode->Values[2];

    Value = Group::GetGroupDisplayName(i64GroupID);
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::ColumnCurrSpeakerGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[3].IsNull())
    {
        return;
    }

    UINT32  ulSpeaker = (UINT32)ANode->Values[3];

    if (ulSpeaker == 0)
    {
        Value = "";
    }

    Value = User::GetUserAlias(ulSpeaker);
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::tlCallInfoDblClick(TObject *Sender)
{
    btnJoinGroupClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::tlCallInfoCustomDrawDataCell(TcxCustomTreeList *Sender,
          TcxCanvas *ACanvas, TcxTreeListEditCellViewInfo *AViewInfo,
          bool &ADone)
{
    if (!AViewInfo->Node ||
        AViewInfo->Node->Values[2].IsNull() )
    {
        return;
    }

    INT64   i64GroupID = (INT64)AViewInfo->Node->Values[2];

    if (i64GroupID == g_i64MainListenGroupID)
    {
        ACanvas->Canvas->Brush->Color = g_CallColorGreen;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::btnListenClick(TObject *Sender)
{
    if (tlCallInfo->SelectionCount == 0)
    {
        return;
    }

    INT64       i64GroupID = (INT64)tlCallInfo->Selections[0]->Values[2];

    if (g_i64MainListenGroupID == i64GroupID)
    {
        StopMainListen();
    }
    else
    {
        StartMainListen(i64GroupID);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::pmiBreakoffClick(TObject *Sender)
{
    if (tlCallInfo->SelectionCount == 0)
    {
        InfoMsg("未选定记录");
        return;
    }

    TcxTreeListNode *Node = tlCallInfo->Selections[0];

    if (Node->Values[2].IsNull())
    {
        return;
    }

    INT64   i64GroupID = (INT64)Node->Values[2];

    eSDK_GroupBreakoff(i64GroupID);
}
//---------------------------------------------------------------------------
void __fastcall TFormCallIn::InsertCallInInfoList(UINT32 ulUserID, UINT32 ulCallerID,
                                                  UINT32 ulReqCallType, UINT32 ulUEType,
                                                  UINT32 ulReqTime, UINT32 ulDefaultGroupID,
                                                  AnsiString strGPSInfo)
{
    TcxTreeListNode *Node = tlCallIn->Add();

    Node->Values[0] = gos_get_current_time();
    Node->Values[1] = CALL_TYPE_REQ_CALL;
    Node->Values[2] = ulUserID;
    Node->Values[3] = ulCallerID;
    Node->Values[4] = CALL_STATE_WAIT;
    Node->Values[5] = ulReqCallType;
    Node->Values[6] = GetSortOrder(Node);
    Node->Values[9] = 0;
    Node->Values[10] = -1;
    Node->Values[11] = INVALID_USER_ID;
    Node->Values[12] = INVALID_GROUP_ID;
    Node->Values[14] = ulUEType;
    Node->Values[15] = ulReqTime;
    Node->Values[16] = strGPSInfo;

    if (ulDefaultGroupID != INVALID_GROUP_ID)
    {
        Node->Values[12] = ulDefaultGroupID;
    }

    AutoSelectNode(Node);
}

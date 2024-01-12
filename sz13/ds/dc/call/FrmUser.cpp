//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmUser.h"
#include "main.h"
#include "FrmCDR.h"
#include "FrmCallIn.h"
#include "DlgSubUser.h"
#include "FrmLocalRecManage.h"
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
#pragma link "cxStyles"
#pragma link "cxTextEdit"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "cxTrackBar"
#pragma link "dxSkinBlue"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "MDIForm"
#pragma link "dxBar"
#pragma link "cxClasses"
#pragma link "cxImageList"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxImageComboBox"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma resource "*.dfm"
TFormUser *FormUser;

extern void ShowHalfDuplexP2PCallWin(UINT32 ulUserID);

//---------------------------------------------------------------------------
__fastcall TFormUser::TFormUser(TComponent* Owner)
    : TSubForm(Owner)
{
    m_ulCurrListenUserID = INVALID_USER_ID;
    m_ulCurrListenUserTime = 0;

    TimerGPS->Enabled = AllowGISSub();

    InitWorkPanel();
    InitUser();

    if (!AllowHalfDuplexP2PCall())
    {
        rbFullDuplex->Checked = true;

        rbHalfDuplex->Visible = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::InitWorkPanel()
{
    TcxButton   *btns[] = {
                            btnP2PCall,
                            btnHangup,
                            btnP2PTransfer,
                            btnStartListen,
                            btnStartEnvListen,
                            btnStopListen,
                            btnStopEnvListen,
                          };

    gbWorkPanel->Width = 150;

    for (UINT32 i=0; i<ARRAY_SIZE(btns); i++)
    {
        btns[i]->TabOrder = i;
        btns[i]->Top = 240+95*i-95*2;
        btns[i]->Width = g_iMidToolbarButtonWidth;
        btns[i]->Height = g_iMidToolbarButtonHeight;
        btns[i]->Left = (btns[i]->Parent->Width - btns[i]->Width)/2;
    }

    btnStopListen->Top = btnStartListen->Top;
    btnStopListen->Visible = false;

    btnStopEnvListen->Top = btnStartEnvListen->Top;
    btnStopEnvListen->Visible = false;
}

void __fastcall TFormUser::InitSkin()
{
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::UpdateGISInfo(EVENT_GIS_REPORT_T &stEvent)
{
    AnsiString  strInfo = GIS::GetGISText(stEvent.ulUserID);

    if (strInfo.IsEmpty())
    {
        return;
    }

    for (int i=0; i<tlMain->AbsoluteCount; i++)
    {
        TcxTreeListNode     *Node = tlMain->AbsoluteItems[i];

        if (!Node->Values[2].IsNull())
        {
            UINT32  ulUserID = Node->Values[0];

            if (ulUserID == stEvent.ulUserID)
            {
                Node->Values[7] = strInfo;
                return;
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::InitUser()
{
    m_mUserNode.clear();

    tlMain->BeginUpdate();
    tlMain->Clear();

    TcxTreeListNode     *Node;
    TcxTreeListNode     *SubNode;
    TcxTreeListNode     *UserNode;
    UINT32              i;
    UINT32              ulUserType;
    UINT32              ulFuncType;
    UINT32              ulDevStatus;
    UINT32              ulCellID;
    UINT32              ulCallStatus;
    CHAR                acUserName[64];

    // 车站终端，列车终端，维护终端，管理终端、其他
    Node = tlMain->Add();
    Node->Values[0] = "车站终端";

    VECTOR<STATION_UE_INFO_T*>  &vStationUE = StationUE::GetStationUE();

    for (i=0; i<vStationUE.size(); i++)
    {
        STATION_UE_INFO_T   *pstInfo = vStationUE[i];

        SubNode = Node->AddChild();
        SubNode->Values[0] = pstInfo->ulUserID;
        SubNode->Values[2] = pstInfo->ulDevStatus;
        SubNode->Values[3] = STATION_UE;
    }

    VECTOR<PORTAL_UE_INFO_T*>   &vPortalUE = PortalUE::GetPortalUE();

    for (i=0; i<vPortalUE.size(); i++)
    {
        PORTAL_UE_INFO_T    *pstInfo = vPortalUE[i];

        if (pstInfo->ulFuncType == UE_STATION )
        {
            SubNode = Node->AddChild();
            SubNode->Values[0] = pstInfo->ulUserID;
            SubNode->Values[2] = pstInfo->ulDevStatus;
            SubNode->Values[3] = pstInfo->ulType;
            SubNode->Values[7] = GIS::GetGISText(pstInfo->dLon, pstInfo->dLat);
        }
    }

    Node = tlMain->Add();
    Node->Values[0] = "列车终端";

    VECTOR<TRAIN_UE_INFO_T*>    &vTrainUE = TrainUE::GetTrainUE();

    for (i=0; i<vTrainUE.size(); i++)
    {
        TRAIN_UE_INFO_T     *pstInfo = vTrainUE[i];
        TRAIN_INFO_T        *pstTrain = Train::GetTrainInfo(pstInfo->ulTrainUnitID);

        SubNode = Node->AddChild();
        SubNode->Values[0] = pstInfo->ulUserID;
        SubNode->Values[2] = pstInfo->ulDevStatus;
        SubNode->Values[3] = TRAIN_UE;

        if (pstTrain)
        {
            SubNode->Values[7] = GIS::GetGISText(pstTrain->dLon, pstTrain->dLat);
        }

    }

    Node = tlMain->Add();
    Node->Values[0] = "维护终端";
    for (i=0; i<vPortalUE.size(); i++)
    {
        PORTAL_UE_INFO_T    *pstInfo = vPortalUE[i];

        if (pstInfo->ulFuncType == UE_FIX)
        {
            SubNode = Node->AddChild();
            SubNode->Values[0] = pstInfo->ulUserID;
            SubNode->Values[2] = pstInfo->ulDevStatus;
            SubNode->Values[3] = pstInfo->ulType;
            SubNode->Values[7] = GIS::GetGISText(pstInfo->dLon, pstInfo->dLat);
        }
    }

    Node = tlMain->Add();
    Node->Values[0] = "管理终端";

    VECTOR<DC_USER_INFO_T*>  vDCUser;

    DCUser::GetDCUser(vDCUser);

    for (UINT32 i=0; i<vDCUser.size(); i++)
    {
        DC_USER_INFO_T  *pstInfo = vDCUser[i];

        if (APP::IsCurrDCUserID(pstInfo->ulUserID))
        {
            continue;
        }

        SubNode = Node->AddChild();
        SubNode->Values[0] = pstInfo->ulUserID;
        SubNode->Values[2] = pstInfo->ulDevStatus;
        SubNode->Values[3] = DC_USER;
    }

    for (i=0; i<vPortalUE.size(); i++)
    {
        PORTAL_UE_INFO_T    *pstInfo = vPortalUE[i];

        if (pstInfo->ulFuncType == UE_MANAGE)
        {
            SubNode = Node->AddChild();
            SubNode->Values[0] = pstInfo->ulUserID;
            SubNode->Values[2] = pstInfo->ulDevStatus;
            SubNode->Values[3] = pstInfo->ulType;
            SubNode->Values[7] = GIS::GetGISText(pstInfo->dLon, pstInfo->dLat);
        }
    }

    Node = tlMain->Add();
    Node->Values[0] = "其他";
    for (i=0; i<vPortalUE.size(); i++)
    {
        PORTAL_UE_INFO_T    *pstInfo = vPortalUE[i];

        if (pstInfo->ulFuncType != UE_FIX &&
            pstInfo->ulFuncType != UE_MANAGE &&
            pstInfo->ulFuncType != UE_STATION)
        {
            SubNode = Node->AddChild();
            SubNode->Values[0] = pstInfo->ulUserID;
            SubNode->Values[2] = pstInfo->ulDevStatus;
            SubNode->Values[3] = pstInfo->ulType;
            SubNode->Values[7] = GIS::GetGISText(pstInfo->dLon, pstInfo->dLat);
        }
    }

    for (int i=tlMain->AbsoluteCount-1; i>=0; i--)
    {
        Node = tlMain->AbsoluteItems[i];
        if (!Node->Values[2].IsNull())
        {
            UINT32  ulUserID = Node->Values[0];

            if (!pmiShowAllUser->Down &&
                !g_sSubUser.Has(ulUserID))
            {
                Node->Delete();
            }
            else
            {
                if (User::GetUserInfo(ulUserID, acUserName, &ulUserType, &ulFuncType, &ulDevStatus, &ulCellID, &ulCallStatus))
                {
                    Node->Values[1] = acUserName;
                    Node->Values[2] = ulUserType;
                    Node->Values[3] = ulFuncType;
                    Node->Values[4] = ulDevStatus;
                    Node->Values[5] = ulCellID;
                    Node->Values[6] = ulCallStatus;
                }

                m_mUserNode[ulUserID] = Node;
            }
        }
    }

    //TimerGPSTimer(this);

    tlMain->Root->Expand(true);
    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::UpdateP2PCallStatus(EVENT_P2PCALL_STATUS_T &stEvent)
{
    UINT32  ulStatus = 0xffffffff;

    if (stEvent.ulCallee == INVALID_USER_ID)
    {
        return;
    }

    GosLog(LOG_DETAIL, "call status %u, caller is %u, callee is %u",
              stEvent.ulP2PCallStatus, stEvent.ulCaller, stEvent.ulCallee);

    if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_ANSWERED )
    {
        ulStatus = CALL_STATUS_RECV_SUCC;
        LocalRec::StartP2PCallRecord(stEvent.ulCaller, REC_TYPE_AUDIO, REC_CALL_TYPE_P2P);
    }
    else if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_PICKEDUP)        // 指示被叫端已接听
    {
        ulStatus = CALL_STATUS_RECV_SUCC;
        LocalRec::StartP2PCallRecord(stEvent.ulCaller, REC_TYPE_AUDIO, REC_CALL_TYPE_P2P);
    }
    else if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_HANGUPED_ACTIVE) // 指示已主动挂断
    {
        ulStatus = CALL_STATUS_HANGUP;
    }
    else if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_CANCELLED) // 指示点呼请求已取消(调度台不接而超时中止）
    {
        ulStatus = CALL_STATUS_CLOSE;
    }
    else if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_RELEASED)    // 指示对端拒绝接听(调度台主叫）
    {
        ulStatus = CALL_STATUS_REJECT;
    }
    else if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_REMOTE_NOANSWER) // 对端无响应
    {
        ulStatus = CALL_STATUS_NOANSWER;
    }
    else if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_RELEASED_BUSY)   // 指示对端忙
    {
        ulStatus = CALL_STATUS_BUSY;
    }
    else if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_HANGUPED)    // 指示对端已挂断
    {
        ulStatus = CALL_STATUS_HANGUP;
    }
    else if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_FC_REJECTED) // 指示对端拒绝
    {
        ulStatus = CALL_STATUS_REJECT;
    }
    else
    {
        return;
    }

    SaveP2PCallInfo(CALL_TYPE_P2P, stEvent.ulCaller, stEvent.ulCallee, ulStatus);

    if (P2P::IsP2PCallOver(ulStatus))
    {
        P2P::CallOver();
        CallOver();

        LocalRec::StopP2PCallRecord(stEvent.ulCaller);

        FormCallIn->StopP2PCall(stEvent.ulCaller);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::pmiPrivateCallClick(TObject *Sender)
{
/*    if (P2P::IsCalling())
    {
        InfoMsg("正在呼叫中");
        return;
    }        */

    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlMain->Selections[0];

    if (Node->Values[2].IsNull())
    {
        return;
    }

    StopListen();

    edtCalleeNumber->Text = Node->Values[0];
    btnDialClick(this);

}
//---------------------------------------------------------------------------
void __fastcall TFormUser::tlMainGetNodeImageIndex(TcxCustomTreeList *Sender,
          TcxTreeListNode *ANode, TcxTreeListImageIndexType AIndexType,
          TImageIndex &AIndex)
{
    if (ANode->Values[0].IsNull() ||
        ANode->Values[2].IsNull() )
    {
        AIndex = 0;
        return;
    }

    BOOL    bOnline = User::IsUserOnline((UINT32)ANode->Values[0]);
    UINT32  ulType = (UINT32)ANode->Values[2];

    if (ulType == LARGE_PORTAL_UE )
    {
        AIndex = bOnline?1:2;
    }
    else if (ulType == SMALL_PORTAL_UE )
    {
        AIndex = bOnline?3:4;
    }
    else if (ulType == STATION_UE)
    {
        AIndex = bOnline?5:6;
    }
    else if (ulType == TRAIN_UE)
    {
        AIndex = bOnline?7:8;
    }
    else if (ulType == DC_USER)
    {
        AIndex = bOnline?9:10;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::btnDialClick(TObject *Sender)
{
    AnsiString  strNumber = edtCalleeNumber->Text.Trim();
    bool        bFullDuplex = rbFullDuplex->Checked;
    int         iUserID = strNumber.ToIntDef(0);

    if (iUserID <= 0)
    {
        InfoMsg("号码非法");
        return;
    }

    if (!IsMDCValid())
    {
        ErrMsg("呼叫失败：调度台已断开与无线集群服务器的连接");
        return;
    }

    if (bFullDuplex)
    {
        Dial(iUserID);
    }
    else
    {
        ShowHalfDuplexP2PCallWin(iUserID);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::Dial(UINT32 ulUserID)
{
    if (P2P::IsCalling())
    {
        InfoMsg("正在通话中");
        return;
    }

    StopListen();

    if (!P2P::Dial(ulUserID))
    {
        InfoMsg("呼出失败");
        return;
    }

    btnDial->Enabled = false;
    btnDialHangup->Enabled = true;
    btnTransfer->Enabled = true;

    edtCalleeNumber->Enabled = false;
    rbFullDuplex->Enabled = false;
    rbHalfDuplex->Enabled = false;

//    eSDK_VolumeControl(ulUserID, tbP2PDialVolume->Position);
    P2P::SetVolume(tbP2PDialVolume->Position);
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::CallOver()
{
    btnDial->Enabled = true;
    btnDialHangup->Enabled = false;
    btnTransfer->Enabled = false;

    edtCalleeNumber->Enabled = true;
    rbFullDuplex->Enabled = true;
    rbHalfDuplex->Enabled = true;
//    P2P::SetSpeakingStatus(false);
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::btnDialHangupClick(TObject *Sender)
{
    P2P::Hangup();
    CallOver();
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::UpdateUser(EVENT_USER_STATUS_T &stEvent)
{
    if (m_mUserNode.find(stEvent.ulUserNumber) == m_mUserNode.end())
    {
        return;
    }

    TcxTreeListNode *Node = m_mUserNode[stEvent.ulUserNumber];

    Node->Values[6] = stEvent.ulStatus;
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::tbP2PDialVolumePropertiesChange(TObject *Sender)
{
    /// eSDK_VolumeControl(m_ulCallee, tbP2PDialVolume->Position);
    P2P::SetVolume(tbP2PDialVolume->Position);
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::FormCreate(TObject *Sender)
{
    SetImage(pmiPrivateCall, IMG_P2P_CALL);
    SetImage(pmiHangup, IMG_HANGUP);
    SetImage(pmiSubUser, IMG_SET);

    SetImage(pmiStartListen, IMG_START_LISTEN);
    SetImage(pmiStopListen, IMG_STOP_LISTEN);
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::pmiStartListenClick(TObject *Sender)
{
    if (P2P::IsCalling())
    {
        InfoMsg("正在呼叫中");
        return;
    }

    if (m_ulCurrListenUserID != INVALID_USER_ID)
    {
        InfoMsg("正在侦听中");
        return;
    }

    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlMain->Selections[0];

    if (Node->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulUserID = (UINT32)Node->Values[0];

    if (!eSDK_StartUserDiscreetListen(ulUserID))
    {
        InfoMsg("监听失败");
        return;
    }

    m_ulCurrListenUserID = ulUserID;
    m_ulCurrListenUserTime = gos_get_uptime();

    pmiStartListen->Visible = ivNever;
    pmiStopListen->Visible = ivAlways;
    pmiStopListen->Enabled = true;

    pmiStartEnvListen->Visible = ivAlways;
    pmiStopEnvListen->Visible = ivNever;
    pmiStartEnvListen->Enabled = false;

    btnStartListen->Visible = false;
    btnStopListen->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::pmiStartEnvListenClick(TObject *Sender)
{
    if (P2P::IsCalling())
    {
        InfoMsg("正在呼叫中");
        return;
    }

    if (m_ulCurrListenUserID != INVALID_USER_ID)
    {
        InfoMsg("正在侦听中");
        return;
    }

    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlMain->Selections[0];

    if (Node->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulUserID = (UINT32)Node->Values[0];

    if (!eSDK_StartEnvListen(ulUserID))
    {
        InfoMsg("侦听失败");
        return;
    }

    m_ulCurrListenUserID = ulUserID;
    m_ulCurrListenUserTime = gos_get_uptime();

    pmiStartListen->Visible = ivAlways;
    pmiStopListen->Visible = ivNever;
    pmiStartListen->Enabled = false;

    pmiStartEnvListen->Visible = ivNever;
    pmiStopEnvListen->Visible = ivAlways;
    pmiStopEnvListen->Enabled = true;

    btnStartEnvListen->Visible = false;
    btnStopEnvListen->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::pmiStopListenClick(TObject *Sender)
{
    StopListen();
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::pmiStopEnvListenClick(TObject *Sender)
{
    StopListen();
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::TimerCheckTimer(TObject *Sender)
{
    gbCallPanel->Visible = IsShowCallWin();

    if (!P2P::IsCalling())
    {
        btnP2PCall->Enabled = true;
        btnHangup->Enabled = true;
        //btnHangup->Enabled = false;
        btnP2PTransfer->Enabled = false;
    }
    else
    {
        btnP2PCall->Enabled = true;
        //btnP2PCall->Enabled = false;
        btnHangup->Enabled = true;
        btnP2PTransfer->Enabled = true;
    }

    if (m_ulCurrListenUserID != INVALID_USER_ID)
    {
        UINT32      ulTime = gos_get_uptime() - m_ulCurrListenUserTime;

        if (ulTime > GetMaxListenPeriod())
        {
            StopListen();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::StopListen()
{
    if (m_ulCurrListenUserID == INVALID_USER_ID)
    {
        return;
    }

    eSDK_StopUserDiscreetListen(m_ulCurrListenUserID);
    eSDK_StopEnvListen(m_ulCurrListenUserID);

    m_ulCurrListenUserID = INVALID_USER_ID;

    pmiStartListen->Visible = ivAlways;
    pmiStopListen->Visible = ivNever;
    pmiStartListen->Enabled = true;

    pmiStartEnvListen->Visible = ivAlways;
    pmiStopEnvListen->Visible = ivNever;
    pmiStartEnvListen->Enabled = true;

    btnStartListen->Visible = true;
    btnStopListen->Visible = false;

    btnStartEnvListen->Visible = true;
    btnStopEnvListen->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::pmiRefreshClick(TObject *Sender)
{
    if (LoadPortalUEInfo())
    {
        InitUser();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::gbCallPanelResize(TObject *Sender)
{
    gbP2PDial->Left = (gbP2PDial->Parent->Width - gbP2PDial->Width)/2;
    gbP2PDial->Top = (gbP2PDial->Parent->Height - gbP2PDial->Height)/2;
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::btnTransferClick(TObject *Sender)
{
    UINT32 ulObjectUserNumber;

    if (!ToInt(edtDialTransferNumbe->Text, &ulObjectUserNumber))
    {
        LabelDialStatus->Caption = "转接号码非法";
        edtDialTransferNumbe->SetFocus();
        return;
    }

    if (!P2P::CallTransfer(ulObjectUserNumber))
    {
        InfoMsg("转接失败");
        return;
    }

    InfoMsg("转接成功");
    CallOver();
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::pmiBreakInClick(TObject *Sender)
{
    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlMain->Selections[0];

    if (Node->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulUserID = (UINT32)Node->Values[0];

    if (P2P::IsCalling())
    {
        InfoMsg("正在通话中");
        return;
    }

    StopListen();

    if (!P2P::Breakin(ulUserID))
    {
        InfoMsg("强插失败");
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::pmiBreakOffClick(TObject *Sender)
{
    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlMain->Selections[0];

    if (Node->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulUserID = (UINT32)Node->Values[0];

    if (!eSDK_P2PBreakoff(ulUserID))
    {
        InfoMsg("强拆失败");
        return;
    }
}

BOOL SaveSubUser(IntSet &sOld, IntSet &sNew)
{
    GJsonParam          ReqParam;
    GJsonTupleParam     TupleParam;

    for (UINT32 i=0; i<sOld.Size(); i++)
    {
        GJsonParam  SubParam;

        SubParam.Add("UserID", sOld.Get(i));
        TupleParam.Add(SubParam);
    }

    ReqParam.Add("DelUser", TupleParam);

    TupleParam.Clear();
    for (UINT32 i=0; i<sNew.Size(); i++)
    {
        GJsonParam  SubParam;

        SubParam.Add("UserID", sNew.Get(i));
        TupleParam.Add(SubParam);
    }

    ReqParam.Add("AddUser", TupleParam);

    if (!SyncSendServer(EV_DC_DIS_REQ, MSG_DCSaveSubUserInfo, ReqParam))
    {
        return FALSE;
    }

    return TRUE;
}

//---------------------------------------------------------------------------
void __fastcall TFormUser::pmiSubUserClick(TObject *Sender)
{
    DialogSubUser->Init(g_sSubUser);
    if (DialogSubUser->ShowModal() != mrOk)
    {
        return;
    }

    UINT32  ulValue;
    IntSet  sOld;
    IntSet  sNew;
    VECTOR<UINT32>  vUser;

    sOld.Init(g_sSubUser);

    DialogSubUser->Get(sNew);
    sNew.GetList(vUser);

    // 去除相同的部分
    for (UINT32 i=0; i<vUser.size(); i++)
    {
        ulValue = vUser[i];
        if (sOld.Del(ulValue))
        {
            sNew.Del(ulValue);
        }
    }

    if (!SaveSubUser(sOld, sNew))
    {
        InfoMsg("操作失败");
        GosLog(LOG_ERROR, "save sub user failed");
        return;
    }

    InfoMsg("操作成功");

    DialogSubUser->Get(g_sSubUser);

    InitUser();
}

//---------------------------------------------------------------------------
void __fastcall TFormUser::ColumnFuncTypeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[3].IsNull())
    {
        return;
    }

    UINT32  ulType = ANode->Values[3];

    Value = GetUserFuncTypeText(ulType);
}
//---------------------------------------------------------------------------

void __fastcall TFormUser::ColumnTypeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    if (ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulType = ANode->Values[2];

         if (ulType == TRAIN_UE)        Value = "车载台";
    else if (ulType == STATION_UE)      Value = "固定台";
    else if (ulType == SMALL_PORTAL_UE) Value = "手持台(小屏)";
    else if (ulType == LARGE_PORTAL_UE) Value = "手持台(大屏)";
    else if (ulType == DC_USER)         Value = "调度台";
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::ColumnStatusGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[6].IsNull())
    {
        return;
    }

    UINT32  ulStatus = ANode->Values[6];

    if (!User::IsUserOnline(ulStatus))
    {
        Value = "离线";
    }
    else
    {
        Value = "";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::ColumnCallStatusGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[6].IsNull())
    {
        return;
    }

    UINT32  ulStatus = ANode->Values[6];

    Value = eSDKData::GetUserCallStatus(ulStatus);
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::pmiShowAllUserClick(TObject *Sender)
{
    InitUser();
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::pmiHangupClick(TObject *Sender)
{
    btnDialHangupClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::TimerGPSTimer(TObject *Sender)
{
    if (!TimerGPS->Enabled)
    {
        return;
    }

    TimerGPS->Interval = 60*1000;

    VECTOR<UINT32>      vUserID;
    VECTOR<PORTAL_UE_INFO_T*>   &vPortalUE = PortalUE::GetPortalUE();

    for (UINT32 i=0; i<vPortalUE.size(); i++)
    {
        PORTAL_UE_INFO_T    *pstInfo = vPortalUE[i];

        vUserID.push_back(pstInfo->ulUserID);
    }

    eSDK_GISSub(vUserID);
}
//---------------------------------------------------------------------------
void __fastcall TFormUser::UpdateTrainInfo()
{
    UINT32              ulUserID;
    UINT32              ulType;
    TRAIN_UE_INFO_T     *pstInfo;
    TRAIN_INFO_T        *pstTrain;

    tlMain->BeginUpdate();

    for (int i=0; i<tlMain->AbsoluteCount; i++)
    {
        TcxTreeListNode     *Node = tlMain->AbsoluteItems[i];

        if (Node->Values[3].IsNull())
        {
            continue;
        }

        ulType = Node->Values[3];
        if (ulType != TRAIN_UE)
        {
            continue;
        }

        ulUserID = Node->Values[0];
        pstInfo = TrainUE::GetTrainUE(ulUserID);
        if (!pstInfo)
        {
            continue;
        }

        pstTrain = Train::GetTrainInfo(pstInfo->ulTrainUnitID);
        if (pstTrain)
        {
            Node->Values[7] = GIS::GetGISText(pstTrain->dLon, pstTrain->dLat);
        }
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------

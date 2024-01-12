//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmGroup.h"
#include "DlgPatchGroupMember.h"
#include "DlgDynamicGroupMember.h"
#include "DlgMultiGroupMember.h"
#include "DlgStaticGroupMember.h"
#include "DlgNewPatchGroup.h"
#include "DlgNewDynamicGroup.h"
#include "DlgNewMultiGroup.h"
#include "main.h"
#include "FrmGroupCall.h"
#include "FrmVideoCall.h"
#include "DlgSubGroup.h"
#include "DlgSetGroupInfo.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxCustomData"
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
#pragma link "cxPC"
#pragma link "dxSkinscxPCPainter"
#pragma link "cxProgressBar"
#pragma link "cxCheckBox"
#pragma link "cxClasses"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "dxBarBuiltInMenu"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxExtEditRepositoryItems"
#pragma link "cxScrollBox"
#pragma resource "*.dfm"
TFormGroup *FormGroup;

static INT64    g_i64CurrListenGroupID = INVALID_GROUP_ID;      // 正在监听的群组
static UINT32   g_ulCurrListenGroupTime = 0;                    // 监听开始时间

static VectorMap<INT64, UINT32>     g_mStationGroupStatus;

static VOID InitStationGroupStatus()
{
    VECTOR<STATION_UE_INFO_T*>  &vStationGroup = StationUE::GetStationUE();
    UINT32                      ulGroupStatus = GROUP_STATUS_UNKNOWN;
    GROUP_INFO_T                stGroup;
    STATION_INFO_T              *pstStation;

    g_mStationGroupStatus.Clear();
    for (UINT32 i=0; i<vStationGroup.size(); i++)
    {
        pstStation = Station::GetStationInfo(vStationGroup[i]->ulStationID);
        if (!pstStation)
        {
            continue;
        }

        g_mStationGroupStatus.Add(vStationGroup[i]->i64DefaultGroupID, ulGroupStatus);

        stGroup.i64GroupID = vStationGroup[i]->i64DefaultGroupID;
        sprintf(stGroup.acGroupName, pstStation->acStationName);
        stGroup.ulGroupType = STATION_GROUP;
        stGroup.ulPriority = 10;
        stGroup.ulDCUserID = INVALID_USER_ID;

        Group::AddGroup(&stGroup);

        Group::AddAutoJoinGroup(stGroup.i64GroupID, FALSE);
    }
}

static INT32 GetGroupSpeakTime(INT64 i64GroupID)
{
    EVENT_GROUP_STATUS_T    *pstStatus = Group::GetGroupCallStatus(i64GroupID);

    if (!pstStatus)
    {
        return -60;
    }

    return pstStatus->ulSpeakTime;
}

static BOOL SaveSubGroup(Int64Set &sOld, Int64Set &sNew)
{
    GJsonParam          ReqParam;
    GJsonTupleParam     TupleParam;

    for (UINT32 i=0; i<sOld.Size(); i++)
    {
        GJsonParam  SubParam;

        SubParam.Add("GroupID", sOld.Get(i));
        TupleParam.Add(SubParam);
    }

    ReqParam.Add("DelGroup", TupleParam);

    TupleParam.Clear();
    for (UINT32 i=0; i<sNew.Size(); i++)
    {
        GJsonParam  SubParam;

        SubParam.Add("GroupID", sNew.Get(i));
        TupleParam.Add(SubParam);
    }

    ReqParam.Add("AddGroup", TupleParam);

    if (!SyncSendServer(EV_DC_DIS_REQ, MSG_DCSaveSubGroupInfo, ReqParam))
    {
        return FALSE;
    }

    return TRUE;
}

//---------------------------------------------------------------------------
void SendSetDynamicGroup(INT64 i64GroupID, VECTOR<UINT32> *pvAddUser, VECTOR<UINT32> *pvDelUser)
{
    GJsonParam          Param;
    GJsonTupleParam     AddUserTupleParam;
    GJsonTupleParam     DelUserTupleParam;

    Param.Add("GroupID", i64GroupID);

    if (pvAddUser)
    {
        for (UINT32 i=0; i<pvAddUser->size(); i++)
        {
            GJsonParam          SubParam;

            SubParam.Add("UserID", pvAddUser->at(i));

            if (TrainUE::GetTrainUE(pvAddUser->at(i)))
            {
                AddUserTupleParam.Add(SubParam);
            }
        }

        Param.Add("AddUser", AddUserTupleParam);
    }

    if (pvDelUser)
    {
        for (UINT32 i=0; i<pvDelUser->size(); i++)
        {
            GJsonParam          SubParam;

            SubParam.Add("UserID", pvDelUser->at(i));

            if (TrainUE::GetTrainUE(pvDelUser->at(i)))
            {
                DelUserTupleParam.Add(SubParam);
            }
        }

        Param.Add("DelUser", DelUserTupleParam);
    }

    if (AddUserTupleParam.GetTupleNum() > 0 ||
        DelUserTupleParam.GetTupleNum() > 0 )
    {
        SendServer(EV_DC_DIS_APP_REQ, MSG_DCSetDynamicGroup, Param);
    }
}

void __fastcall TFormGroup::InitWorkPanel()
{
    TcxButton   *btns[] = { btnJoinGroup,
                            btnUnSubGroup,
                            btnGroupMember,
                            btnPTT
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

    btnPTT->Width = 120;
    btnPTT->Height = 90;
    btnPTT->Top = btnPTT->Parent->Height - btnPTT->Height - 60;
    btnPTT->Left = (btnPTT->Parent->Width - btnPTT->Width)/2;
}

void __fastcall TFormGroup::InitSkin()
{
    SetSkin(tlMember, true);
}
//---------------------------------------------------------------------------
TcxTreeListNode* __fastcall TFormGroup::GetSelectNode()
{
    if (tlGroup->SelectionCount == 0)
    {
        return NULL;
    }

    TcxTreeListNode     *Node = tlGroup->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return NULL;
    }

    return Node;
}
//---------------------------------------------------------------------------
GROUP_INFO_T* __fastcall TFormGroup::GetSelectGroup()
{
    if (tlGroup->SelectionCount == 0)
    {
        return NULL;
    }

    TcxTreeListNode     *Node = GetSelectNode();

    if (!Node || Node->Values[1].IsNull())
    {
        return NULL;
    }

    INT64   i64GroupID = (INT64)Node->Values[1];
    GROUP_INFO_T    *pstGroup = Group::GetGroup(i64GroupID);

    return pstGroup;
}
//---------------------------------------------------------------------------
__fastcall TFormGroup::TFormGroup(TComponent* Owner)
    : TSubForm(Owner)
{
    InitWorkPanel();

    m_i64GroupID = INVALID_GROUP_ID;

    InitStationGroupStatus();
    pcMain->ActivePageIndex = -1;
    pcMain->ActivePage = tsStaticGroup;

    InitMsgHandler(EV_DC_DIS_RSP, MSG_DCSaveGroupStatus, OnSaveGroupStatusRsp);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::ShowGroupCallStatus(EVENT_GROUP_STATUS_T &stEvent)
{
    TcxTreeListNode *Node = GetGroupNode(stEvent.i64GroupID);

    if (Node)
    {
        Node->Values[4] = stEvent.acSpeakerName;
        Node->Values[6] = stEvent.ulSpeakTime;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::UpdateGroupCallInfo(TcxTreeListNode *Node, INT64 i64GroupID, UINT32 ulGroupStatus)
{
    if (!Node)
    {
        Node = GetGroupNode(i64GroupID);
    }

    if (Node)
    {
        Node->Values[2] = ulGroupStatus;
    }

    if (ulGroupStatus == GROUP_STATUS_UNSUB ||
        ulGroupStatus == GROUP_STATUS_DELETE)
    {
        InitCtrlPanel();

        return;
    }

    EVENT_GROUP_STATUS_T    *pstGroupStatus = Group::GetGroupCallStatus(i64GroupID);
    BOOL                    bIsEmergencyCall = FALSE;

    if (pstGroupStatus)
    {
        bIsEmergencyCall = pstGroupStatus->bIsEmergencyCall;
    }

    InitCtrlPanel();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::tlGroupDblClick(TObject *Sender)
{
    return;
/*
    UINT32  ulStatus = GROUP_STATUS_UNKNOWN;
    TcxTreeListNode     *Node = GetSelectNode();

    if (!Node)
    {
        return;
    }

    if (!Node->Values[2].IsNull())
    {
        ulStatus = Node->Values[2];
    }

    if (ulStatus == GROUP_STATUS_UNSUB ||
        ulStatus == GROUP_STATUS_UNKNOWN)
    {
        pmiJoinGroupClick(this);
    }
    else if (ulStatus == GROUP_STATUS_JOIN)
    {
        pmiUnsubGroupClick(this);
    }      */
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::pmiSubGroupClick(TObject *Sender)
{
    TcxTreeListNode     *Node = GetSelectNode();

    if (!Node)
    {
        return;
    }

    INT64   i64GroupID = (INT64)Node->Values[1];

    if (!Group::GetGroup(i64GroupID))
    {
        return;
    }

    if (!eSDK_SubGroup(i64GroupID))
    {
        InfoMsg("监听组失败!");
        return;
    }

    Group::UpdateGroupSubStatus(i64GroupID, GROUP_STATUS_SUB, TRUE);
    UpdateGroupCallInfo(Node, i64GroupID, GROUP_STATUS_SUB);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::pmiUnsubGroupClick(TObject *Sender)
{
    if (tlGroup->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlGroup->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    INT64   i64GroupID = (INT64)Node->Values[1];

    if (!Group::GetGroup(i64GroupID))
    {
        return;
    }

    if (!Group::ExitGroup(i64GroupID))
    {
        InfoMsg("退出组失败!");
        return;
    }

    Group::UpdateGroupSubStatus(i64GroupID, GROUP_STATUS_UNSUB, TRUE);
    UpdateGroupCallInfo(Node, i64GroupID, GROUP_STATUS_UNSUB);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::tlGroupFocusedNodeChanged(TcxCustomTreeList *Sender,
          TcxTreeListNode *APrevFocusedNode, TcxTreeListNode *AFocusedNode)
{
    tlGroupClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::ShowGroupMember(VECTOR<UINT32> &vUser, VECTOR<INT64> &vGroup)
{
    TcxTreeListNode     *ParentNode;
    TcxTreeListNode     *Node;
    BOOL                bOnline;
    UINT32              ulUserID;
    UINT32              ulUserType;
    UINT32              ulFuncType;
    UINT32              ulStatus;
    UINT32              ulCellID;
    UINT32              ulCallStatus;
    CHAR                acUserName[64];

    tlMember->BeginUpdate();
    tlMember->Clear();

    if (vUser.size() > 0)
    {
        ParentNode = tlMember->Add();
        ParentNode->ImageIndex = -1;
        ParentNode->Values[0] = "终端";
        for (UINT32 i=0; i<vUser.size(); i++)
        {
            ulUserID = vUser[i];

            Node = ParentNode->AddChild();
            Node->Values[0] = User::GetUserDisplayName(ulUserID);
        }
    }

    if (vGroup.size() > 0)
    {
        ParentNode = tlMember->Add();
        ParentNode->ImageIndex = -1;
        ParentNode->Values[0] = "群组";
        for (UINT32 i=0; i<vGroup.size(); i++)
        {
            Node = ParentNode->AddChild();
            Node->ImageIndex = 0;

            Node->Values[0] = Group::GetGroupDisplayName(vGroup[i]);
        }
    }

    tlMember->Root->Expand(true);
    tlMember->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::InitCtrlPanel()
{
    TcxTreeListNode *FocusedNode = GetSelectNode();

    pmiSubGroup->Enabled = false;
    pmiJoinGroup->Enabled = false;
    pmiUnsubGroup->Enabled = false;
    pmiShowMember->Enabled = false;

    if (!FocusedNode || FocusedNode->Values[2].IsNull())
    {
        return;
    }

    pmiShowMember->Enabled = true;

    UINT32      ulGroupStatus = FocusedNode->Values[2];
    INT64       i64GroupID = FocusedNode->Values[1];

    if (ulGroupStatus == GROUP_STATUS_UNSUB)
    {
        pmiSubGroup->Enabled = true;
        pmiJoinGroup->Enabled = true;
        pmiUnsubGroup->Enabled = false;
    }
    else if (ulGroupStatus == GROUP_STATUS_SUB)
    {
        pmiSubGroup->Enabled = false;
        pmiJoinGroup->Enabled = true;
        pmiUnsubGroup->Enabled = false;
    }
    else if (ulGroupStatus == GROUP_STATUS_JOIN)
    {
        pmiSubGroup->Enabled = false;
        pmiJoinGroup->Enabled = false;

        if (!GroupCfg::IsMulticastGroup(i64GroupID) ) // 通播组
        {
            pmiUnsubGroup->Enabled = true;
        }
    }
    else
    {
        pmiSubGroup->Enabled = true;
        pmiJoinGroup->Enabled = true;
        pmiUnsubGroup->Enabled = false;
    }

    if (pcMain->ActivePage == tsPatchGroup ||
        pcMain->ActivePage == tsMultiGroup )
    {
        pmiSubGroup->Enabled = false;
        pmiJoinGroup->Enabled = false;
        pmiUnsubGroup->Enabled = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::FocusGroupCall(INT64 iGroupID)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::pmiShowMemberClick(TObject *Sender)
{
    GROUP_INFO_T    *pstGroup = GetSelectGroup();

    if (!pstGroup)
    {
        return;
    }

    SetGroupMember(pstGroup->i64GroupID);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::SetPatchGroupMember(INT64 i64PatchGroupID, INT64 i64OriGroupID)
{
    GROUP_INFO_T    *pstGroup = Group::GetGroup(i64PatchGroupID);

    if (!pstGroup)
    {
        ErrMsg("获取派接组信息失败");
        return;
    }

    VECTOR<INT64>   vMember;
    VECTOR<INT64>   vAddMember;
    VECTOR<INT64>   vDelMember;

    if (!eSDK_GetPatchGroupMembers(i64PatchGroupID, vMember))
    {
        InfoMsg("获取组成员失败!");
        return;
    }

    if (!DialogPatchGroupMember)
    {
        DialogPatchGroupMember = new TDialogPatchGroupMember(Application);
    }

    DialogPatchGroupMember->Init(i64PatchGroupID, vMember, i64OriGroupID);
    if (DialogPatchGroupMember->ShowModal() != mrOk)
    {
        return;
    }

    DialogPatchGroupMember->GetMember(vAddMember, vDelMember);
    delete DialogPatchGroupMember;
    DialogPatchGroupMember = NULL;

    if (!eSDK_AddPatchGroupMember(i64PatchGroupID, pstGroup->acGroupName, vAddMember))
    {
        ErrMsg("添加组成员失败");
        return;
    }

    if (!eSDK_DelPatchGroupMember(i64PatchGroupID, pstGroup->acGroupName, vDelMember))
    {
        ErrMsg("删除组成员失败");
        return;
    }

    InfoMsg("设置派接组成员成功");
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::SetGroupMember(INT64 i64GroupID)
{
    GROUP_INFO_T    *pstGroup = Group::GetGroup(i64GroupID);

    if (!pstGroup)
    {
        return;
    }

    if (pstGroup->ulGroupType == PATCH_GROUP)
    {
        SetPatchGroupMember(i64GroupID, INVALID_GROUP_ID);

        pmiRefreshClick(this);
    }
    else if (pstGroup->ulGroupType == DYNAMIC_GROUP)
    {
        VECTOR<UINT32>  vUser;
        VECTOR<INT64>   vGroup;
        VECTOR<UINT32>  vAddMember;
        VECTOR<UINT32>  vDelMember;

        if (!eSDK_GetGroupMember(i64GroupID, vUser, vGroup))
        {
            InfoMsg("获取组成员失败!");
            return;
        }

        if (!DialogDynamicGroupMember)
        {
            DialogDynamicGroupMember = new TDialogDynamicGroupMember(Application);
        }

        DialogDynamicGroupMember->Init(pstGroup, vUser);
        if (DialogDynamicGroupMember->ShowModal() != mrOk)
        {
            return;
        }

        DialogDynamicGroupMember->GetMember(vAddMember, vDelMember);
        delete DialogDynamicGroupMember;
        DialogDynamicGroupMember = NULL;

        if (!eSDK_SetDynamicGroupUser(i64GroupID, vAddMember, vDelMember))
        {
            ErrMsg("设置组成员失败");
            return;
        }

        SendSetDynamicGroup(i64GroupID, &vAddMember, &vDelMember);

        InfoMsg("设置组成员成功");
        pmiRefreshClick(this);
    }
    else if (pstGroup->ulGroupType == MULTI_GROUP)
    {
        VECTOR<INT64>   vGroup;
        VECTOR<INT64>   vAddMember;
        VECTOR<INT64>   vDelMember;

        if (!MultiGroup::GetMultiGroupMember(i64GroupID, vGroup))
        {
            InfoMsg("获取组成员失败!");
            return;
        }

        if (!DialogMultiGroupMember)
        {
            DialogMultiGroupMember = new TDialogMultiGroupMember(Application);
        }

        DialogMultiGroupMember->Init(i64GroupID, vGroup);
        if (DialogMultiGroupMember->ShowModal() != mrOk)
        {
            return;
        }

        DialogMultiGroupMember->GetMember(vAddMember, vDelMember);
        delete DialogMultiGroupMember;
        DialogMultiGroupMember = NULL;

        if (!MultiGroup::SetMultiGroupMember(i64GroupID, vAddMember, vDelMember))
        {
            ErrMsg("设置组成员失败");
            return;
        }

        InfoMsg("设置组成员成功");
        pmiRefreshClick(this);
    }
    else
    {
        VECTOR<UINT32>  vUser;
        VECTOR<INT64>   vGroup;

        if (!eSDK_GetGroupMember(i64GroupID, vUser, vGroup))
        {
            InfoMsg("获取组成员失败!");
            return;
        }

        DialogStaticGroupMember->Init(pstGroup, vUser, vGroup);
        DialogStaticGroupMember->ShowModal();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::ShowGroupInfo(TcxTabSheet *CurrPage)
{
    if (CurrPage == NULL)
    {
        CurrPage = pcMain->ActivePage;
    }

    pmiSetGroupInfo->Enabled = false;

    tlGroup->Parent = CurrPage;

    if (CurrPage == tsStaticGroup)
    {
        pmiSetGroupInfo->Enabled = true;
        ColumnCreator->Visible = false;
        ShowGroup(STATIC_GROUP);
    }
    else if (CurrPage == tsDynamicGroup)
    {
        ShowGroup(DYNAMIC_GROUP);
        ColumnCreator->Visible = true;
    }
    else if (CurrPage == tsPatchGroup)
    {
        ShowGroup(PATCH_GROUP);
        ColumnCreator->Visible = true;
    }
    else if (CurrPage == tsMultiGroup)
    {
        ShowGroup(MULTI_GROUP);
        ColumnCreator->Visible = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::SaveGroupStatus(INT64 i64GroupID, UINT32 ulGroupStatus)
{
    GJsonParam  Param;

    Param.Add("DCUserID", APP::GetCurrDCUserID());
    Param.Add("GroupID", i64GroupID);
    Param.Add("GroupStatus", ulGroupStatus);

    SendServer(EV_DC_DIS_REQ, MSG_DCSaveGroupStatus, Param);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::AutoSubGroup()
{
    VECTOR<INT64>   vGroup;
    INT64           i64GroupID;
    GROUP_STATUS_T  *pstStatus = NULL;
    TcxTreeListNode *Node;
    GROUP_INFO_T    *pstGroup;
    DC_GROUP_INFO_T *pstDynamicGroup;
    static UINT32   ulIndex = 0;
    UINT32          j;
    BOOL            bShow;

    TimerCheck->Interval = 5000;
    Group::GetAutoJoinGroup(vGroup);

    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        j = MOD(ulIndex+i, vGroup.size());
        i64GroupID = vGroup[j];

        pstStatus = Group::GetAutoJoinGroup(i64GroupID);
        if (!pstStatus)
        {
            continue;
        }

        if (pstStatus->bNeedSave)
        {
            SaveGroupStatus(i64GroupID, pstStatus->ulGroupStatus);
        }

        // 不自动订阅车站组
        if (Station::GetStationByGroupID(i64GroupID))
        {
            continue;
        }

        if (GroupCfg::IsMulticastGroup(i64GroupID))
        {
            continue;
        }

        pstGroup = Group::GetGroup(i64GroupID);
        pstDynamicGroup = DynamicGroup::GetDynamicGroup(i64GroupID);
        if (!pstGroup && !pstDynamicGroup)
        {
            GosLog(LOG_ERROR, "auto sub group: unknown group " FORMAT_I64, i64GroupID);
            Group::DelAutoJoinGroup(i64GroupID);
            continue;
        }

        if (pstStatus->ulGroupStatus == GROUP_STATUS_TOJOIN)
        {
            if (!Group::SubJoinGroup(i64GroupID))
            {
                TimerCheck->Interval = 1000;
                GosLog(LOG_ERROR, "join group " FORMAT_I64 " failed", i64GroupID);
            }
            else
            {
                TimerCheck->Interval = 100;
                GosLog(LOG_INFO, "join group " FORMAT_I64 " successful", i64GroupID);

                pstStatus->ulGroupStatus = GROUP_STATUS_JOIN;

                bShow = TRUE;

                if (GroupCfg::IsLineBrdGroup(i64GroupID) )
                {
                    bShow = FALSE;
                }
                else if (pstDynamicGroup)
                {
                    if (gos_str_like(pstDynamicGroup->acGroupName, GROUP_NAME_TRAIN))
                    {
                        bShow = FALSE;
                    }
                }

                if (bShow)
                {
                    UpdateGroupCallInfo(NULL, i64GroupID, GROUP_STATUS_JOIN);
                }
            }
        }
        else if (pstStatus->ulGroupStatus == GROUP_STATUS_TOUNSUB)
        {
            if (!Group::ExitGroup(i64GroupID))
            {
                TimerCheck->Interval = 1000;
                GosLog(LOG_ERROR, "unsub group " FORMAT_I64 " failed", i64GroupID);
            }
            else
            {
                pstStatus->ulGroupStatus = GROUP_STATUS_UNSUB;
                UpdateGroupCallInfo(NULL, i64GroupID, GROUP_STATUS_UNSUB);

                TimerCheck->Interval = 100;
                GosLog(LOG_INFO, "unsub group " FORMAT_I64 " successful", i64GroupID);
            }
        }
        else
        {
            continue;
        }

        break;
    }

    ulIndex = j+1;
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::OnSaveGroupStatusRsp(UINT32 ulSeqID, GJson &Json)
{
    UINT32  ulDCUserID;
    UINT32  ulGroupStatus;
    INT64   i64GroupID;
    char    *szResult = Json.GetValue("Result");

    if (!szResult)
    {
        return;
    }

    if (!Json.GetValue("DCUserID", &ulDCUserID))
    {
        GosLog(LOG_ERROR, "invalid DCUserID in msg %s", MSG_DCSaveGroupStatus);
        return;
    }

    if (ulDCUserID != APP::GetCurrDCUserID())
    {
        GosLog(LOG_ERROR, "invalid DC user(%u) in msg %s", ulDCUserID, MSG_DCSaveGroupStatus);
        return;
    }

    if (!Json.GetValue("GroupID", &i64GroupID))
    {
        GosLog(LOG_ERROR, "invalid GroupID in msg %s", MSG_DCSaveGroupStatus);
        return;
    }

    if (!Json.GetValue("GroupStatus", &ulGroupStatus))
    {
        GosLog(LOG_ERROR, "invalid GroupStatus in msg %s", MSG_DCSaveGroupStatus);
        return;
    }

    if (strcasecmp(szResult, "true") != 0)
    {
        return;
    }

    Group::CommitGroupSubStatus(i64GroupID, ulGroupStatus);
    DeleteConfirmRequest(ulSeqID);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::TimerCheckTimer(TObject *Sender)
{
    static bool bInit = false;

    if (!bInit)
    {
        bInit = Group::LoadStaticGroup();
        if (!bInit)
        {
            return;
        }

        ShowGroupInfo();
    }

    if (g_i64CurrListenGroupID != INVALID_GROUP_ID)
    {
        UINT32      ulTime = gos_get_uptime() - g_ulCurrListenGroupTime;

        if (ulTime > GetMaxListenPeriod())
        {
            pmiStopListenClick(this);
        }
    }

    AutoSubGroup();

    Group::CheckAutoJoinGroup();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::ShowSubGroup()
{
    VECTOR<INT64>   vGroup;

    Group::GetAutoJoinGroup(vGroup);

    tlGroup->BeginUpdate();
    tlGroup->Clear();

    TcxTreeListNode *Node;
    TRAIN_INFO_T    *pstTrain;
    GROUP_INFO_T    *pstGroup;

    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        pstGroup = Group::GetGroup(vGroup[i]);
        if (!pstGroup)
        {
            continue;
        }

        if (!Group::IsVisibleGroup(pstGroup) ||
            pstGroup->ulGroupType == STATION_GROUP ||
            Train::GetTrainInfo(pstGroup->i64GroupID) )
        {
            // 列车组、车站组不在订阅组管理
            continue;
        }

        UINT32  ulGroupStatus = Group::GetGroupSubStatus(pstGroup->i64GroupID);

        if (ulGroupStatus == GROUP_STATUS_SUB ||
            ulGroupStatus == GROUP_STATUS_TOJOIN ||
            ulGroupStatus == GROUP_STATUS_JOIN )
        {
            Node = tlGroup->Add();
            Node->Values[0] = Group::GetGroupName(pstGroup);
            Node->Values[1] = pstGroup->i64GroupID;
            Node->Values[2] = ulGroupStatus;
            Node->Values[6] = GetGroupSpeakTime(pstGroup->i64GroupID);
        }
    }

    tlGroup->Root->Expand(true);
    tlGroup->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::ColumnStatusGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (!ANode || ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulGroupStatus = ANode->Values[2];

    if (ulGroupStatus == GROUP_STATUS_SUB)
    {
        Value = "监听";
    }
    else if (ulGroupStatus == GROUP_STATUS_JOIN)
    {
        Value = "对讲";
    }
    else
    {
        Value = "";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::ShowAllGroup()
{
    TcxTreeListNode     *Node;
    TcxTreeListNode     *ChildNode;
    std::vector<UINT32> vGroupType;

    Group::GetGroupType(vGroupType);

    tlGroup->BeginUpdate();
    tlGroup->Clear();

    for (UINT32 i=0; i<vGroupType.size(); i++)
    {
        UINT32      ulGroupType = vGroupType[i];
        AnsiString  strGroupType = Group::GetGroupTypeName(ulGroupType);
        VECTOR<GROUP_INFO_T*>   vGroup;

        Group::GetGroup(ulGroupType, vGroup);

        Node = tlGroup->Add();
        Node->Values[0] = strGroupType;
        Node->Values[1] = INVALID_GROUP_ID;
        Node->Values[6] = -60;

        for (UINT32 j=0; j<vGroup.size(); j++)
        {
            GROUP_INFO_T    *pstGroup = vGroup[j];

            if (gos_str_like(pstGroup->acGroupName, GROUP_NAME_TRAIN))
            {
                continue;
            }

            ChildNode = Node->AddChild();

            ChildNode->Values[0] = Group::GetGroupName(pstGroup);
            ChildNode->Values[1] = pstGroup->i64GroupID;
            ChildNode->Values[2] = Group::GetGroupSubStatus(pstGroup->i64GroupID);
            ChildNode->Values[3] = pstGroup->ulDCUserID;
            ChildNode->Values[6] = GetGroupSpeakTime(pstGroup->i64GroupID);
        }
    }

    tlGroup->Root->Expand(true);
    tlGroup->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::ShowGroup(UINT32 ulGroupType)
{
    VECTOR<GROUP_INFO_T*>   vGroup;
    GROUP_INFO_T            *pstGroup;
    BOOL                    bShow;

    Group::GetGroup(ulGroupType, vGroup);

    tlMember->Clear();

    tlGroup->BeginUpdate();
    tlGroup->Clear();

    TcxTreeListNode *Node = NULL;

    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        GROUP_INFO_T    *pstGroup = vGroup[i];

        if (!pstGroup)
        {
            continue;
        }

        if (!Group::IsVisibleGroup(pstGroup))
        {
            continue;
        }

        if (ulGroupType == STATIC_GROUP &&
            !pmiShowAllGroup->Down)
        {
            if (!g_sSubGroup.Has(pstGroup->i64GroupID))
            {
                continue;
            }
        }
        else if (ulGroupType == DYNAMIC_GROUP)
        {
            if (GroupCfg::IsLineBrdGroup(pstGroup->i64GroupID))
            {
                continue;
            }

            if (gos_str_like(pstGroup->acGroupName, GROUP_NAME_TRAIN) ||
                gos_str_like(pstGroup->acGroupName, GROUP_NAME_DC_BRD_TRAINS) ||
                gos_str_like(pstGroup->acGroupName, GROUP_NAME_REQ_CALL) ||
                gos_str_like(pstGroup->acGroupName, GROUP_NAME_MULTI_USER) )
            {
                continue;
            }

            if (pstGroup->ulDCUserID != APP::GetCurrDCUserID())
            {
                continue;
            }
        }
        else if (ulGroupType == PATCH_GROUP)
        {
            if (PatchGroup::IsTmpPatchGroup(pstGroup->acGroupName))
            {
                continue;
            }
        }

        UINT32  ulGroupStatus = Group::GetGroupSubStatus(pstGroup->i64GroupID);

        Node = tlGroup->Add();
        Node->Values[0] = Group::GetGroupName(pstGroup);
        Node->Values[1] = pstGroup->i64GroupID;
        Node->Values[2] = ulGroupStatus;
        Node->Values[3] = pstGroup->ulDCUserID;
        Node->Values[6] = GetGroupSpeakTime(pstGroup->i64GroupID);
    }

    if (tlGroup->AbsoluteCount > 0)
    {
        tlGroup->AbsoluteItems[0]->Selected = true;
    }

    tlGroup->Root->Expand(true);
    tlGroup->EndUpdate();

    if (tlGroup->AbsoluteCount > 0)
    {
        tlGroup->AbsoluteItems[0]->Selected = true;
        tlGroupClick(this);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::pmiJoinGroupClick(TObject *Sender)
{
    TcxTreeListNode     *Node = GetSelectNode();

    if (!Node)
    {
        return;
    }

    INT64   i64GroupID = (INT64)Node->Values[1];

    if (!Group::GetGroup(i64GroupID))
    {
        return;
    }

    if (!Group::SubJoinGroup(i64GroupID))
    {
        InfoMsg("加入组失败!");
        return;
    }

    Group::UpdateGroupSubStatus(i64GroupID, GROUP_STATUS_JOIN, TRUE);
    UpdateGroupCallInfo(Node, i64GroupID, GROUP_STATUS_JOIN);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::TimerSpeakTimeTimer(TObject *Sender)
{
    UINT32  ulCurrTime = gos_get_uptime();

    TcxTreeListNode *Node = tlGroup->Root->getFirstChild();

    for(; Node; Node=Node->GetNext())
    {
        INT32   iSpeakTime = (INT32)Node->Values[6];
        UINT32  ulTime = (ulCurrTime - iSpeakTime);

        if (ulTime >= 60)
        {
            Node->Values[5] = 0;
        }
        else
        {
            Node->Values[5] = 60 - ulTime;
        }
    }
}
//---------------------------------------------------------------------------
TcxTreeListNode* __fastcall TFormGroup::GetGroupNode(INT64 i64GroupID)
{
    INT64           i64CurrGroupID;
    TcxTreeListNode *Node = tlGroup->Root->getFirstChild();

    for(; Node; Node=Node->GetNext())
    {
        if (Node->Values[1].IsNull())
        {
            continue;
        }

        i64CurrGroupID = (INT64)Node->Values[1];
        if (i64CurrGroupID == i64GroupID)
        {
            return Node;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::pmiCreatePatchGroupClick(TObject *Sender)
{
    DialogNewPatchGroup->Init();
    DialogNewPatchGroup->ShowModal();

    if (DialogNewPatchGroup->ModalResult != mrOk)
    {
        return;
    }

    AnsiString strGroupName = DialogNewPatchGroup->GetGroupName();

    BOOL    bRet = eSDK_CreatePatchGroup(strGroupName.c_str());

    if (!bRet)
    {
        ErrMsg("创建派接组失败");
    }
    else
    {
        InfoMsg("创建派接组成功");
        PatchGroup::LoadPatchGroup(TRUE);

        SaveOperLog("创建派接组：%s", strGroupName.c_str());
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::AddPatchGroup(AnsiString strGroupName, INT64 i64GroupID)
{
    PatchGroup::LoadPatchGroup(TRUE);

    if (pcMain->ActivePage == tsPatchGroup)
    {
        ShowGroupInfo();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::TimerCheckPatchGroupTimer(TObject *Sender)
{
    if (!PatchGroup::IsPatchGroupLoaded())
    {
        if (PatchGroup::LoadPatchGroup(TRUE) &&
            pcMain->ActivePage == tsPatchGroup)
        {
            ShowGroupInfo();
        }

        return;
    }

    PatchGroup::CheckPatchGroup();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::pmGroupPopup(TObject *Sender)
{
    pmiCreatePatchGroup->Enabled = false;
    pmiDeletePatchGroup->Enabled = false;
    pmiCreateDynamicGroup->Enabled = false;
    pmiDeleteDynamicGroup->Enabled = false;
    pmiCreateMultiGroup->Enabled = false;
    pmiDeleteMultiGroup->Enabled = false;
    pmiSetSubGroup->Enabled = false;

    pmiJoinGroup->Enabled = true;
    pmiNewGroupCall->Enabled = true;
    pmiStartListen->Enabled = true;

    if (pcMain->ActivePage == tsPatchGroup)
    {
        pmiCreatePatchGroup->Enabled = true;
        pmiDeletePatchGroup->Enabled = true;
    }
    else if (pcMain->ActivePage == tsDynamicGroup)
    {
        pmiCreateDynamicGroup->Enabled = true;
        pmiDeleteDynamicGroup->Enabled = true;
    }
    else if (pcMain->ActivePage == tsMultiGroup)
    {
        pmiCreateMultiGroup->Enabled = true;
        pmiDeleteMultiGroup->Enabled = true;
    }
    else if (pcMain->ActivePage == tsStaticGroup)
    {
        pmiSetSubGroup->Enabled = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::pmiDeletePatchGroupClick(TObject *Sender)
{
    TcxTreeListNode     *Node = GetSelectNode();

    if (!Node)
    {
        return;
    }

    INT64           i64GroupID = Node->Values[1];
    GROUP_INFO_T    *pstGroup = Group::GetGroup(i64GroupID);

    if (!pstGroup)
    {
        return;
    }

    if (!Group::IsPatchGroup(pstGroup->ulGroupType))
    {
        return;
    }

    if (pstGroup->ulDCUserID != APP::GetCurrDCUserID())
    {
        InfoMsg("无权删除");
        return;
    }

    AnsiString strGroupName = Group::GetGroupDisplayName(i64GroupID);

    if (!AskMsg("删除派接组[%s]吗？", strGroupName.c_str()))
    {
        return;
    }

    BOOL    bRet = PatchGroup::CancelPatchGroup(i64GroupID);

    if (!bRet)
    {
        ErrMsg("删除组失败");
        return;
    }

    Node->Delete();

    InfoMsg("删除组成功");

    SaveOperLog("删除派接组：%s", strGroupName.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::ColumnSpeakTimeGetEditProperties(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, TcxCustomEditProperties *&EditProperties)
{
    if (ANode->Values[5].IsNull())
    {
        return;
    }

    UINT32      ulTime = ANode->Values[5];

    if (ulTime > 0)
    {
        EditProperties = EditRepositoryProgressBar->Properties;
    }
    else
    {
        EditProperties = NULL;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::ColumnSpeakTimeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    Value = "";
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::FormCreate(TObject *Sender)
{
    SetImage(pmiSubGroup,   IMG_SUB_GROUP);
    SetImage(pmiUnsubGroup, IMG_UNSUB_GROUP);
    SetImage(pmiJoinGroup,  IMG_JOIN_GROUP);
    SetImage(pmiNewGroupCall,   IMG_PTT_DIAL);

    SetImage(pmiCreatePatchGroup, IMG_ADD);
    SetImage(pmiDeletePatchGroup, IMG_DEL);
    SetImage(pmiCreateDynamicGroup, IMG_ADD);
    SetImage(pmiDeleteDynamicGroup, IMG_DEL);
    SetImage(pmiCreateMultiGroup, IMG_ADD);
    SetImage(pmiDeleteMultiGroup, IMG_DEL);

    SetImage(pmiShowMember, IMG_MEMBER);

    SetImage(pmiStartListen, IMG_START_LISTEN);
    SetImage(pmiStopListen, IMG_STOP_LISTEN);

    SetImage(pmiRefresh, IMG_REFRESH);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::TimerCheckTrainGroupTimer(TObject *Sender)
{
    return;
}

//---------------------------------------------------------------------------
void __fastcall TFormGroup::TimerCheckStationGroupTimer(TObject *Sender)
{
    // TODO
    return;

    // 自动加入车站组
/*
    VECTOR<DC_GROUP_INFO_T*>    vGroup;
    DC_GROUP_INFO_T             *pstGroup;
    UINT32                      ulIndex;
    INT64                       i64GroupID;
    UINT32                      *pulGroupStatus;
    static UINT32               ulCurrCheckIndex = 0;

    TimerCheckStationGroup->Interval = 5000;

    if (ulCurrCheckIndex >= g_mStationGroupStatus.Size())
    {
        ulCurrCheckIndex = 0;
    }

    for (UINT32 i=0; i<g_mStationGroupStatus.Size(); i++)
    {
        ulIndex = MOD(ulCurrCheckIndex+i, g_mStationGroupStatus.Size());
        i64GroupID = g_mStationGroupStatus.GetKey(i);
        pulGroupStatus = g_mStationGroupStatus[i];
        if (*pulGroupStatus == GROUP_STATUS_JOIN)
        {
            continue;
        }

        if (SubJoinGroup(i64GroupID))
        {
            GosLog(LOG_INFO, "auto join station group " FORMAT_I64 " successful", i64GroupID);
            *pulGroupStatus = GROUP_STATUS_JOIN;
            TimerCheckStationGroup->Interval = 10;
        }
        else
        {
            GosLog(LOG_INFO, "auto join station group " FORMAT_I64 " failed", i64GroupID);
            TimerCheckStationGroup->Interval = 1000;
        }

        break;
    }

    ulCurrCheckIndex = ulIndex+1;  */
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::pmiStartListenClick(TObject *Sender)
{
    TcxTreeListNode     *Node = GetSelectNode();

    if (!Node)
    {
        return;
    }

    INT64   i64GroupID = (INT64)Node->Values[1];

    if (!Group::GetGroup(i64GroupID))
    {
        return;
    }

    if (!eSDK_StartGroupDiscreetListen(i64GroupID))
    {
        InfoMsg("侦听失败");
        return;
    }

    g_i64CurrListenGroupID = i64GroupID;
    g_ulCurrListenGroupTime = gos_get_uptime();
    pmiStartListen->Visible = ivNever;
    pmiStopListen->Visible = ivAlways;

    SaveOperLog("开始侦听: "FORMAT_I64, i64GroupID);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::pmiStopListenClick(TObject *Sender)
{
    eSDK_StopGroupDiscreetListen(g_i64CurrListenGroupID);
    g_i64CurrListenGroupID = INVALID_GROUP_ID;

    pmiStartListen->Visible = ivAlways;
    pmiStopListen->Visible = ivNever;

    SaveOperLog("结束侦听: "FORMAT_I64, g_i64CurrListenGroupID);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::pmiCreateDynamicGroupClick(TObject *Sender)
{
    DialogNewDynamicGroup->Init();
    DialogNewDynamicGroup->ShowModal();

    if (DialogNewDynamicGroup->ModalResult != mrOk)
    {
        return;
    }

    INT64   i64GroupID;
    CHAR    acGroupName[64];
    BOOL    bRet;
    VECTOR<UINT32>  vUser;

    DialogNewDynamicGroup->GetGroupInfo(&i64GroupID, acGroupName, vUser);
    bRet = DynamicGroup::CreateTempDynamicGroup(i64GroupID, acGroupName, vUser);
    if (!bRet)
    {
        ErrMsg("创建动态组失败");
        return;
    }

    InfoMsg("创建动态组成功");

    ShowGroup(DYNAMIC_GROUP);

    UpdateGroupCallInfo(NULL, i64GroupID, GROUP_STATUS_TOJOIN);

    SendSetDynamicGroup(i64GroupID, &vUser, NULL);

    SaveOperLog("创建动态组: "FORMAT_I64"(%s)", i64GroupID, acGroupName);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::pmiDeleteDynamicGroupClick(TObject *Sender)
{
    TcxTreeListNode     *Node = GetSelectNode();

    if (!Node)
    {
        return;
    }

    INT64           i64GroupID = Node->Values[1];
    GROUP_INFO_T    *pstGroup = Group::GetGroup(i64GroupID);

    if (!pstGroup)
    {
        return;
    }

    if (pstGroup->ulDCUserID != APP::GetCurrDCUserID())
    {
        InfoMsg("无权删除");
        return;
    }

    AnsiString  strGroupName = Group::GetGroupDisplayName(i64GroupID);

    if (!AskMsg("删除动态组[%s]吗？", strGroupName.c_str()) )
    {
        return;
    }

    VECTOR<UINT32>      vUser;

    eSDK_GetDynamicGroupMember(i64GroupID, vUser);

    BOOL    bRet = eSDK_CancelDynamicGroup(i64GroupID);

    if (!bRet)
    {
        ErrMsg("删除组失败");
        return;
    }

    Node->Delete();
    Group::DelGroup(i64GroupID);

    SendSetDynamicGroup(i64GroupID, &vUser, NULL);

    InfoMsg("删除组成功");

    SaveOperLog("删除动态组: "FORMAT_I64"(%s)", i64GroupID, strGroupName.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::pmiCreateMultiGroupClick(TObject *Sender)
{
    DialogNewMultiGroup->Init();
    DialogNewMultiGroup->ShowModal();

    if (DialogNewMultiGroup->ModalResult != mrOk)
    {
        return;
    }

    INT64   i64GroupID;
    CHAR    acGroupName[64];
    BOOL    bRet;
    VECTOR<INT64>  vGroup;

    DialogNewMultiGroup->GetGroupInfo(&i64GroupID, acGroupName, vGroup);
    bRet = MultiGroup::CreateMultiGroup(i64GroupID, acGroupName, vGroup);
    if (!bRet)
    {
        ErrMsg("创建多选组失败");
        return;
    }

    InfoMsg("创建多选组成功");
    pmiRefreshClick(this);

    ShowGroup(MULTI_GROUP);

    SaveOperLog("创建多选组: "FORMAT_I64"(%s)", i64GroupID, acGroupName);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::pmiDeleteMultiGroupClick(TObject *Sender)
{
    TcxTreeListNode     *Node = GetSelectNode();
    GROUP_INFO_T        *pstGroup = GetSelectGroup();

    if (!Node || !pstGroup)
    {
        return;
    }

    if (pstGroup->ulDCUserID != APP::GetCurrDCUserID())
    {
        InfoMsg("无权删除");
        return;
    }

    AnsiString  strGroupName = Group::GetGroupDisplayName(pstGroup->i64GroupID);

    if (!AskMsg("删除多选组[%s]吗？", strGroupName.c_str()))
    {
        return;
    }

    BOOL    bRet = MultiGroup::CancelMultiGroup(pstGroup->i64GroupID);

    if (!bRet)
    {
        ErrMsg("删除组失败");
        return;
    }

    Node->Delete();
    InfoMsg("删除组成功");

    pmiRefreshClick(this);

    SaveOperLog("删除多选组: %s", strGroupName.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::pmiRefreshClick(TObject *Sender)
{
    LoadSubGroupInfo();

    if (pcMain->ActivePage == tsStaticGroup)
    {
        Group::LoadStaticGroup();
    }
    else if (pcMain->ActivePage == tsPatchGroup)
    {
        PatchGroup::LoadPatchGroup(TRUE);
    }
    else if (pcMain->ActivePage == tsMultiGroup)
    {
        MultiGroup::LoadMultiGroup(TRUE);
    }

    ShowGroupInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::pmiNewGroupCallClick(TObject *Sender)
{
    FormGroupCall->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::FormShow(TObject *Sender)
{
    gbGroupMember->Style->Font->Size = UICfg::GetGroupBoxFontSize();
    gbGroupMember->Style->Font->Name = "宋体";

    btnPTT->Font->Size = UICfg::GetButtonPTTFontSize();
    btnPTT->Font->Name = UICfg::GetButtonPTTFontName();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::pmiSetSubGroupClick(TObject *Sender)
{
    DialogSubGroup->Init(g_sSubGroup);
    if (DialogSubGroup->ShowModal() != mrOk)
    {
        return;
    }

    Int64Set        sOld;
    Int64Set        sNew;

    sOld.Init(g_sSubGroup);

    DialogSubGroup->Get(sNew);

    RemoveCommonMember(sOld, sNew);

    if (!SaveSubGroup(sOld, sNew))
    {
        InfoMsg("操作失败");
        GosLog(LOG_ERROR, "save sub group failed");
        return;
    }

    InfoMsg("操作成功");

    DialogSubGroup->Get(g_sSubGroup);

    ShowGroupInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::pmiShowAllGroupClick(TObject *Sender)
{
    ShowGroupInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::pmiSetGroupInfoClick(TObject *Sender)
{
    TcxTreeListNode *Node = GetSelectNode();

    if (!Node)
    {
        return;
    }

    INT64           i64GroupID = Node->Values[1];
    GROUP_INFO_T    *pstGroupInfo = Group::GetGroupMarkInfo(i64GroupID);
    CHAR            acGroupName[64] = {0};
    UINT32          ulFuncType = GROUP_FUNC_OTHER;

    if (pstGroupInfo)
    {
        strcpy(acGroupName, pstGroupInfo->acGroupName);
        ulFuncType = pstGroupInfo->ulFuncType;
    }

    DialogSetGroupInfo->Init(i64GroupID, acGroupName, ulFuncType);
    if (DialogSetGroupInfo->ShowModal() != mrOk)
    {
        return;
    }

    DialogSetGroupInfo->Get(acGroupName, ulFuncType);
    SaveGroupInfo(i64GroupID, acGroupName, ulFuncType);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::ColumnCreatorGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (!ANode->Values[3].IsNull())
    {
        if ((UINT32)ANode->Values[3] == INVALID_USER_ID)
        {
            Value = "";
            return;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::btnPTTMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    OnPTTDial();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::btnPTTMouseEnter(TObject *Sender)
{
    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::btnPTTMouseLeave(TObject *Sender)
{
    btnPTT->LookAndFeel->SkinName = GetDefaultSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::btnPTTMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
    OnPTTRelease();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::pcMainPageChanging(TObject *Sender, TcxTabSheet *NewPage,
          bool &AllowChange)
{
    ShowGroupInfo(NewPage);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::OnPTTDial()
{
    m_i64GroupID = INVALID_GROUP_ID;

    if (tlGroup->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlGroup->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    m_i64GroupID = Node->Values[1];

    if (PTT::Dial(m_i64GroupID))
    {
        btnPTT->LookAndFeel->SkinName = UICfg::GetPTTDownSkinName();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::OnPTTRelease()
{
    PTT::Release();
    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::tlGroupClick(TObject *Sender)
{
    InitCtrlPanel();
    tlMember->Clear();
    if (tlGroup->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlGroup->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    INT64   i64GroupID = (INT64)Node->Values[1];

    VECTOR<UINT32>  vUser;
    VECTOR<INT64>   vGroup;

    if (!Group::GetGroupMember(i64GroupID, vUser, vGroup))
    {
        GosLog(LOG_ERROR, "get group member of "FORMAT_I64"failed!", i64GroupID);
        return;
    }

    ShowGroupMember(vUser, vGroup);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::RefreshPatchGroupPanel()
{
    pmiRefreshClick(this);
    ColumnCreator->Visible = true;

    if (pcMain->ActivePage == tsPatchGroup)
    {
        ShowGroup(PATCH_GROUP);
    }

    pmiRefreshClick(this);
    tlGroupClick(this);
}
//---------------------------------------------------------------------------
INT64 __fastcall TFormGroup::GetFocusNodeGroupID()
{
    if (tlGroup->SelectionCount == 0)
    {
        return INVALID_GROUP_ID;
    }

    TcxTreeListNode *Node = tlGroup->Selections[0];

    if (!Node || Node->Values[1].IsNull())
    {
        return INVALID_GROUP_ID;
    }

    return (INT64)Node->Values[1];
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::pmiNewVideoGroupCallClick(TObject *Sender)
{
    INT64   i64GroupID = GetFocusNodeGroupID();

    if (i64GroupID == INVALID_GROUP_ID)
    {
        return;
    }

    if (!FormVideoCall->StartVideoGroupCall(i64GroupID))
    {
        GosLog(LOG_ERROR, "video group call to "FORMAT_I64 " failed", i64GroupID);
        return;
    }

    FormMain->ActivePage(PAGE_VIDEO);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroup::TimerCheckMultiGroupTimer(TObject *Sender)
{
    TimerCheckMultiGroup->Interval = 5*1000;

    if (!MultiGroup::IsMultiGroupLoaded())
    {
        if (MultiGroup::LoadMultiGroup(TRUE) &&
            pcMain->ActivePage == tsMultiGroup)
        {
            ShowGroupInfo();
        }

        return;
    }
}
//---------------------------------------------------------------------------


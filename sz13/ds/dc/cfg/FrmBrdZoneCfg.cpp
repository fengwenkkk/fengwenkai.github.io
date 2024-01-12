//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmBrdZoneCfg.h"
#include "DlgSetBrdZone.h"
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
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma resource "*.dfm"
TFormBrdZoneCfg *FormBrdZoneCfg;

//---------------------------------------------------------------------------
__fastcall TFormBrdZoneCfg::TFormBrdZoneCfg(TComponent* Owner)
    : TSubForm(Owner)
{
    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormBrdZoneCfg::btnAddClick(TObject *Sender)
{
    if (!DialogSetBrdZone->Init())
    {
        return;
    }

    if (DialogSetBrdZone->ShowModal() != mrOk)
    {
        return;
    }

    BRD_ZONE_INFO_T     stInfo;

    DialogSetBrdZone->Get(stInfo);

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("StationID", stInfo.ulStationID);
    ReqJsonParam.Add("ZoneID", stInfo.ulZoneID);
    ReqJsonParam.Add("ZoneName", stInfo.acZoneName);
    ReqJsonParam.Add("GroupID", stInfo.i64GroupID);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCAddBrdZone, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormBrdZoneCfg::btnDelClick(TObject *Sender)
{
    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlMain->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    AnsiString  strStationName = Node->Texts[0];
    AnsiString  strZoneName = Node->Texts[1];

    if (!AskMsg((CHAR*)"删除车站%s的站区%s吗？", strStationName.c_str(), strZoneName.c_str()))
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;
    UINT32      ulStationID = Node->Values[0];
    UINT32      ulZoneID = Node->Values[1];

    ReqJsonParam.Add("StationID", ulStationID);
    ReqJsonParam.Add("ZoneID", ulZoneID);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCDelBrdZone, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormBrdZoneCfg::btnSetClick(TObject *Sender)
{
    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlMain->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    BRD_ZONE_INFO_T     stInfo;

    stInfo.ulStationID = Node->Values[0];
    stInfo.ulZoneID = Node->Values[1];
    StrCpy(stInfo.acZoneName, Node->Texts[2]);
    stInfo.i64GroupID = Node->Values[3];

    DialogSetBrdZone->Init(stInfo);
    if (DialogSetBrdZone->ShowModal() != mrOk)
    {
        return;
    }

    DialogSetBrdZone->Get(stInfo);

    if (String(stInfo.acZoneName) == Node->Texts[2] &&
        stInfo.i64GroupID == (INT64)Node->Values[3])
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("StationID", stInfo.ulStationID);
    ReqJsonParam.Add("ZoneID", stInfo.ulZoneID);
    ReqJsonParam.Add("ZoneName", stInfo.acZoneName);
    ReqJsonParam.Add("GroupID", stInfo.i64GroupID);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCSetBrdZone, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormBrdZoneCfg::btnRefreshClick(TObject *Sender)
{
    if (!LoadBrdZoneInfo())
    {
        return;
    }

    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormBrdZoneCfg::InitView()
{
    TcxTreeListNode             *Node;
    VECTOR<BRD_ZONE_INFO_T>     &vInfo = Group::GetBrdZoneInfo();

    tlMain->BeginUpdate();
    tlMain->Clear();
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        Node = tlMain->Add();
        Node->Values[0] = vInfo[i].ulStationID;
        Node->Values[1] = vInfo[i].ulZoneID;
        Node->Values[2] = vInfo[i].acZoneName;
        Node->Values[3] = vInfo[i].i64GroupID;
        Node->Values[4] = "";
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormBrdZoneCfg::Column1GetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[0].IsNull())
    {
        return;
    }

    UINT32  ulStationID = ANode->Values[0];

    Value = Station::GetStationName(ulStationID);
}
//---------------------------------------------------------------------------
void __fastcall TFormBrdZoneCfg::TimerGetGroupMemberTimer(TObject *Sender)
{
    TcxTreeListNode     *Node = tlMain->Root->getFirstChild();

    for (;Node; Node=Node->getNextSibling())
    {
        if (!Node->Texts[4].IsEmpty())
        {
            continue;
        }

        VECTOR<UINT32>      vMember;
        VECTOR<INT64>       vGroup;
        INT64               i64GroupID = Node->Values[3];

        if (eSDK_GetGroupMember(i64GroupID, vMember, vGroup))
        {
            TimerGetGroupMember->Interval = 1;
        }
        else
        {
            TimerGetGroupMember->Interval = 3000;
        }

        return;
    }

    TimerGetGroupMember->Enabled = 10000;
}
//---------------------------------------------------------------------------

void __fastcall TFormBrdZoneCfg::FormCreate(TObject *Sender)
{
    SetImage(btnAdd, IMG_ADD);
    SetImage(btnDel, IMG_DEL);
    SetImage(btnSet, IMG_SET);
    SetImage(btnRefresh, IMG_REFRESH);
}
//---------------------------------------------------------------------------


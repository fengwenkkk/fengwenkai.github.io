//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"

#include "FrmDCUserCfg.h"
#include "DlgSetDCUser.h"
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
TFormDCUser *FormDCUser;

static void ConvertStationList(UINT32 *pulStationList, UINT32 ulStationNum, CHAR *szStationList)
{
    if (ulStationNum == 0)
    {
        *szStationList = '\0';
        return;
    }

    for (UINT32 i=0; i<ulStationNum; i++)
    {
        szStationList += sprintf(szStationList, "%s%u", i==0?"":",", pulStationList[i]);
    }
}

//---------------------------------------------------------------------------
__fastcall TFormDCUser::TFormDCUser(TComponent* Owner)
    : TSubForm(Owner)
{
    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormDCUser::btnAddClick(TObject *Sender)
{
    DialogSetDCUser->Init();
    if (DialogSetDCUser->ShowModal() != mrOk)
    {
        return;
    }

    DC_USER_INFO_T  stInfo = {0};

    DialogSetDCUser->Get(stInfo);

    GJsonParam  ReqJsonParam;
    GJson       RspJson;
    CHAR        acStationList[256];

    ConvertStationList(stInfo.aulStationList, stInfo.ulStationNum, acStationList);

    ReqJsonParam.Add("UserID", stInfo.ulUserID);
    ReqJsonParam.Add("UserName", stInfo.acName);
    ReqJsonParam.Add("DCType", stInfo.ulDCType);
    ReqJsonParam.Add("StationList", acStationList);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCAddDCUser, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormDCUser::btnDelClick(TObject *Sender)
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

    AnsiString  strUserID = Node->Texts[1];

    if (!AskMsg((CHAR*)"删除调度台%s吗？", strUserID.c_str()))
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("UserID", strUserID.c_str());

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCDelDCUser, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormDCUser::btnSetClick(TObject *Sender)
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

    UINT32  ulUserID = (UINT32)Node->Values[1];

    DC_USER_INFO_T  *pstInfo = DCUser::GetDCUser(ulUserID);

    if (!pstInfo)
    {
        return;
    }

    DC_USER_INFO_T  stInfo;

    memcpy(&stInfo, pstInfo, sizeof(stInfo));

    DialogSetDCUser->Init(stInfo);
    if (DialogSetDCUser->ShowModal() != mrOk)
    {
        return;
    }

    DC_USER_INFO_T  stNewInfo = {0};

    DialogSetDCUser->Get(stNewInfo);

    if (stInfo.ulUserID == stNewInfo.ulUserID &&
        strcmp(stInfo.acName, stNewInfo.acName) == 0 &&
        stInfo.ulDCType == stNewInfo.ulDCType &&
        stInfo.ulStationNum == stNewInfo.ulStationNum )
    {
        if (stInfo.ulStationNum == 0 ||
            memcmp(stInfo.aulStationList, stNewInfo.aulStationList,
                   stInfo.ulStationNum*sizeof(stInfo.aulStationList[0])) == 0)
        {
            return;
        }
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;
    CHAR        acStationList[256];

    ConvertStationList(stNewInfo.aulStationList, stNewInfo.ulStationNum, acStationList);

    ReqJsonParam.Add("UserID", stNewInfo.ulUserID);
    ReqJsonParam.Add("UserName", stNewInfo.acName);
    ReqJsonParam.Add("DCType", stNewInfo.ulDCType);
    ReqJsonParam.Add("StationList", acStationList);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCSetDCUser, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormDCUser::tlMainColumn5GetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[1].IsNull())
    {
        return;
    }

    UINT32  ulDCUserID = (UINT32)ANode->Values[1];
    DC_USER_INFO_T  *pstInfo = DCUser::GetDCUser(ulDCUserID);

    if (!pstInfo)
    {
        return;
    }

    Value = eSDKData::GetUserCallStatus(pstInfo->ulCallStatus);
}
//---------------------------------------------------------------------------
void __fastcall TFormDCUser::btnRefreshClick(TObject *Sender)
{
    if (!LoadDCUserInfo())
    {
        return;
    }

    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormDCUser::InitView()
{
    VECTOR<DC_USER_INFO_T*>     vUser;
    TcxTreeListNode             *Node;

    DCUser::GetDCUser(vUser);
    tlMain->Clear();
    for (UINT32 i=0; i<vUser.size(); i++)
    {
        Node = tlMain->Add();
        Node->Values[1] = vUser[i]->ulUserID;
        Node->Values[2] = vUser[i]->acName;
        Node->Values[3] = vUser[i]->ulDCType;
        Node->Values[4] = "";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormDCUser::tlP2PCallColumn1GetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[3].IsNull())
    {
        return;
    }

    UINT32  ulDCType = ANode->Values[3];
    Value = GetDCTypeText(ulDCType);
}
//---------------------------------------------------------------------------

void __fastcall TFormDCUser::FormCreate(TObject *Sender)
{
    SetImage(btnAdd, IMG_ADD);
    SetImage(btnDel, IMG_DEL);
    SetImage(btnSet, IMG_SET);
    SetImage(btnRefresh, IMG_REFRESH);
}
//---------------------------------------------------------------------------

void __fastcall TFormDCUser::ColumnZoneRangeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[1].IsNull())
    {
        return;
    }

    UINT32  ulDCUserID = (UINT32)ANode->Values[1];
    DC_USER_INFO_T  *pstInfo = DCUser::GetDCUser(ulDCUserID);

    if (!pstInfo)
    {
        return;
    }

    Value = "正线全部区间";

    if (pstInfo->ulStationNum == 0)
    {
        return;
    }

    Value = "";
    for (UINT32 i=0; i<pstInfo->ulStationNum; i++)
    {
        if (i > 0)
        {
            Value += ",";
        }

        Value += Station::GetStationName(pstInfo->aulStationList[i]);
    }
}
//---------------------------------------------------------------------------


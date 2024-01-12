//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"

#include "MsgDialog.h"
#include "msg.h"
#include "ds_public.h"
#include "ClientUtil.h"
#include "Resource.h"
#include "ClientCommon.h"
#include "RemoteMsg.h"
#include "ClientInit.h"
#include "User.h"

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
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinLilian"
#pragma link "dxSkinMcSkin"
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

extern AnsiString GetDCTypeText(UINT32 ulDCType);

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

    UINT32      ulUserID;
    AnsiString  strUserName;
    UINT32      ulDCType;

    DialogSetDCUser->Get(ulUserID, strUserName, ulDCType);

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("UserID", ulUserID);
    ReqJsonParam.Add("UserName", strUserName.c_str());
    ReqJsonParam.Add("DCType", ulDCType);

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

    if (!AskMsg((CHAR*)"删除调度台用户%s吗？", strUserID.c_str()))
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

    UINT32      ulUserID = (UINT32)Node->Values[1];
    AnsiString  strUserName = Node->Values[2];
    UINT32      ulDCType = (UINT32)Node->Values[3];

    DialogSetDCUser->Init(ulUserID, strUserName, ulDCType);
    if (DialogSetDCUser->ShowModal() != mrOk)
    {
        return;
    }

    DialogSetDCUser->Get(ulUserID, strUserName, ulDCType);

    if (ulUserID == (UINT32)Node->Values[1] &&
        strUserName == Node->Texts[2] &&
        ulDCType == (UINT32)Node->Values[3])
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("UserID", ulUserID);
    ReqJsonParam.Add("UserName", strUserName.c_str());
    ReqJsonParam.Add("DCType", ulDCType);

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
    DC_USER_INFO_T  *pstInfo = GetDCUserInfo(ulDCUserID);

    if (!pstInfo)
    {
        return;
    }

    Value = GetUserCallStatus(pstInfo->ulCallStatus);
}
//---------------------------------------------------------------------------
void __fastcall TFormDCUser::btnRefreshClick(TObject *Sender)
{
    if (!LoadDCUserInfo(3))
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

    GetDCUser(vUser);
    tlMain->Clear();
    for (UINT32 i=0; i<vUser.size(); i++)
    {
        Node = tlMain->Add();
        Node->Values[0] = vUser[i]->ulUserIndex;
        Node->Values[1] = vUser[i]->ulUserID;
        Node->Values[2] = vUser[i]->acName;
        Node->Values[3] = vUser[i]->ulDCType;
        Node->Values[4] = vUser[i]->ulCallStatus;
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


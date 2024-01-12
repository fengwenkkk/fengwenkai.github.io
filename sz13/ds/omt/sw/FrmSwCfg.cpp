//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"

#include "MsgDialog.h"
#include "msg.h"
#include "ds_public.h"
#include "ClientUtil.h"
#include "Resource.h"
#include "ClientCommon.h"
#include "RemoteMsg.h"
#include "ClientInit.h"

#include "FrmUserCfg.h"
#include "FrmSwCfg.h"
#include "DlgSetSw.h"
#include "FrmVersion.h"
#include "FrmSwUpdate.h"
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
#pragma link "cxPC"
#pragma link "dxBarBuiltInMenu"
#pragma resource "*.dfm"
TFormSwCfg *FormSwCfg;


extern VECTOR<OMC_SW_INFO_T>    g_vSwInfo;
extern AnsiString GetSwTypeText(UINT32 ulSwType);
extern BOOL LoadSwInfo();

//---------------------------------------------------------------------------
__fastcall TFormSwCfg::TFormSwCfg(TComponent* Owner)
    : TSubForm(Owner)
{
    pcMain->ActivePageIndex = 0;
    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormSwCfg::btnAddClick(TObject *Sender)
{
    DialogSetSw->Init();
    if (DialogSetSw->ShowModal() != mrOk)
    {
        return;
    }

    OMC_SW_INFO_T   stInfo = {0};

    DialogSetSw->Get(stInfo);

    GJsonParam  ReqJsonParam;
 // GJson       RspJson;

    ReqJsonParam.Add("sw_ver", stInfo.acSwVer);
    ReqJsonParam.Add("sw_file", stInfo.acSwFile);
    ReqJsonParam.Add("sw_type", stInfo.ulSwType);
    ReqJsonParam.Add("sw_time", stInfo.ulSwTime);

    if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_AddSwInfo, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormSwCfg::btnDelClick(TObject *Sender)
{
    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlMain->Selections[0];

    if (Node->Values[0].IsNull())
    {
        return;
    }

    AnsiString  strVer = Node->Texts[0];
    AnsiString  strFile = Node->Texts[1];

    if (!AskMsg((CHAR*)"删除版本%s吗？", strVer.c_str()))
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("sw_ver", strVer);
    ReqJsonParam.Add("sw_file", strFile);

    if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_DelSwInfo, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormSwCfg::btnRefreshClick(TObject *Sender)
{
    if (!LoadSwInfo())
    {
        return;
    }

    FormSwUpdate->UpdateSwInfo();

    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormSwCfg::InitView()
{
    TcxTreeListNode         *Node;

    tlMain->BeginUpdate();
    tlMain->Clear();
    for (UINT32 i=0; i<g_vSwInfo.size(); i++)
    {
        Node = tlMain->Add();
        Node->Values[0] = g_vSwInfo[i].acSwVer;
        Node->Values[1] = g_vSwInfo[i].acSwFile;
        Node->Values[2] = g_vSwInfo[i].ulSwType;
        Node->Values[3] = GetTextTime(g_vSwInfo[i].ulSwTime);
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormSwCfg::ColumnTypeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulSwType = ANode->Values[2];

    Value = GetSwTypeText(ulSwType);
}
//---------------------------------------------------------------------------
void __fastcall TFormSwCfg::FormCreate(TObject *Sender)
{
    SetImage(btnAdd, IMG_ADD);
    SetImage(btnDel, IMG_DEL);
    SetImage(btnRefresh, IMG_REFRESH);
}
//---------------------------------------------------------------------------
void __fastcall TFormSwCfg::FormShow(TObject *Sender)
{
    //FormSwUpdate->gbMain->Parent = tsSwUpdate;
    //FormUserCfg->UserPanel->Parent = tsUserCfg;
    //FormVersion->PanelVersion->Parent = tsUserCfg;
}
//---------------------------------------------------------------------------


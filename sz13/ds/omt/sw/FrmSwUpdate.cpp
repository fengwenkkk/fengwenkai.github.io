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

#include "FrmSwUpdate.h"
#include "DlgSetSw.h"
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
#pragma link "cxMaskEdit"
#pragma resource "*.dfm"
TFormSwUpdate *FormSwUpdate;


extern VECTOR<OMC_SW_UPDATE_INFO_T>     g_vSwUpdateInfo;
extern VECTOR<OMC_SW_INFO_T>            g_vSwInfo;

extern AnsiString GetSwTypeText(UINT32 ulSwType);

extern BOOL LoadSwInfo();
extern BOOL GetSwUpdateInfo();

extern Dict g_DictSwType;

OMC_SW_UPDATE_INFO_T* GetSwUpdateInfo(CHAR *szNeID)
{
    for (UINT32 i=0; i<g_vSwUpdateInfo.size(); i++)
    {
        if (strcmp(g_vSwUpdateInfo[i].acNeID, szNeID) == 0)
        {
            return &g_vSwUpdateInfo[i];
        }
    }

    return NULL;
}

//---------------------------------------------------------------------------
__fastcall TFormSwUpdate::TFormSwUpdate(TComponent* Owner)
    : TSubForm(Owner)
{
    g_DictSwType.Init(cbSwType);
    cbSwType->ItemIndex = 0;

    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormSwUpdate::btnUpdateClick(TObject *Sender)
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

    AnsiString  strNewVer = cbSwVer->Text;

    if (strNewVer.IsEmpty())
    {
        WarnMsg("请选择要升级的版本");
        return;
    }

    AnsiString      strNeID;
    AnsiString      strVer;
    GJsonParam      ReqJsonParam;
    GJsonParam      JsonParam;
    GJsonTupleParam JsonTupleParam;
    GJson           RspJson;

    for (int i=0; i<tlMain->SelectionCount; i++)
    {
        strNeID = tlMain->Selections[i]->Texts[0];
        strVer = tlMain->Selections[i]->Texts[2];
        if (strVer != strNewVer)
        {
            JsonParam.Clear();
            JsonParam.Add("ne_id", strNeID);
            JsonTupleParam.Add(JsonParam);
        }
    }

    if (JsonTupleParam.GetTupleNum() == 0)
    {
        return;
    }

    ReqJsonParam.Add("sw_ver", strNewVer);
    ReqJsonParam.Add("ne_list", JsonTupleParam);

    if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_UpdateSw, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormSwUpdate::btnUpdateAllClick(TObject *Sender)
{
    AnsiString  strNewVer = cbSwVer->Text;

    if (strNewVer.IsEmpty())
    {
        WarnMsg("请选择批量升级设备的类型");
        return;
    }

    AnsiString      strNeID;
    AnsiString      strVer;
    GJsonParam      ReqJsonParam;
    GJsonParam      JsonParam;
    GJsonTupleParam JsonTupleParam;
    GJson           RspJson;

    for (int i=0; i<tlMain->AbsoluteCount; i++)
    {
        strNeID = tlMain->AbsoluteItems[i]->Texts[0];
        strVer = tlMain->AbsoluteItems[i]->Texts[2];
        if (strVer != strNewVer)
        {
            JsonParam.Clear();
            JsonParam.Add("ne_id", strNeID);
            JsonTupleParam.Add(JsonParam);
        }
    }

    if (JsonTupleParam.GetTupleNum() == 0)
    {
        return;
    }

    ReqJsonParam.Add("sw_ver", strNewVer);
    ReqJsonParam.Add("ne_list", JsonTupleParam);

    if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_UpdateSw, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormSwUpdate::btnRefreshClick(TObject *Sender)
{
    if (!GetSwUpdateInfo())
    {
        return;
    }

    if (!LoadSwInfo())
    {
        return;
    }

    cbSwTypePropertiesChange(this);

    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormSwUpdate::UpdateSwInfo()
{
    cbSwTypePropertiesChange(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormSwUpdate::InitView()
{
    TcxTreeListNode         *Node;
    OMC_SW_UPDATE_INFO_T    *pstUpdateInfo;
    NE_BASIC_INFO_T         *pstNeBasicInfo;
    UINT32                  ulSwType;
    VectorMap<AnsiString, NE_BASIC_INFO_T>  mNEBasicInfo;
    NEBasic::GetNEBasicInfo(mNEBasicInfo);

    g_DictSwType.GetID(&ulSwType);

    tlMain->BeginUpdate();
    tlMain->Clear();
    for (UINT32 i=0; i<mNEBasicInfo.Size(); i++)
    {
        pstNeBasicInfo = mNEBasicInfo[i];
        if ( *(pstNeBasicInfo->acDevName) == '\0')
        {
            continue;
        }

        pstUpdateInfo = GetSwUpdateInfo(pstNeBasicInfo->acNeID);

        if ((ulSwType == SW_TYPE_TX &&
             pstNeBasicInfo->ulDevType == OMC_DEV_TYPE_TX) ||
            (ulSwType == SW_TYPE_FX &&
             pstNeBasicInfo->ulDevType == OMC_DEV_TYPE_FX) )
        {
            Node = tlMain->Add();
            Node->Values[0] = pstNeBasicInfo->acNeID;
            Node->Values[1] = pstNeBasicInfo->acDevName;
            Node->Values[2] = pstNeBasicInfo->acHardwareVer;

            if (pstUpdateInfo &&
                strcmp(pstNeBasicInfo->acHardwareVer, pstUpdateInfo->acNewSwVer) != 0)
            {
                Node->Values[3] = pstUpdateInfo->acNewSwVer;
            }
            else
            {
                Node->Values[3] = "";
            }
        }
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormSwUpdate::FormCreate(TObject *Sender)
{
    SetImage(btnRefresh, IMG_REFRESH);
}
//---------------------------------------------------------------------------
void __fastcall TFormSwUpdate::cbSwTypePropertiesChange(TObject *Sender)
{
    InitView();

    UINT32                  ulSwType;

    g_DictSwType.GetID(&ulSwType);
    cbSwVer->Properties->Items->Clear();

    for (UINT32 i=0; i<g_vSwInfo.size(); i++)
    {
        if (g_vSwInfo[i].ulSwType == ulSwType)
        {
            cbSwVer->Properties->Items->Add(g_vSwInfo[i].acSwVer);
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormSwUpdate::ColumnSetFlagGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[4].IsNull())
    {
        return;
    }

    String  strVer = ANode->Values[2];
    String  strNewVer = ANode->Values[3];

    if (!strVer.IsEmpty() &&
        strVer == strNewVer)
    {
        Value = "";
        return;
    }

    BOOL    bSetFlag = ANode->Values[4];

    if (!bSetFlag)
    {
        Value = "命令未下方";
    }
    else
    {
        Value = "升级中";
    }
}
//---------------------------------------------------------------------------


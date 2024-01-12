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

#include "FrmOperLog.h"
#include "DlgSetTrainUE.h"
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
TFormOperLog *FormOperLog;

extern BOOL GetDevOperLog(AnsiString strNeID, UINT32 ulStartTime, UINT32 ulEndTime, VECTOR<OMA_OPERLOG_T> &vLog);

//---------------------------------------------------------------------------
__fastcall TFormOperLog::TFormOperLog(TComponent* Owner)
    : TSubForm(Owner)
{
    //InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormOperLog::btnRefreshClick(TObject *Sender)
{
    UINT32  ulStartTime = 0;
    UINT32  ulEndTime = 0xffffffff;
    VECTOR<OMA_OPERLOG_T> vLog;

    if (!GetDevOperLog(m_strNeID, ulStartTime, ulEndTime, vLog))
    {
        return;
    }

    ShowLog(vLog);
}
//---------------------------------------------------------------------------
void __fastcall TFormOperLog::Init(AnsiString strNeID)
{
    m_strNeID = strNeID;

    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormOperLog::ShowLog(VECTOR<OMA_OPERLOG_T> &vLog)
{
    TcxTreeListNode             *Node;
    UINT32          j;

    tlInfo->BeginUpdate();
    tlInfo->Clear();

    for (UINT32 i=0; i<vLog.size(); i++)
    {
        Node = tlInfo->Add();

        Node->Values[0] = GetTextTime(vLog[i].ulTime);
        Node->Values[1] = vLog[i].acLogInfo;
    }

    tlInfo->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormOperLog::FormCreate(TObject *Sender)
{
    SetImage(btnRefresh, IMG_REFRESH);
    SetImage(btnClose, IMG_CLOSE);
}
//---------------------------------------------------------------------------
void __fastcall TFormOperLog::btnCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormOperLog::FormClose(TObject *Sender, TCloseAction &Action)
{
    Action = caHide;
}


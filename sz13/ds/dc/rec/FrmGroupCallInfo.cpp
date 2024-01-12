//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmGroupCallInfo.h"
#include "main.h"

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
#pragma link "cxCalendar"
#pragma link "cxDateUtils"
#pragma link "cxMaskEdit"
#pragma link "dxCore"
#pragma link "cxPC"
#pragma link "cxSpinEdit"
#pragma link "cxTimeEdit"
#pragma link "dxBarBuiltInMenu"
#pragma link "cxClasses"
#pragma link "cxLocalization"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma resource "*.dfm"
TFormGroupCallInfo *FormGroupCallInfo;

static ID_VALUE_T   s_astGroupCallType[] = {
                                   {CALL_TYPE_UNKNOWN,          "全部"},
                                   {CALL_TYPE_PTT,              "组呼"},
                                   {CALL_TYPE_EMERGENCY_CALL,   "紧急呼叫"},
                                   {CALL_TYPE_VIDEO_PTT,        "视频组呼"},
                                   };

static Dict g_DictGroupCallType(s_astGroupCallType, ARRAY_SIZE(s_astGroupCallType));

//---------------------------------------------------------------------------
__fastcall TFormGroupCallInfo::TFormGroupCallInfo(TComponent* Owner)
    : TSubForm(Owner)
{
    g_DictGroupCallType.Init(cbCallType);
    g_DictGroupCallType.SetID(CALL_TYPE_UNKNOWN);

    m_ulStartTime = 0;
    m_ulEndTime   = gos_get_current_time();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallInfo::btnQueryClick(TObject *Sender)
{
    UINT32  ulStartTime = 0;
    UINT32  ulEndTime = gos_get_current_time();
    UINT32  ulCallType = CALL_TYPE_UNKNOWN;
    UINT32  ulDCUserID = INVALID_USER_ID;
    AnsiString  strTmp;
    INT64       i64GroupID = INVALID_GROUP_ID;

    g_DictGroupCallType.GetID(cbCallType, &ulCallType);

    if (!deStartTime->Text.IsEmpty())
    {
        ulStartTime = GetTime(deStartTime->Date);
    }

    if (!deEndTime->Text.IsEmpty())
    {
        ulEndTime = GetTime(deEndTime->Date);
    }

    strTmp = edtResourceID->Text.Trim();
    if (!strTmp.IsEmpty())
    {
        if (!gos_atoi64(strTmp.c_str(), &i64GroupID) )
        {
            edtResourceID->SetFocus();
            InfoMsg("号码非法");
            return;
        }
    }

    VECTOR<GROUP_CALL_INFO_T>   vInfo;

    if (!LoadGroupCallInfo(ulStartTime, ulEndTime, ulCallType, ulDCUserID, i64GroupID, vInfo))
    {
        InfoMsg("查询失败");
        return;
    }

    InitView(vInfo);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallInfo::InitView(VECTOR<GROUP_CALL_INFO_T> &vInfo)
{
    TcxTreeListNode     *Node;
    GROUP_CALL_INFO_T   *pstInfo;
    BOOL                bDesc = FALSE;

    if (vInfo.size() == 0)
    {
        btnNext->Enabled = false;
        btnPrev->Enabled = false;
    }
    else
    {
        btnNext->Enabled = true;
        btnPrev->Enabled = true;

        if (vInfo[0].ulTime < vInfo[vInfo.size()-1].ulTime)
        {
            m_ulStartTime = vInfo[0].ulTime;
            m_ulEndTime   = vInfo[vInfo.size()-1].ulTime;
        }
        else
        {
            bDesc = TRUE;
            m_ulEndTime   = vInfo[0].ulTime;
            m_ulStartTime = vInfo[vInfo.size()-1].ulTime;
        }
    }

    tlMain->BeginUpdate();
    tlMain->Clear();

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        if (bDesc)
        {
            pstInfo = &vInfo[vInfo.size()-1-i];
        }
        else
        {
            pstInfo = &vInfo[i];
        }

        Node = tlMain->Add();
        Node->Values[0] = i+1;
        Node->Values[1] = GetTextTime(pstInfo->ulTime);
        Node->Values[2] = pstInfo->ulCallType;
        Node->Values[3] = pstInfo->i64GroupID;
        Node->Values[4] = pstInfo->ulSpeaker;
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallInfo::ColumnCallTypeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    if (!ANode || ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulCallType = ANode->Values[2];

    Value = g_DictGroupCallType.GetValue(ulCallType);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallInfo::btnNextClick(TObject *Sender)
{
    deStartTime->Date = GetDateTime(m_ulEndTime);
    deEndTime->Clear();

    btnQueryClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallInfo::btnPrevClick(TObject *Sender)
{
    deStartTime->Clear();
    deEndTime->Date = GetDateTime(m_ulStartTime);

    btnQueryClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallInfo::FormCreate(TObject *Sender)
{
    SetImage(btnQuery,  IMG_QUERY);
    SetImage(btnNext,   IMG_NEXT);
    SetImage(btnPrev,   IMG_PREV);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmRealBrdInfo.h"
#include "main.h"
#include "FrmCallIn.h"

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
#pragma link "cxImageComboBox"
#pragma resource "*.dfm"
TFormRealBrdInfo *FormRealBrdInfo;

//---------------------------------------------------------------------------
__fastcall TFormRealBrdInfo::TFormRealBrdInfo(TComponent* Owner)
    : TSubForm(Owner)
{
    m_ulStartTime = 0;
    m_ulEndTime   = gos_get_current_time();
}
//---------------------------------------------------------------------------
void __fastcall TFormRealBrdInfo::btnQueryClick(TObject *Sender)
{
    UINT32  ulStartTime = 0;
    UINT32  ulEndTime = gos_get_current_time();

    if (!deStartTime->Text.IsEmpty())
    {
        ulStartTime = GetTime(deStartTime->Date);
    }

    if (!deEndTime->Text.IsEmpty())
    {
        ulEndTime = GetTime(deEndTime->Date);
    }

    VECTOR<REAL_BRD_INFO_T>     vInfo;

    if (!LoadRealBrdInfo(ulStartTime, ulEndTime, vInfo))
    {
        InfoMsg("≤È—Ø ß∞‹");
        return;
    }

    InitView(vInfo);
}
//---------------------------------------------------------------------------
void __fastcall TFormRealBrdInfo::InitView(VECTOR<REAL_BRD_INFO_T> &vInfo)
{
    TcxTreeListNode     *Node;
    REAL_BRD_INFO_T     *pstInfo;
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

        if (vInfo[0].ulStartTime < vInfo[vInfo.size()-1].ulStartTime)
        {
            m_ulStartTime = vInfo[0].ulStartTime;
            m_ulEndTime   = vInfo[vInfo.size()-1].ulStartTime;
        }
        else
        {
            bDesc = TRUE;
            m_ulEndTime   = vInfo[0].ulStartTime;
            m_ulStartTime = vInfo[vInfo.size()-1].ulStartTime;
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
        Node->Values[1] = GetTextTime(pstInfo->ulStartTime);
        Node->Values[2] = GetTextTime(pstInfo->ulEndTime);
        Node->Values[3] = pstInfo->ulDCUserID;
        Node->Values[4] = pstInfo->acTrainList;
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormRealBrdInfo::btnNextClick(TObject *Sender)
{
    deStartTime->Date = GetDateTime(m_ulEndTime);
    deEndTime->Clear();

    btnQueryClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormRealBrdInfo::btnPrevClick(TObject *Sender)
{
    deStartTime->Clear();
    deEndTime->Date = GetDateTime(m_ulStartTime);

    btnQueryClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormRealBrdInfo::FormCreate(TObject *Sender)
{
    SetImage(btnQuery,  IMG_QUERY);
    SetImage(btnNext,   IMG_NEXT);
    SetImage(btnPrev,   IMG_PREV);
}
//---------------------------------------------------------------------------

void __fastcall TFormRealBrdInfo::FormShow(TObject *Sender)
{
    FormCallIn->PauseVoiceAlarm();
}
//---------------------------------------------------------------------------

void __fastcall TFormRealBrdInfo::FormDeactivate(TObject *Sender)
{
    FormCallIn->ResumeVoiceAlarm();
}
//---------------------------------------------------------------------------


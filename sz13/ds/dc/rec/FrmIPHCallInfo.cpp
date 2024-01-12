//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmIPHCallInfo.h"
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
TFormIPHCallInfo *FormIPHCallInfo;

//---------------------------------------------------------------------------
__fastcall TFormIPHCallInfo::TFormIPHCallInfo(TComponent* Owner)
    : TSubForm(Owner)
{
    m_ulStartTime = 0;
    m_ulEndTime   = gos_get_current_time();
}
//---------------------------------------------------------------------------
void __fastcall TFormIPHCallInfo::btnQueryClick(TObject *Sender)
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

    VECTOR<IPH_TALK_INFO_T>     vInfo;

    if (!LoadIPHTalkInfo(ulStartTime, ulEndTime, vInfo))
    {
        InfoMsg("≤È—Ø ß∞‹");
        return;
    }

    InitView(vInfo);
}
//---------------------------------------------------------------------------
void __fastcall TFormIPHCallInfo::InitView(VECTOR<IPH_TALK_INFO_T> &vInfo)
{
    TcxTreeListNode     *Node;
    IPH_TALK_INFO_T     *pstInfo;
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

        if (vInfo[0].ulApplyTime < vInfo[vInfo.size()-1].ulApplyTime)
        {
            m_ulStartTime = vInfo[0].ulApplyTime;
            m_ulEndTime   = vInfo[vInfo.size()-1].ulApplyTime;
        }
        else
        {
            bDesc = TRUE;
            m_ulEndTime   = vInfo[0].ulApplyTime;
            m_ulStartTime = vInfo[vInfo.size()-1].ulApplyTime;
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
        Node->Values[1] = GetTextTime(pstInfo->ulApplyTime);
        Node->Values[2] = pstInfo->ulTrainUnitID;
        Node->Values[3] = pstInfo->acTrainID;
        Node->Values[4] = pstInfo->ulCarriageID;
        Node->Values[5] = pstInfo->ulIPHDevID;
        Node->Values[6] = pstInfo->ulAcceptDCUserID==INVALID_USER_ID?AnsiString(""):AnsiString(pstInfo->ulAcceptDCUserID);
        Node->Values[7] = pstInfo->ulAcceptTime==0?AnsiString(""):GetTextTime(pstInfo->ulAcceptTime);
        Node->Values[8] = pstInfo->ulStopTime==0?AnsiString(""):GetTextTime(pstInfo->ulStopTime);
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormIPHCallInfo::btnNextClick(TObject *Sender)
{
    deStartTime->Date = GetDateTime(m_ulEndTime);
    deEndTime->Clear();

    btnQueryClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormIPHCallInfo::btnPrevClick(TObject *Sender)
{
    deStartTime->Clear();
    deEndTime->Date = GetDateTime(m_ulStartTime);

    btnQueryClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormIPHCallInfo::FormCreate(TObject *Sender)
{
    SetImage(btnQuery,  IMG_QUERY);
    SetImage(btnNext,   IMG_NEXT);
    SetImage(btnPrev,   IMG_PREV);
}
//---------------------------------------------------------------------------


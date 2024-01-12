//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmReqCallInfo.h"
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
TFormReqCallInfo *FormReqCallInfo;

ID_VALUE_T  s_astReqCallType[] = { {INVALID_REQ_CALL,           "全部"},
                                   {TRAIN_REQ_CALL_DC,          "列车请呼调度"},
                                   {TRAIN_REQ_CALL_STATION,     "列车请呼车站"},
                                   {TRAIN_EMERGENCY_REQ_CALL_DC,"列车请呼调度"},
                                   {STATION_REQ_CALL_DC,        "车站请呼调度"},
                                   {STATION_REQ_CALL_TRAIN,     "车站请呼列车"},
                                   {STATION_EMERGENCY_REQ_CALL_DC, "车站紧急请呼调度"},
                                   };

Dict    g_DictReqCallType(s_astReqCallType, ARRAY_SIZE(s_astReqCallType));

//---------------------------------------------------------------------------
__fastcall TFormReqCallInfo::TFormReqCallInfo(TComponent* Owner)
    : TSubForm(Owner)
{
    g_DictReqCallType.Init(cbReqCallType);
    g_DictReqCallType.SetID(INVALID_REQ_CALL);

    m_ulStartTime = 0;
    m_ulEndTime   = gos_get_current_time();

    deEndTime->Date = Now();
}
//---------------------------------------------------------------------------
void __fastcall TFormReqCallInfo::btnQueryClick(TObject *Sender)
{
    UINT32  ulStartTime = 0;
    UINT32  ulEndTime = gos_get_current_time();
    UINT32  ulReqCallType;
    AnsiString  strTmp;
 // INT64   i64ResourceID = INVALID_GROUP_ID;

    g_DictReqCallType.GetID(cbReqCallType, &ulReqCallType);

    if (!deStartTime->Text.IsEmpty())
    {
        ulStartTime = GetTime(deStartTime->Date);
    }

    if (!deEndTime->Text.IsEmpty())
    {
        ulEndTime = GetTime(deEndTime->Date);
    }
 /*
    strTmp = edtResourceID->Text.Trim();
    if (!strTmp.IsEmpty())
    {
        if (!gos_atoi64(strTmp.c_str(), &i64ResourceID) ||
            i64ResourceID <= 0)
        {
            edtResourceID->SetFocus();
            InfoMsg("号码非法");
            return;
        }
    }  */

    VECTOR<REQ_CALL_INFO_T>     vInfo;

    if (!LoadReqCallInfo(ulStartTime, ulEndTime, ulReqCallType, vInfo))
    {
        InfoMsg("查询失败");
        return;
    }

    InitView(vInfo);
}
//---------------------------------------------------------------------------
void __fastcall TFormReqCallInfo::InitView(VECTOR<REQ_CALL_INFO_T> &vInfo)
{
    TcxTreeListNode     *Node;
    REQ_CALL_INFO_T     *pstInfo;
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

        if (vInfo[0].ulReqTime < vInfo[vInfo.size()-1].ulReqTime)
        {
            m_ulStartTime = vInfo[0].ulReqTime;
            m_ulEndTime   = vInfo[vInfo.size()-1].ulReqTime;
        }
        else
        {
            bDesc = TRUE;
            m_ulEndTime   = vInfo[0].ulReqTime;
            m_ulStartTime = vInfo[vInfo.size()-1].ulReqTime;
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
        Node->Values[1] = GetTextTime(pstInfo->ulReqTime);
        Node->Values[2] = pstInfo->ulReqCallType;
        Node->Values[3] = pstInfo->ulReqUserID;
        Node->Values[4] = pstInfo->ulCaller;
        Node->Values[5] = pstInfo->ulDCUserID;
        Node->Values[6] = GetTextTime(pstInfo->ulAcceptTime);
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormReqCallInfo::ColumnReqCallTypeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    if (!ANode || ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulCallType = ANode->Values[2];

    Value = g_DictReqCallType.GetValue(ulCallType);
}
//---------------------------------------------------------------------------
void __fastcall TFormReqCallInfo::btnNextClick(TObject *Sender)
{
    deStartTime->Date = GetDateTime(m_ulEndTime);
    deEndTime->Clear();

    btnQueryClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormReqCallInfo::btnPrevClick(TObject *Sender)
{
    deStartTime->Clear();
    deEndTime->Date = GetDateTime(m_ulStartTime);

    btnQueryClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormReqCallInfo::FormCreate(TObject *Sender)
{
    SetImage(btnQuery,  IMG_QUERY);
    SetImage(btnNext,   IMG_NEXT);
    SetImage(btnPrev,   IMG_PREV);
}
//---------------------------------------------------------------------------
void __fastcall TFormReqCallInfo::ColumnCallerGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (!ANode ||
        ANode->Values[2].IsNull() ||
        ANode->Values[4].IsNull() )
    {
        return;
    }

    UINT32  ulCallType = ANode->Values[2];
    UINT32  ulCaller = ANode->Values[4];

    if (ulCallType == TRAIN_REQ_CALL_DC ||
        ulCallType == TRAIN_REQ_CALL_STATION )
    {
        Value = AnsiString("列车") + AnsiString(ulCaller);
    }
    else if (ulCallType == STATION_REQ_CALL_DC ||
             ulCallType == STATION_REQ_CALL_TRAIN)
    {
        Value = GetStationName(ulCaller);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormReqCallInfo::ColumnDCUserIDGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (!ANode || ANode->Values[5].IsNull())
    {
        return;
    }

    UINT32  ulUserID = ANode->Values[5];

    if (ulUserID == INVALID_USER_ID)
    {
        Value = "";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormReqCallInfo::ColumnAcceptTimeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (!ANode || ANode->Values[5].IsNull())
    {
        return;
    }

    UINT32  ulUserID = ANode->Values[5];

    if (ulUserID == INVALID_USER_ID)
    {
        Value = "";
    }
}
//---------------------------------------------------------------------------


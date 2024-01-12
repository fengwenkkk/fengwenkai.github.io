//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmP2PCallInfo.h"
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
TFormP2PCallInfo *FormP2PCallInfo;

static ID_VALUE_T  s_astP2PCallType[] = {
                                        {CALL_TYPE_UNKNOWN,         "全部"},
                                        {CALL_TYPE_P2P,             "个呼"},
                                        {CALL_TYPE_P2P_TRANSFER,    "个呼转接"},
                                        {CALL_TYPE_VIDEO_P2P,       "视频个呼"},
                                        {CALL_TYPE_VIDEO_REALPLAY,  "视频上拉"},
                                        {CALL_TYPE_VIDEO_RECV,      "视频回传"},
                                        {CALL_TYPE_VIDEO_DISPATCH,  "视频分发"},
                                        };

static Dict g_DictP2PCallType(s_astP2PCallType, ARRAY_SIZE(s_astP2PCallType));

//---------------------------------------------------------------------------
__fastcall TFormP2PCallInfo::TFormP2PCallInfo(TComponent* Owner)
    : TSubForm(Owner)
{
    g_DictP2PCallType.Init(cbCallType);
    g_DictP2PCallType.SetID(CALL_TYPE_UNKNOWN);

    m_ulStartTime = 0;
    m_ulEndTime   = gos_get_current_time();
}
//---------------------------------------------------------------------------
void __fastcall TFormP2PCallInfo::btnQueryClick(TObject *Sender)
{
    UINT32  ulStartTime = 0;
    UINT32  ulEndTime = gos_get_current_time();
    UINT32  ulCallType = CALL_TYPE_UNKNOWN;
    AnsiString  strTmp;
    UINT32      ulUserID = INVALID_USER_ID;

    g_DictP2PCallType.GetID(cbCallType, &ulCallType);

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
        if (!gos_atou(strTmp.c_str(), &ulUserID) )
        {
            edtResourceID->SetFocus();
            InfoMsg("号码非法");
            return;
        }
    }

    VECTOR<P2P_CALL_INFO_T>     vInfo;

    if (!LoadP2PCallInfo(ulStartTime, ulEndTime, ulCallType, ulUserID, vInfo))
    {
        InfoMsg("查询失败");
        return;
    }

    InitView(vInfo);
}
//---------------------------------------------------------------------------
void __fastcall TFormP2PCallInfo::InitView(VECTOR<P2P_CALL_INFO_T> &vInfo)
{
    TcxTreeListNode     *Node;
    P2P_CALL_INFO_T     *pstInfo;
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
        Node->Values[3] = pstInfo->ulCaller;
        Node->Values[4] = pstInfo->ulCallee;
        Node->Values[5] = pstInfo->ulStatus;
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormP2PCallInfo::ColumnCallTypeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    if (!ANode || ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulCallType = ANode->Values[2];

    Value = g_DictP2PCallType.GetValue(ulCallType);
}
//---------------------------------------------------------------------------
void __fastcall TFormP2PCallInfo::btnNextClick(TObject *Sender)
{
    deStartTime->Date = GetDateTime(m_ulEndTime);
    deEndTime->Clear();

    btnQueryClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormP2PCallInfo::btnPrevClick(TObject *Sender)
{
    deStartTime->Clear();
    deEndTime->Date = GetDateTime(m_ulStartTime);

    btnQueryClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormP2PCallInfo::FormCreate(TObject *Sender)
{
    SetImage(btnQuery,  IMG_QUERY);
    SetImage(btnNext,   IMG_NEXT);
    SetImage(btnPrev,   IMG_PREV);
}
//---------------------------------------------------------------------------

void __fastcall TFormP2PCallInfo::ColumnStatusGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (!ANode || ANode->Values[5].IsNull())
    {
        return;
    }

    UINT32      ulStatus = ANode->Values[5];

    Value = GetP2PCallStatusText(ulStatus);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmEmergencyCallInfo.h"
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
TFormEmergencyCallInfo *FormEmergencyCallInfo;

static ID_VALUE_T   s_astCDRType[] = {
                        {CDR_TYPE_P2P,  "个呼"},
                        {CDR_TYPE_PTT,  "组呼"}};

static Dict g_DictCDRType(s_astCDRType, ARRAY_SIZE(s_astCDRType));

static AnsiString GetCallUserText(UINT32 ulUserID)
{
    if (ulUserID == INVALID_USER_ID)
    {
        return "";
    }

    return ulUserID;
}

//---------------------------------------------------------------------------
__fastcall TFormEmergencyCallInfo::TFormEmergencyCallInfo(TComponent* Owner)
    : TSubForm(Owner)
{
    g_DictCDRType.Init(cbCDRType);
    g_DictCDRType.SetID(CDR_TYPE_P2P);

    pcMain->ActivePageIndex = 0;

    m_ulStartTime = 0;
    m_ulEndTime   = gos_get_current_time();

  //    deEndTime->Date = Now();
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCallInfo::InitView(VECTOR<GROUP_CALL_INFO_T> &vInfo)
{
    TcxTreeListNode     *Node;
    GROUP_CALL_INFO_T   *pstInfo;
    BOOL                bDesc = FALSE;

    if (vInfo.size() == 0)
    {
        btnNext->Enabled = false;
        btnPrev->Enabled = false;
        btnExport->Enabled = false;
        btnPrint->Enabled = false;
    }
    else
    {
        btnNext->Enabled = true;
        btnPrev->Enabled = true;
        btnExport->Enabled = true;
        btnPrint->Enabled = true;

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
    tlMain->Columns[3]->Visible = true;
    tlMain->Columns[4]->Visible = true;
    tlMain->Columns[5]->Visible = true;
    tlMain->Columns[6]->Visible = false;
    tlMain->Columns[7]->Visible = false;

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
        Node->Values[1] = pstInfo->ulTime;
        Node->Values[2] = pstInfo->ulCallType;
        Node->Values[3] = pstInfo->i64GroupID;
        Node->Values[4] = Group::GetGroupName(pstInfo->i64GroupID);
        Node->Values[5] = pstInfo->ulSpeaker;
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCallInfo::InitView(VECTOR<P2P_CALL_INFO_T> &vInfo)
{
    TcxTreeListNode     *Node;
    P2P_CALL_INFO_T     *pstInfo;
    BOOL                bDesc = FALSE;

    if (vInfo.size() == 0)
    {
        btnNext->Enabled = false;
        btnPrev->Enabled = false;
        btnExport->Enabled = false;
        btnPrint->Enabled = false;
    }
    else
    {
        btnNext->Enabled = true;
        btnPrev->Enabled = true;
        btnExport->Enabled = true;
        btnPrint->Enabled = true;

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
    tlMain->Columns[3]->Visible = false;
    tlMain->Columns[4]->Visible = false;
    tlMain->Columns[5]->Visible = false;
    tlMain->Columns[6]->Visible = true;
    tlMain->Columns[7]->Visible = true;

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
        Node->Values[1] = pstInfo->ulTime;
        Node->Values[2] = pstInfo->ulCallType;
        Node->Values[6] = pstInfo->ulCaller;
        Node->Values[7] = pstInfo->ulCallee;
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCallInfo::btnExportClick(TObject *Sender)
{
    if (!SaveDialog->Execute())
    {
        return;
    }

    AnsiString  strFile = SaveDialog->FileName;
    CHAR        *szFilePostfix = gos_get_file_postfix(strFile.c_str());
    bool        bRet;

    if (strcasecmp(szFilePostfix, "xls") == 0)
    {
        bRet = ExportTreeListToExcel(tlMain, strFile);
    }
    else if (strcasecmp(szFilePostfix, "csv") == 0)
    {
        bRet = ExportTreeListToCsv(tlMain, strFile);
    }
    else
    {
        InfoMsg("无效的导出文件格式");
        return;
    }

    if (!bRet)
    {
        ErrMsg("导出失败");
    }
    else
    {
        InfoMsg("导出成功");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCallInfo::cxTreeListColumn9GetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (!ANode || ANode->Values[1].IsNull())
    {
        return;
    }

    UINT32  ulCallType = ANode->Values[1];

    Value = GetCallTypeText(ulCallType, 0);
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCallInfo::tlP2PColumn5GetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    if (!ANode || ANode->Values[4].IsNull())
    {
        return;
    }

    UINT32  ulCallStatus = ANode->Values[4];

    Value = GetP2PCallStatusText(ulCallStatus);
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCallInfo::cxTreeListColumn4GetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (!ANode || ANode->Values[1].IsNull())
    {
        return;
    }

    UINT32  ulCallType = ANode->Values[1];

    Value = GetCallTypeText(ulCallType, 0);
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCallInfo::tlMainColumn5GetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    if (!ANode || ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulCallType = ANode->Values[2];

    Value = GetCallTypeText(ulCallType, 0);
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCallInfo::Column2GetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    if (!ANode || ANode->Values[1].IsNull())
    {
        return;
    }

    UINT32  ulTime = ANode->Values[1];

    Value = GetTextTime(ulTime);
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCallInfo::NewP2PCall(UINT32 ulCallType, UINT32 ulCaller, UINT32 ulCallee, UINT32 ulStatus)
{
    tlP2PCall->BeginUpdate();

    TcxTreeListNode *Node = tlP2PCall->Add();

    Node->Values[0] = GetTimeText();
    Node->Values[1] = ulCallType;
    Node->Values[2] = ulCaller;
    Node->Values[3] = ulCallee;
    Node->Values[4] = ulStatus;

    if (tlP2PCall->AbsoluteCount > 1000)
    {
        tlP2PCall->AbsoluteItems[0]->Delete();
    }

    tlP2PCall->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCallInfo::NewGroupCall(UINT32 ulCallType, INT64 i64GroupID, UINT32 ulSpeaker)
{
    tlGroupCall->BeginUpdate();

    TcxTreeListNode *Node = tlGroupCall->Add();

    Node->Values[0] = GetTimeText();
    Node->Values[1] = ulCallType;
    Node->Values[2] = i64GroupID;
    Node->Values[3] = Group::GetGroupName(i64GroupID);
    Node->Values[4] = ulSpeaker;

    if (tlGroupCall->AbsoluteCount > 1000)
    {
        tlGroupCall->AbsoluteItems[0]->Delete();
    }

    tlGroupCall->EndUpdate();
}

void __fastcall TFormEmergencyCallInfo::btnPrintClick(TObject *Sender)
{
    ::Print(tlMain, "呼叫记录");
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCallInfo::btnNextClick(TObject *Sender)
{
    deStartTime->Date = GetDateTime(m_ulEndTime);
    deEndTime->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCallInfo::btnPrevClick(TObject *Sender)
{
    deStartTime->Clear();
    deEndTime->Date = GetDateTime(m_ulStartTime);
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCallInfo::FormCreate(TObject *Sender)
{
    SetImage(btnQuery,  IMG_QUERY);
    SetImage(btnExport, IMG_EXPORT);
    SetImage(btnPrint,  IMG_PRINT);
    SetImage(btnNext,   IMG_NEXT);
    SetImage(btnPrev,   IMG_PREV);
}
//---------------------------------------------------------------------------

void __fastcall TFormEmergencyCallInfo::TimerInitTimer(TObject *Sender)
{
    TimerInit->Enabled = false;
}
//---------------------------------------------------------------------------


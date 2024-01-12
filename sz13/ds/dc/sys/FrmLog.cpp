//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmLog.h"
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
TFormLog *FormLog;

//---------------------------------------------------------------------------
__fastcall TFormLog::TFormLog(TComponent* Owner)
    : TSubForm(Owner)
{
    m_ulStartTime = 0;
    m_ulEndTime   = gos_get_current_time();
}
//---------------------------------------------------------------------------
void __fastcall TFormLog::btnQueryClick(TObject *Sender)
{
    UINT32  ulStartTime = 0;
    UINT32  ulEndTime = 0xffffffff;
    UINT32  ulDCUserID = INVALID_USER_ID;
    AnsiString  strTmp;

    if (!deStartTime->Text.IsEmpty())
    {
        ulStartTime = GetTime(deStartTime->Date);
    }

    if (!deEndTime->Text.IsEmpty())
    {
        ulEndTime = GetTime(deEndTime->Date);
    }

    strTmp = edtDCUserID->Text.Trim();
    if (!strTmp.IsEmpty())
    {
        if (!ToInt(strTmp, &ulDCUserID) )
        {
            edtDCUserID->SetFocus();
            InfoMsg("用户非法");
            return;
        }

        if (!DCUser::GetDCUser(ulDCUserID) )
        {
            edtDCUserID->SetFocus();
            InfoMsg("未知用户");
            return;
        }
    }

    VECTOR<OPER_LOG_INFO_T>     vInfo;

    if (!LoadOperLogInfo(ulStartTime, ulEndTime, ulDCUserID, vInfo))
    {
        InfoMsg("查询失败");
        return;
    }

    InitView(vInfo);
}
//---------------------------------------------------------------------------
void __fastcall TFormLog::InitView(VECTOR<OPER_LOG_INFO_T> &vInfo)
{
    TcxTreeListNode     *Node;
    OPER_LOG_INFO_T     *pstInfo;
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
        Node->Values[1] = pstInfo->ulTime;
        Node->Values[2] = pstInfo->ulUserID;
        Node->Values[3] = DCOperator::GetDCOperatorName(pstInfo->acDCOperatorID);
        Node->Values[4] = pstInfo->acLogInfo;
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormLog::btnExportClick(TObject *Sender)
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
void __fastcall TFormLog::ColumnTimeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
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
void __fastcall TFormLog::btnPrintClick(TObject *Sender)
{
    ::Print(tlMain, "操作日志");
}
//---------------------------------------------------------------------------
void __fastcall TFormLog::btnNextClick(TObject *Sender)
{
    deStartTime->Date = GetDateTime(m_ulEndTime);
    deEndTime->Clear();

    btnQueryClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormLog::btnPrevClick(TObject *Sender)
{
    deStartTime->Clear();
    deEndTime->Date = GetDateTime(m_ulStartTime);

    btnQueryClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormLog::FormCreate(TObject *Sender)
{
    SetImage(btnQuery,  IMG_QUERY);
    SetImage(btnExport, IMG_EXPORT);
    SetImage(btnPrint,  IMG_PRINT);
    SetImage(btnNext,   IMG_NEXT);
    SetImage(btnPrev,   IMG_PREV);
}
//---------------------------------------------------------------------------


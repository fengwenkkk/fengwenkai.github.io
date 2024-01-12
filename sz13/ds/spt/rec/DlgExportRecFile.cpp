//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "ThreadDownload.h"
#include "DlgExportRecFile.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxLabel"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxTrackBar"
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
#pragma link "cxTextEdit"
#pragma link "cxProgressBar"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxBevel"
#pragma link "cxCustomData"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxInplaceContainer"
#pragma link "cxStyles"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "cxDropDownEdit"
#pragma link "cxMaskEdit"
#pragma link "cxCheckBox"
#pragma link "cxImageComboBox"
#pragma link "cxImageList"
#pragma link "FolderDialog"
#pragma resource "*.dfm"
TDialogExportRecFile *DialogExportRecFile;

ThreadDownload      *g_ThreadDownload = NULL;

extern AnsiString GetRecFileName(AnsiString strFile);

//---------------------------------------------------------------------------
void __fastcall TDialogExportRecFile::InitSkin()
{
}
//---------------------------------------------------------------------------
__fastcall TDialogExportRecFile::TDialogExportRecFile(TComponent* Owner)
    : TDialogForm(Owner)
{
    edtPath->Text = LoadConfInfo("conf", "rec_file_export_path");

    if (!DirectoryExists(edtPath->Text))
    {
        edtPath->Text = "";
    }

    InitMsgHandler(EV_DOWNLOAD_MSG, OnDownloadInd);
}
//---------------------------------------------------------------------------
bool __fastcall TDialogExportRecFile::Init(VECTOR<DOWNLOAD_FILE_INFO_T> &vInfo)
{
    m_vInfo.clear();
    m_vInfo.assign(vInfo.begin(), vInfo.end());

    m_acExportPath[0] = '\0';
    m_ulCurrDownloadIndex = 0;
    btnStart->Enabled = true;
    btnEnd->Enabled = true;
    pbFile->Position = 0;
    pbFile->Properties->Max = m_vInfo.size();;

    tlMain->BeginUpdate();
    tlMain->Clear();

    for (UINT32 i=0; i<m_vInfo.size(); i++)
    {
        TcxTreeListNode *Node = tlMain->Add();

        Node->Values[0] = i+1;
        Node->Values[1] = m_vInfo[i].acFile;
    }

    tlMain->EndUpdate();

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TDialogExportRecFile::btnEndClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TDialogExportRecFile::Stop()
{
    m_ulCurrDownloadIndex = 0xffffffff;
}
//---------------------------------------------------------------------------
void __fastcall TDialogExportRecFile::FormClose(TObject *Sender, TCloseAction &Action)
{
    Stop();

    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogExportRecFile::OnDownloadInd(UINT8 *pucMsg, UINT32 ulMsgLen)
{
    DOWNLOAD_FILE_INFO_T    *pstInfo = (DOWNLOAD_FILE_INFO_T*)pucMsg;
    AnsiString              strFile;

    for (int i=0; i<tlMain->AbsoluteCount; i++)
    {
        strFile = tlMain->AbsoluteItems[i]->Values[1];
        if (strFile == pstInfo->acFile)
        {
            tlMain->AbsoluteItems[i]->Values[2] = pstInfo->acResult;
            pbFile->Position++;
            break;
        }
    }

    for (UINT32 i=0; i<m_vInfo.size(); i++)
    {
        if (strcmp(m_vInfo[i].acFile, pstInfo->acFile) == 0)
        {
            strcpy(m_vInfo[i].acResult, pstInfo->acResult);
            break;
        }
    }

    m_ulCurrDownloadIndex++;
    DownloadFile();
}
//---------------------------------------------------------------------------
void __fastcall TDialogExportRecFile::Get(VECTOR<DOWNLOAD_FILE_INFO_T> &vInfo)
{
    vInfo.clear();
    vInfo.assign(m_vInfo.begin(), m_vInfo.end());
}
//---------------------------------------------------------------------------
void __fastcall TDialogExportRecFile::DownloadFile()
{
    if (m_ulCurrDownloadIndex >= m_vInfo.size())
    {
        return;
    }

    DOWNLOAD_FILE_INFO_T    stInfo = m_vInfo[m_ulCurrDownloadIndex];

    sprintf(stInfo.acLocalFile, "%s/%s", m_acExportPath, m_vInfo[m_ulCurrDownloadIndex].acLocalFile);

    g_ThreadDownload = new ThreadDownload(stInfo);

    g_ThreadDownload->Start();
}
//---------------------------------------------------------------------------
void __fastcall TDialogExportRecFile::FormCreate(TObject *Sender)
{
    SetImage(btnStart, IMG_EXPORT);
    SetImage(btnEnd, IMG_CLOSE);
}
//---------------------------------------------------------------------------
void __fastcall TDialogExportRecFile::btnStartClick(TObject *Sender)
{
    if (!DirectoryExists(edtPath->Text))
    {
        InfoMsg("导出目录不存在");
        return;
    }

    StrCpy(m_acExportPath, edtPath->Text);

    SaveConfInfo("conf", "rec_file_export_path", m_acExportPath);

    pbFile->Position = 0;

    btnStart->Enabled = false;
    btnEnd->Enabled = true;

    DownloadFile();
}
//---------------------------------------------------------------------------
void __fastcall TDialogExportRecFile::btnPathClick(TObject *Sender)
{
    FolderDialog->Directory = edtPath->Text;
    if (!FolderDialog->Execute())
    {
        return;
    }

    edtPath->Text = FolderDialog->Directory;
    if (!DirectoryExists(edtPath->Text))
    {
        InfoMsg("导出目录不存在");
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogExportRecFile::ColumnFileGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[1].IsNull())
    {
        return;
    }

    AnsiString  strFile = ANode->Values[1];
    Value = GetRecFileName(strFile);
}
//---------------------------------------------------------------------------


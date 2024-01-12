//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"

#include "FrmSDSManage.h"
#include "DlgSetTrain.h"
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
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxImage"
#pragma link "cxSplitter"
#pragma resource "*.dfm"
TFormSDSManage *FormSDSManage;

extern void SetFormParent(TForm *Child, TWinControl *Parent);
extern AnsiString LoadTextFile(CHAR *szFile);
//---------------------------------------------------------------------------
__fastcall TFormSDSManage::TFormSDSManage(TComponent* Owner)
    : TSubForm(Owner)
{
    m_bInitView = false;

    Application->CreateForm(__classid(TFormMediaPlayer), &m_FormMediaPlayer);
    SetFormParent(m_FormMediaPlayer, PanelMediaPlayer);

    if (IsSimpleSMSMode())
    {
        ColumnConfirmRate->Visible = false;
        ColumnNeedReply->Visible = false;

        ColumnConfirmTime->Visible = false;
    }

    m_DictSDSType = GetDictSDSType();
    m_DictSDSType->Init(cbSDSType);

    InitMsgHandler(EV_DC_DIS_RSP, MSG_DCGetSDSReply, OnGetSDSReplyRsp);
}
//---------------------------------------------------------------------------
void __fastcall TFormSDSManage::btnQueryClick(TObject *Sender)
{
    UINT32  ulStartTime = GetTime(deStartTime->Date);
    UINT32  ulEndTime = GetTime(deEndTime->Date);
    UINT32  ulSDSType;
    AnsiString  strSendUserName;
    AnsiString  strKeyWord;
    VECTOR<SDS_INFO_T>  vInfo;

    m_DictSDSType->GetID(cbSDSType, &ulSDSType);

    if (ulEndTime > 0)
    {
        ulEndTime += 86400;
    }

    strSendUserName = edtSendUserName->Text.Trim();

    strKeyWord = edtKeyword->Text.Trim();
    if (!LoadSDSInfo(ulStartTime, ulEndTime, ulSDSType,
                     strSendUserName.c_str(), strKeyWord.c_str(), vInfo))
    {
        InfoMsg("查询失败");
        return;
    }

    ShowSDSInfo(vInfo);
}
//---------------------------------------------------------------------------
void __fastcall TFormSDSManage::ShowSDSInfo(VECTOR<SDS_INFO_T> &vInfo)
{
    TcxTreeListNode     *Node;
    CHAR                acPercent[32];
    BOOL                bNeedReply = FALSE;

    m_bInitView = true;

    tlMain->BeginUpdate();
    tlMain->Clear();
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        SDS_INFO_T  &stInfo = vInfo[i];

        Node = tlMain->Add();
        Node->Values[0] = stInfo.ulSDSID;
        Node->Values[1] = GetTextTime(stInfo.ulSendTime);
        Node->Values[2] = stInfo.ulSDSType;
        Node->Values[4] = stInfo.ulSDSIndex;
        if (stInfo.acSendUserName[0])
        {
            Node->Values[5] = stInfo.acSendUserName;
        }
        else
        {
            Node->Values[5] = stInfo.ulSendUserID;
        }

        if (stInfo.ulRecvUserNum == 0 ||
            !stInfo.bNeedReply)
        {
            Node->Values[6] = "";
        }
        else
        {
            sprintf(acPercent, "%d%c", 100*stInfo.ulReplyUserNum/stInfo.ulRecvUserNum, '%');
            Node->Values[6] = acPercent;
        }

        Node->Values[7] = stInfo.acSDSInfo;
        Node->Values[8] = stInfo.bNeedReply;
        Node->Values[9] = stInfo.acFile;

        if (i == 0)
        {
            Node->Selected = true;
            bNeedReply = stInfo.bNeedReply;
        }
    }

    tlMain->EndUpdate();

    m_bInitView = false;

    if (bNeedReply)
    {
        tlMainDblClick(this);
    }
    else
    {
        tlSDSReply->Clear();
    }

    ShowDetail();
}
//---------------------------------------------------------------------------
void __fastcall TFormSDSManage::tlMainFocusedNodeChanged(TcxCustomTreeList *Sender,
          TcxTreeListNode *APrevFocusedNode, TcxTreeListNode *AFocusedNode)
{
    if (m_bInitView)
    {
        return;
    }

    if (m_FormMediaPlayer)
    {
        m_FormMediaPlayer->Stop();
    }

    ShowDetail();
}
//---------------------------------------------------------------------------
void __fastcall TFormSDSManage::ShowDetail()
{
    if (m_bInitView)
    {
        return;
    }

    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlMain->Selections[0];

    if (!Node->Values[2].IsNull() &&
        (UINT32)Node->Values[2] == SDS_TYPE_SMS)
    {
        btnOpenAttachFile->Enabled = true;

        AutoShowSMSAttach(Node);
        return;
    }
    else
    {
        btnOpenAttachFile->Enabled = false;
    }

    mmSDSInfo->BringToFront();

    tlSDSReply->Clear();

    if (!Node || Node->Values[7].IsNull())
    {
        mmSDSInfo->Text = "";
        return;
    }

    mmSDSInfo->Text = Node->Values[7];

    if (Node->Values[8].IsNull())
    {
        return;
    }

    BOOL    bNeedReply = Node->Values[8];

    if (bNeedReply)
    {
        SplitterReply->Visible = true;
        tlSDSReply->Visible = true;
        tlMainDblClick(this);
    }
    else
    {
        SplitterReply->Visible = false;
        tlSDSReply->Visible = false;
        tlSDSReply->Clear();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormSDSManage::btnExportClick(TObject *Sender)
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
void __fastcall TFormSDSManage::tlMainDblClick(TObject *Sender)
{
    if (m_bInitView)
    {
        return;
    }

    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlMain->Selections[0];

    if (Node->Values[0].IsNull() ||
        Node->Values[8].IsNull() )
    {
        return;
    }

    UINT32  ulSDSID = Node->Values[0];
    BOOL    bNeedReply = Node->Values[8];

    if (!bNeedReply)
    {
        return;
    }

    GJsonParam  Param;

    Param.Add("SDSID", ulSDSID);

    SendServer(EV_DC_DIS_REQ, MSG_DCGetSDSReply, Param);
}
//---------------------------------------------------------------------------
void __fastcall TFormSDSManage::OnGetSDSReplyRsp(UINT32 ulSeqID, GJson &Json)
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

    UINT32          ulSDSID = Node->Values[0];
    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = Json.GetSubJson();
    SDS_REPLY_T     stInfo;

    if (vSubJson.size() == 0 ||
        !vSubJson[0]->GetValue("SDSID", &stInfo.ulSDSID) ||
        stInfo.ulSDSID != ulSDSID)
    {
        return;
    }

    tlSDSReply->BeginUpdate();
    tlSDSReply->Clear();

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];

        if (!pSubJson->GetValue("UserID", &stInfo.ulUserID) ||
            !pSubJson->GetValue("ReplyTime", &stInfo.ulReplyTime) )
        {
            GosLog(LOG_ERROR, "parse sds reply failed on %s", pSubJson->GetText());
            continue;
        }

        Node = tlSDSReply->Add();
        Node->Values[0] = stInfo.ulUserID;
        if (stInfo.ulReplyTime > 0)
        {
            Node->Values[1] = GetTextTime(stInfo.ulReplyTime);
        }
    }

    tlSDSReply->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormSDSManage::btnPrintClick(TObject *Sender)
{
    ::Print(tlMain, "电子工单");
}
//---------------------------------------------------------------------------
void __fastcall TFormSDSManage::FormCreate(TObject *Sender)
{
    SetImage(btnQuery,  IMG_QUERY);
    SetImage(btnExport, IMG_EXPORT);
    SetImage(btnPrint,  IMG_PRINT);
}
//---------------------------------------------------------------------------

void __fastcall TFormSDSManage::ColumnNeedReplyGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[8].IsNull())
    {
        return;
    }

    BOOL bNeedReply = ANode->Values[8];

    if (bNeedReply)
    {
        Value = "是";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormSDSManage::AutoShowSMSAttach(TcxTreeListNode *Node)
{
    AnsiString  strFile;
    char        *szPostfix;
    AnsiString  strFileType;
/*    BOOL        bNeedReply = Node->Values[9];

    if (bNeedReply)
    {
        SplitterReply->Visible = true;
        tlSDSReply->Visible = true;
        tlMainDblClick(this);
    }
    else   */
    {
        SplitterReply->Visible = false;
        tlSDSReply->Visible = false;
        tlSDSReply->Clear();
    }

    if (Node->Values[9].IsNull())
    {
        goto error;
    }

    strFile = Node->Values[9];
    szPostfix = gos_get_file_postfix(strFile.c_str());

    if (!szPostfix)
    {
        goto error;
    }

    PanelMediaPlayer->BringToFront();

    strFileType = AnsiString(szPostfix).LowerCase();

    if (strFileType == "mp4" ||
        strFileType == "avi" ||
        strFileType == "mkv" ||
        strFileType == "mp3" ||
        strFileType == "m4a" ||
        strFileType == "wav" ||
        strFileType == "amr" )
    {
        if (m_FormMediaPlayer &&
            FileExists(strFile))
        {
            PanelImage->Visible = false;
            m_FormMediaPlayer->Visible = true;
            m_FormMediaPlayer->SetFile(strFile);
        }

        return;
    }
    else if (strFileType == "jpg" ||
             strFileType == "jpeg" ||
             strFileType == "png" ||
             strFileType == "gif" )
    {
        PanelImage->BringToFront();
        PanelImage->Visible = true;
        ImageAttach->Clear();

        try
        {
            if (FileExists(strFile))
            {
                ImageAttach->Picture->LoadFromFile(strFile);
            }

        }
        catch (Exception &exception)
        {
            WarnMsg("无法显示");
        }
        catch(...)
        {
            WarnMsg("无法显示");
        }

        return;
    }
    else if (strFileType == "txt" ||
             strFileType == "log" )
    {
        mmSDSInfo->BringToFront();

        if (FileExists(strFile))
        {
            mmSDSInfo->Text = LoadTextFile(strFile.c_str());
        }
        else
        {
            mmSDSInfo->Text = "";
        }
        return;
    }

error:
    ImageAttach->Clear();
    PanelImage->Visible = true;
    PanelImage->BringToFront();
}
//---------------------------------------------------------------------------

void __fastcall TFormSDSManage::btnOpenAttachFileClick(TObject *Sender)
{
    if (tlMain->SelectionCount == 0)
    {
        InfoMsg("请选择一条彩信");
        return;
    }

    CHAR        acValue[1024];
    AnsiString  strFile;
    TcxTreeListNode     *Node = tlMain->Selections[0];

    if (Node->Values[9].IsNull())
    {
        InfoMsg("请选择一条彩信");
        return;
    }

    if ((UINT32)Node->Values[2] != SDS_TYPE_SMS)
    {
        InfoMsg("请选择彩信记录");
        return;
    }

    strFile = Node->Values[9];

    if (!FileExists(strFile))
    {
        return;
    }

    sprintf(acValue, "/n,/select,%s", strFile.c_str());
    ShellExecute(NULL, "open", "Explorer.exe", acValue, NULL, SW_SHOWDEFAULT);

}
//---------------------------------------------------------------------------

void __fastcall TFormSDSManage::ColumnSDSTypeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulSDSType = (UINT32)ANode->Values[2];

    Value = GetSDSTypeText(ulSDSType);
}
//---------------------------------------------------------------------------


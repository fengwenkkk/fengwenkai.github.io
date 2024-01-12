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

#include "FrmDevFile.h"
#include "FrmTextReader.h"
#include "FrmMediaPlayer.h"
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
#pragma link "cxImageComboBox"
#pragma link "cxImageList"
#pragma link "cxCheckBox"
#pragma link "cxCalendar"
#pragma link "cxDateUtils"
#pragma link "cxMaskEdit"
#pragma link "dxCore"
#pragma link "cxClasses"
#pragma resource "*.dfm"
TFormDevFile *FormDevFile;

extern UINT16       g_usFtpPort;
extern CHAR         g_acFtpVerDir[256];
extern CHAR         g_acFtpLogFileDir[256];
extern CHAR         g_acFtpUser[32];
extern CHAR         g_acFtpPwd[32];

extern Dict g_DictDevType;
extern AnsiString GetDevTypeText(UINT32 ulDevType);
extern INT32 GetOnlineStatusByNeID(AnsiString strNeID);
extern BOOL GetDevFileList(UINT32 ulDevFileType, AnsiString strNeID, UINT32 ulStartTime, UINT32 ulEndTime, VECTOR<OMA_FILE_INFO_T> &vInfo);

static ID_VALUE_T   s_astDevFileType[] = {
											{DEV_FILE_LOG,  "��־�ļ�"},
											{DEV_FILE_REC,  "¼���ļ�"},
                                		 };

Dict    g_DictDevFileType(s_astDevFileType, ARRAY_SIZE(s_astDevFileType));

AnsiString GetFileTypeText(UINT32 ulFileType)
{
    for (UINT32 i=0; i<ARRAY_SIZE(s_astDevFileType); i++)
    {
        if (s_astDevFileType[i].ulID == ulFileType)
        {
            return s_astDevFileType[i].szValue;
        }
    }

    return "";
}
//---------------------------------------------------------------------------
__fastcall TFormDevFile::TFormDevFile(TComponent* Owner)
    : TSubForm(Owner)
{
    g_DictDevFileType.Init(cbFileType);
    g_DictDevFileType.SetID(DEV_FILE_LOG);

	//g_DictDevType.Init(cbDevType, TRUE, TRUE);
	//g_DictDevType.SetID(cbDevType, OMC_DEV_TYPE_TX);

	cbDevType->Properties->Items->Add("����̨");
	cbDevType->Properties->Items->Add("�̶�̨");
    cbDevType->ItemIndex = 0;

    UpdateDevList();
}
//---------------------------------------------------------------------------
void __fastcall TFormDevFile::UpdateDevList()
{
    AnsiString  strDevName = cbDev->Text;

    cbDevTypePropertiesChange(this);

    cbDev->ItemIndex = cbDev->Properties->Items->IndexOf(strDevName);
}
//---------------------------------------------------------------------------
void __fastcall TFormDevFile::btnQueryClick(TObject *Sender)
{
    AnsiString  strNeName = cbDev->Text.Trim();
    AnsiString  strNeID;
    UINT32  ulDevFileType;
    UINT32  ulStartTime = GetTime(deStartTime->Date);
    UINT32  ulEndTime = GetTime(deEndTime->Date);
    VECTOR<OMA_FILE_INFO_T>     vInfo;
    TimerRefreshFileList->Enabled = false;
	if (ulEndTime == 0)
    {
        ulEndTime = 0xffffffff;
    }

    if (!g_DictDevFileType.GetID(&ulDevFileType))
    {
        InfoMsg("��ѡ���ļ�����");
        return;
    }

    if (strNeName.IsEmpty())
    {
        InfoMsg("��ѡ���豸����");
        return;
    }

    strNeID = GetNeIDByDevName(strNeName);
    if (strNeID.IsEmpty())
    {
        InfoMsg("δ֪���豸����");
        return;
    }

    if (ulStartTime > 0 && ulEndTime > 0 &&
        ulStartTime > ulEndTime )
    {
        InfoMsg("��ʼʱ�䲻�ܴ��ڽ���ʱ��");
        return;
    }

    tlMain->Clear();

	if (!GetDevFileList(ulDevFileType, strNeID, ulStartTime, ulEndTime, vInfo))
    {
        return;
    }

    tlMain->BeginUpdate();
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        TcxTreeListNode     *Node = tlMain->Add();

        Node->Values[0] = i+1;
        Node->Values[1] = strNeID;
        Node->Values[2] = vInfo[i].acFileName;
        Node->Values[3] = ulDevFileType;
        Node->Values[4] = GetTextTime(vInfo[i].ulFileTime);
        Node->Values[5] = vInfo[i].acFileSize;
        Node->Values[6] = vInfo[i].bFileExist;
    }

    tlMain->EndUpdate();

    tlMainSelectionChanged(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormDevFile::FormCreate(TObject *Sender)
{
//  SetImage(btnImport, IMG_IMPORT);
}
//---------------------------------------------------------------------------
void __fastcall TFormDevFile::ColumnFileTypeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulFileType = ANode->Values[2];

    Value = GetFileTypeText(ulFileType);
}
//---------------------------------------------------------------------------
void __fastcall TFormDevFile::cbDevTypePropertiesChange(TObject *Sender)
{
    UINT32  ulDevType;
    VectorMap<AnsiString, NE_BASIC_INFO_T>  mNEBasicInfo;
    NEBasic::GetNEBasicInfo(mNEBasicInfo);

    cbDev->Properties->Items->Clear();

    if (!g_DictDevType.GetID(cbDevType, &ulDevType))
    {
        return;
    }

    cbDev->Properties->Items->BeginUpdate();

    for (UINT32 i=0; i<mNEBasicInfo.Size(); i++)
    {
        NE_BASIC_INFO_T *pstInfo = mNEBasicInfo.GetValueByIndex(i);

        if (pstInfo->ulDevType == ulDevType
            && *(pstInfo->acDevName) != '\0')
		{
            cbDev->Properties->Items->Add(pstInfo->acDevName);
        }

    }

    cbDev->ItemIndex = -1;

    cbDev->Properties->Items->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormDevFile::btnDownloadClick(TObject *Sender)
{
	TcxTreeListNode     *Node = tlMain->SelectionCount>0?tlMain->Selections[0]:NULL;

	if (!Node)
	{
		return;
	}

	m_strNeID = "";
	m_strFile = "";
	m_strNeID = Node->Values[1];
	m_strFile = Node->Values[2];
	GJsonParam  Param;

	if (GetOnlineStatusByNeID(m_strNeID) <= 0)
	{
		WarnMsg("�豸�����ߣ��޷�����!");
		return;
	}

	Param.Add("NeID", m_strNeID);
	Param.Add("File", m_strFile);

	SendServer(EV_OMT_OMC_REQ, MSG_OMT_UploadDevFile, Param);

    TimerRefreshFileList->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormDevFile::ColumnStateGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[6].IsNull())
    {
        return;
    }

	BOOL    bExist = ANode->Values[6];

    if (!bExist)
    {
        Value = "������";
    }
    else
    {
        Value = "";
    }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TFormDevFile::DownloadCurrentFile()
{
	TcxTreeListNode     *Node = tlMain->SelectionCount>0?tlMain->Selections[0]:NULL;

	if (!Node)
    {
		return "";
	}

	AnsiString  strNeID = Node->Values[1];
	AnsiString  strFile = Node->Values[2];
    BOOL        bExist = Node->Values[6];

    if (!bExist)
	{
		btnQueryClick(this);
        return "";
    }

    CHAR    *szRawFile;
    CHAR    acServerFile[256];
    CHAR    acLocalFile[256];
	BOOL    bCancel = FALSE;

    szRawFile = gos_right_strchr(strFile.c_str(), '/');
    if (!szRawFile)
    {
        szRawFile = strFile.c_str();
    }
    else
    {
        szRawFile++;
    }

	sprintf(acServerFile, "%s/%s", g_acFtpLogFileDir, szRawFile);
    sprintf(acLocalFile, "%s//tmp//%s", gos_get_root_path(), szRawFile);

	BOOL bRet = gos_ftp_get(g_acFtpUser, g_acFtpPwd, g_stLocalCfg.aucOMCAddr,
                        g_usFtpPort, acServerFile, acLocalFile);

    if (!bRet)
	{
        return "";
    }

    return acLocalFile;
}
//---------------------------------------------------------------------------
AnsiString GetRecFileName(AnsiString strURL)
{
    CHAR        *szTmp;

    szTmp = gos_right_strchr(strURL.c_str(), '/');
    if (!szTmp)
    {
        szTmp = gos_right_strchr(strURL.c_str(), '\\');
	}

    if (szTmp)
    {
        return szTmp+1;
    }

    return strURL;
}
//---------------------------------------------------------------------------
void __fastcall TFormDevFile::btnExportClick(TObject *Sender)
{
	AnsiString strLocalFile = DownloadCurrentFile();

	if (strLocalFile.IsEmpty())
	{
		InfoMsg("��ȡ�ļ�ʧ��");
		return;
	}

	SaveDialog->Filter = "*.*|*.*";
	SaveDialog->DefaultExt = "*.*";

	CHAR        *szFilePostfix = gos_get_file_postfix(strLocalFile.c_str());

	if (strstr(szFilePostfix, "txt"))
	{
		SaveDialog->Filter = "*.txt|*.txt";
		SaveDialog->DefaultExt = "*.txt";
	}
	else if (strstr(szFilePostfix, "wav"))
	{
		SaveDialog->Filter = "*.wav|*.wav";
		SaveDialog->DefaultExt = "*.wav";
	}
	else if (strstr(szFilePostfix, "mp3"))
	{
		SaveDialog->Filter = "*.mp3|*.mp3";
		SaveDialog->DefaultExt = "*.mp3";
	}
	else if (strstr(szFilePostfix, "mp4"))
	{
		SaveDialog->Filter = "*.mp4|*.mp4";
		SaveDialog->DefaultExt = "*.mp4";
	}
	else
	{
		InfoMsg("��Ч�ĵ����ļ���ʽ");
		return;
	}


	SaveDialog->FileName = GetRecFileName(strLocalFile);

	if (!SaveDialog->Execute() || SaveDialog->FileName.IsEmpty())
	{
		InfoMsg("����ʧ��");
		return;
	}

	AnsiString strFileName = SaveDialog->FileName;

	if (!gos_rename_file(strLocalFile.c_str(), strFileName.c_str()))
	{
		InfoMsg("������ָ��Ŀ¼ʧ��");
		return;
	}

	InfoMsg("�����ɹ�");
}
//---------------------------------------------------------------------------
void __fastcall TFormDevFile::btnQueryFileClick(TObject *Sender)
{
	AnsiString strLocalFile = DownloadCurrentFile();
	if (strLocalFile.IsEmpty())
    {
        InfoMsg("��ȡ�ļ�ʧ��");
        return;
    }
 // AnsiString strLocalFile = "F:\\git-code\\sz14\\ds\\bin\\tmp\\6432-15ed_2021-09-16_log.txt1";

	FormTextReader->Init(strLocalFile.c_str());
	FormTextReader->Parent = this;
	FormTextReader->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormDevFile::tlMainFocusedNodeChanged(TcxCustomTreeList *Sender,
          TcxTreeListNode *APrevFocusedNode, TcxTreeListNode *AFocusedNode)
{
    tlMainSelectionChanged(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormDevFile::btnPlayClick(TObject *Sender)
{
	AnsiString strLocalFile = DownloadCurrentFile();

    if (strLocalFile.IsEmpty())
    {
		InfoMsg("��ȡ�ļ�ʧ��");
        return;
    }

    FormMediaPlayer->Init(strLocalFile.c_str());
    FormMediaPlayer->Parent = this;
    FormMediaPlayer->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormDevFile::tlMainSelectionChanged(TObject *Sender)
{
    btnExport->Enabled = false;
    btnQueryFile->Enabled = false;
	btnPlay->Enabled = false;
    pmiExport->Enabled = false;
	pmiPlay->Enabled = false;
	pmiBrowser->Enabled = false;

    TcxTreeListNode     *Node = tlMain->SelectionCount>0?tlMain->Selections[0]:NULL;

    if (!Node)
    {
        return;
    }

    if (Node->Values[6].IsNull())
    {
        return;
    }

	BOOL    bExist = Node->Values[6];

    if (bExist)
    {
        btnExport->Enabled = true;

        if (Node->Values[3].IsNull())
        {
            return;
        }

        UINT32  ulFileType = Node->Values[3];

        if (ulFileType == DEV_FILE_LOG)
        {
			btnQueryFile->Enabled = true;
			pmiBrowser->Enabled = true;
			btnQueryFile->BringToFront();
        }
		else if (ulFileType == DEV_FILE_REC)
		{
			btnPlay->Enabled = true;
            pmiPlay->Enabled = true;
			btnPlay->BringToFront();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormDevFile::TimerRefreshFileListTimer(TObject *Sender)
{

	GJsonParam  Param;

	Param.Add("NeID", m_strNeID);
	Param.Add("File", m_strFile);

	SendServer(EV_OMT_OMC_REQ, MSG_OMT_UploadDevFile, Param);

	btnQueryClick(this);
	TimerRefreshFileList->Enabled = false;
}
//---------------------------------------------------------------------------


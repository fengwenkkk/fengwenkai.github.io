//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"
#include "MsgDialog.h"
#include "GCommon.h"
#include "ClientUtil.h"
#include "GTransFile.h"
#include "MakeMsg.h"
#include "MakeUtil.h"
#include "main.h"
#include "ThreadMake.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxControls"
#pragma link "cxGraphics"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxPC"
#pragma link "dxSkinBlack"
#pragma link "dxSkinBlue"
#pragma link "dxSkinCaramel"
#pragma link "dxSkinCoffee"
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinFoggy"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkiniMaginary"
#pragma link "dxSkinLilian"
#pragma link "dxSkinLiquidSky"
#pragma link "dxSkinLondonLiquidSky"
#pragma link "dxSkinMcSkin"
#pragma link "dxSkinMoneyTwins"
#pragma link "dxSkinOffice2007Black"
#pragma link "dxSkinOffice2007Blue"
#pragma link "dxSkinOffice2007Green"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinOffice2007Silver"
#pragma link "dxSkinOffice2010Black"
#pragma link "dxSkinOffice2010Blue"
#pragma link "dxSkinOffice2010Silver"
#pragma link "dxSkinPumpkin"
#pragma link "dxSkinsCore"
#pragma link "dxSkinscxPCPainter"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinsdxStatusBarPainter"
#pragma link "dxSkinSeven"
#pragma link "dxSkinSharp"
#pragma link "dxSkinSilver"
#pragma link "dxSkinSpringTime"
#pragma link "dxSkinStardust"
#pragma link "dxSkinSummer2008"
#pragma link "dxSkinValentine"
#pragma link "dxSkinXmas2008Blue"
#pragma link "dxStatusBar"
#pragma link "dxTabbedMDI"
#pragma link "cxClasses"
#pragma link "cxCustomData"
#pragma link "cxData"
#pragma link "cxDataStorage"
#pragma link "cxDBData"
#pragma link "cxEdit"
#pragma link "cxFilter"
//#pragma link "cxGrid"
#pragma link "cxStyles"
#pragma link "dxRibbon"
#pragma link "dxRibbonSkins"
#pragma link "dxSkinsForm"
#pragma link "dxLayoutContainer"
#pragma link "dxLayoutControl"
#pragma link "dxLayoutLookAndFeels"
#pragma link "cxInplaceContainer"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "cxDBTL"
#pragma link "cxTLData"
#pragma link "cxMaskEdit"
#pragma link "cxContainer"
#pragma link "cxTreeView"
#pragma link "cxTextEdit"
#pragma link "cxButtons"
#pragma link "dxBar"
#pragma link "dxBarExtItems"
#pragma link "dxSkinsdxBarPainter"
#pragma link "dxNavBar"
#pragma link "dxNavBarBase"
#pragma link "dxNavBarCollns"
#pragma link "dxSkinsdxNavBarPainter"
#pragma link "dxNavBarStyles"
#pragma link "cxButtonEdit"
#pragma link "cxCalc"
#pragma link "cxHyperLinkEdit"
#pragma link "cxListView"
#pragma link "cxShellCommon"
#pragma link "cxShellTreeView"
#pragma link "cxBarEditItem"
#pragma link "cxCheckBox"
#pragma link "cxSplitter"
#pragma link "cxDropDownEdit"
#pragma link "cxLabel"
#pragma link "cxGroupBox"
#pragma link "cxPropertiesStore"
#pragma link "cxProgressBar"
#pragma link "cxTrackBar"
#pragma link "cxMemo"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinDevExpressStyle"
#pragma link "dxSkinHighContrast"
#pragma link "dxSkinMetropolis"
#pragma link "dxSkinMetropolisDark"
#pragma link "dxSkinOffice2013DarkGray"
#pragma link "dxSkinOffice2013LightGray"
#pragma link "dxSkinOffice2013White"
#pragma link "dxSkinOffice2016Colorful"
#pragma link "dxSkinOffice2016Dark"
#pragma link "dxSkinSevenClassic"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinTheAsphaltWorld"
#pragma link "dxSkinTheBezier"
#pragma link "dxSkinVisualStudio2013Blue"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxSkinVisualStudio2013Light"
#pragma link "dxSkinVS2010"
#pragma link "dxSkinWhiteprint"
#pragma link "dxTaskbarProgress"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxDrawTextUtils"
#pragma link "dxBkgnd"
#pragma link "dxPrnDev"
#pragma link "dxPrnPg"
#pragma link "dxPSCompsProvider"
#pragma link "dxPSCore"
#pragma link "dxPScxEditorProducers"
#pragma link "dxPScxExtEditorProducers"
#pragma link "dxPScxPageControlProducer"
#pragma link "dxPSEdgePatterns"
#pragma link "dxPSEngn"
#pragma link "dxPSFillPatterns"
#pragma link "dxPSGlbl"
#pragma link "dxPSPDFExport"
#pragma link "dxPSPDFExportCore"
#pragma link "dxPSPrVwAdv"
#pragma link "dxPSPrVwRibbon"
#pragma link "dxPSPrVwStd"
#pragma link "dxPSUtl"
#pragma link "dxWrap"
#pragma link "trayicon"
#pragma resource "*.dfm"
TFormMain *FormMain;

#define MAX_THREAD_NUM      32

AnsiString  g_strVer = "V1.2";

bool        g_bRebuild = false;
//HANDLE      g_hMainHandle = NULL;

GTransFile  *g_pTransFile = NULL;

AnsiString  g_strMakingName = "";
AnsiString  g_strMakingProject = "";   // 正在编译的项目目录
AnsiString  g_strMakingOption = "";
AnsiString  g_strMakingIncPath = "";

UINT32      g_ulMakeingStartTime = 0;
UINT32      g_ulMakeingActiveTime = 0;

UINT32      g_ulMakingIdleTime = 60*3;  // make空闲等待时间，超时结束本次make

AnsiString  g_strSrcIncPath  = "";      // 项目自定义目录

IP_ADDR_T   g_stMakingPeer = {0};

AnsiString  g_strMakeName = "";

AnsiString  g_strProjRootPath = ""; // 工程归属的项目路径（非工程路径）
AnsiString  g_strSrcPath = "";
AnsiString  g_strObjPath = "";
AnsiString  g_strDefine = "";
AnsiString  g_strZipFile = "";

bool g_bAgentMode = false;

VECTOR<std::string>  g_vMakePath;

TCTrayIcon  *TrayIcon = NULL;

extern VECTOR<IP_ADDR_T>     g_vPeerInfo;

void StopMakingTask()
{
    GosLog(LOG_INFO, "stop task %s", g_strMakingName.c_str());

    g_strMakingName = "";
    g_ulMakeingStartTime = 0;
    g_ulMakeingActiveTime = 0;
}

AnsiString GetProjectFileName(char *szFile)
{
    AnsiString      strFile;

    if (strlen(szFile) <= g_strSrcPath.Length() ||
        !gos_str_like(szFile, g_strSrcPath.c_str()) )
    {
        return "";
    }

    return szFile + g_strSrcPath.Length() + 1;
}

void __fastcall TFormMain::InitSkin()
{
    tlSourceFile->OptionsSelection->CellSelect = true;
}

//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TSubForm(Owner)
{
    if (g_bAgentMode)
    {
        WindowState = wsNormal;

        BorderStyle = bsNone;
        Left = 0;
        Top  = 0;
        Width = 1;
        Height = 1;

        InitTrayIcon();
    }
    else
    {
        WindowState = wsMaximized;
    }

    Application->Title = "NetMake - " + g_strVer;
    Caption = Application->Title;

  //  Application->Icon = this->Icon;
//    tlSourceFile->Width = 800;

    gos_init_socket();

/*    edtProject->Text = LoadProjectFromReg();
    if (FileExists(edtProject->Text))
    {
        InitSourceFile(false);
    }   */

//    tlSourceFile->OnCustomDrawDataCell = tlSourceFileCustomDrawDataCell;

    AnsiString strProjectFile = LoadConfInfo("Builder", "ProjectFile");

    edtProject->Text = strProjectFile;

    if (!edtProject->Text.IsEmpty())
    {
        LoadProjectFile(strProjectFile.c_str());
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    //g_hMainHandle = NULL;

    btnStopClick(this);

    if (TrayIcon)
    {
        TrayIcon->Visible = false;
    }

    CanClose = true;
}

UINT32 __fastcall TFormMain::GetFileState(char *szFile)
{
    char    acSourceFile[512];
    char    acObjFile[512];
    char    *szTmp;
    UINT32  ulSourceCreateTime;
    UINT32  ulSourceModifyTime;
    UINT32  ulObjCreateTime;
    UINT32  ulObjModifyTime;

    gos_get_file_rawname(szFile, acSourceFile);
    szTmp = strchr(acSourceFile, '.');
    if (!szTmp)
    {
        return MAKE_STATE_IDLE;
    }

    strcpy(szTmp, ".obj");
    sprintf(acObjFile, "%s/%s/%s", m_acProjectPath, "debug", acSourceFile);
    sprintf(acSourceFile, "%s/%s", m_acProjectPath, szFile);

    gos_get_file_time(acSourceFile, &ulSourceCreateTime, &ulSourceModifyTime);
    gos_get_file_time(acObjFile, &ulObjCreateTime, &ulObjModifyTime);

    if (ulSourceModifyTime < ulObjModifyTime)
    {
        return MAKE_STATE_COMPILED;
    }

    return MAKE_STATE_IDLE;
}

bool __fastcall TFormMain::InitSourceFile(char *szFile, bool bRebuild)
{
    bool    bRet = false;
    int     iSize = gos_get_file_size(szFile);
    char    *szBuf;
    FILE    *fp;

    btnStart->Enabled = false;
    btnRebuild->Enabled = false;
    btnClear->Enabled = false;

    if (iSize <= 0)
    {
        return false;
    }

    SaveProject(szFile);

    gos_get_file_path(szFile, m_acProjectPath);

    szBuf = (char*)calloc(1, iSize+1);
    if (!szBuf)
    {
        return false;
    }

    fp = fopen(szFile, "rb");
    if (!fp)
    {
        free(szBuf);
        return false;
    }

    if (iSize != fread(szBuf, 1, iSize, fp))
    {
        free(szBuf);
        fclose(fp);
        return false;
    }

    fclose(fp);

    char    *szFileName;
    char    *szMakeOrder;
    char    *szStart = strstr(szBuf, "<ItemGroup>");
    char    *szEnd;
    UINT32  ulLen;
    FILE_MAKE_INFO_T    stFileMakeInfo;
    int                 iColumn = 0;
    TcxTreeListNode     *Node;
    UINT32              ulMakeOrder = 0;
    UINT32              ulCreateTime;
    VECTOR<FILE_MAKE_INFO_T>    vMakeInfo;

    stFileMakeInfo.ulAllocNum = 0;
    stFileMakeInfo.ulAllocTime = 0;
    memset(&stFileMakeInfo.stPeer, 0, sizeof(stFileMakeInfo.stPeer));

    if (!szStart)
    {
        goto end;
    }

    pbMake->Properties->Max = 10;
    pbMake->Position = 0;

    while(1)
    {
        /*
        <CppCompile Include="call\DlgVideoCallCfg.cpp">
            <Form>DialogVideoCallCfg</Form>
            <FormType>dfm</FormType>
            <DependentOn>call\DlgVideoCallCfg.h</DependentOn>
            <BuildOrder>23</BuildOrder>
        </CppCompile>
        */
        szStart = strstr(szStart, "<CppCompile");
        if (!szStart)
        {
            break;
        }

        szStart = strchr(szStart + strlen("<CppCompile"), '=');
        if (!szStart)
        {
            break;
        }

        szStart++;
        szEnd = strchr(szStart, '>');
        if (!szEnd)
        {
            break;
        }

        szFileName = gos_left_trim_string(szStart);
        szMakeOrder = szEnd+1;
        *szEnd = '\0';

        szFileName = gos_trim_string(szFileName);
        if (*szFileName != '"')
        {
            break;
        }

        szFileName++;
        ulLen = strlen(szFileName);
        if (ulLen <= 1)
        {
            break;
        }

        if (szFileName[ulLen-1] != '"')
        {
            break;
        }

        szFileName[ulLen-1] = '\0';

        ulMakeOrder = 0;
        szMakeOrder = strstr(szMakeOrder, "<BuildOrder>");
        if (szMakeOrder)
        {
            szMakeOrder += strlen("<BuildOrder>");
            szEnd = strchr(szMakeOrder, '<');
            if (szEnd)
            {
                *szEnd = '\0';
                if (!gos_atou(szMakeOrder, &ulMakeOrder))
                {
                    ulMakeOrder = 0;
                }
            }
        }

        szStart = szEnd+1;

        stFileMakeInfo.strFile = szFileName;
        stFileMakeInfo.ulMakeOrder = ulMakeOrder;
        stFileMakeInfo.ulState = bRebuild?MAKE_STATE_IDLE:GetFileState(szFileName);
        stFileMakeInfo.strResult = "";

        if (FileMakeInfo::IsFilteredFile(szFileName))
        {
            stFileMakeInfo.ulState = MAKE_STATE_FILTERED;
        }

        vMakeInfo.push_back(stFileMakeInfo);
    }

    FileMakeInfo::Init(vMakeInfo);

    tlSourceFile->BeginUpdate();
    tlSourceFile->Clear();

    //倒序初始化，使得远程编译与IDE编译不冲突
    for (int i=0; i<vMakeInfo.size(); i++)
    {
        if (iColumn == 0)
        {
            Node = tlSourceFile->Root->AddChild();
        }

        Node->Values[2*iColumn] = vMakeInfo[i].strFile;
        Node->Values[2*iColumn+1] = vMakeInfo[i].ulState;

        iColumn++;
        if (iColumn == 3)
        {
            iColumn = 0;
        }
    }

    bRet = true;

end:
    GOS_FREE(szBuf);

    tlSourceFile->EndUpdate();

    if (bRet)
    {
        btnStart->Enabled = true;
        btnRebuild->Enabled = true;
        btnClear->Enabled = true;
    }

    return bRet;
}

bool __fastcall TFormMain::InitBuilderPara()
{
  /*  char        acPath[1024];
    char        acProjFile[1024];

    StrCpy(acProjFile, edtProject->Text);
    gos_get_file_path(acProjFile, acPath);

    AnsiString  strCmd = LoadBuilderCmd(acProjFile);

    if (strCmd.IsEmpty())
    {
        return false;
    }      */

    return true;
}
void __fastcall TFormMain::btnStopClick(TObject *Sender)
{
    btnStop->Enabled = false;

    btnOpen->Enabled = true;
    btnStart->Enabled = true;
    btnRebuild->Enabled = true;
    btnClear->Enabled = true;

    g_strMakeName = "";
    pbMake->Position = 0;

    FileMakeInfo::ClearObjZipFile();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::LogBuilderInfo(BUILD_INFO_T *pstInfo)
{
    Log(AnsiString("============ build file: ") + AnsiString(pstInfo->acSourceFile));
    Log(pstInfo->acBuilderInfo);
}
void __fastcall TFormMain::Log(AnsiString strText)
{
    mmInfo->Lines->Add(strText);
    mmInfo->Lines->Add("\n");
}
//---------------------------------------------------------------------------
TcxTreeListNode* __fastcall TFormMain::GetUnbuildSourceFile(UINT32 &ulColIndex)
{
  /*    std::map<AnsiString, TcxTreeListNode*>::iterator  it = g_mBuilderInfo.begin();

    for (;it!=g_mBuilderInfo.end(); it++)
    {
        TcxTreeListNode *Node = it->second;

        int iState = (int)Node->Values[1];

        if (iState == BS_IDLE)
        {
            return Node;
        }
    }  */

    int             iState;
    TcxTreeListNode *Node = tlSourceFile->Root->getFirstChild();

    while(Node)
    {
        iState = (int)Node->Values[1];
        if (g_bRebuild || iState == MAKE_STATE_IDLE)
        {
            ulColIndex = 0;
            return Node;
        }

        if (Node->Values[3].IsNull())
        {
            break;
        }

        iState = (int)Node->Values[3];
        if (g_bRebuild || iState == MAKE_STATE_IDLE)
        {
            ulColIndex = 2;
            return Node;
        }

        if (Node->Values[5].IsNull())
        {
            break;
        }

        iState = (int)Node->Values[5];
        if (g_bRebuild || iState == MAKE_STATE_IDLE)
        {
            ulColIndex = 4;
            return Node;
        }

        Node = Node->getNextSibling();
    }

    return NULL;
}

void __fastcall TFormMain::TimerCheckTaskTimer(TObject *Sender)
{
    TimerCheckTask->Interval = 5*60*1000;

    ClearOldTask(NULL);
}

void FormatMakeResult(CHAR *szFile, CHAR *szResult, BOOL &bIsSucc)
{
    char *szPos = strstr(szResult, szFile);

    bIsSucc = FALSE;
    if (!szPos)
    {
        return;
    }

    szPos += strlen(szFile);
    szPos = strchr(szPos, ':');
    if (!szPos)
    {
        return;
    }

    szPos = gos_trim_string(szPos+1);
    memmove(szResult, szPos, strlen(szPos)+1);

    szPos = strstr(szResult, "error");
    if (!szPos)
    {
        bIsSucc = TRUE;
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::OnInnerMsg(TMessage Message)
{
    //BUILD_INNER_MSG_T         *pstMsg = (BUILD_INNER_MSG_T *)Message.LParam;

    /*
    if (pstMsg->ulMsgID == MSG_BUILD_INFO)
    {
        BUILD_INFO_T    *pstInfo = (BUILD_INFO_T*)pstMsg->aucMsg;

        AnsiString  strFile = pstInfo->acSourceFile;
        UINT32      ulState;

        BUILD_FILE_INFO_T   *pstFile = GetBuildFileInfo(strFile);

        if (!pstFile) //g_mBuilderInfo.find(strFile) == g_mBuilderInfo.end())
        {
            GosLog(LOG_ERROR, "未知文件%s", pstInfo->acSourceFile);
            goto end;
        }

        FormatBuildResult(pstInfo, ulState);

        pstFile->strResult = pstInfo->acBuilderInfo;

        //LogBuilderInfo(pstInfo);

        TcxTreeListNode     *Node = pstFile->Node; //g_mBuilderInfo[strFile];

        int  iColIndex = 2*pstFile->iColIndex+1;

        if (Node->Values[iColIndex] != BS_SUCC &&
            Node->Values[iColIndex] != BS_FAIL)
        {
            pbBuild->Position ++;
            tpBuild->Position ++;
        }

        Node->Values[iColIndex] = ulState;//GetBuildState(pstInfo->acResult);
    }
    */

end:
 //   free(pstMsg);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ShowMakeResult(AnsiString strFile, char *szResult, UINT32 ulState)
{
    int             iColIndex = -1;
    TcxTreeListNode *Node;

    if (strFile.IsEmpty())
    {
        return;
    }

    for(int i=0; i<tlSourceFile->AbsoluteCount; i++)
    {
        Node = tlSourceFile->AbsoluteItems[i];

        if (Node->Texts[0] == strFile)
        {
            iColIndex = 0;
            break;
        }

        if (Node->Texts[2] == strFile)
        {
            iColIndex = 2;
            break;
        }

        if (Node->Texts[4] == strFile)
        {
            iColIndex = 4;
            break;
        }
    }

    if (iColIndex < 0)
    {
        return;
    }

    Node->Values[iColIndex+1] = ulState;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormShow(TObject *Sender)
{
    //g_hMainHandle = this->Handle;
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::btnOpenClick(TObject *Sender)
{
    edtProject->Text = "";
    OpenDialog->Title = "打开工程文件";

    char    acFile[256];

    if (!OpenDialog->Execute())
    {
        return;
    }

    StrCpy(acFile, OpenDialog->FileName);

    LoadProjectFile(acFile);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::LoadProjectFile(char *szFile)
{
    if (!FileExists(szFile))
    {
        return;
    }

    char    acPath[256];
  //  strcpy(acFile, "d:\\code_sz\\ds\\dc.make");
    gos_get_file_path(szFile, acPath);

    GConfFile   Conf(szFile);
    char    *szProject = Conf.Get("project");
    char    *szMakePath = Conf.Get("path");
    char    *szFilterFile = Conf.Get("filter_file");
    char    acProjectPath[256];
    char    acCBProj[256];
    char    acSrcPath[256];

    if (!szProject || !szMakePath)
    {
        return;
    }

    sprintf(acCBProj, "%s/%s", acPath, szProject);

    if (!gos_file_exist(acCBProj))
    {
        return;
    }

    edtProject->Text = acCBProj;

/*  if (!InitBuilderPara())
    {
        return;
    }
  */

    if (szFilterFile)
    {
        FileMakeInfo::InitFilterFile(szFilterFile);
    }

    if (!InitSourceFile(acCBProj, false))
    {
        return;
    }

    gos_get_file_path(szProject, acProjectPath);
    InitMakePath(acPath, acProjectPath, szMakePath, g_vMakePath);

    GetProjectParam(acCBProj, g_strSrcIncPath, g_strObjPath, g_strDefine);
    g_strProjRootPath = acPath;

    gos_get_file_path(acCBProj, acSrcPath);
    g_strSrcPath = acSrcPath + strlen(acPath)+1;

    g_strObjPath = acSrcPath + AnsiString("\\") + g_strObjPath;

    SaveConfInfo("Builder", "ProjectFile", szFile);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ColumnStateGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    int     iIndex = Sender->Position->ColIndex;

    if (iIndex != 1 && iIndex != 3 && iIndex != 5)
    {
        return;
    }

    if (ANode->Values[iIndex-1].IsNull() ||
        ANode->Values[iIndex].IsNull())
    {
        Value = "";
        return;
    }

    int     iValue = ANode->Values[iIndex];

    if (iValue == MAKE_STATE_IDLE)
    {
        Value = "";
    }
    else if (iValue == MAKE_STATE_COMPILED)
    {
        Value = "--";
    }
    else if (iValue == MAKE_STATE_FILTERED)
    {
        Value = "**";
    }
    else if (iValue == MAKE_STATE_ALLOCED)
    {
        Value = "编译中";
    }
    else if (iValue == MAKE_STATE_SUCC)
    {
        Value = "成功";
    }
    else if (iValue == MAKE_STATE_FAIL)
    {
        Value = "错误";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::tlSourceFileCustomDrawDataCell(TcxCustomTreeList *Sender,
          TcxCanvas *ACanvas, TcxTreeListEditCellViewInfo *AViewInfo,
          bool &ADone)
{
    int iIndex = AViewInfo->Column->ItemIndex;

    if (iIndex != 1 && iIndex != 3 && iIndex != 5)
    {
        return;
    }

    if (AViewInfo->Node->Values[iIndex].IsNull())
    {
        return;
    }

    int iValue = AViewInfo->Node->Values[iIndex];

    if (iValue == MAKE_STATE_ALLOCED)
    {
        ACanvas->Canvas->Brush->Color = clGreen;
    }
    else if (iValue == MAKE_STATE_FAIL)
    {
        ACanvas->Canvas->Brush->Color = (TColor)RGB(192,0,0);//clGreen;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnClearClick(TObject *Sender)
{
    char        acPath[1024];
    char        acProjFile[1024];

    StrCpy(acProjFile, edtProject->Text);
    gos_get_file_path(acProjFile, acPath);

    gos_delete_dir(g_strObjPath.c_str());

    InitSourceFile(acProjFile, true);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnRebuildClick(TObject *Sender)
{
    btnClearClick(this);

    btnStartClick(this);
    FileMakeInfo::Rebuild();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerQueryMakeTimer(TObject *Sender)
{
    if (g_strMakingName.IsEmpty())
    {
        if (g_bAgentMode)
        {
            SendAllPeer(EV_GET_MAKE_TASK_REQ, NULL, 0);
        }
    }
    else
    {
        UINT32  ulCurrTime = gos_get_uptime();

        if (g_ulMakeingActiveTime > 0 &&
            (g_ulMakeingActiveTime + g_ulMakingIdleTime) < ulCurrTime)
        {
            GosLog(LOG_INFO, "stop task %u %u %u",
                   g_ulMakeingActiveTime, g_ulMakingIdleTime, ulCurrTime);
            StopMakingTask();
            return;
        }

        if (g_bAgentMode)
        {
            AllocMakeFileReq();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SendAllPeer(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    for (UINT32 i=0; i<g_vPeerInfo.size(); i++)
    {
        SendPeer(&g_vPeerInfo[i], usMsgID, pvMsg, ulMsgLen);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnPeerMsg(IP_ADDR_T *pstPeerAddr, UINT16 usMsgID, UINT8 *pucMsg, UINT32 ulMsgLen)
{
    if (usMsgID == EV_GET_MAKE_TASK_REQ)
    {
        OnGetMakeTaskReq(pstPeerAddr);
    }
    else if (usMsgID == EV_GET_MAKE_TASK_RSP)
    {
        if (ulMsgLen != sizeof(GET_MAKE_TASK_RSP_T))
        {
            goto fail;
        }

        OnGetMakeTaskRsp(pstPeerAddr, (GET_MAKE_TASK_RSP_T*)pucMsg);
    }
    else if (usMsgID == EV_STOP_MAKE_TASK_REQ)
    {
        if (ulMsgLen != sizeof(STOP_MAKE_TASK_REQ_T))
        {
            goto fail;
        }

        OnStopMakeTaskReq(pstPeerAddr, (STOP_MAKE_TASK_REQ_T*)pucMsg);
    }
    else if (usMsgID == EV_ALLOC_MAKE_FILE_REQ)
    {
        if (ulMsgLen != sizeof(ALLOC_MAKE_FILE_REQ_T))
        {
            goto fail;
        }

        OnAllocMakeFileReq(pstPeerAddr, (ALLOC_MAKE_FILE_REQ_T*)pucMsg);
    }
    else if (usMsgID == EV_ALLOC_MAKE_FILE_RSP)
    {
        if (ulMsgLen != sizeof(ALLOC_MAKE_FILE_RSP_T))
        {
            goto fail;
        }

        OnAllocMakeFileRsp(pstPeerAddr, (ALLOC_MAKE_FILE_RSP_T*)pucMsg);
    }
    else if (usMsgID == EV_REPORT_MAKE_INFO_REQ)
    {
        if (ulMsgLen != sizeof(REPORT_MAKE_INFO_REQ_T))
        {
            goto fail;
        }

        OnReportMakeInfoReq(pstPeerAddr, (REPORT_MAKE_INFO_REQ_T*)pucMsg);
    }
    else
    {
        GosLog(LOG_ERROR, "unknown msg %u", usMsgID);
    }

    return;

fail:
    GosLog(LOG_ERROR, "invalid msg len %u of msg %u", ulMsgLen, usMsgID);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnGetMakeTaskReq(IP_ADDR_T *pstPeerAddr)
{
    GET_MAKE_TASK_RSP_T     stRsp = {0};

    if (g_strMakeName.IsEmpty() ||
        g_strZipFile.IsEmpty())
    {
        return;
    }

    if (!FileMakeInfo::HasIdle())
    {
        return;
    }

    CHAR    *szZipFile = g_strZipFile.c_str();
    UINT32  ulTime;

    stRsp.ulFileLen = gos_get_file_size(szZipFile);
    gos_get_file_time(szZipFile, &ulTime, &stRsp.ulFileTime);

    strcpy(stRsp.acMakeName, g_strMakeName.c_str());

    szZipFile = gos_get_simple_file_name(szZipFile);
    strcpy(stRsp.acZipFile, szZipFile);

    StrCpy(stRsp.acMakeOption, g_stMakeEnv.strMakeOption);
    StrCpy(stRsp.acIncPath, g_strSrcIncPath);

    SendPeer(pstPeerAddr, EV_GET_MAKE_TASK_RSP, (UINT8*)&stRsp, sizeof(stRsp));
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnStopMakeTaskReq(IP_ADDR_T *pstPeerAddr, STOP_MAKE_TASK_REQ_T *pstReq)
{
    if (g_strMakingName != pstReq->acMakeName)
    {
        return;
    }

    StopMakingTask();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnAllocMakeFileReq(IP_ADDR_T *pstPeerAddr, ALLOC_MAKE_FILE_REQ_T *pstReq)
{
    if (g_strMakeName != pstReq->acMakeName)
    {
        OnGetMakeTaskReq(pstPeerAddr);

        return;
    }

    ALLOC_MAKE_FILE_RSP_T   stRsp = {0};
    FILE_MAKE_INFO_T        *pstInfo = FileMakeInfo::GetIdle(pstPeerAddr);

    memset(&stRsp, 0, sizeof(stRsp));
    strcpy(stRsp.acMakeName, pstReq->acMakeName);
    stRsp.ulIndex = pstReq->ulIndex;
    if (pstInfo)
    {
        StrCpy(stRsp.acSrcPath, g_strSrcPath);
        StrCpy(stRsp.acFile, pstInfo->strFile);

        ShowMakeResult(stRsp.acFile, "", MAKE_STATE_ALLOCED);
    }

    SendPeer(pstPeerAddr, EV_ALLOC_MAKE_FILE_RSP, (UINT8*)&stRsp, sizeof(stRsp));

   //   AnsiString  strFile = GetProjectFileName(stRsp.acFile);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnGetMakeTaskRsp(IP_ADDR_T *pstPeerAddr, GET_MAKE_TASK_RSP_T *pstRsp)
{
    // 非空表示正在进行
    if (g_strMakingName == pstRsp->acMakeName &&
        !g_strMakingName.IsEmpty())
    {
        return;
    }

    // 下载并解压缩文件
    GosLog(LOG_INFO, "sync file %s from "IP_FMT, pstRsp->acZipFile, IP_ARG(pstPeerAddr->aucIP));
    if (!DownloadMakeDir(pstPeerAddr->aucIP, pstRsp->acMakeName, pstRsp->acZipFile, pstRsp->ulFileLen))
    {
        GosLog(LOG_ERROR, "download make file %s from "IP_FMT" failed", pstRsp->acZipFile, IP_ARG(pstPeerAddr->aucIP));
        return;
    }

    GosLog(LOG_INFO, "new make task %s, prev make task is %s",
           pstRsp->acMakeName, g_strMakingName.c_str());

    // 进入make状态
    g_strMakingName = pstRsp->acMakeName;
    g_strMakingProject.printf("%s\\%s", gos_get_root_path(), pstRsp->acMakeName);

    ClearOldTask(g_strMakingProject.c_str());

    g_strMakingOption = pstRsp->acMakeOption;
    g_strMakingIncPath = pstRsp->acIncPath;

    memcpy(&g_stMakingPeer, pstPeerAddr, sizeof(g_stMakingPeer));

    MakingFilePool::Init(pstRsp->acMakeName);

    AllocMakeFileReq();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnAllocMakeFileRsp(IP_ADDR_T *pstPeerAddr, ALLOC_MAKE_FILE_RSP_T *pstRsp)
{
    // 非空表示正在进行
    if (g_strMakingName != pstRsp->acMakeName &&
        !g_strMakingName.IsEmpty())
    {
        return;
    }

    // 已经没有可分配的数据了， 设置为初始化状态
    if (pstRsp->acFile[0] == '\0')
    {
        MakingFilePool::Init(pstRsp->ulIndex);
        return;
    }

    MAKING_FILE_INFO_T  stInfo;

    memcpy(&stInfo.stPeerAddr, pstPeerAddr, sizeof(IP_ADDR_T));
    strcpy(stInfo.acMakeName, g_strMakingName.c_str());
    strcpy(stInfo.acProject, g_strMakingProject.c_str());
    strcpy(stInfo.acSrcPath, pstRsp->acSrcPath);
    strcpy(stInfo.acFile, pstRsp->acFile);
    strcpy(stInfo.acIncPath, g_strMakingIncPath.c_str());
    strcpy(stInfo.acMakeOption, g_strMakingOption.c_str());

    MakingFilePool::Alloced(pstRsp->ulIndex, stInfo);

}

//---------------------------------------------------------------------------
void __fastcall TFormMain::OnReportMakeInfoReq(IP_ADDR_T *pstPeerAddr, REPORT_MAKE_INFO_REQ_T *pstReq)
{
    // 非空表示正在进行
    if (g_strMakeName != pstReq->acMakeName)
    {
        return;
    }

    CHAR        acFile[256];
    CHAR        acMakeResult[1024];
    CHAR        acLocalObjZipFile[256];
    CHAR        acLocalObjFile[256];
    CHAR        *szObjFile = gos_get_simple_file_name(pstReq->acObjFile);
    UINT8       aucLocalAddr[] = {127,0,0,1};
    CHAR        acOriObjFile[256];
    BOOL        bSucc;

    if (pstReq->acObjFile[0] == '\0')
    {
        bSucc = FALSE;
    }
    else
    {
        FormatMakeResult(pstReq->acFile, pstReq->acMakeResult, bSucc);
    }

    // 下载并解压缩文件
    if (!bSucc)
    {
        GosLog(LOG_INFO, "get make result of %s, make error", pstReq->acFile);
    }
    else
    {
        GosLog(LOG_INFO, "get make result of %s, obj file is %s", pstReq->acFile, szObjFile);

        sprintf(acLocalObjZipFile, "%s/%s", g_strObjPath.c_str(), szObjFile);
        sprintf(acOriObjFile, "%s/%s", pstReq->acMakeName, szObjFile);

        strcpy(acLocalObjFile, acLocalObjZipFile);
        gos_format_path(acLocalObjFile);

        char    *szTmp = gos_right_strchr(acLocalObjFile, '.');

        if (szTmp)
        {
            *szTmp = '\0';
        }

        if (gos_file_exist(acLocalObjFile) &&
            gos_file_exist(acLocalObjZipFile) )
        {
            GosLog(LOG_INFO, "obj file %s is already exist", szObjFile);
        }
        else
        {
            if (memcmp(pstPeerAddr->aucIP, aucLocalAddr, 4) == 0)
            {
                gos_copy_file(acOriObjFile, acLocalObjZipFile);
                if (!gos_file_exist(acLocalObjZipFile))
                {
                    GosLog(LOG_ERROR, "copy file %s failed", pstReq->acObjFile);
                    return;
                }
            }
            else if (!ObjFile::Add(pstPeerAddr->aucIP, pstReq->acObjFile, acLocalObjZipFile))
            {
                GosLog(LOG_ERROR, "download file %s from "IP_FMT" failed", pstReq->acObjFile, IP_ARG(pstPeerAddr->aucIP));
                return;
            }
            else
            {
                GosLog(LOG_INFO, "add download file %s from "IP_FMT" succ", pstReq->acObjFile, IP_ARG(pstPeerAddr->aucIP));
            }
        }
    }

    AnsiString          strFile = GetProjectFileName(pstReq->acFile);
    FILE_MAKE_INFO_T    *pstMakeInfo = FileMakeInfo::Get(strFile);

    if (pstMakeInfo)
    {
        GosLog(LOG_INFO, "make time of %s is %u", pstReq->acFile, gos_get_uptime()-pstMakeInfo->ulAllocTime);
    }

    FileMakeInfo::SetResult(strFile, pstPeerAddr, pstReq->acMakeResult, bSucc);

    ShowMakeResult(strFile, pstReq->acMakeResult, bSucc?MAKE_STATE_SUCC:MAKE_STATE_FAIL);

    UINT32  ulUnMakedCount = FileMakeInfo::GetUnMakeCount();
    UINT32  ulTotalNum = FileMakeInfo::GetTotalMakeCount();

    pbMake->Properties->Max = ulTotalNum;
    pbMake->Position = ulTotalNum - ulUnMakedCount;

    pbMake->Properties->Text = gos_get_uptime() - g_ulMakeingStartTime;

    if (FileMakeInfo::IsAllMakeDone())
    {
        MakeDone();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MakeDone()
{
    btnStop->Enabled = false;
    btnOpen->Enabled = true;
    btnRebuild->Enabled = true;

    g_strMakeName = "";
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::AllocMakeFileReq()
{
    if (g_strMakingName.IsEmpty())
    {
        return;
    }

    if (MakingFilePool::IsEmpty() )
    {
        GosLog(LOG_INFO, "making pool is empty");
        g_strMakingName = "";
        return;
    }

    UINT32      ulIndex = 0;

    if (!MakingFilePool::Alloc(g_strMakingName.c_str(), ulIndex))
    {
        return;
    }

    ALLOC_MAKE_FILE_REQ_T  stReq;

    strcpy(stReq.acMakeName, g_strMakingName.c_str());
    stReq.ulIndex = ulIndex;

    SendPeer(&g_stMakingPeer, EV_ALLOC_MAKE_FILE_REQ, &stReq, sizeof(stReq));
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnStartClick(TObject *Sender)
{
    char    acFile[256];
 // char    acDir[256];
    char    acTime[32];
    char    acProject[64];
    char    *szEnd;
    char    acMakeName[256];
    char    acZipFile[256];

    StrCpy(acFile, edtProject->Text);
 // gos_get_file_path(acFile, acDir);

    ObjFile::Clear();

    gos_get_file_rawname(acFile, acProject);
    szEnd = gos_right_strchr(acProject, '.');
    if (!szEnd)
    {
        return;
    }

    *szEnd = '\0';

    gos_get_text_time_ex(NULL, "%04u%02u%02u_%02u%02u%02u", acTime);
    sprintf(acMakeName, "%s_%s", acTime, acProject);
    sprintf(acZipFile, "%s/%s.rar", gos_get_root_path(), acMakeName);

    if (!ZipMakeFile(g_strProjRootPath.c_str(), g_vMakePath, acZipFile))
    {
        GosLog(LOG_ERROR, "create zip file %s failed", acZipFile);
        InfoMsg("创建工程压缩文件失败");
        btnStopClick(this);
        return;
    }

    g_strMakeName = acMakeName;
    g_strZipFile = acZipFile;

    btnStop->Enabled = true;
    btnStart->Enabled = false;
    btnRebuild->Enabled = false;
    btnClear->Enabled = false;
    btnOpen->Enabled = false;

    g_ulMakeingStartTime = gos_get_uptime();

    GosLog(LOG_INFO, "start make task %s", acMakeName);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerInitTimer(TObject *Sender)
{
    if (!TimerInit->Enabled)
    {
        return;
    }

    if (g_bAgentMode)
    {
        Visible = false;
    }

    if (!g_pTransFile)
    {
        UINT16      usPort = g_bAgentMode? g_usServerTransFilePort:g_usClientTransFilePort;

        g_pTransFile = new GTransFile(usPort, TRUE);

        g_pTransFile->Start();
    }

    UINT16  usUDPPort = g_bAgentMode?g_usServerUDPPort:g_usClientUDPPort;

    if (!InitUDPServer(usUDPPort))
    {
        return;
    }

    InitDownloadThread();

    InitThreadMake();

    TimerInit->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnInnerMakeMsg(TMessage Message)
{
    INNER_MAKE_MSG_T    *pstInnerMsg = (INNER_MAKE_MSG_T*)Message.LParam;
    MAKE_MSG_HDR_T      *pstMsg = (MAKE_MSG_HDR_T*)pstInnerMsg->aucMsg;

    OnPeerMsg(&pstInnerMsg->stPeerAddr, pstMsg->usMsgID, (UINT8*)(pstMsg+1), pstMsg->usMsgLen);

    free(pstInnerMsg);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::InitTrayIcon()
{
    TrayIcon = new TCTrayIcon(this);

//  TrayIcon->Visible = false;
    TrayIcon->OnMinimize = TrayIconMinimize;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TrayIconMinimize(TObject *Sender)
{
    Hide();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TrayIconRestore(TObject *Sender)
{
    Show();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ShowMakeResult()
{
    TcxTreeListNode *Node;

    if (tlSourceFile->SelectionCount == 0)
    {
        return;
    }

    Node = tlSourceFile->Selections[0];

    TcxTreeListColumn   *Column = tlSourceFile->FocusedColumn;

    if (!Column)
    {
        return;
    }

    int     iColIndex = Column->Position->ColIndex;

    if (iColIndex == 1 || iColIndex == 3 || iColIndex == 5)
    {
        if (Node->Values[iColIndex-1].IsNull())
        {
            return;
        }

        AnsiString  strFile = Node->Values[iColIndex-1];
        AnsiString  strResult;
        UINT32      ulState;
        IP_ADDR_T   stPeer;

        if (strFile.IsEmpty())
        {
            return;
        }

        if (FileMakeInfo::GetResult(strFile, stPeer, strResult, ulState))
        {
            mmInfo->Text = "编译主机: " + GetTextIP(stPeer.aucIP);
            mmInfo->Lines->Append("编译结果: " + GetMakeStateText(ulState));
            mmInfo->Lines->Append("");
            mmInfo->Lines->Append(strResult);
        }
        else
        {
            mmInfo->Text = "未知文件";
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::tlSourceFileFocusedNodeChanged(TcxCustomTreeList *Sender,
          TcxTreeListNode *APrevFocusedNode, TcxTreeListNode *AFocusedNode)
{
    ShowMakeResult();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::tlSourceFileFocusedColumnChanged(TcxCustomTreeList *Sender,
          TcxTreeListColumn *APrevFocusedColumn, TcxTreeListColumn *AFocusedColumn)

{
    ShowMakeResult();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerCheckObjTimer(TObject *Sender)
{
    VECTOR<FILE_MAKE_INFO_T*>   vLocalMake;

 /* if (g_strMakeName.IsEmpty())
    {
        return;
    }   */

    if (FileMakeInfo::IsAllMakeDone())
    {
        MakeDone();
    }

    FileMakeInfo::CheckObj(vLocalMake);

    for (UINT32 i=0; i<vLocalMake.size(); i++)
    {
        ShowMakeResult(vLocalMake[i]->strFile, "", MAKE_STATE_COMPILED);
    }

    if (vLocalMake.size() > 0)
    {
        pbMake->Properties->Text = gos_get_uptime() - g_ulMakeingStartTime;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::edtProjectPropertiesButtonClick(TObject *Sender, int AButtonIndex)
{
    edtProject->Text = "";
    OpenDialog->Title = "打开工程文件";

    char    acFile[256];

    if (!OpenDialog->Execute())
    {
        return;
    }

    StrCpy(acFile, OpenDialog->FileName);

    LoadProjectFile(acFile);
}
//---------------------------------------------------------------------------



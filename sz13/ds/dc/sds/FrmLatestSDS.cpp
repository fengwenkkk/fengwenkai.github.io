//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmLatestSDS.h"
#include "main.h"

#include "FrmCallIn.h"
#include "DlgNewSDS.h"
#include "DlgNewSMS.h"
#include "DlgSetOperator.h"
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
#pragma link "cxSplitter"
#pragma link "cxImage"
#pragma link "dxGDIPlusClasses"
#pragma link "cxImageComboBox"
#pragma resource "*.dfm"
TFormLatestSDS *FormLatestSDS;

static UINT32           g_ulCurrSDSID = 0;
static VECTOR<UINT32>   g_vRecvSDSFailedUser;

extern void SetFormParent(TForm *Child, TWinControl *Parent);
extern BOOL LoadSDSInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulSDSType,
                        CHAR *szSendUserName, CHAR *szKeyword, VECTOR<SDS_INFO_T> &vInfo);

BOOL SendSDS(const CHAR *szMsg, UINT32 ulSDSID, VECTOR<UINT32> &vUser)
{
    if (SendSMSByRealMsg())
    {
        return eSDK_SendRealMsg(szMsg, ulSDSID, vUser);
    }
    else
    {
        VECTOR<INT64> vGroup;

        return eSDK_SendMsg(szMsg, ulSDSID, vUser, vGroup);
    }
}

BOOL SendSDSInfo(SDS_INFO_T &stInfo, VECTOR<UINT32> &vUser)
{
    AnsiString  strCmd = "";
    CHAR        acCmd[1024];
    UINT32      ulLen = 0;

    stInfo.ulSendTime = gos_get_current_time();

    if (stInfo.ulSDSType == SDS_TYPE_FORMAL_DISPATCH ||
        stInfo.ulSDSType == SDS_TYPE_ORAL_DISPATCH )
    {
        ulLen += sprintf(acCmd+ulLen, "调度命令编号:%u,", stInfo.ulSDSIndex);
        ulLen += sprintf(acCmd+ulLen, "发送时间:%s,", gos_get_text_time(&stInfo.ulSendTime));
        ulLen += sprintf(acCmd+ulLen, "发送人:%s,", stInfo.acSendUserName);
        ulLen += sprintf(acCmd+ulLen, "类型:%s,", stInfo.ulSDSType==SDS_TYPE_FORMAL_DISPATCH?"书面命令":"口头命令");
        ulLen += sprintf(acCmd+ulLen, "内容:");

        strCmd = AnsiString(acCmd) + AnsiString(stInfo.acSDSInfo);

        // 调度命令采用实时短信发送
        if (!SendSDS(strCmd.c_str(), stInfo.ulSDSID, vUser))
        {
            GosLog(LOG_ERROR, "send dispatch msg failed: %s", strCmd.c_str());
            return FALSE;
        }
    }
    else if (stInfo.ulSDSType == SDS_TYPE_OPERATE)
    {
        ulLen += sprintf(acCmd+ulLen, "运营信息编号:%u,", stInfo.ulSDSIndex);
        ulLen += sprintf(acCmd+ulLen, "发送时间:%s,", gos_get_text_time(&stInfo.ulSendTime));
        ulLen += sprintf(acCmd+ulLen, "发送人:%s,", stInfo.acSendUserName);
        ulLen += sprintf(acCmd+ulLen, "内容:");

        strCmd = AnsiString(acCmd) + AnsiString(stInfo.acSDSInfo);

        // 运营信息采用实时短信发送
        if (!SendSDS(strCmd.c_str(), stInfo.ulSDSID, vUser))
        {
            GosLog(LOG_ERROR, "send operate msg failed: %s", strCmd.c_str());
            return FALSE;
        }
    }
    else if (stInfo.ulSDSType == SDS_TYPE_NORMAL ||
             stInfo.ulSDSType == SDS_TYPE_EMERG_STATUS ||
             stInfo.ulSDSType == SDS_TYPE_STATUS )
    {
        ulLen += sprintf(acCmd+ulLen, "发送时间:%s,", gos_get_text_time(&stInfo.ulSendTime));
        ulLen += sprintf(acCmd+ulLen, "发送人:%s,", stInfo.acSendUserName);
        ulLen += sprintf(acCmd+ulLen, "内容:");

        strCmd = AnsiString(acCmd) + AnsiString(stInfo.acSDSInfo);

        if (!SendSDS(strCmd.c_str(), stInfo.ulSDSID, vUser))
        {
            GosLog(LOG_ERROR, "send msg failed: %s", strCmd.c_str());
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }

    GosLog(LOG_INFO, "send msg succ: %s", strCmd.c_str());
    return TRUE;
}

BOOL SendSDSInfo(SDS_INFO_T &stInfo, IntSet &sUser)
{
    VECTOR<UINT32>  vUser;

    sUser.GetList(vUser);

    return SendSDSInfo(stInfo, vUser);
}

void __fastcall TFormLatestSDS::InitWorkPanel()
{
    TcxButton   *btns[] = { btnRefresh,
                            btnAdd,
                            btnAddSms,
                            btnOpenAttachFile,
                            btnResend,
                            btnReply
                          };

    gbWorkPanel->Width = 120;

    for (UINT32 i=0; i<ARRAY_SIZE(btns); i++)
    {
        btns[i]->TabOrder = i;
        btns[i]->Top = 50+95*i;
        btns[i]->Width = g_iMidToolbarButtonWidth;
        btns[i]->Height = g_iMidToolbarButtonHeight;
        btns[i]->Left = (btns[i]->Parent->Width - btns[i]->Width)/2;
    }
}
//---------------------------------------------------------------------------
__fastcall TFormLatestSDS::TFormLatestSDS(TComponent* Owner)
    : TSubForm(Owner)
{
    InitWorkPanel();

    m_FormMediaPlayer = NULL;
    m_bInitView = false;

    if (IsSimpleSMSMode())
    {
        ColumnConfirmRate->Visible = false;
        ColumnNeedReply->Visible = false;

        ColumnConfirmTime->Visible = false;
    }

    sprintf(m_acSMSPath, "%s/data/mms/%u", GetNativeServicePath(), APP::GetCurrDCUserID());
    gos_format_path(m_acSMSPath);

    InitMsgHandler(EV_DC_DIS_RSP, MSG_DCGetLatestSDSInfo, OnGetSDSInfoRsp);
    InitMsgHandler(EV_DC_DIS_RSP, MSG_DCGetSDSReply, OnGetSDSReplyRsp);
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::btnRefreshClick(TObject *Sender)
{
    RefreshSDSInfo();
}
//---------------------------------------------------------------------------
// MESSAGE DATE="2021-10-13" FLAG="0" FROM="11002" MSGID="1634113871429" NOTICEID="12001"
// SUBJECT="11002" TIME="16:31:10" TRID="1634113870385" TYPE="0001">
static AnsiString GetXmlAttr(CHAR *szXml, const CHAR *szAttr)
{
    CHAR    *szEnd;
    CHAR    *szValue = strstr(szXml, szAttr);

    if (!szValue)
    {
        return "";
    }

    szValue += strlen(szAttr);
    szValue = gos_left_trim_string(szValue);
    if (*szValue++ != '=')
    {
        return "";
    }

    szValue = gos_left_trim_string(szValue);

    if (*szValue++ != '"')
    {
        return "";
    }

    szEnd = strchr(szValue, '"');
    if (!szEnd)
    {
        return "";
    }

    UINT32      ulLen = szEnd - szValue;
    AnsiString  strValue(szValue, ulLen);

    return strValue;
}

static AnsiString GetXmlNodeValue(CHAR *szXml, const CHAR *szAttr)
{
    AnsiString  strAttr;

    strAttr.printf("<%s>", szAttr);

    CHAR    *szStart = strstr(szXml, strAttr.c_str());

    if (!szStart)
    {
        return "";
    }

    szStart += strAttr.Length();

    strAttr.printf("</%s>", szAttr);

    CHAR    *szEnd = strstr(szStart, strAttr.c_str());

    if (!szEnd)
    {
        return NULL;
    }

    int iLen = szEnd-szStart;

    AnsiString strValue = AnsiString(szStart, iLen);

    return strValue;

  /*  char    *szTmp = (char*)malloc(iLen+1);

    memcpy(szTmp, szStart, iLen);
    szTmp[iLen] = '\0';
//    AnsiString  strValue(szStart, (int)(szEnd-szStart));

    AnsiString  strValue = szTmp;

    iLen = strValue.Length();

    strValue.printf("%s", szTmp);

    iLen = strValue.Length();

    strValue = AnsiString(szStart, iLen);

    free(szTmp);
    return strValue;  */
}

/*
char* utf8_to_ascii(char* szUtf8)
{
    int         wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szUtf8, strlen(szUtf8), NULL, 0);
    wchar_t*    wszString = new wchar_t[wcsLen + 1];

    ::MultiByteToWideChar(CP_UTF8, NULL, szUtf8, strlen(szUtf8), wszString, wcsLen);
    wszString[wcsLen] = '\0';

    int ansiLen = ::WideCharToMultiByte(CP_ACP, NULL, wszString, wcslen(wszString), NULL, 0, NULL, NULL);
    char* szAnsi = (char*)malloc(ansiLen+1);

    ::WideCharToMultiByte(CP_ACP, NULL, wszString, wcslen(wszString), szAnsi, ansiLen, NULL, NULL);
    szAnsi[ansiLen] = '\0';

    delete wszString;

    return szAnsi;
}
*/

void __fastcall TFormLatestSDS::LoadLocalSMSInfo(VECTOR<SDS_INFO_T> &vInfo, UINT32 ulStartTime)
{
    CHAR        acFile[512];
    SDS_INFO_T  stSMS;
    CHAR        acDate[32];
    CHAR        acTime[32];
    CHAR        acType[8];
    CHAR        *szValue;
    INT32       iLen;
    GOS_DATETIME_T stTime;
    FILE        *fp;

    sprintf(acFile, "%s/inbox.xml", m_acSMSPath);
    iLen = gos_get_file_size(acFile);
    if (iLen <= 0)
    {
        return;
    }

    szValue = (CHAR*)malloc(iLen+1);
    fp = fopen(acFile, "rb");
    fread(szValue, 1, iLen, fp);
    fclose(fp);
    szValue[iLen] = '\0';

    CHAR *szSMS = gos_utf8_to_ascii(szValue, NULL);

    free(szValue);

    if (!szSMS)
    {
        return;
    }

    CHAR        *szMsg = szSMS;
    CHAR        *szNextMsg = NULL;
    CHAR        *szTmp;
    CHAR        *szContent;
    CHAR        *szFile;
    AnsiString  strValue;

    for(; szMsg; szMsg=szNextMsg)
    {
        memset(&stSMS, 0, sizeof(stSMS));

        szMsg = strstr(szMsg, "<MESSAGE");
        if (!szMsg)
        {
            break;
        }

        szMsg += strlen("<MESSAGE");
        szTmp = strstr(szMsg, "</MESSAGE>");
        if (!szTmp)
        {
            break;
        }

        *szTmp = '\0';
        szNextMsg = szTmp + strlen("</MESSAGE>");

        if (strstr(szMsg, "16:17"))
        {
            szMsg += 0;
        }

        strValue = GetXmlNodeValue(szMsg, "CONTENT");
        strncpy(stSMS.acSDSInfo, strValue.c_str(), sizeof(stSMS.acSDSInfo)-1);

        strValue = GetXmlNodeValue(szMsg, "ATTACHFILE");
        if (!strValue.IsEmpty())
        {
            snprintf(stSMS.acFile, sizeof(stSMS.acFile)-1, "%s/%s", GetNativeServicePath(), strValue.c_str());
            gos_format_path(stSMS.acFile);
        }

        szTmp = strchr(szMsg, '>');
        if (!szTmp)
        {
            continue;
        }

        *szTmp = '\0';

        strValue = GetXmlAttr(szMsg, "TYPE");
        if (strValue != SDS_TYPE_TEXT_SMS)
        {
            continue;
        }

        strValue = GetXmlAttr(szMsg, "FROM");
        if (!gos_atou(strValue.c_str(), &stSMS.ulSendUserID))
        {
            continue;
        }

        strValue = GetXmlAttr(szMsg, "DATE") + " " + GetXmlAttr(szMsg, "TIME");
        if (!gos_parse_time(strValue.c_str(), &stTime))
        {
            continue;
        }

        stSMS.ulSendTime = gos_mktime(&stTime);
        if (stSMS.ulSendTime < ulStartTime)
        {
            continue;
        }

        strValue = GetXmlAttr(szMsg, "MSGID");
        szValue = strValue.c_str();
        iLen = strValue.Length();
        if (iLen > 9)
        {
            szValue += (iLen - 9);
        }

        if (!gos_atou(szValue, &stSMS.ulSDSID))
        {
            continue;
        }

        sprintf(stSMS.acSendUserName, "%u", stSMS.ulSendUserID);

        stSMS.ulSDSType = SDS_TYPE_SMS;

        vInfo.push_back(stSMS);
    }

    free(szSMS);

}
#if 0
void __fastcall TFormLatestSDS::LoadLocalSMSInfo(VECTOR<SDS_INFO_T> &vInfo, UINT32 ulStartTime)
{
    GXmlDoc     *pDoc = GXmlDoc::New();
    CHAR        acFile[512];
    GXmlNode    *pRootNode;
    GXmlNode    *pNode;
    GXmlNode    *pSubNode;
    SDS_INFO_T  stSMS;
    CHAR        acDate[32];
    CHAR        acTime[32];
    CHAR        acType[8];
    CHAR        *szValue;
    INT32       iLen;
    GOS_DATETIME_T stTime;
    TStringList     *slFile = new TStringList();

    sprintf(acFile, "%s/inbox.xml", m_acSMSPath);
    slFile->LoadFromFile(acFile);

    AnsiString      strSMSInfo = slFile->Text;

    strSMSInfo = UTF8ToString(strSMSInfo.c_str()).c_str();

    delete slFile;

    const char      *szSMS = strSMSInfo.c_str();

    if (!pDoc->ParseString((CHAR*)szSMS))
    {
        goto end;
    }

    pRootNode = pDoc->GetRootNode();
    if (!pRootNode)
    {
        goto end;
    }

    pRootNode = pRootNode->GetFirstChild("RECIVER");
    if (!pRootNode)
    {
        goto end;
    }

    pNode = pRootNode->GetFirstChild("MESSAGE");
    for(; pNode; pNode=pNode->GetSibling())
    {
/*    <MESSAGE DATE="2021-10-13" FLAG="0" FROM="11002" MSGID="1634113871429" NOTICEID="12001"
SUBJECT="11002" TIME="16:31:10" TRID="1634113870385" TYPE="0001">
      <CONTENT>111</CONTENT>       </MESSAGE>*/
        memset(&stSMS, 0, sizeof(stSMS));

        if (!pNode->GetAttrValue("DATE", acDate, 10+1) ||
            !pNode->GetAttrValue("TIME", acTime, 8+1) ||
            !pNode->GetAttrValue("FROM", &stSMS.ulSendUserID) ||
            !pNode->GetAttrValue("TYPE", acType, sizeof(acType))  )
        {
            continue;
        }

        if (strcmp(acType, SDS_TYPE_TEXT_SMS) != 0)
        {
            continue;
        }

        sprintf(acDate+strlen(acDate), " %s", acTime);
        if (!gos_parse_time(acDate, &stTime))
        {
            continue;
        }

        stSMS.ulSendTime = gos_mktime(&stTime);
        if (stSMS.ulSendTime < ulStartTime)
        {
            continue;
        }

        szValue = pNode->GetAttrValue("MSGID");
        if (!szValue)
        {
            continue;
        }

        iLen = strlen(szValue);
        if (iLen > 9)
        {
            szValue += (iLen - 9);
        }

        if (!gos_atou(szValue, &stSMS.ulSDSID))
        {
            continue;
        }

        sprintf(stSMS.acSendUserName, "%u", stSMS.ulSendUserID);

        stSMS.ulSDSType = SDS_TYPE_SMS;

        pSubNode = pNode->GetFirstChild("CONTENT");
        if (pSubNode)
        {
            szValue = pSubNode->GetValue();
            if (szValue)
            {
                strncpy(stSMS.acSDSInfo, szValue, sizeof(stSMS.acSDSInfo)-1);
            }
        }

        // <CONTENT/>
        // <ATTACHFILE>./data/mms//13006/20211102_183421_895_from_11001/IMG_20211102_183349.jpg</ATTACHFILE>
        pSubNode = pNode->GetFirstChild("ATTACHFILE");
        if (pSubNode)
        {
            szValue = pSubNode->GetValue();
            if (szValue)
            {
                snprintf(stSMS.acFile, sizeof(stSMS.acFile)-1, "%s/%s", GetNativeServicePath(), szValue);
                gos_format_path(stSMS.acFile);
            }
        }

        vInfo.push_back(stSMS);
    }

end:
    GXmlDoc::Delete(pDoc);
}
#endif
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::AddLocalSMSInfo(VECTOR<SDS_INFO_T> &vInfo)
{
    VECTOR<SDS_INFO_T>      vSMSInfo;
    UINT32                  ulStartTime = gos_get_current_time();
    GOS_DATETIME_T          stTime;

    gos_get_localtime(&ulStartTime, &stTime);
    stTime.ucHour = 0;
    stTime.ucMinute = 0;
    stTime.ucSecond = 0;
    stTime.ucSn = 0;

    ulStartTime = gos_mktime(&stTime);

    LoadLocalSMSInfo(vSMSInfo, ulStartTime);

    vInfo.clear();
    vInfo.assign(vSMSInfo.begin(), vSMSInfo.end());
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::ShowSDSInfo(VECTOR<SDS_INFO_T> &vInfo)
{
    TcxTreeListNode     *Node;
    CHAR                acRate[64];
    BOOL                bNeedReply = FALSE;

    m_bInitView = true;

//    AddLocalSMSInfo(vInfo);

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
            if (stInfo.ulRecvSuccUserNum > stInfo.ulRecvUserNum)
            {
                stInfo.ulRecvSuccUserNum = stInfo.ulRecvUserNum;
            }

            sprintf(acRate, "%u/%u", stInfo.ulRecvSuccUserNum, stInfo.ulRecvUserNum);
            Node->Values[6] = acRate;
        }

        if (stInfo.ulRecvUserNum == 0 ||
            !stInfo.bNeedReply)
        {
            Node->Values[7] = "";
        }
        else
        {
            if (stInfo.ulReplyUserNum > stInfo.ulRecvUserNum)
            {
                stInfo.ulReplyUserNum = stInfo.ulRecvUserNum;
            }

            sprintf(acRate, "%u/%u", stInfo.ulReplyUserNum, stInfo.ulRecvUserNum);
            Node->Values[7] = acRate;
        }

        Node->Values[8] = stInfo.acSDSInfo;
        Node->Values[9] = stInfo.bNeedReply;

        if (stInfo.ulSDSType == SDS_TYPE_SMS)
        {
            Node->Values[10] = stInfo.acFile;
        }

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

    GosLog(LOG_INFO, "udpate latest SDS over");
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::tlMainFocusedNodeChanged(TcxCustomTreeList *Sender,
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
void __fastcall TFormLatestSDS::ShowDetail()
{
    g_ulCurrSDSID = 0;
    btnResend->Enabled = false;

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
        (UINT32)Node->Values[2] == SDS_TYPE_NORMAL)
    {
        btnReply->Enabled = true;
    }
    else
    {
        btnReply->Enabled = false;
    }

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

    if (!Node || Node->Values[8].IsNull())
    {
        mmSDSInfo->Text = "";
        return;
    }

    mmSDSInfo->Text = Node->Values[8];

    if (Node->Values[9].IsNull())
    {
        return;
    }

    BOOL    bNeedReply = Node->Values[9];

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

AnsiString LoadTextFile(CHAR *szFile)
{
    INT32   iLen = gos_get_file_size(szFile);

    if (iLen <= 0)
    {
        return "";
    }

    CHAR    *szText = (CHAR*)calloc(1, iLen+1);

    FILE *fp = fopen(szFile, "rb");

    fread(szText, 1, iLen, fp);

    fclose(fp);

    CHAR    *szAnsiText = gos_utf8_to_ascii(szText, NULL);

    AnsiString  strText = szAnsiText;

    free(szText);
    free(szAnsiText);

    return strText;
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::AutoShowSMSAttach(TcxTreeListNode *Node)
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

    if (Node->Values[10].IsNull())
    {
        goto error;
    }

    strFile = Node->Values[10];
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
void __fastcall TFormLatestSDS::btnAddClick(TObject *Sender)
{
    NewSDS(INVALID_USER_ID);
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::NewSDS(UINT32 ulUserID)
{
    VECTOR<UINT32> vUserID;

    vUserID.push_back(ulUserID);

    NewSDS(vUserID);
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::NewSDS(VECTOR<UINT32> &vUserID)
{
    VECTOR<INT64>   vGroupID;

    NewSDS(vUserID, vGroupID);
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::NewSDS(VECTOR<INT64> &vGroupID)
{
    VECTOR<UINT32>  vUserID;

    NewSDS(vUserID, vGroupID);
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::NewSDS(VECTOR<UINT32> &vUserID, VECTOR<INT64> &vGroupID)
{
    DialogNewSDS->Init(vUserID, vGroupID);
    if (DialogNewSDS->ShowModal() != mrOk)
    {
        return;
    }

    SDS_INFO_T      stInfo;
    VECTOR<UINT32>  vUser;
    VECTOR<INT64>   vGroup;

    GJsonParam          ReqJsonParam;
    GJson               RspJson;
    GJsonTupleParam     JsonTupleParam;
    BOOL                bRet = FALSE;
    VECTOR<UINT32>      vMemberUser;
    VECTOR<INT64>       vMemberGroup;
    IntSet              sUser;

    DialogNewSDS->Get(stInfo, vUser, vGroup);

    for (UINT32 i=0; i<vUser.size(); i++)
    {
        sUser.Add(vUser[i]);
    }

    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        eSDK_GetGroupMember(vGroup[i], vMemberUser, vMemberGroup);
        sUser.Add(vMemberUser);
    }

    sUser.Del(APP::GetCurrDCUserID());

    if (sUser.Size() == 0)
    {
        InfoMsg("没有有效的接收者");
        return;
    }

    for (UINT32 i=0; i<sUser.Size(); i++)
    {
        GJsonParam  SubJsonParam;

        SubJsonParam.Add("UserID", sUser[i]);
        JsonTupleParam.Add(SubJsonParam);
    }

    ReqJsonParam.Add("SendUserID", stInfo.ulSendUserID);
    ReqJsonParam.Add("SendUserName", stInfo.acSendUserName);
    ReqJsonParam.Add("SDSType", stInfo.ulSDSType);
    ReqJsonParam.Add("SDSInfo", EncodeSMSMsg(stInfo.acSDSInfo));
    ReqJsonParam.Add("File", "");
    ReqJsonParam.Add("NeedReply", stInfo.bNeedReply);
    ReqJsonParam.Add("User", JsonTupleParam);

    if (!SyncSendServer(EV_DC_DIS_REQ, MSG_DCAddSDSInfo, ReqJsonParam, RspJson))
    {
        GosLog(LOG_ERROR, "sync send msg MSG_DCAddSDSInfo to server failed");
        WarnMsg("操作失败");
        goto end;
    }

    if (!RspJson.GetValue("SDSID", &stInfo.ulSDSID) ||
        !RspJson.GetValue("SDSIndex", &stInfo.ulSDSIndex))
    {
        GosLog(LOG_ERROR, "parse msg %s failed", RspJson.GetText());
        WarnMsg("操作失败");
        goto end;
    }

    if (!SendSDSInfo(stInfo, sUser))
    {
        GosLog(LOG_ERROR, "send real msg %s failed", stInfo.acSDSInfo);
        WarnMsg("操作失败!");
        goto end;
    }

    bRet = TRUE;

end:
    ReqJsonParam.Clear();

    ReqJsonParam.Add("SDSID", stInfo.ulSDSID);

    const CHAR  *szMsgName = bRet?MSG_DCSetSDSInfo:MSG_DCDelSDSInfo;

    SendServer(EV_DC_DIS_REQ, szMsgName, ReqJsonParam);
    if (bRet)
    {
        RefreshSDSInfo();
        InfoMsg("操作成功!");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::OnGetSDSInfoRsp(UINT32 ulSeqID, GJson &Json)
{
    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = Json.GetSubJson();
    SDS_INFO_T      stInfo;
    VECTOR<SDS_INFO_T>  vInfo;

    GosLog(LOG_INFO, "udpate latest SDS");

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];

        if (!pSubJson->GetValue("SDSID", &stInfo.ulSDSID) ||
            !pSubJson->GetValue("SendUserID", &stInfo.ulSendUserID) ||
            !pSubJson->GetValue("SendUserName", stInfo.acSendUserName, sizeof(stInfo.acSendUserName)) ||
            !pSubJson->GetValue("SendTime", &stInfo.ulSendTime) ||
            !pSubJson->GetValue("SDSIndex", &stInfo.ulSDSIndex) ||
            !pSubJson->GetValue("SDSType", &stInfo.ulSDSType) ||
            !pSubJson->GetValue("SDSInfo", stInfo.acSDSInfo, sizeof(stInfo.acSDSInfo)) ||
            !pSubJson->GetValue("NeedReply", &stInfo.bNeedReply) ||
            !pSubJson->GetValue("RecvUserNum", &stInfo.ulRecvUserNum) ||
            !pSubJson->GetValue("RecvSuccUserNum", &stInfo.ulRecvSuccUserNum) ||
            !pSubJson->GetValue("ReplyUserNum", &stInfo.ulReplyUserNum) ||
            !pSubJson->GetValue("File", stInfo.acFile, sizeof(stInfo.acFile)) )
        {
            GosLog(LOG_ERROR, "parse sds info failed on %s", pSubJson->GetText());
            return;
        }

        DecodeJsonMsg(stInfo.acSDSInfo, sizeof(stInfo.acSDSInfo)-1);

        vInfo.push_back(stInfo);
    }

    ShowSDSInfo(vInfo);
}

//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::OnGetSDSReplyRsp(UINT32 ulSeqID, GJson &Json)
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

    g_ulCurrSDSID = ulSDSID;
    g_vRecvSDSFailedUser.clear();

    tlSDSReply->BeginUpdate();
    tlSDSReply->Clear();

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];

        if (!pSubJson->GetValue("UserID", &stInfo.ulUserID) ||
            !pSubJson->GetValue("RecvTime", &stInfo.ulRecvTime)  ||
            !pSubJson->GetValue("SendResult", &stInfo.ulSendResult) ||
            !pSubJson->GetValue("ReplyTime", &stInfo.ulReplyTime) )
        {
            GosLog(LOG_ERROR, "parse sds reply failed on %s", pSubJson->GetText());
            continue;
        }

        Node = tlSDSReply->Add();
        Node->Values[0] = stInfo.ulUserID;
        Node->Values[1] = stInfo.ulRecvTime;
        Node->Values[2] = stInfo.ulReplyTime;

        if (stInfo.ulRecvTime == 0)
        {
            g_vRecvSDSFailedUser.push_back(stInfo.ulUserID);
        }
    }

    if (g_vRecvSDSFailedUser.size() > 0)
    {
        btnResend->Enabled = true;
    }

    tlSDSReply->EndUpdate();

    tlSDSReply->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::TimerCheckTimer(TObject *Sender)
{
    if (!m_FormMediaPlayer)
    {
        m_FormMediaPlayer = FormMediaPlayer;
//        Application->CreateForm(__classid(TFormMediaPlayer), &m_FormMediaPlayer);
        SetFormParent(m_FormMediaPlayer, PanelMediaPlayer);
    }

    TimerCheck->Enabled = false;
    RefreshSDSInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::RefreshSDSInfo()
{
    GJsonParam  Param;

    SendServer(EV_DC_DIS_REQ, MSG_DCGetLatestSDSInfo, Param);
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::tlMainDblClick(TObject *Sender)
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
        Node->Values[9].IsNull() )
    {
        return;
    }

    UINT32  ulSDSID = Node->Values[0];
    BOOL    bNeedReply = Node->Values[9];

    if (!bNeedReply)
    {
        return;
    }

    GJsonParam  Param;

    Param.Add("SDSID", ulSDSID);

    SendServer(EV_DC_DIS_REQ, MSG_DCGetSDSReply, Param);
}

bool IsAckSDS(const char *szSDSContent, UINT32 &ulSDSID)
{
    AnsiString  strSDS = AnsiString(szSDSContent).UpperCase().Trim();
    CHAR        *szSDS = strSDS.c_str();

    if (gos_str_like(szSDS, "HZ") )
    {
        szSDS += strlen("HZ");
    }
    else if (gos_str_like(szSDS, "回执") )
    {
        szSDS += strlen("回执");
    }
    else
    {
        return false;
    }

    return gos_atou(szSDS, &ulSDSID);
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::ShowNewSMS(EVENT_SDS_REPORT_T &stSDS)
{
    UINT32      ulSDSID = 0;
    UINT32      ulUserID = INVALID_USER_ID;
    CHAR        *szUserName = NULL;

    FormCallIn->StartSDSAlarm();

    if (IsAckSDS(stSDS.acContent, ulSDSID))
    {
        GosLog(LOG_INFO, "receive SMS(%s) reply from %s", stSDS.acContent, stSDS.acFrom);

        SaveUESDSReply(stSDS.acFrom, ulSDSID);
        return;
    }

    if (gos_atou(stSDS.acFrom, &ulUserID) )
    {
        szUserName = User::GetUserName(ulUserID);
    }

    if (!szUserName)
    {
        szUserName = stSDS.acFrom;
    }

    if (AnsiString(szUserName) == APP::GetCurrDCUserID() &&
        stSDS.acContent[0] == '\0')
    {
        return;
    }

    TcxTreeListNode     *Node = tlMain->Add();

    Node->Values[0] = 0;
    Node->Values[1] = AnsiString(stSDS.acSendDate) + " " + AnsiString(stSDS.acSendTime);
    //Node->Values[2] = SDS_TYPE_SMS_MMS;// "彩信";
    Node->Values[2] = SDS_TYPE_SMS;// "彩信";
    Node->Values[3] = "";
    Node->Values[4] = 0;
    Node->Values[5] = szUserName;
    Node->Values[6] = "";
    Node->Values[7] = "";
    Node->Values[8] = stSDS.acContent;
    Node->Values[9] = FALSE;
    Node->Values[10] = stSDS.acFile;

    SaveNewSMS(stSDS.acFrom, szUserName, stSDS.acContent, stSDS.acFile);
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::ShowNewSDS(EVENT_SDS_REPORT_T &stSDS)
{
    UINT32      ulSDSID = 0;

    FormCallIn->StartSDSAlarm();

    if (IsAckSDS(stSDS.acContent, ulSDSID))
//    if (gos_atou(stSDS.acContent, &ulSDSID))
    {
        GosLog(LOG_INFO, "receive SDS(%s) reply from %s", stSDS.acContent, stSDS.acFrom);

        SaveUESDSReply(stSDS.acFrom, ulSDSID);
        return;
    }

    UINT32      ulUserID = INVALID_USER_ID;
    CHAR        *szUserName = NULL;

    if (gos_atou(stSDS.acFrom, &ulUserID) )
    {
        szUserName = User::GetUserName(ulUserID);
    }

    if (!szUserName)
    {
        szUserName = stSDS.acFrom;
    }

    if (AnsiString(szUserName) == APP::GetCurrDCUserID() &&
        stSDS.acContent[0] == '\0')
    {
        return;
    }

    TcxTreeListNode     *Node = tlMain->Add();

    Node->Values[0] = 0;
    Node->Values[1] = AnsiString(stSDS.acSendDate) + " " + AnsiString(stSDS.acSendTime);
    Node->Values[2] = SDS_TYPE_NORMAL;// "普通信息";
    Node->Values[3] = "";
    Node->Values[4] = 0;
    Node->Values[5] = szUserName;
    Node->Values[6] = "";
    Node->Values[7] = "";
    Node->Values[8] = stSDS.acContent;
    Node->Values[9] = FALSE;
    Node->Values[10] = "";

    SaveNewSDS(stSDS.acFrom, szUserName, stSDS.acContent);
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::UpdateSDSSendStatus(EVENT_SDS_SEND_STATUS_T &stSDS)
{
    UINT32      ulSDSID = 0;
    CHAR        acReceiver[32];
    CHAR        acMsgID[32];
    BOOL        bSendFlag;          // true
    UINT32      ulRetCode;
    BOOL        bSendSucc = FALSE;

    if (!stSDS.bSendFlag)
    {
        return;
    }

    if (stSDS.ulRetCode == SDS_SEND_NO_ACK )
    {
        return;
    }

    if (stSDS.ulRetCode == SDS_SEND_SUCC)
    {
        bSendSucc = TRUE;
    }

    if (!gos_atou(stSDS.acMsgID, &ulSDSID))
    {
        return;
    }

    SaveUESDSRecv(stSDS.acReceiver, ulSDSID, bSendSucc);
    TimerCheck->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::ColumnNeedReplyGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (!ANode || ANode->Values[9].IsNull())
    {
        return;
    }

    BOOL bNeedReply = ANode->Values[9];

    if (bNeedReply)
    {
        Value = "是";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::btnReplyClick(TObject *Sender)
{
    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    UINT32              ulUserID = INVALID_USER_ID;
    TcxTreeListNode     *Node = tlMain->Selections[0];

    AnsiString  strUserID = Node->Values[5];

    ToInt(strUserID, &ulUserID);
    NewSDS(ulUserID);
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::ColumnSDSTypeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (!ANode || ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulValue = (UINT32)ANode->Values[2];

    Value = GetSDSTypeText(ulValue);
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::btnResendClick(TObject *Sender)
{
    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlMain->Selections[0];

    SDS_INFO_T  stInfo = {0};

    stInfo.ulSDSID = Node->Values[0];
    if (stInfo.ulSDSID != g_ulCurrSDSID)
    {
        return;
    }

    if (!Node->Values[4].IsNull())
    {
        stInfo.ulSDSIndex = (UINT32)Node->Values[4];
    }

    stInfo.ulSendTime = gos_get_current_time();
    StrCpy(stInfo.acSendUserName, APP::GetDCLoginName());
    stInfo.ulSDSType = (UINT32)Node->Values[2];

    String  strSDSInfo = Node->Values[8];

    StrCpy(stInfo.acSDSInfo, strSDSInfo);

    if (!SendSDSInfo(stInfo, g_vRecvSDSFailedUser))
    {
        WarnMsg("操作失败!");
    }
    else
    {
        InfoMsg("发送成功!");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::ColumnRecvTimeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[1].IsNull())
    {
        return;
    }

    UINT32      ulTime = ANode->Values[1];

    if (ulTime == 0)
    {
        Value = "";
    }
    else
    {
        Value = GetTimeText(ulTime);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::ColumnConfirmTimeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (!ANode || ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32      ulTime = ANode->Values[2];

    if (ulTime == 0)
    {
        Value = "";
    }
    else
    {
        Value = GetTimeText(ulTime);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::tlSDSReplyGetCellHint(TcxCustomTreeList *Sender, TObject *ACell,
          UnicodeString &AText, bool &ANeedShow)
{
    TcxTreeListEditCellViewInfo *CellInfo = (TcxTreeListEditCellViewInfo*)ACell;

    int iColIndex = CellInfo->Column->ItemIndex;

    ANeedShow = false;

    if (iColIndex == 1 ||
        iColIndex == 2 )
    {
        UINT32  ulTime = CellInfo->Node->Values[iColIndex];

        if (ulTime > 0)
        {
            AText = GetTextTime(ulTime);
            ANeedShow = true;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::btnAddSmsClick(TObject *Sender)
{
    NewSMS(INVALID_USER_ID);
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::NewSMS(UINT32 ulUserID)
{
    VECTOR<UINT32> vUserID;

    vUserID.push_back(ulUserID);

    NewSMS(vUserID);
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::NewSMS(VECTOR<UINT32> &vUserID)
{
    VECTOR<INT64>   vGroupID;

    NewSMS(vUserID, vGroupID);
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::NewSMS(VECTOR<INT64> &vGroupID)
{
    VECTOR<UINT32>  vUserID;

    NewSMS(vUserID, vGroupID);
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::NewSMS(VECTOR<UINT32> &vUserID, VECTOR<INT64> &vGroupID)
{
    DialogNewSMS->Init(vUserID, vGroupID);
    if (DialogNewSMS->ShowModal() != mrOk)
    {
        return;
    }

    AnsiString      strSMSFile;
    AnsiString      strSMSTitle;
    VECTOR<UINT32>  vUser;
    VECTOR<INT64>   vGroup;
    UINT32          ulSDSID;

    GJsonParam          ReqJsonParam;
    GJson               RspJson;
    GJsonTupleParam     JsonTupleParam;
    BOOL                bRet = FALSE;
    VECTOR<UINT32>      vMemberUser;
    VECTOR<INT64>       vMemberGroup;
    IntSet              sUser;

    DialogNewSMS->Get(strSMSTitle, strSMSFile, vUser, vGroup);

    if (vUser.size() == 0 && vGroup.size() == 0)
    {
        InfoMsg("没有有效的接收者");
        return;
    }

    for (UINT32 i=0; i<vUser.size(); i++)
    {
        sUser.Add(vUser[i]);
    }

    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        eSDK_GetGroupMember(vGroup[i], vMemberUser, vMemberGroup);
        sUser.Add(vMemberUser);
    }

    sUser.Del(APP::GetCurrDCUserID());

    if (sUser.Size() == 0)
    {
        InfoMsg("没有有效的接收者");
        return;
    }

    for (UINT32 i=0; i<sUser.Size(); i++)
    {
        GJsonParam  SubJsonParam;

        SubJsonParam.Add("UserID", sUser[i]);
        JsonTupleParam.Add(SubJsonParam);
    }

    ReqJsonParam.Add("SendUserID", APP::GetCurrDCUserID());
    ReqJsonParam.Add("SendUserName", APP::GetDCLoginName());
    ReqJsonParam.Add("SDSType", SDS_TYPE_SMS);
    ReqJsonParam.Add("SDSInfo", EncodeSMSMsg(strSMSTitle));
    ReqJsonParam.Add("File", strSMSFile);
    ReqJsonParam.Add("NeedReply", FALSE);
    ReqJsonParam.Add("User", JsonTupleParam);

    if (!SyncSendServer(EV_DC_DIS_REQ, MSG_DCAddSDSInfo, ReqJsonParam, RspJson))
    {
        GosLog(LOG_ERROR, "sync send msg MSG_DCAddSDSInfo to server failed");
        WarnMsg("操作失败");
        goto end;
    }

    if (!RspJson.GetValue("SDSID", &ulSDSID) )
    {
        GosLog(LOG_ERROR, "parse msg %s failed", RspJson.GetText());
        WarnMsg("操作失败");
        goto end;
    }

    //if (!SendSDSInfo(stInfo, sUser))
    if(!eSDK_SendMsg(strSMSTitle, strSMSFile, ulSDSID, vUser, vGroup))
    {
        GosLog(LOG_ERROR, "send msg %s failed", strSMSTitle.c_str());
        WarnMsg("操作失败!");
        goto end;
    }

    bRet = TRUE;

end:
    ReqJsonParam.Clear();

    ReqJsonParam.Add("SDSID", ulSDSID);

    const CHAR  *szMsgName = bRet?MSG_DCSetSDSInfo:MSG_DCDelSDSInfo;

    SendServer(EV_DC_DIS_REQ, szMsgName, ReqJsonParam);
    if (bRet)
    {
        RefreshSDSInfo();
        InfoMsg("操作成功!");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::btnOpenAttachFileClick(TObject *Sender)
{
    if (tlMain->SelectionCount == 0)
    {
        InfoMsg("请选择一条彩信");
        return;
    }

    //
    CHAR        acValue[1024];
    AnsiString  strFile;
    TcxTreeListNode     *Node = tlMain->Selections[0];

    if (Node->Values[10].IsNull())
    {
        InfoMsg("请选择一条彩信");
        return;
    }

    if ((UINT32)Node->Values[2] != SDS_TYPE_SMS)
    {
        InfoMsg("请选择彩信记录");
        return;
    }

    strFile = Node->Values[10];
    if (!FileExists(strFile))
    {
        return;
    }

    sprintf(acValue, "/n,/select,%s", strFile.c_str());
    ShellExecute(NULL, "open", "Explorer.exe", acValue, NULL, SW_SHOWDEFAULT);

    return ;
}
//---------------------------------------------------------------------------
void __fastcall TFormLatestSDS::tlMainClick(TObject *Sender)
{
    ShowDetail();
}
//---------------------------------------------------------------------------

void __fastcall TFormLatestSDS::ColumnAttachFileGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (!ANode || ANode->Values[10].IsNull())
    {
        return;
    }

    AnsiString  strValue = ANode->Values[10];

    if (strValue.IsEmpty())
    {
        return;
    }

    if (gos_str_like(strValue.c_str(), m_acSMSPath))
    {
        Value = strValue.c_str() + strlen(m_acSMSPath) + 1;
    }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "omc.h"
#include "MsgDialog.h"
#include "msg.h"
#include "ds_public.h"
#include "ClientUtil.h"
#include "Resource.h"
#include "ClientCommon.h"
#include "RemoteMsg.h"
#include "ClientInit.h"

#include "FrmCfg.h"
#include "FrmPerf.h"
#include "FrmNeInfo.h"
#include "FrmOperLog.h"
#include "DlgSetNe.h"
#include "FrmAlarm.h"
#include "FrmHistoryAlarm.h"
#include "FrmDevFile.h"
#include "FrmPerf.h"
#include "FrmActiveAlarm.h"
#include "DlgImportFile.h"
#include "GTransFile.h"

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
#pragma link "cxMaskEdit"
#pragma link "cxClasses"
#pragma link "AdvSmoothButton"
#pragma resource "*.dfm"
TFormCfg *FormCfg;
extern UINT32   g_usTransFilePort;
std::map<AnsiString, TcxTreeListNode*>      g_mDevMap;

ID_VALUE_T   astDevType[] = {
								{OMC_DEV_TYPE_ALL,  "全部"},
								{OMC_DEV_TYPE_TX,   "车载台"},
                                {OMC_DEV_TYPE_FX,   "固定台"},
								{OMC_DEV_TYPE_DC,   "调度台"},
                                {OMC_DEV_TYPE_DIS,  "调度服务器"},
								{OMC_DEV_TYPE_REC,  "录音录像服务器"},
							};

Dict    g_DictDevType(astDevType, ARRAY_SIZE(astDevType));

AnsiString GetDevTypeText(UINT32 ulDevType)
{
    for (UINT32 i=0; i<ARRAY_SIZE(astDevType); i++)
    {
		if (astDevType[i].ulID == ulDevType)
        {
            return astDevType[i].szValue;
        }
    }

    return "";
}


bool IsDevListSame(VectorMap<AnsiString, NE_BASIC_INFO_T> &mInfo)
{
//  if (mInfo.Size() != m_mNEBasicInfo.Size())
//  {
//      return false;
//  }
//
//  for (UINT32 i=0; i<mInfo.Size(); i++)
//    {
//        NE_BASIC_INFO_T     *pstNewInfo = mInfo[i];
//      NE_BASIC_INFO_T     *pstInfo = m_mNEBasicInfo.GetValue(pstNewInfo->acNeID);
//
//        if (!pstInfo)
//        {
//            return false;
//      }
//
//        if (strcmp(pstInfo->acNeID, pstNewInfo->acNeID) != 0 ||
//            strcmp(pstInfo->acDevName, pstNewInfo->acDevName) != 0 )
//      {
//          return false;
//      }
//    }
//
//  return true;
}

AnsiString GetOnlineSec(UINT32 ulTime)
{
    UINT32      ulDay;
    UINT32      ulHour;
    UINT32      ulMin;
    UINT32      ulSec;

    ulDay = ulTime/86400;
    ulTime -= ulDay*86400;

    ulHour = ulTime/3600;
    ulTime -= ulHour*3600;

    ulMin = ulTime/60;
    ulTime -= ulMin*60;

    ulSec = ulTime;

    CHAR    acText[64];

    if (ulDay > 0)
    {
        sprintf(acText, "%u天%u小时%u分%u秒", ulDay, ulHour, ulMin, ulSec);
    }
    else if (ulHour > 0)
    {
        sprintf(acText, "%u小时%u分%u秒", ulHour, ulMin, ulSec);
    }
    else //if (ulMin > 0)
    {
        sprintf(acText, "%u分%u秒", ulMin, ulSec);
    }

    return acText;
}
//---------------------------------------------------------------------------
__fastcall TFormCfg::TFormCfg(TComponent* Owner)
    : TSubForm(Owner)
{
#ifdef HAVE_TAU
	g_DictDevType.Add(OMC_DEV_TYPE_TAU, "TAU");
#endif

	g_DictDevType.Init(cbDevType);
    g_DictDevType.SetID(cbDevType, OMC_DEV_TYPE_ALL);

    InitMsgHandler(EV_OMT_OMC_RSP, MSG_OMT_GetNeBasicInfo, OnGetNeBasicInfoRsp);
}
//---------------------------------------------------------------------------

bool  CheckDevInfo(AnsiString &strDevName,AnsiString &strMAC)
{
   for (UINT32 i=0; i<g_mNEBasicInfo.Size(); i++)
    {
        NE_BASIC_INFO_T     *pstInfo = g_mNEBasicInfo.GetValueByIndex(i);

        if (strDevName == pstInfo->acDevName)
        {
           WarnMsg("设备名称已存在，请重新输入设备信息");
           return false;
        }

        if (strMAC == pstInfo->acDevMac)
        {
            WarnMsg("MAC地址已存在,请重新输入设备信息");
            return false;
        }
    }

    return true;
}

//---------------------------------------------------------------------------
void __fastcall TFormCfg::btnAddDevClick(TObject *Sender)
{
    DialogSetNe->Init();
    if (DialogSetNe->ShowModal() != mrOk)
    {
        return;
    }

    AnsiString      strMAC;
    AnsiString      strDevName;
    UINT32          ulDevType;
    UINT32          ulDevArea;
    DialogSetNe->Get(ulDevType, strDevName, strMAC);

    if(!CheckDevInfo(strDevName,strMAC))
    {
      return;
    }

    GJsonParam  ReqJsonParam;

    ReqJsonParam.Add("MAC", strMAC);
    ReqJsonParam.Add("DevType", ulDevType);
    ReqJsonParam.Add("DevName", strDevName);

    if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_AddNe, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::btnDelDevClick(TObject *Sender)
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

    AnsiString  strNeID = Node->Texts[0];
    AnsiString  strDevName = Node->Texts[1];

    if (!AskMsg((CHAR*)"删除设备%s吗?", strDevName.c_str()))
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("NeID", strNeID);

    if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_DelNe, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::btnSetDevClick(TObject *Sender)
{
    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlMain->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    TRAIN_INFO_T    stInfo;

    AnsiString strNeID = Node->Values[0];
    AnsiString strDevName = Node->Values[1];
    UINT32     ulDevType = Node->Values[2];
    AnsiString strMAC = Node->Values[5];
	DialogSetNe->Init(ulDevType, strDevName, strMAC);
    if (DialogSetNe->ShowModal() != mrOk)
    {
        return;
    }

    AnsiString strNewDevName;
    UINT32     ulNewDevType;
    AnsiString strNewMAC;

    DialogSetNe->Get(ulNewDevType, strNewDevName, strNewMAC);

    if (strNewDevName == strDevName)
    {
        return;
    }

    for (UINT32 i =0; i < m_mNEBasicInfo.Size(); i++)
    {
        if (strNewDevName == m_mNEBasicInfo[i]->acDevName)
        {
           WarnMsg("设备名称重复");
           return;
        }
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("NeID", strNeID);
    ReqJsonParam.Add("DevName", strNewDevName);
    ReqJsonParam.Add("DevType", ulNewDevType);
    ReqJsonParam.Add("MAC", strNewMAC);

    if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_SetNe, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::btnRefreshClick(TObject *Sender)
{
    static UINT32 ulLastTime = 0;
    UINT32  ulCurrTime = gos_get_uptime_1ms();

    if (ulLastTime == 0 ||(ulLastTime+1000) < ulCurrTime)
    {
        SendServer(EV_OMT_OMC_REQ, MSG_OMT_GetNeBasicInfo);
        ulLastTime = gos_get_uptime_1ms();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::InitNode(TcxTreeListNode *Node, NE_BASIC_INFO_T *pstInfo)
{
    UINT32      ulOnlineTime = gos_get_current_time() - pstInfo->iOnlineSec;

    Node->Values[0]  = pstInfo->acNeID;
    Node->Values[1]  = pstInfo->acDevName;
    Node->Values[2]  = pstInfo->ulDevType;
    Node->Values[3]  = pstInfo->acModelName;
    Node->Values[4]  = pstInfo->acDevIp;
    Node->Values[5]  = pstInfo->acDevMac;
    Node->Values[6]  = pstInfo->iOnlineSec<0?AnsiString(""):GetTextTime(ulOnlineTime);
    Node->Values[7]  = pstInfo->iOnlineSec<0?0:pstInfo->iOnlineSec;
    Node->Values[8]  = pstInfo->iOnlineSec<0?0:1;
    Node->Values[9]  = pstInfo->acHardwareVer;
    Node->Values[10] = pstInfo->acSoftwareVer;
    Node->Values[11] = pstInfo->acAndroidVer;
    Node->Values[12] = pstInfo->acRadioVer;
}
//---------------------------------------------------------------------------
bool HaveStr(AnsiString str, AnsiString strFilter)
{
    return str.UpperCase().Pos(strFilter) > 0;
}
//---------------------------------------------------------------------------

void __fastcall  TFormCfg::InitView()
{
    TcxTreeListNode     *Node;
    bool                bShowOffline = cbShowOfflineDev->Checked;
    AnsiString          strFilter = edtSearch->Text.Trim().UpperCase();
    UINT32              ulDevType;
    NEBasic::GetNEBasicInfo(m_mNEBasicInfo);

    if (!g_DictDevType.GetID(cbDevType, &ulDevType))
    {
        ulDevType = OMC_DEV_TYPE_ALL;
    }

    tlMain->BeginUpdate();
    tlMain->Clear();

    for (UINT32 i=0; i<m_mNEBasicInfo.Size(); i++)
    {
        NE_BASIC_INFO_T     *pstInfo = m_mNEBasicInfo.GetValueByIndex(i);

        if (!bShowOffline &&
            pstInfo->iOnlineSec < 0)
        {
            continue;
        }

        if (ulDevType != OMC_DEV_TYPE_ALL &&
            pstInfo->ulDevType != ulDevType)
        {
            continue;
        }

        if (!strFilter.IsEmpty() &&
            !HaveStr(pstInfo->acDevName, strFilter) &&
            !HaveStr(pstInfo->acDevMac, strFilter) )
        {
            continue;
        }

        Node = tlMain->Add();
        InitNode(Node, pstInfo);
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::btnImportDevClick(TObject *Sender)
{
    if (!OpenDialog->Execute())
    {
        return;
    }

    AnsiString  strFile = OpenDialog->FileName;

    DialogImportFile->ImportFromFile(strFile.c_str());
    if (DialogImportFile->ShowModal()!= mrOk)
    {
        InfoMsg("导入文件失败");
        return;
    }

    DialogImportFile->ImportFromFile(strFile.c_str());
    btnRefreshClick(this);

//  if (!ImportFromFile(tlMain, strFile.c_str(), true))
//  {
//      InfoMsg("加载文件失败");
//      return;
//  }

}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::FormCreate(TObject *Sender)
{
    SetImage(btnAddDev, IMG_ADD);
    SetImage(btnDelDev, IMG_DEL);
    SetImage(btnSetDev, IMG_SET);
    SetImage(btnImportDev, IMG_IMPORT);
    SetImage(btnExportDev, IMG_EXPORT);

    SetImage(pmiAddDev, IMG_ADD);
    SetImage(pmiDelDev, IMG_DEL);
    SetImage(pmiSetDev, IMG_SET);
    SetImage(pmiImportDev, IMG_IMPORT);
    SetImage(pmiRefresh, IMG_REFRESH);


    SetImage(btnQuery, IMG_QUERY);
    SetImage(btnRefresh, IMG_REFRESH);

}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::ColumnDevTypeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulDevType = ANode->Values[2];

    Value = GetDevTypeText(ulDevType);
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::ColumnOnlineTimeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[7].IsNull())
    {
        return;
    }

    INT32  iSec = ANode->Values[7];

    if (iSec <= 0)
    {
        Value = "";
    }
    else
    {
        Value = GetOnlineSec(iSec);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::btnDevInfoClick(TObject *Sender)
{
    if (tlMain->SelectionCount <= 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlMain->Selections[0];
    AnsiString          strNeID = Node->Values[0];
    NE_BASIC_INFO_T     *pstInfo = m_mNEBasicInfo.GetValue(strNeID);

    if (!pstInfo)
    {
        return;
    }

    if (pstInfo->ulDevType == OMC_DEV_TYPE_TX ||
        pstInfo->ulDevType == OMC_DEV_TYPE_FX ||
        pstInfo->ulDevType == OMC_DEV_TYPE_DC ||
        pstInfo->ulDevType == OMC_DEV_TYPE_DIS||
        pstInfo->ulDevType == OMC_DEV_TYPE_REC ||
        pstInfo->ulDevType == OMC_DEV_TYPE_TAU )
    {
        FormNeInfo->Init(strNeID);
        FormNeInfo->Parent = this;
        FormNeInfo->Visible = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::pmiOperLogClick(TObject *Sender)
{
    if (tlMain->SelectionCount <= 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlMain->Selections[0];

    if (Node->Values[0].IsEmpty())
    {
        return;
    }

    AnsiString      strNeID = Node->Values[0];
    FormOperLog->Init(strNeID);
    FormOperLog->Parent = this;
    FormOperLog->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::OnGetNeBasicInfoRsp(UINT32 ulSeqID, GJson &Json)
{
	CHAR    acFile[256] = {0};
	CHAR    acLocalFile[256] = {0};
    NE_BASIC_INFO_T stInfo = {0};
    VectorMap<AnsiString, NE_BASIC_INFO_T>  mNEBasicInfo;
    std::map<UINT32, UINT32> mDevOnlineCount;

	if (!Json.GetValue("FileName", acFile, sizeof(acFile)))
    {
        GosLog(LOG_ERROR, "invalid FileName in msg format: %s", Json.GetText());
        return;
    }

	sprintf(acLocalFile, "%s/%s", gos_get_root_path(), acFile);

	if (!TransFile(g_stLocalCfg.aucOMCAddr, g_usTransFilePort, acFile, acLocalFile))
	{
		GosLog(LOG_ERROR, "DownLoad file %s fail", acLocalFile);
        return;
    }

    GJson           JsonResult;
    GJson           *pSubJson;

	if (!JsonResult.ParseFile(acLocalFile))
	{
		gos_delete_file(acLocalFile);
        GosLog(LOG_ERROR, "ParseFile fail: %s", acLocalFile);
        return;
	}
	gos_delete_file(acLocalFile);

    VECTOR<GJson*>  &vSubJson = JsonResult.GetSubJson();

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];

        if (!pSubJson->GetValue("ne_id", stInfo.acNeID, sizeof(stInfo.acNeID)) ||
            !pSubJson->GetValue("mac", stInfo.acDevMac, sizeof(stInfo.acDevMac)) ||
            !pSubJson->GetValue("ip", stInfo.acDevIp, sizeof(stInfo.acDevIp)) ||
            !pSubJson->GetValue("type", &stInfo.ulDevType) ||
            !pSubJson->GetValue("name", stInfo.acDevName, sizeof(stInfo.acDevName)) ||
            !pSubJson->GetValue("sw_ver", stInfo.acSoftwareVer, sizeof(stInfo.acSoftwareVer)) ||
            !pSubJson->GetValue("hw_ver", stInfo.acHardwareVer, sizeof(stInfo.acHardwareVer)) ||
            !pSubJson->GetValue("model", stInfo.acModelName, sizeof(stInfo.acModelName)) ||
            !pSubJson->GetValue("online_sec", &stInfo.iOnlineSec))
        {
            GosLog(LOG_ERROR, "invalid msg format: %s", Json.GetText());
            return;
        }

        if (stInfo.ulDevType == OMC_DEV_TYPE_TX ||
            stInfo.ulDevType == OMC_DEV_TYPE_FX  )
        {
             if (!pSubJson->GetValue("android_ver", stInfo.acAndroidVer,sizeof(stInfo.acAndroidVer)) ||
                 !pSubJson->GetValue("radio_ver", stInfo.acRadioVer, sizeof(stInfo.acRadioVer)))
             {
                GosLog(LOG_ERROR, "invalid msg format: %s", Json.GetText());
                return;
             }
        }

        mNEBasicInfo.Add(stInfo.acNeID, stInfo);

        // 不同种类设备在线数量统计
        if (stInfo.iOnlineSec > 0)
        {
//            AnsiString  strNeID  =  stInfo.acNeID;
            ++mDevOnlineCount[stInfo.ulDevType];
        }

    }

    NEBasic::InitNEBasicInfo(mNEBasicInfo);
    NEBasic::InitDevOnlineCount(mDevOnlineCount);
    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::cbShowOfflineDevClick(TObject *Sender)
{
    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::btnQueryClick(TObject *Sender)
{
    InitView();
}
//---------------------------------------------------------------------------

void __fastcall TFormCfg::edtSearchKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
   if (Key == 13) // enter
    {
        InitView();
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormCfg::cbDevTypePropertiesChange(TObject *Sender)
{
    InitView();
}
//---------------------------------------------------------------------------

void __fastcall TFormCfg::TimerCheckTimer(TObject *Sender)
{
    SendServer(EV_OMT_OMC_REQ, MSG_OMT_GetNeBasicInfo);
}
//---------------------------------------------------------------------------

void __fastcall TFormCfg::pmiRebootClick(TObject *Sender)
{
    if (tlMain->SelectionCount <= 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlMain->Selections[0];

    AnsiString  strNeID = Node->Values[0];
    GJsonParam  ReqJsonParam;

    ReqJsonParam.Add("NeID", strNeID);

    if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_RebootNe, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }
    else
    {
        InfoMsg("操作成功");
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormCfg::tlMainDblClick(TObject *Sender)
{
    btnDevInfoClick(this);
}
//---------------------------------------------------------------------------

void __fastcall TFormCfg::edtSearchPropertiesChange(TObject *Sender)
{
    InitView();
}
//---------------------------------------------------------------------------

void __fastcall TFormCfg::pmMainPopup(TObject *Sender)
{
    if (tlMain->SelectionCount <= 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlMain->Selections[0];
    AnsiString          strNeID = Node->Values[0];
    NE_BASIC_INFO_T     *pstInfo = m_mNEBasicInfo.GetValue(strNeID);

    if (!pstInfo)
    {
        return;
    }

    if (pstInfo->ulDevType == OMC_DEV_TYPE_TX ||
        pstInfo->ulDevType == OMC_DEV_TYPE_FX )
    {
		pmiReboot->Enabled = true;
		pmiDevOperLog->Enabled = true;
		pmiDevDetail->Enabled = true;
    }
    else
    {
		pmiReboot->Enabled = false;
		pmiDevOperLog->Enabled = false;
        pmiDevDetail->Enabled = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormCfg::pmiDevDetailClick(TObject *Sender)
{
	BOOL    bShow = pmiDevDetail->Down;

	ColumnAndroidVer->Visible   = bShow;
	ColumnRadioVer->Visible     = bShow;
	ColumnModel->Visible        = bShow;
}
//---------------------------------------------------------------------------

void __fastcall TFormCfg::btnExportDevClick(TObject *Sender)
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


void __fastcall TFormCfg::ColumnStatusGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    if (ANode->Values[8].IsNull())
    {
        return;
    }

    BOOL    bOnline = ANode->Values[8];

    Value = bOnline?"在线":"离线";
}
//---------------------------------------------------------------------------

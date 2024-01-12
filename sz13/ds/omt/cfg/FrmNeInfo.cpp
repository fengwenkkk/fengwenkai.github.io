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

#include "FrmNeInfo.h"
#include "DlgSetTrainUE.h"
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
#pragma link "cxClasses"
#pragma resource "*.dfm"
TFormNeInfo *FormNeInfo;

extern AnsiString GetDevTypeText(UINT32 ulDevType);
extern AnsiString GetOnlineSec(UINT32 ulTime);

VOID AddNameValue(VECTOR<NAME_VALUE_T> &v, AnsiString strName, AnsiString strValue)
{
    NAME_VALUE_T    st;

    st.strName = strName;
    st.strValue = strValue;
    v.push_back(st);
}

BOOL GetDevInfo(AnsiString strNeID, UINT32 &ulDevType, VECTOR<NAME_VALUE_T> &vInfo, VECTOR<NAME_VALUE_T> &vStatus)
{
    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("NeID", strNeID);  //根据 DevName 获取设备信息

    if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_GetNeInfo, ReqJsonParam, RspJson))
    {
        return FALSE;
    }

    GJson   SubJson;
    UINT32  i;
    BOOL    bValid = FALSE;
    char    *szGroup = RspJson.GetValue("Info");

    if (!szGroup)
    {
        return FALSE;
    }

    SubJson.Parse(szGroup);
    std::vector<JSON_KEY_VALUE_T>   vList;

    SubJson.GetKeyValueList(vList);
    for (i=0; i<vList.size(); i++)
    {

        if (strcmp(vList[i].szKey, "设备类型") == 0)
        {
            if (!gos_atou(vList[i].szValue, &ulDevType))
            {
                return FALSE;
            }

            bValid = TRUE;

            AddNameValue(vInfo, vList[i].szKey, GetDevTypeText(ulDevType));
        }
        else
        {
            AddNameValue(vInfo, vList[i].szKey, vList[i].szValue);
        }
    }

    if (!bValid)
    {
        return FALSE;
    }

    szGroup = RspJson.GetValue("Status");
    if (!szGroup)
    {
        return FALSE;
    }

    SubJson.Parse(szGroup);
    SubJson.GetKeyValueList(vList);

    for (i=0; i<vList.size(); i++)
    {
        AddNameValue(vStatus, vList[i].szKey, vList[i].szValue);
    }

    return TRUE;
}

VOID ConvertTxRunStatus(UINT32 ulStatus, OMA_TX_RUN_STATUS_T &stInfo)
{
    stInfo.ulIsOnline             =  ulStatus & 1;
    stInfo.ulGatewayReachable     = (ulStatus >> 1) & 1;
    stInfo.ulMDCRegStatus         = (ulStatus >> 2) & 1;
    stInfo.ulOMCLinkStatus        = (ulStatus >> 3) & 1;
    stInfo.ulDISLinkStatus        = (ulStatus >> 4) & 1;
    stInfo.ulSelfCheckStatus      = (ulStatus >> 5) & 1;
    stInfo.ulDevCoverStatus       = (ulStatus >> 6) & 1;
    stInfo.ulDriveMode            = (ulStatus >> 7) & 1;
    stInfo.ulEmergencyCallStatus  = (ulStatus >> 8) & 1;
    stInfo.ulEmergencyBraceStatus = (ulStatus >> 9) & 1;
    stInfo.ulIPHLinkStatus        = (ulStatus >> 10)& 1;
}

//---------------------------------------------------------------------------
__fastcall TFormNeInfo::TFormNeInfo(TComponent* Owner)
    : TSubForm(Owner)
{
    //InitView();

}
//---------------------------------------------------------------------------
void __fastcall TFormNeInfo::btnRefreshClick(TObject *Sender)
{
    VECTOR<NAME_VALUE_T>    vInfo;
    VECTOR<NAME_VALUE_T>    vStatus;
    UINT32      ulDevType;
//  OMA_DEV_INFO_T  stDevInfo;

    if (!GetDevInfo(m_strNeID, ulDevType, vInfo, vStatus))
    {
        return;
    }

    tlInfo->Columns[0]->Width = 150;
    tlInfo->Columns[1]->Width = 280;
    tlInfo->Columns[2]->Width = 150;
    tlInfo->Columns[3]->Width = 280;

    tlStatus->Columns[0]->Width = 150;
    tlStatus->Columns[1]->Width = 280;
    tlStatus->Columns[2]->Width = 150;
    tlStatus->Columns[3]->Width = 280;

/*
    if (ulDevType == OMC_DEV_TYPE_TX)
    {
        OMA_TX_RUN_STATUS_T     stRunStatus;
        CHAR                    acGateway[32];

        sprintf(acGateway, IP_FMT, IP_ARG(stDevInfo.stNeInfo.aucGateway));

        NTOHL_IT(stDevInfo.unDevStatus.stTxStatus.ulRunStatus);
        ConvertTxRunStatus(stDevInfo.unDevStatus.stTxStatus.ulRunStatus, stRunStatus);

        AddNameValue(g_vInfo, "设备名称",  stDevInfo.stNeBasicInfo.acDevName);
        AddNameValue(g_vInfo, "设备类型",  GetDevTypeText(ntohl(stDevInfo.stNeBasicInfo.ulDevType)));
        AddNameValue(g_vInfo, "MAC地址",  stDevInfo.stNeBasicInfo.acDevMac);
        AddNameValue(g_vInfo, "IP地址",  stDevInfo.stNeBasicInfo.acDevIp);
        AddNameValue(g_vInfo, "网关",  acGateway);
        AddNameValue(g_vInfo, "在线状态",  ntohl(stDevInfo.stNeBasicInfo.iOnlineSec)>=0?"在线":"离线");
        AddNameValue(g_vInfo, "软件版本",  stDevInfo.stNeBasicInfo.acSoftwareVer);
        AddNameValue(g_vInfo, "硬件版本",  stDevInfo.stNeBasicInfo.acHardwareVer);
        AddNameValue(g_vInfo, "运行时长",  GetOnlineSec(ntohl(stDevInfo.stNeInfo.ulUptime)));

        AddNameValue(g_vStatus, "CPU使用率(%)",  ntohl(stDevInfo.stNeInfo.ulCpuUsage));
        AddNameValue(g_vStatus, "内存使用率(%)",  ntohl(stDevInfo.stNeInfo.ulMemUsage));
        AddNameValue(g_vStatus, "硬盘使用率(%)",  ntohl(stDevInfo.stNeInfo.ulDiskUsage));

        AddNameValue(g_vStatus, "网关连接状态",  stRunStatus.ulGatewayReachable==0?"不通"   :"正常");
        AddNameValue(g_vStatus, "注册到集群服务器",  stRunStatus.ulMDCRegStatus==0?"否" :"是");
        AddNameValue(g_vStatus, "网管服务器连接状态",  stRunStatus.ulOMCLinkStatus==0?"不通"    :"正常");
        AddNameValue(g_vStatus, "CAD服务器连接状态",  stRunStatus.ulDISLinkStatus==0?"不通" :"正常");
        AddNameValue(g_vStatus, "自检状态", stRunStatus.ulSelfCheckStatus==0?"未通过":"正常");
        AddNameValue(g_vStatus, "设备盖状态",  stRunStatus.ulDevCoverStatus==0?"关闭"   :"打开");
        AddNameValue(g_vStatus, "驾驶模式",  stRunStatus.ulDriveMode==0?"无人驾驶":"有人驾驶");
        AddNameValue(g_vStatus, "紧急呼叫状态",  stRunStatus.ulEmergencyCallStatus==0?"否"  :"是");
        AddNameValue(g_vStatus, "紧急停车状态",  stRunStatus.ulEmergencyBraceStatus==0?"否" :"是");
        AddNameValue(g_vStatus, "IPH连接状态",  stRunStatus.ulIPHLinkStatus==0?"不通"   :"正常");

        AddNameValue(g_vStatus, "当前小区",  stDevInfo.unDevStatus.stTxStatus.stLteStatus.ulPCI);
        AddNameValue(g_vStatus, "RSRP",  stDevInfo.unDevStatus.stTxStatus.stLteStatus.iRSRP);
        AddNameValue(g_vStatus, "RSRQ",  stDevInfo.unDevStatus.stTxStatus.stLteStatus.iRSRQ);
        AddNameValue(g_vStatus, "RSSI",  stDevInfo.unDevStatus.stTxStatus.stLteStatus.iRSSI);
        AddNameValue(g_vStatus, "SINR",  stDevInfo.unDevStatus.stTxStatus.stLteStatus.iSINR);
    }  */

    ShowNeInfo(vInfo, vStatus);
}
//---------------------------------------------------------------------------
void __fastcall TFormNeInfo::Init(AnsiString strNeID)
{
    m_strNeID = strNeID;

    tlInfo->OptionsView->Headers = false;
    tlStatus->OptionsView->Headers = false;

    Caption = "设备详情";
    tlInfo->Clear();
    tlStatus->Clear();

    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormNeInfo::ShowNeInfo(VECTOR<NAME_VALUE_T> &vInfo, VECTOR<NAME_VALUE_T> &vStatus)
{
    TcxTreeListNode     *Node;
    UINT32              j;

    tlInfo->BeginUpdate();
    tlInfo->Clear();

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        if (MOD(i,2) == 0)
        {
            Node = tlInfo->Add();
            j = 0;
        }

        Node->Values[j++] = vInfo[i].strName;
        Node->Values[j++] = vInfo[i].strValue;
    }

    tlInfo->EndUpdate();

    tlStatus->BeginUpdate();
    tlStatus->Clear();

    for (UINT32 i=0; i<vStatus.size(); i++)
    {
        if (MOD(i,2) == 0)
        {
            Node = tlStatus->Add();
            j = 0;
        }

        Node->Values[j++] = vStatus[i].strName;
        Node->Values[j++] = vStatus[i].strValue;
    }

    tlStatus->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormNeInfo::FormCreate(TObject *Sender)
{
    SetImage(btnRefresh, IMG_REFRESH);
    SetImage(btnClose, IMG_CLOSE);
}
//---------------------------------------------------------------------------
void __fastcall TFormNeInfo::btnCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormNeInfo::FormClose(TObject *Sender, TCloseAction &Action)
{
    Action = caHide;
}


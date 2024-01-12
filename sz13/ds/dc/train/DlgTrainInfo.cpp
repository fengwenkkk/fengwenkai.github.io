//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "pis.h"
#include "DlgTrainInfo.h"

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
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "cxTextEdit"
#pragma link "cxCheckBox"
#pragma link "cxDropDownEdit"
#pragma link "cxMaskEdit"
#pragma link "cxButtonEdit"
#pragma link "cxCustomData"
#pragma link "cxInplaceContainer"
#pragma link "cxStyles"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "cxMemo"
#pragma link "cxImageList"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxGroupBox"
#pragma resource "*.dfm"
TDialogTrainInfo *DialogTrainInfo;

AnsiString GetIPHStatusText(BOOL bIsIPHOnline, UINT32 ulIPHStatus)
{
    if (!bIsIPHOnline)   return "离线";

    if (ulIPHStatus == IPH_STATUS_NORMAL  )   return "正常";
    if (ulIPHStatus == IPH_STATUS_ABNORMAL)   return "异常";

    return "未知";
}

AnsiString GetPAStatusText(BOOL bIsPAOnline, UINT32 ulPAStatus)
{
    if (!bIsPAOnline)   return "离线";

    if (ulPAStatus == PA_STATUS_NA  )   return "系统不可用";
    if (ulPAStatus == PA_STATUS_IDLE)   return "空闲";
    if (ulPAStatus == PA_STATUS_WORK)   return "广播中";

    return "未知";
}

AnsiString GetTCText(UINT32 ulTC)
{
    if (ulTC == 0)   return "";

    return AnsiString("TC") + ulTC;
}

//---------------------------------------------------------------------------
__fastcall TDialogTrainInfo::TDialogTrainInfo(TComponent* Owner)
    : TDialogForm(Owner)
{
}

void __fastcall TDialogTrainInfo::InitSkin()
{
    SetSkin(tlMain);
}
//---------------------------------------------------------------------------
void __fastcall TDialogTrainInfo::Init(TRAIN_INFO_T *pstTrain)
{
    tlMain->BeginUpdate();
    tlMain->Clear();

    TcxTreeListNode     *Node;

    Node = tlMain->Add();
    Node->Values[0] = "车次号";
    Node->Values[1] = pstTrain->astAppTrainInfo[0].acTrainID;
    Node->Values[2] = pstTrain->astAppTrainInfo[1].acTrainID;

    Node = tlMain->Add();
    Node->Values[0] = "车载台位置";
    Node->Values[1] = GetTCText(pstTrain->astAppTrainInfo[0].ulTC);
    Node->Values[2] = GetTCText(pstTrain->astAppTrainInfo[1].ulTC);

    Node = tlMain->Add();
    Node->Values[0] = "当前通话组";
    Node->Values[1] = GetIntText(pstTrain->astAppTrainInfo[0].i64CurrentGroupID);
    Node->Values[2] = GetIntText(pstTrain->astAppTrainInfo[1].i64CurrentGroupID);

    Node = tlMain->Add();
    Node->Values[0] = "当前车站";
    Node->Values[1] = Station::GetStationName(pstTrain->astAppTrainInfo[0].ulStationID);
    Node->Values[2] = Station::GetStationName(pstTrain->astAppTrainInfo[1].ulStationID);

    Node = tlMain->Add();
    Node->Values[0] = "小区";
    Node->Values[1] = GetIntText(pstTrain->astAppTrainInfo[0].ulCellID);
    Node->Values[2] = GetIntText(pstTrain->astAppTrainInfo[1].ulCellID);

    Node = tlMain->Add();
    Node->Values[0] = "呼叫类型";
    Node->Values[1] = GetCallTypeText(pstTrain->astAppTrainInfo[0].ulCallType, 0);
    Node->Values[2] = GetCallTypeText(pstTrain->astAppTrainInfo[1].ulCallType, 0);

    Node = tlMain->Add();
    Node->Values[0] = "呼叫号码";
    Node->Values[1] = GetIntText(pstTrain->astAppTrainInfo[0].i64CallID);
    Node->Values[2] = GetIntText(pstTrain->astAppTrainInfo[1].i64CallID);

    Node = tlMain->Add();
    Node->Values[0] = "当前时间";
    Node->Values[1] = GetTextTime(pstTrain->astAppTrainInfo[0].ulCurrentTime);
    Node->Values[2] = GetTextTime(pstTrain->astAppTrainInfo[1].ulCurrentTime);

    Node = tlMain->Add();
    Node->Values[0] = "运行时间（秒）";
    Node->Values[1] = pstTrain->astAppTrainInfo[0].ulUptime;
    Node->Values[2] = pstTrain->astAppTrainInfo[1].ulUptime;

    Node = tlMain->Add();
    Node->Values[0] = "终端版本";
    Node->Values[1] = pstTrain->astAppTrainInfo[0].acAPPVer;
    Node->Values[2] = pstTrain->astAppTrainInfo[1].acAPPVer;

    Node = tlMain->Add();
    Node->Values[0] = "IPH设备状态";
    Node->Values[1] = GetIPHStatusText(pstTrain->astAppTrainInfo[0].bIsIPHOnline, pstTrain->astAppTrainInfo[0].ulIPHStatus);
    Node->Values[2] = GetIPHStatusText(pstTrain->astAppTrainInfo[1].bIsIPHOnline, pstTrain->astAppTrainInfo[1].ulIPHStatus);

    Node = tlMain->Add();
    Node->Values[0] = "广播设备状态";
    Node->Values[1] = GetPAStatusText(pstTrain->astAppTrainInfo[0].bIsPAOnline, pstTrain->astAppTrainInfo[0].ulPAStatus);
    Node->Values[2] = GetPAStatusText(pstTrain->astAppTrainInfo[1].bIsPAOnline, pstTrain->astAppTrainInfo[1].ulPAStatus);

    Node = tlMain->Add();
    Node->Values[0] = "广播主备状态";
    Node->Values[1] = pstTrain->astAppTrainInfo[0].bIsMasterPA?"主用":"备用";
    Node->Values[2] = pstTrain->astAppTrainInfo[1].bIsMasterPA?"主用":"备用";

    tlMain->Root->Expand(true);
    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TDialogTrainInfo::btnOKClick(TObject *Sender)
{
   ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TDialogTrainInfo::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
}
//---------------------------------------------------------------------------


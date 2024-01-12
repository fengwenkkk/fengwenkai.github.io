//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmGroupCfg.h"
#include "main.h"
#include "FrmPredefinedSMSCfg.h"
#include "FrmSDSTemplate.h"
#include "AudioDev.h"
#include "AudioVolume.h"

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
#pragma link "cxScrollBox"
#pragma link "cxCheckBox"
#pragma link "cxRadioGroup"
#pragma resource "*.dfm"
TFormGroupCfg *FormGroupCfg;

void __fastcall TFormGroupCfg::Init()
{
    GROUP_CFG_T     *pstGroupCfg = GroupCfg::GetGroupCfg();

    edtUpLineTrainGroupID->Text      = pstGroupCfg->i64UpLineTrainGroupID;
    edtDownLineTrainGroupID->Text    = pstGroupCfg->i64DownLineTrainGroupID;
    edtMainLineTrainGroupID->Text    = pstGroupCfg->i64MainLineTrainGroupID;
    edtUpLineTrainBrdGroupID->Text   = pstGroupCfg->i64UpLineTrainBrdGroupID;
    edtDownLineTrainBrdGroupID->Text = pstGroupCfg->i64DownLineTrainBrdGroupID;
    edtMainLineTrainBrdGroupID->Text = pstGroupCfg->i64MainLineTrainBrdGroupID;
    edtAllStationGroupID->Text       = pstGroupCfg->i64AllStationGroupID;
    edtStartTempDynamicGroupID->Text = pstGroupCfg->i64StartTempDynamicGroupID;
    edtEndTempDynamicGroupID->Text   = pstGroupCfg->i64EndTempDynamicGroupID;
    edtDriverGroupID->Text           = pstGroupCfg->i64DriverGroupID;
    edtDepotDriverGroupID->Text      = pstGroupCfg->i64DepotDriverGroupID;
}
//---------------------------------------------------------------------------
__fastcall TFormGroupCfg::TFormGroupCfg(TComponent* Owner)
    : TSubForm(Owner)
{
    Init();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCfg::FormCreate(TObject *Sender)
{
    SetImage(btnSet,     IMG_SET);
    SetImage(btnCancel,  IMG_CANCEL);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCfg::btnCancelClick(TObject *Sender)
{
    Init();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCfg::btnSetClick(TObject *Sender)
{
    GROUP_CFG_T     stGroupCfg;
    GROUP_CFG_T     *pstGroupCfg = GroupCfg::GetGroupCfg();

    memcpy(&stGroupCfg, pstGroupCfg, sizeof(stGroupCfg));

    if (!ToInt64(edtUpLineTrainGroupID->Text, &stGroupCfg.i64UpLineTrainGroupID))
    {
        InfoMsg("无效的上行列车通播组");
        edtUpLineTrainGroupID->SetFocus();
        return;
    }

    if (!ToInt64(edtDownLineTrainGroupID->Text, &stGroupCfg.i64DownLineTrainGroupID))
    {
        InfoMsg("无效的下行列车通播组");
        edtDownLineTrainGroupID->SetFocus();
        return;
    }

    if (!ToInt64(edtMainLineTrainGroupID->Text, &stGroupCfg.i64MainLineTrainGroupID))
    {
        InfoMsg("无效的正线列车通播组");
        edtMainLineTrainGroupID->SetFocus();
        return;
    }

    if (!ToInt64(edtUpLineTrainBrdGroupID->Text, &stGroupCfg.i64UpLineTrainBrdGroupID))
    {
        InfoMsg("无效的上行列车广播组");
        edtUpLineTrainBrdGroupID->SetFocus();
        return;
    }

    if (!ToInt64(edtDownLineTrainBrdGroupID->Text, &stGroupCfg.i64DownLineTrainBrdGroupID))
    {
        InfoMsg("无效的下行列车广播组");
        edtDownLineTrainBrdGroupID->SetFocus();
        return;
    }

    if (!ToInt64(edtMainLineTrainBrdGroupID->Text, &stGroupCfg.i64MainLineTrainBrdGroupID))
    {
        InfoMsg("无效的正线列车广播组");
        edtMainLineTrainBrdGroupID->SetFocus();
        return;
    }

    if (!ToInt64(edtAllStationGroupID->Text, &stGroupCfg.i64AllStationGroupID))
    {
        InfoMsg("无效的固定台通播组");
        edtAllStationGroupID->SetFocus();
        return;
    }

    if (!ToInt64(edtDriverGroupID->Text, &stGroupCfg.i64DriverGroupID))
    {
        InfoMsg("无效的正线司机通播组");
        edtDriverGroupID->SetFocus();
        return;
    }

    if (!ToInt64(edtDepotDriverGroupID->Text, &stGroupCfg.i64DepotDriverGroupID))
    {
        InfoMsg("无效的车辆段司机通播组");
        edtDepotDriverGroupID->SetFocus();
        return;
    }

    if (!ToInt64(edtStartTempDynamicGroupID->Text, &stGroupCfg.i64StartTempDynamicGroupID))
    {
        InfoMsg("无效的临时动态组起始号码");
        edtStartTempDynamicGroupID->SetFocus();
        return;
    }

    if (!ToInt64(edtEndTempDynamicGroupID->Text, &stGroupCfg.i64EndTempDynamicGroupID))
    {
        InfoMsg("无效的临时动态组结束号码");
        edtEndTempDynamicGroupID->SetFocus();
        return;
    }

    if (memcmp(&stGroupCfg, pstGroupCfg, sizeof(stGroupCfg)) == 0)
    {
        InfoMsg("操作成功");
        return;
    }

    GJsonParam  ReqJsonParam;

    ReqJsonParam.Add("UpLineTrainGroupID"     , stGroupCfg.i64UpLineTrainGroupID     );
    ReqJsonParam.Add("DownLineTrainGroupID"   , stGroupCfg.i64DownLineTrainGroupID   );
    ReqJsonParam.Add("MainLineTrainGroupID"   , stGroupCfg.i64MainLineTrainGroupID   );
    ReqJsonParam.Add("UpLineTrainBrdGroupID"  , stGroupCfg.i64UpLineTrainBrdGroupID  );
    ReqJsonParam.Add("DownLineTrainBrdGroupID", stGroupCfg.i64DownLineTrainBrdGroupID);
    ReqJsonParam.Add("MainLineTrainBrdGroupID", stGroupCfg.i64MainLineTrainBrdGroupID);
    ReqJsonParam.Add("AllStationGroupID"      , stGroupCfg.i64AllStationGroupID      );
    ReqJsonParam.Add("StartTempDynamicGroupID", stGroupCfg.i64StartTempDynamicGroupID);
    ReqJsonParam.Add("EndTempDynamicGroupID"  , stGroupCfg.i64EndTempDynamicGroupID  );
    ReqJsonParam.Add("DriverGroupID"          , stGroupCfg.i64DriverGroupID          );
    ReqJsonParam.Add("DepotDriverGroupID"     , stGroupCfg.i64DepotDriverGroupID     );

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCSetGroupCfg, ReqJsonParam))
    {
        ErrMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCfg::MainPanelResize(TObject *Sender)
{
    int  iLeft = (gbCfg->Parent->Width - gbCfg->Width)/2;
    int  iTop  = (gbCfg->Parent->Height - gbCfg->Height)/2;

    gbCfg->Left = iLeft<0?0:iLeft;
    gbCfg->Top  = iTop<0?0:iTop;
}
//---------------------------------------------------------------------------



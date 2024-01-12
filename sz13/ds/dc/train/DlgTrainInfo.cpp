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
    if (!bIsIPHOnline)   return "����";

    if (ulIPHStatus == IPH_STATUS_NORMAL  )   return "����";
    if (ulIPHStatus == IPH_STATUS_ABNORMAL)   return "�쳣";

    return "δ֪";
}

AnsiString GetPAStatusText(BOOL bIsPAOnline, UINT32 ulPAStatus)
{
    if (!bIsPAOnline)   return "����";

    if (ulPAStatus == PA_STATUS_NA  )   return "ϵͳ������";
    if (ulPAStatus == PA_STATUS_IDLE)   return "����";
    if (ulPAStatus == PA_STATUS_WORK)   return "�㲥��";

    return "δ֪";
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
    Node->Values[0] = "���κ�";
    Node->Values[1] = pstTrain->astAppTrainInfo[0].acTrainID;
    Node->Values[2] = pstTrain->astAppTrainInfo[1].acTrainID;

    Node = tlMain->Add();
    Node->Values[0] = "����̨λ��";
    Node->Values[1] = GetTCText(pstTrain->astAppTrainInfo[0].ulTC);
    Node->Values[2] = GetTCText(pstTrain->astAppTrainInfo[1].ulTC);

    Node = tlMain->Add();
    Node->Values[0] = "��ǰͨ����";
    Node->Values[1] = GetIntText(pstTrain->astAppTrainInfo[0].i64CurrentGroupID);
    Node->Values[2] = GetIntText(pstTrain->astAppTrainInfo[1].i64CurrentGroupID);

    Node = tlMain->Add();
    Node->Values[0] = "��ǰ��վ";
    Node->Values[1] = Station::GetStationName(pstTrain->astAppTrainInfo[0].ulStationID);
    Node->Values[2] = Station::GetStationName(pstTrain->astAppTrainInfo[1].ulStationID);

    Node = tlMain->Add();
    Node->Values[0] = "С��";
    Node->Values[1] = GetIntText(pstTrain->astAppTrainInfo[0].ulCellID);
    Node->Values[2] = GetIntText(pstTrain->astAppTrainInfo[1].ulCellID);

    Node = tlMain->Add();
    Node->Values[0] = "��������";
    Node->Values[1] = GetCallTypeText(pstTrain->astAppTrainInfo[0].ulCallType, 0);
    Node->Values[2] = GetCallTypeText(pstTrain->astAppTrainInfo[1].ulCallType, 0);

    Node = tlMain->Add();
    Node->Values[0] = "���к���";
    Node->Values[1] = GetIntText(pstTrain->astAppTrainInfo[0].i64CallID);
    Node->Values[2] = GetIntText(pstTrain->astAppTrainInfo[1].i64CallID);

    Node = tlMain->Add();
    Node->Values[0] = "��ǰʱ��";
    Node->Values[1] = GetTextTime(pstTrain->astAppTrainInfo[0].ulCurrentTime);
    Node->Values[2] = GetTextTime(pstTrain->astAppTrainInfo[1].ulCurrentTime);

    Node = tlMain->Add();
    Node->Values[0] = "����ʱ�䣨�룩";
    Node->Values[1] = pstTrain->astAppTrainInfo[0].ulUptime;
    Node->Values[2] = pstTrain->astAppTrainInfo[1].ulUptime;

    Node = tlMain->Add();
    Node->Values[0] = "�ն˰汾";
    Node->Values[1] = pstTrain->astAppTrainInfo[0].acAPPVer;
    Node->Values[2] = pstTrain->astAppTrainInfo[1].acAPPVer;

    Node = tlMain->Add();
    Node->Values[0] = "IPH�豸״̬";
    Node->Values[1] = GetIPHStatusText(pstTrain->astAppTrainInfo[0].bIsIPHOnline, pstTrain->astAppTrainInfo[0].ulIPHStatus);
    Node->Values[2] = GetIPHStatusText(pstTrain->astAppTrainInfo[1].bIsIPHOnline, pstTrain->astAppTrainInfo[1].ulIPHStatus);

    Node = tlMain->Add();
    Node->Values[0] = "�㲥�豸״̬";
    Node->Values[1] = GetPAStatusText(pstTrain->astAppTrainInfo[0].bIsPAOnline, pstTrain->astAppTrainInfo[0].ulPAStatus);
    Node->Values[2] = GetPAStatusText(pstTrain->astAppTrainInfo[1].bIsPAOnline, pstTrain->astAppTrainInfo[1].ulPAStatus);

    Node = tlMain->Add();
    Node->Values[0] = "�㲥����״̬";
    Node->Values[1] = pstTrain->astAppTrainInfo[0].bIsMasterPA?"����":"����";
    Node->Values[2] = pstTrain->astAppTrainInfo[1].bIsMasterPA?"����":"����";

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


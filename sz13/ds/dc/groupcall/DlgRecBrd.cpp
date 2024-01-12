//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "UICfg.h"
#include "DlgRealBrd.h"
#include "DlgRecBrd.h"
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
#pragma link "cxProgressBar"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxRadioGroup"
#pragma link "cxDropDownEdit"
#pragma link "cxMaskEdit"
#pragma link "dxBevel"
#pragma link "cxCustomData"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxImageComboBox"
#pragma link "cxImageList"
#pragma link "cxInplaceContainer"
#pragma link "cxStyles"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "cxSpinEdit"
#pragma resource "*.dfm"
TDialogRecBrd *DialogRecBrd;

static VECTOR<REC_BRD_CFG_T>    g_vRecBrdCfg;

void TrainRecBrd(VECTOR<UINT32> &vTrain)
{
    if (!DialogRecBrd)
    {
        return;
    }

    DialogRecBrd->Init();
    DialogRecBrd->TrainBrd(vTrain);
    DialogRecBrd->ShowModal();
}

void TrainRecBrd(UINT32 ulTrainUnitID)
{
    VECTOR<UINT32>  vTrain;

    vTrain.push_back(ulTrainUnitID);

    TrainRecBrd(vTrain);
}

void LineRecBrd(UINT32 ulBrdType)
{
    if (!DialogRecBrd)
    {
        return;
    }

    DialogRecBrd->Init();
    DialogRecBrd->LineBrd(ulBrdType);
    DialogRecBrd->ShowModal();
}

void InterLockStationRecBrd(UINT32 ulStationID)
{
    if (!DialogRecBrd)
    {
        return;
    }

    DialogRecBrd->Init();
    DialogRecBrd->InterLockStationBrd(ulStationID);
    DialogRecBrd->ShowModal();
}

void StationRecBrd(UINT32 ulStationID)
{
    if (!DialogRecBrd)
    {
        return;
    }

    DialogRecBrd->Init();
    DialogRecBrd->StationBrd(ulStationID);
    DialogRecBrd->ShowModal();
}

VECTOR<REC_BRD_CFG_T> & GetRecBrdCfg()
{
    return g_vRecBrdCfg;
}

VOID InitRecBrdCfg(VECTOR<REC_BRD_CFG_T> &vInfo)
{
    g_vRecBrdCfg.clear();
    g_vRecBrdCfg.assign(vInfo.begin(), vInfo.end());
}

BOOL IsRecBrdIDExist(UINT32 ulRecID)
{
    for (UINT32 i=0; i<g_vRecBrdCfg.size(); i++)
    {
        if (g_vRecBrdCfg[i].ulRecID == ulRecID)
        {
            return TRUE;
        }
    }

    return FALSE;
}

void __fastcall TDialogRecBrd::InitSkin()
{
}
//---------------------------------------------------------------------------
__fastcall TDialogRecBrd::TDialogRecBrd(TComponent* Owner)
    : TDialogForm(Owner)
{
    InitMsgHandler(EV_SA_DIS_DC_REQ, MSG_DCStartRecBrd, OnDCStartRecBrdRsp);
}
//---------------------------------------------------------------------------
void __fastcall TDialogRecBrd::OnDCStartRecBrdRsp(UINT32 ulSeqID, GJson &Json)
{
    UINT32      ulTrainUnitID;
    UINT32      ulRecID;
    UINT32      ulBrdSeqID;
    INT32       iResult;
    CHAR        *szSAID;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        GosLog(LOG_ERROR, "invalid TrainUnitID in msg %s", Json.GetText());
        return;
    }

    if (!Json.GetValue("RecID", &ulRecID))
    {
        GosLog(LOG_ERROR, "invalid RecID in msg %s", Json.GetText());
        return;
    }

    if (!Json.GetValue("BrdSeqID", &ulBrdSeqID))
    {
        GosLog(LOG_ERROR, "invalid BrdSeqID in msg %s", Json.GetText());
        return;
    }

    szSAID = Json.GetValue("SAID");
    if (!szSAID)
    {
        szSAID = "";
    }

    if (!Json.GetValue("Result", &iResult))
    {
        GosLog(LOG_ERROR, "invalid Result in msg %s", Json.GetText());
        return;
    }

    GosLog(LOG_INFO, "recv PA start rec broadcast rsp(%u(%s), %u, %u, %d)", ulTrainUnitID, szSAID, ulRecID, ulBrdSeqID, iResult);

    bool    bAllSucc = true;

    for (int i=0; i<tlMain->AbsoluteCount; i++)
    {
        TcxTreeListNode     *Node = tlMain->AbsoluteItems[i];

        if (!Node->Values[0].IsNull() &&
            (UINT32)Node->Values[0] == ulTrainUnitID)
        {
            if ((INT32)Node->Values[4] != REC_BRD_RESULT_SUCC)
            {
                Node->Values[4] = iResult;
            }
        }

        if ((INT32)Node->Values[4] != REC_BRD_RESULT_SUCC)
        {
            bAllSucc = false;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogRecBrd::InitRecBrdInfo()
{
    m_DictRecBrdCfg.Clear();

    for (UINT32 i=0; i<g_vRecBrdCfg.size(); i++)
    {
        m_DictRecBrdCfg.Add(g_vRecBrdCfg[i].ulRecID, g_vRecBrdCfg[i].acRecInfo);
    }

    m_DictRecBrdCfg.Init(cbRec);
}
//---------------------------------------------------------------------------
void __fastcall TDialogRecBrd::Init()
{
    TimerCheck->Enabled = false;
    btnBrd->Visible = true;
    btnBrd->Enabled = true;
    cbRec->Enabled = true;

    InitTree();
}
//---------------------------------------------------------------------------
void __fastcall TDialogRecBrd::InitTree()
{
    InitRecBrdInfo();

    tlMain->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TDialogRecBrd::InterLockStationBrd(UINT32 ulStationID)
{
    VECTOR<TRAIN_INFO_T*>   vTrain;

    Train::GetTrainOfStation(ulStationID, vTrain);

    TrainBrd(vTrain);

    edtBrdType->Text = "集中站列车广播 - " + Station::GetStationName(ulStationID);
}
//---------------------------------------------------------------------------
void __fastcall TDialogRecBrd::StationBrd(UINT32 ulStationID)
{
    VECTOR<TRAIN_INFO_T*>   vTrain;

    Train::GetTrainOfInterLockStation(ulStationID, vTrain);

    TrainBrd(vTrain);
    edtBrdType->Text = "站管区列车广播 - " + Station::GetStationName(ulStationID);
}
//---------------------------------------------------------------------------
void __fastcall TDialogRecBrd::LineBrd(UINT32 ulBrdType)
{
    VECTOR<TRAIN_INFO_T*>   vTrainInfo;
    VECTOR<TRAIN_INFO_T*>   &vTrain = Train::GetTrainInfo();
    TRAIN_INFO_T            *pstTrain;

    for (UINT32 i=0; i<vTrain.size(); i++)
    {
        pstTrain = vTrain[i];
        if (pstTrain->ulRegisterDepotID != DEPOT_ID_MAINLINE)
        {
            continue;
        }

        if (ulBrdType == MAINLINE_TRAIN)
        {
            vTrainInfo.push_back(pstTrain);
        }
        else if (ulBrdType == UPLINE_TRAIN &&
                 pstTrain->ulDirection == TRAIN_DIRECTION_UPSIDE)
        {
            vTrainInfo.push_back(pstTrain);
        }
        else if (ulBrdType == DOWNLINE_TRAIN &&
                 pstTrain->ulDirection == TRAIN_DIRECTION_DOWNSIDE)
        {
            vTrainInfo.push_back(pstTrain);
        }
    }

    TrainBrd(vTrain);

    if (ulBrdType == MAINLINE_TRAIN)
    {
        edtBrdType->Text = "正线列车广播";
    }
    else if (ulBrdType == UPLINE_TRAIN)
    {
        edtBrdType->Text = "上行列车广播";
    }
    else if (ulBrdType == DOWNLINE_TRAIN)
    {
        edtBrdType->Text = "下行列车广播";
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogRecBrd::TrainBrd(VECTOR<TRAIN_INFO_T*> &vTrainInfo)
{
    TcxTreeListNode     *Node;
    TRAIN_INFO_T        *pstTrainInfo;

    if (vTrainInfo.size() == 0)
    {
        return;
    }

    tlMain->BeginUpdate();
    tlMain->Clear();
    m_TupleParam.Clear();

    for (UINT32 i=0; i<vTrainInfo.size(); i++)
    {
        pstTrainInfo = vTrainInfo[i];

        Node = tlMain->Add();
        Node->Values[0] = pstTrainInfo->ulTrainUnitID;
        Node->Values[1] = pstTrainInfo->acTrainID;
        Node->Values[2] = Station::GetStationName(pstTrainInfo->ulStationID);
        Node->Values[3] = pstTrainInfo->ulDirection;
        Node->Values[4] = REC_BRD_RESULT_NULL;

        GJsonParam      SubParam;

        SubParam.Add("TrainUnitID", pstTrainInfo->ulTrainUnitID);
        m_TupleParam.Add(SubParam);
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TDialogRecBrd::TrainBrd(VECTOR<UINT32> &vTrain)
{
    VECTOR<TRAIN_INFO_T*>   vTrainInfo;
    TRAIN_INFO_T            *pstTrainInfo;

    for (UINT32 i=0; i<vTrain.size(); i++)
    {
        pstTrainInfo = Train::GetTrainInfo(vTrain[i]);
        if (pstTrainInfo)
        {
            vTrainInfo.push_back(pstTrainInfo);
        }
    }

    if (vTrainInfo.size() > 1)
    {
        edtBrdType->Text = "多列车广播";
    }
    else
    {
        edtBrdType->Text = "列车广播";
    }

    TrainBrd(vTrainInfo);
}
//---------------------------------------------------------------------------
void __fastcall TDialogRecBrd::btnBrdClick(TObject *Sender)
{
    UINT32  ulRecID;
    UINT32  ulBrdTime = 1;

    if (tlMain->AbsoluteCount <= 0)
    {
        InfoMsg("没有需要广播的列车");
        return;
    }

    if (!m_DictRecBrdCfg.GetID(&ulRecID))
    {
        InfoMsg("请选择预录音广播内容");
        cbRec->SetFocus();
        return;
    }

    if (!ToInt(edtBrdTime->Text, &ulBrdTime))
    {
        InfoMsg("广播次数非法");
        edtBrdTime->SetFocus();
        return;
    }

    if (ulBrdTime < 1 || ulBrdTime > 10)
    {
        InfoMsg("广播次数有效范围1-10");
        edtBrdTime->SetFocus();
        return;
    }

    // 向服务器发送消息
    GJsonParam      Param;
    UINT32          ulTrainUnitID;
    UINT32          ulStationID;
    TcxTreeListNode *Node;
    TcxTreeListNode *NewNode;

    if (m_TupleParam.GetTupleNum() == 0)
    {
        return;
    }

    Param.Add("RecID", ulRecID);
    Param.Add("BrdTime", ulBrdTime);
    Param.Add("Train", m_TupleParam);

    SendServer(EV_DC_DIS_SA_REQ, MSG_DCStartRecBrd, Param);

    btnBrd->Enabled = false;
    TimerCheck->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TDialogRecBrd::btnCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TDialogRecBrd::FormClose(TObject *Sender, TCloseAction &Action)
{
    TimerCheck->Enabled = false;

    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogRecBrd::FormCreate(TObject *Sender)
{
    SetImage(btnClose, IMG_EXIT);
}
//---------------------------------------------------------------------------
void __fastcall TDialogRecBrd::ColumnBrdStatusGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (!ANode || ANode->Values[4].IsNull())
    {
        return;
    }

    int     iResult = ANode->Values[4];

    if (iResult == REC_BRD_RESULT_SUCC)
    {
        Value = "成功";
    }
    else if (iResult == REC_BRD_RESULT_FAIL)
    {
        Value = "失败";
    }
    else
    {
        Value = "";
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogRecBrd::tlMainCustomDrawDataCell(TcxCustomTreeList *Sender,
          TcxCanvas *ACanvas, TcxTreeListEditCellViewInfo *AViewInfo,
          bool &ADone)
{
    if (!AViewInfo->Node || AViewInfo->Node->Values[4].IsNull() ||
        AViewInfo->Column->ItemIndex != 4)
    {
        return;
    }

    INT32   iStatus = AViewInfo->Node->Values[4];

    if (iStatus == REC_BRD_RESULT_FAIL)
    {
        ACanvas->Canvas->Brush->Color = UICfg::GetBrdFailColor();
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogRecBrd::ColumnTrainGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[0].IsNull())
    {
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogRecBrd::TimerCheckTimer(TObject *Sender)
{
    TimerCheck->Enabled = false;
}
//---------------------------------------------------------------------------


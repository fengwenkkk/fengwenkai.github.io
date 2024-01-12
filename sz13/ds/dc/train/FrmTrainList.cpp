#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "pis.h"
#include "main.h"
#include "FrmTrainList.h"
#include "FrmGroup.h"
#include "FrmUser.h"
#include "FrmTrain.h"
#include "DlgRealBrd.h"
#include "DlgRecBrd.h"
#include "FrmGroupCallWin.h"
#include "FrmLatestSDS.h"
#include "DlgSetTrainID.h"
#include "DlgSetTrainGroup.h"
#include "DlgSetTrainDepotID.h"
#include "FrmGroup.h"
#include "FrmCallIn.h"
#include "DlgTrainInfo.h"
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
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
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
#pragma link "cxImageComboBox"
#pragma link "cxPC"
#pragma link "dxSkinscxPCPainter"
#pragma link "cxProgressBar"
#pragma link "cxClasses"
#pragma link "cxImageList"
#pragma link "dxBarBuiltInMenu"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxTrackBar"
#pragma link "dxCameraControl"
#pragma link "AdvSmoothToggleButton"
#pragma link "cxCheckBox"
#pragma link "dxToggleSwitch"
#pragma link "cxLabel"
#pragma link "AdvSmoothPanel"
#pragma link "cxScrollBox"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxMaskEdit"
#pragma link "cxRadioGroup"
#pragma resource "*.dfm"
TFormTrainList *FormTrainList;

static ID_VALUE_T  s_astTrainRange[] = {
                                {SINGLE_TRAIN,          "单列列车"},
                                {MULTI_TRAIN,           "多列列车"},
                                {UPLINE_TRAIN,          "上行列车"},
                                {DOWNLINE_TRAIN,        "下行列车"},
                                {MAINLINE_TRAIN,        "正线列车"},
                                };
static Dict     g_DictTrainRange(s_astTrainRange, ARRAY_SIZE(s_astTrainRange));

void __fastcall TFormTrainList::InitWorkPanel()
{
    TcxButton   *btns[] = { btnGroupCall,
                            btnRealBrd,
                            btnRecBrd,
                            btnSetTrainPos,
//                            btnCallDriver,
                            btnPTT,
                          };

    gbWorkPanel->Width = 150;

    for (UINT32 i=0; i<ARRAY_SIZE(btns); i++)
    {
        btns[i]->TabOrder = i;
        btns[i]->Top = 240+95*i;
        btns[i]->Width = g_iMidToolbarButtonWidth;
        btns[i]->Height = g_iMidToolbarButtonHeight;
        btns[i]->Left = (btns[i]->Parent->Width - btns[i]->Width)/2;
    }

    btnPTT->Width = 120;
    btnPTT->Height = 90;
    btnPTT->Top = btnPTT->Parent->Height - btnPTT->Height - 60;
    btnPTT->Left = (btnPTT->Parent->Width - btnPTT->Width)/2;

    gbSelectTrain->Left = (gbSelectTrain->Parent->Width - gbSelectTrain->Width)/2;
}

void __fastcall TFormTrainList::InitSkin()
{
}
//---------------------------------------------------------------------------
__fastcall TFormTrainList::TFormTrainList(TComponent* Owner)
    : TSubForm(Owner)
{
    pmiShowCallWin->Down = IsShowCallWin();

    m_bIsPatchGroup = false;

    // 去除批量移车功能（本项目的车辆目的地无法批量设置）
    pmiSetTrainDepotID->Visible = ivNever;;

    m_ulCheckNodeNum = 0;
    m_ulDCGetATSUptime = 0;
    m_ulLastDISATSTimeDiff = 0;

    InitTrain();

    InitWorkPanel();

    if (IsSimpleSMSMode())
    {
        pmiTrainSDS->Caption = "短信";
    }

    InitMsgHandler(EV_DC_DIS_RSP, MSG_DCLoadTrainInfo, OnDCLoadTrainInfoRsp);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::pmiTrainSDSClick(TObject *Sender)
{
    UINT32  ulTrainUnitID = GetSelectTrain();

    if (ulTrainUnitID == INVALID_TRAIN_UNIT_ID)
    {
        return;
    }

    VECTOR<UINT32> vUserID;

    Train::GetTrainUserID(ulTrainUnitID, vUserID);

    if (vUserID.size() == 0)
    {
        return;
    }

    FormLatestSDS->NewSDS(vUserID);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::pmiRealBrdClick(TObject *Sender)
{
    UINT32  ulTrainUnitID = GetSelectTrain();

    if (ulTrainUnitID == INVALID_TRAIN_UNIT_ID)
    {
        return;
    }

    TrainRealBrd(ulTrainUnitID);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::InitTrain()
{
    UpdateTrainInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::tlTrainCustomDrawDataCell(TcxCustomTreeList *Sender, TcxCanvas *ACanvas,
          TcxTreeListEditCellViewInfo *AViewInfo, bool &ADone)
{
    if (!AViewInfo->Node)
    {
        return;
    }

    if (AViewInfo->Node->Checked)
    {
        ACanvas->Canvas->Brush->Color = UICfg::GetTreeNodeCheckedColor();
    }

    /*
    if (!AViewInfo->Node->Values[6].IsNull() &&
        AViewInfo->Column->ItemIndex == 6 )
    {
        UINT32  ulTrainStatus = AViewInfo->Node->Values[6];

        if (ulTrainStatus == TRAIN_STATUS_OFFLINE)
        {
            ACanvas->Canvas->Brush->Color = UICfg::GetOfflineDevColor();
        }
    } */

    if (!AViewInfo->Node->Values[7].IsNull() &&
        AViewInfo->Column->ItemIndex == 7 )
    {
        AnsiString  strCallStatus = AViewInfo->Node->Texts[7];

        if (!strCallStatus.IsEmpty())
        {
            ACanvas->Canvas->Brush->Color = UICfg::GetCallingColor();
        }
    }
}
//---------------------------------------------------------------------------
/**
 * @brief           更新列车信息显示列表
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
void __fastcall TFormTrainList::UpdateTrainInfo()
{
    UINT32          ulTrainUnitID;
    TcxTreeListNode *Node;
    TRAIN_INFO_T    *pstTrain;
    BOOL            bInDepot;
    IntSet          sTrain;
    bool            bShowAll = pmiShowAllTrain->Down;
    VECTOR<TRAIN_INFO_T*>       vTrain;
    VECTOR<TcxTreeListNode*>    vNode;
    VectorMap<UINT32, TRAIN_INFO_T*>    mTrain;

    Train::GetTrainInfo(vTrain);
    for (UINT32 i=0; i<vTrain.size(); i++)
    {
        pstTrain = vTrain[i];
        if (bShowAll ||
            (Station::IsTrainInDepot(pstTrain) &&
             DCUser::IsStationInDCManageRange(pstTrain->ulStationID)))
        {
            mTrain.Add(pstTrain->ulTrainUnitID, pstTrain);
        }
    }

    tlTrain->BeginUpdate();

    tlTrain->Root->CheckGroupType = ncgCheckGroup;  ///显示根复选框
    tlTrain->OptionsView->CheckGroups = rbMultiTrain->Checked;    //显示子复选框

    tlTrain->OptionsSelection->MultiSelect = rbMultiTrain->Checked;

    for (int i=0; i<tlTrain->AbsoluteCount; i++)
    {
        Node = tlTrain->AbsoluteItems[i];
        ulTrainUnitID = (UINT32)Node->Values[1];

        TRAIN_INFO_T   **ppstTrain = mTrain.GetValue(ulTrainUnitID);

        if (!ppstTrain)
        {
            vNode.push_back(Node);
            continue;
        }

        pstTrain = *ppstTrain;
        mTrain.Del(ulTrainUnitID);

        Node->Values[0] = pstTrain->acTrainID;
        Node->Values[2] = pstTrain->ulDirection;
        Node->Values[3] = pstTrain->ulStationID;
        Node->Values[4] = pstTrain->astAppTrainInfo[0].i64CurrentGroupID;
        Node->Values[5] = Depot::GetDepotName(pstTrain);
        Node->Values[6] = pstTrain->ulTrainStatus;

        Node->Texts[7] = GetCallTypeText(pstTrain->astAppTrainInfo[0].ulCallType, 0);
        Node->Values[8] = pstTrain->astAppTrainInfo[0].i64CallID;
        if (Node->Texts[7].IsEmpty())
        {
            Node->Texts[7] = GetCallTypeText(pstTrain->astAppTrainInfo[1].ulCallType, 0);
            Node->Values[8] = pstTrain->astAppTrainInfo[1].i64CallID;
        }

        if (pstTrain->astAppTrainInfo[0].ulDriverID != INVALID_USER_ID)
        {
            Node->Texts[9] = pstTrain->astAppTrainInfo[0].ulDriverID;
        }
        else if (pstTrain->astAppTrainInfo[1].ulDriverID != INVALID_USER_ID)
        {
            Node->Texts[9] = pstTrain->astAppTrainInfo[1].ulDriverID;
        }
        else
        {
            Node->Texts[9] = "";
        }

        Node->Values[11] = pstTrain->ulDriveMode;
        Node->Texts[12] = pstTrain->usSectionID;
        Node->Texts[13] = GIS::GetGISText(pstTrain->dLon, pstTrain->dLat);
        Node->Values[14] = pstTrain->ulLostRealLocation;

        if (rbMultiTrain->Checked)
        {
            Node->CheckGroupType = ncgCheckGroup;
        }
        else
        {
            Node->CheckGroupType = ncgNone;
        }
    }

    for (UINT32 i=0; i<vNode.size(); i++)
    {
        vNode[i]->Delete();
    }

    // 增加新的Train节点
    for (UINT32 i=0; i<mTrain.Size(); i++)
    {
        pstTrain = *mTrain[i];

        Node = tlTrain->Add();

        Node->Values[0] = pstTrain->acTrainID;
        Node->Values[1] = pstTrain->ulTrainUnitID;
        Node->Values[2] = pstTrain->ulDirection;
        Node->Values[3] = pstTrain->ulStationID;
        Node->Values[4] = pstTrain->astAppTrainInfo[0].i64CurrentGroupID;
        Node->Values[5] = Depot::GetDepotName(pstTrain);
        Node->Values[6] = pstTrain->ulTrainStatus;

        Node->Texts[7] = GetCallTypeText(pstTrain->astAppTrainInfo[0].ulCallType, 0);
        Node->Values[8] = pstTrain->astAppTrainInfo[0].i64CallID;
        if (Node->Texts[7].IsEmpty())
        {
            Node->Texts[7] = GetCallTypeText(pstTrain->astAppTrainInfo[1].ulCallType, 0);
            Node->Values[8] = pstTrain->astAppTrainInfo[1].i64CallID;
        }

        if (pstTrain->astAppTrainInfo[0].ulDriverID != INVALID_USER_ID)
        {
            Node->Texts[9] = pstTrain->astAppTrainInfo[0].ulDriverID;
        }
        else if (pstTrain->astAppTrainInfo[1].ulDriverID != INVALID_USER_ID)
        {
            Node->Texts[9] = pstTrain->astAppTrainInfo[1].ulDriverID;
        }
        else
        {
            Node->Texts[9] = "";
        }

        Node->Values[11] = pstTrain->ulDriveMode;
        Node->Texts[12] = pstTrain->usSectionID;
        Node->Texts[13] = GIS::GetGISText(pstTrain->dLon, pstTrain->dLat);
        Node->Values[14] = pstTrain->ulLostRealLocation;

        if (rbMultiTrain->Checked)
        {
            Node->CheckGroupType = ncgCheckGroup;
        }
        else
        {
            Node->CheckGroupType = ncgNone;
        }
    }

    tlTrain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::tlTrainNodeCheckChanged(TcxCustomTreeList *Sender, TcxTreeListNode *ANode,
          TcxCheckBoxState AState)
{
    UpdateCheckNodeNum();
    tlTrain->Update();
    tlTrain->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::UpdateCheckNodeNum()
{
    m_ulCheckNodeNum = 0;

    for (TcxTreeListNode *Node=tlTrain->Root->GetFirstChildVisible();
         Node; Node=Node->GetNextSiblingVisible())
    {
        if (Node->Checked)
        {
            m_ulCheckNodeNum++;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::GetSelectTrain(VECTOR<UINT32> &vTrain)
{
    vTrain.clear();

    for (TcxTreeListNode *Node=tlTrain->Root->GetFirstChildVisible();
         Node; Node=Node->GetNextSiblingVisible())
    {
        if (Node->Checked)
        {
            UINT32          ulTrainUnitID = Node->Values[1];

            vTrain.push_back(ulTrainUnitID);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::btnRealBrdClick(TObject *Sender)
{
    if (rbSingleTrain->Checked)
    {
        pmiRealBrdClick(this);
    }
    else if (rbMultiTrain->Checked)
    {
        VECTOR<UINT32>  vTrain;

        GetSelectTrain(vTrain);

        if (vTrain.size() == 0)
        {
            InfoMsg("未选择列车");
            return;
        }

        TrainRealBrd(vTrain);
    }
    else if (rbUpLineTrain->Checked)
    {
        UpLineTrainRealBrd();
    }
    else if (rbDownLineTrain->Checked)
    {
        DownLineTrainRealBrd();
    }
    else if (rbMainLineTrain->Checked)
    {
        MainLineTrainRealBrd();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::ColumnStationGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[3].IsNull())
    {
        return;
    }

    int iValue = (int)ANode->Values[3];
    Value = Station::GetStationName(iValue);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::OnDCLoadTrainInfoRsp(UINT32 ulSeqID, GJson &Json)
{
    CHAR            *szCfgData = Json.GetValue("CfgData");
    GJson           CfgDataJson;

    if (!CfgDataJson.Parse(szCfgData))
    {
        GosLog(LOG_ERROR, "parse train info failed");

        return;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = CfgDataJson.GetSubJson();
    TRAIN_INFO_T    stInfo = {0};
    TRAIN_INFO_T    *pstTrain;
    VECTOR<TRAIN_INFO_T>    vInfo;
    INT32           iTime;
    BOOL            bChanged = FALSE;
    UINT32          ulMinTimeDiff = 0;
    UINT32          ulATSDataTimeDiff = 0xffffffff;

    SetConnectToDIS(TRUE);
    FormMain->SetStatusButtonStatus(FormMain->btnDISStatus, TRUE);

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!ParseTrainInfo(pSubJson, stInfo))
        {
            GosLog(LOG_ERROR, "parse train info failed on %s", pSubJson->GetText());
            return;
        }

        if (i == 0)
        {
            if (!pSubJson->GetValue("ATSDataTimeDiff", &ulATSDataTimeDiff) )
            {
                GosLog(LOG_ERROR, "invalid ATSDataTimeDiff of %s", pSubJson->GetText());
                ulATSDataTimeDiff = 0xffffffff;

//                return FALSE;
            }

        }

        if (i == 0 ||
            stInfo.ulTimeDiff < ulMinTimeDiff)
        {
            ulMinTimeDiff = stInfo.ulTimeDiff;

           // GosLog(LOG_INFO, "curr max ATS time is %u, diff is %u", ulMaxUptime, gos_get_sys_uptime() - ulMaxUptime);
        }

        vInfo.push_back(stInfo);
    }

    if (ulATSDataTimeDiff == 0xffffffff)
    {
        GosLog(LOG_INFO, "recv ATS data of %u seconds ago", ulMinTimeDiff);

        if (ulMinTimeDiff != m_ulLastDISATSTimeDiff)
        {
            bChanged = TRUE;
            m_ulLastDISATSTimeDiff = ulMinTimeDiff;
        }
    }
    else
    {
        GosLog(LOG_INFO, "recv ATS data of %u seconds ago", ulATSDataTimeDiff);

        if (ulATSDataTimeDiff != m_ulLastDISATSTimeDiff)
        {
            bChanged = TRUE;
            m_ulLastDISATSTimeDiff = ulATSDataTimeDiff;
        }
    }

    // 收到应答设置为0
    m_ulDCGetATSUptime = 0;
    if (bChanged)
    {
        FormMain->UpdateATSStatus(TRUE);
    }

    Train::InitTrainInfo(vInfo, ulATSDataTimeDiff);

    /// 检查并加入列车组
    Group::CheckTrainGroup();

    /// 检查并加入车站组
    Group::CheckStationGroup();

    // 更新显示列车信息的列表
    UpdateTrainInfo();

    if (FormTrain)
    {
        FormTrain->InitStationMap();
        FormTrain->InitTrainMap();
        FormTrain->InitDepotTrainMap();
    }

    CheckATSStatus();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::CheckATSStatus()
{
    BOOL    bATSValid = TRUE;
    UINT32  ulCurrUptime = gos_get_uptime();

    // 如果一段时间内没有收到服务器的ATS数据，表示断开
    if (m_ulDCGetATSUptime > 0 &&
        (UINT32)(ulCurrUptime - m_ulDCGetATSUptime) > GetATSValidPeriod())
    {
        GosLog(LOG_ERROR, "lost ATS data for %u seconds", (UINT32)(ulCurrUptime - m_ulDCGetATSUptime));
        bATSValid = FALSE;
    }
    else
    {
        // 如果收到的DIS数据中ATS时间戳没有变化
        if (m_ulLastDISATSTimeDiff > GetATSValidPeriod())
        {
            GosLog(LOG_ERROR, "time of ATS data is halted for %u seconds", m_ulLastDISATSTimeDiff);
            bATSValid = FALSE;
        }
    }

    FormMain->UpdateATSStatus(bATSValid);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::TimerGetATSTimer(TObject *Sender)
{
    TimerGetATS->Interval = 3000;

    SendServer(EV_DC_DIS_REQ, MSG_DCLoadTrainInfo);

    // 记录最早一次发送时间
    if (m_ulDCGetATSUptime == 0)
    {
        m_ulDCGetATSUptime = gos_get_uptime();
    }

    CheckATSStatus();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::FormCreate(TObject *Sender)
{
    SetImage(pmiPTTDialTrain, IMG_PTT_DIAL);
    SetImage(pmiRealBrd, IMG_BRD);
    SetImage(pmiTrainSDS, IMG_SDS);

    SetImage(pmiSetTrainPos, IMG_SWITCH);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::ColumnCurrGroupIDGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (!ANode || ANode->Values[4].IsNull())
    {
        return;
    }

    INT64 i64GroupID = ANode->Values[4];

    if (i64GroupID == INVALID_GROUP_ID)
    {
        Value = "";
    }
    else
    {
        Value = Group::GetGroupDisplayName(i64GroupID);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::pmiRecBrdClick(TObject *Sender)
{
    UINT32  ulTrainUnitID = GetSelectTrain();

    if (ulTrainUnitID == INVALID_TRAIN_UNIT_ID)
    {
        return;
    }

    TrainRecBrd(ulTrainUnitID);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::pmiSetTrainPosClick(TObject *Sender)
{
    UINT32          ulTrainPos;
    UINT32          ulTrainUnitID = GetSelectTrain();

    if (ulTrainUnitID == INVALID_TRAIN_UNIT_ID)
    {
        return;
    }

    TRAIN_INFO_T    *pstTrain = Train::GetTrainInfo(ulTrainUnitID);

    if (!pstTrain)
    {
        InfoMsg("未知列车");
        return;
    }

    SetTrainPos(pstTrain);
}
//---------------------------------------------------------------------------
UINT32 __fastcall TFormTrainList::GetSelectTrain()
{
    if (tlTrain->SelectionCount == 0)
    {
        return INVALID_TRAIN_UNIT_ID;
    }

    TcxTreeListNode     *Node = tlTrain->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return INVALID_TRAIN_UNIT_ID;
    }

    return (UINT32)Node->Values[1];
}
//---------------------------------------------------------------------------
INT64 __fastcall TFormTrainList::GetSelectTrainGroupID()
{
    GROUP_CFG_T     *pstGroupCfg = GroupCfg::GetGroupCfg();

    m_bIsPatchGroup = false;

    if (rbUpLineTrain->Checked)
    {
        return pstGroupCfg->i64UpLineTrainGroupID;
    }
    else if (rbDownLineTrain->Checked)
    {
        return pstGroupCfg->i64DownLineTrainGroupID;
    }
    else if (rbMainLineTrain->Checked)
    {
        return pstGroupCfg->i64MainLineTrainGroupID;
    }
    else if (rbSingleTrain->Checked)
    {
        UINT32              ulTrainUnitID = GetSelectTrain();
        TRAIN_INFO_T        *pstTrain = Train::GetTrainInfo(ulTrainUnitID);

        if (!pstTrain)
        {
            return INVALID_GROUP_ID;
        }

        return pstTrain->i64DefaultGroupID;
    }
    else if (rbMultiTrain->Checked)
    {
        VECTOR<UINT32>  vTrain;
        VECTOR<INT64>   vGroupID;

        GetSelectTrain(vTrain);
        for (UINT32 i=0; i<vTrain.size(); i++)
        {
            TRAIN_INFO_T    *pstTrain = Train::GetTrainInfo(vTrain[i]);

            if (pstTrain)
            {
                vGroupID.push_back(pstTrain->i64DefaultGroupID);
            }
        }

        if (vGroupID.size() == 0)
        {
        //    InfoMsg("请选择列车");
            return INVALID_GROUP_ID;
        }

        if (vGroupID.size() == 1)
        {
            return vGroupID[0];
        }

        INT64   i64GroupID = PatchGroup::AllocTmpPatchGroup(vGroupID);

        if (i64GroupID == INVALID_GROUP_ID)
        {
            ErrMsg("派接组资源已达上限，创建失败");
            return INVALID_GROUP_ID;
        }

        m_bIsPatchGroup = true;
        return i64GroupID;
    }
    else
    {
        return INVALID_GROUP_ID;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::btnGroupCallClick(TObject *Sender)
{
    INT64       i64GroupID = GetSelectTrainGroupID();

    if (i64GroupID == INVALID_GROUP_ID)
    {
        return;
    }

    if (!Group::SubJoinGroup(i64GroupID))
    {
        InfoMsg("加入组失败!");
        return;
    }

    Group::UpdateGroupSubStatus(i64GroupID, GROUP_STATUS_JOIN, FALSE);

    if (IsShowCallWin())
    {
        AnsiString      strName = "";

        if (rbMultiTrain->Checked)
        {
            strName = "多选列车";
        }

        ShowGroupCallWin(i64GroupID, strName, m_bIsPatchGroup);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::btnSDSClick(TObject *Sender)
{
    VECTOR<UINT32>      vTrain;
    VECTOR<UINT32>      vUserID;
    UINT32              ulUserID1;
    UINT32              ulUserID2;

    GetSelectTrain(vTrain);
    for (UINT32 i=0; i<vTrain.size(); i++)
    {
        Train::GetTrainUserID(vTrain[i], &ulUserID1, &ulUserID2);

        if (ulUserID1 != INVALID_USER_ID)
        {
            vUserID.push_back(ulUserID1);
        }

        if (ulUserID2 != INVALID_USER_ID)
        {
            vUserID.push_back(ulUserID2);
        }
    }

    FormLatestSDS->NewSDS(vUserID);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::btnRecBrdClick(TObject *Sender)
{
    if (rbSingleTrain->Checked)
    {
        pmiRecBrdClick(this);
    }
    else if (rbMultiTrain->Checked)
    {
        VECTOR<UINT32>  vTrain;

        GetSelectTrain(vTrain);

        if (vTrain.size() == 0)
        {
            return;
        }

        TrainRecBrd(vTrain);
    }
    else if (rbUpLineTrain->Checked)
    {
        LineRecBrd(UPLINE_TRAIN);
    }
    else if (rbDownLineTrain->Checked)
    {
        LineRecBrd(DOWNLINE_TRAIN);
    }
    else if (rbMainLineTrain->Checked)
    {
        LineRecBrd(MAINLINE_TRAIN);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::rbSelectTrainClick(TObject *Sender)
{
    if (!rbMultiTrain->Checked)
    {
        for (int i=0; i<tlTrain->AbsoluteCount; i++)
        {
            tlTrain->AbsoluteItems[i]->Checked = false;
        }
    }

    InitTrain();
}
//---------------------------------------------------------------------------

void __fastcall TFormTrainList::tlTrainClick(TObject *Sender)
{
    if (!rbMultiTrain->Checked)
    {
        return;
    }

    if (tlTrain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlTrain->Selections[0];

    Node->Checked = !Node->Checked;

    Node->Selected = false;//Node->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::pmiShowAllTrainClick(TObject *Sender)
{
    InitTrain();
}
//---------------------------------------------------------------------------

void __fastcall TFormTrainList::btnSetTrainPosClick(TObject *Sender)
{
    UINT32          ulTrainUnitID = GetSelectTrain();
    TRAIN_INFO_T    *pstTrain = Train::GetTrainInfo(ulTrainUnitID);

    if (!pstTrain)
    {
        InfoMsg("未知列车");
        return;
    }

    SetTrainPos(pstTrain);
}
//---------------------------------------------------------------------------

void __fastcall TFormTrainList::FormShow(TObject *Sender)
{
    btnPTT->Font->Size = UICfg::GetButtonPTTFontSize();
    btnPTT->Font->Name = UICfg::GetButtonPTTFontName();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::btnPTTMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    OnPTTDial();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::btnPTTMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    OnPTTRelease();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::ColumnDevStatusGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[6].IsNull())
    {
        return;
    }

    UINT32  ulTrainStatus = ANode->Values[6];

    if (ulTrainStatus == TRAIN_STATUS_ONLINE)
    {
        Value = "在线";
    }
    else if (ulTrainStatus == TRAIN_STATUS_OFFLINE)
    {
        Value = "离线";
    }
    else if (ulTrainStatus == TRAIN_STATUS_SILENT)
    {
        Value = "静默";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::btnPTTMouseEnter(TObject *Sender)
{
    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::btnPTTMouseLeave(TObject *Sender)
{
    btnPTT->LookAndFeel->SkinName = GetDefaultSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::ColumnCallNumberGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (!ANode || ANode->Values[8].IsNull())
    {
        return;
    }

    INT64   i64Number = (INT64)ANode->Values[8];

    if (i64Number <= 0)
    {
        Value = "";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::pmiSetTrainDepotIDClick(TObject *Sender)
{
    VECTOR<UINT32>  vTrain;

    GetSelectTrain(vTrain);

    if (vTrain.size() == 0)
    {
        InfoMsg("请选择列车");
        return;
    }

    DialogSetTrainDepotID->Init(vTrain);
    if (DialogSetTrainDepotID->ShowModal() != mrOk)
    {
        return;
    }

    CHAR    acDestID[3];
    UINT32  ulDestDepotID;
    DEPOT_INFO_T    *pstDepot = Depot::GetDepotInfo(APP::GetDepotID());

    if (!pstDepot)
    {
        ErrMsg("服务器未配置本车辆段");
        return;
    }

    DialogSetTrainDepotID->Get(&ulDestDepotID, acDestID);

    GJsonParam          Param;
    GJsonTupleParam     JsonTupleParam;

    for (UINT32 i=0; i<vTrain.size(); i++)
    {
        GJsonParam  SubJsonParam;

        SubJsonParam.Add("TrainUnitID", vTrain[i]);
        JsonTupleParam.Add(SubJsonParam);
    }

    Param.Add("DestDepotID", ulDestDepotID);
    Param.Add("DestID", acDestID);
    Param.Add("TrainDirection", pstDepot->ulTrainDirection);
    Param.Add("Train", JsonTupleParam);

    if (!SyncSendServer(EV_DC_DIS_REQ, MSG_DCSetTrainDepotID, Param))
    {
        InfoMsg("批量挪车失败");
    }
    else
    {
        InfoMsg("批量挪车成功");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::pmiPTTDialTrainClick(TObject *Sender)
{
    UINT32  ulTrainUnitID = GetSelectTrain();

    if (ulTrainUnitID == INVALID_TRAIN_UNIT_ID)
    {
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::pmiShowCallWinClick(TObject *Sender)
{
    BOOL    bShow = pmiShowCallWin->Down;

    SetShowCallWin(bShow);

    SaveLocalConfInfo();

    btnPTT->Visible = !bShow;
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::pmTrainPopup(TObject *Sender)
{
    pmiShowCallWin->Down = IsShowCallWin();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::btnCallDriverClick(TObject *Sender)
{
    GROUP_CFG_T     *pstGroupCfg = GroupCfg::GetGroupCfg();
    INT64   i64GroupID = pstGroupCfg->i64DriverGroupID;

    if (APP::GetDepotID() != DEPOT_ID_MAINLINE)
    {
        i64GroupID = pstGroupCfg->i64DepotDriverGroupID;
    }

    ShowGroupCallWin(i64GroupID);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::OnPTTDial()
{
    if (m_i64CurrGroupID == INVALID_GROUP_ID ||
        !m_bIsPatchGroup)
    {
        m_i64CurrGroupID = GetSelectTrainGroupID();
        if (m_i64CurrGroupID == INVALID_GROUP_ID)
        {
            return;
        }
    }

    if (PTT::Dial(m_i64CurrGroupID))
    {
        btnPTT->LookAndFeel->SkinName = UICfg::GetPTTDownSkinName();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::OnPTTRelease()
{
    PTT::Release();
    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::NewGroupCall(INT64 i64GroupID)
{
    for (int i=0; i<tlTrain->AbsoluteCount; i++)
    {
        if (!tlTrain->AbsoluteItems[i]->Values[4].IsNull() &&
            (INT64)tlTrain->AbsoluteItems[i]->Values[4] == i64GroupID)
        {
            tlTrain->AbsoluteItems[i]->Texts[7] = GetCallTypeText(CALL_TYPE_PTT, 0);
            break;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::CloseGroupCall(INT64 i64GroupID)
{
    if (m_i64CurrGroupID == i64GroupID)
    {
        if (m_bIsPatchGroup)
        {
            PatchGroup::CancelPatchGroup(m_i64CurrGroupID);
            m_bIsPatchGroup = false;
        }

        m_i64CurrGroupID = INVALID_GROUP_ID;
    }

    for (int i=0; i<tlTrain->AbsoluteCount; i++)
    {
        if (!tlTrain->AbsoluteItems[i]->Values[4].IsNull() &&
            (INT64)tlTrain->AbsoluteItems[i]->Values[4] == i64GroupID)
        {
            tlTrain->AbsoluteItems[i]->Texts[7] = "";
            break;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::pmiTrainGroupMemberClick(TObject *Sender)
{
    UINT32  ulTrainUnitID = GetSelectTrain();

    if (ulTrainUnitID == INVALID_TRAIN_UNIT_ID)
    {
        return;
    }

    INT64   i64GroupID = Train::GetTrainGroupID(ulTrainUnitID);

    if (i64GroupID != INVALID_GROUP_ID)
    {
        FormGroup->SetGroupMember(i64GroupID);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::pmiTrainInfoClick(TObject *Sender)
{
    UINT32  ulTrainUnitID = GetSelectTrain();

    if (ulTrainUnitID == INVALID_TRAIN_UNIT_ID)
    {
        return;
    }

    TRAIN_INFO_T    *pstTrainInfo = Train::GetTrainInfo(ulTrainUnitID);

    if (!pstTrainInfo)
    {
        return;
    }

    DialogTrainInfo->Init(pstTrainInfo);
    DialogTrainInfo->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::ColumnTrainUnitIDGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[1].IsNull())
    {
        return;
    }

    UINT32  ulTrainUnitID = ANode->Values[1];

    Value = Train::GetTrainUnitIDText(ulTrainUnitID);
}
//---------------------------------------------------------------------------

void __fastcall TFormTrainList::ColumnDriveModeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[11].IsNull())
    {
        return;
    }

    UINT32  ulDriveMode = ANode->Values[11];

    if (ulDriveMode == ATS_DRIVE_MODE_AM)
    {
        Value = "AM";
    }
    else if (ulDriveMode == ATS_DRIVE_MODE_CM)
    {
        Value = "CM";
    }
    else if (ulDriveMode == ATS_DRIVE_MODE_RM)
    {
        Value = "RM";
    }
    else if (ulDriveMode == ATS_DRIVE_MODE_EUM)
    {
        Value = "EUM";
    }
    else if (ulDriveMode == ATS_DRIVE_MODE_FAM)
    {
        Value = "FAM";
    }
    else if (ulDriveMode == ATS_DRIVE_MODE_CAM)
    {
        Value = "CAM";
    }
    else if (ulDriveMode == ATS_DRIVE_MODE_RRM)
    {
        Value = "RRM";
    }
    else if (ulDriveMode == ATS_DRIVE_MODE_NULL)
    {
        Value = "";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::ColumnSectionGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[12].IsNull())
    {
        return;
    }

    UINT32  ulSectionID = ANode->Values[12];

    SECTION_INFO_T  *pstSection = Section::GetSection(ulSectionID);

    if (pstSection)
    {
        Value = String(pstSection->acSectionName) + "(" + ulSectionID + ")";
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormTrainList::ColumnDirectionGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulDirecton = ANode->Values[2];

    if (ulDirecton == TRAIN_DIRECTION_UPSIDE)
    {
        Value = "上行";
    }
    else if (ulDirecton == TRAIN_DIRECTION_DOWNSIDE)
    {
        Value = "下行";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::ColumnATSStatusGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[14].IsNull())
    {
        return;
    }

    UINT32 ulLostRealLocation = (UINT32)ANode->Values[14];

    if (!IsATSValid())
    {
        ulLostRealLocation |= 0x100;
    }
    else
    {
        ulLostRealLocation = (UINT8)ulLostRealLocation;
    }

    if (ulLostRealLocation != (UINT32)ANode->Values[14])
    {
        ANode->Values[14] = ulLostRealLocation;
    }

    if (ulLostRealLocation == FALSE)
    {
        Value = "正常";
    }
    else if (ulLostRealLocation == TRUE ||
             (ulLostRealLocation & 0x100))
    {
        Value = "丢失";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainList::UpdateATSStatus()
{
    tlTrain->BeginUpdate();
    tlTrain->EndUpdate();
}
//---------------------------------------------------------------------------


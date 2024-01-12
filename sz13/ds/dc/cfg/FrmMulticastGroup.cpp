//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmMulticastGroup.h"

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
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma resource "*.dfm"
TFormMulticastGroupCfg *FormMulticastGroupCfg;

// 本form已经在工程中去除
static VECTOR<MULTICAST_GROUP_T>   g_vMulticastGroup;

//---------------------------------------------------------------------------
__fastcall TFormMulticastGroupCfg::TFormMulticastGroupCfg(TComponent* Owner)
    : TSubForm(Owner)
{
    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormMulticastGroupCfg::btnRefreshClick(TObject *Sender)
{
    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormMulticastGroupCfg::InitView()
{
    TcxTreeListNode     *Node = NULL;
    UINT32              ulTrainNum = Train::GetNormalTrainNum();
    UINT32              ulStationNum = Station::GetNormalStationNum();
    VECTOR<UINT32>      vUserID;
    UINT32              ulDCNum;

    DCUser::GetTrainDCUser(vUserID);
    ulDCNum = vUserID.size();

    tlMain->Clear();
    for (UINT32 i=0; i<g_vMulticastGroup.size(); i++)
    {
        MULTICAST_GROUP_T   &stGroup = g_vMulticastGroup[i];

        if (stGroup.i64GroupID == g_stGroupCfg.i64AllStationGroupID)
        {
            stGroup.ulTargetNum = ulStationNum + ulDCNum;
        }
        else
        {
            stGroup.ulTargetNum = 2*ulTrainNum + ulDCNum;    // 每列车2个车载台
        }

        Node = tlMain->Add();
        Node->Values[0] = stGroup.i64GroupID;
        Node->Values[1] = stGroup.szGroupName;
        Node->Values[2] = stGroup.ulTargetNum;
        Node->Values[3] = stGroup.vUserID.size();
        Node->Values[4] = "待检测";

        if (i == 0)
        {
            Node->Selected = true;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMulticastGroupCfg::FormCreate(TObject *Sender)
{
    SetImage(btnRefresh, IMG_REFRESH);
}
//---------------------------------------------------------------------------
void __fastcall TFormMulticastGroupCfg::tlMainFocusedNodeChanged(TcxCustomTreeList *Sender,
          TcxTreeListNode *APrevFocusedNode, TcxTreeListNode *AFocusedNode)
{
    if (!AFocusedNode || AFocusedNode->Values[0].IsNull())
    {
        return;
    }

    INT64   i64GroupID = AFocusedNode->Values[0];
    MULTICAST_GROUP_T   *pstGroup;

    for (UINT32 i=0; i<g_vMulticastGroup.size(); i++)
    {
        if (g_vMulticastGroup[i].i64GroupID == i64GroupID)
        {
            pstGroup = &g_vMulticastGroup[i];
            break;
        }
    }

    if (!pstGroup)
    {
        return;
    }

    TcxTreeListNode *Node;
    VECTOR<UINT32>  vExistedUserID;
    BOOL            bExist;

    eSDK_GetDynamicGroupMember(pstGroup->i64GroupID, vExistedUserID);

    tlUser->BeginUpdate();
    tlUser->Clear();

    for (UINT32 i=0; i<pstGroup->vUserID.size(); i++)
    {
        Node = tlUser->Add();
        Node->Values[0] = pstGroup->vUserID[i];

        bExist = FALSE;
        for (UINT32 j=0; j<vExistedUserID.size(); j++)
        {
            if (vExistedUserID[j] == pstGroup->vUserID[i])
            {
                Node->Values[1] = "";
                vExistedUserID.erase(vExistedUserID.begin()+j);
                bExist = TRUE;
                break;
            }
        }

        if (!bExist)
        {
            Node->Values[1] = "未加入";
        }
    }

    for (UINT32 j=0; j<vExistedUserID.size(); j++)
    {
        Node = tlUser->Add();
        Node->Values[0] = vExistedUserID[j];
        Node->Values[1] = "未知用户";
    }

    tlUser->EndUpdate();
}
//---------------------------------------------------------------------------


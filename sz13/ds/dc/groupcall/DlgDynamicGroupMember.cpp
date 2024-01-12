//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "UICfg.h"
#include "DlgDynamicGroupMember.h"

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
TDialogDynamicGroupMember *DialogDynamicGroupMember;

//---------------------------------------------------------------------------
__fastcall TDialogDynamicGroupMember::TDialogDynamicGroupMember(TComponent* Owner)
    : TDialogForm(Owner)
{
    memset(&m_stGroupInfo, 0, sizeof(m_stGroupInfo));
}

void __fastcall TDialogDynamicGroupMember::InitSkin()
{
    SetSkin(tlMain, true);
}
//---------------------------------------------------------------------------
void __fastcall TDialogDynamicGroupMember::btnCloseClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogDynamicGroupMember::Init(GROUP_INFO_T *pstGroup, VECTOR<UINT32> &vMember)
{
    memcpy(&m_stGroupInfo, pstGroup, sizeof(m_stGroupInfo));
    Caption = "动态组 - " + Group::GetGroupDisplayName(pstGroup->i64GroupID);

    m_vAddMember.clear();
    m_vDelMember.clear();
    m_vMember.assign(vMember.begin(), vMember.end());

    tlMain->Root->CheckGroupType = ncgCheckGroup;
    tlMain->OptionsView->CheckGroups = true;

    tlMain->BeginUpdate();
    tlMain->Clear();

    TcxTreeListNode     *Node;
    TcxTreeListNode     *SubNode;
    TcxTreeListNode     *UserNode;
    VECTOR<PORTAL_UE_INFO_T*>   vPortalUE;

    Node = tlMain->Add();
    Node->Values[0] = "调度台";
    Node->CheckGroupType = ncgCheckGroup;

    VECTOR<DC_USER_INFO_T*>     vUser;

    DCUser::GetDCUser(vUser);

    for (UINT32 i=0; i<vUser.size(); i++)
    {
        DC_USER_INFO_T  *pstInfo = vUser[i];

        UserNode = Node->AddChild();
        UserNode->Values[0] = pstInfo->ulUserID;
        UserNode->Values[1] = pstInfo->ulUserID;
        UserNode->Values[2] = DC_USER;
        UserNode->CheckGroupType = ncgCheckGroup;
    }

    VECTOR<STATION_INFO_T*> &vStationInfo = Station::GetStationInfo();

    Node = tlMain->Add();
    Node->Values[0] = "车站";
    Node->CheckGroupType = ncgCheckGroup;
    for (UINT32 i=0; i<vStationInfo.size(); i++)
    {
        SubNode = Node->AddChild();
        SubNode->Values[0] = AnsiString(vStationInfo[i]->ulStationID) + AnsiString(vStationInfo[i]->acStationName);
        SubNode->CheckGroupType = ncgCheckGroup;

        STATION_UE_INFO_T   *pstStationUE = StationUE::GetStationUEByStationID(vStationInfo[i]->ulStationID);

        if (pstStationUE)
        {
            UserNode = SubNode->AddChild();
            UserNode->Values[0] = pstStationUE->ulUserID;
            UserNode->Values[1] = pstStationUE->ulUserID;
            UserNode->Values[2] = STATION_UE;
            UserNode->CheckGroupType = ncgCheckGroup;
        }

        VECTOR<PORTAL_UE_INFO_T*>     vPortalUE;

        PortalUE::GetPortalUE(vStationInfo[i]->ulStationID, vPortalUE);

        for (UINT32 j=0; j<vPortalUE.size(); j++)
        {
            UserNode = SubNode->AddChild();
            UserNode->Values[0] = vPortalUE[j]->ulUserID;
            UserNode->Values[1] = vPortalUE[j]->ulUserID;
            UserNode->Values[2] = vPortalUE[j]->ulType;
            UserNode->CheckGroupType = ncgCheckGroup;
        }

        if (!SubNode->getFirstChild())
        {
            SubNode->Delete();
        }
    }

    VECTOR<TRAIN_INFO_T*>   vTrainInfo;
    VECTOR<UINT32>          vTrainUserID;

    Train::GetTrainInfo(vTrainInfo);

    Node = tlMain->Add();
    Node->Values[0] = "上行列车";
    Node->CheckGroupType = ncgCheckGroup;
    for (UINT32 i=0; i<vTrainInfo.size(); i++)
    {
        if (vTrainInfo[i]->ulDirection != TRAIN_DIRECTION_UPSIDE)
        {
            continue;
        }

        SubNode = Node->AddChild();
        SubNode->Values[0] = Train::GetTrainAlias(vTrainInfo[i]);  //vTrainInfo[i]->acTrainID;
        SubNode->CheckGroupType = ncgCheckGroup;

        Train::GetTrainUserID(vTrainInfo[i]->ulTrainUnitID, vTrainUserID);

        for (UINT32 j=0; j<vTrainUserID.size(); j++)
        {
            UserNode = SubNode->AddChild();
            UserNode->Values[0] = vTrainUserID[j];
            UserNode->Values[1] = vTrainUserID[j];
            UserNode->Values[2] = TRAIN_UE;
            UserNode->CheckGroupType = ncgCheckGroup;
        }

        if (!SubNode->getFirstChild())
        {
            SubNode->Delete();
        }
    }

    if (!Node->getFirstChild())
    {
        Node->Delete();
    }

    Node = tlMain->Add();
    Node->Values[0] = "下行列车";
    Node->CheckGroupType = ncgCheckGroup;
    for (UINT32 i=0; i<vTrainInfo.size(); i++)
    {
        if (vTrainInfo[i]->ulDirection != TRAIN_DIRECTION_DOWNSIDE)
        {
            continue;
        }

        SubNode = Node->AddChild();
        SubNode->Values[0] = Train::GetTrainAlias(vTrainInfo[i]);  //vTrainInfo[i]->acTrainID;
        SubNode->CheckGroupType = ncgCheckGroup;

        Train::GetTrainUserID(vTrainInfo[i]->ulTrainUnitID, vTrainUserID);

        for (UINT32 j=0; j<vTrainUserID.size(); j++)
        {
            UserNode = SubNode->AddChild();
            UserNode->Values[0] = vTrainUserID[j];
            UserNode->Values[1] = vTrainUserID[j];
            UserNode->Values[2] = TRAIN_UE;
            UserNode->CheckGroupType = ncgCheckGroup;
        }

        if (!SubNode->getFirstChild())
        {
            SubNode->Delete();
        }
    }

    if (!Node->getFirstChild())
    {
        Node->Delete();
    }

    tlMain->Root->Collapse(true);

    for (UINT32 j=0; j<vMember.size(); j++)
    {
        for (int i=0; i<tlMain->AbsoluteCount; i++)
        {
            Node = tlMain->AbsoluteItems[i];
            if (Node->Values[1].IsNull())
            {
                continue;
            }

            if ((UINT32)Node->Values[1] == vMember[j])
            {
                Node->Checked = true;

                for (TcxTreeListNode *ParentNode = Node->Parent;
                     ParentNode; ParentNode = ParentNode->Parent)
                {
                    ParentNode->Expand(true);
                    if (ParentNode->Parent == tlMain->Root)
                    {
                        break;
                    }
                }

                break;
            }
        }
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TDialogDynamicGroupMember::btnOKClick(TObject *Sender)
{
    UINT32              ulUserID;
    bool                bExist;
    TcxTreeListNode     *Node = tlMain->Root->getFirstChild();

    m_vAddMember.clear();
    m_vDelMember.clear();
    m_vDelMember.assign(m_vMember.begin(), m_vMember.end());

    tlMain->BeginUpdate();

    for (;Node; Node=Node->GetNext())
    {
        if (!Node->Checked ||
            Node->Values[1].IsNull())
        {
            continue;
        }

        ulUserID = (UINT32)Node->Values[1];
        bExist = false;
        for (UINT32 i=0; i<m_vDelMember.size(); i++)
        {
            if (m_vDelMember[i] == ulUserID)
            {
                bExist = true;
                m_vDelMember.erase(m_vDelMember.begin()+i);
                break;
            }
        }

        if (!bExist)
        {
            m_vAddMember.push_back(ulUserID);
        }
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogDynamicGroupMember::FormDeactivate(TObject *Sender)
{
    memset(&m_stGroupInfo, 0, sizeof(m_stGroupInfo));

    m_vMember.clear();
}
//---------------------------------------------------------------------------
void __fastcall TDialogDynamicGroupMember::GetMember(VECTOR<UINT32> &vAddMember, VECTOR<UINT32> &vDelMember)
{
    vAddMember.assign(m_vAddMember.begin(), m_vAddMember.end());
    vDelMember.assign(m_vDelMember.begin(), m_vDelMember.end());
}
void __fastcall TDialogDynamicGroupMember::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnClose, IMG_CLOSE);
}
//---------------------------------------------------------------------------

void __fastcall TDialogDynamicGroupMember::tlMainCustomDrawDataCell(TcxCustomTreeList *Sender,
          TcxCanvas *ACanvas, TcxTreeListEditCellViewInfo *AViewInfo,
          bool &ADone)
{
    if (AViewInfo->Node && AViewInfo->Node->Checked)
    {
        ACanvas->Canvas->Brush->Color = UICfg::GetTreeNodeCheckedColor();
    }
}
//---------------------------------------------------------------------------


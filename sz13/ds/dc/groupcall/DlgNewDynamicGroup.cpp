//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "DlgNewDynamicGroup.h"
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
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "dxBevel"
#pragma link "dxBevel"
#pragma resource "*.dfm"
TDialogNewDynamicGroup *DialogNewDynamicGroup;
//---------------------------------------------------------------------------
__fastcall TDialogNewDynamicGroup::TDialogNewDynamicGroup(TComponent* Owner)
    : TDialogForm(Owner)
{
    Init();
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewDynamicGroup::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

void __fastcall TDialogNewDynamicGroup::InitSkin()
{
    SetSkin(tlMain, true);
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewDynamicGroup::InitTree()
{
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

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewDynamicGroup::Init()
{
    edtGroupID->Text = "";
    edtName->Text = "";

    InitTree();
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewDynamicGroup::btnOKClick(TObject *Sender)
{
    AnsiString  strName = edtName->Text.Trim();

    if (!ToInt64(edtGroupID->Text, &m_i64GroupID) ||
        m_i64GroupID <= 0)
    {
        ErrMsg("组编号非法");
        edtGroupID->Focused();
        ModalResult = mrNone;
        return;
    }

    if (strName.Length() == 0)
    {
        ErrMsg("组名称不能为空");
        edtName->Focused();
        ModalResult = mrNone;
        return;
    }

    if (strName.Length() >= (int)sizeof(m_acGroupName))
    {
        ErrMsg("组名称长度不能大于%u", sizeof(m_acGroupName)-1);
        edtName->Focused();
        ModalResult = mrNone;
        return;
    }

    StrCpy(m_acGroupName, strName);

    UINT32              ulUserID;
    TcxTreeListNode     *Node = tlMain->Root->getFirstChild();

    m_vUser.clear();
    for (;Node; Node=Node->GetNext())
    {
        if (!Node->Checked ||
            Node->Values[1].IsNull())
        {
            continue;
        }

        ulUserID = (UINT32)Node->Values[1];
        m_vUser.push_back(ulUserID);
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewDynamicGroup::GetGroupInfo(INT64 *pi64GroupID, CHAR *szGroupName, VECTOR<UINT32> &vUser)
{
    *pi64GroupID = m_i64GroupID;
    strcpy(szGroupName, m_acGroupName);
    vUser.swap(m_vUser);
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewDynamicGroup::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "DlgSelectUser.h"
//#include "DlgGroupMember.h"

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
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinLilian"
#pragma link "dxSkinMcSkin"
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
TDialogSelectUser *DialogSelectUser;

extern TcxTreeListNode* ShowGroup(TcxTreeList *tlMain, TcxTreeListNode *Node, INT64 i64GroupID, AnsiString strName, UINT32 ulGroupType);
//---------------------------------------------------------------------------
__fastcall TDialogSelectUser::TDialogSelectUser(TComponent* Owner)
    : TDialogForm(Owner)
{
//  memset(&m_stGroupInfo, 0, sizeof(m_stGroupInfo));
}

void __fastcall TDialogSelectUser::InitSkin()
{
    SetSkin(tlMain, true);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSelectUser::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;

    m_vUser.clear();
}
//---------------------------------------------------------------------------
void __fastcall TDialogSelectUser::InitVideoUser(IntSet &sSelectedUser)
{
    tlMain->BeginUpdate();
    tlMain->Clear();

    TcxTreeListNode     *Node;
    TcxTreeListNode     *SubNode;
    TcxTreeListNode     *UserNode;
    VECTOR<PORTAL_UE_INFO_T*>   vPortalUE;

    Node = tlMain->Add();
    Node->Values[0] = "调度台";
    for (UINT32 i=0; i<g_vDCUserInfo.size(); i++)
    {
        DC_USER_INFO_T  &stInfo = g_vDCUserInfo[i];

        if (stInfo.ulUserID == g_ulCurrDCUserID)
        {
            continue;
        }

        SubNode = Node->AddChild();
        SubNode->Values[0] = stInfo.ulUserID;
        SubNode->Values[1] = stInfo.ulCallStatus;
        SubNode->Values[2] = DC_USER;

    /*  if (sSelectedUser.Has(stInfo.ulUserID))
        {
            SubNode->CheckGroupType = ncgNone;
        }
        else
        {
            SubNode->CheckGroupType = ncgCheckGroup;
        }   */
    }

    VECTOR<STATION_INFO_T*> &vStationInfo = GetStationInfo();

    for (UINT32 i=0; i<vStationInfo.size(); i++)
    {
        Node = tlMain->Add();
        Node->Values[0] = AnsiString(vStationInfo[i]->ulStationID) + AnsiString(vStationInfo[i]->acStationName);

        STATION_UE_INFO_T   *pstStationUE = GetStationUEByStationID(vStationInfo[i]->ulStationID);

        if (pstStationUE)
        {
            SubNode = Node->AddChild();
            SubNode->Values[0] = pstStationUE->ulUserID;
            SubNode->Values[1] = pstStationUE->ulCallStatus;
            SubNode->Values[2] = STATION_UE;

        /*  if (sSelectedUser.Has(pstStationUE->ulUserID))
            {
                SubNode->CheckGroupType = ncgNone;
            }
            else
            {
                SubNode->CheckGroupType = ncgCheckGroup;
            }      */
        }

        GetPortalUE(vStationInfo[i]->ulStationID, vPortalUE);

        for (UINT32 j=0; j<vPortalUE.size(); j++)
        {
            SubNode = Node->AddChild();
            SubNode->Values[0] = vPortalUE[j]->ulUserID;
            SubNode->Values[1] = vPortalUE[j]->ulCallStatus;
            SubNode->Values[2] = vPortalUE[j]->ulType;

      /*        if (sSelectedUser.Has(vPortalUE[j]->ulUserID))
            {
                SubNode->CheckGroupType = ncgNone;
            }
            else
            {
                SubNode->CheckGroupType = ncgCheckGroup;
            }         */
        }

        if (Node->getFirstChild() == NULL)
        {
            Node->Delete();
        }
    }

    VECTOR<TRAIN_INFO_T*>   vTrainInfo;
    VECTOR<UINT32>          vTrainUserID;

    GetTrainInfo(vTrainInfo);

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
        SubNode->Values[0] = GetTrainAlias(vTrainInfo[i]);//->acTrainName;
        SubNode->CheckGroupType = ncgCheckGroup;

        GetTrainUserID(vTrainInfo[i]->ulTrainUnitID, vTrainUserID);

        for (UINT32 j=0; j<vTrainUserID.size(); j++)
        {
            TRAIN_UE_INFO_T     *pstTrainUE = GetTrainUE(vTrainUserID[j]);

            if (!pstTrainUE)
            {
                continue;
            }

            UserNode = SubNode->AddChild();
            UserNode->Values[0] = vTrainUserID[j];
            UserNode->Values[1] = pstTrainUE->ulCallStatus;
            UserNode->Values[2] = TRAIN_UE;

            UserNode->CheckGroupType = ncgCheckGroup;
        }

        if (SubNode->getFirstChild() == NULL)
        {
            SubNode->Delete();
        }
    }

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
        SubNode->Values[0] = GetTrainAlias(vTrainInfo[i]);//->acTrainName;
        SubNode->CheckGroupType = ncgCheckGroup;

        GetTrainUserID(vTrainInfo[i]->ulTrainUnitID, vTrainUserID);

        for (UINT32 j=0; j<vTrainUserID.size(); j++)
        {
            TRAIN_UE_INFO_T     *pstTrainUE = GetTrainUE(vTrainUserID[j]);

            if (!pstTrainUE)
            {
                continue;
            }

            UserNode = SubNode->AddChild();
            UserNode->Values[0] = vTrainUserID[j];
            UserNode->Values[1] = pstTrainUE->ulCallStatus;
            UserNode->Values[2] = TRAIN_UE;
            UserNode->CheckGroupType = ncgCheckGroup;
        }

        if (SubNode->getFirstChild() == NULL)
        {
            SubNode->Delete();
        }
    }

    for (int i=0; i<tlMain->AbsoluteCount; i++)
    {
        Node = tlMain->AbsoluteItems[i];

        Node->CheckGroupType = ncgCheckGroup;

        if (!Node->Values[1].IsNull())
        {
            UINT32  ulUserID = Node->Values[0];

            if (sSelectedUser.Has(ulUserID))
            {
                Node->Checked = true;
            }
        }
    }

    tlMain->Root->Expand(true);
    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TDialogSelectUser::btnOKClick(TObject *Sender)
{
    ModalResult = mrNone;

    UINT32              ulUserID;
    TcxTreeListNode     *Node;

    m_vUser.clear();
    for (int i=0; i<tlMain->AbsoluteCount; i++)
    {
        Node = tlMain->AbsoluteItems[i];
        if (Node->Values[1].IsNull() ||
            !Node->Checked )
        {
            continue;
        }

        ulUserID = Node->Values[0];
        m_vUser.push_back(ulUserID);
    }

    if (m_vUser.size() == 0)
    {
        return;
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TDialogSelectUser::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSelectUser::tlMainGetNodeImageIndex(TcxCustomTreeList *Sender,
          TcxTreeListNode *ANode, TcxTreeListImageIndexType AIndexType,
          TImageIndex &AIndex)
{
    if (ANode->Values[1].IsNull())
    {
        AIndex = 0;
        return;
    }

    if (ANode->Values[2].IsNull())
    {
        AIndex = 0;
        return;
    }

    BOOL    bOnline = IsUserOnline((UINT32)ANode->Values[1]);
    UINT32  ulType = (UINT32)ANode->Values[2];

    if (ulType == LARGE_PORTAL_UE )
    {
        AIndex = bOnline?1:2;
    }
    else if (ulType == SMALL_PORTAL_UE )
    {
        AIndex = bOnline?3:4;
    }
    else if (ulType == STATION_UE)
    {
        AIndex = bOnline?5:6;
    }
    else if (ulType == TRAIN_UE)
    {
        AIndex = bOnline?7:8;
    }
    else if (ulType == DC_USER)
    {
        AIndex = bOnline?9:10;
    }
}
//---------------------------------------------------------------------------


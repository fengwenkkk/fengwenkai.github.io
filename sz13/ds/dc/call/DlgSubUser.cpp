//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "DlgSubUser.h"
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
TDialogSubUser *DialogSubUser;

//---------------------------------------------------------------------------
__fastcall TDialogSubUser::TDialogSubUser(TComponent* Owner)
    : TDialogForm(Owner)
{
}

void __fastcall TDialogSubUser::InitSkin()
{
    SetSkin(tlMain, true);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSubUser::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSubUser::Init(IntSet &sUser, BOOL bVideoUserOnly)
{
    IntSet  sHideUesr;

    Init(sUser, sHideUesr, bVideoUserOnly);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSubUser::Init(IntSet &sUser, IntSet &sHideUser, BOOL bVideoUserOnly)
{
    tlMain->BeginUpdate();
    tlMain->Clear();

    tlMain->Root->CheckGroupType = ncgCheckGroup;
    tlMain->OptionsView->CheckGroups = true;

    TcxTreeListNode     *Node;
    TcxTreeListNode     *SubNode;
    TcxTreeListNode     *UserNode;
    UINT32              i;
    UINT32              ulUserType;
    UINT32              ulFuncType;
    UINT32              ulStatus;
    UINT32              ulCellID;
    CHAR                acUserName[64];

    // 车站终端，列车终端，维护终端，管理终端、其他
    Node = tlMain->Add();
    Node->Values[0] = "车站终端";
    if (!bVideoUserOnly)
    {
        VECTOR<STATION_UE_INFO_T*>  vStationUE = StationUE::GetStationUE();

        for (i=0; i<vStationUE.size(); i++)
        {
            STATION_UE_INFO_T   *pstInfo = vStationUE[i];

            SubNode = Node->AddChild();
            SubNode->Values[0] = pstInfo->ulUserID;
            SubNode->Values[1] = pstInfo->ulCallStatus;
            SubNode->Values[2] = STATION_UE;
        }
    }

    VECTOR<PORTAL_UE_INFO_T*>   vPortalUE = PortalUE::GetPortalUE();

    for (i=0; i<vPortalUE.size(); i++)
    {
        PORTAL_UE_INFO_T    *pstInfo = vPortalUE[i];

        if (pstInfo->ulFuncType == UE_STATION )
        {
            SubNode = Node->AddChild();
            SubNode->Values[0] = pstInfo->ulUserID;
            SubNode->Values[1] = pstInfo->ulCallStatus;
            SubNode->Values[2] = pstInfo->ulType;
        }
    }

    if (!bVideoUserOnly)
    {
        Node = tlMain->Add();
        Node->Values[0] = "列车终端";

        VECTOR<TRAIN_UE_INFO_T*>    vTrainUE = TrainUE::GetTrainUE();

        for (i=0; i<vTrainUE.size(); i++)
        {
            TRAIN_UE_INFO_T     *pstInfo = vTrainUE[i];

            SubNode = Node->AddChild();
            SubNode->Values[0] = pstInfo->ulUserID;
            SubNode->Values[1] = pstInfo->ulCallStatus;
            SubNode->Values[2] = TRAIN_UE;
        }
    }

    Node = tlMain->Add();
    Node->Values[0] = "维护终端";
    for (i=0; i<vPortalUE.size(); i++)
    {
        PORTAL_UE_INFO_T    *pstInfo = vPortalUE[i];

        if (pstInfo->ulFuncType == UE_FIX)
        {
            SubNode = Node->AddChild();
            SubNode->Values[0] = pstInfo->ulUserID;
            SubNode->Values[1] = pstInfo->ulCallStatus;
            SubNode->Values[2] = pstInfo->ulType;
        }
    }

    VECTOR<DC_USER_INFO_T*>    vDCUserInfo;

    DCUser::GetDCUser(vDCUserInfo);

    Node = tlMain->Add();
    Node->Values[0] = "管理终端";
    if (!bVideoUserOnly)
    {
        for (UINT32 i=0; i<vDCUserInfo.size(); i++)
        {
            DC_USER_INFO_T  *pstInfo = vDCUserInfo[i];

            if (pstInfo->ulUserID == APP::GetCurrDCUserID())
            {
                continue;
            }

            SubNode = Node->AddChild();
            SubNode->Values[0] = pstInfo->ulUserID;
            SubNode->Values[1] = pstInfo->ulCallStatus;
            SubNode->Values[2] = DC_USER;
        }
    }

    for (i=0; i<vPortalUE.size(); i++)
    {
        PORTAL_UE_INFO_T    *pstInfo = vPortalUE[i];

        if (pstInfo->ulFuncType == UE_MANAGE)
        {
            SubNode = Node->AddChild();
            SubNode->Values[0] = pstInfo->ulUserID;
            SubNode->Values[1] = pstInfo->ulCallStatus;
            SubNode->Values[2] = pstInfo->ulType;
        }
    }

    Node = tlMain->Add();
    Node->Values[0] = "其他";
    for (i=0; i<vPortalUE.size(); i++)
    {
        PORTAL_UE_INFO_T    *pstInfo = vPortalUE[i];

        if (pstInfo->ulFuncType != UE_FIX &&
            pstInfo->ulFuncType != UE_MANAGE &&
            pstInfo->ulFuncType != UE_STATION)
        {
            SubNode = Node->AddChild();
            SubNode->Values[0] = pstInfo->ulUserID;
            SubNode->Values[1] = pstInfo->ulCallStatus;
            SubNode->Values[2] = pstInfo->ulType;
        }
    }

    tlMain->Root->Expand(true);

    for (int i=tlMain->AbsoluteCount-1; i>=0; i--)
    {
        Node = tlMain->AbsoluteItems[i];
        Node->CheckGroupType = ncgCheckGroup;
    }

    for (int i=tlMain->AbsoluteCount-1; i>=0; i--)
    {
        Node = tlMain->AbsoluteItems[i];
        if (!Node->Values[1].IsNull())
        {
            UINT32  ulUserID = Node->Values[0];

            if (sHideUser.Has(ulUserID))
            {
                Node->Delete();
            }
            else if (sUser.Has(ulUserID))
            {
                Node->Checked = true;
            }
        }
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TDialogSubUser::Get(IntSet &sUser)
{
    UINT32              ulUserID;
    TcxTreeListNode     *Node;

    sUser.Clear();
    tlMain->Root->Expand(true);
    for (int i=0; i<tlMain->AbsoluteCount; i++)
    {
        Node = tlMain->AbsoluteItems[i];
        if (Node->Values[1].IsNull() ||
            !Node->Checked )
        {
            continue;
        }

        ulUserID = Node->Values[0];
        sUser.Add(ulUserID);
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogSubUser::btnOKClick(TObject *Sender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSubUser::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSubUser::tlMainGetNodeImageIndex(TcxCustomTreeList *Sender,
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

    BOOL    bOnline = User::IsUserOnline((UINT32)ANode->Values[1]);
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
void __fastcall TDialogSubUser::btnFilterClick(TObject *Sender)
{
    AnsiString          strFilterUser = edtUser->Text.Trim();
    AnsiString          strUserID;
    TcxTreeListNode     *Node;
    TcxTreeListNode     *ParentNode;

    tlMain->BeginUpdate();
    for (int i=0; i<tlMain->AbsoluteCount; i++)
    {
        Node = tlMain->AbsoluteItems[i];

        if (!Node->Values[0].IsNull() )
        {
            strUserID = Node->Values[0];

            if (strFilterUser.IsEmpty() ||
                strUserID.Pos(strFilterUser) > 0)
            {
                for (ParentNode=Node; ParentNode; ParentNode=ParentNode->Parent)
                {
                    ParentNode->Visible = true;
                }
            }
            else
            {
                Node->Visible = false;
            }
        }
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------


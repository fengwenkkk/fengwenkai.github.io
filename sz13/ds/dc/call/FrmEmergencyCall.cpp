//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmEmergencyCall.h"
#include "main.h"
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
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma resource "*.dfm"

TFormEmergencyCall *FormEmergencyCall;
//---------------------------------------------------------------------------
__fastcall TFormEmergencyCall::TFormEmergencyCall(TComponent* Owner)
    : TSubForm(Owner)
{
}
//---------------------------------------------------------------------------
INT64 __fastcall TFormEmergencyCall::GetSelectCall()
{
    TcxTreeListNode *Node = NULL;

    if (tlMain->Count == 1)
    {
        Node = tlMain->Root->getFirstChild();
    }
    else if (tlMain->SelectionCount > 0)
    {
        Node = tlMain->Selections[0];
    }

    if (!Node || Node->Values[2].IsNull())
    {
        return INVALID_GROUP_ID;
    }

    INT64   i64GroupID = (INT64)Node->Values[2];

    return i64GroupID;
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCall::btnPTTDialMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y)
{
    m_i64GroupID = GetSelectCall();
    if (m_i64GroupID == INVALID_GROUP_ID)
    {
        return;
    }

    if (!eSDK_PTTDial(m_i64GroupID))
    {
        InfoMsg("È¡µÃ»°È¨Ê§°Ü!");
        return;
    }

    btnPTTDial->LookAndFeel->SkinName = UICfg::GetPTTDownSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCall::btnPTTDialMouseEnter(TObject *Sender)
{
    btnPTTDial->LookAndFeel->SkinName = UICfg::GetPTTSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCall::btnPTTDialMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    btnPTTDial->LookAndFeel->SkinName = UICfg::GetPTTSkinName();

    if (m_i64GroupID == INVALID_GROUP_ID)
    {
        return;
    }

    PTT::Release(m_i64GroupID);
}
//---------------------------------------------------------------------------
TcxTreeListNode* __fastcall TFormEmergencyCall::GetEmergencyCallNode(INT64 i64GroupID)
{
    TcxTreeListNode *Node = tlMain->Root->getFirstChild();

    for(; Node; Node=Node->getNextSibling())
    {
        INT64   iCurrGroupID = (INT64)Node->Values[2];

        if (i64GroupID == iCurrGroupID)
        {
            return Node;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCall::NewEmergencyCall(INT64 i64GroupID, AnsiString strSpeaker)
{
    TcxTreeListNode     *Node = GetEmergencyCallNode(i64GroupID);

    if (Node)
    {
        return;
    }

    Node = tlMain->Add();

    Node->Values[0] = GetTimeText();
    Node->Values[1] = Group::GetGroupName(i64GroupID);
    Node->Values[2] = i64GroupID;
    Node->Values[3] = strSpeaker;

    if (tlMain->SelectionCount == 0)
    {
        Node->Selected = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCall::CloseEmergencyCall(INT64 i64GroupID)
{
    TcxTreeListNode     *Node = GetEmergencyCallNode(i64GroupID);

    if (!Node)
    {
        return;
    }

    Node->Delete();
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCall::UpdateEmergencyCallSpeaker(INT64 i64GroupID, AnsiString strSpeaker)
{
    TcxTreeListNode     *Node = GetEmergencyCallNode(i64GroupID);

    if (!Node)
    {
        return;
    }

    Node->Values[3] = strSpeaker;
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCall::UpdateEmergencyCallIdle(INT64 i64GroupID)
{
    TcxTreeListNode     *Node = GetEmergencyCallNode(i64GroupID);

    if (!Node)
    {
        return;
    }

    Node->Values[3] = "";
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCall::tlMainFocusedNodeChanged(TcxCustomTreeList *Sender,
          TcxTreeListNode *APrevFocusedNode, TcxTreeListNode *AFocusedNode)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormEmergencyCall::btnExitClick(TObject *Sender)
{
    INT64   i64GroupID = GetSelectCall();

    if (i64GroupID == INVALID_GROUP_ID)
    {
        return;
    }

    if (!ExitGroup(i64GroupID))
    {
        InfoMsg("ÍË³ö½ô¼±ºô½ÐÊ§°Ü!");
        return;
    }

    m_i64GroupID = INVALID_GROUP_ID;

    TcxTreeListNode     *Node = GetEmergencyCallNode(i64GroupID);

    if (!Node)
    {
        return;
    }

    Node->Delete();
}
//---------------------------------------------------------------------------


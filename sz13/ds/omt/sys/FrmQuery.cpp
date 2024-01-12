//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"

#include "MsgDialog.h"
#include "msg.h"
#include "Resource.h"

#include "FrmQuery.h"
#include "FrmCDR.h"
#include "FrmLocalRecManage.h"
#include "FrmLog.h"
#include "FrmIPHCallInfo.h"
#include "FrmRecBrdInfo.h"
#include "FrmReqCallInfo.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxControls"
#pragma link "cxCustomData"
#pragma link "cxDropDownEdit"
#pragma link "cxGraphics"
#pragma link "cxInplaceContainer"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxStyles"
#pragma link "cxTextEdit"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "dxSkinBlue"
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinLilian"
#pragma link "dxSkinMcSkin"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinOffice2013DarkGray"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "cxContainer"
#pragma link "cxEdit"
#pragma link "cxGroupBox"
#pragma link "cxButtons"
#pragma link "cxPC"
#pragma link "dxSkinscxPCPainter"
#pragma link "cxRadioGroup"
#pragma link "cxCheckBox"
#pragma link "cxImageComboBox"
#pragma link "cxMemo"
#pragma link "cxLabel"
#pragma link "cxTrackBar"
#pragma link "cxMaskEdit"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxBarBuiltInMenu"
#pragma link "cxClasses"
#pragma link "dxBar"
#pragma resource "*.dfm"

TFormQuery *FormQuery;


void __fastcall TFormQuery::InitWorkPanel()
{
    TcxButton   *btns[] = { btnCDR,
                            btnReqCall,
                            btnRecBrd,
                            btnIPH,
                            btnSDS,
                            btnRec,
                            btnLog,
                          };

    for (UINT32 i=0; i<ARRAY_SIZE(btns); i++)
    {
        btns[i]->TabOrder = i;
        btns[i]->Top = 50+95*i;
        btns[i]->Width = g_iMidToolbarButtonWidth;
        btns[i]->Height = g_iMidToolbarButtonHeight;
        btns[i]->Left = (btns[i]->Parent->Width - btns[i]->Width)/2;
    }
}

void __fastcall TFormQuery::InitSkin()
{
    TcxButton   *btns[] = { btnCDR,
                            btnReqCall,
                            btnRecBrd,
                            btnIPH,
                            btnSDS,
                            btnRec,
                            btnLog,
                          };

    for (UINT32 i=0; i<ARRAY_SIZE(btns); i++)
    {
        btns[i]->LookAndFeel->SkinName = g_strRightToolbarButtonSkinName;
    }
}
//---------------------------------------------------------------------------
__fastcall TFormQuery::TFormQuery(TComponent* Owner)
    : TSubForm(Owner)
{
    InitWorkPanel();
}
//---------------------------------------------------------------------------
void __fastcall TFormQuery::ShowForm(TForm *Form)
{
    if (Form->Parent != gbMain)
    {
        Form->Parent = gbMain;
        Form->BorderStyle = bsNone;
        Form->Align = alClient;
        Form->Visible = true;
    }

    Form->BringToFront();
}
//---------------------------------------------------------------------------
void __fastcall TFormQuery::TimerInitTimer(TObject *Sender)
{
    TimerInit->Enabled = false;
    btnCDRClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormQuery::btnCDRClick(TObject *Sender)
{
    ShowForm(FormCDR);
}
//---------------------------------------------------------------------------
void __fastcall TFormQuery::btnSDSClick(TObject *Sender)
{
  //    ShowForm(FormLatestSDS);
    ShowForm(FormSDSManage);
}
//---------------------------------------------------------------------------
void __fastcall TFormQuery::btnRecClick(TObject *Sender)
{
    ShowForm(FormLocalRecManage);
}
//---------------------------------------------------------------------------
void __fastcall TFormQuery::btnLogClick(TObject *Sender)
{
    ShowForm(FormLog);
}
//---------------------------------------------------------------------------
void __fastcall TFormQuery::btnRecBrdClick(TObject *Sender)
{
    ShowForm(FormRecBrdInfo);
}
//---------------------------------------------------------------------------
void __fastcall TFormQuery::btnIPHClick(TObject *Sender)
{
    ShowForm(FormIPHCallInfo);
}
//---------------------------------------------------------------------------
void __fastcall TFormQuery::btnReqCallClick(TObject *Sender)
{
    ShowForm(FormReqCallInfo);
}
//---------------------------------------------------------------------------


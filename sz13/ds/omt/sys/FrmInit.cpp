//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmInit.h"
#include "FrmLogin.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxGraphics"

#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxEdit"
#pragma link "cxTextEdit"
#pragma link "cxMaskEdit"
#pragma link "dxSkinsForm"
#pragma link "dxGDIPlusClasses"
#pragma link "cxLabel"
#pragma link "cxProgressBar"
#pragma link "cxGroupBox"
#pragma link "cxScrollBox"
#pragma resource "*.dfm"

TFormInit *FormInit;

static UINT32   g_ulInitIndex = 0;
static UINT32   g_ulInitNum = 30;

//---------------------------------------------------------------------
__fastcall TFormInit::TFormInit(TComponent* AOwner)
    : TForm(AOwner)
{
    ModalResult = mrNone;

/*  Width = FormLogin->Width;
    Height = FormLogin->Height;

    FormLogin->PanelMain->Parent = PanelMain;
    FormLogin->pbInit->Visible = true; */

    Height = 36;//FormLogin->pbInit->Height;
    Width = 200;

    FormLogin->pbInit->Parent = sbMain;
    FormLogin->pbInit->Align = alClient;


    FormLogin->pbInit->Align = alNone;
    FormLogin->pbInit->Left = -1;
    FormLogin->pbInit->Top = -1;
    FormLogin->pbInit->Height = Height+2;
    FormLogin->pbInit->Width = Width+2;

    FormLogin->pbInit->Visible = true;
    FormLogin->pbInit->BringToFront();
}

//void SetInitInfoEx(AnsiString strInitInfo)
//{
//  if (FormInit)
//    {
//      g_ulInitIndex++;
//        if (g_ulInitNum < g_ulInitIndex)
//      {
//            g_ulInitNum = g_ulInitIndex;
//        }
//
//      FormInit->SetInitProgress(g_ulInitIndex, g_ulInitNum, strInitInfo);
//  }
//}
//
//void SetInitInfo(AnsiString strInitInfo)
//{
//  SetInitInfoEx("正在初始化" + strInitInfo);
//}
//
//void __fastcall TFormInit::SetInitProgress(UINT32 ulPos, UINT32 ulMaxPos, AnsiString strInitInfo)
//{
//  pbInit->Properties->Max = ulMaxPos;
//  pbInit->Position = ulPos;
//
//    pbInit->Properties->Text = strInitInfo;
//  pbInit->Invalidate();
//  pbInit->Update();
//}
//
//void SetInitOver()
//{
//  if (FormLogin)
//  {
//      FormInit->SetInitProgress(g_ulInitIndex, g_ulInitIndex, "初始化成功");
//  }
//
//  SaveConfInfo("conf", "max_init_item", g_ulInitIndex);
//}


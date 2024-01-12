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

bool GetMaxInitNum()
{
    return LoadConfInfo("conf", "max_init_item", (INT32*)&g_ulInitNum);
}

void SaveMaxInitNum()
{
    SaveConfInfo("conf", "max_init_item", g_ulInitNum);
}

void SetInitOver()
{
    if (FormInit)
    {
        FormInit->SetInitProgress(g_ulInitIndex, g_ulInitIndex, "初始化成功");
    }

    SaveConfInfo("conf", "max_init_item", g_ulInitIndex);
}

void SetInitInfoEx(AnsiString strInitInfo)
{
    if (FormInit)
    {
        g_ulInitIndex++;
        if (g_ulInitNum < g_ulInitIndex)
        {
            g_ulInitNum = g_ulInitIndex;
        }

        FormInit->SetInitProgress(g_ulInitIndex, g_ulInitNum, strInitInfo);
    }
}

void SetInitInfo(AnsiString strInitInfo)
{
    SetInitInfoEx("正在初始化" + strInitInfo);
}


//---------------------------------------------------------------------
__fastcall TFormInit::TFormInit(TComponent* AOwner)
    : TForm(AOwner)
{
    ModalResult = mrNone;
    Height = 36;
    Width = 200;

    pbInit->Properties->Max = g_ulInitNum;
    pbInit->Position = 0;
    pbInit->Properties->Text = "正在初始化";

    pbInit->Align = alNone;
    pbInit->Left = -1;
    pbInit->Top = -1;
    pbInit->Height = Height+2;
    pbInit->Width = Width+2;

}

void __fastcall TFormInit::SetInitProgress(UINT32 ulPos, UINT32 ulMaxPos, AnsiString strInitInfo)
{
    pbInit->Properties->Max = ulMaxPos;
    pbInit->Position = ulPos;

    pbInit->Properties->Text = strInitInfo;
    pbInit->Invalidate();
    pbInit->Update();
}


//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FrmPlayAndRec.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "dxBevel"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"

#pragma link "dxSkinMoneyTwins"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma resource "*.dfm"
TFramePlayAndRec *FramePlayAndRec;


TFramePlayAndRec* NewFramePlayAndRec(TWinControl *ParentCtrl)
{
    TFramePlayAndRec    *Frame = new TFramePlayAndRec(ParentCtrl);

    Frame->Parent = ParentCtrl;
    Frame->Align = alClient;

    return Frame;
}

//---------------------------------------------------------------------------
__fastcall TFramePlayAndRec::TFramePlayAndRec(TComponent* Owner)
    : TFrame(Owner)
{
    PanelVideo->Left = 1;
    PanelVideo->Top = 1;
    PanelVideo->Anchors << akRight;
    PanelVideo->Anchors << akBottom;
}
//---------------------------------------------------------------------------
__fastcall TFramePlayAndRec::~TFramePlayAndRec()
{
    if (m_Thread)
    {
        m_Thread->Free();
    }
}
//---------------------------------------------------------------------------
bool __fastcall TFramePlayAndRec::Start(char *szUrl, char *szFile, bool bTcpMode)
{
    Image->Visible = false;
    gbVideo->Visible = true;

    m_Thread = new ThreadPlayAndRec(gbVideo->Handle, bTcpMode);

    m_Thread->SetURL(szUrl);

    m_Thread->Play();

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TFramePlayAndRec::Stop()
{
    if (m_Thread)
    {
        m_Thread->Stop();
    }
}
void __fastcall TFramePlayAndRec::GrabPic()
{
    if (!m_Thread)
    {
        return;
    }

    m_Thread->Stop();

    TRect  Rect = TRect(0,0, gbVideo->Width, gbVideo->Height);
  //  PanelVideo->
#if 0
    Graphics::TBitmap   *Bitmap = new Graphics::TBitmap;

    Bitmap->Width  = gbVideo->Width;
    Bitmap->Height = gbVideo->Height;
  //    Bitmap->Assign(Image1->Picture->Graphic);

    Bitmap->Canvas->CopyRect(Rect, gbVideo->Canvas->Canvas, Rect);
    Bitmap->SaveToFile("D:\\test.bmp");

    delete Bitmap;

    Image->Picture->LoadFromFile("D:\\test.bmp");
#else
    //Image->Assign(Bitmap->Graphic);
    Image->Canvas->CopyRect(Rect, gbVideo->Canvas->Canvas, Rect);
#endif

    Image->Visible = true;
    gbVideo->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TFramePlayAndRec::FrameResize(TObject *Sender)
{
    PanelVideo->Width = PanelVideo->Parent->Width-2;
    PanelVideo->Height = PanelVideo->Parent->Height-2;
}
//---------------------------------------------------------------------------


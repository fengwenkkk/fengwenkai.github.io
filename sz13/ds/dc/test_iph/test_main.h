//---------------------------------------------------------------------------

#ifndef test_mainH
#define test_mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:    // IDE-managed Components
    TButton *btnSend;
    TButton *btnStop;
    TTimer *TimerSend;
    void __fastcall btnSendClick(TObject *Sender);
    void __fastcall btnStopClick(TObject *Sender);
    void __fastcall TimerSendTimer(TObject *Sender);

private:    // User declarations
    void __fastcall SendPCMData();

public:     // User declarations
    __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif

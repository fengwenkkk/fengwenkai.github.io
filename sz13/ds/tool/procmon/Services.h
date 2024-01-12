//---------------------------------------------------------------------------
#ifndef ServicesH
#define ServicesH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <SvcMgr.hpp>
#include <vcl.h>
//---------------------------------------------------------------------------
class TServiceMon : public TService
{
__published:    // IDE-managed Components
    void __fastcall ServiceStart(TService *Sender, bool &Started);
    void __fastcall ServiceStop(TService *Sender, bool &Stopped);
private:        // User declarations
public:         // User declarations
    __fastcall TServiceMon(TComponent* Owner);
    TServiceController __fastcall GetServiceController(void);

    friend void __stdcall ServiceController(unsigned CtrlCode);
};
//---------------------------------------------------------------------------
extern PACKAGE TServiceMon *ServiceMon;
//---------------------------------------------------------------------------
#endif

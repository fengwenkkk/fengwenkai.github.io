//---------------------------------------------------------------------------
#include "Services.h"

#include "g_include.h"
#include "GPublic.h"
#include "procfunc.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TServiceMon *ServiceMon;

extern VOID StartMonitorThread();
extern VOID StopMonitor();

//---------------------------------------------------------------------------
__fastcall TServiceMon::TServiceMon(TComponent* Owner)
    : TService(Owner)
{
}

TServiceController __fastcall TServiceMon::GetServiceController(void)
{
    return (TServiceController) ServiceController;
}

void __stdcall ServiceController(unsigned CtrlCode)
{
    ServiceMon->Controller(CtrlCode);
}
//---------------------------------------------------------------------------
void __fastcall TServiceMon::ServiceStart(TService *Sender, bool &Started)
{
    GosLog(LOG_FATAL, "start mon ... ");
    StartMonitorThread();
}
//---------------------------------------------------------------------------
void __fastcall TServiceMon::ServiceStop(TService *Sender, bool &Stopped)
{
    GosLog(LOG_FATAL, "stop mon ... ");
    StopMonitor();
}
//---------------------------------------------------------------------------


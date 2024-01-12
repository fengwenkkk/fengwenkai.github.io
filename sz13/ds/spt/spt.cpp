//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "vcl.h"

USEFORM("rec\FrmRecManage.cpp", FormRecManage);
USEFORM("rec\DlgExportRecFile.cpp", DialogExportRecFile);
USEFORM("main.cpp", FormMain);
USEFORM("sys\FrmInit.cpp", FormInit);
USEFORM("sys\FrmLogin.cpp", FormLogin);
USEFORM("sys\DlgSetSkin.cpp", DialogSetSkin);
USEFORM("sys\DlgExit.cpp", DialogExit);
USEFORM("sys\DlgLock.cpp", DialogLock);
USEFORM("sys\DlgSetPwd.cpp", DialogSetPwd);
//---------------------------------------------------------------------------
extern WINAPI ClientMain();

//---------------------------------------------------------------------------

WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    return ClientMain();
}


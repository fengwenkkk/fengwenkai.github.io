//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"
#include "MsgDialog.h"
#include "GCommon.h"
#include "MDIForm.h"
#include "main.h"

//---------------------------------------------------------------------------
USEFORM("main.cpp", FormMain);
//---------------------------------------------------------------------------
extern AnsiString g_strSkinName;

//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    g_strSkinName = "Office2010Blue";

    try
    {
        Application->Initialize();
        Application->MainFormOnTaskBar = false;
        Application->CreateForm(__classid(TFormMain), &FormMain);
        Application->Run();

    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    catch (...)
    {
        try
        {
            throw Exception("");
        }
        catch (Exception &exception)
        {
            Application->ShowException(&exception);
        }
    }

//  SaveLocalCfg();

    return 0;
}
//---------------------------------------------------------------------------

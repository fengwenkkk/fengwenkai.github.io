//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"

#include "GCommon.h"
#include "MDIForm.h"

//---------------------------------------------------------------------------
USEFORM("main.cpp", FormMain);
//---------------------------------------------------------------------------
extern AnsiString g_strSkinName;

//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
   // g_strSkinName = "Office2016Colorful";

    if (gos_is_proc_running())
    {
        return -1;
    }

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

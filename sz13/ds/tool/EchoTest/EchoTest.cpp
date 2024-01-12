//---------------------------------------------------------------------------
#include "g_include.h"
#include <vcl.h>
#include "ds_public.h"
#include "msg.h"
#include "main.h"


#pragma hdrstop

//---------------------------------------------------------------------------
USEFORM("main.cpp", FormPIS);
//---------------------------------------------------------------------------
extern AnsiString g_strSkinName;

//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    gos_time_init();
    g_strSkinName = "Lilian";

    GosInitLog();

    try
    {
        Application->Initialize();
//      Application->MainFormOnTaskBar = true;
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

    exit(0);

    return 0;
}
//---------------------------------------------------------------------------

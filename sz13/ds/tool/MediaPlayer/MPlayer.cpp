//---------------------------------------------------------------------------

#include <vcl.h>
#include "MDIForm.h"


#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
#include <Vcl.Styles.hpp>
#include <Vcl.Themes.hpp>
USEFORM("main.cpp", FormMain);
//---------------------------------------------------------------------------
extern BOOL SystemInit();

extern AnsiString  g_strSkinName;

//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    g_strSkinName = "Lilian";

    try
    {
        GosInitLog();
        Application->Initialize();
        Application->MainFormOnTaskBar = false;
        TStyleManager::TrySetStyle("Iceberg Classico");
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
    return 0;
}
//---------------------------------------------------------------------------

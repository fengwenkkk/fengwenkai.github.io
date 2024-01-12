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

//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    SetDefaultSkinName("Lilian");

    if (!SystemInit())
    {
        String  strErr = "³õÊ¼»¯Ê§°Ü";
        Application->MessageBoxA(NULL, strErr.c_str());
        return -1;
    }

    try
    {
        Application->Initialize();
//      Application->MainFormOnTaskBar = true;
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

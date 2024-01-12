//---------------------------------------------------------------------------
#include "g_include.h"
#include <vcl.h>
#include "ds_public.h"
#include "pis.h"
#include "msg.h"
#include "ThreadIPH.h"
#include "ThreadPcmPlay.h"
#include "PAUDPServerThread.h"
#include "main.h"


#pragma hdrstop

//---------------------------------------------------------------------------
USEFORM("main.cpp", FormPIS);
//---------------------------------------------------------------------------
extern AnsiString g_strSkinName;
extern ThreadIPH *g_pThreadIPH;
extern ThreadPcmPlay  *g_ThreadPcmPlay;
extern PAUDPThread     *g_ThreadPAUDPPlay;
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
        Application->CreateForm(__classid(TFormPIS), &FormPIS);
         g_pThreadIPH = new ThreadIPH(FormPIS->Handle, DC_RECV_IPH_PORT);

        g_ThreadPAUDPPlay = new PAUDPThread(FormPIS->Handle);
        if (g_ThreadPAUDPPlay != NULL)
        {
            g_ThreadPAUDPPlay->Init();
        }

        //g_pThreadIPH->Start();
        //g_pThreadIPH->StartPlay(1, 1, 1);

        g_ThreadPcmPlay = new ThreadPcmPlay();
        g_ThreadPcmPlay->Start();

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

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "MDIForm.h"

#include "main.h"
#include "FrmLogin.h"
#include "FrmInit.h"

//---------------------------------------------------------------------------
#include <Vcl.Styles.hpp>
#include <Vcl.Themes.hpp>

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
USEFORM("FrmHome.cpp", FormHome);
USEFORM("devlist\FrmDevList.cpp", FormDevList);
USEFORM("devlist\FrmDevInfo.cpp", FormDevInfo);
USEFORM("cfg\FrmOperLog.cpp", FormOperLog);
USEFORM("cfg\FrmNeInfo.cpp", FormNeInfo);
USEFORM("cfg\FrmMediaPlayer.cpp", FormMediaPlayer);
USEFORM("cfg\FrmTextReader.cpp", FormTextReader);
USEFORM("cfg\FrmUserCfg.cpp", FormUserCfg);
USEFORM("main.cpp", FormMain);
USEFORM("sys\FrmInit.cpp", FormInit);
USEFORM("sys\DlgSetSkin.cpp", DialogSetSkin);
USEFORM("sys\DlgLock.cpp", DialogLock);
USEFORM("sys\FrmLog.cpp", FormLog);
USEFORM("topo\FrmTopo.cpp", FormTopo);
USEFORM("sys\FrmSystemManagement.cpp", FormSystem);
USEFORM("sys\FrmLogin.cpp", FormLogin);
USEFORM("sys\DlgExit.cpp", DialogExit);
USEFORM("perf\FrmPerf.cpp", FormPerf);
USEFORM("sw\DlgSetSw.cpp", DialogSetSw);
USEFORM("sw\FrmVersion.cpp", FormVersion);
USEFORM("sw\FrmSwUpdate.cpp", FormSwUpdate);
USEFORM("sw\FrmSwCfg.cpp", FormSwCfg);
USEFORM("..\client_if\ffmpeg\FrmPlayAndRec.cpp", FramePlayAndRec); /* TFrame: File Type */
USEFORM("cfg\DlgImportFile.cpp", DialogImportFile);
USEFORM("alarm\FrmHistoryAlarm.cpp", FormHistoryAlarm);
USEFORM("alarm\FrmAlarmTrend.cpp", FormAlarmTrend);
USEFORM("cfg\DlgSetNe.cpp", DialogSetNe);
USEFORM("cfg\FrmDevFile.cpp", FormDevFile);
USEFORM("cfg\FrmCfg.cpp", FormCfg);
USEFORM("cfg\DlgSetOmtOperator.cpp", DialogSetOmtOperator);
USEFORM("alarm\FrmAlarmStat.cpp", FormAlarmStat);
USEFORM("alarm\DlgSetAlarmLevel.cpp", DialogSetAlarmLevel);
USEFORM("alarm\FrmActiveAlarm.cpp", FormActiveAlarm);
USEFORM("alarm\FrmAlarmReport.cpp", FormAlarmReport);
USEFORM("alarm\FrmAlarmCfg.cpp", FormAlarmCfg);
USEFORM("alarm\FrmAlarm.cpp", FormAlarm);
//---------------------------------------------------------------------------
extern WINAPI ClientMain();

extern AnsiString GetAppSkinName();
extern void SetTreeListSkinName(AnsiString strSkinName);

WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    AnsiString  strSkinName = GetAppSkinName();

    SetTreeListSkinName(strSkinName);

    return ClientMain();
}


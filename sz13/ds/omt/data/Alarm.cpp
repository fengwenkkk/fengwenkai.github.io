//---------------------------------------------------------------------------

#pragma hdrstop

#include "ClientInclude.h"
#include "Alarm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

static VectorMap<AnsiString,    ALARM_INFO_T>        s_mActiveAlarm;
static VectorMap<AnsiString,    ALARM_INFO_T>        s_mHistoryAlarm;
static VECTOR<ALARM_LEVEL_CFG_T>                     s_vAlarmLevelCfg;
static std::map<UINT32, UINT32>                      s_mDevAlarmCount;
namespace Alarm
{
void InitActiveAlarm(VectorMap<AnsiString, ALARM_INFO_T> &mActiveAlarm)
{
    s_mActiveAlarm.Clear();
	s_mActiveAlarm.AddAll(mActiveAlarm);
}

void GetActiveAlarm(VectorMap<AnsiString, ALARM_INFO_T> &mActiveAlarm)
{
   mActiveAlarm.Clear();
   mActiveAlarm.AddAll(s_mActiveAlarm);
}

void InitAlarmLevelCfg(VECTOR<ALARM_LEVEL_CFG_T>    &vAlarmCfg)
{
	s_vAlarmLevelCfg.clear();
	s_vAlarmLevelCfg.assign(vAlarmCfg.begin(), vAlarmCfg.end());
}

void GetAlarmLevelCfg(VECTOR<ALARM_LEVEL_CFG_T>     &vAlarmCfg)
{
    vAlarmCfg.clear();
	vAlarmCfg.assign(s_vAlarmLevelCfg.begin(), s_vAlarmLevelCfg.end());
}

void InitHistoryAlarm(VectorMap<AnsiString, ALARM_INFO_T>   &mHistoryAlarm)
{
     s_mHistoryAlarm.Clear();
     s_mHistoryAlarm.AddAll(mHistoryAlarm);
}

void GetHistoryAlarm(VectorMap<AnsiString, ALARM_INFO_T>    &mHistoryAlarm)
{
    mHistoryAlarm.Clear();
    mHistoryAlarm.AddAll(s_mHistoryAlarm);
}

void InitDevAlarmCount(std::map<UINT32, UINT32> &mDevAlarmCount)
{
    s_mDevAlarmCount.clear();
    s_mDevAlarmCount =  mDevAlarmCount;
}

void GetDevAlarmCount(std::map<UINT32, UINT32> &mDevAlarmCount)
{
    mDevAlarmCount.clear();
    mDevAlarmCount =  s_mDevAlarmCount;
}
}

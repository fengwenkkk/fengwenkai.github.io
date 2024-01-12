//---------------------------------------------------------------------------

#ifndef AlarmH
#define AlarmH
//---------------------------------------------------------------------------
#endif

namespace Alarm
{
// ActiveAlarmInfo
void InitActiveAlarm(VectorMap<AnsiString, ALARM_INFO_T> &mActiveAlarm);
void GetActiveAlarm(VectorMap<AnsiString, ALARM_INFO_T> &mActiveAlarm);
// AlarmLevelCfg
void InitAlarmLevelCfg(VECTOR<ALARM_LEVEL_CFG_T>    &vAlarmCfg);
void GetAlarmLevelCfg(VECTOR<ALARM_LEVEL_CFG_T>     &vAlarmCfg);
// History Alarm
void InitHistoryAlarm(VectorMap<AnsiString, ALARM_INFO_T>   &mHistoryAlarm);
void GetHistoryAlarm(VectorMap<AnsiString, ALARM_INFO_T>    &mHistoryAlarm);
// 不同类型设备活跃告警统计
void InitDevAlarmCount(std::map<UINT32, UINT32> &mDevAlarmCount);
void GetDevAlarmCount(std::map<UINT32, UINT32> &mDevAlarmCount);
}

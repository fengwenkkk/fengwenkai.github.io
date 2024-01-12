#ifndef UICfgH
#define UICfgH

extern const int    g_iMidToolbarButtonWidth;
extern const int    g_iMidToolbarButtonHeight;
extern const UINT32 g_ulTrainBroadcastGroupPriority;
extern const UINT32 g_ulTrainBroadcastMaxPeriod;
extern const UINT32 g_ulTrainDefaultGroupPriority;
extern const UINT32 g_ulTrainDefaultGroupMaxPeriod;
extern const UINT32 g_ulDynamicGroupPriority;
extern const UINT32 g_ulDynamicGroupMaxPeriod;


namespace UICfg
{

AnsiString GetToolbarFontName();
AnsiString GetStatusbarFontName();

AnsiString GetP2PSkinName();
AnsiString GetPTTDownSkinName();
AnsiString GetPTTSkinName();
AnsiString GetEmergencyCallSkinName();

AnsiString GetVolumeSkinName();
AnsiString GetStatusbarButtonSkinName();
AnsiString GetToolbarButtonSkinName();
AnsiString GetLeftToolbarButtonSkinName();
AnsiString GetRightToolbarButtonSkinName();
AnsiString GetToolbarButtonEnterSkinName();

AnsiString GetGroupCallWinSkinName();
AnsiString GetTrainSkinName();
AnsiString GetActiveTrainSkinName();

AnsiString GetStationSkinName();
AnsiString GetActiveStationSkinName();
AnsiString GetInterLockStationSkinName();

AnsiString GetActiveVideoWinSkinName();
AnsiString GetDefaultVideoWinSkinName();

int GetGroupBoxFontSize();
int GetButtonPTTFontSize();
AnsiString GetButtonPTTFontName();

TColor GetTreeNodeCheckedColor();
TColor GetBackgroundColor();
TColor GetLeftBackgroundColor();
TColor GetBrdFailColor();
TColor GetOfflineDevColor();
TColor GetCallingColor();

}  // namespace UICfg

#endif

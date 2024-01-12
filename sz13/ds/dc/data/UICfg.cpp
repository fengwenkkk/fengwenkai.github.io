#include "ClientInclude.h"

const int       g_iMidToolbarButtonWidth = 108;
const int       g_iMidToolbarButtonHeight = 72;
const UINT32    g_ulTrainBroadcastGroupPriority = 2;
const UINT32    g_ulTrainBroadcastMaxPeriod = 120;
const UINT32    g_ulTrainDefaultGroupPriority = 4;
const UINT32    g_ulTrainDefaultGroupMaxPeriod = 3*60;
const UINT32    g_ulDynamicGroupPriority = 4;
const UINT32    g_ulDynamicGroupMaxPeriod = 3*60;

namespace UICfg
{

static AnsiString g_strToolbarFontName = "Î¢ÈíÑÅºÚ";
static AnsiString g_strStatusbarFontName = "ËÎÌå";

static AnsiString g_strP2PSkinName = "Sharp";
static AnsiString g_strPTTDownSkinName = "GlassOceans";
static AnsiString g_strPTTSkinName = "MoneyTwins";//VisualStudio2013Light";
static AnsiString g_strEmergencyCallSkinName = "Office2007Pink";

static AnsiString g_strVolumeSkinName = "Office2010Silver";
static AnsiString g_strStatusbarButtonSkinName = "DevExpressDarkStyle";
static AnsiString g_strToolbarButtonSkinName = "GlassOceans";
static AnsiString g_strLeftToolbarButtonSkinName = "GlassOceans";
static AnsiString g_strRightToolbarButtonSkinName = "Office2013DarkGray";
static AnsiString g_strToolbarButtonEnterSkinName = "Blueprint";

static AnsiString g_strGroupCallWinSkinName = "Office2010Blue";
static AnsiString g_strTrainSkinName = "Office2010Blue";
static AnsiString g_strActiveTrainSkinName = "VisualStudio2013Light";

static AnsiString g_strStationSkinName = "Office2010Blue";
static AnsiString g_strActiveStationSkinName = "VisualStudio2013Light";
static AnsiString g_strInterLockStationSkinName = "Office2010Blue";//"VisualStudio2013Blue";

static AnsiString g_strActiveVideoWinSkinName = "Office2010Blue";
static AnsiString g_strDefaultVideoWinSkinName = "Office2010Silver";

static int        g_iGroupBoxFontSize = 16;
static int        g_iButtonPTTFontSize = 32;
static AnsiString g_strButtonPTTFontName = "Arial";

AnsiString GetToolbarFontName           (){return g_strToolbarFontName            ;}
AnsiString GetStatusbarFontName         (){return g_strStatusbarFontName          ;}

AnsiString GetP2PSkinName               (){return g_strP2PSkinName                ;}
AnsiString GetPTTDownSkinName           (){return g_strPTTDownSkinName            ;}
AnsiString GetPTTSkinName               (){return g_strPTTSkinName                ;}
AnsiString GetEmergencyCallSkinName     (){return g_strEmergencyCallSkinName      ;}

AnsiString GetVolumeSkinName            (){return g_strVolumeSkinName             ;}
AnsiString GetStatusbarButtonSkinName   (){return g_strStatusbarButtonSkinName    ;}
AnsiString GetToolbarButtonSkinName     (){return g_strToolbarButtonSkinName      ;}
AnsiString GetLeftToolbarButtonSkinName (){return g_strLeftToolbarButtonSkinName  ;}
AnsiString GetRightToolbarButtonSkinName(){return g_strRightToolbarButtonSkinName ;}
AnsiString GetToolbarButtonEnterSkinName(){return g_strToolbarButtonEnterSkinName ;}

AnsiString GetGroupCallWinSkinName      (){return g_strGroupCallWinSkinName       ;}
AnsiString GetTrainSkinName             (){return g_strTrainSkinName              ;}
AnsiString GetActiveTrainSkinName       (){return g_strActiveTrainSkinName        ;}

AnsiString GetStationSkinName           (){return g_strStationSkinName            ;}
AnsiString GetActiveStationSkinName     (){return g_strActiveStationSkinName      ;}
AnsiString GetInterLockStationSkinName  (){return g_strInterLockStationSkinName   ;}

AnsiString GetActiveVideoWinSkinName    (){return g_strActiveVideoWinSkinName     ;}
AnsiString GetDefaultVideoWinSkinName   (){return g_strDefaultVideoWinSkinName    ;}

int        GetGroupBoxFontSize          (){return g_iGroupBoxFontSize             ;}
int        GetButtonPTTFontSize         (){return g_iButtonPTTFontSize            ;}
AnsiString GetButtonPTTFontName         (){return g_strButtonPTTFontName          ;}

static TColor   g_ColorTreeNodeChecked  = clSkyBlue;//clCream;//clWebDeepPink;
static TColor   g_ColorBackground       = (TColor)RGB(80,100,136); //RGB(80,100,136);//102,145,196);  68 90 126
static TColor   g_ColorLeftBackground   = (TColor)RGB(90,123,165);//119,140,172); //RGB(80,100,136);//102,145,196);  68 90 126
static TColor   g_ColorBrdFail          = clRed;
static TColor   g_ColorOfflineDev       = clGray;
static TColor   g_ColorCalling          = clGreen;

TColor GetTreeNodeCheckedColor  (){return g_ColorTreeNodeChecked    ;}
TColor GetBackgroundColor       (){return g_ColorBackground         ;}
TColor GetLeftBackgroundColor   (){return g_ColorLeftBackground     ;}
TColor GetBrdFailColor          (){return g_ColorBrdFail            ;}
TColor GetOfflineDevColor       (){return g_ColorOfflineDev         ;}
TColor GetCallingColor          (){return g_ColorCalling            ;}

} // namespace UICfg

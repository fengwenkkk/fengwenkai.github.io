//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"
#include "MsgDialog.h"
#include "GCommon.h"
#include "ClientUtil.h"

#include "ThreadBuilder.h"
#include "main.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxControls"
#pragma link "cxGraphics"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxPC"
#pragma link "dxSkinBlack"
#pragma link "dxSkinBlue"
#pragma link "dxSkinCaramel"
#pragma link "dxSkinCoffee"
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinFoggy"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkiniMaginary"
#pragma link "dxSkinLilian"
#pragma link "dxSkinLiquidSky"
#pragma link "dxSkinLondonLiquidSky"
#pragma link "dxSkinMcSkin"
#pragma link "dxSkinMoneyTwins"
#pragma link "dxSkinOffice2007Black"
#pragma link "dxSkinOffice2007Blue"
#pragma link "dxSkinOffice2007Green"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinOffice2007Silver"
#pragma link "dxSkinOffice2010Black"
#pragma link "dxSkinOffice2010Blue"
#pragma link "dxSkinOffice2010Silver"
#pragma link "dxSkinPumpkin"
#pragma link "dxSkinsCore"
#pragma link "dxSkinscxPCPainter"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinsdxStatusBarPainter"
#pragma link "dxSkinSeven"
#pragma link "dxSkinSharp"
#pragma link "dxSkinSilver"
#pragma link "dxSkinSpringTime"
#pragma link "dxSkinStardust"
#pragma link "dxSkinSummer2008"
#pragma link "dxSkinValentine"
#pragma link "dxSkinXmas2008Blue"
#pragma link "dxStatusBar"
#pragma link "dxTabbedMDI"
#pragma link "cxClasses"
#pragma link "cxCustomData"
#pragma link "cxData"
#pragma link "cxDataStorage"
#pragma link "cxDBData"
#pragma link "cxEdit"
#pragma link "cxFilter"
//#pragma link "cxGrid"
#pragma link "cxStyles"
#pragma link "dxRibbon"
#pragma link "dxRibbonSkins"
#pragma link "dxSkinsForm"
#pragma link "dxLayoutContainer"
#pragma link "dxLayoutControl"
#pragma link "dxLayoutLookAndFeels"
#pragma link "cxInplaceContainer"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "cxDBTL"
#pragma link "cxTLData"
#pragma link "cxMaskEdit"
#pragma link "cxContainer"
#pragma link "cxTreeView"
#pragma link "cxTextEdit"
#pragma link "cxButtons"
#pragma link "dxBar"
#pragma link "dxBarExtItems"
#pragma link "dxSkinsdxBarPainter"
#pragma link "dxNavBar"
#pragma link "dxNavBarBase"
#pragma link "dxNavBarCollns"
#pragma link "dxSkinsdxNavBarPainter"
#pragma link "dxNavBarStyles"
#pragma link "cxButtonEdit"
#pragma link "cxCalc"
#pragma link "cxHyperLinkEdit"
#pragma link "cxListView"
#pragma link "cxShellCommon"
#pragma link "cxShellTreeView"
#pragma link "cxBarEditItem"
#pragma link "cxCheckBox"
#pragma link "cxSplitter"
#pragma link "cxDropDownEdit"
#pragma link "cxLabel"
#pragma link "cxGroupBox"
#pragma link "cxPropertiesStore"
#pragma link "cxProgressBar"
#pragma link "cxTrackBar"
#pragma link "cxMemo"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinDevExpressStyle"
#pragma link "dxSkinHighContrast"
#pragma link "dxSkinMetropolis"
#pragma link "dxSkinMetropolisDark"
#pragma link "dxSkinOffice2013DarkGray"
#pragma link "dxSkinOffice2013LightGray"
#pragma link "dxSkinOffice2013White"
#pragma link "dxSkinOffice2016Colorful"
#pragma link "dxSkinOffice2016Dark"
#pragma link "dxSkinSevenClassic"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinTheAsphaltWorld"
#pragma link "dxSkinTheBezier"
#pragma link "dxSkinVisualStudio2013Blue"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxSkinVisualStudio2013Light"
#pragma link "dxSkinVS2010"
#pragma link "dxSkinWhiteprint"
#pragma link "dxTaskbarProgress"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxDrawTextUtils"
#pragma link "dxBkgnd"
#pragma link "dxPrnDev"
#pragma link "dxPrnPg"
#pragma link "dxPSCompsProvider"
#pragma link "dxPSCore"
#pragma link "dxPScxEditorProducers"
#pragma link "dxPScxExtEditorProducers"
#pragma link "dxPScxPageControlProducer"
#pragma link "dxPSEdgePatterns"
#pragma link "dxPSEngn"
#pragma link "dxPSFillPatterns"
#pragma link "dxPSGlbl"
#pragma link "dxPSPDFExport"
#pragma link "dxPSPDFExportCore"
#pragma link "dxPSPrVwAdv"
#pragma link "dxPSPrVwRibbon"
#pragma link "dxPSPrVwStd"
#pragma link "dxPSUtl"
#pragma link "dxWrap"
#pragma resource "*.dfm"
TFormMain *FormMain;

#define MAX_THREAD_NUM      32

#if __BORLANDC__ > 0x0700
char *g_aszSkinName[] = {
    "Black",
    "Blue",
    "Blueprint",
    "Caramel",
    "Coffee",
    "DarkRoom",
    "DarkSide",
    "DevExpressDarkStyle",
    "DevExpressStyle",
    "Foggy",
//  "GlassOceans",
//  "HighContrast",
    "iMaginary",
    "Lilian",
    "LiquidSky",
    "LondonLiquidSky",
    "McSkin",
    "Metropolis",
    "MetropolisDark",
    "MoneyTwins",
    "Office2007Black",
    "Office2007Blue",
    "Office2007Green",
//  "Office2007Pink",
    "Office2007Silver",
    "Office2010Black",
    "Office2010Blue",
    "Office2010Silver",
    "Office2013DarkGray",
    "Office2013LightGray",
    "Office2013White",
    "Office2016Colorful",
    "Office2016Dark",
//  "Pumpkin",
//  "Seven",
//  "SevenClassic",
    "Sharp",
    "SharpPlus",
    "Silver",
//  "SpringTime",
    "Stardust",
//  "Summer2008",
    "TheAsphaltWorld",
    "TheBezier",
//  "Valentine",
    "VisualStudio2013Blue",
    "VisualStudio2013Dark",
    "VisualStudio2013Light",
    "VS2010",
    "Whiteprint",
//  "Xmas2008Blue",
} ;
#else
char *g_aszSkinName[] = {
    "UserSkin",
    "Black",
    "Blue",
    "Caramel",
    "Coffee",
    "DarkRoom",
    "DarkSide",
    "Foggy",
    "GlassOceans",
    "iMaginary",
    "Lilian",
    "LiquidSky",
    "LondonLiquidSky",
    "McSkin",
    "MoneyTwins",
    "Office2007Black",
    "Office2007Blue",
    "Office2007Green",
    "Office2007Pink",
    "Office2007Silver",
    "Office2010Black",
    "Office2010Blue",
    "Office2010Silver",
    "Pumpkin",
    "Seven",
    "Sharp",
    "Silver",
    "SpringTime",
    "Stardust",
    "Summer2008",
    "Valentine",
    "Xmas2008Blue",
};
#endif


UINT32          g_ulThreadNum = 0;
ThreadBuilder  *g_apThread[MAX_THREAD_NUM];
BOOL            g_bThreadRunning[MAX_THREAD_NUM];
BOOL            g_bBuildering = FALSE;

typedef struct
{
    AnsiString      strFile;
    UINT32          ulState;
    AnsiString      strResult;
    TcxTreeListNode *Node;
    int             iColIndex;
}BUILD_FILE_INFO_T;

//std::map<AnsiString, TcxTreeListNode*>      g_mBuilderInfo;
std::vector<BUILD_FILE_INFO_T>        g_vBuildFileInfo;

HANDLE      g_hMainHandle = NULL;

bool        g_bRebuild = false;

#define BS_IDLE         0
#define BS_BUILDING     1
#define BS_SUCC         10
#define BS_FAIL         11
#define BS_MISSINGFILE  12      // 文件不存在
#define BS_COMPILED     13      // obj文件时间大于源文件时间（不需要编译）


typedef struct
{
    AnsiString      strBccExe;
    AnsiString      strRootDir;
    AnsiString      strDefine;
    AnsiString      strSysIncPath;
    AnsiString      strSysLibPath;

    AnsiString      strTmsPath;
    AnsiString      strDevPath;
    AnsiString      strRaizePath;
    AnsiString      strTeeIncPath;
    AnsiString      strTeeObjPath;
    AnsiString      strTeeLibPath;
}BDS_SYS_INFO_T;

extern void WaitCommand(CHAR *szCmd, BOOL bShow);

BUILD_FILE_INFO_T* GetBuildFileInfo(AnsiString strFile)
{
    for (UINT32 i=0; i<g_vBuildFileInfo.size(); i++)
    {
        BUILD_FILE_INFO_T   *pstFile = &g_vBuildFileInfo[i];

        if (pstFile->strFile == strFile)
        {
            return pstFile;
        }
    }

    return NULL;
}

UINT32 GetCpuNum()
{
    SYSTEM_INFO     sysInfo = {0};

    GetSystemInfo(&sysInfo);

    return sysInfo.dwNumberOfProcessors;
}

UINT32 GetCpuCoreNum()
{
    return GetCpuNum();

    char    acResultFile[256];
    char    acCmdFile[256];
    FILE    *fp;
    char    acBuf[4*1024];
    char    *szTmp = NULL;
    UINT32  ulNum = 0;
    int     iSize;
    int     i;

    sprintf(acResultFile, "%s/cpu.info", gos_get_root_path());
    sprintf(acCmdFile, "%s/temp.bat", gos_get_root_path());

    fp = fopen(acCmdFile, "w+t");
    if (!fp)
    {
        goto fail;
    }

    fprintf(fp, "WMIC CPU Get NumberOfLogicalProcessors /Format:List > %s", acResultFile);
    fclose(fp);

    WaitCommand(acCmdFile, false);

    iSize = gos_get_file_size(acResultFile);
    if (iSize <= 0)
    {
        goto fail;
    }

    fp = fopen(acResultFile, "rb");
    if (!fp)
    {
        goto fail;
    }

    memset(acBuf, 0, sizeof(acBuf));
    fread(acBuf, 1, sizeof(acBuf)-1, fp);
    fclose(fp);

    memset(acBuf, ' ', 2);


    // 0D 00 0A 00 0D 00 0A 00
    for (i=1; i<iSize/2; i++)
    {
        acBuf[i] = acBuf[2*i];
    }

    acBuf[i] = '\0';

    szTmp = gos_trim_string(acBuf);

    szTmp = strstr(szTmp, "NumberOfLogicalProcessors");
    if (!szTmp)
    {
        goto fail;
    }

    szTmp += strlen("NumberOfLogicalProcessors");
    szTmp = strchr(szTmp, '=');
    if (!szTmp)
    {
        goto fail;
    }

    szTmp++;
    szTmp = gos_trim_string(szTmp);
    if (!gos_atou(szTmp, &ulNum) || ulNum == 0)
    {
        goto fail;
    }

    return ulNum;

fail:
    return GetCpuNum();
}

bool ReadRegValue(TRegistry *Reg, char *szKey, char *szValue, AnsiString &strValue)
{
    if (!Reg->ValueExists(szValue))
    {
        ErrMsg("读注册表值失败：Key=%s, 名称=%s", szKey, szValue);
        return false;
    }

    strValue = Reg->ReadString(szValue);

    return true;
}

bool ReplaceStr(AnsiString &strSrc, AnsiString strFromStr, AnsiString strToStr)
{
    bool    bExist = false;

    while(1)
    {
        int     iPos = strSrc.Pos(strFromStr);

        if (iPos <= 0)
        {
            break;
        }

        bExist = true;
        strSrc = strSrc.SubString(1, iPos-1) + strToStr + strSrc.SubString(iPos+strFromStr.Length(), strSrc.Length());
    }

    return bExist;
}

bool ReplaceStr(AnsiString &strSrc, const char *szFromStr, const char *szToStr)
{
    return ::ReplaceStr(strSrc, (AnsiString)szFromStr, (AnsiString)szToStr);
}

bool ReplaceStr(AnsiString &strSrc, const char *szFromStr, AnsiString strToStr)
{
    return ::ReplaceStr(strSrc, (AnsiString)szFromStr, strToStr);
}

AnsiString ConvertPath(AnsiString strPath)
{
    if (strPath.Pos(" ") > 0)
    {
        return "\"" + strPath + "\"";
    }

    return strPath;
}

void FormatPath(AnsiString &strPath)
{
    AnsiString      strNewPath = "";
    AnsiString      strTmpPath;

    strPath += ";";
    while(1)
    {
        int iPos = strPath.Pos(";");

        if (iPos <= 0)
        {
            break;
        }

        strTmpPath = strPath.SubString(1, iPos-1);
        if (strTmpPath.Length() > 0 && strTmpPath[strTmpPath.Length()] == '\\' )
        {
            strTmpPath = strTmpPath.SubString(1, strTmpPath.Length()-1);
        }

        strPath =  strPath.SubString(iPos+1, strPath.Length());

        strNewPath += ConvertPath(strTmpPath) + ";";
    }

    strPath = strNewPath.SubString(1, strNewPath.Length()-1);
}

#if 1
bool GetBDSSysInfo(BDS_SYS_INFO_T &stSysInfo)
{
    TRegistry   *Reg = new TRegistry;
    char        *szKey;
    AnsiString  strName;
    bool        bRet = false;

    try
    {
        Reg->RootKey = HKEY_CURRENT_USER;

//      szKey = "Software\\CodeGear\\BDS\\7.0";
        szKey = "Software\\Embarcadero\\BDS\\20.0";
        if (!Reg->OpenKey(szKey, false))
        {
            ErrMsg("打开注册表Key失败：%s", szKey);
            goto end;
        }

//        stSysInfo.strAppName = Reg->ReadString("App");
        stSysInfo.strRootDir = Reg->ReadString("RootDir");
        if (stSysInfo.strRootDir.Length() > 0 && stSysInfo.strRootDir[stSysInfo.strRootDir.Length()] == '\\')
        {
            stSysInfo.strRootDir = stSysInfo.strRootDir.SubString(1, stSysInfo.strRootDir.Length()-1);
        }
        stSysInfo.strBccExe = stSysInfo.strRootDir + "\\bin\\bcc32.exe";

        Reg->CloseKey();

        szKey = "Software\\Embarcadero\\BDS\\20.0\\Environment Variables";
        if (!Reg->OpenKey(szKey, false))
        {
            ErrMsg("打开注册表Key失败：%s", szKey);
            goto end;
        }

        if (!ReadRegValue(Reg, szKey, "TMS", stSysInfo.strTmsPath) ||
            !ReadRegValue(Reg, szKey, "DXVCL", stSysInfo.strDevPath) ||
//            !ReadRegValue(Reg, szKey, "RAZPATH", stSysInfo.strRaizePath) ||
            !ReadRegValue(Reg, szKey, "TEEINCPATH", stSysInfo.strTeeIncPath) ||
//            !ReadRegValue(Reg, szKey, "TEEOBJPATH", stSysInfo.strTeeObjPath) ||
            !ReadRegValue(Reg, szKey, "TEELIBPATH", stSysInfo.strTeeLibPath) )
        {
            goto end;
        }

        Reg->CloseKey();

   /*     // HKEY_CURRENT_USER\Software\CodeGear\BDS\7.0\C++\Paths
        szKey = "Software\\CodeGear\\BDS\\7.0\\C++\\Paths";
        if (!Reg->OpenKey(szKey, false))
        {
            ErrMsg("打开注册表Key失败：%s", szKey);
            goto end;
        }

        stSysInfo.strDefine = "_DEBUG";

        if (!ReadRegValue(Reg, szKey, "IncludePath", stSysInfo.strSysIncPath) ||
            !ReadRegValue(Reg, szKey, "LibraryPath", stSysInfo.strSysLibPath) )
        {
            goto end;
        }

        ReplaceStr(stSysInfo.strSysIncPath, "$(BDS)", stSysInfo.strRootDir);
        ReplaceStr(stSysInfo.strSysLibPath, "$(BDS)", stSysInfo.strRootDir);

        Reg->CloseKey();     */

        bRet = true;
    }
    __finally
    {
end:
        if (Reg)
        {
            delete Reg;
        }
    }

    return bRet;
}
#endif

bool GetXmlValue(char *szText, char *szKey, AnsiString &strValue)
{
    char    acKeyStart[256];
    char    acKeyEnd[256];
    char    *szStart;
    char    *szEnd;

    strValue = "";

    sprintf(acKeyStart, "<%s>", szKey);
    sprintf(acKeyEnd, "</%s>", szKey);

    szStart = strstr(szText, acKeyStart);
    if (!szStart)
    {
        return false;
    }

    szStart += strlen(acKeyStart);
    szEnd = strstr(szStart, acKeyEnd);
    if (!szEnd)
    {
        return false;
    }

    int iLen = szEnd - szStart;

    strValue = AnsiString(szStart, iLen);
    return true;
}

// -D_DEBUG;NO_STRICT;_BCB_;_NMS_CLIENT;_UNICODE_
bool GetProjectParam(char *szProjFile, AnsiString &strDefine, AnsiString &strIncPath, AnsiString &strLibPath, AnsiString &strObjPath)
{
    int     iSize = gos_get_file_size(szProjFile);
    char    *szBuf;
    FILE    *fp;
    AnsiString  strTmp;

    if (iSize <= 0)
    {
        return false;
    }

    szBuf = (char*)calloc(1, iSize+1);
    if (!szBuf)
    {
        return false;
    }

    fp = fopen(szProjFile, "rb");
    if (!fp)
    {
        free(szBuf);
        return false;
    }

    if (iSize != fread(szBuf, 1, iSize, fp))
    {
        free(szBuf);
        fclose(fp);
        return false;
    }

    fclose(fp);

    bool    bRet = false;
    char    *szKey = strstr(szBuf, "<PropertyGroup Condition=\"'$(Base)'!=''\">");
    char    *szEnd;

    if (!szKey)
    {
        goto end;
    }

    szKey += strlen("<PropertyGroup Condition=\"'$(Base)'!=''\">");
    szEnd = strstr(szKey, "</PropertyGroup>");
    if (!szEnd)
    {
        goto end;
    }

 //   *szEnd = '\0';

    if (!GetXmlValue(szKey, "Defines", strDefine) ||
        !GetXmlValue(szKey, "IncludePath", strIncPath) ||
        !GetXmlValue(szKey, "ILINK_LibraryPath", strLibPath) ||
        !GetXmlValue(szKey, "IntermediateOutputDir", strObjPath) )
    {
        goto end;
    }

    // get debug obj path
    szKey = strstr(szBuf, "<PropertyGroup Condition=\"'$(Cfg_1)'!=''\">");
    if (szKey && GetXmlValue(szKey, "IntermediateOutputDir", strTmp))
    {
        strObjPath = strTmp;
    }

    bRet = true;

end:
    free(szBuf);
    return bRet;
    // <PropertyGroup Condition="'$(Base)'!=''">
    // <Defines>NO_STRICT;_BCB_;_NMS_CLIENT;_UNICODE_;$(Defines)</Defines>
}

AnsiString GetIncPath(char *szProjFile)
{
    int     iSize = gos_get_file_size(szProjFile);
    char    *szBuf;
    FILE    *fp;
    AnsiString  strTmp;
    AnsiString  strIncPath = "";

    if (iSize <= 0)
    {
        return "";
    }

    szBuf = (char*)calloc(1, iSize+1);
    if (!szBuf)
    {
        return "";
    }

    fp = fopen(szProjFile, "rb");
    if (!fp)
    {
        free(szBuf);
        return "";
    }

    if (iSize != fread(szBuf, 1, iSize, fp))
    {
        free(szBuf);
        fclose(fp);
        return "";
    }

    fclose(fp);

    bool    bRet = false;
    char    *szKey = strstr(szBuf, "<PropertyGroup Condition=\"'$(Base)'!=''\">");
    char    *szEnd;

    if (!szKey)
    {
        goto end;
    }

    szKey += strlen("<PropertyGroup Condition=\"'$(Base)'!=''\">");
    szEnd = strstr(szKey, "</PropertyGroup>");
    if (!szEnd)
    {
        goto end;
    }

 //   *szEnd = '\0';

    if (!GetXmlValue(szKey, "IncludePath", strIncPath) )
    {
        goto end;
    }

    // <BCC_IncludePath>../dc_public;../dtp;$(BCC_IncludePath)</BCC_IncludePath>
    szKey = strstr(szBuf, "<PropertyGroup Condition=\"'$(Base_Win32)'!=''\">");
    if (szKey && GetXmlValue(szKey, "BCC_IncludePath", strTmp))
    {
        if (!strIncPath.IsEmpty())
        {
            strIncPath += ";";
        }

//      RemoveStr(strTmp, "$(BCC_IncludePath)");

        strIncPath += strTmp;
    }

    // get debug obj path
    szKey = strstr(szBuf, "<PropertyGroup Condition=\"'$(Cfg_1_Win32)'!=''\">");
    if (szKey && GetXmlValue(szKey, "IncludePath", strTmp))
    {
        if (!strIncPath.IsEmpty())
        {
            strIncPath += ";";
        }

//      RemoveStr(strTmp, "$(IncludePath)");
        strIncPath += strTmp;
    }

    bRet = true;

end:
    free(szBuf);

    return bRet?strIncPath:AnsiString("");
}

#if 0
AnsiString LoadBuilderCmd(char *szProjFile)
{
    // HKEY_CURRENT_USER\Software\CodeGear\BDS\7.0\Environment Variables

    // HKEY_CURRENT_USER\Software\CodeGear\BDS\7.0\RootDir /App
    BDS_SYS_INFO_T  stSysInfo;

    if (!GetBDSSysInfo(stSysInfo))
    {
        return "";
    }

    AnsiString      strIncPath = "";
    AnsiString      strLibPath = "";
    AnsiString      strDefine = "";
    AnsiString      strObjPath = "";

    if (!GetProjectParam(szProjFile, strDefine, strIncPath, strLibPath, strObjPath))
    {
        return "";
    }

    // NO_STRICT;_BCB_;_NMS_CLIENT;_UNICODE_;$(Defines)
    ReplaceStr(strDefine, "$(Defines)", stSysInfo.strDefine);

    // ... $(IncludePath)
    ReplaceStr(strIncPath, "$(BDS)", stSysInfo.strRootDir);
    ReplaceStr(strIncPath, "$(IncludePath)", stSysInfo.strSysIncPath);
    ReplaceStr(strIncPath, "$(TMSPATH)", stSysInfo.strTmsPath);
    ReplaceStr(strIncPath, "$(DEVPATH)", stSysInfo.strDevPath);
    ReplaceStr(strIncPath, "$(RAZPATH)", stSysInfo.strRaizePath);
    ReplaceStr(strIncPath, "$(TEEINCPATH)", stSysInfo.strTeeIncPath);

    FormatPath(strIncPath);

    ReplaceStr(strLibPath, "$(ILINK_LibraryPath)", stSysInfo.strSysLibPath);
//ReplaceStr(strIncPath, "$(TEEOBJPATH", stSysInfo.strTeeObjPath) ||
//ReplaceStr(strIncPath, "$(TEELIBPATH", stSysInfo.strTeeLibPath) )

    FormatPath(stSysInfo.strBccExe);

//    char    *szExe = "\"c:\\program files (x86)\\embarcadero\\rad studio\\7.0\\bin\\bcc32.exe\"";
//    char    *szMacro = "-D_DEBUG;NO_STRICT;_BCB_;_NMS_CLIENT;_UNICODE_";
//    char    *szInc = "-Inetdev;preview;alarmdev;led;hik_dec;C:\\TeeChart2017\\Compiled\\Delphi14\\Include;matrix;report;sec;alarm;topo;sys;common;plat;..\\nms_public;cfg;perf;report;alarm;common;plat;cfg;perf;sec;sys;topo;..\\nms_public;..\\gos;..\\plat\\public;..\\dtp;\"c:\\program files (x86)\\embarcadero\\rad studio\\7.0\\include\";\"c:\\program files (x86)\\embarcadero\\rad studio\\7.0\\include\\dinkumware\";\"c:\\program files (x86)\\embarcadero\\rad studio\\7.0\\include\\vcl\";C:\\TMS;C:\\Raize\\Lib\\RS2010;C:\\DevExp_11\\Library\\RS14;..\\LedAPI;camera;deccfg;..\\ffmpeg\\include;.;\"c:\\program files (x86)\\embarcadero\\rad studio\\7.0\\ObjRepos\\Cpp\";\"c:\\program files (x86)\\embarcadero\\rad studio\\7.0\\include\\Indy10\";\"c:\\program files (x86)\\embarcadero\\rad studio\\7.0\\RaveReports\\Lib\"";
    AnsiString  strParam = " -y -k -r- -c -tWM -tW -Hs- -C8 -w-par -Od -v -vi- -o";
    char    acParamPch[1024]; // -H=Debug\iCityVMS.pch
    char    acProjectName[256];

    gos_get_file_rawname(szProjFile, acProjectName);

    CHAR *szEnd = acProjectName + strlen(acProjectName);
    for (; szEnd>=acProjectName; szEnd--)
    {
        if (*szEnd == '.' )
        {
            *szEnd = '\0';
            break;
        }
    }

//    sprintf(acParamPch, " -H=Debug\\%s.pch ", acProjectName);
    if (strObjPath.Length() > 0)
    {
        if (strObjPath[strObjPath.Length()] == '\\' ||
            strObjPath[strObjPath.Length()] == '/' )
        {
            strObjPath = strObjPath.SubString(1, strObjPath.Length()-1);
        }
    }

    if (!strObjPath.IsEmpty())
    {
        strParam += strObjPath + AnsiString("\\");

        char    acObjPath[256];

        gos_get_file_path(szProjFile, acObjPath);
        sprintf(acObjPath+strlen(acObjPath), "/%s", strObjPath.c_str());
        gos_format_path(acObjPath);

        if (!gos_file_exist(acObjPath))
        {
            gos_create_dir(acObjPath);
        }
    }

    AnsiString  strCmd = stSysInfo.strBccExe +
                         AnsiString(" -D") + AnsiString(strDefine) +
                         AnsiString(" -I") + AnsiString(strIncPath) +
                         strParam;

    return strCmd;
}
#else
/*
c:\program files (x86)\embarcadero\studio\20.0\bin\bcc32.exe -D_DEBUG;DEV_EXP_18;WIN32 -nDebug -I
../dc_public;../dtp;..\gplat;..\dtp;
"c:\program files (x86)\embarcadero\studio\20.0\include\windows\vcl";
"C:\DevExp18.1.6\Library\RAD Studio 10.3";
cfg;..\audio;sms;iph;if;groupcall;data;call;sys;common;plat;..\gos\public;..\eLTE_SDK\include;..\ds_public;
"c:\program files (x86)\embarcadero\studio\20.0\include";
..\ffmpeg\include;..\libcurl\include;"c:\program files (x86)\embarcadero\studio\20.0\include";
"c:\program files (x86)\embarcadero\studio\20.0\include\dinkumware";
"c:\program files (x86)\embarcadero\studio\20.0\include\windows\crtl";
"c:\program files (x86)\embarcadero\studio\20.0\include\windows\sdk";
"c:\program files (x86)\embarcadero\studio\20.0\include\windows\rtl";
"c:\program files (x86)\embarcadero\studio\20.0\include\windows\vcl";
"c:\program files (x86)\embarcadero\studio\20.0\include\windows\fmx";
C:\Users\Public\Documents\Embarcadero\Studio\20.0\hpp\Win32
-y -Q -k -r- -c -tM -tW -C8 -w-par -Od -v -vi- -H=Debug\dc.pch -oDebug\DlgSetFixedUE.obj -H cfg\DlgSetFixedUE.cpp
*/

AnsiString LoadBuilderCmd(char *szProjFile)
{
    AnsiString      strIncPath = GetIncPath(szProjFile);

    // 去除 $(BDS)\include;$(IncludePath)"
    ReplaceStr(strIncPath, "$(IncludePath)", "");
    ReplaceStr(strIncPath, "$(BDS)\\include", "");
    ReplaceStr(strIncPath, ";;", ";");

    BDS_SYS_INFO_T  stSysInfo;

    if (!GetBDSSysInfo(stSysInfo))
    {
        return "";
    }

//  ReplaceStr(strIncPath, "$(BDS)", stSysInfo.strRootDir);
//  ReplaceStr(strIncPath, "$(IncludePath)", stSysInfo.strSysIncPath);
    ReplaceStr(strIncPath, "$(TMS)", stSysInfo.strTmsPath);
    ReplaceStr(strIncPath, "$(DXVCL)", stSysInfo.strDevPath);
//    ReplaceStr(strIncPath, "$(RAZPATH)", stSysInfo.strRaizePath);
    ReplaceStr(strIncPath, "$(TEEINCPATH)", stSysInfo.strTeeIncPath);

    AnsiString  strCmd = "\"c:\\program files (x86)\\embarcadero\\studio\\20.0\\bin\\bcc32.exe\" "
                        "-D_DEBUG;DEV_EXP_18;WIN32 -nDebug -I";

    /*
    "..\\dc_public;..\\dtp;..\\gplat;..\\dtp;"
    "cfg;..\\audio;sms;iph;if;groupcall;data;call;sys;common;plat;..\\gos\\public;"
    "..\\eLTE_SDK\\include;..\\ds_public;"
    "..\\ffmpeg\\include;..\\libcurl\\include;"
    */

    char *szSysIncPath =
    "\"c:\\DevExp18.1.6\\Library\\RAD Studio 10.3\";"
    "\"c:\\program files (x86)\\embarcadero\\studio\\20.0\\include\";"
    "\"c:\\program files (x86)\\embarcadero\\studio\\20.0\\include\\dinkumware\";"
    "\"c:\\program files (x86)\\embarcadero\\studio\\20.0\\include\\windows\\crtl\";"
    "\"c:\\program files (x86)\\embarcadero\\studio\\20.0\\include\\windows\\sdk\";"
    "\"c:\\program files (x86)\\embarcadero\\studio\\20.0\\include\\windows\\rtl\";"
    "\"c:\\program files (x86)\\embarcadero\\studio\\20.0\\include\\windows\\vcl\";"
    "\"c:\\program files (x86)\\embarcadero\\studio\\20.0\\include\\windows\\fmx\";"
    "\"c:\\Users\\Public\\Documents\\Embarcadero\\Studio\\20.0\\hpp\\Win32\" ";

   //   char *szOption = "-y -Q -k -r- -c -tM -tW -C8 -w-par -Od -v -vi- -H=Debug\\dc.pch -oDebug\\";
    char *szOption = "-y -Q -k -r- -c -tM -tW -C8 -w-par -Od -v -vi- -oDebug\\";

    strCmd += strIncPath;
    strCmd += szSysIncPath;
    strCmd += szOption;
    //  \DlgSetFixedUE.obj -H cfg\DlgSetFixedUE.cpp

    return strCmd;
}
#endif

AnsiString LoadProjectFromReg()
{
    TRegistry   *Reg = new TRegistry;
    AnsiString  strProject = "";

    try
    {
        Reg->RootKey = HKEY_CURRENT_USER;

        if (Reg->OpenKey("Software\\iBuilder\\BDS7.0", false))
        {
            AnsiString strName = "ProjectName";
            if (Reg->ValueExists(strName))
            {
                strProject = Reg->ReadString(strName);
            }
        }
    }
    __finally
    {
        Reg->CloseKey();
        delete Reg;
    }

    return strProject;
}

void SaveProjectToReg(AnsiString strProject)
{
    TRegistry   *Reg = new TRegistry;

    try
    {
        Reg->RootKey = HKEY_CURRENT_USER;
        Reg->LazyWrite = false;

        if (Reg->OpenKey("Software\\iBuilder\\BDS7.0", true))
        {
            Reg->WriteString("ProjectName", strProject);
        }
    }
    __finally
    {
        Reg->CloseKey();
        delete Reg;
    }
}

void __fastcall TFormMain::InitSkin()
{
    tlSourceFile->OptionsSelection->CellSelect = true;
}

//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TSubForm(Owner)
{
    Application->Title = "iBuilder";
    Caption = Application->Title;

    tlSourceFile->Width = 800;
    tlSourceFile->OptionsSelection->CellSelect = true;

    gos_init_socket();

    UINT32  ulCpuCoreNum = GetCpuCoreNum();

    if (ulCpuCoreNum > ARRAY_SIZE(g_apThread))
    {
        ulCpuCoreNum = ARRAY_SIZE(g_apThread);
    }

    cbCpuNum->Clear();
    for (UINT32 i=0; i<ulCpuCoreNum; i++)
    {
        cbCpuNum->Properties->Items->Add(i+1);
    }

    cbCpuNum->ItemIndex = ulCpuCoreNum-1;

    edtProject->Text = LoadProjectFromReg();
    if (FileExists(edtProject->Text))
    {
        InitSourceFile(false);
    }

   // Width = 1000;
   // Height = 700;

    Application->OnMessage = MyOnMessage;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    g_hMainHandle = NULL;

    btnStopClick(this);

    CanClose = true;
}

UINT32 __fastcall TFormMain::GetFileState(char *szFile)
{
    char    acSourceFile[512];
    char    acObjFile[512];
    char    *szTmp;
    UINT32  ulSourceCreateTime;
    UINT32  ulSourceModifyTime;
    UINT32  ulObjCreateTime;
    UINT32  ulObjModifyTime;

    gos_get_file_rawname(szFile, acSourceFile);
    szTmp = strchr(acSourceFile, '.');
    if (!szTmp)
    {
        return BS_IDLE;
    }

    strcpy(szTmp, ".obj");
    sprintf(acObjFile, "%s/%s/%s", m_acProjectPath, "debug", acSourceFile);
    sprintf(acSourceFile, "%s/%s", m_acProjectPath, szFile);

    gos_get_file_time(acSourceFile, &ulSourceCreateTime, &ulSourceModifyTime);
    gos_get_file_time(acObjFile, &ulObjCreateTime, &ulObjModifyTime);

    if (ulSourceModifyTime < ulObjModifyTime)
    {
        return BS_COMPILED;
    }

    return BS_IDLE;
}

bool __fastcall TFormMain::InitSourceFile(bool bRebuild)
{
    AnsiString strFile = edtProject->Text;
    bool    bRet = false;
    int     iSize = gos_get_file_size(strFile.c_str());
    char    *szBuf;
    FILE    *fp;

    btnStart->Enabled = false;
    btnRebuild->Enabled = false;
    btnClear->Enabled = false;

    if (iSize <= 0)
    {
        return false;
    }

    SaveProjectToReg(edtProject->Text);

    gos_get_file_path(strFile.c_str(), m_acProjectPath);

    szBuf = (char*)calloc(1, iSize+1);
    if (!szBuf)
    {
        return false;
    }

    fp = fopen(strFile.c_str(), "rb");
    if (!fp)
    {
        free(szBuf);
        return false;
    }

    if (iSize != fread(szBuf, 1, iSize, fp))
    {
        free(szBuf);
        fclose(fp);
        return false;
    }

    fclose(fp);

    char    *szFileName;
    char    *szStart = strstr(szBuf, "<ItemGroup>");
    char    *szEnd;
    UINT32  ulLen;
    BUILD_FILE_INFO_T   stBuildFileInfo;
    int                 iColumn = 0;
    TcxTreeListNode     *Node;

    tlSourceFile->BeginUpdate();
    tlSourceFile->Clear();

    if (!szStart)
    {
        goto end;
    }

    g_vBuildFileInfo.clear();
    pbBuild->Properties->Max = 0;
    pbBuild->Position = 0;

    tpBuild->Total = 0;
    tpBuild->Position = 0;

    while(1)
    {
        // <CppCompile Include="..\common\ClientCommon.cpp">
        szStart = strstr(szStart, "<CppCompile");
        if (!szStart)
        {
            break;
        }

        szStart = strchr(szStart + strlen("<CppCompile"), '=');
        if (!szStart)
        {
            break;
        }

        szStart++;
        szEnd = strchr(szStart, '>');
        if (!szEnd)
        {
            break;
        }

        szFileName = gos_left_trim_string(szStart);
        szStart = szEnd+1;
        *szEnd = '\0';

        szFileName = gos_trim_string(szFileName);
        if (*szFileName != '"')
        {
            break;
        }

        szFileName++;
        ulLen = strlen(szFileName);
        if (ulLen <= 1)
        {
            break;
        }

        if (szFileName[ulLen-1] != '"')
        {
            break;
        }

        szFileName[ulLen-1] = '\0';

        if (iColumn == 0)
        {
            Node = tlSourceFile->Root->AddChild();
        }

        stBuildFileInfo.strFile = szFileName;
        stBuildFileInfo.ulState = bRebuild?BS_IDLE:GetFileState(szFileName);
        stBuildFileInfo.strResult = "";
        stBuildFileInfo.Node = Node;
        stBuildFileInfo.iColIndex = iColumn;

        Node->Values[2*iColumn] = stBuildFileInfo.strFile;
        Node->Values[2*iColumn+1] = stBuildFileInfo.ulState;

        //g_mBuilderInfo[AnsiString(szFileName)] = Node;
        g_vBuildFileInfo.push_back(stBuildFileInfo);

        if (stBuildFileInfo.ulState == BS_IDLE)
        {
            pbBuild->Properties->Max++;
        }

        iColumn++;
        if (iColumn == 3)
        {
            iColumn = 0;
        }
    }

    tpBuild->Total = pbBuild->Properties->Max;

    bRet = true;

end:
    GOS_FREE(szBuf);

    tlSourceFile->EndUpdate();

    if (bRet)
    {
        btnStart->Enabled = true;
        btnRebuild->Enabled = true;
        btnClear->Enabled = true;
    }

    return bRet;
}

UINT32 __fastcall TFormMain::GetThreadNum()
{
    //int   iCpuNum = edtCpuNum->Text.ToIntDef(-1);

    int  iCpuNum = cbCpuNum->Text.ToIntDef(1);//tbThreadNum->Position;

    if (iCpuNum <= 0)
    {
        return 0;
    }

    if (iCpuNum > ARRAY_SIZE(g_apThread))
    {
        iCpuNum = ARRAY_SIZE(g_apThread);
    }

    return iCpuNum;
}

bool __fastcall TFormMain::InitBuilderPara()
{
    char        acPath[1024];
    char        acProjFile[1024];

    StrCpy(acProjFile, edtProject->Text);
    gos_get_file_path(acProjFile, acPath);

    AnsiString  strCmd = LoadBuilderCmd(acProjFile);

    if (strCmd.IsEmpty())
    {
        return false;
    }

    UINT32  ulThreadNum = GetThreadNum();

    for (UINT32 i=0; i<ulThreadNum; i++)
    {
        g_apThread[i]->SetBuilderCmd(strCmd.c_str());
        g_apThread[i]->SetProjectPath(acPath);
    }

    return true;
}

void __fastcall TFormMain::btnStartClick(TObject *Sender)
{
    g_bRebuild = false;
    btnStart->Enabled = false;
    btnRebuild->Enabled = false;
    btnStop->Enabled = true;
    btnClear->Enabled = false;

    memset(g_apThread, 0, sizeof(g_apThread));

    g_ulThreadNum = GetThreadNum();

    for (UINT32 i=0; i<g_ulThreadNum; i++)
    {
        g_bThreadRunning[i] = TRUE;
        g_apThread[i] = new ThreadBuilder(i+1, &g_bThreadRunning[i]);
        g_apThread[i]->Start();
    }

    if (!InitBuilderPara())
    {
        btnStopClick(this);
        return;
    }

    g_bBuildering = TRUE;
}

void __fastcall TFormMain::btnStopClick(TObject *Sender)
{
    UINT32  i, j;

    g_bBuildering = FALSE;
    btnStop->Enabled = false;

    for (UINT32 i=0; i<g_ulThreadNum; i++)
    {
        if (g_bThreadRunning[i])
        {
            g_apThread[i]->Stop();
        }
    }

    while(1)
    {
        bool    bRunning = FALSE;

        for (i=0; i<g_ulThreadNum; i++)
        {
            if (g_bThreadRunning[i])
            {
                bRunning = TRUE;
                break;
            }
        }

        if (!bRunning)
        {
            break;
        }

        gos_sleep_1ms(100);
    }

    gos_sleep_1ms(100);

    for (i=0; i<g_ulThreadNum; i++)
    {
        delete g_apThread[i];
        g_apThread[i] = NULL;
    }

    btnStart->Enabled = true;
    btnRebuild->Enabled = true;
    btnClear->Enabled = true;
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::LogBuilderInfo(BUILD_INFO_T *pstInfo)
{
    Log(AnsiString("============ build file: ") + AnsiString(pstInfo->acSourceFile));
    Log(pstInfo->acBuilderInfo);
}
void __fastcall TFormMain::Log(AnsiString strText)
{
    mmInfo->Lines->Add(strText);
    mmInfo->Lines->Add("\n");
}
//---------------------------------------------------------------------------
TcxTreeListNode* __fastcall TFormMain::GetUnbuildSourceFile(UINT32 &ulColIndex)
{
  /*    std::map<AnsiString, TcxTreeListNode*>::iterator  it = g_mBuilderInfo.begin();

    for (;it!=g_mBuilderInfo.end(); it++)
    {
        TcxTreeListNode *Node = it->second;

        int iState = (int)Node->Values[1];

        if (iState == BS_IDLE)
        {
            return Node;
        }
    }  */

    int             iState;
    TcxTreeListNode *Node = tlSourceFile->Root->getFirstChild();

    while(Node)
    {
        iState = (int)Node->Values[1];
        if (g_bRebuild || iState == BS_IDLE)
        {
            ulColIndex = 0;
            return Node;
        }

        if (Node->Values[3].IsNull())
        {
            break;
        }

        iState = (int)Node->Values[3];
        if (g_bRebuild || iState == BS_IDLE)
        {
            ulColIndex = 2;
            return Node;
        }

        if (Node->Values[5].IsNull())
        {
            break;
        }

        iState = (int)Node->Values[5];
        if (g_bRebuild || iState == BS_IDLE)
        {
            ulColIndex = 4;
            return Node;
        }

        Node = Node->getNextSibling();
    }

    return NULL;
}

void __fastcall TFormMain::tmrCheckTimer(TObject *Sender)
{
    if (!g_bBuildering)
    {
        return;
    }

    UINT32  ulThreadNum = GetThreadNum();

    for (UINT32 i=0; i<ulThreadNum; i++)
    {
        UINT32          ulColIndex = 0;
        TcxTreeListNode *Node = GetUnbuildSourceFile(ulColIndex);

        if (!Node)
        {
            btnStopClick(this);
            break;
        }

        AnsiString  strFile = Node->Values[ulColIndex];

        if (g_apThread[i]->SetSourceFile(strFile.c_str()))
        {
            Node->Values[ulColIndex+1] = BS_BUILDING;
        }
    }
}

void FormatBuildResult(BUILD_INFO_T *pstInfo, UINT32 &ulState)
{
    char *szPos = strstr(pstInfo->acBuilderInfo, pstInfo->acSourceFile);

    ulState = BS_FAIL;
    if (!szPos)
    {
        return;
    }

    szPos += strlen(pstInfo->acSourceFile);
    szPos = strchr(szPos, ':');
    if (!szPos)
    {
        return;
    }

    szPos = gos_trim_string(szPos+1);
    memmove(pstInfo->acBuilderInfo, szPos, strlen(szPos)+1);

    szPos = strstr(pstInfo->acBuilderInfo, "error");
    if (szPos)
    {
        ulState = BS_FAIL;
    }
    else
    {
        ulState = BS_SUCC;
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::OnInnerMsg(TMessage Message)
{
    BUILD_INNER_MSG_T       *pstMsg = (BUILD_INNER_MSG_T *)Message.LParam;

    if (pstMsg->ulMsgID == MSG_BUILD_INFO)
    {
        BUILD_INFO_T    *pstInfo = (BUILD_INFO_T*)pstMsg->aucMsg;

        AnsiString  strFile = pstInfo->acSourceFile;
        UINT32      ulState;

        BUILD_FILE_INFO_T   *pstFile = GetBuildFileInfo(strFile);

        if (!pstFile) //g_mBuilderInfo.find(strFile) == g_mBuilderInfo.end())
        {
            GosLog(LOG_ERROR, "未知文件%s", pstInfo->acSourceFile);
            goto end;
        }

        FormatBuildResult(pstInfo, ulState);

        pstFile->strResult = pstInfo->acBuilderInfo;

        //LogBuilderInfo(pstInfo);

        TcxTreeListNode     *Node = pstFile->Node; //g_mBuilderInfo[strFile];

        int  iColIndex = 2*pstFile->iColIndex+1;

        if (Node->Values[iColIndex] != BS_SUCC &&
            Node->Values[iColIndex] != BS_FAIL)
        {
            pbBuild->Position ++;
            tpBuild->Position ++;
        }

        Node->Values[iColIndex] = ulState;//GetBuildState(pstInfo->acResult);
    }

end:
    free(pstMsg);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormShow(TObject *Sender)
{
    g_hMainHandle = this->Handle;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnOpenClick(TObject *Sender)
{
    OpenDialog->Title = "打开工程文件";

    if (!OpenDialog->Execute())
    {
        return;
    }

    AnsiString  strFile = OpenDialog->FileName;

    edtProject->Text = strFile;

/*  if (!InitBuilderPara())
    {
        return;
    }
  */

    if (!InitSourceFile(false))
    {
        return;
    }

}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ColumnStateGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    int     iIndex = Sender->Position->ColIndex;

    if (iIndex != 1 && iIndex != 3 && iIndex != 5)
    {
        return;
    }

    if (ANode->Values[iIndex].IsNull())
    {
        return;
    }

    int     iValue = ANode->Values[iIndex];

    if (iValue == BS_IDLE)
    {
        Value = "";
    }
    else if (iValue == BS_COMPILED)
    {
        Value = "--";
    }
    else if (iValue == BS_BUILDING)
    {
        Value = "编译中";
    }
    else if (iValue == BS_SUCC)
    {
        Value = "完成";
    }
    else if (iValue == BS_FAIL)
    {
        Value = "编译错误";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::tlSourceFileCustomDrawDataCell(TcxCustomTreeList *Sender,
          TcxCanvas *ACanvas, TcxTreeListEditCellViewInfo *AViewInfo,
          bool &ADone)
{
    int iIndex = AViewInfo->Column->ItemIndex;

    if (iIndex != 1 && iIndex != 3 && iIndex != 5)
    {
        return;
    }

    if (AViewInfo->Node->Values[iIndex].IsNull())
    {
        return;
    }

    int iValue = AViewInfo->Node->Values[iIndex];

    if (iValue == BS_BUILDING)
    {
        ACanvas->Canvas->Brush->Color = clGreen;
    }
    else if (iValue == BS_FAIL)
    {
        ACanvas->Canvas->Brush->Color = (TColor)RGB(192,0,0);//clGreen;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnClearClick(TObject *Sender)
{
    char        acPath[1024];
    char        acProjFile[1024];

    StrCpy(acProjFile, edtProject->Text);
    gos_get_file_path(acProjFile, acPath);

    strcat(acPath, "\\debug");

    gos_delete_dir(acPath);

    InitSourceFile(true);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::tlSourceFileClick(TObject *Sender)
{
    TcxTreeListNode *Node;

    if (tlSourceFile->SelectionCount == 0)
    {
        return;
    }

    Node = tlSourceFile->Selections[0];

    TcxTreeListColumn   *Column = tlSourceFile->FocusedColumn;

    if (!Column)
    {
        return;
    }

    int     iColIndex = Column->Position->ColIndex;

    if (iColIndex == 1 || iColIndex == 3 || iColIndex == 5)
    {
        AnsiString  strFile = Node->Values[iColIndex-1];

        BUILD_FILE_INFO_T   *pstFile = GetBuildFileInfo(strFile);

        if (pstFile)
        {
            mmInfo->Text = pstFile->strResult;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnRebuildClick(TObject *Sender)
{
    int             iState;
    TcxTreeListNode *Node = tlSourceFile->Root->getFirstChild();

    while(Node)
    {
        Node->Values[1] = BS_IDLE;

        if (Node->Values[3].IsNull())
        {
            break;
        }

        Node->Values[3] = BS_IDLE;

        if (Node->Values[5].IsNull())
        {
            break;
        }

        Node->Values[5] = BS_IDLE;

        Node = Node->getNextSibling();
    }

    InitSourceFile(true);
    btnStartClick(this);
}

bool g_bStart = false;
//---------------------------------------------------------------------------
void __fastcall TFormMain::MyOnMessage(tagMSG &Msg, bool &Handled)
{
    if (!g_bStart)
    {
        return;
    }

    UINT32      ulValue = Msg.message;
    UINT32      ulWord  = Msg.wParam;
    UINT32      ulLong  = Msg.lParam;

    if (ulValue == 0x8080 ||
        ulValue == WM_MOUSEMOVE ||
        ulValue == WM_TIMER ||
        ulValue == 0x812D)
    {
        return;
    }

    static UINT32 i = 0;
    char    acText[128];

    sprintf(acText, "%d %X %X %X", i, ulValue, ulWord, ulLong);
    mmInfo->Lines->Append(acText);

/*  if (1)//Msg.message >= WM_KEYFIRST && Msg.message <= WM_KEYLAST)
    {
        mmInfo->Lines->Append(String(++i) + String(" ") + String(ulValue));
    }
    else if (Msg.message == WM_KEYDOWN || Msg.message == WM_KEYUP )
    {
        mmInfo->Lines->Append(String(++i) + String(" ") + String(ulValue));
    } */
}

void __fastcall TFormMain::tlSourceFileKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
   //
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::tlSourceFileKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
//  mmInfo->Lines->Append("--");
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::tlSourceFileDblClick(TObject *Sender)
{
   // g_bStart = !g_bStart;
}
//---------------------------------------------------------------------------


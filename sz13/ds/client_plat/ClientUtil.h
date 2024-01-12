//---------------------------------------------------------------------------

#ifndef ClientUtilH
#define ClientUtilH
//---------------------------------------------------------------------------

#include <vector>
#include "cxDropDownEdit.hpp"

#include <teEngine.hpp>
#include "g_include.h"
#include "Chart.hpp"
#include <typeinfo>
#include "cxVGrid.hpp"
#include "TableCfg.h"

class Dict
{
public:
    Dict();
    /**
     * @brief           使用键值对数组来初始化本对象
     * @param           pstMapRec       [in]    ID_VALUE_T 数组
     * @param           ulMapRecNum     [in]    数组长度
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-10-28 11:07:06
     * @note
     */
    Dict(ID_VALUE_T *pstMapRec, UINT32 ulMapRecNum);

    /**
     * @brief           往下拉框中添加信息
     * @param           cb          [in/out]    要修改的下拉框
     * @param           bClear      [in]        是否清空下拉框中之前的信息(默认清空)
     * @param           bHideFirst  [in]        是否跳过第一个键值对（默认不跳过）
     * @return          VOID
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-10-28 11:08:23
     * @note
     */
    VOID Init(TcxComboBox *cb, BOOL bClear=TRUE, BOOL bHideFirst=FALSE);

    /// 设置上一次 init 或 set 操作下拉列表中的当前选中的 ID
    VOID SetID(UINT32 ulID);

    /// 设置下拉列表中的当前选中的 ID
    VOID SetID(TcxComboBox *cb, UINT32 ulID);

    /// 获取上一次 init 或 set 操作下拉列表中的 ID
    BOOL  GetID(UINT8 *pucID);
    BOOL  GetID(UINT16 *pusID);
    BOOL  GetID(UINT32 *pulID);

    /// 获取下拉列表中的 当前选中的ID
    BOOL  GetID(TcxComboBox *cb, UINT8 *pucID);
    BOOL  GetID(TcxComboBox *cb, UINT16 *pusID);
    BOOL  GetID(TcxComboBox *cb, UINT32 *pulID);

    /// 通过入参的字符串获取对应的 ID
    BOOL  GetID(Variant value, UINT32 *pulID);
    /// 通过字符串获取对应的 ID
    BOOL  GetID(CHAR* szValue, UINT32 *pulID);
    /// 通过入参返回入参对应的字符串
    CHAR* GetValue(UINT32 ulID);
    /// 添加键值对。 Key: UINT32 Value: CHAR*
    BOOL  Add(ID_VALUE_T *pstRec);
    /// 添加键值对。 Key: UINT32 Value: CHAR*
    BOOL  Add(UINT32 ulID, CHAR *szValue);
    /// 清除键值对信息
    VOID  Clear();

private:
    std::vector<ID_VALUE_T> m_MapRec;   ///< 用于数字对应的字符串
    TcxComboBox             *m_cb;      ///< 用于记录上一次操作的下拉列表
};

AnsiString GetTextFileSize(CHAR *szFile);
AnsiString GetTextFileSize(INT64 i64FileSize);
AnsiString GetTextFileTime(INT32 iTime);
AnsiString GetTextIP(UINT8 *pucIP);
AnsiString GetTextMAC(UINT8 *pucMAC);

void Focus(TWinControl *control);
AnsiString GetTextIP(UINT8 *aucIP);
AnsiString GetTextIP(UINT8 *aucIP, UINT32 ulNetMask);
AnsiString GetTextTime(UINT32 ulSecond, CHAR *szFormat=NULL);
AnsiString GetTimeText(UINT32 ulTime);
AnsiString GetTimeText();
AnsiString GetTextUptime(UINT32 ul10ms);
TDateTime GetDateTime(UINT32 ulTime);
UINT32 GetTime(const TDateTime &DateTime);
AnsiString GetTextTemp(INT32 iValue);
void ExecShellCmd(AnsiString strCmd);
AnsiString GetPwdText(AnsiString strPwd);
AnsiString DoubleText(double dValue, int iNum);

TcxEditorRow* AddTextRow(TcxCategoryRow *GroupRow, AnsiString strCaption, AnsiString strValue, TColor uColor=clWhite);
TcxEditorRow* AddTextRow(TcxCustomVerticalGrid *Grid, TcxCategoryRow *GroupRow, AnsiString strCaption, AnsiString strValue, TColor clColor=clWhite);
TcxCategoryRow* AddGroupRow(TcxVerticalGrid *Grid, AnsiString strGroupName);
AnsiString GetClassName(TObject *obj);
void SetVerticalGridHeaderWidth(TcxVerticalGrid *Grid, UINT32 ulWidth);
UINT32 TimeToInt(AnsiString strTime);
BOOL TextTimeToInt(AnsiString strTime, UINT32 *pulTime);
AnsiString GetIntText(INT64 i64Value);

void Print(TcxTreeList *treeList, AnsiString strTitle);
TABLE_CFG_T* LoadTableCfg(CHAR *pchName);
BOOL SaveTableCfg(CHAR *pchName, TABLE_CFG_T *pstTableNewCfg);
BOOL SaveTableCfg(CHAR *pchName, TABLE_PRINT_CFG_T *pstTableNewPrintCfg);
BOOL SaveTableCfg(CHAR *pchName, TABLE_COLUMN_CFG_T *pstTableNewColumnCfg);

void SetChartAxisRange(TChartAxis *Axis, double dNewMin, double dNewMax);
void SetChartAxisRange(TChartAxis *Axis, TChartSeries *Series);
void SetChartAxisIncrement(TChartAxis *Axis, UINT32 ulIncementNum);
void SetChartAxisPrecision(TChartAxis *Axis, int iPrecision);

bool RunProc(char* szProcName, char* szProcCmd, char* szStartDir);

#ifdef _UNICODE_
int StrCmp(UnicodeString str, char *sz);
#endif

INT32 StrToInt(AnsiString str);
INT64 StrToInt64(AnsiString str);

int StrCmp(AnsiString str, char *sz);
int StrCmp(char *sz, AnsiString str);
char* StrCpy(char *sz, AnsiString str);

BOOL ToFloat(AnsiString str, DOUBLE *d);
BOOL ToInt(AnsiString str, UINT32 *u);
BOOL ToInt(AnsiString str, INT32 *i);
BOOL ToInt64(AnsiString str, INT64 *i);
BOOL ToInt(AnsiString str, UINT16 *u);
BOOL GetIP(AnsiString strAddr, UINT8 *pucAddr);
BOOL GetIPPort(AnsiString strAddr, UINT8 *pucAddr, UINT16 *pusPort);

#ifdef _UNICODE_
UINT32 TimeToInt(UnicodeString strTime);
int StrCmp(char *sz, UnicodeString str);
char* StrCpy(char *sz, UnicodeString str);
BOOL ToFloat(UnicodeString str, DOUBLE *d);
BOOL ToInt(UnicodeString str, UINT32 *u);
BOOL ToInt(UnicodeString str, INT32 *i);
BOOL ToInt64(UnicodeString str, INT64 *i);
UINT64 ToInt64(UINT32 ulHigh, UINT32 ulLow);
BOOL GetIP(UnicodeString strAddr, UINT8 *pucAddr);
BOOL GetIPPort(UnicodeString strAddr, UINT8 *pucAddr, UINT16 *pusPort);
#endif

bool ImportFromFile(TcxTreeList *tl, char *szCSV, bool bWithIndexColumn);
bool ExportTreeListToExcel(TcxCustomTreeList *tl, AnsiString strFile);
bool ExportTreeListToCsv(TcxCustomTreeList *tl, AnsiString strFile);

bool EnablePriv();
void CloseApp();
AnsiString UTF8ToAnsi(CHAR* szUtf8);

void EncryptString(CHAR *szIn, CHAR *szOut);
void DecryptString(CHAR *szIn, CHAR *szOut);
VOID RemoveCommonMember(VECTOR<UINT32> &s, VECTOR<UINT32> &d);
VOID RemoveMember(VECTOR<UINT32> &s, UINT32 d);
VOID AppendMember(VECTOR<UINT32> &d, VECTOR<UINT32> &s);
BOOL Exist(VECTOR<UINT32> &s, UINT32 d);
BOOL IsSame(VECTOR<UINT32> &s, VECTOR<UINT32> &d);
BOOL Exist(VECTOR<INT64> &s, INT64 d);
BOOL IsSame(VECTOR<INT64> &s, VECTOR<INT64> &d);
VOID RemoveCommonMember(IntSet &s, IntSet &d);
VOID RemoveCommonMember(Int64Set &s, Int64Set &d);

AnsiString GetConfValue(CHAR *szFile, const CHAR *szGroup, const CHAR *szKey);
bool ParseStringTuple(AnsiString strIn, AnsiString strSeperator, VECTOR<AnsiString> &vOut);

class SmartString
{
public:
    SmartString();
    SmartString(char *str);
    SmartString(const SmartString &other);
    SmartString &operator=(const SmartString &other);
    ~SmartString();

    operator CHAR *();
    SmartString &operator=(char *str);

private:
    char *m_szStr;
};

AnsiString FromStyledString(AnsiString str);
AnsiString ToStyledString(CHAR *szStr);
AnsiString ToStyledString(AnsiString str);

/**
 * @brief           获取 edt 中的文字
 * @param           edt    [in]
 * @return          std::string
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-14 14:24:27
 * @note
 */
std::string GetEdtText(TcxTextEdit * edt);

void SetEditable(TcxTextEdit *edt, bool bEditable);
void SetEditable(TcxComboBox *cb, bool bEditable);
void SetEditable(TcxCheckListBox *clb, bool bEditable);

bool ReplaceString(AnsiString &strSrc, AnsiString strFrom, AnsiString strTo);
AnsiString DecodeJsonMsg(AnsiString strMsgInfo);
BOOL DecodeJsonMsg(CHAR *szInfo, UINT32 ulMaxLen);
std::string UnzipString(CHAR *szMsgInfo);

#endif

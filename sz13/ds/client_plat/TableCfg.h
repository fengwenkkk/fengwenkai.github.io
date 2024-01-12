//---------------------------------------------------------------------------

#ifndef TableCfgH
#define TableCfgH

#include <cxTL.hpp>

#pragma pack(push, 4)

#define TABLE_MAX_DISPLAY_COLUNM 64

typedef struct
{
    UINT32 ulFontSize;
    UINT32 ulBold;
    UINT32 ulHeight;
}TABLE_PRINT_FONT_CFG_T;

typedef struct
{
    UINT32 ulTop;
    UINT32 ulBottom;
    UINT32 ulLeft;
    UINT32 ulRight;
}TABLE_PAGE_MARGIN_T;

typedef struct
{
    TABLE_PRINT_FONT_CFG_T stPrintTitleCfg;
    TABLE_PRINT_FONT_CFG_T stPrintFootCfg;
    TABLE_PAGE_MARGIN_T    stPageMargin;
    UINT8                  usPageOientation;
}TABLE_PRINT_CFG_T;

typedef struct
{
    UINT32                 auDisplayColumnWidth[TABLE_MAX_DISPLAY_COLUNM];
    UINT8                  usColumnNum;
}TABLE_COLUMN_CFG_T;

typedef struct
{
    CHAR                  *pszName;
    UINT32                 auDisplayColumnWidth[TABLE_MAX_DISPLAY_COLUNM];
    UINT8                  usColumnNum;
    TABLE_PRINT_FONT_CFG_T stPrintTitleCfg;
    TABLE_PRINT_FONT_CFG_T stPrintFootCfg;
    TABLE_PAGE_MARGIN_T    stPageMargin;
    UINT8                  usPageOientation;
}TABLE_CFG_T;

class LocalTableCfg
{
public:
    static LocalTableCfg* GetInstance();

    TABLE_CFG_T* GetTableCfg(CHAR *pchName);
    BOOL         SaveTableCfg(TABLE_CFG_T *pstTableCfg);
    BOOL         ReLoad();
    BOOL         SaveToFile();

private:
    VECTOR<TABLE_CFG_T> m_vstTableCfg;

    LocalTableCfg();
    ~LocalTableCfg();
    BOOL Load();
    BOOL CreateNewFile(CHAR *pchFile);
};

TABLE_CFG_T* LoadTableCfg(CHAR *pchName);

BOOL SaveTableCfg(CHAR *szName, TABLE_CFG_T *pstTableNewCfg);
BOOL SaveTableCfg(CHAR *szName, TABLE_PRINT_CFG_T *pstTableNewPrintCfg);
BOOL SaveTableCfg(CHAR *szName, TABLE_COLUMN_CFG_T *pstTableNewColumnCfg);

void LoadTableInfo(TcxCustomTreeList *table);
void SaveTableInfo(TcxCustomTreeList *table);

void SaveConfInfo(AnsiString strItemName, AnsiString strKey, INT32 iValue);
void SaveConfInfo(AnsiString strItemName, AnsiString strKey, AnsiString strValue);
bool LoadConfInfo(AnsiString strItemName, AnsiString strKey, INT32 &iValue);
bool LoadConfInfo(AnsiString strItemName, AnsiString strKey, INT32 *piValue);
bool LoadConfInfo(AnsiString strItemName, AnsiString strKey, INT32 *piValue, INT32 iMin, INT32 iMax);
AnsiString LoadConfInfo(AnsiString strItemName, AnsiString strKey);

#pragma pack(pop)
//---------------------------------------------------------------------------
#endif

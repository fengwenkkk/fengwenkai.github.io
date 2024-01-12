#pragma hdrstop

#include <vcl.h>
#include "g_include.h"
#include "GCommon.h"

#include <Registry.hpp>
#include "TableCfg.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

static AnsiString    g_strRegKey = "\\SOFTWARE\\GBCOM\\";

TABLE_CFG_T* LoadTableCfg(CHAR *pchName)
{
    LocalTableCfg *pTableList = LocalTableCfg::GetInstance();
    if (pTableList == NULL || pchName == NULL)
    {
        return NULL;
    }

    return pTableList->GetTableCfg(pchName);
}

BOOL SaveTableCfg(CHAR *pchName, TABLE_CFG_T *pstTableNewCfg)
{
    UINT16        i          = 0;
    TABLE_CFG_T   *pTableCfg  = NULL;
    LocalTableCfg *pTableList = LocalTableCfg::GetInstance();

    if (pTableList == NULL)
    {
        return FALSE;
    }

    if (pchName == NULL || pstTableNewCfg == NULL)
    {
        return FALSE;
    }

    pTableCfg = pTableList->GetTableCfg(pchName);
    if (pTableCfg == NULL)
    {
        return pTableList->SaveTableCfg(pstTableNewCfg);
    }

    pTableCfg->usColumnNum      = pstTableNewCfg->usColumnNum;
    pTableCfg->usPageOientation = pstTableNewCfg->usPageOientation;
    memcpy(&pTableCfg->stPageMargin, &pstTableNewCfg->stPageMargin, sizeof(pTableCfg->stPageMargin));
    memcpy(&pTableCfg->stPrintTitleCfg, &pstTableNewCfg->stPrintTitleCfg, sizeof(pTableCfg->stPrintTitleCfg));
    memcpy(&pTableCfg->stPrintFootCfg, &pstTableNewCfg->stPrintFootCfg, sizeof(pTableCfg->stPrintFootCfg));

    for (i=0; i<pstTableNewCfg->usColumnNum; i++)
    {
        pTableCfg->auDisplayColumnWidth[i] = pstTableNewCfg->auDisplayColumnWidth[i];
    }

    return pTableList->SaveToFile();
}

BOOL SaveTableCfg(CHAR *pchName, TABLE_COLUMN_CFG_T *pstTableNewColumnCfg)
{
    UINT16         i          = 0;
    TABLE_CFG_T   *pTableCfg  = NULL;
    LocalTableCfg *pTableList = LocalTableCfg::GetInstance();
    TABLE_CFG_T    stTableCfg;

    if (pTableList == NULL)
    {
        return FALSE;
    }

    if (pchName == NULL || pstTableNewColumnCfg == NULL)
    {
        return FALSE;
    }

    pTableCfg = pTableList->GetTableCfg(pchName);
    if (pTableCfg == NULL)
    {
        memset(&stTableCfg, 0, sizeof(stTableCfg));

        stTableCfg.pszName     = strdup(pchName);
        stTableCfg.usColumnNum = pstTableNewColumnCfg->usColumnNum;

        for (i=0; i<pstTableNewColumnCfg->usColumnNum; i++)
        {
            stTableCfg.auDisplayColumnWidth[i] = pstTableNewColumnCfg->auDisplayColumnWidth[i];
        }

        return pTableList->SaveTableCfg(&stTableCfg);
    }

    pTableCfg->usColumnNum = pstTableNewColumnCfg->usColumnNum;

    for (i=0; i<pstTableNewColumnCfg->usColumnNum; i++)
    {
        pTableCfg->auDisplayColumnWidth[i] = pstTableNewColumnCfg->auDisplayColumnWidth[i];
    }

    return pTableList->SaveToFile();
}

BOOL SaveTableCfg(CHAR *pchName, TABLE_PRINT_CFG_T *pstTableNewPrintCfg)
{
    TABLE_CFG_T   *pTableCfg  = NULL;
    LocalTableCfg *pTableList = LocalTableCfg::GetInstance();
    TABLE_CFG_T    stTableCfg;

    if (pTableList == NULL)
    {
        return FALSE;
    }

    if (pchName == NULL || pstTableNewPrintCfg == NULL)
    {
        return FALSE;
    }

    pTableCfg = pTableList->GetTableCfg(pchName);
    if (pTableCfg == NULL)
    {
        memset(&stTableCfg, 0, sizeof(stTableCfg));

        stTableCfg.pszName     = strdup(pchName);
        stTableCfg.usPageOientation = pstTableNewPrintCfg->usPageOientation;
        memcpy(&stTableCfg.stPageMargin, &pstTableNewPrintCfg->stPageMargin, sizeof(stTableCfg.stPageMargin));
        memcpy(&stTableCfg.stPrintTitleCfg, &pstTableNewPrintCfg->stPrintTitleCfg, sizeof(stTableCfg.stPrintTitleCfg));
        memcpy(&stTableCfg.stPrintFootCfg, &pstTableNewPrintCfg->stPrintFootCfg, sizeof(stTableCfg.stPrintFootCfg));

        return pTableList->SaveTableCfg(&stTableCfg);
    }

    pTableCfg->usPageOientation = pstTableNewPrintCfg->usPageOientation;
    memcpy(&pTableCfg->stPageMargin, &pstTableNewPrintCfg->stPageMargin, sizeof(pTableCfg->stPageMargin));
    memcpy(&pTableCfg->stPrintTitleCfg, &pstTableNewPrintCfg->stPrintTitleCfg, sizeof(pTableCfg->stPrintTitleCfg));
    memcpy(&pTableCfg->stPrintFootCfg, &pstTableNewPrintCfg->stPrintFootCfg, sizeof(pTableCfg->stPrintFootCfg));

    return pTableList->SaveToFile();
}

LocalTableCfg* LocalTableCfg::GetInstance()
{
    static LocalTableCfg* pInstance = NULL;

    if (pInstance == NULL)
    {
        pInstance = new LocalTableCfg();
        if (pInstance == NULL)
        {
            return NULL;
        }

        if (!pInstance->Load())
        {
            delete pInstance;

            pInstance = NULL;

            return pInstance;
        }
    }

    return pInstance;
}

LocalTableCfg::LocalTableCfg()
{
    m_vstTableCfg.clear();
}

LocalTableCfg::~LocalTableCfg()
{
}

TABLE_CFG_T* LocalTableCfg::GetTableCfg(CHAR *pchName)
{
    UINT32 i = 0;

    for (i=0; i<m_vstTableCfg.size(); i++)
    {
        if (strcmp(m_vstTableCfg[i].pszName, pchName) == 0)
        {
            return &m_vstTableCfg[i];
        }
    }

    return NULL;
}

BOOL LocalTableCfg::SaveTableCfg(TABLE_CFG_T *pstTableCfg)
{
    m_vstTableCfg.push_back(*pstTableCfg);

    return SaveToFile();
}

BOOL LocalTableCfg::CreateNewFile(CHAR *pchFile)
{
    HANDLE hFile     = NULL;
    BOOL   bRet      = FALSE;
    INT32  iWriteLen = 0;

    hFile = gos_fopen(pchFile, "wb");
    if (hFile == NULL)
    {
        return FALSE;
    }

    iWriteLen = fprintf(gos_get_fp(hFile), "<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n<TableCfgList>\r\n");
    if (iWriteLen <= 0)
    {
        goto END;
    }

    iWriteLen = fprintf(gos_get_fp(hFile), "</TableCfgList>");
    if (iWriteLen <= 0)
    {
        goto END;
    }

    bRet = TRUE;
END:
    gos_fclose(hFile);

    return bRet;
}

BOOL LocalTableCfg::Load()
{
    GXmlDoc    *pDoc              = GXmlDoc::New();
    GXmlNode   *pRootNode         = NULL;
    GXmlNode   *pListNode         = NULL;
    GXmlNode   *pDisplayNode      = NULL;
    GXmlNode   *pColumnNode       = NULL;
    GXmlNode   *pPrintCfgNode     = NULL;
    GXmlNode   *pPrintCfgItemNode = NULL;
    CHAR       *pchName           = NULL;
    UINT32      ulWidth           = 0;
    UINT32      ulID              = 0;
    BOOL        bRet              = FALSE;
    CHAR        acFile[256];
    TABLE_CFG_T stTableCfg;

    sprintf(acFile, "%s/config/local_table_cfg.xml", gos_get_root_path());

    if (!gos_file_exist(acFile))
    {
        if (!CreateNewFile(acFile))
        {
            return FALSE;
        }
    }

    if (!pDoc)
    {
        GosLog(LOG_ERROR, "Invalid pDoc.(LocalTableCfg-Load)");

        return FALSE;
    }

    if (!pDoc->ParseFile(acFile))
    {
        GosLog(LOG_ERROR, "Cannnot open file(%s)", acFile);

        goto END;
    }

    pRootNode = pDoc->GetRootNode();
    if (pRootNode == NULL)
    {
        return FALSE;
    }

    pListNode = pRootNode->GetFirstChild("List");
    while (pListNode != NULL)
    {
        memset(&stTableCfg, 0, sizeof(stTableCfg));

        pDisplayNode = pListNode->GetFirstChild("Display");
        if (pDisplayNode == NULL)
        {
            return FALSE;
        }

        pColumnNode = pDisplayNode->GetFirstChild("Column");
        while (pColumnNode != NULL)
        {
            if (!pColumnNode->GetAttrValue("ID", &ulID))
            {
                return FALSE;
            }

            if (ulID > ARRAY_SIZE(stTableCfg.auDisplayColumnWidth)-1)
            {
                return FALSE;
            }

            if (!pColumnNode->GetAttrValue("Width", &ulWidth))
            {
                return FALSE;
            }

            stTableCfg.auDisplayColumnWidth[ulID] = ulWidth;

            stTableCfg.usColumnNum ++;

            pColumnNode = pColumnNode->GetSibling();
        }

        pPrintCfgNode = pListNode->GetFirstChild("PrintCfg");
        if (pPrintCfgNode == NULL)
        {
            return FALSE;
        }

        pPrintCfgItemNode = pPrintCfgNode->GetFirstChild("Title");
        if (pPrintCfgItemNode == NULL)
        {
            return FALSE;
        }

        if (!pPrintCfgItemNode->GetAttrValue("FontSize", &stTableCfg.stPrintTitleCfg.ulFontSize))
        {
            return FALSE;
        }

        if (!pPrintCfgItemNode->GetAttrValue("Bold", &stTableCfg.stPrintTitleCfg.ulBold))
        {
            return FALSE;
        }

        if (!pPrintCfgItemNode->GetAttrValue("Height", &stTableCfg.stPrintTitleCfg.ulHeight))
        {
            return FALSE;
        }

        pPrintCfgItemNode = pPrintCfgNode->GetFirstChild("Foot");
        if (pPrintCfgItemNode == NULL)
        {
            return FALSE;
        }

        if (!pPrintCfgItemNode->GetAttrValue("FontSize", &stTableCfg.stPrintFootCfg.ulFontSize))
        {
            return FALSE;
        }

        if (!pPrintCfgItemNode->GetAttrValue("Bold", &stTableCfg.stPrintFootCfg.ulBold))
        {
            return FALSE;
        }

        if (!pPrintCfgItemNode->GetAttrValue("Height", &stTableCfg.stPrintFootCfg.ulHeight))
        {
            return FALSE;
        }

        pPrintCfgItemNode = pPrintCfgNode->GetFirstChild("PageMargin");
        if (pPrintCfgItemNode == NULL)
        {
            return FALSE;
        }

        if (!pPrintCfgItemNode->GetAttrValue("Top", &stTableCfg.stPageMargin.ulTop))
        {
            return FALSE;
        }

        if (!pPrintCfgItemNode->GetAttrValue("Bottom", &stTableCfg.stPageMargin.ulBottom))
        {
            return FALSE;
        }

        if (!pPrintCfgItemNode->GetAttrValue("Left", &stTableCfg.stPageMargin.ulLeft))
        {
            return FALSE;
        }

        if (!pPrintCfgItemNode->GetAttrValue("Right", &stTableCfg.stPageMargin.ulRight))
        {
            return FALSE;
        }

        pPrintCfgItemNode = pPrintCfgNode->GetFirstChild("PageOrientation");
        if (pPrintCfgItemNode == NULL)
        {
            return FALSE;
        }

        if (!pPrintCfgItemNode->GetAttrValue("Orientation", &stTableCfg.usPageOientation))
        {
            return FALSE;
        }

        pchName = pListNode->GetAttrValue("Name");
        if (pchName == NULL)
        {
            return FALSE;
        }

        stTableCfg.pszName = strdup(pchName);

        m_vstTableCfg.push_back(stTableCfg);

        pListNode = pListNode->GetSibling();
    }

    bRet = TRUE;
END:
    GXmlDoc::Delete(pDoc);

    return bRet;
}

BOOL LocalTableCfg::ReLoad()
{
    m_vstTableCfg.clear();

    return Load();
}

BOOL LocalTableCfg::SaveToFile()
{
    HANDLE hFile      = NULL;
    INT32  iWriteLen  = 0;
    BOOL   bRet       = FALSE;
    UINT32 i          = 0;
    UINT16 j          = 0;
    CHAR   acFilePath[256];

    sprintf(acFilePath, "%s/config/local_table_cfg.xml", gos_get_root_path());

    hFile = gos_fopen(acFilePath, "wb");
    if (hFile == NULL)
    {
        return FALSE;
    }

    iWriteLen = fprintf(gos_get_fp(hFile), "<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n<TableCfgList>\r\n");
    if (iWriteLen <= 0)
    {
        goto END;
    }

    for (i=0; i<m_vstTableCfg.size(); i++)
    {
        iWriteLen = fprintf(gos_get_fp(hFile), "<List Name=\"%s\">\r\n", m_vstTableCfg[i].pszName);
        if (iWriteLen <= 0)
        {
            goto END;
        }

        iWriteLen = fprintf(gos_get_fp(hFile), "<Display>\r\n", m_vstTableCfg[i].pszName);
        if (iWriteLen <= 0)
        {
            goto END;
        }

        for (j=0; j<m_vstTableCfg[i].usColumnNum; j++)
        {
            iWriteLen = fprintf(gos_get_fp(hFile), "<Column ID=\"%u\" Width=\"%u\"/>\r\n", j, m_vstTableCfg[i].auDisplayColumnWidth[j]);
            if (iWriteLen <= 0)
            {
                goto END;
            }
        }

        iWriteLen = fprintf(gos_get_fp(hFile), "</Display>\r\n", m_vstTableCfg[i].pszName);
        if (iWriteLen <= 0)
        {
            goto END;
        }

        iWriteLen = fprintf(gos_get_fp(hFile), "<PrintCfg>\r\n");
        if (iWriteLen <= 0)
        {
            goto END;
        }

        iWriteLen = fprintf(gos_get_fp(hFile), "<Title FontSize=\"%u\" Bold=\"%u\" Height=\"%u\"/>\r\n",
                            m_vstTableCfg[i].stPrintTitleCfg.ulFontSize,
                            m_vstTableCfg[i].stPrintTitleCfg.ulBold,
                            m_vstTableCfg[i].stPrintTitleCfg.ulHeight);
        if (iWriteLen <= 0)
        {
            goto END;
        }

        iWriteLen = fprintf(gos_get_fp(hFile), "<Foot FontSize=\"%u\" Bold=\"%u\" Height=\"%u\"/>\r\n",
                            m_vstTableCfg[i].stPrintFootCfg.ulFontSize,
                            m_vstTableCfg[i].stPrintFootCfg.ulBold,
                            m_vstTableCfg[i].stPrintFootCfg.ulHeight);
        if (iWriteLen <= 0)
        {
            goto END;
        }

        iWriteLen = fprintf(gos_get_fp(hFile), "<PageMargin Top=\"%u\" Bottom=\"%u\" Left=\"%u\" Right=\"%u\"/>\r\n",
                            m_vstTableCfg[i].stPageMargin.ulTop,
                            m_vstTableCfg[i].stPageMargin.ulBottom,
                            m_vstTableCfg[i].stPageMargin.ulLeft,
                            m_vstTableCfg[i].stPageMargin.ulRight);
        if (iWriteLen <= 0)
        {
            goto END;
        }

        iWriteLen = fprintf(gos_get_fp(hFile), "<PageOrientation Orientation=\"%u\"/>\r\n", m_vstTableCfg[i].usPageOientation);
        if (iWriteLen <= 0)
        {
            goto END;
        }

        iWriteLen = fprintf(gos_get_fp(hFile), "</PrintCfg>\r\n");
        if (iWriteLen <= 0)
        {
            goto END;
        }

        iWriteLen = fprintf(gos_get_fp(hFile), "</List>\r\n");
        if (iWriteLen <= 0)
        {
            goto END;
        }
    }

    iWriteLen = fprintf(gos_get_fp(hFile), "</TableCfgList>");
    if (iWriteLen <= 0)
    {
        goto END;
    }

    bRet = TRUE;
END:
    gos_fclose(hFile);

    return bRet;
}

AnsiString GetTableRegKey(TcxCustomTreeList *table)
{
    CHAR        acProcName[256] = {0};

    if (!gos_get_proc_name(acProcName, FALSE))
    {
        strcpy(acProcName, "default");
    }

    AnsiString  strAppName = acProcName;
    AnsiString  strKey = g_strRegKey + strAppName;

    strKey += AnsiString("\\") + table->Owner->Name;
    strKey += AnsiString("\\width_") + table->Name;

    return strKey;
}

void LoadTableInfo(TcxCustomTreeList *table)
{
    TRegistry   *Reg = new TRegistry;
    AnsiString  strKey = GetTableRegKey(table);
    AnsiString  strName;
    int         iWidth;

    try
    {
        Reg->RootKey = HKEY_LOCAL_MACHINE;
        if (Reg->OpenKey(strKey, false))
        {
            for (INT32 i=0; i<table->ColumnCount; i++)
            {
                strName = table->Columns[i]->Name;
                if (Reg->ValueExists(strName))
                {
                    iWidth = Reg->ReadInteger(strName);
                    if (iWidth != table->Columns[i]->Width)
                    {
                        table->Columns[i]->Width = iWidth;
                    }
                }
            }
            Reg->CloseKey();
        }
    }
    __finally
    {
        delete Reg;
    }
}

void SaveTableInfo(TcxCustomTreeList *table)
{
    TRegistry   *Reg = new TRegistry;
    AnsiString  strKey = GetTableRegKey(table);
    AnsiString  strName;

    try
    {
        Reg->RootKey = HKEY_LOCAL_MACHINE;
        Reg->LazyWrite = false;

        if (Reg->OpenKey(strKey, true))
        {
            for (INT32 i=0; i<table->ColumnCount; i++)
            {
                strName = table->Columns[i]->Name;
                Reg->WriteInteger(strName, table->Columns[i]->Width);
            }
            Reg->CloseKey();
        }
    }
    __finally
    {
        delete Reg;
    }
}

bool LoadConfInfo(AnsiString strItemName, AnsiString strKey, INT32 &iValue)
{
    TRegistry   *Reg = new TRegistry;
    CHAR        acProcName[256] = {0};
    bool        bRet = false;

    if(!gos_get_proc_name(acProcName, FALSE))
    {
        strcpy(acProcName, "default");
    }

    AnsiString  strAppName = acProcName;
    AnsiString  strRegKey = g_strRegKey + strAppName;
    AnsiString  strName;

    strRegKey += AnsiString("\\") + strItemName;

    try
    {
        Reg->RootKey = HKEY_LOCAL_MACHINE;
        if (Reg->OpenKey(strRegKey, false))
        {
            if(Reg->ValueExists(strKey))
            {
                iValue = Reg->ReadInteger(strKey);
                bRet = true;
            }
            Reg->CloseKey();
        }
    }
    __finally
    {
        delete Reg;
    }

    return bRet;
}

bool LoadConfInfo(AnsiString strItemName, AnsiString strKey, INT32 *piValue)
{
    INT32   iValue;

    if (!LoadConfInfo(strItemName, strKey, iValue))
    {
        return false;
    }

    *piValue = iValue;

    return true;
}

bool LoadConfInfo(AnsiString strItemName, AnsiString strKey, INT32 *piValue, INT32 iMin, INT32 iMax)
{
    INT32   iValue;

    if (!LoadConfInfo(strItemName, strKey, iValue))
    {
        return false;
    }

    if (iValue < iMin)
    {
        iValue = iMin;
    }

    if (iValue > iMax)
    {
        iValue = iMax;
    }

    *piValue = iValue;

    return true;
}

/**
 * @brief           读取 windows 注册表中的配置项
 * @param           strItemName     [in]
 * @param           strKey          [in]
 * @return          AnsiString 读取到的配置项的值
 * @author          leijie(leijie@gbcom.com.cn)
 * @date            2022-02-14 09:17:46
 * @note
 */
AnsiString LoadConfInfo(AnsiString strItemName, AnsiString strKey)
{
    TRegistry   *Reg = new TRegistry;
    CHAR        acProcName[256] = {0};
    AnsiString  strValue = "";

    if(!gos_get_proc_name(acProcName, FALSE))
    {
        strcpy(acProcName, "default");
    }

    AnsiString  strName;
    AnsiString  strAppName = acProcName;
    AnsiString  strRegKey = g_strRegKey + strAppName;

    strRegKey += AnsiString("\\") + strItemName;

    try
    {
        Reg->RootKey = HKEY_LOCAL_MACHINE;
        if (Reg->OpenKey(strRegKey, false))
        {
            if(Reg->ValueExists(strKey))
            {
                strValue = Reg->ReadString(strKey);
            }
            Reg->CloseKey();
        }
    }
    __finally
    {
        delete Reg;
    }

    if(strValue.IsEmpty())
    {
        GosLog(LOG_ERROR, "load cfg failed! ItemName: %s, Key: %s", strItemName.c_str(), strKey.c_str());
    }

    return strValue;
}

/**
 * @brief           保存 windows 注册表中的配置项
 * @param           strItemName     [in]
 * @param           strKey          [in]   配置项主键
 * @param           iValue          [in]   配置项的值
 * @author          leijie(leijie@gbcom.com.cn)
 * @date            2022-02-14 09:19:07
 * @note
 */
void SaveConfInfo(AnsiString strItemName, AnsiString strKey, INT32 iValue)
{
    TRegistry   *Reg = new TRegistry;
    CHAR        acProcName[256] = {0};

    if(!gos_get_proc_name(acProcName, FALSE))
    {
        strcpy(acProcName, "default");
    }

    AnsiString  strAppName = acProcName;
    AnsiString  strRegKey = g_strRegKey + strAppName;

    strRegKey += AnsiString("\\") + strItemName;

    try
    {
        Reg->RootKey = HKEY_LOCAL_MACHINE;
        Reg->LazyWrite = false;

        if (Reg->OpenKey(strRegKey, true))
        {
            Reg->WriteInteger(strKey, iValue);

            Reg->CloseKey();
        }
    }
    __finally
    {
        delete Reg;
    }
}

void SaveConfInfo(AnsiString strItemName, AnsiString strKey, AnsiString strValue)
{
    TRegistry   *Reg = new TRegistry;
    CHAR        acProcName[256] = {0};

    if(!gos_get_proc_name(acProcName, FALSE))
    {
        strcpy(acProcName, "default");
    }

    AnsiString  strAppName = acProcName;
    AnsiString  strRegKey = g_strRegKey + strAppName;

    strRegKey += AnsiString("\\") + strItemName;

    try
    {
        Reg->RootKey = HKEY_LOCAL_MACHINE;
        Reg->LazyWrite = false;

        if (Reg->OpenKey(strRegKey, true))
        {
            Reg->WriteString(strKey, strValue);

            Reg->CloseKey();
        }
    }
    __finally
    {
        delete Reg;
    }
}

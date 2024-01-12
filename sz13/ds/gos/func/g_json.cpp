#ifdef __BORLANDC__
#include <vcl.h>
#endif

#include "g_include.h"

//#define CVT_JSON_TEXT

// 判断后面是否是:，}]
static BOOL IsEndQuotationMark(CHAR *szJson)
{
    for (; *szJson; szJson++)
    {
        if (gos_isspace(*szJson))
        {
            continue;
        }

        if (*szJson == '"')
        {
            return FALSE;
        }

        if (*szJson == ':' ||
            *szJson == ',' ||
            *szJson == '}' ||
            *szJson == ']' )
        {
            return TRUE;
        }
    }

    return FALSE;
}

// 去除头尾双引号
static inline CHAR* RemoveQuotationMark(CHAR *szText)
{
    UINT32  ulLen = strlen(szText);

    if (ulLen >= 2 &&
        szText[0] == '"' &&
        szText[ulLen-1] == '"')
    {
        szText[ulLen-1] = '\0';
        szText++;
    }

    szText = gos_trim_string(szText);

    return szText;
}

// 转换"&quot;"，去除头尾双引号
static inline CHAR* ConvertStringValue(CHAR *szText)
{
    UINT32  ulLen = strlen(szText);

    if (ulLen >= 2 &&
        szText[0] == '"' &&
        szText[ulLen-1] == '"')
    {
        szText[ulLen-1] = '\0';
        szText++;
        ulLen -= 2;
    }

#ifdef CVT_JSON_TEXT
    // 转换"&quot;"
    static CHAR     *szMark = (CHAR*)"&quot;";
    static UINT32   ulMarkLen = strlen(szMark);
    CHAR            *szStart = strstr(szText, szMark);
    CHAR            *szEnd = szText+ulLen;
    CHAR            *szNext;

    while(1)
    {
        if (!szStart)
        {
            break;
        }

        szNext = szStart+ulMarkLen;
        *szStart++ = '"';
        memmove(szStart, szNext, szEnd-szNext+1);

        szStart = strstr(szStart, szMark);
    }
#endif

    szText = gos_trim_string(szText);

    return szText;
}

VOID GJson::Init()
{
    m_szJson = NULL;
    m_szOriJson = NULL;
    m_bParseSucc = FALSE;
    m_strCurrKey = "";

    m_KeyValue = new GList(16, 0, sizeof(JSON_KEY_VALUE_T));
}

GJson::GJson()
{
    Init();
}

GJson::GJson(std::string strJson)
{
    Init();

    Parse((CHAR*)strJson.c_str());
}

#ifdef __BORLANDC__
GJson::GJson(const String& strJson)
{
    Init();

    AnsiString  strTmpJson = strJson;

    Parse(strTmpJson.c_str());
}

GJson::GJson(const AnsiString& strJson)
{
    Init();

    Parse(strJson.c_str());
}
#endif

GJson::GJson(CHAR *szJson)
{
    Init();

    Parse(szJson);
}

GJson::~GJson()
{
    for (UINT32 i=0; i<m_vSubJson.size(); i++)
    {
        GJson   *SubJson = m_vSubJson[i];

        delete SubJson;
    }

//    GOS_FREE(m_szJson);
    GOS_FREE(m_szOriJson);

    if (m_KeyValue)
    {
        delete m_KeyValue;
    }
}

CHAR *GJson::GetText()
{
    if (!m_szOriJson)
    {
        return (CHAR*)"";
    }

    return m_szOriJson;
}

BOOL GJson::ParseFile(const CHAR *szFile)
{
    CHAR    *szText = NULL;
    HANDLE  hFile = NULL;
    BOOL    bRet = FALSE;
    INT32   iFileLen = gos_get_file_size(szFile);

    m_strCurrKey = "";

    if (iFileLen <= 0)
    {
        return FALSE;
    }

    hFile = gos_fopen(szFile, "rb");
    if (!hFile)
    {
        return FALSE;
    }

    szText = (CHAR*)calloc(1, iFileLen+1);
    if (!szText)
    {
        goto end;
    }

    if ((INT32)iFileLen != gos_fread(hFile, szText, iFileLen))
    {
        goto end;
    }

    *(szText+iFileLen) = '\0';
    bRet = Parse(szText);

end:
    gos_fclose(hFile);

    GOS_FREE(szText);

    return bRet;
}

BOOL GJson::ParseArray(const std::string &strJson)
{
    return ParseArray((CHAR *)strJson.c_str());
}

BOOL GJson::ParseArray(CHAR *szJsonText)
{
    UINT32      ulLen;
    CHAR        *szJson;
    CHAR        *szTmpJson;

    m_strCurrKey = "";
//    GOS_FREE(m_szJson);
    GOS_FREE(m_szOriJson);

    m_bParseSucc = FALSE;
    m_vSubJson.clear();
//    m_vKeyValue.clear();
    m_KeyValue->Clear();

    if (!szJsonText)
    {
        return FALSE;
    }

#if 0
    szTmpJson = strdup(szJsonText);

    szJson = gos_trim_string(szTmpJson);
    m_szJson = strdup(szJson);
    m_szOriJson = strdup(szJson);
    free(szTmpJson);

    ulLen = strlen(m_szJson);
    if (ulLen < 2)
    {
        return FALSE;
    }
#else
    szJson = gos_left_trim_string(szJsonText);
    ulLen = strlen(szJson);
    if (ulLen < 2)
    {
        return FALSE;
    }

    for (szTmpJson=szJson+ulLen-1; szTmpJson>=szJson; szTmpJson--, ulLen--)
    {
        if (!gos_isspace(*szTmpJson))
        {
            break;
        }
    }

    if (ulLen < 2)
    {
        return FALSE;
    }

    m_szOriJson = (CHAR*)malloc(2*(ulLen+1));
    memcpy(m_szOriJson, szJson, ulLen);
    m_szOriJson[ulLen] = '\0';

    m_szJson = m_szOriJson+ulLen+1;
    memcpy(m_szJson, szJson, ulLen);
    m_szJson[ulLen] = '\0';
#endif

    if (m_szJson[0] != '[' ||
        m_szJson[ulLen-1] != ']')
    {
        return FALSE;
    }

    if (ulLen == 2)
    {
        m_bParseSucc = TRUE;
        return TRUE;
    }

    m_szJson[ulLen-1] = ',';
    szJson = m_szJson+1;
    ulLen -= 2;

    while(1)
    {
        if (szJson[0] == '\0')
        {
            break;
        }

        int iNum1 = 0;
        int iNum2Left = 0;
        int iNum2Right = 0;
        int iNum3Left = 0;
        int iNum3Right = 0;
        int iPosEnd = -1;

        // locate ','
        for (UINT32 i=0; szJson[i]; i++)
        {
            if (szJson[i] == '"')
            {
                if (iNum1 == 0)
                {
                    iNum1 = 1;
                }
                else
                {
                    // 判断后面是否是:，}]
                    if (IsEndQuotationMark(szJson+i+1))
                    {
                        iNum1 = 0;
                        continue;
                    }
                }
            }

            if (iNum1 == 1)
            {
                continue;
            }

            if (szJson[i] == '{')
            {
                iNum2Left++;
            }
            else if (szJson[i] == '}')
            {
                iNum2Right++;
            }
            else if (szJson[i] == '[')
            {
                iNum3Left++;
            }
            else if (szJson[i] == ']')
            {
                iNum3Right++;
            }
            else if (szJson[i] == ',')
            {
                if (iNum2Left == iNum2Right &&
                    iNum3Left == iNum3Right )
                {
                    iPosEnd = i;
                    break;
                }
            }
        }

        if (iNum1 != 0 ||
            iNum2Left != iNum2Right ||
            iNum3Left != iNum3Right ||
            iPosEnd < 0)
        {
            return FALSE;
        }

        szJson[iPosEnd] = '\0';

        GJson   *SubJson = new GJson();

        if (!SubJson->Parse(szJson))
        {
            delete SubJson;
            return FALSE;
        }

        m_vSubJson.push_back(SubJson);

        szJson += (iPosEnd+1);
        szJson = gos_left_trim_string(szJson);
    }

    m_bParseSucc = TRUE;

    return TRUE;
}

BOOL GJson::Parse(const std::string& str)
{
    return Parse((CHAR*)str.c_str());
}

BOOL GJson::Parse(CHAR *szJsonText)
{
    UINT32      ulLen;
    CHAR        *szJson;
    CHAR        *szTmpJson;

    m_strCurrKey = "";
 //   GOS_FREE(m_szJson);
    GOS_FREE(m_szOriJson);

    m_bParseSucc = FALSE;
    m_vSubJson.clear();
//    m_vKeyValue.clear();
    m_KeyValue->Clear();

    if (!szJsonText)
    {
        return FALSE;
    }

#if 0
    szTmpJson = strdup(szJsonText);

    szJson = gos_trim_string(szTmpJson);
    m_szOriJson = strdup(szJson);
    m_szJson = strdup(szJson);
    free(szTmpJson);

    ulLen = strlen(m_szJson);
    if (ulLen < 2)
    {
        return FALSE;
    }
#else

    szJson = gos_left_trim_string(szJsonText);
    ulLen = strlen(szJson);
    if (ulLen < 2)
    {
        return FALSE;
    }

    for (szTmpJson=szJson+ulLen-1; szTmpJson>=szJson; szTmpJson--, ulLen--)
    {
        if (!gos_isspace(*szTmpJson))
        {
            break;
        }
    }

    if (ulLen < 2)
    {
        return FALSE;
    }

    m_szOriJson = (CHAR*)malloc(2*(ulLen+1));
    memcpy(m_szOriJson, szJson, ulLen);
    m_szOriJson[ulLen] = '\0';

    m_szJson = m_szOriJson+ulLen+1;
    memcpy(m_szJson, szJson, ulLen);
    m_szJson[ulLen] = '\0';
#endif

    if (m_szJson[0] == '[' &&
        m_szJson[ulLen-1] == ']')
    {
        return ParseArray(szJsonText);
    }

    if (m_szJson[0] != '{' ||
        m_szJson[ulLen-1] != '}')
    {
        return FALSE;
    }

    // {"a":"","b":"","c":{}}
    // or [{},{}]
    // remove '{' and '}'
    m_szJson[ulLen-1] = ',';
    szJson = gos_left_trim_string(m_szJson+1);
    if (strcmp(szJson, ",") == 0)
    {
        *szJson = '\0';
    }

    while(1)
    {
        if (szJson[0] == '\0')
        {
            break;
        }

        int iNum1 = 0;
        int iNum2Left = 0;
        int iNum2Right = 0;
        int iNum3Left = 0;
        int iNum3Right = 0;
        int iNum4 = 0;
        int iPosMid = -1;
        int iPosEnd = -1;
        JSON_KEY_VALUE_T    stKeyValue = {NULL, NULL};

        // locate ','
       // ulLen = strlen(szJson);
        //for (UINT32 i=0; i<=ulLen; i++)
        for (UINT32 i=0; szJson[i]; i++)
        {
            if (szJson[i] == '"')
            {
                if (iNum1 == 0)
                {
                    iNum1 = 1;
                }
                else
                {
                    // 判断后面是否是:，}]
                    if (IsEndQuotationMark(szJson+i+1))
                    {
                        iNum1 = 0;
                        continue;
                    }
                }
            }

            if (iNum1 == 1)
            {
                continue;
            }

            if (szJson[i] == '{')
            {
                iNum2Left++;
            }
            else if (szJson[i] == '}')
            {
                iNum2Right++;
            }
            else if (szJson[i] == '[')
            {
                iNum3Left++;
            }
            else if (szJson[i] == ']')
            {
                iNum3Right++;
            }
            else if (szJson[i] == ':')
            {
                if (iNum2Left == iNum2Right &&
                    iNum3Left == iNum3Right )
                {
                    iNum4++;
                    iPosMid = i;
                }
            }
            else if (szJson[i] == ',')
            {
                if (iNum2Left == iNum2Right &&
                    iNum3Left == iNum3Right )
                {
                    iPosEnd = i;
                    break;
                }
            }
        }

        if (iNum4 != 1 ||
            iNum1 != 0 ||
            iNum2Left != iNum2Right ||
            iNum3Left != iNum3Right ||
            iPosMid <= 1 ||
            iPosMid >= iPosEnd)
        {
//            m_vKeyValue.clear();
            m_KeyValue->Clear();
            return FALSE;
        }

        stKeyValue.szKey = szJson;
        szJson[iPosMid] = '\0';

        stKeyValue.szValue = szJson + iPosMid + 1;
        szJson[iPosEnd] = '\0';

        szJson += (iPosEnd+1);
        szJson = gos_left_trim_string(szJson);
        if (*szJson == '\0')
        {
            ulLen = strlen(stKeyValue.szValue);
            if (ulLen > 0 && stKeyValue.szValue[ulLen-1] == ',')
            {
                stKeyValue.szValue[ulLen-1] = '\0';
            }
        }

        stKeyValue.szKey = RemoveQuotationMark(stKeyValue.szKey);
        stKeyValue.szValue = RemoveQuotationMark(stKeyValue.szValue);
//        m_vKeyValue.push_back(stKeyValue);
        m_KeyValue->Add(&stKeyValue, sizeof(stKeyValue));
    }

    m_bParseSucc = TRUE;
    return TRUE;
}

BOOL GJson::GetValue(const CHAR *szKey, CHAR *szValue, UINT32 ulMaxLen)
{
    CHAR    *szTmp = GetValue(szKey);

    if (!szTmp)
    {
        return FALSE;
    }

    if (strlen(szTmp) >= ulMaxLen)
    {
        return FALSE;
    }

    strcpy(szValue, szTmp);
    return TRUE;
}

CHAR *GJson::GetValue(const CHAR *szKey)
{
/*    for (UINT32 i=0; i<m_vKeyValue.size(); i++)
    {
        if (strcmp(szKey, m_vKeyValue[i].szKey) == 0)
        {
            return m_vKeyValue[i].szValue;
        }
    }
    */

    if (!szKey)
    {
        m_strCurrKey = "";
        return FALSE;
    }

    m_strCurrKey = szKey;

    GLISTNODE   *pNode = m_KeyValue->GetHeadNode();
    JSON_KEY_VALUE_T    *pstValue;

    for(; pNode; pNode=m_KeyValue->GetNextNode(pNode))
    {
        pstValue = (JSON_KEY_VALUE_T*)m_KeyValue->GetNodeData(pNode);
        if (pstValue && strcmp(szKey, pstValue->szKey) == 0)
        {
            return pstValue->szValue;
        }
    }

    return NULL;
}

#ifdef __BORLANDC__
BOOL GJson::GetValue(const CHAR *szKey, AnsiString &strValue)
{
    CHAR    *szValue = GetValue(szKey);

    if (!szValue)
    {
        return FALSE;
    }

    strValue = szValue;
    return TRUE;
}
#endif

BOOL GJson::GetValue(const CHAR *szKey, std::string &strValue)
{
    CHAR    *szValue = GetValue(szKey);

    if (!szValue)
    {
        return FALSE;
    }

    strValue = szValue;
    return TRUE;
}

BOOL GJson::GetValue(const CHAR *szKey, bool& bRet)
{
    CHAR *szValue = GetValue(szKey);

    if (!szValue)
    {
        return FALSE;
    }

    bRet = (strcmp(szValue, "true") == 0);
    return TRUE;
}

BOOL GJson::GetBoolValue(const CHAR *szKey, BOOL bDefault)
{
    CHAR    *szValue = GetValue(szKey);

    if (!szValue)
    {
        return bDefault;
    }

    if (strcasecmp(szValue, "true") == 0)
    {
        return TRUE;
    }
    else if (strcasecmp(szValue, "false") == 0)
    {
        return FALSE;
    }

    return bDefault;
}

BOOL GJson::GetBool(const CHAR *szKey, BOOL &bOut)
{
    CHAR    *szValue = GetValue(szKey);

    if (!szValue)
    {
        return FALSE;
    }

    if (strcasecmp(szValue, "true") == 0)
    {
        bOut = TRUE;
        return TRUE;
    }
    else if (strcasecmp(szValue, "false") == 0)
    {
        bOut = FALSE;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GJson::GetValue(const CHAR *szKey, INT32 &iValue)
{
    return GetValue(szKey, &iValue);
}

BOOL GJson::GetValue(const CHAR *szKey, UINT32 &ulValue)
{
    return GetValue(szKey, &ulValue);
}

BOOL GJson::GetValue(const CHAR *szKey, INT32 *piValue)
{
    CHAR    *szValue = GetValue(szKey);

    return gos_atoi(szValue, piValue);
}

BOOL GJson::GetValue(const CHAR *szKey, UINT32 *pulValue)
{
    CHAR    *szValue = GetValue(szKey);

    return gos_atou(szValue, pulValue);
}

BOOL GJson::GetValue(const CHAR *szKey, INT16 *psValue)
{
    INT32   iValue;

    if (!GetValue(szKey, &iValue))
    {
        return FALSE;
    }

    if (iValue > 0x7fff || iValue < -0x8000)
    {
        return FALSE;
    }

    *psValue = (INT16)iValue;

    return TRUE;
}

BOOL GJson::GetValue(const CHAR *szKey, UINT16 *pusValue)
{
    UINT32      ulValue;

    if (!GetValue(szKey, &ulValue))
    {
        return FALSE;
    }

    if (ulValue > 0xffff)
    {
        return FALSE;
    }

    *pusValue = (UINT16)ulValue;

    return TRUE;
}

BOOL GJson::GetValue(const CHAR *szKey, INT8 *pcValue)
{
    INT32   iValue;

    if (!GetValue(szKey, &iValue))
    {
        return FALSE;
    }

    if (iValue > 0x7f || iValue < -0x80)
    {
        return FALSE;
    }

    *pcValue = (INT8)iValue;

    return TRUE;
}

BOOL GJson::GetValue(const CHAR *szKey, UINT8 *pucValue)
{
    UINT32      ulValue;

    if (!GetValue(szKey, &ulValue))
    {
        return FALSE;
    }

    if (ulValue > 0xff)
    {
        return FALSE;
    }

    *pucValue = (UINT8)ulValue;

    return TRUE;
}

BOOL GJson::GetValue(const CHAR *szKey, INT64 &i64Value)
{
    return GetValue(szKey, &i64Value);
}

BOOL GJson::GetValue(const CHAR *szKey, INT64 *pi64Value)
{
    CHAR    *szValue = GetValue(szKey);

    return gos_atoi64(szValue, pi64Value);
}

BOOL GJson::GetValue(const CHAR *szKey, DOUBLE *pdValue)
{
    CHAR    *szValue = GetValue(szKey);

    return gos_atof(szValue, pdValue);
}

BOOL GJson::GetValue(const CHAR *szKey, UINT64 &u64Value)
{
    return GetValue(szKey, &u64Value);
}

BOOL GJson::GetValue(const CHAR *szKey, UINT64 *pu64Value)
{
    CHAR    *szValue = GetValue(szKey);

    return gos_atou64(szValue, pu64Value);
}

BOOL GJson::GetIP(const CHAR *szKey, UINT8 *pucValue)
{
    CHAR    *szValue = GetValue(szKey);

    return gos_get_ip(szValue, pucValue);
}

BOOL GJson::GetMAC(const CHAR *szKey, UINT8 *pucValue)
{
    CHAR    *szValue = GetValue(szKey);

    return gos_get_mac(szValue, pucValue);
}

VOID GJson::GetSubJson(std::vector<GJson *> &vSubJson)
{
    vSubJson.clear();
    vSubJson.assign(m_vSubJson.begin(), m_vSubJson.end());
}

VOID GJson::GetKeyValueList(std::vector<JSON_KEY_VALUE_T> &vKeyValueList)
{
    vKeyValueList.clear();
//    vKeyValueList.assign(m_vKeyValue.begin(), m_vKeyValue.end());

    GLISTNODE   *pNode = m_KeyValue->GetHeadNode();
    JSON_KEY_VALUE_T    *pstValue;

    for(; pNode; pNode=m_KeyValue->GetNextNode(pNode))
    {
        pstValue = (JSON_KEY_VALUE_T*)m_KeyValue->GetNodeData(pNode);
        if (pstValue)
        {
            vKeyValueList.push_back(*pstValue);
        }
    }
}

GJson* GJson::GetSubJson(UINT32 ulIndex)
{
    if (ulIndex >= m_vSubJson.size())
    {
        return NULL;
    }

    return m_vSubJson[ulIndex];
}

// GJsonParam
GJsonParam::GJsonParam()
{
}

GJsonParam::~GJsonParam()
{
    Clear();
}

VOID GJsonParam::Clear()
{
    m_strParam.Clear();
    m_strValue.Clear();
}

CHAR* GJsonParam::GetString()
{
    m_strValue.Clear();
    m_strValue.Append("{");
    m_strValue.Append(m_strParam.GetString());
    m_strValue.Append("}");

    return m_strValue.GetString();
}

VOID GJsonParam::Add(const CHAR *szName, GJsonTupleParam *JsonTupleParam)
{
    CHAR    *szValue = JsonTupleParam->GetString();

    Add(szName, szValue, FALSE);
}

VOID GJsonParam::Add(const CHAR *szName, GJsonTupleParam &JsonTupleParam)
{
    Add(szName, &JsonTupleParam);
}

VOID GJsonParam::Add(const CHAR *szName, GJsonParam *JsonParam)
{
    CHAR    *szValue = JsonParam->GetString();

    Add(szName, szValue, FALSE);
}

VOID GJsonParam::Add(const CHAR *szName, GJsonParam &JsonParam)
{
    Add(szName, &JsonParam);
}

VOID GJsonParam::Add(const CHAR *szName, const CHAR *szValue, BOOL bString)
{
    if (m_strParam.Length() > 0)
    {
        m_strParam.Append(",");
    }

    m_strParam.Append("\"");
    m_strParam.Append(szName);
    m_strParam.Append("\":");

    if (bString)
    {
        m_strParam.Append("\"");
    }

#ifdef CVT_JSON_TEXT
    // 替换双引号
    UINT32  ulLen;
    CHAR    *szMark = NULL;
    CHAR    *szStart = (CHAR*)szValue;

    if (bString)
    {
        szMark = (CHAR*)strchr(szStart, '"');
    }

    if (szMark != NULL)
    {
        while(1)
        {
            ulLen = (UINT32)(szMark-szStart);

            m_strParam.Append(szStart, ulLen, FALSE);
            m_strParam.Append("&quot;");
            szStart += (ulLen+1);

            szMark = (CHAR*)strchr(szStart, '"');
            if (!szMark)
            {
                m_strParam.Append(szStart);
                break;
            }
        }
    }
    else
#endif
    {
        m_strParam.Append(szValue);
    }

    if (bString)
    {
        m_strParam.Append("\"");
    }
}

VOID GJsonParam::Add(const CHAR *szName, CHAR *szValue, BOOL bString)
{
    Add(szName, (const CHAR*)szValue, bString);
}

#ifdef __BORLANDC__
VOID GJsonParam::Add(const CHAR *szName, AnsiString strValue, BOOL bString)
{
    const CHAR    *szValue = strValue.c_str();

    Add(szName, szValue, bString);
}

VOID GJsonParam::Add(const CHAR *szName, UnicodeString strValue, BOOL bString)
{
    AnsiString  strTmpValue = strValue;

    Add(szName, strTmpValue.c_str(), bString);
}

VOID GJsonParam::AddString(const CHAR *szName, AnsiString strValue, BOOL bString)
{
    const CHAR *szValue = strValue.c_str();

    Add(szName, szValue, bString);
}
#endif

VOID GJsonParam::Add(const CHAR *szName, const std::string &strValue, BOOL bString)
{
    const CHAR    *szValue = strValue.c_str();

    Add(szName, szValue, bString);
}

VOID GJsonParam::AddTuple(const CHAR *szName, CHAR *szValue)
{
    if (m_strParam.Length() > 0)
    {
        m_strParam.Append(",");
    }

    m_strParam.Append(szName);
    m_strParam.Append(":");
    m_strParam.Append(szValue);
}

VOID GJsonParam::Add(const CHAR *szName, UINT64 u64Value)
{
    CHAR    acValue[64];

    sprintf(acValue, FORMAT_U64, u64Value);

    Add(szName, acValue, FALSE);
}

VOID GJsonParam::Add(const CHAR *szName, INT64 i64Value)
{
    CHAR    acValue[64];

    sprintf(acValue, FORMAT_I64, i64Value);

    Add(szName, acValue, FALSE);
}

VOID GJsonParam::Add(const CHAR *szName, UINT32 ulValue)
{
    CHAR    acValue[64];

    sprintf(acValue, "%u", ulValue);

    Add(szName, acValue, FALSE);
}

VOID GJsonParam::Add(const CHAR *szName, INT32 iValue)
{
    CHAR    acValue[64];

    sprintf(acValue, "%d", iValue);

    Add(szName, acValue, FALSE);
}

VOID GJsonParam::Add(const CHAR *szName, UINT16 usValue)
{
    Add(szName, (UINT32)usValue);
}

VOID GJsonParam::Add(const CHAR *szName, INT16 sValue)
{
    Add(szName, (INT32)sValue);
}

VOID GJsonParam::Add(const CHAR *szName, UINT8 ucValue)
{
    Add(szName, (UINT32)ucValue);
}

VOID GJsonParam::Add(const CHAR *szName, INT8 cValue)
{
    Add(szName, (INT32)cValue);
}

VOID GJsonParam::Add(const CHAR *szName, DOUBLE dValue)
{
    CHAR    acText[128];

    sprintf(acText, "%f", dValue);
    Add(szName, acText);
}

VOID GJsonParam::Add(const CHAR *szName, bool bValue)
{
    CHAR acText[7] = {0};

    sprintf(acText, "%s", bValue?"true":"false");
    Add(szName, acText);
}

VOID GJsonParam::AddIP(const CHAR *szName, const UINT8 *pucValue)
{
    CHAR    acValue[64];

    sprintf(acValue, IP_FMT, IP_ARG(pucValue));

    Add(szName, acValue);
}

GJsonParam::GJsonParam(const GJsonParam& other)
{
    m_strParam = other.m_strParam;
    m_strValue = other.m_strValue;
}

GJsonParam& GJsonParam::operator=(const GJsonParam& other)
{
    if (&other != this)
    {
        m_strParam = other.m_strParam;
        m_strValue = other.m_strValue;
    }

    return *this;
}

// GJsonTupleParam
GJsonTupleParam::GJsonTupleParam(UINT32 ulSize)
{
    m_strParam.SetLength(ulSize);

    m_ulTupleNum = 0;
}

GJsonTupleParam::~GJsonTupleParam()
{
    Clear();
}

VOID GJsonTupleParam::Clear()
{
    m_ulTupleNum = 0;
    m_strValue.Clear();
    m_strParam.Clear();
}

VOID GJsonTupleParam::Add(GJsonParam *JsonParam)
{
    CHAR    *szValue = JsonParam->GetString();

    if (m_strParam.Length() > 0)
    {
        m_strParam.Append(",");
    }
    m_strParam.Append(szValue);

    m_ulTupleNum++;
}

VOID GJsonTupleParam::Add(const std::string &strJson)
{
    CHAR *szValue = (CHAR *)strJson.c_str();

    if (m_strParam.Length() > 0)
    {
        m_strParam.Append(",");
    }
    m_strParam.Append(szValue);

    m_ulTupleNum++;
}

VOID GJsonTupleParam::Add(GJsonParam &JsonParam)
{
    Add(&JsonParam);
}

CHAR* GJsonTupleParam::GetString()
{
    m_strValue.Clear();
    m_strValue.Append("[");
    m_strValue.Append(m_strParam.GetString());
    m_strValue.Append("]");

    return m_strValue.GetString();
}

UINT32 GJsonTupleParam::GetStringSize()
{
    return m_strParam.Length()+2;
}


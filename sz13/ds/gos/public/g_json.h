#ifndef G_JSON_H
#define G_JSON_H

#ifdef __BORLANDC__
#include <vcl.h>
#endif

#include "g_string.h"
#include "g_list.h"
#include "gos_json.h"
#include <string>

class GJson
{
public:
    GJson();
    GJson(std::string strGJson);
#ifdef __BORLANDC__
    GJson(const AnsiString& strGJson);
    GJson(const String& strGJson);
#endif
    GJson(CHAR *szGJson);

    ~GJson();

    GJson(GJson &other)
    {
        m_szJson = NULL;
        m_szOriJson = NULL;
        m_bParseSucc = FALSE;
        Parse(other.GetText());
    }

    GJson &operator=(GJson &other)
    {
        if (&other != this)
        {
            Parse(other.GetText());
        }
        return *this;
    }

    CHAR *GetText();

    BOOL ParseFile(const CHAR *szFile);
    BOOL Parse(CHAR *szGJson);
    BOOL Parse(const std::string &str);

    /// 该函数的功能在 Parse() 函数已经包含，推荐直接使用 Parse()
    BOOL ParseArray(CHAR *szGJson);
    /// 该函数的功能在 Parse() 函数已经包含，推荐直接使用 Parse()
    BOOL ParseArray(const std::string &strJson);

#ifdef __BORLANDC__
    BOOL GetValue(const CHAR *szKey, AnsiString &strValue);
#endif

    BOOL GetValue(const CHAR *szKey, std::string &strValue);

    CHAR* GetValue(const CHAR *szKey);
    BOOL GetValue(const CHAR *szKey, CHAR *szValue, UINT32 ulMaxLen);

    /// 该函数无法判断获取失败，不推荐使用
    BOOL GetBoolValue(const CHAR *szKey, BOOL bDefault=FALSE);
    /// 由于  GetBoolValue 函数并不合理， 设计这个函数来获取 BOOL 类型
    BOOL GetBool(const CHAR *szKey, BOOL &bOut);
    BOOL GetValue(const CHAR *szKey, bool &bValue);

    BOOL GetValue(const CHAR *szKey, INT32 *piValue);
    BOOL GetValue(const CHAR *szKey, UINT32 *pulValue);
    BOOL GetValue(const CHAR *szKey, INT32 &iValue);
    BOOL GetValue(const CHAR *szKey, UINT32 &ulValue);

    BOOL GetValue(const CHAR *szKey, INT16 *psValue);
    BOOL GetValue(const CHAR *szKey, UINT16 *pusValue);

    BOOL GetValue(const CHAR *szKey, INT8 *pcValue);
    BOOL GetValue(const CHAR *szKey, UINT8 *pucValue);

    BOOL GetValue(const CHAR *szKey, INT64 *pi64Value);
    BOOL GetValue(const CHAR *szKey, INT64 &i64Value);
    BOOL GetValue(const CHAR *szKey, DOUBLE *pdValue);

    BOOL GetValue(const CHAR *szKey, UINT64 *pu64Value);
    BOOL GetValue(const CHAR *szKey, UINT64 &u64Value);

    BOOL GetIP(const CHAR *szKey, UINT8 *pucValue);
    BOOL GetMAC(const CHAR *szKey, UINT8 *pucValue);

    std::vector<GJson*>& GetSubJson(){return m_vSubJson;};
    VOID GetSubJson(std::vector<GJson*> &vSubJson);
    UINT32 GetSubJsonCount(){return m_vSubJson.size();};
    GJson* GetSubJson(UINT32 ulIndex);

    VOID GetKeyValueList(std::vector<JSON_KEY_VALUE_T> &vKeyValueList);

    const CHAR *GetCurrKey(){return m_strCurrKey.c_str();};

private:
    BOOL    m_bParseSucc;
    CHAR    *m_szJson;
    CHAR    *m_szOriJson;
    std::string     m_strCurrKey;       // 当前GetValue的key

    VOID Init();

    //std::vector<JSON_KEY_VALUE_T>   m_vKeyValue;
    GList                       *m_KeyValue;
    std::vector<GJson*>         m_vSubJson;
};

class GJsonTupleParam;

class GJsonParam
{
public:
    GJsonParam();
    ~GJsonParam();
    GJsonParam(const GJsonParam &other);
    GJsonParam &operator=(const GJsonParam &other);

    VOID Clear();

    VOID Add(const CHAR *szName, CHAR *szValue, BOOL bString=TRUE);
    VOID Add(const CHAR *szName, const CHAR *szValue, BOOL bString=TRUE);
#ifdef __BORLANDC__
    VOID Add(const CHAR *szName, AnsiString strValue, BOOL bString=TRUE);
    VOID Add(const CHAR *szName, UnicodeString strValue, BOOL bString=TRUE);
    VOID AddString(const CHAR *szName, AnsiString strValue, BOOL bString = TRUE);
#endif
    /**
     * @brief           添加一个字符串的key-value
     * @param           szName      [in] key
     * @param           strValue    [in] value
     * @param           bString     [in] 用于设定该字符串前后是否需要添加双引号，TRUE 添加，FALSE 不添加
     * @return          VOID
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @note
     */
    VOID Add(const CHAR *szName, const std::string &strValue, BOOL bString = TRUE);

    VOID AddTuple(const CHAR *szName, CHAR *szValue);

    VOID Add(const CHAR *szName, UINT64 u64Value);
    VOID Add(const CHAR *szName, INT64 i64Value);
    VOID Add(const CHAR *szName, UINT32 ulValue);
    VOID Add(const CHAR *szName, INT32 iValue);
    VOID Add(const CHAR *szName, UINT16 ulValue);
    VOID Add(const CHAR *szName, INT16 iValue);
    VOID Add(const CHAR *szName, UINT8 ulValue);
    VOID Add(const CHAR *szName, INT8 iValue);
    VOID Add(const CHAR *szName, DOUBLE dValue);
    VOID Add(const CHAR *szName, bool bValue);
    VOID AddIP(const CHAR *szName, const UINT8 *pucValue);
    VOID Add(const CHAR *szName, GJsonTupleParam *JsonTupleParam);
    VOID Add(const CHAR *szName, GJsonTupleParam &JsonTupleParam);
    VOID Add(const CHAR *szName, GJsonParam *JsonParam);
    VOID Add(const CHAR *szName, GJsonParam &JsonParam);

    operator CHAR*(){return GetString();}
    operator std::string(){return GetString();}
    CHAR* GetString();


private:
    GString     m_strParam;
    GString     m_strValue;
};

class GJsonTupleParam
{
public:
    GJsonTupleParam(UINT32 ulSize=0);
    ~GJsonTupleParam();

    VOID Clear();
    UINT32 GetTupleNum(){return m_ulTupleNum;}

    UINT32 GetStringSize();
    VOID Add(const std::string &strJson);
    VOID Add(GJsonParam *JsonParam);
    VOID Add(GJsonParam &JsonParam);

    operator CHAR*(){return GetString();}
    operator std::string(){return GetString();}
    CHAR* GetString();

private:
    UINT32      m_ulTupleNum;
    GString     m_strParam;
    GString     m_strValue;
};

//---------------------------------------------------------------------------
#endif

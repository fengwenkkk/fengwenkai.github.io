//---------------------------------------------------------------------------

#ifndef SimpleXmlH
#define SimpleXmlH

typedef struct
{
    char    *szField;
    char    *szValue;
}_FIELD_VALUE_T;

class SimpleXml
{
public:
    SimpleXml(char *szXml);
    ~SimpleXml();
    CHAR* GetField(const CHAR *szField);
    BOOL GetField(const CHAR *szField, CHAR *szValue, UINT32 ulMaxLen);
    BOOL GetField(const CHAR *szField, INT32 *piValue);
    BOOL GetField(const CHAR *szField, INT64 *pi64Value);
    BOOL GetField(const CHAR *szField, UINT32 *pulValue);

private:
    char    *m_szXml;
    BOOL    m_bParsed;
    BOOL    Parse(char *szXml);
    std::vector<_FIELD_VALUE_T> m_vField;
};

//---------------------------------------------------------------------------
#endif

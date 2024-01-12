//---------------------------------------------------------------------------

#ifndef XmlH
#define XmlH

class XML;

typedef struct
{
    char    *szNodeName;
    XML     *xml;
}_NODE_VALUE_T;

class XML
{
public:
    XML(CHAR *szXml, BOOL bUntf=FALSE);
    XML(AnsiString strXml);
    ~XML();
    static XML* GetNullXML();

    VOID GetNodeNameList(std::vector<AnsiString> &vList);
    VOID GetNode(AnsiString strNodeName, std::vector<XML*> &vList);
    XML* GetNode(AnsiString strNodeName);

    BOOL    IsNull();
    CHAR*   GetValue();
    BOOL    GetAttr(AnsiString strAttrName, UINT32 *pulValue);
    BOOL    GetAttr(AnsiString strAttrName, INT32 *piValue);
    BOOL    GetAttr(AnsiString strAttrName, INT64 *pi64Value);
    BOOL    GetAttr(AnsiString strAttrName, DOUBLE *pdValue);
    BOOL    GetAttr(AnsiString strAttrName, CHAR *szValue, UINT32 ulMaxLen);
    BOOL    GetAttr(AnsiString strAttrName, AnsiString &strValue);
    CHAR*   GetAttr(AnsiString strAttrName);

    UINT32     NodeNum();
    XML*       Node(UINT32 ulIndex);
    XML*       operator [] (UINT32 ulIndex);//{return Node[ulIndex];};

private:
    char    *m_szXml;
    BOOL    m_bParsed;
    BOOL    Parse(char *szXml);
    std::vector<_NODE_VALUE_T>  m_vNode;
};

//---------------------------------------------------------------------------
#endif

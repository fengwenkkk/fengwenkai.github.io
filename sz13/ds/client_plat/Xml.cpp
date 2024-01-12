#include "vcl.h"
#include "g_include.h"
#include "Xml.h"

static BOOL IsNodeNameEnd(CHAR* szValue, CHAR *szNodeName)
{
    UINT32  ulNodeNameLen = strlen(szNodeName);

    szValue = gos_left_trim_string(szValue);
    if (memcmp(szValue, szNodeName, ulNodeNameLen) != 0)
    {
        return FALSE;
    }

    szValue = gos_left_trim_string(szValue+ulNodeNameLen);
    if (*szValue == '>')
    {
        return TRUE;
    }

    return FALSE;
}

static CHAR* LocateNodeEnd(CHAR* szValue, CHAR *szNodeName)
{
    BOOL    bLeft = FALSE;
    BOOL    bRight = FALSE;
    INT32   iLeftStartNum = 0;
    INT32   iLeftEndNum = 0;
    INT32   iRightStartNum = 0;
    INT32   iRightEndNum = 0;

    for (UINT32 i=0; ;i++)
    {
        if (szValue[i] == '\0')
        {
            return NULL;
        }

        if (szValue[i] == '<')
        {
            if (szValue[i+1] != '/')
            {
                if (bLeft)
                {
                    return NULL;
                }

                iLeftStartNum++;
                bLeft = TRUE;
            }
            else // "</"
            {
                if (bRight)
                {
                    return NULL;
                }

                if (bLeft)
                {
                    return NULL;
                }

                iRightStartNum++;
                bRight = TRUE;

                if (iLeftStartNum == iLeftEndNum &&
                    iLeftStartNum == (iRightStartNum-1) &&
                    iRightStartNum == (iRightEndNum+1))
                {
                    if (IsNodeNameEnd(szValue+i+2, szNodeName))
                    {
                        return szValue+i;
                    }
                }
            }
        }
        else if (szValue[i] == '>')
        {
            if (i > 0)
            {
                if (szValue[i-1] == '/')
                {
                    if (!bLeft)
                    {
                        return NULL;
                    }

                    iLeftEndNum++;
                    bLeft = FALSE;

                    iRightStartNum++;
                    iRightEndNum++;
                    bRight = FALSE;

                    continue;
                }
            }

            if (bLeft)
            {
                iLeftEndNum++;
                bLeft = FALSE;
            }
            else if (bRight)
            {
                iRightEndNum++;
                bRight = FALSE;
            }
        }
    }
}

XML::XML(CHAR *szXml, BOOL bUtf8)
{
    m_szXml = NULL;

    if (szXml)
    {
        AnsiString strXML = "";

        if (bUtf8)
        {
            strXML = Utf8ToAnsi(szXml);
            szXml = strXML.c_str();
        }

        Parse(szXml);
    }
}

XML::XML(AnsiString strXML)
{
    m_szXml = NULL;
    Parse(strXML.c_str());
}

XML::~XML()
{
    GOS_FREE(m_szXml);

    for (UINT32 i=0; i<m_vNode.size(); i++)
    {
        XML *xml = m_vNode[i].xml;

        if (xml != GetNullXML())
        {
            delete xml;
        }
    }
}

BOOL XML::Parse(char *szXml)
{
    char            *szTmp;
    char            *szEnd;
    char            *szEnd2;
    char            *szNodeName;
    char            *szNodeValue;
    _NODE_VALUE_T   stNode;

    m_vNode.clear();

    GOS_FREE(m_szXml);

    szXml = gos_left_trim_string(szXml);
    if (!szXml)
    {
        m_bParsed = TRUE;
        return TRUE;
    }

    m_szXml = strdup(szXml);

    if (m_szXml[0] != '<')
    {
        m_bParsed = TRUE;
        return TRUE;
    }

    szTmp = m_szXml;

    while(1)
    {
        szTmp = gos_trim_string(szTmp);
        if (*szTmp == '\0')
        {
            break;
        }

        if (*szTmp != '<')
        {
            goto err;
        }

        szEnd = strchr(szTmp, '>');
        if (!szEnd)
        {
            goto err;
        }

        szNodeName = szTmp+1;
        *szEnd = '\0';

        szNodeName = gos_trim_string(szNodeName);
        if (*szNodeName == '\0')
        {
            goto err;
        }

        szEnd2 = szNodeName + strlen(szNodeName)-1;
        if (*szEnd2 == '/')
        {
            *szEnd2 = '\0';
            szNodeName = gos_trim_string(szNodeName);

            szTmp = szEnd+1;

            stNode.szNodeName = szNodeName;
            stNode.xml = GetNullXML();
            m_vNode.push_back(stNode);
        }
        else
        {
            szNodeValue = gos_left_trim_string(szEnd+1);
            szEnd = LocateNodeEnd(szNodeValue, szNodeName);
            if (!szEnd)
            {
                goto err;
            }

            *szEnd = '\0';
            szTmp = strchr(szEnd+1, '>');
            szTmp++;

            stNode.szNodeName = szNodeName;
            stNode.xml = new XML(AnsiString(szNodeValue));
            m_vNode.push_back(stNode);
        }
    }

    m_bParsed = TRUE;
    return TRUE;

err:
    m_bParsed = FALSE;
    return FALSE;
}

/*
"<Content><GroupInfoList>
<GroupInfo><GroupID>1005</GroupID><GroupCategory>1</GroupCategory><GroupPriority>15</GroupPriority><GroupName>测试组</GroupName></GroupInfo>
<GroupInfo><GroupID>7000</GroupID><GroupCategory>1</GroupCategory><GroupPriority>10</GroupPriority><GroupName>寰创通信</GroupName></GroupInfo>
<GroupInfo><GroupID>8002</GroupID><GroupCategory>1</GroupCategory><GroupPriority>15</GroupPriority><GroupName>8002</GroupName></GroupInfo>
<GroupInfo><GroupID>8003</GroupID><GroupCategory>1</GroupCategory><GroupPriority>15</GroupPriority><GroupName>车载台行调</GroupName></GroupInfo>
<GroupInfo><GroupID>8004</GroupID><GroupCategory>1</GroupCategory><GroupPriority>15</GroupPriority><GroupName>车辆段调度</GroupName></GroupInfo>
<GroupInfo><GroupID>8005</GroupID><GroupCategory>1</GroupCategory><GroupPriority>15</GroupPriority><GroupName>固定台行调</GroupName></GroupInfo>
</GroupInfoList></Content>"
*/

VOID XML::GetNodeNameList(std::vector<AnsiString> &vList)
{
    vList.clear();

    for (UINT32 i=0; i<m_vNode.size(); i++)
    {
        BOOL    bExist = FALSE;
        for (UINT32 j=0; j<vList.size(); j++)
        {
            if (vList[j] == m_vNode[i].szNodeName)
            {
                bExist = TRUE;
                break;
            }
        }

        if (!bExist)
        {
            vList.push_back(m_vNode[i].szNodeName);
        }
    }
}

VOID XML::GetNode(AnsiString strNodeName, std::vector<XML*> &vList)
{
    vList.clear();

    for (UINT32 i=0; i<m_vNode.size(); i++)
    {
        if (strNodeName == m_vNode[i].szNodeName)
        {
            vList.push_back(m_vNode[i].xml);
        }
    }
}

XML* XML::GetNode(AnsiString strNodeName)
{
    for (UINT32 i=0; i<m_vNode.size(); i++)
    {
        if (strNodeName == m_vNode[i].szNodeName)
        {
            return m_vNode[i].xml;
        }
    }

    return XML::GetNullXML();
}

UINT32 XML::NodeNum()
{
    return m_vNode.size();
};

XML* XML::Node(UINT32 ulIndex)
{
    if (ulIndex >= m_vNode.size())
    {
        return GetNullXML();
    }

    return m_vNode[ulIndex].xml;
}

XML* XML::operator [] (UINT32 ulIndex)
{
    return Node(ulIndex);
};

CHAR* XML::GetValue()
{
    return m_szXml;
}

BOOL XML::GetAttr(AnsiString strAttrName, UINT32 *pulValue)
{
    AnsiString  strValue = GetAttr(strAttrName);

    if (strValue.IsEmpty())
    {
        return FALSE;
    }

    return gos_atou(strValue.c_str(), pulValue);
}

BOOL XML::GetAttr(AnsiString strAttrName, INT64 *pi64Value)
{
    AnsiString  strValue = GetAttr(strAttrName);

    if (strValue.IsEmpty())
    {
        return FALSE;
    }

    return gos_atoi64(strValue.c_str(), pi64Value);
}

BOOL XML::GetAttr(AnsiString strAttrName, INT32 *piValue)
{
    AnsiString  strValue = GetAttr(strAttrName);

    if (strValue.IsEmpty())
    {
        return FALSE;
    }

    return gos_atoi(strValue.c_str(), piValue);
}

BOOL XML::GetAttr(AnsiString strAttrName, DOUBLE *pdValue)
{
    AnsiString  strValue = GetAttr(strAttrName);

    if (strValue.IsEmpty())
    {
        return FALSE;
    }

    return gos_atof(strValue.c_str(), pdValue);
}

BOOL XML::GetAttr(AnsiString strAttrName, CHAR *szValue, UINT32 ulMaxLen)
{
    AnsiString  strValue = GetAttr(strAttrName);

    if (strValue.Length() >= (int)ulMaxLen)
    {
        memcpy(szValue, strValue.c_str(), ulMaxLen-1);
        szValue[ulMaxLen-1] = '\0';
        return TRUE;
    }

    strcpy(szValue, strValue.c_str());
    return TRUE;
}

CHAR* XML::GetAttr(AnsiString strAttrName)
{
    XML *SubXML = GetNode(strAttrName);

    if (!SubXML)
    {
        return "";
    }

    return SubXML->GetValue();
}

BOOL XML::GetAttr(AnsiString strAttrName, AnsiString &strValue)
{
    XML *SubXML = GetNode(strAttrName);

    if (!SubXML)
    {
        return FALSE;
    }

    strValue = SubXML->GetValue();

    return TRUE;
}


static XML  *g_pNullXml = NULL;

XML* XML::GetNullXML()
{
    if (!g_pNullXml)
    {
        g_pNullXml = new XML(NULL);
    }

    return g_pNullXml;
}

BOOL XML::IsNull()
{
    return NodeNum() == 0;
}

void test_xml()
{
    char *szXml = "<Content><GroupInfoList>\
<GroupInfo><GroupID>1005</GroupID><GroupCategory>1</GroupCategory><GroupPriority>15</GroupPriority><GroupName>测试组</GroupName></GroupInfo>\
<GroupInfo><GroupID>7000</GroupID><GroupCategory>1</GroupCategory><GroupPriority>10</GroupPriority><GroupName>寰创通信</GroupName></GroupInfo>\
<GroupInfo><GroupID>8002</GroupID><GroupCategory>1</GroupCategory><GroupPriority>15</GroupPriority><GroupName>8002</GroupName></GroupInfo>   \
<GroupInfo><GroupID>8003</GroupID><GroupCategory>1</GroupCategory><GroupPriority>15</GroupPriority><GroupName>车载台行调</GroupName></GroupInfo>\
<GroupInfo><GroupID>8004</GroupID><GroupCategory>1</GroupCategory><GroupPriority>15</GroupPriority><GroupName>车辆段调度</GroupName></GroupInfo> \
<GroupInfo><GroupID>8005</GroupID><GroupCategory>1</GroupCategory><GroupPriority>15</GroupPriority><GroupName>固定台行调</GroupName></GroupInfo> \
</GroupInfoList></Content>";

    XML     xml(szXml);
    XML     *SubXML;
    std::vector<XML*> vList;

    xml.GetNode("Content")->GetNode("GroupInfoList")->GetNode("GroupInfo", vList);

    for (UINT32 i=0; i<vList.size(); i++)
    {
        SubXML = vList[i];

        AnsiString  strID = SubXML->GetAttr("GroupID");
        AnsiString  strPri = SubXML->GetAttr("GroupPriority");
    }
}

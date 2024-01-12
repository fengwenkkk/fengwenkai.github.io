#include "vcl.h"
#include "g_include.h"
#include "SimpleXml.h"

SimpleXml::SimpleXml(char *szXml)
{
    m_szXml = NULL;
    Parse(szXml);
}

SimpleXml::~SimpleXml()
{
    GOS_FREE(m_szXml);
}

CHAR* SimpleXml::GetField(const CHAR *szField)
{
    for (UINT32 i=0; i<m_vField.size(); i++)
    {
        if (strcmp(szField, m_vField[i].szField) == 0)
        {
            return m_vField[i].szValue;
        }
    }

    return NULL;
}

BOOL SimpleXml::GetField(const CHAR *szField, CHAR *szValue, UINT32 ulMaxLen)
{
    for (UINT32 i=0; i<m_vField.size(); i++)
    {
        if (strcmp(szField, m_vField[i].szField) == 0)
        {
            if (strlen(m_vField[i].szValue) >= ulMaxLen)
            {
                return FALSE;
            }

            strcpy(szValue, m_vField[i].szValue);
            return TRUE;
        }
    }

    *szValue = '\0';

    return FALSE;
}

BOOL SimpleXml::GetField(const CHAR *szField, INT32 *piValue)
{
    CHAR    *szValue = GetField(szField);

    if (!szValue)
    {
        return FALSE;
    }

    return gos_atoi(szValue, piValue);
}

BOOL SimpleXml::GetField(const CHAR *szField, INT64 *pi64Value)
{
    CHAR    *szValue = GetField(szField);

    if (!szValue)
    {
        return FALSE;
    }

    return gos_atoi64(szValue, pi64Value);
}

BOOL SimpleXml::GetField(const CHAR *szField, UINT32 *pulValue)
{
    CHAR    *szValue = GetField(szField);

    if (!szValue)
    {
        return FALSE;
    }

    return gos_atou(szValue, pulValue);
}

BOOL SimpleXml::Parse(char *szXml)
{
    char            *szTmp;
    char            *szEnd;
    char            *szField;
    char            *szValue;
    _FIELD_VALUE_T  stField;
    AnsiString      strXml;

    m_vField.clear();

    GOS_FREE(m_szXml);

    strXml = Utf8ToAnsi(szXml);
    m_szXml = strdup(strXml.c_str());

    szTmp = gos_trim_string(m_szXml);
    if (memcmp(szTmp, "<Content>", strlen("<Content>")) != 0)
    {
        goto err;
    }
    szTmp += strlen("<Content>");

    if (strlen(szTmp) < strlen("</Content>"))
    {
        goto err;
    }

    szEnd = szTmp + strlen(szTmp) - strlen("</Content>");
    if (strcmp(szEnd, "</Content>") != 0)
    {
        goto err;
    }
    *szEnd = '\0';

    while(1)
    {
        // <GroupID>1001</GroupID>
        szTmp = gos_left_trim_string(szTmp);
        if (*szTmp == '\0')
        {
            break;
        }

        if (*szTmp++ != '<')
        {
            goto err;
        }

        szEnd = strchr(szTmp, '>');
        if (!szEnd)
        {
            goto err;
        }
        *szEnd = '\0';

        szField = szTmp;
        szValue = szEnd+1;

        szEnd = strstr(szValue, szField);
        if (!szEnd)
        {
            goto err;
        }

        if (memcmp(szEnd-2, "</", 2) != 0)
        {
            goto err;
        }

        *(szEnd-2) = '\0';
        szEnd += strlen(szField);
        if (*szEnd++ != '>')
        {
            goto err;
        }

        stField.szField = szField;
        stField.szValue = szValue;
        m_vField.push_back(stField);

        szTmp = szEnd;
    }

    return TRUE;

err:
    return FALSE;
}

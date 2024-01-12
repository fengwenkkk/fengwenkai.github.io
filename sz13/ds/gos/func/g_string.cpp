#include "g_include.h"

VOID GString::Init()
{
    UINT32  ulLen = 1024;

    m_szString = (CHAR*)calloc(1, ulLen);
    m_ulMaxLen = ulLen-1;
    m_ulCurrLen = 0;
}

GString::GString()
{
    Init();
}

GString::GString(UINT32 x)
{
    Init();

    sprintf("%u", x);
}

GString::GString(INT32 x)
{
    Init();

    sprintf("%d", x);
}

GString::GString(INT64 x)
{
    Init();

    sprintf(FORMAT_I64, x);
}

GString::GString(const CHAR *x)
{
    Init();

    if (!x)
    {
        return;
    }

    sprintf("%s", x);
}

GString::~GString()
{
    if (m_szString)
    {
        free(m_szString);
    }
}

CHAR *GString::GetString() const
{
    return m_szString;
}

CHAR* GString::CloneString()
{
    if (!m_szString)
    {
        return NULL;
    }

    return strdup(m_szString);
}

UINT32 GString::Length()
{
    return m_ulCurrLen;
}

BOOL GString::SetLength(UINT32 ulNewLength)
{
    if (ulNewLength <= m_ulMaxLen)
    {
        return TRUE;
    }

    UINT32 ulNewLen = ((ulNewLength+1023)/1024)*1024;

    CHAR    *szTmp = (CHAR*)calloc(1, ulNewLen);

    if (!szTmp)
    {
        return FALSE;
    }

    memcpy(szTmp, m_szString, m_ulCurrLen);

    m_ulMaxLen = ulNewLen;
    free(m_szString);
    m_szString = szTmp;

    return TRUE;
}

VOID GString::Clear()
{
    m_szString[0] = '\0';
    m_ulCurrLen = 0;
}

BOOL GString::Append(const CHAR *szString)
{
    return Append((CHAR*)szString);
}

BOOL GString::Append(CHAR *szString)
{
    UINT32  ulLen = strlen(szString);

    return Append(szString, ulLen, FALSE);
}

BOOL GString::Append(CHAR *szString, UINT32 ulLen, BOOL bNewLine)
{
    if (!szString)
    {
        return FALSE;
    }

    if (0 == ulLen)
    {
        return TRUE;
    }

    UINT32  ulNewLen = ulLen + m_ulCurrLen + (bNewLine?1:0) + 1;

    if (ulNewLen > m_ulMaxLen)
    {
        ulNewLen = ((ulNewLen+1023)/1024)*1024;

        CHAR    *szTmp = (CHAR*)calloc(1, ulNewLen);

        if (!szTmp)
        {
            return FALSE;
        }

        memcpy(szTmp, m_szString, m_ulCurrLen);
        memcpy(szTmp+m_ulCurrLen, szString, ulLen+1);
        m_ulCurrLen += ulLen;

        if (bNewLine)
        {
            szTmp[m_ulCurrLen++] = '\n';
            szTmp[m_ulCurrLen] = '\0';
        }

        m_ulMaxLen = ulNewLen;
        free(m_szString);
        m_szString = szTmp;
    }
    else
    {
        memcpy(m_szString+m_ulCurrLen, szString, ulLen+1);
        m_ulCurrLen += ulLen;

        if (bNewLine)
        {
            m_szString[m_ulCurrLen++] = '\n';
            m_szString[m_ulCurrLen] = '\0';
        }
    }

    return TRUE;
}

INT32 GString::cat_sprintf(const CHAR *szFormat, ...)
{
    va_list     vaValue;
    INT32       iDataLen = 0;
    CHAR        *szNewString;

    while(1)
    {
        va_start(vaValue, szFormat);
        iDataLen = vsnprintf(m_szString+m_ulCurrLen, m_ulMaxLen-2-m_ulCurrLen, szFormat, vaValue);
        va_end(vaValue);

        // vsnprintf 在 valog内容超出buf大小时，返回负数
        if (iDataLen <= 0)
        {
            m_szString[m_ulCurrLen] = '\0';

            if (m_ulMaxLen >= 1024*1024*1024)
            {
                return iDataLen;
            }

            m_ulMaxLen *= 2;

            szNewString = (CHAR*)calloc(1, m_ulMaxLen);
            if (!szNewString)
            {
                return iDataLen;
            }

            strcpy(szNewString, m_szString);
            free(m_szString);
            m_szString = szNewString;
        }
        else
        {
            m_ulCurrLen += iDataLen;
            return iDataLen;
        }
    }
}

INT32 GString::sprintf(const CHAR *szFormat, ...)
{
    va_list     vaValue;
    INT32       iDataLen = 0;
    CHAR        *szNewString;

    m_ulCurrLen = 0;
    *m_szString = '\0';

    while(1)
    {
        va_start(vaValue, szFormat);
        iDataLen = vsnprintf(m_szString+m_ulCurrLen, m_ulMaxLen-2-m_ulCurrLen, szFormat, vaValue);
        va_end(vaValue);

        // vsnprintf 在 valog内容超出buf大小时，返回负数
        if (iDataLen <= 0)
        {
            m_szString[m_ulCurrLen] = '\0';

            if (m_ulMaxLen >= 1024*1024*1024)
            {
                return iDataLen;
            }

            m_ulMaxLen *= 2;

            szNewString = (CHAR*)calloc(1, m_ulMaxLen);
            if (!szNewString)
            {
                return iDataLen;
            }

            strcpy(szNewString, m_szString);
            free(m_szString);
            m_szString = szNewString;
        }
        else
        {
            m_ulCurrLen += iDataLen;
            return iDataLen;
        }
    }
}

GString::GString(const GString& other)
{
    m_ulCurrLen = other.m_ulCurrLen;
    m_ulMaxLen = other.m_ulMaxLen;
    m_szString = (CHAR *)calloc(1, m_ulMaxLen);
    strcpy(m_szString, other.m_szString);
}

GString& GString::operator=(const GString& other)
{
    if (&other != this)
    {
        GOS_FREE(m_szString);
        m_ulCurrLen = other.m_ulCurrLen;
        m_ulMaxLen = other.m_ulMaxLen;
        m_szString = (CHAR *)calloc(1, m_ulMaxLen);
        strcpy(m_szString, other.m_szString);
    }
    return *this;
}

GString::operator const CHAR*()
{
    return (const CHAR*)m_szString;
}

GString::operator CHAR*()
{
    return m_szString;
}


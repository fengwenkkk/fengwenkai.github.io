#include "g_include.h"

typedef struct
{
    CHAR    acKey[128];
    CHAR    acValue[512];
    CHAR    *szComment;
    UINT32  ulCommentLen;
    CHAR    *szGroupComment;
    UINT32  ulGroupCommentLen;
    UINT32  ulGroupID;
}CONF_KEY_VALUE_T;

#define DEFAULT_GROUP_ID    0
#define LAST_GROUP_ID       0xfffffffe
#define INVALID_GROUP_KEY   0xffffffff

static BOOL _gos_hash_key_cmp_string(VOID* pSrcKey, VOID *pDestKey)
{
    return strcasecmp((CHAR*)pSrcKey, (CHAR*)pDestKey) == 0;
}

static UINT32 _gos_hash_string(VOID *pKey)
{
    CHAR    *szKey = (CHAR*)pKey;
    UINT32  ulValue = 0;
    UINT32  ulKey;
    static UINT32   ulDelta = (UINT32)('A' - 'a');

    while(*szKey)
    {
        ulKey = (*szKey++);
        if (ulKey >= 'a' && ulKey <= 'z')
        {
            ulKey += ulDelta;
        }

        ulValue += (ulValue<<7)+(ulValue<<1)+ulKey;
    }

    return ulValue;
}

GConfFile::GConfFile(CHAR *szFileName)
{
    Init(szFileName);
}

GConfFile::GConfFile(const std::string &strFileName)
{
    Init((CHAR *)strFileName.c_str());
}

void GConfFile::Init(CHAR *szFileName)
{
    HANDLE  hFile = gos_fopen(szFileName, "rb");
    INT32   iSize = gos_get_fsize(hFile);

    m_szOriText = NULL;
    m_szConf = NULL;
    m_hValues = (HANDLE)gos_hashlist_init(32, 0, 32, sizeof(CONF_KEY_VALUE_T), 0, _gos_hash_string, _gos_hash_key_cmp_string, gos_malloc, gos_free);
    if (!m_hValues)
    {
        return;
    }

    m_vGroupIDList.clear();

    if (!hFile || iSize < 0)
    {
        return;
    }

    m_szConf = (CHAR*)calloc(1, iSize+1);

    if (!m_szConf)
    {
        gos_fclose(hFile);
        return;
    }

    if (iSize != gos_fread(hFile, m_szConf, iSize))
    {
        gos_fclose(hFile);
        return;
    }

    gos_fclose(hFile);

    sprintf(m_acFile, szFileName);
    gos_format_path(m_acFile);

    m_szOriText = strdup(m_szConf);
    ParseConf(m_szConf);
}

GConfFile::~GConfFile()
{
    GOS_FREE(m_szOriText);
    GOS_FREE(m_szConf);
    gos_hashlist_free((GHASH*)m_hValues);

    m_vGroupIDList.clear();
}

const CHAR* GConfFile::GetString() const
{
    return m_szOriText?(const CHAR*)m_szOriText:"";
}

static CHAR* LeftTrim(CHAR *szText)
{
    while(*szText != '\0')
    {
        if (gos_isspace(*szText) &&
            *szText != '\r' &&
            *szText != '\n' )
        {
            szText++;
        }
        else
        {
            break;
        }
    }

    return szText;
}

VOID GConfFile::ParseConf(CHAR *szConf)
{
    CHAR            *szKey = szConf;
    CHAR            *szEnd;
    CHAR            *szValue;
    CHAR            *szGroupStart = NULL;
    CHAR            *szGroupEnd = NULL;
    BOOL            bIsEnd = FALSE;
    CHAR            *szCommentStart = NULL;
    CHAR            *szCommentEnd = NULL;
    CHAR            *szGroupCommentStart = NULL;
    CHAR            *szGroupCommentEnd = NULL;
    UINT32          ulGroupID = 0;
    CONF_KEY_VALUE_T     stKeyValue;

    while(!bIsEnd)
    {
        szEnd = strchr(szKey, '\n');
        if (!szEnd)
        {
            bIsEnd = TRUE;
        }

        szKey = LeftTrim(szKey);
        if (szKey[0] == '\0')
        {
            break;
        }

        if (*szKey == '[')
        {
            szGroupStart = szKey;
            szGroupEnd = strchr(szKey, ']');
            if (!szGroupEnd)
            {
                 szGroupStart = NULL;
                 szGroupEnd = NULL;
            }
            else
            {
                if (szCommentStart)
                {
                    szGroupCommentStart = szCommentStart;
                    szGroupCommentEnd = szKey;

                    szCommentStart = NULL;
                    szCommentEnd = NULL;
                }

                m_vGroupIDList.push_back(++ulGroupID);
            }
        }

        if (*szKey == '#' || *szKey == '\r' || *szKey == '\n')
        {
            if (szCommentStart == NULL)
            {
                szCommentStart = szKey;
            }

            if (bIsEnd)
            {
                szCommentEnd = szKey+strlen(szKey)+1;
                break;
            }
            else
            {
                szCommentEnd = szEnd+1;
                szKey = szEnd+1;
            }
        }
        else
        {
            if (szEnd)
            {
                szEnd[0] = '\0';
                gos_trim_string(szKey);
            }

            szValue = strchr(szKey, '=');
            if (szValue)
            {
                szValue[0] = '\0';
                szKey = gos_trim_string(szKey);
                szValue = gos_trim_string(szValue+1);

                if (strlen(szKey) >= sizeof(stKeyValue.acKey) ||
                    strlen(szValue) >= sizeof(stKeyValue.acValue))
                {
                    continue;
                }

                memset(&stKeyValue, 0, sizeof(stKeyValue));

                if (!szGroupStart || !szGroupEnd)
                {
                    strcpy(stKeyValue.acKey, szKey);
                }

                if (szGroupStart && szGroupEnd)
                {
                    if ((strlen(szKey) + (szGroupEnd-szGroupStart)) >= sizeof(stKeyValue.acKey))
                    {
                        continue;
                    }

                    memcpy(stKeyValue.acKey, szGroupStart, szGroupEnd-szGroupStart+1);
                    strcpy(stKeyValue.acKey + (szGroupEnd-szGroupStart+1), szKey);

                    stKeyValue.ulGroupID = ulGroupID;
                }
                else
                {
                    stKeyValue.ulGroupID = DEFAULT_GROUP_ID;
                }

                strcpy(stKeyValue.acValue, szValue);
                stKeyValue.szComment = szCommentStart;
                if (szCommentStart)
                {
                    stKeyValue.ulCommentLen = (UINT32)(szCommentEnd - szCommentStart);
                }

                stKeyValue.szGroupComment = szGroupCommentStart;
                if (szGroupCommentStart)
                {
                    stKeyValue.ulGroupCommentLen = (UINT32)(szGroupCommentEnd - szGroupCommentStart);
                }
                gos_hashlist_add((GHASH*)m_hValues, (VOID*)&stKeyValue, sizeof(stKeyValue));

                szCommentStart = NULL;
                szCommentEnd = NULL;
            }

            szKey = szEnd+1;
        }
    }

    if (szCommentStart)
    {
        memset(&stKeyValue, 0, sizeof(stKeyValue));
        strcpy(stKeyValue.acKey, "");
        strcpy(stKeyValue.acValue, "");
        stKeyValue.ulGroupID = LAST_GROUP_ID;
        stKeyValue.szComment = szCommentStart;
        stKeyValue.ulCommentLen = strlen(szCommentStart);

        gos_hashlist_add((GHASH*)m_hValues, (VOID*)&stKeyValue, sizeof(stKeyValue));
    }
}

CHAR* GConfFile::Get(const CHAR *szKey) const
{
    CONF_KEY_VALUE_T     *pstKey = (CONF_KEY_VALUE_T*)gos_hashlist_get_string((GHASH*)m_hValues, (CHAR*)szKey);

    if (!pstKey)
    {
        return NULL;
    }

    return pstKey->acValue;
}

CHAR* GConfFile::Get(const CHAR *szGroupKey, const CHAR *szKey)
{
    CHAR            acKey[128] = {0};
    CONF_KEY_VALUE_T     *pstKey = NULL;

    if (!szGroupKey || !szKey)
    {
        return NULL;
    }

    if ((strlen(szGroupKey) + strlen(szKey) + 2) > sizeof(acKey))
    {
        return NULL;
    }

    sprintf(acKey, "[%s]%s", szGroupKey, szKey);

    pstKey = (CONF_KEY_VALUE_T*)gos_hashlist_get_string((GHASH*)m_hValues, (CHAR*)acKey);
    if (!pstKey)
    {
        return NULL;
    }

    return pstKey->acValue;
}

BOOL GConfFile::Get(const CHAR *szGroupKey, const CHAR *szKey, std::string &strValue)
{
    CHAR* szValue = Get(szGroupKey, szKey);

    if (!szValue)
    {
        return FALSE;
    }

    strValue = szValue;
    return TRUE;
}

BOOL GConfFile::Get(const CHAR *szGroupKey, const CHAR *szKey, CHAR *szRetValue, UINT32 ulMaxLen)
{
    CHAR    *szValue = Get(szGroupKey, szKey);

    *szRetValue = '\0';
    if (!szValue || strlen(szValue) >= ulMaxLen)
    {
        return FALSE;
    }

    strcpy(szRetValue, szValue);

    return TRUE;
}

BOOL GConfFile::Get(const char *szGroupKey, const char *szKey, INT32 iDefaultValue, INT32 &iValue)
{
    return Get(szGroupKey, szKey, iDefaultValue, &iValue);
}

BOOL GConfFile::Get(const CHAR *szGroupKey, const CHAR *szKey, INT32 iDefaultValue, INT32 *piValue)
{
    CHAR            *szValue = Get(szGroupKey, szKey);

    if (!szValue)
    {
        *piValue = iDefaultValue;
        return TRUE;
    }

    return gos_atoi(szValue, piValue);
}

BOOL GConfFile::Get(const CHAR *szGroupKey, const CHAR *szKey, UINT16& usValue)
{
    return Get(szGroupKey, szKey, &usValue);
}

BOOL GConfFile::Get(const CHAR *szGroupKey, const CHAR *szKey, UINT16 *pusValue)
{
    CHAR            *szValue = Get(szGroupKey, szKey);

    if (!szValue)
    {
        return FALSE;
    }

    INT32 i = 0;
    if(!gos_atoi(szValue, &i))
    {
        return FALSE;
    }

    if (i < 0 || i > 0xffff)
    {
        return FALSE;
    }

    *pusValue = (UINT16)i;
    return TRUE;
}

BOOL GConfFile::Get(const char *szGroupKey, const CHAR *szKey, INT32 &iValue)
{
    return Get(szGroupKey, szKey, &iValue);
}

BOOL GConfFile::Get(const CHAR *szGroupKey, const CHAR *szKey, INT32 *piValue)
{
    CHAR            *szValue = Get(szGroupKey, szKey);

    if (!szValue)
    {
        return FALSE;
    }

    return gos_atoi(szValue, piValue);
}

BOOL GConfFile::Get(const char *szGroupKey, const char *szKey, UINT32 &ulValue)
{
    return Get(szGroupKey, szKey, &ulValue);
}

BOOL GConfFile::Get(const CHAR *szGroupKey, const CHAR *szKey, UINT32 ulDefaultValue, UINT32 *pulValue)
{
    CHAR            *szValue = Get(szGroupKey, szKey);

    if (!szValue)
    {
        *pulValue = ulDefaultValue;
        return TRUE;
    }

    return gos_atou(szValue, pulValue);
}

BOOL GConfFile::Get(const CHAR *szGroupKey, const CHAR *szKey, UINT16 usDefaultValue, UINT16 *pusValue)
{
    UINT32      ulValue = 0xffffffff;

    if (!Get(szGroupKey, szKey, usDefaultValue, &ulValue))
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

BOOL GConfFile::Get(const CHAR *szGroupKey, const CHAR *szKey, UINT32 *pulValue)
{
    CHAR *szValue = Get(szGroupKey, szKey);

    if (!szValue)
    {
        return FALSE;
    }

    return gos_atou(szValue, pulValue);
}

BOOL GConfFile::Get(const CHAR *szGroupKey, const CHAR *szKey, bool &bValue)
{
    CHAR *szValue = Get(szGroupKey, szKey);

    if (!szValue)
    {
        return FALSE;
    }

    if (strcasecmp(szValue, "true") == 0 ||
        strcmp(szValue, "1") == 0 )
    {
        bValue = true;
        return TRUE;
    }
    else if (strcasecmp(szValue, "false") == 0 ||
             strcmp(szValue, "0") == 0 )
    {
        bValue = false;
        return TRUE;
    }

    return FALSE;
}

BOOL GConfFile::Get(const CHAR *szGroupKey, const CHAR *szKey, bool bDefault, bool &bValue)
{
    CHAR *szValue = Get(szGroupKey, szKey);

    if (!szValue)
    {
        bValue = bDefault;
        return TRUE;
    }

    if (strcasecmp(szValue, "true") == 0 ||
        strcmp(szValue, "1") == 0 )
    {
        bValue = true;
        return TRUE;
    }
    else if (strcasecmp(szValue, "false") == 0 ||
        strcmp(szValue, "0") == 0 )
    {
        bValue = false;
        return TRUE;
    }

    return FALSE;
}

BOOL GConfFile::Set(const CHAR *szGroupKey, const CHAR *szKey, UINT32 ulValue)
{
    CHAR    acValue[32];

    sprintf(acValue, "%u", ulValue);
    return Set(szGroupKey, szKey, acValue);
}

BOOL GConfFile::Set(const CHAR *szGroupKey, const CHAR *szKey, CHAR *szValue)
{
    CONF_KEY_VALUE_T    *pstKey = NULL;
    CHAR                acKey[128];

    if ((strlen(szGroupKey) + strlen(szKey) + 2) > sizeof(acKey))
    {
        return FALSE;
    }

    if (strlen(szValue) >= sizeof(pstKey->acValue))
    {
        return FALSE;
    }

    sprintf(acKey, "[%s]%s", szGroupKey, szKey);

    pstKey = (CONF_KEY_VALUE_T*)gos_hashlist_get_string((GHASH*)m_hValues, (CHAR*)acKey);
    if (!pstKey)
    {
        CONF_KEY_VALUE_T     stKey = {0};

        strcpy(stKey.acKey, acKey);

        pstKey = (CONF_KEY_VALUE_T*)gos_hashlist_add((GHASH*)m_hValues, &stKey, sizeof(stKey));
        if (!pstKey)
        {
            return FALSE;
        }

        pstKey->ulGroupID = GetGroupKeyID((CHAR*)szGroupKey);
        if (pstKey->ulGroupID == INVALID_GROUP_KEY)
        {
            pstKey->ulGroupID = 0;
            for (UINT32 i=0; i<m_vGroupIDList.size(); i++)
            {
                if (pstKey->ulGroupID < m_vGroupIDList[i])
                {
                    pstKey->ulGroupID = m_vGroupIDList[i];
                }
            }

            pstKey->ulGroupID++;
            m_vGroupIDList.push_back(pstKey->ulGroupID);
        }
    }

    strcpy(pstKey->acValue, szValue);

    return TRUE;
}

BOOL GConfFile::Get(const CHAR *szKey, UINT32 ulDefault, UINT32 *pulValue)
{
    CHAR    *szValue = Get(szKey);

    if (!szValue)
    {
        *pulValue = ulDefault;
        return TRUE;
    }

    return gos_atou(szValue, pulValue);
}

BOOL GConfFile::Get(const CHAR *szKey, UINT32 *pulValue)
{
    CHAR    *szValue = Get(szKey);

    if (!szValue)
    {
        return FALSE;
    }

    return gos_atou(szValue, pulValue);
}

BOOL GConfFile::Get(const CHAR *szKey, UINT16 usDefault, UINT16 *pusValue)
{
    UINT32  ulValue;

    if (!Get(szKey, (UINT32)usDefault, &ulValue) ||
        ulValue > 0xffff)
    {
        return FALSE;
    }

    *pusValue = (UINT16)ulValue;

    return TRUE;
}

BOOL GConfFile::Get(const CHAR *szKey, UINT16 *pusValue)
{
    UINT32  ulValue;

    if (!Get(szKey, &ulValue) ||
        ulValue > 0xffff)
    {
        return FALSE;
    }

    *pusValue = (UINT16)ulValue;

    return TRUE;
}

BOOL GConfFile::Get(const CHAR *szKey, INT32 iDefault, INT32 *piValue)
{
    CHAR    *szValue = Get(szKey);

    if (!szValue)
    {
        *piValue = iDefault;
        return TRUE;
    }

    return gos_atoi(szValue, piValue);
}

BOOL GConfFile::Get(const CHAR *szKey, INT32 *piValue)
{
    CHAR    *szValue = Get(szKey);

    if (!szValue)
    {
        return FALSE;
    }

    return gos_atoi(szValue, piValue);
}

BOOL GConfFile::GetIP(const CHAR *szKey, UINT8 *pucValue)
{
    CHAR    *szValue = Get(szKey);

    if (!szValue)
    {
        return FALSE;
    }

    return gos_get_ip(szValue, pucValue);
}

BOOL GConfFile::Get(const CHAR *szKey, CHAR *szRetValue, UINT32 ulMaxLen)
{
    CHAR    *szValue = Get(szKey);

    if (!szValue || strlen(szValue) >= ulMaxLen)
    {
        return FALSE;
    }

    strcpy(szRetValue, szValue);

    return TRUE;
}

BOOL GConfFile::GetBool(const CHAR *szKey, BOOL bDefault, BOOL *pbValue)
{
    CHAR *szValue = Get(szKey);

    if (!szValue)
    {
        *pbValue = bDefault;
        return TRUE;
    }

    if (strcasecmp(szValue, "true") == 0 ||
        strcmp(szValue, "1") == 0 )
    {
        *pbValue = TRUE;
        return TRUE;
    }
    else if (strcasecmp(szValue, "false") == 0 ||
             strcmp(szValue, "0") == 0 )
    {
        *pbValue = FALSE;
        return TRUE;
    }

    return FALSE;
}

BOOL GConfFile::GetBool(const CHAR *szKey, BOOL *pbValue)
{
    CHAR *szValue = Get(szKey);

    if (!szValue)
    {
        return FALSE;
    }

    if (strcasecmp(szValue, "true") == 0 ||
        strcmp(szValue, "1") == 0 )
    {
        *pbValue = TRUE;
        return TRUE;
    }
    else if (strcasecmp(szValue, "false") == 0 ||
             strcmp(szValue, "0") == 0 )
    {
        *pbValue = FALSE;
        return TRUE;
    }

    return FALSE;
}

BOOL GConfFile::GetBool(const CHAR *szGroupKey, const CHAR *szKey, BOOL &bValue)
{
    CHAR *szValue = Get(szGroupKey, szKey);

    if (!szValue)
    {
        return FALSE;
    }

    if (strcasecmp(szValue, "true") == 0 ||
        strcmp(szValue, "1") == 0 )
    {
        bValue = TRUE;
        return TRUE;
    }
    else if (strcasecmp(szValue, "false") == 0 ||
             strcmp(szValue, "0") == 0 )
    {
        bValue = FALSE;
        return TRUE;
    }

    return FALSE;
}

BOOL GConfFile::GetBool(const CHAR *szGroupKey, const CHAR *szKey, BOOL bDefault, BOOL &bValue)
{
    CHAR *szValue = Get(szGroupKey, szKey);

    if (!szValue)
    {
        bValue = bDefault;
        return TRUE;
    }

    if (strcasecmp(szValue, "true") == 0 ||
        strcmp(szValue, "1") == 0 )
    {
        bValue = TRUE;
        return TRUE;
    }
    else if (strcasecmp(szValue, "false") == 0 ||
             strcmp(szValue, "0") == 0 )
    {
        bValue = FALSE;
        return TRUE;
    }

    return FALSE;
}

BOOL GConfFile::_Set(const CHAR *szKey, CHAR *szValue)
{
    CONF_KEY_VALUE_T     *pstKey = (CONF_KEY_VALUE_T*)gos_hashlist_get_string((GHASH*)m_hValues, (CHAR*)szKey);

    if (!pstKey)
    {
        return FALSE;
    }

    if (strlen(szValue) >= sizeof(pstKey->acValue))
    {
        return FALSE;
    }

    strcpy(pstKey->acValue, szValue);

    return TRUE;
}

BOOL GConfFile::Set(const CHAR *szKey, UINT32 ulValue)
{
    CHAR    acValue[32];

    sprintf(acValue, "%u", ulValue);

    return Set(szKey, acValue);
}

BOOL GConfFile::Set(const CHAR *szKey, INT32 iValue)
{
    CHAR    acValue[32];

    sprintf(acValue, "%d", iValue);

    return Set(szKey, acValue);
}

BOOL GConfFile::Set(const CHAR *szKey, CHAR *szValue)
{
    CONF_KEY_VALUE_T     stKey = {0};

    if (_Set(szKey, szValue))
    {
        return TRUE;
    }

    szKey = gos_trim_string((CHAR*)szKey);
    szValue = gos_trim_string(szValue);

    if (strlen(szKey) >= sizeof(stKey.acKey) ||
        strlen(szValue) >= sizeof(stKey.acValue))
    {
        return FALSE;
    }

    strcpy(stKey.acKey, szKey);
    strcpy(stKey.acValue, szValue);

    gos_hashlist_add((GHASH*)m_hValues, (VOID*)&stKey, sizeof(stKey));

    return TRUE;
}

BOOL GConfFile::SetIP(const CHAR *szKey, UINT8 *pucValue)
{
    CHAR    acValue[16];

    sprintf(acValue, IP_FMT, IP_ARG(pucValue));

    return Set(szKey, acValue);
}

VOID GConfFile::GetKeyList(VECTOR<CHAR*> &vKey) const
{
    GHASHNODE   *pNode;
    GHASHNODE   *pNextNode;
    CONF_KEY_VALUE_T *pstKeyValue;

    vKey.clear();

    pNode = gos_hashlist_get_first((GHASH*)m_hValues);
    while(pNode)
    {
        pNextNode = gos_hashlist_get_next((GHASH*)m_hValues, pNode);
        pstKeyValue = (CONF_KEY_VALUE_T*)gos_hashlist_get_node_data(pNode);
        vKey.push_back(pstKeyValue->acKey);
        pNode = pNextNode;
    }
}

VOID GConfFile::GetGroupKeyList(VECTOR<CHAR*> &vKey, CHAR *szGroupKey)
{
    UINT32      ulGroupKeyID = GetGroupKeyID(szGroupKey);

    GetGroupKeyList(vKey, ulGroupKeyID);
}

UINT32 GConfFile::GetGroupKeyID(const CHAR *szGroupKey)
{
    GHASHNODE       *pNode=NULL;
    GHASHNODE       *pNextNode = NULL;
    CONF_KEY_VALUE_T     *pstKeyValue = NULL;

    pNode = gos_hashlist_get_first((GHASH*)m_hValues);
    while(pNode)
    {
        pNextNode = gos_hashlist_get_next((GHASH*)m_hValues, pNode);
        pstKeyValue = (CONF_KEY_VALUE_T*)gos_hashlist_get_node_data(pNode);

        if (strncmp(szGroupKey, pstKeyValue->acKey+1, strlen(szGroupKey)) == 0)
        {
            return pstKeyValue->ulGroupID;
        }

        pNode = pNextNode;
    }

    return INVALID_GROUP_KEY;
}

BOOL GConfFile::HasGroupKey(const CHAR *szGroupKey)
{
    return GetGroupKeyID(szGroupKey) != INVALID_GROUP_KEY;
}

BOOL GConfFile::GetGroupName(UINT32 ulGroupID, CHAR *szName, UINT32 ulMaxLen)
{
    GHASHNODE       *pNode=NULL;
    GHASHNODE       *pNextNode = NULL;
    CONF_KEY_VALUE_T     *pstKeyValue = NULL;
    CHAR            *szGroupStart = NULL;
    CHAR            *szGroupEnd = NULL;
    INT32           iLen;

    *szName = '\0';

    pNode = gos_hashlist_get_first((GHASH*)m_hValues);
    while(pNode)
    {
        pNextNode = gos_hashlist_get_next((GHASH*)m_hValues, pNode);
        pstKeyValue = (CONF_KEY_VALUE_T*)gos_hashlist_get_node_data(pNode);

        if (pstKeyValue->ulGroupID != ulGroupID)
        {
            pNode = pNextNode;
            continue;
        }

        if (pstKeyValue->acKey[0])
        {
            if (pstKeyValue->acKey[0] != '[')
            {
                return FALSE;
            }

            szGroupStart = pstKeyValue->acKey+1;
            szGroupEnd = strchr(pstKeyValue->acKey, ']');
            if (!szGroupEnd)
            {
                return FALSE;
            }
            else
            {
                szGroupEnd--;
                iLen = szGroupEnd-szGroupStart+1;
                if (iLen >= (int)ulMaxLen || iLen <=0)
                {
                    return FALSE;
                }

                memcpy(szName, szGroupStart, iLen);
                szName[iLen] = '\0';

                return TRUE;
            }
        }

        break;
    }

    return FALSE;
}

VOID GConfFile::GetGroupIDList(VECTOR<UINT32> &vGroupIDList)
{
    vGroupIDList.clear();

    for (UINT32 i = 0; i < m_vGroupIDList.size(); i++)
    {
        vGroupIDList.push_back(m_vGroupIDList[i]);
    }
}

VOID GConfFile::GetGroupKeyList(VECTOR<CHAR*> &vKey, UINT32 ulGroupID)
{
    GHASHNODE      *pNode;
    GHASHNODE      *pNextNode;
    CONF_KEY_VALUE_T    *pstKeyValue;
    CHAR           acGroupName[64] = {0};

    vKey.clear();

    pNode = gos_hashlist_get_first((GHASH*)m_hValues);
    while(pNode)
    {
        pNextNode = gos_hashlist_get_next((GHASH*)m_hValues, pNode);
        pstKeyValue = (CONF_KEY_VALUE_T*)gos_hashlist_get_node_data(pNode);

        if(pstKeyValue->ulGroupID == ulGroupID)
        {
            if (ulGroupID != DEFAULT_GROUP_ID)
            {
                if (!GetGroupName(ulGroupID, acGroupName, sizeof(acGroupName)))
                {
                    pNode = pNextNode;
                    continue;
                }

                if (strlen(acGroupName) < (strlen(pstKeyValue->acKey)+2) ) // 2为左右中括号[]的长度
                {
                    vKey.push_back(pstKeyValue->acKey+strlen(acGroupName)+2);
                }

                memset(acGroupName,  0, sizeof(acGroupName));
            }
            else
            {
                vKey.push_back(pstKeyValue->acKey);
            }
        }
        pNode = pNextNode;
    }
}

BOOL GConfFile::Del(const CHAR *szGroupKey, const CHAR *szKey)
{
    CHAR                acKey[128] = {0};

    if (!szGroupKey || !szKey)
    {
        return FALSE;
    }

    if ((strlen(szGroupKey) + strlen(szKey) + 2) > sizeof(acKey))
    {
        return FALSE;
    }

    sprintf(acKey, "[%s]%s", szGroupKey, szKey);

    return gos_hashlist_remove_string((GHASH*)m_hValues, (CHAR*)acKey);
}

BOOL GConfFile::DelGroup(const CHAR *szGroupName)
{
    UINT32              ulGroupID;
    CHAR                acGroupName[64];

    for (VECTOR<UINT32>::iterator iter = m_vGroupIDList.begin(); iter != m_vGroupIDList.end(); iter++)
    {
        ulGroupID = *iter;

        if (GetGroupName(ulGroupID, acGroupName, sizeof(acGroupName)))
        {
            if (strcasecmp(acGroupName, szGroupName) == 0)
            {
                m_vGroupIDList.erase(iter);

                return TRUE;
            }
        }
    }

    return FALSE;
}

BOOL GConfFile::SaveGroup(CHAR *szFile)
{
    CHAR                acFile[512+4];
    FILE                *fp = NULL;
    VECTOR<UINT32>      vGroupKey;
    GString             strData;

    if (!SaveGroup(strData))
    {
        return FALSE;
    }

    sprintf(acFile, "%s.tmp", m_acFile);
    fp = fopen(acFile, "wb");

    if (!fp)
    {
        return FALSE;
    }

    if (strData.Length() != fwrite(strData.GetString(), 1, strData.Length(), fp))
    {
        fclose(fp);
        return FALSE;
    }

    fclose(fp);

    if (szFile == NULL)
    {
        szFile = m_acFile;
    }

    return gos_rename_file(acFile, szFile);
}

BOOL GConfFile::SaveGroup(GString &strData)
{
    GHASHNODE           *pNode;
    GHASHNODE           *pNextNode;
    CONF_KEY_VALUE_T    *pstKeyValue;
    VECTOR<UINT32>      vGroupKey;
    UINT32              ulGroupID;
    CHAR                acGroupName[64] = {0};
    BOOL                bPrintGroup;

    strData.Clear();

    GetGroupIDList(vGroupKey);

    for (INT32 i=-1; i<(INT32)(vGroupKey.size()+1); i++)
    {
        bPrintGroup = FALSE;
        acGroupName[0] = '\0';

        if (i == -1)
        {
            ulGroupID = DEFAULT_GROUP_ID;
        }
        else if (i < (INT32)vGroupKey.size())
        {
            ulGroupID = vGroupKey[i];
            if (GetGroupName(ulGroupID, acGroupName, sizeof(acGroupName)))
            {
                if (acGroupName[0])
                {
                    bPrintGroup = TRUE;
                }
            }
        }
        else
        {
            ulGroupID = LAST_GROUP_ID;
        }

        pNode = gos_hashlist_get_first((GHASH*)m_hValues);
        while(pNode)
        {
            pNextNode = gos_hashlist_get_next((GHASH*)m_hValues, pNode);
            pstKeyValue = (CONF_KEY_VALUE_T*)gos_hashlist_get_node_data(pNode);

            if (pstKeyValue->ulGroupID != ulGroupID)
            {
                pNode = pNextNode;
                continue;
            }

            if (acGroupName[0])
            {
                if (bPrintGroup)
                {
                    if (pstKeyValue->szGroupComment && pstKeyValue->ulGroupCommentLen > 0)
                    {
                        strData.Append(pstKeyValue->szGroupComment, pstKeyValue->ulGroupCommentLen, FALSE);
                    }

                    strData.Append("[");
                    strData.Append(acGroupName);
                    strData.Append("]\r\n");

                    bPrintGroup = FALSE;
                }
            }

            if (pstKeyValue->szComment && pstKeyValue->ulCommentLen > 0)
            {
                strData.Append(pstKeyValue->szComment, pstKeyValue->ulCommentLen, FALSE);
            }

            if (pstKeyValue->acKey[0])
            {
                if (ulGroupID == DEFAULT_GROUP_ID)
                {
                    strData.Append(pstKeyValue->acKey);
                    strData.Append(" = ");
                    strData.Append(pstKeyValue->acValue);
                    strData.Append("\r\n");
                }
                else
                {
                    if (acGroupName[0])
                    {
                        strData.Append(pstKeyValue->acKey+strlen(acGroupName)+2);
                        strData.Append(" = ");
                        strData.Append(pstKeyValue->acValue);
                        strData.Append("\r\n");
                    }
                }
            }

            pNode = pNextNode;
        }
    }

    return TRUE;
}

BOOL GConfFile::Save(CHAR *szFile)
{
    return SaveGroup(szFile);
}


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
}KEY_VALUE_T;

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

VOID gos_conf_parse(GCONF *pConf, CHAR *szConf)
{
    CHAR            *szKey = szConf;
    CHAR            *szEnd;
    CHAR            *szValue;
    CHAR            *szGroupStart = NULL;
    CHAR            *szGroupEnd = NULL;
    BOOL            bRecoverFlag;
    BOOL            bIsEnd = FALSE;
    CHAR            *szCommentStart = NULL;
    CHAR            *szCommentEnd = NULL;
    CHAR            *szGroupCommentStart = NULL;
    CHAR            *szGroupCommentEnd = NULL;
    KEY_VALUE_T     stKeyValue;
    UINT32          ulGroupID = 0;
    BOOL            bHasGroupKey = FALSE;

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
            bHasGroupKey = FALSE;

            szGroupStart = szKey;
            szGroupEnd = strchr(szKey, ']');
            if (!szGroupEnd)
            {
                szGroupStart = NULL;
                szGroupEnd = NULL;
            }
            else
            {
                bHasGroupKey = TRUE;

                if (szCommentStart)
                {
                    szGroupCommentStart = szCommentStart;
                    szGroupCommentEnd = szKey;

                    szCommentStart = NULL;
                    szCommentEnd = NULL;
                }

                ulGroupID++;
                gos_list_add(pConf->pGroupIDList, &ulGroupID, sizeof(ulGroupID));
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

            if (szKey[0] == '!')
            {
                bRecoverFlag = TRUE;
                szKey++;
            }
            else
            {
                bRecoverFlag = FALSE;
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
                gos_hashlist_add(pConf->pKeyValueHash, (VOID*)&stKeyValue, sizeof(stKeyValue));

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

        gos_hashlist_add(pConf->pKeyValueHash, (VOID*)&stKeyValue, sizeof(stKeyValue));
    }
}

GCONF* gos_conf_init(CHAR *szFileName)
{
    GCONF   *pConf = (GCONF*)calloc(1, sizeof(GCONF));
    HANDLE  hFile = NULL;
    INT32   iSize;
    INT32   iReadSize;
    BOOL    bRet = FALSE;

    if (!pConf)
    {
        return NULL;
    }

    hFile = gos_fopen(szFileName, "rb");
    if (!hFile)
    {
        goto end;
    }

    iSize = gos_get_fsize(hFile);
    if (iSize < 0)
    {
        goto end;
    }

    pConf->pKeyValueHash = gos_hashlist_init(32, 0, 32, sizeof(KEY_VALUE_T), 0, _gos_hash_string, _gos_hash_key_cmp_string, gos_malloc, gos_free);
    if (!pConf->pKeyValueHash)
    {
        goto end;
    }

    pConf->pGroupIDList = gos_list_init(0, 0, sizeof(UINT32), NULL, NULL, NULL, NULL);
    if (!pConf->pGroupIDList)
    {
        goto end;
    }

    pConf->szConf = (CHAR*)calloc(1, iSize+1);

    if (!pConf->szConf)
    {
        goto end;
    }

    iReadSize = gos_fread(hFile, pConf->szConf, iSize);
    if (iSize != iReadSize)
    {
        goto end;
    }

    sprintf(pConf->acFile, szFileName);
    gos_format_path(pConf->acFile);

    gos_conf_parse(pConf, pConf->szConf);

    bRet = TRUE;

end:
    gos_fclose(hFile);

    if (!bRet)
    {
        gos_conf_free(pConf);
        pConf = NULL;
    }

    return pConf;
}

VOID gos_conf_free(GCONF *pConf)
{
    if (!pConf)
    {
        return;
    }

    gos_hashlist_free(pConf->pKeyValueHash);
    gos_list_free(pConf->pGroupIDList);
    GOS_FREE(pConf->szConf);

    free(pConf);
}

CHAR* gos_conf_get_string(GCONF *pConf, const CHAR *szKey)
{
    KEY_VALUE_T     *pstKey = (KEY_VALUE_T*)gos_hashlist_get_string(pConf->pKeyValueHash, (CHAR*)szKey);

    if (!pstKey)
    {
        return NULL;
    }

    return pstKey->acValue;
}

BOOL gos_conf_get_int32_ex(GCONF *pConf, const CHAR *szKey, INT32 iDefaultValue, INT32 *piValue)
{
    CHAR    *szValue = gos_conf_get_string(pConf, szKey);

    if (!szValue)
    {
        *piValue = iDefaultValue;
        return TRUE;
    }

    return gos_atoi(szValue, piValue);
}

CHAR* gos_conf_get_group_string(GCONF *pConf, const CHAR *szGroupKey, const CHAR *szKey)
{
    CHAR            acKey[128] = {0};
    KEY_VALUE_T     *pstKey = NULL;

    if (!szGroupKey || !szKey)
    {
        return NULL;
    }

    if ((strlen(szGroupKey) + strlen(szKey) + 2) > sizeof(acKey))
    {
        return NULL;
    }

    sprintf(acKey, "[%s]%s", szGroupKey, szKey);

    pstKey = (KEY_VALUE_T*)gos_hashlist_get_string(pConf->pKeyValueHash, (CHAR*)acKey);
    if (!pstKey)
    {
        return NULL;
    }

    return pstKey->acValue;
}

BOOL gos_conf_get_group_string_ex(GCONF *pConf, const CHAR *szGroupKey, const CHAR *szKey, CHAR *szRetValue, UINT32 ulMaxLen)
{
    CHAR    *szValue = gos_conf_get_group_string(pConf, szGroupKey, szKey);

    *szRetValue = '\0';
    if (!szValue || strlen(szValue) >= ulMaxLen)
    {
        return FALSE;
    }

    strcpy(szRetValue, szValue);

    return TRUE;
}

BOOL gos_conf_get_group_int32(GCONF *pConf, const CHAR *szGroupKey, const CHAR *szKey, INT32 iDefaultValue, INT32 *piValue)
{
    CHAR    *szValue = gos_conf_get_group_string(pConf, szGroupKey, szKey);

    if (!szValue)
    {
        *piValue = iDefaultValue;
        return TRUE;
    }

    return gos_atoi(szValue, piValue);
}

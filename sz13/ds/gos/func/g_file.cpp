#include "g_include.h"

GFile* GFile::Open(CHAR *szFile, const CHAR *szMode, BOOL bSafeMode)
{
    GFile   *pInstance = new GFile();

    if (!pInstance)
    {
        return NULL;
    }

    if (!pInstance->Init(szFile, szMode, bSafeMode))
    {
        delete pInstance;
        return NULL;
    }

    return pInstance;
}

BOOL GFile::Close(GFile *pstGFile)
{
    if (pstGFile == NULL)
    {
        return TRUE;
    }

    delete pstGFile;

    return TRUE;
}

GFile::GFile()
{
    m_acFile[0] = '0';
    m_acBakFile[0] = '0';
    m_hFile = NULL;
    m_bSafeMode = FALSE;
}

GFile::~GFile()
{
    if (m_hFile == NULL)
    {
        if (m_acBakFile[0] != '\0')
        {
            gos_rename_file(m_acBakFile, m_acFile);
        }
    }
    else
    {
        gos_fclose(m_hFile);

        gos_delete_file(m_acBakFile);
    }
}

BOOL GFile::CopyFile(CHAR *pchBakFile, CHAR *pchSourFile)
{
    HANDLE hBakFile   = NULL;
    HANDLE hSourFile  = NULL;
    INT32  iReadLen   = 0;
    INT32  iWriteLen  = 0;
    BOOL   bRet       = FALSE;
    UINT32 ulDataSize = sizeof(CHAR)*1024;
    CHAR  *pchData    = (CHAR *)malloc(ulDataSize);

    if (pchBakFile == NULL || pchSourFile == NULL)
    {
        goto END;
    }

    hBakFile = gos_fopen(pchBakFile, "wb");
    if (hBakFile == NULL)
    {
        goto END;
    }

    hSourFile = gos_fopen(pchSourFile, "rb");
    if (hSourFile == NULL)
    {
        goto END;
    }

    while (!gos_feof(hSourFile))
    {
        iReadLen = gos_fread(hSourFile, pchData, ulDataSize);
        if (iReadLen < 0)
        {
            goto END;
        }

        iWriteLen = gos_fwrite(hBakFile, pchData, iReadLen);
        if (iWriteLen != iReadLen)
        {
            goto END;
        }
    }

    bRet = TRUE;
END:
    free(pchData);

    gos_fclose(hSourFile);
    gos_fclose(hBakFile);

    return bRet;
}

BOOL GFile::Init(CHAR *szFile, const CHAR *szMode, BOOL bSafeMode)
{
    m_bSafeMode = bSafeMode;

    if (strlen(szFile) >= sizeof(m_acFile))
    {
        return FALSE;
    }

    strcpy(m_acFile, szFile);
    if (bSafeMode)
    {
        sprintf(m_acBakFile, "%s.bak", szFile);

        if (!CopyFile(m_acBakFile, szFile))
        {
            return FALSE;
        }
    }

    m_hFile = gos_fopen(szFile, szMode);
    if (m_hFile == NULL)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL GFile::Read(VOID *pvData, UINT32 ulLen)
{
    INT32   iLen = gos_fread(m_hFile, pvData, ulLen);

    if (iLen != (INT32)ulLen)
    {
        gos_fclose(m_hFile);

        m_hFile = NULL;

        return FALSE;
    }

    return TRUE;
}

BOOL GFile::Write(VOID *pvData, UINT32 ulLen)
{
    INT32   iLen = gos_fwrite(m_hFile, pvData, ulLen);

    if (iLen != (INT32)ulLen)
    {
        gos_fclose(m_hFile);

        m_hFile = NULL;

        return FALSE;
    }

    return TRUE;
}

UINT32 GFile::Printf(const CHAR *szFormat, ...)
{
    va_list vaText;
    FILE    *fp = gos_get_fp(m_hFile);
    INT32   iLen;

    if (!fp)
    {
        return 0;
    }

    va_start(vaText, szFormat);

    iLen = vfprintf(fp, szFormat, vaText);

    va_end(vaText);

    if (iLen < 0)
    {
        gos_fclose(m_hFile);

        m_hFile = NULL;

        return 0;
    }

    return (UINT32)iLen;
}

VOID GFile::Flush()
{
    gos_fflush(m_hFile);
}

FILE* GFile::GetFile()
{
    return gos_get_fp(m_hFile);
}

BOOL GFile::Eof()
{
    return gos_feof(m_hFile);
}

INT64 GFile::GetPos()
{
    INT64   i64Pos = -1;

    if (gos_fgetpos(m_hFile, &i64Pos) < 0)
    {
        return -1;
    }

    return i64Pos;
}

BOOL GFile::SetPos(INT64 i64Pos)
{
    if (gos_fsetpos(m_hFile, i64Pos) < 0)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL GFile::Skip(INT64 i64SkipLen)
{
    if (gos_fskip(m_hFile, i64SkipLen) < 0)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL GFile::Skip(INT64 i64Offset, INT32 iWhence)
{
    if (gos_fseek(m_hFile, i64Offset, iWhence) < 0)
    {
        return FALSE;
    }

    return TRUE;
}

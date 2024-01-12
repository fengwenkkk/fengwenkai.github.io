#include "g_include.h"

GFileDB::GFileDB(UINT32 ulMaxBufRecNum, UINT32 ulMaxBufTime)
{
    memset(&m_stFileHdr, 0, sizeof(m_stFileHdr));

    m_hFile = NULL;
    m_ulMaxBufRecNum = ulMaxBufRecNum;
    m_ulMaxBufTime = ulMaxBufTime;

    m_ulLastSaveTime = 0;
}

GFileDB::~GFileDB()
{
    gos_fclose(m_hFile);
}

BOOL GFileDB::SetPos(INT64 i64Pos)
{
    if (gos_fseek(m_hFile, i64Pos, SEEK_SET) < 0)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL GFileDB::Read(VOID *pucData, UINT32 ulLen)
{
    if ((INT32)ulLen != gos_fread(m_hFile, pucData, ulLen))
    {
        gos_fclose(m_hFile);
        m_hFile = NULL;

        return FALSE;
    }

    return TRUE;
}

BOOL GFileDB::Write(VOID *pucData, UINT32 ulLen)
{
    if ((INT32)ulLen != gos_fwrite(m_hFile, pucData, ulLen))
    {
        gos_fclose(m_hFile);
        m_hFile = NULL;

        return FALSE;
    }

    return TRUE;
}

BOOL GFileDB::Create(CHAR *szFileName, UINT32 ulVer, UINT32 ulRecLen, UINT32 ulMaxRecNum)
{
    INT32  iBufSize = m_ulMaxBufRecNum*m_stFileHdr.ulRecLen;

    if (iBufSize == 0)
    {
        iBufSize = -1;
    }

    m_hFile = gos_fopen_ex(szFileName, "w+b", iBufSize, NULL);

    if (!m_hFile)
    {
        return FALSE;
    }

    m_stFileHdr.ulVer = ulVer;
    m_stFileHdr.ulRecLen = ulRecLen;
    m_stFileHdr.ulMaxRecNum = ulMaxRecNum;
    m_stFileHdr.ulRecNum = 0;
    m_stFileHdr.ulStartRecIndex = 0;

    if (!Write(&m_stFileHdr, sizeof(m_stFileHdr)))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL GFileDB::Load(CHAR *szFileName, UINT32 ulVer, UINT32 ulRecLen, UINT32 ulMaxRecNum)
{
    INT32  iBufSize = m_ulMaxBufRecNum*m_stFileHdr.ulRecLen;

    if (iBufSize == 0)
    {
        iBufSize = -1;
    }

    m_hFile = gos_fopen_ex(szFileName, "r+b", iBufSize, NULL);

    if (!m_hFile)
    {
        return Create(szFileName, ulVer, ulRecLen, ulMaxRecNum);
    }

    if (!Read(&m_stFileHdr, sizeof(m_stFileHdr)))
    {
        return Create(szFileName, ulVer, ulRecLen, ulMaxRecNum);
    }

    if (ulVer != m_stFileHdr.ulVer ||
        ulRecLen != m_stFileHdr.ulRecLen)
    {
        return Create(szFileName, ulVer, ulRecLen, ulMaxRecNum);
    }

    if (m_ulMaxBufRecNum != m_stFileHdr.ulMaxRecNum)
    {
        if (!RewriteFile())
        {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL GFileDB::Add(VOID *pucRec)
{
    UINT32  ulTime = gos_get_uptime() - m_ulLastSaveTime;
    UINT32  ulWriteRecIndex;

    if (m_stFileHdr.ulRecNum < m_stFileHdr.ulMaxRecNum)
    {
        ulWriteRecIndex = m_stFileHdr.ulRecNum;
        m_stFileHdr.ulRecNum++;
    }
    else
    {
        ulWriteRecIndex = m_stFileHdr.ulStartRecIndex;//m_stFileHdr.ulRecNum;
        m_stFileHdr.ulStartRecIndex = MOD(m_stFileHdr.ulStartRecIndex+1, m_stFileHdr.ulMaxRecNum);
    }

    INT64   i64WritePos = sizeof(m_stFileHdr) + m_stFileHdr.ulRecLen*ulWriteRecIndex;

    if (!SetPos(i64WritePos))
    {
        goto fail;
    }

    if (!Write(pucRec, m_stFileHdr.ulRecLen))
    {
        goto fail;
    }

    if (!SetPos(0))
    {
        goto fail;
    }

    if (!Write(&m_stFileHdr, sizeof(m_stFileHdr)))
    {
        goto fail;
    }

    if (ulTime > m_ulMaxBufTime)
    {
        Flush();
    }

    return TRUE;

fail:
    gos_fclose(m_hFile);
    m_hFile = NULL;

    return FALSE;
}

VOID GFileDB::Clear()
{
    m_stFileHdr.ulStartRecIndex = 0;
    m_stFileHdr.ulRecNum = 0;

    m_ulLastSaveTime = gos_get_uptime();

    if (!SetPos(0))
    {
        return;
    }

    if (!Write(&m_stFileHdr, sizeof(m_stFileHdr)))
    {
        return;
    }
}

UINT32 GFileDB::GetNum()
{
    if (!m_hFile)
    {
        return 0;
    }

    return m_stFileHdr.ulRecNum;
}

BOOL GFileDB::Get(UINT32 ulMaxRecNum, VOID *pvRec, UINT32 *pulRecNum)
{
    UINT32  i;
    UINT8   *pucRec = (UINT8*)pvRec;
    INT64   i64ReadPos = sizeof(m_stFileHdr) + m_stFileHdr.ulRecLen*m_stFileHdr.ulStartRecIndex;
    UINT32  ulReadPos;

    pulRecNum[0] = 0;

    if (!SetPos(i64ReadPos))
    {
        return FALSE;
    }

    if (ulMaxRecNum > m_stFileHdr.ulRecNum)
    {
        ulMaxRecNum = m_stFileHdr.ulRecNum;
    }

    for (i=0; i<ulMaxRecNum; i++)
    {
        // 到文件尾，折返从文件记录块起始位置读取
        ulReadPos = i+m_stFileHdr.ulStartRecIndex;
        if (ulReadPos == m_stFileHdr.ulMaxRecNum)
        {
            if (!SetPos(sizeof(m_stFileHdr)))
            {
                return FALSE;
            }
        }

        if (!Read(pucRec, m_stFileHdr.ulRecLen))
        {
            return FALSE;
        }

        pulRecNum[0] ++;
        pucRec += m_stFileHdr.ulRecLen;
    }

    return TRUE;
}

BOOL GFileDB::Get(UINT32 ulRecIndex, VOID *pucRec)
{
    if (ulRecIndex >= m_stFileHdr.ulRecNum)
    {
        return FALSE;
    }

    UINT32  ulRecPos = MOD(ulRecIndex + m_stFileHdr.ulStartRecIndex, m_stFileHdr.ulMaxRecNum);//;
    INT64   i64Pos   = sizeof(m_stFileHdr) + ulRecPos*m_stFileHdr.ulRecLen;

    if (!SetPos(i64Pos))
    {
        return FALSE;
    }

    if (!Read(pucRec, m_stFileHdr.ulRecLen))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL GFileDB::Flush()
{
    gos_fflush(m_hFile);

    m_ulLastSaveTime = gos_get_uptime();

    return TRUE;
}

BOOL GFileDB::RewriteFile()
{
    HANDLE hFile         = NULL;
    UINT32 ulCopyDataNum = 0;
    UINT32 i             = 0;
    UINT32 ulIndex       = 0;
    INT32  iWriteLen     = 0;
    VOID  *pvData        = malloc(m_stFileHdr.ulRecLen);

    CHAR acFileName[256];

    sprintf(acFileName, "%s/config/TempFile.temp", gos_get_root_path());

    if (pvData == NULL)
    {
        goto fail;
    }

    hFile = gos_fopen(acFileName, "w+b");
    if (hFile == NULL)
    {
        goto fail;
    }

    if (m_ulMaxBufRecNum > m_stFileHdr.ulMaxRecNum)
    {
        ulCopyDataNum = m_stFileHdr.ulRecNum;
    }
    else
    {
        if (m_stFileHdr.ulRecNum < m_ulMaxBufRecNum)
        {
            ulCopyDataNum = m_stFileHdr.ulRecNum;
        }
        else
        {
            ulCopyDataNum = m_ulMaxBufRecNum;

            ulIndex = m_stFileHdr.ulRecNum - m_ulMaxBufRecNum;
        }
    }

    for (i=0; i<ulCopyDataNum; i++)
    {
        if (!Get(ulIndex, pvData))
        {
            goto fail;
        }

        iWriteLen = gos_fwrite(hFile, pvData, m_stFileHdr.ulRecLen);
        if ((UINT32)iWriteLen != m_stFileHdr.ulRecLen)
        {
            goto fail;
        }

        ulIndex ++;
    }

    gos_fseek(hFile, 0, SEEK_SET);

    if (!SetPos(0))
    {
        goto fail;
    }

    m_stFileHdr.ulMaxRecNum     = m_ulMaxBufRecNum;
    m_stFileHdr.ulRecNum        = ulCopyDataNum;
    m_stFileHdr.ulStartRecIndex = 0;

    if (!Write(&m_stFileHdr, sizeof(m_stFileHdr)))
    {
        goto fail;
    }

    for (i=0; i<ulCopyDataNum && !gos_feof(hFile); i++)
    {
        iWriteLen = gos_fread(hFile, pvData, m_stFileHdr.ulRecLen);
        if ((UINT32)iWriteLen != m_stFileHdr.ulRecLen)
        {
            goto fail;
        }

        if (!Write(pvData, iWriteLen))
        {
            goto fail;
        }
    }

    gos_fclose(hFile);

    gos_delete_file(acFileName);

    free(pvData);

    return TRUE;
fail:
    gos_fclose(m_hFile);

    m_hFile = NULL;

    gos_fclose(hFile);

    free(pvData);

    return FALSE;
}

#include "g_include.h"

static BOOL gos_filedb_set_pos(FILEDB_HANDLE_T *pstHandle, INT32 iPos)
{
    if (iPos < 0 || gos_fseek(pstHandle->m_hFile, iPos, SEEK_SET) < 0)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL gos_filedb_read(FILEDB_HANDLE_T *pstHandle, VOID *pucData, UINT32 ulLen)
{
    if ((INT32)ulLen != gos_fread(pstHandle->m_hFile, pucData, ulLen))
    {
        gos_fclose(pstHandle->m_hFile);
        pstHandle->m_hFile = NULL;

        return FALSE;
    }

    return TRUE;
}

BOOL gos_filedb_write(FILEDB_HANDLE_T *pstHandle, VOID *pucData, UINT32 ulLen)
{
    INT32   iLen;

    if (!pstHandle->m_hFile)
    {
        printf("invalid file handle\n");
        return FALSE;
    }

    iLen = gos_fwrite(pstHandle->m_hFile, pucData, ulLen);
    if (iLen != ulLen)
    {
        gos_fclose(pstHandle->m_hFile);
        pstHandle->m_hFile = NULL;

        printf("file write %d failed, return %d\n", ulLen, iLen);

        return FALSE;
    }

    return TRUE;
}

HANDLE gos_filedb_init(UINT32 ulMaxBufRecNum, UINT32 ulMaxBufTime)
{
    FILEDB_HANDLE_T     *pstHandle = calloc(1, sizeof(FILEDB_HANDLE_T));

    if (!pstHandle)
    {
        return NULL;
    }

    pstHandle->m_hFile = NULL;
    pstHandle->m_ulMaxBufRecNum = ulMaxBufRecNum;
    pstHandle->m_ulMaxBufTime = ulMaxBufTime;
    pstHandle->m_ulLastSaveTime = 0;

    return pstHandle;
}

VOID gos_filedb_free(HANDLE hFileDB)
{
    FILEDB_HANDLE_T     *pstHandle = (FILEDB_HANDLE_T*)hFileDB;

    if (pstHandle)
    {
        gos_fclose(pstHandle->m_hFile);

        free(pstHandle);
    }
}

BOOL gos_filedb_create(HANDLE hFileDB, CHAR *szFileName, UINT32 ulVer, UINT32 ulRecLen, UINT32 ulMaxRecNum)
{
    FILEDB_HANDLE_T *pstHandle = (FILEDB_HANDLE_T*)hFileDB;
    INT32           iBufSize = -1;

    if (!pstHandle)
    {
        return FALSE;
    }

    if (0 == ulRecLen)
    {
        printf("invalid rec len %u\n", ulRecLen);
        return FALSE;
    }

    if (0 == ulMaxRecNum)
    {
        printf("invalid max rec num %u\n", ulMaxRecNum);
        return FALSE;
    }

    pstHandle->m_hFile = gos_fopen_ex(szFileName, "w+b", iBufSize, NULL);
    if (!pstHandle->m_hFile)
    {
        printf("reset file %s failed\n", szFileName);
        return FALSE;
    }

    pstHandle->m_stFileHdr.ulVer = ulVer;
    pstHandle->m_stFileHdr.ulRecLen = ulRecLen;
    pstHandle->m_stFileHdr.ulMaxRecNum = ulMaxRecNum;
    pstHandle->m_stFileHdr.ulRecNum = 0;
    pstHandle->m_stFileHdr.ulStartRecIndex = 0;

    if (!gos_filedb_write(pstHandle, &pstHandle->m_stFileHdr, sizeof(pstHandle->m_stFileHdr)))
    {
        printf("write hdr of file %s failed\n", szFileName);
        return FALSE;
    }

    gos_filedb_flush(pstHandle);

    return TRUE;
}

BOOL gos_filedb_load(HANDLE hFileDB, CHAR *szFileName, UINT32 ulVer, UINT32 ulRecLen, UINT32 ulMaxRecNum)
{
    FILEDB_HANDLE_T *pstHandle = (FILEDB_HANDLE_T*)hFileDB;
    INT32           iBufSize;

    if (!pstHandle)
    {
        return FALSE;
    }

    iBufSize = pstHandle->m_ulMaxBufRecNum*pstHandle->m_stFileHdr.ulRecLen;
    if (iBufSize == 0)
    {
        iBufSize = -1;
    }

    if (!gos_file_exist(szFileName))
    {
        return gos_filedb_create(hFileDB, szFileName, ulVer, ulRecLen, ulMaxRecNum);
    }

    pstHandle->m_hFile = gos_fopen_ex(szFileName, "r+b", iBufSize, NULL);
    if (!pstHandle->m_hFile)
    {
        printf("open file %s failed\n", szFileName);
        return gos_filedb_create(hFileDB, szFileName, ulVer, ulRecLen, ulMaxRecNum);
    }

    if (!gos_filedb_read(pstHandle, &pstHandle->m_stFileHdr, sizeof(pstHandle->m_stFileHdr)))
    {
        printf("read file %s failed\n", szFileName);
        return gos_filedb_create(hFileDB, szFileName, ulVer, ulRecLen, ulMaxRecNum);
    }

    if (ulVer != pstHandle->m_stFileHdr.ulVer ||
        ulRecLen != pstHandle->m_stFileHdr.ulRecLen)
    {
        printf("invalid file hdr, ver=%X, rec len=%u\n", pstHandle->m_stFileHdr.ulVer, pstHandle->m_stFileHdr.ulRecLen);
        return gos_filedb_create(hFileDB, szFileName, ulVer, ulRecLen, ulMaxRecNum);
    }

    printf("load file ver=%X, rec len=%u, rec num=%d\n", ulVer, ulRecLen, pstHandle->m_stFileHdr.ulRecNum);

    return TRUE;
}

BOOL gos_filedb_add(HANDLE hFileDB, VOID *pucRec, UINT32 ulRecLen)
{
    FILEDB_HANDLE_T *pstHandle = (FILEDB_HANDLE_T*)hFileDB;
    UINT32          ulTime;
    UINT32          ulWriteRecIndex;
    INT32           iWritePos;

    if (!pstHandle)
    {
        printf("invalid scan rec handle\n");
        return FALSE;
    }

    if (ulRecLen != pstHandle->m_stFileHdr.ulRecLen)
    {
        printf("invalid rec len(%u), it should be %u\n", ulRecLen, pstHandle->m_stFileHdr.ulRecLen);
        return FALSE;
    }

    ulTime = gos_get_uptime() - pstHandle->m_ulLastSaveTime;

    if (pstHandle->m_stFileHdr.ulRecNum < pstHandle->m_stFileHdr.ulMaxRecNum)
    {
        ulWriteRecIndex = pstHandle->m_stFileHdr.ulRecNum;
        pstHandle->m_stFileHdr.ulRecNum++;
    }
    else
    {
        ulWriteRecIndex = pstHandle->m_stFileHdr.ulStartRecIndex;
        pstHandle->m_stFileHdr.ulStartRecIndex = MOD(pstHandle->m_stFileHdr.ulStartRecIndex+1, pstHandle->m_stFileHdr.ulMaxRecNum);
    }

    iWritePos = sizeof(pstHandle->m_stFileHdr) + pstHandle->m_stFileHdr.ulRecLen*ulWriteRecIndex;
    if (!gos_filedb_set_pos(pstHandle, iWritePos))
    {
        printf("filedb set pos to %u failed\n", iWritePos);
        goto fail;
    }

    if (!gos_filedb_write(pstHandle, pucRec, pstHandle->m_stFileHdr.ulRecLen))
    {
        printf("filedb write rec failed\n");
        goto fail;
    }

    if (!gos_filedb_set_pos(pstHandle, 0))
    {
        printf("filedb set pos to 0 failed\n");
        goto fail;
    }

    if (!gos_filedb_write(pstHandle, &pstHandle->m_stFileHdr, sizeof(pstHandle->m_stFileHdr)))
    {
        printf("filedb write hdr failed\n");
        goto fail;
    }

    if (ulTime > pstHandle->m_ulMaxBufTime)
    {
        gos_filedb_flush(pstHandle);
    }

    return TRUE;

fail:
    gos_fclose(pstHandle->m_hFile);
    pstHandle->m_hFile = NULL;

    return FALSE;
}

VOID gos_filedb_clear(HANDLE hFileDB)
{
    FILEDB_HANDLE_T *pstHandle = (FILEDB_HANDLE_T*)hFileDB;

    if (!pstHandle)
    {
        return;
    }

    pstHandle->m_stFileHdr.ulStartRecIndex = 0;
    pstHandle->m_stFileHdr.ulRecNum = 0;

    pstHandle->m_ulLastSaveTime = gos_get_uptime();

    if (!gos_filedb_set_pos(pstHandle, 0))
    {
        return;
    }

    if (!gos_filedb_write(pstHandle, &pstHandle->m_stFileHdr, sizeof(pstHandle->m_stFileHdr)))
    {
        return;
    }
}

UINT32 gos_filedb_get_rec_num(HANDLE hFileDB)
{
    FILEDB_HANDLE_T *pstHandle = (FILEDB_HANDLE_T*)hFileDB;

    if (!pstHandle)
    {
        return 0;
    }

    if (!pstHandle->m_hFile)
    {
        return 0;
    }

    return pstHandle->m_stFileHdr.ulRecNum;
}

BOOL gos_filedb_get_multi_rec(HANDLE hFileDB, UINT32 ulMaxRecNum, VOID *pvRec, UINT32 *pulRecNum)
{
    FILEDB_HANDLE_T *pstHandle = (FILEDB_HANDLE_T*)hFileDB;
    UINT32  i;
    UINT8   *pucRec = (UINT8*)pvRec;
    INT32   iReadPos = sizeof(pstHandle->m_stFileHdr) + pstHandle->m_stFileHdr.ulRecLen*pstHandle->m_stFileHdr.ulStartRecIndex;
    UINT32  ulCurrReadPos;

    pulRecNum[0] = 0;

    if (!pstHandle)
    {
        return FALSE;
    }

    if (!gos_filedb_set_pos(pstHandle, iReadPos))
    {
        return FALSE;
    }

    if (ulMaxRecNum > pstHandle->m_stFileHdr.ulRecNum)
    {
        ulMaxRecNum = pstHandle->m_stFileHdr.ulRecNum;
    }

    for (i=0; i<ulMaxRecNum; i++)
    {
        // 到文件尾，折返从文件记录块起始位置读取
        ulCurrReadPos = i+pstHandle->m_stFileHdr.ulStartRecIndex;
        if (ulCurrReadPos == pstHandle->m_stFileHdr.ulMaxRecNum)
        {
            if (!gos_filedb_set_pos(pstHandle, sizeof(pstHandle->m_stFileHdr)))
            {
                return FALSE;
            }
        }

        if (!gos_filedb_read(pstHandle, pucRec, pstHandle->m_stFileHdr.ulRecLen))
        {
            return FALSE;
        }

        pulRecNum[0] ++;
        pucRec += pstHandle->m_stFileHdr.ulRecLen;
    }

    return TRUE;
}

BOOL gos_filedb_get_rec(HANDLE hFileDB, UINT32 ulRecIndex, VOID *pucRec)
{
    FILEDB_HANDLE_T *pstHandle = (FILEDB_HANDLE_T*)hFileDB;
    UINT32  ulRecPos;
    INT     iPos;

    if (!pstHandle)
    {
        return FALSE;
    }

    if (ulRecIndex >= pstHandle->m_stFileHdr.ulRecNum)
    {
        return FALSE;
    }

    ulRecPos = MOD(ulRecIndex + pstHandle->m_stFileHdr.ulStartRecIndex, pstHandle->m_stFileHdr.ulMaxRecNum);//;
    iPos   = sizeof(pstHandle->m_stFileHdr) + ulRecPos*pstHandle->m_stFileHdr.ulRecLen;

    if (!gos_filedb_set_pos(pstHandle, iPos))
    {
        return FALSE;
    }

    if (!gos_filedb_read(pstHandle, pucRec, pstHandle->m_stFileHdr.ulRecLen))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL gos_filedb_flush(HANDLE hFileDB)
{
    FILEDB_HANDLE_T *pstHandle = (FILEDB_HANDLE_T*)hFileDB;

    if (!pstHandle)
    {
        return FALSE;
    }

    gos_fflush(pstHandle->m_hFile);

    pstHandle->m_ulLastSaveTime = gos_get_uptime();

    return TRUE;
}


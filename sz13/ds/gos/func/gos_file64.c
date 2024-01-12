#include "gos_api.h"
#include "gos_func.h"
#include "gos_file.h"
#include "gos_hashlist.h"

#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER  0xFFFFFFFF
#endif

extern CHAR* gos_get_text_time(UINT32 *pulCurrTime);

/************************************************************************/
/* ��ȡ������Խṹ��ͷָ���ƫ����                                     */
/************************************************************************/
typedef struct
{
    CHAR                    acFile[512];

    FILE                    *fp;
    INT32                   fd;

    BOOL                    bBufMode;       /* �Ƿ���û���
                                               FALSE:�����棬����read/write
                                               TRUE :���棬����fread/fwrite */

    UINT8                   *pucBuf;

    BOOL                    bNeedFree;      /* pucBuf�Ƿ���Ҫ�ͷ� */
#ifdef _BCB_
    INT64                   i64Pos;
#endif
}GOS_FILE_T;

#define GOS_MAX_OPENED_FILE_NUM     32
typedef struct
{
    CHAR                    acFile[512];
    UINT32                  ulOpenTime;
}_GOS_OPEN_FILE_INFO_T;

static HANDLE       s_hFileList = NULL;

static HANDLE       s_hMutex = NULL;
static UINT32       s_ulOpenedFileNum = 0;

BOOL gos_file_init(VOID)
{
    if (s_hFileList)
    {
        return TRUE;
    }

    if (!s_hMutex)
    {
        s_hMutex = gos_mutex_init();
        if (!s_hMutex)
        {
            return FALSE;
        }
    }

    gos_mutex(s_hMutex);
    s_hFileList = gos_hash_init(0, GOS_MAX_OPENED_FILE_NUM, GOS_MAX_OPENED_FILE_NUM*2, sizeof(_GOS_OPEN_FILE_INFO_T), 0,
                                gos_hash_string, gos_hash_key_cmp_string, gos_malloc, gos_free);
    gos_unmutex(s_hMutex);

    if (!s_hFileList)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

/*******************************************************************************
* ��������: gos_fopen_ex
* ��    ��: ��ʼ���ļ�����
* ��    ��:
* ��������          ����/���       ����
* szFileName        IN              �ļ���
* szOpenMode        IN              �򿪷�ʽ
* iBufSize          IN              д�ļ���������С(
                                    <0: �޻���
                                    =0: fwrite�ڲ��������
                                    >0: �������û���
* pucBuf            IN              �ⲿ�ļ�������
* ��������:
* ˵    ��:
*******************************************************************************/
HANDLE gos_fopen_ex(CHAR *szFileName, const CHAR *szOpenMode, INT32 iBufSize, UINT8 *pucBuf)
{
    GOS_FILE_T      *pstFile = NULL;
    UINT32          ulBufSize = iBufSize;      /* �ļ���������С */
    _GOS_OPEN_FILE_INFO_T   stFileInfo;

    if (NULL == szFileName || *szFileName == '\0' ||
        NULL == szOpenMode || *szOpenMode == '\0' )
    {
        return NULL;
    }

    memset(&stFileInfo, 0, sizeof(stFileInfo));

    pstFile = (GOS_FILE_T*)GOS_MALLOC(sizeof(GOS_FILE_T));
    if (!pstFile)
    {
        return NULL;
    }

    memset(pstFile, 0, sizeof(GOS_FILE_T));

    if (strchr(szOpenMode, 'r') != NULL)
    {
        pucBuf = NULL;
        iBufSize = 0;
    }

    /* ������ */
    if (iBufSize < 0)
    {
        pstFile->bBufMode = FALSE;
    }
    /* ����fwrite�Ļ������ */
    else if (0 == iBufSize)
    {
        pstFile->bBufMode = TRUE;
    }
    /* ����fwrite�Ļ������ */
    else
    {
        pstFile->bBufMode = TRUE;

        /* ָ��bufʱ�������ⲿbuf */
        if (pucBuf)
        {
            pstFile->pucBuf = pucBuf;
        }
        else
        {
            /* δָ��bufʱ����̬���� */
            ulBufSize = ((iBufSize+1023)/1024)*1024;
            pstFile->pucBuf = (UINT8*)GOS_MALLOC(ulBufSize);
            if (NULL == pstFile->pucBuf)
            {
                /* �˻�Ϊfwrite����ģʽ */
                ulBufSize = 0;
            }
            else
            {
                pstFile->bNeedFree = TRUE;
            }
        }
    }

    strncpy(pstFile->acFile, szFileName, sizeof(pstFile->acFile)-1);
    gos_format_path(pstFile->acFile);

    pstFile->fp = fopen(pstFile->acFile, szOpenMode);

    if (NULL == pstFile->fp)
    {
        if (pstFile->bNeedFree)
        {
            GOS_FREE(pstFile->pucBuf);
        }

        GOS_FREE(pstFile);

        return NULL;
    }

    pstFile->fd = fileno(pstFile->fp);

    if (pstFile->pucBuf)
    {
        setvbuf(pstFile->fp, (CHAR*)pstFile->pucBuf, _IOFBF, ulBufSize);
    }

    if (s_hFileList)
    {
        gos_mutex(s_hMutex);

        s_ulOpenedFileNum++;
        strncpy(stFileInfo.acFile, pstFile->acFile, sizeof(stFileInfo.acFile)-1);
        stFileInfo.ulOpenTime = time(NULL);
        gos_hash_add((GHASH*)s_hFileList, &stFileInfo, sizeof(stFileInfo));

        if (s_ulOpenedFileNum > GOS_MAX_OPENED_FILE_NUM)
        {
            gos_unmutex(s_hMutex);

            gos_fclose(pstFile);

            return NULL;
        }

        gos_unmutex(s_hMutex);
    }

    return pstFile;
}

HANDLE gos_fopen(CHAR *szFileName, const CHAR *szOpenMode)
{
    return gos_fopen_ex(szFileName, szOpenMode, 0, NULL);
}

BOOL gos_feof(HANDLE hFile)
{
    GOS_FILE_T  *pstFile = (GOS_FILE_T *)hFile;

    if (!hFile)
    {
        return TRUE;
    }

    return feof(pstFile->fp);
}

FILE *gos_get_fp(HANDLE hFile)
{
    GOS_FILE_T  *pstFile = (GOS_FILE_T *)hFile;

    if (!hFile)
    {
        return NULL;
    }

    return pstFile->fp;
}

CHAR *gos_get_fname(HANDLE hFile)
{
    GOS_FILE_T  *pstFile = (GOS_FILE_T *)hFile;

    if (!hFile)
    {
        return "";
    }

    return pstFile->acFile;
}

UINT32 gos_get_fsize(HANDLE hFile)
{
    UINT32 ulFileSize = 0;

    if (!hFile)
    {
        return 0;
    }

    gos_fseek(hFile, 0, SEEK_END);

    ulFileSize = gos_ftell(hFile);

    gos_fseek(hFile, 0, SEEK_SET);

    return ulFileSize;
}

INT32 gos_ftell(HANDLE hFile)
{
    GOS_FILE_T *pstFile = (GOS_FILE_T *)hFile;

    if (!hFile)
    {
        return 0;
    }

    return ftell(pstFile->fp);
}

/*******************************************************************************
* ��������: gos_fclose
* ��    ��: �ر��ļ�����
* ��    ��:
* ��������          ����/���       ����
* pstFile           IN              �ļ�����
* ��������:
* ˵    ��:
*******************************************************************************/
VOID gos_fclose(HANDLE hFile)
{
    GOS_FILE_T *pstFile = (GOS_FILE_T *)hFile;

    if (!pstFile)
    {
        return;
    }

    if (s_hFileList)
    {
        gos_mutex(s_hMutex);
        s_ulOpenedFileNum--;
        gos_hash_remove((GHASH*)s_hFileList, pstFile->acFile);
        gos_unmutex(s_hMutex);
    }

    if (pstFile->fp)
    {
        fclose(pstFile->fp);
    }

    if (pstFile->bNeedFree)
    {
        GOS_FREE(pstFile->pucBuf);
    }

    GOS_FREE(pstFile);
}

VOID gos_print_opened_file(VOID)
{
    GHASHNODE *pNode = NULL;
    _GOS_OPEN_FILE_INFO_T   *pstFileInfo;

    if (s_hFileList)
    {
        gos_mutex(s_hMutex);

        pNode = gos_hash_get_first((GHASH*)s_hFileList);
        while(pNode)
        {
            pstFileInfo = (_GOS_OPEN_FILE_INFO_T*)gos_hash_get_node_data(pNode);
            if (pstFileInfo)
            {
                printf("\n ---> [%s] %s", gos_get_text_time(&pstFileInfo->ulOpenTime), pstFileInfo->acFile);
            }

            pNode = gos_hash_get_next((GHASH*)s_hFileList, pNode);
        }

        gos_unmutex(s_hMutex);
    }
}

/*******************************************************************************
* ��������: gos_fwrite
* ��    ��: �ļ�����д����
* ��    ��:
* ��������          ����/���       ����
* pstFile           IN              �ļ�����
* pData             IN              �ļ�����
* ulLen             IN              д���ݴ�С
* ��������: TRUE/FALSE
* ˵    ��:
*******************************************************************************/
INT32 gos_fwrite(HANDLE hFile, VOID* pData, UINT32 ulLen)
{
    GOS_FILE_T  *pstFile = (GOS_FILE_T *)hFile;
    INT32       iWriteSize;

    if (!hFile)
    {
        return -1;
    }

    if (!pstFile->bBufMode)
    {
#ifdef _OSWIN32_
        iWriteSize = _write(pstFile->fd, pData, ulLen);
#else
        iWriteSize = write(pstFile->fd, pData, ulLen);
#endif
    }
    else
    {
        iWriteSize = fwrite(pData, 1, ulLen, pstFile->fp);
    }

    if (iWriteSize != (INT32)ulLen)
    {
        return -1;
    }

#ifdef _BCB_
    pstFile->i64Pos += iWriteSize;
#endif

    return iWriteSize;
}

/*******************************************************************************
* ��������: gos_fread
* ��    ��: �ļ����������
* ��    ��:
* ��������          ����/���       ����
* pstFile           IN              �ļ�����
* pData             IN              �ļ�����
* ulLen             IN              ��ȡ����
* ��������: TRUE/FALSE
* ˵    ��:
*******************************************************************************/
INT32 gos_fread(HANDLE hFile, VOID* pData, UINT32 ulLen)
{
    GOS_FILE_T  *pstFile = (GOS_FILE_T *)hFile;
    INT32       iReadSize;

    if (!hFile)
    {
        return -1;
    }

    if (!pstFile->bBufMode)
    {
#ifdef _OSWIN32_
        iReadSize = _read(pstFile->fd, pData, ulLen);
#else
        iReadSize = read(pstFile->fd, pData, ulLen);
#endif
    }
    else
    {
        iReadSize = fread(pData, 1, ulLen, pstFile->fp);
    }

#ifdef _BCB_
    if (iReadSize > 0)
    {
        pstFile->i64Pos += iReadSize;
    }
#endif

    return iReadSize;
}

/*******************************************************************************
* ��������: gos_fgetpos
* ��    ��: ��ȡ�ļ������ǰƫ����
* ��    ��:
* ��������          ����/���       ����
* pstFile           IN              �ļ�����
* piPos             OUT             ƫ����
* ��������:
* ˵    ��:
*******************************************************************************/
INT32 gos_fgetpos(HANDLE hFile, INT64 *piPos)
{
    INT32       iRet = 0;
    GOS_FILE_T  *pstFile = (GOS_FILE_T *)hFile;

    if (!hFile)
    {
        return -1;
    }

#ifdef _OSWIN32_
#ifdef _BCB_
    *piPos = pstFile->i64Pos;
#else
    iRet = fgetpos(pstFile->fp, piPos);
#endif
#else
    fpos64_t    i64Pos = {0};

    iRet = fgetpos64(pstFile->fp, &i64Pos);

    *piPos = (INT64)i64Pos.__pos;
#endif

    return iRet;
}

/*******************************************************************************
* ��������: gos_fsetpos
* ��    ��: �����ļ������ǰƫ����
* ��    ��:
* ��������          ����/���       ����
* pstFile           IN              �ļ�����
* iOffset           IN              ƫ����
* ��������:
* ˵    ��:
*******************************************************************************/
INT32 gos_fsetpos(HANDLE hFile, INT64 iOffset)
{
    GOS_FILE_T  *pstFile = (GOS_FILE_T *)hFile;

#ifdef _OSWIN32_
#ifdef _BCB_
    INT32    iPos = (INT32)iOffset;

    if (!hFile)
    {
        return -1;
    }

    if (iOffset > 0x7fffffff)
    {
        return -1;
    }

    if (fsetpos(pstFile->fp, &iPos) < 0)
    {
        return -1;
    }

    pstFile->i64Pos = iOffset;
    return 0;
#else
    INT64    i64Pos = iOffset;

    if (!hFile)
    {
        return -1;
    }

    return fsetpos(pstFile->fp, &i64Pos);
#endif
#else
    fpos64_t    i64Pos = {0};
    i64Pos.__pos = iOffset;

    if (!hFile)
    {
        return -1;
    }

    return fsetpos64(pstFile->fp, &i64Pos);
#endif
}

/*******************************************************************************
* ��������: gos_fskip
* ��    ��: �����ļ������ǰ���ƫ����
* ��    ��:
* ��������          ����/���       ����
* pstFile           IN              �ļ�����
* iSkip             IN              ƫ����
* ��������:
* ˵    ��:
*******************************************************************************/
INT32 gos_fskip(HANDLE hFile, INT64 iSkip)
{
    GOS_FILE_T  *pstFile = (GOS_FILE_T *)hFile;

#ifdef _OSWIN32_
#ifdef _BCB_
    INT32 iPos=0;
#else
    INT64 i64Pos=0;
#endif

    if (!hFile)
    {
        return -1;
    }

#ifdef _BCB_
    if (fgetpos(pstFile->fp, &iPos) != 0)
    {
        return -1;
    }

    iPos += iSkip;

    return fsetpos(pstFile->fp, &iPos);
#else
    if (fgetpos(pstFile->fp, &i64Pos) != 0)
    {
        return -1;
    }

    i64Pos += iSkip;

    return fsetpos(pstFile->fp, &i64Pos);
#endif
#else
    fpos64_t    i64Pos = {0};

    if (!hFile)
    {
        return -1;
    }

    if (fgetpos64(pstFile->fp, &i64Pos) != 0)
    {
        return -1;
    }

    i64Pos.__pos += iSkip;

    return fsetpos64(pstFile->fp, &i64Pos);
#endif
}

/*******************************************************************************
* ��������: gos_fskip
* ��    ��: �����ļ������ǰ���ƫ����
* ��    ��:
* ��������          ����/���       ����
* pstFile           IN              �ļ�����
* piPos             IN              ƫ����
* ��������:
* ˵    ��:
*******************************************************************************/
INT32 gos_fseek(HANDLE hFile, INT64 iOffset, INT32 iWhence)
{
    GOS_FILE_T  *pstFile = (GOS_FILE_T *)hFile;

    if (!hFile)
    {
        return -1;
    }

    if (iWhence == SEEK_SET)
    {
        return gos_fsetpos(hFile, iOffset);
    }
    else if (iWhence == SEEK_CUR)
    {
        return gos_fskip(hFile, iOffset);
    }
    else if (iWhence == SEEK_END)
    {
#ifdef _OSWIN32_
#ifdef _BCB_
        struct stati64  stStat;
        if (_stati64(pstFile->acFile, &stStat) != 0)
        {
            return -1;
        }

        return gos_fsetpos(hFile, stStat.st_size+iOffset);
#else
        if (iOffset <0x80000000)
        {
            return fseek(pstFile->fp, (long)iOffset, iWhence);
        }
        else
        {
            return _lseeki64(pstFile->fd, iOffset, iWhence);
        }
#endif
#else
        return  fseeko64(pstFile->fp, iOffset, iWhence);
#endif
    }

    return -1;
}

INT32 gos_fflush(HANDLE hFile)
{
    GOS_FILE_T  *pstFile = (GOS_FILE_T *)hFile;

    if (pstFile && pstFile->fp)
    {
        return fflush(pstFile->fp);
    }

    return -1;
}

INT32 gos_file_truncate(CHAR *szFile, INT64 iLen)
{
#ifdef _OSWIN32_
    UINT32  ulRet;
    HANDLE  hFile;
    LONG    lDistanceToMove;
    LONG    lDistanceToMoveHigh;

    hFile = CreateFile(szFile,                     // �����ļ������ơ�
                       GENERIC_WRITE|GENERIC_READ, // д�Ͷ��ļ���
                       0,                          // �������д��
                       NULL,                       // ȱʡ��ȫ���ԡ�
                       OPEN_EXISTING,              // ����ļ����ڣ�Ҳ������
                       FILE_ATTRIBUTE_NORMAL,      // һ����ļ���
                       NULL);                      // ģ���ļ�Ϊ�ա�
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return -1;
    }

    lDistanceToMove = (LONG)iLen;
    lDistanceToMoveHigh = (iLen>>32);
    ulRet = SetFilePointer(hFile, lDistanceToMove, &lDistanceToMoveHigh, FILE_BEGIN);
    if (ulRet == INVALID_SET_FILE_POINTER)
    {
        CloseHandle(hFile);
        return -1;
    }

    SetEndOfFile(hFile);

    CloseHandle(hFile);

    return 0;
#else

    off64_t  stLength = iLen;
    return truncate64(szFile, stLength);

#endif
}


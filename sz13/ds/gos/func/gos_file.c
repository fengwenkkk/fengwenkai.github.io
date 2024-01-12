#include "gos_api.h"
#include "gos_func.h"
#include "gos_file.h"
#include "gos_hashlist.h"

#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER  0xFFFFFFFF
#endif

extern CHAR* gos_get_text_time(UINT32 *pulCurrTime);

/************************************************************************/
/* 获取主键相对结构体头指针的偏移量                                     */
/************************************************************************/
typedef struct
{
    CHAR                    acFile[512];

    FILE                    *fp;
    INT32                   fd;

    BOOL                    bBufMode;       /* 是否采用缓存
                                               FALSE:不缓存，采用read/write
                                               TRUE :缓存，采用fread/fwrite */

    UINT8                   *pucBuf;

    BOOL                    bNeedFree;      /* pucBuf是否需要释放 */
#ifdef _BCB_
    INT32                   iPos;
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
* 函数名称: gos_fopen_ex
* 功    能: 初始化文件对象
* 参    数:
* 参数名称          输入/输出       描述
* szFileName        IN              文件名
* szOpenMode        IN              打开方式
* iBufSize          IN              写文件缓冲区大小(
                                    <0: 无缓冲
                                    =0: fwrite内部缓存机制
                                    >0: 重新设置缓存
* pucBuf            IN              外部文件缓冲区
* 函数返回:
* 说    明:
*******************************************************************************/
HANDLE gos_fopen_ex(const CHAR *szFileName, const CHAR *szOpenMode, INT32 iBufSize, UINT8 *pucBuf)
{
    GOS_FILE_T      *pstFile = NULL;
    UINT32          ulBufSize = iBufSize;      /* 文件缓冲区大小 */
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

    /* 不缓存 */
    if (iBufSize < 0)
    {
        pstFile->bBufMode = FALSE;
    }
    /* 采用fwrite的缓存机制 */
    else if (0 == iBufSize)
    {
        pstFile->bBufMode = TRUE;
    }
    /* 设置fwrite的缓存机制 */
    else
    {
        pstFile->bBufMode = TRUE;

        /* 指定buf时，采用外部buf */
        if (pucBuf)
        {
            pstFile->pucBuf = pucBuf;
        }
        else
        {
            /* 未指定buf时，动态创建 */
            ulBufSize = ((iBufSize+1023)/1024)*1024;
            pstFile->pucBuf = (UINT8*)GOS_MALLOC(ulBufSize);
            if (NULL == pstFile->pucBuf)
            {
                /* 退化为fwrite缓存模式 */
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

HANDLE gos_fopen(const CHAR *szFileName, const CHAR *szOpenMode)
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
        return (CHAR*)"";
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
* 函数名称: gos_fclose
* 功    能: 关闭文件对象
* 参    数:
* 参数名称          输入/输出       描述
* pstFile           IN              文件对象
* 函数返回:
* 说    明:
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
* 函数名称: gos_fwrite
* 功    能: 文件对象写操作
* 参    数:
* 参数名称          输入/输出       描述
* pstFile           IN              文件对象
* pData             IN              文件内容
* ulLen             IN              写数据大小
* 函数返回: TRUE/FALSE
* 说    明:
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
    pstFile->iPos += iWriteSize;
#endif

    return iWriteSize;
}

/*******************************************************************************
* 函数名称: gos_fread
* 功    能: 文件对象读操作
* 参    数:
* 参数名称          输入/输出       描述
* pstFile           IN              文件对象
* pData             IN              文件内容
* ulLen             IN              读取长度
* 函数返回: TRUE/FALSE
* 说    明:
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
        pstFile->iPos += iReadSize;
    }
#endif

    return iReadSize;
}

/*******************************************************************************
* 函数名称: gos_fgetpos
* 功    能: 获取文件句柄当前偏移量
* 参    数:
* 参数名称          输入/输出       描述
* pstFile           IN              文件对象
* piPos             OUT             偏移量
* 函数返回:
* 说    明:
*******************************************************************************/
INT32 gos_fgetpos(HANDLE hFile, INT32 *piPos)
{
    GOS_FILE_T  *pstFile = (GOS_FILE_T *)hFile;

    if (!hFile)
    {
        return -1;
    }

    *piPos = ftell(pstFile->fp);

    return 0;
}

/*******************************************************************************
* 函数名称: gos_fsetpos
* 功    能: 设置文件句柄当前偏移量
* 参    数:
* 参数名称          输入/输出       描述
* pstFile           IN              文件对象
* iOffset           IN              偏移量
* 函数返回:
* 说    明:
*******************************************************************************/
INT32 gos_fsetpos(HANDLE hFile, INT32 iOffset)
{
    GOS_FILE_T  *pstFile = (GOS_FILE_T *)hFile;

    if (!hFile)
    {
        return -1;
    }

    if (fseek(pstFile->fp, iOffset, SEEK_SET) < 0)
    {
        return -1;
    }

    return 0;
}

/*******************************************************************************
* 函数名称: gos_fskip
* 功    能: 设置文件句柄当前相对偏移量
* 参    数:
* 参数名称          输入/输出       描述
* pstFile           IN              文件对象
* iSkip             IN              偏移量
* 函数返回:
* 说    明:
*******************************************************************************/
INT32 gos_fskip(HANDLE hFile, INT32 iSkip)
{
    GOS_FILE_T  *pstFile = (GOS_FILE_T *)hFile;

    if (!hFile)
    {
        return -1;
    }

    if (fseek(pstFile->fp, iSkip, SEEK_CUR) < 0)
    {
        return -1;
    }

    return 0;
}

/*******************************************************************************
* 函数名称: gos_fskip
* 功    能: 设置文件句柄当前相对偏移量
* 参    数:
* 参数名称          输入/输出       描述
* pstFile           IN              文件对象
* piPos             IN              偏移量
* 函数返回:
* 说    明:
*******************************************************************************/
INT32 gos_fseek(HANDLE hFile, INT32 iOffset, INT32 iWhence)
{
    GOS_FILE_T  *pstFile = (GOS_FILE_T *)hFile;

    if (!hFile)
    {
        return -1;
    }

    if (iWhence == SEEK_SET ||
        iWhence == SEEK_CUR ||
        iWhence == SEEK_END )
    {
        if (fseek(pstFile->fp, iOffset, iWhence) < 0)
        {
            return -1;
        }

        return 0;
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

INT32 gos_file_truncate(const CHAR *szFile, INT32 iLen)
{
#ifdef _OSWIN32_
    UINT32  ulRet;
    HANDLE  hFile;
    LONG    lDistanceToMove;
    LONG    lDistanceToMoveHigh;

    hFile = CreateFile(szFile,                     // 创建文件的名称。
                       GENERIC_WRITE|GENERIC_READ, // 写和读文件。
                       0,                          // 不共享读写。
                       NULL,                       // 缺省安全属性。
                       OPEN_EXISTING,              // 如果文件存在，也创建。
                       FILE_ATTRIBUTE_NORMAL,      // 一般的文件。
                       NULL);                      // 模板文件为空。
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return -1;
    }

    lDistanceToMove = (LONG)iLen;
    lDistanceToMoveHigh = 0;//(iLen>>32);
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

    off_t  stLength = iLen;
    return truncate(szFile, stLength);
#endif
}

FILE* gos_popen(const CHAR *szCmd, const CHAR *szMode)
{
    if (!szCmd || !szMode)
    {
        return FALSE;
    }

#ifdef _OSWIN32_
    return _popen(szCmd, szMode);
#else
    return popen(szCmd, szMode);
#endif
}

VOID gos_pclose(FILE *fp)
{
    if (!fp)
    {
        return;
    }

#ifdef _OSWIN32_
    _pclose(fp);
#else
    pclose(fp);
#endif
}

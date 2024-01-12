#ifndef _GOS_FILE_H
#define _GOS_FILE_H

#ifdef __cplusplus
extern "C"{
#endif

BOOL    gos_file_init(VOID);
HANDLE  gos_fopen_ex (const CHAR *szFileName, const CHAR *szOpenMode, INT32 iBufSize, UINT8 *pucBuf);
HANDLE  gos_fopen    (const CHAR *szFileName, const CHAR *szOpenMode);
BOOL    gos_feof     (HANDLE hFile);
FILE*   gos_get_fp   (HANDLE hFile);
CHAR*   gos_get_fname(HANDLE hFile);
UINT32  gos_get_fsize(HANDLE hFile);
INT32   gos_fwrite   (HANDLE hFile, VOID* pData, UINT32 ulLen);
INT32   gos_fread    (HANDLE hFile, VOID *pData, UINT32 ulLen);
INT32   gos_fgetpos  (HANDLE hFile, INT32 *piPos);
INT32   gos_fsetpos  (HANDLE hFile, INT32 iPos);
INT32   gos_fskip    (HANDLE hFile, INT32 iSkip);
INT32   gos_fseek    (HANDLE hFile, INT32 iOffset, INT32 iWhence);
INT32   gos_ftell    (HANDLE hFile);
VOID    gos_fclose   (HANDLE hFile);
INT32   gos_fflush   (HANDLE hFile);

INT32   gos_file_truncate(const CHAR *szFile, INT32 iLen);

VOID    gos_print_opened_file(VOID);

FILE*   gos_popen(const CHAR *szCmd, const CHAR *szMode);
VOID    gos_pclose(FILE *fp);

#ifdef __cplusplus
}
#endif

#endif /* _GOS_FILE_H */
/******************************* 头文件结束 **********************************/

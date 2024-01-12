#ifndef _PDB_UTIL_H
#define _PDB_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

BOOL DCISemV(HANDLE ulSmID, UINT32 ulCount);
BOOL DCISemP(HANDLE ulSmID, UINT32 ulTimeout, UINT32 ulCount);
HANDLE DCICreateLock(VOID);
BOOL DCIDeleteLock(HANDLE hLockId);
BOOL DCILock(HANDLE hLockId, UINT32 ulTimeout);
BOOL DCIUnlock(HANDLE hLockId);
BOOL DCIStrNCmpCase(CHAR *szSrc, CHAR *szDst, UINT32 ulLen);
BOOL RemoveInsertSql(CHAR *szSql, UINT16 usParaNum, UINT16 usValuesPairNum);
BOOL RemoveDelSql(CHAR *szSql, UINT16 usValuesPairNum);
BOOL RemoveOneValuesSql(CHAR *szSql, UINT16 usParaNum);
BOOL RemoveOneCharSql(CHAR *szSql, CHAR cChar);

#ifdef __cplusplus
}
#endif

#endif

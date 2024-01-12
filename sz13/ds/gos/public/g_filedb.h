#ifndef G_FILEDB_H
#define G_FILEDB_H

#include "gos_filedb.h"

class GFileDB
{
public:
    GFileDB(UINT32 ulMaxBufRecNum, UINT32 ulMaxBufTime);
    ~GFileDB();

    BOOL Create(CHAR *szFileName, UINT32 ulVer, UINT32 ulRecLen, UINT32 ulMaxRecNum);
    BOOL Load(CHAR *szFileName, UINT32 ulVer, UINT32 ulRecLen, UINT32 ulMaxRecNum);

    BOOL Add(VOID *pucRec);
    VOID Clear();
    UINT32 GetNum();
    BOOL Get(UINT32 ulMaxRecNum, VOID *pucRec, UINT32 *pulRecNum);
    BOOL Get(UINT32 ulRecIndex, VOID *pucRec);

    BOOL Flush();

private:
    BOOL SetPos(INT64 i64Pos);
    BOOL Read(VOID *pucData, UINT32 ulLen);
    BOOL Write(VOID *pucData, UINT32 ulLen);
    BOOL RewriteFile();

    FILEDB_HDR_T    m_stFileHdr;
    HANDLE          m_hFile;
    UINT32          m_ulMaxBufRecNum;   // 最大缓存记录数
    UINT32          m_ulMaxBufTime;     // 最大缓存时间，单位:秒

    UINT32          m_ulLastSaveTime;
};

#endif

#ifndef GOS_FILEDB_H
#define GOS_FILEDB_H

#pragma pack(push, 4)

typedef struct
{
    UINT32          ulVer;
    UINT32          ulRecLen;
    UINT32          ulMaxRecNum;
    UINT32          ulRecNum;           // 文件中存储的记录数
    UINT32          ulStartRecIndex;    // 文件中起始记录位置序号
    UINT32          aulResv[3];
}FILEDB_HDR_T;

typedef struct
{
    HANDLE          m_hFile;
    UINT32          m_ulMaxBufRecNum;   // 最大缓存记录数
    UINT32          m_ulMaxBufTime;     // 最大缓存时间，单位:秒

    UINT32          m_ulLastSaveTime;

    FILEDB_HDR_T    m_stFileHdr;
}FILEDB_HANDLE_T;

#pragma pack(pop)

HANDLE gos_filedb_init(UINT32 ulMaxBufRecNum, UINT32 ulMaxBufTime);
VOID gos_filedb_free(HANDLE hFileDB);
BOOL gos_filedb_create(HANDLE hFileDB, CHAR *szFileName, UINT32 ulVer, UINT32 ulRecLen, UINT32 ulMaxRecNum);
BOOL gos_filedb_load(HANDLE hFileDB, CHAR *szFileName, UINT32 ulVer, UINT32 ulRecLen, UINT32 ulMaxRecNum);
BOOL gos_filedb_add(HANDLE hFileDB, VOID *pucRec, UINT32 ulRecLen);
VOID gos_filedb_clear(HANDLE hFileDB);
UINT32 gos_filedb_get_rec_num(HANDLE hFileDB);
BOOL gos_filedb_get_multi_rec(HANDLE hFileDB, UINT32 ulMaxRecNum, VOID *pvRec, UINT32 *pulRecNum);
BOOL gos_filedb_get_rec(HANDLE hFileDB, UINT32 ulRecIndex, VOID *pucRec);
BOOL gos_filedb_flush(HANDLE hFileDB);


#endif

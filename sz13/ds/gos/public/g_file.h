#ifndef G_FILE_H
#define G_FILE_H

class GFile
{
public:
    static GFile* Open(CHAR *szFile, const CHAR *szMode, BOOL bSafeMode=FALSE);
    static BOOL   Close(GFile *pstGFile);

    BOOL    Read(VOID *pvData, UINT32 ulLen);
    BOOL    Write(VOID *pvData, UINT32 ulLen);
    UINT32  Printf(const CHAR *szFormat, ...);

    VOID    Flush();
    FILE*   GetFile();
    BOOL    Eof();

    INT64   GetPos();
    BOOL    SetPos(INT64 i64Pos);

    BOOL    Skip(INT64 i64SkipLen);
    BOOL    Skip(INT64 i64Offset, INT32 iWhence);

private:
    GFile();
    ~GFile();

    BOOL Init(CHAR *szFile, const CHAR *szMode, BOOL bSafeMode);
    BOOL CopyFile(CHAR *pchBakFile, CHAR *pchSourFile);

    CHAR    m_acFile[512];
    CHAR    m_acBakFile[512];
    HANDLE  m_hFile;
    BOOL    m_bSafeMode;
};

#endif

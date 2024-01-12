#ifndef ThreadFormatH
#define ThreadFormatH

typedef struct
{
    bool    bConvertTab;
    bool    bTrimRight;
    bool    bUnixFile;
    bool    bAnsiFile;
}FILE_FORMAT_CFG_T;

class ThreadFormat :public GThread
{
public:
    ThreadFormat();
    void Stop();

    GOS_THREAD_RET ThreadEntry(void* pPara);
    void SetCfg(FILE_FORMAT_CFG_T &stCfg);

private:

    BOOL        m_bRunning;

    FILE_FORMAT_CFG_T   m_stFormatCfg;

    void FormatFile(char *szFile);
};

void ClearFormatFile();
void InitFormatFile(VECTOR<AnsiString> &vFile);
void GetFormatInfo(UINT32 &ulTotalNum, UINT32 &ulCurrNum);

BOOL GetFileList(CHAR *szRootDir, VECTOR<AnsiString> &vFilePostfix, VECTOR<AnsiString> &vFile, BOOL bClear);
BOOL GetFileList(CHAR *szRootDir, VECTOR<AnsiString> &vSubDir, VECTOR<AnsiString> &vFilePostfix, VECTOR<AnsiString> &vFile);

#endif

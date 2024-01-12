#ifndef ThreadDownloadH
#define ThreadDownloadH

#define EV_DOWNLOAD_MSG              0x2999

typedef struct
{
    UINT32      ulRecType;
    CHAR        acFile[256];
    CHAR        acLocalFile[256];
    CHAR        acDCAddr[32];
    CHAR        acResult[64];
}DOWNLOAD_FILE_INFO_T;


class ThreadDownload :public GThread
{
public:
    ThreadDownload(DOWNLOAD_FILE_INFO_T &stInfo);

    GOS_THREAD_RET ThreadEntry(VOID* pPara);

private:
    DOWNLOAD_FILE_INFO_T    m_stInfo;
//  BOOL                    m_bRunning;
};


#endif

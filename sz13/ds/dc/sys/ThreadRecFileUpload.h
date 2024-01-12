//---------------------------------------------------------------------------

#ifndef ThreadRecFileUploadH
#define ThreadRecFileUploadH

#include "g_include.h"
//#include "DISPublic.h"

typedef struct
{
    CHAR    acFile[256];  // 带路径的文件名
    UINT8   aucDCAddr[4]; // 调度台IP地址
    UINT32  ulFileSize;   // 文件大小
    UINT32  ulRecType;    // 文件类型
    UINT32  ulCallType;   // 呼叫类型
}LOCAL_TEMP_REC_INFO_T;

class ThreadRecFileUp: public GThread
{
public:
    ThreadRecFileUp();

    VOID RecFileUpload(BOOL bIsIPH);
    BOOL RecFileMove(CHAR *szOldFilePath, BOOL bIsIPH);
    BOOL GetIPHRecFileList(CHAR *szDir);
    BOOL GetRealBrdFileList(CHAR *szDir);
    virtual GOS_THREAD_RET ThreadEntry(void* pPara);

private:

    CHAR    m_acTempRecIPHFileDir[256];
    CHAR    m_acTempRecRealBrdFileDir[256];

    VECTOR<LOCAL_TEMP_REC_INFO_T>        m_vRecInfo;
};
//---------------------------------------------------------------------------
#endif

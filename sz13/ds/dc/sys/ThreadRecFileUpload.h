//---------------------------------------------------------------------------

#ifndef ThreadRecFileUploadH
#define ThreadRecFileUploadH

#include "g_include.h"
//#include "DISPublic.h"

typedef struct
{
    CHAR    acFile[256];  // ��·�����ļ���
    UINT8   aucDCAddr[4]; // ����̨IP��ַ
    UINT32  ulFileSize;   // �ļ���С
    UINT32  ulRecType;    // �ļ�����
    UINT32  ulCallType;   // ��������
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

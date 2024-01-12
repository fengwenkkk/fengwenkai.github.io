#ifndef REC_FILE_MANAGER
#define REC_FILE_MANAGER

#include "g_include.h"
#include "RecPublic.h"

class RecFileManager// : public GThread
{
public:

    static RecFileManager &GetInstance()
    {
        static RecFileManager *pInstance = NULL; //静态变量保证线程安全
        if (NULL == pInstance)
        {
            pInstance = new RecFileManager();
        }

        return *pInstance;
    }

    BOOL    Init(CHAR *szDirOri, CHAR *szDirTemp, UINT32 ulTempSaveDays, UINT32 ulOriSaveDays, UINT32 ulRecSaveDays);
    BOOL    GetOriRecFileList(CHAR *szDir);
    BOOL    GetTempRecFileList(CHAR *szDir);

    VOID    Start();
    VOID    CheckOriFile();
    VOID    CheckRecFile();
    VOID    CheckTempFile();

    //virtual GOS_THREAD_RET ThreadEntry(void* pPara);
private:
    RecFileManager();
    ~RecFileManager();

    /// 该函数不实现，防止拷贝操作
    RecFileManager(const RecFileManager &other);
    /// 该函数不实现，防止拷贝操作
    RecFileManager &operator=(const RecFileManager &other);

    CfgDao          *m_pDao;
    CHAR            m_acDirOri[512];
    CHAR            m_acDirTemp[512];

    UINT32          m_ulRecSaveTime;
    UINT32          m_ulOriSaveTime;
    UINT32          m_ulTempSaveTime;

    VECTOR<std::string>     m_vRecOriList;
    VECTOR<std::string>     m_vRecTempList;

    //BOOL     m_bRunning;

};

#endif
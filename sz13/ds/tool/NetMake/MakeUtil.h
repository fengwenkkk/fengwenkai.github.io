#ifndef MakeUtilH
#define MakeUtilH

#include "msg.h"

#define MAKE_STATE_IDLE         0
//#define MAKE_STATE_PRE_ALLOCED  1       // 预订
#define MAKE_STATE_ALLOCED      2       // 已分配
#define MAKE_STATE_SUCC         10
#define MAKE_STATE_FAIL         11
#define MAKE_STATE_NOFILE       12      // 文件不存在
#define MAKE_STATE_COMPILED     13      // obj文件时间大于源文件时间（不需要编译）
#define MAKE_STATE_FILTERED     21      // 过滤文件，不需要编译


typedef struct
{
    AnsiString      strBccExe;
    AnsiString      strRootDir;
    AnsiString      strDefine;
    AnsiString      strSysIncPath;
    AnsiString      strSysLibPath;

    AnsiString      strTmsPath;
    AnsiString      strDevPath;
    AnsiString      strRaizePath;
    AnsiString      strTeeIncPath;
    AnsiString      strTeeObjPath;
    AnsiString      strTeeLibPath;
}BDS_SYS_INFO_T;

// 文件编译结果、分配等状态
typedef struct
{
    UINT32          ulMakeOrder;
    AnsiString      strFile;
    AnsiString      strObjFile;
    UINT32          ulState;
    AnsiString      strResult;

    IP_ADDR_T       stPeer;
    UINT32          ulAllocTime;
    UINT32          ulAllocNum;     // 已经分配的次数
}FILE_MAKE_INFO_T;

// 正在编译的文件信息
typedef struct
{
    IP_ADDR_T   stPeerAddr;
    CHAR        acMakeName[128];
    CHAR        acProject[128];
    CHAR        acSrcPath[256];
    CHAR        acFile[256];
    CHAR        acIncPath[4096];
    CHAR        acMakeOption[256];
    CHAR        acObjFile[256];
}MAKING_FILE_INFO_T;

typedef struct
{
    UINT8   aucServerAddr[4];
    UINT16  usServerPort;
    CHAR    acRemoteFile[256];
    CHAR    acLocalFile[256];
}OBJ_FILE_INFO_T;


#define WM_BUILD_INNER_MSG        (WM_USER+0x20)

#define MSG_BUILD_INFO      1

typedef struct
{
    CHAR        acSourceFile[256];
    CHAR        acBuilderInfo[60*1024];
}BUILD_INFO_T;

#pragma pack(push, 1)

#define EV_MSG_START                        EV_USER_START

#define EV_GET_MAKE_TASK_REQ                (EV_MSG_START+1)
#define EV_STOP_MAKE_TASK_REQ               (EV_MSG_START+2)

#define EV_ALLOC_MAKE_FILE_REQ              (EV_MSG_START+5)    // 分配
#define EV_REPORT_MAKE_INFO_REQ             (EV_MSG_START+6)

#define EV_GET_MAKE_TASK_RSP                EV_RSP(EV_GET_MAKE_TASK_REQ)
#define EV_STOP_MAKE_TASK_RSP               EV_RSP(EV_STOP_MAKE_TASK_REQ)

#define EV_ALLOC_MAKE_FILE_RSP              EV_RSP(EV_ALLOC_MAKE_FILE_REQ)
#define EV_REPORT_MAKE_INFO_RSP             EV_RSP(EV_REPORT_MAKE_INFO_REQ)

typedef struct
{
    UINT16      usMsgID;
    UINT16      usMsgLen;
}MAKE_MSG_HDR_T;

typedef struct
{
    CHAR        acMakeName[128];    // 名称包含：时间_地址_端口_项目
    CHAR        acZipFile[128];     //
    UINT32      ulFileLen;
    UINT32      ulFileTime;

    CHAR        acMakeOption[128];
    CHAR        acSrcPath[128];     // 工程相对项目根目录路径
    CHAR        acIncPath[4096];
}GET_MAKE_TASK_RSP_T;

// server -> client
typedef struct
{
    CHAR        acMakeName[128];
}STOP_MAKE_TASK_REQ_T;

// client -> server
typedef struct
{
    CHAR        acMakeName[128];
    UINT32      ulIndex;
}ALLOC_MAKE_FILE_REQ_T;

// server -> client
typedef struct
{
    CHAR        acMakeName[128];
    UINT32      ulIndex;
    CHAR        acSrcPath[128];
    CHAR        acFile[256];
}ALLOC_MAKE_FILE_RSP_T;

// client -> server
typedef struct
{
    CHAR        acMakeName[128];
    CHAR        acFile[256];
    CHAR        acMakeResult[1024];
    CHAR        acObjFile[256];         // 编译成功则包含obj文件名，失败为空
}REPORT_MAKE_INFO_REQ_T;

typedef struct
{
    CHAR        acMakeName[128];
    CHAR        acFile[256];
}REPORT_MAKE_INFO_RSP_T;

#pragma pack(pop)



typedef struct
{
    AnsiString  strSrcPath;
    AnsiString  strObjPath;
    AnsiString  strSrcName;     // 无cpp后缀
}MAKE_REQ_T;

typedef struct
{
    AnsiString  strBccExe;     // bcc32.exe
    AnsiString  strBccIncPath;
    AnsiString  strBccHppPath;

    AnsiString  strTeePath;
    AnsiString  strDevExpPath;
    AnsiString  strTmsPath;

    AnsiString  strMakeOption;
}MAKE_ENV_T;

void SendBuildCmd();

extern MAKE_ENV_T       g_stMakeEnv;

extern UINT16           g_usServerUDPPort;
extern UINT16           g_usClientUDPPort;
extern UINT32           g_usServerTransFilePort;
extern UINT32           g_usClientTransFilePort;

bool LoadCfg();

void WaitCommand(CHAR *szCmd, BOOL bShow);
BOOL RunBatCmd(UINT32 ulIndex, CHAR *szCmd);

AnsiString GetMakeStateText(UINT32 ulState);

namespace FileMakeInfo
{
    void InitFilterFile(char *szFilterFile);
    bool IsFilteredFile(char *szFilterFile);

    BOOL IsAllMakeDone();
    UINT32 GetTotalMakeCount();
    UINT32 GetUnMakeCount();

    VOID Check();
    VOID CheckObj(VECTOR<FILE_MAKE_INFO_T*> &vLocalMake);
    BOOL HasIdle();
    VOID Init(VECTOR<FILE_MAKE_INFO_T> &vInfo);
    FILE_MAKE_INFO_T* Get(AnsiString strFile);
    bool SetResult(AnsiString strFile, IP_ADDR_T *pstPeer, AnsiString strResult, bool bSucc);
    bool GetResult(AnsiString strFile, IP_ADDR_T &stPeer, AnsiString &strResult, UINT32 &ulState);
    FILE_MAKE_INFO_T* GetIdle(IP_ADDR_T *pstPeer);
    void Rebuild();
    void ClearObjZipFile();
};

void GetProjectParam(char *szProjFile, AnsiString &strIncPath, AnsiString &strObjPath, AnsiString &strDefine);
AnsiString GetMakeCmd(MAKING_FILE_INFO_T &stFileInfo);

void SaveProject(AnsiString strProject);
bool DownloadMakeDir(UINT8 *pucServerAddr, AnsiString strMakeName, char *szZipFile, UINT32 ulFileLen);
bool ZipMakeFile(char *szDir, VECTOR<std::string> &vMakePath, char *szZipFile);
bool InitMakePath(char *szRootPath, char *szProjectPath, char *szMakePath, VECTOR<std::string> &vMakePath);

void ClearOldTask(CHAR *szResvDir);

void InitDownloadThread();

namespace ObjFile
{
    void Clear();
    bool Add(UINT8 *pucServerAddr, char *szRemoteFile, char *szLocalFile);
    bool Get(OBJ_FILE_INFO_T &stInfo);
}

#endif

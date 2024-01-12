#ifndef ClientInitH
#define ClientInitH

#define DEFAULT_MDC_PORT    5060
#define DEFAULT_DIS_PORT    GOS_DEFAULT_SERVER_PORT

extern RemoteMsg        *g_pRemoteMsg;
extern RemoteMsg        *g_pMRSRemoteMsg;

BOOL InitClientEnv();
BOOL LoadStationInfo(CHAR *szMsgInfo=NULL);
BOOL LoadTrainInfo(CHAR *szMsgInfo=NULL);
BOOL LoadSectionInfo(CHAR *szMsgInfo=NULL);

BOOL ParseTrainInfo(GJson *pSubJson, TRAIN_INFO_T &stInfo);
BOOL LoadSectionInfo(CHAR *szMsgInfo);

#endif

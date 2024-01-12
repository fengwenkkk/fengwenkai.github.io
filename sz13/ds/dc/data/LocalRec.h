#ifndef LocalRecH
#define LocalRecH

namespace LocalRec
{

void GetIPHRecTmpDir(CHAR *szDir);
void GetIPHRecDir(CHAR *szDir);
void GetPARecTmpDir(CHAR *szDir);
void GetPARecDir(CHAR *szDir);

void StartP2PCallRecord(UINT32 ulUserID, UINT32 ulRecType, UINT32 ulRecCallType);
void StopP2PCallRecord(UINT32 ulUserID);
void StartGroupCallRecord(INT64 i64GroupID, UINT32 ulRecType);
void StopGroupCallRecord(INT64 i64GroupID);
void UpdateGroupCallRecord(INT64 i64GroupID);

}  // namespace LocalRec

#endif

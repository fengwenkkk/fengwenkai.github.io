#include "ClientInclude.h"

namespace LocalRec
{

typedef struct
{
    UINT32  ulRecType;
    UINT32  ulRecCallType;
    UINT32  ulStartTime;
    UINT32  ulLastTime;
}REC_INFO_T;

static RawIntMap<REC_INFO_T>        g_mP2PCallRecInfo;
static RawInt64Map<REC_INFO_T>      g_mGroupCallRecInfo;

void GetIPHRecTmpDir(CHAR *szDir)
{
    sprintf(szDir, "%s\\%s", gos_get_root_path(), "data\\iph_tmp");
}

void GetIPHRecDir(CHAR *szDir)
{
    sprintf(szDir, "%s\\%s", gos_get_root_path(), "data\\iph_rec");
}

void GetPARecTmpDir(CHAR *szDir)
{
    sprintf(szDir, "%s\\%s", gos_get_root_path(), "data\\pa_tmp");
}

void GetPARecDir(CHAR *szDir)
{
    sprintf(szDir, "%s\\%s", gos_get_root_path(), "data\\pa_rec");
}

void StartP2PCallRecord(UINT32 ulUserID, UINT32 ulRecType, UINT32 ulRecCallType)
{
    if (g_mP2PCallRecInfo.Has(ulUserID))
    {
        return;
    }

    REC_INFO_T  stRecInfo = {0};

    stRecInfo.ulRecType = ulRecType;
    stRecInfo.ulRecCallType = ulRecCallType;
    stRecInfo.ulStartTime = gos_get_uptime();

    if (!eSDK_StartRecord(ulUserID, ulRecType, ulRecCallType))
    {
        return;
    }

    g_mP2PCallRecInfo.Add(ulUserID, &stRecInfo);
}

void StopP2PCallRecord(UINT32 ulUserID)
{
    REC_INFO_T  *pstRecInfo = g_mP2PCallRecInfo.GetValue(ulUserID);

    if (!pstRecInfo)
    {
        return;
    }

    if (!eSDK_StopRecord(ulUserID, pstRecInfo->ulRecType, pstRecInfo->ulRecCallType))
    {
        return;
    }

    g_mP2PCallRecInfo.Del(ulUserID);
}

void StartGroupCallRecord(INT64 i64GroupID, UINT32 ulRecType)
{
    REC_INFO_T  *pstRecInfo = g_mGroupCallRecInfo.GetValue(i64GroupID);

    if (pstRecInfo)
    {
        pstRecInfo->ulLastTime = gos_get_uptime();

        return;
    }

    REC_INFO_T  stRecInfo;

    stRecInfo.ulRecType = ulRecType;
    stRecInfo.ulRecCallType = REC_CALL_TYPE_PTT;
    stRecInfo.ulStartTime = gos_get_uptime();
    stRecInfo.ulLastTime = stRecInfo.ulStartTime;

    if (!eSDK_StartRecord(i64GroupID, ulRecType, stRecInfo.ulRecCallType))
    {
        return;
    }

    g_mGroupCallRecInfo.Add(i64GroupID, &stRecInfo);
}

// 检查并判断是否需要另存为新的
void UpdateGroupCallRecord(INT64 i64GroupID)
{
    REC_INFO_T  *pstRecInfo = g_mGroupCallRecInfo.GetValue(i64GroupID);

    if (!pstRecInfo)
    {
        return;
    }

    // 单次组呼录音最大时长
    UINT32  ulTime = gos_get_uptime();
    UINT32  ulRecType = pstRecInfo->ulRecType;

    if ((ulTime - pstRecInfo->ulStartTime) > GetMaxGroupCallRecordTime() ||
        (ulTime - pstRecInfo->ulLastTime)  > GetMaxGroupCallRecordIdleTime() )
    {
        if (!eSDK_StopRecord(i64GroupID, ulRecType, pstRecInfo->ulRecCallType))
        {
            return;
        }

        g_mGroupCallRecInfo.Del(i64GroupID);

        StartGroupCallRecord(i64GroupID, ulRecType);
    }
}

void StopGroupCallRecord(INT64 i64GroupID)
{
    REC_INFO_T  *pstRecInfo = g_mGroupCallRecInfo.GetValue(i64GroupID);

    if (!pstRecInfo)
    {
        return;
    }

    if (!eSDK_StopRecord(i64GroupID, pstRecInfo->ulRecType, pstRecInfo->ulRecCallType))
    {
        return;
    }

    g_mGroupCallRecInfo.Del(i64GroupID);
}

}  // namespace LocalRec

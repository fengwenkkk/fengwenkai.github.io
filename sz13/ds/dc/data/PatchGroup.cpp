#include "ClientInclude.h"

typedef struct
{
    INT64       i64PatchGroupID;
    CHAR        acGroupName[64];
    UINT32      ulAllocTime;
}PATCH_GROUP_STATUS_T;

static VectorMap<INT64, PATCH_GROUP_STATUS_T>       g_mTmpPatchGroup;    // ��ʱ�ɽ���
static VectorMap<INT64, SDK_PATCH_GROUP_INFO_T>     g_mAllPatchGroup;

static UINT32   g_ulMaxPatchGroupTalkTime = 300; // �ɽ������ͨ��ʱ��
static UINT32   g_ulMinTmpPatchGroupNum = 1;
static UINT32   g_ulMaxTmpPatchGroupNum = 8;
static BOOL     g_bPatchGroupLoaded = FALSE;  // �ɽ������״̬��FALSE��ʾδ���ػ���Ҫ����

static CHAR     g_acTmpPatchGroupNamePrefix[] = "_��ʱ�ɽ���";

namespace PatchGroup
{
VOID SetMinTmpPatchGroupNum(UINT32 ulNum)
{
    g_ulMinTmpPatchGroupNum = MIN(4, ulNum);
}

BOOL IsPatchGroupLoaded()
{
    return g_bPatchGroupLoaded;
}

BOOL IsTmpPatchGroup(CHAR *szGroupName)
{
    return gos_str_like(szGroupName, g_acTmpPatchGroupNamePrefix);
}

static BOOL CreateTmpPatchGroup()
{
    if (g_mTmpPatchGroup.Size() >= g_ulMaxTmpPatchGroupNum)
    {
        return FALSE;
    }

    CHAR    acGroupName[256];

    sprintf(acGroupName, "%s_%u", g_acTmpPatchGroupNamePrefix, APP::GetCurrDCUserID());

    BOOL   bRet = eSDK_CreatePatchGroup(acGroupName);

    if (!bRet)
    {
        GosLog(LOG_ERROR, "create patch group failed");
    }
    else
    {
        GosLog(LOG_INFO, "create patch group succ, current patch group num is %u", g_mTmpPatchGroup.Size());
    }

    return bRet;
}

BOOL CancelTmpPatchGroup(INT64 i64PatchGroupID)
{
    PATCH_GROUP_STATUS_T    *pstStatus = g_mTmpPatchGroup.GetValue(i64PatchGroupID);

    if (!pstStatus)
    {
        return FALSE;
    }

    if (g_mTmpPatchGroup.Size() > g_ulMinTmpPatchGroupNum)
    {
        if (!eSDK_CancelPatchGroup(i64PatchGroupID))
        {
            GosLog(LOG_ERROR, "cancel tmp patch group " FORMAT_I64 " failed", i64PatchGroupID);
            return FALSE;
        }

        GosLog(LOG_INFO, "cancel tmp patch group "FORMAT_I64, i64PatchGroupID);

        Group::DelGroup(i64PatchGroupID);
        g_mTmpPatchGroup.Del(i64PatchGroupID);
    }
    else
    {
        pstStatus->ulAllocTime = 0;
        eSDK_ClearPatchGroupMember(i64PatchGroupID, pstStatus->acGroupName);
        GosLog(LOG_INFO, "clear member of tmp patch group "FORMAT_I64 ", current patch group num is %u",
                i64PatchGroupID, g_mTmpPatchGroup.Size());
    }


    g_bPatchGroupLoaded = FALSE;

    return TRUE;
}

BOOL CancelPatchGroup(INT64 i64PatchGroupID)
{
    // ��ʱ�ɽ���
    if (g_mTmpPatchGroup.GetValue(i64PatchGroupID))
    {
        return CancelTmpPatchGroup(i64PatchGroupID);
    }

    // ��ͨ�ɽ���
    if (!eSDK_CancelPatchGroup(i64PatchGroupID))
    {
        GosLog(LOG_ERROR, "cancel patch group " FORMAT_I64 " failed", i64PatchGroupID);
        return FALSE;
    }

    GosLog(LOG_INFO, "cancel patch group "FORMAT_I64, i64PatchGroupID);

    Group::DelGroup(i64PatchGroupID);
    g_bPatchGroupLoaded = FALSE;

    return TRUE;
}

// ���Ѿ����ڵ��ɽ����л�ȡδ����ġ��Ѿ����䵫�Խ�ʱ���Ѿ���ʱ���ɽ���
static INT64 GetFreeTmpPatchGroup(CHAR *szGroupName)
{
    UINT32                  ulTime = gos_get_uptime();
    PATCH_GROUP_STATUS_T    *pstStatus = NULL;
    PATCH_GROUP_STATUS_T    *pstFreeStatus = NULL;

    for (UINT32 i=0; i<g_mTmpPatchGroup.Size(); i++)
    {
        pstStatus = g_mTmpPatchGroup[i];
        if (pstStatus->ulAllocTime == 0 ||
            (pstStatus->ulAllocTime+g_ulMaxPatchGroupTalkTime) < ulTime )
        {
            pstFreeStatus = pstStatus;
            break;
        }
    }

    if (!pstFreeStatus)
    {
        GosLog(LOG_ERROR, "alloc patch group failed, no free group");
        return INVALID_GROUP_ID;
    }

    pstFreeStatus->ulAllocTime = ulTime;
    if (szGroupName)
    {
        strcpy(szGroupName, pstFreeStatus->acGroupName);
    }

    GosLog(LOG_INFO, "patch group "FORMAT_I64 " is alloced", pstFreeStatus->i64PatchGroupID);

    return pstFreeStatus->i64PatchGroupID;
}

// ����һ���ɽ���
INT64 AllocTmpPatchGroup(VECTOR<INT64> &vGroupID)
{
    UINT32                  ulTime = gos_get_uptime();
    PATCH_GROUP_STATUS_T    *pstStatus = NULL;
    PATCH_GROUP_STATUS_T    *pstFreeStatus = NULL;

    for (UINT32 i=0; i<g_mTmpPatchGroup.Size(); i++)
    {
        pstStatus = g_mTmpPatchGroup[i];
        if (pstStatus->ulAllocTime == 0 ||
            (pstStatus->ulAllocTime+g_ulMaxPatchGroupTalkTime) < ulTime )
        {
            pstFreeStatus = pstStatus;
            break;
        }
    }

    if (!pstFreeStatus)
    {
        GosLog(LOG_ERROR, "alloc tmp patch group failed, no free group");
        return INVALID_GROUP_ID;
    }

    pstFreeStatus->ulAllocTime = ulTime;

    GosLog(LOG_INFO, "tmp patch group "FORMAT_I64 " is alloced", pstFreeStatus->i64PatchGroupID);

    /*
    if (i64PatchGroupID == INVALID_GROUP_ID)
    {
        return INVALID_GROUP_ID;
    } */

    if (!eSDK_AddPatchGroupMember(pstFreeStatus->i64PatchGroupID, pstFreeStatus->acGroupName, vGroupID))
    {
        GosLog(LOG_INFO, "add patch group member of "FORMAT_I64 " failed", pstFreeStatus->i64PatchGroupID);
        CancelPatchGroup(pstFreeStatus->i64PatchGroupID);
        return INVALID_GROUP_ID;
    }

    return pstFreeStatus->i64PatchGroupID;
}

// �Զ�������ɾ����ʱ�ɽ���
VOID CheckPatchGroup()
{
    // �Զ�������ʱ�ɽ���
    if (g_mTmpPatchGroup.Size() < g_ulMinTmpPatchGroupNum)
    {
        if (CreateTmpPatchGroup())
        {
            g_bPatchGroupLoaded = FALSE;
            LoadPatchGroup();
        }

        return;
    }

    if (g_mTmpPatchGroup.Size() <= g_ulMinTmpPatchGroupNum)
    {
        return;
    }

    UINT32                  ulTime = gos_get_uptime();
    PATCH_GROUP_STATUS_T    *pstStatus = NULL;
    VECTOR<INT64>           vGroupID;

    g_mTmpPatchGroup.GetKeyList(vGroupID);

    for (UINT32 i=0; i<vGroupID.size(); i++)
    {
        pstStatus = g_mTmpPatchGroup.GetValue(vGroupID[i]);
        if (!pstStatus)
        {
            continue;
        }

        if (pstStatus->ulAllocTime == 0 ||
            (pstStatus->ulAllocTime+g_ulMaxPatchGroupTalkTime) < ulTime)
        {
            INT64   i64PatchGroupID = pstStatus->i64PatchGroupID;

            if (eSDK_CancelPatchGroup(i64PatchGroupID))
            {
                g_mTmpPatchGroup.Del(i64PatchGroupID);
                Group::DelGroup(i64PatchGroupID);
                g_bPatchGroupLoaded = FALSE; // ֻҪɾ��������Ҫ���¼���

                GosLog(LOG_INFO, "auto delete tmp patch group "FORMAT_I64, i64PatchGroupID);

                // ÿ��ֻɾ��һ����ʱ�ɽ���
                break;
            }
        }
    }
}

// ��eMDC�����ɽ��飬������Group�� g_mTmpPatchGroup
BOOL LoadPatchGroup(BOOL bForce)
{
    VectorMap<INT64, SDK_PATCH_GROUP_INFO_T>    mPatchGroup;
    VECTOR<INT64>   vGroupID;
    INT64           i64GroupID;

    // ����Ƿ�ǿ��ģʽ�������Ѿ����ع�������Ҫ����
    if (!bForce && g_bPatchGroupLoaded)
    {
        return TRUE;
    }

    if (!IsMDCValid())
    {
        return FALSE;
    }

    // �������ʧ�ܣ��Զ����ü���״̬Ϊδ����
    if (!eSDK_GetPatchGroups(mPatchGroup))
    {
        GosLog(LOG_ERROR, "load patch group failed");
        g_bPatchGroupLoaded = FALSE;
        return FALSE;
    }

    VECTOR<SDK_PATCH_GROUP_INFO_T*>   &vAllPatchGroup = mPatchGroup.GetValueList();

    g_mAllPatchGroup.Clear();
    g_mAllPatchGroup.AddAll(mPatchGroup);

    Group::GetGroup(PATCH_GROUP, vGroupID);

    GosLog(LOG_INFO, "load patch group ...");

    // ��group��Ϣ��ɾ�������ڵ�PatchGroup
    for (UINT32 i=0; i<vGroupID.size(); i++)
    {
        i64GroupID = vGroupID[i];

        GosLog(LOG_INFO, "current patch group "FORMAT_I64, i64GroupID);

        if (!mPatchGroup.HasKey(i64GroupID))
        {
            GosLog(LOG_INFO, "del current patch group "FORMAT_I64, i64GroupID);
            Group::DelGroup(i64GroupID);
        }
    }

    // ɾ��g_mPatchGroupStatus���Ѿ������ڵ��ɽ���
    VECTOR<INT64>   vPatchGroup;

    g_mTmpPatchGroup.GetKeyList(vPatchGroup);

    for (UINT32 i=0; i<vPatchGroup.size(); i++)
    {
        if (!mPatchGroup.HasKey(vPatchGroup[i]))
        {
            g_mTmpPatchGroup.Del(vPatchGroup[i]);

            GosLog(LOG_INFO, "remove tmp patch group "FORMAT_I64, vPatchGroup[i]);
        }
    }

    // ���group��g_mPatchGroupStatus�в����ڵ��ɽ���
    for (UINT32 i=0; i<mPatchGroup.Size(); i++)
    {
        GROUP_INFO_T        stGroup = {0};
        SDK_PATCH_GROUP_INFO_T  *pstPatchGroup = mPatchGroup[i];

        if (pstPatchGroup->ulDCUserID == APP::GetCurrDCUserID())
        {
            stGroup.i64GroupID = pstPatchGroup->i64GroupID;
            strncpy(stGroup.acGroupName, pstPatchGroup->acGroupName, sizeof(stGroup.acGroupName)-1);
            stGroup.ulGroupType = PATCH_GROUP;
            stGroup.ulPriority = pstPatchGroup->ulPriority;
            stGroup.ulDCUserID = pstPatchGroup->ulDCUserID;

            Group::AddGroup(&stGroup);

            if (!g_mTmpPatchGroup.HasKey(pstPatchGroup->i64GroupID))
            {
                // �������ʱ�ɽ���
                if (IsTmpPatchGroup(pstPatchGroup->acGroupName))
                {
                    PATCH_GROUP_STATUS_T    stStatus = {0};

                    stStatus.i64PatchGroupID = pstPatchGroup->i64GroupID;
                    strncpy(stStatus.acGroupName, pstPatchGroup->acGroupName, sizeof(stGroup.acGroupName)-1);
                    stStatus.ulAllocTime = 0;

                    g_mTmpPatchGroup.Add(pstPatchGroup->i64GroupID, stStatus);

                    GosLog(LOG_INFO, "add tmp patch group "FORMAT_I64, pstPatchGroup->i64GroupID);
                }
            }
        }
        else    // ����ɽ���Ǳ�����̨��������ɾ��֮
        {
            Group::DelGroup(pstPatchGroup->i64GroupID);

            //GosLog(LOG_DETAIL, "remove other dc's patch group "FORMAT_I64, pstPatchGroup->i64GroupID);
        }
    }

    GosLog(LOG_INFO, "load patch group end, current patch group num is %u", g_mTmpPatchGroup.Size());

    g_bPatchGroupLoaded = TRUE;

//    CheckPatchGroup();

    return TRUE;
}

VOID LoadAllPatchGroupMember()
{
    VECTOR<INT64>           vMember;
    SDK_PATCH_GROUP_INFO_T  *pstInfo;

    for (UINT32 i=0; i<g_mAllPatchGroup.Size(); i++)
    {
        SDK_PATCH_GROUP_INFO_T  *pstInfo = g_mAllPatchGroup[i];

        if (!eSDK_GetPatchGroupMembers(pstInfo->i64GroupID, vMember))
        {
            GosLog(LOG_ERROR, "load patch member of "FORMAT_I64 " failed", pstInfo->i64GroupID);
            continue;
        }

        pstInfo->ulMemberNum = MIN(vMember.size(), ARRAY_SIZE(pstInfo->ai64MemberGroupID));

        for (UINT32 j=0; j<pstInfo->ulMemberNum; j++)
        {
            pstInfo->ai64MemberGroupID[j] = vMember[j];
        }
    }
}

// �ж�Ⱥ���Ƿ��Ѿ�����ĳ�ɽ���
BOOL GetBelongingPatchGroupInfo(INT64 i64GroupID, INT64 &i64PatchGroupID, UINT32 &ulDCuserID)
{
    SDK_PATCH_GROUP_INFO_T    *pstInfo = NULL;

    for (UINT32 i=0; i<g_mAllPatchGroup.Size(); i++)
    {
        pstInfo = g_mAllPatchGroup[i];
        for (UINT32 j=0; j<pstInfo->ulMemberNum; j++)
        {
            if (pstInfo->ai64MemberGroupID[j] == i64GroupID)
            {
                i64PatchGroupID = pstInfo->i64GroupID;
                ulDCuserID = pstInfo->ulDCUserID;
                return TRUE;
            }
        }
    }

    return FALSE;
}

} // namespace PatchGroup

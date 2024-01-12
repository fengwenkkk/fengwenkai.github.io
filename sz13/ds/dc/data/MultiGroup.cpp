#include "ClientInclude.h"

namespace MultiGroup
{
BOOL                             g_bMultiGroupLoaded = FALSE;
VECTOR<SDK_MULTI_GROUP_INFO_T>   g_vMultiGroupInfo;      // 本调度台从SDK获取的多选组信息（GroupName是空的）
VECTOR<SDK_MULTI_GROUP_INFO_T>   g_vOtherMultiGroupInfo; // DIS存储的其它多选组信息

BOOL IsMultiGroupLoaded()
{
    return g_bMultiGroupLoaded;
}

static BOOL IsMultiGroupSame(VECTOR<SDK_MULTI_GROUP_INFO_T> &s, VECTOR<SDK_MULTI_GROUP_INFO_T> &d)
{
    if (s.size() != d.size())
    {
        return FALSE;
    }

    for (UINT32 i=0; i<s.size(); i++)
    {
        BOOL    bExist = FALSE;

        for (UINT32 j=0; j<d.size(); j++)
        {
            if (s[i].i64GroupID == d[j].i64GroupID)
            {
                bExist = TRUE;
                if (s[i].ulMemberNum != d[i].ulMemberNum ||
                    memcmp(s[i].ai64MemberGroupID, d[j].ai64MemberGroupID,
                           s[i].ulMemberNum*sizeof(s[i].ai64MemberGroupID)) != 0)
                {
                    return FALSE;
                }

                break;
            }
        }

        if (!bExist)
        {
            return FALSE;
        }
    }

    return TRUE;
}

static VOID ReplaceMultiGroupName(VECTOR<SDK_MULTI_GROUP_INFO_T> &s)
{
    for (UINT32 i=0; i<s.size(); i++)
    {
        for (UINT32 j=0; j<g_vMultiGroupInfo.size(); j++)
        {
            if (s[i].i64GroupID == g_vMultiGroupInfo[j].i64GroupID)
            {
                // eSDK存在BUG，查询结果中组号在调度台重启后是空的
                if (g_vMultiGroupInfo[j].acGroupName[0] != '\0')
                //vMultiGroupInfo[j].acGroupName[0] == '\0')
                {
                    strcpy(s[i].acGroupName, g_vMultiGroupInfo[j].acGroupName);
                }
                break;
            }
        }
    }
}

BOOL LoadMultiGroup (BOOL bForce)
{
    // 如果是非强制模式，并且已经加载过，则不需要加载
    if (!bForce && g_bMultiGroupLoaded)
    {
        return TRUE;
    }

    VECTOR<SDK_MULTI_GROUP_INFO_T>   vMultiGroupInfo;

    if (!eSDK_GetMultiGroup(vMultiGroupInfo))
    {
        return FALSE;
    }

    BOOL    bIsSame = IsMultiGroupSame(g_vMultiGroupInfo, vMultiGroupInfo);

    ReplaceMultiGroupName(vMultiGroupInfo);

    if (!bIsSame)
    {
        SaveMultiGroupInfo(vMultiGroupInfo);
        g_vMultiGroupInfo.swap(vMultiGroupInfo);
        Group::UpdateMultiGroup(g_vMultiGroupInfo);
    }

    g_bMultiGroupLoaded = TRUE;

    return TRUE;
}

BOOL CreateMultiGroup(INT64 i64GroupID, CHAR *szGroupName, VECTOR<INT64> &vGroupID)
{
    if (!eSDK_CreateMultiGroup(i64GroupID, szGroupName, vGroupID))
    {
        return FALSE;
    }

    // eSDk存在BUG，需要调度系统单独维护组ID和组号的对应关系。此处插入后保证加载数据后触发存储
    SDK_MULTI_GROUP_INFO_T      stInfo;

    stInfo.ulDCUserID = APP::GetCurrDCUserID();
    stInfo.i64GroupID = i64GroupID;
    strcpy(stInfo.acGroupName, szGroupName);
    stInfo.ulMemberNum = vGroupID.size();
    for (UINT32 i=0; i<vGroupID.size(); i++)
    {
        stInfo.ai64MemberGroupID[i] = vGroupID[i];
    }

    g_vMultiGroupInfo.push_back(stInfo);

    Group::UpdateMultiGroup(g_vMultiGroupInfo);

    SaveMultiGroupInfo(g_vMultiGroupInfo);

    return TRUE;
}

BOOL CancelMultiGroup(INT64 i64GroupID)
{
    if (!eSDK_CancelMultiGroup(i64GroupID))
    {
        return FALSE;
    }

    for (UINT32 i=0; i<g_vMultiGroupInfo.size(); i++)
    {
        if (g_vMultiGroupInfo[i].i64GroupID == i64GroupID)
        {
            g_vMultiGroupInfo.erase(g_vMultiGroupInfo.begin()+i);
            break;
        }
    }

    Group::UpdateMultiGroup(g_vMultiGroupInfo);
    Group::DelGroup(i64GroupID);
    SaveMultiGroupInfo(g_vMultiGroupInfo);

    return TRUE;
}

BOOL SetMultiGroupMember(INT64 i64GroupID, VECTOR<INT64> &vAddGroupID, VECTOR<INT64> &vDelGroupID)
{
    BOOL    bRet = TRUE;

    if (vAddGroupID.size() > 0)
    {
        bRet = eSDK_AddMultiGroupMember(i64GroupID, vAddGroupID);
    }

    if (vDelGroupID.size() > 0)
    {
        bRet &= eSDK_DelMultiGroupMember(i64GroupID, vDelGroupID);
    }

    LoadMultiGroup(TRUE);

    return bRet;
}

BOOL GetMultiGroupMember(INT64 i64GroupID, VECTOR<INT64> &vGroup)
{
    vGroup.clear();
    for (UINT32 i=0; i<g_vMultiGroupInfo.size(); i++)
    {
        SDK_MULTI_GROUP_INFO_T  &stInfo = g_vMultiGroupInfo[i];

        if (stInfo.i64GroupID == i64GroupID)
        {
            for (UINT32 j=0; j<stInfo.ulMemberNum; j++)
            {
                vGroup.push_back(stInfo.ai64MemberGroupID[j]);
            }

            return TRUE;
        }
    }

    return FALSE;
}

BOOL ConvertMultiGroup(MULTI_GROUP_INFO_T &s, SDK_MULTI_GROUP_INFO_T &d)
{
    //char    acGroupMember[1024+1];
   // char    *szTmp;
   // char    *szMember = acGroupMember;
    INT64   i64GroupID;
    VECTOR<AnsiString>   vGroup;

    memset(&d, 0, sizeof(d));
    d.ulDCUserID = s.ulDCUserID;
    d.i64GroupID = s.i64GroupID;
    strcpy(d.acGroupName, s.acGroupName);

    ParseStringTuple(s.acGroupMember, ",", vGroup);

    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        if (!ToInt64(vGroup[i], &i64GroupID))
        {
            return FALSE;
        }

        if (d.ulMemberNum >= ARRAY_SIZE(d.ai64MemberGroupID))
        {
            GosLog(LOG_ERROR, "too much multi group member of "FORMAT_I64, s.i64GroupID);
            break;
        }

        d.ai64MemberGroupID[d.ulMemberNum++] = i64GroupID;
    }

    /*
    sprintf(acGroupMember, "%s,", s.acGroupMember);

    while(*szMember != '\0')
    {
        szTmp = strchr(szMember, ',');
        if (!szTmp)
        {
            break;
        }

        *szTmp = '\0';
        if (!gos_atoi64(szMember, &i64GroupID))
        {
            return FALSE;
        }

        d.vGroup.push_back(i64GroupID);
        szMember = szTmp+1;
    }   */

    return TRUE;
}

VOID InitAllMultiGroup(VECTOR<MULTI_GROUP_INFO_T> &vInfo)
{
    SDK_MULTI_GROUP_INFO_T  stInfo;

    g_vMultiGroupInfo.clear();
    g_vOtherMultiGroupInfo.clear();

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        ConvertMultiGroup(vInfo[i], stInfo);

        if (stInfo.ulDCUserID == APP::GetCurrDCUserID())
        {
            g_vMultiGroupInfo.push_back(stInfo);
        }
        else
        {
            g_vOtherMultiGroupInfo.push_back(stInfo);
        }
    }

    Group::UpdateMultiGroup(g_vMultiGroupInfo);
}

// 获取群组是否已经加入某多选组
BOOL GetBelongingMultiGroupInfo(INT64 i64GroupID, INT64 &i64MultiGroupID, UINT32 &ulDCUserID)
{
    for (UINT32 i=0; i<g_vMultiGroupInfo.size(); i++)
    {
        for (UINT32 j=0; j<g_vMultiGroupInfo[i].ulMemberNum; j++)
        {
            if (g_vMultiGroupInfo[i].ai64MemberGroupID[j] == i64GroupID)
            {
                i64MultiGroupID = g_vMultiGroupInfo[i].i64GroupID;
                ulDCUserID = APP::GetCurrDCUserID();
                return TRUE;
            }
        }
    }

    for (UINT32 i=0; i<g_vOtherMultiGroupInfo.size(); i++)
    {
        for (UINT32 j=0; j<g_vOtherMultiGroupInfo[i].ulMemberNum; j++)
        {
            if (g_vOtherMultiGroupInfo[i].ai64MemberGroupID[j] == i64GroupID)
            {
                i64MultiGroupID = g_vOtherMultiGroupInfo[i].i64GroupID;
                ulDCUserID = g_vOtherMultiGroupInfo[i].ulDCUserID;
                return TRUE;
            }
        }
    }

    return FALSE;
}


} // namespace PatchGroup

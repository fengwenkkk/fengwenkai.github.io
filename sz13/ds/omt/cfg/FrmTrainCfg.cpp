//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"

#include "MsgDialog.h"
#include "msg.h"
#include "ds_public.h"
#include "ClientUtil.h"
#include "Resource.h"
#include "ClientCommon.h"
#include "RemoteMsg.h"
#include "ClientInit.h"
#include "Train.h"
#include "User.h"
#include "DynamicGroup.h"

#include "FrmTrainCfg.h"
#include "DlgSetTrain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxCustomData"
#pragma link "cxDropDownEdit"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
#pragma link "cxInplaceContainer"
#pragma link "cxLabel"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxMemo"
#pragma link "cxStyles"
#pragma link "cxTextEdit"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "dxSkinBlue"
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinLilian"
#pragma link "dxSkinMcSkin"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "MDIForm"
#pragma link "dxBar"
#pragma link "cxProgressBar"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma resource "*.dfm"
TFormTrainCfg *FormTrainCfg;


UINT32  g_ulCurrCheckedTrainUnitID = INVALID_TRAIN_UNIT_ID;

static int  g_iCurrCheckIndex = -1;

extern AnsiString GetTrainTypeText(UINT32 ulType);

#ifdef ESDK_SIMU

extern CHAR* GetTrainMulticastGroupName(INT64 i64GroupID);

VECTOR<_DYNA_GROUP_T>           g_vDynaGroup;

DC_GROUP_INFO_T* _GetDynamicGroup(INT64 i64GroupID)
{
    for (UINT32 i=0; i<g_vDynaGroup.size(); i++)
    {
        if (g_vDynaGroup[i].i64GroupID == i64GroupID)
        {
            return (DC_GROUP_INFO_T*)1;
        }
    }

    return NULL;
}

void _eSDK_PrintGroupInfo(TcxMemo *m)
{
    m->Clear();

    for (UINT32 i=0; i<g_vDynaGroup.size(); i++)
    {
        char    acLine[1024];
        UINT32  ulLen = 0;

        ulLen = sprintf(acLine, "group=" FORMAT_I64 " name =%s, ", g_vDynaGroup[i].i64GroupID, g_vDynaGroup[i].acGroupName);
        for (UINT32 j=0; j<g_vDynaGroup[i].vUser.size(); j++)
        {
            ulLen += sprintf(acLine+ulLen, "%u ", g_vDynaGroup[i].vUser[j]);
        }

        m->Lines->Append(acLine);
    }
}

BOOL RemoveMember(VECTOR<UINT32> &v, UINT32 ulMember)
{
    for (UINT32 i=0; i<v.size(); i++)
    {
        if (v[i] == ulMember)
        {
            v.erase(v.begin() + i);
            return TRUE;
        }
    }

    return FALSE;
}

BOOL _eSDK_CreateDynamicGroup(INT64 i64GroupID, VECTOR<UINT32> &vUserID, UINT32 ulPriority, UINT32 ulMaxPeriod, CHAR *szGroupName)
{
    _DYNA_GROUP_T       stGroup;

    stGroup.i64GroupID = i64GroupID;
    strcpy(stGroup.acGroupName, szGroupName);
    stGroup.vUser.assign(vUserID.begin(), vUserID.end());

    g_vDynaGroup.push_back(stGroup);

    return TRUE;
}

BOOL _eSDK_CreateDynamicGroup(INT64 i64GroupID, UINT32 ulPriority, UINT32 ulMaxPeriod, CHAR *szGroupName)
{
    VECTOR<UINT32>  vUserID;

    return _eSDK_CreateDynamicGroup(i64GroupID, vUserID, ulPriority, ulMaxPeriod, szGroupName);
}

BOOL _eSDK_SetDynamicGroupUser(INT64 i64GroupID, VECTOR<UINT32> &vAddUserID, VECTOR<UINT32> &vDelUserID)
{
    for (UINT32 i=0; i<g_vDynaGroup.size(); i++)
    {
        _DYNA_GROUP_T   &stGroup = g_vDynaGroup[i];

        if (stGroup.i64GroupID == i64GroupID)
        {
            for (UINT32 j=0; j<vDelUserID.size(); j++)
            {
                RemoveMember(stGroup.vUser, vDelUserID[j]);
            }

            for (UINT32 j=0; j<vAddUserID.size(); j++)
            {
                stGroup.vUser.push_back(vAddUserID[j]);
            }

            return TRUE;
        }
    }

    return FALSE;
}

BOOL _eSDK_GetDynamicGroupMember(INT64 i64GroupID, VECTOR<UINT32> &vUser, VECTOR<INT64> &vGroup)
{
    vUser.clear();

    for (UINT32 i=0; i<g_vDynaGroup.size(); i++)
    {
        _DYNA_GROUP_T   &stGroup = g_vDynaGroup[i];

        if (stGroup.i64GroupID == i64GroupID)
        {
            vUser.assign(stGroup.vUser.begin(), stGroup.vUser.end());
            return TRUE;
        }
    }

    return FALSE;
}

BOOL _eSDK_GetDynamicGroupMember(INT64 i64GroupID, VECTOR<UINT32> &vUser)
{
    VECTOR<INT64> vGroup;

    return _eSDK_GetDynamicGroupMember(i64GroupID, vUser, vGroup);
}

BOOL _eSDK_IsLogined()
{
    return TRUE;
}

VOID _AddDynamicGroup(GROUP_INFO_T &stGroup)
{
}

BOOL _CreateMainLineTrainGroup(TRAIN_INFO_T *pstTrain)
{
    GROUP_INFO_T    stGroup = {0};
    UINT32          ulMaxPeriod = g_ulTrainDefaultGroupMaxPeriod;
    VECTOR<UINT32>  vUserID;
    BOOL            bRet;

    stGroup.ulPriority = g_ulTrainDefaultGroupPriority;
    stGroup.ulDCUserID = g_ulCurrDCUserID;
    stGroup.i64GroupID = pstTrain->i64MainLineGroupID;
    stGroup.ulGroupType = TRAIN_GROUP;
    sprintf(stGroup.acGroupName, "%s_%u", GROUP_NAME_MAINLINE_TRAIN, pstTrain->ulTrainUnitID);

    GetTrainUserID(pstTrain->ulTrainUnitID, vUserID);
    if (vUserID.size() > 0)
    {
        bRet = eSDK_CreateDynamicGroup(stGroup.i64GroupID, vUserID, stGroup.ulPriority, ulMaxPeriod, stGroup.acGroupName);
    }
    else
    {
        bRet = eSDK_CreateDynamicGroup(stGroup.i64GroupID, stGroup.ulPriority, ulMaxPeriod, stGroup.acGroupName);
    }

    if (!bRet)
    {
        return FALSE;
    }

    AddGroup(&stGroup);
    _AddDynamicGroup(stGroup);
//  eSDK_SubJoinGroup(stGroup.i64GroupID);

    return TRUE;
}

BOOL _CreateDepotTrainGroup(TRAIN_INFO_T *pstTrain)
{
    GROUP_INFO_T    stGroup = {0};
    UINT32          ulMaxPeriod = g_ulTrainDefaultGroupMaxPeriod;
    VECTOR<UINT32>  vUserID;
    BOOL            bRet;

    stGroup.ulPriority = g_ulTrainDefaultGroupPriority;
    stGroup.ulDCUserID = g_ulCurrDCUserID;
    stGroup.i64GroupID = pstTrain->i64DepotGroupID;
    stGroup.ulGroupType = TRAIN_GROUP;
    sprintf(stGroup.acGroupName, "%s_%u", GROUP_NAME_DEPOT_TRAIN, pstTrain->ulTrainUnitID);

    GetTrainUserID(pstTrain->ulTrainUnitID, vUserID);
    if (vUserID.size() > 0)
    {
        bRet = eSDK_CreateDynamicGroup(stGroup.i64GroupID, vUserID, stGroup.ulPriority, ulMaxPeriod, stGroup.acGroupName);
    }
    else
    {
        bRet = eSDK_CreateDynamicGroup(stGroup.i64GroupID, stGroup.ulPriority, ulMaxPeriod, stGroup.acGroupName);
    }

    if (!bRet)
    {
        return FALSE;
    }

    AddGroup(&stGroup);
    _AddDynamicGroup(stGroup);
//  eSDK_SubJoinGroup(stGroup.i64GroupID);

    return TRUE;
}

BOOL _CreateTrainBrdGroup(TRAIN_INFO_T *pstTrain)
{
    GROUP_INFO_T    stGroup = {0};
    UINT32          ulMaxPeriod = g_ulTrainBroadcastMaxPeriod;
    VECTOR<UINT32>  vUserID;
    BOOL            bRet;

    stGroup.ulPriority = g_ulTrainBroadcastGroupPriority;
    stGroup.ulDCUserID = g_ulCurrDCUserID;
    stGroup.i64GroupID = pstTrain->i64BrdGroupID;
    stGroup.ulGroupType = TRAIN_GROUP;
    sprintf(stGroup.acGroupName, "%s_%u", GROUP_NAME_TRAIN_BRD, pstTrain->ulTrainUnitID);

    GetTrainUserID(pstTrain->ulTrainUnitID, vUserID);
    if (vUserID.size() > 0)
    {
        bRet = eSDK_CreateDynamicGroup(stGroup.i64GroupID, vUserID, stGroup.ulPriority, ulMaxPeriod, stGroup.acGroupName);
    }
    else
    {
        bRet = eSDK_CreateDynamicGroup(stGroup.i64GroupID, stGroup.ulPriority, ulMaxPeriod, stGroup.acGroupName);
    }

    if (!bRet)
    {
        return FALSE;
    }

    AddGroup(&stGroup);
    _AddDynamicGroup(stGroup);
//  eSDK_SubJoinGroup(stGroup.i64GroupID);

    return TRUE;
}

BOOL CreateTrainMulticastGroup(INT64 i64GroupID)
{
    GROUP_INFO_T    stGroup = {0};
    UINT32          ulMaxPeriod = g_ulTrainBroadcastMaxPeriod;
    BOOL            bRet;
    CHAR            *szGroupName = GetTrainMulticastGroupName(i64GroupID);

    if (!szGroupName)
    {
        return FALSE;
    }

    stGroup.ulPriority = g_ulTrainBroadcastGroupPriority;
    stGroup.ulDCUserID = g_ulCurrDCUserID;
    stGroup.i64GroupID = i64GroupID;
    stGroup.ulGroupType = TRAIN_GROUP;

    if (i64GroupID == UPLINE_TRAIN_BRD_GROUPID  &&
        i64GroupID == DOWNLINE_TRAIN_BRD_GROUPID &&
        i64GroupID == ALLLINE_TRAIN_BRD_GROUPID )
    {
        ulMaxPeriod = g_ulTrainDefaultGroupMaxPeriod;
        stGroup.ulPriority = g_ulTrainDefaultGroupPriority;
    }

    sprintf(stGroup.acGroupName, szGroupName);

    bRet = eSDK_CreateDynamicGroup(stGroup.i64GroupID, stGroup.ulPriority, ulMaxPeriod, stGroup.acGroupName);
    if (!bRet)
    {
        return FALSE;
    }

    AddGroup(&stGroup);
    AddDynamicGroup(stGroup);

    return TRUE;
}

BOOL CreateStationMulticastGroup(INT64 i64GroupID)
{
    GROUP_INFO_T    stGroup = {0};
    UINT32          ulMaxPeriod = g_ulDynamicGroupMaxPeriod;
    BOOL            bRet;

    stGroup.ulPriority = g_ulDynamicGroupPriority;
    stGroup.ulDCUserID = g_ulCurrDCUserID;
    stGroup.i64GroupID = i64GroupID;
    stGroup.ulGroupType = STATION_GROUP;
    sprintf(stGroup.acGroupName, "车站通播组");

    bRet = eSDK_CreateDynamicGroup(stGroup.i64GroupID, stGroup.ulPriority, ulMaxPeriod, stGroup.acGroupName);
    if (!bRet)
    {
        return FALSE;
    }

    AddGroup(&stGroup);
    AddDynamicGroup(stGroup);

    return TRUE;
}

#endif

//---------------------------------------------------------------------------
__fastcall TFormTrainCfg::TFormTrainCfg(TComponent* Owner)
    : TSubForm(Owner)
{
    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::btnAddClick(TObject *Sender)
{
    DialogSetTrain->Init();
    if (DialogSetTrain->ShowModal() != mrOk)
    {
        return;
    }

    TRAIN_INFO_T        stInfo;

    DialogSetTrain->Get(stInfo);

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("DCUserID", g_ulCurrDCUserID);
    ReqJsonParam.Add("TrainUnitID", stInfo.ulTrainUnitID);
    ReqJsonParam.Add("TrainType", stInfo.ulTrainType);
    ReqJsonParam.Add("DefaultGroupID", stInfo.i64DefaultGroupID);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCAddTrain, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);

    CheckTrainGroup();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::btnDelClick(TObject *Sender)
{
    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlMain->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    UINT32      ulID = Node->Values[0];
    AnsiString  strName = Node->Texts[1];

    if (!AskMsg((CHAR*)"删除列车%s吗？", strName.c_str()))
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("TrainUnitID", ulID);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCDelTrain, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::btnSetClick(TObject *Sender)
{
    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlMain->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    TRAIN_INFO_T    stInfo;

    stInfo.ulTrainUnitID = (UINT32)Node->Values[0];
    stInfo.ulTrainType = (UINT32)Node->Values[1];
    stInfo.i64DefaultGroupID = (INT64)Node->Values[4];

    DialogSetTrain->Init(stInfo);
    if (DialogSetTrain->ShowModal() != mrOk)
    {
        return;
    }

    DialogSetTrain->Get(stInfo);

    if (stInfo.ulTrainUnitID == (UINT32)Node->Values[0] &&
        stInfo.ulTrainType == (UINT32)Node->Values[1])
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("TrainUnitID", stInfo.ulTrainUnitID);
    ReqJsonParam.Add("TrainType", stInfo.ulTrainType);
    ReqJsonParam.Add("DefaultGroupID", stInfo.i64DefaultGroupID);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCSetTrain, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::btnRefreshClick(TObject *Sender)
{
    if (!LoadTrainInfo())
    {
        return;
    }

    InitView();

#ifdef ESDK_SIMU
    mmESDKGroup->Visible = true;
    ::_eSDK_PrintGroupInfo(mmESDKGroup);
#endif
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::InitNode(TcxTreeListNode *Node, TRAIN_INFO_T *pstTrain)
{
    UINT32      ulUserID1;
    UINT32      ulUserID2;

    GetTrainUserID(pstTrain->ulTrainUnitID, &ulUserID1, &ulUserID2);

    Node->Values[0] = pstTrain->ulTrainUnitID;
    Node->Values[1] = pstTrain->ulTrainType;
    Node->Values[2] = ulUserID1;
    Node->Values[3] = ulUserID2;
    Node->Values[4] = pstTrain->i64DefaultGroupID;
//  Node->Values[5] = pstTrain->i64DepotGroupID;
//  Node->Values[6] = pstTrain->i64BrdGroupID;
    Node->Values[7] = 0;//pstTrain->ulDCUserID;
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::InitView()
{
    TcxTreeListNode         *Node;
    VECTOR<TRAIN_INFO_T*>   vInfo;
    TRAIN_INFO_T            *pstTrain;

    GetTrainInfo(vInfo);

    tlMain->BeginUpdate();

    for (int i=tlMain->AbsoluteCount-1; i>=0; i--)
    {
        Node = tlMain->AbsoluteItems[i];
        pstTrain = NULL;

        for (UINT32 j=0; j<vInfo.size(); j++)
        {
            if ((UINT32)Node->Values[0] == vInfo[j]->ulTrainUnitID)
            {
                pstTrain = vInfo[j];
                InitNode(Node, pstTrain);
                vInfo.erase(vInfo.begin() + j);
                break;
            }
        }

        if (!pstTrain)
        {
            Node->Delete();
        }
    }

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        Node = tlMain->Add();

        InitNode(Node, vInfo[i]);
        Node->Values[8] = "";
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::Column2GetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    if (ANode->Values[1].IsNull())
    {
        return;
    }

    UINT32  ulType = ANode->Values[1];

    Value = GetTrainTypeText(ulType);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::btnImportClick(TObject *Sender)
{
    if (!OpenDialog->Execute())
    {
        return;
    }

    AnsiString  strFile = OpenDialog->FileName;

    if (!ImportFromFile(tlMain, strFile.c_str(), false))
    {
        InfoMsg("加载文件失败");
        return;
    }

}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::FormCreate(TObject *Sender)
{
    SetImage(btnAdd, IMG_ADD);
    SetImage(btnDel, IMG_DEL);
    SetImage(btnSet, IMG_SET);
    SetImage(btnRefresh, IMG_REFRESH);
    SetImage(btnImport, IMG_IMPORT);
}
//---------------------------------------------------------------------------
BOOL __fastcall TFormTrainCfg::CheckTrainGroup(UINT32 ulTrainUnitID, INT64 i64GroupID, BOOL &bCheckGroup, TcxTreeListNode *Node)
{
    VECTOR<UINT32>  vExistedUserID;     // 集群中已经存在的组成员
    VECTOR<UINT32>  vUserID;            // TrainUE记录中的用户
    VECTOR<UINT32>  vDCUserID;
    int             i, j;
    UINT32          ulNum;

    if (bCheckGroup)
    {
        Node->Values[8] = "";
        return FALSE;
    }

    GetTrainUserID(ulTrainUnitID, vUserID);
    ulNum = vUserID.size();

    // 获取组成员
    if (!eSDK_GetDynamicGroupMember(i64GroupID, vExistedUserID))
    {
        Node->Values[8] = "获取组成员失败";
        return FALSE;
    }

    // 过滤掉共同的成员
    GetTrainDCUser(vDCUserID);
    AppendMember(vUserID, vDCUserID);
    RemoveCommonMember(vUserID, vExistedUserID);

    if (vUserID.size() == 0 && vExistedUserID.size() == 0)
    {
        if (ulNum == 2)
        {
            // 更新状态，TODO 先不存数据库，本地缓存方式存储
            bCheckGroup = TRUE;
            Node->Values[8] = "";
        }
        else if (ulNum < 2)
        {
            Node->Values[8] = "车载台必须为2个";
        }
        else
        {
            Node->Values[8] = AnsiString("车载台有") + ulNum + AnsiString("个，必须为2个");
        }

        // 没有动作
        return FALSE;
    }

    // 经过过滤后，将数据库中的加入，已经存在的删除
    eSDK_SetDynamicGroupUser(i64GroupID, vUserID, vExistedUserID);
    Node->Values[8] = "设置组成员";

    // 返回成功（保证本次不在继续检查,eSDK调用需要耗一定时间，不能很频繁）
    return TRUE;
}

//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::CheckTrainGroup()
{
   /*

    if (!eSDK_IsLogined() || tlMain->AbsoluteCount == 0)
    {
        return;
    }

    TRAIN_INFO_T    *pstTrain;
    BOOL            bExist;
    UINT32          ulTrainUnitID;
    TcxTreeListNode *Node;

    if (g_iCurrCheckIndex < 0)
    {
        g_iCurrCheckIndex = 0;
    }
    else if (g_iCurrCheckIndex >= tlMain->AbsoluteCount)
    {
        g_iCurrCheckIndex = 0;
        TimerCheck->Interval = 10000; // 检查一次结束后自动恢复成慢检查方式
    }

    for (;g_iCurrCheckIndex<tlMain->AbsoluteCount; g_iCurrCheckIndex++)
    {
        int     i = g_iCurrCheckIndex;

        Node = tlMain->AbsoluteItems[i];
        if (Node->Values[0].IsNull())
        {
            continue;
        }

        ulTrainUnitID = Node->Values[0];
        pstTrain = GetTrainInfo(ulTrainUnitID);
        if (!pstTrain)
        {
            Node->Values[8] = "未知列车";
            continue;
        }

        if (pstTrain->ulTrainType != TRAIN_TYPE_NORMAL)
        {
            Node->Values[8] = "非运营列车";
            continue;
        }

        if (pstTrain->ulDCUserID != g_ulCurrDCUserID)
        {
            Node->Values[8] = "需创建者检查";
            continue;
        }

        bExist = FALSE;
        if (!GetDynamicGroup(pstTrain->i64MainLineGroupID))
        {
            Node->Values[8] = "正在创建正线列车组";
            CreateMainLineTrainGroup(pstTrain);
            bExist = TRUE;
        }
        else if (CheckTrainGroup(pstTrain->ulTrainUnitID, pstTrain->i64MainLineGroupID, pstTrain->bCheckMainLineGroup, Node))
        {
            bExist = TRUE;
        }

        if (!GetDynamicGroup(pstTrain->i64DepotGroupID))
        {
            Node->Values[8] = "正在创建车辆段列车组";
            CreateDepotTrainGroup(pstTrain);
            bExist = TRUE;
        }
        else if (CheckTrainGroup(pstTrain->ulTrainUnitID, pstTrain->i64DepotGroupID, pstTrain->bCheckDepotGroup, Node))
        {
            bExist = TRUE;
        }

        if (!GetDynamicGroup(pstTrain->i64BrdGroupID))
        {
            Node->Values[8] = "正在创建列车广播组";
            CreateTrainBrdGroup(pstTrain);
            bExist = TRUE;
        }
        else if (CheckTrainGroup(pstTrain->ulTrainUnitID, pstTrain->i64BrdGroupID, pstTrain->bCheckBrdGroup, Node))
        {
            bExist = TRUE;
        }

        // 每次只检查一列车
        if (bExist)
        {
            break;
        }
    }

    g_iCurrCheckIndex++;   */
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::TimerCheckTimer(TObject *Sender)
{
    CheckTrainGroup();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::btnCheckClick(TObject *Sender)
{
    if (!AskMsg("对所有列车组进行检查吗"))
    {
        return;
    }

    g_iCurrCheckIndex = -1;
    TimerCheck->Interval = 10;

    VECTOR<TRAIN_INFO_T*>   &vTrainInfo = GetTrainInfo();
   /*
    for (UINT32 i=0; i<vTrainInfo.size(); i++)
    {
        vTrainInfo[i]->bCheckMainLineGroup = FALSE;
        vTrainInfo[i]->bCheckDepotGroup = FALSE;
        vTrainInfo[i]->bCheckBrdGroup = FALSE;
    }   */

    tlMain->BeginUpdate();
    for (int i=0; i<tlMain->AbsoluteCount; i++)
    {
        tlMain->AbsoluteItems[i]->Values[8] = "待检测";
    }

    tlMain->EndUpdate();

    InitView();
}
//---------------------------------------------------------------------------


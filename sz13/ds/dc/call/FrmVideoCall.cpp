//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "UICfg.h"
#include "DlgVideoCallCfg.h"
#include "FrmVideoCall.h"
#include "main.h"
#include "FrmCallIn.h"
#include "FrmCDR.h"
#include "FrmLocalRecManage.h"
#include "DlgSubUser.h"
#include "DlgSubGroup.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxCustomData"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
#pragma link "cxInplaceContainer"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxStyles"
#pragma link "cxTextEdit"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "dxSkinBlue"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "MDIForm"
#pragma link "dxBar"
#pragma link "cxClasses"
#pragma link "cxImageList"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxImageComboBox"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxPC"
#pragma link "dxBarBuiltInMenu"
#pragma link "cxScrollBox"
#pragma resource "*.dfm"
TFormVideoCall *FormVideoCall;

static bool m_bFullScreenMode = false;
static HWND m_hwndPanelParent = NULL;
static TPanel   *g_FullScreenPanel = NULL;
static TAlign   g_PanelAlign = alClient;
static TWinControl  *g_FullScreenPanelParent = NULL;

static VECTOR<TFormVideoWin*>       g_vVideoWin;

BOOL IsFullScreen()
{
    return m_bFullScreenMode;
}
//---------------------------------------------------------------------------
void FullScreen(TPanel *Panel)
{
    int cx = GetSystemMetrics(SM_CXSCREEN);
    int cy = GetSystemMetrics(SM_CYSCREEN);

    HWND hWnd = Panel->Handle;

    g_PanelAlign = Panel->Align;
    Panel->Align = alNone;
    g_FullScreenPanel = Panel;
    g_FullScreenPanelParent = Panel->Parent;

    m_hwndPanelParent = ::GetParent(hWnd);

    ::SetParent(hWnd, NULL);        //这里设置为NULL

    SetWindowPos(hWnd, HWND_TOPMOST, 0,0, cx, cy, SWP_FRAMECHANGED);//这里顶层

    ::SetForegroundWindow(GetDesktopWindow());
    ::SetForegroundWindow(hWnd);

    Panel->Repaint();
    m_bFullScreenMode = true;
}
//---------------------------------------------------------------------------
void ExitFullScreen()
{
    TPanel *Panel = g_FullScreenPanel;
    HWND hWnd = Panel->Handle;

    ::SetParent(hWnd, m_hwndPanelParent);

    ::SetFocus(m_hwndPanelParent);
    ::SetFocus(hWnd);

    Panel->Parent = g_FullScreenPanelParent;//gbVideoWin1;
    Panel->Align = g_PanelAlign;//alClient;
    Panel->Visible = true;

    m_bFullScreenMode = false;
}
//---------------------------------------------------------------------------
BOOL SaveSubVideoUser(IntSet &sOld, IntSet &sNew)
{
    GJsonParam          ReqParam;
    GJsonTupleParam     TupleParam;

    for (UINT32 i=0; i<sOld.Size(); i++)
    {
        GJsonParam  SubParam;

        SubParam.Add("UserID", sOld.Get(i));
        TupleParam.Add(SubParam);
    }

    ReqParam.Add("DelUser", TupleParam);

    TupleParam.Clear();
    for (UINT32 i=0; i<sNew.Size(); i++)
    {
        GJsonParam  SubParam;

        SubParam.Add("UserID", sNew.Get(i));
        TupleParam.Add(SubParam);
    }

    ReqParam.Add("AddUser", TupleParam);

    if (!SyncSendServer(EV_DC_DIS_REQ, MSG_DCSaveSubVideoUserInfo, ReqParam))
    {
        return FALSE;
    }

    return TRUE;
}
//---------------------------------------------------------------------------
UINT32 GetVideoWinNum()
{
    return g_vVideoWin.size();
}
//---------------------------------------------------------------------------
// 调度台被叫，caller为终端
TFormVideoWin* GetVideoWin(UINT32 ulCaller)
{
    for (UINT32 i=0; i<g_vVideoWin.size(); i++)
    {
        if (g_vVideoWin[i]->GetUserID() == ulCaller)
        {
            return g_vVideoWin[i];
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
TFormVideoWin* GetVideoWin(UINT32 ulCaller, UINT32 ulCallee)
{
    for (UINT32 i=0; i<g_vVideoWin.size(); i++)
    {
        UINT32  ulUserID = g_vVideoWin[i]->GetUserID();
        UINT32  ulCallType = g_vVideoWin[i]->GetCallType();

        if (ulCallType == CALL_TYPE_VIDEO_P2P )
        {
            if (g_vVideoWin[i]->IsOutCalling())
            {
                if (ulCallee == ulUserID)
                {
                    return g_vVideoWin[i];
                }
            }
            else
            {
                if (ulCaller == ulUserID)
                {
                    return g_vVideoWin[i];
                }
            }
        }
        else if (ulCallType == CALL_TYPE_VIDEO_REALPLAY )
        {
            if (ulCallee == ulUserID)
            {
                return g_vVideoWin[i];
            }
        }
        else if (ulCallType == CALL_TYPE_VIDEO_RECV)
        {
            if (ulCaller == ulUserID)
            {
                return g_vVideoWin[i];
            }
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
TFormVideoWin* CreateVideoWin()
{
    TFormVideoWin   *pVideoWin = new TFormVideoWin(Application);

    pVideoWin->Parent = FormVideoCall->gbVideoWin;
    pVideoWin->Visible = false;
    g_vVideoWin.push_back(pVideoWin);

    return pVideoWin;
}
//---------------------------------------------------------------------------
VECTOR<TFormVideoWin*>& GetAllVideoWin()
{
    return g_vVideoWin;
}
//---------------------------------------------------------------------------
TFormVideoWin* GetIdleVideoWin(UINT32 ulUserID)
{
    UINT32          ulNewNum = 0;
    UINT32          ulTotalWinNum = g_vVideoWin.size();
    TFormVideoWin   *pDeleteVideoWin = NULL;

    for (UINT32 i=0; i<ulTotalWinNum; i++)
    {
        if (g_vVideoWin[i]->GetUserID() == ulUserID)
        {
            pDeleteVideoWin = g_vVideoWin[i];

            delete pDeleteVideoWin;
            g_vVideoWin.erase(g_vVideoWin.begin()+i);
            CreateVideoWin();

            break;
        }
    }

    for (UINT32 i=0; i<ulTotalWinNum; i++)
    {
        if (!g_vVideoWin[i]->IsPlaying())
        {
            return g_vVideoWin[i];
        }
    }

    if (ulTotalWinNum == 0)
    {
        ulNewNum = 1 - ulTotalWinNum;
    }
    else if (ulTotalWinNum == 1)
    {
        ulNewNum = 4 - ulTotalWinNum;
    }
    else if (ulTotalWinNum == 4)
    {
        ulNewNum = 9 - ulTotalWinNum;
    }
    else if (ulTotalWinNum == 9)
    {
        ulNewNum = 16 - ulTotalWinNum;
    }
    else
    {
        GosLog(LOG_ERROR, "invalid video win number %u", ulTotalWinNum);
        return NULL;
    }

    TFormVideoWin   *pNewVideoWin = NULL;

    for (UINT32 i=0; i<ulNewNum; i++)
    {
        if (!pNewVideoWin)
        {
            pNewVideoWin = CreateVideoWin();
        }
        else
        {
            CreateVideoWin();
        }
    }

    return pNewVideoWin;
}

BOOL SaveSubVideoGroup(Int64Set &sOld, Int64Set &sNew)
{
    GJsonParam          ReqParam;
    GJsonTupleParam     TupleParam;

    for (UINT32 i=0; i<sOld.Size(); i++)
    {
        GJsonParam  SubParam;

        SubParam.Add("GroupID", sOld.Get(i));
        TupleParam.Add(SubParam);
    }

    ReqParam.Add("DelGroup", TupleParam);

    TupleParam.Clear();
    for (UINT32 i=0; i<sNew.Size(); i++)
    {
        GJsonParam  SubParam;

        SubParam.Add("GroupID", sNew.Get(i));
        TupleParam.Add(SubParam);
    }

    ReqParam.Add("AddGroup", TupleParam);

    if (!SyncSendServer(EV_DC_DIS_REQ, MSG_DCSaveSubVideoGroupInfo, ReqParam))
    {
        return FALSE;
    }

    return TRUE;
}

//---------------------------------------------------------------------------
UINT32 __fastcall TFormVideoCall::GetSelectUser()
{
    if (tlUser->SelectionCount == 0)
    {
        return INVALID_USER_ID;
    }

    TcxTreeListNode *Node = tlUser->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return INVALID_USER_ID;
    }

    return Node->Values[1];
}

void __fastcall TFormVideoCall::InitSkin()
{
    SetSkin(tlUser, true);
    SetSkin(tlGroup, true);
}

void __fastcall TFormVideoCall::InitWorkPanel()
{
    TcxButton   *btns[] = { btnVideoRealPlay,
                            btnVideoP2PCall,
                            btnVideoGroupCall,
                            btnPTT,
                          };

    gbWorkPanel->Width = 120;

    UINT32  j = 0;

    for (UINT32 i=0; i<ARRAY_SIZE(btns); i++)
    {
        if (!btns[i]->Visible)
        {
            continue;
        }

        btns[i]->TabOrder = i;
        btns[i]->Top = 50+95*j;
        btns[i]->Width = g_iMidToolbarButtonWidth;
        btns[i]->Height = g_iMidToolbarButtonHeight;
        btns[i]->Left = (btns[i]->Parent->Width - btns[i]->Width)/2;

        j++;
    }
}

//---------------------------------------------------------------------------
__fastcall TFormVideoCall::TFormVideoCall(TComponent* Owner)
    : TSubForm(Owner)
{
    m_bInit = false;

    btnVideoP2PCall->Visible = AllowP2PVideoCall();

    pcMain->ActivePageIndex = 0;

    InitUser();
    InitGroup();

    InitWorkPanel();

    SetButton();

    m_bInit = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::InitUser()
{
    m_mUserNode.clear();

    tlUser->BeginUpdate();
    tlUser->Clear();

    TcxTreeListNode     *Node;
    TcxTreeListNode     *SubNode;
    TcxTreeListNode     *UserNode;
    UINT32              i;
    UINT32              ulUserType;
    UINT32              ulFuncType;
    UINT32              ulDevStatus;
    UINT32              ulCellID;
    UINT32              ulCallStatus;
    CHAR                acUserName[64];
    VECTOR<PORTAL_UE_INFO_T*>  &vPortalUE = PortalUE::GetPortalUE();

    // 车站终端，列车终端，维护终端，管理终端、其他
    Node = tlUser->Add();
    Node->Values[0] = "车站终端";
    for (i=0; i<vPortalUE.size(); i++)
    {
        PORTAL_UE_INFO_T    *pstInfo = vPortalUE[i];

        if (pstInfo->ulFuncType == UE_STATION )
        {
            SubNode = Node->AddChild();
            SubNode->Values[1] = pstInfo->ulUserID;
            SubNode->Values[2] = pstInfo->ulType;
        }
    }

    Node = tlUser->Add();
    Node->Values[0] = "维护终端";
    for (i=0; i<vPortalUE.size(); i++)
    {
        PORTAL_UE_INFO_T    *pstInfo = vPortalUE[i];

        if (pstInfo->ulFuncType == UE_FIX)
        {
            SubNode = Node->AddChild();
            SubNode->Values[1] = pstInfo->ulUserID;
            SubNode->Values[2] = pstInfo->ulType;
        }
    }

    Node = tlUser->Add();
    Node->Values[0] = "管理终端";
    for (i=0; i<vPortalUE.size(); i++)
    {
        PORTAL_UE_INFO_T    *pstInfo = vPortalUE[i];

        if (pstInfo->ulFuncType == UE_MANAGE)
        {
            SubNode = Node->AddChild();
            SubNode->Values[1] = pstInfo->ulUserID;
            SubNode->Values[2] = pstInfo->ulType;
        }
    }

    Node = tlUser->Add();
    Node->Values[0] = "其他";
    for (i=0; i<vPortalUE.size(); i++)
    {
        PORTAL_UE_INFO_T    *pstInfo = vPortalUE[i];

        if (pstInfo->ulFuncType != UE_FIX &&
            pstInfo->ulFuncType != UE_MANAGE &&
            pstInfo->ulFuncType != UE_STATION)
        {
            SubNode = Node->AddChild();
            SubNode->Values[1] = pstInfo->ulUserID;
            SubNode->Values[2] = pstInfo->ulType;
        }
    }

    for (int i=tlUser->AbsoluteCount-1; i>=0; i--)
    {
        Node = tlUser->AbsoluteItems[i];
        if (Node->Values[1].IsNull())
        {
            continue;
        }

        UINT32  ulUserID = Node->Values[1];

        if (!pmiShowAllUser->Down &&
            !g_sSubVideoUser.Has(ulUserID))
        {
            Node->Delete();
        }
        else
        {
            if (User::GetUserInfo(ulUserID, acUserName, &ulUserType, &ulFuncType, &ulDevStatus, &ulCellID, &ulCallStatus))
            {
                Node->Values[3] = ulCallStatus;
            }

            m_mUserNode[ulUserID] = Node;
        }
    }

    tlUser->Root->Expand(true);
    tlUser->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::InitGroup()
{
    tlGroup->BeginUpdate();
    tlGroup->Clear();

    TcxTreeListNode     *Node;
    TcxTreeListNode     *SubNode;
    UINT32              i;
    VECTOR<GROUP_INFO_T*>       vGroup;
    VECTOR<TRAIN_INFO_T*>       &vTrainInfo = Train::GetTrainInfo();
    VECTOR<STATION_UE_INFO_T*>  &vStationUE = StationUE::GetStationUE();

    Group::GetGroup(STATIC_GROUP, vGroup);
    for (int i=vGroup.size()-1; i>=0; i--)
    {
        if (!pmiShowAllGroup->Down &&
            !g_sSubVideoGroup.Has(vGroup[i]->i64GroupID))
        {
            vGroup.erase(vGroup.begin()+i);
        }
    }

    char    acName[128];

    // 列车组、车站组、维护组、多职能组、其他
    Node = tlGroup->Add();
    Node->Values[0] = "列车组";
    for (i=0; i<vTrainInfo.size(); i++)
    {
        sprintf(acName, "列车%u("FORMAT_I64")", vTrainInfo[i]->ulTrainUnitID, vTrainInfo[i]->i64DefaultGroupID);
        SubNode = Node->AddChild();
        SubNode->Values[0] = acName;
        SubNode->Values[1] = vTrainInfo[i]->i64DefaultGroupID;
    }

    Node = tlGroup->Add();
    Node->Values[0] = "车站组";
    for (i=0; i<vStationUE.size(); i++)
    {
        AnsiString  strStationName = Station::GetStationName(vStationUE[i]->ulStationID);

        sprintf(acName, "%s("FORMAT_I64")", strStationName.c_str(), vStationUE[i]->i64DefaultGroupID);
        SubNode = Node->AddChild();
        SubNode->Values[0] = acName;
        SubNode->Values[1] = vStationUE[i]->i64DefaultGroupID;
    }

    Node = tlGroup->Add();
    Node->Values[0] = "维护组";
    for (i=0; i<vGroup.size(); i++)
    {
        if (Group::GetGroupFuncType(vGroup[i]->i64GroupID) != GROUP_FUNC_FIX)
        {
            continue;
        }

        SubNode = Node->AddChild();
        SubNode->Values[0] = Group::GetGroupDisplayName(vGroup[i]->i64GroupID);
        SubNode->Values[1] = vGroup[i]->i64GroupID;
    }

    Node = tlGroup->Add();
    Node->Values[0] = "多职能组";
    for (i=0; i<vGroup.size(); i++)
    {
        if (Group::GetGroupFuncType(vGroup[i]->i64GroupID) != GROUP_FUNC_MULTIFUNC)
        {
            continue;
        }

        SubNode = Node->AddChild();
        SubNode->Values[0] = Group::GetGroupDisplayName(vGroup[i]->i64GroupID);
        SubNode->Values[1] = vGroup[i]->i64GroupID;
    }

    Node = tlGroup->Add();
    Node->Values[0] = "其他";
    for (i=0; i<vGroup.size(); i++)
    {
//      GosLog(LOG_INFO, "static node %u "FORMAT_I64, vGroup[i]->ulFuncType, vGroup[i]->i64GroupID);

        if (Group::GetGroupFuncType(vGroup[i]->i64GroupID) != GROUP_FUNC_OTHER)
        {
            continue;
        }

        SubNode = Node->AddChild();
        SubNode->Values[0] = Group::GetGroupDisplayName(vGroup[i]->i64GroupID);
        SubNode->Values[1] = vGroup[i]->i64GroupID;
    }

    tlGroup->Root->Expand(true);
    tlGroup->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::pmiVideoRealPlayClick(TObject *Sender)
{
    UINT32  ulUserID = GetSelectUser();

    if (ulUserID == INVALID_USER_ID)
    {
        return;
    }

    if (!StartVideoCall(ulUserID, FALSE))
    {
        return;
    }
}
//---------------------------------------------------------------------------
bool __fastcall TFormVideoCall::GetVideoCfg(UINT32 ulUserID, UINT32 &ulVideoFormat, UINT32 &ulCameraType, BOOL &bNeedConfirm, BOOL &bHasVoice)
{
    DialogVideoCallCfg->Init();
    if (DialogVideoCallCfg->ShowModal() != mrOk)
    {
        return false;
    }

    DialogVideoCallCfg->GetVideoCfg(ulVideoFormat, ulCameraType, bNeedConfirm, bHasVoice);

    return true;
}
//---------------------------------------------------------------------------
bool __fastcall TFormVideoCall::StartVideoCall(UINT32 ulUserID, BOOL bIsVideoCall)
{
    UINT32          ulVideoFormat;
    UINT32          ulCameraType;
    BOOL            bNeedConfirm;
    BOOL            bHasVoice;
    TFormVideoWin   *pNewVideoWin;

    if (!GetVideoCfg(ulUserID, ulVideoFormat, ulCameraType, bNeedConfirm, bHasVoice))
    {
        return false;
    }

    pNewVideoWin = GetIdleVideoWin(ulUserID);
    if (!pNewVideoWin)
    {
        InfoMsg("视频窗口已达最大值，\n请先关闭部分视频窗口。");

        return false;
    }

    ResizeVideoWin();

    if (!bIsVideoCall)
    {
        return pNewVideoWin->Play(ulUserID, ulVideoFormat, ulCameraType, bNeedConfirm, bHasVoice);
    }
    else
    {
        return pNewVideoWin->Call(ulUserID, ulVideoFormat, ulCameraType, bNeedConfirm, bHasVoice);
    }
}
//---------------------------------------------------------------------------
bool __fastcall TFormVideoCall::RecvVideoCall(UINT32 ulCallType, UINT32 ulUserID, UINT32 ulVideoFormat, UINT32 ulCameraType)
{
    TFormVideoWin   *pNewVideoWin;

    pNewVideoWin = GetIdleVideoWin(ulUserID);
    if (!pNewVideoWin)
    {
        eSDK_P2PVideoReject(ulUserID);
        SaveP2PVideoCallInfo(ulCallType, ulUserID, APP::GetCurrDCUserID(), CALL_STATUS_REJECT);
        InfoMsg("视频窗口已达最大值，\n请先关闭部分视频窗口。");

        return false;
    }

    ResizeVideoWin();
    if (!pNewVideoWin->Recv(ulCallType, ulUserID, ulVideoFormat, ulCameraType))
    {
        SaveP2PVideoCallInfo(ulCallType, ulUserID, APP::GetCurrDCUserID(), CALL_STATUS_RECV_FAIL);
        ErrMsg("接收视频失败！");
        return false;
    }

    SaveP2PVideoCallInfo(ulCallType, ulUserID, APP::GetCurrDCUserID(), CALL_STATUS_RECV_SUCC);

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::StopVideoPlay()
{
    TimerCheck->Interval = 1;
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::ResizeVideoWin()
{
    if (!m_bInit)
    {
        return;
    }

    int             iWidth = gbVideoWin->Width;
    int             iHeight = gbVideoWin->Height;
    UINT32          ulNum = g_vVideoWin.size();
    TFormVideoWin   *pVideoWin;
    UINT32          i, j;
    UINT32          ulRowNum;
    UINT32          ulColNum;
    int             x, y;
    int             iWinWidth;
    int             iWinHeight;
    int             iLastWinWidth;
    int             iLastWinHeight;

    if (ulNum == 1)
    {
        ulRowNum = 1;
    }
    else if (ulNum == 4)
    {
        ulRowNum = 2;
    }
    else if (ulNum == 9)
    {
        ulRowNum = 3;
    }
    else if (ulNum == 16)
    {
        ulRowNum = 4;
    }
    else
    {
        gbVideoWin->Invalidate();
        gbVideoWin->Update();
        return;
    }

    ulColNum = ulRowNum;
    iWinWidth = iWidth/ulColNum;
    iWinHeight = iHeight/ulRowNum;
    iLastWinWidth = iWidth - iWinWidth*ulColNum;
    iLastWinHeight = iHeight - iWinHeight*ulRowNum;

    for (i=0; i<ulRowNum; i++)
    {
        for (j=0; j<ulColNum; j++)
        {
            pVideoWin = g_vVideoWin[i*ulColNum+j];
            pVideoWin->Visible = true;
            pVideoWin->Align = alNone;
            pVideoWin->Left = j*iWinWidth;
            pVideoWin->Top = i*iWinHeight;

            pVideoWin->Width = iWinWidth;
            pVideoWin->Height = iWinHeight;

            if ((int)j < iLastWinWidth)
            {
                pVideoWin->Width++;
            }

            if ((int)i < iLastWinHeight)
            {
                pVideoWin->Height++;
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::FullScreenVideoWin(TFormVideoWin *pWin)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::UpdateUser(EVENT_USER_STATUS_T &stEvent)
{
    if (m_mUserNode.find(stEvent.ulUserNumber) == m_mUserNode.end())
    {
        return;
    }

    TcxTreeListNode *Node = m_mUserNode[stEvent.ulUserNumber];

    Node->Values[3] = stEvent.ulStatus;
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::tlUserGetNodeImageIndex(TcxCustomTreeList *Sender,
          TcxTreeListNode *ANode, TcxTreeListImageIndexType AIndexType,
          TImageIndex &AIndex)
{
    if (ANode->Values[2].IsNull() ||
        ANode->Values[3].IsNull() )
    {
        AIndex = 0;
        return;
    }

    UINT32  ulType = (UINT32)ANode->Values[2];
    BOOL    bOnline = User::IsUserOnline((UINT32)ANode->Values[3]);

    if (ulType == LARGE_PORTAL_UE )
    {
        AIndex = bOnline?1:2;
    }
    else if (ulType == SMALL_PORTAL_UE )
    {
        AIndex = bOnline?3:4;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::pmiExitFullScreenClick(TObject *Sender)
{
    ExitFullScreen();
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::UpdateP2PVideoCallStatus(EVENT_P2PVIDEO_STATUS_T &stEvent)
{
    GosLog(LOG_INFO, "video call status: caller=%u, callee=%u, status=%u",
            stEvent.ulCaller, stEvent.ulCallee, stEvent.ulStatus);

    if (stEvent.ulStatus == P2PVIDEO_IND_STATUS_PICKEDUP)   // 调度台为被叫
    {
        TFormVideoWin   *pWin = GetVideoWin(stEvent.ulCaller);

        if (pWin)
        {
            pWin->Play();

            LocalRec::StartP2PCallRecord(stEvent.ulCaller, REC_TYPE_VIDEO, REC_CALL_TYPE_VIDEO);
        }
    }
    else if (stEvent.ulStatus == P2PVIDEO_IND_STATUS_ANSWERED)  // 调度台为主叫
    {
        TFormVideoWin   *pWin = GetVideoWin(stEvent.ulCallee);

        if (pWin)
        {
            LocalRec::StartP2PCallRecord(stEvent.ulCallee, REC_TYPE_VIDEO, REC_CALL_TYPE_VIDEO);
        }
    }
    else if (stEvent.ulStatus == P2PVIDEO_IND_STATUS_HANGUPED               || // 被对端挂断
             stEvent.ulStatus == P2PVIDEO_IND_STATUS_HANGUPED_ACTIVE        || // 实时视频被调度台主动挂断。
             stEvent.ulStatus == P2PVIDEO_IND_STATUS_CANCELLED              || // 实时视频请求被取消
             stEvent.ulStatus == P2PVIDEO_IND_STATUS_RELEASED_BUSY          || // 对端忙
             stEvent.ulStatus == P2PVIDEO_IND_STATUS_REMOTE_NOTCONNECTED    || // 对端未注册
             stEvent.ulStatus == P2PVIDEO_IND_STATUS_REMOTE_NOTFOUND        || // 未找到对端资源。
             stEvent.ulStatus == P2PVIDEO_IND_STATUS_RELEASED_GENERAL_ERROR )  // 发生普通错误
    {
        TFormVideoWin   *pWin = GetVideoWin(stEvent.ulCaller, stEvent.ulCallee);

        if (pWin)
        {
            pWin->Stop();
        }
        else
        {
            GosLog(LOG_ERROR, "invalid video caller(%u) or callee(%u)", stEvent.ulCaller, stEvent.ulCallee);
        }

        if (stEvent.ulCaller == APP::GetCurrDCUserID())
        {
            P2P::CallOver(stEvent.ulCallee);
        }
        else if (stEvent.ulCallee == APP::GetCurrDCUserID())
        {
            P2P::CallOver(stEvent.ulCaller);
        }

        FormCallIn->StopP2PVideoCall(stEvent.ulCaller);
        LocalRec::StopP2PCallRecord(stEvent.ulCaller);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::gbVideoWinResize(TObject *Sender)
{
    ResizeVideoWin();
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::gbVideoDblClick(TObject *Sender)
{
    if (gbVideo->Parent == MainPanel)
    {
        return;
    }

    FormMain->ActivePage(PAGE_VIDEO);

    gbVideo->Invalidate();
    gbVideo->Update();
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::TimerCheckTimer(TObject *Sender)
{
    UINT32  ulPlayingNum = 0;
    UINT32  ulDelStartIndex = 0xffffffff;
    UINT32  i;

    btnPTT->Enabled = (GetSelectedGroupCallVideoWin() != NULL);

    TimerCheck->Interval = 1000;

    VECTOR<TFormVideoWin*>  vPlayingWin;
    VECTOR<TFormVideoWin*>  vIdleWin;

    for (i=0; i<g_vVideoWin.size(); i++)
    {
        if (g_vVideoWin[i]->IsPlaying())
        {
            vPlayingWin.push_back(g_vVideoWin[i]);
            ulPlayingNum++;
        }
        else
        {
            vIdleWin.push_back(g_vVideoWin[i]);
        }
    }

    if (ulPlayingNum == 0)
    {
        ulDelStartIndex = 0;
    }
    else if (ulPlayingNum == 1)
    {
        ulDelStartIndex = 1;
    }
    else if (ulPlayingNum <= 4)
    {
        ulDelStartIndex = 4;
    }
    else if (ulPlayingNum <= 9)
    {
        ulDelStartIndex = 9;
    }

    if (ulDelStartIndex >= g_vVideoWin.size())
    {
        return;
    }

    g_vVideoWin.swap(vPlayingWin);
    for (i=0; i<vIdleWin.size(); i++)
    {
        g_vVideoWin.push_back(vIdleWin[i]);
    }

    for (int j=g_vVideoWin.size()-1; j>=(int)ulDelStartIndex; j--)
    {
        TFormVideoWin *pDeleteVideoWin = g_vVideoWin[j];

        delete pDeleteVideoWin;

        g_vVideoWin.erase(g_vVideoWin.begin()+j);
    }

    ResizeVideoWin();
}
//----------------------------------------------------------------------------
void __fastcall TFormVideoCall::pmiStopVideoClick(TObject *Sender)
{
    UINT32  ulUserID = GetSelectUser();

    if (ulUserID == INVALID_USER_ID)
    {
        return;
    }

    TFormVideoWin   *pWin = GetVideoWin(ulUserID, ulUserID);

    if (!pWin)
    {
        return;
    }

    pWin->Stop();
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::SaveP2PVideoCallInfo(UINT32 ulCallType, UINT32 ulCaller, UINT32 ulCallee, UINT32 ulStatus)
{
    ::SaveP2PVideoCallInfo(ulCallType, ulCaller, ulCallee, ulStatus);
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::FormCreate(TObject *Sender)
{
    SetImage(pmiVideoRealPlay, IMG_RECV);
    SetImage(pmiVideoCall, IMG_P2P_CALL);
    SetImage(pmiStopVideo, IMG_STOP);
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::pmiShowAllUserClick(TObject *Sender)
{
    InitUser();
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::pmiSetSubUserClick(TObject *Sender)
{
    DialogSubUser->Init(g_sSubVideoUser, TRUE);
    if (DialogSubUser->ShowModal() != mrOk)
    {
        return;
    }

    UINT32  ulValue;
    IntSet  sOld;
    IntSet  sNew;
    VECTOR<UINT32>  vUser;

    sOld.Init(g_sSubVideoUser);

    DialogSubUser->Get(sNew);
    sNew.GetList(vUser);

    // 去除相同的部分
    for (UINT32 i=0; i<vUser.size(); i++)
    {
        ulValue = vUser[i];
        if (sOld.Del(ulValue))
        {
            sNew.Del(ulValue);
        }
    }

    if (!SaveSubVideoUser(sOld, sNew))
    {
        InfoMsg("操作失败");
        GosLog(LOG_ERROR, "save sub video user failed");
        return;
    }

    InfoMsg("操作成功");

    DialogSubUser->Get(g_sSubVideoUser);

    InitUser();
}
//---------------------------------------------------------------------------

void __fastcall TFormVideoCall::pmiRefreshClick(TObject *Sender)
{
    if (pcMain->ActivePage == tsUser)
    {
        LoadSubVideoUserInfo();
        if (LoadPortalUEInfo())
        {
            InitUser();
        }
    }
    else
    {
        LoadSubVideoGroupInfo();
        // TODO 加载群组（包括FuncType)
        InitGroup();
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormVideoCall::ColumnNameGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[1].IsNull() )
    {
        return;
    }

    UINT32  ulUserID = (UINT32)ANode->Values[1];

    Value = User::GetUserDisplayName(ulUserID);
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::pcMainPageChanging(TObject *Sender, TcxTabSheet *NewPage,
          bool &AllowChange)
{
    SetButton();
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::SetButton()
{
    if (pcMain->ActivePage == tsUser)
    {
        btnVideoRealPlay->Enabled = true;
        btnVideoP2PCall->Enabled = true;
        btnPTT->Enabled = false;

        pmiVideoRealPlay->Enabled = true;
        pmiStopVideo->Enabled = true;
        pmiShowAllUser->Enabled = true;
        pmiSetSubUser->Enabled = true;
        pmiShowAllGroup->Enabled = false;
        pmiSetSubGroup->Enabled = false;
    }
    else
    {
        btnVideoRealPlay->Enabled = false;
        btnVideoP2PCall->Enabled = false;
        btnPTT->Enabled = true;

        pmiVideoRealPlay->Enabled = false;
        pmiStopVideo->Enabled = false;
        pmiShowAllUser->Enabled = false;
        pmiSetSubUser->Enabled = false;
        pmiShowAllGroup->Enabled = true;
        pmiSetSubGroup->Enabled = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::pcMainChange(TObject *Sender)
{
    SetButton();
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::pmiShowAllGroupClick(TObject *Sender)
{
    InitGroup();
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::pmiSetSubGroupClick(TObject *Sender)
{
    DialogSubGroup->Init(g_sSubVideoGroup);
    if (DialogSubGroup->ShowModal() != mrOk)
    {
        return;
    }

    Int64Set        sOld;
    Int64Set        sNew;

    sOld.Init(g_sSubVideoGroup);

    DialogSubGroup->Get(sNew);

    RemoveCommonMember(sOld, sNew);

    if (!SaveSubVideoGroup(sOld, sNew))
    {
        InfoMsg("操作失败");
        GosLog(LOG_ERROR, "save sub video group failed");
        return;
    }

    InfoMsg("操作成功");

    DialogSubGroup->Get(g_sSubVideoGroup);

    InitGroup();
}
//---------------------------------------------------------------------------

void __fastcall TFormVideoCall::TimerInitTimer(TObject *Sender)
{
    TimerInit->Enabled = false;

    pmiRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::FormShow(TObject *Sender)
{
    btnPTT->Font->Size = UICfg::GetButtonPTTFontSize();
    btnPTT->Font->Name = UICfg::GetButtonPTTFontName();
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::gbWorkPanelResize(TObject *Sender)
{
    btnPTT->Top = btnPTT->Parent->Height - btnPTT->Height - 60;
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::tlGroupGetNodeImageIndex(TcxCustomTreeList *Sender,
          TcxTreeListNode *ANode, TcxTreeListImageIndexType AIndexType,
          TImageIndex &AIndex)
{
    AIndex = 0;

    if (ANode->Parent == NULL ||
        ANode->Parent->Values[0].IsNull())
    {
        return;
    }

    AnsiString  strName = ANode->Parent->Values[0];

    if (strName == "列车组")
    {
        AIndex = 1;
    }
    else if (strName == "车站组")
    {
        AIndex = 2;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::btnPTTMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    OnPTTDial();
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::btnPTTMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    OnPTTRelease();
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::btnPTTMouseEnter(TObject *Sender)
{
    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::btnPTTMouseLeave(TObject *Sender)
{
    btnPTT->LookAndFeel->SkinName = GetDefaultSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::OnPTTDial()
{
    TFormVideoWin   *FrmVideoWin = GetSelectedGroupCallVideoWin();

    if (!FrmVideoWin)
    {
        return;
    }

    INT64   i64GroupID = FrmVideoWin->GetGroupID();

    if (PTT::Dial(i64GroupID))
    {
        btnPTT->LookAndFeel->SkinName = UICfg::GetPTTDownSkinName();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::OnPTTRelease()
{
    PTT::Release();
    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::btnVideoGroupCallClick(TObject *Sender)
{
    INT64   i64GroupID = GetSelectGroup();

    if (i64GroupID == INVALID_GROUP_ID)
    {
        return;
    }

    if (!StartVideoGroupCall(i64GroupID))
    {
        GosLog(LOG_ERROR, "video group call to "FORMAT_I64 " failed", i64GroupID);
        return;
    }
}
//---------------------------------------------------------------------------
INT64 __fastcall TFormVideoCall::GetSelectGroup()
{
    if (tlGroup->SelectionCount == 0)
    {
        return INVALID_USER_ID;
    }

    TcxTreeListNode *Node = tlGroup->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return INVALID_GROUP_ID;
    }

    return (INT64)Node->Values[1];
}
//---------------------------------------------------------------------------
bool __fastcall TFormVideoCall::StartVideoGroupCall(INT64 i64GroupID)
{
    UINT32          ulUserID = INVALID_USER_ID;
    UINT32          ulVideoFormat;
    UINT32          ulCameraType;
    BOOL            bNeedConfirm;
    BOOL            bHasVoice;
    TFormVideoWin   *pNewVideoWin;

    if (!GetVideoCfg(ulUserID, ulVideoFormat, ulCameraType, bNeedConfirm, bHasVoice))
    {
        return false;
    }

    pNewVideoWin = GetIdleVideoWin(ulUserID);
    if (!pNewVideoWin)
    {
        InfoMsg("视频窗口已达最大值，\n请先关闭部分视频窗口。");

        return false;
    }

    ResizeVideoWin();

    pNewVideoWin->InitVideoGroupCall(i64GroupID, ulVideoFormat, ulCameraType, bNeedConfirm, bHasVoice);

    return true;
  //  pNewVideoWin->Play(ulUserID, ulVideoFormat, ulCameraType, bNeedConfirm, bHasVoice);
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::tlUserBeginDragNode(TcxCustomTreeList *Sender, TcxTreeListNode *ANode,
          bool &Allow)
{
    if (ANode->Values[1].IsNull())
    {
        Allow = false;
        m_ulDragUserID = INVALID_USER_ID;
    }
    else
    {
        Allow = true;
        m_ulDragUserID = ANode->Values[1];
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::tlUserEndDrag(TObject *Sender, TObject *Target, int X,
          int Y)
{
    //   m_ulDragUserID = INVLAID_USER_ID;
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoCall::pmiVideoCallClick(TObject *Sender)
{
    UINT32  ulUserID = GetSelectUser();

    if (ulUserID == INVALID_USER_ID)
    {
        return;
    }

    if (!StartVideoCall(ulUserID, TRUE))
    {
        GosLog(LOG_INFO, "video call to %u failed", ulUserID);
        InfoMsg("视频个呼失败");
        /*
        if (!eSDK_IsCameraValid())
        {
            InfoMsg("无法打开摄像头");
            return;
        } */
    }
}
//---------------------------------------------------------------------------



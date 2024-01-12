//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "eLTE_Types.h"
#include "eLTE_SDK.h"
#include "ThreadRTPPlay.h"
#include "FrmTest.h"
#include "main.h"
#include "DlgSetSkin.h"
#include "DlgRealBrd.h"
#include "PcmPlay.h"
#include "ffmpeg_rec.h"
#include "AudioVolume.h"
#include "AudioDev.h"
#include "ThreadPcmPlay.h"
#include "FrmCallIn.h"
#include "FrmGroup.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxMemo"
#pragma link "cxTextEdit"
#pragma link "dxSkinBlue"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "MDIForm"
#pragma link "cxPC"
#pragma link "dxSkinscxPCPainter"
#pragma link "cxLabel"
#pragma link "cxCheckBox"
#pragma link "dxBar"
#pragma link "cxClasses"
#pragma link "dxBarBuiltInMenu"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxPropertiesStore"
#pragma link "cxDropDownEdit"
#pragma link "cxMaskEdit"
#pragma link "cxSplitter"
#pragma link "cxSpinEdit"
#pragma link "cxImage"
#pragma link "dxGDIPlusClasses"
#pragma link "cxCustomData"
#pragma link "cxImageComboBox"
#pragma link "cxInplaceContainer"
#pragma link "cxStyles"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "cxButtonEdit"
#pragma link "cxGroupBox"
#pragma resource "*.dfm"
TFormTest *FormTest;

UINT32  g_ulMaxLogLine = 1000;

extern VOID GetMicList(VECTOR<AnsiString> &vList);

static ThreadRTPPlay        *m_ThreadRTPPlay;
UINT32  g_ulThreadRTPStatus = SDL_PLAY_START;

static ThreadPcmPlay   *g_ThreadPcmPlay = NULL;

UINT32  m_ulSampleFreq = 16000;
UINT32  m_ulPCMFrameLen = 2;
UINT32  m_ulSendFrameNumPer100MillSecond = 100*m_ulSampleFreq/1000;
UINT8   *m_pucPCMData = NULL;
UINT32  m_ulCurrFrameIndex = 0;
UINT32  m_ulPCMFrameNum = 0;
UINT32  m_ulWavFileHdrLen = 78;

extern BOOL eSDK_SubJoinGroup(INT64 i64GroupID);

BOOL LoadPCMData(CHAR *szFile)
{
    FILE    *fp;
    INT32   iSize;

    if (m_pucPCMData)
    {
        return TRUE;
    }

    iSize = gos_get_file_size(szFile);
    if (iSize <= (int)m_ulWavFileHdrLen)
    {
        return FALSE;
    }

    iSize -= m_ulWavFileHdrLen;
    m_ulPCMFrameNum = iSize/m_ulPCMFrameLen;
    m_pucPCMData = (UINT8*)calloc(1, iSize);
    if (!m_pucPCMData)
    {
        fclose(fp);
        return FALSE;
    }

    fp = fopen(szFile, "rb");
    if (!fp)
    {
        GOS_FREE(m_pucPCMData);
        return FALSE;
    }

    fseek(fp, SEEK_SET, m_ulWavFileHdrLen);
    if (iSize != (int)fread(m_pucPCMData, 1, iSize, fp))
    {
        fclose(fp);
        GOS_FREE(m_pucPCMData);
        return FALSE;
    }

    fclose(fp);
    return TRUE;
}

UINT32 __fastcall TFormTest::GetUserID()
{
    INT32 iNumber = edtUser->Text.ToIntDef(-1);

    if (iNumber <= 0)
    {
        return INVALID_USER_ID;
    }

    return iNumber;
}

INT64 __fastcall TFormTest::GetGroupID()
{
    INT64 i64GroupID = INVALID_GROUP_ID;

    if (!ToInt64(edtGroup->Text, &i64GroupID))
    {
        return INVALID_GROUP_ID;
    }

    return i64GroupID;
}

void __fastcall TFormTest::OnGetESDKEvent(UINT8 *pucMsg, UINT32 ulMsgLen)
{
    ESDK_EVENT_T    *pstEvent = (ESDK_EVENT_T*)pucMsg;

    eSDKEventCallBack(pstEvent->iEventType, pstEvent->acMsg, pstEvent->ulMsgLen);
}

void TFormTest::eSDKEventCallBack(INT32 iEventType, char *szEvent, UINT32 ulSize)
{
    char    *szPos;

    if (iEventType == ELTE_Event_NotifyUserStatus)      // 设备状态变化
    {
    }
    else if (iEventType == ELTE_Event_NotifyP2pvideocallStatus)     // 实时视频浏览
    {
    }
    else if (iEventType == ELTE_Event_NotifyResourceStatus)         // 资源状态变化
    {
    }
    else if (iEventType == ELTE_Event_NotifyProvisionChange)        // 设备属性配置变更通知
    {
    }
    else if (iEventType == ELTE_Event_NotifyProvisionAllResync)     // 自动下载配置数据通知
    {
    }
    else if (iEventType == ELTE_Event_NotifyP2pcallStatus)          // 点呼状态变化
    {
    }
    else if (iEventType == ELTE_Event_NotifyGroupStatus)            // 组呼状态变化
    {
    }
    else if (iEventType == ELTE_Event_NotifyModuleStatus)           // 模块组件状态
    {
    }
    else if (iEventType == ELTE_Event_NotifyGISReport)  // GIS位置信息通知
    {
    }
    else if (iEventType == ELTE_Event_NotifySDSReport)              // 短信/彩信接收上报
    {
        EVENT_SDS_REPORT_T      stEvent = {0};

        if (!ParseEventSDSReport(szEvent, &stEvent))
        {
            GosLog(LOG_ERROR, "parse NotifySDSReport failed: %s", szEvent);
            return;
        }
    }
    else if (iEventType == ELTE_Event_NotifySDSSendStatus)          // 短信/彩信发送状态通知
    {
    }
    else if (iEventType == ELTE_Event_NotifyServiceStatus)          // 本地服务停止通知
    {
    }
}
//---------------------------------------------------------------------------
__fastcall TFormTest::TFormTest(TComponent *Owner) : TSubForm(Owner)
{
    InitMsgHandler(EV_DC_DIS_RSP, MSG_SAGetRecBrdCfg, OnDCGetRecBrdCfgRsp);

    InitMsgHandler(EV_ESDK_MSG, OnGetESDKEvent);
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::OnDCGetRecBrdCfgRsp(UINT32 ulSeqID, GJson &Json)
{
    mmInfo->Text = Json.GetText();
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton43Click(TObject *Sender)
{
    UINT32 ulUserID = GetUserID();
    UINT32 ulVideoFormat = VIDEO_FMT_CIF;
    UINT32 ulCameraType = CAMERA_TYPE_FRONT;
    BOOL bNeedConfirm = FALSE;
    BOOL bHasVoice = TRUE;

    P2P::PullVideo(ulUserID, ulVideoFormat, ulCameraType, bNeedConfirm, bHasVoice);

    eSDK_SetVideoPlayWindow(ulUserID, Panel7->Handle);
}

//---------------------------------------------------------------------------
void __fastcall TFormTest::btnGetGroupClick(TObject *Sender)
{
    std::vector<DC_GROUP_INFO_T>    vGroup;
    char        *szInfo = NULL;

    BOOL bRet = eSDK_GetDcGroups(vGroup);

    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        char    acInfo[128];

        sprintf(acInfo, FORMAT_I64 " %s %u", vGroup[i].i64GroupID, vGroup[i].acGroupName, vGroup[i].ulGroupType);
        Info(acInfo);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnP2PDialClick(TObject *Sender)
{
    if (!P2P::Dial(GetUserID()))
    {
        InfoMsg("点呼失败!");
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnP2PHangupClick(TObject *Sender)
{
    if (!P2P::Hangup())
    {
        InfoMsg("点呼挂断失败!");
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnP2PRecvClick(TObject *Sender)
{
    if (!P2P::Recv(GetUserID()))
    {
        InfoMsg("点呼接听失败!");
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnP2PTransferClick(TObject *Sender)
{
    INT32   iObjectID = edtTransferUser->Text.ToIntDef(-1);

    if (iObjectID <= 0)
    {
        edtTransferUser->Text = "";
        return;
    }

    if (!eSDK_P2PTransfer(GetUserID(), iObjectID))
    {
        InfoMsg("点呼转接失败!");
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnP2PRejectClick(TObject *Sender)
{
    if (!eSDK_P2PReject(GetUserID()))
    {
        InfoMsg("点呼拒绝失败!");
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnGetUserClick(TObject *Sender)
{
    std::vector<DC_INFO_T>  vDcUser;

    eSDK_GetDcUsers(vDcUser);

    for (UINT32 i=0; i <vDcUser.size(); i++)
    {
        char    acInfo[1024];

        sprintf(acInfo, "UserID=%u, Name=%s, Pri=%u, Ctgy=%u",
            vDcUser[i].ulUserID,
            vDcUser[i].acUserName,
            vDcUser[i].ulUserPriority,
            vDcUser[i].ulUserCategory );

        Info(acInfo);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton44Click(TObject *Sender)
{
    P2P::StopVideo(GetUserID());
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnGeMICClick(TObject *Sender)
{
    std::vector<AnsiString>     vList;

    eSDK_GetSpeakerList(vList);
    for (UINT32 i=0; i < vList.size(); i++)
    {
        mmInfo->Lines->Append(vList[i]);
    }

    eSDK_GetMicList(vList);
    for (UINT32 i=0; i < vList.size(); i++)
    {
        mmInfo->Lines->Append(vList[i]);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnCreatePatchGroupClick(TObject *Sender)
{
    char acName[128];

    sprintf(acName, "test_patch_%u", GetGroupID());

    eSDK_CreatePatchGroup(acName);
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnGetPatchGroupClick(TObject *Sender)
{
    VectorMap<INT64, SDK_PATCH_GROUP_INFO_T> vPatchGroup;
    CHAR        acInfo[128];

    eSDK_GetPatchGroups(vPatchGroup);

    for (UINT32 i=0; i<vPatchGroup.Size(); i++)
    {
        sprintf(acInfo, FORMAT_I64" %u %s", vPatchGroup[i]->i64GroupID, vPatchGroup[i]->ulDCUserID, vPatchGroup[i]->acGroupName);
        mmInfo->Lines->Append(acInfo);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnGetGroupsClick(TObject *Sender)
{
    std::vector<DC_GROUP_INFO_T>    vGroup;

    BOOL bRet = eSDK_GetDcGroups(9, vGroup);

    if (!bRet)
    {
        InfoMsg("查询失败");
        return;
    }

    mmInfo->Lines->BeginUpdate();
    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        UINT32  ulCreator = eSDK_GetGroupCreator(vGroup[i].i64GroupID);

        mmInfo->Lines->Append(AnsiString(vGroup[i].i64GroupID) + " - " + AnsiString(vGroup[i].acGroupName) +
                              " - " + AnsiString(ulCreator));
    }

    mmInfo->Lines->EndUpdate();
}

BOOL IsUsersSame(VECTOR<UINT32> &vUserInfo, VECTOR<UINT32> &vUser)
{
    if (vUser.size() != vUserInfo.size())
    {
        return FALSE;
    }

    for (UINT32 i=0; i<vUser.size(); i++)
    {
        BOOL    bExist = FALSE;

        for (UINT32 j=0; j<vUserInfo.size(); j++)
        {
            if (vUserInfo[j] == vUser[i])
            {
                bExist = TRUE;
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

//---------------------------------------------------------------------------
void __fastcall TFormTest::btnCancelDynamicGroupClick(TObject *Sender)
{
    INT64   i64GroupID = GetGroupID();

    if (eSDK_CancelDynamicGroup(i64GroupID))
    {
        InfoMsg("删除动态组成功");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton9Click(TObject *Sender)
{
    CHAR    acServerIP[32];

    StrCpy(acServerIP, edtMRSAddr->Text);
    INT32 iRet = ELTE_SDK_ProvisionManagerInitMRS(acServerIP);
    if(0 == iRet)
    {
        InfoMsg("初始化录音录像服务器成功");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton10Click(TObject *Sender)
{
    int     iCallType = edtCallType->Text.ToIntDef(-1);
    int     iCaller = edtCaller->Text.ToIntDef(-1);
    int     iCallee = edtCallee->Text.ToIntDef(-1);
    int     iResourceID = edtResourceID->Text.ToIntDef(-1);
    CHAR    acStartTime[32];
    CHAR    acEndTime[32];
    CHAR    acMsg[1024];

    StrCpy(acStartTime, edtRecStartTime->Text);
    StrCpy(acEndTime, edtRecEndTime->Text);

    sprintf(acMsg, "<Content>"
            "<RECQueryInfo>"
            "<CallType>%d</CallType>"
            "<Caller>%d</Caller>"
            "<Callee>%d</Callee>"
            "<ResourceID>%d</ResourceID>"
            "<StartSec>%s</StartSec>"
            "<EndSec>%s</EndSec>"
            "</RECQueryInfo>"
            "</Content>", iCallType, iCaller, iCallee, iResourceID, acStartTime, acEndTime);
/*
            "<StartSec>2019-12-18 00:00:00</StartSec>"
            "<EndSec>2019-12-19 18:00:00</EndSec>"
  */

    CHAR    *szResult = NULL;
    INT32   iRet = ELTE_SDK_GetUserRECFileInfoList(acMsg, &szResult);

    if(0 == iRet)
    {
        Log("---查询---", TRUE);
        Log(acMsg, TRUE);
        Log("---结果---", TRUE);
        Log(Utf8ToAnsi(szResult), TRUE);
        ELTE_SDK_ReleaseBuffer(szResult);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnSubGroupClick(TObject *Sender)
{
    if (!eSDK_SubGroup(GetGroupID()))
    {
        InfoMsg("监听组失败!");
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnUnsubGroupClick(TObject *Sender)
{
    if (!eSDK_UnSubGroup(GetGroupID()))
    {
        InfoMsg("去订阅组失败!");
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnJoinGroupClick(TObject *Sender)
{
    if (!eSDK_SubJoinGroup(GetGroupID()))
    {
        InfoMsg("加入组失败!");
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnPTTClick(TObject *Sender)
{
    if (!PTT::Dial(GetGroupID()))
    {
        InfoMsg("组呼（抢权）失败!");
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnPTTReleaseClick(TObject *Sender)
{
    if (!PTT::Release())
    {
        InfoMsg("释放话权失败!");
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnGroupBreakoffClick(TObject *Sender)
{
    if (!eSDK_GroupBreakoff(GetGroupID()))
    {
        InfoMsg("强拆组呼失败!");
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnPTTHangupClick(TObject *Sender)
{
    if (!eSDK_PTTHangup(GetGroupID()))
    {
        InfoMsg("退出组呼失败!");
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnStartGroupDiscreetListenClick(TObject *Sender)
{
    if (!eSDK_StartGroupDiscreetListen(GetGroupID()))
    {
        InfoMsg("开始缜密侦听失败!");
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnStopGroupDiscreetListenClick(TObject *Sender)
{
    if (!eSDK_StopGroupDiscreetListen(GetGroupID()))
    {
        InfoMsg("结束缜密侦听失败!");
        return;
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::btnStartUserDiscreetListenClick(TObject *Sender)
{
    if (!eSDK_StartUserDiscreetListen(GetUserID()))
    {
        InfoMsg("开始缜密侦听失败!");
        return;
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::btnStopUserDiscreetListenClick(TObject *Sender)
{
    if (!eSDK_StopUserDiscreetListen(GetUserID()))
    {
        InfoMsg("结束缜密侦听失败!");
        return;
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::btnPTTEmergencyClick(TObject *Sender)
{
    if (!eSDK_PTTEmergency(GetGroupID()))
    {
        InfoMsg("发起紧急组呼失败!");
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnLoginClick(TObject *Sender)
{
/*
    StrCpy(g_stLocalCfg.acDCUserName, edtLoginUser->Text);
    StrCpy(g_stLocalCfg.acDCUserPwd, edtLoginPwd->Text);
           */
    if (!APP::Login(this->Handle))
    {
        InfoMsg("登录失败");
        return;
    }

    InfoMsg("登录成功");

//  FormMain->btnLoginClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cbTriggerEventReportPropertiesChange(TObject *Sender)
{
    int     iStatus = cbTriggerEventReport->Checked;

    ELTE_SDK_TriggerStatusReport(iStatus);
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::Log(AnsiString strLog, BOOL bForceShow)
{
    if (bForceShow == FALSE && !cbAllowLog->Checked)
    {
        return;
    }

    mmSDKEvent->Lines->Append(strLog);

    while(mmSDKEvent->Lines->Count > (int)g_ulMaxLogLine)
    {
        mmSDKEvent->Lines->Delete(0);
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormTest::Info(AnsiString strInfo)
{
    mmInfo->Lines->Append(strInfo);
}

void __fastcall TFormTest::btnStartPreviewClick(TObject *Sender)
{
    HWND hWndPlayer = PanelRTP->Handle;

    if (m_ThreadRTPPlay)
    {
        m_ThreadRTPPlay->Stop();
    }

    m_ThreadRTPPlay = new ThreadRTPPlay(FALSE, hWndPlayer, &g_ulThreadRTPStatus);

    AnsiString strURL = edtRTPUrl->Text;

    m_ThreadRTPPlay->SetURL(strURL.c_str());

    m_ThreadRTPPlay->Start();
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnStopPreviewClick(TObject *Sender)
{
    if (!m_ThreadRTPPlay)
    {
        return;
    }

    m_ThreadRTPPlay->Stop();
    m_ThreadRTPPlay = NULL;

    PanelRTP->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnPlayPCMClick(TObject *Sender)
{
    UINT32 ulChanNum = 1;
    UINT32 ulSampleRate = 16000;
    UINT32 ulBitsPerSample = 16;

    HANDLE hPlay = PCMPlayInit(this->Handle, ulChanNum, ulSampleRate, ulBitsPerSample);

    if (!hPlay)
    {
        return;
    }

    FILE    *fp;
    UINT32  ulInterval = 500;
    UINT32  ulBufLen = (ulChanNum*ulSampleRate*ulBitsPerSample/8)*ulInterval/1000;
    char    *frame_buf = (char*)malloc(ulBufLen);

    AnsiString  strFile = edtWavFile->Text;

    fp = fopen(strFile.c_str(), "rb");
    fseek(fp, 78, SEEK_SET);

    PCMPlayStart(hPlay);
    //for (UINT32 i=0; i<100000; i++)
    while(1)
    {
        int iReadSize = fread(frame_buf, 1, ulBufLen, fp);
        if (iReadSize == (int)ulBufLen)
        {
            PCMPlayPushData(hPlay, frame_buf, iReadSize);
        }
        else
        {
            break;

           // fseek(fp, 0, SEEK_SET);
           // continue;
        }

        gos_sleep_1ms(450);
        //Sleep(ulInterval);
    }

    free(frame_buf);
    fclose(fp);

    PCMPlayStop(hPlay);
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnGetPatchMemberClick(TObject *Sender)
{
    VECTOR<INT64>   vMember;

    INT32 iRet = eSDK_GetPatchGroupMembers(GetGroupID(), vMember);

    if (iRet == FALSE)
    {
        GosLog(LOG_ERROR, "查找派接组(%lld)成员失败!", GetGroupID());
        ErrMsg("查找派接组(%lld)成员失败!", GetGroupID());
        return;
    }

    for (UINT32 i=0; i<vMember.size(); i++)
    {
        mmInfo->Lines->Append(vMember[i]);
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton3Click(TObject *Sender)
{
    UINT32  ulNumber = GetUserID();

    BOOL    bRet = P2P::RecvVideo(ulNumber);

    bRet = eSDK_SetVideoPlayWindow(ulNumber, Panel7->Handle);
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton5Click(TObject *Sender)
{
    UINT32  ulNumber = 4071;
    BOOL    bRet = eSDK_SetVideoPlayWindow(ulNumber, Panel7->Handle);
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton6Click(TObject *Sender)
{
    Panel7->BringToFront();
    eSDK_SetVideoPlayWindow(GetUserID(), Panel7->Handle);
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton7Click(TObject *Sender)
{
    eSDK_SetVideoPlayWindow(GetUserID(), Panel2->Handle);
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnClearLogClick(TObject *Sender)
{
    mmSDKEvent->Text = "";
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnAddDynamicGroupMemberClick(TObject *Sender)
{
    INT64   i64GroupID = GetGroupID();
    VECTOR<UINT32>  vUser;

    UINT32  ulUserID = edtDnaUser->Text.ToIntDef(0);

    if (0 == ulUserID)
    {
        return;
    }

    vUser.push_back(ulUserID);
    if (!eSDK_AddDynamicGroupUser(i64GroupID, vUser))
    {
        InfoMsg("添加组成员失败");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnDelDynamicGroupMemberClick(TObject *Sender)
{
    INT64   i64GroupID = GetGroupID();
    VECTOR<UINT32>  vUser;

    UINT32  ulUserID = edtDnaUser->Text.ToIntDef(0);

    if (0 == ulUserID)
    {
        return;
    }

    vUser.push_back(ulUserID);
    if (!eSDK_DelDynamicGroupUser(i64GroupID, vUser))
    {
        InfoMsg("删除组成员失败");
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::cxButton15Click(TObject *Sender)
{
    INT64   i64GroupID = GetGroupID();

    if (eSDK_CancelMultiGroup(i64GroupID))
    {
        InfoMsg("删除动态组成功");
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::cxButton14Click(TObject *Sender)
{
    INT64   i64GroupID = GetGroupID();
    VECTOR<INT64>   vGroup;

    if (!ToInt64(edtMultiGroup1->Text, &i64GroupID))
    {
        InfoMsg("多选组成员1非法");
        return;
    }

    vGroup.push_back(i64GroupID);

    if (!ToInt64(edtMultiGroup2->Text, &i64GroupID))
    {
        InfoMsg("多选组成员2非法");
        return;
    }

    vGroup.push_back(i64GroupID);

    i64GroupID = GetGroupID();
    if (eSDK_CreateMultiGroup(i64GroupID, "MultiGroup1", vGroup))
    {
        InfoMsg("创建多选组成功");
    }
    else
    {
        InfoMsg("创建多选组失败");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnGetGroupMemberClick(TObject *Sender)
{
    VECTOR<UINT32>  vUser;
    VECTOR<INT64>   vGroup;

    eSDK_GetGroupMember(GetGroupID(), vUser, vGroup);

    AnsiString  strCreator = eSDK_GetGroupCreator(GetGroupID());

    Info("创建者：" + strCreator);
    for (UINT32 i=0; i<vUser.size(); i++)
    {
        Info(vUser[i]);
    }

    if (vGroup.size() > 0)
    {
        Info("组：");
        for (UINT32 i=0; i<vGroup.size(); i++)
        {
            Info(vGroup[i]);
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::cxButton17Click(TObject *Sender)
{
    AnsiString  strVer = eSDK_GetVer();

    mmInfo->Lines->Append("eSDK Ver = " + strVer);
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton19Click(TObject *Sender)
{
    INT64   iGroupID = GetGroupID();

    BOOL bRet = eSDK_StartRecord(iGroupID, REC_TYPE_AUDIO, REC_CALL_TYPE_PTT);

    if (!bRet)
    {
        InfoMsg("开始录音失败");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton20Click(TObject *Sender)
{
    INT64   iGroupID = GetGroupID();

    BOOL bRet = eSDK_StopRecord(iGroupID, REC_TYPE_AUDIO, REC_CALL_TYPE_PTT);

    if (!bRet)
    {
        InfoMsg("结束录音失败");
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::btnCreateDynamicGroupClick(TObject *Sender)
{
    INT64   i64GroupID = GetGroupID();
    VECTOR<UINT32>  vUser;
    VECTOR<INT64>   vGroup;
    INT32   iUserID1 = edtDnaUser->Text.ToIntDef(-1);
    INT32   iUserID2 = edtDnaUser2->Text.ToIntDef(-1);
    INT64   i64GroupID1 = -1;
    INT64   i64GroupID2 = -1;

    edtDynamicGroup->Text = "";

    ToInt64(edtDnaGroup->Text, &i64GroupID1);
    ToInt64(edtDnaGroup2->Text, &i64GroupID2);
    if (iUserID1 < 0  &&
        iUserID2 < 0 &&
        i64GroupID1 < 0 &&
        i64GroupID2 < 0)
    {
        InfoMsg("终端和群组成员不能都为空");
        return;
    }

    if (iUserID1 > 0)       vUser.push_back(iUserID1);
    if (iUserID2 > 0)       vUser.push_back(iUserID2);
    if (i64GroupID1 > 0)    vGroup.push_back(i64GroupID1);
    if (i64GroupID2 > 0)    vGroup.push_back(i64GroupID2);

    CHAR    acGroupName[64];

    StrCpy(acGroupName, edtGroupName->Text);

    BOOL    bRet = eSDK_CreateDynamicGroup(i64GroupID, vGroup, vUser, 15, 60, acGroupName);

    if (!bRet)
    {
        InfoMsg("创建组失败");
        return;
    }

    edtDynamicGroup->Text = i64GroupID;
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton12Click(TObject *Sender)
{
    CHAR    *szInfo = NULL;

    INT32 iRet = ELTE_SDK_GetDepartmentInfoList(&szInfo);

    if (iRet == 0)
    {
        Log(Utf8ToAnsi(szInfo));
    }

    ELTE_SDK_ReleaseBuffer(szInfo);
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton13Click(TObject *Sender)
{
    VECTOR<INT64>   vGroup;
    int     iPeriod;

    ToInt(edtDeletePeriod->Text, &iPeriod);
    if (iPeriod <= 0)
    {
        iPeriod = 10;
    }

    for (int i=0; i<mmInfo->Lines->Count; i++)
    {
        INT64   i64GroupID;
        AnsiString  strLine = mmInfo->Lines->Strings[i];

        if (!ToInt64(strLine, &i64GroupID))
        {
            continue;
        }

        vGroup.push_back(i64GroupID);
    }

    mmInfo->Clear();
    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        BOOL    bRet = eSDK_CancelDynamicGroup(vGroup[i]);
        CHAR    acText[256];
        UINT32  ulCreator;

        if (bRet)
        {
            sprintf(acText, "删除组 "FORMAT_I64 " 成功", vGroup[i]);
        }
        else
        {
            ulCreator = eSDK_GetGroupCreator(vGroup[i]);
            sprintf(acText, "删除组 "FORMAT_I64 " 失败, 组创建者：%u", vGroup[i], ulCreator);
        }

        mmInfo->Lines->Append(acText);
        gos_sleep_1ms(500);
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::btnLogoutClick(TObject *Sender)
{
    APP::Logout();
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::cxButton16Click(TObject *Sender)
{
    std::vector<DC_GROUP_INFO_T>    vGroup;
    char        *szInfo = NULL;

    BOOL bRet = eSDK_GetDcGroups(GROUP_TYPE_GENERAL, vGroup);

    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        char    acInfo[128];

        sprintf(acInfo, FORMAT_I64 " %s %u", vGroup[i].i64GroupID, vGroup[i].acGroupName, vGroup[i].ulGroupType);

        Info(acInfo);
    }
}

void StartBrd(TRAIN_INFO_T *pstTrain)
{
    VECTOR<TRAIN_INFO_T*>   vTrain;

    vTrain.push_back(pstTrain);
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::TimerStartBrdTimer(TObject *Sender)
{
    static UINT32   ulLastTime = 0;

    if (!cbBrd->Checked)
    {
        return;
    }

    UINT32  ulCurrTime = gos_get_uptime();
    UINT32  ulTrainUnitID = edtBrdTrain->Text.ToIntDef(0);
    TRAIN_INFO_T            *pstTrain = Train::GetTrainInfo(ulTrainUnitID);

    if (!pstTrain)
    {
        LabelBrd->Caption = "未知列车";
    }

    // stop Brd
    StartBrd(pstTrain);

    TimerStopBrd->Enabled = true;
    GosLog(LOG_INFO, "brd time start");
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cbBrdPropertiesChange(TObject *Sender)
{
    TimerStopBrd->Enabled = false;

    if (cbBrd->Checked)
    {
        TimerStartBrd->Enabled = true;
    }
    else
    {
        TimerStartBrd->Enabled = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::TimerStopBrdTimer(TObject *Sender)
{
    TimerStopBrd->Enabled = false;
    GosLog(LOG_INFO, "brd time stop");

    UINT32  ulTrainUnitID = edtBrdTrain->Text.ToIntDef(0);
    TRAIN_INFO_T            *pstTrain = Train::GetTrainInfo(ulTrainUnitID);

    if (!pstTrain)
    {
        LabelBrd->Caption = "未知列车";
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::cxButton4Click(TObject *Sender)
{
    VECTOR<INT64> vGroup;
    AnsiString strGroupName = edtPatchGroupName->Text;

    vGroup.push_back(edtPatchGroupMember->Text.ToIntDef(0));
    vGroup.push_back(edtPatchGroupMember2->Text.ToIntDef(0));

    if (!eSDK_AddPatchGroupMember(GetGroupID(), strGroupName.c_str(), vGroup))
    {
        InfoMsg("添加成员失败");
        return;
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::cxButton21Click(TObject *Sender)
{
    char    acText[256];
    AnsiString strSpeaker = cbMonSpeaker->Text;
    AssignGroupSpeaker(GetGroupID(), strSpeaker.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::cxButton22Click(TObject *Sender)
{
    AssignGroupSpeaker(GetGroupID(), NULL);
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::cxButton23Click(TObject *Sender)
{
    VECTOR<AnsiString>  vList;
    UINT32              i;

    if (eSDK_GetSpeakerList(vList))
    {
        cbMonSpeaker->Properties->Items->Clear();

        for (i=0; i<vList.size(); i++)
        {
            cbMonSpeaker->Properties->Items->Add(vList[i]);
        }
        }

    GetMicList(vList);

    cbMic->Properties->Items->Clear();

    for (i=0; i<vList.size(); i++)
    {
        cbMic->Properties->Items->Add(vList[i]);
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::cxButton24Click(TObject *Sender)
{
    CHAR    *szName = "";

    BOOL bRet = eSDK_ClearPatchGroupMember(GetGroupID(), szName);
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton26Click(TObject *Sender)
{
    UINT32  ulVolume = 0;
    AnsiString  strName = cbMic->Text;

    if (GetMicVolume(strName, &ulVolume))
    {
        edtMicVolume->Value = ulVolume;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton27Click(TObject *Sender)
{
    UINT32  ulVolume = edtMicVolume->Value;
    AnsiString  strName = cbMic->Text;

    if (!SetMicVolume(strName.c_str(), ulVolume))
    {
        InfoMsg("设置失败");
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::btnInitPCMClick(TObject *Sender)
{
    if (!g_ThreadPcmPlay)
    {
        g_ThreadPcmPlay = ThreadPcmPlay::GetInstance();

        AnsiString  strFile = edtWavFile->Text;

        LoadPCMData(strFile.c_str());
    }

    if (!m_pucPCMData)
    {
        InfoMsg("文件不存在");
        return;
    }

    UINT32 ulSampleRate = 16000;
    UINT32 ulChanNum = 1;
    UINT32 ulBytesPerSample = 2;

    g_ThreadPcmPlay->Play(NULL, ulChanNum, ulSampleRate, ulBytesPerSample);

    btnStopPCM->Enabled = false;

    TimerPlayPCM->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::cxButton28Click(TObject *Sender)
{
    if (!g_ThreadPcmPlay)
    {
        g_ThreadPcmPlay = ThreadPcmPlay::GetInstance();

        AnsiString  strFile = edtWavFile->Text;

        LoadPCMData(strFile.c_str());
    }

    if (!m_pucPCMData)
    {
        InfoMsg("文件不存在");
        return;
    }

    UINT32 ulSampleRate = 16000;
    UINT32 ulChanNum = 1;
    UINT32 ulBytesPerSample = 2;

    g_ThreadPcmPlay->Play(NULL, ulChanNum, ulSampleRate, ulBytesPerSample);

    btnStopPCM->Enabled = true;

    TimerPlayPCM->Enabled = true;

    m_ulCurrFrameIndex = m_ulPCMFrameNum/3;
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::btnPausePCMClick(TObject *Sender)
{
    TimerPlayPCM->Enabled = false;
    g_ThreadPcmPlay->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton29Click(TObject *Sender)
{
    TimerPlayPCM->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::btnStopPCMClick(TObject *Sender)
{
    if (g_ThreadPcmPlay)
    {
        g_ThreadPcmPlay->Stop();
    }

    TimerPlayPCM->Enabled = false;

    btnPlayPCM->Enabled = true;
    btnStopPCM->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::TimerPlayPCMTimer(TObject *Sender)
{
    if (!g_ThreadPcmPlay || !TimerPlayPCM->Enabled)
    {
        return;
    }

    UINT32  ulSize = m_ulSendFrameNumPer100MillSecond*m_ulPCMFrameLen;

    if (btnStopPCM->Enabled == false)
    {
        return;
   /*       static UINT8    *pucEmptyPCMData = NULL;

        if (!pucEmptyPCMData)
        {
            pucEmptyPCMData = (UINT8*)calloc(1, ulSize);
        }

        g_ThreadPcmPlay->PushData(pucEmptyPCMData, ulSize);

        return;   */
    }

    if ((m_ulCurrFrameIndex+m_ulSendFrameNumPer100MillSecond) >= m_ulPCMFrameNum)
    {
        m_ulCurrFrameIndex = 0;
    }

    UINT8   *pucCurrPCMData = m_pucPCMData + m_ulCurrFrameIndex*m_ulPCMFrameLen;

    m_ulCurrFrameIndex += m_ulSendFrameNumPer100MillSecond;

    g_ThreadPcmPlay->PushData(pucCurrPCMData, ulSize);
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton31Click(TObject *Sender)
{
    UINT32          ulUserID = GetUserID();
    VECTOR<INT64>   vGroup;
    INT64           i64GroupID = GetGroupID();
    VECTOR<UINT32>  vUserID;

    eSDK_GetGroupMember(i64GroupID, vUserID, vGroup);

    BOOL    bRet = eSDK_StartVideoDispatch(ulUserID, vUserID, TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton32Click(TObject *Sender)
{
/*    ELTE_CHAR* pInfo = NULL;

    ELTE_INT32 iRet = ELTE_SDK_GetMultiSelectedGrpInfoVec(APP::GetSDKLoginAccount().c_str(), &pInfo);

    mmInfo->Text = pInfo?pInfo:"";

    ELTE_SDK_ReleaseBuffer(pInfo);        */

    VECTOR<SDK_MULTI_GROUP_INFO_T>   vMultiGroupInfo;

    if (!eSDK_GetMultiGroup(vMultiGroupInfo))
    {
        return;
    }

    for (UINT32 i=0; i<vMultiGroupInfo.size(); i++)
    {
        char    acInfo[512];

        sprintf(acInfo, FORMAT_I64" %s %u", vMultiGroupInfo[i].i64GroupID,
                vMultiGroupInfo[i].acGroupName, vMultiGroupInfo[i].ulDCUserID);

        mmInfo->Lines->Append(acInfo);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::edtSMSFilePropertiesButtonClick(TObject *Sender, int AButtonIndex)
{
    if (!OpenDialog->Execute())
    {
        return;
    }

    edtSMSFile->Text = OpenDialog->FileName;
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::btnSendSMSClick(TObject *Sender)
{
    VECTOR<UINT32>  vUserID;
    VECTOR<INT64>   vGroupID;

    UINT32  ulUserID;

    if (!ToInt(edtSMSUser1->Text, &ulUserID))
    {
        edtSMSUser1->Text = "";
    }
    else
    {
        vUserID.push_back(ulUserID);
    }

    if (!ToInt(edtSMSUser2->Text, &ulUserID))
    {
        edtSMSUser2->Text = "";
    }
    else
    {
        vUserID.push_back(ulUserID);
    }

    INT64   i64GroupID;
    UINT32  ulSMSID = gos_get_current_time();

    if (!ToInt64(edtSMSGroup1->Text, &i64GroupID))
    {
        edtSMSGroup1->Text = "";
    }
    else
    {
        vGroupID.push_back(i64GroupID);
    }

    if (!ToInt64(edtSMSGroup2->Text, &i64GroupID))
    {
        edtSMSGroup2->Text = "";
    }
    else
    {
        vGroupID.push_back(i64GroupID);
    }

    BOOL bRet = eSDK_SendMsg(edtSMSText->Text, edtSMSFile->Text, ulSMSID, vUserID, vGroupID);

    if (!bRet)
    {
        Info("发送彩信失败");
    }
    else
    {
        Info("发送彩信成功");
    }
}

//---------------------------------------------------------------------------

void __fastcall TFormTest::btnConfirmGroupValidClick(TObject *Sender)
{
    INT64 iGroupID = edtCheckGroup->Text.ToIntDef(-1);

    VectorMap<INT64, SDK_PATCH_GROUP_INFO_T> vPatchGroup;
    CHAR acInfo[128];

    eSDK_GetPatchGroups(vPatchGroup);

    for (UINT32 i = 0; i < vPatchGroup.Size(); i++)
    {
        vPatchGroup[i]->i64GroupID;
        vPatchGroup[i]->acGroupName;

        VECTOR<INT64> vMember;
        INT32 iRet = eSDK_GetPatchGroupMembers(GetGroupID(), vMember);
        if (iRet == 1)
        {
            for (UINT32 i = 0; i < vMember.size(); i++)
            {
                if (vMember.at(i) == iGroupID)
                {
                    InfoMsg("该成员组已经被派接");
                    return;
                }
            }
        }
        else
        {
            InfoMsg("查找派接组成员失败");
            return;
        }
    }
    InfoMsg("该成员组没有被派接");
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::btnQueryPatchGroupInfoClick(TObject *Sender)
{
    INT64 iGroupID = GetGroupID();

    SDK_PATCH_GROUP_INFO_T stOut;

    if (!eSDK_GetPatchGroupInfo(iGroupID, stOut))
    {
        GosLog(LOG_ERROR, "eSDK_GetPatchGroupInfo failed! PatchGroupID(%lld)", iGroupID);
        InfoMsg("查询失败！");
        return;
    }

    AnsiString strShow;
    strShow = strShow + "PatchGroupName: " + stOut.acGroupName +
              " PatchGroupNumber: " + stOut.i64GroupID +
              " SetUpDCID: " + stOut.ulDCUserID +
              " DCPatchIndex: " + stOut.ulPatchIndex +
              " PGPriority: " + stOut.ulPriority;

    mmInfo->Lines->Append(strShow.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnDeletePatchMemberClick(TObject *Sender)
{
    INT64 iGroupID = GetGroupID();

    if (!eSDK_CancelPatchGroup(iGroupID))
    {
        ErrMsg("删除组失败");
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton8Click(TObject *Sender)
{
    std::vector<DC_GROUP_INFO_T>    vGroup;
    char        *szInfo = NULL;
    INT32   iRet = ELTE_SDK_GetDcCameraAttribute(APP::GetSDKLoginAccount().c_str(), &szInfo);
    XML     xml(szInfo, TRUE);
    XML     *SubXML;
    std::vector<XML*> vList;

    eSDK_Free(szInfo);

    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        char    acInfo[128];

        sprintf(acInfo, FORMAT_I64 " %s %u", vGroup[i].i64GroupID, vGroup[i].acGroupName, vGroup[i].ulGroupType);

        Info(acInfo);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::btnSkinCfgClick(TObject *Sender)
{
    DialogSetSkin->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton18Click(TObject *Sender)
{
    UINT32 ulUserID = GetUserID();
    UINT32 ulVideoFormat = VIDEO_FMT_CIF;
    UINT32 ulCameraType = CAMERA_TYPE_FRONT;
    BOOL bNeedConfirm = FALSE;
    BOOL bHasVoice = TRUE;

    P2P::VideoCall(ulUserID, ulVideoFormat, ulCameraType, bNeedConfirm, bHasVoice);

    eSDK_SetVideoPlayWindow(ulUserID, Panel7->Handle);
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton33Click(TObject *Sender)
{
    INT32 iVideoChanID = edtVideoChanID->Text.ToIntDef(0);
    INT32 iUserID = edtVWallUserID->Text.ToIntDef(0);

    if (iVideoChanID <= 0 ||
        iUserID <= 0)
    {
        InfoMsg("视频源编号或视频源号码非法");
        return;
    }

    eSDK_VWallStart(iVideoChanID, iUserID);
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton34Click(TObject *Sender)
{
    INT32 iVideoChanID = edtVideoChanID->Text.ToIntDef(0);
    INT32 iUserID = edtVWallUserID->Text.ToIntDef(0);

    if (iVideoChanID <= 0 ||
        iUserID <= 0)
    {
        InfoMsg("视频源编号或视频源号码非法");
        return;
    }

    eSDK_VWallStop(iVideoChanID, iUserID);
}
//---------------------------------------------------------------------------

void __fastcall TFormTest::cxButton35Click(TObject *Sender)
{
    VECTOR<ESDK_VWALL_INFO_T>   vInfo;

    if (!eSDK_GetVWallInfo(vInfo))
    {
        return;
    }

    for(UINT32 i=0; i<vInfo.size(); i++)
    {
        CHAR    acText[256];

        sprintf(acText, "VideoChanID = %u, State = %u",
                vInfo[i].ulVideoChanID, vInfo[i].ulState);

        mmInfo->Lines->Append(acText);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTest::cxButton36Click(TObject *Sender)
{
 /*   GJsonParam          ReqJsonParam;
    GJson               RspJson;
    GJsonTupleParam     JsonTupleParam;

    GJsonParam  SubJsonParam;

    SubJsonParam.Add("UserID", "13002");
    JsonTupleParam.Add(SubJsonParam);

    ReqJsonParam.Add("SendUserID", "13002");
    ReqJsonParam.Add("SendUserName", "13002");
    ReqJsonParam.Add("SDSType", SDS_TYPE_NORMAL);
    ReqJsonParam.Add("SDSInfo", EncodeJsonMsg("\""));
    ReqJsonParam.Add("File", "");
    ReqJsonParam.Add("NeedReply", FALSE);
    ReqJsonParam.Add("User", JsonTupleParam);

    if (!SyncSendServer(EV_DC_DIS_REQ, MSG_DCAddSDSInfo, ReqJsonParam, RspJson))
    {
        GosLog(LOG_ERROR, "sync send msg MSG_DCAddSDSInfo to server failed");
        return;
    }
                       */

    bool    bRealMsg = false;
    char    *aszText[] = {
        "~`!@#$%^&*()-_+=",
        "{}[]|\\",
        ";:",
        "<>,.?/",
        "123\"456",
        "123'456",
        "123\r\n456",
        "123\t 456",
        };

    VECTOR<UINT32> vUserID;
    UINT32  ulSDSID = gos_get_current_time();
    UINT32  ulUserID = APP::GetCurrDCUserID();

    vUserID.push_back(ulUserID);

    for (UINT32 i=0; i<ARRAY_SIZE(aszText); i++)
    {
        eSDK_SendRealMsg(aszText[i], ulSDSID++, vUserID);
    }

}
//---------------------------------------------------------------------------
extern AnsiString DecodeSMSMsg(AnsiString strSDSText);

void __fastcall TFormTest::cxButton37Click(TObject *Sender)
{
    UINT32      ulTime = gos_get_uptime_1ms();

    BOOL        bRet = eSDK_IsCameraValid();

    ulTime = gos_get_uptime_1ms() - ulTime;

    AnsiString strSDSText = "abc&quot;123";

    AnsiString str = DecodeSMSMsg(strSDSText);

//    InfoMsg("摄像机状态%s(%u)", bRet?"正常":"异常", ulTime);
}
//---------------------------------------------------------------------------


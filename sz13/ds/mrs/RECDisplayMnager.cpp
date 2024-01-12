#include "RECDisplayMnager.h"
#include "mq/resourcestatus_indicator.h"
#include "mq/p2pvideocallstatus_indicator.h"
#include "mq/p2pcallstatus_indicator.h"
#include "mq/grpcallstatus_indicator.h"
#include "mq/modulestatus_indicator.h"
#include "mq/reccallstatus_indicator.h"
#include "operationmanager.h"
#include "uiparameter.h"
#include <list>
#include <string>
#include  "mq/sds_message_indicator.h"
#include "dcstate.h"
//#include "sdk_helper.h"
//#include "TDHelper.h"

#include <string>
#include <iostream>
#include <map>

#include <fstream>
#include <time.h>

#ifdef WIN32
#include <winsock2.h>
#include "windows.h"
#include <process.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#endif

#include "RecPublic.h"
#include "rec_util.h"

using namespace std;

extern BOOL g_bDCLogined;

//BYTE* Parse_RTP_Packet(BYTE pBuf[], unsigned short nSize, int* outSize);


static res_status_value_t   g_enRegResult;
static int                  g_bGetRegResult = FALSE;

res_status_value_t GetLoginStatus()
{
    for (int i=0; i<100; i++)
    {
        if (g_bGetRegResult)
        {
            return g_enRegResult;
        }

        gos_sleep_1ms(100);
    }

    return STATUS_REGFAIL;
}

RECDisplayMnager::RECDisplayMnager(void)
{

}

RECDisplayMnager::~RECDisplayMnager(void)
{
}


/*
UINT __stdcall recvudp(LPVOID cbCallStatus)//(void* callStatus)
{
    BOOL    bRet = RecvP2PCallAudio((RECP2pcallStatusIndicator*)cbCallStatus);

    return bRet?0:(-1);
}

UINT __stdcall recvVideoudp(LPVOID cbCallStatus)
{
#if defined( WIN32 ) || defined( _WIN32 )
    auto tid = GetCurrentThreadId();
    cout<<"thread id="<<tid<<endl;
    auto callStatus = (RECP2pvideocallStatusIndicator*)cbCallStatus;
    if (!callStatus)
    {
        GosLog(LOG_ERROR, "P2pvideocallStatusIndicator is NULL!");
        return -1;
    }
    auto callid = callStatus->getCallId();
    auto caller = callStatus->getRECCaller();
    auto callee = callStatus->getRECCallee();
    cout<<"insert1 map callid="<<callid<<",caller="<<caller<<",callee="<<callee<<endl;
    P2PAudioInd* p2pAInd = new P2PAudioInd;
    p2pAInd->bClosed = false;
    p2pAInd->threadid = tid;

    CTMap.insert(std::make_pair(callid, p2pAInd));

    auto cport = callStatus->getlocalaudioport();
    cout<<"the port of video amr data="<<cport<<endl;
    GosLog(LOG_DETAIL, "new thread, client port"<<cport);
    WORD wVersionRequested;
    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0)
    {
        printf("WSAStartup failed with error: %d\n", err);
        GosLog(LOG_ERROR, "WSAStartup failed with error!");
        return -1;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        printf("Could not find a usable version of Winsock.dll\n");
        GosLog(LOG_ERROR, "Could not find a usable version of Winsock.dll!");
        WSACleanup();
        return -1;
    }
    SOCKET sockClient = socket(AF_INET,SOCK_DGRAM, 0);
    SOCKADDR_IN addrServ;
    addrServ.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    addrServ.sin_family = AF_INET;
    addrServ.sin_port = htons(cport);
    int nBindRes = bind(sockClient,(struct sockaddr *)&addrServ,sizeof(addrServ));
    if (nBindRes < 0 )
    {
        cout<<"local port bind failed !"<<endl;
        GosLog(LOG_ERROR, "local port bind failed !"<<cport);
        return -1;
    }
    string strCallee = to_string(long long(callStatus->getRECCallee()));
    string strCaller = to_string(long long(callStatus->getRECCaller()));
    time_t nowtime;
    //获取当前时间
    nowtime = time(NULL);
    tm *localTime = localtime(&nowtime);
    string strYear = to_string(long long(localTime->tm_year + 1900));
    string strMonth = to_string(long long(localTime->tm_mon + 1));
    string strDay = to_string(long long(localTime->tm_mday));
    string strHour = to_string(long long(localTime->tm_hour));
    string strMin = to_string(long long(localTime->tm_min));
    string strSenc = to_string(long long(localTime->tm_sec));
    string strFileName = strCaller+ "-" +strCallee+ "-" +strYear+ "年" +strMonth+ "月" +strDay+ "日" +strHour+ "时" +strMin+ "分" +strSenc +"秒.amr";
    LPCSTR result = strFileName.c_str();
    // 每次都创建一个新文件
    HANDLE hFile = CreateFileA(result, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
    {
        cout<<"create file error!"<<endl;
        GosLog(LOG_ERROR, "create amr file error!");
        return -1;
    }
    // 写入文件头
    char fileHead[6];
    memset(fileHead, 0, 6);
    memcpy(fileHead,"#!AMR\n",6);
    UINT32 dwWriSize = 0;
    BOOL bRet = WriteFile(hFile, fileHead, 6, &dwWriSize, NULL);

    // 轮流写入每一帧数据
    int nSize = sizeof(addrServ);
    char recvBuff[1050];
    int nIndex = 0;
    while(1)
    {
        //判断全局变量对用的threadid中参数是否是bye
        if (0 < CTMap.size())
        {
            auto iter = CTMap.find(callid);
            if (iter != CTMap.end())
            {
                //cout<<"find this threadid info!"<<tid<<",callid ="<<callid<<endl;
                auto ind = iter->second;
                bool bCloseFlag = ind->bClosed;
                //cout<<"bCloseFlag="<<bCloseFlag<<endl;
                if (bCloseFlag) //关闭file文件和socket流
                {
                    cout<<"call has been closed"<<endl;
                    if (INVALID_HANDLE_VALUE != hFile)
                    {
                        cout<<"the handle has been closed"<<endl;
                        CloseHandle(hFile);
                    }
                    closesocket(sockClient);
                    //清除P2PAudioInd中记录
                    cout<<"clear ctmap"<<endl;
                    CTMap.erase(iter);
                    return -1;
                }
            }
        }
        memset(recvBuff, 0, sizeof(recvBuff));
        //设置超时
        struct timeval tv;
        fd_set readfds;
        int i=0;
        unsigned int n=0;
        char buf[1024];
        struct sockaddr addr;
        // socklen_t len;

        FD_ZERO(&readfds);
        FD_SET(sockClient,&readfds);
        tv.tv_sec=3;
        tv.tv_usec=10;
        select(sockClient+1,&readfds,NULL,NULL,&tv);

        if(FD_ISSET(sockClient,&readfds))
        {
            int nDataLen = recvfrom(sockClient, recvBuff,sizeof(recvBuff), 0, (struct sockaddr*)&addrServ, &nSize);
            //cout<<"nDataLen="<<nDataLen<<endl;
            if (EWOULDBLOCK == nDataLen || EAGAIN == nDataLen)
            {
                auto err = GetLastError();
                cout<<"recieve Audio Frame timeout, error="<<err<<endl;
                GosLog(LOG_WARN, "recieve Audio Frame timeout!");
                continue;
            }
            // 接收到的数据没有音频部分
            if (nDataLen <= AUDIO_HEAD_LEN)
            {
                continue;
            }
            else if(nDataLen == SOCKET_ERROR )
            {
                auto err = GetLastError();
                cout<<"recieve Audio Frame error="<<err<<endl;
                GosLog(LOG_ERROR, "recieve Audio Frame error!");
                return -1;
            }
            else
            {
                int nRealLen = nDataLen - AUDIO_HEAD_LEN;
                char *data = new char[nRealLen];
                memset(data, 0, nRealLen);
                for (int i=0; i<nRealLen; i++)
                {
                    int j = i + AUDIO_HEAD_LEN;
                    data[i] = recvBuff[j];
                }
                WriteFile(hFile, data, nRealLen, &dwWriSize, NULL);
                delete[]data;
            }
        }
    }
    #endif
    return 0;
}
*/

//通知用户和群组资源的状态变更信息,需要YC实现
void RECDisplayMnager::notifyUserStatus(ResourceStatusIndicator * resourceStatus)
{
    if (resourceStatus->getStatusType() == USERSTATUS)
    {
        printf("Received USERSTATUS event\n");
        UserStatusIndicator* us=dynamic_cast<UserStatusIndicator*>(resourceStatus);
        if(us->getStatusValue() == STATUS_TXMTING && us->getCallType()==UPLOADP2P)
        {
            //申请拷贝流...OP
            printf("Received TXMTING event\n");
            MSISDN id=resourceStatus->getResId();
        }
        else if (us->getStatusValue()== STATUS_TXMTIDLE && us->getCallType()==UPLOADP2P)
        {
            //delete sdp
            //告知中心结束
            printf("Received TXMTIDLE event\n");
            MSISDN id=resourceStatus->getResId();
            printf("video stop\n");
        }
    }
}

//通知视频呼叫的状态变更信息（包括视频回传），需要YC实现
void RECDisplayMnager::notifyP2pvideocallStatus(P2pvideocallStatusIndicator * callStatus)
{
    if(!callStatus)
    {
        return;
    }

    P2pvideocall_Status_t vcallstatus = callStatus->getStatus();
    long caller = callStatus->getCaller();
    long callee = callStatus->getCallee();
    long callid = callStatus->getCallID();

    RECP2pvideocallStatusIndicator* RECP2pvideocallStatus = dynamic_cast<RECP2pvideocallStatusIndicator*>(callStatus);

    callid = RECP2pvideocallStatus->getCallId();

    GosLog(LOG_DETAIL, "notifyP2pvideocallStatus: callid=%u, caller=%u, callee=%u, call status=%u", callid, caller, callee, vcallstatus);

    switch (vcallstatus)
    {
    case P2Pvideocall_IND_STATUS_UNKNOWN:
        GosLog(LOG_ERROR, "unknown P2P call status");
        break;

    case P2Pvideocall_IND_STATUS_MONITOR:
    case P2Pvideocall_IND_STATUS_RECEIVED:
        {
//          cout<<"Receive video call! To receive or reject:"<<endl;
    //      RECP2pvideocallStatusIndicator* RECP2pvideocallStatus = dynamic_cast<RECP2pvideocallStatusIndicator*>(callStatus);
            //std::unique_ptr<RECP2pvideocallStatusIndicator> RECvideoStatus(RECP2pvideocallStatus);

        //  cout<<"status type="<< RECP2pvideocallStatus->getStatus()<<endl;
            long from = RECP2pvideocallStatus->getCaller();
            MSISDN resid = RECP2pvideocallStatus->getCallId();
            long recCaller = RECP2pvideocallStatus->getRECCaller();
            long recCallee = RECP2pvideocallStatus->getRECCallee();

            GosLog(LOG_DETAIL, "to recv dial call, callID=%u, caller=%u, callee=%u", resid, recCaller, recCallee);

            OperationManager* om = OperationManager_T::instance();
            //ResourceInfo* resInfo = new ResourceInfo(resid);
            ResourceInfo  resInfo(resid);

            int ret = om->invokeOp(&resInfo, P2P_VIDEO_RECV);
            //ret = SDK_Helper::scheduleAsynchOperation(&resInfo, P2P_RECV);
            GosLog(LOG_INFO, "p2p video recv ret=%d", ret);
        }

        break;

    case P2Pvideocall_IND_STATUS_ANSWERED:
        GosLog(LOG_INFO, "callee has received this video call");
        break;

    case P2Pvideocall_IND_STATUS_HANGUPED:
    case P2Pvideocall_IND_STATUS_CANCELLED:
        GosLog(LOG_INFO, "callee has hangup this video call %d", callid);

        CloseRecAudioThread(callid);
        CloseRecVideoThread(callid);

        break;

    case P2Pvideocall_IND_STATUS_ACKED:
        {
            GosLog(LOG_DETAIL, "recv videocall status acked");

            /*
            unsigned long   ulRemoteIP = callStatus->getRemoteIp();

            auto remoteaudioPort = callStatus->getremoteaudioport();
            auto remotevideoPort = callStatus->getremotevideoport();

            auto localaudioPort = callStatus->getlocalaudioport();
            auto localvideoPort = callStatus->getlocalvideoport();

            //char* remoteIP = new char[16];
            //memset(remoteIP,0,16);
            */

            RECP2pvideocallStatusIndicator* RECP2pvideocallStatus = dynamic_cast<RECP2pvideocallStatusIndicator*>(callStatus);

            long recCaller = RECP2pvideocallStatus->getRECCaller();
            long recCallee = RECP2pvideocallStatus->getRECCallee();

            P2P_CALL_STATUS_INFO_T      *pstAudioCallStatus = (P2P_CALL_STATUS_INFO_T*)calloc(1, sizeof(P2P_CALL_STATUS_INFO_T));

            pstAudioCallStatus->iLocalPort = RECP2pvideocallStatus->getlocalaudioport();
            pstAudioCallStatus->ulCallID = callid;
            pstAudioCallStatus->ulCaller = recCaller;
            pstAudioCallStatus->ulCallee = recCallee;

            // 接收音频数据
            //_beginthreadex(NULL, 0, recvVideoudp, callStatus/*.get()*/, 0,NULL);


            // 接收视屏数据
            //_beginthreadex(NULL, 0, recvH264Data, callStatus/*.get()*/, 0,NULL);

            P2P_CALL_STATUS_INFO_T      *pstVideoCallStatus = (P2P_CALL_STATUS_INFO_T*)calloc(1, sizeof(P2P_CALL_STATUS_INFO_T));

            pstVideoCallStatus->iLocalPort = RECP2pvideocallStatus->getlocalvideoport();
            pstVideoCallStatus->ulCallID = callid;
            pstVideoCallStatus->ulCaller = recCaller;
            pstVideoCallStatus->ulCallee = recCallee;

            CHAR     acTime[32];
            gos_get_text_time_ex(NULL, "%04u%02u%02u%02u%02u%02u", acTime);
            
            strcpy(pstVideoCallStatus->acTime, acTime);
            strcpy(pstAudioCallStatus->acTime, acTime);

            GosLog(LOG_INFO, "recv video call %u at port %d and %d", pstVideoCallStatus->ulCallID, pstAudioCallStatus->iLocalPort, pstVideoCallStatus->iLocalPort);

            gos_create_thread(RecvP2PVideoCallAudio, 32*1024, pstAudioCallStatus);

            gos_create_thread(RecvP2PVideoCallVideo, 32*1024, pstVideoCallStatus);
        }
        break;

    default:
        break;
    }
}

//通知用户普通短消息的服务接收实现
void RECDisplayMnager::notifySMS(SdsMessageIndicator * sdsmsg)
{
  //const std::string EXMPP_MSG_TYPE_NORMAL     = "0001";
  //const std::string EXMPP_MSG_TYPE_STATUS     = "0002";
  //const std::string EXMPP_MSG_TYPE_ALERT      = "0003";
  //const std::string EXMPP_MSG_TYPE_MMS        = "0004";
  //const std::string EXMPP_MSG_TYPE_DROP       = "0005";
  //const std::string EXMPP_MSG_TYPE_ACK        = "0010";
    if (sdsmsg->getSdsType() == EXMPP_MSG_TYPE_NORMAL)
    {
        GosLog(LOG_INFO, "new SDS from %s, to %s, content: %s, retcode %s",
                sdsmsg->getSdsFrom().c_str(),
                sdsmsg->getSdsTo().c_str(),
                sdsmsg->getSdsContent().c_str(),
                sdsmsg->getRetCode().c_str());
    }
    else if (sdsmsg->getSdsType() == EXMPP_MSG_TYPE_STATUS)
    {
        printf("Sds EXMPP_MSG_TYPE_STATUS information ");
    }
    else if (sdsmsg->getSdsType() == EXMPP_MSG_TYPE_MMS)
    {
        GosLog(LOG_INFO, "new mms from %s, to %s, content: %s, retcode %s",
                sdsmsg->getSdsFrom().c_str(),
                sdsmsg->getSdsTo().c_str(),
                sdsmsg->getSdsContent().c_str(),
                sdsmsg->getRetCode().c_str());   

        RecvfileList lFileList = sdsmsg->getSdsFilename();

        RecvfileList::iterator it = lFileList.begin();

        for (; it != lFileList.end(); it++)
        {
            GosLog(LOG_INFO, "filename : %s",it->c_str());
        }
    }
    else if(sdsmsg->getSdsType() == EXMPP_MSG_TYPE_ACK)
    {
        
    }
    else
    {
        //printf("Finsh!");
    }
}

//通知用户状态短消息的服务接收实现
void RECDisplayMnager::notifyUserSdsStatus(SdsMessageIndicator * sdsmsg)
{
    if(sdsmsg->getSdsType()==EXMPP_MSG_TYPE_STATUS)
    {
//      printf("Sds  EXMPP_MSG_TYPE_STATUS information received\n");
    }
    else
    {
    //  printf("notifyUserSdsStatus is callback-ed but NOT a status message!");
    }
}

void RECDisplayMnager::notifyResourceStatus(ResourceStatusIndicator * resourceStatus)
{
    //cout<<"notify Resource Status"<<endl;

    if (resourceStatus == NULL)
    {
        return;
    }

    /*
    cout<<"status type="<<resourceStatus->getStatusType()<<endl;
    if(resourceStatus->getStatusType() == GRPPATCHSTATUS)
    {
        cout<<"patch group id="<<resourceStatus->getResId()<<","<<resourceStatus->getAttachingGrp()<<endl;
    }

    if (resourceStatus->getStatusType() == USERDGNASTATUS)
    {
        cout<<"recv dynamic group message, dynamic groupid="<<resourceStatus->getAttachingGrp()<<",resource id="<<resourceStatus->getResId()<<endl;
        ResourceInfo res(resourceStatus->getAttachingGrp());

        int ret = SDK_Helper::execsynchOperation(&res, GRP_SUB);
    } */

    if (RESREGSTATUS_PROXY == resourceStatus->getStatusType())
    {
        res_status_value_t  value = resourceStatus->getStatusValue();
        if(STATUS_REGOK == value)
        {
            g_enRegResult = STATUS_REGOK;
            g_bGetRegResult = TRUE;
            //TDHelper::getinstance()->signalLoginStatus(true,STATUS_REGOK);
        }
        else
        {
            if (STATUS_UNAUTHORIZED != value)
            {
                g_enRegResult = value;
                g_bGetRegResult = TRUE;
                //TDHelper::getinstance()->signalLoginStatus(false,value);
            }
        }
    }
}

/*
void ConvertUL2StringIP(char* & desip, const unsigned long ulIP)
{
  //由unsigned long转换成string
    struct in_addr  ipadr;
    //ipadr.S_un.S_addr = ulIP;

    memcpy(&ipadr.s_addr, &ulIP, 4);

    strcpy_s( desip, strlen(inet_ntoa(ipadr))+1,inet_ntoa(ipadr) );
}*/

void RECDisplayMnager::notifyP2pcallStatus(P2pcallStatusIndicator * callStatus)
{
    if(callStatus == NULL)
    {
        return;
    }

    RECP2pcallStatusIndicator* RECcallStatus = dynamic_cast<RECP2pcallStatusIndicator*>(callStatus);
    //std::unique_ptr<RECP2pcallStatusIndicator> RECcallStatus(RECcallStatus1);

    MSISDN  callid = RECcallStatus->getCallId();
    long recCallee = RECcallStatus->getRECCallee();
    long recCaller = RECcallStatus->getRECCaller();
    P2pcall_Status_t  enCallStatus = RECcallStatus->getStatus();

    GosLog(LOG_DETAIL, "get P2P call status, callID=%u, caller=%u, callee=%u, status=%u", callid, recCaller, recCallee, enCallStatus);

    if (enCallStatus == P2P_IND_STATUS_RECEIVED)
    {
#if 0
        P2P_CALL_STATUS_INFO_T      *pstCallStatus = (P2P_CALL_STATUS_INFO_T*)calloc(1, sizeof(P2P_CALL_STATUS_INFO_T));

        pstCallStatus->iLocalPort = RECcallStatus->getLocalPort();
        pstCallStatus->ulCallID = callid;
        pstCallStatus->ulCaller = recCallee;
        pstCallStatus->ulCallee = recCaller;

        gos_create_thread(RecvP2PCallAudio, 32*1024, pstCallStatus);

        GosLog(LOG_DETAIL, "ack P2P call, caller=%u, callee=%u, callid=%u",
            pstCallStatus->ulCaller, pstCallStatus->ulCallee, callid);
#else  // TODO 下面的demo源码的目前的是什么

        OperationManager* om = OperationManager_T::instance();

        if (!om)
        {
            GosLog(LOG_ERROR, "get om failed");
            return;
        }

        //ResourceInfo* resInfo = new ResourceInfo(callid);
        ResourceInfo resInfo(callid);

        int ret = om->invokeOp(&resInfo, P2P_RECV);

        GosLog(LOG_DETAIL, "recv P2P call, callid=%u ret = %d", callid, ret);
        //ret = SDK_Helper::scheduleAsynchOperation(&resInfo, P2P_RECV);
#endif
    }
    else if (enCallStatus == P2P_IND_STATUS_RELEASED_BUSY)
    {
        GosLog(LOG_INFO, "call already send out, but callee is busy now, callid is %u", callid);
    }
    else if (enCallStatus == P2P_IND_STATUS_PROCEEDING)
    {
        //GosLog(LOG_DETAIL, "waiting for callee's answer, calllid is %u", callid);
    }
    else if (enCallStatus == P2P_IND_STATUS_REMOTE_NOANSWER)
    {
        GosLog(LOG_DETAIL, "remote callee didn't answer the call, callid is %u", callid);
    }
    else if (enCallStatus == P2P_IND_STATUS_PICKEDUP)
    {
        GosLog(LOG_DETAIL, "callee has picked up this call, callid is %u", callid);
    }
    else if (enCallStatus == P2P_IND_STATUS_HANGUPED ||
             enCallStatus == P2P_IND_STATUS_HANGUPED_ACTIVE)
    {
        GosLog(LOG_DETAIL, "the call has been closed, callid=%u", callid);

        CloseRecAudioThread(callid);
    }
    else if (enCallStatus == P2P_IND_STATUS_ACKED ||
             enCallStatus == P2P_IND_STATUS_ANSWERED)
    {

        //开始接收用户面数据

    //  unsigned long   ulRemoteIP = RECcallStatus->getRemoteIP();
//      int remotePort = RECcallStatus->getRemotePort();
//      int localPort = RECcallStatus->getLocalPort();

        /*
        char* remoteIP = new char[16];
        memset(remoteIP,0,16);
        ConvertUL2StringIP(remoteIP, ulRemoteIP); */

        //_beginthreadex(NULL, 0, recvudp, RECcallStatus/*.get()*/, 0,NULL);

        P2P_CALL_STATUS_INFO_T      *pstCallStatus = (P2P_CALL_STATUS_INFO_T*)calloc(1, sizeof(P2P_CALL_STATUS_INFO_T));

        pstCallStatus->iLocalPort = RECcallStatus->getLocalPort();
        pstCallStatus->ulCallID = RECcallStatus->getCallId();
        pstCallStatus->ulCaller = RECcallStatus->getRECCaller();
        pstCallStatus->ulCallee = RECcallStatus->getRECCallee();

        gos_create_thread(RecvP2PCallAudio, 32*1024, pstCallStatus);

        GosLog(LOG_DETAIL, "ack P2P call, caller=%u, callee=%u, callid=%u",
                pstCallStatus->ulCaller, pstCallStatus->ulCallee, callid);

        GosLog(LOG_INFO, "sound type is %u, mix is %u", RECcallStatus->getSoundPtype(), RECcallStatus->getMix());
    }
    //发起紧急点呼
    else if (enCallStatus == P2P_IND_STATUS_EMERGENCY_BEGIN)
    {
        OperationManager* om = OperationManager_T::instance();
        //ResourceInfo* resInfo = new ResourceInfo(callid);
        ResourceInfo  resInfo(callid);
        int ret = om->invokeOp(&resInfo, P2P_RECV);

        GosLog(LOG_INFO, "recv emergency call, callid=%u", callid);
    }
    //紧急点呼开始
    else if (enCallStatus == P2P_IND_STATUS_EMERGENCY)
    {
        //_beginthreadex(NULL, 0, recvudp, RECcallStatus/*.get()*/, 0,NULL);
        P2P_CALL_STATUS_INFO_T      *pstCallStatus = (P2P_CALL_STATUS_INFO_T*)calloc(1, sizeof(P2P_CALL_STATUS_INFO_T));

        pstCallStatus->iLocalPort = RECcallStatus->getLocalPort();
        pstCallStatus->ulCallID = RECcallStatus->getCallId();
        pstCallStatus->ulCaller = RECcallStatus->getRECCaller();
        pstCallStatus->ulCallee = RECcallStatus->getRECCallee();

        gos_create_thread(RecvP2PCallAudio, 32*1024, pstCallStatus);

        GosLog(LOG_INFO, "an emergency voice conversation is started, caller=%u, callee=%u, callid=%u",
                pstCallStatus->ulCaller, pstCallStatus->ulCallee, callid);

    }
    // 发起半双工点呼
    else if (enCallStatus == P2P_IND_STATUS_HALFDUPLEX_BEGIN)
    {
        OperationManager* om = OperationManager_T::instance();
        //ResourceInfo* resInfo = new ResourceInfo(callid);
        ResourceInfo  resInfo(callid);
        int ret = om->invokeOp(&resInfo, P2P_RECV);

        GosLog(LOG_DETAIL, "half duplex p2p call request received, callid=%u", callid);
    }
    else if (enCallStatus == P2P_IND_STATUS_TX_BEGIN)
    {
        GosLog(LOG_DETAIL, "half_duplex p2p call transmit begin, callid=%u", callid);
    }
    else if (enCallStatus == P2P_IND_STATUS_TX_IDLE)
    {
        GosLog(LOG_DETAIL, "half_duplex p2p call transmit idle, callid=%u", callid);
    }
    else if (enCallStatus == P2P_IND_STATUS_TX_VOICE)
    {
        GosLog(LOG_DETAIL, "half_duplex p2p call transmit voice, callid=%u", callid);
    }
    else if (enCallStatus == P2P_IND_STATUS_FC_ACCEPTED)
    {
        GosLog(LOG_DETAIL, "half_duplex p2p floor accepted, callid=%u", callid);
    }
    else if (enCallStatus == P2P_IND_STATUS_FC_REJECTED)
    {
        GosLog(LOG_DETAIL, "half_duplex p2p floor rejected, callid=%u", callid);
    }
    // 挂断紧急点呼（或半双工点呼）
    else if (enCallStatus == P2P_IND_STATUS_CANCELLED)
    {
        GosLog(LOG_INFO, "the call has been closed, callid=%u", callid);

        CloseRecAudioThread(callid);
    }
}

/*
UINT __stdcall recGroupoudp(LPVOID cbCallStatus)
{
#if defined( WIN32 ) || defined( _WIN32 )
    auto tid = GetCurrentThreadId();
    cout<<"thread id="<<tid<<endl;
    auto callStatus = (RECGroupStatusIndicator*)cbCallStatus;
    if (!callStatus)
    {
        GosLog(LOG_ERROR, "GroupStatusIndicator is NULL!");
        return -1;
    }
    auto callid = callStatus->getCallId();
    auto caller = callStatus->getRECCaller();
    auto callee = callStatus->getRECCallee();
    cout<<"insert1 map callid="<<callid<<",caller="<<caller<<",callee="<<callee<<endl;
    P2PAudioInd* p2pAInd = new P2PAudioInd;
    p2pAInd->bClosed = false;
    p2pAInd->threadid = tid;

    CTMap.insert(std::make_pair(callid, p2pAInd));

    auto cport = callStatus->getLocalPort();
    GosLog(LOG_DETAIL, "new thread, client port"<<cport);
    WORD wVersionRequested;
    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0)
    {
        printf("WSAStartup failed with error: %d\n", err);
        GosLog(LOG_ERROR, "WSAStartup failed with error!");
        return -1;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        printf("Could not find a usable version of Winsock.dll\n");
        GosLog(LOG_ERROR, "Could not find a usable version of Winsock.dll!");
        WSACleanup();
        return -1;
    }
    SOCKET sockClient = socket(AF_INET,SOCK_DGRAM, 0);
    SOCKADDR_IN addrServ;
    addrServ.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    addrServ.sin_family = AF_INET;
    addrServ.sin_port = htons(cport);
    int nBindRes = bind(sockClient,(struct sockaddr *)&addrServ,sizeof(addrServ));
    if (nBindRes < 0 )
    {
        cout<<"local port bind failed !"<<endl;
        GosLog(LOG_ERROR, "local port bind failed !")
        return -1;
    }
    string strCallee = to_string(long long(callStatus->getRECCallee()));
    string strCaller = to_string(long long(callStatus->getRECCaller()));
    time_t nowtime;
    //获取当前时间
    nowtime = time(NULL);
    tm *localTime = localtime(&nowtime);
    string strYear = to_string(long long(localTime->tm_year + 1900));
    string strMonth = to_string(long long(localTime->tm_mon + 1));
    string strDay = to_string(long long(localTime->tm_mday));
    string strHour = to_string(long long(localTime->tm_hour));
    string strMin = to_string(long long(localTime->tm_min));
    string strSenc = to_string(long long(localTime->tm_sec));
    string strFileName = strCaller+ "-" +strCallee+ "-" +strYear+ "年" +strMonth+ "月" +strDay+ "日" +strHour+ "时" +strMin+ "分" +strSenc +"秒.amr";
    LPCSTR result = strFileName.c_str();
    // 每次都创建一个新文件
    HANDLE hFile = CreateFileA(result, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
    {
        cout<<"create file error!"<<endl;
        GosLog(LOG_ERROR, "create amr file error!")
        return -1;
    }
    // 写入文件头
    char fileHead[6];
    memset(fileHead, 0, 6);
    memcpy(fileHead,"#!AMR\n",6);
    UINT32 dwWriSize = 0;
    BOOL bRet = WriteFile(hFile, fileHead, 6, &dwWriSize, NULL);

    // 轮流写入每一帧数据
    int nSize = sizeof(addrServ);
    char recvBuff[1050];
    int nIndex = 0;
    while(1)
    {
        //判断全局变量对用的threadid中参数是否是bye
        if (0 < CTMap.size())
        {
            auto iter = CTMap.find(callid);
            if (iter != CTMap.end())
            {
                auto ind = iter->second;
                bool bCloseFlag = ind->bClosed;
                if (bCloseFlag) //关闭file文件和socket流
                {
                    cout<<"call has been closed"<<endl;
                    if (INVALID_HANDLE_VALUE != hFile)
                    {
                        CloseHandle(hFile);
                    }
                    closesocket(sockClient);
                    //清除P2PAudioInd中记录
                    cout<<"clear ctmap"<<endl;
                    CTMap.erase(iter);
                    return -1;
                }
            }
        }
        memset(recvBuff, 0, sizeof(recvBuff));
        //设置超时
        struct timeval tv;
        fd_set readfds;
        int i=0;
        unsigned int n=0;
        char buf[1024];
        struct sockaddr addr;
        // socklen_t len;

        FD_ZERO(&readfds);
        FD_SET(sockClient,&readfds);
        tv.tv_sec=3;
        tv.tv_usec=10;
        select(sockClient+1,&readfds,NULL,NULL,&tv);

        if(FD_ISSET(sockClient,&readfds))
        {
            int nDataLen = recvfrom(sockClient, recvBuff,sizeof(recvBuff), 0, (struct sockaddr*)&addrServ, &nSize);
            //cout<<"nDataLen="<<nDataLen<<endl;
            if (EWOULDBLOCK == nDataLen || EAGAIN == nDataLen)
            {
                auto err = GetLastError();
                cout<<"recieve Audio Frame timeout, error="<<err<<endl;
                GosLog(LOG_WARN, "recieve Audio Frame timeout!");
                continue;
            }
            else if(nDataLen == SOCKET_ERROR )
            {
                auto err = GetLastError();
                cout<<"recieve Audio Frame error="<<err<<endl;
                GosLog(LOG_ERROR, "recieve Audio Frame error!");
                return -1;
            }
            else
            {
                int nRealLen = nDataLen - AUDIO_HEAD_LEN;
                char *data = new char[nRealLen];
                memset(data, 0, nRealLen);
                for (int i=0; i<nRealLen; i++)
                {
                    int j = i + AUDIO_HEAD_LEN;
                    data[i] = recvBuff[j];
                }
                WriteFile(hFile, data, nRealLen, &dwWriSize, NULL);
                delete[]data;
            }
        }
    }
#endif
    return 0;
}*/

void RECDisplayMnager::notifyGroupStatus(GrpDspInfo * grpInfo)
{
    if (grpInfo == NULL)
    {
        return;
    }

    RECGroupStatusIndicator* RecGrpCallStatus = dynamic_cast<RECGroupStatusIndicator*>(grpInfo);

    UINT32  ulStatus = grpInfo->getSpStatus();
    UINT32  ulCallID = grpInfo->getCallID();
    INT64   i64GrouPID = grpInfo->getGrpId();

    ulCallID = RecGrpCallStatus->getCallId();

    GosLog(LOG_DETAIL, "group(" FORMAT_I64 ") call(%u) status %u", i64GrouPID, ulCallID, ulStatus);

    if (ulStatus == PTT_IND_STATUS_UNKNOWN)
    {
        GosLog(LOG_DETAIL, "unknow group call status, callid: %u", ulCallID);
        return;
    }
    else if (ulStatus == PTT_IND_STATUS_RECEIVED )
    {
        //RECGroupStatusIndicator* RecGrpCallStatus = dynamic_cast<RECGroupStatusIndicator*>(grpInfo);

        long long recCaller = RecGrpCallStatus->getRECCaller();
        long long recCallee = RecGrpCallStatus->getRECCallee();

        GosLog(LOG_DETAIL, "group call is successfully received,groupID=" FORMAT_I64 ",caller=" FORMAT_I64 ",callee=" FORMAT_I64 ",callid=%d",
                RecGrpCallStatus->getGrpId(), RecGrpCallStatus->getRECCaller(), RecGrpCallStatus->getRECCallee(), RecGrpCallStatus->getCallId());
            //_beginthreadex(NULL, 0, recGroupoudp, RecGrpCallStatus/*.get()*/, 0,NULL);

        GROUP_CALL_STATUS_INFO_T    *pstCallStatus = (GROUP_CALL_STATUS_INFO_T*)calloc(1, sizeof(GROUP_CALL_STATUS_INFO_T));

        pstCallStatus->i64GroupID = RecGrpCallStatus->getGrpId();
        pstCallStatus->ulCallID = RecGrpCallStatus->getCallId();
        pstCallStatus->iLocalPort = RecGrpCallStatus->getLocalPort();

        gos_create_thread(RecvGroupCallAudio, 32*1024, pstCallStatus);

    }
    else if (ulStatus == PTT_IND_STATUS_SNATCH)
    {
        GosLog(LOG_INFO, "group(" FORMAT_I64 ") call(%u) start", i64GrouPID, ulCallID);
    }
    else if (ulStatus == PTT_IND_STATUS_WAITING)
    {
        GosLog(LOG_INFO, "waiting for callee to accept ,callid : %u", ulCallID);
    }
    else if (ulStatus == PTT_IND_STATUS_CLOSE_OK)
    {

        //RECGroupStatusIndicator* RecGrpCallStatus = dynamic_cast<RECGroupStatusIndicator*>(grpInfo);

        ulCallID = RecGrpCallStatus->getCallId();

        GosLog(LOG_INFO, "group(" FORMAT_I64 ") call(%u) has been closed", i64GrouPID, ulCallID);
        //stop record

        CloseRecAudioThread(ulCallID);
    }
    else if (ulStatus == PTT_IND_STATUS_TX_BEGIN)
    {
        GosLog(LOG_INFO, "group(" FORMAT_I64 ") call current speaker is=%u, speak name=%s", i64GrouPID, (UINT32)grpInfo->getSpeaker(), grpInfo->getSpeakerName().c_str());
    }
    //nobody is talking in the group
    else if (ulStatus == PTT_IND_STATUS_TX_IDLE)
    {
        GosLog(LOG_INFO, "group(" FORMAT_I64 ") call is idle", i64GrouPID);
    }
    else if (ulStatus == PTT_IND_STATUS_EMERGENCY_BEGIN)
    {
        GosLog(LOG_INFO, "begin emergency group(" FORMAT_I64 ") call %u", i64GrouPID, ulCallID);

        RECGroupStatusIndicator* RecGrpCallStatus = dynamic_cast<RECGroupStatusIndicator*>(grpInfo);
    //  _beginthreadex(NULL, 0, recGroupoudp, RecGrpCallStatus/*.get()*/, 0,NULL);

        GROUP_CALL_STATUS_INFO_T    *pstCallStatus = (GROUP_CALL_STATUS_INFO_T*)calloc(1, sizeof(GROUP_CALL_STATUS_INFO_T));

        pstCallStatus->i64GroupID = RecGrpCallStatus->getGrpId();
        pstCallStatus->ulCallID = RecGrpCallStatus->getCallId();
        pstCallStatus->iLocalPort = RecGrpCallStatus->getLocalPort();

        gos_create_thread(RecvGroupCallAudio, 32*1024, pstCallStatus);
    }
    else if (ulStatus == PTT_IND_STATUS_EMERGENCY_UPDATE)
    {
        GosLog(LOG_INFO, "switch to emergency group call");
    //RECGroupStatusIndicator* RecGrpCallStatus = dynamic_cast<RECGroupStatusIndicator*>(grpInfo);
    //_beginthreadex(NULL, 0, recGroupoudp, RecGrpCallStatus/*.get()*/, 0,NULL);
    }

}

const char* GetStatusText(module_status_t enStatus)
{
    if (enStatus == SIP_DISCONNECT)         return "SIP link disconnected";
    if (enStatus == SIP_KEEPALIVE_REJECT)   return "SIP link keepalive failed";
    if (enStatus == SIP_START_RUNNING)      return "MDC restarted";
    if (enStatus == KICK_OFF)               return "DC is kick-off";
    if (enStatus == PASSWORD_CHANGE)        return "DC password changed";
    if (enStatus == MODULE_DISCONNECT)      return "Server not connected";

    return "";
}
void RECDisplayMnager::notifyModuleStatus(DCModuleStatusIndicator * moduleStatus)
{
    if (moduleStatus == NULL)
    {
        return;
    }

    module_status_t     enStatus = moduleStatus->getModuleStatus();

//  GosLog(LOG_INFO, "notifyModuleStatus : %u", enStatus);

    if (enStatus == RESOURCE_STATUS_NORMAL)
    {
        GosLog(LOG_INFO, "mrs is normally runninng");
    }
    else if (enStatus == SDS_CONNECT)
    {
        GosLog(LOG_INFO, "SDS is connected");
    }

    if (enStatus == SIP_DISCONNECT ||           // SIP link disconnected
        enStatus == SIP_KEEPALIVE_REJECT ||     // SIP link keepalive failed.Application is best to restart itself.
        enStatus == SIP_START_RUNNING    ||     // MDC restarted (for MRS only)
        enStatus == KICK_OFF ||                 // DC is kick-off
        enStatus == PASSWORD_CHANGE ||          // DC password changed
        enStatus == MODULE_DISCONNECT )         // Server not connected
    {
        const char  *szStatusText = GetStatusText(enStatus);

        GosLog(LOG_FATAL, "current status is %u(%s), need to login", enStatus, szStatusText);

        g_bDCLogined = FALSE;
    }

}

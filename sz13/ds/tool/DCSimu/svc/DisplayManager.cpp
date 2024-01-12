#include "StdAfx.h"
#include "DisplayManager.h"
#include "mq/resourcestatus_indicator.h"
#include "mq/p2pvideocallstatus_indicator.h"
#include "mq/p2pcallstatus_indicator.h"
#include "mq/grpcallstatus_indicator.h"
#include "mq/modulestatus_indicator.h"
#include "mq/provchange_indicator.h"
#include "operationmanager.h"
#include "uiparameter.h"
#include <list>
#include <string>
#include  "mq/sds_message_indicator.h"
#include "dcstate.h"
#include "sdk_helper.h"
#include "OperationProxy.h"

#include <string>
#include <iostream>
using namespace std;


TDDisplayMnager::TDDisplayMnager(void)
{

}


TDDisplayMnager::~TDDisplayMnager(void)
{
}

//通知用户和群组资源的状态变更信息,需要YC实现
void TDDisplayMnager::notifyUserStatus(ResourceStatusIndicator * resourceStatus)
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
void TDDisplayMnager::notifyP2pvideocallStatus(P2pvideocallStatusIndicator * callStatus)
{
  cout<<"enter sdk callback notifyP2pvideocallStatus!"<<endl;
  if(!callStatus)
  {
    cout<<"notify p2pvideo call status pointer is nullptr!"<<endl;
    return;
  }
  auto vcallstatus =  P2Pvideocall_IND_STATUS_UNKNOWN;
  vcallstatus = callStatus->getStatus();
  auto caller = callStatus->getCaller();
  cout<<"caller="<<caller<<", call status="<<vcallstatus<<endl;
  switch (vcallstatus)
  {
  case P2Pvideocall_IND_STATUS_UNKNOWN:
    cout<<"unknown status"<<endl;
    break;
  case P2Pvideocall_IND_STATUS_RECEIVED:
    cout<<"Receive video call! To receive or reject:"<<endl;
    break;
  case P2Pvideocall_IND_STATUS_ANSWERED:
    cout<<"Remote guy has received this video call!"<<endl;
    break;
  case P2Pvideocall_IND_STATUS_HANGUPED:
    cout<<"Remote guy has hangup this video call!"<<endl;
    break;
  default:
    break;
  }
  cout<<"leave sdk callback notifyP2pvideocallStatus!"<<endl;
}



//通知用户普通短消息的服务接收实现
void TDDisplayMnager::notifySMS(SdsMessageIndicator * sdsmsg)
{
  //const std::string EXMPP_MSG_TYPE_NORMAL     = "0001";
  //const std::string EXMPP_MSG_TYPE_STATUS     = "0002";
  //const std::string EXMPP_MSG_TYPE_ALERT      = "0003";
  //const std::string EXMPP_MSG_TYPE_MMS        = "0004";
  //const std::string EXMPP_MSG_TYPE_DROP       = "0005";
  //const std::string EXMPP_MSG_TYPE_ACK        = "0010";

    //add by xy(xwx656025) 2019/5/21 解决除中英文乱码问题。接收时解码码
    std::string str = sdsmsg->getSdsContent();
    int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

    wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
    memset(pwBuf, 0, nwLen * 2 + 2);

    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

    int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

    char * pBuf = new char[nLen + 1];
    memset(pBuf, 0, nLen + 1);

    WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

    std::string retStr = pBuf;

    delete []pBuf;
    delete []pwBuf;

    pBuf = NULL;
    pwBuf = NULL;
    //end by xy 2019/5/21

  if (sdsmsg->getSdsType()==EXMPP_MSG_TYPE_NORMAL)
  {
    cout<<"Sds  EXMPP_MSG_TYPE_NORMAL information\n"<<sdsmsg->getSdsFrom()<<sdsmsg->getSdsTo()
      <<sdsmsg->getSdsDate()<<retStr<<sdsmsg->getRetCode()<<endl;
  }
  else if(sdsmsg->getSdsType()==EXMPP_MSG_TYPE_STATUS)
  {
    printf("Sds  EXMPP_MSG_TYPE_STATUS information \n");
  }
  else if (EXMPP_MSG_TYPE_ENCRYPT_NORMAL == sdsmsg->getSdsType())
  {
    cout<<"receive encrypted SMS message!"<<endl;
    //获取caller发来的秘钥
    const std::string key = sdsmsg->getSdsCmkey();
    std::string encryptedContent = sdsmsg->getSdsContent();
    cout<<"encryptedContent:"<<encryptedContent<<",key:"<<key<<endl;
    //用此key来获得index，然后用此index向CM进行bb07解密内容
    std::string decryptedContent = "";
    //CMDHandler::DecryptSDSContent(key, encryptedContent, decryptedContent);
    SDK_Helper::decryptSDSContent(key, encryptedContent, decryptedContent);
    printf("\n receive short message=%s\n", decryptedContent.c_str());

  }
  else
  {
    printf("notifySMS Finsh!");
  }
}

//通知用户状态短消息的服务接收实现
void TDDisplayMnager::notifyUserSdsStatus(SdsMessageIndicator * sdsmsg)
{
  if(sdsmsg->getSdsType()==EXMPP_MSG_TYPE_STATUS)
  {
    printf("Sds  EXMPP_MSG_TYPE_STATUS information received\n");
  }
  else
  {
    printf("notifyUserSdsStatus is callback-ed but NOT a status message!");
  }
}

void TDDisplayMnager::notifyResourceStatus(ResourceStatusIndicator * resourceStatus)
{
  cout<<"notify Resource Status"<<endl;
  if(resourceStatus == NULL)
  {
    cout<<"notify Resource Status but the pointer is nullptr"<<endl;
    return;
  }
  cout<<"status type="<<resourceStatus->getStatusType()<<endl;
  if(resourceStatus->getStatusType() == GRPPATCHSTATUS)
  {
    cout<<"patch group id="<<resourceStatus->getResId()<<","<<resourceStatus->getAttachingGrp()<<endl;
  }
  if(resourceStatus->getStatusType() == USERDGNASTATUS)
  {
    cout<<"recv dynamic group message, dynamic groupid="<<resourceStatus->getAttachingGrp()<<",resource id="<<resourceStatus->getResId()<<endl;
    ResourceInfo res(resourceStatus->getAttachingGrp());

    int ret = SDK_Helper::execsynchOperation(&res, GRP_SUB);
    cout<<""<<endl;
  }


  if (RESREGSTATUS_PROXY == resourceStatus->getStatusType())
  {
    auto value = resourceStatus->getStatusValue();
    if(STATUS_REGOK == value)
    {
      TDHelper::getinstance()->signalLoginStatus(true,STATUS_REGOK);
    }
    else
    {
      if (STATUS_UNAUTHORIZED != value)
      {
        TDHelper::getinstance()->signalLoginStatus(false,value);
      }

    }
  }

}

void TDDisplayMnager::notifyP2pcallStatus(P2pcallStatusIndicator * callStatus)
{
  cout<<"notify P2p_call status"<<endl;
  if(callStatus == NULL)
  {
    cout<<"notify P2p_call Status but the pointer is nullptr"<<endl;
    return;
  }
  cout<<"status type="<< callStatus->getStatus()<<endl;
  if(callStatus->getStatus() == P2P_IND_STATUS_RECEIVED)
  {
    if (callStatus->getEncryptFalg())
    {
      cout<<"this is an encrypted voice call"<<endl;
    }
    cout<<"call from: "<<callStatus->getCaller()
      <<", and the caller's ip is: "<<callStatus->getRemoteIP()<<endl;

  }
  if(callStatus->getStatus() == P2P_IND_STATUS_RELEASED_BUSY)
  {
    cout<<"call already send out,but callee is busy now\n"<<endl;
    cout<<"your callid is: "<<callStatus->getCallID()<<endl;
  }
  if(callStatus->getStatus() == P2P_IND_STATUS_PROCEEDING)
  {
    cout<<"waiting for callee's answer\n"<<endl;
    cout<<"your callid is: "<<callStatus->getCallID();
  }
  if(callStatus->getStatus() == P2P_IND_STATUS_REMOTE_NOANSWER)
  {
    cout<<"remote callee didn't answer your call\n"<<endl;
    cout<<"calllid is: "<<callStatus->getCallID();
  }
}

void TDDisplayMnager::notifyGroupStatus(GrpDspInfo * grpInfo)
{
  if(grpInfo == NULL)
  {
    cout<<"notify group status,but the groupinfo ptr is nullptr"<<endl;
    return;
  }

  if (grpInfo->isMultiGrp())
  {
    cout<<endl<<"is multiple selected group notify"<<endl;

    auto status = grpInfo->getSpStatus();
    cout<<endl<<"status="<<status<<endl;
    if (MULTI_PTT_IND_STATUS_DIAL_OK == status)
    {
      cout<<endl<<"MULTI_PTT_IND_STATUS_DIAL_OK"<<endl;
    }
    else if (MULTI_PTT_IND_STATUS_DIAL_FAILED == status)
    {
      cout<<endl<<"MULTI_PTT_IND_STATUS_DIAL_FAILED"<<endl;
    }
    else if (MULTI_PTT_IND_STATUS_RELEASE_OK == status)
    {
      cout<<endl<<"MULTI_PTT_IND_STATUS_RELEASE_OK"<<endl;
    }
    else if (MULTI_PTT_IND_STATUS_RELEASE_FAILED == status)
    {
      cout<<endl<<"MULTI_PTT_IND_STATUS_RELEASE_FAILED"<<endl;
    }
    else if (MULTI_PTT_IND_STATUS_SNATCH_OK== status)
    {
      cout<<endl<<"MULTI_PTT_IND_STATUS_SNATCH_OK"<<endl;

    }
    else if (MULTI_PTT_IND_STATUS_SNATCH_REJECT == status)
    {
      cout<<endl<<"MULTI_PTT_IND_STATUS_SNATCH_REJECT"<<endl;
    }
    else if (MULTI_PTT_IND_STATUS_BEEN_SNATCHED == status)
    {
      cout<<endl<<"MULTI_PTT_IND_STATUS_BEEN_SNATCHED"<<endl;
    }
    else  if (MULTI_PTT_IND_STATUS_TX_IDLE == status)
    {
      cout<<endl<<endl<<"MULTI_PTT_IND_STATUS_TX_IDLE"<<endl;
    }
    else if (MULTI_PTT_IND_STATUS_CLOSED == status)
    {
      cout<<endl<<"MULTI_PTT_IND_STATUS_CLOSED"<<endl;
    }
    else if (PTT_IND_STATUS_TX_BEGIN == status)
    {
      cout<<endl<<"PTT_IND_STATUS_TX_BEGIN somebody is talking!"<<endl;
    }
    else
    {
      cout<<endl<<"not have this status, just ignore!"<<endl;
    }
    cout<<endl<<"speaker is:"<<grpInfo->getSpeakerStr()<<",speakerName is:"<<grpInfo->getSpeakerName()<<endl;
    return;
  }
  if(grpInfo->getSpStatus() == PTT_IND_STATUS_UNKNOWN)
  {
    cout<<endl<<"groupcall status meets unknow problem\n"<<endl;
    cout<<endl<<"the callId of this call is: "<<grpInfo->getCallID();
  }
  if(grpInfo->getSpStatus() == PTT_IND_STATUS_RECEIVED)
  {
    if (grpInfo->getEncFlag())
    {
      cout<<"this is an encrypted group call"<<endl;
      UBP_SDK_DEBUG("this is an encrypted group call");
    }
    cout<<endl<<"groupcall is successfully received\n"<<endl;
    cout<<endl<<"the callid of this call is: "<<grpInfo->getCallID();
  }
  if(grpInfo->getSpStatus() == PTT_IND_STATUS_ACCEPT)
  {
    cout<<endl<<"groupcall is successfully accepted\n"<<endl;
    cout<<endl<<"the callid of this call is: "<<grpInfo->getCallID();
  }
  if(grpInfo->getSpStatus() == PTT_IND_STATUS_WAITING)
  {
    cout<<endl<<"waiting for callee to accept\n"<<endl;
    cout<<endl<<"the callid of this call is: "<<grpInfo->getCallID();
  }

}

void TDDisplayMnager::notifyModuleStatus(DCModuleStatusIndicator * moduleStatus)
{
  if(moduleStatus == NULL)
  {
    cout<<"notify DCModule Status,but the moduleStatus ptr is null\n"<<endl;
    return;
  }
  if(moduleStatus->getModuleStatus() == RESOURCE_STATUS_NORMAL)
  {
    cout<<"DConsole is normally runninng\n"<<endl;
  }
  if(moduleStatus->getModuleStatus() == SDS_CONNECT)
  {
    cout<<"SDS is connected\n"<<endl;
  }
}


void TDDisplayMnager::notifyProvisionChange(ProvChangeIndicator* provch)
{
  auto mo  = provch->getChangeMO();
  if (MO_MultiSelectedGroup == mo)
  {
    ProvChangeIndicator_MultiSelectedGrp* provMultiGp = dynamic_cast<ProvChangeIndicator_MultiSelectedGrp*>(provch);
    auto moType = provch->getChangeType();
    if (CHANGE_ADD == moType)
    {
      auto mulgrp = provMultiGp->multiGrpId;
      std::cout<<"notifyProvisionChange MO_MultiSelectedGroup, multiGrp ISDN:"<<mulgrp<<std::endl;
    }
  }
}




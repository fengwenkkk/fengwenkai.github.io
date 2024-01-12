#include "connectionControl.h"

#include "appcontext.h"
#include "dcstate.h"

#include "iostream"
#include "MRSUDPPortManager.h"

using namespace std;

//进行端口分配
//51000-56999
//int REC_PORT_BEGIN = 51000;

extern unsigned int AllocUDPPort();

bool RecSdpControl::get_Connection_info( ServInfo *Sinfo,ConnInfo *Cinfo )
{
    ApplicationContext * ac = ApplicationContext::getInstance();
    DcState* dc = ac->getConsoleState();
    string loaclIP = dc->getLocalIP();
    unsigned int ulPort;
    Service_type call_type = Sinfo->getServiceType();
    Cinfo->setLocalIp(const_cast<char*>(loaclIP.c_str()));

    Cinfo->setAudioport(0);
    Cinfo->setVideoport(0);

    switch(call_type)
    {
    case PTT:
    case DIAL: //分配语音端口
        //REC_PORT_BEGIN = REC_PORT_BEGIN+2;//此处请根据自己的端口分配策略进行实际分配端口
        ulPort = MrsUDPPortManager::GetInstance().AllocUDPPort(); //AllocUDPPort();
        if (ulPort > 0)
        {
            Cinfo->setAudioport(ulPort);
        }
        break;

    case VIDEO_UPLOAD: //分配视频和语音端口
    case VIDEO_P2P:
        if(!Sinfo->getMute())
        {
            //REC_PORT_BEGIN = REC_PORT_BEGIN+2;
            ulPort = MrsUDPPortManager::GetInstance().AllocUDPPort(); //AllocUDPPort();
            if (ulPort > 0)
            {
                Cinfo->setAudioport(ulPort);
            }
        }

        ulPort = MrsUDPPortManager::GetInstance().AllocUDPPort(); //AllocUDPPort();
        if (ulPort > 0)
        {
            Cinfo->setVideoport(ulPort);
        }

        break;

    default:
        printf("invalid call type=%u\n", call_type);
        break;
    }


  //int call_type = Sinfo->getServiceType();
  //P_RecCallInfo_T call(new RecCallInfo(Sinfo->getCaller(),Sinfo->getCallee(),call_type));
  //call->SetSipCallId(Sinfo->getCallId()); //设置callid

  ////mod by wangzheng wwx70140
  ////Ñ¡ÔñÒ»¸öMRS NODE
  //ubp::mrs::common::P_MrsNodeInfo_T node = ubp::mrs::common::MrsNodeInfoMgr_T::instance()->GetBestDataNode(call_type);
  //if (!node)
  //{
  //  REC_LOG_ERROR("RecSdpControl::get_Connection_info:can not get a valid mrs node.call = "<<call->ToString().c_str());
  //  return false;
  //}

  //Cinfo->setLocalIp(const_cast<char*>(node->Ip().c_str()));

  ////mod by fwx32067, for DTS2015030404300, MRS_C Node Use Media_IP.
  //if(node->NodeType() == 0)              //MRS_C Node
  //{
  //  ServiceMgr *pSvcMgr = ServiceEnvImpl_T::instance()->GetModule<ServiceMgr>();
  //  std::string med_ip = pSvcMgr->MedIP();
  //  Cinfo->setLocalIp(const_cast<char*>(med_ip.c_str()));
  //  REC_LOG_DEBUG("RecSdpControl::get_Connection_info: MRS_C Node reset IP Addr: " << med_ip << ", Node IP: " << node->Ip());
  //}

  //Cinfo->setVideoport(0);
  //Cinfo->setAudioport(0);

  //unsigned int audioPort = 0;
  //unsigned int videoPort = 0;

  //switch(call_type)
  //{
  //case PTT:
  //case DIAL:
  //case VIDEO_P2P:        //分配语音端口
  //  if (!node->RequestRtpPort(audioPort))
  //  {
  //    REC_LOG_ERROR("RecSdpControl::get_Connection_info:reqest audioPort fail.call = "<<call->ToString().c_str());
  //    return false;
  //  }

  //  Cinfo->setAudioport(audioPort);
  //  break;
  //case VIDEO_UPLOAD:

  //  if (!node->RequestRtpPort(videoPort))
  //  {
  //    REC_LOG_ERROR("RecSdpControl::get_Connection_info:reqest videoPort fail.call = "<<call->ToString().c_str());
  //    return false;
  //  }
  //  if (Sinfo->getMute() == 0)
  //  {
  //    if (!node->RequestRtpPort(audioPort))
  //    {
  //      REC_LOG_ERROR("RecSdpControl::get_Connection_info:reqest audioPort fail.call = "<<call->ToString().c_str());
  //      node->ReleaseRtpPort(videoPort);
  //      return false;
  //    }
  //  }

  //  Cinfo->setVideoport(videoPort);
  //  Cinfo->setAudioport(audioPort);
  //  break;
  //default:
  //  REC_LOG_ERROR("RecSdpControl::get_Connection_info:invalid call type call = "<<call->ToString().c_str());
  //  return false;
  //}

  //CallLocalPortInfo local_port_info;
  //local_port_info.node_id = node->NodeId();
  //local_port_info.audio_port = Cinfo->getAudioport();
  //local_port_info.video_port = Cinfo->getVideoport();
  //local_port_info.ip = node->Ip();

  ////mod by fwx32067, for DTS2015030404300, MRS_C Node Use Media_IP.
  //if(node->NodeType() == 0)              //MRS_C Node
  //{
  //  ServiceMgr *pSvcMgr = ServiceEnvImpl_T::instance()->GetModule<ServiceMgr>();
  //  std::string med_ip = pSvcMgr->MedIP();
  //  local_port_info.ip = med_ip;
  //  REC_LOG_DEBUG("RecSdpControl::get_Connection_info: MRS_C Node reset IP in map, Addr: " << med_ip << ", Node IP: " << node->Ip());
  //}

  //P_RecCallPortMonitorObj_T mon_t(new RecCallPortMonitorObj(call));
  //mon_t->local_port_info = local_port_info;
  //RecCallPortMonitor_T::instance()->RegCallPortInfo(mon_t);

  return true;
}

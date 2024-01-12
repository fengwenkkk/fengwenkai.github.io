//#include "StdAfx.h"
#include "OperationProxy.h"
#include "sdk\include\logger.h"
#include "appcontext.h"
#include "operationmanager.h"
#include "provisionmanager.h"
#include "sdk\include\uiparameter.h"
#include "sdk/include/sdk_helper.h"
#include <iostream>
#include "string"
#include <fstream>
#include "TDHelper.h"

using namespace std;

#include <codecvt>
#include <locale>

#include "ACE_wrappers/include/ace/Future.h"

#include "util.h"

OperationProxy::OperationProxy(void)
{
  ApplicationContext * ac = ApplicationContext::getInstance();
  DcState* dc = ac->getConsoleState();
  m_dcid = dc->getDCID();
}


OperationProxy::~OperationProxy(void)
{
}

//===================================================================
//  函数名称：  StartP2PCall
//  功能描述：  用户发起点呼
//  调用清单：
//  被调清单：
//  输入参数：  终端号码或者调度台号码
//  输出参数：
//    返回值：
//  其他说明：
//===================================================================
void OperationProxy::StartP2PCall(MSISDN mobileid, const std::string& cmmsg)
{
  ResourceInfo* resInfo = NULL;
  resInfo = new ResourceInfo(mobileid);
  if (resInfo == NULL)
  {
      return;
  }

  /* 应用程序调用operationmanager来注册
  */

  //ACE_INT32 ret = OperationManager_T::instance()->invokeOp(&resInfo, P2P_DIALOUT);
  OperationManager* om = OperationManager_T::instance();
  if (cmmsg.empty())
  {
    om->invokeOp(resInfo,P2P_DIALOUT);
  }
  else
  {
    om->invokeOp_multipara(resInfo, (void*)(cmmsg.c_str()), ENC_P2P_DIALOUT);
  }
  delete resInfo;
  resInfo = NULL;
}
//===================================================================
//  函数名称：  hangupP2PVoiceCall
//  功能描述：  用户挂断语音通话(全双工、半双工)
//  调用清单：
//  被调清单：
//  输入参数：  终端号码或者调度台号码
//  输出参数：
//    返回值：
//  其他说明：
//===================================================================

void OperationProxy::hangupP2PVoiceCall(MSISDN dest_userid)
{
  ResourceInfo* resInfo = new ResourceInfo(dest_userid);
  if (resInfo == NULL)
  {
      return;
  }
  OperationManager* om = new OperationManager;
  om->invokeOp(resInfo, P2P_HANGUP);
  delete resInfo;
  resInfo = NULL;
}

//===================================================================
//  函数名称：  SubscribeGrp
//  功能描述：  用户发起群组订阅
//  调用清单：
//  被调清单：
//  输入参数：  订阅群组列表
//  输出参数：
//    返回值：
//  其他说明：
//===================================================================
int OperationProxy::SubscribeGrp(MSISDN DCID, DC_LIST64 grouplist)
{
  ResourceInfo* resInfo = NULL;

  resInfo = new ResourceInfo(DCID);
  if (resInfo == NULL)
  {
      return -1;
  }

  /* 应用程序调用operationmanager来注册*/
  //ACE_INT32 ret = OperationManager_T::instance()->invokeOp(&resInfo, P2P_DIALOUT);
  OperationManager* om = OperationManager_T::instance();

  int ret = om->invokeOp_multipara(resInfo,&grouplist,GRP_SUB_BATCH);

  delete resInfo;
  resInfo = NULL;
  return ret;
}

//===================================================================
//  函数名称：  StartP2P_HDP_Call
//  功能描述：  用户发起半双工点呼
//  调用清单：
//  被调清单：
//  输入参数：  终端号码或者调度台号码
//  输出参数：
//    返回值：
//  其他说明：
//===================================================================
void OperationProxy::startP2P_HDP_Call(MSISDN mobileid, std::string& cmmsg)
{
  ResourceInfo* resInfo = NULL;

  resInfo = new ResourceInfo(mobileid);
  if (resInfo == NULL)
  {
      return;
  }

  /* 应用程序调用operationmanager来注册
  */

  //ACE_INT32 ret = OperationManager_T::instance()->invokeOp(&resInfo, P2P_DIALOUT);
  OperationManager* om = OperationManager_T::instance();
  if (cmmsg.empty())
  {
    om->invokeOp(resInfo,P2P_HALFDPX_DIAL);
  }
  else
  {
    UBP_SDK_DEBUG("startP2P ENC HDP_Call");
    om->invokeOp_multipara(resInfo, (void*)(cmmsg.c_str()),  ENC_P2P_HALFDPX_DIAL);
  }

  delete resInfo;
  resInfo = NULL;

}

//===================================================================
//  函数名称：  releaseP2P_HDP_Call
//  功能描述：  释放半双工点呼
//  调用清单：
//  被调清单：
//  输入参数：  终端号码
//  输出参数：
//    返回值：
//  其他说明：
//===================================================================
void OperationProxy::releaseP2P_HDP_Call(MSISDN mobileid)
{
  ResourceInfo* resInfo = NULL;

  resInfo = new ResourceInfo(mobileid);
  if (resInfo == NULL)
  {
      return;
  }
  /* 应用程序调用operationmanager来注册
  */

  //ACE_INT32 ret = OperationManager_T::instance()->invokeOp(&resInfo, P2P_DIALOUT);
  OperationManager* om = OperationManager_T::instance();
  om->invokeOp(resInfo,P2P_HALFDPX_RELEASE);

  delete resInfo;
  resInfo = NULL;
}

//===================================================================
//  函数名称：  SubscribeGIS
//  功能描述：  向用户发起GIS订阅和去订阅消息
//  调用清单：
//  被调清单：
//  输入参数：  终端号码
//  输出参数：
//    返回值：
//  其他说明：
//===================================================================
void OperationProxy::SubscribeGIS(const std::string& sublist,bool bSub,const std::string& serverIP)
{
  ResourceInfo* resInfo = NULL;
  resInfo = new ResourceInfo(m_dcid);
  if (resInfo == NULL)
  {
      return;
  }
  cout<<"SubscribeGIS dcid="<<m_dcid<<endl;

  /* 应用程序调用operationmanager来注册
  */
  OperationManager * om = OperationManager_T::instance();
  GIS_parameter gis;
  if (bSub)
    gis.subtype=MsgSUBGIS;
  else
    gis.subtype=MsgUNSUBGIS;

  gis.msg_body= sublist;   //   关注的终端列表，以分号分隔

  //sprintf_s(sub,sizeof(sub),"subscribe@broadcast.%s",serverIP); //请按这样的格式填写subscriber
  std::string sub = "subscribe@broadcast." + serverIP;
  gis.subscriber=sub;

  int ret=om->invokeOp_multipara(resInfo,&gis,GIS_SUBSCRIBE);

  delete resInfo;
  resInfo = NULL;

}

//===================================================================
//  函数名称：  SdsSendMessage
//  功能描述：  向用户发送短消息
//  调用清单：
//  被调清单：
//  输入参数：  终端号码或者调度台号码
//  输出参数：
//    返回值：
//  其他说明：
//===================================================================
void OperationProxy::SdsSendMessage(MSISDN mobileid, char*& serverIP, std::string& msg, std::string& calleeKey)
{
  ResourceInfo* resInfo = NULL;
  resInfo = new ResourceInfo(mobileid);
  if (resInfo == NULL)
  {
      return;
  }

  /*
  应用程序调用operationmanager来注册
  */
  OperationManager * om = OperationManager_T::instance();

  //char receiver[100]={0};
  //sprintf_s(receiver,sizeof(receiver),"%d@broadcast.%s",mobileid,serverIP);//请按这样的格式填写receiver

  //add by xy(xwx656025) 2019/5/21 解决除中英文乱码问题。发送时转码
  std::string str = msg;
  int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

  wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
  ZeroMemory(pwBuf, nwLen * 2 + 2);

  ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

  int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

  char * pBuf = new char[nLen + 1];
  ZeroMemory(pBuf, nLen + 1);

  ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

  std::string retStr(pBuf);

  delete []pwBuf;
  delete []pBuf;

  pwBuf = NULL;
  pBuf = NULL;
  //end by xy 2019/5/21

  SDS_parameter pSds;
  pSds.msg_body=retStr;
  stringstream userid;
  userid<<mobileid;
  string s=userid.str();
  pSds.receiver=s;
  pSds.cmkey = calleeKey;
  if (calleeKey.empty())
  {
    pSds.sdstype=EXMPP_MSG_TYPE_NORMAL;
  }
  else
  {
    pSds.sdstype=EXMPP_MSG_TYPE_ENCRYPT_NORMAL;
  }

  //om->invokeOp_multipara(resInfo,&pSds,SDS_SEND);

  SDK_Helper::execsynchOperation(resInfo, SDS_SEND, &pSds);

  delete resInfo;
  resInfo = NULL;

}

//===================================================================
//  函数名称：  SdsSendMMS
//  功能描述：  向用户发送彩信
//  调用清单：
//  被调清单：
//  输入参数：  终端号码或者调度台号码
//  输出参数：
//    返回值：
//  其他说明：
//===================================================================
void OperationProxy::SdsSendMMS(MSISDN mobiled, const char* serverIP, const bool encryptd /* = 0 */)
{
  ResourceInfo* resInfo = NULL;

  resInfo = new ResourceInfo(mobiled);
  if (resInfo == NULL)
  {
      return;
  }

  /* 应用程序调用operationmanager来注册
  */
  OperationManager * om = OperationManager_T::instance();

  char receiver[100]={0};
  sprintf_s(receiver,sizeof(receiver),"%d@broadcast.%s",mobiled,serverIP);  //请按这样的格式填写receiver

  SDS_parameter pSds;
  pSds.msg_body=" you need  send  my infromation .";
  pSds.receiver=receiver;
  if (!encryptd)
  {
    pSds.sdstype=EXMPP_MSG_TYPE_MMS;
  }
  else
  {
    pSds.sdstype = EXMPP_MSG_TYPE_ENCRYPT_MMS;
  }

  //pSds.files.push_back("C:\\wangshiwei\\svn_4.0\\code\\src\\dc2\\sdk\\examples\\MdcProxy\\MdcProxy.cpp");       //取固定文档方便测试,使用时需指定好文档路径
  std::string file_path = "C:\\Temp\\测试中文.txt";
  std::string utf_string_path = TDHelper::getinstance()->stringToUTF8(file_path);
  UBP_SDK_DEBUG("file_path="<<file_path<<",utf_string_path="<<utf_string_path);
  pSds.files.push_back(utf_string_path);       //取固定文档方便测试,使用时需指定好文档路径
  om->invokeOp_multipara(resInfo,&pSds,SDS_SEND);

  delete resInfo;
  resInfo = NULL;
}

//===================================================================
//  函数名称：  SdsSendMMS
//  功能描述：  向用户发送彩信
//  调用清单：
//  被调清单：
//  输入参数：  终端号码或者调度台号码
//  输出参数：
//    返回值：
//  其他说明：
//===================================================================
#define MAX_ENC_LENGTH 1024
#if 0
void OperationProxy::EncSdsSendMMS(MSISDN mobileid,const char* serverIP,std::string& msg,int index,std::string& calleeKey)
{
  ResourceInfo* resInfo = NULL;

  resInfo = new ResourceInfo(mobileid);
  if (resInfo == NULL)
  {
      return;
  }

  /* 应用程序调用operationmanager来注册
  */
  OperationManager * om = OperationManager_T::instance();

  char receiver[100]={0};
  sprintf_s(receiver,sizeof(receiver),"%d@broadcast.%s",mobileid,serverIP); //请按这样的格式填写receiver

  SDS_parameter pSds;
  pSds.msg_body=msg;
  pSds.receiver=receiver;
  pSds.sdstype = EXMPP_MSG_TYPE_ENCRYPT_MMS;
  pSds.cmkey = calleeKey;
  //pSds.files.push_back("C:\\wangshiwei\\svn_4.0\\code\\src\\dc2\\sdk\\examples\\MdcProxy\\MdcProxy.cpp");       //取固定文档方便测试,使用时需指定好文档路径
  std::string file_path = "C:\\temp\\test.jpg";
  std::string utf_string_path = TDHelper::getinstance()->stringToUTF8(file_path);

  std::string file_path_new = "C:\\temp\\test1.jpg";
  std::string utf_string_path_new = TDHelper::getinstance()->stringToUTF8(file_path_new);
  ifstream fin(utf_string_path.c_str(), ios::ios_base::binary);
  ofstream fout(utf_string_path_new.c_str(),ios::ios_base::binary);
  if(index==-1)
  {
      int tempindex = getCallerSDSIndex(receiver,transactionId);
      KM_LOG_DEBUG("EncryptSDSContent getCallerSDSIndex ret:"<<tempindex);
      printf("EncryptSDSContent getCallerSDSIndex ret:%d\n",tempindex);
      if (-1 == tempindex)
      {
        KM_LOG_ERROR("EncryptSDSContent getCallerSDSIndex error!");
        return ;
      }
      index=tempindex;
  }
  CMDHandler::EncSdsSendMMS(index,fin,fout);
  fin.close();
  fout.close();

  UBP_SDK_DEBUG("file_path="<<file_path<<",utf_string_path="<<utf_string_path);
  //pSds.files.push_back(utf_string_path_new);       //取固定文档方便测试,使用时需指定好文档路径
  om->invokeOp_multipara(resInfo,&pSds,SDS_SEND);
#if 0
  ResourceInfo* resInfo = NULL;
  resInfo = new ResourceInfo(mobileid);
  if (resInfo == NULL)
  {
      return;
  }

  /*
  应用程序调用operationmanager来注册
  */
  OperationManager * om = OperationManager_T::instance();

  //char receiver[100]={0};
  //sprintf_s(receiver,sizeof(receiver),"%d@broadcast.%s",mobileid,serverIP);//请按这样的格式填写receiver

  SDS_parameter pSds;
  pSds.msg_body=msg;
  stringstream userid;
  userid<<mobileid;
  string s=userid.str();
  pSds.receiver=s;
  pSds.cmkey = calleeKey;
  if (calleeKey.empty())
  {
    pSds.sdstype=EXMPP_MSG_TYPE_NORMAL;
  }
  else
  {
    pSds.sdstype=EXMPP_MSG_TYPE_ENCRYPT_MMS;
  }

  //om->invokeOp_multipara(resInfo,&pSds,SDS_SEND);

  SDK_Helper::execsynchOperation(resInfo, SDS_SEND, &pSds);
#endif

  delete resInfo;
  resInfo = NULL;
}
#endif

//===================================================================
//  函数名称：  StartP2pVideo
//  功能描述：  发起视频回传
//  调用清单：
//  被调清单：
//  输入参数：  摄像头号码或者终端号码
//  输出参数：  返回摄像头或者终端的号码
//    返回值：,
//  其他说明：
//===================================================================
void OperationProxy::StartP2pVideo(MSISDN mobileid)
{
  ResourceInfo* resInfo = NULL;
  resInfo = new ResourceInfo(mobileid);
  if (resInfo == NULL)
  {
      return;
  }

  /*应用程序调用operationmanager来注册
  */
  OperationManager * om = OperationManager_T::instance();
  Video_UL_parameter VideoParam;
  VideoParam.cameratype = "1";
  VideoParam.fmtvalue = "D1";
  VideoParam.mutetype = "0";
  VideoParam.user_confirm_type = "1";
  VideoParam.ptztype = "0";
  int ret = om->invokeOp_multipara(resInfo, &VideoParam, P2P_VIDEO_MONITOR);

  delete resInfo;
  resInfo = NULL;
}

//===================================================================
//  函数名称：  StopP2pVideo
//  功能描述：  终止视频回传
//  调用清单：
//  被调清单：
//  输入参数：  终端号码
//  输出参数：
//    返回值：
//  其他说明：
//===================================================================
void OperationProxy::StopP2pVideo(MSISDN mobileid)
{
  SDK_Helper::scheduleAsynchOperation(&ResourceInfo(mobileid), P2P_VIDEO_HANGUP);
}

//===================================================================
//  函数名称：  VideoRecv
//  功能描述：  接收视频回传
//  调用清单：
//  被调清单：
//  输入参数：  对端号码
//  输出参数：
//    返回值：
//  其他说明：
//===================================================================
void OperationProxy::VideoRecv(MSISDN mobileid)
{
  SDK_Helper::scheduleAsynchOperation(&ResourceInfo(mobileid), P2P_VIDEO_RECV);
}

//===================================================================
//  函数名称：  recv_dail
//  功能描述：  点呼接听
//  调用清单：
//  被调清单：
//  输入参数：  呼叫方号码
//  输出参数：
//    返回值：
//  其他说明：
//===================================================================
int OperationProxy::recv_dail(std::string& from)
{
  cout<<"to recv dial call"<<endl;
  OperationManager* om = OperationManager_T::instance();
  int ret = -1;
  ResourceInfo* resInfo = NULL;
  resInfo = new ResourceInfo(atoi(from.c_str()));
  if (resInfo == NULL)
  {
      return -1;
  }
  ret = om->invokeOp(resInfo, P2P_RECV);
  cout<<"ret = "<<ret<<endl;
  return ret;

  delete resInfo;
  resInfo = NULL;
}

//===================================================================
//  函数名称：  loadProvision
//  功能描述：  发起录音录像路径查询
//  调用清单：
//  被调清单：
//  输入参数：  查询的开始时间和结束时间按如下方式填写"2014-01-22 11:24:06"
//  输出参数：
//    返回值：
//  其他说明：
//===================================================================
void OperationProxy::loadProvision(std::string Start_sec,std::string End_Sec)
{
  ProvisionManager * prov = ProvisionManager_T::instance();
  prov->initMRS(g_stIniCfg.acServerAddr);//初始化MRS录像系统
  DcProperty* dcprop=prov->getDcProperty(g_ulDCUserID);

  if(dcprop!=NULL)
  {
    printf("DC alias is %s \r\n",dcprop->Alias.c_str());
  }
  ///////测试getUserRec如需要可以去除注释使用测试////////////////////////////////////////////////
  RECQueryInfo* cond=new RECQueryInfo();
  if (cond == NULL)
  {
      return;
  }
  //下面是查询条件的填写，请按自己的需求填写这些参数
  cond->call_type = ENUM_REC_UNKNOW_CALL_TYPE;
  cond->caller=-1;
  cond->callee=-1;
  //cond->resourceid = 1009;
  cond->start_sec=Start_sec;
  cond->end_sec=End_Sec;
  //获取查询的结果存入一个List中
  UserRecFileInfoList* reclist=prov->getUserRecFileInfoList(cond);
  //打印出录音文件的每一个录音日期
  if(!reclist)
  {
    printf("Rec NULL\r\n");
  }
  else
  {
    for (UserRecFileInfoList::const_iterator it = reclist->begin(); it!=reclist->end();it++)
    {
      //////////////如需测试打印请启用如下代码////////////////////////////
      printf("%s Rec Time\r\n",(*it)->start_sec.c_str());
    }
  }
  /////////////////////////////////////////////////////////////////////////////////////

  delete cond;
  cond = NULL;
}

//===================================================================
//  函数名称：  addMembersIntoDynamicGroup
//  功能描述：  动态组添加成员
//  调用清单：
//  被调清单：
//  输入参数：  成员号码
//  输出参数：
//    返回值：
//  其他说明：
//===================================================================
int OperationProxy::addMembersIntoDynamicGroup(const std::string& input)
{
  ApplicationContext * ac = ApplicationContext::getInstance();
  DcState* dc = ac->getConsoleState();

  istringstream input_string(input);
  string dump = "";
  string members = "";
  int groupid=0;
  input_string>>dump>>members>>dump>>groupid;
  cout<<"mem="<<members<<endl;
  StringVec sv;
  DGNA_Modify_parameter mod_dng;
  sv = TDHelper::getinstance()->splitStr(":",members);
  for(int i=0; i!=sv.size();++i)
  {
    mod_dng.addList[i] = atoi(sv[i].c_str());
  }
  cout<<"groupid="<<groupid<<endl;

  mod_dng.grpId = groupid;
  mod_dng.dcId = dc->getDCID();

  int ret = SDK_Helper::execsynchOperation(&ResourceInfo(1001),DGNA_EDIT, &mod_dng);
  return ret;
}

//===================================================================
//  函数名称：  delMembersFromDynamicGroup
//  功能描述：  动态组删除成员
//  调用清单：
//  被调清单：
//  输入参数：  成员号码
//  输出参数：
//    返回值：
//  其他说明：
//===================================================================
int OperationProxy::delMembersFromDynamicGroup(const std::string& input)
{
  ApplicationContext * ac = ApplicationContext::getInstance();
  DcState* dc = ac->getConsoleState();

  istringstream input_string(input);
  string dump = "";
  string members = "";
  int groupid=0;
  input_string>>dump>>members>>dump>>groupid;
  cout<<"members="<<members<<endl;

  StringVec sv;
  DGNA_Modify_parameter mod_dng;
  sv = TDHelper::getinstance()->splitStr(":",members);
  for(int i=0; i!=sv.size();++i)
  {
    mod_dng.delList[i] = atoi(sv[i].c_str());
  }
  cout<<"groupid="<<groupid<<endl;
  mod_dng.grpId = groupid;
  mod_dng.dcId = dc->getDCID();

  int ret = SDK_Helper::execsynchOperation(&ResourceInfo(dc->getDCID()),DGNA_EDIT, &mod_dng);
  return ret;
}

//===================================================================
//  函数名称：  startP2PVideoDial
//  功能描述：  发起视频点呼
//  调用清单：
//  被调清单：
//  输入参数：  终端号码
//  输出参数：
//    返回值：
//  其他说明：
//===================================================================
void OperationProxy::startP2PVideoDial(MSISDN mobileid)
{
  Video_UL_parameter vlp;
  vlp.fmtvalue = "CIF";
  vlp.mutetype = "0";
  vlp.cameratype = "1";
  vlp.user_confirm_type = "1";
  SDK_Helper::execsynchOperation(&ResourceInfo(mobileid),P2P_VIDEO_DIAL, &vlp);
}

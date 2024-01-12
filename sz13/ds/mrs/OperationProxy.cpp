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
//  �������ƣ�  StartP2PCall
//  ����������  �û�������
//  �����嵥��
//  �����嵥��
//  ���������  �ն˺�����ߵ���̨����
//  ���������
//    ����ֵ��
//  ����˵����
//===================================================================
void OperationProxy::StartP2PCall(MSISDN mobileid, const std::string& cmmsg)
{
  ResourceInfo* resInfo = NULL;
  resInfo = new ResourceInfo(mobileid);
  if (resInfo == NULL)
  {
      return;
  }

  /* Ӧ�ó������operationmanager��ע��
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
//  �������ƣ�  hangupP2PVoiceCall
//  ����������  �û��Ҷ�����ͨ��(ȫ˫������˫��)
//  �����嵥��
//  �����嵥��
//  ���������  �ն˺�����ߵ���̨����
//  ���������
//    ����ֵ��
//  ����˵����
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
//  �������ƣ�  SubscribeGrp
//  ����������  �û�����Ⱥ�鶩��
//  �����嵥��
//  �����嵥��
//  ���������  ����Ⱥ���б�
//  ���������
//    ����ֵ��
//  ����˵����
//===================================================================
int OperationProxy::SubscribeGrp(MSISDN DCID, DC_LIST64 grouplist)
{
  ResourceInfo* resInfo = NULL;

  resInfo = new ResourceInfo(DCID);
  if (resInfo == NULL)
  {
      return -1;
  }

  /* Ӧ�ó������operationmanager��ע��*/
  //ACE_INT32 ret = OperationManager_T::instance()->invokeOp(&resInfo, P2P_DIALOUT);
  OperationManager* om = OperationManager_T::instance();

  int ret = om->invokeOp_multipara(resInfo,&grouplist,GRP_SUB_BATCH);

  delete resInfo;
  resInfo = NULL;
  return ret;
}

//===================================================================
//  �������ƣ�  StartP2P_HDP_Call
//  ����������  �û������˫�����
//  �����嵥��
//  �����嵥��
//  ���������  �ն˺�����ߵ���̨����
//  ���������
//    ����ֵ��
//  ����˵����
//===================================================================
void OperationProxy::startP2P_HDP_Call(MSISDN mobileid, std::string& cmmsg)
{
  ResourceInfo* resInfo = NULL;

  resInfo = new ResourceInfo(mobileid);
  if (resInfo == NULL)
  {
      return;
  }

  /* Ӧ�ó������operationmanager��ע��
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
//  �������ƣ�  releaseP2P_HDP_Call
//  ����������  �ͷŰ�˫�����
//  �����嵥��
//  �����嵥��
//  ���������  �ն˺���
//  ���������
//    ����ֵ��
//  ����˵����
//===================================================================
void OperationProxy::releaseP2P_HDP_Call(MSISDN mobileid)
{
  ResourceInfo* resInfo = NULL;

  resInfo = new ResourceInfo(mobileid);
  if (resInfo == NULL)
  {
      return;
  }
  /* Ӧ�ó������operationmanager��ע��
  */

  //ACE_INT32 ret = OperationManager_T::instance()->invokeOp(&resInfo, P2P_DIALOUT);
  OperationManager* om = OperationManager_T::instance();
  om->invokeOp(resInfo,P2P_HALFDPX_RELEASE);

  delete resInfo;
  resInfo = NULL;
}

//===================================================================
//  �������ƣ�  SubscribeGIS
//  ����������  ���û�����GIS���ĺ�ȥ������Ϣ
//  �����嵥��
//  �����嵥��
//  ���������  �ն˺���
//  ���������
//    ����ֵ��
//  ����˵����
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

  /* Ӧ�ó������operationmanager��ע��
  */
  OperationManager * om = OperationManager_T::instance();
  GIS_parameter gis;
  if (bSub)
    gis.subtype=MsgSUBGIS;
  else
    gis.subtype=MsgUNSUBGIS;

  gis.msg_body= sublist;   //   ��ע���ն��б��Էֺŷָ�

  //sprintf_s(sub,sizeof(sub),"subscribe@broadcast.%s",serverIP); //�밴�����ĸ�ʽ��дsubscriber
  std::string sub = "subscribe@broadcast." + serverIP;
  gis.subscriber=sub;

  int ret=om->invokeOp_multipara(resInfo,&gis,GIS_SUBSCRIBE);

  delete resInfo;
  resInfo = NULL;

}

//===================================================================
//  �������ƣ�  SdsSendMessage
//  ����������  ���û����Ͷ���Ϣ
//  �����嵥��
//  �����嵥��
//  ���������  �ն˺�����ߵ���̨����
//  ���������
//    ����ֵ��
//  ����˵����
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
  Ӧ�ó������operationmanager��ע��
  */
  OperationManager * om = OperationManager_T::instance();

  //char receiver[100]={0};
  //sprintf_s(receiver,sizeof(receiver),"%d@broadcast.%s",mobileid,serverIP);//�밴�����ĸ�ʽ��дreceiver

  //add by xy(xwx656025) 2019/5/21 �������Ӣ���������⡣����ʱת��
  std::string str = msg;
  int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

  wchar_t * pwBuf = new wchar_t[nwLen + 1];//һ��Ҫ��1����Ȼ�����β��
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
//  �������ƣ�  SdsSendMMS
//  ����������  ���û����Ͳ���
//  �����嵥��
//  �����嵥��
//  ���������  �ն˺�����ߵ���̨����
//  ���������
//    ����ֵ��
//  ����˵����
//===================================================================
void OperationProxy::SdsSendMMS(MSISDN mobiled, const char* serverIP, const bool encryptd /* = 0 */)
{
  ResourceInfo* resInfo = NULL;

  resInfo = new ResourceInfo(mobiled);
  if (resInfo == NULL)
  {
      return;
  }

  /* Ӧ�ó������operationmanager��ע��
  */
  OperationManager * om = OperationManager_T::instance();

  char receiver[100]={0};
  sprintf_s(receiver,sizeof(receiver),"%d@broadcast.%s",mobiled,serverIP);  //�밴�����ĸ�ʽ��дreceiver

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

  //pSds.files.push_back("C:\\wangshiwei\\svn_4.0\\code\\src\\dc2\\sdk\\examples\\MdcProxy\\MdcProxy.cpp");       //ȡ�̶��ĵ��������,ʹ��ʱ��ָ�����ĵ�·��
  std::string file_path = "C:\\Temp\\��������.txt";
  std::string utf_string_path = TDHelper::getinstance()->stringToUTF8(file_path);
  UBP_SDK_DEBUG("file_path="<<file_path<<",utf_string_path="<<utf_string_path);
  pSds.files.push_back(utf_string_path);       //ȡ�̶��ĵ��������,ʹ��ʱ��ָ�����ĵ�·��
  om->invokeOp_multipara(resInfo,&pSds,SDS_SEND);

  delete resInfo;
  resInfo = NULL;
}

//===================================================================
//  �������ƣ�  SdsSendMMS
//  ����������  ���û����Ͳ���
//  �����嵥��
//  �����嵥��
//  ���������  �ն˺�����ߵ���̨����
//  ���������
//    ����ֵ��
//  ����˵����
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

  /* Ӧ�ó������operationmanager��ע��
  */
  OperationManager * om = OperationManager_T::instance();

  char receiver[100]={0};
  sprintf_s(receiver,sizeof(receiver),"%d@broadcast.%s",mobileid,serverIP); //�밴�����ĸ�ʽ��дreceiver

  SDS_parameter pSds;
  pSds.msg_body=msg;
  pSds.receiver=receiver;
  pSds.sdstype = EXMPP_MSG_TYPE_ENCRYPT_MMS;
  pSds.cmkey = calleeKey;
  //pSds.files.push_back("C:\\wangshiwei\\svn_4.0\\code\\src\\dc2\\sdk\\examples\\MdcProxy\\MdcProxy.cpp");       //ȡ�̶��ĵ��������,ʹ��ʱ��ָ�����ĵ�·��
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
  //pSds.files.push_back(utf_string_path_new);       //ȡ�̶��ĵ��������,ʹ��ʱ��ָ�����ĵ�·��
  om->invokeOp_multipara(resInfo,&pSds,SDS_SEND);
#if 0
  ResourceInfo* resInfo = NULL;
  resInfo = new ResourceInfo(mobileid);
  if (resInfo == NULL)
  {
      return;
  }

  /*
  Ӧ�ó������operationmanager��ע��
  */
  OperationManager * om = OperationManager_T::instance();

  //char receiver[100]={0};
  //sprintf_s(receiver,sizeof(receiver),"%d@broadcast.%s",mobileid,serverIP);//�밴�����ĸ�ʽ��дreceiver

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
//  �������ƣ�  StartP2pVideo
//  ����������  ������Ƶ�ش�
//  �����嵥��
//  �����嵥��
//  ���������  ����ͷ��������ն˺���
//  ���������  ��������ͷ�����ն˵ĺ���
//    ����ֵ��,
//  ����˵����
//===================================================================
void OperationProxy::StartP2pVideo(MSISDN mobileid)
{
  ResourceInfo* resInfo = NULL;
  resInfo = new ResourceInfo(mobileid);
  if (resInfo == NULL)
  {
      return;
  }

  /*Ӧ�ó������operationmanager��ע��
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
//  �������ƣ�  StopP2pVideo
//  ����������  ��ֹ��Ƶ�ش�
//  �����嵥��
//  �����嵥��
//  ���������  �ն˺���
//  ���������
//    ����ֵ��
//  ����˵����
//===================================================================
void OperationProxy::StopP2pVideo(MSISDN mobileid)
{
  SDK_Helper::scheduleAsynchOperation(&ResourceInfo(mobileid), P2P_VIDEO_HANGUP);
}

//===================================================================
//  �������ƣ�  VideoRecv
//  ����������  ������Ƶ�ش�
//  �����嵥��
//  �����嵥��
//  ���������  �Զ˺���
//  ���������
//    ����ֵ��
//  ����˵����
//===================================================================
void OperationProxy::VideoRecv(MSISDN mobileid)
{
  SDK_Helper::scheduleAsynchOperation(&ResourceInfo(mobileid), P2P_VIDEO_RECV);
}

//===================================================================
//  �������ƣ�  recv_dail
//  ����������  �������
//  �����嵥��
//  �����嵥��
//  ���������  ���з�����
//  ���������
//    ����ֵ��
//  ����˵����
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
//  �������ƣ�  loadProvision
//  ����������  ����¼��¼��·����ѯ
//  �����嵥��
//  �����嵥��
//  ���������  ��ѯ�Ŀ�ʼʱ��ͽ���ʱ�䰴���·�ʽ��д"2014-01-22 11:24:06"
//  ���������
//    ����ֵ��
//  ����˵����
//===================================================================
void OperationProxy::loadProvision(std::string Start_sec,std::string End_Sec)
{
  ProvisionManager * prov = ProvisionManager_T::instance();
  prov->initMRS(g_stIniCfg.acServerAddr);//��ʼ��MRS¼��ϵͳ
  DcProperty* dcprop=prov->getDcProperty(g_ulDCUserID);

  if(dcprop!=NULL)
  {
    printf("DC alias is %s \r\n",dcprop->Alias.c_str());
  }
  ///////����getUserRec����Ҫ����ȥ��ע��ʹ�ò���////////////////////////////////////////////////
  RECQueryInfo* cond=new RECQueryInfo();
  if (cond == NULL)
  {
      return;
  }
  //�����ǲ�ѯ��������д���밴�Լ���������д��Щ����
  cond->call_type = ENUM_REC_UNKNOW_CALL_TYPE;
  cond->caller=-1;
  cond->callee=-1;
  //cond->resourceid = 1009;
  cond->start_sec=Start_sec;
  cond->end_sec=End_Sec;
  //��ȡ��ѯ�Ľ������һ��List��
  UserRecFileInfoList* reclist=prov->getUserRecFileInfoList(cond);
  //��ӡ��¼���ļ���ÿһ��¼������
  if(!reclist)
  {
    printf("Rec NULL\r\n");
  }
  else
  {
    for (UserRecFileInfoList::const_iterator it = reclist->begin(); it!=reclist->end();it++)
    {
      //////////////������Դ�ӡ���������´���////////////////////////////
      printf("%s Rec Time\r\n",(*it)->start_sec.c_str());
    }
  }
  /////////////////////////////////////////////////////////////////////////////////////

  delete cond;
  cond = NULL;
}

//===================================================================
//  �������ƣ�  addMembersIntoDynamicGroup
//  ����������  ��̬����ӳ�Ա
//  �����嵥��
//  �����嵥��
//  ���������  ��Ա����
//  ���������
//    ����ֵ��
//  ����˵����
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
//  �������ƣ�  delMembersFromDynamicGroup
//  ����������  ��̬��ɾ����Ա
//  �����嵥��
//  �����嵥��
//  ���������  ��Ա����
//  ���������
//    ����ֵ��
//  ����˵����
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
//  �������ƣ�  startP2PVideoDial
//  ����������  ������Ƶ���
//  �����嵥��
//  �����嵥��
//  ���������  �ն˺���
//  ���������
//    ����ֵ��
//  ����˵����
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

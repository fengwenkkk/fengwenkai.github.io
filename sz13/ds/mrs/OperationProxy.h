//#include "sdk\include\logger.h"
#include "logger.h"
#include "appcontext.h"
#include "operationmanager.h"

#include "sdk\include\uiparameter.h"
#include "sdk/include/mq/modulestatus_indicator.h"

#include "sdk\include\logger.h"

#include "sdk/include/mq/resourcestatus_indicator.h"
typedef std::vector<std::string> StringVec;

#pragma once


class OperationProxy
{
public:
  OperationProxy(void);
  ~OperationProxy(void);
  //p2p voice dial
  void StartP2PCall(MSISDN mobileid, const std::string& cmmsg);
  void hangupP2PVoiceCall(MSISDN dest_userid);

  //p2p half voice dial
  void startP2P_HDP_Call(MSISDN mobileid, std::string& cmmsg);
  void releaseP2P_HDP_Call(MSISDN mobileid);

  //send message, 0: unencrypted, 1: encrypted
  void  SdsSendMessage(MSISDN mobileid, char*& serverIP, std::string& msg, std::string& calleeKey);
  void SdsSendMMS(MSISDN mobiled, const char* serverIP, const bool encryptd = 0);
  void EncSdsSendMMS(MSISDN mobileid,const char* serverIP,std::string& msg,int index,std::string& calleeKey);
  //subgis
  void SubscribeGIS(const std::string& sublist,bool bSub,const std::string& serverIP);

  //
  //start p2p video dial
  void startP2PVideoDial(MSISDN mobileid);

  void StartP2pVideo(MSISDN mobileid);
  void StopP2pVideo(MSISDN mobileid);
  void loadProvision(std::string Start_sec,std::string End_Sec);
  void VideoRecv(MSISDN mobile);
  int SubscribeGrp(MSISDN DCID, DC_LIST64 grouplist);
  int recv_dail(std::string& from); // recv p2pdial
  int recv_video(std::string& from);// recv p2pvideoCall

  int addMembersIntoDynamicGroup(const std::string& input);
  int delMembersFromDynamicGroup(const std::string& input);

private:
  MSISDN m_dcid;
};

#if 0
class TDHelper
{
public:
  TDHelper(){;}
  ~TDHelper(){;}
  static TDHelper* getinstance(void);
  //以：分隔字符串“1001:1002:1003”
  StringVec splitStr(const char* delims, const std::string& str);
  //将终端号以：拼接在一起,如：from = 1001, to = 1005,则返回1001;1002;1003;1004;1005
  const std::string splicingID(const std::string& from, const std::string& to);

  /*inline */void do_sip_module(DCModuleStatusIndicator* module);

  std::string stringToUTF8(const std::string & str);
  //char* Utf8ToAnsi(const char *utf8);
  //std::string utf8_to_string(const char *utf8str, const locale& loc);
  int CN2Unicode(const char *input,wchar_t *output);
  std::wstring s2ws(const std::string& s);
  std::string wstringToString(const std::wstring& wstr);
  res_status_value_t waitLoginStatus();
  void signalLoginStatus(bool bSuccess, res_status_value_t cause);
private:
  static TDHelper* instance;
};

#endif

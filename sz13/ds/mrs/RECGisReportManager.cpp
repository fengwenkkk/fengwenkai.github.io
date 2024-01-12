#include "RECGisReportManager.h"
#include "mq/gisreportindicator.h"
#include "mq/resourcestatus_indicator.h"
#include "operationmanager.h"
#include "uiparameter.h"

#include <string>
#include "iostream"

using namespace std;

#define FOR_EACH(X, Y)  for(Y = X.begin(); Y != X.end(); Y++)

RECGisReportManager::RECGisReportManager(void)
{
}


RECGisReportManager::~RECGisReportManager(void)
{
}

    //GIS上报数
void RECGisReportManager::notifyGISReport(GISReportIndicator *ind)
{
  std::cout<<"enter notifyGISReport"<<std::endl;
    std::string result=FormatAttribute(ind);
    printf(result.c_str());
}

    //GIS订阅通知
 void RECGisReportManager::notifyGISSubRsp(GISReportIndicator *ind )
 {

     std::string result=FormatAttribute(ind);
    printf(result.c_str());
 }



std::string RECGisReportManager::FormatAttribute(GISReportIndicator * pIndicator)
{
  std::cout<<"enter RECGisReportManager::FormatAttribute"<<std::endl;
    if(!pIndicator) return "";
    std::cout<<"enter RECGisReportManager::FormatAttribute type="<<pIndicator->getMsgType()<<std::endl;
    //-----------------------------------------------------
    //
    //1001:0;1002:1;1003:0;

    string szAckResult = "";
    if (pIndicator->isAck())
    {
      char buf[32];
      AckResultMap& map = pIndicator->getAckRsp();
      AckResultMap::iterator it;
      FOR_EACH(map, it)
      {
        sprintf_s(buf, sizeof(buf), "%d:%d;", it->first, it->second);
        szAckResult += buf;
      }
    }
    else
    {
      szAckResult = "";
    }


    //-----------------------------------------------------
    char str[512] = {0};

    sprintf_s(str,sizeof(str),
        "<Attr name=\"_sender\" value=\"%s\"/>"
        "<Attr name=\"_latitude\" value=\"%s\"/>"
        "<Attr name=\"_longitude\" value=\"%s\"/>"
        "<Attr name=\"_time\" value=\"%s\"/>"
        "<Attr name=\"_msgTp\" value=\"%s\"/>"
        "<Attr name=\"_rspMap\" value=\"%s\"/>"

        , pIndicator->getSender().c_str()
        , pIndicator->getLatitude().c_str()
        , pIndicator->getLongitude().c_str()
        , pIndicator->getTime().c_str()
        , pIndicator->getMsgType().c_str()
        , szAckResult.c_str()
        )
        ;


    return str;
}



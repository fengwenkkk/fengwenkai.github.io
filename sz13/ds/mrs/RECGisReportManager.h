#pragma once
#include "sdk\include\gisreportmanager.h"
#include <string>

class RECGisReportManager :
    public GISReportManager
{
public:
    RECGisReportManager(void);
    virtual ~RECGisReportManager(void);


    //GIS上报数据
    virtual void notifyGISReport(GISReportIndicator *) ;
    //GIS订阅通知
    virtual void notifyGISSubRsp(GISReportIndicator * );
    std::string FormatAttribute(GISReportIndicator * pIndicator);
};


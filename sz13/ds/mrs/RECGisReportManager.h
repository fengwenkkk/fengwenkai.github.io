#pragma once
#include "sdk\include\gisreportmanager.h"
#include <string>

class RECGisReportManager :
    public GISReportManager
{
public:
    RECGisReportManager(void);
    virtual ~RECGisReportManager(void);


    //GIS�ϱ�����
    virtual void notifyGISReport(GISReportIndicator *) ;
    //GIS����֪ͨ
    virtual void notifyGISSubRsp(GISReportIndicator * );
    std::string FormatAttribute(GISReportIndicator * pIndicator);
};


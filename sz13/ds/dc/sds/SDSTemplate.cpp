#include <vcl.h>
#pragma hdrstop

#include "g_include.h"

#include "MsgDialog.h"
#include "msg.h"

#include "SDSTemplate.h"

static MAP<AnsiString, AnsiString>  g_mSDSTemplateParam;
static VECTOR<AnsiString>           g_vSDSTemplateParamName;
static VECTOR<SDSTemplate*>         g_vSDSTemplate;

static VECTOR<PREDEFINED_SMS_INFO_T>   g_vPredefinedSMS;

VOID InitPredefinedSMS(VECTOR<PREDEFINED_SMS_INFO_T> &vInfo)
{
    g_vPredefinedSMS.swap(vInfo);
}

VECTOR<PREDEFINED_SMS_INFO_T>& GetPredefinedSMS()
{
    return g_vPredefinedSMS;
}

void InitSDSTemplateParam(AnsiString strParam, AnsiString strDisplayName)
{
    strParam = AnsiString(TEMPLATE_PARAM_MARK) + strParam + AnsiString(TEMPLATE_PARAM_MARK);

    g_vSDSTemplateParamName.push_back(strParam);
    g_mSDSTemplateParam[strParam] = strDisplayName;
}

void InitSDSTemplateParam()
{
    if (g_mSDSTemplateParam.size() > 0)
    {
        return;
    }

    InitSDSTemplateParam(TEMPLATE_PARAM_STATION,       "车站");
    InitSDSTemplateParam(TEMPLATE_PARAM_DEPOT,         "停车段");
    InitSDSTemplateParam(TEMPLATE_PARAM_FIX,           "维修段");
    InitSDSTemplateParam(TEMPLATE_PARAM_TRAIN,         "列车");
    InitSDSTemplateParam(TEMPLATE_PARAM_PLATFORM,      "站台");
    InitSDSTemplateParam(TEMPLATE_PARAM_DC,            "调度台");
    InitSDSTemplateParam(TEMPLATE_PARAM_STATION_RADIO, "固定台");
    InitSDSTemplateParam(TEMPLATE_PARAM_TRAIN_RADIO,   "车载台");
    InitSDSTemplateParam(TEMPLATE_PARAM_STATION_UE,    "站务人员");
    InitSDSTemplateParam(TEMPLATE_PARAM_TRAIN_UE,      "司乘人员");
}

AnsiString GetSDSTemplateParamDisplayName(AnsiString strName)
{
    if (g_mSDSTemplateParam.find(strName) == g_mSDSTemplateParam.end())
    {
        return "";
    }

    AnsiString  strDisplayName = g_mSDSTemplateParam[strName];

    return strDisplayName;
}

SDSTemplate::SDSTemplate()
{
    InitSDSTemplateParam();
    memset(&m_stTemplate, 0, sizeof(m_stTemplate));
}

BOOL SDSTemplate::Init(SDS_TEMPLATE_T &stTemplate)
{
    memcpy(&m_stTemplate, &stTemplate, sizeof(stTemplate));
    m_vParam.clear();

    if (m_stTemplate.acSDSInfo[0] == '\0')
    {
        memset(&m_stTemplate, 0, sizeof(m_stTemplate));
        m_vParam.clear();

        return FALSE;
    }

    UINT32      i;
    INT32       iPos;
    AnsiString          strParam;
    TEMPLATE_PARAM_T    stParam;
    AnsiString          strCmd = m_stTemplate.acSDSInfo;
    BOOL                bExist;

    while(!strCmd.IsEmpty())
    {
        bExist = FALSE;
        for (i=0; i<g_vSDSTemplateParamName.size(); i++)
        {
            strParam = g_vSDSTemplateParamName[i];
            iPos = strCmd.Pos(strParam);
            if (iPos > 1)
            {
                stParam.ulParamType = TEMPLATE_PARAM_TYPE_TEXT;
                stParam.strParam = strCmd.SubString(1, iPos-1);
                m_vParam.push_back(stParam);
                strCmd = strCmd.SubString(iPos, strCmd.Length()).Trim();

                iPos = 1;//strCmd.Pos(astrParam[i]);
            }

            if (iPos == 1)
            {
                stParam.ulParamType = TEMPLATE_PARAM_TYPE_PARAM;
                stParam.strParam = strCmd.SubString(iPos, strParam.Length());
                m_vParam.push_back(stParam);

                strCmd = strCmd.SubString(strParam.Length()+1, strCmd.Length()).Trim();

                bExist = TRUE;
                break;
            }
        }

        if (bExist)
        {
            continue;
        }

        if (!strCmd.IsEmpty())
        {
            stParam.ulParamType = TEMPLATE_PARAM_TYPE_TEXT;
            stParam.strParam = strCmd;
            m_vParam.push_back(stParam);
            break;
        }
    }

    return TRUE;
}

VOID InitSDSTemplate(VECTOR<SDSTemplate*> &vTemplate)
{
    g_vSDSTemplate.clear();

    for (UINT32 i=0; i<g_vSDSTemplate.size(); i++)
    {
        SDSTemplate *pTemplate = g_vSDSTemplate[i];

        delete pTemplate;
    }

    g_vSDSTemplate.swap(vTemplate);
}

VECTOR<SDSTemplate*>& GetSDSTemplate()
{
    return g_vSDSTemplate;
}

SDSTemplate* GetSDSTemplate(AnsiString strTitle)
{
    for (UINT32 i=0; i<g_vSDSTemplate.size(); i++)
    {
        SDSTemplate *pTemplate = g_vSDSTemplate[i];

        if (strTitle == pTemplate->GetTemplate().acTitle)
        {
            return pTemplate;
        }
    }

    return NULL;
}

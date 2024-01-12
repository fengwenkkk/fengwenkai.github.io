//---------------------------------------------------------------------------

#ifndef SDSTemplateH
#define SDSTemplateH
//---------------------------------------------------------------------------

#define TEMPLATE_PARAM_MARK             "#"

#define TEMPLATE_PARAM_TYPE_TEXT        1       // 文本
#define TEMPLATE_PARAM_TYPE_PARAM       2       // 参数

#define TEMPLATE_PARAM_STATION          "station"
#define TEMPLATE_PARAM_DEPOT            "depot"
#define TEMPLATE_PARAM_FIX              "fix"       // 维修段
#define TEMPLATE_PARAM_TRAIN            "train"
#define TEMPLATE_PARAM_PLATFORM         "platform"

#define TEMPLATE_PARAM_DC               "dc"
#define TEMPLATE_PARAM_STATION_RADIO    "station_radio" // 固定台
#define TEMPLATE_PARAM_TRAIN_RADIO      "train_radio"   // 车载台
#define TEMPLATE_PARAM_STATION_UE       "station_ue"    // 车站手持台
#define TEMPLATE_PARAM_TRAIN_UE         "train_ue"      // 列车司乘人员手持台

typedef struct
{
    UINT32              ulParamType;
    AnsiString          strParam;
}TEMPLATE_PARAM_T;

class SDSTemplate
{
public:
    SDSTemplate();
    BOOL Init(SDS_TEMPLATE_T &stTemplate);
    VECTOR<TEMPLATE_PARAM_T>& GetParam(){return m_vParam;};
    SDS_TEMPLATE_T& GetTemplate(){return m_stTemplate;};

private:
    SDS_TEMPLATE_T              m_stTemplate;
    VECTOR<TEMPLATE_PARAM_T>    m_vParam;
};

VOID InitPredefinedSMS(VECTOR<PREDEFINED_SMS_INFO_T> &vInfo);
VECTOR<PREDEFINED_SMS_INFO_T>& GetPredefinedSMS();

VOID InitSDSTemplate(VECTOR<SDSTemplate*> &vTemplate);
VECTOR<SDSTemplate*>& GetSDSTemplate();
SDSTemplate* GetSDSTemplate(AnsiString strTitle);
AnsiString GetSDSTemplateParamDisplayName(AnsiString strName);


#endif

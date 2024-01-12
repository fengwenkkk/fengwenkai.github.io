//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "omc.h"
#include "ClientInit.h"
#include "msg.h"
#include "DlgImportFile.h"
#include <vector>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxCustomData"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
#pragma link "cxInplaceContainer"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxStyles"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "dxSkinBlack"
#pragma link "dxSkinBlue"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinCaramel"
#pragma link "dxSkinCoffee"
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinDevExpressStyle"
#pragma link "dxSkinFoggy"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinHighContrast"
#pragma link "dxSkiniMaginary"
#pragma link "dxSkinLilian"
#pragma link "dxSkinLiquidSky"
#pragma link "dxSkinLondonLiquidSky"
#pragma link "dxSkinMcSkin"
#pragma link "dxSkinMetropolis"
#pragma link "dxSkinMetropolisDark"
#pragma link "dxSkinMoneyTwins"
#pragma link "dxSkinOffice2007Black"
#pragma link "dxSkinOffice2007Blue"
#pragma link "dxSkinOffice2007Green"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinOffice2007Silver"
#pragma link "dxSkinOffice2010Black"
#pragma link "dxSkinOffice2010Blue"
#pragma link "dxSkinOffice2010Silver"
#pragma link "dxSkinOffice2013DarkGray"
#pragma link "dxSkinOffice2013LightGray"
#pragma link "dxSkinOffice2013White"
#pragma link "dxSkinOffice2016Colorful"
#pragma link "dxSkinOffice2016Dark"
#pragma link "dxSkinPumpkin"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSeven"
#pragma link "dxSkinSevenClassic"
#pragma link "dxSkinSharp"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinSilver"
#pragma link "dxSkinSpringTime"
#pragma link "dxSkinStardust"
#pragma link "dxSkinSummer2008"
#pragma link "dxSkinTheAsphaltWorld"
#pragma link "dxSkinTheBezier"
#pragma link "dxSkinValentine"
#pragma link "dxSkinVisualStudio2013Blue"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxSkinVisualStudio2013Light"
#pragma link "dxSkinVS2010"
#pragma link "dxSkinWhiteprint"
#pragma link "dxSkinXmas2008Blue"
#pragma link "cxTextEdit"
#pragma link "cxButtons"
#pragma link "MDIForm"
#pragma link "cxImageComboBox"
#pragma resource "*.dfm"
TDialogImportFile *DialogImportFile;
//---------------------------------------------------------------------------

extern VectorMap<AnsiString, NE_BASIC_INFO_T>   g_mNEBasicInfo;

extern Dict g_DictDevType;
extern AnsiString GetDevTypeText(UINT32 ulDevType);

__fastcall TDialogImportFile::TDialogImportFile(TComponent* Owner)
    : TDialogForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TDialogImportFile::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

void __fastcall TDialogImportFile::ColumnDevNameGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[1].IsNull())
    {
        return;
    }

    UINT32  ulDevType = ANode->Values[1];

    Value = GetDevTypeText(ulDevType);
}
//---------------------------------------------------------------------------
bool  CheckDevInfo(AnsiString &strDevName,AnsiString &strMAC,AnsiString &strCheckRet)
{
    VectorMap<AnsiString, NE_BASIC_INFO_T>  mNEBasicInfo;
    NEBasic::GetNEBasicInfo(mNEBasicInfo);

   for (UINT32 i=0; i<mNEBasicInfo.Size(); i++)
    {
        NE_BASIC_INFO_T     *pstInfo = mNEBasicInfo.GetValueByIndex(i);

        if (strDevName == pstInfo->acDevName)
        {
            strCheckRet = "设备名称已存在";
            return false;
        }

        if (strMAC == pstInfo->acDevMac)
        {
            strCheckRet = "设备MAC已存在";
            return false;
        }
    }

    return true;
}

bool  TDialogImportFile::ImportFromFile(const char *szCSV)
{
    char     acBuf[1024] = {0};
    UINT32  ulMaxFieldNum = 3;
    FILE    *fp = fopen(szCSV, "rb");
    VectorMap<AnsiString, NE_BASIC_INFO_T>  mNeInfo;
    NE_BASIC_INFO_T stNeInfo = {0};

    if (!fp)
    {
        ErrMsg("打开文件失败");
        return false;
    }

    if(fgets(acBuf, sizeof(acBuf), fp) == NULL)  //跳过表头
    {
        fclose(fp);
        WarnMsg("导入表格为空");
        return false;
    }

    TcxTreeListNode *Node;
    tlImportDev->Clear();
    tlImportDev->BeginUpdate();
    while ((fgets(acBuf,sizeof(acBuf),fp) != NULL))
    {
        std::vector<std::string> vDevInfo;
        char    *p = strtok(acBuf,",");

        while(p)
        {
            vDevInfo.push_back(std::string(p));
            p = strtok(NULL, ",");
        }

        if(vDevInfo.size() != ulMaxFieldNum)
        {
            continue;
        }

        Node = tlImportDev->Add();
        for (UINT32 ulFieldIndex=0; ulFieldIndex < ulMaxFieldNum; ulFieldIndex++)
        {
            Node->Texts[ulFieldIndex] = vDevInfo.at(ulFieldIndex).c_str();
        }

        AnsiString  strDevName = Node->Values[0];
        AnsiString  strDevType = Node->Values[1];
        UINT32      ulDevType =  OMC_DEV_TYPE_ALL;
        g_DictDevType.GetID(strDevType,&ulDevType);
        AnsiString  strMAC = Node->Values[2];
        AnsiString  strCheckRet;

        if(!CheckDevInfo(strDevName, strMAC, strCheckRet))
        {
            Node->Texts[3] = strCheckRet;
            continue;
        }

        GJsonParam  ReqJsonParam;
        ReqJsonParam.Add("MAC", strMAC);
        ReqJsonParam.Add("DevType", ulDevType);
        ReqJsonParam.Add("DevName", strDevName);
        if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_AddNe, ReqJsonParam))
        {
            continue;
        }
        else
        {
            Node->Texts[3] = "成功";
        }
    }

    tlImportDev->EndUpdate();

    fclose(fp);
    return true;
}
//---------------------------------------------------------------------------

void __fastcall TDialogImportFile::btnOKClick(TObject *Sender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TDialogImportFile::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
    SetImage(btnExport, IMG_EXPORT);
}
//---------------------------------------------------------------------------

void __fastcall TDialogImportFile::btnExportClick(TObject *Sender)
{
    if (!SaveDialog->Execute())
    {
        return;
    }

    AnsiString  strFile = SaveDialog->FileName;
    CHAR        *szFilePostfix = gos_get_file_postfix(strFile.c_str());
    bool        bRet;

    if (strcasecmp(szFilePostfix, "xls") == 0)
    {
        bRet = ExportTreeListToExcel(tlImportDev, strFile);
    }
    else if (strcasecmp(szFilePostfix, "csv") == 0)
    {
        bRet = ExportTreeListToCsv(tlImportDev, strFile);
    }
    else
    {
        InfoMsg("无效的导出文件格式");
        return;
    }

    if (!bRet)
    {
        ErrMsg("导出失败");
    }
    else
    {
        InfoMsg("导出成功");
    }
}
//---------------------------------------------------------------------------


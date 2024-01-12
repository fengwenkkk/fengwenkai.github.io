//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "UICfg.h"
#include "ClientData.h"
#include "SDSTemplate.h"

#include "DlgNewSDS.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxLabel"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxTrackBar"
#pragma link "dxSkinBlue"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "cxTextEdit"
#pragma link "cxCheckBox"
#pragma link "cxDropDownEdit"
#pragma link "cxMaskEdit"
#pragma link "cxButtonEdit"
#pragma link "cxCustomData"
#pragma link "cxInplaceContainer"
#pragma link "cxStyles"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "cxMemo"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxBevel"
#pragma link "cxGroupBox"
#pragma link "cxRichEdit"
#pragma link "cxClasses"
#pragma link "cxDBExtLookupComboBox"
#pragma link "cxExtEditRepositoryItems"
#pragma link "cxCheckComboBox"
#pragma link "cxEditRepositoryItems"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxImageList"
#pragma link "cxPC"
#pragma link "dxBarBuiltInMenu"
#pragma resource "*.dfm"
TDialogNewSDS *DialogNewSDS;

void __fastcall TDialogNewSDS::InitTitle(UINT32 ulSDSType)
{
    VECTOR<SDSTemplate*>    &vTemplate = GetSDSTemplate();

    cbTitle->Properties->Items->Clear();

    if (IsSimpleSMSMode())
    {
        cbTitle->Properties->Items->Add("");
    }

    for (UINT32 i=0; i<vTemplate.size(); i++)
    {
        SDS_TEMPLATE_T      &stTemplate = vTemplate[i]->GetTemplate();

        if (stTemplate.ulSDSType == ulSDSType)
        {
            cbTitle->Properties->Items->Add(stTemplate.acTitle);
        }
    }
}

//---------------------------------------------------------------------------
__fastcall TDialogNewSDS::TDialogNewSDS(TComponent* Owner)
    : TDialogForm(Owner)
{
    UINT32  i;

    Height = 700;

    m_DictSDSType = GetDictSDSTemplateType();

    m_DictSDSType->Init(cbSDSType);

    cbTitle->Properties->Items->Clear();

    VECTOR<STATION_INFO_T*> &vStation = Station::GetStationInfo();

    erccbStation->Properties->Items->Clear();
    for (i=0; i<vStation.size(); i++)
    {
        TcxCheckComboBoxItem    *Item = erccbStation->Properties->Items->Add();

        Item->Description = vStation[i]->acStationName;
    }

    ercbStation->Properties->Items->Clear();
    for (i=0; i<vStation.size(); i++)
    {
        ercbStation->Properties->Items->Add(vStation[i]->acStationName);
    }

    VECTOR<STATION_INFO_T*> vDepot;

    Station::GetDepotStationInfo(vDepot);

    ercbDepot->Properties->Items->Clear();
    for (i=0; i<vDepot.size(); i++)
    {
        ercbStation->Properties->Items->Add(vDepot[i]->acStationName);
    }

    HideSDSInfo();

    if (IsSimpleSMSMode())
    {
        gbInfo->Visible = false;

        gbPreview->Caption = "短信内容";

        m_DictSDSType->SetID(cbSDSType, SDS_TYPE_NORMAL);

        this->Caption = "短信";
    }

}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSDS::InitSkin()
{
    tlInfo->OptionsSelection->CellSelect = true;

    SetSkin(tlUser, true);
    SetSkin(tlGroup, true);
    tlUser->OptionsView->CheckGroups = true;
    tlGroup->OptionsView->CheckGroups = true;
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSDS::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSDS::InitUser(VECTOR<UINT32> &vUserID)
{
    tlUser->Root->CheckGroupType = ncgCheckGroup;
    tlUser->OptionsView->CheckGroups = true;

    tlUser->BeginUpdate();
    tlUser->Clear();

    TcxTreeListNode     *Node;
    TcxTreeListNode     *SubNode;
    TcxTreeListNode     *UserNode;
    VECTOR<PORTAL_UE_INFO_T*>   vPortalUE;

    Node = tlUser->Add();
    Node->Values[0] = "调度台";
    Node->CheckGroupType = ncgCheckGroup;

    VECTOR<DC_USER_INFO_T*>     vUser;

    DCUser::GetDCUser(vUser);

    for (UINT32 i=0; i<vUser.size(); i++)
    {
        DC_USER_INFO_T  *pstInfo = vUser[i];

        UserNode = Node->AddChild();
        UserNode->Values[0] = pstInfo->ulUserID;
        UserNode->Values[1] = pstInfo->ulUserID;
        UserNode->Values[2] = DC_USER;
        UserNode->CheckGroupType = ncgCheckGroup;
    }

    VECTOR<STATION_INFO_T*> &vStationInfo = Station::GetStationInfo();

    Node = tlUser->Add();
    Node->Values[0] = "车站";
    Node->CheckGroupType = ncgCheckGroup;
    for (UINT32 i=0; i<vStationInfo.size(); i++)
    {
        SubNode = Node->AddChild();
        SubNode->Values[0] = AnsiString(vStationInfo[i]->ulStationID) + AnsiString(vStationInfo[i]->acStationName);
        SubNode->CheckGroupType = ncgCheckGroup;

        STATION_UE_INFO_T   *pstStationUE = StationUE::GetStationUEByStationID(vStationInfo[i]->ulStationID);

        if (pstStationUE)
        {
            UserNode = SubNode->AddChild();
            UserNode->Values[0] = pstStationUE->ulUserID;
            UserNode->Values[1] = pstStationUE->ulUserID;
            UserNode->Values[2] = STATION_UE;
            UserNode->CheckGroupType = ncgCheckGroup;
        }

        VECTOR<PORTAL_UE_INFO_T*>     vPortalUE;

        PortalUE::GetPortalUE(vStationInfo[i]->ulStationID, vPortalUE);

        for (UINT32 j=0; j<vPortalUE.size(); j++)
        {
            UserNode = SubNode->AddChild();
            UserNode->Values[0] = vPortalUE[j]->ulUserID;
            UserNode->Values[1] = vPortalUE[j]->ulUserID;
            UserNode->Values[2] = vPortalUE[j]->ulType;
            UserNode->CheckGroupType = ncgCheckGroup;
        }
    }

    VECTOR<TRAIN_INFO_T*>   vTrainInfo;
    VECTOR<UINT32>          vTrainUserID;

    Train::GetTrainInfo(vTrainInfo);

    Node = tlUser->Add();
    Node->Values[0] = "上行列车";
    Node->CheckGroupType = ncgCheckGroup;
    for (UINT32 i=0; i<vTrainInfo.size(); i++)
    {
        if (vTrainInfo[i]->ulDirection != TRAIN_DIRECTION_UPSIDE)
        {
            continue;
        }

        SubNode = Node->AddChild();
        SubNode->Values[0] = Train::GetTrainAlias(vTrainInfo[i]);
        SubNode->CheckGroupType = ncgCheckGroup;

        Train::GetTrainUserID(vTrainInfo[i]->ulTrainUnitID, vTrainUserID);

        for (UINT32 j=0; j<vTrainUserID.size(); j++)
        {
            UserNode = SubNode->AddChild();
            UserNode->Values[0] = vTrainUserID[j];
            UserNode->Values[1] = vTrainUserID[j];
            UserNode->Values[2] = TRAIN_UE;
            UserNode->CheckGroupType = ncgCheckGroup;
        }
    }

    Node = tlUser->Add();
    Node->Values[0] = "下行列车";
    Node->CheckGroupType = ncgCheckGroup;
    for (UINT32 i=0; i<vTrainInfo.size(); i++)
    {
        if (vTrainInfo[i]->ulDirection != TRAIN_DIRECTION_DOWNSIDE)
        {
            continue;
        }

        SubNode = Node->AddChild();
        SubNode->Values[0] = Train::GetTrainAlias(vTrainInfo[i]);
        SubNode->CheckGroupType = ncgCheckGroup;

        Train::GetTrainUserID(vTrainInfo[i]->ulTrainUnitID, vTrainUserID);

        for (UINT32 j=0; j<vTrainUserID.size(); j++)
        {
            UserNode = SubNode->AddChild();
            UserNode->Values[0] = vTrainUserID[j];
            UserNode->Values[1] = vTrainUserID[j];
            UserNode->Values[2] = TRAIN_UE;
            UserNode->CheckGroupType = ncgCheckGroup;
        }
    }

    tlUser->Root->Collapse(true);

    for (UINT32 j=0; j<vUserID.size(); j++)
    {
        for (INT32 i=0; i<tlUser->AbsoluteCount; i++)
        {
            Node = tlUser->AbsoluteItems[i];
            if (Node->Values[1].IsNull())
            {
                continue;
            }

            if ((UINT32)Node->Values[1] == vUserID[j])
            {
                Node->Checked = true;

                for (TcxTreeListNode *ParentNode = Node->Parent;
                     ParentNode; ParentNode = ParentNode->Parent)
                {
                    ParentNode->Expand(true);
                    if (ParentNode->Parent == tlUser->Root)
                    {
                        break;
                    }
                }

                break;
            }
        }
    }

    tlUser->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSDS::InitGroup(VECTOR<INT64> &vGroupID)
{
    tlGroup->BeginUpdate();
    tlGroup->Clear();

    tlGroup->Root->CheckGroupType = ncgCheckGroup;
    tlGroup->OptionsView->CheckGroups = true;

    TcxTreeListNode     *Node;
    TcxTreeListNode     *SubNode;
    UINT32              i;
    VECTOR<GROUP_INFO_T*>       vGroup;
    VECTOR<TRAIN_INFO_T*>       &vTrainInfo = Train::GetTrainInfo();
    VECTOR<STATION_UE_INFO_T*>  &vStationUE = StationUE::GetStationUE();

    Group::GetGroup(STATIC_GROUP, vGroup);

    // 列车组、车站组、维护组、多职能组、其他
    Node = tlGroup->Add();
    Node->Values[0] = "列车组";
    for (i=0; i<vTrainInfo.size(); i++)
    {
        SubNode = Node->AddChild();
        SubNode->Values[0] = vTrainInfo[i]->ulTrainUnitID;
        SubNode->Values[1] = vTrainInfo[i]->i64DefaultGroupID;
    }

    Node = tlGroup->Add();
    Node->Values[0] = "车站组";
    for (i=0; i<vStationUE.size(); i++)
    {
        SubNode = Node->AddChild();
        SubNode->Values[0] = Station::GetStationName(vStationUE[i]->ulStationID);
        SubNode->Values[1] = vStationUE[i]->i64DefaultGroupID;
    }

    Node = tlGroup->Add();
    Node->Values[0] = "维护组";
    for (i=0; i<vGroup.size(); i++)
    {
        if (vGroup[i]->ulFuncType != GROUP_FUNC_FIX)
        {
            continue;
        }

        SubNode = Node->AddChild();
        SubNode->Values[0] = Group::GetGroupDisplayName(vGroup[i]->i64GroupID);
        SubNode->Values[1] = vGroup[i]->i64GroupID;
    }

    Node = tlGroup->Add();
    Node->Values[0] = "多职能组";
    for (i=0; i<vGroup.size(); i++)
    {
        if (vGroup[i]->ulFuncType != GROUP_FUNC_MULTIFUNC)
        {
            continue;
        }

        SubNode = Node->AddChild();
        SubNode->Values[0] = Group::GetGroupDisplayName(vGroup[i]->i64GroupID);
        SubNode->Values[1] = vGroup[i]->i64GroupID;
    }

    Node = tlGroup->Add();
    Node->Values[0] = "其他";
    for (i=0; i<vGroup.size(); i++)
    {
        if (vGroup[i]->ulFuncType == GROUP_FUNC_TRAIN ||
            vGroup[i]->ulFuncType == GROUP_FUNC_STATION ||
            vGroup[i]->ulFuncType == GROUP_FUNC_FIX ||
            vGroup[i]->ulFuncType == GROUP_FUNC_MULTIFUNC )
        {
            continue;
        }

        SubNode = Node->AddChild();
        SubNode->Values[0] = Group::GetGroupDisplayName(vGroup[i]->i64GroupID);
        SubNode->Values[1] = vGroup[i]->i64GroupID;
    }

    for (int i=0; i<tlGroup->AbsoluteCount; i++)
    {
        Node = tlGroup->AbsoluteItems[i];
        Node->CheckGroupType = ncgCheckGroup;
    }

    for (int i=0; i<tlGroup->AbsoluteCount; i++)
    {
        Node = tlGroup->AbsoluteItems[i];
        if (!Node->Values[1].IsNull())
        {
            INT64   i64GroupID = (INT64)Node->Values[1];

            for (UINT32 j=0; j<vGroupID.size(); j++)
            {
                if (vGroupID[j] == i64GroupID)
                {
                    Node->Checked = true;
                    break;
                }
            }
        }
    }

    tlGroup->Root->Expand(true);
    tlGroup->EndUpdate();
}

//---------------------------------------------------------------------------
void __fastcall TDialogNewSDS::Init(VECTOR<UINT32> &vUserID, VECTOR<INT64> &vGroupID)
{
    if (IsSimpleSMSMode())
    {
        InitTitle(SDS_TYPE_NORMAL);
    }

    cbTitle->ItemIndex = -1;
    cbNeedReply->Enabled = true;
    tlInfo->Clear();
    edtSDSInfo->Clear();
    mmUser->Clear();
    edtSendUserName->Text = APP::GetDCLoginName();

    InitUser(vUserID);
    InitGroup(vGroupID);

    pcMain->ActivePageIndex = 0;

    m_vUserID.clear();
    m_vGroupID.clear();
    memset(&m_stInfo, 0, sizeof(m_stInfo));

    HideSDSInfo();
}
//---------------------------------------------------------------------------
bool __fastcall TDialogNewSDS::GetUserList(VECTOR<UINT32> &vUser)
{
    AnsiString  strUserList = mmUser->Text.Trim() + ",";
    AnsiString  strUser;
    char        *szUserList;
    char        *szUser;
    char        *szTmp;
    int         iPos;
    int         iUserID;

    vUser.clear();

    if (strUserList == ",")
    {
        return true;
    }

    szUserList = strdup(strUserList.c_str());

    for (int i=0; i<strUserList.Length()-1; i++)
    {
        if (memcmp(szUserList+i, "，", 2) == 0)
        {
            szUserList[i] = ',';
            szUserList[i+1] = ' ';
        }
    }

    strUserList = szUserList;
    free(szUserList);

    while(1)
    {
        iPos = strUserList.Pos(",");

        if (iPos > 0)
        {
            strUser = strUserList.SubString(1, iPos-1).Trim();
            strUserList = strUserList.SubString(iPos+1, strUserList.Length()).Trim();

            if (strUser.IsEmpty() ||
                strUser == ","  )
            {
                continue;
            }

            iUserID = strUser.ToIntDef(-1);
            if (iUserID <= 0)
            {
                return false;
            }

            vUser.push_back(iUserID);
        }

        if (strUserList == "" ||
            strUserList == "," )
        {
            break;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSDS::btnOKClick(TObject *Sender)
{
    ModalResult = mrNone;

    TcxTreeListNode *Node;
    UINT32          ulUserID;
    INT64           i64GroupID;
    AnsiString      strTmp;

    memset(&m_stInfo, 0, sizeof(m_stInfo));

    m_stInfo.ulSendUserID = APP::GetCurrDCUserID();
    if (!m_DictSDSType->GetID(cbSDSType, &m_stInfo.ulSDSType) ||
        m_stInfo.ulSDSType == SDS_TYPE_NULL )
    {
        InfoMsg("消息类别不能为空");
        cbSDSType->Focused();
        return;
    }

    strTmp = edtSendUserName->Text.Trim();
    if (strTmp.IsEmpty())
    {
        InfoMsg("发送人不能为空");
        edtSendUserName->Focused();
        return;
    }
    else if (strTmp.Length() >= (int)sizeof(m_stInfo.acSendUserName))
    {
        InfoMsg("发送人长度不能超过%u", sizeof(m_stInfo.acSendUserName) -1);
        edtSendUserName->Focused();
        return;
    }

    StrCpy(m_stInfo.acSendUserName, strTmp);

    m_stInfo.bNeedReply = cbNeedReply->Checked;

    if (m_stInfo.ulSDSType != SDS_TYPE_NORMAL)
    {
        for (Node=tlInfo->Root->getFirstChild(); Node; Node=Node->getNextSibling())
        {
            if (!Node->Values[2].IsNull())
            {
                if (Node->Values[1].IsNull() ||
                    Node->Texts[1].IsEmpty())
                {
                    AnsiString      str = Node->Texts[0];

                    InfoMsg("%s不能为空", str.c_str());
                    tlInfo->SetFocus();
                    return;
                }
            }
        }
    }

    strTmp = edtSDSInfo->Text.Trim();
    if (strTmp.IsEmpty())
    {
        InfoMsg("信息内容不能为空");
        edtSDSInfo->Focused();
        return;
    }

    if (strTmp.Length() >= (int)sizeof(m_stInfo.acSDSInfo) )
    {
        InfoMsg("信息内容长度不能超过%u", sizeof(m_stInfo.acSDSInfo) -1);
        edtSDSInfo->Focused();
        return;
    }

    StrCpy(m_stInfo.acSDSInfo, strTmp);

    m_vUserID.clear();
    for (INT32 i=0; i<tlUser->AbsoluteCount; i++)
    {
        Node = tlUser->AbsoluteItems[i];
        if (!Node->Checked || Node->Values[2].IsNull())
        {
            continue;
        }

        ulUserID = Node->Values[1];
        m_vUserID.push_back(ulUserID);
    }

    m_vGroupID.clear();
    for (INT32 i=0; i<tlGroup->AbsoluteCount; i++)
    {
        Node = tlGroup->AbsoluteItems[i];
        if (!Node->Checked || Node->Values[1].IsNull())
        {
            continue;
        }

        i64GroupID = (INT64)Node->Values[1];
        m_vGroupID.push_back(i64GroupID);
    }

    VECTOR<UINT32>  vUser;

    if (!GetUserList(vUser))
    {
        InfoMsg("接收人号码错误");
        return;
    }

    AppendMember(m_vUserID, vUser);

    if (m_vUserID.size() == 0 &&
        m_vGroupID.size() == 0 )
    {
        InfoMsg("接收人不能为空");
        tlUser->Focused();
        return;
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSDS::Get(SDS_INFO_T &stInfo, VECTOR<UINT32> &vUserID, VECTOR<INT64> &vGroupID)
{
    memcpy(&stInfo, &m_stInfo, sizeof(stInfo));

    vUserID.clear();
    vGroupID.clear();

    vUserID.assign(m_vUserID.begin(), m_vUserID.end());
    vGroupID.assign(m_vGroupID.begin(), m_vGroupID.end());
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSDS::cbTitlePropertiesChange(TObject *Sender)
{
    AnsiString  strTitle = cbTitle->Text.Trim();

    HideSDSInfo();

    SDSTemplate *pTemplate = GetSDSTemplate(strTitle);

    if (!pTemplate)
    {
        edtSDSInfo->Text = "";
        return;
    }

    if (IsSimpleSMSMode())
    {
        SDS_TEMPLATE_T  &stTemplate = pTemplate->GetTemplate();

        edtSDSInfo->Text = stTemplate.acSDSInfo;

        return;
    }

    VECTOR<TEMPLATE_PARAM_T>    &vParam = pTemplate->GetParam();
    BOOL                        bTextOnly = TRUE;
    AnsiString                  strText = "";

    tlInfo->Clear();
    for (UINT32 i=0; i<vParam.size(); i++)
    {
        TEMPLATE_PARAM_T    &stParam = vParam[i];

        TcxTreeListNode *Node = tlInfo->Add();

        if (stParam.ulParamType == TEMPLATE_PARAM_TYPE_TEXT)
        {
            Node->Values[0] = "文本";
            Node->Values[1] = stParam.strParam;
            strText = stParam.strParam;
        }
        else
        {
            Node->Values[0] = GetSDSTemplateParamDisplayName(stParam.strParam);
            Node->Values[2] = stParam.strParam;

            bTextOnly = FALSE;
        }
    }

    if (!bTextOnly)
    {
        ShowSDSInfo();
    }
    else
    {
        edtSDSInfo->Text = strText;
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSDS::Column2GetEditProperties(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, TcxCustomEditProperties *&EditProperties)
{
    if (ANode->Values[2].IsNull() ||
        ANode->Texts[2].IsEmpty())
    {
        return;
    }

    AnsiString  strParam = ANode->Texts[2];

    if (strParam.Length() < 3)
    {
        return;
    }

    strParam = strParam.SubString(2, strParam.Length());
    strParam = strParam.SubString(1, strParam.Length()-1);

    if (strParam == TEMPLATE_PARAM_STATION)
    {
        EditProperties = ercbStation->Properties;
    }
    else if (strParam == TEMPLATE_PARAM_DEPOT)
    {
        EditProperties = ercbDepot->Properties;
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSDS::tlInfoDataChanged(TObject *Sender)
{
    SetPreviewInfo();
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSDS::SetPreviewInfo()
{
    AnsiString  strTitle = cbTitle->Text.Trim();

    if (strTitle == "自定义")
    {
        return;
    }

    SDSTemplate *pTemplte = GetSDSTemplate(strTitle);

    if (!pTemplte)
    {
        return;
    }

    TcxTreeListNode             *Node;
    String                      strText = "";
    VECTOR<int>                 vStartPos;
    int                         iStartPos;

    for (Node=tlInfo->Root->getFirstChild(); Node; Node=Node->getNextSibling())
    {
        if (!Node->Values[2].IsNull())
        {
            iStartPos = strText.Length();
            vStartPos.push_back(iStartPos);
        }

        strText += Node->Texts[1];
    }

    edtSDSInfo->Text = strText;

    strText = "";
    for (Node=tlInfo->Root->getFirstChild(); Node; Node=Node->getNextSibling())
    {
        if (!Node->Values[2].IsNull())
        {
            iStartPos = vStartPos[0];
            vStartPos.erase(vStartPos.begin());
            edtSDSInfo->SelStart = iStartPos;
            edtSDSInfo->SelLength = Node->Texts[1].Length();
            edtSDSInfo->SelAttributes->Color = clRed;
        }
    }

    edtSDSInfo->SelLength = 0;
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSDS::cbSDSTypePropertiesChange(TObject *Sender)
{
    UINT32  ulSDSType;

    if (!m_DictSDSType->GetID(cbSDSType, &ulSDSType))
    {
        return;
    }

    if (ulSDSType == SDS_TYPE_FORMAL_DISPATCH ||
        ulSDSType == SDS_TYPE_ORAL_DISPATCH )
    {
        cbNeedReply->Checked = true;
        cbNeedReply->Properties->ReadOnly = true;
    }
    else if (ulSDSType == SDS_TYPE_OPERATE)
    {
        cbNeedReply->Checked = false;
        cbNeedReply->Properties->ReadOnly = true;
    }
    else
    {
        cbNeedReply->Checked = false;
        cbNeedReply->Properties->ReadOnly = false;
    }

    // 初始化Title
    InitTitle(ulSDSType);

    edtSDSInfo->Text = "";

    HideSDSInfo();
    return;
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSDS::HideSDSInfo()
{
    gbInfo->Visible = false;

    gbPreview->Top = gbReceiver->Top;
    gbPreview->Height = 400;
    gbUser->Top = gbPreview->Top + gbPreview->Height + 10;
    gbUser->Height = gbReceiver->Top + gbReceiver->Height - gbUser->Top;
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSDS::ShowSDSInfo()
{
    gbInfo->Visible = true;

    gbUser->Top = 535;
    gbUser->Height = gbReceiver->Top + gbReceiver->Height - gbUser->Top;

    gbPreview->Top = gbInfo->Top + gbInfo->Height + 10;
    gbPreview->Height = gbUser->Top - 10 - gbPreview->Top;
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSDS::tlInfoEdited(TcxCustomTreeList *Sender, TcxTreeListColumn *AColumn)
{
    SetPreviewInfo();
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSDS::EditValueChanged(TObject *Sender)
{
    SetPreviewInfo();
}
//---------------------------------------------------------------------------

void __fastcall TDialogNewSDS::tlUserCustomDrawDataCell(TcxCustomTreeList *Sender,
          TcxCanvas *ACanvas, TcxTreeListEditCellViewInfo *AViewInfo,
          bool &ADone)
{
    if (AViewInfo->Node && AViewInfo->Node->Checked)
    {
        ACanvas->Canvas->Brush->Color = UICfg::GetTreeNodeCheckedColor();
    }
}
//---------------------------------------------------------------------------

void __fastcall TDialogNewSDS::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_SDS_REPLY);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSDS::tlUserGetNodeImageIndex(TcxCustomTreeList *Sender,
          TcxTreeListNode *ANode, TcxTreeListImageIndexType AIndexType,
          TImageIndex &AIndex)
{
    if (ANode->Values[1].IsNull())
    {
        AIndex = 0;
        return;
    }

    if (ANode->Values[2].IsNull())
    {
        AIndex = 0;
        return;
    }

    BOOL    bOnline = User::IsUserOnline((UINT32)ANode->Values[1]);
    UINT32  ulType = (UINT32)ANode->Values[2];

    if (ulType == LARGE_PORTAL_UE )
    {
        AIndex = bOnline?1:2;
    }
    else if (ulType == SMALL_PORTAL_UE )
    {
        AIndex = bOnline?3:4;
    }
    else if (ulType == STATION_UE)
    {
        AIndex = bOnline?5:6;
    }
    else if (ulType == TRAIN_UE)
    {
        AIndex = bOnline?7:8;
    }
    else if (ulType == DC_USER)
    {
        AIndex = bOnline?9:10;
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSDS::tlUserNodeCheckChanged(TcxCustomTreeList *Sender, TcxTreeListNode *ANode,
          TcxCheckBoxState AState)
{
    UpdateUser();
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSDS::tlGroupNodeCheckChanged(TcxCustomTreeList *Sender,
          TcxTreeListNode *ANode, TcxCheckBoxState AState)
{
    UpdateUser();
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSDS::UpdateUser()
{
}

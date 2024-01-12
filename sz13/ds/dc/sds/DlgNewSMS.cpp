//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"

#include "MsgDialog.h"
#include "msg.h"
#include "ds_public.h"
#include "ClientUtil.h"
#include "RemoteMsg.h"
#include "ClientInit.h"
#include "Resource.h"
#include "ats.h"
#include "Train.h"
#include "User.h"
#include "Group.h"
#include "Station.h"
#include "SDSTemplate.h"

#include "DlgNewSMS.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxLabel"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxTextEdit"
#pragma link "dxSkinBlue"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinOffice2010Silver"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxCustomData"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxGroupBox"
#pragma link "cxInplaceContainer"
#pragma link "cxPC"
#pragma link "cxStyles"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "dxBarBuiltInMenu"
#pragma link "cxButtonEdit"
#pragma link "cxButtons"
#pragma link "cxMaskEdit"
#pragma link "cxImageList"
#pragma link "cxCheckBox"
#pragma link "cxClasses"
#pragma link "cxEditRepositoryItems"
#pragma link "cxExtEditRepositoryItems"
#pragma link "cxMemo"
#pragma link "cxRichEdit"
#pragma link "dxBevel"
#pragma resource "*.dfm"
TDialogNewSMS *DialogNewSMS;

#define MAX_SMS_FILE_SIZE  (16*1000*1000)
#define MAX_SMS_TITLE_SIZE (512)

//---------------------------------------------------------------------------
__fastcall TDialogNewSMS::TDialogNewSMS(TComponent* Owner)
    : TDialogForm(Owner)
{
    UINT32  i;
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

    //HideSDSInfo();
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSMS::InitUser(VECTOR<UINT32> &vUserID)
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
            /*
            if (sSelectedUser.Has(vPortalUE[j]->ulUserID))
            {
                SubNode->CheckGroupType = ncgNone;
            }
            else
            {
                SubNode->CheckGroupType = ncgCheckGroup;
            }*/
        }
    }

    VECTOR<TRAIN_INFO_T*>   vTrainInfo;
    VECTOR<UINT32>          vTrainUserID;
  //    UINT32                  ulTrainPos = GetTrainPos(g_ulDCType);

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
        SubNode->Values[0] = Train::GetTrainAlias(vTrainInfo[i]);  // vTrainInfo[i]->acTrainID;
        SubNode->CheckGroupType = ncgCheckGroup;

        Train::GetTrainUserID(vTrainInfo[i]->ulTrainUnitID, vTrainUserID);

        for (UINT32 j=0; j<vTrainUserID.size(); j++)
        {
            UserNode = SubNode->AddChild();
            UserNode->Values[0] = vTrainUserID[j];
            UserNode->Values[1] = vTrainUserID[j];
            UserNode->Values[2] = TRAIN_UE;
            UserNode->CheckGroupType = ncgCheckGroup;

        //    UserNode->Visible = false;
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
        SubNode->Values[0] = Train::GetTrainAlias(vTrainInfo[i]);  //vTrainInfo[i]->acTrainID;
        SubNode->CheckGroupType = ncgCheckGroup;

        Train::GetTrainUserID(vTrainInfo[i]->ulTrainUnitID, vTrainUserID);

        for (UINT32 j=0; j<vTrainUserID.size(); j++)
        {
            UserNode = SubNode->AddChild();
            UserNode->Values[0] = vTrainUserID[j];
            UserNode->Values[1] = vTrainUserID[j];
            UserNode->Values[2] = TRAIN_UE;
            UserNode->CheckGroupType = ncgCheckGroup;

           // UserNode->Visible = false;
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
void __fastcall TDialogNewSMS::InitSkin()
{
    SetSkin(tlUser, true);
    SetSkin(tlGroup, true);
    tlUser->OptionsView->CheckGroups = true;
    tlGroup->OptionsView->CheckGroups = true;
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSMS::InitGroup(VECTOR<INT64> &vGroupID)
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
    ///VECTOR<STATION_UE_INFO_T*>   &vStationUE = StationUE::GetStationUEList();
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
void __fastcall TDialogNewSMS::edtSMSFilePropertiesButtonClick(TObject *Sender, int AButtonIndex)
{
    if (!OpenDialog->Execute())
    {
        return;
    }

    edtSMSFile->Text = OpenDialog->FileName;
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewSMS::Get(AnsiString &strSMSTitle, AnsiString &strSMSFile, VECTOR<UINT32> &vUserID, VECTOR<INT64> &vGroupID)
{
    strSMSTitle = m_strSMSTitle;
    strSMSFile  = m_strSMFile;

    vUserID.clear();
    vGroupID.clear();

    vUserID.assign(m_vUserID.begin(), m_vUserID.end());
    vGroupID.assign(m_vGroupID.begin(), m_vGroupID.end());
}
//---------------------------------------------------------------------------

void __fastcall TDialogNewSMS::Init(VECTOR<UINT32> &vUserID, VECTOR<INT64> &vGroupID)
{
    //cbTitle->ItemIndex = -1;
    //cbNeedReply->Enabled = true;
    //tlInfo->Clear();
    mmUser->Clear();
    edtSDSInfo->Clear();
    edtSMSFile->Clear();
    edtSendUserName->Text = APP::GetDCLoginName();

    InitUser(vUserID);
    InitGroup(vGroupID);

    pcMain->ActivePageIndex = 0;

    m_vUserID.clear();
    m_vGroupID.clear();
    //HideSDSInfo();
}
//---------------------------------------------------------------------------

void __fastcall TDialogNewSMS::btnSendClick(TObject *Sender)
{
    ModalResult = mrNone;

    TcxTreeListNode *Node;
    UINT32          ulUserID;
    INT64           i64GroupID;
    AnsiString      strTmp;

    //memset(&m_stInfo, 0, sizeof(m_stInfo));


/**********************************************************************
    strTmp = edtSendUserName->Text.Trim();
    if (strTmp.IsEmpty())
    {
        InfoMsg("发送人不能为空");
        edtSendUserName->Focused();
        return;
    }
    else if (strTmp.Length() >= sizeof(m_stInfo.acSendUserName))
    {
        InfoMsg("发送人长度不能超过%u", sizeof(m_stInfo.acSendUserName) -1);
        edtSendUserName->Focused();
        return;
    }

    StrCpy(m_stInfo.acSendUserName, strTmp);
**********************************************************************/

    strTmp = edtSDSInfo->Text.Trim();
    if (strTmp.IsEmpty())
    {
        InfoMsg("请填写彩信标题");
        edtSDSInfo->Focused();
        return;
    }
    else if (strTmp.Length() >= MAX_SMS_TITLE_SIZE )
    {
        InfoMsg("信息内容长度不能超过%u", MAX_SMS_TITLE_SIZE);
        edtSDSInfo->Focused();
        return;
    }
    m_strSMSTitle = strTmp;

    strTmp = edtSMSFile->Text.Trim();
    if (strTmp.IsEmpty())
    {
        InfoMsg("请添加附件");
        edtSMSFile->Focused();
        return;
    }
    else if (gos_get_file_size(strTmp.c_str()) > MAX_SMS_FILE_SIZE)
    {
        InfoMsg("请添加小于2M大小的附件");
        edtSMSFile->Focused();
        return;
    }

    m_strSMFile = strTmp;

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

    GetUserList(vUser);
    AppendMember(m_vUserID, vUser);

    if (m_vUserID.size() == 0 &&
        m_vGroupID.size() == 0 )
    {
        InfoMsg("请填写或勾选接收人");
        tlUser->Focused();
        return;
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TDialogNewSMS::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

void __fastcall TDialogNewSMS::tlUserCustomDrawDataCell(TcxCustomTreeList *Sender,
          TcxCanvas *ACanvas, TcxTreeListEditCellViewInfo *AViewInfo,
          bool &ADone)
{
    if (AViewInfo->Node && AViewInfo->Node->Checked)
    {
        ///ACanvas->Canvas->Brush->Color = UICfg::g_TreeNodeCheckedColor;
        ACanvas->Canvas->Brush->Color = UICfg::GetTreeNodeCheckedColor();
    }
}
//---------------------------------------------------------------------------

void __fastcall TDialogNewSMS::tlUserGetNodeImageIndex(TcxCustomTreeList *Sender,
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

bool __fastcall TDialogNewSMS::GetUserList(VECTOR<UINT32> &vUser)
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
void __fastcall TDialogNewSMS::FormShow(TObject *Sender)
{
    edtSDSInfo->SetFocus();
}
//---------------------------------------------------------------------------


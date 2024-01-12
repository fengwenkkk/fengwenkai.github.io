//---------------------------------------------------------------------------

#ifndef FrmTrainCfgH
#define FrmTrainCfgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "cxButtons.hpp"
#include "cxContainer.hpp"
#include "cxControls.hpp"
#include "cxCustomData.hpp"
#include "cxDropDownEdit.hpp"
#include "cxEdit.hpp"
#include "cxGraphics.hpp"
#include "cxGroupBox.hpp"
#include "cxInplaceContainer.hpp"
#include "cxLabel.hpp"
#include "cxLookAndFeelPainters.hpp"
#include "cxLookAndFeels.hpp"
#include "cxMemo.hpp"
#include "cxStyles.hpp"
#include "cxTextEdit.hpp"
#include "cxTL.hpp"
#include "cxTLdxBarBuiltInMenu.hpp"
#include "dxSkinBlack.hpp"
#include "dxSkinBlue.hpp"
#include "dxSkinCaramel.hpp"
#include "dxSkinCoffee.hpp"
#include "dxSkinDarkRoom.hpp"
#include "dxSkinDarkSide.hpp"
#include "dxSkinFoggy.hpp"
#include "dxSkinGlassOceans.hpp"
#include "dxSkiniMaginary.hpp"
#include "dxSkinLilian.hpp"
#include "dxSkinLiquidSky.hpp"
#include "dxSkinLondonLiquidSky.hpp"
#include "dxSkinMcSkin.hpp"
#include "dxSkinMoneyTwins.hpp"
#include "dxSkinOffice2007Black.hpp"
#include "dxSkinOffice2007Blue.hpp"
#include "dxSkinOffice2007Green.hpp"
#include "dxSkinOffice2007Pink.hpp"
#include "dxSkinOffice2007Silver.hpp"
#include "dxSkinOffice2010Black.hpp"
#include "dxSkinOffice2010Blue.hpp"
#include "dxSkinOffice2010Silver.hpp"
#include "dxSkinPumpkin.hpp"
#include "dxSkinsCore.hpp"
#include "dxSkinsDefaultPainters.hpp"
#include "dxSkinSeven.hpp"
#include "dxSkinSharp.hpp"
#include "dxSkinSilver.hpp"
#include "dxSkinSpringTime.hpp"
#include "dxSkinStardust.hpp"
#include "dxSkinSummer2008.hpp"
#include "dxSkinValentine.hpp"
#include "dxSkinXmas2008Blue.hpp"
#include "MDIForm.h"
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include "dxBar.hpp"
#include "dxSkinsdxBarPainter.hpp"
#include "cxProgressBar.hpp"
#include "dxSkinBlueprint.hpp"
#include "dxSkinDevExpressDarkStyle.hpp"
#include "dxSkinDevExpressStyle.hpp"
#include "dxSkinHighContrast.hpp"
#include "dxSkinMetropolis.hpp"
#include "dxSkinMetropolisDark.hpp"
#include "dxSkinOffice2013DarkGray.hpp"
#include "dxSkinOffice2013LightGray.hpp"
#include "dxSkinOffice2013White.hpp"
#include "dxSkinOffice2016Colorful.hpp"
#include "dxSkinOffice2016Dark.hpp"
#include "dxSkinSevenClassic.hpp"
#include "dxSkinSharpPlus.hpp"
#include "dxSkinTheAsphaltWorld.hpp"
#include "dxSkinTheBezier.hpp"
#include "dxSkinVisualStudio2013Blue.hpp"
#include "dxSkinVisualStudio2013Dark.hpp"
#include "dxSkinVisualStudio2013Light.hpp"
#include "dxSkinVS2010.hpp"
#include "dxSkinWhiteprint.hpp"
#include <Vcl.Dialogs.hpp>
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"

//#define ESDK_SIMU

#ifdef ESDK_SIMU
#define eSDK_SetDynamicGroupUser    _eSDK_SetDynamicGroupUser
#define eSDK_GetDynamicGroupMember  _eSDK_GetDynamicGroupMember
#define eSDK_IsLogined              _eSDK_IsLogined
#define eSDK_CreateDynamicGroup     _eSDK_CreateDynamicGroup
#define GetDynamicGroup             _GetDynamicGroup
#define AddDynamicGroup             _AddDynamicGroup

#define CreateMainLineTrainGroup    _CreateMainLineTrainGroup
#define CreateDepotTrainGroup       _CreateDepotTrainGroup
#define CreateTrainBrdGroup         _CreateTrainBrdGroup
#define CreateTrainMulticastGroup   _CreateTrainMulticastGroup
#define CreateStationMulticastGroup _CreateStationMulticastGroup

DC_GROUP_INFO_T* _GetDynamicGroup(INT64 i64GroupID);
BOOL _eSDK_IsLogined();
BOOL _eSDK_CreateDynamicGroup(INT64 i64GroupID, VECTOR<UINT32> &vUserID, UINT32 ulPriority, UINT32 ulMaxPeriod, CHAR *szGroupName);
BOOL _eSDK_CreateDynamicGroup(INT64 i64GroupID, UINT32 ulPriority, UINT32 ulMaxPeriod, CHAR *szGroupName);
BOOL _eSDK_SetDynamicGroupUser(INT64 i64GroupID, VECTOR<UINT32> &vAddUserID, VECTOR<UINT32> &vDelUserID);
BOOL _eSDK_GetDynamicGroupMember(INT64 i64GroupID, VECTOR<UINT32> &vUser, VECTOR<INT64> &vGroup);
BOOL _eSDK_GetDynamicGroupMember(INT64 i64GroupID, VECTOR<UINT32> &vUser);

BOOL _CreateMainLineTrainGroup(TRAIN_INFO_T *pstTrain);
BOOL _CreateDepotTrainGroup(TRAIN_INFO_T *pstTrain);
BOOL _CreateTrainBrdGroup(TRAIN_INFO_T *pstTrain);
BOOL _CreateTrainMulticastGroup(INT64 i64GroupID);
BOOL _CreateStationMulticastGroup(INT64 i64GroupID);
VOID _AddDynamicGroup(GROUP_INFO_T &stGroup);

typedef struct
{
    INT64           i64GroupID;
    CHAR            acGroupName[64];
    VECTOR<UINT32>  vUser;
}_DYNA_GROUP_T;

#endif

//---------------------------------------------------------------------------
class TFormTrainCfg : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbMain;
    TcxTreeList *tlMain;
    TcxTreeListColumn *Column1;
    TcxTreeListColumn *Column2;
    TcxGroupBox *gbCtrl;
    TcxButton *btnDel;
    TcxButton *btnAdd;
    TcxButton *btnRefresh;
    TcxButton *btnSet;
    TcxButton *btnImport;
    TcxButton *btnCheck;
    TOpenDialog *OpenDialog;
    TTimer *TimerCheck;
    TcxTreeListColumn *tlMainColumn3;
    TcxTreeListColumn *tlMainColumn4;
    TcxTreeListColumn *tlMainColumn5;
    TcxTreeListColumn *tlMainColumn6;
    TcxTreeListColumn *tlMainColumn7;
    TcxTreeListColumn *tlMainColumn8;
    TcxTreeListColumn *tlMainColumn9;
    TcxMemo *mmESDKGroup;
    void __fastcall btnAddClick(TObject *Sender);
    void __fastcall btnDelClick(TObject *Sender);
    void __fastcall btnRefreshClick(TObject *Sender);
    void __fastcall btnSetClick(TObject *Sender);
    void __fastcall Column2GetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall btnImportClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall TimerCheckTimer(TObject *Sender);
    void __fastcall btnCheckClick(TObject *Sender);
private:    // User declarations
    virtual void __fastcall InitSkin(){};

    BOOL __fastcall CheckTrainGroup(UINT32 ulTrainUnitID, INT64 i64GroupID, BOOL &bCheckGroup, TcxTreeListNode *Node);
    void __fastcall CheckTrainGroup();
    void __fastcall InitNode(TcxTreeListNode *Node, TRAIN_INFO_T *pstTrain);
    void __fastcall InitView();
public:     // User declarations

    __fastcall TFormTrainCfg(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TFormTrainCfg *FormTrainCfg;
//---------------------------------------------------------------------------
#endif

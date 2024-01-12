//---------------------------------------------------------------------------

#ifndef TrainMapH
#define TrainMapH
//---------------------------------------------------------------------------

#include "AdvSmoothPanel.hpp"
#include "cxGroupBox.hpp"


#define MAX_TRAIN_NUM_OF_STATION        6

typedef struct
{
    UINT32          ulStationID;
    UINT32          ulTrainNum;
    TRAIN_INFO_T    *apstTrainInfo[MAX_TRAIN_NUM_OF_STATION];
    INT32           aiTrainOrder[MAX_TRAIN_NUM_OF_STATION];
}TRAIN_POS_INFO_T;


namespace TrainMap
{
int GetMaxColNum();
void SetColNum(int iColNum);

int GetColNum();
int GetRowMargin();
int GetTrainWidth();
int GetTrainHeight();
int GetDepotTrainHeight();
int GetTrainMargin();
int GetStationMargin();
int GetStationWidth();
int GetStationHeight();
int GetColWidth();
int GetRowHeight();
void SetRowHeight(int iRowHeight);
void GetStationPos(UINT32 ulWidth, UINT32 ulStationIndex, int &x, int &y, UINT32 *pulRowIndex=NULL);
};

// 参考dis中GetTrainOrder的计算方法, = (SectionID-SectionIDOnPlatForm)*256 + IndexInSection
BOOL IsTrainVisible(UINT32 ulTrainUnitID);
bool IsOnPlatform(int iTrainOrder);
void SetTrainPos(TRAIN_POS_INFO_T *pstTrainPos, int iPlatformIndex, UINT32 ulLeftNum, UINT32 ulRightNum);
void SetTrainPosIndex(TRAIN_POS_INFO_T *pstTrainPos);

namespace TrainMapCtrl
{
void Init(BOOL bShowTrainUnitID,  bool bShowStationID,
          TcxGroupBox *gbMap, TcxGroupBox *gbDepotMap,
          TAdvSmoothPanel *PanelTransferTrack,
          TAdvSmoothPanel *PanelTurnbackTrack,
          TdxBarPopupMenu *pmTrain, TdxBarPopupMenu *pmStation,
          TcxButton *btnTrainUp,
          TcxButton *btnTrainDown,
          TcxButton *btnDepotTrainUp,
          TcxButton *btnDepotTrainDown,

          TMouseEvent     btnTrainMouseDown,
          TNotifyEvent    btnTrainMouseEnter,
          TNotifyEvent    btnTrainMouseLeave,
          TNotifyEvent    btnTrainClick,

          TMouseEvent     btnStationMouseDown,
          TNotifyEvent    btnStationMouseEnter,
          TNotifyEvent    btnStationMouseLeave,
          TNotifyEvent    btnStationClick,

          TDragOverEvent OnDragOver);

void InitTrack(UINT32 ulStationNum);

void InitDepotMap(TcxTabSheet *tsDepot);
void InitDepotTrainMap(UINT32 ulDepotID);
void InitTrainMap(BOOL bShowTrainUnitID, bool bShowDepotTrackTrain);
void InitStation(STATION_INFO_T *pstStation, bool bIsLast);
UINT32 GetCurrStationID();
UINT32 GetCurrTrainUnitID(UINT32 ulDepotID);
void GetSelectTrain(UINT32 ulDepotID, VECTOR<UINT32> &vTrain);

void GetTrain(VECTOR<TcxButton *> &vTrain);
void GetStation(VECTOR<TcxButton *> &vStation);
}

#endif

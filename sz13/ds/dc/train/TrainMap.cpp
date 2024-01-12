#include "ClientInclude.h"
#include "TrainMap.h"


#define SWAP(a, b) {a=a+b; b=a-b; a=a-b; }

static bool g_bReverseTrainPos = true;  // 上行列车在下


typedef struct
{
    UINT32          ulStationID;
    UINT32          ulTrainNum;
    TRAIN_INFO_T    *apstTrainInfo[MAX_TRAIN_NUM_OF_STATION];
    INT32           aiTrainOrder[MAX_TRAIN_NUM_OF_STATION];
}DEPOT_TRAIN_POS_INFO_T;

static std::map<UINT32, TcxButton*>     m_mTrainCtrlMap;
static std::map<UINT32, TcxButton*>     m_mStationCtrlMap;
static VECTOR<TAdvSmoothPanel*>         m_vTrackLine;

//static std::map<UINT32, TcxTabSheet*>   m_mDepotMap;

static std::map<UINT32, TAdvSmoothPanel*>     m_mDepotSectionMap;

namespace TrainMap
{

static int  m_iPanelMapWidth = -1;
static int  m_iPanelMapHeight = -1;

static int  m_iMaxColNum = 5;       // 如果车站较多时，可以改成5
static int  m_iMaxRowMargin = 60;   // 最大行间距
static int  m_iRowMargin = 6;       // 行间距
static int  m_iTrainWidth = 42;  //48;//52;
static int  m_iMinTrainHeight = 28;
static int  m_iTrainHeight = 36;
static int  m_iTrainMargin = 2;     // 列车间的间距
static int  m_iStationMargin = 8;   // 站区间的间距
static int  m_iDepotTrainHeight = 28;
static int  m_iStationHeight = 30;
static int  m_iStationWidth = 114;

static UINT32   g_ulMaxTrainNumOfStation = MAX_TRAIN_NUM_OF_STATION;

int GetMaxColNum()
{
    return 5;
}

void SetColNum(int iColNum)
{
    m_iMaxColNum = iColNum;
    if (m_iMaxColNum <= 3)
    {
        m_iMaxColNum = 3;
    }

    if (m_iMaxColNum > GetMaxColNum())
    {
        m_iMaxColNum = GetMaxColNum();
    }
}

int GetColNum()
{
    return m_iMaxColNum;
}

int GetRowMargin()
{
    return m_iRowMargin;
}

int GetTrainWidth()
{
    return m_iTrainWidth;
}

int GetTrainHeight()
{
    return m_iTrainHeight;
}

int GetDepotTrainHeight()
{
    int iScreenWidth = GetSystemMetrics(SM_CXSCREEN);

    if (iScreenWidth >= 2560)
    {
        return m_iTrainHeight;
    }
    else
    {
        return m_iDepotTrainHeight;
    }
}

int GetTrainMargin()
{
    return m_iTrainMargin;
}

int GetStationMargin()
{
    return m_iStationMargin;
}

int GetStationWidth()
{
    return m_iStationWidth;
}

int GetStationHeight()
{
    return m_iStationHeight;
}

int GetColWidth()
{
    return GetTrainWidth()*g_ulMaxTrainNumOfStation + GetStationMargin();
}

int GetRowHeight()
{
  //  return m_iStationHeight + 4*m_iTrainHeight + m_iRowMargin;    // 4支持显示停车轨列车
    return m_iStationHeight + 2*m_iTrainHeight + m_iRowMargin;    // 4支持显示停车轨列车
}

void SetRowHeight(int iRowHeight)
{
    int iDelta;

    for (UINT32 i=0; i<3; i++)
    {
        iDelta = iRowHeight - GetRowHeight();
        if (iDelta == 0)
        {
            return;
        }

        if (i > 0)
        {
            i  += 0;
        }

        // 不足时的处理
        if (iDelta < 0)
        {
            m_iTrainHeight += (iDelta-1)/2;

            /*
            if (iDelta < -8)
            {
                m_iTrainHeight += iDelta/2;
            }
            else
            {
                m_iTrainHeight -= 8/2;
            } */

            if (m_iTrainHeight < m_iMinTrainHeight)
            {
                m_iTrainHeight = m_iMinTrainHeight;
                if (m_iRowMargin > 15)
                {
                    m_iRowMargin -= 10;
                }
                else if (m_iRowMargin > 10)
                {
                    m_iRowMargin -= 8;
                }
                else if (m_iRowMargin > 5)
                {
                    m_iRowMargin -= 4;
                }
            }

            iDelta = iRowHeight - GetRowHeight();
        }

        if (iDelta > 0)
        {
            m_iRowMargin += iDelta;
            if (m_iRowMargin > m_iMaxRowMargin)
            {
                m_iRowMargin = m_iMaxRowMargin;
            }

            return;
        }
    }
}

void GetStationPos(UINT32 ulWidth, UINT32 ulStationIndex, int &x, int &y, UINT32 *pulRowIndex)
{
    int     iColNum = ulWidth/ TrainMap::GetColWidth();

    if (iColNum <= 0)
    {
        iColNum = 1;
    }

    UINT32  ulColIndex = MOD(ulStationIndex, iColNum);
    UINT32  ulRowIndex = ulStationIndex/iColNum;

    x = ulColIndex*TrainMap::GetColWidth();
//    y = ulRowIndex*TrainMap::GetRowHeight() + TrainMap::GetRowMargin() + 2*TrainMap::GetTrainHeight();
    y = ulRowIndex*TrainMap::GetRowHeight() + TrainMap::GetRowMargin()/2 + TrainMap::GetTrainHeight();

    if (pulRowIndex)
    {
        *pulRowIndex = ulRowIndex;
    }
}


}; // end namespace TrainMap

BOOL IsTrainVisible(UINT32 ulTrainUnitID)
{
    TRAIN_INFO_T    *pstTrain = Train::GetTrainInfo(ulTrainUnitID);

    if (!pstTrain)
    {
        return FALSE;
    }

    return TRUE;
}

// 参考dis中GetTrainOrder的计算方法, = (SectionID-SectionIDOnPlatForm)*256 + IndexInSection
bool IsOnPlatform(int iTrainOrder)
{
    UINT8   ucIndexInSection = (UINT8)iTrainOrder;

    iTrainOrder -= ucIndexInSection;

    if (iTrainOrder == 0)
    {
        return true;
    }

    return false;
}

#if (MAX_TRAIN_NUM_OF_STATION == 7)
void SetTrainPos(TRAIN_POS_INFO_T *pstTrainPos, int iPlatformIndex, UINT32 ulLeftNum, UINT32 ulRightNum)
{
    int     iRightStartIndex = ulLeftNum;

    if (iPlatformIndex >= 0)
    {
        if (iPlatformIndex != ulLeftNum)
        {
            GosLog(LOG_ERROR, "invalid platform index %d", iPlatformIndex);
            return;
        }

        pstTrainPos->apstTrainInfo[iPlatformIndex]->iTrainStationPosIndex = 0;
        iRightStartIndex++;
    }

    if (ulLeftNum == 1)
    {
        pstTrainPos->apstTrainInfo[0]->iTrainStationPosIndex = -4;
    }
    else if (ulLeftNum == 2)
    {
        pstTrainPos->apstTrainInfo[0]->iTrainStationPosIndex = -4;
        pstTrainPos->apstTrainInfo[1]->iTrainStationPosIndex = -2;
    }
    else if (ulLeftNum == 3)
    {
        pstTrainPos->apstTrainInfo[0]->iTrainStationPosIndex = -6;
        pstTrainPos->apstTrainInfo[1]->iTrainStationPosIndex = -4;
        pstTrainPos->apstTrainInfo[2]->iTrainStationPosIndex = -2;
    }

    if (ulRightNum == 1)
    {
        pstTrainPos->apstTrainInfo[iRightStartIndex]->iTrainStationPosIndex = 4;
    }
    else if (ulRightNum == 2)
    {
        pstTrainPos->apstTrainInfo[iRightStartIndex]->iTrainStationPosIndex = 2;
        pstTrainPos->apstTrainInfo[iRightStartIndex+1]->iTrainStationPosIndex = 4;
    }
    else if (ulRightNum == 3)
    {
        pstTrainPos->apstTrainInfo[iRightStartIndex]->iTrainStationPosIndex = 2;
        pstTrainPos->apstTrainInfo[iRightStartIndex+1]->iTrainStationPosIndex = 4;
        pstTrainPos->apstTrainInfo[iRightStartIndex+2]->iTrainStationPosIndex = 6;
    }
}

void SetTrainPosIndex(TRAIN_POS_INFO_T *pstTrainPos)
{
    UINT32          i;
    INT64           ai64Pos[MAX_TRAIN_NUM_OF_STATION];
    TRAIN_INFO_T    *pstTmp;
    INT32           iOnPlatFormIndex = -1;
    UINT32          ulLeftNum = 0;
    UINT32          ulRightNum = 0;

    for (i=0; i<pstTrainPos->ulTrainNum; i++)
    {
        ai64Pos[i] = pstTrainPos->apstTrainInfo[i]->iTrainOrder;
    }

    for (i=0; i<pstTrainPos->ulTrainNum; i++)
    {
        for (UINT32 j=i+1; j<pstTrainPos->ulTrainNum; j++)
        {
            if (ai64Pos[i] > ai64Pos[j])
            {
                SWAP(ai64Pos[i], ai64Pos[j]);
                pstTmp = pstTrainPos->apstTrainInfo[i];
                pstTrainPos->apstTrainInfo[i] = pstTrainPos->apstTrainInfo[j];
                pstTrainPos->apstTrainInfo[j] = pstTmp;
            }
        }
    }

    for (i=0; i<pstTrainPos->ulTrainNum; i++)
    {
        if (pstTrainPos->apstTrainInfo[i]->ulPlatformID > 0)
      //    if (IsOnPlatform(pstTrainPos->apstTrainInfo[i]->iTrainOrder) )
        {
            if (iOnPlatFormIndex < 0)
            {
                iOnPlatFormIndex = i;
            }
            else
            {
                GosLog(LOG_ERROR, "another train in platform %u, prev is %u",
                        pstTrainPos->apstTrainInfo[i]->ulPlatformID,
                        pstTrainPos->apstTrainInfo[iOnPlatFormIndex]->ulPlatformID);
            }
        }
    }

    if (iOnPlatFormIndex < 0)
    {
        for (i=0; i<pstTrainPos->ulTrainNum; i++)
        {
            if (IsOnPlatform(pstTrainPos->apstTrainInfo[i]->iTrainOrder) )
            {
                if (iOnPlatFormIndex < 0)
                {
                    iOnPlatFormIndex = i;
                }
                else
                {
                    GosLog(LOG_WARN, "another train in platform %u, prev is %u",
                            pstTrainPos->apstTrainInfo[i]->ulPlatformID,
                            pstTrainPos->apstTrainInfo[iOnPlatFormIndex]->ulPlatformID);
                }
            }
        }
    }

    if (iOnPlatFormIndex >= 0)
    {
        for (i=0; i<pstTrainPos->ulTrainNum; i++)
        {
            if (pstTrainPos->apstTrainInfo[i]->iTrainOrder < pstTrainPos->apstTrainInfo[iOnPlatFormIndex]->iTrainOrder)
            {
                ulLeftNum++;
            }
        }

        // 1表示在站台上的车
        ulRightNum = pstTrainPos->ulTrainNum - ulLeftNum - 1;
    }
    else
    {
        for (i=0; i<pstTrainPos->ulTrainNum; i++)
        {
            if (pstTrainPos->apstTrainInfo[i]->iTrainOrder < 0)
            {
                ulLeftNum++;
            }
        }

        // 站台上的没有车
        ulRightNum = pstTrainPos->ulTrainNum - ulLeftNum;
    }

    if (ulLeftNum <= 3)
    {
        if (ulRightNum <= 3)
        {
            SetTrainPos(pstTrainPos, iOnPlatFormIndex, ulLeftNum, ulRightNum);
        }
        else
        {
            SetTrainPos(pstTrainPos, pstTrainPos->ulTrainNum-4, pstTrainPos->ulTrainNum-4, 3);
        }
    }
    else
    {
        SetTrainPos(pstTrainPos, 3, 3, pstTrainPos->ulTrainNum-4);
    }
}
#endif

#if (MAX_TRAIN_NUM_OF_STATION == 6)
void SetTrainPos(TRAIN_POS_INFO_T *pstTrainPos, int iPlatformIndex, UINT32 ulLeftNum, UINT32 ulRightNum)
{
    int     iRightStartIndex = ulLeftNum;
    UINT32  ulTotlaNum = ulLeftNum + ulRightNum;

    if (iPlatformIndex >= 0)
    {
        ulTotlaNum++;
    }

    if (iPlatformIndex >= 0)
    {
        if ((UINT32)iPlatformIndex != ulLeftNum)
        {
            GosLog(LOG_ERROR, "invalid platform index %d", iPlatformIndex);

            return;
        }

        pstTrainPos->apstTrainInfo[iPlatformIndex]->iTrainStationPosIndex = 0;
        iRightStartIndex++;
    }

    if (ulTotlaNum >= 6)
    {
        pstTrainPos->apstTrainInfo[0]->iTrainStationPosIndex = -5;
        pstTrainPos->apstTrainInfo[1]->iTrainStationPosIndex = -3;
        pstTrainPos->apstTrainInfo[2]->iTrainStationPosIndex = -1;
        pstTrainPos->apstTrainInfo[3]->iTrainStationPosIndex = 1;
        pstTrainPos->apstTrainInfo[4]->iTrainStationPosIndex = 3;
        pstTrainPos->apstTrainInfo[5]->iTrainStationPosIndex = 5;
    }
    else
    {
        if (ulLeftNum == 1)
        {
            pstTrainPos->apstTrainInfo[0]->iTrainStationPosIndex = -2;
        }
        else if (ulLeftNum == 2)
        {
            pstTrainPos->apstTrainInfo[0]->iTrainStationPosIndex = -4;
            pstTrainPos->apstTrainInfo[1]->iTrainStationPosIndex = -2;
        }

        if (ulRightNum == 1)
        {
            pstTrainPos->apstTrainInfo[iRightStartIndex]->iTrainStationPosIndex = 2;
        }
        else if (ulRightNum == 2)
        {
            pstTrainPos->apstTrainInfo[iRightStartIndex]->iTrainStationPosIndex = 2;
            pstTrainPos->apstTrainInfo[iRightStartIndex+1]->iTrainStationPosIndex = 4;
        }
    }
}

void SetTrainPosIndex(TRAIN_POS_INFO_T *pstTrainPos)
{
    UINT32          i;
    INT64           ai64Pos[MAX_TRAIN_NUM_OF_STATION];
    TRAIN_INFO_T    *pstTmp;
    INT32           iOnPlatFormIndex = -1;
    UINT32          ulLeftNum = 0;
    UINT32          ulRightNum = 0;

    for (i=0; i<pstTrainPos->ulTrainNum; i++)
    {
        ai64Pos[i] = pstTrainPos->apstTrainInfo[i]->iTrainOrder;
    }

    for (i=0; i<pstTrainPos->ulTrainNum; i++)
    {
        for (UINT32 j=i+1; j<pstTrainPos->ulTrainNum; j++)
        {
            if (ai64Pos[i] > ai64Pos[j])
            {
                SWAP(ai64Pos[i], ai64Pos[j]);
                pstTmp = pstTrainPos->apstTrainInfo[i];
                pstTrainPos->apstTrainInfo[i] = pstTrainPos->apstTrainInfo[j];
                pstTrainPos->apstTrainInfo[j] = pstTmp;
            }
        }
    }

    for (i=0; i<pstTrainPos->ulTrainNum; i++)
    {
        if (pstTrainPos->apstTrainInfo[i]->ulPlatformID > 0)
      //    if (IsOnPlatform(pstTrainPos->apstTrainInfo[i]->iTrainOrder) )
        {
            if (iOnPlatFormIndex < 0)
            {
                iOnPlatFormIndex = i;
            }
            else
            {
                GosLog(LOG_ERROR, "another train in platform %u, prev is %u",
                        pstTrainPos->apstTrainInfo[i]->ulPlatformID,
                        pstTrainPos->apstTrainInfo[iOnPlatFormIndex]->ulPlatformID);
            }
        }
    }

    if (iOnPlatFormIndex < 0)
    {
        for (i=0; i<pstTrainPos->ulTrainNum; i++)
        {
            if (IsOnPlatform(pstTrainPos->apstTrainInfo[i]->iTrainOrder) )
            {
                if (iOnPlatFormIndex < 0)
                {
                    iOnPlatFormIndex = i;
                }
                else
                {
                    GosLog(LOG_WARN, "another train in platform %u, prev is %u",
                            pstTrainPos->apstTrainInfo[i]->ulPlatformID,
                            pstTrainPos->apstTrainInfo[iOnPlatFormIndex]->ulPlatformID);
                }
            }
        }
    }

    if (iOnPlatFormIndex >= 0)
    {
        for (i=0; i<pstTrainPos->ulTrainNum; i++)
        {
            if (pstTrainPos->apstTrainInfo[i]->iTrainOrder < pstTrainPos->apstTrainInfo[iOnPlatFormIndex]->iTrainOrder)
            {
                ulLeftNum++;
            }
        }

        // 1表示在站台上的车
        ulRightNum = pstTrainPos->ulTrainNum - ulLeftNum - 1;
    }
    else
    {
        for (i=0; i<pstTrainPos->ulTrainNum; i++)
        {
            if (pstTrainPos->apstTrainInfo[i]->iTrainOrder < 0)
            {
                ulLeftNum++;
            }
        }

        // 站台上的没有车
        ulRightNum = pstTrainPos->ulTrainNum - ulLeftNum;
    }

    if (pstTrainPos->ulTrainNum >= 6)
    {
        SetTrainPos(pstTrainPos, 3, 3, 2);
        return;
    }

    // 以下最多5辆车
    if (ulLeftNum <= 2)
    {
        if (ulRightNum <= 2)
        {
            SetTrainPos(pstTrainPos, iOnPlatFormIndex, ulLeftNum, ulRightNum);
        }
        else
        {
            SetTrainPos(pstTrainPos, pstTrainPos->ulTrainNum-3, pstTrainPos->ulTrainNum-3, 2);
        }
    }
    else
    {
        SetTrainPos(pstTrainPos, 2, 2, pstTrainPos->ulTrainNum-3);
    }
}
#endif

namespace TrainMapCtrl
{
TAdvSmoothPanel *m_PanelTransferTrack = NULL;
TAdvSmoothPanel *m_PanelTurnbackTrack = NULL;
TcxGroupBox     *m_gbMap = NULL;
TcxGroupBox     *m_gbDepotMap = NULL;

TdxBarPopupMenu *m_pmTrain = NULL;
TdxBarPopupMenu *m_pmStation = NULL;

TcxButton       *m_btnTrainUp = NULL;
TcxButton       *m_btnTrainDown = NULL;
TcxButton       *m_btnDepotTrainUp = NULL;
TcxButton       *m_btnDepotTrainDown = NULL;

TDragOverEvent  m_OnDragOver = NULL;

TMouseEvent     m_btnTrainMouseDown = NULL;
TNotifyEvent    m_btnTrainMouseEnter = NULL;
TNotifyEvent    m_btnTrainMouseLeave = NULL;
TNotifyEvent    m_btnTrainClick = NULL;

TMouseEvent     m_btnStationMouseDown = NULL;
TNotifyEvent    m_btnStationMouseEnter = NULL;
TNotifyEvent    m_btnStationMouseLeave = NULL;
TNotifyEvent    m_btnStationClick = NULL;

BOOL    m_bShowTrainUnitID = TRUE;
bool    m_bShowStationID = false;

void Init(BOOL bShowTrainUnitID, bool bShowStationID,
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

          TDragOverEvent OnDragOver)
{
    m_bShowTrainUnitID = bShowTrainUnitID;
    m_bShowStationID = bShowStationID;

    m_gbMap = gbMap;
    m_gbDepotMap = gbDepotMap;
    m_PanelTransferTrack = PanelTransferTrack;
    m_PanelTurnbackTrack = PanelTurnbackTrack;

    m_pmTrain = pmTrain;
    m_pmStation = pmStation;

    m_btnTrainUp = btnTrainUp;
    m_btnTrainDown = btnTrainDown;
    m_btnDepotTrainUp = btnDepotTrainUp;
    m_btnDepotTrainDown = btnDepotTrainDown;

    m_btnTrainMouseDown = btnTrainMouseDown;
    m_btnTrainMouseEnter = btnTrainMouseEnter;
    m_btnTrainMouseLeave = btnTrainMouseLeave;
    m_btnTrainClick = btnTrainClick;

    m_btnStationMouseDown = btnStationMouseDown;
    m_btnStationMouseEnter = btnStationMouseEnter;
    m_btnStationMouseLeave = btnStationMouseLeave;
    m_btnStationClick = btnStationClick;

    m_OnDragOver = OnDragOver;
}

void InitTrack(UINT32 ulStationNum)
{
    m_PanelTransferTrack->Visible = true;
    m_PanelTurnbackTrack->Visible = true;

//    UINT32  ulStationNum = Station::GetNormalStationNum();
    int     iColNum;

    iColNum = (m_gbMap->Parent->Width - m_PanelTransferTrack->Width - m_PanelTurnbackTrack->Width - 4) / TrainMap::GetColWidth();
    if (iColNum > TrainMap::GetMaxColNum())
    {
        iColNum = TrainMap::GetMaxColNum();
    }

    if (ulStationNum == 0)
    {
        return;
    }

    if (iColNum <= 0)
    {
        iColNum = 1;
    }

    if (ulStationNum <= 4*6 &&
        iColNum > 4)
    {
        iColNum = 4;
        TrainMap::SetColNum(iColNum);
    }

    int iRowNum = (ulStationNum+iColNum-1)/iColNum;
    int iLineHeight = 2;
    int i;
    int iTopMargin = -1;
    TAdvSmoothPanel  *Panel;

    m_gbMap->Align = alNone;

    m_gbMap->Width = iColNum*TrainMap::GetColWidth() + 4 + m_PanelTurnbackTrack->Width;
    m_gbMap->Left = (m_gbMap->Parent->Width - m_gbMap->Width + m_PanelTransferTrack->Width)/2;

    int iRowHeight = (m_gbMap->Parent->Height-10)/iRowNum;

    TrainMap::SetRowHeight(iRowHeight);

    m_gbMap->Height = iRowNum*TrainMap::GetRowHeight();

    int x = 0;
    int y = (TrainMap::GetRowHeight() - iLineHeight)/2;

    while ((int)m_vTrackLine.size() < iRowNum)
    {
        Panel = new TAdvSmoothPanel(m_gbMap);
        Panel->Parent = m_gbMap;
        Panel->OnDragOver = m_OnDragOver;
        Panel->SendToBack();
        m_vTrackLine.push_back(Panel);
    }

    for (i=iRowNum; i<(int)m_vTrackLine.size(); i++)
    {
        m_vTrackLine[i]->Visible = false;
    }

    for (i=0; i<iRowNum; i++)
    {
        Panel = m_vTrackLine[i];
        Panel->Visible = true;

        Panel->Height = iLineHeight;
        if (i < (iRowNum-1))
        {
            Panel->Width = TrainMap::GetColWidth()*iColNum;
        }
        else
        {
            int iLastColNum = MOD(ulStationNum, iColNum);

            if (iLastColNum == 0)
            {
                iLastColNum = iColNum;
            }

            Panel->Width = TrainMap::GetColWidth()*iLastColNum;
            m_PanelTurnbackTrack->Left = x + Panel->Width;// - PanelTurnbackTrack->Width/2;
            m_PanelTurnbackTrack->Top = y +  TrainMap::GetRowHeight()*i + iLineHeight/2 - m_PanelTurnbackTrack->Height/2;

//            gbMap->Height = iRowNum*TrainMap::GetRowHeight();
//            iTopMargin = (gbMap->Parent->Height - gbMap->Height)/2;
            if (iTopMargin < 2)
            {
                iTopMargin = 2;
            }
        }

        Panel->Left = x;
        Panel->Top = y + TrainMap::GetRowHeight()*i;
    }

    iTopMargin = (m_gbMap->Parent->Height - m_gbMap->Height)/2;
    if (iTopMargin < 0)
    {
        iTopMargin = 0;
    }
    m_gbMap->Top = iTopMargin;

    m_PanelTransferTrack->Parent = m_gbMap->Parent;
    m_PanelTransferTrack->Left = m_gbMap->Left - m_PanelTransferTrack->Width;
    m_PanelTransferTrack->Top = m_gbMap->Top + y + iLineHeight/2 - m_PanelTransferTrack->Height/2;

    m_gbMap->Visible = true;
}

static bool GetDepotTrainPos(TRAIN_INFO_T *pstTrain, int &x, int &y)
{
    if (m_mDepotSectionMap.find(pstTrain->usSectionID) == m_mDepotSectionMap.end())
    {
        return false;
    }

    TAdvSmoothPanel *Panel = m_mDepotSectionMap[pstTrain->usSectionID];

    x = Panel->Left + (Panel->Width - TrainMap::GetTrainWidth())/2;
    y = Panel->Top - TrainMap::GetDepotTrainHeight();

    return true;
}

static void ShowDepotTrain(TRAIN_INFO_T *pstTrainInfo, UINT32 &ulUnknownPosIndex)
{
    TcxButton   *Train;
    AnsiString  strName = Train::GetTrainID(pstTrainInfo);

    if (m_bShowTrainUnitID)
    {
        strName = Train::GetTrainUnitIDText(pstTrainInfo);
    }

    Train = m_mTrainCtrlMap[pstTrainInfo->ulTrainUnitID];
    if (!Train)
    {
        Train = new TcxButton(m_gbDepotMap);

        Train->LookAndFeel->SkinName = GetDefaultSkinName();
        m_mTrainCtrlMap[pstTrainInfo->ulTrainUnitID] = Train;
        Train->Width = TrainMap::GetTrainWidth() - TrainMap::GetTrainMargin();
        Train->Layout = Buttons::blGlyphTop;
        Train->Caption = strName;
        Train->Font->Size = 10;
        //Train->Font->Name = "宋体";

        Train->PopupMenu = m_pmTrain;
    }
    else
    {
//      Train = m_mTrainCtrlMap[pstTrainInfo->ulTrainUnitID];
        Train->Caption = strName;
    }

    Train->Height = TrainMap::GetDepotTrainHeight();
    Train->Parent = m_gbDepotMap;
    Train->SpeedButtonOptions->Transparent = true;
    Train->SpeedButtonOptions->CanBeFocused = false;
    Train->SpeedButtonOptions->AllowAllUp = true;

    if (pstTrainInfo->ulDirection == TRAIN_DIRECTION_UPSIDE)
    {
        Train->Glyph = m_btnDepotTrainUp->Glyph;
    }
    else if (pstTrainInfo->ulDirection == TRAIN_DIRECTION_DOWNSIDE)
    {
        Train->Glyph = m_btnDepotTrainDown->Glyph;
    }

    Train->Visible = IsTrainVisible(pstTrainInfo->ulTrainUnitID);
    Train->OptionsImage->Layout = blGlyphTop;

    int x,y;

    if (!GetDepotTrainPos(pstTrainInfo, x, y))
    {
        x = 20;
        y = 10 + ulUnknownPosIndex*(TrainMap::GetDepotTrainHeight()+5);

        ulUnknownPosIndex++;
    }

    Train->Left = x;
    Train->Top  = y;

    Train->Tag = pstTrainInfo->ulTrainUnitID;
    Train->OnMouseDown = m_btnTrainMouseDown;
    Train->OnMouseEnter = m_btnTrainMouseEnter;
    Train->OnMouseLeave = m_btnTrainMouseLeave;
    Train->OnClick = m_btnTrainClick;
    Train->OnDragOver = m_OnDragOver;

    Train->OptionsImage->Margin = -1;
    Train->OptionsImage->Spacing = -2;
}

void InitSection(TcxGroupBox *gb, SECTION_INFO_T *pstSection, int iMinX, int iMinY, int iRangeX, int iRangeY)
{
    TAdvSmoothPanel *Panel;
    TcxLabel        *Label;
    int             iLineHeight = 2;
    int             iMarginX = 0;
    int             iMarginY = TrainMap::GetDepotTrainHeight();
    int             iMarginY2 = 5;
    double          dRateX = 1;
    double          dRateY = ((double)gb->Height - iMarginY - iMarginY2)/iRangeY;
    int iScreenWidth = GetSystemMetrics(SM_CXSCREEN);

    if (iScreenWidth <= 1600)
    {
        dRateX = 0.9*((double)gb->Width)/iRangeX;
    }

    if (dRateY > 10)
    {
        dRateY = 10;
    }

    iMarginX = (gb->Width - dRateX*iRangeX)/2;

    if (m_mDepotSectionMap.find(pstSection->ulSectionID) == m_mDepotSectionMap.end())
    {
        Panel = new TAdvSmoothPanel(gb);
        m_mDepotSectionMap[pstSection->ulSectionID] = Panel;

        Label = new TcxLabel(gb);
        Label->Caption = pstSection->acSectionName;
        Label->Transparent = true;
        Label->Style->TextColor = clGray;
        Label->Parent = gb;

        Panel->Tag = (int)Label;
    }
    else
    {
        Panel = m_mDepotSectionMap[pstSection->ulSectionID];
        Panel->Visible = true;

        Label = (TcxLabel*)Panel->Tag;
        Label->Visible = true;
    }

    Panel->Parent = gb;
    Panel->SendToBack();

    Panel->Left = iMarginX + dRateX*(pstSection->x1 - iMinX);
    Panel->Top  = iMarginY + dRateY*(pstSection->y1 - iMinY);

    if (pstSection->y1 == pstSection->y2)
    {
        Panel->Height = iLineHeight;
        Panel->Width = dRateX*(pstSection->x2 - pstSection->x1);

        Label->Left = Panel->Left + 5;
        Label->Top  = Panel->Top - 20;
    }
    else
    {
        Panel->Height = dRateY*(pstSection->y2 - pstSection->y1);
        Panel->Width = iLineHeight;

        Label->Left = Panel->Left - 20;
        Label->Top  = Panel->Top + 5;
    }
}

void InitDepotMap(TcxTabSheet *tsDepot)
{
    if (!tsDepot)
    {
        return;
    }

    SECTION_INFO_T  *pstSection;
    UINT32          ulDepotID = (UINT32)tsDepot->Tag;
    VECTOR<SECTION_INFO_T*>     vSection;

    if (ulDepotID == DEPOT_ID_UNKNOWN)
    {
        return;
    }

    Section::GetDepotSection(ulDepotID, vSection);

    std::map<UINT32, TAdvSmoothPanel*>::iterator it = m_mDepotSectionMap.begin();

    for (; it!=m_mDepotSectionMap.end(); it++)
    {
        it->second->Visible = false;

        TcxLabel    *Label = (TcxLabel*)it->second->Tag;

        Label->Visible = false;
    }

    int     iMinX = 10000;
    int     iMaxX = -1;
    int     iMinY = 10000;
    int     iMaxY = -1;

    for (UINT32 i=0; i<vSection.size(); i++)
    {
        pstSection = vSection[i];
        if (pstSection->x1 < 0)
        {
            continue;
        }

        iMinX = MIN(pstSection->x1, iMinX);
        iMaxX = MAX(pstSection->x1, iMaxX);
        iMinY = MIN(pstSection->y1, iMinY);
        iMaxY = MAX(pstSection->y1, iMaxY);

        iMinX = MIN(pstSection->x2, iMinX);
        iMaxX = MAX(pstSection->x2, iMaxX);
        iMinY = MIN(pstSection->y2, iMinY);
        iMaxY = MAX(pstSection->y2, iMaxY);
    }

    int     iRangeX = MAX(100, iMaxX - iMinX);
    int     iRangeY = MAX(100, iMaxY - iMinY);

    m_gbDepotMap->Width  = m_gbDepotMap->Parent->Width-2;
    m_gbDepotMap->Height = m_gbDepotMap->Parent->Height-6;
    m_gbDepotMap->Top = 4;
    m_gbDepotMap->Left = 0;

    for (UINT32 i=0; i<vSection.size(); i++)
    {
        pstSection = vSection[i];
        if (pstSection->x1 < 0)
        {
            continue;
        }

        InitSection(m_gbDepotMap, pstSection, iMinX, iMinY, iRangeX, iRangeY);
    }

    return;

    int     iDeltaX = m_gbDepotMap->Parent->Width  - m_gbDepotMap->Width;
    int     iDeltaY = m_gbDepotMap->Parent->Height - m_gbDepotMap->Height;

    if (iDeltaX > 0)
    {
        m_gbDepotMap->Left += iDeltaX/2;
    }
    else
    {
        m_gbDepotMap->Left = 0;
    }

    if (iDeltaY > 0)
    {
        m_gbDepotMap->Top += iDeltaY/2;
    }
    else
    {
        m_gbDepotMap->Top = 0;
    }
}

void InitDepotTrainMap(UINT32 ulDepotID)
{
    UINT32  ulTrainUnitID;
    VECTOR<TRAIN_INFO_T*>   vTrainInfo;

    if (ulDepotID == DEPOT_ID_UNKNOWN)
    {
        return;
    }

    Train::GetDepotTrainInfo(ulDepotID, vTrainInfo);

    std::map<UINT32, TcxButton*>::iterator it;
    TRAIN_INFO_T        *pstTrainInfo;
    STATION_INFO_T      *pstStation;

    // 搜索不存在的，并隐藏
    for(it=m_mTrainCtrlMap.begin();it!=m_mTrainCtrlMap.end(); it++)
    {
        ulTrainUnitID = it->first;
        pstTrainInfo = Train::GetTrainInfo(ulTrainUnitID);

        if (Station::GetDepotID(pstTrainInfo) != ulDepotID)
        {
            if (it->second->Parent == m_gbDepotMap)
            {
                it->second->Left = -1000;   // 隐藏
            }
        }
    }

    UINT32  ulUnknownPosIndex = 0;

    for (UINT32 i=0; i<vTrainInfo.size(); i++)
    {
        ShowDepotTrain(vTrainInfo[i], ulUnknownPosIndex);
    }
}

static void ShowTrain(bool bShowDepotTrackTrain, TRAIN_INFO_T *pstTrainInfo)
{
    int         iStationIndex = Station::GetStationIndex(pstTrainInfo->ulStationID);
    int         x = 0;
    int         y = 0;
    UINT32      ulRowIndex;
    UINT32      ulColIndex;
    TcxButton   *Train;
    AnsiString  strName = Train::GetTrainID(pstTrainInfo);
    bool        bVisible = true;

    if (pstTrainInfo->ulDepotTrackFlag && !bShowDepotTrackTrain)
    {
        bVisible = false;
    }
    else if (!pstTrainInfo->ulDepotTrackFlag && bShowDepotTrackTrain)
    {
        bVisible = false;
    }

    if (m_bShowTrainUnitID)
    {
//        strName = pstTrainInfo->ulTrainUnitID;
        strName = Train::GetTrainUnitIDText(pstTrainInfo);
    }

    if (iStationIndex < 0)
    {
        return;
    }

    Train = m_mTrainCtrlMap[pstTrainInfo->ulTrainUnitID];
   // if (m_mTrainCtrlMap.find(pstTrainInfo->ulTrainUnitID) == m_mTrainCtrlMap.end())
    if (!Train)
    {
        Train = new TcxButton(m_gbMap);

        Train->LookAndFeel->SkinName = GetDefaultSkinName();
        m_mTrainCtrlMap[pstTrainInfo->ulTrainUnitID] = Train;
        Train->Width = TrainMap::GetTrainWidth() - TrainMap::GetTrainMargin();
        Train->Layout = Buttons::blGlyphTop;
        Train->Caption = strName;
        Train->Font->Size = 10;

        Train->PopupMenu = m_pmTrain;
    }
    else
    {
        Train->Caption = strName;
    }

    Train->Height = TrainMap::GetTrainHeight();
    Train->Parent = m_gbMap;
    Train->SpeedButtonOptions->AllowAllUp = true;
    Train->SpeedButtonOptions->Transparent = true;
    Train->SpeedButtonOptions->CanBeFocused = false;

    TrainMap::GetStationPos(m_gbMap->Width, iStationIndex, x, y, &ulRowIndex);

    if (pstTrainInfo->ulDirection == TRAIN_DIRECTION_UPSIDE)
    {
        if (pstTrainInfo->ulDepotTrackFlag)
        {
            Train->Glyph = m_btnDepotTrainUp->Glyph;
        }
        else
        {
            Train->Glyph = m_btnTrainUp->Glyph;
        }

    }
    else if (pstTrainInfo->ulDirection == TRAIN_DIRECTION_DOWNSIDE)
    {
        if (pstTrainInfo->ulDepotTrackFlag)
        {
            Train->Glyph = m_btnDepotTrainDown->Glyph;
        }
        else
        {
            Train->Glyph = m_btnTrainDown->Glyph;
        }
    }

    Train->Visible = bVisible && IsTrainVisible(pstTrainInfo->ulTrainUnitID);

    int     iPos = 0;

    if (pstTrainInfo->ulDirection == TRAIN_DIRECTION_UPSIDE)
    {
        if (g_bReverseTrainPos)
        {
            Train->Top = y + TrainMap::GetStationHeight() + 2;
            Train->OptionsImage->Layout = blGlyphTop;

         /*   if (pstTrainInfo->ulDepotTrackFlag)
            {
                Train->Top += Train->Height;
            }    */
        }
        else
        {
            Train->Top = y - 2 - Train->Height;
            Train->OptionsImage->Layout = blGlyphBottom;

            /*
            if (pstTrainInfo->ulDepotTrackFlag)
            {
                Train->Top -= Train->Height;
            } */
        }

        iPos = pstTrainInfo->iTrainStationPosIndex;// - 2;
    }
    else if (pstTrainInfo->ulDirection == TRAIN_DIRECTION_DOWNSIDE)
    {
        if (g_bReverseTrainPos)
        {
            Train->Top = y - 2 - Train->Height;
            Train->OptionsImage->Layout = blGlyphBottom;

  /*          if (pstTrainInfo->ulDepotTrackFlag)
            {
                Train->Top -= Train->Height;
            } */
        }
        else
        {
            Train->Top = y + TrainMap::GetStationHeight() + 2;
            Train->OptionsImage->Layout = blGlyphTop;

           /* if (pstTrainInfo->ulDepotTrackFlag)
            {
                Train->Top += Train->Height;
            }    */
        }

        iPos = -pstTrainInfo->iTrainStationPosIndex;// - 2;
    }
    else
    {
        return;
    }

//    iPos *= Train->Width/2;
    iPos *= TrainMap::GetTrainWidth()/2;

    Train->Left = x + (TrainMap::GetColWidth() - Train->Width)/2 + iPos + TrainMap::GetTrainMargin()/2;

    Train->Tag = pstTrainInfo->ulTrainUnitID;
    Train->OnMouseDown = m_btnTrainMouseDown;
    Train->OnMouseEnter = m_btnTrainMouseEnter;
    Train->OnMouseLeave = m_btnTrainMouseLeave;
    Train->OnClick = m_btnTrainClick;
    Train->OnDragOver = m_OnDragOver;

    Train->OptionsImage->Margin = -1;
    Train->OptionsImage->Spacing = -2;

    bool bManageable = DCUser::IsStationInDCManageRange(pstTrainInfo->ulStationID);

    Train->Enabled = bManageable;
    if (!bManageable)
    {
        Train->Down = false;
    }
}

void InitTrainMap(BOOL bShowTrainUnitID, bool bShowDepotTrackTrain)
{
    UINT32  ulTrainUnitID;
    VECTOR<TRAIN_INFO_T*>   vTrainInfo;

    m_bShowTrainUnitID = bShowTrainUnitID;

    Train::GetTrainInfo(vTrainInfo);

    std::map<UINT32, TcxButton*>::iterator it;
    TRAIN_INFO_T        *pstTrainInfo;
    STATION_INFO_T      *pstStation;

    // 搜索不存在的，并隐藏
    for(it=m_mTrainCtrlMap.begin();it!=m_mTrainCtrlMap.end(); it++)
    {
        ulTrainUnitID = it->first;
        pstTrainInfo = Train::GetTrainInfo(ulTrainUnitID);
        pstStation = NULL;
        if (pstTrainInfo)
        {
            pstStation = Station::GetStationInfo(pstTrainInfo->ulStationID);
        }

        if (!pstStation || pstStation->ulStationType != STATION_TYPE_NORMAL)
        {
            if (it->second->Parent == m_gbMap)
            {
                it->second->Left = -1000;
            }
        }
    }

    // 计算列车位置
    TRAIN_POS_INFO_T    *pstTrainPos;
    UINT32              ulTrainPosKey;

    // 显示正线运营列车
    VectorMap<UINT32, TRAIN_POS_INFO_T>     mTrainPosInfo;          // 正线运营列车
    VectorMap<UINT32, TRAIN_POS_INFO_T>     mDepotTrainPosInfo;     // 正线停车线列车

    for (int i=vTrainInfo.size()-1; i>=0; i--)
    {
        pstTrainInfo = vTrainInfo[i];

        if (pstTrainInfo->ulTrainUnitID == 1407)
        {
            pstTrainInfo->ulTrainUnitID += 0;
        }

        pstStation = Station::GetStationInfo(pstTrainInfo->ulStationID);
        if (!pstStation)
        {
//            GosLog(LOG_ERROR, "get station of train %u failed", pstTrainInfo->ulTrainUnitID);
            vTrainInfo.erase(vTrainInfo.begin()+i);
            continue;
        }

        if (pstStation->ulStationType != STATION_TYPE_NORMAL)
        {
            vTrainInfo.erase(vTrainInfo.begin()+i);
            continue;
        }

        ulTrainPosKey = (pstTrainInfo->ulDirection<<16) | pstTrainInfo->ulStationID;
        if (!pstTrainInfo->ulDepotTrackFlag)
        {
            pstTrainPos = mTrainPosInfo.GetValue(ulTrainPosKey);
            if (!pstTrainPos)
            {
                TRAIN_POS_INFO_T    stTrainPos = {0};

                pstTrainPos = mTrainPosInfo.Add(ulTrainPosKey, stTrainPos);
            }
        }
        else
        {
            pstTrainPos = mDepotTrainPosInfo.GetValue(ulTrainPosKey);
            if (!pstTrainPos)
            {
                TRAIN_POS_INFO_T    stTrainPos = {0};

                pstTrainPos = mDepotTrainPosInfo.Add(ulTrainPosKey, stTrainPos);
            }
        }

        if (pstTrainPos->ulTrainNum == ARRAY_SIZE(pstTrainPos->apstTrainInfo))
        {
            continue;
        }

  //      InitTrainPos(pstTrainInfo);   // 本方法废弃
        pstTrainPos->apstTrainInfo[pstTrainPos->ulTrainNum] = pstTrainInfo;
//        pstTrainPos->aiTrainPos[pstTrainPos->ulTrainNum] = pstTrainPos->ulTrainNum;
        pstTrainPos->aiTrainOrder[pstTrainPos->ulTrainNum] = pstTrainInfo->iTrainOrder;
        pstTrainPos->ulTrainNum++;
    }

    for (UINT32 i=0; i<mTrainPosInfo.Size(); i++)
    {
        pstTrainPos = mTrainPosInfo[i];

        if (pstTrainPos->ulTrainNum == 0)
        {
            continue;
        }

        SetTrainPosIndex(pstTrainPos);
    }

    for (UINT32 i=0; i<mDepotTrainPosInfo.Size(); i++)
    {
        pstTrainPos = mDepotTrainPosInfo[i];

        if (pstTrainPos->ulTrainNum == 0)
        {
            continue;
        }

        SetTrainPosIndex(pstTrainPos);
    }

    for (UINT32 i=0; i<vTrainInfo.size(); i++)
    {
        ShowTrain(bShowDepotTrackTrain, vTrainInfo[i]);
    }
}

void InitStation(STATION_INFO_T *pstStation, bool bIsLast)
{
    TcxButton   *Station = NULL;

    if (m_mStationCtrlMap.find(pstStation->ulStationIndex) == m_mStationCtrlMap.end())
    {
        AnsiString  strName;

        if (m_bShowStationID)
        {
            strName = AnsiString(pstStation->ulStationID) + AnsiString(pstStation->acStationName);
        }
        else
        {
            strName = AnsiString(pstStation->acStationName);
        }

        Station = new TcxButton(m_gbMap);

        m_mStationCtrlMap[pstStation->ulStationIndex] = Station;

        Station->Parent = m_gbMap;
        Station->ParentFont = false;
        Station->Font->Size = 12;
        Station->Caption = strName;
        Station->PopupMenu = m_pmStation;

        Station->OnMouseEnter = m_btnStationMouseEnter;
        Station->OnMouseDown = m_btnStationMouseDown;
        Station->OnMouseLeave = m_btnStationMouseLeave;
        Station->OnClick = m_btnStationClick;
        Station->OnDragOver = m_OnDragOver;

        Station->Tag = pstStation->ulStationID;

        if (pstStation->ulInterLockStationID == pstStation->ulStationID)
        {
            Station->LookAndFeel->SkinName = UICfg::GetInterLockStationSkinName();
        }
        else
        {
            Station->LookAndFeel->SkinName = UICfg::GetStationSkinName();
        }
    }
    else
    {
        Station = m_mStationCtrlMap[pstStation->ulStationIndex];
    }

    Station->SpeedButtonOptions->AllowAllUp = true;
    Station->SpeedButtonOptions->CanBeFocused = false;

    int x;
    int y;

    TrainMap::GetStationPos(m_gbMap->Width, pstStation->ulStationIndex, x, y, NULL);

    Station->Left = x + (TrainMap::GetColWidth() - TrainMap::GetStationWidth() - TrainMap::GetStationMargin())/2;
    Station->Top = y;
    Station->Width = TrainMap::GetStationWidth();
    Station->Height = TrainMap::GetStationHeight();

    /*
    if (bIsLast)
    {
        gbMap->Height = Station->Top + Station->Height + 2 + 2*TrainMap::GetTrainHeight();
    } */

    bool bManageable = DCUser::IsStationInDCManageRange(pstStation->ulStationID);

    Station->Enabled = bManageable;
    if (!bManageable)
    {
        Station->Down = false;
    }

    Station->SpeedButtonOptions->Transparent = false;
}

UINT32 GetCurrStationID()
{
    std::map<UINT32, TcxButton*>::iterator it;
    TcxButton           *btn;
    UINT32              ulStationID = INVALID_STATION_ID;

    for(it=m_mStationCtrlMap.begin(); it!=m_mStationCtrlMap.end(); it++)
    {
        btn = it->second;
        if (btn->SpeedButtonOptions->Down)
        {
            if (ulStationID == INVALID_STATION_ID)
            {
                ulStationID = btn->Tag;
            }
            else
            {
                return INVALID_STATION_ID;   // 表示有多站被选中
            }
        }
    }

    return ulStationID;
}

void GetSelectTrain(UINT32 ulDepotID, VECTOR<UINT32> &vTrain)
{
    std::map<UINT32, TcxButton*>::iterator it;
    TcxButton           *btn;
    UINT32              ulTrainUnitID;

    for(it=m_mTrainCtrlMap.begin(); it!=m_mTrainCtrlMap.end(); it++)
    {
        btn = it->second;
        if (!btn->SpeedButtonOptions->Down)
        {
            continue;
        }

        if (Train::GetTrainDepotID(it->first) == ulDepotID)
        {
            ulTrainUnitID = it->first;
            vTrain.push_back(ulTrainUnitID);
        }
    }
}

UINT32 GetCurrTrainUnitID(UINT32 ulDepotID)
{
    std::map<UINT32, TcxButton*>::iterator it;
    TcxButton           *btn;
    UINT32              ulTrainUnitID = INVALID_TRAIN_UNIT_ID;

    for(it=m_mTrainCtrlMap.begin(); it!=m_mTrainCtrlMap.end(); it++)
    {
        btn = it->second;
        if (btn->SpeedButtonOptions->Down)
        {
            if (Train::GetTrainDepotID(it->first) == ulDepotID)
            {
                if (ulTrainUnitID == INVALID_TRAIN_UNIT_ID)
                {
                    ulTrainUnitID = it->first;
                }
                else
                {
                    return INVALID_TRAIN_UNIT_ID;   // 表示有多车被选中
                }
            }
        }
    }

    return ulTrainUnitID;
}

void GetTrain(VECTOR<TcxButton *> &vTrain)
{
    std::map<UINT32, TcxButton*>::iterator it;
    TcxButton           *btn;

    for(it=m_mTrainCtrlMap.begin(); it!=m_mTrainCtrlMap.end(); it++)
    {
        btn = it->second;
        vTrain.push_back(btn);
    }
}

void GetStation(VECTOR<TcxButton *> &vStation)
{
    std::map<UINT32, TcxButton*>::iterator it;
    TcxButton           *btn;

    for(it=m_mStationCtrlMap.begin(); it!=m_mStationCtrlMap.end(); it++)
    {
        btn = it->second;
        vStation.push_back(btn);
    }
}

}

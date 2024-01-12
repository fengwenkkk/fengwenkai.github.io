#include <vcl.h>
#include "g_include.h"

#include "GroupList.h"

// GroupImte
GroupItem::GroupItem(AnsiString strGroupName)
{
    m_strGroupName = strGroupName;
}

void GroupItem::Add(AnsiString strItemName, AnsiString strItemValue)
{
    NAME_VALUE_T    stItem;

    stItem.strName = strItemName;
    stItem.strValue = strItemValue;

    m_vNameValues.push_back(stItem);
}

void GroupItem::Add(AnsiString strItemName, AnsiString strItemValue1, AnsiString strItemValue2)
{
    Add(strItemName, strItemValue1 + strItemValue2);
}

bool GroupItem::Set(AnsiString strItemName, AnsiString strItemValue)
{
    for (UINT32 i=0; i<m_vNameValues.size(); i++)
    {
        if (m_vNameValues[i].strName == strItemName)
        {
            m_vNameValues[i].strValue = strItemValue;
            return true;
        }
    }

    return false;
}

NAME_VALUE_T* GroupItem::GetItem(UINT32 ulIndex)
{
    if (ulIndex >= m_vNameValues.size())
    {
        return NULL;
    }

    return &m_vNameValues[ulIndex];
}

// GroupList
GroupList::~GroupList()
{
    Clear();
}

GroupItem* GroupList::Add(AnsiString strGroupName)
{
    GroupItem   *pItem = new GroupItem(strGroupName);

    if (pItem)
    {
        m_vItems.push_back(pItem);
    }

    return pItem;
}

GroupItem* GroupList::GetItem(UINT32 ulIndex)
{
    if (ulIndex >= m_vItems.size())
    {
        return NULL;
    }

    return m_vItems[ulIndex];
}

void GroupList::Clear()
{
    for (UINT32 i=0; i<m_vItems.size(); i++)
    {
        GroupItem   *pItem = m_vItems[i];
        delete pItem;
    }

    m_vItems.clear();
}

void GroupList::Init(TcxLabel *lbl)
{
    lbl->Caption = "";

    if (m_vItems.size() == 0)
    {
        return;
    }

    UINT32          i, j;
    //int             iRowIndex = 0;
    NAME_VALUE_T    *pstNameValue;
    AnsiString      strReturn = "\r\n";
    UINT32          ulMaxLen = 0;
    UINT32          ulSpaceNum;
    AnsiString      strInfo = "";
    AnsiString      strLine;

    for (i=0; i<m_vItems.size(); i++)
    {
        GroupItem   *pItem = m_vItems[i];

        for (j=0; j<pItem->GetItemNum(); j++)
        {
            pstNameValue = pItem->GetItem(j);

            if (pstNameValue->strName.Length() > (int)ulMaxLen)
            {
                ulMaxLen = pstNameValue->strName.Length();
            }
        }
    }

    for (i=0; i<m_vItems.size(); i++)
    {
        GroupItem   *pItem = m_vItems[i];

        for (j=0; j<pItem->GetItemNum(); j++)
        {
            pstNameValue = pItem->GetItem(j);

            ulSpaceNum = ulMaxLen - pstNameValue->strName.Length();
            if (ulSpaceNum > 0)
            {
                strInfo += AnsiString::StringOfChar(' ', ulSpaceNum);
            }

            strLine = pstNameValue->strName + AnsiString(": ") +
                      pstNameValue->strValue + AnsiString("\r\n");

            strInfo += strLine;
        }
    }

    lbl->Caption = strInfo;
}

void GroupList::Init(TcxTreeList *tl, bool bShowGroup)
{
    TcxTreeListNode    *GroupNode;
    TcxTreeListNode    *Node;
    TcxTreeListColumn   *Column;

    tl->Clear();

    tl->OptionsView->ShowRoot = bShowGroup;
    tl->OptionsView->Indicator = true;

    if (m_vItems.size() == 0)
    {
        return;
    }

    UINT32          i, j;
    NAME_VALUE_T    *pstNameValue;

    for (i=0; i<m_vItems.size(); i++)
    {
        GroupItem   *pItem = m_vItems[i];

        if (bShowGroup)
        {
            GroupNode = tl->Add();
            GroupNode->Values[0] = pItem->GetGroupName();

            for (j=0; j<pItem->GetItemNum(); j++)
            {
                pstNameValue = pItem->GetItem(j);

                Node = GroupNode->AddChild();

                Node->Values[0] = pstNameValue->strName;
                Node->Values[1] = pstNameValue->strValue;
            }

            GroupNode->Expand(true);
        }
        else
        {
            for (j=0; j<pItem->GetItemNum(); j++)
            {
                pstNameValue = pItem->GetItem(j);

                Node = tl->Add();

                Node->Values[0] = pstNameValue->strName;
                Node->Values[1] = pstNameValue->strValue;
            }
        }
    }
}

#ifndef GroupListH
#define GroupListH

#include <vector>
#include "cxLabel.hpp"
#include "cxTL.hpp"

typedef struct
{
    AnsiString  strName;
    AnsiString  strValue;
}NAME_VALUE_T;

class GroupItem
{
public:
    GroupItem(AnsiString strGroupName);
    ~GroupItem(){};

    AnsiString  GetGroupName(){return m_strGroupName;};
    void Add(AnsiString strItemName, AnsiString strItemValue);
    void Add(AnsiString strItemName, AnsiString strItemValue1, AnsiString strItemValue2);
    bool Set(AnsiString strItemName, AnsiString strItemValue);

    UINT32          GetItemNum(){return m_vNameValues.size();};
    NAME_VALUE_T*   GetItem(UINT32 ulIndex);

private:
    AnsiString                      m_strGroupName;
    std::vector<NAME_VALUE_T>       m_vNameValues;
};

class GroupList
{
public:
    GroupList(){};
    ~GroupList();

    GroupItem*      Add(AnsiString strGroupName);
    UINT32          GetGroupNum(){return m_vItems.size();};
    GroupItem*      GetItem(UINT32 ulIndex);

    void            Clear();
    void            Init(TcxLabel *lbl);
    void            Init(TcxTreeList *tl, bool bShowGroup=true);

private:
    std::vector<GroupItem*>     m_vItems;
};

#endif

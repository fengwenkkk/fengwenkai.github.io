/**
 * @file            DictUtility.h
 * @brief           Dict 类的另一种实现
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-12-07 13:16:03
 * @copyright       Copyright (c) 2021 GBCOM
 */

#ifndef DS_CLIENT_PLAT_DICT_UTILITY_H
#define DS_CLIENT_PLAT_DICT_UTILITY_H

#include <vcl.h>
#include <cxDropDownEdit.hpp>
#include <string>
#include <map>
#include "g_include.h"

namespace gos
{
class DictUtility
{
public:
    DictUtility() {}
    DictUtility(const std::map<INT32, std::string>& tempMap) : m_KeyValueMap(tempMap) {}

    /**
     * @brief           初始化本数据结构
     * @param           Map    [in]
     * @return          VOID
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-12-07 13:37:09
     * @note
     */
    VOID Init(const std::map<INT32, std::string>& Map)
    {
        m_KeyValueMap = Map;
    }

    /**
     * @brief           初始化下拉列表
     * @param           cb          [in/out]  下拉列表
     * @param           bClear      [in]       是否首先清空下拉列表
     * @param           bSkipFirst  [in]       是否跳过首项
     * @return          VOID
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-12-07 13:37:20
     * @note
     */
    VOID InitComboBox(TcxComboBox* cb, const BOOL bClear = TRUE, const BOOL bSkipFirst = FALSE)
    {
        if (!cb)
        {
            return;
        }

        if (bClear)
        {
            /// 清除下拉框内的内容
            cb->Properties->Items->Clear();
        }

        std::map<INT32, std::string>::iterator it = m_KeyValueMap.begin();
        BOOL bLoopFirst = TRUE;
        /// 在下拉框内添加内容
        for (; it != m_KeyValueMap.end(); ++it)
        {
            if (bSkipFirst && bLoopFirst)
            {
                bLoopFirst = FALSE;
                continue;
            }

            cb->Properties->Items->Add(it->second.c_str());
        }

        cb->ItemIndex = -1;
    }

    /**
     * @brief           获取ID的数值
     * @param           cb      [in]
     * @param           iID     [out]   输出的 ID
     * @return          BOOL
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-12-07 14:09:41
     * @note
     */
    BOOL GetComoboBoxID(TcxComboBox* cb, INT32& iID)
    {
        AnsiString str = cb->Text;
        return GetID(str.c_str(), iID);
    }

    /**
     * @brief           设置 ComoboBox 的值
     * @param           cb      [in/out]
     * @param           iID     [in]
     * @return          BOOL
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-12-07 14:18:55
     * @note
     */
    BOOL SetComoboBoxID(TcxComboBox* cb, const INT32& iID)
    {
        std::string strValue;
        if (GetString(iID, strValue))
        {
            cb->ItemIndex = cb->Properties->Items->IndexOf(strValue.c_str());
            return TRUE;
        }
        return FALSE;
    }

    /**
     * @brief           通过 string 获取 ID
     * @param           str     [in]
     * @param           ID      [out]
     * @return          BOOL
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-12-07 14:19:16
     * @note
     */
    BOOL GetID(const std::string& str, INT32& iID)
    {
        std::map<INT32, std::string>::iterator it = m_KeyValueMap.begin();
        /// 在下拉框内添加内容
        for (; it != m_KeyValueMap.end(); ++it)
        {
            if (it->second == str)
            {
                iID = it->first;
                return TRUE;
            }
        }
        return FALSE;
    }

    /**
     * @brief           通过 ID 获取 string
     * @param           ID      [in]
     * @param           str     [out]
     * @return          BOOL
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-12-07 14:20:11
     * @note
     */
    BOOL GetString(const INT32& ID, std::string& str)
    {
        if (m_KeyValueMap.find(ID) != m_KeyValueMap.end())
        {
            str = m_KeyValueMap[ID];
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

private:
    std::map<INT32, std::string> m_KeyValueMap;
};
}  // namespace gos

#endif  ///< DS_CLIENT_PLAT_DICT_UTILITY_H
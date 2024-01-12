/**
 * @file          parser_interface.hpp
 * @brief         存放解析接口类的实现
 * @author        lijiancong(lijiancong@gbcom.com.cn)
 * @version       0.1
 * @date          2021-04-23 09:04:29
 *
 * @copyright     Copyright (c) 2021 GBCOM
 *
 */

#ifndef GOS_PARSER_INTERFACE_HPP__
#define GOS_PARSER_INTERFACE_HPP__

#include <vector>
#include <string>
#include "g_json.h"

namespace gos
{
/**
 * @brief         定义解析字符串的接口类
 *
 * @tparam        FORMAT_T  传入的结构体
 *
 * @author        lijiancong(lijiancong@gbcom.com.cn)
 */
template <typename FORMAT_T>
class ParserInterface
{
public:
    /// 虚析构函数
    virtual ~ParserInterface() {}

    /**
     * @brief         用来把结构体来转换成格式字符串
     * @param         stInfo    [in]    数据结构体
     * @param         str       [out]   格式字符串
     * @return        true      转换成功
     * @return        false     转换失败
     *
     * @author        lijiancong(lijiancong@gbcom.com.cn)
     */
    virtual bool ToFormatString(const FORMAT_T &stInfo, std::string &str) = 0;

    /**
     * @brief         用来把格式字符串转换成数据结构体
     * @param         strFormatString   [in]    传入的格式字符串
     * @param         stInfo            [out]   解析后传出的字符串
     * @return        true              转换成功
     * @return        false             转换失败
     *
     * @author        lijiancong(lijiancong@gbcom.com.cn)
     */
    virtual bool ToStruct(const std::string &strFormatString, FORMAT_T &stInfo) = 0;

    /**
     * @brief           把该结构体的数组转为格式字符串字符串
     * @param           stInfo    [in]  入参结构体
     * @param           str       [out] 组装好的格式化字符串
     * @return          true    格式化成功
     * @return          false   格式化失败
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @note
     */
    bool ToVecString(const std::vector<FORMAT_T> &vec, std::string &str)
    {
        GJsonTupleParam Tuple;
        for (size_t i = 0; i < vec.size(); ++i)
        {
            std::string strSub;
            if (!ToFormatString(vec.at(i), strSub))
            {
                return false;
            }
            Tuple.Add(strSub);
        }
        str = Tuple.GetString();
        return true;
    }

    /**
     * @brief           把字符串转化为结构体数组
     * @param           strFormatString     [in]    格式化字符串
     * @param           vec                 [out]   结构体数组
     * @return          true
     * @return          false
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @note
     */
    bool ToVector(const std::string &strFormatString, std::vector<FORMAT_T> &vec)
    {
        if (strFormatString == "null")
        {
            return true;
        }

        vec.clear();
        GJson Groups;
        if (!Groups.Parse(strFormatString))
        {
            return false;
        }

        std::vector<GJson *> vecJson = Groups.GetSubJson();
        for (size_t i = 0; i < vecJson.size(); ++i)
        {
            FORMAT_T st;
            if (!ToStruct(vecJson.at(i)->GetText(), st))
            {
                return false;
            }
            vec.push_back(st);
        }
        return true;
    }
};
}  // namespace gos

#endif  ///< GOS_PARSER_INTERFACE_HPP__

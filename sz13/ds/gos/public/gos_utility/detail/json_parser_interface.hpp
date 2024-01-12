/**
 * @file            json_parser_interface.hpp
 * @brief           json 解析器接口类实现
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-12-29 11:34:08
 * @copyright       Copyright (c) 2021 GBCOM
 */

#ifndef GOS_PUBLIC_GOS_UTILITY_JSON_PARSER_INTERFACE_HPP__
#define GOS_PUBLIC_GOS_UTILITY_JSON_PARSER_INTERFACE_HPP__

#include <string>

namespace gos
{
template <typename T>
class json_parser
{
public:

    /// 虚析构函数, 防止内存泄漏
    virtual ~json_parser() {}

    /**
     * @brief           结构体转 Json 字符串的接口函数
     * @param           str    [out]
     * @return          true
     * @return          false
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2022-01-04 08:53:10
     * @note
     */
    virtual bool StructToJson(std::string &str) const = 0;

    /**
     * @brief           Json 字符串转结构体
     * @param           str    [out]
     * @return          true
     * @return          false
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2022-01-04 08:54:41
     * @note
     */
    virtual bool JsonToStruct(const std::string &str) = 0;

    /**
     * @brief           转化结构体数组到 Json 字符串
     * @param           vec    [in]  入参结构体数组
     * @param           str    [out] 生成的 Json 字符串
     * @return          true
     * @return          false
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-12-29 11:42:56
     * @note
     */
    bool VectorToJson(const std::vector<T> &vec, std::string &str) const
    {
        GJsonTupleParam Tuple;
        for (size_t i = 0; i < vec.size(); ++i)
        {
            std::string strSub;
            if (!vec.at(i).StructToJson(strSub))
            {
                return false;
            }
            Tuple.Add(strSub);
        }
        str = Tuple.GetString();
        return true;
    }

    /**
     * @brief           转换 Json 字符串到结构体数组
     * @param           strFormatString     [in]    Json 字符串
     * @param           vec                 [out]   结构体数组
     * @return          true
     * @return          false
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-12-29 11:44:30
     * @note
     */
    bool JsonToVector(const std::string &strFormatString, std::vector<T> &vec) const
    {
        if (strFormatString == "null")
        {
            return true;
        }

        vec.clear();
        GJson Json;
        if (!Json.Parse(strFormatString))
        {
            return false;
        }

        std::vector<GJson *> vecJson = Json.GetSubJson();
        for (size_t i = 0; i < vecJson.size(); ++i)
        {
            T st;
            if (!st.JsonToStruct(vecJson.at(i)->GetText()))
            {
                return false;
            }
            vec.push_back(st);
        }
        return true;
    }
};
}  // namespace gos

#endif  ///< GOS_PUBLIC_GOS_UTILITY_JSON_PARSER_INTERFACE_HPP__
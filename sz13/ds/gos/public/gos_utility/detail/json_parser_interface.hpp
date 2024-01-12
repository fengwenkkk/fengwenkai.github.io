/**
 * @file            json_parser_interface.hpp
 * @brief           json �������ӿ���ʵ��
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

    /// ����������, ��ֹ�ڴ�й©
    virtual ~json_parser() {}

    /**
     * @brief           �ṹ��ת Json �ַ����Ľӿں���
     * @param           str    [out]
     * @return          true
     * @return          false
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2022-01-04 08:53:10
     * @note
     */
    virtual bool StructToJson(std::string &str) const = 0;

    /**
     * @brief           Json �ַ���ת�ṹ��
     * @param           str    [out]
     * @return          true
     * @return          false
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2022-01-04 08:54:41
     * @note
     */
    virtual bool JsonToStruct(const std::string &str) = 0;

    /**
     * @brief           ת���ṹ�����鵽 Json �ַ���
     * @param           vec    [in]  ��νṹ������
     * @param           str    [out] ���ɵ� Json �ַ���
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
     * @brief           ת�� Json �ַ������ṹ������
     * @param           strFormatString     [in]    Json �ַ���
     * @param           vec                 [out]   �ṹ������
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
/**
 * @file          parser_interface.hpp
 * @brief         ��Ž����ӿ����ʵ��
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
 * @brief         ��������ַ����Ľӿ���
 *
 * @tparam        FORMAT_T  ����Ľṹ��
 *
 * @author        lijiancong(lijiancong@gbcom.com.cn)
 */
template <typename FORMAT_T>
class ParserInterface
{
public:
    /// ����������
    virtual ~ParserInterface() {}

    /**
     * @brief         �����ѽṹ����ת���ɸ�ʽ�ַ���
     * @param         stInfo    [in]    ���ݽṹ��
     * @param         str       [out]   ��ʽ�ַ���
     * @return        true      ת���ɹ�
     * @return        false     ת��ʧ��
     *
     * @author        lijiancong(lijiancong@gbcom.com.cn)
     */
    virtual bool ToFormatString(const FORMAT_T &stInfo, std::string &str) = 0;

    /**
     * @brief         �����Ѹ�ʽ�ַ���ת�������ݽṹ��
     * @param         strFormatString   [in]    ����ĸ�ʽ�ַ���
     * @param         stInfo            [out]   �����󴫳����ַ���
     * @return        true              ת���ɹ�
     * @return        false             ת��ʧ��
     *
     * @author        lijiancong(lijiancong@gbcom.com.cn)
     */
    virtual bool ToStruct(const std::string &strFormatString, FORMAT_T &stInfo) = 0;

    /**
     * @brief           �Ѹýṹ�������תΪ��ʽ�ַ����ַ���
     * @param           stInfo    [in]  ��νṹ��
     * @param           str       [out] ��װ�õĸ�ʽ���ַ���
     * @return          true    ��ʽ���ɹ�
     * @return          false   ��ʽ��ʧ��
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
     * @brief           ���ַ���ת��Ϊ�ṹ������
     * @param           strFormatString     [in]    ��ʽ���ַ���
     * @param           vec                 [out]   �ṹ������
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

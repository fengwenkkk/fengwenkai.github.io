/**
 * @file          database_operator.hpp
 * @brief         ���ݿ���������ĺ���
 * @author        lijiancong(lijiancong@gbcom.com.cn)
 * @version       0.1
 * @date          2021-04-22 16:14:30
 *
 * @copyright     Copyright (c) 2021 GBCOM
 *
 */

#ifndef DS_DS_PUBLIC_LEE_UTILITY_DETAIL_HPP__
#define DS_DS_PUBLIC_LEE_UTILITY_DETAIL_HPP__

#include <vector>

namespace gos
{
/**
 * @brief           ���ݿ�Ľӿں���
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @par ʾ��:
 * @code
namespace db {
/// SDSInfo��ṹ�Ķ���
namespace sdsinfo {
    /// SDSInfo ����������
    struct Key
    {
        int iSDSID; ///< ���ӹ���ID
    };

    /// SDSInfo��ṹ����
    struct Info
    {
        int iSDSID;         ///< ���ӹ���ID
        int iSendTime;      ///< ����ʱ��
        int iSendUserID;    ///< �����û�ID
        char acSendUserName[32]; ///< ����������
        int iSDSType;       ///< ���ӹ�������
        int iSDSIndex;      ///< ���ӹ���Index
    };

    struct SDSType
    {
        static const int SDS_TYPE_OPERATE = 3;   ///< ��Ӫ��Ϣ����
        static const int SDS_TYPE_NORMAL  = 9;   ///< ��ͨ��Ϣ
    };
}
}

class DBInstance{};

/// ��SDSInfo��Ĳ�����
class SDSInfoOperator : public DataBaseOperatorInterface<db::sdsinfo::Info, db::sdsinfo::Key, DBInstance>
{
public:
    virtual bool Insert(const db::sdsinfo::Info& stInfo, const DBInstance& pDao) {return true;}
    virtual bool Delete(const db::sdsinfo::Key& stKey, const DBInstance& pDao) {return true;}
    virtual bool Update(const db::sdsinfo::Info& stInfo, const DBInstance& pDao) {return true;}
    virtual bool Query(const db::sdsinfo::Key& stKey, const DBInstance& pDao, db::sdsinfo::Info& stInfo) {return true;}
    virtual bool InsertOrUpdate(const db::sdsinfo::Info& stInfo, const DBInstance& pDao) {return true;}
};
}
 * @endcode
 * @note
 */
template <typename TABLE_KEY, typename TABLE_INFO, typename DB_TEMPLATE>
class DataBaseOperatorInterface
{
public:
    /// ����������
    virtual ~DataBaseOperatorInterface() {}
    /// ����һ����¼
    virtual bool Insert(const TABLE_INFO& stInfo, DB_TEMPLATE pDao) = 0;
    /// ɾ��һ����¼
    virtual bool Delete(const TABLE_KEY& stKey, DB_TEMPLATE pDao) = 0;
    /// ����һ����¼
    virtual bool Update(const TABLE_INFO& stInfo, DB_TEMPLATE pDao) = 0;
    /// ��ѯһ����¼
    virtual bool Query(const TABLE_KEY& stKey, DB_TEMPLATE pDao, TABLE_INFO& stInfo) = 0;
    /// ����һ����¼�����������¼��������¸�����¼
    /// virtual bool InsertOrUpdate(const TABLE_INFO& stInfo, const DB_TEMPLATE& pDao) = 0;
};
}  // namespace gos

#endif  ///< DS_DS_PUBLIC_LEE_UTILITY_DETAIL_HPP__

/**
 * @file          database_operator.hpp
 * @brief         数据库操作类对象的函数
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
 * @brief           数据库的接口函数
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @par 示例:
 * @code
namespace db {
/// SDSInfo表结构的定义
namespace sdsinfo {
    /// SDSInfo 表主键定义
    struct Key
    {
        int iSDSID; ///< 电子工单ID
    };

    /// SDSInfo表结构定义
    struct Info
    {
        int iSDSID;         ///< 电子工单ID
        int iSendTime;      ///< 发送时间
        int iSendUserID;    ///< 发送用户ID
        char acSendUserName[32]; ///< 发送者名称
        int iSDSType;       ///< 电子工单类型
        int iSDSIndex;      ///< 电子工单Index
    };

    struct SDSType
    {
        static const int SDS_TYPE_OPERATE = 3;   ///< 运营信息发布
        static const int SDS_TYPE_NORMAL  = 9;   ///< 普通消息
    };
}
}

class DBInstance{};

/// 对SDSInfo表的操作类
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
    /// 虚析构函数
    virtual ~DataBaseOperatorInterface() {}
    /// 插入一条记录
    virtual bool Insert(const TABLE_INFO& stInfo, DB_TEMPLATE pDao) = 0;
    /// 删除一条记录
    virtual bool Delete(const TABLE_KEY& stKey, DB_TEMPLATE pDao) = 0;
    /// 更新一条记录
    virtual bool Update(const TABLE_INFO& stInfo, DB_TEMPLATE pDao) = 0;
    /// 查询一条记录
    virtual bool Query(const TABLE_KEY& stKey, DB_TEMPLATE pDao, TABLE_INFO& stInfo) = 0;
    /// 插入一条记录，如果该条记录存在则更新该条记录
    /// virtual bool InsertOrUpdate(const TABLE_INFO& stInfo, const DB_TEMPLATE& pDao) = 0;
};
}  // namespace gos

#endif  ///< DS_DS_PUBLIC_LEE_UTILITY_DETAIL_HPP__

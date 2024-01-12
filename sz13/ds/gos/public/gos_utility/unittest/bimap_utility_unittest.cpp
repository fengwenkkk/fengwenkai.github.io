/**
 * @file            bimap_utility_unittest.cpp
 * @brief           测试双键数据结构
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-08-21 10:47:44
 *
 * @copyright       Copyright (c) 2021 GBCOM
 *
 */
#include <string>
#include "g_include.h"

#ifdef USE_GOS_UNITTEST

struct test_bimap
{
    int iGroupID;
    std::string strSessionID;

    test_bimap() : iGroupID(-1) {}
    test_bimap(const int& i, const std::string& str) : iGroupID(i), strSessionID(str) {}
    bool operator==(const test_bimap& other) const
    {
        return other.iGroupID == iGroupID && other.strSessionID == strSessionID;
    }
};

TEST_CASE("gos::bimap 简单测试", "[gos][bimap]")
{
    gos::bimap<int, std::string, test_bimap> m_biMap;
    test_bimap st(55, "666");
    test_bimap st1(77, "777");
    test_bimap st2(88, "888");

    /// "insert and find"
    REQUIRE(m_biMap.insert(st.iGroupID, st.strSessionID, st));
    REQUIRE(m_biMap.insert(st1.iGroupID, st1.strSessionID, st1));
    REQUIRE(m_biMap.insert(st2.iGroupID, st2.strSessionID, st2));
    if (m_biMap.left_find(st.iGroupID) != m_biMap.end())
    {
        REQUIRE(m_biMap.left_find(st.iGroupID)->value == st);
        REQUIRE(m_biMap.right_find(st.strSessionID)->value == st);
    }

    /// "delete"
    REQUIRE(m_biMap.left_erase(st.iGroupID));
    REQUIRE(m_biMap.left_find(st.iGroupID) == m_biMap.end());
    REQUIRE(m_biMap.right_find(st.strSessionID) == m_biMap.end());

    /// "clear and empty"
    REQUIRE(!m_biMap.empty());
    m_biMap.clear();
    REQUIRE(m_biMap.empty());
}

#endif
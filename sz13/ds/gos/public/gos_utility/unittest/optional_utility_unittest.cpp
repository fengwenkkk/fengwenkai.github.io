/**
 * @file          optional_utility_unittest.hpp
 * @brief         optional_utility的单元测试文件
 * @author        lijiancong(lijiancong@gbcom.com.cn)
 * @version       0.1
 * @date          2021-04-28 15:44:38
 *
 * @copyright     Copyright (c) 2021 GBCOM
 *
 */

#include "g_include.h"

#ifdef  USE_GOS_UNITTEST

TEST_CASE("gos::optional() 简单测试", "[lee][optional]"){
    gos::optional<int> op;
    REQUIRE(!op.has_value());
    REQUIRE(op.value_or(1111) == 1111);
}

TEST_CASE("gos::optional(T) 简单测试", "[lee][optional]"){
    gos::optional<int> op(1);
    REQUIRE(op.has_value());
    REQUIRE(op.value_or(1111) == 1);
}

TEST_CASE("gos::optional(const optional& other) 简单测试", "[lee][optional]") {
    gos::optional<int> op;
    gos::optional<int> temp = op;
    REQUIRE(!temp.has_value());
    REQUIRE(temp.value_or(1111) == 1111);

    gos::optional<int> op2(2);
    gos::optional<int> temp2(op2);
    REQUIRE(temp2.has_value());
    REQUIRE(temp2.value_or(1111) == 2);
}

inline gos::optional<int> test_int_empty_optional() {
    return gos::optional<int>();
}

inline gos::optional<std::string> test_string_empty_optional() {
    return gos::optional<std::string>();
}

inline gos::optional<int> test_int_optional() {
    return gos::optional<int>(1);
}

inline gos::optional<std::string> test_string_optional() {
    return gos::optional<std::string>("test_string_optional");
}

inline gos::optional<std::vector<int> > test_vector_empty_optional(){
    gos::optional<std::vector<int> > temp;
    return temp;
}

inline gos::optional<std::vector<int> > test_vector_optional() {
    std::vector<int> vec(2, 1);
    return gos::optional<std::vector<int> >(vec);
}

TEST_CASE("gos::optional 简单测试", "[lee][optional]") {
    gos::optional<int> temp = test_int_empty_optional();
    REQUIRE(!temp.has_value());
    REQUIRE(temp.value_or(11) == 11);

    gos::optional<std::string> temp1 = test_string_empty_optional();
    REQUIRE(!temp1.has_value());
    REQUIRE(temp1.value_or("") == "");

    gos::optional<int> temp2 = test_int_optional();
    REQUIRE(temp2.has_value());
    REQUIRE(temp2.value_or(11) == 1);

    gos::optional<std::string> temp3 = test_string_optional();
    REQUIRE(temp3.has_value());
    REQUIRE(temp3.value_or("") == "test_string_optional");

    gos::optional<std::vector<int> > temp4 = test_vector_empty_optional();
    REQUIRE(!temp4.has_value());
    std::vector<int> vec;
    REQUIRE(temp4.value_or(vec) == vec);

    gos::optional<std::vector<int> > temp5 = test_vector_optional();
    REQUIRE(temp5.has_value());
    std::vector<int> vec2(2, 1);
    REQUIRE(temp5.value_or(vec) == vec2);
}

inline gos::optional<int> test_int_optional_2() {
    return 1;
}

inline gos::optional<std::string> test_string_optional_2() {
    return std::string("test2");
}

struct test_optional_struct {
    int x;
    std::string y;
    bool operator==(const test_optional_struct& other) const {
        return x == other.x && y == other.y;
    }
};

inline gos::optional<test_optional_struct> test_struct_optional_2() {
    test_optional_struct xx;
    xx.x = 1;
    xx.y = "test2";
    return gos::optional<test_optional_struct>(xx);
}

TEST_CASE("gos::optional 隐式构造函数简单测试", "[lee][optional]") {
    REQUIRE(test_int_optional_2().has_value());
    REQUIRE(test_int_optional_2().value() == 1);

    REQUIRE(test_string_optional_2().has_value());
    REQUIRE(test_string_optional_2().value() == "test2");

    REQUIRE(test_struct_optional_2().has_value());
    test_optional_struct s = {1, std::string("test2")};
    REQUIRE(test_struct_optional_2().value() == s);
}

#endif

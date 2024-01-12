/**
 * @file            string_stream_unittest.cpp
 * @brief           string_stream 的测试用例
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-08-05 17:39:11
 *
 * @copyright       Copyright (c) 2021 GBCOM
 *
 */
#include "g_include.h"

#ifdef USE_GOS_UNITTEST

TEST_CASE("string_stream 的简单测试", "[gos][string_stream]")
{
    int i = -10;
    unsigned int ul = 109;
    bool b0 = true;
    bool b1 = false;
    double d = 0.10;
    float f = static_cast<float>(-0.10);
    gos::string_stream stream;
    stream << i << ul << b0 << b1 << d << f;
    REQUIRE("-10109truefalse0.1-0.1" == stream.str());
}

#endif
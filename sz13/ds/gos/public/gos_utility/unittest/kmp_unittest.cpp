#include "g_include.h"

#ifdef  USE_GOS_UNITTEST

TEST_CASE("kmp 的简单测试", "[gos][kmp]")
{
    std::string src;
    std::string find;

    src = "safadfaf";
    find = "af";
    REQUIRE(1 == gos::kmp_search(src.c_str(), find.c_str()));

    src = "safadfaf";
    find = "afadfaf";
    REQUIRE(1 == gos::kmp_search(src.c_str(), find.c_str()));

    src = "safadfaf";
    find = "1afadfaf";
    REQUIRE(std::string::npos == gos::kmp_search(src.c_str(), find.c_str()));

    src = "safa\r\ndfaf";
    find = "\r\n";
    REQUIRE(4 == gos::kmp_search(src.c_str(), find.c_str()));

    src = "safa\rdfaf";
    find = "\r\n";
    REQUIRE(std::string::npos == gos::kmp_search(src.c_str(), find.c_str()));

    src = "safa\rdfaf\r\n";
    find = "";
    REQUIRE(0 == gos::kmp_search(src.c_str(), find.c_str()));
}

TEST_CASE("kmp 的偏移量测试", "[gos][kmp]")
{
    std::string src;
    std::string find;

    src = "safadfaf";
    find = "af";
    REQUIRE(1 == gos::kmp_search(src.c_str(), find.c_str(), 1));

    src = "safadfaf";
    find = "af";
    REQUIRE(6 == gos::kmp_search(src.c_str(), find.c_str(), 2));

    src = "safadfaf";
    find = "af";
    REQUIRE(std::string::npos == gos::kmp_search(src.c_str(), find.c_str(), 7));

    src = "safadfaf";
    find = "af";
    REQUIRE(std::string::npos == gos::kmp_search(src.c_str(), find.c_str(), 8));
}

TEST_CASE("kmp 的特殊值测试", "[gos][kmp]")
{
    std::string src;
    std::string find;

    src = "safadfaf";
    find = "af";
    REQUIRE(std::string::npos == gos::kmp_search(NULL, NULL));
    REQUIRE(std::string::npos == gos::kmp_search(src.c_str(), NULL));
    REQUIRE(std::string::npos == gos::kmp_search(NULL, find.c_str()));

    char acSrc[] = "asfasf";
    char acFind[] = "as";
    REQUIRE(0 == gos::kmp_search(acSrc, acFind));
    REQUIRE(3 == gos::kmp_search(acSrc, acFind, 1));

    char acSrc0[] = "asfasf";
    char acFind0[] = "\r\n";

    REQUIRE(std::string::npos == gos::kmp_search(acSrc0, acFind0));
}

#endif
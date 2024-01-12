
#include "g_include.h"
#include <cstring>

#ifdef USE_GOS_UNITTEST

#define TEST_BINARY_FILE_NAME "./binary_file_test.bin"
#define TEST_BINARY_FOLDER_FILE_NAME "./test/binary_file_test.bin"

TEST_CASE("binary_file 的简单测试", "[gos][binary_file]")
{
    /// 如果存在测试文件则先删除
    gos_delete_file(TEST_BINARY_FILE_NAME);

    /// 创建读写对象
    gos::binary_file obj(TEST_BINARY_FILE_NAME);
    unsigned char auc[256] = {0};
    for (unsigned i = 0; i < 256; i++)
    {
        auc[i] = i;
    }

    /// 写入内容
    REQUIRE(256 == obj.write(auc, 256));

    /// 第一次读取
    unsigned char aucRead[256] = {0};
    REQUIRE(256 == obj.read(aucRead, sizeof(aucRead)));
    REQUIRE(0 == memcmp(auc, aucRead, sizeof(auc)));

    /// 第二次读取
    unsigned char aucRead1[256] = {0};
    REQUIRE(256 == obj.read(aucRead1, sizeof(aucRead1)));
    REQUIRE(0 == memcmp(auc, aucRead1, sizeof(auc)));

    /// 第二次写入
    REQUIRE(256 == obj.write(auc, 256));
    /// 读取
    unsigned char aucRead2[256 * 2] = {0};
    REQUIRE(256 * 2 == obj.read(aucRead2, sizeof(aucRead2)));
    REQUIRE(0 == memcmp(auc, aucRead2, sizeof(auc)));
    REQUIRE(0 == memcmp(auc, aucRead2 + sizeof(auc), sizeof(auc)));
}

TEST_CASE("binary_file 的读取测试", "[gos][binary_file]")
{
    if (gos_file_exist(TEST_BINARY_FILE_NAME))
    {
        gos::binary_file obj(TEST_BINARY_FILE_NAME);
        unsigned char auc[256] = {0};
        for (unsigned i = 0; i < 256; i++)
        {
            auc[i] = i;
        }
        /// 读取
        unsigned char aucRead2[256 * 2] = {0};
        REQUIRE(256 * 2 == obj.read(aucRead2, sizeof(aucRead2)));
        REQUIRE(0 == memcmp(auc, aucRead2, sizeof(auc)));
        REQUIRE(0 == memcmp(auc, aucRead2 + sizeof(auc), sizeof(auc)));
    }
}

TEST_CASE("binary_file 的带文件夹前缀写入测试", "[gos][binary_file]")
{
    /// 如果存在测试文件则先删除
    gos_delete_file(TEST_BINARY_FOLDER_FILE_NAME);

    /// 创建读写对象
    gos::binary_file obj(TEST_BINARY_FOLDER_FILE_NAME);
    unsigned char auc[256] = {0};
    for (unsigned i = 0; i < 256; i++)
    {
        auc[i] = i;
    }

    /// 写入内容
    REQUIRE(256 == obj.write(auc, 256));

    /// 第一次读取
    unsigned char aucRead[256] = {0};
    REQUIRE(256 == obj.read(aucRead, sizeof(aucRead)));
    REQUIRE(0 == memcmp(auc, aucRead, sizeof(auc)));

    /// 第二次读取
    unsigned char aucRead1[256] = {0};
    REQUIRE(256 == obj.read(aucRead1, sizeof(aucRead1)));
    REQUIRE(0 == memcmp(auc, aucRead1, sizeof(auc)));

    /// 第二次写入
    REQUIRE(256 == obj.write(auc, 256));
    /// 读取
    unsigned char aucRead2[256 * 2] = {0};
    REQUIRE(256 * 2 == obj.read(aucRead2, sizeof(aucRead2)));
    REQUIRE(0 == memcmp(auc, aucRead2, sizeof(auc)));
    REQUIRE(0 == memcmp(auc, aucRead2 + sizeof(auc), sizeof(auc)));
}

TEST_CASE("binary_file 的带文件夹前缀读取测试", "[gos][binary_file]")
{
    if (gos_file_exist(TEST_BINARY_FOLDER_FILE_NAME))
    {
        gos::binary_file obj(TEST_BINARY_FOLDER_FILE_NAME);
        unsigned char auc[256] = {0};
        for (unsigned i = 0; i < 256; i++)
        {
            auc[i] = i;
        }
        /// 读取
        unsigned char aucRead2[256 * 2] = {0};
        REQUIRE(256 * 2 == obj.read(aucRead2, sizeof(aucRead2)));
        REQUIRE(0 == memcmp(auc, aucRead2, sizeof(auc)));
        REQUIRE(0 == memcmp(auc, aucRead2 + sizeof(auc), sizeof(auc)));
    }
}

#endif
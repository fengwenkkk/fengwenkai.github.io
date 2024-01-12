
#include "g_include.h"
#include <cstring>

#ifdef USE_GOS_UNITTEST

#define TEST_BINARY_FILE_NAME "./binary_file_test.bin"
#define TEST_BINARY_FOLDER_FILE_NAME "./test/binary_file_test.bin"

TEST_CASE("binary_file �ļ򵥲���", "[gos][binary_file]")
{
    /// ������ڲ����ļ�����ɾ��
    gos_delete_file(TEST_BINARY_FILE_NAME);

    /// ������д����
    gos::binary_file obj(TEST_BINARY_FILE_NAME);
    unsigned char auc[256] = {0};
    for (unsigned i = 0; i < 256; i++)
    {
        auc[i] = i;
    }

    /// д������
    REQUIRE(256 == obj.write(auc, 256));

    /// ��һ�ζ�ȡ
    unsigned char aucRead[256] = {0};
    REQUIRE(256 == obj.read(aucRead, sizeof(aucRead)));
    REQUIRE(0 == memcmp(auc, aucRead, sizeof(auc)));

    /// �ڶ��ζ�ȡ
    unsigned char aucRead1[256] = {0};
    REQUIRE(256 == obj.read(aucRead1, sizeof(aucRead1)));
    REQUIRE(0 == memcmp(auc, aucRead1, sizeof(auc)));

    /// �ڶ���д��
    REQUIRE(256 == obj.write(auc, 256));
    /// ��ȡ
    unsigned char aucRead2[256 * 2] = {0};
    REQUIRE(256 * 2 == obj.read(aucRead2, sizeof(aucRead2)));
    REQUIRE(0 == memcmp(auc, aucRead2, sizeof(auc)));
    REQUIRE(0 == memcmp(auc, aucRead2 + sizeof(auc), sizeof(auc)));
}

TEST_CASE("binary_file �Ķ�ȡ����", "[gos][binary_file]")
{
    if (gos_file_exist(TEST_BINARY_FILE_NAME))
    {
        gos::binary_file obj(TEST_BINARY_FILE_NAME);
        unsigned char auc[256] = {0};
        for (unsigned i = 0; i < 256; i++)
        {
            auc[i] = i;
        }
        /// ��ȡ
        unsigned char aucRead2[256 * 2] = {0};
        REQUIRE(256 * 2 == obj.read(aucRead2, sizeof(aucRead2)));
        REQUIRE(0 == memcmp(auc, aucRead2, sizeof(auc)));
        REQUIRE(0 == memcmp(auc, aucRead2 + sizeof(auc), sizeof(auc)));
    }
}

TEST_CASE("binary_file �Ĵ��ļ���ǰ׺д�����", "[gos][binary_file]")
{
    /// ������ڲ����ļ�����ɾ��
    gos_delete_file(TEST_BINARY_FOLDER_FILE_NAME);

    /// ������д����
    gos::binary_file obj(TEST_BINARY_FOLDER_FILE_NAME);
    unsigned char auc[256] = {0};
    for (unsigned i = 0; i < 256; i++)
    {
        auc[i] = i;
    }

    /// д������
    REQUIRE(256 == obj.write(auc, 256));

    /// ��һ�ζ�ȡ
    unsigned char aucRead[256] = {0};
    REQUIRE(256 == obj.read(aucRead, sizeof(aucRead)));
    REQUIRE(0 == memcmp(auc, aucRead, sizeof(auc)));

    /// �ڶ��ζ�ȡ
    unsigned char aucRead1[256] = {0};
    REQUIRE(256 == obj.read(aucRead1, sizeof(aucRead1)));
    REQUIRE(0 == memcmp(auc, aucRead1, sizeof(auc)));

    /// �ڶ���д��
    REQUIRE(256 == obj.write(auc, 256));
    /// ��ȡ
    unsigned char aucRead2[256 * 2] = {0};
    REQUIRE(256 * 2 == obj.read(aucRead2, sizeof(aucRead2)));
    REQUIRE(0 == memcmp(auc, aucRead2, sizeof(auc)));
    REQUIRE(0 == memcmp(auc, aucRead2 + sizeof(auc), sizeof(auc)));
}

TEST_CASE("binary_file �Ĵ��ļ���ǰ׺��ȡ����", "[gos][binary_file]")
{
    if (gos_file_exist(TEST_BINARY_FOLDER_FILE_NAME))
    {
        gos::binary_file obj(TEST_BINARY_FOLDER_FILE_NAME);
        unsigned char auc[256] = {0};
        for (unsigned i = 0; i < 256; i++)
        {
            auc[i] = i;
        }
        /// ��ȡ
        unsigned char aucRead2[256 * 2] = {0};
        REQUIRE(256 * 2 == obj.read(aucRead2, sizeof(aucRead2)));
        REQUIRE(0 == memcmp(auc, aucRead2, sizeof(auc)));
        REQUIRE(0 == memcmp(auc, aucRead2 + sizeof(auc), sizeof(auc)));
    }
}

#endif
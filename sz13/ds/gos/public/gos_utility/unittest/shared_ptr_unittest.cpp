#include "g_include.h"

#ifdef USE_GOS_UNITTEST

TEST_CASE("shared_ptr µÄ¼òµ¥²âÊÔ", "[gos][shared_ptr]")
{
    SECTION("×î¼ò²âÊÔ")
    {
        gos::memory_check_impl obj;
        obj.start();  ///< ¿ªÊ¼¼ì²âÄÚ´æÐ¹Â©
        {
            gos::shared_ptr_array<int> sp0;
            gos::shared_ptr_array<int> sp1(sp0);
            gos::shared_ptr_array<int> sp2;
            sp2 = sp0;

            gos::shared_ptr_array<std::string> sp3;
            gos::shared_ptr_array<std::string> sp4(sp3);
            gos::shared_ptr_array<std::string> sp5;
            sp5 = sp3;
        }
        bool iRet = obj.finish();  ///< ½áÊø¼ì²âÄÚ´æÐ¹Â©
        REQUIRE(iRet);
    }

    SECTION("¼òµ¥²âÊÔ")
    {
        gos::memory_check_impl obj;
        obj.start();  ///< ¿ªÊ¼¼ì²âÄÚ´æÐ¹Â©

        {
            gos::shared_ptr<int> sp(new int(5));
        }
        bool iRet = obj.finish();  ///< ½áÊø¼ì²âÄÚ´æÐ¹Â©
        REQUIRE(iRet);
    }
}

struct st_test_shared_ptr
{
    st_test_shared_ptr()
    {
        sz = new char[55];
    }

    char* sz;
};

class st_test_shared_ptr_delete
{
public:
    void operator()(st_test_shared_ptr* p)
    {
        delete[] p->sz;
        delete p;
    }
};

TEST_CASE("shared_ptr µÄÎö¹¹Æ÷²âÊÔ", "[gos][shared_ptr]")
{
    gos::memory_check_impl obj;
    obj.start();  ///< ¿ªÊ¼¼ì²âÄÚ´æÐ¹Â©

    {
        gos::shared_ptr<st_test_shared_ptr, st_test_shared_ptr_delete> sp(new st_test_shared_ptr);
    }

    bool iRet = obj.finish();  ///< ½áÊø¼ì²âÄÚ´æÐ¹Â©
    REQUIRE(iRet);
}

TEST_CASE("shared_ptr µÄÊý×éÎö¹¹Æ÷²âÊÔ", "[gos][shared_ptr]")
{
    SECTION("¼òµ¥²âÊÔ")
    {
        gos::memory_check_impl obj;
        obj.start();  ///< ¿ªÊ¼¼ì²âÄÚ´æÐ¹Â©

        {
            gos::shared_ptr<int, gos::array_delete<int> > sp(new int[5]);
        }
        bool iRet = obj.finish();  ///< ½áÊø¼ì²âÄÚ´æÐ¹Â©
        REQUIRE(iRet);
    }

    SECTION("¿½±´²âÊÔ")
    {
        gos::memory_check_impl obj;
        obj.start();  ///< ¿ªÊ¼¼ì²âÄÚ´æÐ¹Â©

        {
            int* p = new int[5];
            gos::shared_ptr<int, gos::array_delete<int> > sp(p);
            gos::shared_ptr<int, gos::array_delete<int> > sp1 = sp;
            gos::shared_ptr<int, gos::array_delete<int> > sp2;
            sp2 = sp;
        }
        bool iRet = obj.finish();  ///< ½áÊø¼ì²âÄÚ´æÐ¹Â©
        REQUIRE(iRet);
    }
}

TEST_CASE("shared_ptr_array µÄ²âÊÔ", "[gos][shared_ptr_array]")
{
    SECTION("×î¼ò²âÊÔ")
    {
        gos::memory_check_impl obj;
        obj.start();  ///< ¿ªÊ¼¼ì²âÄÚ´æÐ¹Â©
        {
            gos::shared_ptr_array<int> sp0;
            gos::shared_ptr_array<int> sp1(sp0);
            gos::shared_ptr_array<int> sp2;
            sp2 = sp0;

            gos::shared_ptr_array<std::string> sp3;
            gos::shared_ptr_array<std::string> sp4(sp3);
            gos::shared_ptr_array<std::string> sp5;
            sp5 = sp3;
        }
        bool iRet = obj.finish();  ///< ½áÊø¼ì²âÄÚ´æÐ¹Â©
        REQUIRE(iRet);
    }

    SECTION("¼òµ¥²âÊÔ")
    {
        gos::memory_check_impl obj;
        obj.start();  ///< ¿ªÊ¼¼ì²âÄÚ´æÐ¹Â©
        {
            gos::shared_ptr_array<int> sp(new int[5]);
        }
        bool iRet = obj.finish();  ///< ½áÊø¼ì²âÄÚ´æÐ¹Â©
        REQUIRE(iRet);
    }

    SECTION("¿½±´²âÊÔ")
    {
        gos::memory_check_impl obj;
        obj.start();  ///< ¿ªÊ¼¼ì²âÄÚ´æÐ¹Â©
        {
            gos::shared_ptr_array<int> sp(new int[5]);
            gos::shared_ptr_array<int> sp1;
            sp1 = sp;
            gos::shared_ptr_array<int> sp2(sp);
        }
        bool iRet = obj.finish();  ///< ½áÊø¼ì²âÄÚ´æÐ¹Â©
        REQUIRE(iRet);
    }

    SECTION("³ÉÔ±º¯Êý²âÊÔ")
    {
        gos::memory_check_impl obj;
        obj.start();  ///< ¿ªÊ¼¼ì²âÄÚ´æÐ¹Â©

        {
            int* p = new int[5];
            gos::iota(p, p + 4, 0);

            gos::shared_ptr_array<int> sp(p);
            gos::shared_ptr_array<int> sp1;
            sp1 = sp;
            gos::shared_ptr_array<int> sp2(sp1);

            REQUIRE(std::equal(sp.get(), sp.get() + 4, sp2.get()));
            REQUIRE(*sp == *sp2);
        }
        bool iRet = obj.finish();  ///< ½áÊø¼ì²âÄÚ´æÐ¹Â©
        REQUIRE(iRet);
    }
}

#endif
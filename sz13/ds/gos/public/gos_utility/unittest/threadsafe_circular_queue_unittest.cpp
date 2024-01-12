#include "g_include.h"

#ifdef USE_GOS_UNITTEST
TEST_CASE("gos::threadsafe_circular_queue �򵥲���", "[gos][threadsafe_circular_queue]")
{
    SECTION("�򵥽ӿڲ��� �������̰߳�ȫ")
    {
        gos::memory_check_impl obj;
        obj.start();
        {
            int temp;
            gos::threadsafe_circular_queue<int> queue(3);
            REQUIRE(!queue.pop(temp));
            queue.push(1);
            queue.push(2);
            queue.push(3);
            REQUIRE(queue.pop(temp));
            REQUIRE(temp == 1);
            queue.push(4);
            queue.push(5);

            REQUIRE(queue.pop(temp));
            REQUIRE(temp == 3);
        }
        bool iRet = obj.finish();
        REQUIRE(iRet);
    }

    SECTION("����������� �������̰߳�ȫ")
    {
        gos::memory_check_impl obj;
        obj.start();
        {
            gos::threadsafe_circular_queue<int> queue(3);
            for (int i = 0; i < 1000; i++)
            {
                queue.push(i);
            }

            int temp;
            REQUIRE(queue.pop(temp));
            REQUIRE(temp == 1000 - 3);
        }
        bool iRet = obj.finish();
        REQUIRE(iRet);
    }
}

#endif
#include "g_include.h"

#ifdef USE_GOS_UNITTEST
TEST_CASE("GJson������ �򵥲��� ", "[gos][GJson]")
{
    SECTION("����json����")
    {
        char acJson[] = "{\"name\": \"BeJson\",\"url\": \"http://www.bejson.com\",\"page\": 88}";
        GJson json;
        REQUIRE(json.Parse(acJson));
        REQUIRE(0 == strcmp(json.GetValue("name"), "BeJson"));
        REQUIRE(0 == strcmp(json.GetValue("url"), "http://www.bejson.com"));
        REQUIRE(0 == strcmp(json.GetValue("page"), "88"));
    }

    SECTION("����json����")
    {
        char acJson[] = "{\"name\": \"BeJson\",\"url\": \"http://www.bejson.com,\",\"page\": 88}";
        GJson json;
        REQUIRE(json.Parse(acJson));
        REQUIRE(0 == strcmp(json.GetValue("name"), "BeJson"));
        REQUIRE(0 == strcmp(json.GetValue("url"), "http://www.bejson.com,"));
        REQUIRE(0 == strcmp(json.GetValue("page"), "88"));
    }

    SECTION("����json����")
    {
        char acJson[] = "{\"name\": \"BeJson\",\"url\": \"http://www.bejson.com,,,,\",\"page\": 88}";
        GJson json;
        REQUIRE(json.Parse(acJson));
        REQUIRE(0 == strcmp(json.GetValue("name"), "BeJson"));
        REQUIRE(0 == strcmp(json.GetValue("url"), "http://www.bejson.com,,,,"));
        REQUIRE(0 == strcmp(json.GetValue("page"), "88"));
    }

    SECTION("����json����")
    {
        char acJson[] = "{\"name\": \"BeJson\",\"page\": 88,\"url\": \"http://www.bejson.com,,,,\"}";
        GJson json;
        REQUIRE(json.Parse(acJson));
        REQUIRE(0 == strcmp(json.GetValue("name"), "BeJson"));
        REQUIRE(0 == strcmp(json.GetValue("url"), "http://www.bejson.com,,,,"));
        REQUIRE(0 == strcmp(json.GetValue("page"), "88"));
    }

    SECTION("�쳣json����,����滨����ǰ���˸�����")
    {
        char acJson[] = "{\"name\": \"BeJson\",\"url\": \"http://www.bejson.com\",\"page\": 88,}";
        GJson json;
        REQUIRE(!json.Parse(acJson));
    }

    SECTION("�쳣json������ value�з��ַ�����ֵ�м���Ӷ���")
    {
        char acJson[] = "{\"name\": \"BeJson\",\"url\": \"http://www.bejson.com\",\"page\": 8,8}";
        GJson json;
        REQUIRE(!json.Parse(acJson));
    }

    SECTION("�쳣json���������ڴ����ŵĽ���")
    {
        GJsonParam param;
        char acToken[] = ",,,";
        param.Add("key", acToken);
        GJson json;
        REQUIRE(json.Parse(param.GetString()));
        REQUIRE(0 == strcmp(acToken, json.GetValue("key")));
    }
}

TEST_CASE("GJson��������", "[gos][GJson]")
{
    SECTION("������������, ������Ƕjson����")
    {
        std::string str =
            "{\"name\": \"BeJson\",\"url\": \"http://www.bejson.com\",\"page\": 88,\"isNonProfit\": true,\"address\": "
            "{\"street\": \"�Ƽ�԰·.\",\"city\": \"��������\",\"country\": \"�й�\"},"
            "\"links\": [{\"name\": \"Google\",\"url\": \"http://www.google.com\"},{\"name\": \"Baidu\",\"url\": "
            "\"http://www.baidu.com\"},{\"name\": \"SoSo\",\"url\": \"http://www.SoSo.com\"}]}";
        GJson Json;
        REQUIRE(Json.Parse(str));

        std::string strAddress = Json.GetValue("address");
        GJson AddressJson;
        REQUIRE(AddressJson.Parse(strAddress));
        REQUIRE(0 == strcmp(AddressJson.GetValue("street"), "�Ƽ�԰·."));
        REQUIRE(0 == strcmp(AddressJson.GetValue("city"), "��������"));
        REQUIRE(0 == strcmp(AddressJson.GetValue("country"), "�й�"));

        std::string strLinks = Json.GetValue("links");
        GJson LinksJson;
        REQUIRE(LinksJson.Parse(strLinks));
        std::vector<GJson*> vecJson = LinksJson.GetSubJson();
        REQUIRE(vecJson.size() == 3);
        for (size_t index = 0; index < vecJson.size(); ++index)
        {
            if (index == 0)
            {
                REQUIRE(0 == strcmp(vecJson.at(index)->GetValue("name"), "Google"));
                REQUIRE(0 == strcmp(vecJson.at(index)->GetValue("url"), "http://www.google.com"));
            }
            else if (index == 1)
            {
                REQUIRE(0 == strcmp(vecJson.at(index)->GetValue("name"), "Baidu"));
                REQUIRE(0 == strcmp(vecJson.at(index)->GetValue("url"), "http://www.baidu.com"));
            }
            else if (index == 2)
            {
                REQUIRE(0 == strcmp(vecJson.at(index)->GetValue("name"), "SoSo"));
                REQUIRE(0 == strcmp(vecJson.at(index)->GetValue("url"), "http://www.SoSo.com"));
            }
        }
    }
}

TEST_CASE("GJson ���������ַ�", "[gos][GJson]")
{
    SECTION("�����ַ�")
    {
        std::string str = "{\"data\":\"��%����&*������+}\��|����������\"}";

        GJson Json;
        REQUIRE(Json.Parse(str));
        std::string strData;
        REQUIRE(Json.GetValue("data", strData));
        REQUIRE(strData == "��%����&*������+}\��|����������");
    }

    SECTION("�޷����������д���ת���ַ� \" ")
    {
        std::string str = "{\"data\":\" \" \"}";

        GJson Json;
        /// �޷����� `"`
        REQUIRE(!Json.Parse(str));
    }

    SECTION("ת���ַ� \\")
    {
        std::string str = "{\"data\":\" \\ \"}";

        GJson Json;
        REQUIRE(Json.Parse(str));
        std::string strData;
        REQUIRE(Json.GetValue("data", strData));

        REQUIRE(strData == " \\ ");
    }
}

TEST_CASE("GJsonTupleParam ���ܲ���", "[gos][GJsonTupleParam]")
{
    SECTION("GJsonTupleParam: 10, GJsonParam: 100")
    {
        gos::memory_check_impl ss;
        ss.start();
        for (unsigned ul = 0; ul < 10; ++ul)
        {
            PROFILER_INFO("GJsonTupleParam ���ܲ��� GJsonTupleParam: 10, GJsonParam: 100");
            GJsonTupleParam JsonTupleParam;
            GJsonParam JsonParam;
            for (unsigned i = 0; i < 100; ++i)
            {
                JsonParam.Add("ne_id", "jlasdf");
                JsonParam.Add("mac", "lsajfda");
                JsonParam.Add("ip", "jlasfj");
                JsonParam.Add("type", 232);
                JsonParam.Add("name", "alsjf");
                JsonParam.Add("alias", "dlafjk");
                JsonParam.Add("sw_ver", "jlfzsjdf");
                JsonParam.Add("hw_ver", "jflajd");
                JsonParam.Add("model", "jflaj");
                JsonParam.Add("online_sec", 66);
                JsonParam.Add("android_ver", "jladj");
                JsonParam.Add("radio_ver", "aljdfkla");
                JsonTupleParam.Add(JsonParam);
            }
        }
        bool iRet = ss.finish();  ///< ����ֱ��ʹ�� REQUIRE() �����ú���������� finish ��������false
        CHECK(!iRet);
    }

    SECTION("GJsonTupleParam: 10, GJsonParam: 1k")
    {
        gos::memory_check_impl ss;
        ss.start();
        for (unsigned ul = 0; ul < 10; ++ul)
        {
            PROFILER_INFO("GJsonTupleParam ���ܲ��� GJsonTupleParam: 10, GJsonParam: 1k");
            GJsonTupleParam JsonTupleParam;
            GJsonParam JsonParam;
            for (unsigned i = 0; i < 1000; ++i)
            {
                JsonParam.Add("ne_id", "jlasdf");
                JsonParam.Add("mac", "lsajfda");
                JsonParam.Add("ip", "jlasfj");
                JsonParam.Add("type", 232);
                JsonParam.Add("name", "alsjf");
                JsonParam.Add("alias", "dlafjk");
                JsonParam.Add("sw_ver", "jlfzsjdf");
                JsonParam.Add("hw_ver", "jflajd");
                JsonParam.Add("model", "jflaj");
                JsonParam.Add("online_sec", 66);
                JsonParam.Add("android_ver", "jladj");
                JsonParam.Add("radio_ver", "aljdfkla");
                JsonTupleParam.Add(JsonParam);
            }
        }
        bool iRet = ss.finish();  ///< ����ֱ��ʹ�� REQUIRE() �����ú���������� finish ��������false
        CHECK(!iRet);
    }

    SECTION("GJsonTupleParam: 10, GJsonParam: 10k")
    {
        gos::memory_check_impl ss;
        ss.start();
        for (unsigned ul = 0; ul < 10; ++ul)
        {
            PROFILER_INFO("GJsonTupleParam ���ܲ��� GJsonTupleParam: 10, GJsonParam: 10k");
            GJsonTupleParam JsonTupleParam;
            GJsonParam JsonParam;
            for (unsigned i = 0; i < 10000; ++i)
            {
                JsonParam.Add("ne_id", "jlasdf");
                JsonParam.Add("mac", "lsajfda");
                JsonParam.Add("ip", "jlasfj");
                JsonParam.Add("type", 232);
                JsonParam.Add("name", "alsjf");
                JsonParam.Add("alias", "dlafjk");
                JsonParam.Add("sw_ver", "jlfzsjdf");
                JsonParam.Add("hw_ver", "jflajd");
                JsonParam.Add("model", "jflaj");
                JsonParam.Add("online_sec", 66);
                JsonParam.Add("android_ver", "jladj");
                JsonParam.Add("radio_ver", "aljdfkla");
                JsonTupleParam.Add(JsonParam);
            }
        }
        bool iRet = ss.finish();  ///< ����ֱ��ʹ�� REQUIRE() �����ú���������� finish ��������false
        CHECK(!iRet);
    }
}

#endif

#include <stdio.h>
#include <stdlib.h>
#include "curl.h"

static size_t WriteData(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written = fwrite(ptr, size, nmemb, stream);

    return written;
}

static BOOL CheckFile(CHAR *szFile)
{
    FILE    *fp = fopen(szFile, "rb");
    CHAR    acText[1024];
    UINT32  ulSize = 0;

    if (!fp)
    {
        return FALSE;
    }

    memset(acText, 0, sizeof(acText));
    ulSize = fread(acText, 1, sizeof(acText)-1, fp);
    fclose(fp);

    if (0 == ulSize)
    {
        return FALSE;
    }

    for (UINT32 i=0; i<ulSize; i++)
    {
        if (!isascii(acText[i]))
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL DownloadHttpFile(char *szUrl, char *szLocalFile, char *szUser, char *szPwd)
{
    CURL        *curl;
    FILE        *fp;
    CURLcode    code;
    CURLINFO    response_code;
    CHAR        errbuf[CURL_ERROR_SIZE];
    BOOL        bRet = FALSE;

    curl = curl_easy_init();
    if (!curl)
    {
        return FALSE;
    }

    fp = fopen(szLocalFile, "wb");
    if (!fp)
    {
        curl_easy_cleanup(curl);
        return FALSE;
    }

    curl_easy_setopt(curl, CURLOPT_URL, szUrl);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);     //可以看到调试信息
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);

    curl_easy_setopt(curl, CURLOPT_USERNAME, szUser);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, szPwd);
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);

    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 5000);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 5000);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 3);

    code = curl_easy_perform(curl);
    if (code != CURLE_OK)
    {
        fclose(fp);
        goto end;
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    fclose(fp);
    bRet = CheckFile(szLocalFile);

end:
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return bRet;
}

#if 0
int test_http_download()
{
    char *szUrl = "https://172.22.1.21:8018/mrs_web/downloadFileFromMrs?filetype=1&amp;sessionId=1577762214745402-39725-648-0-0-5071-7008&amp;fileId=268435457";
    char *szLocalFile = "d:\\test1.dat";
    char *szUserPwd = "1001:1001";

//    http://172.22.1.21:8000/ubp/rec/2020/06/17/09/5071_7007_20200617092309/5071_7007_20200617092310_544931.mp4
    //rtsp://172.22.1.21:8554/1_1592356989890896-39725-181-0-2-5071-7007_301989889

//  szUrl = "http://172.22.1.21:8000/ubp/rec/2019/12/19/13/5071_1001_20191219135808/5071_1001_20191219135808_692997.amr";
    szUrl = "https://172.22.1.21:8018/mrs_web/downloadFileFromMrs?filetype=1&sessionId=1577762214745402-39725-648-0-0-5071-7008&fileId=268435457";
    BOOL bRet = DownloadHttpFile(szUrl, szLocalFile, szUserPwd);

    return 0;

}
#endif

#include "gos_api.h"
#include "gos_xml.h"

void test_xml()
{
    BOOL bRet = 1;
    char *szFile = "d:\\kpi_163.xml";

    UINT32  ulTime = gos_get_uptime_1ms();

    GOS_XML_DOCUMENT_T* pDoc = gos_xml_create_document();

    ulTime = gos_get_uptime_1ms();

    bRet = gos_xml_parse_file(pDoc, szFile, FALSE);

    ulTime = gos_get_uptime_1ms() - ulTime;

    printf("%u", ulTime);

    gos_xml_destroy_document(pDoc);

    getchar();
    exit(1);
}

int main()
{
    test_xml();

    return 0;
}

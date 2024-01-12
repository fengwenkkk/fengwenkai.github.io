#include "vcl.h"
#include "g_include.h"
#include "CSVFile.h"

CSVFile::CSVFile(CHAR *szFile)
{
    m_szText = strdup(szFile);
}

CSVFile::~CSVFile()
{
    GOS_FREE(m_szText);
}

BOOL CSVFile::Parse(char *szText)
{
    m_ulFieldNum = 0;
    m_bParsed = FALSE;

    return TRUE;
}

//---------------------------------------------------------------------------

#ifndef CSVFileH
#define CSVFileH

typedef struct
{
    VECTOR<char*>   vValue;
}_CSV_RECORD_T;

class CSVFile
{
public:
    CSVFile(CHAR *File);
    ~CSVFile();

private:
    char    *m_szText;
    BOOL    m_bParsed;
    UINT32  m_ulFieldNum;

    BOOL    Parse(char *szText);
    VECTOR<_CSV_RECORD_T>   m_vRecord;
};

//---------------------------------------------------------------------------
#endif

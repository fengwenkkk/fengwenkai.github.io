#ifndef G_STRING_H
#define G_STRING_H

class GString
{
public:
    GString();

    GString(UINT32 x);
    GString(INT32 x);
    GString(INT64 x);
    GString(const CHAR *x);

    ~GString();

    /**
     * @brief           ¿½±´¹¹Ôìº¯Êý
     * @param           other    [in]
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @note
     */
    GString(const GString& other);

    /**
     * @brief           ¿½±´¸³Öµ·û
     * @param           other    [in]
     * @return          GString&
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @note
     */
    GString& operator=(const GString& other);

    CHAR*   GetString() const;
    CHAR*   CloneString();
    UINT32  Length();
    BOOL    SetLength(UINT32 ulNewLength);

    VOID    Clear();
    BOOL    Append(CHAR *szString);
    BOOL    Append(const CHAR *szString);
    BOOL    Append(CHAR *szString, UINT32 ulLen, BOOL bNewLine);

    INT32   cat_sprintf(const CHAR *szFormat, ...);
    INT32   sprintf(const CHAR *szFormat, ...);

    operator const CHAR* ();
    operator CHAR* ();

private:
    CHAR    *m_szString;
    UINT32  m_ulMaxLen;
    UINT32  m_ulCurrLen;

    VOID Init();

};

#endif

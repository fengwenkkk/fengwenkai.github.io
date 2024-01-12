#ifndef _G_CONF_FILE_H
#define _G_CONF_FILE_H

class GConfFile
{
public:
    GConfFile(CHAR *szFileName);
    GConfFile(const std::string &strFileName);
    ~GConfFile();

    BOOL    Save(CHAR *szFile=NULL);
    BOOL    Set(const CHAR *szGroupKey, const CHAR *szKey, UINT32 ulValue);
    BOOL    Set(const CHAR *szGroupKey, const CHAR *szKey, CHAR *szValue);
    BOOL    Set(const CHAR *szKey, CHAR *szValue);
    BOOL    Set(const CHAR *szKey, UINT32 ulValue);
    BOOL    Set(const CHAR *szKey, INT32 iValue);
    BOOL    SetIP(const CHAR *szKey, UINT8 *pucValue);

    CHAR*   Get(const CHAR *szGroupKey, const CHAR *szKey);
    BOOL    Get(const CHAR *szGroupKey, const CHAR *szKey, CHAR *szRetValue, UINT32 ulMaxLen);
    BOOL    Get(const CHAR *szGroupKey, const CHAR *szKey, std::string &strValue);
    BOOL    Get(const CHAR *szGroupKey, const CHAR *szKey, INT32 *piValue);
    BOOL    Get(const CHAR *szGroupKey, const CHAR *szKey, INT32 &iValue);
    BOOL    Get(const CHAR *szGroupKey, const CHAR *szKey, INT32 iDefault, INT32 *piValue);
    BOOL    Get(const CHAR *szGroupKey, const CHAR *szKey, INT32 iDefaultValue, INT32 &iValue);
    BOOL    Get(const CHAR *szGroupKey, const CHAR *szKey, UINT32 *pulValue);
    BOOL    Get(const CHAR *szGroupKey, const CHAR *szKey, UINT32 &ulValue);
    BOOL    Get(const CHAR *szGroupKey, const CHAR *szKey, UINT16& usValue);
    BOOL    Get(const CHAR *szGroupKey, const CHAR *szKey, UINT16 *pusValue);
    BOOL    Get(const CHAR *szGroupKey, const CHAR *szKey, UINT32 ulDefault, UINT32 *pulValue);
    BOOL    Get(const CHAR *szGroupKey, const CHAR *szKey, UINT16 usDefault, UINT16 *pusValue);
    BOOL    Get(const CHAR *szGroupKey, const CHAR *szKey, bool &bValue);
    BOOL    Get(const CHAR *szGroupKey, const CHAR *szKey, bool bDefault, bool &bValue);

    CHAR*   Get(const CHAR *szKey) const;
    BOOL    Get(const CHAR *szKey, CHAR *szRetValue, UINT32 ulMaxLen);
    BOOL    Get(const CHAR *szKey, INT32 *piValue);
    BOOL    Get(const CHAR *szKey, INT32 iDefault, INT32 *piValue);
    BOOL    Get(const CHAR *szKey, UINT32 *pulValue);
    BOOL    Get(const CHAR *szKey, UINT32 ulDefault, UINT32 *pulValue);
    BOOL    Get(const CHAR *szKey, UINT16 *pusValue);
    BOOL    Get(const CHAR *szKey, UINT16 usDefault, UINT16 *pusValue);
    BOOL    GetIP(const CHAR *szKey, UINT8 *pucValue);
    BOOL    GetBool(const CHAR *szKey, BOOL bDefault, BOOL *pbValue);
    BOOL    GetBool(const CHAR *szKey, BOOL *pbValue);
    BOOL    GetBool(const CHAR *szGroupKey, const CHAR *szKey, BOOL &bValue);
    BOOL    GetBool(const CHAR *szGroupKey, const CHAR *szKey, BOOL bDefault, BOOL &bValue);

    VOID    GetKeyList(VECTOR<CHAR*> &vKey) const;
    VOID    GetGroupKeyList(VECTOR<CHAR*> &vKey, CHAR *szGroupKey);
    UINT32  GetGroupKeyID(const CHAR *szGroupKey);
    BOOL    HasGroupKey(const CHAR *szGroupKey);
    VOID    GetGroupIDList(VECTOR<UINT32> &vGroupIDList);
    VOID    GetGroupKeyList(VECTOR<CHAR*> &vKey, UINT32 ulGroupID);
    BOOL    GetGroupName(UINT32 ulGroupID, CHAR *acName, UINT32 ulMaxLen);
    BOOL    SaveGroup(CHAR *szFile = NULL);

    BOOL    Del(const CHAR *szGroupKey, const CHAR *szKey);
    BOOL    DelGroup(const CHAR *szGroupName);
    BOOL    SaveGroup(GString &strData);

    const CHAR* GetString() const;

private:
    CHAR                m_acFile[512];
    HANDLE              m_hValues;
    VECTOR<UINT32>      m_vGroupIDList;
    CHAR                *m_szConf;
    CHAR                *m_szOriText;

    /// 拷贝构造函数不实现，避免拷贝操作
    GConfFile(const GConfFile &obj);
    /// 拷贝赋值符不实现，避免拷贝操作
    GConfFile &operator=(const GConfFile &obj);

    void    Init(CHAR *szFileName);
    VOID    ParseConf(CHAR *szConf);
    BOOL    _Set(const CHAR *szKey, CHAR *szValue);
};

#endif

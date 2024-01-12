#include "g_include.h"


#ifndef WIN32
#define O_BINARY        0
#endif

//========================================================
typedef struct
{
    char    *szKey;
    char    *szValue;
}CFG_KEY_VALUE_T;


static char* trim_left_str(char *szText)
{
    if (!szText)
    {
        return NULL;
    }

    while(*szText != '\0')
    {
        if (gos_isspace(*szText))
        {
            szText++;
            continue;
        }

        break;
    }

    return szText;
}

static void trim_right_str(char *szText)
{
    char    *szEnd;

    if (!szText)
    {
        return;
    }

    for (szEnd=szText+strlen(szText)-1; szEnd>=szText; szEnd--)
    {
        if (!gos_isspace(*szEnd))
        {
            break;
        }

        *szEnd = '\0';
    }
}

static char* trim_str(char *szText)
{
    szText = trim_left_str(szText);
    trim_right_str(szText);

    return szText;
}

static char* get_value(char *szLine, char *szKey, int iKeyLen)
{
    if (!szLine)
    {
        return NULL;
    }

    szLine = trim_left_str(szLine);
    if (strncmp(szLine, szKey, iKeyLen) != 0)
    {
        return NULL;
    }

    szLine = trim_left_str(szLine+iKeyLen);
    if (*szLine != '=')
    {
        return NULL;
    }

    szLine = trim_str(szLine+1);

    return szLine;
}

static int read_lock_wait(int fd)
{
#ifdef WIN32
    return 0;
#else
    struct flock lock;

    memset(&lock, 0, sizeof(lock));

    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
//  lock.l_pid = getpid();

    return fcntl(fd, F_SETLKW, &lock);
#endif
}

static int write_lock_wait(int fd)
{
#ifdef WIN32
    return 0;
#else
    struct flock lock;

    memset(&lock,  0,  sizeof(lock));

//  return lockf(fd, F_LOCK, 0);

    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
//  lock.l_pid = getpid();

    /*if (fcntl(fd, F_GETFL, 0) < 0)
    {
        return -1;
    }*/

    return fcntl(fd, F_SETLKW, &lock);
#endif
}

static int file_unlock(int fd)
{
#ifdef WIN32
    return 0;
#else
    struct flock lock;

    memset(&lock,  0,  sizeof(lock));

//  return lockf(fd, F_ULOCK, 0);

    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;

    //lock.l_pid = getpid();

    return fcntl(fd, F_SETLKW, &lock);
#endif
}

static int get_line_num(char *szText)
{
    char    *szTmp;
    int     iNum = 0;

    while(*szText)
    {
        szTmp = strchr(szText, '\n');
        if (!szTmp)
        {
            break;
        }

        iNum++;
        szText = szTmp+1;
    }

    return iNum+1;
}

static int vram_save(int fd, CFG_KEY_VALUE_T *pstCfg, unsigned int ulCfgNum)
{
    unsigned int    i;
    int             iLen = 0;
    int             iTotalLen = 0;

#ifndef WIN32
    ftruncate(fd, 0);
#else
    iTotalLen = lseek(fd, 0, SEEK_END);
#endif

    lseek(fd, 0, SEEK_SET);
    for (i=0; i<ulCfgNum; i++)
    {
        iLen += write(fd, pstCfg[i].szKey, strlen(pstCfg[i].szKey));
        iLen += write(fd, "=", 1);

        iLen += write(fd, pstCfg[i].szValue, strlen(pstCfg[i].szValue));
        iLen += write(fd, "\n", 1);
    }

#ifdef WIN32
    if (iLen > 0)
    {
        for (i=iLen; i<iTotalLen; i++)
        {
            write(fd, " ", 1);
        }
    }
#endif

    return 0;
}

static int parse_value(char *szLine, CFG_KEY_VALUE_T* pstCfg)
{
    char    *szTmp = strchr(szLine, '=');

    if (!szTmp)
    {
        return -1;
    }

    *szTmp = '\0';

    pstCfg->szKey = trim_str(szLine);
    pstCfg->szValue = szTmp+1;//trim_str(szTmp+1);

    return 0;
}

int vram_get(char *szCfgFile, CHAR *szKey, CHAR *szValue, UINT32 ulMaxLen)
{
    int     fd = -1;
    int     iSize;
    char    *szBuf = NULL;
    char    *szLine;
    char    *szTmp;
    char    *szTmpValue;
    int     iKeyLen;
    int     iValueLen;
    int     iRet = -1;

    if (!gos_file_exist(szCfgFile))
    {
        return -1;
    }

    if (!szKey || !szValue ||
        ulMaxLen <= 1)
    {
        goto end;
    }

    memset(szValue, 0, ulMaxLen);

    iKeyLen = strlen(szKey);
    if (iKeyLen == 0)
    {
        goto end;
    }

    //fd = open((char*)g_szCfgFile, O_RDONLY);
    fd = open(szCfgFile, O_BINARY|O_RDWR, "0644");
    if (fd < 0)
    {
        goto end;
    }

    if (write_lock_wait(fd) < 0)
    {
        printf("%s == read_lock fail\n", gos_get_text_time(NULL));

        close(fd);
        fd = -1;
        goto end;
    }

    iSize = lseek(fd, 0, SEEK_END);
    if (iSize < 0)
    {
        goto end;
    }

    lseek(fd, 0, SEEK_SET);

    szBuf = (char*)calloc(1, iSize+2);
    if (!szBuf)
    {
        goto end;
    }

    if ((iSize = read(fd, szBuf, iSize)) < 0)
    {
        goto end;
    }

    szBuf[iSize] = '\n';
    szBuf[iSize+1] = '\0';
    szLine = szBuf;

    while(1)
    {
        szTmp = strchr(szLine, '\n');
        if (!szTmp)
        {
            break;
        }

        *szTmp = '\0';

        szTmpValue = get_value(szLine, (char*)szKey, iKeyLen);
        if (szTmpValue)
        {
            iValueLen = strlen(szTmpValue);
            if (iValueLen >= ulMaxLen)
            {
                goto end;
            }

            strcpy(szValue, szTmpValue);

            iRet = 0;
            break;
        }

        szLine = szTmp+1;
        if (*szLine == '\0')
        {
            break;
        }
    }

end:
    if (fd >= 0)
    {
        file_unlock(fd);
        close(fd);
    }

    if (szBuf)
    {
        free(szBuf);
    }

    return iRet;
}

int vram_quick_save(int fd, int iPos, char *szValue, int iValueLen)
{
    int iSize = lseek(fd, iPos, SEEK_SET);

    if (iSize < 0)
    {
        return -1;
    }

    write(fd, szValue, iValueLen);

    return 0;
}

// 获取去除了\r \n的字符串长度
int vram_strlen(char *s)
{
    int i = 0;

    for(;s[i];i++)
    {
        if (s[i] == '\n')
        {
            break;
        }
    }

#ifdef WIN32
    if (i > 0)
    {
        if (s[i-1] == '\r')
        {
            i--;
        }
    }
#endif

    return i;
}

/**
 * @brief           将文件szCfgFile切割，使用CFG_KEY_VALUE_T保存每一行的key-value
 *                  循环比对
 * @param           szCfgFile    [in/out]
 * @param           szKey    [in/out]
 * @param           szValue    [in/out]
 * @return          int
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *
 * @endcode
 * @note
 */
int vram_set(char *szCfgFile, char *szKey, char *szValue)
{
    int     fd = 0;
    int     iSize;
    char    *szBuf = NULL;
    char    *szLine;
    char    *szTmp;
    int     iKeyLen;
    int     iRet = -1;
    int     iExist = 0;
    int     iPos;
    int     iOldValueLen;
    int     iNewValueLen;
    char    *szNewValue = NULL;
    unsigned int ulCfgNum = 0;
    CFG_KEY_VALUE_T  *pstCfg = NULL;

    if (!gos_file_exist(szCfgFile))
    {
        return -1;
    }

    if (!szKey || !szValue)
    {
        return -1;
    }

    iKeyLen = strlen(szKey);
    if (iKeyLen == 0)
    {
        return -1;
    }

    fd = open((char*)szCfgFile, O_BINARY|O_RDWR|O_CREAT, "0644");
    if (fd < 0)
    {
        goto end;
    }

    if (write_lock_wait(fd) < 0)
    {
        printf("%s == write_lock fail\n", gos_get_text_time(NULL));
        //close(fd);
        //fd = -1;
        goto end;
    }

    iSize = lseek(fd, 0, SEEK_END);
    if (iSize < 0)
    {
        goto end;
    }

    lseek(fd, 0, SEEK_SET);

    szBuf = (char*)calloc(1, iSize+2);
    if (!szBuf)
    {
        goto end;
    }

    if ((iSize = read(fd, szBuf, iSize)) < 0)
    {
        goto end;
    }

    szBuf[iSize] = '\n';

    //获取文件行数
    ulCfgNum = get_line_num(szBuf);
    pstCfg = (CFG_KEY_VALUE_T*)calloc(sizeof(CFG_KEY_VALUE_T), ulCfgNum);
    if (!pstCfg)
    {
        goto end;
    }

    ulCfgNum = 0;
    szLine = szBuf;
    while(1)
    {
        szTmp = strchr(szLine, '\n');
        if (!szTmp)
        {
            break;
        }

        *szTmp = '\0';
        szLine = trim_left_str(szLine); //切出一行
        if (*szLine != '\0')
        {
            if (parse_value(szLine, pstCfg+ulCfgNum) == 0)
            {
                //进入一次后不会再次进入
                if (iExist == 0 &&
                    strcmp(szKey, pstCfg[ulCfgNum].szKey) == 0)
                {
                    iExist = 1;

                    iOldValueLen = vram_strlen(pstCfg[ulCfgNum].szValue);
                    iNewValueLen = vram_strlen(szValue);

                    if (iOldValueLen >= iNewValueLen)
                    {
                        iPos = (int)(pstCfg[ulCfgNum].szValue - szBuf);

                        memcpy(pstCfg[ulCfgNum].szValue, szValue, iNewValueLen);
                        if (iOldValueLen > iNewValueLen)
                        {
                            memset(pstCfg[ulCfgNum].szValue+iNewValueLen, ' ', iOldValueLen-iNewValueLen);
                        }

                        iRet = vram_quick_save(fd, iPos, pstCfg[ulCfgNum].szValue, iOldValueLen);
                        goto end;
                    }

                    szNewValue = (char*)calloc(1, iNewValueLen+12);
                    if (!szNewValue)
                    {
                        goto end;
                    }

                    memcpy(szNewValue, szValue, iNewValueLen);
                    memset(szNewValue+iNewValueLen, ' ', 10);
                    szNewValue[iNewValueLen+10] = '\r';
                    pstCfg[ulCfgNum].szValue = szNewValue;

                }

                ulCfgNum++;
            }
        }

        szLine = szTmp+1;
        if (*szLine == '\0')
        {
            break;
        }
    }

    //文件中没有关键字为szKey的行
    if (iExist == 0)
    {
        pstCfg[ulCfgNum].szKey = (char*)szKey;
        pstCfg[ulCfgNum].szValue = szValue;
        ulCfgNum++;
    }

    if (vram_save(fd, pstCfg, ulCfgNum) == 0)
    {
        iRet = 0;
    }

    GOS_FREE(szNewValue);

end:
    if (fd >= 0)
    {
        file_unlock(fd);
        close(fd);
    }

    if (szBuf)
    {
        free(szBuf);
    }

    if (pstCfg)
    {
        free(pstCfg);
    }

    return iRet;
}

int vram_del(char *szCfgFile, const char *szKey)
{
    int     fd = 0;
    int     iSize;
    char    *szBuf = NULL;
    char    *szLine;
    char    *szTmp;
    int     iKeyLen;
    int     iRet = -1;
    int     iExist = 0;
    unsigned int ulCfgNum = 0;
    CFG_KEY_VALUE_T *pstCfg = NULL;

    if (!gos_file_exist(szCfgFile))
    {
        return -1;
    }

    if (!szKey)
    {
        return -1;
    }

    iKeyLen = strlen(szKey);
    if (iKeyLen == 0)
    {
        return -1;
    }

    fd = open((char*)szCfgFile, O_BINARY|O_RDWR|O_CREAT, "0644");
    if (fd < 0)
    {
        goto end;
    }

    if (write_lock_wait(fd) < 0)
    {
        printf("%s == write_lock fail\n", gos_get_text_time(NULL));
        close(fd);
        fd = -1;
        goto end;
    }

    iSize = lseek(fd, 0, SEEK_END);
    if (iSize < 0)
    {
        goto end;
    }

    lseek(fd, 0, SEEK_SET);

    szBuf = (char*)calloc(1, iSize+2);
    if (!szBuf)
    {
        goto end;
    }

    if ((iSize = read(fd, szBuf, iSize)) < 0)
    {
        goto end;
    }

    szBuf[iSize] = '\n';

    ulCfgNum = get_line_num(szBuf);
    pstCfg = (CFG_KEY_VALUE_T*)calloc(sizeof(CFG_KEY_VALUE_T), ulCfgNum);
    if (!pstCfg)
    {
        goto end;
    }

    ulCfgNum = 0;
    szLine = szBuf;
    while(1)
    {
        szTmp = strchr(szLine, '\n');
        if (!szTmp)
        {
            break;
        }

        *szTmp = '\0';
        szLine = trim_str(szLine);
        if (*szLine != '\0')
        {
            if (parse_value(szLine, pstCfg+ulCfgNum) == 0)
            {
                if (strcmp(szKey, pstCfg[ulCfgNum].szKey) != 0)
                {
                    ulCfgNum++;
                }
                else
                {
                    iExist = 1;
                }
            }
        }

        szLine = szTmp+1;
        if (*szLine == '\0')
        {
            break;
        }
    }

    if (iExist)
    {
        if (vram_save(fd, pstCfg, ulCfgNum) == 0)
        {
            iRet = 0;
        }
    }

end:
    if (fd >= 0)
    {
        file_unlock(fd);
        close(fd);
    }

    if (szBuf)
    {
        free(szBuf);
    }

    return iRet;
}

int vram_commit()
{
    return 0;
}

void test_vram()
{
#ifdef WIN32
    char       *szCfgFile = (char*)"c:\\ram_cfg.ini";
#else
    char       *szCfgFile = (char*)"/tmp/ram_cfg.ini";
#endif

    char *szValue = (char*)"value 12345";
    char    acValue[32];
    UINT32  i;
    UINT32  ulStartTime = gos_get_uptime_1ms();

    for (i=0; i<1000; i++)
    {
        vram_get(szCfgFile, "key", acValue, sizeof(acValue));
    }

    printf("get time = %u\n", gos_get_uptime_1ms() - ulStartTime);

    ulStartTime = gos_get_uptime_1ms();

    for (i=0; i<1000; i++)
    {
        vram_set(szCfgFile, "key", "12345");
    }

    printf("set time = %u\n", gos_get_uptime_1ms() - ulStartTime);

    getchar();
    exit(0);
}

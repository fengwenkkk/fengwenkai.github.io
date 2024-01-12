#include "g_include.h"

BOOL ZipFile(CHAR *szRootDir, VECTOR<std::string> &vFile, CHAR *szZipFile)
{
    UINT32      ulTotalLen = 0;
    INT32       iFileSize;
    FILE        *fp = NULL;
    CHAR        *szBuf = NULL;
    CHAR        *szOut = NULL;
    UINT16      usFileNameLen;
    UINT32      ulOffset = 0;
    BOOL        bRet = FALSE;
    HANDLE      hZip = NULL;
    UINT32      ulOutLen = 0;
    UINT32      ulRootDirLen = 0;
    std::string strFile;
    CHAR        acRootDir[512];
    CHAR        *szFile;
    CHAR        *szSimpleFile;

    if (!szRootDir)
    {
        return FALSE;
    }

    ulRootDirLen = sprintf(acRootDir, szRootDir);
    if (ulRootDirLen <= 1)
    {
        return FALSE;
    }

    if (acRootDir[ulRootDirLen-1] == '\\' || acRootDir[ulRootDirLen-1] == '/')
    {
        acRootDir[ulRootDirLen-1] = '\0';
    }
    else
    {
        ulRootDirLen ++;  // 包含结束符
    }

    for (UINT32 i=0; i<vFile.size(); i++)
    {
        szFile = (CHAR*)vFile[i].c_str();
        szSimpleFile = szFile + ulRootDirLen;
        iFileSize = gos_get_file_size(szFile);
        if (iFileSize <= 0)
        {
            continue;
        }

        ulTotalLen += sizeof(UINT16) + strlen(szSimpleFile) + 1 + sizeof(UINT32) + iFileSize;
    }

    if (ulTotalLen == 0)
    {
        return FALSE;
    }

//  ulTotalLen += sizeof(UINT16)+ulRootDirLen;
    szBuf = (CHAR*)calloc(1, ulTotalLen*2+4096);
    if (!szBuf)
    {
        return FALSE;
    }

    szOut = szBuf+ulTotalLen;

    ulOffset = 0;
    for (UINT32 i=0; i<vFile.size(); i++)
    {
        if (fp)
        {
            fclose(fp);
        }

        //sprintf(acFile, "%s/%s", acRootDir, vFile[i].c_str());
        szFile = (CHAR*)vFile[i].c_str();
        szSimpleFile = szFile + ulRootDirLen;
        iFileSize = gos_get_file_size(szFile);
        if (iFileSize < 0)
        {
            goto end;
        }

        if (iFileSize == 0)
        {
            continue;
        }

        fp = fopen(szFile, "rb");
        if (!fp)
        {
            goto end;
        }

        // 文件名长度
        usFileNameLen = strlen(szSimpleFile)+1;
        SET_SHORT(szBuf+ulOffset, htons(usFileNameLen));
        ulOffset += sizeof(UINT16);

        // 文件名
        memcpy(szBuf+ulOffset, szSimpleFile, usFileNameLen);
        ulOffset += usFileNameLen;

        // 文件长度
        SET_INT(szBuf+ulOffset, htonl(iFileSize));
        ulOffset += sizeof(UINT32);

        // 文件
        if (iFileSize != fread(szBuf+ulOffset, 1, iFileSize, fp))
        {
            goto end;
        }

        ulOffset += iFileSize;
    }

    if (ulOffset != ulTotalLen)
    {
        goto end;
    }

    hZip = gos_comp_init();
    gos_comp(hZip, szBuf, ulTotalLen, szOut, &ulOutLen);
    if (ulOutLen == 0)
    {
        goto end;
    }
//  ulOutLen = ulTotalLen;
//  szOut = szBuf;

    if (fp)
    {
        fclose(fp);
    }

    fp = fopen(szZipFile, "wb");
    if (!fp)
    {
        goto end;
    }

    ulTotalLen = htonl(ulTotalLen);
    if (sizeof(UINT32) != fwrite(&ulTotalLen, 1, sizeof(UINT32), fp))
    {
        goto end;
    }

    if (ulOutLen != fwrite(szOut, 1, ulOutLen, fp))
    {
        goto end;
    }

    bRet = TRUE;

end:
    if (fp)
    {
        fclose(fp);
    }

    gos_comp_free(hZip);
    GOS_FREE(szBuf);

    return bRet;
}

BOOL GetFileList(CHAR *szRootDir, VECTOR<std::string> &vFilePostfix, VECTOR<std::string> &vFile, BOOL bClear)
{
    HANDLE      hDir = gos_open_dir(szRootDir);
    BOOL        bRet = FALSE;
    CHAR        acFile[512];
    BOOL        bIsDir;
    CHAR        *szFilePostfix;
    UINT32      i;

    if (bClear)
    {
        vFile.clear();
    }

    while(gos_get_next_file(hDir, acFile, &bIsDir))
    {
        if (bIsDir)
        {
            if (!GetFileList(acFile, vFilePostfix, vFile, FALSE))
            {
                goto end;
            }
        }
        else
        {
            szFilePostfix = gos_get_file_postfix(acFile);
            for (i=0; i<vFilePostfix.size(); i++)
            {
                if (strcasecmp(vFilePostfix[i].c_str(), szFilePostfix) == 0)
                {
                    vFile.push_back(acFile);
                }
            }
        }
    }

    bRet = TRUE;

end:
    gos_close_dir(hDir);

    return bRet;
}

BOOL GetFileList(CHAR *szRootDir, VECTOR<std::string> &vSubDir, VECTOR<std::string> &vFilePostfix, VECTOR<std::string> &vFile)
{
    CHAR    acRootDir[512];

    vFile.clear();

    for (UINT32 i=0; i<vSubDir.size(); i++)
    {
        sprintf(acRootDir, "%s/%s", szRootDir, vSubDir[i].c_str());
        if (!GetFileList(acRootDir, vFilePostfix, vFile, FALSE))
        {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL ZipDir(CHAR *szRootDir, VECTOR<std::string> &vSubDir, VECTOR<std::string> &vFilePostfix, CHAR *szZipFile)
{
    VECTOR<std::string>     vFile;

    if (!GetFileList(szRootDir, vSubDir, vFilePostfix, vFile))
    {
        return FALSE;
    }

    return ZipFile(szRootDir, vFile, szZipFile);
}

BOOL UnzipDir(CHAR *szRootDir, CHAR *szZipFile)
{
    FILE    *fp = NULL;
    INT32   iFileSize = gos_get_file_size(szZipFile);
    CHAR    *szBuf = NULL;
    CHAR    *szOutBuf = NULL;
    UINT32  ulOutLen;
    UINT32  ulUnzipOutLen;
    BOOL    bRet = FALSE;
    UINT32  ulOffset;
    UINT16  *pusFileNameLen;
    UINT32  *pulFileLen;
    CHAR    *szFileName;
    CHAR    *szFileInfo;
    CHAR    acFile[512];
    CHAR    acDir[512];
    CHAR    acRootDir[256];
    UINT32  ulRootDirLen;
    HANDLE  hZip = NULL;

    if (!szRootDir)
    {
        return FALSE;
    }

    ulRootDirLen = sprintf(acRootDir, szRootDir);
    if (ulRootDirLen <= 1)
    {
        return FALSE;
    }

    if (acRootDir[ulRootDirLen-1] == '\\' || acRootDir[ulRootDirLen-1] == '/')
    {
        acRootDir[ulRootDirLen-1] = '\0';
    }

    if (iFileSize <= 0)
    {
        return FALSE;
    }

    szBuf = (CHAR*)calloc(1, iFileSize);
    fp = fopen(szZipFile, "rb");
    if (!fp)
    {
        goto end;
    }

    if (iFileSize != fread(szBuf, 1, iFileSize, fp))
    {
        goto end;
    }

    ulOutLen = GET_INT(szBuf);
    ulOutLen = ntohl(ulOutLen);

    hZip = gos_comp_init();
    szOutBuf = (CHAR*)calloc(1, ulOutLen);
    if (!szOutBuf)
    {
        goto end;
    }

    ulUnzipOutLen = gos_get_decomp_size(szBuf+sizeof(UINT32));
    gos_decomp(hZip, szBuf+sizeof(UINT32), szOutBuf, &ulUnzipOutLen);

    if (ulUnzipOutLen != ulOutLen)
    {
        goto end;
    }
//  memcpy(szOutBuf, szBuf+4, ulOutLen);

    ulOffset = 0;

    while(ulOffset < ulOutLen)
    {
        pusFileNameLen = (UINT16*)(szOutBuf+ulOffset);
        NTOHS_IT(pusFileNameLen[0]);
        ulOffset += sizeof(UINT16);

        szFileName = szOutBuf + ulOffset;
        ulOffset += pusFileNameLen[0];

        pulFileLen = (UINT32*)(szOutBuf+ulOffset);
        NTOHL_IT(pulFileLen[0]);
        ulOffset += sizeof(UINT32);

        szFileInfo = szOutBuf + ulOffset;
        ulOffset += pulFileLen[0];

        sprintf(acFile, "%s/%s", acRootDir, szFileName);
        gos_format_path(acFile);
        gos_get_file_path(acFile, acDir);
        if (!gos_file_exist(acDir))
        {
            gos_create_redir(acDir);
        }

        if (fp)
        {
            fclose(fp);
        }

        fp = fopen(acFile, "wb");
        if (!fp)
        {
            goto end;
        }

        if (pulFileLen[0] != fwrite(szFileInfo, 1, pulFileLen[0], fp))
        {
            goto end;
        }
    }

    bRet = TRUE;

end:
    if (fp)
    {
        fclose(fp);
    }

    GOS_FREE(szBuf);
    GOS_FREE(szOutBuf);

    gos_comp_free(hZip);

    return bRet;
}

BOOL ZipFile(CHAR *szFile, CHAR *szZipFile)
{
    UINT32      ulTotalLen = 0;
    INT32       iFileSize;
    FILE        *fp = NULL;
    CHAR        *szBuf = NULL;
    CHAR        *szOut = NULL;
    UINT16      usFileNameLen;
    UINT32      ulOffset = 0;
    BOOL        bRet = FALSE;
    HANDLE      hZip = NULL;
    UINT32      ulOutLen = 0;
    //std::string   strFile;

    if (!szFile)
    {
        return FALSE;
    }

    iFileSize = gos_get_file_size(szFile);
    if (iFileSize <= 0)
    {
        return FALSE;
    }

    fp = fopen(szFile, "rb");
    if (!fp)
    {
        return FALSE;
    }

    szBuf = (CHAR*)calloc(1, iFileSize*2+4096);
    if (!szBuf)
    {
        goto end;
    }

    szOut = szBuf+iFileSize;

    // 文件
    if (iFileSize != fread(szBuf, 1, iFileSize, fp))
    {
        goto end;
    }

    hZip = gos_comp_init();
    gos_comp(hZip, szBuf, iFileSize, szOut, &ulOutLen);
    if (ulOutLen == 0)
    {
        goto end;
    }

    fclose(fp);
    fp = fopen(szZipFile, "wb");
    if (!fp)
    {
        goto end;
    }

    if (ulOutLen != fwrite(szOut, 1, ulOutLen, fp))
    {
        goto end;
    }

    bRet = TRUE;

end:
    if (fp)
    {
        fclose(fp);
    }

    gos_comp_free(hZip);
    GOS_FREE(szBuf);

    return bRet;
}

BOOL UnzipFile(CHAR *szZipFile, CHAR *szFile)
{
    FILE    *fp = NULL;
    INT32   iFileSize = gos_get_file_size(szZipFile);
    CHAR    *szBuf = NULL;
    CHAR    *szOutBuf = NULL;
    UINT32  ulOutLen;
    UINT32  ulUnzipOutLen;
    BOOL    bRet = FALSE;
//  UINT32  ulOffset;
//  UINT16  *pusFileNameLen;
//  UINT32  *pulFileLen;
//  CHAR    *szFileName;
//  CHAR    *szFileInfo;
//  CHAR    acFile[512];
    CHAR    acDir[512];
//  CHAR    acRootDir[256];
//  UINT32  ulRootDirLen;
    HANDLE  hZip = NULL;

    if (iFileSize <= 0)
    {
        return FALSE;
    }

    szBuf = (CHAR*)calloc(1, iFileSize);
    fp = fopen(szZipFile, "rb");
    if (!fp)
    {
        goto end;
    }

    if (iFileSize != fread(szBuf, 1, iFileSize, fp))
    {
        goto end;
    }

    hZip = gos_comp_init();
    ulOutLen = gos_get_decomp_size(szBuf);

    szOutBuf = (CHAR*)calloc(1, ulOutLen);
    if (!szOutBuf)
    {
        goto end;
    }

    gos_decomp(hZip, szBuf, szOutBuf, &ulUnzipOutLen);

    if (ulUnzipOutLen != ulOutLen)
    {
        goto end;
    }

    gos_get_file_path(szFile, acDir);
    if (!gos_file_exist(acDir))
    {
        gos_create_redir(acDir);
    }

    fclose(fp);
    fp = fopen(szFile, "wb");
    if (!fp)
    {
        goto end;
    }

    if (ulOutLen != fwrite(szOutBuf, 1, ulOutLen, fp))
    {
        goto end;
    }

    bRet = TRUE;

end:
    if (fp)
    {
        fclose(fp);
    }

    GOS_FREE(szBuf);
    GOS_FREE(szOutBuf);

    gos_comp_free(hZip);

    return bRet;
}

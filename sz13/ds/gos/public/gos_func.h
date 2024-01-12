/******************************** 头文件保护开头 *****************************/

#ifndef _GOS_FUNC_H
#define _GOS_FUNC_H

#ifdef __cplusplus
extern "C" {
#endif

#define PING_OK             0
#define PING_FAIL           1
#define PING_ERROR          2

#define GOS_PROP_INT        0
#define GOS_PROP_STRING     1
#define GOS_PROP_IP         2

typedef struct
{
    CHAR        *szKey;
    UINT32      ulType;

    UINT32      ulReadTime;     /* 判断是否被读取，多次则表示重复 */
    UINT32      ulReadValue;    /* 对于STRING，对应的是一个指针，由调用者分配；
                                   对于IP地址，对应的是一个BYTE[4]数组 */
    VOID*       pValue;         /* 真正需要赋值的位置 */
}GOS_PROP_DESC_T;

typedef struct
{
    UINT32      ulIndex;
    UINT8       aucAddr[4];
    UINT8       aucMac[6];
    UINT8       ucIsUp;
    CHAR        acName[256+1];
    CHAR        acDesc[128];
}GOS_ADAPTER_INFO_T;

#define HEX_CHAR_TO_INT(h)      ((h)>='0' && (h)<='9')?(h)-'0':(((h)>='a' && (h)<='f')?(h)-'a'+0x0a:(((h)>='A' && (h)<='F')?(h)-'A'+0x0a:0xff))
#define BCD_TO_CHAR(b)          ((b)<0x0a?(b)+'0':(b)+'A')

#define DIGIT_CHAR_TO_INT(h)    (((h)>='0' && (h)<='9')?(h)-'0':0xff)

/******************************** 外部函数原形声明 ****************************/
#ifdef _OSWIN32_
INT32 gos_snprintf(CHAR * szDst, UINT32 ulBufLen, const CHAR *szFormat, ...);
#else
#define     gos_snprintf    snprintf
#endif

UINT32      gos_get_current_time(VOID);
VOID        gos_get_text_time_ex(UINT32 *pulCurrTime, const CHAR *szFormat, CHAR *szTime);
CHAR*       gos_get_text_time(UINT32 *pulCurrTime);
VOID        gos_print_code(UINT8 *pucMsg, UINT32 ulLen, UINT32 ulLevel);
UINT32      gos_get_rand(VOID);
UINT32      gos_get_range_rand(UINT32 uMin, UINT32 uMax);

UINT32      gos_get_gmtime(VOID);
VOID        gos_set_gmtime(UINT32 ulSecond);
BOOL        gos_set_localtime(UINT32 ulSecond);
UINT8       gos_hex_to_byte(UINT8 ucHexChar);
BOOL        gos_hexstr_to_uint(const CHAR *szHex, UINT32 *pulValue);
BOOL        gos_hexstr_to_byte(const CHAR *szHex, UINT8* pucByte);
VOID        gos_byte_to_hexstr(UINT8* pucByte, UINT32 ulLen, CHAR *szHex);

CHAR*       gos_trim_string(CHAR *szText);
CHAR*       gos_left_trim_string(CHAR *szText);
VOID        gos_right_trim_string(CHAR *szText);
INT32       gos_right_memcmp(VOID* pMem1, VOID *pMem2, UINT32 ulLen);
UINT32      gos_get_prime(UINT32 ulStart);
BOOL        gos_get_file_time(const CHAR* szFileName, UINT32 *pulCreateTime, UINT32 *pulModifyTime);
INT32       gos_get_file_size(const CHAR* szFileName);
INT64       gos_get_file_size_ex(const CHAR *szFile);
BOOL        gos_save_file(const CHAR *szFile, VOID *pData, UINT32 ulLen);
BOOL        gos_save_string_to_file(const CHAR *szFile, const CHAR *szText);
CHAR*       gos_load_string_from_file(const CHAR *szFile);

UINT16      gos_get_client_id(VOID);
UINT32      gos_get_thread_id(VOID);

VOID        gos_close_file_hander(HANDLE hFile);
HANDLE      gos_find_first_file(const CHAR *szDir, CHAR *szFile, BOOL *pbIsDir);
BOOL        gos_find_next_file(HANDLE hFile, const CHAR *szDir, CHAR *szFile, BOOL *pbIsDir);

HANDLE      gos_open_dir(const CHAR *szDir);
BOOL        gos_get_next_file(HANDLE hDir, CHAR *szFile, BOOL *pbIsDir);
VOID        gos_close_dir(HANDLE hDir);

CHAR*       gos_format_path(CHAR *szFile);
VOID        gos_get_file_path(const CHAR *szFile, CHAR *szPath);
VOID        gos_get_file_rawname(const CHAR *szFile, CHAR *szRawName);

BOOL        gos_set_file_executable(const CHAR *szFile);
BOOL        gos_set_file_writable(const CHAR *szFile);
BOOL        gos_set_file_readable(const CHAR *szFile);
BOOL        gos_set_dir_executable(const CHAR *szDir, BOOL bRecursive);
BOOL        gos_set_dir_writable(const CHAR *szDir, BOOL bRecursive);
BOOL        gos_set_dir_readable(const CHAR *szDir, BOOL bRecursive);

UINT64      gos_htonll(UINT64 u64Data);
UINT64      gos_ntohll(UINT64 u64Data);

BOOL        gos_atoi(const CHAR *szData, INT32 *piValue);
BOOL        gos_atou(const CHAR *szData, UINT32 *pulValue);
BOOL        gos_atoi64(const CHAR *szData, INT64 *pi64Value);
BOOL        gos_atou64(const CHAR *szData, UINT64 *pu64Value);
BOOL        gos_atof(const CHAR *szData, DOUBLE *pfValue);
INT32       gos_itoa(UINT32 ulDigit, CHAR *szDigit);
INT32       gos_i64toa(UINT64 u64Digit, CHAR *szDigit);

CHAR*       gos_right_strchr(const CHAR *szText, CHAR cChar);
CHAR*       gos_right_strstr(const CHAR *szText, const CHAR *szSub);
INT32       gos_find_str(const CHAR *szText, const CHAR *szSub);
INT32       gos_right_find_str(const CHAR *szText, const CHAR *szSub);

VOID        gos_str_to_upper(CHAR *szText);
VOID        gos_str_to_lower(CHAR *szText);

CHAR*       gos_get_simple_file_name(const CHAR *szFileName);
BOOL        gos_str_like(const CHAR *szText, const CHAR *szLike);
BOOL        gos_str_end_like(const CHAR *szText, const CHAR *szLike);
BOOL        gos_get_tmp_file_name(const CHAR *szTmpDir, const CHAR *szPrefix, CHAR *szTmpFile);
INT32       gos_time_cmp(GOS_DATETIME_T *pstTime1, GOS_DATETIME_T *pstTime2);
BOOL        gos_parse_time(const CHAR *szTime, GOS_DATETIME_T *pstTime);
BOOL        gos_parse_compact_time(const CHAR *szTime, GOS_DATETIME_T *pstTime);

BOOL        gos_string_to_bcd(const CHAR *szString, UINT8 *pucBCD, UINT32 ulBCDLen);
BOOL        gos_string_to_bcd_string(const CHAR *szString, CHAR *pucBCD);
BOOL        gos_digit_to_bcd(const CHAR *szString, UINT8 *pucBCD, UINT32 ulBCDLen);
BOOL        gos_bcd_to_string(UINT8 *pucBCD, UINT32 ulBCDLen, CHAR *szString);
INT32       gos_bcd_to_digit_string(UINT8 *pucBCD, UINT32 ulBCDLen, CHAR *szString);

BOOL        gos_get_ip(const CHAR *szIP, UINT8* pucIP);
BOOL        gos_get_short(const CHAR *szPort, UINT16 *pusShort);
BOOL        gos_get_ip_port(const CHAR *szAddr, UINT8* pucIP, UINT16 *pusPort);
BOOL        gos_get_mac(const CHAR *szMac, UINT8* pucMac);

// 压缩解压缩
HANDLE      gos_comp_init(VOID);
VOID        gos_comp_free(HANDLE hZip);
VOID        gos_comp(HANDLE hZip, const CHAR *pucIn, UINT32 ulInLen, CHAR *pucOut, UINT32 *pulOutLen);
VOID        gos_decomp(HANDLE hZip, const CHAR *pucIn, CHAR *pucOut, UINT32 *pulOutLen);
UINT32      gos_get_comp_size(const CHAR *pucCompressed);
UINT32      gos_get_decomp_size(const CHAR *pucCompressed);

BOOL        gos_get_disk_model(const CHAR *szDev, CHAR *szModel, UINT32 ulMaxModelLen);
BOOL        gos_get_adapter_info(GOS_ADAPTER_INFO_T *pstAdapterInfo, UINT32 ulMaxNum, UINT32 *pulNum);
BOOL        gos_get_apapter_status(UINT8 *pucMacAddr, UINT8 *pucIsAdapterUp);
BOOL        gos_is_local_ip(UINT8 *pucAddr);

UINT32      gos_get_pid();
BOOL        gos_get_proc_full_name(CHAR *szProcName);
BOOL        gos_get_proc_name(CHAR *szProcName, BOOL bResvPostfix);
CHAR*       gos_get_file_postfix(const CHAR *szFile);
BOOL        gos_is_proc_running(VOID);
BOOL        gos_get_proc_mem_usage(UINT32 *pulMemSize);

INT32       gos_get_cpu_usage (VOID);
VOID        gos_get_mem_usage (INT64 *pi64TotalMem, INT64 *pi64FreeMem);
VOID        gos_get_disk_usage(INT64 *pi64TotalSize, INT64 *pi64FreeSize);

BOOL        gos_ftp_get(const CHAR *szUser, const CHAR *szPwd, UINT8* pucSvrIP, UINT16 usPort, const CHAR *szSvrFile, const CHAR *szLocalFile);
BOOL        gos_ftp_put(const CHAR *szUser, const CHAR *szPwd, UINT8* pucSvrIP, UINT16 usPort, const CHAR *szSvrFile, const CHAR *szLocalFile);
CHAR*       gos_ftp_get_last_err(VOID);

BOOL        gos_ping(UINT8 *pucAddr, UINT32 ulSize, UINT32 ulTimeout, DOUBLE *pdDelay, UINT16 *pusSeq, UINT32 *pulTTL, UINT8 *pucLocalAddr);
BOOL        gos_simple_ping(UINT8 *pucAddr, UINT32 ulTimeout);
BOOL        gos_pings(PING_T *pstPingInfo, UINT32 ulNum, UINT32 ulTimeout);

INT32       gos_encode_utf8_to_unicode(CHAR* szUtf8, UINT32 *pulUnicode);
INT32       gos_encode_unicode_to_utf8(UINT32 ulUnicode, CHAR* szUtf8);
CHAR*       gos_utf8_to_ascii(const CHAR *szUtf8, CHAR *szAscii);
CHAR*       gos_utf8_to_ascii_ex(const CHAR *szUtf8, UINT32 ulUtf8Len, CHAR *szAscii);

BOOL        gos_file_utf8_to_ascii(const CHAR *szFile, const CHAR *szNewFile);
CHAR*       gos_ascii_to_utf8(const CHAR *szAscii, CHAR *szUtf8);

BOOL        gos_unicode_hexchar_to_string(const CHAR *szHex, CHAR *szText);
VOID        gos_string_to_unicode_hexchar(const CHAR *szText, CHAR *szHex);

VOID        gos_parse_string(CHAR *szIn, CHAR **pszOut, UINT32 *pulOutNum);

BOOL        gos_base64_encode(const UINT8 *pucSrc, INT32 iSrclen, UINT8 *pucDst, INT32 *piDstLen);
BOOL        gos_base64_decode(const UINT8 *pucSrc, INT32 iSrclen, UINT8 *pucDst, INT32 *piDstLen);

BOOL        gos_merge_file(const CHAR* szTmpFile, const CHAR* szNewFile);

VOID        gos_encrypt_byte(UINT8 *pucByte);
VOID        gos_decrypt_byte(UINT8 *pucByte);
VOID        gos_encrypt_data(UINT8 *pucData, UINT32 ulDataLen);
VOID        gos_decrypt_data(UINT8 *pucData, UINT32 ulDataLen);

///< CRC
UINT16      gos_ansi_crc16(UINT8 *pucData, UINT32 ulLen);

BOOL        gos_estimate_cpu_endian (); /// 判断CPU大小端

/// 判断 入参 是否是本地 IP
BOOL        gos_is_local_addr(UINT8 *pucAddr);

#ifdef __cplusplus
}
#endif

#endif /* _GOS_FUNC_H */
/******************************* 头文件结束 **********************************/

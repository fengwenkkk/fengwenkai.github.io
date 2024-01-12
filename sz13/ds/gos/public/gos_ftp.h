/******************************** 头文件保护开头 *****************************/

#ifndef _GOS_FTP_H
#define _GOS_FTP_H

#ifdef __cplusplus
extern "C" {
#endif

/* FTP相关宏定义 */
#define GOS_FTP_CMD_PUT_FILE         ((UINT32)(0x01)) /* 上传 */
#define GOS_FTP_CMD_PUT_DIR_PACK     ((UINT32)(0x02)) /* 上传并压缩 */
#define GOS_FTP_CMD_GET_FILE         ((UINT32)(0x11)) /* 下载 */
#define GOS_FTP_CMD_GET_DIR_PACK     ((UINT32)(0x12)) /* 下载压缩包并解压到目录 */

#define GOS_FTP_CMD_CREATE_DIR       ((UINT32)(0x21)) /* 创建目录 */
#define GOS_FTP_CMD_DELETE_DIR       ((UINT32)(0x22)) /* 创建目录 */
#define GOS_FTP_CMD_DELETE_FILE      ((UINT32)(0x23)) /* 删除文件 */
#define GOS_FTP_CMD_GET_DIRINFO      ((UINT32)(0x24)) /* 获取目录下文件个数和大小 */

#define GOS_FS_FILE_ABSPATH_LEN     (256+5)     /* 绝对路径名称长度*/
#define GOS_FS_FILEDIR_LEN          (64)        /*文件相对路径名称长度*/
#define GOS_FTP_USERNAME_LEN        (32)        /* FTP请求的用户名长度 */
#define GOS_FTP_PASSWORD_LEN        (32)        /* FTP请求的口令长度 */
#define GOS_FS_FILENAME_LEN         (32+5)      /* 文件名长度（不含路径） */

#define GOS_FTP_DEFAULT_PORT        21

#if defined (_OSWIN32_)     /* WINDOWS*/
#include <Wininet.h>

#define GOS_FILE_ORDONLY        (_O_RDONLY)    /* 只读 */
#define GOS_FILE_OWRONLY        (_O_WRONLY)    /* 只写 */
#define GOS_FILE_ORDWR          (_O_RDWR)      /* 读写 */

#define GOS_FILE_OCREATE        (_O_CREAT)     /* 创建 */
#define GOS_FILE_OAPPEND        (_O_APPEND)    /* 追加 */
#define GOS_FILE_OTRUNC         (_O_TRUNC)     /* 清除原有内容 */

#define GOS_FILE_OTEXT          (_O_TEXT)      /* 文本方式 */
#define GOS_FILE_OBINARY        (_O_BINARY)    /* 二进制方式 */

typedef struct
{
    UINT32      ulCmd ;                 /*FTP服务类型*/
    UINT32      ulLen ;                 /*用户传入的内存大小*/

    HINTERNET   pvInternetSession ;     /*internet句柄*/
    HINTERNET   pvConnectSession ;      /*connection句柄*/
    HINTERNET   pvFTPSession ;          /*ftp句柄*/

    CHAR        *szServerFileName ;     /*服务器文件名称，包含绝对路径*/
    CHAR        *szServerIP ;           /*服务器IP地址*/

    UINT16      usPort;

    CHAR        *szUserName ;           /*FTP用户名称*/
    CHAR        *szPasswd ;             /*FTP用户密码*/
    CHAR        *szMemBuf ;             /*TFTP用户传入的内存指针*/
}GOS_FTP_CONNECT_T;
#endif  /* #if defined (_OSWIN32_) */

#pragma pack(push, 4)

/*FTP客户端传输请求消息*/
typedef struct
{
    CHAR        acUserName[GOS_FTP_USERNAME_LEN];           /* 用户名，最大长度20个字符，包括'\0'结束符*/
    CHAR        acPasswd[GOS_FTP_PASSWORD_LEN];         /* 密码，最大长度20个字符，包括'\0'结束符 */
    UINT32      ulServerIP;                                 /* ftp服务器ip地址 */
    UINT16      usPort;                                     /* ftp服务器端口 */
    UINT16      resv1;

    CHAR        acServerFile[GOS_FS_FILE_ABSPATH_LEN];      /* 服务器端文件名(相对路径) */
    CHAR        acClientFile[GOS_FS_FILE_ABSPATH_LEN];      /* 客户端文件名(绝对路径) */

    UINT32      ulFtpCmd;       /* 传输命令：上传 GOS_FILE_PUT，下载 GOS_FILE_GET*/
    UINT32      ulLen;          /* 缓存空间字节尺寸，如果为0，则写入文件；否则写入缓存 */
    UINT8       *pucMem;        /* 则文件下载直接写入该缓存空间 */
} GOS_FTP_REQ_MSG_T;

/*文件属性类型定义*/
typedef struct
{
    UINT16      usMode;          /* 保留，暂不使用file mode, Bit mask for file-mode information. */
    UINT16      usReserved;
    UINT32      ulSize;          /* size of file, in bytes */
    UINT32      ulAccessTime;    /* time of last access */
    UINT32      ulModifyTime;    /* time of last modification */
    UINT32      ulChangTime;     /* time of last change of file status */
} GOS_FILE_STAT_T;

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

typedef struct
{
    CHAR        acFile[MAX_PATH];
}GOS_FTP_FILE_T;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif /* _GOS_FTP_H */
/******************************* 头文件结束 **********************************/

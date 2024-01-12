/******************************** ͷ�ļ�������ͷ *****************************/

#ifndef _GOS_FTP_H
#define _GOS_FTP_H

#ifdef __cplusplus
extern "C" {
#endif

/* FTP��غ궨�� */
#define GOS_FTP_CMD_PUT_FILE         ((UINT32)(0x01)) /* �ϴ� */
#define GOS_FTP_CMD_PUT_DIR_PACK     ((UINT32)(0x02)) /* �ϴ���ѹ�� */
#define GOS_FTP_CMD_GET_FILE         ((UINT32)(0x11)) /* ���� */
#define GOS_FTP_CMD_GET_DIR_PACK     ((UINT32)(0x12)) /* ����ѹ��������ѹ��Ŀ¼ */

#define GOS_FTP_CMD_CREATE_DIR       ((UINT32)(0x21)) /* ����Ŀ¼ */
#define GOS_FTP_CMD_DELETE_DIR       ((UINT32)(0x22)) /* ����Ŀ¼ */
#define GOS_FTP_CMD_DELETE_FILE      ((UINT32)(0x23)) /* ɾ���ļ� */
#define GOS_FTP_CMD_GET_DIRINFO      ((UINT32)(0x24)) /* ��ȡĿ¼���ļ������ʹ�С */

#define GOS_FS_FILE_ABSPATH_LEN     (256+5)     /* ����·�����Ƴ���*/
#define GOS_FS_FILEDIR_LEN          (64)        /*�ļ����·�����Ƴ���*/
#define GOS_FTP_USERNAME_LEN        (32)        /* FTP������û������� */
#define GOS_FTP_PASSWORD_LEN        (32)        /* FTP����Ŀ���� */
#define GOS_FS_FILENAME_LEN         (32+5)      /* �ļ������ȣ�����·���� */

#define GOS_FTP_DEFAULT_PORT        21

#if defined (_OSWIN32_)     /* WINDOWS*/
#include <Wininet.h>

#define GOS_FILE_ORDONLY        (_O_RDONLY)    /* ֻ�� */
#define GOS_FILE_OWRONLY        (_O_WRONLY)    /* ֻд */
#define GOS_FILE_ORDWR          (_O_RDWR)      /* ��д */

#define GOS_FILE_OCREATE        (_O_CREAT)     /* ���� */
#define GOS_FILE_OAPPEND        (_O_APPEND)    /* ׷�� */
#define GOS_FILE_OTRUNC         (_O_TRUNC)     /* ���ԭ������ */

#define GOS_FILE_OTEXT          (_O_TEXT)      /* �ı���ʽ */
#define GOS_FILE_OBINARY        (_O_BINARY)    /* �����Ʒ�ʽ */

typedef struct
{
    UINT32      ulCmd ;                 /*FTP��������*/
    UINT32      ulLen ;                 /*�û�������ڴ��С*/

    HINTERNET   pvInternetSession ;     /*internet���*/
    HINTERNET   pvConnectSession ;      /*connection���*/
    HINTERNET   pvFTPSession ;          /*ftp���*/

    CHAR        *szServerFileName ;     /*�������ļ����ƣ���������·��*/
    CHAR        *szServerIP ;           /*������IP��ַ*/

    UINT16      usPort;

    CHAR        *szUserName ;           /*FTP�û�����*/
    CHAR        *szPasswd ;             /*FTP�û�����*/
    CHAR        *szMemBuf ;             /*TFTP�û�������ڴ�ָ��*/
}GOS_FTP_CONNECT_T;
#endif  /* #if defined (_OSWIN32_) */

#pragma pack(push, 4)

/*FTP�ͻ��˴���������Ϣ*/
typedef struct
{
    CHAR        acUserName[GOS_FTP_USERNAME_LEN];           /* �û�������󳤶�20���ַ�������'\0'������*/
    CHAR        acPasswd[GOS_FTP_PASSWORD_LEN];         /* ���룬��󳤶�20���ַ�������'\0'������ */
    UINT32      ulServerIP;                                 /* ftp������ip��ַ */
    UINT16      usPort;                                     /* ftp�������˿� */
    UINT16      resv1;

    CHAR        acServerFile[GOS_FS_FILE_ABSPATH_LEN];      /* ���������ļ���(���·��) */
    CHAR        acClientFile[GOS_FS_FILE_ABSPATH_LEN];      /* �ͻ����ļ���(����·��) */

    UINT32      ulFtpCmd;       /* ��������ϴ� GOS_FILE_PUT������ GOS_FILE_GET*/
    UINT32      ulLen;          /* ����ռ��ֽڳߴ磬���Ϊ0����д���ļ�������д�뻺�� */
    UINT8       *pucMem;        /* ���ļ�����ֱ��д��û���ռ� */
} GOS_FTP_REQ_MSG_T;

/*�ļ��������Ͷ���*/
typedef struct
{
    UINT16      usMode;          /* �������ݲ�ʹ��file mode, Bit mask for file-mode information. */
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
/******************************* ͷ�ļ����� **********************************/

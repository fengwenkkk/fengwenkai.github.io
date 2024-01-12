#ifndef _PDB_DEF_H
#define _PDB_DEF_H

#include "g_include.h"

/* DB TYPE*/
#define PDBT_ODBC           0x10000         /* ODBC */

#define PDBT_NOSQL          0               /* 不支持数据库 */
#define PDBT_MYSQL          1               /* MYSQL */
#define PDBT_MSSQL          (PDBT_ODBC|2)   /* MS SQL Server */
#define PDBT_ORACLE         3               /* ORACLE */

/* PDB CLI Transaction name length macro*/
#define PDB_MAX_TRANS_LEN   32

/* PDB CLI Error info size macro*/
#define PDB_MAX_ERR_LEN     1024

/* PDB CLI SQL command length definition */
#define PDB_MAX_SQL_LEN     (64*1024)

/* PDB result code definition*/
typedef enum
{
    PDB_SUCCESS,
    PDB_SUCCESS_WITH_INFO,
    PDB_STILL_EXECUTING,
    PDB_INVALID_HANDLE,
    PDB_NO_DATA,
    PDB_NEED_DATA,
    PDB_CONTINUE,

    PDB_ERROR_CREATE_ENV,
    PDB_ERROR_PARA,
    PDB_ERROR_SET_ATTR,

    PDB_ERROR_CREATE_CONN,
    PDB_ERROR_FREE_CONN,
    PDB_ERROR_ALLOC_CONN,

    PDB_ERROR_CREATE_STMT,
    PDB_ERROR_FREE_STMT,
    PDB_ERROR_INIT_STMT,
    PDB_ERROR_STMT_PREPARE,

    PDB_ERROR_FREE_RSLT,
    PDB_ERROR_BIND,
    PDB_ERROR_EXEC,
    PDB_ERROR_COMMIT,
    PDB_ERROR_ROLLBACK,
    PDB_ERROR_FETCH,

    PDB_ERROR = 0xffff
}PDBRET;

/* ODBC STMT属性 */
typedef enum
{
    ODBC_ATTR_PARAM_BIND_OFFSET_PTR,    /* 参数绑定偏移值 */
    ODBC_ATTR_PARAM_BIND_TYPE      ,    /* 参数绑定类型 */
    ODBC_ATTR_PARAM_OPERATION_PTR  ,    /* 参数操作类型(PROCEED/IGNORE) */
    ODBC_ATTR_PARAM_STATUS_PTR     ,    /* 参数执行状态 */
    ODBC_ATTR_PARAMS_PROCESSED_PTR ,    /* 参数执行数目 */
    ODBC_ATTR_PARAMSET_SIZE        ,    /* 批量绑定参数集大小 */
    ODBC_ATTR_ROW_ARRAY_SIZE       ,    /* 结果集预取行数 */
    ODBC_ATTR_ROW_BIND_OFFSET_PTR  ,    /* 结果集绑定偏移值 */
    ODBC_ATTR_ROW_BIND_TYPE        ,    /* 结果集绑定类型 */
    ODBC_ATTR_ROW_OPERATION_PTR    ,    /* 结果集操作类型(PROCEED/IGNORE) */
    ODBC_ATTR_ROW_STATUS_PTR       ,    /* 结果集状态 */
    ODBC_ATTR_ROWS_FETCHED_PTR     ,    /* 获取的行数 */

    ODBC_ATTR_INVALID,
}PDB_STMT_ATTR_E;

/* PDB datatype macro definition*/
//#define PDBT_CHR              0   /* SQLT_CHR   character string */
#define PDBT_STRING             1   /* SQLT_STR   zero terminated string */
#define PDBT_INT8               2   /* SQLT_AFC   tiny integer  */
#define PDBT_INT16              3   /* SQLT_INT   small integer  */
#define PDBT_INT32              4   /* SQLT_INT   integer  */
#define PDBT_UINT8              5   /* SQLT_AFC   tiny integer  */
#define PDBT_UINT16             6   /* SQLT_INT   small integer  */
#define PDBT_UINT32             7   /* SQLT_INT   integer  */

//#define PDBT_BYTE             21  /* SQLT_BIN   binary data(DTYBIN) */
#define PDBT_FLOAT              22  /* SQLT_FLT   Floating point number */
#define PDBT_DOUBLE             23  /* SQLT_FLT   Floating point number */
#define PDBT_DECIMAL            24  /* MYSQL DECIMAL */

#define PDBT_DATE               31  /* SQLT_DAT   date in oracle format */
#define PDBT_SYSDATE            32  /* get dbms system date*/

#define PDBT_NUM                41  /* number */
#define PDBT_RAW                42  /* raw */

#define PDBT_INT64              51  /* INT64 */
#define PDBT_UINT64             52  /* UINT64 */

#define PDBT_BIT                53  /* MYSQL BIT */

#define PDBT_BLOB               54  /* MYSQL BLOB: binary data 2^16-1 */
#define PDBT_LONGBLOB           57 /* MYSQL LONGBLOG: (2^32-1) */
#define PDBT_NULL               58 /* MYSQL NULL */


#define PDBT_UNKNOW             0xffff

/* PDB CLI SQL commnd type definition*/
#define PDB_SQL_SELECT                  1
#define PDB_SQL_UPDATE                  2
#define PDB_SQL_DELETE                  3
#define PDB_SQL_INSERT                  4
#define PDB_SQL_CREATE                  5
#define PDB_SQL_DROP                    6
#define PDB_SQL_ALTER                   7
#define PDB_SQL_BEGIN                   8
#define PDB_SQL_DECLARE                 9
#define PDB_SQL_UNKNOWN                 0xff

/* parameter type */
#define PDB_PARAM_INPUT                 1
#define PDB_PARAM_OUTPUT                2
#define PDB_PARAM_INPUT_OUTPUT          3

/* error level */
#define PDB_ERROR_LEVEL_NULL            0
#define PDB_ERROR_LEVEL_LOW             1
#define PDB_ERROR_LEVEL_MID             2
#define PDB_ERROR_LEVEL_HIGH            3
#define PDB_PRIMARYKEY_VIOLATED         4 /*SQL:23000 ORACLE:ORA-00001*/

/* PDBCancel type */
#define PDB_UNBIND                      0
#define PDB_KEEPBIND                    1

#define PDB_MAX_FIELD_NUM               (1000)
#define PDB_MAX_BIND_NUM                (1020)

#define PDB_MAX_SERVER_LEN      128     // Oracle为服务名
#define PDB_MAX_DBNAME_LEN      64
#define PDB_MAX_USER_LEN        64
#define PDB_MAX_PWD_LEN         64

#define PDB_MAX_STMT_NUM        128

#define MAX_FILE_PATH_LEN       260
#define INVALID_INDEX           (-1)

#define LOCK_FOREVER            0xFFFFFFFF  /* Infinite timeout */
#define LOCK_TIMEOUT            (60*1000)

typedef struct
{
    CHAR                acServer[PDB_MAX_SERVER_LEN];       // MSSQL/Oracle下为"地址\服务名"
    UINT16              usPort;
    UINT32              ulDBType;
    BOOL                bUseDSN;                            // 是否采用DSN连接ODBC
    CHAR                acDBName[PDB_MAX_DBNAME_LEN];
    CHAR                acUserName[PDB_MAX_USER_LEN];
    CHAR                acPassword[PDB_MAX_PWD_LEN];
    CHAR                acCharacterSet[32];
}PDB_CONN_PARAM_T;

typedef struct
{
    UINT32      ulTimeout;      /* SQL操作超时时间(s) */
    UINT16      usMaxDop;       /* max degree of parallelism(Oracle) */

    UINT32      ulPrefetchRow;  /* 数据库中预提取的记录数 */
    UINT32      ulPrefetchMem;  /* 数据库中预提取记录占用的最大内存 */
}PDB_OPTION_T;

typedef struct
{
    UINT8       *pucData;
    UINT32      ulLen;
}PDB_BLOB_T;

typedef BOOL (*PDB_CHECK_CONN)(VOID*);

#define PDB_DATA_IS_NULL        1   /* 数据为NULL */
#define PDB_DATA_IS_NOT_NULL    0   /* 数据不为NULL */


#endif

#ifndef _PDB_TEST_H
#define _PDB_TEST_H

/*
#define INT32   int
#define INT16   short
#define INT8    char
#define INT64   __int64
#define UINT32  unsigned int
#define UINT16  unsigned short
#define UINT8   unsigned char
#define CHAR    char
#define BOOL    int
#define FLOAT   float
*/

/*

typedef int         INT32;
typedef short       INT16;
typedef char        INT8;
typedef __int64     INT64;
typedef unsigned __int64 UINT64;
typedef unsigned int    UINT32;
typedef unsigned short  UINT16;
typedef unsigned char   UINT8;
typedef char        CHAR;
typedef int         BOOL;
typedef float       FLOAT;
typedef double      DOUBLE; */

#define MAX_TIME_FMT_LEN    20
#define MAX_LOGFILE_NUM     10
#define MAX_LOGFILE_SIZE    100 //100*1024*1024
#define MAX_PRN_MSG_LEN     1024
#define INVALID_INDEX       (-1)

typedef enum
{
    TABLE1,
    TABLE2,
    TABLE3,

    TABLE_NUM
};

typedef struct
{
    UINT32      ulPartID;
    UINT32      ulID;
    INT8        cInt8;
    INT16       sInt16;
    INT32       iInt32;
    INT64       i64Int64;
    UINT8       ucInt8;
    UINT16      usInt16;
    UINT32      ulInt32;
    FLOAT       fFloat;
}TABLE2_T;

typedef struct
{
    UINT32      ulPartID;
    UINT32      ulID;
    INT8        cInt8;
    INT16       sInt16;
    INT32       iInt32;
    INT64       i64Int64;
    UINT8       ucInt8;
    UINT16      usInt16;
    UINT32      ulInt32;
    UINT64      u64Int64;
    FLOAT       fDecimal;
    FLOAT       fFloat;
    DOUBLE      dDouble;
    CHAR        acString[6000];
    UINT64      bit1;
    //UINT8     bit;
    UINT8       bit:1;
    UINT8       tinyBlob[255];
    UINT8       blob[65535];
    UINT8       *pMediumBlob; //[16777215];
    UINT8       *pLongBlob; //[4294967295];
    //CHAR        acString[6000];
    //UINT8       aucByte[2000];
}TABLE1_T;

/*
typedef struct
{
    BOOL    bLogToFile;
    BOOL    bTRltToFile;
    UINT32  ulMaxLogFileSize;
    UINT32  ulMaxLogFileNum;
    UINT32  ulLogParam;
    CHAR    acLogFileDir[MAX_FILE_PATH_LEN];
    CHAR    acRltFilePath[MAX_FILE_PATH_LEN];
}PRN_T;
*/

//BOOL pdb_init_all_table(VOID);
VOID pdb_test_print(CHAR* szPrnInfo, ...);
//VOID pdb_err_print(CHAR* pucPrnMsg, UINT32 ulPrnPara);
/*
BOOL pdb_ini_env(PDBEnvBase **pEnv,
                    PDB_CONNPARA_T *pDbConPara,
                    UINT32 ulDbConNum);
BOOL pdb_free_env();
*/
BOOL pdb_test_qry(PDBConnBase   *pDBConn);
BOOL pdb_test_ins(PDBConnBase *pDBConn);
BOOL pdb_test_mod(PDBConnBase *pDBConn);
BOOL pdb_test_del(PDBConnBase *pDBConn);

BOOL pdb_test_bulk_ins(PDBConnBase *pDBConn, UINT32 ulExecNum);
BOOL pdb_test_bulk_mod(PDBConnBase *pDBConn, UINT32 ulExecNum);
BOOL pdb_test_bulk_del(PDBConnBase *pDBConn, UINT32 ulExecNum);
BOOL pdb_test_qry_table1(PDBConnBase *pDBConn);

#endif
